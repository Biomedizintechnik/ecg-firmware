#include "SMAFilter.h"
#include <stdlib.h>
#include <string.h>

SMAFilter::SMAFilter(int length) {
	this->length = length;
	this->pos = 0;
	this->sum = 0;
	this->history = (int*)malloc(sizeof(int)*length);
	memset(this->history, 0, sizeof(int)*length);
}

int SMAFilter::process(int value) {
	this->history[this->pos] = value;
	this->sum += value;
	
	this->pos = (this->pos + 1) % this->length;
	this->sum -= this->history[this->pos];
	
	return this->sum/(this->length-1);
}