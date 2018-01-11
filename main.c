#define F_CPU 16000000UL


#include<avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "serial/uart.h"
#include <stdio.h>
#include "monitor.h"
/*
TODO
calculo desbalanceamento
criterios

*/


transformador tr;

int main(void) {
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

    //get measurement and evaluates max and min values
    update_status(&tr);

    if(one_sec_passed()){
      //printf("Em 1 segundo:\n");
      clear_one_sec_flag();
      //printf("media : %.2f\n", teste);
      //printf("soma : %lu\n", tr.soma[SENS_A]);
      //printf("max : %.2f\n", (0.01953125)* (float) tr.max[SENS_A]);
      //printf("min : %.2f\n", (0.01953125)* (float) tr.min[SENS_A]);
      //printf("amostras : %u\n", tr.num_amostras);
      //printf("diferenca : %u\n", tr.delta[SENS_A]);
      analyse_samples(&tr);
      evaluate_criteria(&tr);
      update_clock(&tr);
      //printf("corrente : %.2f\n", get_current(tr.delta[SENS_A]));
      //printf("criterio 20 : %d\n",tr.criterio_20);
      //printf("criterio 40 : %d\n",tr.criterio_40);

      //printf("cronometro 20 : %d\n",tr.tempo_20[SENS_A]);
      //printf("cronometro 40 : %d\n",tr.tempo_40[SENS_A]);
      debug(&tr);
      printf("\n\n\n");
      reset_values(&tr);



      //printf("sensor A: %d\n", get_adc(SENS_A));
      //printf("sensor B: %d\n", get_adc(SENS_B));
      //printf("sensor C: %d\n", get_adc(SENS_C));


    }
    //_delay_ms(1000);
    //uart_puts("hello world\n");
    //printf("Hi there\n");
  }

}
