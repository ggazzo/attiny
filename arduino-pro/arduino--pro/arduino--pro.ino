#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "Wire.h"
#include "Storage.h"
#include "Measurement.h"
#include "calibration.h"
#include "3rd/CmdBuffer.h"
#include "3rd/CmdCallback.h"
#include "3rd/CmdParser.h"
#define LED_PIN 13 // (Galileo/Arduino is 13)

CmdCallback < 7 > cmdCallback;
bool autoread = false;


void initialize();

#ifdef LED_PIN
bool blinkState = false;
#endif
// MPU control/status vars
extern StoreStruct storage;
extern bool dmpReady;
extern uint16_t packetSize;
extern uint8_t mpuIntStatus; // holds actual interrupt status byte from MPU
extern uint8_t devStatus; // return status after each device operation (0 = success, !0 = error)

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================
MPU6050 mpu;
void setup() {
    Wire.begin();
    TWBR = 24; // 400kHz I2C clock (200kHz if CPU is 8MHz)
    Serial.begin(115200);
    while (!Serial);
    initialize();
    Serial.println(F("Device started..."));
    cmdCallback.addCmd("<calibration>", & Calibration);
    cmdCallback.addCmd("<mpu_initialize>", & mpu_initialize);
    cmdCallback.addCmd("<read_ypr>", & read_ypr);
    cmdCallback.addCmd("<save_offsets>", &save_offsets);
    cmdCallback.addCmd("<read_offsets>", &read_offsets);
    cmdCallback.addCmd("<auto_start>", & auto_start);
    cmdCallback.addCmd("<auto_stop>", & auto_stop);
#ifdef LED_PIN
    pinMode(LED_PIN, OUTPUT);
#endif
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
#ifdef LED_PIN
    blinkState = !blinkState;
    digitalWrite(LED_PIN, blinkState);
#endif

    if (autoread) _read_ypr(); else delay(100);
}

void serialEvent() {
    CmdBuffer < 32 > cmdBuffer;
    CmdParser cmdParser;
    if (cmdBuffer.readFromSerial( & Serial)) {
        // parse command line
        if (cmdParser.parseCmd( & cmdBuffer) != CMDPARSER_ERROR) {
            // search command in store and call function
            if (cmdCallback.processCmd( & cmdParser)) {
                // FIXME: handling cmd not found
            }
        }
    }
}

void mpu_initialize(CmdParser * myParser) {
    initialize();
}
void auto_stop(CmdParser * myParser) {
    autoread = false;
}
void auto_start(CmdParser * myParser) {
    autoread = true;
}
void Calibration(CmdParser * myParser)  {
  _Calibration();
}
void read_ypr(CmdParser * myParser) {
    _read_ypr();
}
void save_offsets(CmdParser * myParser) {
    _save_offsets();
}
void read_offsets(CmdParser * myParser) {
    _read_offsets();
}
