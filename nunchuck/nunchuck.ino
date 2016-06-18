#include "lowp.h"
#include "TinyWireM.h"
#define Wire TinyWireM
#define SERIAL_BAUDRATE 115200
#include "AsmTinySerial.h"

#include "nun.h"
Nunchuk nunchuk;

#define CLR(x,y) (x&=(~(1<<y)))

#define SET(x,y) (x|=(1<<y))

#define PIN_DEBUG 1
#define PIN_INTERRUPT 5


void pulse() {
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
nunchuk.initialize();
}


char buffer[40];
void loop() {
  if (nunchuk.update()) {

        sprintf(buffer, "%d ", nunchuk.joystick_x());
        SerialTx(buffer);

        sprintf(buffer, "%d ", nunchuk.joystick_y());
        SerialTx(buffer);

        sprintf(buffer, "%d ", nunchuk.x_acceleration());
        SerialTx(buffer);

        sprintf(buffer, "%d ", nunchuk.y_acceleration());
        SerialTx(buffer);

        sprintf(buffer, "%d ", nunchuk.z_acceleration());
        SerialTx(buffer);

        sprintf(buffer, "%d ", nunchuk.z_button());
        SerialTx(buffer);


        sprintf(buffer, "%d \n", nunchuk.c_button());
        SerialTx(buffer);
  }else {
    SerialTx("deu ruim \n");
  }
#ifdef PIN_DEBUG
  pulse();
#endif
sleep();
}
