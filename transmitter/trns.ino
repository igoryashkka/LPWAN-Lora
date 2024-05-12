#include <SPI.h>
#include <LoRa.h>


#define SS_PIN 10
#define RST_PIN 9
#define DIO0_PIN 2


#define LORA_FREQ 433E6 


#define LOCAL_ADDRESS 0x01
#define DESTINATION_ESP32_SLAVE 0x02


String baseMessage = "TEXTTEXTTEXT";
unsigned long messageCounter = 0;


unsigned long previousMillis = 0;
#define SEND_INTERVAL 5000 // Time interval in milliseconds

void setup() {
  Serial.begin(115200);
  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);

  Serial.println("Initializing LoRa...");
  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("LoRa initialization failed. Check your connections.");
    while (true);
  }
  Serial.println("LoRa initialized successfully.");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= SEND_INTERVAL) {
    previousMillis = currentMillis;
    sendMessage();
  }
}

void sendMessage() {
  String outgoingMessage = baseMessage + " " + String(++messageCounter);
  Serial.print("Sending message to ESP32 Slave: ");
  Serial.println(outgoingMessage);

  LoRa.beginPacket();
  LoRa.write(DESTINATION_ESP32_SLAVE);
  LoRa.write(LOCAL_ADDRESS);
  LoRa.write(outgoingMessage.length());
  LoRa.print(outgoingMessage);
  LoRa.endPacket();
}
