#include <ESP8266WiFi.h>
#include "ESPAsyncWebServer.h"

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>

const char* ssid = "Arm";
const char* password = "123456789";

Adafruit_MPU6050 mpu;

AsyncWebServer server(80);

String readAccelX() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  return String(a.acceleration.x);
}

String readAccelY() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  return String(a.acceleration.y);
}

String readAccelZ() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  return String(a.acceleration.z);
}

String readA() {
  int potValue = analogRead(A0);
  return String(potValue);
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/accelX", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readAccelX().c_str());
  });
  server.on("/accelY", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readAccelY().c_str());
  });
  server.on("/accelZ", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readAccelZ().c_str());
  });
  server.on("/A", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", readA().c_str());
  });

  bool status = mpu.begin();
  if (!status) {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    while (1);
  }

  server.begin();
}

void loop() {
  Serial.print("Accel X: ");
  Serial.print(readAccelX());
  Serial.print("  |  Accel Y: ");
  Serial.print(readAccelY());
  Serial.print("  |  Accel Z: ");
  Serial.print(readAccelZ());
  Serial.print("  |  Pot Value: ");
  Serial.println(readA());
  delay(1000);
}
