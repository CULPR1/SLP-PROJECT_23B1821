#include <ESP32Servo.h>
#include <Wire.h>
#include "sbus.h"

#define AS5600_ADDR 0x36
#define STICK_DEADZONE 30      
#define NEUTRAL 992

bfs::SbusRx sbus_rx(&Serial2, 16, -1, true);
bfs::SbusData data;

bool armed = false;
unsigned long lastSbusTime = 0;

Servo tail_motor;
const int tailpin = 26;
Servo main_motor;
const int mainpin = 18;

unsigned long lastPrintTime = 0;
const int printInterval = 100;

uint16_t readRegister16(uint8_t reg) {
  Wire.beginTransmission(AS5600_ADDR);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom((uint8_t)AS5600_ADDR, (uint8_t)2);
  unsigned long t = millis();
  while (Wire.available() < 2) {
    if (millis() - t > 5) return 0xFFFF;
  }
  uint16_t high = Wire.read();
  uint16_t low  = Wire.read();
  return (high << 8) | low;
}

bool inWindow(float angle, float b1, float b2) {
  if (b1 > b2) return (angle >= b2 && angle <= b1);
  else return (angle >= b2 || angle <= b1);
}


void failsafe() {
  main_motor.writeMicroseconds(1000);
  tail_motor.writeMicroseconds(1000);
  armed = false;
}


void setup() {
  Serial.begin(115200);

  tail_motor.attach(tailpin, 1000, 2000);
  tail_motor.writeMicroseconds(1000);
  main_motor.attach(mainpin, 1000, 2000);
  main_motor.writeMicroseconds(1000);

  delay(3000);

  sbus_rx.Begin();

  Wire.begin();
  Wire.setClock(400000);

  Serial.println("System ready.");
}

void loop() {
  if (sbus_rx.Read()) {
    data = sbus_rx.data();
    lastSbusTime = millis();
  }

  if (millis() - lastSbusTime > 500) {
    failsafe();
    if (millis() - lastPrintTime >= 500) {
      lastPrintTime = millis();
      Serial.println("SBUS signal lost! Failsafe active.");
    }
    return;
  }

  // Calculate Stick Angles
  float y_axis = data.ch[1];
  float x_axis = data.ch[0];
  float neutral_x = x_axis - NEUTRAL;
  float neutral_y = y_axis - NEUTRAL;
  bool stickNeutral = (abs(neutral_x) < STICK_DEADZONE && abs(neutral_y) < STICK_DEADZONE);

  float angleDeg = 0;
  float bound_1  = 0;
  float bound_2  = 0;

  if (!stickNeutral) {
    float angleRad = atan2(neutral_y, neutral_x);
    angleDeg = (angleRad * 180.0 / PI) - 90;
    if (angleDeg < 0) angleDeg += 360;
    bound_1 = fmod(angleDeg + 90.0, 360.0);
    bound_2 = fmod(angleDeg - 90.0 + 360.0, 360.0);
  }

  // FIX 6 — map now starts at 1000 not 1026
  int throttle = map(data.ch[2], 174, 1810, 1000, 2000);
  throttle = constrain(throttle, 1000, 2000);

  // FIX 1 — arming now checks mapped throttle value instead of raw ch[2]
  bool tryArm = (data.ch[5] > 1500);
  if (tryArm && !armed) {
    if (throttle <= 1050) armed = true;
  } else if (!tryArm) {
    armed = false;
  }

  // FIX 2 — encoder failure now requires 5 consecutive failures before failsafe
  static int encoderFailCount = 0;
  uint16_t rawAngle = readRegister16(0x0C);
  if (rawAngle == 0xFFFF) {
    encoderFailCount++;
    if (encoderFailCount > 5) {
      Serial.println("Encoder failure! Failsafe active.");
      failsafe();
      return;
    }
  } else {
    encoderFailCount = 0;
  }
  float deg_angle = rawAngle * 360.0 / 4096.0;

  int finalSpeed = 1000;
  if (armed) {
    if (stickNeutral) {
      finalSpeed = throttle;
    } else {
      // FIX 3 — inWindow result flipped: motor runs full INSIDE window, slows OUTSIDE
      finalSpeed = inWindow(deg_angle, bound_1, bound_2) ? throttle : 1000;
    }
    main_motor.writeMicroseconds(finalSpeed);
    tail_motor.writeMicroseconds(finalSpeed);
  } else {
    failsafe();
  }

  // --- SERIAL TELEMETRY ---
  if (millis() - lastPrintTime >= printInterval) {
    lastPrintTime = millis();
    Serial.print("EncAngle: ");      Serial.print(deg_angle, 2);
    Serial.print(" | StickAngle: "); Serial.print(angleDeg, 2);
    Serial.print(" | WinStart: ");   Serial.print(bound_2, 2);
    Serial.print(" | WinEnd: ");     Serial.print(bound_1, 2);
    Serial.print(" | Throttle: ");   Serial.print(throttle);
    Serial.print(" | MotorOut: ");   Serial.print(finalSpeed);
    Serial.print(" | EncFails: ");   Serial.print(encoderFailCount);
    Serial.print(" | Armed: ");      Serial.println(armed ? "YES" : "NO");
  }
}