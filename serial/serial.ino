#include "lowp.h"
#define SERIAL_BAUDRATE 115200
#include "AsmTinySerial.h"

#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define PIN_DEBUG 1


void pulse() {
#ifdef PIN_DEBUG
  delay(500);
  SET(PORTB, PIN_DEBUG);
  delay(500);
  CLR(PORTB, PIN_DEBUG) ;
  SerialTx("hello\n");
#endif
}

void setup() {
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
