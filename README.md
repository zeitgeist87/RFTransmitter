RFTransmitter
===========

![433 MHz modules](https://github.com/zeitgeist87/RFTransmitter/raw/master/images/xy-mk-5v.jpg)

This is an Arduino library for low-cost 433 MHz transmitter modules with a focus on
reliable one-way communication and forward error correction. It
has no dependencies and works any digital output pin.

The corresponding [RFReceiver library](https://github.com/zeitgeist87/RFReceiver)
for the 433 MHz receiver modules can
be found [here](https://github.com/zeitgeist87/RFReceiver).

Usage
-----

![433 MHz module connection](https://github.com/zeitgeist87/RFTransmitter/raw/master/images/xy-mk-5v-connect.jpg)

```cpp
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
```