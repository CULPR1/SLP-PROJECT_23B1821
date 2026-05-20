#include <Wire.h>

// AS5600 I2C address and registers
#define AS5600_ADDR   0x36
#define REG_ANGLE_H   0x0E  // High byte of 12-bit angle
#define REG_STATUS    0x0B  // Magnet detection status
#define REG_AGC       0x1A  // Automatic gain control
#define REG_MAGNITUDE 0x1B  // Magnitude (2 bytes, 0x1B-0x1C)

// ── helpers ───────────────────────────────────────────────
uint16_t readAngleRaw() {
  Wire.beginTransmission(AS5600_ADDR);
  Wire.write(REG_ANGLE_H);
  Wire.endTransmission(false);          // repeated-start
  Wire.requestFrom(AS5600_ADDR, 2);
  uint16_t hi = Wire.read();
  uint16_t lo = Wire.read();
  return ((hi << 8) | lo) & 0x0FFF;    // 12 bits, 0–4095
}

uint8_t readStatus() {
  Wire.beginTransmission(AS5600_ADDR);
  Wire.write(REG_STATUS);
  Wire.endTransmission(false);
  Wire.requestFrom(AS5600_ADDR, 1);
  return Wire.read();
}

uint8_t readAGC() {
  Wire.beginTransmission(AS5600_ADDR);
  Wire.write(REG_AGC);
  Wire.endTransmission(false);
  Wire.requestFrom(AS5600_ADDR, 1);
  return Wire.read();
}

// ── velocity tracking ─────────────────────────────────────
uint16_t prevRaw      = 0;
float    totalAngle   = 0;   // cumulative degrees (unwrapped)
uint32_t prevTime     = 0;

float unwrapAngle(uint16_t raw) {
  // Convert to degrees, detect wrap-around, accumulate
  float deg = raw * 360.0f / 4096.0f;
  float prev = prevRaw * 360.0f / 4096.0f;
  float delta = deg - prev;

  if (delta >  180.0f) delta -= 360.0f;  // wrap CW→CCW
  if (delta < -180.0f) delta += 360.0f;  // wrap CCW→CW

  totalAngle += delta;
  prevRaw = raw;
  return totalAngle;
}

// ── setup ─────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);          // SDA=21, SCL=22
  Wire.setClock(400000);       // 400 kHz fast mode

  delay(100);                  // let encoder boot

  // Check magnet status
  uint8_t status = readStatus();
  bool magnetDetected = status & (1 << 5);  // bit 5 = MD
  bool tooWeak        = status & (1 << 4);  // bit 4 = ML
  bool tooStrong      = status & (1 << 3);  // bit 3 = MH

  Serial.println("=== AS5600 initialised ===");
  Serial.printf("Status: 0x%02X  Magnet: %s  Weak: %s  Strong: %s\n",
                status,
                magnetDetected ? "YES" : "NO",
                tooWeak        ? "YES" : "NO",
                tooStrong      ? "YES" : "NO");

  if (!magnetDetected) {
    Serial.println("WARNING: no magnet detected — check placement!");
  }

  prevRaw  = readAngleRaw();
  prevTime = millis();
}

// ── main loop ─────────────────────────────────────────────
void loop() {
  uint16_t raw   = readAngleRaw();
  float    angle = raw * 360.0f / 4096.0f;   // 0–360°
  float    unwrapped = unwrapAngle(raw);

  uint32_t now   = millis();
  uint32_t dt_ms = now - prevTime;
  prevTime = now;

  // RPM from unwrapped angle change per time step
  // (delta is already accumulated inside unwrapAngle; re-derive from totalAngle)
  static float lastTotal = 0;
  float deltaDeg = totalAngle - lastTotal;
  lastTotal = totalAngle;
  float rpm = (deltaDeg / 360.0f) / (dt_ms / 60000.0f);

  Serial.printf("Raw: %4u  Angle: %7.2f°  Unwrapped: %9.2f°  RPM: %7.1f  AGC: %3u\n",
                raw, angle, unwrapped, rpm, readAGC());

  delay(20);  // 50 Hz sample rate
}