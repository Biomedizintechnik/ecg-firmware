#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "util.h"
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

static void uart_sendchar(char c) {
	while(!(USARTD0_STATUS & USART_DREIF_bm));
	USARTD0_DATA = c;
}

static void uart_sendbyte(uint8_t b) {
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
	sendstr("AT+NAME=");
	sendstr(name);
	sendstr("\r\n");
	
	_delay_ms(500);
	sendstr("AT+UART=115200,1,0\r\n");
	
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

void Bluetooth::sendstr(const char* text) {
	while (*text) {
		uart_sendchar(*text++);
	}
}

void Bluetooth::sendint(int num) {
	char buf[10];
	itoa(num, buf, 10);
	Bluetooth::sendstr(buf);
}

void Bluetooth::sendword(uint16_t word) {
	uart_sendbyte((uint8_t)(word >> 8));
	uart_sendbyte((uint8_t)word);
}