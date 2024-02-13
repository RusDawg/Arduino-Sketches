#include <IRremote.h>

const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
unsigned long button_code = 0; // set a variable for button_code 
decode_results results;

// Add these lines for LED code
int redpin = 6; //select the pin for the red LED
int greenpin =10;// select the pin for the green LED
int bluepin =9; // select the pin for the  blue LED
int x = 1;
int flash = 0;
int brightness = 100;
unsigned int state_LED = 0;
// define states for LED
// "OFF"   = 0; "ON"   = 1;   "RED"   = 2;   "GREEN"   = 3;
// "BLUE"  = 4; "FADE" = 5; "FLASH"   = 6;  "STROBE"   = 7;


void setup(){
  // Add this line for LED code
  pinMode(redpin, OUTPUT); pinMode(bluepin, OUTPUT); pinMode(greenpin, OUTPUT);

  Serial.begin(9600);
  irrecv.enableIRIn();
}

void loop() {
  // Check if the IR receiver has received a signal
  if (irrecv.decode()) {
    Serial.println(irrecv.decodedIRData.decodedRawData, HEX);
    button_code = (irrecv.decodedIRData.decodedRawData);
    switch(button_code) {
      case 0xF20DFF00:
      Serial.println("ON"); // Add these lines for LED code - turns on all 3 LEDs
      analogWrite(redpin, brightness); analogWrite(greenpin, brightness); analogWrite(bluepin, brightness); delay(50);
      state_LED = 1; // ON
      break;
      case 0xE01FFF00:
      Serial.println("OFF"); // Add these lines for LED code - turns off LEDs
      analogWrite(redpin, 0); analogWrite(greenpin, 0); analogWrite(bluepin, 0); delay(50);
      state_LED = 0; // OFF
      break;
      case 0xE619FF00:
      Serial.println("RED"); // Add these lines for LED code - Turns on Red
      analogWrite(redpin, brightness); analogWrite(greenpin, 0); analogWrite(bluepin, 0); delay(50);
      state_LED = 2; // RED
      break;
      case 0xE41BFF00:
      Serial.println("GREEN"); // Add these lines for LED code - Turns on Green
      analogWrite(redpin, 0); analogWrite(greenpin, brightness); analogWrite(bluepin, 0); delay(50);
      state_LED = 3; // GREEN
      break;
      case 0xEE11FF00:
      Serial.println("BLUE"); // Add these lines for LED code - Turns on Blue
      analogWrite(redpin, 0); analogWrite(greenpin, 0); analogWrite(bluepin, brightness); delay(50);
      state_LED = 4; // BLUE
      break;
      case 0xE51AFF00:
      Serial.println("FADE"); // Add these lines for LED code - FADE
       x = 1;
      for (int i = 1; i > -1; i = i + x) {
        analogWrite(redpin, i); analogWrite(greenpin, i); analogWrite(bluepin, i); 
        if (i==brightness){ // Reverse direction of fade increment at top and bottom of intensity
          x=-x;
        }
        delay(20);
      }
      state_LED = 5; // FADE
      break;
      case 0xB24DFF00:
      Serial.println("FLASH"); // Add these lines for LED code - FLASH
      flash = brightness; x = 1;
      for (int i = 1; i > -1; i = i + x) {
        analogWrite(redpin, flash); analogWrite(greenpin, flash); analogWrite(bluepin, flash);
        if (flash == 0){ // toggle between bright and off for flashing
          flash = brightness;
        }
        else if (flash == brightness){
          flash = 0;
        }
        if (i==15){ // loop through this number of flashes then back to zero before automatic break
          x=-x;
        }
        delay(300); // delay between flashes
      }
      state_LED = 6; // FLASH
      break;
      case 0xFF00FF00:
      Serial.println("STROBE"); // Add these lines for LED code - STROBE
      flash = brightness; x = 1;
      for (int i = 1; i > -1; i = i + x) {
        analogWrite(redpin, flash); analogWrite(greenpin, flash); analogWrite(bluepin, flash);
        if (flash == 0){
          flash = brightness;
        }
        else if (flash == brightness){
          flash = 0;
        }
        if (i==25){
          x=-x;
        }
        delay(50); // delay between increments
      }
      state_LED = 7; // STROBE
      break;
      case 0xE21DFF00: // brightness down
      brightness = brightness - 20;
      if (brightness < 10){
        brightness = 10;
      }
      switch(state_LED) {
        case 0: // OFF
        analogWrite(redpin, 0); analogWrite(greenpin, 0); analogWrite(bluepin, 0); delay(50);
        break;
        case 1: // ON
        analogWrite(redpin, brightness); analogWrite(greenpin, brightness); analogWrite(bluepin, brightness); delay(50);
        break;
        case 2: // RED
        analogWrite(redpin, brightness); analogWrite(greenpin, 0); analogWrite(bluepin, 0); delay(50);
        break;
        case 3: // GREEN
        analogWrite(redpin, 0); analogWrite(greenpin, brightness); analogWrite(bluepin, 0); delay(50);
        break;
        case 4: // BLUE
        analogWrite(redpin, 0); analogWrite(greenpin, 0); analogWrite(bluepin, brightness); delay(50);
        break;
      }
      break;
      case 0xF609FF00: // brightness up
      brightness = brightness + 20;
      if (brightness > 255){
        brightness = 255;
      }
      switch(state_LED) {
        case 0: // OFF
        analogWrite(redpin, 0); analogWrite(greenpin, 0); analogWrite(bluepin, 0); delay(50);
        break;
        case 1: // ON
        analogWrite(redpin, brightness); analogWrite(greenpin, brightness); analogWrite(bluepin, brightness); delay(50);
        break;
        case 2: // RED
        analogWrite(redpin, brightness); analogWrite(greenpin, 0); analogWrite(bluepin, 0); delay(50);
        break;
        case 3: // GREEN
        analogWrite(redpin, 0); analogWrite(greenpin, brightness); analogWrite(bluepin, 0); delay(50);
        break;
        case 4: // BLUE
        analogWrite(redpin, 0); analogWrite(greenpin, 0); analogWrite(bluepin, brightness); delay(50);
        break;
      }
      break;
    }
    // Reset the IR receiver for the next signal
    irrecv.resume();
    if (irrecv.decode()) {
    Serial.println(irrecv.decodedIRData.decodedRawData, HEX);
    }
  }
}
