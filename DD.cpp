#include "Arduino.h"
#include "DD.h"

bool DD::repaint;

byte DD::setColor(byte src, byte color) { //6,7,5 -> 10,9,11=(2,1,3)
  switch (color) {
    case RED:
      return src | (1 << 2);
    case GREEN:
      return src | (1 << 3);
    case BLUE:
      return src | (1 << 1);
    case BLACK:
      return 0;
    case WHITE:
      return src | (1 << 2)| (1 << 3) | (1 << 1);
      break;
  }
}
bool DD::isColor(byte src, byte color) {
  switch (color) {
    case RED:
      return (src & (1 << 2)) != 0;
    case GREEN:
      return (src & (1 << 1)) != 0;
    case BLUE:
      return (src & (1 << 3)) != 0;
    case BLACK:
      return src == 0;
    case WHITE:
      //todo
      return false;
     }
  return false;
}

void DD::draw(byte *src, int stop_index, int start_index, byte color) {

  start_index += DELTA_DISPLAYO;
  start_index %= 60;
  stop_index += +DELTA_DISPLAYO;
  stop_index %= 60;

  if (start_index < 0) {
    start_index = 60 + start_index; //example from -10 it will create 50
  }
  if (stop_index < 0) {
    stop_index = 60 + stop_index; //example from -10 it will create 50
  }
  //37


  start_index = 59 - start_index;
  stop_index = 59 - stop_index;

  if (start_index > stop_index) {
    for (byte i = start_index; i < 60; i++) {
      src[i] = setColor(src[i], color);
    }
    //Serial.print('=');
    for (byte i = 0; i < stop_index + 1; i++) {
      src[i] = setColor(src[i], color);
    }
  }
  else {
    for (byte i = start_index; i < stop_index + 1; i++) {
      src[i] = setColor(src[i], color);
    }
  }
}
void DD::drawMColor(byte *src, byte stop_index, byte start_index, byte color) {
  start_index += DELTA_DISPLAYO;
  start_index %= 60;
  stop_index += +DELTA_DISPLAYO;
  stop_index %= 60;

  if (start_index < 0) {
    start_index = 60 + start_index; //example from -10 it will create 50
  }
  if (stop_index < 0) {
    stop_index = 60 + stop_index; //example from -10 it will create 50
  }


  start_index = 59 - start_index;
  stop_index = 59 - stop_index;

  if (start_index > stop_index) {
    for (byte i = start_index; i < 60; i++) {
      src[i] = color;

    }
    for (byte i = 0; i < stop_index + 1; i++) {
      src[i] = color;

    }
  }
  else {
    for (byte i = start_index; i < stop_index + 1; i++) {
      src[i] = color;
    }
  }
}
void DD::drawCifernik(byte *src) {
  for (int i = 0; i < 4; i++) {
    draw(src, i * 15, i * 15 + 1, BLUE);
    draw(src, i * 15, i * 15 + 1, RED);
    draw(src, i * 15, i * 15 + 1, GREEN);
  }
}
void DD::drawCifernik(byte *src, byte count, byte color) {
  uint8_t o = 60 / count;
  for (int i = 0; i < count; i++) {
    draw(src, i * o, i * o + 1, color);
  }
}
void DD::clean(byte *src) {
  for (int i = 0; i < 60; i++) {
    src[i] = 0;
  }
}
void DD::drawArm(byte *src, byte count, byte index, byte color) {
  uint8_t o = 60 / count;
  draw(src, index * o, index * o + 1, color);
}
void DD::drawArmMColor(byte *src, byte count, byte index, byte color) {
  uint8_t o = 60 / count;
  drawMColor(src, index * o, index * o + 1, color);
}
void DD::copyFrom(byte *from, byte *to) {
  for (int i = 0; i < 60; i++) {
    to[i] = from[i];
  }
}

