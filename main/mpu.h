#include "TinyWireM.h"

#define MPU_ADDRESS 0x68
#define BUFFER_SIZE 14

struct data_{
    int accX, accY, accZ;
    int temp;
    int gyroX, gyroY, gyroZ;
};

class MPU_C
{
public:
  void init();
  void update();
  void start();
  void get_roll_pitch();
  struct data_ *data;
  float roll = 0, pitch;
private:
  void read();
  char buffer[BUFFER_SIZE];
  float _roll, _pitch;
  float gyroXrate, gyroYrate;
  uint32_t timer, dt;
};


void  MPU_C::get_roll_pitch() {
  _roll  = atan2(data->accY, data->accZ) * RAD_TO_DEG;
  _pitch = atan(-data->accX / sqrt(pow(data->accY,2) + pow(data->accZ,2) )) * RAD_TO_DEG;
}

void MPU_C::init(){

  Wire.begin();
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);          
  

  
  data = (data_ *)&buffer;
}
void MPU_C::read(){
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission();
  Wire.requestFrom(MPU_ADDRESS,BUFFER_SIZE);
//  int byte_counter = 0;
//  while (Wire.available() && byte_counter < BUFFER_SIZE){
//    this->buffer[byte_counter++] = Wire.receive();
//    }
   
    this->data->accX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
    this->data->accY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    this->data->accZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
    this->data->temp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
    this->data->gyroX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    this->data->gyroY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    this->data->gyroZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
    
  timer = micros();
  this->get_roll_pitch();
}

void MPU_C::start() {  
  this->read();
  roll = _roll;
  pitch = _pitch;
}

void MPU_C::update(){
  dt = (float)(micros() - timer) / 1000000;
  this->read();

  gyroXrate = data->gyroX / 131.0;
  gyroYrate = data->gyroY / 131.0;

  if ((_roll < -90 && roll > 90) || (_roll > 90 && roll < -90)) {
    roll = _roll;
  }
  _roll = 0.95 * (roll + gyroXrate * dt) + 0.05 * _roll;
  
  if (abs(_roll) > 90)
    gyroYrate = -gyroYrate;
  _pitch = 0.95 * (pitch + gyroYrate * dt) + 0.05 * _pitch;


  roll = (roll + _roll) / 2 ;
  pitch = (pitch + _pitch) / 2;
}

MPU_C MPU;
