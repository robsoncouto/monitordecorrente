#include<avr/io.h>
#include<avr/interrupt.h>
#include <avr/sleep.h>
#include "serial/uart.h"
#include <stdio.h>
#include "monitor.h"

#define F_CPU 16000000UL

#include <util/delay.h>

uint8_t lamp_state;

uint16_t get_adc(uint8_t channel){
  ADMUX&=0xF0;
  ADMUX|=channel;
  ADCSRA |= (1<<ADSC);
  while(ADCSRA & (1<<ADSC));
  return (ADCW);
}

float get_voltage(uint8_t channel){
  float sample;
  sample=get_adc(channel);
  sample*=0.0048828125;//5V/1024
  return sample;
}


int rawAnalogReadWithSleep(uint8_t channel)
{
  //from get_adc
  ADMUX&=0xF0;
  ADMUX|=channel;
  ADCSRA |= (1<<ADSC);


 // Generate an interrupt when the conversion is finished
 ADCSRA |= _BV( ADIE );

 // Enable Noise Reduction Sleep Mode
 set_sleep_mode( SLEEP_MODE_ADC );
 sleep_enable();

 // Any interrupt will wake the processor including the millis interrupt so we have to...
 // Loop until the conversion is finished
 do
 {
   // The following line of code is only important on the second pass.  For the first pass it has no effect.
   // Ensure interrupts are enabled before sleeping
   sei();
   // Sleep (MUST be called immediately after sei)
   sleep_cpu();
   // Checking the conversion status has to be done with interrupts disabled to avoid a race condition
   // Disable interrupts so the while below is performed without interruption
   cli();
 }
 // Conversion finished?  If not, loop.
 while( ( (ADCSRA & (1<<ADSC)) != 0 ) );

 // No more sleeping
 sleep_disable();
 // Enable interrupts
 sei();

 // The Arduino core does not expect an interrupt when a conversion completes so turn interrupts off
 ADCSRA &= ~ _BV( ADIE );

 // Return the conversion result
 return( ADC );
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
  lamp_state|=0x1e&(1<<lamp);
  shift_out(lamp_state);
}

void lamp_off(uint8_t lamp){
  lamp_state&=~(0x1e&(1<<lamp));
  shift_out(lamp_state);
}
void shift_out(uint8_t data){
  PORTD&=~(1<<HC595_LATCH);
  PORTD&=~(1<<HC595_CLOCK);
  for(int i=0;i<8;i++){
    PORTD&=~(1<<HC595_CLOCK);
    _delay_ms(1);
    if(data&(1<<i)){
      PORTD|=(1<<HC595_DATA);
    }else{
      PORTD&=~(1<<HC595_DATA);
    }
    _delay_ms(1);
    PORTD|=(1<<HC595_CLOCK);
    _delay_ms(1);
  }
  PORTD|=(1<<HC595_DATA);
  PORTD|=(1<<HC595_LATCH);
  PORTD|=(1<<HC595_CLOCK);
}


void init_hardware(void){
  //ports
  DDRD=(1<<HC595_DATA)|(1<<HC595_LATCH)|(1<<HC595_CLOCK)|(0<<SELECT_75)|(0<<SELECT_150);
  DDRB=(0<<SELECT_225);
  //serial coms
  uart_init(UART_BAUD_SELECT(115200,16000000UL));
  //timers
  init_tim1();
  //adc
  ADMUX = (1<<REFS0)|(1<<MUX1)|(1<<MUX0);//PIN ADC7 used
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS0);
  sei();

  //pull ups
  //PORTD=(1<<SELECT_75);
  //PORTB=(1<<SELECT_150)|(1<<SELECT_225);

  //lamp_on(LAMP_ON);
  ready=0;
  shift_out(0);
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
    tr->max[i]=0;
    tr->min[i]=0xFF;
    tr->num_amostras=0;
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

void update_status(transformador* tr){
  uint16_t sample=0;
  //SENSOR A
  sample=get_adc(SENS_A);
  if(sample>tr->max[SENS_A]){
    tr->max[SENS_A]=sample;
  }
  if(sample<tr->min[SENS_A]){
    tr->min[SENS_A]=sample;
  }
  tr->soma[SENS_A]+=sample;


  //SENSOR B
  sample=get_adc(SENS_B);
  if(sample>tr->max[SENS_B]){
    tr->max[SENS_B]=sample;
  }
  if(sample<tr->min[SENS_B]){
    tr->min[SENS_B]=sample;
  }
  tr->soma[SENS_B]+=sample;

  //SENSOR C
  sample=get_adc(SENS_C);
  if(sample>tr->max[SENS_C]){
    tr->max[SENS_C]=sample;
  }
  if(sample<tr->min[SENS_C]){
    tr->min[SENS_C]=sample;
  }
  tr->soma[SENS_C]+=sample;


  tr->num_amostras++;
}

void analyse_samples(transformador* tr){
  tr->media[SENS_A]= tr->soma[SENS_A]/tr->num_amostras;
  tr->delta[SENS_A]=tr->max[SENS_A]-tr->media[SENS_A];

  tr->media[SENS_B]= tr->soma[SENS_B]/tr->num_amostras;
  tr->delta[SENS_B]=tr->max[SENS_B]-tr->media[SENS_B];

  tr->media[SENS_C]= tr->soma[SENS_C]/tr->num_amostras;
  tr->delta[SENS_C]=tr->max[SENS_C]-tr->media[SENS_C];

}

float get_current(uint16_t difference){
  return (float) difference*gain*(0.00488281);
}

void reset_values(transformador* tr){
  for(int i=0;i<3;i++){
    tr->media[i]=0;
    tr->soma[i]=0;
    tr->min[i]=0xFF;
    tr->max[i]=0;
  }
  tr->num_amostras=0;
}

void evaluate_criteria(transformador* tr){
      for(int i=0;i<3;i++){
      if( get_current(tr->delta[i]) < 0.0){
        printf("error!\n" );
        return;
      }

      if(get_current(tr->delta[i])>tr->criterio_40){
        if(tr->overload[i]==0){
          tr->overload[i]=40;
          tr->tempo_40[i]=min2seconds(2);
        }
      }else if(get_current(tr->delta[i])>tr->criterio_20){
        if(tr->overload[i]==0){
          tr->overload[i]=20;
          tr->tempo_20[i]=min2seconds(4);
        }
      }else {
        tr->overload[i]=0;
        tr->tempo_20[i]=0;
        tr->tempo_40[i]=0;
      }
    }
}
void turn_on_lamps(uint8_t i){
    //static uint8_t lamps=0; 
    //printf("lamp - :%d", i);
    //lamps|=(1<<i);
    shift_out(i);
    
}
void update_clock(transformador* tr){
  for(int i=0;i<3;i++){
    if(tr->overload[i]==20){
      if(tr->tempo_20[i]>0){
        tr->tempo_20[i]--;
        if(tr->tempo_20[i]==0){
          turn_on_lamps(i);
        }
      }
    } else if(tr->overload[i]==40){
      if(tr->tempo_40[i]>0){
        tr->tempo_40[i]--;
        if(tr->tempo_40[i]==0){
          turn_on_lamps(i);
        }
      }
    }
  }
}

void debug(transformador* tr){
  printf("Em 1 segundo:\n");
  printf("soma : A:%lu B:%lu C:%lu\n", tr->soma[SENS_A], tr->soma[SENS_B], tr->soma[SENS_C]);
  printf("max   : A:%5.2f B:%5.2f C:%5.2f\n", (0.00488281)* (float) tr->max[SENS_A], (0.00488281)* (float) tr->max[SENS_B], (0.00488281)* (float) tr->max[SENS_C]);
  printf("min   : A:%5.2f B:%5.2f C:%5.2f\n", (0.00488281)* (float) tr->min[SENS_A], (0.00488281)* (float) tr->min[SENS_B], (0.00488281)* (float) tr->min[SENS_C]);
  printf("media : A:%5.2f B:%5.2f C:%5.2f\n", (0.00488281)* (float) tr->media[SENS_A], (0.00488281)* (float) tr->media[SENS_B], (0.00488281)* (float) tr->media[SENS_C]);
  printf("amostras : %u\n", tr->num_amostras);
  printf("delta : A:%5.2f B:%5.2f C:%5.2f\n", (0.00488281)* (float) tr->delta[SENS_A], (0.00488281)* (float) tr->delta[SENS_B], (0.00488281)* (float) tr->delta[SENS_C]);
  printf("corrente : A:%f B:%f C:%f\n", get_current(tr->delta[SENS_A]), get_current(tr->delta[SENS_B]), get_current(tr->delta[SENS_C]));
  printf("criterio 20 : %.2f\n",tr->criterio_20);
  printf("criterio 40 : %.2f\n",tr->criterio_40);

  printf("sobrecarga : A:%d B:%d C:%d\n",tr->overload[SENS_A], tr->overload[SENS_B], tr->overload[SENS_C]);

  printf("cronometro 20 : A:%d B:%d C:%d\n",tr->tempo_20[SENS_A], tr->tempo_20[SENS_B], tr->tempo_20[SENS_C]);
  printf("cronometro 40 : A:%d B:%d C:%d\n",tr->tempo_40[SENS_A], tr->tempo_40[SENS_B], tr->tempo_40[SENS_C]);

}
