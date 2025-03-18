         /////////////////////////////////////////////  
        //           RFID Desk Lamp                //
       //                                         //
      //          ---------------------          //
     //             (Arduino Nano)              //           
    //            by Kutluhan Aktar           // 
   //                                         //
  /////////////////////////////////////////////

// Supersede your old desk lamp with a new one controlled by an RFID tag or card and personalize it with an RGB color pattern to turn the lamp off.
// You can register a new UID with the code below by turning it into uncommented.
// By using three potentiometer, you can adjust RGB led color scheme.
// In this case, for turning the lamp off, you have to make all potentiometer values zero. But, if you want to change this pattern, you can change it in the UID() function.  
// ON :
// When the UID is accurate and controlLed is HIGH.
// OFF :
// Red Potentiometer Value = 0
// Green Potentiometer Value = 0
// Blue Potentiometer Value = 0
// Connections
// Arduino Nano :           
//                                MFRC522
// Pin 9  ----------------------- RST
// Pin 10 ----------------------- SDA
// Pin 11 ----------------------- MOSI
// Pin 12 ----------------------- MISO
// Pin 13 ----------------------- SCK
//                                Control Led
// Pin 7  -----------------------
//                                RGB
// Pin 3  -----------------------
// Pin 5  -----------------------
// Pin 6  -----------------------
//                                2-Way Relay
// Pin 8  -----------------------
//                                Potentiometer (Red)
// Pin A0 -----------------------
//                                Potentiometer (Green)
// Pin A1 -----------------------
//                                Potentiometer (Blue)
// Pin A2 -----------------------

#include <EEPROM.h>     // We are going to read and write PICC's UIDs from/to EEPROM
#include <SPI.h>        // RC522 Module uses SPI protocol
#include <MFRC522.h>  // Library for Mifare RC522 Devices

// Create MFRC522 instance.
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Define MFRC5222 module key input.
MFRC522::MIFARE_Key key;

// Define the process controller and readCard byte.
int successUID;
byte readCard[4];

// Define openUID and lastRead strings.
String openUID;
String lastRead;

// Examine whether the UID is true or not.
boolean UIDisTrue = false;
int red;
int green;
int blue;

// Define RGB led control led pins.
#define redPin 3
#define greenPin 5
#define bluePin 6
#define controlLed 7
#define relay 8

// Define potentiometer pins.
#define pot_r A0
#define pot_g A1
#define pot_b A2

void setup()
{
  //Protocol Configuration
  Serial.begin(9600);  // Initialize serial communications with PC
  SPI.begin();           // MFRC522 Hardware uses SPI protocol
  mfrc522.PCD_Init();    // Initialize MFRC522 Hardware

  // If you do not register a new UID to EEPROM yet, reload the code after turning these lines into uncommented.
  
  // Save the new card or key tag UID to EEPROM. But do not forget it only has 1KB memory.
  // Serial.print("Approximate the new card or key tag to scan and register new UID.");
  // do{
  // Wait for the new card reading process.
  // successUID = registerCardUID();
  // }while(!successUID);

  // Get the open UID from EEPROM.
  getUIDfromEEPROM();
  Serial.print("UID is received from EEPROM :\n----------------------------------\n");
  Serial.print(openUID);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(controlLed, OUTPUT);
  pinMode(relay, OUTPUT);
}

void loop(){
// Turn relay and controlLed off.
digitalWrite(relay, HIGH);
digitalWrite(controlLed, LOW);
// Get potentiometer data from 0 to 255.
readPotentiometer();
// Adjust RGB led colors in regard to potentiometer values.
adjustColor(red, green, blue);
// Open the desk lamp if the UID is accurate.
UID();

}

int UID(){
  // Get the last UID from MFRC522.
  if ( ! mfrc522.PICC_IsNewCardPresent()){
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()){
    return;
  }
    for(int i=0;i<mfrc522.uid.size;i++){
    lastRead += mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ";
    lastRead += String(mfrc522.uid.uidByte[i], HEX);
    }
    // Arrange lastRead for comparing.
    lastRead.trim();
    lastRead.toUpperCase();

  
  // Activate relay and controlLed if the UID is accurate.
  if(lastRead == openUID){
    UIDisTrue = true;
    while(UIDisTrue == true){
      // Get potentiometer data from 0 to 255.
      readPotentiometer();
      // Adjust RGB led colors in regard to potentiometer values.
      adjustColor(red, green, blue);
      // Turn relay and controlLed on.
      digitalWrite(controlLed, HIGH);
      digitalWrite(relay, LOW);
      // Turn relay and controlLed off.
      if(red == 0 && green == 0 && blue == 0){
        UIDisTrue = false;
        // Dump lastRead.
        lastRead = "";
        }
      }
    }
}

int readPotentiometer(){
  red = map(analogRead(pot_r), 0, 1023, 0, 255);
  green = map(analogRead(pot_g), 0, 1023, 0, 255);
  blue = map(analogRead(pot_b), 0, 1023, 0, 255);
  }

void adjustColor(int r, int g, int b){
 r = 255 - r;
 g = 255 - g;
 b = 255 - b;
 analogWrite(redPin, r);
 analogWrite(greenPin, g);
 analogWrite(bluePin, b);
}

int registerCardUID() {
  // Detect the new card UID. 
  if ( ! mfrc522.PICC_IsNewCardPresent()) { 
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return 0;
  }

  // Display the new UID.
  Serial.print("\n----------------------------------\nNew Card or Key Tag UID : ");
  for (int i = 0; i < mfrc522.uid.size; i++) {  //
    readCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(readCard[i], HEX);
  }
  Serial.print("\n----------------------------------\n");
  
  // Save the new UID to EEPROM. 
  for ( int i = 0; i < mfrc522.uid.size; i++ ){
   EEPROM.write(i, readCard[i] );
  }
  Serial.print("UID is saved successfully to EEPROM.\nIf you want to save another card, use i+4(...) instead i.");
  
  // If the card reading process is successful, return 1 and end the reading process.
  mfrc522.PICC_HaltA();
  return 1;
}

int getUIDfromEEPROM(){
  // Get the open UID from EEPROM.
  for(int i=0;i<4;i++){
    openUID += EEPROM.read(i) < 0x10 ? " 0" : " ";
    openUID += String(EEPROM.read(i), HEX);
    }
  // Arrange openUID for comparing.
  openUID.trim();
  openUID.toUpperCase();
  }
