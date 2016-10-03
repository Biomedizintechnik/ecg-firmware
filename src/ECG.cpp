#include <util/delay.h>
#include <avr/io.h>
#include "ECG.h"
#include "MovingAverageFilter.h"
#include "State.h"
#include "InternalADC.h"

#include "FreeRTOS.h"
#include "task.h"

#define SAMPLE_RATE 250

static MovingAverageFilter ecgFilter(50);
static MovingAverageFilter rateFilter(6);

void ECG::init() {
    // init pulse led
    PORTE.DIRSET = PIN0_bm;
    PORTE.OUTCLR = PIN0_bm;

    for (int i = 0; i < 6; i++) rateFilter.process(800);
    for (int i = 0; i < 50; i++) ecgFilter.process(4095);
    set_pulse_led(false);
}

void ECG::run(void*) {
    static unsigned long time_since_last_beat = 800;

    for (;;) {
        uint16_t value = InternalADC::read();

        taskENTER_CRITICAL();
        State::get()->ecgCurve = value;
        taskEXIT_CRITICAL();

        int avg = ecgFilter.process(value);

        if (time_since_last_beat > 200) {
            if (value > (1.5f * avg)) {
                int avg_time = rateFilter.process(time_since_last_beat);
                uint8_t pulse = 60000/avg_time;

                taskENTER_CRITICAL();
                State::get()->heartRate = pulse;
                State::get()->heartRateUpdated = true;
                taskEXIT_CRITICAL();

                set_pulse_led(true);
                time_since_last_beat = 0;
            }
            else {
                set_pulse_led(false);
            }
        }
        else if (time_since_last_beat > 80) {
            set_pulse_led(false);
        }

        time_since_last_beat += 1000 / SAMPLE_RATE;
        vTaskDelay(1000 / SAMPLE_RATE / portTICK_PERIOD_MS);
    }

}

void ECG::set_pulse_led(bool level) {
    if (level) { PORTE.OUTSET = PIN0_bm; }
    else { PORTE.OUTCLR = PIN0_bm; }
}
