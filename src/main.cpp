/**
   ESP32 + DHT22 Example for Wokwi
   
   https://wokwi.com/arduino/projects/322410731508073042
*/

#include "DHTesp.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

AsyncWebServer server(80);

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int DHT_PIN = 15;

DHTesp dhtSensor;

void setup() {
  Serial.begin(115200);
  WiFi.begin("Wokwi-GUEST", "", 6);
  uint32_t notConnectedCounter = 0;
  while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.println("Wifi connecting...");
      notConnectedCounter++;
      if(notConnectedCounter > 150) { // Reset board if not connected after 15s
          Serial.println("Resetting due to Wifi not connecting...");
          ESP.restart();
      }
  }
  Serial.print("Wifi connected, IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize webserver URLs
  server.on("/api/temp-humidity", HTTP_GET, [](AsyncWebServerRequest *request) {
      AsyncResponseStream *response = request->beginResponseStream("application/json");
      JsonDocument json;
      TempAndHumidity  data = dhtSensor.getTempAndHumidity();
      json["Temp:"] = String(data.temperature, 2);
      json["Humidity:"] = String(data.humidity, 1);
      serializeJson(json, *response);
      request->send(response);
  });

  // Start webserver
  server.begin();
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
}

void loop() {
  TempAndHumidity  data = dhtSensor.getTempAndHumidity();
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("Temp: " + String(data.temperature, 2) + "°C");
  display.println("Humidity: " + String(data.humidity, 1) + "%");
  display.println("---");
  display.display();
  delay(2000); // Wait for a new reading from the sensor (DHT22 has ~0.5Hz sample rate)
}