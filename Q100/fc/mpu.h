#include <Wire.h> 

//Gyro Variables
float elapsedTime, time, timePrev;        //Variables for time control
int gyro_error=0;                         //We use this variable to only calculate once the gyro data error
float Gyr_rawX, Gyr_rawY, Gyr_rawZ;     //Here we store the raw data read 
float Gyro_angle_x, Gyro_angle_y, Gyro_angle_z;         //Here we store the angle value obtained with Gyro data
float Gyro_raw_error_x, Gyro_raw_error_y, Gyro_raw_error_z; //Here we store the initial gyro data error

//Acc Variables
int acc_error=0;                         //We use this variable to only calculate once the Acc data error
float rad_to_deg = 180/3.141592654;      //This value is for pasing from radians to degrees values
float Acc_rawX, Acc_rawY, Acc_rawZ;    //Here we store the raw data read 
float Acc_angle_x, Acc_angle_y, Acc_angle_z;          //Here we store the angle value obtained with Acc data
float Acc_angle_error_x, Acc_angle_error_y, Acc_angle_error_z; //Here we store the initial Acc data error

float Total_angle_x, Total_angle_y, Total_angle_z;
unsigned long loop_timer;
int temperature;
float gyro_roll_input,gyro_pitch_input,gyro_yaw_input;
boolean set_gyro_angles = false;


void calcAccError(){

  /*Here we calculate the acc data error before we start the loop
 * I make the mean of 200 values, that should be enough*/
  if(acc_error==0)
  {
    for(int a=0; a<200; a++)
    {
      Wire.beginTransmission(0x68);
      Wire.write(0x3B);                       //Ask for the 0x3B register- correspond to AcX
      Wire.endTransmission(false);
      Wire.requestFrom(0x68,6,true); 
      
      Acc_rawX=(Wire.read()<<8|Wire.read())/4096.0 ; //each value needs two registres
      Acc_rawY=(Wire.read()<<8|Wire.read())/4096.0 ;
      Acc_rawZ=(Wire.read()<<8|Wire.read())/4096.0 ;

      
      /*---X---*/
      Acc_angle_error_x = Acc_angle_error_x + ((atan((Acc_rawY)/sqrt(pow((Acc_rawX),2) + pow((Acc_rawZ),2)))*rad_to_deg));
      /*---Y---*/
      Acc_angle_error_y = Acc_angle_error_y + ((atan(-1*(Acc_rawX)/sqrt(pow((Acc_rawY),2) + pow((Acc_rawZ),2)))*rad_to_deg)); 
      
      if(a==199)
      {
        Acc_angle_error_x = Acc_angle_error_x/200;
        Acc_angle_error_y = Acc_angle_error_y/200;
        Acc_angle_error_z = Acc_angle_error_z/200;
        acc_error=1;
      }
    }
  }//end of acc error calculation   
}

void calcGyroError(){

/*Here we calculate the gyro data error before we start the loop
 * I make the mean of 200 values, that should be enough*/
  if(gyro_error==0)
  {
    for(int i=0; i<200; i++)
    {
      Wire.beginTransmission(0x68);            //begin, Send the slave adress (in this case 68) 
      Wire.write(0x43);                        //First adress of the Gyro data
      Wire.endTransmission(false);
      Wire.requestFrom(0x68,6,true);           //We ask for just 4 registers 
         
      Gyr_rawX=Wire.read()<<8|Wire.read();     //Once again we shif and sum
      Gyr_rawY=Wire.read()<<8|Wire.read();
      Gyr_rawZ=Wire.read()<<8|Wire.read();
   
      /*---X---*/
      Gyro_raw_error_x = Gyro_raw_error_x + (Gyr_rawX/32.8); 
      /*---Y---*/
      Gyro_raw_error_y = Gyro_raw_error_y + (Gyr_rawY/32.8);
      /*---Z---*/
      Gyro_raw_error_z = Gyro_raw_error_z + (Gyr_rawZ/32.8);
      if(i==199)
      {
        Gyro_raw_error_x = Gyro_raw_error_x/200;
        Gyro_raw_error_y = Gyro_raw_error_y/200;
        Gyro_raw_error_z = Gyro_raw_error_z/200;
        gyro_error=1;
      }
    }
  }//end of gyro error calculation 
}


void connectIMU(){
    //Activate the MPU-6050
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x6B);                                                    //Send the requested starting register
  Wire.write(0x00);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
  //Configure the accelerometer (+/-8g)
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x1C);                                                    //Send the requested starting register
  Wire.write(0x10);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
  //Configure the gyro (1000dps full scale)
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x1B);                                                    //Send the requested starting register
  Wire.write(0x10);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission                                                  //End the transmission with the gyro    
}

void setupIMU() {
  // put your setup code here, to run once:
  Wire.begin();                                                        //Start I2C as master
  connectIMU();
  delay(6000); 
  calcAccError();
  calcGyroError();
}

void read_IMU(){
  Wire.beginTransmission(0x68);                                   //Start communication with the gyro.
  Wire.write(0x3B);                                                       //Start reading @ register 43h and auto increment with every read.
  Wire.endTransmission();                                                 //End the transmission.
  Wire.requestFrom(0x68,14);                                      //Request 14 bytes from the gyro.

  while(Wire.available() < 14);                                           //Wait until the 14 bytes are received.
  Acc_rawX = (Wire.read()<<8|Wire.read())/4096.0;                               //Add the low and high byte to the acc_x variable.
  Acc_rawY = (Wire.read()<<8|Wire.read())/4096.0;                               //Add the low and high byte to the acc_y variable.
  Acc_rawZ = (Wire.read()<<8|Wire.read())/4096.0;                               //Add the low and high byte to the acc_z variable.
  temperature = Wire.read()<<8|Wire.read();                               //Add the low and high byte to the temperature variable.
  Gyr_rawX = Wire.read()<<8|Wire.read();                              //Read high and low part of the angular data.
  Gyr_rawY = Wire.read()<<8|Wire.read();                              //Read high and low part of the angular data.
  Gyr_rawZ = Wire.read()<<8|Wire.read();                              //Read high and low part of the angular data.



      /*Now in order to obtain the gyro data in degrees/seconds we have to divide first
    the raw value by 32.8 because that's the value that the datasheet gives us for a 1000dps range*/
    /*---X---*/
    Gyr_rawX = (Gyr_rawX/32.8) - Gyro_raw_error_x; 
    /*---Y---*/
    Gyr_rawY = (Gyr_rawY/32.8) - Gyro_raw_error_y;
    /*---Z---*/
    Gyr_rawZ = (Gyr_rawZ/32.8) - Gyro_raw_error_z;   

       //0.000001066 = 0.0000611 * (3.142(PI) / 180degr) The Arduino sin function is in radians
   Gyr_rawY += Gyr_rawX * sin(Gyr_rawZ * 0.00012);               //If the IMU has yawed transfer the roll angle to the pitch angel
   Gyr_rawX -= Gyr_rawY * sin(Gyr_rawZ * 0.00012);               //If the IMU has yawed transfer the pitch angle to the roll angel

  
    /*Now we integrate the raw value in degrees per seconds in order to obtain the angle
    * If you multiply degrees/seconds by seconds you obtain degrees */
    /*---X---*/
    Gyro_angle_x = Gyr_rawX*elapsedTime;
    /*---Y---*/
    Gyro_angle_y = Gyr_rawY*elapsedTime;
    /*---Z---*/
    Gyro_angle_z = Gyr_rawZ*elapsedTime;

     /*Now in order to obtain the Acc angles we use euler formula with acceleration values
 after that we substract the error value found before*/  
 /*---X---*/
 Acc_angle_x = (atan((Acc_rawY)/sqrt(pow((Acc_rawX),2) + pow((Acc_rawZ),2)))*rad_to_deg) - Acc_angle_error_x;
 /*---Y---*/
 Acc_angle_y = (atan(-1*(Acc_rawX)/sqrt(pow((Acc_rawY),2) + pow((Acc_rawZ),2)))*rad_to_deg) - Acc_angle_error_y;    

 //////////////////////////////////////Total angle and filter/////////////////////////////////////
 
  if(set_gyro_angles){                                                 //If the IMU is already started
    /*---X axis angle---*/
   Total_angle_x = 0.98 *(Total_angle_x + Gyro_angle_x) + 0.02*Acc_angle_x;
   gyro_roll_input = Total_angle_x;
   /*---Y axis angle---*/
   Total_angle_y = 0.98 *(Total_angle_y + Gyro_angle_y) + 0.02*Acc_angle_y;
   gyro_pitch_input = Total_angle_y;
   /*---Z axis angle---*/
   Total_angle_z = 0.98 *(Total_angle_z + Gyro_angle_z) + 0.02*Acc_angle_z;
   gyro_yaw_input = Total_angle_z;
  }
  else{                                                                //At first start
    gyro_roll_input = Acc_angle_x;                                     //Set the gyro pitch angle equal to the accelerometer pitch angle 
    gyro_pitch_input = Acc_angle_y;                                       //Set the gyro roll angle equal to the accelerometer roll angle 
    set_gyro_angles = true;                                            //Set the IMU started flag
  }

  Serial.print(gyro_roll_input);
  Serial.print(" | ");
  Serial.print(gyro_pitch_input);
  Serial.print(" | ");
  Serial.println(gyro_yaw_input);
}
