#include <SPI.h>
#include <LoRa.h>

#define ss 10
#define rst 9
#define dio0 2
String incomingMessage = "";
byte localAddress = 0x02;

void onReceive(int packetSize) {
  if (packetSize == 0) return;

  int recipient = LoRa.read();
  byte sender = LoRa.read();
  byte incomingLength = LoRa.read();
  incomingMessage = "";
  while (LoRa.available()) {
    incomingMessage += (char)LoRa.read();
  }

  if (recipient != localAddress) {
    return;
  }

  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Message: " + incomingMessage);

  checkAndAdjustDataRate();
}

void setup() {
  Serial.begin(115200);
  LoRa.setPins(ss, rst, dio0);
  Serial.println("Start LoRa init...");
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);
  }
  Serial.println("LoRa init succeeded.");
}

void loop() {
  onReceive(LoRa.parsePacket());
}

void adjustDataRate(int snr, int rssi) {
  if (snr > 15 && rssi > -80) {
    // Excellent conditions
    setSpreadingFactor(7);
    setTxPower(14);
    setSignalBandwidth(500E3);
    setCodingRate4(5);
  } else if (snr > 10 && rssi > -90) {
    // Very good conditions
    setSpreadingFactor(8);
    setTxPower(17);
    setSignalBandwidth(250E3);
    setCodingRate4(6);
  } else if (snr > 5 && rssi > -100) {
    // Good conditions
    setSpreadingFactor(9);
    setTxPower(18);
    setSignalBandwidth(125E3);
    setCodingRate4(7);
  } else if (snr > 0 && rssi > -110) {
    // Fair conditions
    setSpreadingFactor(10);
    setTxPower(20);
    setSignalBandwidth(125E3);
    setCodingRate4(8);
  } else {
    // Poor conditions
    setSpreadingFactor(12);
    setTxPower(20);
    setSignalBandwidth(125E3);
    setCodingRate4(8);
  }
}



void checkAndAdjustDataRate() {
  int currentSNR = packetSnr();
  int currentRSSI = packetRssi();
  currentRSSI = 1;
  currentSNR = 1;
  adjustDataRate(currentSNR, currentRSSI);
}

float packetSnr() {
  return LoRa.packetSnr();
}

int packetRssi() {
  return LoRa.packetRssi();
}

void setTxPower(int level) {
  LoRa.setTxPower(level);
}

void setSpreadingFactor(int sf) {
  LoRa.setSpreadingFactor(sf);
}

void setSignalBandwidth(long sbw) {
  LoRa.setSignalBandwidth(sbw);
}

void setCodingRate4(int denominator) {
  LoRa.setCodingRate4(denominator);
}
