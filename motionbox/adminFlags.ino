#include "common.h"

// MOTION_LED is connected to pin 13
//
#define PIN_MOTION_LED   13
#define PIN_MOTION_LED_OFF LOW
#define PIN_MOTION_LED_ON  HIGH

void refreshFlags() {
#ifdef DEBUG
  Serial.print("refreshFlags flags: "); Serial.println((int) state.flags, DEC);
#endif

  const bool ledOn = getMotionSensorState() && !getStealthState();
  digitalWrite(PIN_MOTION_LED, ledOn ? PIN_MOTION_LED_ON : PIN_MOTION_LED_OFF);
}

void checkDisableMotionSensorBlink() {
    // blink indicator when motion sensor is disabled
    if (getDisableMotionSensor() && !getStealthState()) {
        digitalWrite(PIN_MOTION_LED, digitalRead(PIN_MOTION_LED) == LOW ? HIGH : LOW);
    }
}

void initAdminFlags() {
  pinMode(PIN_MOTION_LED, OUTPUT); digitalWrite(PIN_MOTION_LED, PIN_MOTION_LED_OFF);

  setFlags(0);  // power all off by default  (clear bit means power off)
}

void setFlags(uint8_t flags) {
  if (state.flags == flags) return;
  state.flags = flags;
  refreshFlags();
}

bool getFlag(int flagBit) {
  if (flagBit < 0 || flagBit > 7) return false;
  return bitRead(state.flags, flagBit) != 0;
}

bool setFlag(int flagBit) {
  const uint8_t origFlags = state.flags;
  if (flagBit < 0 || flagBit > 7) return false;
  bitSet(state.flags, flagBit);
  if (origFlags != state.flags) { refreshFlags(); return true; }
  return false;
}

bool clearFlag(int flagBit) {
  const uint8_t origFlags = state.flags;
  if (flagBit < 0 || flagBit > 7) return false;
  bitClear(state.flags, flagBit);
  if (origFlags != state.flags) { refreshFlags(); return true; }
  return false;
}

bool flipFlag(int flagBit) {
  const uint8_t origFlags = state.flags;
  if (flagBit < 0 || flagBit > 7) return false;
  const bool currBit = bitRead(state.flags, flagBit) == 1;
  bitWrite(state.flags, flagBit, !currBit);
  refreshFlags();
  return true;
}

bool getMotionSensorState() { return getFlag(adminFlagMotionDetected); }
void setMotionSensorOn() { setFlag(adminFlagMotionDetected); }
void clearMotionSensorOn() { clearFlag(adminFlagMotionDetected); }
void toggleMotionSensor() { flipFlag(adminFlagMotionDetected); }

bool getDisableMotionSensor() { return getFlag(adminFlagDisableMotionSensor); }
void setDisableMotionSensor() { setFlag(adminFlagDisableMotionSensor); }
void clearDisableMotionSensor() { clearFlag(adminFlagDisableMotionSensor); }
void toggleDisableMotionSensor() { flipFlag(adminFlagDisableMotionSensor); }

bool getStealthState() { return getFlag(adminFlagStealth); }
void setStealthOn() { setFlag(adminFlagStealth); }
void clearStealthOn() { clearFlag(adminFlagStealth); }
void toggleStealthState() { flipFlag(adminFlagStealth); }

