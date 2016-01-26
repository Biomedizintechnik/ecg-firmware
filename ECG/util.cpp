#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "util.h"

volatile unsigned long millis_count = 0;
volatile unsigned long seconds_count = 0;

void xtal_init() {
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

	// enable interrupts
	PMIC.CTRL |= PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
	sei();
}

ISR(TCC0_OVF_vect) {
	++millis_count;
	EVSYS.STROBE = 0xF;
}

ISR(TCC1_OVF_vect) {
	++seconds_count;
}

unsigned long millis(void) {
	uint8_t oldSREG = SREG;
	cli();
	unsigned long result = seconds_count*1000UL + TCC1.CNT;
	SREG = oldSREG;
	return result + millis_count;
}
