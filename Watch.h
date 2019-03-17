#ifndef Watch_h
  #define Watch_h
  #include "Arduino.h"
  #include "Wire.h"
  #define DS3231_I2C_ADDRESS 0x68


class Watch
{
  public:
    Watch(byte *second, byte *minute,byte *hour);
    void setSec(byte sec);
    void setMin(byte mini);
    void setHour(byte mini);
    void saveTime();
    void loadTime();

    byte getSec();
    byte getMin();
    byte getHour();
   
  private:
   byte *second;
   byte *minute;
   byte *hour;
   void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year);
   void readDS3231time(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year);
   byte  decToBcd(byte val);
   byte  bcdToDec(byte val);
   
 

};

#endif
