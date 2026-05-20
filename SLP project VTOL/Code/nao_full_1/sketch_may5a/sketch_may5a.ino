#include <Servo.h>
#include <Wire.h>
#include "sbus.h"

#define AS5600_ADDR 0x36

bfs::SbusRx sbus_rx(&Serial);
bfs::SbusData data;

bool armed = false;

Servo tail_motor;
const int tailpin = 6;

Servo main_motor;
const int mainpin = 8;

uint16_t readRegister16(uint8_t reg) {
  Wire.beginTransmission(AS5600_ADDR);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(AS5600_ADDR, (uint8_t)2);
  if (Wire.available() < 2) return 0;

  uint16_t high = Wire.read();
  uint16_t low  = Wire.read();
  return (high << 8) | low;
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
  if (sbus_rx.Read()) {
    data = sbus_rx.data();
  }

  float y_axis = data.ch[1];
  float x_axis = data.ch[0];

  float neutral_x = x_axis - 992;
  float neutral_y = y_axis - 992;

  float angleRad = atan2(neutral_y, neutral_x);
  float angleDeg = -(angleRad * 180.0 / PI);
  if (angleDeg < 0) angleDeg += 360;

  float bound_1 = angleDeg + 90;
  float bound_2 = angleDeg - 90;

  int throttle = map(data.ch[2], 174, 1810, 1026, 2000);

  armed = (data.ch[5] == 1811);

  uint16_t rawAngle = readRegister16(0x0C);
  float deg_angle = rawAngle * 360.0 / 4096.0;

  if (armed) {
    if (deg_angle > min(bound_1, bound_2) && deg_angle < max(bound_1, bound_2)) {
      main_motor.writeMicroseconds(1100);
      tail_motor.writeMicroseconds(1100);
    } else {
      main_motor.writeMicroseconds(throttle);
      tail_motor.writeMicroseconds(throttle);
    }
  } else {
    main_motor.writeMicroseconds(1000);
    tail_motor.writeMicroseconds(1000);
  }
}