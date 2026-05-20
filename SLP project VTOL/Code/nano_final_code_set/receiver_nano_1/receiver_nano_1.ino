#include <Wire.h>

struct DataPacket {
  float encoderAngle;
  float stickAngle;
  float winStart;
  float winEnd;
  int throttleVal;
  int motorOut;
};

volatile DataPacket incoming;
volatile bool newData = false;

void setup() {
  Serial.begin(115200); // High speed for Serial Monitor
  Wire.begin(0x09);     // Join I2C bus as address 9
  Wire.onReceive(receiveEvent);
  
  Serial.println("--- I2C TELEMETRY MONITOR ONLINE ---");
}

void loop() {
  if (newData) {
    Serial.print("ENC: ");   Serial.print(incoming.encoderAngle, 1);
    Serial.print(" | STK: "); Serial.print(incoming.stickAngle, 1);
    Serial.print(" | WIN: "); Serial.print(incoming.winStart, 0);
    Serial.print("-");        Serial.print(incoming.winEnd, 0);
    Serial.print(" | THR: "); Serial.print(incoming.throttleVal);
    Serial.print(" | OUT: "); Serial.println(incoming.motorOut);
    
    newData = false;
  }
}

// Function that triggers automatically when data arrives
void receiveEvent(int howMany) {
  if (howMany == sizeof(DataPacket)) {
    Wire.readBytes((byte*)&incoming, sizeof(DataPacket));
    newData = true;
  }
}