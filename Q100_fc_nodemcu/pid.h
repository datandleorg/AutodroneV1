
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PID gain and limit settings
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float pid_p_gain_roll = 3.55;  // Gain setting for the roll P-controller
float pid_i_gain_roll = 0.003; // Gain setting for the roll I-controller
float pid_d_gain_roll = 1.05;  // Gain setting for the roll D-controller
int pid_max_roll = 1500;       // Maximum output of the PID-controller (+/-)

float pid_p_gain_pitch = pid_p_gain_roll; // Gain setting for the pitch P-controller.
float pid_i_gain_pitch = pid_i_gain_roll; // Gain setting for the pitch I-controller.
float pid_d_gain_pitch = pid_d_gain_roll; // Gain setting for the pitch D-controller.
int pid_max_pitch = pid_max_roll;         // Maximum output of the PID-controller (+/-)

float pid_p_gain_yaw = 3.55; // Gain setting for the pitch P-controller. //4.0
float pid_i_gain_yaw = 0.02; // Gain setting for the pitch I-controller. //0.02
float pid_d_gain_yaw = 0.0;  // Gain setting for the pitch D-controller.
int pid_max_yaw = 1500;      // Maximum output of the PID-controller (+/-)

boolean auto_level = false; // Auto level on (true) or off (false)

float pid_error_temp;
float pid_i_mem_roll, pid_roll_setpoint, pid_output_roll, pid_last_roll_d_error;
float pid_i_mem_pitch, pid_pitch_setpoint, pid_output_pitch, pid_last_pitch_d_error;
float pid_i_mem_yaw, pid_yaw_setpoint, pid_output_yaw, pid_last_yaw_d_error;

volatile int receiver_input_channel_1 = 1500, receiver_input_channel_2 = 1500, receiver_input_channel_3, receiver_input_channel_4= 1500;
float roll_level_adjust, pitch_level_adjust;

void resetPID(){
            //Reset the PID controllers for a bumpless start.
          pid_i_mem_roll = 0;
          pid_last_roll_d_error = 0;
          pid_i_mem_pitch = 0;
          pid_last_pitch_d_error = 0;
          pid_i_mem_yaw = 0;
          pid_last_yaw_d_error = 0;
}

void calculateSetPoint(){
    //The PID set point in degrees per second is determined by the roll receiver input.
  //In the case of deviding by 3 the max roll rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s ).
  pid_roll_setpoint = 0;
  //We need a little dead band of 16us for better results.
  if(receiver_input_channel_1 > 1508)pid_roll_setpoint = receiver_input_channel_1 - 1508;
  else if(receiver_input_channel_1 < 1492)pid_roll_setpoint = receiver_input_channel_1 - 1492;

  pid_roll_setpoint -= roll_level_adjust;                                   //Subtract the angle correction from the standardized receiver roll input value.
  pid_roll_setpoint /= 3.0;                                                 //Divide the setpoint for the PID roll controller by 3 to get angles in degrees.


  //The PID set point in degrees per second is determined by the pitch receiver input.
  //In the case of deviding by 3 the max pitch rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s ).
  pid_pitch_setpoint = 0;
  //We need a little dead band of 16us for better results.
  if(receiver_input_channel_2 > 1508)pid_pitch_setpoint = receiver_input_channel_2 - 1508;
  else if(receiver_input_channel_2 < 1492)pid_pitch_setpoint = receiver_input_channel_2 - 1492;

  pid_pitch_setpoint -= pitch_level_adjust;                                  //Subtract the angle correction from the standardized receiver pitch input value.
  pid_pitch_setpoint /= 3.0;                                                 //Divide the setpoint for the PID pitch controller by 3 to get angles in degrees.

  //The PID set point in degrees per second is determined by the yaw receiver input.
  //In the case of deviding by 3 the max yaw rate is aprox 164 degrees per second ( (500-8)/3 = 164d/s ).
  pid_yaw_setpoint = 0;
  //We need a little dead band of 16us for better results.
  if(receiver_input_channel_3 > 1050){ //Do not yaw when turning off the motors.
    if(receiver_input_channel_4 > 1508)pid_yaw_setpoint = (receiver_input_channel_4 - 1508)/3.0;
    else if(receiver_input_channel_4 < 1492)pid_yaw_setpoint = (receiver_input_channel_4 - 1492)/3.0;
  }
}

void calculate_ESC(){
  battery_voltage = battery_voltage * 0.92 + (analogRead(0) + 65) * 0.09853;

  //Turn on the led if battery voltage is to low.
  if(battery_voltage < 1000 && battery_voltage > 600)digitalWrite(12, HIGH);


  // throttle = receiver_input_channel_3;                                      //We need the throttle signal as a base signal.

  if (flightStatus){                                                          //The motors are started.
    if (throttle > 2800) throttle = 2800;                                   //We need some room to keep full control at full throttle.
    esc_1 = throttle - pid_output_pitch + pid_output_roll - pid_output_yaw; //Calculate the pulse for esc 1 (front-right - CCW) 3
    esc_2 = throttle + pid_output_pitch + pid_output_roll + pid_output_yaw; //Calculate the pulse for esc 2 (rear-right - CW) 4
    esc_3 = throttle + pid_output_pitch - pid_output_roll - pid_output_yaw; //Calculate the pulse for esc 3 (rear-left - CCW) 5
    esc_4 = throttle - pid_output_pitch - pid_output_roll + pid_output_yaw; //Calculate the pulse for esc 4 (front-left - CW) 6

    if (battery_voltage < 1240 && battery_voltage > 800){                   //Is the battery connected?
      esc_1 += esc_1 * ((1240 - battery_voltage)/(float)3500);              //Compensate the esc-1 pulse for voltage drop.
      esc_2 += esc_2 * ((1240 - battery_voltage)/(float)3500);              //Compensate the esc-2 pulse for voltage drop.
      esc_3 += esc_3 * ((1240 - battery_voltage)/(float)3500);              //Compensate the esc-3 pulse for voltage drop.
      esc_4 += esc_4 * ((1240 - battery_voltage)/(float)3500);              //Compensate the esc-4 pulse for voltage drop.
    } 

    if (esc_1 < 100) esc_1 = 100;                                         //Keep the motors running.
    if (esc_2 < 100) esc_2 = 100;                                         //Keep the motors running.
    if (esc_3 < 100) esc_3 = 100;                                         //Keep the motors running.
    if (esc_4 < 100) esc_4 = 100;                                         //Keep the motors running.

    if(esc_1 > 3000)esc_1 = 3000;                                           //Limit the esc-1 pulse to 2000us.
    if(esc_2 > 3000)esc_2 = 3000;                                           //Limit the esc-2 pulse to 2000us.
    if(esc_3 > 3000)esc_3 = 3000;                                           //Limit the esc-3 pulse to 2000us.
    if(esc_4 > 3000)esc_4 = 3000;                                           //Limit the esc-4 pulse to 2000us.  
  }

  else{
    esc_1 = 0;                                                           //If start is not 2 keep a 1000us pulse for ess-1.
    esc_2 = 0;                                                           //If start is not 2 keep a 1000us pulse for ess-2.
    esc_3 = 0;                                                           //If start is not 2 keep a 1000us pulse for ess-3.
    esc_4 = 0;                                                           //If start is not 2 keep a 1000us pulse for ess-4.
  }

  Serial.println(" ");
  Serial.print("  |  esc_1 : ");
  Serial.print(esc_1);
  Serial.print("  |  esc_2 : ");
  Serial.print(esc_2);  
  Serial.print("  |  esc_3 : ");
  Serial.print(esc_3); 
  Serial.print("  |  esc_4 : ");
  Serial.print(esc_4);
}

void calculate_pid(){
  //Roll calculations
  pid_error_temp = gyro_roll_input - pid_roll_setpoint;
  pid_i_mem_roll += pid_i_gain_roll * pid_error_temp;
  if(pid_i_mem_roll > pid_max_roll)pid_i_mem_roll = pid_max_roll;
  else if(pid_i_mem_roll < pid_max_roll * -1)pid_i_mem_roll = pid_max_roll * -1;

  pid_output_roll = pid_p_gain_roll * pid_error_temp + pid_i_mem_roll + pid_d_gain_roll * (pid_error_temp - pid_last_roll_d_error);
  if(pid_output_roll > pid_max_roll)pid_output_roll = pid_max_roll;
  else if(pid_output_roll < pid_max_roll * -1)pid_output_roll = pid_max_roll * -1;

  pid_last_roll_d_error = pid_error_temp;

  //Pitch calculations
  pid_error_temp = gyro_pitch_input - pid_pitch_setpoint;
  pid_i_mem_pitch += pid_i_gain_pitch * pid_error_temp;
  if(pid_i_mem_pitch > pid_max_pitch)pid_i_mem_pitch = pid_max_pitch;
  else if(pid_i_mem_pitch < pid_max_pitch * -1)pid_i_mem_pitch = pid_max_pitch * -1;

  pid_output_pitch = pid_p_gain_pitch * pid_error_temp + pid_i_mem_pitch + pid_d_gain_pitch * (pid_error_temp - pid_last_pitch_d_error);
  if(pid_output_pitch > pid_max_pitch)pid_output_pitch = pid_max_pitch;
  else if(pid_output_pitch < pid_max_pitch * -1)pid_output_pitch = pid_max_pitch * -1;

  pid_last_pitch_d_error = pid_error_temp;

  //Yaw calculations
  pid_error_temp = gyro_yaw_input - pid_yaw_setpoint;
  pid_i_mem_yaw += pid_i_gain_yaw * pid_error_temp;
  if(pid_i_mem_yaw > pid_max_yaw)pid_i_mem_yaw = pid_max_yaw;
  else if(pid_i_mem_yaw < pid_max_yaw * -1)pid_i_mem_yaw = pid_max_yaw * -1;

  pid_output_yaw = pid_p_gain_yaw * pid_error_temp + pid_i_mem_yaw + pid_d_gain_yaw * (pid_error_temp - pid_last_yaw_d_error);
  if(pid_output_yaw > pid_max_yaw)pid_output_yaw = pid_max_yaw;
  else if(pid_output_yaw < pid_max_yaw * -1)pid_output_yaw = pid_max_yaw * -1;

  pid_last_yaw_d_error = pid_error_temp;
}
