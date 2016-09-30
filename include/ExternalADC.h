#pragma once

class ExternalADC {
public:
    static void init();
    static uint16_t read();
};
