#include "TinyWireM.h"

#define MPU_ADDRESS 0x68
#define BUFFER_SIZE 14

struct data_{
    int accX, accY, accZ;
    int tempRaw;
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
  float roll, pitch;
private:
  void read();
  char buffer[BUFFER_SIZE];
  float _roll, _pitch;
  float gyroXrate, gyroYrate;
  uint32_t timer, dt;
};


void  MPU_C::get_roll_pitch() {
  _roll  = atan2(data->accY, data->accZ) * RAD_TO_DEG;
  _pitch = atan(-data->accX / sqrt(data->accY * data->accY + data->accZ * data->accZ)) * RAD_TO_DEG;
}

void MPU_C::init(){
  Wire.begin();
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x6B);
  //Inicializa o MPU-6050
  Wire.write(0);
  Wire.endTransmission();
  data = (data_ *)&buffer;
}
void MPU_C::read(){
  Wire.beginTransmission(MPU_ADDRESS);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission();
  Wire.requestFrom(MPU_ADDRESS,BUFFER_SIZE);
  int byte_counter = 0;
  while (Wire.available() && byte_counter < BUFFER_SIZE)
    this->buffer[byte_counter++] = Wire.receive();
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

  if ((roll < -90 && _roll > 90) || (roll > 90 && _roll < -90)) {
    _roll = 0.93 * (_roll + gyroXrate * dt) + 0.07 * _roll;
  }
  if (abs(roll) > 90)
    gyroYrate = -gyroYrate;
  _pitch = 0.93 * (_pitch + gyroYrate * dt) + 0.07 * _pitch;


  roll = (roll + _roll) / 2 ;
  pitch = (pitch + _pitch) / 2;
}

MPU_C MPU;
