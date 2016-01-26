#pragma once

class Bluetooth {
public:
	static void init();
	static void setup(const char* name);
	static void sendstr(const char* text);
	static void sendint(int num);
	static void sendword(uint16_t word);
};