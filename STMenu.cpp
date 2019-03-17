#include "Arduino.h"
#include "STMenu.h"
#include "DD.h"
#include "Watch.h"

byte _state = 0;
const byte SET_M_0 = 1;
const byte SET_M_1 = 2;
const byte SET_H = 3;
const byte SET_OK = 4;

STMenu::STMenu(byte *colors, byte *cifernik, Watch *watches, byte *_scheme, bool *_scheme_changed) {
  watch = watches;
  _COLORS = colors;
  _CIFERNIK = cifernik;
  scheme = _scheme;
  scheme_changed = _scheme_changed;

}
bool STMenu::consumeEvent(byte event) {

  switch (event) {
    case BTN_0:
      if (_state == SET_H) {
        _state = SET_M_0;
        _thour = arm_state;
        renderCifernik();
        arm_state = 0;
        renderMenu();
        return false;
      }
      else if (_state == SET_M_0) {
        _state = SET_M_1;
        m0 = arm_state;
        renderCifernik();
        arm_state = 0;
        renderMenu();
        return false;
      }
      else if (_state == 0) {
        _state = SET_H;
        renderCifernik();
        arm_state = 0;
        renderMenu();
        return false;
      }
      else if (_state == SET_M_1) {
        _state = SET_OK;
        m1 = arm_state;
        _tminute = m0 * 10 + m1;
        renderCifernik();
        arm_state = 0;
        renderMenu();
        return false;
      }
      else if (_state == SET_OK) {
        _state = 0;
        watch->setMin(_tminute);
        watch->setHour(_thour);
        watch->saveTime();
        DD::clean(_CIFERNIK);
        arm_state = 0;
        return true;
      }

      break;
    case BTN_1:
      if (_state == SET_H) {
        _state = 0;
        return true;
      }
      else if (_state == SET_M_0) {
        _state = 0;
        STMenu::consumeEvent(BTN_0);
        return false;

      }
      else if (_state == SET_M_1) {
        _state = SET_H;
        STMenu::consumeEvent(BTN_0);
        return false;

      }
      else if (_state == SET_OK) {
        _state = SET_M_0;
        STMenu::consumeEvent(BTN_0);
        return false;

      }

      break;
    case BTN_2:
      if (_state != 0) {
        arm_state++;
        STMenu::renderMenu();
        return false;
      }
      else {
        *scheme = *scheme == SCHEME_DAY ? SCHEME_NIGHT : SCHEME_DAY;
        *scheme_changed = true;
        return true;
      }
      break;
  }


  return false;
}
void STMenu::renderMenu() {
  if (_state != 0)
    DD::copyFrom(_CIFERNIK, _COLORS);

  switch (_state) {
    case SET_M_0:
      arm_state %= 6;
      DD::drawArmMColor(_COLORS, 6, arm_state, DD::setColor(0, BLUE));
      break;
    case SET_M_1:
      arm_state %= 10;
      DD::drawArmMColor(_COLORS, 10, arm_state, DD::setColor(0, BLUE));
      break;
    case SET_H:
      arm_state %= 12;
      DD::drawArmMColor(_COLORS, 12, arm_state, DD::setColor(0, GREEN));
      break;
    case SET_OK:
      DD::drawArmMColor(_COLORS, 12, _thour, DD::setColor(0, GREEN));
      DD::drawArmMColor(_COLORS, 60, _tminute, DD::setColor(0, BLUE));
      break;
  }
}
void STMenu::renderCifernik() {
  DD::clean(_CIFERNIK);
  switch (_state) {
    case SET_M_0:
      DD::drawCifernik(_CIFERNIK, 6, GREEN);
      break;
    case SET_M_1:
      DD::drawCifernik(_CIFERNIK, 10, GREEN);
      break;
    case SET_H:
      DD::drawCifernik(_CIFERNIK, 12, BLUE);
      break;
    case SET_OK:
      DD::drawMColor(_CIFERNIK, 0, 59, DD::setColor(0, WHITE));
      break;
  }
}
