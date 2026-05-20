#include <Servo.h>
#include <Wire.h>
#include "sbus.h"

#define AS5600_ADDR 0x36

// Tune these to your stick's physical deadzone
#define STICK_DEADZONE 30      // ± raw units around neutral (992)
#define NEUTRAL         992

bfs::SbusRx sbus_rx(&Serial);
bfs::SbusData data;

bool armed = false;
unsigned long lastSbusTime = 0;

Servo tail_motor;
const int tailpin = 6;

Servo main_motor;
const int mainpin = 8;

uint16_t readRegister16(uint8_t reg) {
  Wire.beginTransmission(AS5600_ADDR);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(AS5600_ADDR, (uint8_t)2);

  unsigned long t = millis();
  while (Wire.available() < 2) {
    if (millis() - t > 5) return 0xFFFF;
  }

  uint16_t high = Wire.read();
  uint16_t low  = Wire.read();
  return (high << 8) | low;
}

bool inWindow(float angle, float b1, float b2) {
  if (b2 < b1) {
    return angle >= b2 && angle <= b1;
  } else {
    return angle >= b2 || angle <= b1;
  }
}

void failsafe() {
  main_motor.writeMicroseconds(1000);
  tail_motor.writeMicroseconds(1000);
  armed = false;
}

void setup() {
  tail_motor.attach(tailpin, 1000, 2000);
  tail_motor.writeMicroseconds(1000);
  delay(3000);

  main_motor.attach(mainpin, 1000, 2000);
  main_motor.writeMicroseconds(1000);
  delay(3000);

  sbus_rx.Begin();

  Wire.begin();
  Wire.setClock(400000);
}

void loop() {
  // 1. SBUS read + signal loss detection
  if (sbus_rx.Read()) {
    data = sbus_rx.data();
    lastSbusTime = millis();
  }

  if (millis() - lastSbusTime > 100) {
    failsafe();
    return;
  }

  // 2. Joystick → angle
  float y_axis = data.ch[1];
  float x_axis = data.ch[0];

  float neutral_x = x_axis - NEUTRAL;
  float neutral_y = y_axis - NEUTRAL;

  // 3. Deadzone check — if both axes are within ±STICK_DEADZONE, stick is neutral
  bool stickNeutral = (abs(neutral_x) < STICK_DEADZONE && abs(neutral_y) < STICK_DEADZONE);

  float angleDeg = 0;
  float bound_1  = 0;
  float bound_2  = 0;

  if (!stickNeutral) {
    float angleRad = atan2(neutral_y, neutral_x);
    angleDeg = -(angleRad * 180.0 / PI);
    if (angleDeg < 0) angleDeg += 360;

    bound_1 = fmod(angleDeg + 90.0, 360.0);
    bound_2 = fmod(angleDeg - 90.0 + 360.0, 360.0);
  }

  // 4. Throttle with guard
  int throttle = map(data.ch[2], 174, 1810, 1026, 2000);
  throttle = constrain(throttle, 1000, 2000);

  // 5. Arming with throttle-low check
  bool tryArm = (data.ch[5] == 1811);
  if (tryArm && !armed) {
    if (data.ch[2] < 300) armed = true;
  } else if (!tryArm) {
    armed = false;
  }

  // 6. Encoder read with timeout check
  uint16_t rawAngle = readRegister16(0x0C);
  if (rawAngle == 0xFFFF) {
    failsafe();
    return;
  }
  float deg_angle = rawAngle * 360.0 / 4096.0;

  // 7. Motor control
  if (armed) {
    if (stickNeutral) {
      // No directional input — run purely on throttle, no encoder logic
      main_motor.writeMicroseconds(throttle);
      tail_motor.writeMicroseconds(throttle);
    } else {
      // Directional input — use encoder window
      if (inWindow(deg_angle, bound_1, bound_2)) {
        main_motor.writeMicroseconds(1100);
        tail_motor.writeMicroseconds(1100);
      } else {
        main_motor.writeMicroseconds(throttle);
        tail_motor.writeMicroseconds(throttle);
      }
    }
  } else {
    failsafe();
  }
}