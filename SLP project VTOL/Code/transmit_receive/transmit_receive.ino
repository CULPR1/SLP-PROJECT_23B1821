#include <sbus.h>
#include <HardwareSerial.h>

HardwareSerial SerialSBUS(2);

// TRY true first
bfs::SbusRx sbus(&SerialSBUS, 13, -1, true);

bfs::SbusData data;

void setup() {
  Serial.begin(115200);

  Serial.println("Starting SBUS...");

  SerialSBUS.begin(100000, SERIAL_8E2, 13, -1);

  sbus.Begin();
}

void loop() {
  // Debug to ensure loop is running
  // Serial.println("Loop running");
  // delay(200);

  if (sbus.Read()) {
    data = sbus.data();
    Serial.println(data.ch[2]);

    if (data.failsafe) {
      Serial.println("FAILSAFE!");
    }
  }
}
