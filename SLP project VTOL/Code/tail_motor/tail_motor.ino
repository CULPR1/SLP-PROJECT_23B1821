#include <ESP32Servo.h>


Servo tail;
int escpin = 18;
void setup() {
  // put your setup code here, to run on
  
  tail.setPeriodHertz(60);
  tail.attach(19,1000,2000);
  tail.writeMicroseconds(1000);
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  tail.writeMicroseconds(1000);
  delay(2000);
  tail.writeMicroseconds(1100);
  delay(2000);


}
