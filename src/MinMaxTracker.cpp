#include "MinMaxTracker.h"

MinMaxTracker::MinMaxTracker(uint16_t period) {
    this->min = 0;
    this->max = 0;
    this->currMin = 0;
    this->currMax = 0;
    this->pos = 0;
    this->period = period;
}

void MinMaxTracker::process(uint16_t value) {
    if (value > currMax) currMax = value;
    if (value < currMin) currMin = value;

    if (pos < period) {
        pos += 1;
    }
    else {
        max = currMax;
        min = currMin;
        currMax = 0;
        currMin = UINT16_MAX;
        pos = 0;
    }
}

uint16_t MinMaxTracker::getMax() {
    return max;
}

uint16_t MinMaxTracker::getMin() {
    return min;
}

uint16_t MinMaxTracker::getPos() {
    return pos;
}
