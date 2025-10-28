#include "WifiPort2.h"

int ButtonPin = 10;

int LJoyStickYPin = A0;
int LcenteredJoystickY;

int RJoyStickXPin = A1;
int RcenteredJoystickX;

// A structure, similar to our servo and stepper motors, but this one conatins variables to be transmitted
// Any variable you want to transmit/recieve must be initalized in the DataPacket structure
struct DataPacket {

  int AnalogCheck;  //an initial check to show successful transmission
                    //YOU should wire up a simple resistor (220 ohm) circuit and manually probe it with a wire connected to A0
  int ButtonPressed;
  int LJoyStickYValue;
  int RJoyStickXValue;


} data;

//gloabl vars are outside datapacket

WifiPort<DataPacket> WifiSerial;

void setup() {
  //DONT USE PIN13 FOR ANY SENSOR OR ACTUATORS

  Serial.begin(115200);  //preferred transmission rate for Arduino UNO R4
  pinMode(ButtonPin, INPUT_PULLUP);
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

  if (digitalRead(ButtonPin) == LOW) {
      data.ButtonPressed = true;
  }
  else {
    data.ButtonPressed = false;
  }

  LcenteredJoystickY = analogRead(LJoyStickYPin) - 512;
  data.LJoyStickYValue = LcenteredJoystickY;
  
  RcenteredJoystickX = analogRead(RJoyStickXPin) - 512;
  data.RJoyStickXValue = RcenteredJoystickX;


  

  delay(100); // update delay after you get it working to be a smaller number like 10ms to account for WiFi transmission overhead
}
