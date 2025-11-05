#include "WifiPort2.h"

int Button1Pin = 2; //assign buttons to digital pins
int Button2Pin = 3;
int Button3Pin = 4;

int LJoyStickYPin = A0; //assign joysticks to analog pins
int LcenteredJoystickY;

int RJoyStickYPin = A1;
int RcenteredJoystickY;

// A structure, similar to our servo and stepper motors, but this one conatins variables to be transmitted
// Any variable you want to transmit/recieve must be initalized in the DataPacket structure
struct DataPacket {

  int AnalogCheck;  //an initial check to show successful transmission
                    //YOU should wire up a simple resistor (220 ohm) circuit and manually probe it with a wire connected to A0
  int LJoyStickYValue; //create variables for data packet which communciate input values to the receiver
  int RJoyStickYValue;

  int Button1Pressed;
  int Button2Pressed;
  int Button3Pressed;


} data;

//gloabl vars are outside datapacket

WifiPort<DataPacket> WifiSerial;

void setup() {
  //DONT USE PIN13 FOR ANY SENSOR OR ACTUATORS

  Serial.begin(115200);  //preferred transmission rate for Arduino UNO R4
  pinMode(Button1Pin, INPUT_PULLUP);
  pinMode(Button2Pin, INPUT_PULLUP);
  pinMode(Button3Pin, INPUT_PULLUP);
  pinMode(LJoyStickYPin, INPUT);
  pinMode(RJoyStickYPin, INPUT);
  // WifiSerial.begin("ssid_UPDATE_FOR_YOUR_GROUP", "password_UPDATE", WifiPortType::Receiver);
  WifiSerial.begin("ssid_PairAP_CBF66", "passwordAP_07654321", WifiPortType::Transmitter);
  //WifiSerial.begin("ssid_PairAP_CBF66", "passwordAP_07654321", WifiPortType::Emulator); // one board to rule them all debugging
}

void loop() {

  if (WifiSerial.getPortType() == WifiPortType::Transmitter || WifiSerial.getPortType() == WifiPortType::Emulator) {
    WifiSerial.autoReconnect();//try and connect

    //Tx stuff below
    data.AnalogCheck = millis(); // analogRead(A0);
    Serial.print("Sending: " );
    Serial.println(data.AnalogCheck);
    //Tx stuff above


    if (!WifiSerial.sendData(data))//check and see if connection is established and data is sent
      Serial.println("Wifi Send Problem");//oh no it didn't send --> it iwll try and re-connect at the start of the loop

  }


  
  if ((WifiSerial.getPortType() == WifiPortType::Receiver || WifiSerial.getPortType() == WifiPortType::Emulator) && WifiSerial.checkForData()) {

    data = WifiSerial.getData();//received and unpack data structure

    //all Rx stuff below
    Serial.print("Received: " );
    Serial.println(data.AnalogCheck); 
    //all RX stuff above

  }

 // Read buttons
data.Button1Pressed = (digitalRead(Button1Pin) == LOW); //update data packet to tell receiver if buttons are pressed or not. If button is pressed, variable updated to true
data.Button2Pressed = (digitalRead(Button2Pin) == LOW);
data.Button3Pressed = (digitalRead(Button3Pin) == LOW);

// Read joysticks
data.LJoyStickYValue = analogRead(LJoyStickYPin) - 512; //update data packet to tell receiver what the values of the joysticks are, setting the center to zero and extremes to +/- 512
data.RJoyStickYValue = analogRead(RJoyStickYPin) - 512;

if (data.Button1Pressed || data.Button2Pressed || data.Button3Pressed) { //button debugging
  Serial.print("Buttons: B1=");
  Serial.print(data.Button1Pressed);
  Serial.print(" B2=");
  Serial.print(data.Button2Pressed);
  Serial.print(" B3=");
  Serial.println(data.Button3Pressed);
}


  

  delay(100); // update delay after you get it working to be a smaller number like 10ms to account for WiFi transmission overhead
}