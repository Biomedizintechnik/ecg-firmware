#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "Bluetooth.h"

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

void Bluetooth::send_frame(uint16_t frame) {
    uart_send_byte((uint8_t)(frame >> 8));
    uart_send_byte((uint8_t)frame);
}

void Bluetooth::send_ecg(uint16_t value) {
    uint16_t frame = 0b1000000000000000;
    frame |= (value & 0b111110000000) << 1;
    frame |= value & 0b1111111;
    send_frame(frame);
}

void Bluetooth::send_pulse(uint16_t value) {
    uint16_t frame = 0b1010000000000000;
    frame |= (value & 0b10000000) << 2;
    frame |= value & 0b01111111;
    send_frame(frame);
}
