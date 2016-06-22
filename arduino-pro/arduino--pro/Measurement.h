// #define DEBUG
#ifndef IGNORE_MEASUREMENTS
  #define IGNORE_MEASUREMENTS 50
#endif

extern MPU6050 mpu;

bool dmpReady = false; // set true if DMP init was successful

uint8_t mpuIntStatus; // holds actual interrupt status byte from MPU
uint8_t devStatus; // return status after each device operation (0 = success, !0 = error)
volatile bool mpuInterrupt = false;
// ================================================================
// ===               INTERRUPT DETECTION ROUTINE                ===
// ================================================================

void dmpDataReady() {
    mpuInterrupt = true;
}

static uint16_t packetSize; // expected DMP packet size (default is 42 bytes)
static VectorFloat gravity; // [x, y, z]            gravity vector
static Quaternion q; // [w, x, y, z]         quaternion container
static float euler[3]; // [psi, theta, phi]    Euler angle container
static float ypr[3]; // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
static float buffer_ypr[3]; // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

static uint16_t fifoCount; // count of all bytes currently in FIFO
static uint8_t fifoBuffer[64]; // FIFO storage buffer
void _read_ypr() {

    int a = IGNORE_MEASUREMENTS * 2;
    // if programming failed, don't try to do anything
    if (!dmpReady) {
      Serial.println(F("[ERROR] DMP NOT READY"));
      return;
    };

    // wait for MPU interrupt or extra packet(s) available


    do {
        while (!mpuInterrupt){
#ifdef DEBUG
          Serial.println(F("[DEBUG] waiting for interrupt!"));
#endif
        };
        // reset interrupt flag and get INT_STATUS byte
        mpuInterrupt = false;
        mpuIntStatus = mpu.getIntStatus();

        // get current FIFO count
        fifoCount = mpu.getFIFOCount();
        if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
            // reset so we can continue cleanly
            mpu.resetFIFO();
            // Serial.println(F("FIFO overflow!"));
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

            mpu.dmpGetQuaternion( & q, fifoBuffer);
            mpu.dmpGetGravity( & gravity, & q);
            mpu.dmpGetYawPitchRoll(ypr, & q, & gravity);
            a--;
            if (a > IGNORE_MEASUREMENTS) {
#ifdef DEBUG
                Serial.print(F("[DEBUG] Ignoring measurement number:"));
                Serial.print(a-IGNORE_MEASUREMENTS);
#endif
                continue;
            }
#ifdef DEBUG
            Serial.print(F("[DEBUG] Reading measurement number:"));
            Serial.println(a);
#endif
            buffer_ypr[0] += ypr[0];
            buffer_ypr[1] += ypr[1];
            buffer_ypr[2] += ypr[2];
        }

    } while (a > 0);

    buffer_ypr[0] /= IGNORE_MEASUREMENTS;
    buffer_ypr[1] /= IGNORE_MEASUREMENTS;
    buffer_ypr[2] /= IGNORE_MEASUREMENTS;

    Serial.print("[DATA][ypr]\t");
    Serial.print(buffer_ypr[0] * 180 / M_PI);
    Serial.print("\t");
    Serial.print(buffer_ypr[1] * 180 / M_PI);
    Serial.print("\t");
    Serial.println(buffer_ypr[2] * 180 / M_PI);
}

void initialize() {
    mpu.initialize();
    Serial.println(F("[DEBUG]Testing device connections..."));
    if (!mpu.testConnection()) {
        Serial.print(F("[ERROR]MPU6050 connection "));
        Serial.println(F("failed"));
        return;
    }
#ifdef DEBUG
    Serial.print(F("[DEGUB]MPU6050 connection "));
    Serial.println(F("successful"));
    while (Serial.available() && Serial.read()); // empty buffer
    while (!Serial.available()) {
        Serial.println(F("[DEBUG]Send any character to start..."));
        delay(200);
    }

    while (Serial.available() && Serial.read()); // empty buffer again

    Serial.println(F("[DEBUG]Initializing DMP..."));
#endif

    devStatus = mpu.dmpInitialize();
#ifdef DEBUG
    Serial.println(F("[DEBUG]Reading offsets..."));
#endif
    _read_offsets();

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
#ifdef DEBUG
        Serial.println(F("[DEBUG]Enabling DMP..."));
#endif
        mpu.setDMPEnabled(true);
#ifdef DEBUG
        Serial.println(F("[DEBUG]Enabling interrupt detection (Arduino external interrupt 0)..."));
#endif
        attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();
#ifdef DEBUG
        Serial.println(F("[DEBUG]DMP ready! Waiting for first interrupt..."));
#endif
        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        dmpReady = true;
        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        Serial.print(F("[ERROR]DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
    }

}
