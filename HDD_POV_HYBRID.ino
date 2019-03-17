#include "TimerOne.h"
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

#define DELTA_DISPLAYO 37 //how many segments should plotter be rotated to face up

byte second;
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


void setupISR();
void renderTime();

//===========================================SETUP===========================================================================================================

void setup() {
  setupISR();
  DDRD &= ~B11000000;
  DDRB |= B00001110; //output for leds pins: 9,10,11
  PORTD = ~B00011111; //pin 6,7,8 has logical 0

  DDRB &= ~B00110001; //set pins B0 B3,B4 as input

  Timer1.initialize(5000);         // initialize timer1, and set a 1 second period
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

  colorScheme(SCHEME_DAY);

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
volatile bool tik;
volatile bool timeSettingMode;
bool sBTN0;
bool sBTN1;
bool sBTN2;


void renderTime() {
  DD::copyFrom(CIFERNIK, COLORS);
  DD::drawMColor(COLORS, hour * 5 - 1, hour * 5 + 1, HOUR_C);
  DD::drawMColor(COLORS, minute - 1, minute + 1, MIN_C); //note: first end second start_index
  DD::drawMColor(COLORS, second - 1, second + 1, SEC_C);
}
//================================================LOOOP´==========================================================================================================
//================================================LOOOP´==========================================================================================================
//================================================LOOOP´==========================================================================================================
unsigned long lastMillis;
uint8_t secPassed;
void loop() {
  unsigned long curMil = micros() / 1000;
  if (curMil - lastMillis > 1000) {
    tik = true;
    lastMillis = curMil;

    if (!timeSettingMode && (PINB & B00110001) != B00110001) { //has pressed first btn
      timeSettingMode = true;
    }
    if (VIRTUAL_TICKING_ENABLED) {
      second++;
      if (second > 59) {
        second = 0;
        watch.loadTime();
      }
      renderTime();
    }
  }

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
      argument = BTN_2;
    }
    sBTN2 = W2;

    if (argument != 100) {
      if (menu.consumeEvent(argument)) {
        VIRTUAL_TICKING_ENABLED = true;
        DD::drawMColor(CIFERNIK, 0, 59, BACK_C);
        timeSettingMode = false;
        renderTime();
      } else
        VIRTUAL_TICKING_ENABLED = false;
    }
    if (scheme_changed) {
      scheme_changed = 0;
      colorScheme(scheme);
    }
  }
}

//======================================================================================================================================================================================
//======================================================================================================================================================================================
//======================================================================================================================================================================================

void setupISR() {
  cli();
  ADCSRB = 0;           // (Disable) ACME: Analog Comparator Multiplexer Enable
  ACSR =  bit (ACI)     // (Clear) Analog Comparator Interrupt Flag
          | bit (ACIE)    // Analog Comparator Interrupt Enable
          | bit (ACIS1);  // ACIS1, ACIS0: Analog Comparator Interrupt Mode Select (trigger on falling edge)
  sei();
}
volatile uint8_t renderIndex;
unsigned long startOfOtocka;
bool wasOtocka;


ISR (ANALOG_COMP_vect) {
  unsigned long cur = micros(); //saves the time of complete revolution
  unsigned long delta = cur - startOfOtocka;
  if (delta > 10000) {
    renderIndex = 0;
    setDel(delta / 60);
    startOfOtocka = cur;
  }
}
ISR(TIMER1_COMPA_vect) {     // timer compare interrupt service routine{
  byte color = COLORS[renderIndex];
  PORTB |= color;
  PORTB &= ~(~color & B00001110);
  if (renderIndex < 59)
    renderIndex++;
}
void setDel(uint32_t dela) {
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  uint32_t cr = 16 * dela; //this value needs to be divided by prescaller to get Compare Register Value
  uint32_t crv = cr;
  uint8_t prescal = 0;
  if (crv > 0xFFFE) {
    crv = cr / 8;
    prescal = 1;
    if (crv > 0xFFFE) {
      crv = cr / 64;
      prescal = 2;
      if (crv > 0xFFFE) {
        crv = cr / 256;
        prescal = 3;
        if (crv > 0xFFFE) {
          crv = cr / 1024;
          prescal = 4;
        }
      }
    }
  }
  OCR1A = crv;            // compare match register 16MHz/256/2Hz
  TCCR1B |= (1 << WGM12);   // CTC mode
  prescale(prescal);
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
}

/**
   sets prescale for timer 0
   0 - no prescale = 1
   1 - 8
   2 - 64
   3 -256
   4 - 1024
*/
void prescale(uint8_t p) {
  TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12));
  switch (p) {
    case 0:
      TCCR1B |= (1 << CS10);
      break;
    case 1:
      TCCR1B |= (1 << CS11);
      break;
    case 2:
      TCCR1B |= (1 << CS11) | (1 << CS10);
      break;
    case 3:
      TCCR1B |= (1 << CS12);
      break;
    case 4:
      TCCR1B |= (1 << CS12) | (1 << CS10);
      break;
  }
}



