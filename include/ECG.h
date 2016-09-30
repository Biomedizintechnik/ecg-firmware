#pragma once

class ECG {
public:
    static void init();
    static void run();

private:
    static void set_pulse_led(bool level);
};
