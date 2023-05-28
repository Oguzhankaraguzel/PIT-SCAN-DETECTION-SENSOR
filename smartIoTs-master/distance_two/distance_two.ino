#include <SPI.h>
#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti WiFiMulti;

// update for your WiFi
char* ssid = "TurkTelekom_T9C3B";
char* password = "mfJU4Pxk";

// change to your server
IPAddress server(52,174,29,83);

WiFiClient client;
WiFiClient dclient;
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char outBuf[128];
char outCount;


int trigPin = 13; /* Sensorun trig pini Arduinonun 6 numaralı ayağına bağlandı */
int echoPin = 12;  /* Sensorun echo pini Arduinonun 7 numaralı ayağına bağlandı */

long sure;
long uzaklik;

long distance;

long readUltrasonicSensor() {

  digitalWrite(trigPin, LOW); /* sensör pasif hale getirildi */
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH); /* Sensore ses dalgasının üretmesi için emir verildi */
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);  /* Yeni dalgaların üretilmemesi için trig pini LOW konumuna getirildi */ 
  sure = pulseIn(echoPin, HIGH); /* ses dalgasının geri dönmesi için geçen sure ölçülüyor */
  uzaklik= sure /29.1/2; /* ölçülen sure uzaklığa çevriliyor */            
  if(uzaklik > 200)
    uzaklik = 200;
  return uzaklik;
}


byte doEmptyFTP()
{
  if (client.connect(server,21)) {
    Serial.println(F("Command connected"));
  } 
  else {
     Serial.println(F("Command connection failed"));
    return 0;
  }
  client.println(F("USER ceyhuncoskun"));
  client.println(F("PASS ceyhunZENO19"));
  if(!eRcv()) return 0;
  client.println(F("CWD TRASH-2"));
  if(!eRcv()) return 0;
  client.println(F("MKD false-1"));
  if(!eRcv()) return 0;
  client.println(F("QUIT"));
  client.stop();
  Serial.println(F("Command disconnected"));
  return 1;
}


byte doFullFTP()
{
  if (client.connect(server,21)) {
    Serial.println(F("Command connected"));
  } 
  else {
     Serial.println(F("Command connection failed"));
    return 0;
  }
  client.println(F("USER ceyhuncoskun"));
  client.println(F("PASS ceyhunZENO19"));
  if(!eRcv()) return 0;
  client.println(F("CWD TRASH-2"));
  if(!eRcv()) return 0;
  client.println(F("MKD true-1"));
  if(!eRcv()) return 0;
  client.println(F("QUIT"));
  client.stop();
  Serial.println(F("Command disconnected"));
  return 1;
}

byte eRcv()
{
  byte respCode;
  byte thisByte;
  //while(!client.available()) delay(1);
  delay(1000);
  respCode = client.peek();
  outCount = 0;
  while(client.available())
  {  
    thisByte = client.read();    
    Serial.write(thisByte);
    if(outCount < 127)
    {
      outBuf[outCount] = thisByte;
      outCount++;      
      outBuf[outCount] = 0;
    }
  }
  if(respCode >= '4')
  {
    efail();
    return 0;  
  }
  return 1;
}

void efail()
{
  byte thisByte = 0;
  client.println(F("QUIT"));
  while(!client.available()) delay(1);
  while(client.available())
  {  
    thisByte = client.read();    
    Serial.write(thisByte);
  }
  client.stop();
  Serial.println(F("Command disconnected"));
  Serial.println(F("SD closed"));
}

void setup() {
  pinMode(trigPin, OUTPUT); /* trig pini çıkış olarak ayarlandı */
  pinMode(echoPin,INPUT); /* echo pini giriş olarak ayarlandı */
  Serial.begin(115200); /* Seri haberlesme baslatildi */
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);
  while (WiFiMulti.run() != WL_CONNECTED ) {
   delay(500);
    Serial.print(".");
 }
   Serial.println("wifi connect");
}

void loop() {
  distance = readUltrasonicSensor();
  Serial.println();
  Serial.println("distance ");
  Serial.print(distance);
  Serial.print(" cm");
  Serial.println();
  delay(500);
  if (distance <= 10) {
   Serial.print(distance);
    Serial.println("Object detected.\r\n");
    doFullFTP();
   }else{
    Serial.println("Container is empty.");
    doEmptyFTP();
    }
}
