#include <sbus.h>
#include <HardwareSerial.h>
#include <ESP32Servo.h>
#include <Wire.h>


HardwareSerial SerialSBUS(2);


bfs::SbusRx sbus(&SerialSBUS, 16, -1, true);

bfs::SbusData data;


Servo tail_motor;
int tailpin = 18;

Servo main_motor;
int mainpin = 19;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Starting SBUS...");
  SerialSBUS.begin(100000, SERIAL_8E2, 16, -1);
  sbus.Begin();

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

   if (sbus.Read()) {
    data = sbus.data();
    int throttle = data.ch[2];
    int pwm = map(data.ch[2],170,1810,1000,2000);

    main_motor.writeMicroseconds(pwm);

  

  

}
}
