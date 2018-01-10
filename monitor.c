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

volatile uint8_t ready;
//interrupção
ISR(TIMER1_OVF_vect){
  TCNT1H=0x0B;
  TCNT1L=0xDC;
  //PORTB^=(1<<PB4);
  ready=1;
}
uint8_t one_sec_passed(void){
  return ready;
}
void clear_one_sec_flag(void){
  ready=0;
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
  ready=0;
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
  /*
  typedef struct tranf_t {
    uint8_t potencia;
    uint8_t criterio_20;
    uint8_t criterio_40;
    uint32_t tempo_20[3];
    uint32_t tempo_40[3];
    uint16_t   media[3];
    uint32_t  soma[3];
  } transformador;
  */
  tr->potencia=get_transformer_size();
  for(int i=0;i<3;i++){
    tr->tempo_20[i]=0;
    tr->tempo_40[i]=0;
    tr->media[i]=0;
    tr->soma[i]=0;
  }
  switch (tr->potencia) {
    case 75:
      tr->criterio_20=CRTI_20_75;
      tr->criterio_40=CRTI_40_75;
      break;
    case 150:
      tr->criterio_20=CRTI_20_150;
      tr->criterio_40=CRTI_40_150;
      break;
    case 225:
      tr->criterio_20=CRTI_20_225;
      tr->criterio_40=CRTI_40_225;
      break;
    default:
    break;
  }

}
