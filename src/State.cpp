#include "State.h"

State State::instance;

void State::init() {
    instance.ecgCurve = 2048;
    instance.spo2Curve = 2048;
    instance.heartRate = 0;
    instance.spo2Value = 0;
    instance.heartRateUpdated = false;
}
