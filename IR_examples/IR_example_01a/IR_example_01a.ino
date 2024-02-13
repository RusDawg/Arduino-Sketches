#include <IRremote.h>

const int RECV_PIN = 7;
IRrecv irrecv(RECV_PIN);

void setup(){
  Serial.begin(9600);
  irrecv.enableIRIn();
}

void loop() {
  // Check if the IR receiver has received a signal
  if (irrecv.decode()) {
    // Print HEX output on serial interface
    Serial.println(irrecv.decodedIRData.decodedRawData, HEX);
    // Reset the IR receiver for the next signal
    irrecv.resume();
  }
}
