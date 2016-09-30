#include <util/delay.h>
#include <avr/io.h>
#include "ECG.h"
#include "MovingAverageFilter.h"
#include "Bluetooth.h"
#include "InternalADC.h"
#include "Timer.h"

static MovingAverageFilter ecgFilter(50);
static MovingAverageFilter rateFilter(6);

void ECG::init() {
    // init pulse led
    PORTE.DIRSET = PIN0_bm;
    PORTE.OUTCLR = PIN0_bm;

    set_pulse_led(false);
}

void ECG::run() {
    static unsigned long last_beat = 0;

    uint16_t value = InternalADC::read();
    Bluetooth::send_ecg(value);

    int avg = ecgFilter.process(value);
    unsigned long time_since_last_beat = Timer::millis() - last_beat;

    if (time_since_last_beat > 200) {
        if (value > (1.5f * avg)) {
            last_beat = Timer::millis();
            int avg_time = rateFilter.process(time_since_last_beat);
            uint8_t pulse = 60000/avg_time;
            Bluetooth::send_pulse(pulse);
            set_pulse_led(true);
        }
        else {
            set_pulse_led(false);
        }
    }
    else if (time_since_last_beat > 80) {
        set_pulse_led(false);
    }
}

void ECG::set_pulse_led(bool level) {
    if (level) {
        PORTE.OUTSET = PIN0_bm;
    }
    else {
        PORTE.OUTCLR = PIN0_bm;
    }
}
