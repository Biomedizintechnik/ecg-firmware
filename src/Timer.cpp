#include "Timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

volatile static unsigned long millis_count = 0;
volatile static unsigned long seconds_count = 0;

ISR(TCC0_OVF_vect) {
	++millis_count;
	EVSYS.STROBE = 0xF;
}

ISR(TCC1_OVF_vect) {
	++seconds_count;
}

void Timer::init() {
    // enable timer
    TCC0.CTRLA = TC_CLKSEL_DIV8_gc;
    TCC0.PERBUF = 4000;
    TCC0.CTRLB = ( TCC0.CTRLB & ~TC0_WGMODE_gm ) | TC_WGMODE_NORMAL_gc;
    // EVSYS.CH0MUX = EVSYS_CHMUX_TCF0_OVF_gc;
    TCC0.INTCTRLA = TC_OVFINTLVL_HI_gc;

    TCC1.CTRLA = TC_CLKSEL_EVCH0_gc;
    TCC1.PERBUF = 1000;
    TCC1.CTRLB = ( TCC1.CTRLB & ~TC1_WGMODE_gm ) | TC_WGMODE_NORMAL_gc;
    TCC1.CTRLD = TC_EVACT_UPDOWN_gc | TC1_EVDLY_bm;
    TCC1.INTCTRLA = TC_OVFINTLVL_HI_gc;
}

unsigned long Timer::millis() {
    uint8_t oldSREG = SREG;
    cli();
    unsigned long result = seconds_count*1000UL + TCC1.CNT;
    SREG = oldSREG;
    return result + millis_count;
}
