#pragma once

#include <stdint.h>

class Bluetooth {
public:
    static void init();
    static void setup(const char* name);
    static void run(void*);
    static void send_str(const char* text);
    static void send_int(int num);

private:
    static void send_state();
    static void send_14bit_value(uint16_t value);
    static void send_typed_10bit_value(uint8_t type, uint16_t value);
};
