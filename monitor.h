#ifndef MONITOR_H
#define MONITOR_H

//PORT D
#define LAMP_ON  2
#define LAMP_A   3
#define LAMP_B   4
#define LAMP_C   5
#define LAMP_DES 6


#define SELECT_75  7//PORTD
#define SELECT_150 0//PORTB
#define SELECT_225 1//PORTB

#define SENS_A 0
#define SENS_B 1
#define SENS_C 2

void init_hardware(void);
void lamp_on(uint8_t lamp);
void lamp_off(uint8_t lamp);
uint8_t get_transformer_size(void);

typedef struct tranf_t {
  uint8_t potencia;
  uint8_t criterio_20;
  uint8_t criterio_40;
  uint32_t tempo_20[3];
  uint32_t tempo_40[3];
  uint16_t   media[3];
  uint32_t  soma[3];
} transformador;


#endif
