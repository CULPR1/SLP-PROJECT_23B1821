#include <Servo.h>


Servo main_motor;
int escpin = 2;
void setup() {
  // put your setup code here, to run on
  Serial.begin(9600);
  
  analogWriteFrequency(escpin, 60);;
  main_motor.attach(escpin,1000,2000);
  main_motor.writeMicroseconds(1000);
  delay(3000);
}

void loop() {
  // put your main code here, to run repeatedly:
    Serial.println(1000);
    main_motor.writeMicroseconds(1000);
    delay(1000);
    Serial.println(1100);
    main_motor.writeMicroseconds(1100);
    delay(1000);
    
  }


