/*
  SBUS Reader using Bolder Flight Systems SBUS library
  ESP32 WROOM — RXD2 = GPIO16
  
  Install: Library Manager → search "SBUS" → "Bolder Flight Systems SBUS"
  OR: https://github.com/bolderflight/sbus
*/

#include "sbus.h"

// Bolder Flight SBUS object — UART2, GPIO16=RX, GPIO17=TX, inverted=true
bfs::SbusRx sbus(&Serial2, 16, 17, true);
bfs::SbusData data;

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  sbus.Begin();
  Serial.println("Bolder Flight SBUS ready on GPIO16 (RXD2)");
}

void loop() {
  if (sbus.Read()) {
    data = sbus.data();

    // ── channels (0-indexed, raw 172–1811) ──
    Serial.print("CH: ");
    for (int i = 0; i < data.NUM_CH; i++) {
      Serial.printf("%4d ", data.ch[i]);
    }

    // ── flags ──
    Serial.printf("| Lost:%d  FS:%d\n",
                  data.lost_frame,
                  data.failsafe);
  }
}