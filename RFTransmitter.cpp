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

#include "RFTransmitter.h"

const byte MAX_PAYLOAD_SIZE = 80;
const byte MIN_PACKAGE_SIZE = 4;
const byte MAX_PACKAGE_SIZE = MAX_PAYLOAD_SIZE + MIN_PACKAGE_SIZE;

#if defined(__AVR__)
#include <util/crc16.h>
#endif

static inline uint16_t crc_update(uint16_t crc, uint8_t data) {
  #if defined(__AVR__)
    return _crc_ccitt_update(crc, data);
  #else
    // Source: http://www.atmel.com/webdoc/AVRLibcReferenceManual/group__util__crc_1ga1c1d3ad875310cbc58000e24d981ad20.html
    data ^= crc & 0xFF;
    data ^= data << 4;

    return ((((uint16_t)data << 8) | (crc >> 8)) ^ (uint8_t)(data >> 4)
            ^ ((uint16_t)data << 3));
  #endif
}

void RFTransmitter::sendPackage(byte *data, byte len) {
  sendByte(0xAA);

  // Add from, id and crc to the message
  byte packageLen = len + 4;
  sendByteRed(packageLen);

  uint16_t crc = 0xffff;
  crc = crc_update(crc, packageLen);

  for (byte i = 0; i < len; ++i) {
    sendByteRed(data[i]);
    crc = crc_update(crc, data[i]);
  }

  sendByteRed(nodeId);
  crc = crc_update(crc, nodeId);

  sendByteRed(packageId);
  crc = crc_update(crc, packageId);

  sendByteRed(crc & 0xFF);
  sendByteRed(crc >> 8);

  // Closing pulse then to LOW again
  digitalWrite(outputPin, HIGH);
  delayMicroseconds(pulseLength);
  digitalWrite(outputPin, LOW);
}

void RFTransmitter::resend(byte *data, byte len) {
  if (len > MAX_PAYLOAD_SIZE)
    len = MAX_PAYLOAD_SIZE;

  sendPackage(data, len);

  for (byte i = 0; i < resendCount; ++i) {
    delay(random(backoffDelay, backoffDelay << 1));
    sendPackage(data, len);
  }
}

void RFTransmitter::send(byte *data, byte len) {
  ++packageId;
  resend(data, len);
}
