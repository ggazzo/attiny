static char outstr[15];
#define SERIAL_BAUDRATE 115200
#define CLR(x,y) (x&=(~(1<<y)))
#define SET(x,y) (x|=(1<<y))
#define PIN_DEBUG 1
#define PIN_INTERRUPT 5
#define DATA_LENGTH 14


#include "lowp.h"
#include "AsmTinySerial.h"
#include "mpu.h"








void setup() {
  CLR(DDRB, PIN_INTERRUPT);
  SET(PORTB, PIN_INTERRUPT);
#ifdef PIN_DEBUG
  SET(DDRB, PIN_DEBUG);
#endif
SerialTx( "done\n");
  MPU.init();
    SerialTx( "done\n");
}

char buffer[40];
void loop() {
//  sleep();

  MPU.start();
  

  for(char a = 0 ; a < 40 ; a++) {
    MPU.update();
    delay(2);
  }

  dtostrf(MPU.roll ,7, 3, outstr);
  SerialTx( "roll:" );
  SerialTx( outstr );
  dtostrf(MPU.pitch,7, 3, outstr);
  SerialTx( " pitch:" );
  SerialTx( outstr );      
  dtostrf(MPU.data->temp/340.00+36.53,7, 3, outstr);
  SerialTx( " temperature:" );
  SerialTx( outstr );
  SerialTx( "\n");  
   
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
