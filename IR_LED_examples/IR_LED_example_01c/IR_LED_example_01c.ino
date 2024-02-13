#include <IRremote.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);
unsigned long button_code = 0; // set a variable for button_code 
decode_results results;

// Add these lines for LED code
int redpin = 6; //select the pin for the red LED
int greenpin = 10;// select the pin for the green LED
int bluepin = 9; // select the pin for the  blue LED
int iFADE = -1; // save state of FADE feature - default to -1
int cntrFADE = 0; // initialize counter
int iFLASH = -1; // save state of FLASH feature - default to -1
int cntrFLASH = 0; // initialize counter
int delayX = 20; // default 20 ms delay
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
  
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
    // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

}

void loop() {
  // Check if the IR receiver has received a signal
  delay(delayX); // This delay is variable in order to enable flashing and strobing.  should be reset to nominal otherwise.
  if (irrecv.decode() || iFADE>-1 || iFLASH>-1) { // we want the main loop code to run even with no button push in order to FLASH and FADE and STROBE
    if (irrecv.decode() && irrecv.decodedIRData.decodedRawData != 0) { // This last condition keeps this code from  running with IR Repeats which return "0"
      Serial.println(irrecv.decodedIRData.decodedRawData, HEX);
      button_code = (irrecv.decodedIRData.decodedRawData);
      iFADE = -1; // turn off FADE with any other button press
      iFLASH = -1; // turn off FLASH with any other button press
      x = 1; // reset x
      delayX = 20; // reset delay to nominal
    }
    if (button_code == 0xF20DFF00) {
      delayX = 20;
      Serial.println("ON"); // Add these lines for LED code - turns on all 3 LEDs
      analogWrite(redpin, brightness); analogWrite(greenpin, brightness); analogWrite(bluepin, brightness); delay(delayX);
      state_LED = 1; // ON
      display.clearDisplay(); // Clear display buffer
      testscrolltext();    // Draw scrolling text
    }
    if (button_code == 0xE01FFF00) {
      delayX = 20;
      Serial.println("OFF"); // Add these lines for LED code - turns off LEDs
      analogWrite(redpin, 0); analogWrite(greenpin, 0); analogWrite(bluepin, 0); delay(delayX);
      state_LED = 0; // OFF
    }
    if (button_code == 0xE619FF00) {
      delayX = 20;
      Serial.println("RED"); // Add these lines for LED code - Turns on Red
      analogWrite(redpin, brightness); analogWrite(greenpin, 0); analogWrite(bluepin, 0); delay(delayX);
      state_LED = 2; // RED
    }
    if (button_code == 0xE41BFF00) {
      delayX = 20;
      Serial.println("GREEN"); // Add these lines for LED code - Turns on Green
      analogWrite(redpin, 0); analogWrite(greenpin, brightness); analogWrite(bluepin, 0); delay(delayX);
      state_LED = 3; // GREEN
    }
    if (button_code == 0xEE11FF00) {
      delayX = 20;
      Serial.println("BLUE"); // Add these lines for LED code - Turns on Blue
      analogWrite(redpin, 0); analogWrite(greenpin, 0); analogWrite(bluepin, brightness); delay(delayX);
      state_LED = 4; // BLUE
    }
    if (button_code == 0xE51AFF00) { // Add these lines for LED code - FADE
      cntrFADE = cntrFADE + 1; // increment FADE counter
      if (cntrFADE > 500) {
      Serial.println("FADE"); // Output only once every half second
      cntrFADE = 0; // reset counter
      }
      Serial.println("iFADE");
      Serial.println(iFADE);
      iFADE = iFADE + x;
      if (iFADE==brightness || iFADE<0){ // Reverse direction of fade increment at top and bottom of intensity
        x=-x;
        if (iFADE<0) {
          iFADE = 0;
        }
      }
      analogWrite(redpin, iFADE); analogWrite(greenpin, iFADE); analogWrite(bluepin, iFADE); 
      delayX = 20;
      state_LED = 5; // FADE
    }

    if (button_code == 0xB24DFF00) { // Add these lines for LED code - FLASH
      if (cntrFLASH > 5) {
      Serial.println("FLASH"); // Output only once every half second
      cntrFLASH = 0; // reset counter
      }
      cntrFLASH = cntrFLASH + 1;
      if (iFLASH <= 0){ // toggle between bright and off for flashing
        iFLASH = brightness;
      }
      else if (iFLASH == brightness){
        iFLASH = 0;
      }
      analogWrite(redpin, iFLASH); analogWrite(greenpin, iFLASH); analogWrite(bluepin, iFLASH);
      delayX = 300; // delay between flashes
      state_LED = 6; // FLASH
    }

    if (button_code == 0xFF00FF00) { // Add these lines for LED code - STROBE
      if (cntrFLASH > 5) {
      Serial.println("STROBE"); // Output only once every half second
      cntrFLASH = 0; // reset counter
      }
      cntrFLASH = cntrFLASH + 1;
      if (iFLASH <= 0){ // toggle between bright and off for flashing
        iFLASH = brightness;
      }
      else if (iFLASH == brightness){
        iFLASH = 0;
      }
      analogWrite(redpin, iFLASH); analogWrite(greenpin, iFLASH); analogWrite(bluepin, iFLASH);
      delayX = 20; // delay between flashes
      state_LED = 7; // STROBE
    }

    if (button_code == 0xF609FF00) { // brightness up
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
    }
    if (button_code == 0xE21DFF00) { // brightness down
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
    }      
    // Reset the IR receiver for the next signal
    irrecv.resume();
  }
}
void testscrolltext(void) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}