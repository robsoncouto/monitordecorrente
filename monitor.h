#ifndef MONITOR_H
#define MONITOR_H

//connected to 74hc595
#define LAMP_ON  1
#define LAMP_A   2
#define LAMP_B   3
#define LAMP_C   4
#define LAMP_DES 5

#define HC595_DATA 3// PORTD
#define HC595_LATCH 4// PORTD
#define HC595_CLOCK 5// PORTD

#define SELECT_75  6//PORTD
#define SELECT_150 7//PORTD
#define SELECT_225 0//PORTB

#define SENS_A 0
#define SENS_B 1
#define SENS_C 2

#define hour2seconds(x) x*3600
#define min2seconds(x) x*60

static FILE mystdout = FDEV_SETUP_STREAM(uart0_putc, NULL,_FDEV_SETUP_WRITE);

void init_hardware(void);
void lamp_on(uint8_t lamp);
void lamp_off(uint8_t lamp);
uint16_t get_adc(uint8_t channel);
uint8_t get_transformer_size(void);

void clear_one_sec_flag(void);
uint8_t one_sec_passed(void);
float get_current(uint16_t difference);

typedef struct tranf_t {
  uint8_t potencia;
  float criterio_20;
  float criterio_40;
  uint32_t tempo_20[3];
  uint32_t tempo_40[3];
  uint16_t   media[3];
  uint16_t   max[3];
  uint16_t   min[3];
  uint16_t   delta[3];
  uint16_t   overload[3];
  unsigned long  soma[3];
  uint16_t   num_amostras;
} transformador;

void init_structure(transformador* tr);
void reset_values(transformador* tr);
void update_status(transformador* tr);
void update_clock(transformador* tr);
void evaluate_criteria(transformador* tr);
void analyse_samples(transformador* tr);
float get_voltage(uint8_t channel);

#define sqrt3 1.73205080757

#define CRTI_20_75   113.636*1.2//(75000/(220*3)*1,2
#define CRTI_20_150  227.272*1.2//(150000/(220*3)*1,2
#define CRTI_20_225  340.909*1.2//(225000/(220*3)*1,2

#define CRTI_40_75   113.636*1.4//(75000/(220*3)*1,4
#define CRTI_40_150  227.272*1.4//(150000/(220*3)*1,4
#define CRTI_40_225  340.909*1.4//(225000/(220*3)*1,4

#define gain 666.666    

void debug(transformador* tr);

#endif
