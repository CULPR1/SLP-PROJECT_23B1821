#include <Servo.h>
#include <Wire.h>
#include "sbus.h"

#define AS5600_ADDR 0x36
#define NANO2_ADDR 0x09 // Address of the second Nano
#define STICK_DEADZONE 30      
#define NEUTRAL         992

// Structure to pack all your debug data into one fast transmission
struct DataPacket {
  float encoderAngle;
  float stickAngle;
  float winStart;
  float winEnd;
  int throttleVal;
  int motorOut;
};

DataPacket debugData;
unsigned long lastSendTime = 0;
const int sendInterval = 100; // Send telemetry every 100ms

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
  if (b1 > b2) return (angle >= b2 && angle <= b1);
  else return (angle >= b2 || angle <= b1);
}

void failsafe() {
  main_motor.writeMicroseconds(1000);
  tail_motor.writeMicroseconds(1000);
  armed = false;
}

void setup() {
  // We do NOT use Serial.begin() here because hardware serial is for SBUS
  
  tail_motor.attach(tailpin, 1000, 2000);
  tail_motor.writeMicroseconds(1000);
  main_motor.attach(mainpin, 1000, 2000);
  main_motor.writeMicroseconds(1000);
  
  delay(3000);

  sbus_rx.Begin();
  Wire.begin(); // Initialize as Master
  Wire.setClock(400000); // Fast I2C (400kHz)
}

void loop() {
  if (sbus_rx.Read()) {
    data = sbus_rx.data();
    lastSbusTime = millis();
  }

  if (millis() - lastSbusTime > 100) {
    failsafe();
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
    angleDeg = -(angleRad * 180.0 / PI);
    if (angleDeg < 0) angleDeg += 360;
    bound_1 = fmod(angleDeg + 90.0, 360.0);
    bound_2 = fmod(angleDeg - 90.0 + 360.0, 360.0);
  }

  int throttle = map(data.ch[2], 174, 1810, 1026, 2000);
  throttle = constrain(throttle, 1000, 2000);

  // Arming Logic
  bool tryArm = (data.ch[5] > 1500); 
  if (tryArm && !armed) {
    if (data.ch[2] < 300) armed = true;
  } else if (!tryArm) {
    armed = false;
  }

  // Encoder Read
  uint16_t rawAngle = readRegister16(0x0C);
  if (rawAngle == 0xFFFF) {
    failsafe();
    return;
  }
  float deg_angle = rawAngle * 360.0 / 4096.0;

  int finalSpeed = 1000;
  if (armed) {
    if (stickNeutral) {
      finalSpeed = throttle;
    } else {
      finalSpeed = inWindow(deg_angle, bound_1, bound_2) ? 1100 : throttle;
    }
    main_motor.writeMicroseconds(finalSpeed);
    tail_motor.writeMicroseconds(finalSpeed);
  } else {
    failsafe();
  }

  // --- I2C TELEMETRY TRANSMISSION ---
  if (millis() - lastSendTime >= sendInterval) {
    lastSendTime = millis();
    
    // Pack the data
    debugData.encoderAngle = deg_angle;
    debugData.stickAngle   = angleDeg;
    debugData.winStart     = bound_2;
    debugData.winEnd       = bound_1;
    debugData.throttleVal  = throttle;
    debugData.motorOut     = finalSpeed;

    // Push to Nano 2
    Wire.beginTransmission(NANO2_ADDR);
    Wire.write((byte*)&debugData, sizeof(debugData));
    Wire.endTransmission();
  }
}