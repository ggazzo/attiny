#include "EEPROM.h"
extern MPU6050 mpu;

#define CONFIG_START 32

#define CONFIG_VERSION "ls1"

typedef struct _store {
    // This is for mere detection if they are your settings
    char version[4];
    int ax_offset, ay_offset, az_offset, gx_offset, gy_offset, gz_offset;
} StoreStruct ;

StoreStruct storage = {
    CONFIG_VERSION,
    -1436,
    1083,
    1070,
    53,
    7,
    0
};


extern void print_offset();

void _save_offsets() {
    for (unsigned int t = 0; t < sizeof(storage); t++)
        EEPROM.write(CONFIG_START + t, * ((char * ) & storage + t));
    print_offset();
}

void set_offset (){
  mpu.setXGyroOffset(storage.gx_offset);
  mpu.setYGyroOffset(storage.gy_offset);
  mpu.setZGyroOffset(storage.gz_offset);
  mpu.setXAccelOffset(storage.ax_offset);
  mpu.setYAccelOffset(storage.ay_offset);
  mpu.setZAccelOffset(storage.az_offset);
}

void _read_offsets() {
    if (EEPROM.read(CONFIG_START + 0) == CONFIG_VERSION[0] &&
        EEPROM.read(CONFIG_START + 1) == CONFIG_VERSION[1] &&
        EEPROM.read(CONFIG_START + 2) == CONFIG_VERSION[2]) {
        for (unsigned int t = 0; t < sizeof(storage); t++)
            * ((char * ) & storage + t) = EEPROM.read(CONFIG_START + t);

    }
#ifdef DEBUG
    else {
        Serial.println(F("No valid memory"));
    }
#endif
    print_offset();
    set_offset();
}
