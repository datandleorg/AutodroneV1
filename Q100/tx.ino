#include <SPI.h> //Call SPI library so you can communicate with the nRF24L01+
#include <nRF24L01.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/
#include <RF24.h> //nRF2401 libarary found at https://github.com/tmrh20/RF24/
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define YELLOW 0xFFE0

const int pinCE = 7; //This pin is used to set the nRF24 to standby (0) or active mode (1)
const int pinCSN = 8; //This pin is used to tell the nRF24 whether the SPI communication is a command or message to send out

RF24 wirelessSPI(pinCE, pinCSN); // Create your nRF24 object or wireless SPI connection
const uint64_t pAddress = 0xB00B1E5000LL; // Radio pipe addresses for the 2 nodes to communicate.

int VRy1 = A3;
int SW1 = 3;

struct RadioPacket {
 uint32_t y1;
 uint32_t b1;
};

struct RadioAckPacket {
  uint8_t f_status;
};

RadioPacket _radioData;
RadioAckPacket _radioAckData;
void setup() 
{
  Serial.begin(115200); //start serial to communicate process

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  delay(2000);
  display.clearDisplay();
  
  if (!wirelessSPI.begin()) {
    display.setCursor(30,8);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.println("Radio offline");
    display.display();
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }
  
  wirelessSPI.setAutoAck(1); // Ensure autoACK is enabled so rec sends ack packet to let you know it got the transmit packet payload
  wirelessSPI.enableAckPayload(); // Allow optional ack payloads
  wirelessSPI.setPALevel(RF24_PA_MAX);
  wirelessSPI.openWritingPipe(pAddress); // pipe address that we will communicate over, must be the same for each nRF24 module
  wirelessSPI.stopListening(); //transmitter so stop listening for data

  pinMode(VRy1, INPUT);
  pinMode(SW1, INPUT_PULLUP); 

}

void loop() {
   delay(100); 
   sendData();
}

void updateESC(){
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.drawRoundRect(0, 0, 25, 7, 1, WHITE);
    display.fillRoundRect(0, 0, 8 , 7, 1, WHITE);

    display.setCursor(45,0);
    display.println("T:" + String(_radioData.y1));

    display.setCursor(0,8);
    display.println("FS:" + String(_radioAckData.f_status));
}

void updateFlightStatus(){
    display.setCursor(30,8);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.println("Drone offline");
}

void sendData(){
    Serial.println("Sending packet"); 
    Serial.println("");

  _radioData.y1 = map(analogRead(VRy1), 0, 1023, 0, 255);
  _radioData.b1 = digitalRead(SW1);

    
   if (wirelessSPI.write( &_radioData, sizeof(_radioData) )){ 
       Serial.println("writtten");

        if (wirelessSPI.isAckPayloadAvailable()) {
                wirelessSPI.read(&_radioAckData, sizeof(_radioAckData));
                
                Serial.println(" | f_status: ");
                Serial.println(_radioAckData.f_status);
                
                display.clearDisplay();
                updateESC();
                display.display();
         }
   }else{
      Serial.println("packet delivery failed"); 
                display.clearDisplay();
                updateFlightStatus();
                display.display();
   }  
}
