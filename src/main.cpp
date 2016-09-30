#include "ECG.h"
#include "SpO2.h"
#include "Clock.h"
#include "Timer.h"
#include "Bluetooth.h"
#include "InternalADC.h"
#include "ExternalADC.h"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

int main(void) {
    // init SDN control
    PORTD.DIRSET = PIN1_bm;
    PORTD.OUTSET = PIN1_bm;

    Clock::init();
    Timer::init();

    // enable interrupts
    PMIC.CTRL |= PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
    sei();

    //Bluetooth::setup("ECG 10");
    Bluetooth::init();
    InternalADC::init();
    ExternalADC::init();

    ECG::init();
    SpO2::init();

    while(1) {
        //_delay_ms(2);
        //ECG::run();
        _delay_ms(10);
        SpO2::run();
    }
}
