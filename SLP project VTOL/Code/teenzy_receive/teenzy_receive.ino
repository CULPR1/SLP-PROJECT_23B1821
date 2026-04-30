#include "sbus.h"

/* SBUS object on Serial2 (Pin 7) */
bfs::SbusRx sbus_rx(&Serial2);
bfs::SbusData data;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 5000) {} // Wait for monitor
  
  /* Start SBUS communication */
  sbus_rx.Begin();
  Serial.println("SBUS Listener Active on Pin 7");
}

void loop() {
  /* Read() returns true only when a valid packet is parsed */
  if (sbus_rx.Read()) {
    data = sbus_rx.data();

    for (int i = 0; i < 8; i++) {
      Serial.print("Ch");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(data.ch[i]); // Values range 172 - 1811
      Serial.print("\t");
    }
    Serial.println();
  }
}