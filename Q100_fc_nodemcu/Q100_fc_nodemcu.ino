#include <Arduino.h>

boolean radioStatus = false;  
extern float gyro_roll_input = 0,gyro_pitch_input=0,gyro_yaw_input=0;

#include "wifi.h"
#include "mpu.h"

void setup() {
    Serial.begin(115200);
    initRadio();
    setupIMU();
}

void loop() {
    read_IMU();
    notifyClients();
    delay(10);
}
