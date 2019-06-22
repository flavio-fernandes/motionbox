#include "common.h"

#include "TickerScheduler.h"

// evernote uuid: 839B22A0-3FDE-4C65-8B75-DCE6BA1788D8
// Board: Adafruit HUZZAH ESP8266
// programming speed 921600
// serial speed: Serial.begin(115200);

// FWDs
static void tickerFun100ms();
static void tickerFun250ms();
static void tickerFun1sec();
static void tickerFun10min();
const uint8_t numberOfTickers = 4;

TickerScheduler ts(numberOfTickers);
State state;

// ----------------------------------------

void initTicker() {
  // ref: https://github.com/Toshik/TickerScheduler/blob/master/TickerScheduler.h
  //      https://buildmedia.readthedocs.org/media/pdf/arduino-esp8266/docs_to_readthedocs/arduino-esp8266.pdf
  //      https://github.com/esp8266/Arduino/tree/master/libraries
  // bool add(uint8_t i, uint32_t period, tscallback_t, void *, boolean shouldFireNow = false);
  const uint32_t oneSec = 1000;
  const uint32_t tenMin = oneSec * 60 * 10;

  ts.add(0, 100, [&](void *) { tickerFun100ms(); }, nullptr, false);
  ts.add(1, 250, [&](void *) { tickerFun250ms(); }, nullptr, false);
  ts.add(2, oneSec, [&](void *) { tickerFun1sec(); }, nullptr, false);
  ts.add(3, tenMin, [&](void *) { tickerFun10min(); }, nullptr, false);
}

void initGlobals() {
    memset(&state, 0, sizeof(state));

    state.initIsDone = false;
}

void setup() {
#ifdef DEBUG
    Serial.begin(115200); // (921600);
#endif

    // stage 1
    initGlobals();

    // stage 2
    initAdminFlags();   // should be earlier for led status
    initHeartBeat();

    // stage 3
    initMotionSensor();
    initMyMqtt();
    initTicker();

#ifdef DEBUG
    Serial.println("Init finished");
    Serial.print("sizeof(int) == "); Serial.println(sizeof(int), DEC);
    Serial.print("sizeof(unsigned long) == "); Serial.println(sizeof(unsigned long), DEC);
    Serial.print("sizeof(unsigned long long) == "); Serial.println(sizeof(unsigned long long), DEC);
#endif
    state.initIsDone = true;

}

static void tickerFun100ms() {
  heartBeatTick();
}

static void tickerFun250ms() {
  checkDisableMotionSensorBlink();
}

static void tickerFun1sec() {
  mqtt1SecTick();
  updateMotionTick1Sec();
  // debugPrintMotionSensor();
}

static void tickerFun10min() {
#ifdef DEBUG
    Serial.println("10 minutes tick");
#endif

  mqtt10MinTick();
}

void loop() {
  myMqttLoop();
  ts.update();
}
