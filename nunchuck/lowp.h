#define __AVR_ATtiny85__
#include <avr/sleep.h>
#include <avr/wdt.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


#define setup_sleep setup_watchdog
char f_wdt = 0;


// set system into the sleep state
// system wakes up when wtchdog is timed out
#define system_sleep() cbi(ADCSRA,ADEN);\
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);\
  sleep_enable();\
  sleep_mode();\
  sleep_disable();\
  sbi(ADCSRA,ADEN);

#define sleep(); while (f_wdt < 1) {\
    system_sleep();\
  }\
  f_wdt = 0;
// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
#define setup_watchdog()  byte bb;char ww; bb=9 & 7;   bb|= (1<<5);  bb|= (1<<WDCE);  ww=bb;\
  MCUSR &= ~(1<<WDRF);\
  WDTCR |= (1<<WDCE) | (1<<WDE);\
  WDTCR = bb;\
  WDTCR |= _BV(WDIE);


// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect) {
  f_wdt++;  // set global flag
}
