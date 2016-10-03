#pragma once

class ECG {
public:
    static void init();
    static void run(void*);

private:
    static void set_pulse_led(bool level);
};
