// Include thư viện sử dụng.
// Các bạn có thể search google để down các thư viện này
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

//-----------------------------------------
// Để lấy auth key, vào email đăng ký hoặc.
char auth[] = "X1cOAG6Q5230miHok9Wa-hCa6Byqq_bv";
// Tên và mật khẩu wifi mà esp sẽ kết nối.
const char* ssid     = "IOT";
const char* password = "fgw@2019";
//------------------------------------------
// Set Static IP cho ESP
// Để biết được các thông số này thì cách dễ nhất đó là kết nối wifi bằng điện thoại sau đó vào mục thông tin
//IPAddress là địa chỉ IP của ESP, bạn nên cẩn thận, tránh việc sử dụng trùng IP khác có trong mạng
IPAddress local_IP(10, 22, 60, 60);
// Set gateway IP, đây chính là địa chỉ của router
IPAddress gateway(10, 22, 60, 1);
// Set địa chỉ subnet mask
IPAddress subnet(255, 255, 254, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //đây là dns của google, bạn ko //cần thay đổi mục này nếu ko hiểu rõ
IPAddress secondaryDNS(8, 8, 4, 4); //đây là dns của google, bạn ko //cần thay đổi mục này nếu ko hiểu rõ
//------------------------------------------------------------------
BlynkTimer timer;


BLYNK_READ(V5) //Blynk app has something on V5
{
  int sensorData = analogRead(A0); //reading the sensor on A0
  Blynk.virtualWrite(V5, sensorData); //sending to Blynk
}


void setup()
{
  Serial.begin(9600);
//------------------------------------------------------------------
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

  //Bạn có thể sử dụng cách khác để kết nối tới Blynk
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 80);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8080);
}

void loop()
{
  Blynk.run();
}
