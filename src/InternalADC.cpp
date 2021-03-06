#include <avr/io.h>
#include "InternalADC.h"

void InternalADC::init() {
    PORTA.DIRCLR = PIN1_bm;

    ADCA.CTRLA |= ADC_ENABLE_bm;
    ADCA.CTRLB = ADC_RESOLUTION_12BIT_gc;
    ADCA.REFCTRL = ADC_REFSEL_AREFA_gc;
    ADCA.PRESCALER = ADC_PRESCALER_DIV16_gc;
    ADCA.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
    ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN1_gc;
}

uint16_t InternalADC::read() {
    ADCA.CH0.CTRL |= ADC_CH_START_bm;
    while(!ADCA.CH0.INTFLAGS);
    return ADCA.CH0RES;
}
