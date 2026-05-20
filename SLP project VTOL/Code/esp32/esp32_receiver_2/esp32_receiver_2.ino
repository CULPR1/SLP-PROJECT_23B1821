/*
  SBUS → ESC  (throttle on CH3 → GPIO26)
  ESP32 WROOM
  
  Libraries:
    - Bolder Flight Systems SBUS  (Library Manager → "SBUS")
    - ESP32 Servo                 (Library Manager → "ESP32Servo")
*/

#include "sbus.h"
#include <ESP32Servo.h>

// ── SBUS ──────────────────────────────────────────────────
bfs::SbusRx sbus(&Serial2, 16, 17, true);
bfs::SbusData data;

// ── ESC ───────────────────────────────────────────────────
Servo esc;
#define ESC_PIN       26
#define ESC_MIN_US    1000   // full stop  (arm pulse)
#define ESC_MAX_US    2000   // full throttle
#define ESC_ARM_MS    3000   // how long to hold arm pulse on boot

// ── SBUS raw range ────────────────────────────────────────
#define SBUS_MIN      172
#define SBUS_MAX      1811

// ── safety ────────────────────────────────────────────────
#define FAILSAFE_US   ESC_MIN_US   // output when signal lost
#define DEADBAND      5            // raw SBUS units around min, forced to 1000 µs

// ── helpers ───────────────────────────────────────────────
int16_t invertChannel(int16_t raw) {
  return SBUS_MAX - (raw - SBUS_MIN);
}

// Map SBUS raw → ESC microseconds, with deadband at the low end
uint16_t throttleToUs(int16_t raw) {
  raw = constrain(raw, SBUS_MIN, SBUS_MAX);
  if (raw < SBUS_MIN + DEADBAND) return ESC_MIN_US;   // deadband → disarmed pulse
  return (uint16_t) map(raw, SBUS_MIN, SBUS_MAX, ESC_MIN_US, ESC_MAX_US);
}

// ── setup ─────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  // Allocate a timer for the ESC servo signal
  ESP32PWM::allocateTimer(0);
  esc.setPeriodHertz(50);          // standard 50 Hz ESC signal
  esc.attach(ESC_PIN, ESC_MIN_US, ESC_MAX_US);

  // ── ARM sequence ──────────────────────────────────────
  // Hold 1000 µs for ESC_ARM_MS ms so the ESC arms cleanly
  Serial.println("Arming ESC — hold throttle low...");
  esc.writeMicroseconds(ESC_MIN_US);
  delay(ESC_ARM_MS);
  Serial.println("ESC armed");

  sbus.Begin();
  Serial.println("SBUS ready on GPIO16 (RXD2)");
}

// ── loop ──────────────────────────────────────────────────
void loop() {
  if (sbus.Read()) {
    data = sbus.data();

    // CH2 (elevator / pitch) — inverted per your earlier fix
    data.ch[1] = invertChannel(data.ch[1]);

    // ── failsafe / frame-lost → cut throttle ──────────
    if (data.failsafe || data.lost_frame) {
      esc.writeMicroseconds(FAILSAFE_US);
      Serial.println("FAILSAFE — throttle cut");
      return;
    }

    // ── CH3 → throttle ────────────────────────────────
    uint16_t throttle_us = throttleToUs(data.ch[2]);
    esc.writeMicroseconds(throttle_us);

    // ── debug ─────────────────────────────────────────
    Serial.printf("CH3 raw:%4d  throttle:%4d µs  |  Lost:%d FS:%d\n",
                  data.ch[2], throttle_us,
                  data.lost_frame, data.failsafe);
  }
}