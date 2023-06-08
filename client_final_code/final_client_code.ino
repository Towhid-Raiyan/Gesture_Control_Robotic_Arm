#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Servo 1 parameters
#define SERVOMIN_1 520     // 100
#define SERVOMAX_1 550    //405

// Servo 2 parameters
#define SERVOMIN_2 235    //120 215
#define SERVOMAX_2 310       //385

// Servo 3 parameters
#define SERVOMIN_3 290     //90 205
#define SERVOMAX_3 330      //420

// claw
#define SERVO_MIN_A 100
#define SERVO_MAX_A 405
// Roll
#define SERVO_MIN_Y  580  //200
#define SERVO_MAX_Y  200  //580

#define SERVO_CHANNEL_A 0
#define SERVO_CHANNEL_Y 5

const char* ssid = "Arm";
const char* password = "123456789";

// Your server's IP address or domain name with URL path
const char* serverNameX = "http://192.168.4.1/accelX";
const char* serverNameY = "http://192.168.4.1/accelY";
const char* serverNameA = "http://192.168.4.1/A";

String X;
String Y;
String A;

unsigned long previousMillis = 0;
const long interval = 1000;

// Mapping parameters
int x1Min = -90;
int x1Max = -30;
int x2Min = -29;
int x2Max = 30;
int x3Min = 31;
int x3Max = 90;
int xOutputMin = 0;
int xOutputMax = 180;
int yMax = 90;
int yMin = -90;
int aMin = 0;
int aMax = 1023;

// Servo parameters
int servo1Channel = 2;  // Servo channel for X1
int servo2Channel = 3;  // Servo channel for X2
int servo3Channel = 4;  // Servo channel for X3

void setup() {
  Serial.begin(115200);
  Serial.println();

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connected to WiFi");

  pwm.begin();
  pwm.setPWM(SERVO_CHANNEL_A, 0, SERVO_MIN_A);  // Set the initial position of the A-axis servo
  pwm.setPWM(SERVO_CHANNEL_Y, 0, SERVO_MIN_Y);  // Set the initial position of the Y-axis servo
  pwm.setPWMFreq(60);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      X = httpGETRequest(serverNameX);
      Y = httpGETRequest(serverNameY);
      A = httpGETRequest(serverNameA);



      int x1 = X.toInt() * 10;
      int x2 = X.toInt() * 10;
      int x3 = X.toInt() * 10;
      int angleA = A.toInt();
      int angleY = Y.toInt() * 10;

      int mappedX1 = map(x1, x1Min, x1Max, xOutputMin, xOutputMax);
      int mappedX2 = map(x2, x2Min, x2Max, xOutputMin, xOutputMax);
      int mappedX3 = map(x3, x3Min, x3Max, xOutputMin, xOutputMax);
      int pulseA = map(angleA, aMin, aMax, SERVO_MIN_A, SERVO_MAX_A);
      int pulseY = map(angleY, yMin, yMax, SERVO_MIN_Y, SERVO_MAX_Y);

      Serial.println("Values from server: " + X + " " + Y + " " + A);



      // Control Servo 1 (X1)
      int servo1Pulse = map(mappedX1, 0, 180, SERVOMIN_1, SERVOMAX_1);
      pwm.setPWM(servo1Channel, 0, servo1Pulse);


      // Control Servo 2 (X2)
      int servo2Pulse = map(mappedX2, 0, 180, SERVOMIN_2, SERVOMAX_2);
      pwm.setPWM(servo2Channel, 0, servo2Pulse);

      // Control Servo 3 (X3)
      int servo3Pulse = map(mappedX3, 0, 180, SERVOMIN_3, SERVOMAX_3);
      pwm.setPWM(servo3Channel, 0, servo3Pulse);

      
      pwm.setPWM(SERVO_CHANNEL_A, 0, pulseA);  // Control the servo connected to channel 0 (A-axis)
      pwm.setPWM(SERVO_CHANNEL_Y, 0, pulseY);  // Control the servo connected to channel 1 (Y-axis)

      Serial.println("Values of angle for servo: " + String(servo1Pulse) + " " + String(servo2Pulse) + " " + String(servo3Pulse) + " " + String(pulseA) + " " + String(pulseY)) + " ";


      // Save the last HTTP GET request
      previousMillis = currentMillis;
    } else {
      Serial.println("WiFi Disconnected");
    }
  }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;

  // Make HTTP GET request
  http.begin(client, serverName);
  int httpResponseCode = http.GET();

  String payload = "--";

  if (httpResponseCode > 0) {
    payload = http.getString();
  } else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
  return payload;
}
