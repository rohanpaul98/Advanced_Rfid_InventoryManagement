#include <SPI.h>
#include <WiFi.h>
#include <MFRC522.h>

#include <BlynkSimpleEsp32.h>

using namespace std;

#include "time.h"
#define BLYNK_PRINT Serial
#define SS_PIN 5
#define RST_PIN 22
 int cnt=0;

 const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800;   //Replace with your GMT offset (seconds)
const int   daylightOffset_sec = 0;  //Replace with your daylight offset 

char auth[] ="AGR6OUin_Hnk7ScS-XR2GTEiU_O_e5n_"; //replace with your Auth code here
const char* ssid = "R"; // replace with Your SSID
const char* password = "12345678";  //replace with your wifi Password
String arr[7];


MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
SimpleTimer timer;

WidgetLCD lcd(V2);

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

int searchit(String id,int len){
  
  for(int i=0;i<len;i++){
    if(arr[i]== id){
      return i;
    }
  }
  return -1;
}

void iot_rfid(){
  // Look for new cards
   configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Blynk.virtualWrite(V1,cnt);
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
//  Serial.print("UID tag :");
  String content= "";
  byte letter;

  for (byte i = 0; i < mfrc522.uid.size; i++) 

  {
//     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
//     Serial.print(mfrc522.uid.uidByte[i], DEC);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], DEC));
  }
   
  String ref = content.substring(1);
  String temp;
  int d = searchit(ref,cnt);
  delay(1000);
  if(d!=-1){
    arr[d]="";
    cnt--;
     printLocalTime();  
     Serial.print("ITEM SERIAL NO. - ");
     Serial.println(d+1);
     Serial.print("COMPONENT ID :");
     Serial.println(content.substring(1));
     Serial.println("COMPONENT OUT");
     lcd.clear();
     lcd.print(1, 0, content.substring(1));
     lcd.print(0,1,"COMPONENT OUT");
     delay(100);
     
  }else{
    arr[cnt++] = ref;
   
     printLocalTime();  
      Serial.print("ITEM SERIAL NO. - ");
     Serial.println(cnt);
     Serial.print("COMPONENT ID :");
     Serial.println(content.substring(1));  
     Serial.println("COMPONENT IN");
    
     lcd.clear();
     lcd.print(1, 0, content.substring(1));
     lcd.print(0,1,"COMPONENT IN");
     
     
  }
   Blynk.virtualWrite(V1,cnt);
  Serial.println();
}

void setup() 

{

  Serial.begin(9600);   // Initiate a serial communication
  Blynk.begin(auth, ssid, password);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println();
  Serial.println("PLEASE PLACE THE CARD INFRONT OF THE READER TO SCAN");
  Serial.println();
  lcd.clear();
  Serial.println();
  timer.setInterval(1000L, iot_rfid);
}

void loop() 
{
  timer.run(); // Initiates SimpleTimer
  Blynk.run();
}
