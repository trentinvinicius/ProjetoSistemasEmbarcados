#include <asf.h>

#ifndef EEPROM_DRIVER_H_
#define EEPROM_DRIVER_H_

void configure_eeprom(void);
void configure_bod(void);
void write_eeprom(uint8_t *conteudo, int tamanho);
uint8_t * read_eeprom(); 
uint16_t get_offset();



#endif