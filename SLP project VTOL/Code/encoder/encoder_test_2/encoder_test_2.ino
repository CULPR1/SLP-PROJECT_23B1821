#include <Wire.h>
#define AS5600_ADDR  0x36

unsigned long startTime;
unsigned long endTime;
unsigned long executionTime;

String execution(uint16_t start,uint16_t end){

  uint16_t execute = end - start;
  String executed = "Execution Time: ";
  executed = executed + execute + " us";
  return executed;
  
}

uint16_t readRegister16(uint8_t reg) {
  Wire.beginTransmission(AS5600_ADDR);
  Wire.write(reg);
  Wire.endTransmission();

  Wire.requestFrom(AS5600_ADDR, 2);

  uint16_t high = Wire.read();
  uint16_t low  = Wire.read();

  return (high << 8) | low;
}


void setup() {
  Serial.begin(115200);
  Wire.begin(21,22);
}

void loop() {
  // Read the current angle in degrees
  startTime = micros();
  uint16_t rawAngle = readRegister16(0x0C);
  Serial.println(rawAngle);
  endTime = micros();
  Serial.println(execution(startTime,endTime));

  
  startTime = micros();
  float Angle = rawAngle*360.0/4096;
  Serial.println(Angle);
  endTime = micros();
  Serial.println(execution(startTime,endTime));
  delay(2000);
}