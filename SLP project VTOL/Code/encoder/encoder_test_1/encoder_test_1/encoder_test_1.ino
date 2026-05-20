#include <Wire.h>
#include <AS5600.h>

AS5600 encoder;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 5000) {}

  Wire.begin();
  encoder.begin();

  if (!encoder.isConnected()) {
    Serial.println("AS5600 not detected. Check connections.");
    while (1);
  }

  Serial.println("AS5600 initialized.");
}

void loop() {
  float angle = encoder.rawAngle();
  float degrees = (angle * 360.0) / 4096.0;

  Serial.println(degrees);
}