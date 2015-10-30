#include <RFTransmitter.h>

#define NODE_ID          1
#define OUTPUT_PIN       11

// Send on digital pin 11 and identify as node 1
RFTransmitter transmitter(OUTPUT_PIN, NODE_ID);

void setup() {}

void loop() {
  char *msg = "Hello World!";
  transmitter.send((byte *)msg, strlen(msg) + 1);

  delay(5000);
  
  transmitter.resend((byte *)msg, strlen(msg) + 1);
}