#include "EEPROM.h"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"
#include "CmdBuffer.h"
#include "CmdCallback.h"
#include "CmdParser.h"

CmdCallback<5> cmdCallback;

///////////////////////////////////   CONFIGURATION   /////////////////////////////
//Change this 3 variables if you want to fine tune the skecth to your needs.
int buffersize=1000;     //Amount of readings used to average, make it higher to get more precision but sketch will be slower  (default:1000)
int acel_deadzone=8;     //Acelerometer error allowed, make it lower to get more precision, but sketch may not converge  (default:8)
int giro_deadzone=1;     //Giro error allowed, make it lower to get more precision, but sketch may not converge  (default:1)



MPU6050 mpu;
#define LED_PIN 13      // (Galileo/Arduino is 13)
bool blinkState = false;
// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
VectorFloat gravity;    // [x, y, z]            gravity vector
Quaternion q;           // [w, x, y, z]         quaternion container
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
float buffer_ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

// This function is not required when using the Galileo
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {


    Wire.begin();
    TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)

    Serial.begin(115200);
    while (!Serial);
    Serial.println(F("Device started..."));
    cmdCallback.addCmd("<calibration>", &Calibration);
    cmdCallback.addCmd("<mpu_initialize>", &mpu_initialize);
    cmdCallback.addCmd("<read_ypr>", &read_ypr);
    cmdCallback.addCmd("<save_offsets>", &save_offsets);
    cmdCallback.addCmd("<read_offsets>", &read_offsets);
    // configure LED for output
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    // blink LED to indicate activity
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
    delay(100);
}

void serialEvent() {
  CmdBuffer<32> myBuffer;
  CmdParser     myParser;
  cmdCallback.loopCmdProcessing(&myParser, &myBuffer, &Serial);
}

void mpu_initialize(CmdParser *myParser) {
  mpu.initialize();
  Serial.print(F("Testing device connections..."));
  Serial.print(F("MPU6050 connection "));
  if(!mpu.testConnection()){
      Serial.println(F("failed"));
      return;
  }
  Serial.println(F("successful"));

  while (Serial.available() && Serial.read()); // empty buffer
  while (!Serial.available()){
    Serial.println(F("Send any character to start..."));
    delay(200);
  }
  while (Serial.available() && Serial.read()); // empty buffer again

  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();


  Serial.println(F("Reading offsets..."));
  _read_offsets();

  // make sure it worked (returns 0 if so)
  if (devStatus == 0) {
      // turn on the DMP, now that it's ready
      Serial.println(F("Enabling DMP..."));
      mpu.setDMPEnabled(true);
      // enable Arduino interrupt detection
      Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
      attachInterrupt(0, dmpDataReady, RISING);
      mpuIntStatus = mpu.getIntStatus();

      // set our DMP Ready flag so the main loop() function knows it's okay to use it
      Serial.println(F("DMP ready! Waiting for first interrupt..."));
      dmpReady = true;
      // get expected DMP packet size for later comparison
      packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
      // ERROR!
      // 1 = initial memory load failed
      // 2 = DMP configuration updates failed
      // (if it's going to break, usually the code will be 1)
      Serial.print(F("DMP Initialization failed (code "));
      Serial.print(devStatus);
      Serial.println(F(")"));
  }



}


#define CONFIG_VERSION "ls1"

struct StoreStruct {
  // This is for mere detection if they are your settings
  char version[4];
  int ax_offset,ay_offset,az_offset,gx_offset,gy_offset,gz_offset;
} storage = {
  CONFIG_VERSION,
  -1436,
  1083,
  1070,
  53,
  7,
  0
};

#define CONFIG_START 32
void save_offsets(CmdParser *myParser){
  for (unsigned int t=0; t<sizeof(storage); t++)
    EEPROM.write(CONFIG_START + t, *((char*)&storage + t));
  print_offset();
}
void _read_offsets(){
  if (EEPROM.read(CONFIG_START + 0) == CONFIG_VERSION[0] &&
      EEPROM.read(CONFIG_START + 1) == CONFIG_VERSION[1] &&
      EEPROM.read(CONFIG_START + 2) == CONFIG_VERSION[2]){
  for (unsigned int t=0; t<sizeof(storage); t++)
    *((char*)&storage + t) = EEPROM.read(CONFIG_START + t);
        
    }else {
      Serial.println(F("No valid memory"));
    }
  print_offset();
  mpu.setXGyroOffset(storage.gx_offset);
  mpu.setYGyroOffset(storage.gy_offset);
  mpu.setZGyroOffset(storage.gz_offset);
  mpu.setXAccelOffset(storage.ax_offset);
  mpu.setYAccelOffset(storage.ay_offset);
  mpu.setZAccelOffset(storage.az_offset);
}
void read_offsets(CmdParser *myParser){
  _read_offsets();
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================


void read_ypr(CmdParser *myParser){
  int a = 300;
  // if programming failed, don't try to do anything
  if (!dmpReady) return ;

      // wait for MPU interrupt or extra packet(s) available


      while (!mpuInterrupt && fifoCount < packetSize);

      do {
          // reset interrupt flag and get INT_STATUS byte
          mpuInterrupt = false;
          mpuIntStatus = mpu.getIntStatus();
    
          // get current FIFO count
          fifoCount = mpu.getFIFOCount();
          if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
              // reset so we can continue cleanly
              mpu.resetFIFO();
              Serial.println(F("FIFO overflow!"));
    
              continue;
          }

          if (mpuIntStatus & 0x02) {
            
              
              // wait for correct available data length, should be a VERY short wait
              while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    
              // read a packet from FIFO
              mpu.getFIFOBytes(fifoBuffer, packetSize);
    
              // track FIFO count here in case there is > 1 packet available
              // (this lets us immediately read more without waiting for an interrupt)
              fifoCount -= packetSize;
    
              mpu.dmpGetQuaternion(&q, fifoBuffer);
              mpu.dmpGetGravity(&gravity, &q);
              mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);              
              a--;

              if(a>200){continue;}
              buffer_ypr[0] += ypr[0];
              buffer_ypr[1] += ypr[1];
              buffer_ypr[2] += ypr[2];

              
          }
          
  
        } while(a);
        
        buffer_ypr[0] /= 100;
        buffer_ypr[1] /= 100;
        buffer_ypr[2] /= 100;
        
        Serial.print("ypr\t");
        Serial.print(buffer_ypr[0] * 180/M_PI);
        Serial.print("\t");
        Serial.print(buffer_ypr[1] * 180/M_PI);
        Serial.print("\t");
        Serial.println(buffer_ypr[2] * 180/M_PI);
}

int mean_ax,mean_ay,mean_az,mean_gx,mean_gy,mean_gz;
void inline calibration(){
  storage.ax_offset=-mean_ax/8;
  storage.ay_offset=-mean_ay/8;
  storage.az_offset=(16384-mean_az)/8;

  storage.gx_offset=-mean_gx/4;
  storage.gy_offset=-mean_gy/4;
  storage.gz_offset=-mean_gz/4;
  while (1){
    int ready=0;
    mpu.setXAccelOffset(storage.ax_offset);
    mpu.setYAccelOffset(storage.ay_offset);
    mpu.setZAccelOffset(storage.az_offset);

    mpu.setXGyroOffset(storage.gx_offset);
    mpu.setYGyroOffset(storage.gy_offset);
    mpu.setZGyroOffset(storage.gz_offset);

    meansensors();
    Serial.println("...");

    if (abs(mean_ax)<=acel_deadzone) ready++;
    else storage.ax_offset -= mean_ax/acel_deadzone;

    if (abs(mean_ay)<=acel_deadzone) ready++;
    else storage.ay_offset -= mean_ay/acel_deadzone;

    if (abs(16384-mean_az)<=acel_deadzone) ready++;
    else storage.az_offset +=(16384-mean_az)/acel_deadzone;

    if (abs(mean_gx)<=giro_deadzone) ready++;
    else storage.gx_offset -= mean_gx/(giro_deadzone+1);

    if (abs(mean_gy)<=giro_deadzone) ready++;
    else storage.gy_offset -= mean_gy/(giro_deadzone+1);

    if (abs(mean_gz)<=giro_deadzone) ready++;
    else storage.gz_offset -= mean_gz/(giro_deadzone+1);

    if (ready==6) break;
  }
}

void Calibration(CmdParser *myParser) {
    if(!mpu.testConnection()){
      Serial.println(F("MPU connection problem"));
      return;
    }

    mpu.setXAccelOffset(0);
    mpu.setYAccelOffset(0);
    mpu.setZAccelOffset(0);
    mpu.setXGyroOffset(0);
    mpu.setYGyroOffset(0);
    mpu.setZGyroOffset(0);
    Serial.println("\nReading sensors for first time...");
    meansensors();
    delay(1000);

    Serial.println("\nCalculating offsets...");
    calibration();
    delay(1000);

    meansensors();
    Serial.println("\nFINISHED!");
    print_offset();
    Serial.println("Check that your sensor readings are close to 0 0 16384 0 0 0");
}

void print_offset(){
  Serial.print("Your offsets:\t");
    Serial.print(storage.ax_offset);
    Serial.print("\t");
    Serial.print(storage.ay_offset);
    Serial.print("\t");
    Serial.print(storage.az_offset);
    Serial.print("\t");
    Serial.print(storage.gx_offset);
    Serial.print("\t");
    Serial.print(storage.gy_offset);
    Serial.print("\t");
    Serial.println(storage.gz_offset);
    Serial.println("\nData is printed as:\tacelX\tacelY\tacelZ\tgiroX\tgiroY\tgiroZ");
}

void meansensors(){
  int i=0;
  long buff_ax=0,buff_ay=0,buff_az=0,buff_gx=0,buff_gy=0,buff_gz=0;
  int16_t ax, ay, az,gx, gy, gz;

  while (i++<(buffersize+101)){

    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    if (i<100){ continue; }

    buff_ax+=ax;
    buff_ay+=ay;
    buff_az+=az;
    buff_gx+=gx;
    buff_gy+=gy;
    buff_gz+=gz;
    delay(2);
  }

  mean_ax=buff_ax/buffersize;
  mean_ay=buff_ay/buffersize;
  mean_az=buff_az/buffersize;
  mean_gx=buff_gx/buffersize;
  mean_gy=buff_gy/buffersize;
  mean_gz=buff_gz/buffersize;

}
