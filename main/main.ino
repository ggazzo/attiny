#include "lowp.h"
#include "TinyWireM.h"


#define SERIAL_BAUDRATE 115200
#include "AsmTinySerial.h"

#define CLR(x,y) (x&=(~(1<<y)))

#define SET(x,y) (x|=(1<<y))


const int MPU = 0x68;

#define PIN_DEBUG 1
#define DATA_LENGTH 14
/* IMU Data */
struct {
  int accX, accY, accZ;
  int tempRaw;
  int gyroX, gyroY, gyroZ;
} data_mpu;

#define accX data_mpu.accX
#define accY data_mpu.accY
#define accZ data_mpu.accZ

#define tempRaw data_mpu.tempRaw

#define gyroX data_mpu.gyroX
#define gyroY data_mpu.gyroY
#define gyroZ data_mpu.gyroZ

float gyroXrate, gyroYrate;


uint32_t timer, dt;

float roll, pitch;
float roll_final, pitch_final;
float roll_new, pitch_new;

inline void read_data() {
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission();
  Wire.requestFrom(MPU,DATA_LENGTH,(uint8_t*)&data_mpu);
}

void first() {
  read_data();
  dt = (float)(micros() - timer) / 1000000;
  timer = micros();
  get_roll_pitch();
}

#define complementary(); \
  if ((roll < -90 && roll_new > 90) || (roll > 90 && roll_new < -90)) {\
    roll = roll_new;\
    roll = 0.93 * (roll + gyroXrate * dt) + 0.07 * roll;\
  }\
  if (abs(roll) > 90)\
    gyroYrate = -gyroYrate;\
  pitch = 0.93 * (pitch + gyroYrate * dt) + 0.07 * pitch;


void pulse() {
#ifdef PIN_DEBUG
  delay(500);
  SET(PORTB, PIN_DEBUG);
  delay(500);
  CLR(PORTB, PIN_DEBUG) ;
#endif
}

#define request_data(void);  \
  first ();\
  gyroXrate = gyroX / 131.0;\
  gyroYrate = gyroY / 131.0;\
  complementary();


void  get_roll_pitch() {
  roll_new  = atan2(accY, accZ) * RAD_TO_DEG;
  pitch_new = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
}



void setup() {


  setup_sleep();
#ifdef PIN_DEBUG
  SET(DDRB, PIN_DEBUG);
#endif
  // Wire.begin();                 //inicia I2C
  // Wire.beginTransmission(MPU);  //Inicia transmissão para o endereço do MPU
  // Wire.send(0x6B);
  // Wire.send(0);
  // Wire.endTransmission(true);
}


char buffer[40];
void loop() {
  first ();
  
  roll_final = roll =  roll_new;
  pitch_final = pitch =  pitch_new;
  
  for(char a = 5 ; a > 0 ; a--) {
    request_data();
    roll_final = (roll + roll_final) / 2 ;
    pitch_final = (pitch + pitch_final) / 2;
  }

  sprintf(buffer, "%f %f \n", roll_final , pitch_final);
  SerialTx( buffer );
#ifdef PIN_DEBUG
  pulse();
#endif
  sleep();
}
