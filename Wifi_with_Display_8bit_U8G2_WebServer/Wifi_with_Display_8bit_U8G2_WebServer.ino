// This script builds off the simple WiFi Web Server example to add
// a convenient display of the IP address assigned by the WiFi router
// U8G2 adds alternate 128x64 display using u8g2, u8x8 libraries
// v01 adds a timer countdown until screen will go into powersave mode
// WebServer will connect direct to the device using WiFi w/o getting on your WiFi local network
//
// included for the WiFi example
#include <WiFi.h>
#include <WebServer.h>  // added for WebServer feature
// these are included for the OLED display example
// this version uses the universal 8bit graphics library
// see U8G2 examples for more infoserver
#include <Arduino.h>
#include <U8x8lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);  // OLEDs without Reset of the Display
//
const char* ssid = "ArduinoESP32"; // this can be anything you want - browse locak WiFi networks on your laptop or phone to find this SSID
const char* password = "letitbeso"; // anything you like for password
/* Put IP Address details */
IPAddress local_ip(192, 168, 5, 1); // you can set this as you wish or just use this IP - browse to this IP after connecting to WiFi Access Point
IPAddress gateway(192, 168, 5, 1);
IPAddress subnet(255, 255, 255, 0);
// use WebServer  class to set up the server object
WebServer server(80);  // be sure to start the server with "server.begin();" below after WiFi is connected
//
// set up LED pins
uint8_t LED1pin = LED_BUILTIN;
bool LED1status = LOW;
uint8_t LED2pin = 5;
bool LED2status = LOW;
//
// just some variables used for my counter that turns off the display
int iloops = 0;  // flag for first time through the loop and counter
// for a timer
unsigned long dTime = 0;
unsigned long TimeLeft = 30;  // time to leave OLED powered on with IP address displayed
unsigned long lastTime = 0;
unsigned long currentTime = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(2000);
  // next 2 lines are for init of OLED display
  u8x8.begin();
  u8x8.setPowerSave(0);
  //
  pinMode(LED1pin, OUTPUT);  // set the LED pin mode
  pinMode(LED2pin, OUTPUT);
  //
  // We start by starting the WiFi Access Point
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  //
  // establish the messages that will control the LEDs
  server.on("/", handle_OnConnect);
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  server.on("/led2on", handle_led2on);
  server.on("/led2off", handle_led2off);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");
  Serial.println("");
  Serial.println("WiFi Direct AP");
  Serial.println("IP address:    ");
  Serial.println("192.168.5.1    ");
  //
}

void loop() {
  // putyour main code here, to run repeatedly:
  currentTime = millis();  // set current time to millis
  if (iloops == 0) {       // first time through write this to screen
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.drawString(1, 1, "WiFi Direct AP  ");
    u8x8.drawString(1, 2, "Connect via WiFi");
    u8x8.drawString(1, 3, "Then browse to  ");
    u8x8.drawString(1, 4, "IP address:     ");
    u8x8.drawString(1, 5, "192.168.5.1     ");
    u8x8.drawString(1, 6, "Screen auto-off ");
    char buffer[100];                                    // set up a buffer for display
    sprintf(buffer, "in %d seconds.   \n", TimeLeft);    // develop the string buffer
    u8x8.drawString(1, 7, buffer);                       // write to display
    u8x8.refreshDisplay();                               // only required for SSD1606/7
    lastTime = millis();                                 // set last time to millis the first time through
    delay(800);                                          // delay for first time through only
  } else if (iloops > 0 && dTime > 1 && TimeLeft > 0) {  // go in here ~ every second through the loop to increment the counter
    dTime = 0;                                           // reset dTime value
    lastTime = millis();                                 // reset the last time reference after each second
    char buffer[100];                                    // set up a buffer for display
    TimeLeft = TimeLeft - 1;                             // decrement initial timer
    sprintf(buffer, "in %d seconds.   \n", TimeLeft);    // develop the string buffer
    u8x8.drawString(1, 7, buffer);                       // write to display
    delay(20);
  } else if (iloops > 0 && dTime > 1 && TimeLeft == 0) {
    u8x8.setPowerSave(1);  // turn off display after initial timer expires
  }

  // WiFi client
  server.handleClient();
  if (LED1status) {
    digitalWrite(LED1pin, HIGH);
  } else {
    digitalWrite(LED1pin, LOW);
  }
  if (LED2status) {
    digitalWrite(LED2pin, HIGH);
  } else {
    digitalWrite(LED2pin, LOW);
  }
  //
  iloops = iloops + 1;                      // increment loop counter
  dTime = (currentTime - lastTime) / 1000;  // count seconds
}

void handle_OnConnect() {
  LED1status = LOW;
  LED2status = LOW;
  Serial.println("GPIO4 Status: OFF | GPIO5 Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status,LED2status)); 
}

void handle_led1on() {
  LED1status = HIGH;
  Serial.println("GPIO4 Status: ON");
  server.send(200, "text/html", SendHTML(true,LED2status)); 
}

void handle_led1off() {
  LED1status = LOW;
  Serial.println("GPIO4 Status: OFF");
  server.send(200, "text/html", SendHTML(false,LED2status)); 
}

void handle_led2on() {
  LED2status = HIGH;
  Serial.println("GPIO5 Status: ON");
  server.send(200, "text/html", SendHTML(LED1status,true)); 
}

void handle_led2off() {
  LED2status = LOW;
  Serial.println("GPIO5 Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status,false)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

// define HTML string
String SendHTML(uint8_t led1stat,uint8_t led2stat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP32 Web Server</h1>\n";
  ptr +="<h3>Using Access Point(AP) Mode</h3>\n";
  
   if(led1stat)
  {ptr +="<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";}
  else
  {ptr +="<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";}

  if(led2stat)
  {ptr +="<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";}
  else
  {ptr +="<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";}

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}