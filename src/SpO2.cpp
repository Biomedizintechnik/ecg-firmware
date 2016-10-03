#include <util/delay.h>
#include <avr/io.h>
#include "SpO2.h"
#include "MovingAverageFilter.h"
#include "State.h"
#include "ExternalADC.h"

#include "FreeRTOS.h"
#include "task.h"

static MovingAverageFilter curveFilter(10);
static MovingAverageFilter dcFilter(300);

void SpO2::init() {
    PORTC.DIRSET = PIN2_bm;
    PORTC.OUTCLR = PIN2_bm;
    PORTC.DIRSET = PIN3_bm;
    PORTC.OUTCLR = PIN3_bm;

    set_red_led(false);
    set_infrared_led(true);
}

void SpO2::run(void*) {
    for (;;) {
        uint16_t value = ExternalADC::read();
        uint16_t dcValue = dcFilter.process(value);
        int16_t acValue = ((int16_t)dcValue - (int16_t)value)*16;
        uint16_t curveValue = curveFilter.process((uint16_t)(acValue + 2048));

        if (curveValue > 4095) curveValue = 4095;
        else if (curveValue < 0) curveValue = 0;

        taskENTER_CRITICAL();
        State::get()->spo2Curve = curveValue > 4095 ? 4095 : curveValue;
        taskEXIT_CRITICAL();
    }
}

void SpO2::set_red_led(bool level) {
    if (level) { PORTC.OUTSET = PIN2_bm; }
    else { PORTC.OUTCLR = PIN2_bm; }
}

void SpO2::set_infrared_led(bool level) {
    if (level) { PORTC.OUTSET = PIN3_bm; }
    else { PORTC.OUTCLR = PIN3_bm; }
}
