#include "sbus.h"

bfs::SbusRx sbus_rx(&Serial2);
bfs::SbusData data;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial && millis() < 5000) {} // Wait for monitor
  
  /* Start SBUS communication */
  sbus_rx.Begin();
  Serial.println("SBUS Listener Active on Pin 7");

}

void loop() {
  // put your main code here, to run repeatedly:
  if (sbus_rx.Read()) {
    data = sbus_rx.data();


  }
  float y_axis = data.ch[2];
  float x_axis = data.ch[3];

  float Neutral_x = x_axis - 992;
  float Neutral_y = y_axis - 992;

  float angleRad = atan2(Neutral_y,Neutral_x);

  float angleDeg = 360 - (angleRad*180.0 / PI);
  if (angleDeg < 0) angleDeg += 360;


  Serial.println(angleDeg);



}
