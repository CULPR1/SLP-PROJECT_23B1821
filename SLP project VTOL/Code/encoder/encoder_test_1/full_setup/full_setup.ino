#include <Wire.h>
#include <AS5600.h>
#include <ESP32Servo.h>

Servo tail;
int tail_pin = 19;

Servo front;
int front_pin = 18;


AS5600 encoder;


void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!encoder.begin()) {
    Serial.println("AS5600 not detected. Check connections.");
    while (1);
  }

  Serial.println("AS5600 initialized.");

  // put your setup code here, to run once:
  tail.setPeriodHertz(60);
  tail.attach(tail_pin,1000,2000);
  tail.writeMicroseconds(1000);
  delay(2000);

  front.setPeriodHertz(60);
  front.attach(front_pin,1000,2000);
  front.writeMicroseconds(1000);
  delay(2000);

}

void loop() {
  // put your main code here, to run repeatedly:
  tail.writeMicroseconds(1000);
  delay(1000);
  tail.writeMicroseconds(1100);
  delay(1000);
  tail.writeMicroseconds(1000);
  delay(1000);

  front.writeMicroseconds(1000);
  delay(1000);
  front.writeMicroseconds(1100);
  delay(1000);
  front.writeMicroseconds(1000);
  delay(1000);
  

    // Read the current angle in degrees
  float angle = encoder.rawAngle();
  float degrees = (angle*360)/4096 ;
  Serial.println(degrees);

  delay(1000); // Update every 100ms

  

}
