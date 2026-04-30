#include <Wire.h>
#include <AS5600.h>

AS5600 encoder;

unsigned long startTime;
unsigned long endTime;
unsigned long executionTime;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!encoder.begin()) {
    Serial.println("AS5600 not detected. Check connections.");
    while (1);
  }

  //Serial.println("AS5600 initialized.");
}

void loop() {
  // Read the current angle in degrees
  startTime = micros();
  float angle = encoder.rawAngle();
  float degrees = (angle*360)/4096 ;
  Serial.println(degrees);
  endTime = micros();

  executionTime = endTime - startTime;
  Serial.print("ExecutionTime :"); 
  Serial.print(executionTime);
  Serial.println(" um");
  delay(1000);
}
