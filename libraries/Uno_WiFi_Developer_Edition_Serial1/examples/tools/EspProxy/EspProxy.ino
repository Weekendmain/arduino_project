#include <UnoWiFiDevEdSerial1.h>

//#define FLASHING // uncomment for use with esptool and FDT
#define BAUD 115200L

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(BAUD);

#ifdef FLASHING
  digitalWrite(LED_BUILTIN, HIGH);
  Serial1.begin(BAUD * 2); //double speed is necessary, but it disturbs frequency test of the 'download tool'
  Serial1.resetESP(true); // reset to bootloader. no need to push the B/L button while connecting to USB
#else
  digitalWrite(LED_BUILTIN, LOW);
  Serial1.begin(BAUD);
  Serial1.resetESP(); // power cycle
#endif
}

void loop() {
  while (Serial.available()) {
    detectFlashing();
    Serial1.write(Serial.read());
  }
  while (Serial1.available()) {
    Serial.write(Serial1.read());
  }
  detectFlashingEnd();
}

#ifdef FLASHING
void detectFlashing() {
  // empty
}
void detectFlashingEnd() {
  // empty
}
#else
const byte syncFrame[] = {0xC0, 0x00, 0x08, 0x24, 0x00, 0xDD, 0x00, 0x00, 0x00, 0x07, 0x07, 0x12, 0x20};
const byte checkSumPos = 5; // esptool.py and FDT do not send the checksum for sync frame, IDE does
byte syncFrameIndex = 0;

void detectFlashing() {
  if (syncFrameIndex == sizeof(syncFrame))
    return;
  byte b = Serial.peek();
  if (!(b == syncFrame[syncFrameIndex] || (syncFrameIndex == checkSumPos && b == 0x00))) {
    syncFrameIndex = 0;
  } else {
    syncFrameIndex++;
    if (syncFrameIndex == sizeof(syncFrame)) {
      Serial1.resetESP(true); // reset to bootloader
      digitalWrite(LED_BUILTIN, HIGH);
      Serial1.begin(BAUD * 2); //double speed is necessary, but it disturbs frequency test of the 'download tool'
      Serial1.write(syncFrame, sizeof(syncFrame) - 1); // last byte was not read, only peek
    }
  }
}

void detectFlashingEnd() {
  if (syncFrameIndex < sizeof(syncFrame))
    return;
  static unsigned long lastFlashingActivity = 0;
  if (Serial.available()) {
    lastFlashingActivity = millis();
  } else if (millis() - lastFlashingActivity > 5000) { // wait so much. perhaps the tool makes the reset
    digitalWrite(LED_BUILTIN, LOW);
    Serial1.begin(BAUD);
    Serial1.resetESP(); // power cycle ESP after flashing, to avoid GPIO_STRAPPING register ESP bug
    syncFrameIndex = 0;
  }
}
#endif
