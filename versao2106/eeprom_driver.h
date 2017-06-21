/*
 * eeprom_driver.h
 *
 * Created: 06/06/2017 16:08:15
 *  Author: Vinícius
 */ 

#include <asf.h>

#ifndef EEPROM_DRIVER_H_
#define EEPROM_DRIVER_H_

void configure_eeprom(void);
void configure_bod(void);
void write_eeprom(uint8_t *pwm, int tamanho);
uint8_t * read_eeprom(); 
uint16_t get_offset();



#endif /* EEPROM_DRIVER_H_ */