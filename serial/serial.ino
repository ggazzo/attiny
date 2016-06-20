#include "lowp.h"
#define SERIAL_BAUDRATE 115200
#include "AsmTinySerial.h"

#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define PIN_DEBUG 1
#define PIN_INTERRUPT 5

void pulse() {
    // internal pull-up
#ifdef PIN_DEBUG
  delay(500);
  SET(PORTB, PIN_DEBUG);
  delay(500);
  CLR(PORTB, PIN_DEBUG) ;
#endif
}

void setup() {
  CLR(DDRB, PIN_INTERRUPT);
  SET(PORTB, PIN_INTERRUPT) ;

#ifdef PIN_DEBUG
  SET(DDRB, PIN_DEBUG);
#endif
}



void loop() {
  SerialTx("hello\n");
#ifdef PIN_DEBUG
  pulse();
#endif
  sleep();
}
