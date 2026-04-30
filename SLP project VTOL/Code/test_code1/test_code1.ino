#define LED 2
void setup() {
  // put your setup code here, to run once:
  pinMode(LED,OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(200);
  digitalWrite(LED,HIGH);
  delay(200);
  digitalWrite(LED,LOW);

}
