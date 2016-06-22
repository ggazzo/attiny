#define DEBUG
extern StoreStruct storage;
extern MPU6050 mpu;
// MPU control/status vars
extern bool dmpReady;
extern uint8_t mpuIntStatus; // holds actual interrupt status byte from MPU
extern uint8_t devStatus; // return status after each device operation (0 = success, !0 = error)
extern volatile bool mpuInterrupt;
int mean_ax, mean_ay, mean_az, mean_gx, mean_gy, mean_gz;
///////////////////////////////////   CONFIGURATION   /////////////////////////////
//Change this 3 variables if you want to fine tune the skecth to your needs.
int buffersize = 1000; //Amount of readings used to average, make it higher to get more precision but sketch will be slower  (default:1000)
int acel_deadzone = 8; //Acelerometer error allowed, make it lower to get more precision, but sketch may not converge  (default:8)
int giro_deadzone = 1; //Giro error allowed, make it lower to get more precision, but sketch may not converge  (default:1)

void meansensors() {
    int i = 0;
    long buff_ax = 0, buff_ay = 0, buff_az = 0, buff_gx = 0, buff_gy = 0, buff_gz = 0;
    int16_t ax, ay, az, gx, gy, gz;

    while (i++ < 100) {
      while (!mpuInterrupt){
        mpuInterrupt= false;
#ifdef DEBUG
        Serial.println(F("[DEBUG] waiting for interrupt!"));
#endif
      }
        mpu.getMotion6( & ax, & ay, & az, & gx, & gy, & gz);
      }
      i=0;
      while (i++ < buffersize) {
        while (!mpuInterrupt){
          mpuInterrupt= false;
#ifdef DEBUG
          Serial.println(F("[DEBUG] waiting for interrupt!"));
#endif
        };
          mpu.getMotion6( & ax, & ay, & az, & gx, & gy, & gz);

        buff_ax += ax;
        buff_ay += ay;
        buff_az += az;
        buff_gx += gx;
        buff_gy += gy;
        buff_gz += gz;
    }

    mean_ax = buff_ax / buffersize;
    mean_ay = buff_ay / buffersize;
    mean_az = buff_az / buffersize;
    mean_gx = buff_gx / buffersize;
    mean_gy = buff_gy / buffersize;
    mean_gz = buff_gz / buffersize;

}

void inline calibration() {
    storage.ax_offset = -mean_ax / 8;
    storage.ay_offset = -mean_ay / 8;
    storage.az_offset = (16384 - mean_az) / 8;

    storage.gx_offset = -mean_gx / 4;
    storage.gy_offset = -mean_gy / 4;
    storage.gz_offset = -mean_gz / 4;
    while (1) {
        int ready = 0;
        set_offset();
        meansensors();
        Serial.println("...");

        if (abs(mean_ax) <= acel_deadzone) ready++;
        else storage.ax_offset -= mean_ax / acel_deadzone;

        if (abs(mean_ay) <= acel_deadzone) ready++;
        else storage.ay_offset -= mean_ay / acel_deadzone;

        if (abs(16384 - mean_az) <= acel_deadzone) ready++;
        else storage.az_offset += (16384 - mean_az) / acel_deadzone;

        if (abs(mean_gx) <= giro_deadzone) ready++;
        else storage.gx_offset -= mean_gx / (giro_deadzone + 1);

        if (abs(mean_gy) <= giro_deadzone) ready++;
        else storage.gy_offset -= mean_gy / (giro_deadzone + 1);

        if (abs(mean_gz) <= giro_deadzone) ready++;
        else storage.gz_offset -= mean_gz / (giro_deadzone + 1);

        if (ready == 6) break;
    }
}

void _Calibration() {
    if (!mpu.testConnection()) {
        Serial.println(F("[ERROR]MPU connection problem"));
        return;
    }

    mpu.setXAccelOffset(0);
    mpu.setYAccelOffset(0);
    mpu.setZAccelOffset(0);
    mpu.setXGyroOffset(0);
    mpu.setYGyroOffset(0);
    mpu.setZGyroOffset(0);
#ifdef DEBUG
    Serial.println("[DEBUG]Reading sensors for first time...");
#endif
    meansensors();
    delay(1000);

#ifdef DEBUG
    Serial.println("[DEBUG]Calculating offsets...");
#endif

    calibration();
    delay(1000);

    meansensors();
#ifdef DEBUG
    Serial.println("\n[DEBUG]FINISHED!");
    Serial.println("[DEBUG]Check that your sensor readings are close to 0 0 16384 0 0 0");
#endif
    print_offset();
    set_offset();
}

void print_offset() {
    Serial.print("[DATA][offsets]\t\t");
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
#ifdef DEBUG
    Serial.println("\n[DEBUG]Data is printed as:\tacelX\tacelY\tacelZ\tgiroX\tgiroY\tgiroZ");
#endif
}
