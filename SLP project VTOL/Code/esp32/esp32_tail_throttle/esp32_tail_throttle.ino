#include <ESP32Servo.h>
#include "sbus.h"

bfs::SbusRx sbus_rx(&Serial2, 16, 17, true);
bfs::SbusData data;

Servo tail_motor;
const int tailpin = 18;

unsigned long lastSbusTime = 0;
unsigned long lastPrintTime = 0;

void setup() {
  Serial.begin(115200);

  tail_motor.attach(tailpin, 1000, 2000);
  tail_motor.writeMicroseconds(1000);

  Serial.println("Waiting 3s for ESC to arm...");
  delay(3000);

  sbus_rx.Begin();
  Serial.println("SBUS started. Waiting for signal...");
}

void loop() {
  if (sbus_rx.Read()) {
    data = sbus_rx.data();
    lastSbusTime = millis();

    int rawCh2 = data.ch[2];
    int throttle = map(rawCh2, 174, 1810, 1000, 2000);
    throttle = constrain(throttle, 1000, 2000);

    tail_motor.writeMicroseconds(throttle);

    if (millis() - lastPrintTime >= 100) {
      lastPrintTime = millis();
      Serial.print("ch[2] raw: ");
      Serial.print(rawCh2);
      Serial.print(" | mapped throttle: ");
      Serial.println(throttle);
    }
  }

  // Failsafe if SBUS signal lost
  if (millis() - lastSbusTime > 500) {
    tail_motor.writeMicroseconds(1000);
    if (millis() - lastPrintTime >= 500) {
      lastPrintTime = millis();
      Serial.println("SBUS signal lost! Motor stopped.");
    }
  }
}