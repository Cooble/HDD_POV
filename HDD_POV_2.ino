/**#include "TimerOne.h"
#include "DD.h"
#include "Watch.h"
#include "STMenu.h"
#include <avr/interrupt.h>

//BTN=========================
#ifndef BTN_0
#define BTN_0 =0
#define BTN_1 =1
#define BTN_2 =2
#define BTN_3 =3
#endif

#ifndef SCHEME_DAY
#define SCHEME_DAY=0;
#define SCHEME_NIGHT=1;
#endif
//============================

#define DELTA_DISPLAYO 37

bool shouldRefreshTime;

volatile byte second;
byte minute, hour, dayOfWeek, dayOfMonth, month, year;

byte C_WHITE;
byte C_RED;
byte C_GREEN;
byte C_BLUE;
byte C_BLACK = 0;

byte SEC_C;
byte MIN_C;
byte HOUR_C;
byte BACK_C;

bool VIRTUAL_TICKING_ENABLED;

byte COLORS[60];
byte CIFERNIK[60];


byte scheme;
bool scheme_changed;

Watch watch(&second, &minute, &hour);
STMenu menu(COLORS, CIFERNIK, &watch, &scheme, &scheme_changed);

int otocka;
bool wasOtocka;
unsigned long timeOtocka;
unsigned long startOfOtocka;

int delayTime;
//volatile bool isSerial;

volatile boolean triggered;



void setupISR();
void callback();
void renderTime();

//===========================================SETUP===========================================================================================================

void setup() {
  setupISR();
  DDRD &= ~B11000000;
  DDRB |= B00001110; //output for leds pins: 9,10,11
  PORTD = ~B00011111; //pin 6,7,8 has logical 0

  DDRB &= ~B00110001; //set pins B0 B3,B4 as input

  Timer1.initialize(1000000);         // initialize timer1, and set a 1 second period
  Timer1.attachInterrupt(callback);  // attaches callback() as a timer overflow interrupt
  Serial.begin(9600);
  Wire.begin();
  //color setup================================
  C_WHITE = DD::setColor(C_WHITE, RED);
  C_WHITE = DD::setColor(C_WHITE, GREEN);
  C_WHITE = DD::setColor(C_WHITE, BLUE);
  C_RED = DD::setColor(0, RED);
  C_GREEN = DD::setColor(0, GREEN);
  C_BLUE = DD::setColor(0, BLUE);
  //color setup================================

  //color edit===================================
  HOUR_C = C_GREEN;
  MIN_C = C_BLUE;
  SEC_C = C_BLACK;
  BACK_C = C_WHITE;
  //color edit===================================

  watch.loadTime();

  colorScheme(SCHEME_NIGHT);

  VIRTUAL_TICKING_ENABLED = true;
  renderTime();
  delay(500);
}

void colorScheme(byte type) {
  switch (type) {
    case SCHEME_DAY:
      HOUR_C = C_GREEN;
      MIN_C = C_BLUE;
      SEC_C = C_BLACK;
      BACK_C = C_WHITE;
      break;
    case SCHEME_NIGHT:
      HOUR_C = C_GREEN;
      MIN_C = C_BLUE;
      SEC_C = C_RED;
      BACK_C = C_BLACK;
      break;
  }
  DD::drawMColor(CIFERNIK, 0, 59, BACK_C);
  DD::copyFrom(CIFERNIK, COLORS);
}
unsigned long cur;
volatile bool tik;
volatile bool timeSettingMode;
bool sBTN0;
bool sBTN1;
bool sBTN2;


void renderTime() {
  DD::copyFrom(CIFERNIK, COLORS);
  DD::drawMColor(COLORS, hour * 5 - 1, hour * 5 + 1, HOUR_C);
  DD::drawMColor(COLORS, minute - 1, minute + 1, MIN_C); //info note: first end second start_index
  DD::drawMColor(COLORS, second - 1, second + 1, SEC_C);
}
//================================================LOOOP´==========================================================================================================
//================================================LOOOP´==========================================================================================================
//================================================LOOOP´==========================================================================================================
void loop() {
  if (timeSettingMode) {
    bool W0 = (PINB & B00000001) == 0;
    bool W1 = (PINB & B00010000) == 0;
    bool W2 = (PINB & B00100000) == 0;
    byte argument = 100;
    if (W0 && !sBTN0) {
      argument = BTN_0;
    }
    sBTN0 = W0;
    if (W1 && !sBTN1) {
      argument = BTN_1;
    }
    sBTN1 = W1;
    if (W2 && !sBTN2) {
      argument = BTN_3;
    }
    sBTN2 = W2;

    if (argument != 100) {
      if (menu.consumeEvent(argument)) {
        shouldRefreshTime = true;
        VIRTUAL_TICKING_ENABLED = true;
        DD::drawMColor(CIFERNIK, 0, 59, BACK_C);
        timeSettingMode = false;
      } else
        VIRTUAL_TICKING_ENABLED = !menu.active;
    }
  }
  if (tik) {
    tik = false;
    if (scheme_changed) {
      scheme_changed = 0;
      colorScheme(scheme);
    }
  }


  cur = micros();

  if (shouldRefreshTime) {
    watch.loadTime();
    //Serial.print("->");
    //sendTime();

    renderTime();
    shouldRefreshTime = false;
  }
  else if (DD::repaint) {
    //sendTime();
    if (VIRTUAL_TICKING_ENABLED)
      renderTime();
    DD::repaint = false;

  }
  if (triggered) {
    if (!wasOtocka) {
      otocka++;

      wasOtocka = true;
      timeOtocka = (cur - startOfOtocka) / 60;
      startOfOtocka = cur;

      //delayMicroseconds(delayTime);//extremely magic number because triggered is called more times so delay is needed maybe
    }
    triggered = false;
  }
  else wasOtocka = false;

  drawDisplay(cur);


}
void drawDisplay(uint16_t cura) {
  int cur_steps = (cura - startOfOtocka); //how far are we from start of otocka
  int realy = cur_steps / timeOtocka;
  if (!(realy > -1 && realy < 60))
    return;
  byte color = COLORS[realy];
  PORTB |= color;
  PORTB &= ~(~color & B00001110);
}

//======================================================================================================================================================================================
//======================================================================================================================================================================================
//======================================================================================================================================================================================
void sendTime() {
  Serial.print("Time: ");
  Serial.print(second);
  Serial.print(" ");
  Serial.print(minute);
  Serial.print(" ");
  Serial.println(hour);
}
void callback() { //called each 1 sec
  //isSerial=Serial.available();
  tik = true;

  if (!timeSettingMode && (PINB & B00110001) != B00110001) { //has pressed first btn
    timeSettingMode = true;
  }
  if (VIRTUAL_TICKING_ENABLED) {
    second++;
    DD::repaint = true;
    if (second > 59) {
      second = 0;
      shouldRefreshTime = true; //add event load time from rtc
    }
  }
}
void setupISR() {
  ADCSRB = 0;           // (Disable) ACME: Analog Comparator Multiplexer Enable
  ACSR =  bit (ACI)     // (Clear) Analog Comparator Interrupt Flag
          | bit (ACIE)    // Analog Comparator Interrupt Enable
          | bit (ACIS1);  // ACIS1, ACIS0: Analog Comparator Interrupt Mode Select (trigger on falling edge)
}


ISR (ANALOG_COMP_vect) {
  triggered = true;
}

*/

