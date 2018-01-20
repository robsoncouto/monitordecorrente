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

  //DDRB=1<<PINB5;
  sei();
  uart_init(UART_BAUD_SELECT(9600,16000000UL));
  init_hardware();
  lamp_on(LAMP_DES);
  int lamp=0;
  init_structure(&tr);
  while(1) {
    //get measurement and evaluates max and min values
    update_status(&tr);

    if(one_sec_passed()){
      clear_one_sec_flag();
      analyse_samples(&tr);
      evaluate_criteria(&tr);
      update_clock(&tr);
      debug(&tr);
      reset_values(&tr);
      if (lamp==0){
        lamp_on(LAMP_A);
        lamp=1;
      }else{
        lamp_off(LAMP_A);
        lamp=0;
      }

    }
  }

}
