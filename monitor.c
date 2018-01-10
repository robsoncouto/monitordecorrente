#include<avr/io.h>
#include<avr/interrupt.h>
#include "serial/uart.h"
#include "monitor.h"


uint16_t get_adc(uint8_t channel){
  ADMUX&=0xF0;
  ADMUX|=channel;
  ADCSRA |= (1<<ADSC);
  while(ADCSRA & (1<<ADSC));
  return (ADC);
}

//configurar para 1 segundo
void init_tim1(void){  //timer 1
  TCCR1A=0x00;//Normal mode
  TCNT1H=0x0B;//C1;
  TCNT1L=0xDC;//7F;
  TCCR1B=(1<<CS12); //256 prescaller
  TIMSK1=(1<<TOIE1);

}

//interrupção
ISR(TIMER1_OVF_vect){
  TCNT1H=0x0B;
  TCNT1L=0xDC;
  //PORTB^=(1<<PB4);
}

void lamp_on(uint8_t lamp){
  PORTD|=(1<<lamp);
}

void lamp_off(uint8_t lamp){
  PORTD&=~(1<<lamp);
}

void init_hardware(void){
  //ports
  DDRD=(1<<LAMP_A)|(1<<LAMP_B)|(1<<LAMP_C)|(1<<LAMP_ON)|(1<<LAMP_DES)|(0<<SELECT_75);
  DDRB=(0<<SELECT_150)|(0<<SELECT_225)|(1<<4);
  //serial coms
  uart_init(UART_BAUD_SELECT(9600,16000000UL));
  //timers
  init_tim1();
  //adc
  ADMUX = (1<<REFS0)|(1<<MUX1)|(1<<MUX0);//PIN ADC7 used
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS0);
  sei();

  //pull ups
  PORTD=(1<<SELECT_75);
  PORTB=(1<<SELECT_150)|(1<<SELECT_225);

  lamp_on(LAMP_ON);
}

uint8_t get_transformer_size(void){
  if ( ~(PIND&(1<<SELECT_75)) && ((PINB&(1<<SELECT_150))) && ((PINB&(1<<SELECT_225)))){
    return 75;
  }
  if ((PIND&(1<<SELECT_75)) && ~(PINB&(1<<SELECT_150)) && ((PINB&(1<<SELECT_225)))){
    return 150;
  }
  if ((PIND&(1<<SELECT_75)) && ((PINB&(1<<SELECT_150))) && ~(PINB&(1<<SELECT_225))){
    return 225;
  }else{
    return 0;
  }
}

void init_structure(transformador* tr){
  for(int i=0;i<3;i++){
    tr->tempo_20[i]=0;
    tr->tempo_40[i]=0;
  }

}