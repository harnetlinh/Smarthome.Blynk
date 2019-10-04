#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>
#include <DHT.h>
#define DHTPIN 2  
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);

BlynkTimer timer;
void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Serial.println(h);
  Serial.println(t);
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);
}

//-----------------------------------------
// Để lấy auth key, vào email đăng ký hoặc.
SoftwareSerial mySerial(3, 1); //(Rx - Tx)
char auth[] = "mR8XEqO83un4hP_qyZiiSKKcnaZ7kwpA";
// Tên và mật khẩu wifi mà esp sẽ kết nối.
const char* ssid     = "IOT";
const char* password = "fgw@2019";
char msg;
char call;
int CommandValue;
IPAddress local_IP(10, 22, 60, 56);
// Set gateway IP, đây chính là địa chỉ của router
IPAddress gateway(10, 22, 60, 1);
// Set địa chỉ subnet mask
IPAddress subnet(255, 255, 254, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //đây là dns của google, bạn ko //cần thay đổi mục này nếu ko hiểu rõ
IPAddress secondaryDNS(8, 8, 4, 4);
void setup()
{
  dht.begin(); 
  mySerial.begin(9600);   // Setting the baud rate of GSM Module  
  Serial.begin(9600);    // Setting the baud rate of Serial Monitor (Arduino)
  Serial.println("GSM SIM900A BEGIN");
  Serial.println("Enter character for control option:");
  Serial.println("h : to disconnect a call");
  Serial.println("i : to receive a call");
  Serial.println("s : to send message");
  Serial.println("c : to make a call");  
  Serial.println("e : to redial");
  Serial.println();
  delay(100);
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
   
  timer.setInterval(1000L, sendSensor);
}



void SendMessage()
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+84869940695\"\r"); // Replace x with mobile number
  delay(1000);
  mySerial.println("SOS");// The SMS text you want to send
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(1000);
}

void ReceiveMessage()
{
  mySerial.println("AT+CNMI=2,2,0,0,0"); // AT Command to recieve a live SMS
  delay(1000);
  if (mySerial.available()>0)
  {
    msg=mySerial.read();
    Serial.print(msg);
  }
}

void MakeCall()
{
  mySerial.println("ATD+84869940695;"); // ATDxxxxxxxxxx; -- watch out here for semicolon at the end!!
  Serial.println("Calling  "); // print response over serial port
  delay(1000);
}


void HangupCall()
{
  mySerial.println("ATH");
  Serial.println("Hangup Call");
  delay(1000);
}

void ReceiveCall()
{
  mySerial.println("ATA");
  delay(1000);
  {
    call=mySerial.read();
    Serial.print(call);
  }
}

void RedialCall()
{
  mySerial.println("ATDL");
  Serial.println("Redialing");
  delay(1000);
}

BLYNK_WRITE(V9) 
{
  CommandValue = param.asInt();
  if(CommandValue == 1)
  {
    SendMessage();
  }
  if(CommandValue == 2)
  {
    MakeCall();
  }
}
void loop()
{
  Blynk.run();
  timer.run();
}
