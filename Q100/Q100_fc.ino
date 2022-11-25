#define OUTPUT_PIN_1 3  // PWM/Signal output pin // fl
#define OUTPUT_PIN_2 4  // PWM/Signal output pin
#define OUTPUT_PIN_3 5  // PWM/Signal output pin
#define OUTPUT_PIN_4 6  // PWM/Signal output pin
float frequency;        // Frequency in Herz
boolean m_fr = true; // 4
boolean m_fl = true; // 3
boolean m_br = true; // 5
boolean m_bl = true; // 6

void setup() {
  pinMode(OUTPUT_PIN_1, OUTPUT);
  pinMode(OUTPUT_PIN_2, OUTPUT);
  pinMode(OUTPUT_PIN_3, OUTPUT);
  pinMode(OUTPUT_PIN_4, OUTPUT);
  frequency = 490;
}

void loop() {
  writeMotors(30,30,30,30);
  delay(10);
}


void writeMotors(float dutyCycle_1, float dutyCycle_2, float dutyCycle_3, float dutyCycle_4){
  // Calculate the period and the amount of time the output is on for (HIGH) and 
  // off for (LOW).
  double period_1 = 1000000 / frequency;
  double offFor_1 = period_1 - (period_1 * (dutyCycle_1/100));
  double onFor_1 = period_1 - offFor_1;
  double period_2 = 1000000 / frequency;
  double offFor_2 = period_2 - (period_2 * (dutyCycle_2/100));
  double onFor_2 = period_2 - offFor_2;
  double period_3 = 1000000 / frequency;
  double offFor_3 = period_3 - (period_3 * (dutyCycle_3/100));
  double onFor_3 = period_3 - offFor_3;
  double period_4 = 1000000 / frequency;
  double offFor_4 = period_4 - (period_4 * (dutyCycle_4/100));
  double onFor_4 = period_4 - offFor_4;

  if(m_fr){
    digitalWrite(OUTPUT_PIN_1, HIGH);
    delayMicroseconds((long)onFor_1);
    
    digitalWrite(OUTPUT_PIN_1, LOW);
    delayMicroseconds((long)offFor_1); 
  }

  if(m_fl){
    digitalWrite(OUTPUT_PIN_2, HIGH);
    delayMicroseconds((long)onFor_2);
    
    digitalWrite(OUTPUT_PIN_2, LOW);
    delayMicroseconds((long)offFor_2);
  }

  if(m_br){
    digitalWrite(OUTPUT_PIN_3, HIGH);
    delayMicroseconds((long)onFor_3);
    
    digitalWrite(OUTPUT_PIN_3, LOW);
    delayMicroseconds((long)offFor_3);
  }
  

  if(m_bl){
    digitalWrite(OUTPUT_PIN_4, HIGH);
    delayMicroseconds((long)onFor_4);
    
    digitalWrite(OUTPUT_PIN_4, LOW);
    delayMicroseconds((long)offFor_4);
  }  
}
