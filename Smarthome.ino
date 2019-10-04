#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
char auth[] = "NdB0gjJcfxBdMWRh-zaGRySYfmJFeFsS";
const char* ssid     = "HA NGOC LINH";
const char* password = "0982346770";
IPAddress local_IP(192, 168, 1, 50);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8); 
IPAddress secondaryDNS(8, 8, 4, 4);
void setup()
{
  Serial.begin(9600);
 if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Failed to configure");
  }
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 Blynk.config(auth);
  while (Blynk.connect(1000) == false) { 
  }  
}

void loop()
{
  Blynk.run();
  Blynk.virtualWrite(V7, 1);
  
}
