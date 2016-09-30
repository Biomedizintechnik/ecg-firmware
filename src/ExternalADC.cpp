#include <avr/io.h>
#include <avr/interrupt.h>
#include "twi_master_driver.h"
#include "ExternalADC.h"

#define ADC_ADDR 0b1001000
#define F_TWI 400000

TWI_Master_t twi;

void ExternalADC::init() {
    TWI_MasterInit(&twi,
                   &TWIC,
                   TWI_MASTER_INTLVL_LO_gc,
                   TWI_BAUD(F_CPU, F_TWI));
}

uint16_t ExternalADC::read() {
    uint8_t cmd1[] = {0b01, 0b01000001, 0b11100011};

    TWI_MasterWrite(&twi, ADC_ADDR, cmd1, 3);

    while (twi.status != TWIM_STATUS_READY) {}

    _delay_ms(8);

    uint8_t cmd2[] = {0b00};
    TWI_MasterWrite(&twi, ADC_ADDR, cmd2, 1);

    while (twi.status != TWIM_STATUS_READY) {}

    TWI_MasterRead(&twi, ADC_ADDR, 2);

    while (twi.status != TWIM_STATUS_READY) {}

    return (((uint16_t)twi.readData[0]) << 8) | (uint16_t)twi.readData[1];
}

ISR(TWIC_TWIM_vect) {
    TWI_MasterInterruptHandler(&twi);
}
