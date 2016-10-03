#pragma once

#include <stdint.h>

class MovingAverageFilter {
public:
    MovingAverageFilter(uint16_t length);

    uint16_t process(uint16_t value);

private:
    uint16_t* history;
    uint16_t length;
    uint16_t pos;
    uint32_t sum;
};
