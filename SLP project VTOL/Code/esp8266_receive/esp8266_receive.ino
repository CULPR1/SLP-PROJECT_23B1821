// ===== SBUS on ESP8266 (Hardware UART) =====

#define SBUS_PACKET_SIZE 25
uint8_t buffer[SBUS_PACKET_SIZE];
uint8_t idx = 0;

int channels[16];
bool failsafe = false;
bool lostFrame = false;

// ===== LED pin (ESP8266 onboard LED) =====
#define LED_PIN 2   // GPIO2 (D4 on NodeMCU, active LOW)

// ===== Threshold =====
#define THRESHOLD 1000   // adjust as needed (typical SBUS range ~172–1811)

// ===== SBUS Read Function =====
bool readSBUS() {
  while (Serial.available()) {
    uint8_t b = Serial.read();

    if (idx == 0 && b != 0x0F) continue;  // wait for start byte

    buffer[idx++] = b;

    if (idx == SBUS_PACKET_SIZE) {
      idx = 0;

      if (buffer[24] != 0x00) return false;  // invalid frame

      // Decode channels (11-bit packed)
      channels[0]  = ((buffer[1]       | buffer[2]  << 8)                      & 0x07FF);
      channels[1]  = ((buffer[2]  >> 3 | buffer[3]  << 5)                      & 0x07FF);
      channels[2]  = ((buffer[3]  >> 6 | buffer[4]  << 2  | buffer[5]  << 10)  & 0x07FF);
      channels[3]  = ((buffer[5]  >> 1 | buffer[6]  << 7)                      & 0x07FF);
      channels[4]  = ((buffer[6]  >> 4 | buffer[7]  << 4)                      & 0x07FF);
      channels[5]  = ((buffer[7]  >> 7 | buffer[8]  << 1  | buffer[9]  << 9)   & 0x07FF);
      channels[6]  = ((buffer[9]  >> 2 | buffer[10] << 6)                      & 0x07FF);
      channels[7]  = ((buffer[10] >> 5 | buffer[11] << 3)                      & 0x07FF);
      channels[8]  = ((buffer[12]      | buffer[13] << 8)                      & 0x07FF);
      channels[9]  = ((buffer[13] >> 3 | buffer[14] << 5)                      & 0x07FF);
      channels[10] = ((buffer[14] >> 6 | buffer[15] << 2  | buffer[16] << 10)  & 0x07FF);
      channels[11] = ((buffer[16] >> 1 | buffer[17] << 7)                      & 0x07FF);
      channels[12] = ((buffer[17] >> 4 | buffer[18] << 4)                      & 0x07FF);
      channels[13] = ((buffer[18] >> 7 | buffer[19] << 1  | buffer[20] << 9)   & 0x07FF);
      channels[14] = ((buffer[20] >> 2 | buffer[21] << 6)                      & 0x07FF);
      channels[15] = ((buffer[21] >> 5 | buffer[22] << 3)                      & 0x07FF);

      failsafe  = buffer[23] & (1 << 3);
      lostFrame = buffer[23] & (1 << 2);

      return true;
    }
  }
  return false;
}

// ===== Setup =====
void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // LED OFF (active LOW)

  Serial.begin(115200);   // temporary debug
  Serial.println("Init...");

  Serial.swap();          // move RX → GPIO13 (D7)

  Serial.begin(100000, SERIAL_8E2);  // SBUS config
}

// ===== Loop =====
void loop() {
  if (readSBUS()) {

    // 👉 Example: use Channel 1 (index 0)
    int ch = channels[0];

    // ===== LED Control =====
    if (ch > THRESHOLD) {
      digitalWrite(LED_PIN, LOW);   // LED ON
    } else {
      digitalWrite(LED_PIN, HIGH);  // LED OFF
    }

    // (Optional) simple debug using LED blinking pattern
    // since Serial Monitor won't work after swap
  }
}