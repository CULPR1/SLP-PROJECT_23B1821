#include <Wire.h>
#include <AS5600.h>
#include "sbus.h"

AS5600 encoder;

unsigned long startTime;
unsigned long endTime;
unsigned long executionTime;


bfs::SbusRx sbus_rx(&Serial2);
bfs::SbusData data;

void setup() {

  Serial.begin(115200);

  while (!Serial && millis() < 5000) {} // Wait for monitor
  
  /* Start SBUS communication */
  sbus_rx.Begin();
  Serial.println("SBUS Listener Active on Pin 7");
  
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

  

  float angle = encoder.rawAngle();
  float degrees = (angle*360.0)/4096 ;

  Serial.print("Value :");
  Serial.print(angleDeg);
  Serial.print(" Bound_1 :");
  Serial.print(bound_1);
  Serial.print(" Bound_2 :");
  Serial.print(bound_2);
  Serial.print(" degrees :");
  Serial.print(degrees);
  Serial.println();

}
