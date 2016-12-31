#include "MovingAverageFilter.h"
#include "FreeRTOS.h"
#include <stdlib.h>
#include <string.h>

MovingAverageFilter::MovingAverageFilter(uint16_t size) {
    this->size = size;
    this->length = 0;
    this->pos = 0;
    this->sum = 0;
    this->history = (uint16_t*)pvPortMalloc(sizeof(uint16_t)*size);
    memset(this->history, 0, sizeof(uint16_t)*size);
}

uint16_t MovingAverageFilter::process(uint16_t value) {
    this->sum -= this->history[this->pos];
    this->history[this->pos] = value;
    this->sum += value;
    this->pos = (this->pos + 1) % this->size;

    if (this->length < this->size) this->length++;

    return this->sum/this->length;
}
