#pragma once

#include <stdint.h>

class SMAFilter {
private:
	int* history;
	int length;
	int pos;
	int32_t sum;
	
public:
	SMAFilter(int length);
	int process(int value);
};