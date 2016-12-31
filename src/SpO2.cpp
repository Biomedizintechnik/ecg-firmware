#include <util/delay.h>
#include <avr/io.h>
#include "SpO2.h"
#include "MovingAverageFilter.h"
#include "MinMaxTracker.h"
#include "State.h"
#include "ExternalADC.h"

#include "FreeRTOS.h"
#include "task.h"

#define MAX_PERIOD_SAMPLES 250

static MovingAverageFilter curveFilter(10);
static MovingAverageFilter redFilter(5);
static MovingAverageFilter infraredFilter(5);
static MovingAverageFilter infraredDcFilter(MAX_PERIOD_SAMPLES);

static MovingAverageFilter redAmplitudeFilter(7);
static MovingAverageFilter infraredAmplitudeFilter(7);

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
        uint16_t infraredDcValue = infraredDcFilter.process(infraredValue);
        infraredMinMax.process(infraredFilter.process(infraredValue));

        if (redMinMax.getPos() == 0) {
            uint16_t redMax = redMinMax.getMax();
            uint16_t redMin = redMinMax.getMin();
            uint16_t infraredMax = infraredMinMax.getMax();
            uint16_t infraredMin = infraredMinMax.getMin();

            uint16_t redAmplitude = redAmplitudeFilter.process(redMax-redMin);
            uint16_t infraredAmplitude = infraredAmplitudeFilter.process(infraredMax-infraredMin);

            uint16_t SpO2 = 0;
            if (infraredAmplitude != 0) {
                double R = (double)(redAmplitude)/(double)(infraredAmplitude);
                SpO2 = (uint16_t)((43.96 * (R*R) - 106.9 * R + 147.9) + 0.5);
            }

            if (SpO2 > 100) SpO2 = 100;
            else if (SpO2 < 80) SpO2 = 0;

            taskENTER_CRITICAL();
            State::get()->spo2Value = SpO2;
            State::get()->spo2ValueUpdated = true;
            taskEXIT_CRITICAL();
        }

        int16_t infraredAcValue = (int16_t)infraredDcValue - (int16_t)infraredValue;
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
