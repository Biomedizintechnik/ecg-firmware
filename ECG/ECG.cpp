#include <util/delay.h>
#include <avr/io.h>
#include "SMAFilter.h"
#include "Bluetooth.h"
#include "ADC.h"
#include "util.h"

SMAFilter ecgFilter(50);
SMAFilter rateFilter(6);

static uint16_t encodeEcg(uint16_t value) {
	uint16_t result = 0b1000000000000000;
	result |= (value & 0b111110000000) << 1;
	result |= value & 0b1111111;
	return result;
}

static uint16_t encodePulse(uint8_t value) {
	uint16_t result = 0b1010000000000000;
	result |= (value & 0b10000000) << 2;
	result |= value & 0b01111111;
	return result;
}

int main(void) {
	// init pulse LED and SDN control
	PORTD.DIRSET = PIN1_bm;
	PORTD.OUTSET = PIN1_bm;
	PORTE.DIRSET = PIN0_bm;
	PORTE.OUTCLR = PIN0_bm;
		
	xtal_init();
	
	PORTE.OUTSET = PIN0_bm;
	//Bluetooth::setup("ECG 6");
	Bluetooth::init();
	PORTE.OUTCLR = PIN0_bm;
	
	ADC::init();

	unsigned long last_beat = 0;

    while(1) {
        _delay_ms(2);
		uint16_t value = ADC::read();
		Bluetooth::sendword(encodeEcg(value));
		
		int avg = ecgFilter.process(value);
		unsigned long time_since_last_beat = millis() - last_beat;
		
		if (time_since_last_beat > 200) {
			if (value > (1.5f * avg)) {
				last_beat = millis();
				int avg_time = rateFilter.process(time_since_last_beat);
				uint8_t pulse = 60000/avg_time;
				Bluetooth::sendword(encodePulse(pulse));
				PORTE.OUTSET = PIN0_bm;
			}
			else {
				PORTE.OUTCLR = PIN0_bm;
			}
		}
		else if (time_since_last_beat > 80) {
			PORTE.OUTCLR = PIN0_bm;
		}
    }
}
