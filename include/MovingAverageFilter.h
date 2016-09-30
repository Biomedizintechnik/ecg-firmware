#pragma once

#include <stdint.h>

class MovingAverageFilter {
public:
    MovingAverageFilter(int length);
    //~MovingAverageFilter();

    int process(int value);

private:
    int* history;
    int length;
    int pos;
    int32_t sum;
};
