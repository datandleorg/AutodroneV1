// Radio
extern const int pinCE = 9;   // This pin is used to set the nRF24 to standby (0) or active mode (1)
extern const int pinCSN = 10; // This pin is used to tell the nRF24 whether the SPI communication is a command or message to send out
extern int IRQ_PIN = 2;
boolean radioStatus = false;
extern int throttle = 0;
extern int battery_voltage = 0;
extern int esc_1=0, esc_2=0, esc_3=0, esc_4=0;
extern boolean flightStatus = false;
extern float gyro_roll_input = 0,gyro_pitch_input=0,gyro_yaw_input=0;

#include "radio.h";
#include "mpu.h";
#include "pwm.h";
#include "pid.h";

const int flightStatusLED_pin = 7;

void setup()
{
     Serial.begin(115200); // start serial to communicate process
     setupRadio();
     setupIMU();
     setupPWM();
}

void loop()
{
     Serial.println(throttle);
     Serial.println(flightStatus);


     if (flightStatus)
     {
          resetPID();
          calculateSetPoint();
          calculate_pid();
          calculate_ESC();
          sendPWM();
          digitalWrite(LED_BUILTIN, HIGH); 
     }
     else
     {
          digitalWrite(LED_BUILTIN, LOW);
     }
}
