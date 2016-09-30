#include "Clock.h"
#include <avr/io.h>
#include <avr/interrupt.h>

void Clock::init() {
    OSC_CTRL |= OSC_RC32MEN_bm; // enable 32MHz calibrated internal oscillator
    while(!(OSC_STATUS & OSC_RC32MRDY_bm));
    CCP = CCP_IOREG_gc; // trigger protection mechanism
    CLK.CTRL = CLK_SCLKSEL_RC32M_gc; // enable internal  32Mhz crystal

    OSC.CTRL&=(~OSC_RC2MEN_bm); // disable the default 2Mhz oscillator
    OSC.CTRL|= OSC_RC32KEN_bm; // enable 32kHz calibrated internal oscillator
    while (!(OSC.STATUS & OSC_RC32KRDY_bm));
    // enable auto-calibration for the 32Mhz oscillator
    DFLLRC32M.CTRL |= DFLL_ENABLE_bm;

    /*
    // select external osclillator
    OSC.XOSCCTRL = OSC_FRQRANGE_12TO16_gc | OSC_XOSCSEL_XTAL_16KCLK_gc;
    OSC.CTRL = OSC_XOSCEN_bm;
    while(!(OSC_STATUS & OSC_XOSCRDY_bm));
    CCP = CCP_IOREG_gc;
    CLK_CTRL = CLK_SCLKSEL_XOSC_gc;
    */
}
