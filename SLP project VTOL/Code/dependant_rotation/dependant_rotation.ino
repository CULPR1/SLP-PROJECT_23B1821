#include <ESP32Servo.h>
#include <Wire.h>


#define AS5600_ADDR 0x36

Servo tail_motor;
int tailpin = 18;

Servo main_motor;
int mainpin = 19;

bool running = false;
bool check = false;
char incoming = 'd';

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
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin(21,22);

  tail_motor.setPeriodHertz(60);
  tail_motor.attach(tailpin,1000,2000);
  tail_motor.writeMicroseconds(1000);
  delay(3000);

  main_motor.setPeriodHertz(60);
  main_motor.attach(mainpin,1000,2000);
  main_motor.writeMicroseconds(1000);
  delay(3000);


}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0){
    incoming = Serial.read();
    Serial.println(incoming);
    if (incoming == 'w'){
      running = true;
    }
    else if(incoming == 'd'){
      running = false;
    }
  }

    if (running) {
      uint16_t rawAngle = readRegister16(0x0C);
      if (rawAngle > 1024 && rawAngle < 3072) {
        
        // Serial.println(micros());
        Serial.println(rawAngle);
        main_motor.writeMicroseconds(1700);
      }
      else{
        // Serial.println(micros());
        Serial.println(rawAngle);
        main_motor.writeMicroseconds(1100);
      }
    }
    else{
      uint16_t rawAngle = readRegister16(0x0C);
      Serial.println(rawAngle);
      main_motor.writeMicroseconds(1000);
    }
  }



