#define F_CPU 16000000UL


#include<avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
//#include "monitor.h"
#include "serial/uart.h"

/*
criterio1 e 2
tempo 
*/

int main(void) {
  DDRB=1<<PINB5;
  DDRD=(1<<PINB0)|(1<<PINB1);
  sei();
  uart_init(UART_BAUD_SELECT(9600,16000000UL));
  while(1) {
    PORTB^=(1<<PINB5);
    _delay_ms(1000);
    uart_puts(" hello world\n");
  }

}
