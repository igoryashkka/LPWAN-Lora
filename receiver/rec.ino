#include <SPI.h>
#include <LoRa.h>

#define SS_PIN 10
#define RST_PIN 9
#define DIO0_PIN 2
#define SNR_EXCELLENT 15
#define RSSI_EXCELLENT -80
#define SNR_VERY_GOOD 10
#define RSSI_VERY_GOOD -90
#define SNR_GOOD 5
#define RSSI_GOOD -100
#define SNR_FAIR 0
#define RSSI_FAIR -110

String incomingMessage = "";
byte localAddress = 0x02;       
byte destinationMaster = 0x01; 

void setup() {
  Serial.begin(115200);
  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);
  
  Serial.println("Start LoRa init...");
  if (!LoRa.begin(433E6)) {             
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       
  }
  Serial.println("LoRa init succeeded.");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    onReceive(packetSize);
  }
}

void onReceive(int packetSize) {
  int recipient = LoRa.read();       
  byte sender = LoRa.read();         
  byte incomingLength = LoRa.read();  
  incomingMessage = "";
  
  while (LoRa.available()) {
    incomingMessage += (char)LoRa.read();
  }

  if (incomingLength != incomingMessage.length()) {
    Serial.println("Error: Message length does not match length.");
    return; 
  }

  adjustSettingsBySignalQuality();
  printReceivedMessage(sender, incomingLength);
}

void adjustSettingsBySignalQuality() {
  float currentSNR = LoRa.packetSnr();
  int currentRSSI = LoRa.packetRssi();

  if (currentSNR > SNR_EXCELLENT && currentRSSI > RSSI_EXCELLENT) {
    adjustLoraADR(7, 14, 500E3, 5);
  } else if (currentSNR > SNR_VERY_GOOD && currentRSSI > RSSI_VERY_GOOD) {
    adjustLoraADR(8, 17, 250E3, 6);
  } else if (currentSNR > SNR_GOOD && currentRSSI > RSSI_GOOD) {
    adjustLoraADR(9, 18, 125E3, 7);
  } else if (currentSNR > SNR_FAIR && currentRSSI > RSSI_FAIR) {
    adjustLoraADR(10, 20, 125E3, 8);
  } else {
    adjustLoraADR(12, 20, 125E3, 8);
  }
}

void adjustLoraADR(int sf, int txPower, long sbw, int codingRate) {
  LoRa.setSpreadingFactor(sf);
  LoRa.setTxPower(txPower);
  LoRa.setSignalBandwidth(sbw);
  LoRa.setCodingRate4(codingRate);
}

void printReceivedMessage(byte sender, byte incomingLength) {
  Serial.println();
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + incomingMessage);
  Serial.println();
}
