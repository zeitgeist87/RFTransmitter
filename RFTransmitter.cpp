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

#include <util/crc16.h>


const byte MAX_PAYLOAD_SIZE = 80;
const byte MIN_PACKAGE_SIZE = 4;
const byte MAX_PACKAGE_SIZE = MAX_PAYLOAD_SIZE + MIN_PACKAGE_SIZE;

static inline uint16_t crc_update(uint16_t crc, uint8_t data) {
  return _crc_ccitt_update(crc, data);
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
