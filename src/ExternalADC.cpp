#include <avr/io.h>
#include <avr/interrupt.h>
#include "twi_master_driver.h"
#include "ExternalADC.h"

#include "FreeRTOS.h"
#include "task.h"

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
    uint8_t cmd1[] = {0b01, 0b11000001, 0b11100011};

    TWI_MasterWrite(&twi, ADC_ADDR, cmd1, 3);

    vTaskDelay(8 / portTICK_PERIOD_MS);

    while (twi.status != TWIM_STATUS_READY) {}

    uint8_t cmd2[] = {0b00};
    TWI_MasterWriteRead(&twi, ADC_ADDR, cmd2, 1, 2);

    while (twi.status != TWIM_STATUS_READY) {}

    return (((uint16_t)twi.readData[0]) << 8) | (uint16_t)twi.readData[1];
}

ISR(TWIC_TWIM_vect) {
    TWI_MasterInterruptHandler(&twi);
}
