#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "Bluetooth.h"
#include "State.h"

#include "FreeRTOS.h"
#include "task.h"

#define RESET_PIN PIN5_bm
#define KEY_PIN PIN4_bm

enum baudrate {
    BAUD_9600,
    BAUD_38400,
    BAUD_57600,
    BAUD_115200
};

static void uart_init(baudrate baud) {
    PORTD.OUTSET = PIN3_bm;
    PORTD.OUTCLR = PIN2_bm;

    PORTD.DIRSET = PIN3_bm;
    PORTD.DIRCLR = PIN2_bm;

    uint8_t bsel;
    uint8_t bscale;

    // select baud rate (calculated for 32MHz clock)
    switch (baud) {
        case BAUD_38400:
            bsel = 51;
            bscale = 0;
            break;

        case BAUD_57600:
            bsel = 135;
            bscale = 0b1110;
            break;

        case BAUD_115200:
            bsel = 131;
            bscale = 0b1101;
            break;

        case BAUD_9600:
        default:
            bsel = 207;
            bscale = 0;
    }

    USARTD0.BAUDCTRLA = bsel;
    USARTD0.BAUDCTRLB = bscale << USART_BSCALE0_bp;

    // 8 bit / 1 stop bit / no parity bits
    USARTD0.CTRLC = USART_CHSIZE_8BIT_gc;

    // no interrupts
    USARTD0.CTRLA = 0;

    // enable receive and transmit
    USARTD0.CTRLB = USART_TXEN_bm | USART_RXEN_bm;
}

static void uart_send_char(char c) {
    while(!(USARTD0_STATUS & USART_DREIF_bm));
    USARTD0_DATA = c;
}

static void uart_send_byte(uint8_t b) {
    while(!(USARTD0_STATUS & USART_DREIF_bm));
    USARTD0_DATA = b;
}

void Bluetooth::setup(const char* name) {
    uart_init(BAUD_38400);

    PORTD.DIRSET = RESET_PIN;
    PORTD.DIRSET = KEY_PIN;

    PORTD.OUTCLR = RESET_PIN;
    PORTD.OUTSET = KEY_PIN;

    _delay_ms(500);
    PORTD.OUTSET = RESET_PIN;

    _delay_ms(1000);
    send_str("AT+NAME=");
    send_str(name);
    send_str("\r\n");

    _delay_ms(500);
    send_str("AT+UART=115200,1,0\r\n");

    _delay_ms(1000);
    PORTD.OUTCLR = RESET_PIN;
    PORTD.OUTCLR = KEY_PIN;

    _delay_ms(200);
}

void Bluetooth::init() {
    uart_init(BAUD_115200);

    PORTD.DIRSET = RESET_PIN;
    PORTD.DIRSET = KEY_PIN;

    PORTD.OUTSET = RESET_PIN;
    PORTD.OUTCLR = KEY_PIN;
}

/* ECG/SpO2 Serial Protocol
 * 0b10101010: Frame Start
 * 0b0XXXXXXX: ECG Graph Value (7 most significant bits)
 * 0b0XXXXXXX: ECG Graph Value (7 least significant bits)
 * 0b0XXXXXXX: SpO2 Graph Value (7 most significant bits)
 * 0b0XXXXXXX: SpO2 Graph Value (7 least significant bits)
 * ---- One or more optional Fields ----
 * 0b11TTTXXX: Type (3 bits) + Value (3 most significant bits)
 * 0b0XXXXXXX: Value (7 least significant bits)
 * Types:
 * 000: Heart rate
 * 001: SpO2 value
 * [...] Unused
 * 111: Battery voltage
 */

void Bluetooth::run(void*) {
    for (;;) {
        vTaskDelay(8 / portTICK_PERIOD_MS);
        send_state();
    }
}

void Bluetooth::send_str(const char* text) {
    while (*text) {
        uart_send_char(*text++);
    }
}

void Bluetooth::send_int(int num) {
    char buf[10];
    itoa(num, buf, 10);
    Bluetooth::send_str(buf);
}

void Bluetooth::send_14bit_value(uint16_t value) {
    uart_send_byte((value >> 7) & 0b01111111);
    uart_send_byte(value & 0b01111111);
}

void Bluetooth::send_typed_10bit_value(uint8_t type, uint16_t value) {
    uart_send_byte(0b11000000 | ((type & 0b111) << 3) | (value >> 7) & 0b111);
    uart_send_byte(value & 0b01111111);
}

void Bluetooth::send_state() {
    uart_send_byte(0b10101010); // frame start

    State* state = State::get();
    taskENTER_CRITICAL();
    uint16_t ecgCurve = state->ecgCurve;
    uint16_t spo2Curve = state->spo2Curve;
    uint16_t heartRate = state->heartRate;
    uint16_t spo2Value = state->spo2Value;
    uint16_t heartRateUpdated = state->heartRateUpdated;
    if (heartRateUpdated) state->heartRateUpdated = false;
    uint16_t spo2ValueUpdated = state->spo2ValueUpdated;
    if (spo2ValueUpdated) state->spo2ValueUpdated = false;
    taskEXIT_CRITICAL();

    send_14bit_value(ecgCurve);
    send_14bit_value(spo2Curve);

    if (heartRateUpdated) {
        send_typed_10bit_value(0b000, heartRate);
    }

    if (spo2ValueUpdated) {
        send_typed_10bit_value(0b001, spo2Value);
    }
}
