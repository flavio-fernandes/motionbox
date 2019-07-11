#include "common.h"

static const byte motionPin = A0;  /* purple wire */
static const int motionPinThreshold = 999;  /* analog reads 1024 when in motion */

void initMotionSensor() {
    pinMode(motionPin, INPUT);
    updateMotionTick1Sec();  // initial
}

// https://www.teachmemicro.com/pir-sensor-nodemcu-esp8266/
// https://www.teachmemicro.com/pir-motion-sensor/
void updateMotionTick1Sec() {
    static int initializationCountdown = 11;
    const int motionPinValue = analogRead(motionPin);
    const bool currMotionDetected =
        state.overrideMotionPinCountdownSeconds > 0 ||
        (!getDisableMotionSensor() && motionPinValue >= motionPinThreshold);
    const bool sameState = getMotionSensorState() == currMotionDetected;

    if (state.overrideMotionPinCountdownSeconds > 0) {
        --state.overrideMotionPinCountdownSeconds;
#ifdef DEBUG
            Serial.print("current overrideMotionPinCountdownSeconds: ");
            Serial.println(state.overrideMotionPinCountdownSeconds);
#endif
    }

    // during initializationCountdown, simply do nothing other than
    // mess with the lastChange counters
    if (initializationCountdown > 0) {
        state.motionInfo.lastChangedSec =
            state.motionInfo.lastChangedMin = --initializationCountdown;
        state.motionInfo.lastChangedHour = 255;
#ifdef DEBUG
            Serial.print("motion initializationCountdown: "); Serial.println(initializationCountdown);
#endif
        return;
    }

#ifdef DEBUG
    static int extraBump = 0;
    if (!sameState) extraBump = 0;
    if (!sameState || currMotionDetected || extraBump < 2) {
      Serial.print("Motion state: "); Serial.print(getMotionSensorState() ? "y" : "n");
      Serial.print(" currMotionDetected: "); Serial.print(currMotionDetected ? "y" : "n");
      Serial.print(" motionPinValue: "); Serial.print(motionPinValue);
      Serial.print(" extraBump: "); Serial.print(extraBump);
      Serial.println("");
    }
    ++extraBump;
#endif

    // Note: NOT using getMotionSensorOperState(), to keep bit set to what we need.
    //       Factoring getDisableMotionSensor() in is already taken care when 
    //       currMotionDetected is intantiated.
    if (sameState) {
        if (++state.motionInfo.lastChangedSec > 59) {
            state.motionInfo.lastChangedSec = 0;
            if (++state.motionInfo.lastChangedMin > 59) {
                state.motionInfo.lastChangedMin = 0;
                if (state.motionInfo.lastChangedHour < 254) ++state.motionInfo.lastChangedHour;
            }
        }
    } else {
        memset(&state.motionInfo, 0, sizeof(state.motionInfo));
        toggleMotionSensor();
        debugPrintMotionSensor();
    }
}

void debugPrintMotionSensor() {
#ifdef DEBUG
  Serial.print("Motion: ");
  Serial.print(getMotionSensorState() ? "y " : "n "); Serial.print(getDisableMotionSensor() ? "[DISABLED] " : "");
  Serial.print(state.motionInfo.lastChangedHour); Serial.print(":");
  Serial.print(state.motionInfo.lastChangedMin); Serial.print(":");
  Serial.print(state.motionInfo.lastChangedSec);
  Serial.println("");
#endif  // #ifdef DEBUG
}
