#include <avr/io.h>
#include <avr/interrupt.h>
#include "State.h"
#include "ECG.h"
#include "SpO2.h"
#include "Clock.h"
#include "Bluetooth.h"
#include "InternalADC.h"
#include "ExternalADC.h"

#include "FreeRTOS.h"
#include "task.h"

int main(void) {
    // init SDN control
    PORTD.DIRSET = PIN1_bm;
    PORTD.OUTSET = PIN1_bm;

    Clock::init();

    // enable interrupts
    PMIC.CTRL |= PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
    sei();

    State::init();
    //Bluetooth::setup("ECG 10");
    Bluetooth::init();
    InternalADC::init();
    ExternalADC::init();

    ECG::init();
    SpO2::init();

    xTaskCreate(Bluetooth::run, "Bluetooth", configMINIMAL_STACK_SIZE,
                NULL, tskIDLE_PRIORITY + 1, NULL);

    xTaskCreate(ECG::run, "ECG", configMINIMAL_STACK_SIZE,
                NULL, tskIDLE_PRIORITY + 3, NULL);

    xTaskCreate(SpO2::run, "SpO2", configMINIMAL_STACK_SIZE,
                NULL, tskIDLE_PRIORITY + 2, NULL);

    vTaskStartScheduler();
    for(;;);
}
