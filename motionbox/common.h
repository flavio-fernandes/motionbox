#ifndef _COMMON_H

#define _COMMON_H

#include <inttypes.h>

// FIXME: turn debug off
//#define DEBUG 1

// FWD decls... adminFlags
void initAdminFlags();
void setFlags(uint8_t flags);
bool getFlag(int flagBit);
bool setFlag(int flagBit);
bool clearFlag(int flagBit);
bool flipFlag(int flagBit);

typedef enum {
    adminFlagMotionDetected = 0,    // oper
    adminFlagDisableMotionSensor,   // admin
    adminFlagStealth,               // admin
    adminFlagCount = 8
} AdminFlag;

// Wrapper to flag functions above
bool getMotionSensorState();  // also see getMotionSensorOperState()
void setMotionSensorOn();
void clearMotionSensorOn();
void toggleMotionSensor();

void checkDisableMotionSensorBlink();  // blink indicator when motion sensor is disabled
bool getDisableMotionSensor();  // also see getMotionSensorOperState()
void setDisableMotionSensor();
void clearDisableMotionSensor();
void toggleDisableMotionSensor();

bool getMotionSensorOperState() { return getMotionSensorState() && !getDisableMotionSensor(); }

bool getStealthState();
void setStealthOn();
void clearStealthOn();
void toggleStealthState();

// FWD decls... motionSensor
struct MotionInfo {
  uint8_t lastChangedSec;     // 0 to 60
  uint8_t lastChangedMin;     // 0 to 60
  uint8_t lastChangedHour;    // 0 to 255
};
void initMotionSensor();
void updateMotionTick1Sec();
void debugPrintMotionSensor();

// FWD decls...heartBeat
void initHeartBeat();
void heartBeatTick();

// FWS decls... mqtt_client
void initMyMqtt();
void myMqttLoop();
void mqtt1SecTick();
void mqtt10MinTick();
void sendOperState();
void sendNoMotionState();

typedef struct {
    bool initIsDone;

    uint8_t flags;   // see AdminFlag enum

    // This counter will fake a motion pin read to be HIGH
    int overrideMotionPinCountdownSeconds;

    MotionInfo motionInfo;
} State;

extern State state;

#endif // _COMMON_H
