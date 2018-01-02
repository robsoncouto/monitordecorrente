#define F_CPU 16000000UL


#include<avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void init_tim1(void);

int main(void) {
  DDRB=1<<PINB5;

  while(1) {
    PORTB^=(1<<PINB5);
    _delay_ms(1000);
  }

}
