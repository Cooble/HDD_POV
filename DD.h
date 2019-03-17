#ifndef DD_h
  #define DD_h
  #include "Arduino.h"


#ifndef RED
  #define RED 1
  #define GREEN 2
  #define BLUE 0
  #define BLACK 3
  #define WHITE 4
#endif

#ifndef DELTA_DISPLAYO
  #define DELTA_DISPLAYO 38
#endif



class DD
{
  public:
    static void drawCifernik(byte *src);
    static void drawCifernik(byte *src, byte count,byte color);
    static void clean(byte *src);
    static void drawArm(byte *src,byte count,byte index,byte color);
    static void drawArmMColor(byte *src,byte count,byte index,byte color);
    static void copyFrom(byte *from,byte *to);
    static void draw(byte *src,int start_index, int stop_index, byte color);
    static void drawMColor(byte *src,byte start_index, byte stop_index, byte color);
    static bool repaint;
    
    static byte setColor(byte src,byte color);
    static bool isColor(byte src,byte color);
  

  
};
#endif


