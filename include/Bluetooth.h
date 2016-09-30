#pragma once

#include <stdint.h>

class Bluetooth {
public:
	static void init();
	static void setup(const char* name);
	static void send_str(const char* text);
	static void send_int(int num);
	static void send_frame(uint16_t frame);
	static void send_ecg(uint16_t value);
	static void send_pulse(uint16_t value);
};
