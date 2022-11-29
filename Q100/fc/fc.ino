// Radio
extern const int pinCE = 9;   // This pin is used to set the nRF24 to standby (0) or active mode (1)
extern const int pinCSN = 10; // This pin is used to tell the nRF24 whether the SPI communication is a command or message to send out
extern int IRQ_PIN = 2;
boolean radioStatus = false;
extern int throttle = 0;
extern int esc_1, esc_2, esc_3, esc_4;
extern boolean flightStatus = false;

#include "radio.h";
#include "mpu.h";

const int flightStatusLED_pin = 7;

void setup()
{
     Serial.begin(115200); // start serial to communicate process
     setupRadio();
     setupIMU();
}

void loop()
{
     Serial.println(throttle);
     Serial.println(flightStatus);
     if (flightStatus)
     {
          digitalWrite(flightStatusLED_pin, HIGH);
     }
     else
     {
          digitalWrite(flightStatusLED_pin, LOW);
     }
     read_IMU();
}
