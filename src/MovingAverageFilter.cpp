#include "MovingAverageFilter.h"
#include "FreeRTOS.h"
#include <stdlib.h>
#include <string.h>

MovingAverageFilter::MovingAverageFilter(uint16_t length) {
	this->length = length;
	this->pos = 0;
	this->sum = 0;
	this->history = (uint16_t*)pvPortMalloc(sizeof(uint16_t)*length);
	memset(this->history, 0, sizeof(uint16_t)*length);
}

uint16_t MovingAverageFilter::process(uint16_t value) {
	this->history[this->pos] = value;
	this->sum += value;

	this->pos = (this->pos + 1) % this->length;
	this->sum -= this->history[this->pos];

	return this->sum/(this->length-1);
}
