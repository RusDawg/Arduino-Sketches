// This script builds off the simple WiFi Web Server example to add
// a convenient display of the IP address assigned by the WiFi router
// U8G2 adds alternate 128x64 display using u8g2, u8x8 libraries
// v01 adds a timer countdown until screen will go into powersave mode
// WiFiManager adds a feature to use your phone to get the device WiFi connected
// so that you don't need to hard code your SSID and password in the script
//
// included for the WiFi example
#include <WiFi.h>
#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager
// these are included for the OLED display example
// this version uses the universal 8bit graphics library
// see U8G2 examples for more info
#include <Arduino.h>
#include <U8x8lib.h>
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
U8X8_SSD1306_128X64_NONAME_SW_I2C u8x8(/* clock=*/SCL, /* data=*/SDA, /* reset=*/U8X8_PIN_NONE);  // OLEDs without Reset of the Display
//
// disabling these for use with WiFiManager instead
// const char* ssid = "CIA_SafeHouse_3";
// const char* password = "letitbeso";
//
int iloops = 0;  // flag for first time through the loop and counter
// for a timer
unsigned long dTime = 0;
unsigned long TimeLeft = 20;  // time to leave OLED powered on with IP address displayed
unsigned long lastTime = 0;
unsigned long currentTime = 0;
//
WiFiServer server(80);
//
void setup() {
  // put your setup code here, to run once:
  WiFi.mode(WIFI_STA);  // explicitly set mode, esp defaults to STA+AP
  // itis a good practice to make sure your code sets wifi mode how you want it.Serial.begin(115200);
  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;
  // reset settings - wipe stored credentials for testing
  // these are stored by the esp library
  wm.resetSettings(); // uncomment this line for regular use and it won't wipe your login info each restart
  //
  pinMode(LED_BUILTIN, OUTPUT);  // set the LED pin mode
  delay(2000);
  // next 2 lines are for init of OLED display
  u8x8.begin();
  u8x8.setPowerSave(0);
  //
  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result
  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  res = wm.autoConnect("AutoConnectAP");  // anonymous ap - no pass required
  // res = wm.autoConnect("AutoConnectAP","password"); // password protected ap
  if (!res) {
    Serial.println("Failed to connect");
    // ESP.restart();
  } else {
    //if you get here you have connected to the WiFi
    Serial.println("WiFi network connected!! :)");
  }
  // In this script, the server object is set up to listen for incoming connections 
  // on a specified port (usually port 80). When a client connects to the server, 
  // it sends an HTTP request to the server. The server then responds with the appropriate 
  // HTML code, which in this case, is used to display a simple web page that allows the
  // user to control the LED.
  server.begin();
  //
}

void loop() {
  // putyour main code here, to run repeatedly:
  currentTime = millis();  // set current time to millis
  if (iloops == 0) {       // first time through write this to screen
    // Serial.println("First time through loop.");
    u8x8.setFont(u8x8_font_chroma48medium8_r);
    u8x8.drawString(1, 1, "WiFi connected.");
    u8x8.drawString(1, 2, "IP Address:");
    char wifi_info[100];
    sprintf(wifi_info, "%s\n", WiFi.localIP().toString().c_str());
    u8x8.drawString(1, 3, wifi_info);
    u8x8.drawString(1, 6, "Screen auto-off");
    char buffer[100];                                    // set up a buffer for display
    sprintf(buffer, "in %d seconds.   \n", TimeLeft);    // develop the string buffer
    u8x8.drawString(1, 7, buffer);                       // write to display
    u8x8.refreshDisplay();                               // only required for SSD1606/7
    lastTime = millis();                                 // set last time to millis the first time through
    delay(800);                                          // delay for first time through only
  } else if (iloops > 0 && dTime > 1 && TimeLeft > 0) {  // go in here every second through the loop to increment the counter
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
  
  WiFiClient client = server.available();  // listen for incoming clients

  if (client) {                     // if you get a client,
    Serial.println("New Client.");  // print a message out the serial port
    String currentLine = "";        // make a String to hold incoming data from the client
    while (client.connected()) {    // loop while the client's connected
      if (client.available()) {     // if there's bytes to read from the client,
        char c = client.read();     // read a byte, then
        Serial.write(c);            // print it out the serial monitor
        if (c == '\n') {            // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn the built-in LED on.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn the built-in LED off.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);  // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);  // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
  //
  iloops = iloops + 1;                      // increment loop counter
  dTime = (currentTime - lastTime) / 1000;  // count seconds
}
