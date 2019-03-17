#include "Arduino.h"
#include "Wire.h"
#include "Watch.h"



Watch::Watch(byte *s, byte *m, byte *h) {
  second = s;
  minute = m;
  hour = h;

}
void Watch::setSec(byte sec) {
  *second = sec;

}
void Watch::setMin(byte mini) {
  *minute = mini;

}
void Watch::setHour(byte mini) {
  *hour = mini;

}

byte Watch::getSec() {
  return *second;

}
byte Watch::getMin() {
  return *minute;

}
byte Watch::getHour() {
  return *hour;

}

// Convert normal decimal numbers to binary coded decimal
byte  Watch::decToBcd(byte val) {
  return ( (val / 10 * 16) + (val % 10) );
}
// Convert binary coded decimal to normal decimal numbers
byte  Watch::bcdToDec(byte val) {
  return ( (val / 16 * 10) + (val % 16) );
}
void  Watch::setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year) {
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(decToBcd(second)); // set seconds
  Wire.write(decToBcd(minute)); // set minutes
  Wire.write(decToBcd(hour)); // set hours
  Wire.write(decToBcd(dayOfWeek)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month)); // set month
  Wire.write(decToBcd(year)); // set year (0 to 99)
  Wire.endTransmission();
}
void  Watch::readDS3231time(byte *second, byte *minute, byte *hour, byte *dayOfWeek, byte *dayOfMonth, byte *month, byte *year) {
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set DS3231 register pointer to 00h
  Wire.endTransmission();
  Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
  // request seven bytes of data from DS3231 starting from register 00h
  *second = bcdToDec(Wire.read() & 0x7f);
  *minute = bcdToDec(Wire.read());
  *hour = bcdToDec(Wire.read() & 0x3f);
  *dayOfWeek = bcdToDec(Wire.read());
  *dayOfMonth = bcdToDec(Wire.read());
  *month = bcdToDec(Wire.read());
  *year = bcdToDec(Wire.read());
}
/**
   Loads time to variables from rtc module

*/
void  Watch::loadTime() {
  byte dayOfWeek = 1;
  byte dayOfMonth = 1;
  byte month = 1;
  byte year = 1;
  byte ss = 1;
  readDS3231time(&ss, minute, hour, &dayOfWeek, &dayOfMonth, &month, &year);
  *second = ss;
}
/**
   sets time from variables to rtc module
*/
void  Watch::saveTime() {
  byte dayOfWeek = 1;
  byte dayOfMonth = 1;
  byte month = 1;
  byte year = 1;
  byte ss = *second;
  setDS3231time(ss, *minute, *hour, dayOfWeek, dayOfMonth, month, year);
  Watch::loadTime();
}

