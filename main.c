#define F_CPU 16000000UL
#include<avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "serial/uart.h"
#include <stdio.h>
#include "monitor.h"

transformador tr;

int main(void) {
stdout = &mystdout;
sei();
init_hardware();
init_structure(&tr);
  while(1) {
    while(!sample_time());
    update_status(&tr);

    if(one_sec_passed()){
      clear_one_sec_flag();
      analyse_samples(&tr);
      evaluate_criteria(&tr);
      update_clock(&tr);
      debug_messages(&tr);
      reset_values(&tr);
    }
  }
}
