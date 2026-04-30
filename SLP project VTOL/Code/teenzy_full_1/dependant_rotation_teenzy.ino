#include <Servo.h>
#include <Wire.h>
#include "sbus.h"

#define AS5600_ADDR 0x36

bfs::SbusRx sbus_rx(&Serial2);
bfs::SbusData data;

bool armed = false;

Servo tail_motor;
int tailpin = 2;

Servo main_motor;
int mainpin = 3;

bool running = false;
char incoming = 'd';

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

bool deadzone(int n1, int n2, int max, int min){

  if( (n1 < max && n1 > min) && (n2 < max && n2 > min)){

    return true;
  } 
  else{
    return false;
  }
}


void setup() {
  Serial.begin(115200);

  while (!Serial && millis() < 5000) {} // Wait for monitor
  
  /* Start SBUS communication */
  sbus_rx.Begin();
  Serial.println("SBUS Listener Active on Pin 7");
  
  // Use Pins 18 (SDA) and 19 (SCL) for AS5600
  Wire.begin();
  Wire.setClock(400000); 

  // Teensy 4.1 defaults to 50Hz. If you need 60Hz:
  analogWriteFrequency(tailpin, 60);
  analogWriteFrequency(mainpin, 60);

  tail_motor.attach(tailpin, 1000, 2000);
  tail_motor.writeMicroseconds(1000);
  delay(3000);
  main_motor.attach(mainpin, 1000, 2000);
  main_motor.writeMicroseconds(1000);

  delay(3000); // Wait for ESCs to arm
}

void loop() {
  if (sbus_rx.Read()) {
    data = sbus_rx.data();


  }
  float y_axis = data.ch[2];
  float x_axis = data.ch[3];

  float Neutral_x = x_axis - 992;
  float Neutral_y = y_axis - 992;

  float angleRad = atan2(Neutral_y,Neutral_x);

  float angleDeg = -(angleRad*180.0 / PI);
  if (angleDeg < 0) angleDeg += 360;


  float bound_1 = angleDeg + 90;

  float bound_2 = angleDeg - 90;

  int throttle = map(data.ch[0],174,1810,1026,2000);
  if (data.ch[5] == 1811){
    armed = true;
  }
  else{
    armed = false;
  }

  bool deadzone_check = deadzone(x_axis,y_axis, 1002,982);


  // 2. Read Encoder (always do this once per loop)w
  uint16_t rawAngle = readRegister16(0x0C);
  float deg_angle = rawAngle * 360.0/ 4096;



  // 3. Logic Control
  if (armed) {
    if (deadzone_check){
      main_motor.writeMicroseconds(throttle);
      tail_motor.writeMicroseconds(throttle);
    } 
    else{
      if (deg_angle > min(bound_1,bound_2) && deg_angle < max(bound_1,bound_2) ){
      Serial.print(1100);
      Serial.print(" Angle :");
      Serial.print(deg_angle);
      Serial.println();
      main_motor.writeMicroseconds(1100);
      tail_motor.writeMicroseconds(1100);
    } 
      else {
      Serial.print(throttle);
      Serial.print(" Angle :");
      Serial.print(deg_angle);
      Serial.println();
      main_motor.writeMicroseconds(throttle);
      tail_motor.writeMicroseconds(throttle);
    }
    }
  } 
  
  else {
    main_motor.writeMicroseconds(1000);
    tail_motor.writeMicroseconds(1000); // Stop
  }
}