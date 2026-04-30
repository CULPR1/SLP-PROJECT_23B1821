#include <Servo.h>

Servo main_motor;
int main_pin = 14;

bool running = true;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  main_motor.attach(main_pin,1000,2000);
  main_motor.writeMicroseconds(1000);
  delay(3000);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0){
    char character = Serial.read();
    if (character == 's'){
      running = true;
    }
    else if (character == 'd'){
      running = false;
    }

  if (running){
    main_motor.writeMicroseconds(1100);
    delay(1000);
  }

  else{
    main_motor.writeMicroseconds(1000);
    delay(1000);
  }

  }

}
