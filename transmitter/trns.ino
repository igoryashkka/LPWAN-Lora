#include "MQ135.h"
#include "DHT.h"
#include <SPI.h>
#include <LoRa.h>
long temperature = 0;
#define DHTPIN 2 
#define DHTTYPE DHT22  
#define ss 15
#define rst 16 
#define dio0 4
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
String Incoming = "";
String Message = "";            
byte LocalAddress = 0x01;         
byte Destination_ESP32_Slave_1 = 0x02;
byte Destination_ESP32_Slave_2 = 0x03;
unsigned long previousMillis_SendMSG = 0;
const long interval_SendMSG = 3000;
byte Slv = 0;
void sendMessage(String Outgoing, byte Destination) {
  LoRa.beginPacket();             
  LoRa.write(Destination);      
  LoRa.write(LocalAddress);       
  LoRa.write(Outgoing.length()); 
  LoRa.print(Outgoing);           
  LoRa.endPacket();             
}


void onReceive(int packetSize) {
  if (packetSize == 0) return; 

  
  int recipient = LoRa.read();        
  byte sender = LoRa.read();          
  byte incomingLength = LoRa.read();  

  while (LoRa.available()) {
    Incoming += (char)LoRa.read();
  }

  if (incomingLength != Incoming.length()) {
    Serial.println("error: message length does not match length");
    return; 
  }

  if (recipient != LocalAddress) {
    Serial.println("This message is not for me.");
    return;
  }
  Serial.println();
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + Incoming);
 
}

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);


  
  Serial.println(F("DHTxx test!"));

  dht.begin();

  //---------------------------------------- Settings and start Lora Ra-02.
  LoRa.setPins(ss, rst, dio0);

  Serial.println("Start LoRa init...");
  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 or 433 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }
  Serial.println("LoRa init succeeded.");

}
  
void loop() {

unsigned long currentMillis_SendMSG = millis();

  if (currentMillis_SendMSG - previousMillis_SendMSG >= interval_SendMSG) {
    previousMillis_SendMSG = currentMillis_SendMSG;
  float t = dht.readTemperature();


    
    MQ135 gasSensor = MQ135(A0);
    float air_quality = gasSensor.getPPM();
    Serial.print("Air Quality: ");  
    Serial.print(air_quality);
    Serial.println("  PPM");   
    Serial.println();


  
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
   if ( isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
    Slv++;
    if (Slv > 2) Slv = 1;

    Message = "Temp : " + String(t) + "C" + " PPM : " +  String(air_quality);
    if (Slv == 1) {
      Serial.println();
      Serial.print("Send message to ESP32 Slave " + String(Slv));
      Serial.println(" : " + Message);
      sendMessage(Message, Destination_ESP32_Slave_1);
    }

    if (Slv == 2) {
      Serial.println();
      Serial.print("Send message to ESP32 Slave " + String(Slv));
      Serial.println(" : " + Message);
      sendMessage(Message, Destination_ESP32_Slave_2);
    }
 
  
}
  onReceive(LoRa.parsePacket());

}
