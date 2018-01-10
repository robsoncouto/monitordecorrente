#define F_CPU 16000000UL


#include<avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "monitor.h"
#include "serial/uart.h"
#include <stdio.h>

/*
criterio1 e 2
tempo
*/
transformador tr;

int main(void) {
  static FILE mystdout = FDEV_SETUP_STREAM(uart0_putc, NULL,_FDEV_SETUP_WRITE);
	stdout = &mystdout;
  DDRB=1<<PINB5;
  //DDRD=(1<<PINB0)|(1<<PINB1);
  sei();
  uart_init(UART_BAUD_SELECT(9600,16000000UL));
  init_hardware();
  lamp_on(LAMP_DES);
  init_structure(&tr);

  while(1) {
    //printf("size of the transformador: %d\n",tr.potencia);
    //printf("criterio de 20: %d\n",tr.criterio_20);
    //printf("criterio de 40: %d\n",tr.criterio_40);
    //printf("sensor A: %d\n", get_adc(SENS_A));
    //printf("sensor B: %d\n", get_adc(SENS_B));
    //printf("sensor C: %d\n", get_adc(SENS_C));

    update_status(&tr);


    if(one_sec_passed()){
      printf("Hi there\n");
      clear_one_sec_flag();
      tr.media[SENS_A]= tr.soma[SENS_A]/tr.num_amostras;
      printf("media : %u\n", tr.media[SENS_A]);
      printf("soma : %lu\n", tr.soma[SENS_A]);
      printf("max : %u\n", tr.max[SENS_A]);
      printf("min : %u\n", tr.min[SENS_A]);
      printf("amostras : %u\n", tr.num_amostras);
      printf("diff : %u\n", tr.max[SENS_A]-tr.media[SENS_A]);
      reset_values(&tr);

      printf("sensor A: %d\n", get_adc(SENS_A));
      printf("sensor B: %d\n", get_adc(SENS_B));
      printf("sensor C: %d\n", get_adc(SENS_C));


    }
    //_delay_ms(1000);
    //uart_puts("hello world\n");
    //printf("Hi there\n");
  }

}
