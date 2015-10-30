RFTransmitter
===========

This is an Arduino library for low-cost 433 MHz transmitter modules with a focus on
reliable one-way communication and forward error correction. It
has no dependencies and works any digital output pin.

Usage
-----

```cpp
#include <RFTransmitter.h>

#define NODE_ID          1
#define OUTPUT_PIN       1

// Send on digital pin 1 and identify as node 1
RFTransmitter transmitter(OUTPUT_PIN, NODE_ID);

void setup() {}

void loop() {
  char *msg = "Hello World!";
  transmitter.send((byte *)msg, strlen(msg) + 1);

  delay(5000);
  
  transmitter.resend((byte *)msg, strlen(msg) + 1);
}
```