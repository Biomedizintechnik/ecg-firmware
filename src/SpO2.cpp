#include <util/delay.h>
#include <avr/io.h>
#include "SpO2.h"
#include "MovingAverageFilter.h"
#include "MinMaxTracker.h"
#include "State.h"
#include "ExternalADC.h"

#include "FreeRTOS.h"
#include "task.h"

#define MAX_PERIOD_SAMPLES 700

static MovingAverageFilter curveFilter(10);
static MovingAverageFilter redFilter(10);
static MovingAverageFilter infraredFilter(10);
static MovingAverageFilter dcFilter(MAX_PERIOD_SAMPLES);

static MinMaxTracker redMinMax(MAX_PERIOD_SAMPLES);
static MinMaxTracker infraredMinMax(MAX_PERIOD_SAMPLES);

void SpO2::init() {
    PORTC.DIRSET = PIN2_bm;
    PORTC.OUTCLR = PIN2_bm;
    PORTC.DIRSET = PIN3_bm;
    PORTC.OUTCLR = PIN3_bm;

    set_red_led(false);
    set_infrared_led(false);
}

void SpO2::run(void*) {
    for (;;) {
        set_red_led(true);

        vTaskDelay(2 / portTICK_PERIOD_MS);

        uint16_t redValue = ExternalADC::read();
        redMinMax.process(redFilter.process(redValue));

        set_infrared_led(true);

        vTaskDelay(2 / portTICK_PERIOD_MS);

        uint16_t infraredValue = ExternalADC::read();
        infraredMinMax.process(redFilter.process(infraredValue));

        if (redMinMax.getPos() == 0) {
            float redMax = redMinMax.getMax();
            float redMin = redMinMax.getMin();
            float infraredMax = infraredMinMax.getMax();
            float infraredMin = infraredMinMax.getMin();

            int16_t SpO2 = 0;
            if (redMin > 0 && infraredMin > 0) {
                float R = (redMax/redMin)/(infraredMax/infraredMin);
                SpO2 = R*100.0f; //253.0f-(166.0f*R);
            }

            if (SpO2 > 99) SpO2 = 99;
            else if (SpO2 < 60) SpO2 = 60;

            taskENTER_CRITICAL();
            State::get()->spo2Value = SpO2;
            State::get()->spo2ValueUpdated = true;
            taskEXIT_CRITICAL();
        }
        uint16_t dcValue = dcFilter.process(infraredValue);
        int16_t infraredAcValue = (int16_t)dcValue - (int16_t)infraredValue;
        uint16_t curveValue = curveFilter.process((uint16_t)(infraredAcValue*16 + 2048));
        if (curveValue > 4095) curveValue = 4095;
        else if (curveValue < 0) curveValue = 0;

        taskENTER_CRITICAL();
        State::get()->spo2Curve = curveValue;
        taskEXIT_CRITICAL();
    }
}

void SpO2::set_red_led(bool level) {
    if (level) { set_infrared_led(false); PORTC.OUTSET = PIN2_bm; }
    else { PORTC.OUTCLR = PIN2_bm; }
}

void SpO2::set_infrared_led(bool level) {
    if (level) { set_red_led(false); PORTC.OUTSET = PIN3_bm; }
    else { PORTC.OUTCLR = PIN3_bm; }
}
