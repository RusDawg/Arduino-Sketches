#include <IRremote.h>

const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
unsigned long button_code = 0; // set a variable for button_code 

// Add this for led pins
int redpin = 11; //select the pin for the red LED
int greenpin =10;// select the pin for the green LED
int bluepin =9; // select the pin for the  blue LED

void setup(){
  Serial.begin(9600);
  irrecv.enableIRIn();
}

void loop() {
  // Check if the IR receiver has received a signal
  if (irrecv.decode()) {
    //Serial.println(irrecv.decodedIRData.decodedRawData, HEX);
    button_code = (irrecv.decodedIRData.decodedRawData);
    switch(button_code)
    {
      case 0xF20DFF00:
      Serial.println("ON");
      break;
      case 0xE01FFF00:
      Serial.println("OFF");
      break;
      case 0xE619FF00:
      Serial.println("RED");
      break;
      case 0xE41BFF00:
      Serial.println("GREEN");
      break;
      case 0xEE11FF00:
      Serial.println("BLUE");
      break;
    }
    // Reset the IR receiver for the next signal
    irrecv.resume();
  }
}
