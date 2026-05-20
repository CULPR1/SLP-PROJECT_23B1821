#include "sbus.h"
int LED = 13;

/* SBUS object on Serial2 (Pin 7) */
bfs::SbusRx sbus_rx(&Serial);
bfs::SbusData data;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 5000) {} // Wait for monitor
  
  /* Start SBUS communication */
  sbus_rx.Begin();
  Serial.println("SBUS Listener Active on Pin 7");
}

void loop() {
  /* Read() returns true only when a valid packet is parsed */
  if (sbus_rx.Read()) {
    data = sbus_rx.data();

  int throttle = data[2];
  if (throttle > 992){
    
  digitalWrite(LED,HIGH);
    
  }
  else{
    
  digitalWrite(LED,LOW);
  }

  }
}