#include <util/delay.h>
#include <avr/io.h>
#include "SpO2.h"
#include "MovingAverageFilter.h"
#include "Bluetooth.h"
#include "ExternalADC.h"
#include "Timer.h"

static MovingAverageFilter curveFilter(50);
static MovingAverageFilter spo2Filter(6);

void SpO2::init() {
    PORTC.DIRSET = PIN2_bm;
    PORTC.OUTCLR = PIN2_bm;
    PORTC.DIRSET = PIN3_bm;
    PORTC.OUTCLR = PIN3_bm;

    set_red_led(true);
    set_infrared_led(false);
}

void SpO2::run() {
    uint16_t value = ExternalADC::read();

    uint16_t avg = curveFilter.process(value);

    Bluetooth::send_ecg(value);
}

void SpO2::set_red_led(bool level) {
    if (level) { PORTC.OUTSET = PIN2_bm; }
    else { PORTC.OUTCLR = PIN2_bm; }
}

void SpO2::set_infrared_led(bool level) {
    if (level) { PORTC.OUTSET = PIN3_bm; }
    else { PORTC.OUTCLR = PIN3_bm; }
}
