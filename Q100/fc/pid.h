
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PID gain and limit settings
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float pid_p_gain_roll = 3.55;  // Gain setting for the roll P-controller
float pid_i_gain_roll = 0.003; // Gain setting for the roll I-controller
float pid_d_gain_roll = 1.05;  // Gain setting for the roll D-controller
int pid_max_roll = 1000;       // Maximum output of the PID-controller (+/-)

float pid_p_gain_pitch = pid_p_gain_roll; // Gain setting for the pitch P-controller.
float pid_i_gain_pitch = pid_i_gain_roll; // Gain setting for the pitch I-controller.
float pid_d_gain_pitch = pid_d_gain_roll; // Gain setting for the pitch D-controller.
int pid_max_pitch = pid_max_roll;         // Maximum output of the PID-controller (+/-)

float pid_p_gain_yaw = 3.55; // Gain setting for the pitch P-controller. //4.0
float pid_i_gain_yaw = 0.02; // Gain setting for the pitch I-controller. //0.02
float pid_d_gain_yaw = 0.0;  // Gain setting for the pitch D-controller.
int pid_max_yaw = 1000;      // Maximum output of the PID-controller (+/-)

boolean auto_level = false; // Auto level on (true) or off (false)

void calculate_pid()
{

  roll_pid_p = twoX_kp * roll_error;
  pitch_pid_p = twoX_kp * pitch_error;
  yaw_pid_p = yaw_kp * yaw_error;

  if (-3 < roll_error < 3)
  {
    roll_pid_i = roll_pid_i + (twoX_ki * roll_error);
  }
  if (-3 < pitch_error < 3)
  {
    pitch_pid_i = pitch_pid_i + (twoX_ki * pitch_error);
  }
  if (-3 < yaw_error < 3)
  {
    yaw_pid_i = yaw_pid_i + (yaw_ki * yaw_error);
  }

  roll_pid_d = twoX_kd * ((roll_error - roll_previous_error) / elapsedTime);
  pitch_pid_d = twoX_kd * ((pitch_error - pitch_previous_error) / elapsedTime);
  roll_PID = roll_pid_p + roll_pid_i + roll_pid_d;
  pitch_PID = pitch_pid_p + pitch_pid_i + pitch_pid_d;
  yaw_PID = yaw_pid_p + yaw_pid_i;

  if (roll_PID < -400)
  {
    roll_PID = -400;
  }
  else if (roll_PID > 400)
  {
    roll_PID = 400;
  }
  if (pitch_PID < -400)
  {
    pitch_PID = -400;
  }
  else if (pitch_PID > 400)
  {
    pitch_PID = 400;
  }
  if (yaw_PID < -400)
  {
    yaw_PID = -400;
  }
  else if (yaw_PID > 400)
  {
    yaw_PID = 400;
  }
}
