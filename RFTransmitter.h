/*
 * Copyright (C) 2015  Andreas Rohner
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef RFTRANSMITTER_H_
#define RFTRANSMITTER_H_

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#include "pins_arduino.h"
#endif


class RFTransmitter {
    byte packageId;

    const byte nodeId, outputPin;
    // Pulse lenght in microseconds
    const unsigned int pulseLength;
    // Backoff period for repeated sends in milliseconds
    unsigned int backoffDelay;
    // How often a reliable package is resent
    byte resendCount;
    byte lineState;

    void send0() {
      lineState = !lineState;
      digitalWrite(outputPin, lineState);
      delayMicroseconds(pulseLength << 1);
    }

    void send1() {
      digitalWrite(outputPin, !lineState);
      delayMicroseconds(pulseLength);
      digitalWrite(outputPin, lineState);
      delayMicroseconds(pulseLength);
    }

    void send00() {
      send0();
      delayMicroseconds(pulseLength << 1);
    }
    void send01() {
      send1();
      delayMicroseconds(pulseLength << 1);
    }
    void send10() {
      send1();
      send0();
    }
    void send11() {
      send1();
      send1();
    }

    void sendByte(byte data) {
      byte i = 4;
      do {
        switch(data & 3) {
        case 0:
          send00();
          break;
        case 1:
          send01();
          break;
        case 2:
          send10();
          break;
        case 3:
          send11();
          break;
        }
        data >>= 2;
      } while(--i);
    }

    void sendByteRed(byte data) {
      sendByte(data);
      sendByte(data);
      sendByte(data);
    }

    void sendPackage(byte *data, byte len);

  public:
    RFTransmitter(byte outputPin, byte nodeId = 0, unsigned int pulseLength = 100,
        unsigned int backoffDelay = 100, byte resendCount = 1) : packageId(0), nodeId(nodeId), outputPin(outputPin),
        pulseLength(pulseLength), backoffDelay(backoffDelay), resendCount(resendCount) {

      pinMode(outputPin, OUTPUT);
      digitalWrite(outputPin, LOW);
      lineState = LOW;
    }

    void setBackoffDelay(unsigned int millies) { backoffDelay = millies; }
    void send(byte *data, byte len);
    void resend(byte *data, byte len);
    void print(char *message) { send((byte *)message, strlen(message)); }
    void print(unsigned int value, byte base = DEC) {
      char printBuf[5];
      byte len = 0;

      for (byte i = sizeof(printBuf) - 1; i >= 0; --i, ++len) {
        printBuf[i] = "0123456789ABCDEF"[value % base];
        value /= base;
      }

      byte *data = (byte *)printBuf;
      while (*data == '0' && len > 1) {
        --len;
        ++data;
      }

      send(data, len);
    }
};

#endif /* RFTRANSMITTER_H_ */
