#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include <Servo.h> 


#define SS_PIN 15//D8
#define RST_PIN 2//D4
#define BTN_PIN 16//D0- Button lock door
#define SLN_PIN 4//D2 - Button
//#define SDA_PIN 5//D1 
#define DOOR 
Servo myservo;
MFRC522 mfrc522(SS_PIN, RST_PIN);
unsigned long uidDec, uidDecTemp;
int ARRAYindexUIDcard;
int EEPROMstartAddr;
long adminID = 1122539531;
bool beginCard = 0;
bool addCard = 1;
bool skipCard = 0;
int LockSwitch;
unsigned long CardUIDeEPROMread[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
int DoorPin = 0;//D3;

char auth[] = "Rx1CMAWweST40gFu_8_j0qtcbeXzYQ_K";
const char* ssid = "IOT";
const char* password = "fgw@2019";
IPAddress local_IP(10, 22, 60, 54);
// Set gateway IP, đây chính là địa chỉ của router
IPAddress gateway(10, 22, 60, 1);
// Set địa chỉ subnet mask
IPAddress subnet(255, 255, 254, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //đây là dns của google, bạn ko //cần thay đổi mục này nếu ko hiểu rõ
IPAddress secondaryDNS(8, 8, 4, 4);
WidgetLCD lcd(V0);

void setup() {
  Serial.begin(9600);
  pinMode(SLN_PIN, OUTPUT); digitalWrite(SLN_PIN, LOW);
  pinMode(BTN_PIN, INPUT_PULLUP);
  myservo.attach(DoorPin);


  SPI.begin();
  mfrc522.PCD_Init();
 if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Failed to configure");
  }
  // Kết nối mạng Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
//------------------------------------------------------------------
 Blynk.config(auth);
  while (Blynk.connect(1000) == false) { 
  }  
  
  lcd.clear(); //Use it to clear the LCD Widget
  EEPROM.begin(512);
  DisplayWAiT_CARD();
  EEPROMreadUIDcard();

  //digitalWrite(PiezoPin, HIGH), delay(100), digitalWrite(PiezoPin, LOW);
  myservo.write(0);
}

void loop() {

  digitalWrite(SLN_PIN, LOW);

  if (digitalRead(BTN_PIN) == LOW) {
    digitalWrite(SLN_PIN, HIGH); //unlock
    lcd.print(0, 0, " BUTTON UNLOCK ");
    lcd.print(0, 1, "   DOOR OPEN   ");
    
    //digitalWrite(PiezoPin, HIGH), delay(200), digitalWrite(PiezoPin, LOW);
    myservo.write(90);
    delay(2000);
   // lcd_x.clear();
    DisplayWAiT_CARD();
  }

  if (beginCard == 0) {
    if ( ! mfrc522.PICC_IsNewCardPresent()) {  //Look for new cards.
      Blynk.run();
      return;
    }

    if ( ! mfrc522.PICC_ReadCardSerial()) {  //Select one of the cards.
      Blynk.run();
      return;
    }
  }

  //Read "UID".
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uidDecTemp = mfrc522.uid.uidByte[i];
    uidDec = uidDec * 256 + uidDecTemp;
  }

  if (beginCard == 1 || LockSwitch > 0)EEPROMwriteUIDcard();  //uidDec == adminID

  if (LockSwitch == 0) {
    //CardUIDeEPROMread.
    for (ARRAYindexUIDcard = 0; ARRAYindexUIDcard <= 9; ARRAYindexUIDcard++) {
      if (CardUIDeEPROMread[ARRAYindexUIDcard] > 0) {
        if (CardUIDeEPROMread[ARRAYindexUIDcard] == uidDec) {
          
          lcd.print(0, 0, "CARD ACCESS OPEN");
          lcd.print(3, 1, uidDec);
          
          digitalWrite(SLN_PIN, HIGH); //unlock
          //digitalWrite(PiezoPin, HIGH), delay(200), digitalWrite(PiezoPin, LOW);
          myservo.write(90);
          delay(2000);
      //    lcd_x.clear();
          break;
        }
      }
    }

    if (ARRAYindexUIDcard == 10) {
      lcd.print(0, 0, " Card not Found ");
      lcd.print(0, 1, "                ");
      lcd.print(0, 1, "ID : ");
      lcd.print(5, 1, uidDec);
      //for (int i = 0; i <= 2; i++)delay(100), digitalWrite(PiezoPin, HIGH), delay(100), digitalWrite(PiezoPin, LOW);
      myservo.write(0);
      digitalWrite(SLN_PIN, LOW);  //lock();
      delay(2000);
    }

    ARRAYindexUIDcard = 0;
    DisplayWAiT_CARD();
  }

  Blynk.run();
}

BLYNK_WRITE(V1) {
  int a = param.asInt();
  if (a == 1) beginCard = 1; else beginCard = 0;
}

BLYNK_WRITE(V2) {
  int a = param.asInt();
  if (a == 1) {
    skipCard = 1;
    if (EEPROMstartAddr / 5 < 10) EEPROMwriteUIDcard();
  } else {
    skipCard = 0;
  }
}

BLYNK_WRITE(V3) {
  int a = param.asInt();
  if (a == 1) {
    digitalWrite(SLN_PIN, HIGH); //unlock
    lcd.print(0, 0, " APP UNLOCK OK ");
    lcd.print(0, 1, "   DOOR OPEN   ");
    //digitalWrite(PiezoPin, HIGH), delay(200), digitalWrite(PiezoPin, LOW);
    myservo.write(90);
    delay(2000);
    //lcd_x.clear();
    DisplayWAiT_CARD();
  } 
}

void EEPROMwriteUIDcard() {

  if (LockSwitch == 0) {
    lcd.print(0, 0, " START REC CARD ");
    lcd.print(0, 1, "PLEASE TAG CARDS");
    delay(500);
  }

  if (LockSwitch > 0) {
    if (skipCard == 1) {  //uidDec == adminID
      lcd.print(0, 0, "   SKIP RECORD   ");
      lcd.print(0, 1, "                ");
      lcd.print(0, 1, "   label : ");
      lcd.print(11, 1, EEPROMstartAddr / 5);
      EEPROMstartAddr += 5;
      skipCard = 0;
    } else {
      Serial.println("writeCard");
      EEPROM.write(EEPROMstartAddr, uidDec & 0xFF);
      EEPROM.write(EEPROMstartAddr + 1, (uidDec & 0xFF00) >> 8);
      EEPROM.write(EEPROMstartAddr + 2, (uidDec & 0xFF0000) >> 16);
      EEPROM.write(EEPROMstartAddr + 3, (uidDec & 0xFF000000) >> 24);
      EEPROM.commit();
      delay(10);
      lcd.print(0, 1, "                ");
      lcd.print(0, 0, "RECORD OK! IN   ");
      lcd.print(0, 1, "MEMORY : ");
      lcd.print(9, 1, EEPROMstartAddr / 5);
      EEPROMstartAddr += 5;
      delay(500);
    }
  }

  LockSwitch++;

  if (EEPROMstartAddr / 5 == 10) {
    lcd.clear();
    lcd.print(0, 0, "RECORD FINISH");
    delay(2000);
    EEPROMstartAddr = 0;
    uidDec = 0;
    ARRAYindexUIDcard = 0;
    EEPROMreadUIDcard();
  }
}

void EEPROMreadUIDcard() {
  for (int i = 0; i <= 9; i++) {
    byte val = EEPROM.read(EEPROMstartAddr + 3);
    CardUIDeEPROMread[ARRAYindexUIDcard] = (CardUIDeEPROMread[ARRAYindexUIDcard] << 8) | val;
    val = EEPROM.read(EEPROMstartAddr + 2);
    CardUIDeEPROMread[ARRAYindexUIDcard] = (CardUIDeEPROMread[ARRAYindexUIDcard] << 8) | val;
    val = EEPROM.read(EEPROMstartAddr + 1);
    CardUIDeEPROMread[ARRAYindexUIDcard] = (CardUIDeEPROMread[ARRAYindexUIDcard] << 8) | val;
    val = EEPROM.read(EEPROMstartAddr);
    CardUIDeEPROMread[ARRAYindexUIDcard] = (CardUIDeEPROMread[ARRAYindexUIDcard] << 8) | val;

    ARRAYindexUIDcard++;
    EEPROMstartAddr += 5;
  }

  ARRAYindexUIDcard = 0;
  EEPROMstartAddr = 0;
  uidDec = 0;
  LockSwitch = 0;
  DisplayWAiT_CARD();
}

void DisplayWAiT_CARD() {
  lcd.print(0, 0, "   ATTACH THE   ");
  lcd.print(0, 1, "      CARD      ");

  myservo.write(0);
}
BLYNK_WRITE(V16)  // Reset
{
if (param.asInt()==1) {
delay(100);
ESP.restart();
  } 
}
BLYNK_WRITE(V7) // This function gets called everytime the Virtual Button on V0 changes state 
{   
  int value = param.asInt(); // Get value of button as integer
  if ( value == 1 ) {  // This checks the value of the variable and if equals 1, does something like...
    myservo.write(90);  // ... move the servo to position based on another variable
    delay(2000);
  }
  else
    myservo.write(0);
// Place servo control code here based on value data.

}
