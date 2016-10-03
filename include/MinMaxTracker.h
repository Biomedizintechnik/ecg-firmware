#pragma once

#include <stdint.h>

class MinMaxTracker {
public:
    MinMaxTracker(uint16_t period);
    void process(uint16_t value);
    uint16_t getMin();
    uint16_t getMax();
    uint16_t getPos();

private:
    uint16_t min;
    uint16_t max;
    uint16_t currMin;
    uint16_t currMax;
    uint16_t period;
    uint16_t pos;
};
