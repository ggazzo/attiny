#define SERIAL_BAUDRATE 115200
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define PIN_DEBUG 1
#define PIN_INTERRUPT 4
#define DATA_LENGTH 14


#include "lowp.h"
#include "mpu.h"
#include "AsmTinySerial.h"







void setup() {
  CLR(DDRB, PIN_INTERRUPT);
  SET(PORTB, PIN_INTERRUPT);
#ifdef PIN_DEBUG
  SET(DDRB, PIN_DEBUG);
#endif
  MPU.init();
}

char buffer[40];
void loop() {
  sleep();

  MPU.start();
  for(char a = 5 ; a > 0 ; a--) {
    MPU.update();
  }
  sprintf(buffer, "%f %f \n", MPU.roll , MPU.pitch);
  SerialTx( buffer );
#ifdef PIN_DEBUG
  pulse();
#endif
}

void pulse() {
#ifdef PIN_DEBUG
  delay(200);
  SET(PORTB, PIN_DEBUG);
  delay(200);
  CLR(PORTB, PIN_DEBUG) ;
#endif
}
