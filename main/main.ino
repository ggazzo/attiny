#include "lowp.h"
#include <Wire.h>

#include <AsmTinySerial.h>  




const int MPU=0x68;

//#define PIN_DEBUG 1

/* IMU Data */
int accX, accY, accZ;
int gyroX, gyroY, gyroZ;
int tempRaw;

float gyroXrate, gyroYrate;


uint32_t timer, dt;


float roll, pitch;
float roll_final, pitch_final;
float roll_new, pitch_new;
inline void read_data(){
  Wire.beginTransmission(MPU);      //transmite
  Wire.write(0x3B);                 // Endereço 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);     //Finaliza transmissão

  Wire.requestFrom(MPU,14);   //requisita bytes
  // Armazena o valor dos sensores nas variaveis correspondentes
  accX=Wire.read()<<8|Wire.read();  //0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  accY=Wire.read()<<8|Wire.read();  //0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  accZ=Wire.read()<<8|Wire.read();  //0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  tempRaw=Wire.read()<<8|Wire.read();  //0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  gyroX=Wire.read()<<8|Wire.read();  //0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  gyroY=Wire.read()<<8|Wire.read();  //0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  gyroZ=Wire.read()<<8|Wire.read();  //0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

}

void first(){
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



void setup(){

  SerialInit( PB3  , 115200 );
  setup_sleep();
  // Wire.begin();                 //inicia I2C
  // Wire.beginTransmission(MPU);  //Inicia transmissão para o endereço do MPU
  // Wire.write(0x6B);

  // Wire.write(0);
  // Wire.endTransmission(true);
}

#define request_data(void);  first ();\
  gyroXrate = gyroX / 131.0;\
  gyroYrate = gyroY / 131.0;\
  complementary();


void  get_roll_pitch() {
  roll_new  = atan2(accY, accZ) * RAD_TO_DEG;
  pitch_new = atan(-accX / sqrt(accY * accY + accZ * accZ)) * RAD_TO_DEG;
}

void loop(){
    first ();
    char a= 5;
    do {
      roll_final = roll =  roll_new;
      pitch_final = pitch =  pitch_new;
      request_data();
    } while(--a > 0);
    SerialTx( "Init\n" );
    sleep();
}
