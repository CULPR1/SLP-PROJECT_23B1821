#include "sbus.h"

#define LED_PIN 13

bfs::SbusRx sbus_rx(&Serial);
bfs::SbusData data;

unsigned long lastSbusTime = 0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  sbus_rx.Begin();
}

void loop() {
  if (sbus_rx.Read()) {
    data = sbus_rx.data();
    lastSbusTime = millis();
  }

  if (millis() - lastSbusTime > 100) {
    digitalWrite(LED_PIN, LOW);
    return;
  }

  if (data.ch[2] > 900) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}