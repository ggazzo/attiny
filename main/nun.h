/***
 * Excerpted from "Arduino: A Quick-Start Guide",
 * published by The Pragmatic Bookshelf.
 * Copyrights apply to this code. It may not be used to create training material,
 * courses, books, articles, and the like. Contact us if you are in doubt.
 * We make no guarantees that this code is fit for any purpose.
 * Visit http://www.pragmaticprogrammer.com/titles/msard for more book information.
***/
#ifndef __NUNCHUK_H__
#define __NUNCHUK_H__
#define NUNCHUK_BUFFER_SIZE 6
#include <Arduino.h>
#include "TinyWireM.h"

class Nunchuk {
public:
  void initialize();
  bool update();

  int joystick_x() const { return _buffer[0]; }
  int joystick_y() const { return _buffer[1]; }

  int x_acceleration() const {
    return ((int)(_buffer[2]) << 2) | ((_buffer[5] >> 2) & 0x03);
  }

  int y_acceleration() const {
    return ((int)(_buffer[3]) << 2) | ((_buffer[5] >> 4) & 0x03);
  }

  int z_acceleration() const {
    return ((int)(_buffer[4]) << 2) | ((_buffer[5] >> 6) & 0x03);
  }
  bool z_button() const { return !(_buffer[5] & 0x01); }
  bool c_button() const { return !(_buffer[5] & 0x02); }

private:
  void request_data();
  char decode_byte(const char);

  unsigned char _buffer[NUNCHUK_BUFFER_SIZE];
};

#endif




#define NUNCHUK_DEVICE_ID 0x52

void Nunchuk::initialize() {
  Wire.begin(); // <label id="code.nunchuk.master_joins"/>
  Wire.beginTransmission(NUNCHUK_DEVICE_ID);
  Wire.write((byte)0x40);
  Wire.write((byte)0x00);
  Wire.endTransmission();
  update();
}

bool Nunchuk::update() {
  delay(1);
  Wire.requestFrom(NUNCHUK_DEVICE_ID, NUNCHUK_BUFFER_SIZE);
  int byte_counter = 0;
  while (Wire.available() && byte_counter < NUNCHUK_BUFFER_SIZE)
    _buffer[byte_counter++] = decode_byte(Wire.read());
  request_data();
  return byte_counter == NUNCHUK_BUFFER_SIZE;
}

void Nunchuk::request_data() {
  Wire.beginTransmission(NUNCHUK_DEVICE_ID);
  Wire.write((byte)0x00);
  Wire.endTransmission();
}

char Nunchuk::decode_byte(const char b) {
  return (b ^ 0x17) + 0x17;
}
