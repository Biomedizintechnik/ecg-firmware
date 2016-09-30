#pragma once

class InternalADC {
public:
    static void init();
    static uint16_t read();
};
