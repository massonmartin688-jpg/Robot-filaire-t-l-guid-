#include <SPI.h>
#include <LoRa.h>

const int relayPin1 = 7;
const int relayPin2 = 8;
const int INC_PIN = 3;
const int UD_PIN = 4;
const int CS_PIN = 5;

bool relayState1 = false;
bool relayState2 = false;

void setup() {
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(INC_PIN, OUTPUT);
  pinMode(UD_PIN, OUTPUT);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  digitalWrite(relayPin1, LOW);
  digitalWrite(relayPin2, LOW);

  Serial.begin(9600);
  while (!Serial);

  if (!LoRa.begin(525E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Receiver");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String receivedText = "";
    while (LoRa.available()) {
      receivedText += (char)LoRa.read();
    }
    Serial.println("Message reçu: " + receivedText);

    if (receivedText.startsWith("RELAY1_")) {
      relayState1 = (receivedText == "RELAY1_ON");
      digitalWrite(relayPin1, relayState1 ? HIGH : LOW);
      Serial.println(relayState1 ? "Relais 1 activé" : "Relais 1 désactivé");
    } 
    else if (receivedText.startsWith("RELAY2_")) {
      relayState2 = (receivedText == "RELAY2_ON");
      digitalWrite(relayPin2, relayState2 ? HIGH : LOW);
      Serial.println(relayState2 ? "Relais 2 activé" : "Relais 2 désactivé");
    } 
    else if (receivedText.startsWith("POT:")) {
      int targetPos = receivedText.substring(4).toInt();
      setPotentiometer(targetPos);
    }
  }
  delay(50);
}

void setPotentiometer(int targetPos) {
  static int currentPos = 0;
  digitalWrite(CS_PIN, LOW);

  if (targetPos > currentPos) {
    digitalWrite(UD_PIN, HIGH);
    for (int i = currentPos; i < targetPos; i++) {
      digitalWrite(INC_PIN, LOW);
      delay(1);
      digitalWrite(INC_PIN, HIGH);
      delay(1);
    }
  } else if (targetPos < currentPos) {
    digitalWrite(UD_PIN, LOW);
    for (int i = currentPos; i > targetPos; i--) {
      digitalWrite(INC_PIN, LOW);
      delay(1);
      digitalWrite(INC_PIN, HIGH);
      delay(1);
    }
  }

  digitalWrite(CS_PIN, HIGH);
  currentPos = targetPos;
  Serial.println("Potentiomètre réglé à : " + String(targetPos));
}
