#include "WifiPort2.h"
#include <Servo.h>
struct DataPacket {
  int AnalogCheck;
  int LJoyStickYValue;
  int RJoyStickYValue;

  int Button1Pressed;
  int Button2Pressed;
  int Button3Pressed;
} data;
int enable = 5;
int dir1 = 4;
int dir2 = 3;
int enable2 = 11;
int dir2A = 10;
int dir2B = 9;

Servo servo1;
Servo servo2;
int servo1Pin = 2;  
int servo2Pin = 12;

int servo1Angle = 65;  
bool servo2Position = false;
int lastButton2State = 0;

WifiPort<DataPacket> WifiSerial;
void setup() {
  Serial.begin(115200);
  WifiSerial.begin("ssid_PairAP_CBF66", "passwordAP_07654321", WifiPortType::Receiver);

  pinMode(enable, OUTPUT);
  pinMode(dir1, OUTPUT);
  pinMode(dir2, OUTPUT);
  pinMode(enable2, OUTPUT);
  pinMode(dir2A, OUTPUT);
  pinMode(dir2B, OUTPUT);

  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  
  servo1.write(servo1Angle);
  servo2.write(0);
  
  
}
void loop() {
  if (WifiSerial.getPortType() == WifiPortType::Transmitter || WifiSerial.getPortType() == WifiPortType::Emulator) {
    WifiSerial.autoReconnect();
    data.AnalogCheck = millis();
    Serial.print("Sending: ");
    Serial.println(data.AnalogCheck);
    if (!WifiSerial.sendData(data))
      Serial.println("Wifi Send Problem");
  }
  if ((WifiSerial.getPortType() == WifiPortType::Receiver || WifiSerial.getPortType() == WifiPortType::Emulator) && WifiSerial.checkForData()) {
    data = WifiSerial.getData();

    int joystick_value1 = data.RJoyStickYValue;
    int joystick_value2 = data.LJoyStickYValue;
    int deadzone = 25;
    int center = -5;
    int center2 = 9;

    if (abs(joystick_value1 - center) < deadzone) {
      // In deadzone - stop motor
      digitalWrite(dir1, LOW);
      digitalWrite(dir2, LOW);
      analogWrite(enable, 0);
    }
    else if (joystick_value1 > center + deadzone) {
      // Forward
      digitalWrite(dir1, HIGH);
      digitalWrite(dir2, LOW);
      analogWrite(enable, 255);
    }
    else if (joystick_value1 < center - deadzone) {
      // Reverse
      digitalWrite(dir1, LOW);
      digitalWrite(dir2, HIGH);
      analogWrite(enable, 255);
    }
    // Motor 2 control
    if (abs(joystick_value2 - center2) < deadzone) {
      // In deadzone - stop motor
      digitalWrite(dir2A, LOW);
      digitalWrite(dir2B, LOW);
      analogWrite(enable2, 0);
    }
    else if (joystick_value2 > center2 + deadzone) {
      // Forward
      digitalWrite(dir2A, HIGH);
      digitalWrite(dir2B, LOW);
      analogWrite(enable2, 255);
    }
    else if (joystick_value2 < center2 - deadzone) {
      // Reverse
      digitalWrite(dir2A, LOW);
      digitalWrite(dir2B, HIGH);
      analogWrite(enable2, 255);
    }
    
   
    if (data.Button1Pressed) {
      servo1Angle -= 5; 
      if (servo1Angle < 20) {
        servo1Angle = 20;  
      }
      servo1.write(servo1Angle);
      Serial.print("Servo1 decremented to: ");
      Serial.println(servo1Angle);
    }


    if (data.Button3Pressed) {
      servo1Angle += 5;  
      if (servo1Angle > 110) {
        servo1Angle = 110;  
      }
      servo1.write(servo1Angle);
      Serial.print("Servo1 incremented to: ");
      Serial.println(servo1Angle);
    }

    // Button 2 - Servo 2 
    if (data.Button2Pressed && !lastButton2State) {
      servo2Position = !servo2Position;  
      if (servo2Position) {
        servo2.write(45);
        Serial.println("Servo2 to 45");
      } else {
        servo2.write(0);
        Serial.println("Servo2 to 0");
      }
    }
    lastButton2State = data.Button2Pressed;
    
    Serial.print("Joy1: ");
    Serial.print(joystick_value1);
    Serial.print(" | Joy2: ");
    Serial.print(joystick_value2);

    Serial.print(" | Button1: ");
    Serial.print(data.Button1Pressed);
    Serial.print(" | Button2: ");
    Serial.print(data.Button2Pressed);
    Serial.print(" | Button3: ");
    Serial.println(data.Button3Pressed);

    delay(10);
  }
}