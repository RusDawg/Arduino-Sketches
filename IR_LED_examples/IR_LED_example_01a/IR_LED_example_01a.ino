#include <IRremote.h>

const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
unsigned long button_code = 0; // set a variable for button_code 

// Add these lines for LED code
int redpin = 6; //select the pin for the red LED
int greenpin =10;// select the pin for the green LED
int bluepin =9; // select the pin for the  blue LED
int x = 1;
int flash = 0;
int brightness = 100;

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
    switch(button_code)
    {
      case 0xF20DFF00:
      Serial.println("ON");
      // Add this line for LED code - turns on all 3 LEDs
      analogWrite(redpin, brightness); analogWrite(greenpin, brightness); analogWrite(bluepin, brightness); delay(50);
      break;
      case 0xE01FFF00:
      Serial.println("OFF");
      // Add this line for LED code - turns off LEDs
      analogWrite(redpin, 0); analogWrite(greenpin, 0); analogWrite(bluepin, 0); delay(50);
      break;
      case 0xE619FF00:
      Serial.println("RED");
      // Add this line for LED code - Turns on Red
      analogWrite(redpin, brightness); analogWrite(greenpin, 0); analogWrite(bluepin, 0); delay(50);
      break;
      case 0xE41BFF00:
      Serial.println("GREEN");
      // Add this line for LED code - Turns on Green
      analogWrite(redpin, 0); analogWrite(greenpin, brightness); analogWrite(bluepin, 0); delay(50);
      break;
      case 0xEE11FF00:
      Serial.println("BLUE");
      // Add this line for LED code - Turns on Blue
      analogWrite(redpin, 0); analogWrite(greenpin, 0); analogWrite(bluepin, brightness); delay(50);
      break;
      case 0xE21DFF00: // brightness down
      brightness = brightness - 20;
      if(brightness < 10){
        brightness = 10;
      }
      break;
      case 0xF609FF00: // brightness up
      brightness = brightness + 20;
      if(brightness > 255){
        brightness = 255;
      }
      break;
      case 0xE51AFF00:
      Serial.println("FADE");
       x = 1;
      // Add this line for LED code - FADE
      for(int i = 1; i > -1; i = i + x) {
        analogWrite(redpin, i); analogWrite(greenpin, i); analogWrite(bluepin, i); 
        if (i==255){
          x=-x;
        }
        delay(10); // delay between increments
      }
      break;
      case 0xB24DFF00:
      Serial.println("FLASH");
      // Add this line for LED code - FADE
      flash = brightness; x = 1;
      for(int i = 1; i > -1; i = i + x) {
        analogWrite(redpin, flash); analogWrite(greenpin, flash); analogWrite(bluepin, flash);
        if (flash == 0){
          flash = brightness;
        }
        else if (flash == brightness){
          flash = 0;
        }
        if (i==15){
          x=-x;
        }
        delay(300); // delay between increments
      }
      break;
      case 0xFF00FF00:
      Serial.println("STROBE");
      // Add this line for LED code - FADE
      flash = brightness; x = 1;
      for(int i = 1; i > -1; i = i + x) {
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
      break;
    }
    // Reset the IR receiver for the next signal
    irrecv.resume();
  }
}
