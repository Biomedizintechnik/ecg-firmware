#pragma once

#include <stdint.h>

class State {
public:
    uint16_t ecgCurve;
    uint16_t spo2Curve;
    uint16_t heartRate;
    uint16_t spo2Value;
    bool heartRateUpdated;
    bool spo2ValueUpdated;

    static void init();
    static State* get() { return &instance; }

private:
    static State instance;
};
