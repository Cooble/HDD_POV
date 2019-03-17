
#ifndef STMenu_h
  #define STMenu_h
  #include "Arduino.h"
  #include "DD.h"
  #include "Watch.h"
#endif

#ifndef BTN_0
#define BTN_0 0x0
#define BTN_1 0x1
#define BTN_2 0x2
#define BTN_3 0x3
#endif

#ifndef SCHEME_DAY
#define SCHEME_DAY 0
#define SCHEME_NIGHT 1
#endif


class STMenu
{
  public:
    STMenu(byte *colors, byte *cifernik, Watch *watches,byte *_scheme, bool *_scheme_changed);
    bool consumeEvent(byte event);
  private:
    Watch *watch;
   /* const byte SET_M_0=1;
    const byte SET_M_1=2;
    const byte SET_H=3;
    const byte SET_OK=4;*/
  
    int _tminute;
    int _thour;
    byte _state;
    byte arm_state;
    byte m0;
    byte m1;
    byte *_CIFERNIK;
    byte *_COLORS;
    byte *scheme;
    bool *scheme_changed;
    void renderMenu();
    void renderCifernik();
};
