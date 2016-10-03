#pragma once

class SpO2 {
public:
    static void init();
    static void run(void*);

private:
    static void set_red_led(bool level);
    static void set_infrared_led(bool level);
};
