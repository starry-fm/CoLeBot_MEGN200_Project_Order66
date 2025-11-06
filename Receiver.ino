#include "WifiPort2.h"
#include <Servo.h>
struct DataPacket { //Transmitter sent data setup
  int AnalogCheck;
  int LJoyStickYValue;
  int RJoyStickYValue;

  int Button1Pressed;
  int Button2Pressed;
  int Button3Pressed;
  int RJoyButtonPressed
} data;
const int enable = 6; //DC motor pin setup
const int dir1 = 5;
const int dir2 = 7;
const int enable2 = 3;
const int dir2A = 2;
const int dir2B = 4;

Servo servo1;
Servo servo2;
const int servo1Pin = A0;  //Servo motor setup
const int servo2Pin = A1;

int servo1Angle = 80;  //inital conditions for arm/claw
bool servo2Position = false;
int lastButton2State = 0;

WifiPort<DataPacket> WifiSerial;
void setup() {
  Serial.begin(115200);
  WifiSerial.begin("ssid_PairAP_CBF66", "passwordAP_07654321", WifiPortType::Receiver);

  pinMode(enable, OUTPUT); //Motor 1 PWM and directions
  pinMode(dir1, OUTPUT); 
  pinMode(dir2, OUTPUT); 
  pinMode(enable2, OUTPUT); //motor 2 PWM and directions
  pinMode(dir2A, OUTPUT);
  pinMode(dir2B, OUTPUT);

  servo1.attach(servo1Pin); //arm servo
  servo2.attach(servo2Pin); //claw servo
  
  servo1.write(servo1Angle);
  servo2.write(0);
  
  
}

void roboAutonomous() {
  Serial.println("Autonomous mode active");
  unsigned long startTime = millis();
  const unsigned long duration = 10000;

  unsigned long prevTime = 0;
  int state = 0;

  while (millis() - startTime < duration) {
    unsigned long now = millis();

    if (now - prevTime >= 800 && state < 4) {
      prevTime = now;

      switch (state) {
        case 0: // forward
          digitalWrite(dir1, HIGH);
          digitalWrite(dir2, LOW);
          analogWrite(enable, 200);
          break;

        case 1: // backward
          digitalWrite(dir1, LOW);
          digitalWrite(dir2, HIGH);
          analogWrite(enable, 200);
          break;

        case 2: // raise arm
          servo1.write(120);
          break;

        case 3: // lower arm
          servo1.write(55);
          break;
      }

      state = (state + 1) % 4; // cycle through states
    }
  }

  // stop motors after loop
  digitalWrite(dir1, LOW);
  digitalWrite(dir2, LOW);
  analogWrite(enable, 0);
  Serial.println("Autonomous mode complete");
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

    int joystick_value1 = data.RJoyStickYValue; //Use data from transmitter for joystick values
    int joystick_value2 = data.LJoyStickYValue;
    int deadzone = 25;
    int center = -5;
    int center2 = 9;

  //motor 1 control
    if (abs(joystick_value1 - center) < deadzone) {
      // In deadzone, stop motor
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
      // In deadzone, stop motor
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
    //had issue with lack of power to DC motors, so scrapped map function to full power in desired direction
    
   
    if (data.Button1Pressed) { //If button 1 pressed, go down until 56 degrees
      servo1Angle -= 5; 
      if (servo1Angle < 20) {
        servo1Angle = 20;  
      }
      servo1.write(servo1Angle);
      Serial.print("Servo1 decremented to: ");
      Serial.println(servo1Angle);
    }


    if (data.Button3Pressed) { //if button 3 pressed, go up until 120
      servo1Angle += 5;  
      if (servo1Angle > 120) {
        servo1Angle = 120;  
      }
      servo1.write(servo1Angle);
      Serial.print("Servo1 incremented to: ");
      Serial.println(servo1Angle);
    }

    // Button 2 - Servo 2 
    if (data.Button2Pressed && !lastButton2State) { //if button 2 pressed, close, if pressed again, open
      servo2Position = !servo2Position;  
      if (servo2Position) {
        servo2.write(47);
        Serial.println("Servo2 to 44"); //had issue where claw was not closing enough, so increased max servo degree slightly
        delay(20);
      } else {
        servo2.write(0);
        Serial.println("Servo2 to 0");
        delay(20);
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

  if (data.RJoyButtonPressed) {
  roboAutonomous();
  }
}
