﻿/*
 * eeprom_driver.c
 *
 * Created: 06/06/2017 16:08:35
 *  Author: Vinícius
 */ 

#include <asf.h>
#include <eeprom_driver.h>
#include <stdlib.h>

uint16_t offset = 0;
//uint8_t *res; //[EEPROM_PAGE_SIZE]
uint8_t res[1000];

//! [setup]
void configure_eeprom(void)
{
	/* Setup EEPROM emulator service */
//! [init_eeprom_service]
	enum status_code error_code = eeprom_emulator_init();
//! [init_eeprom_service]

//! [check_init_ok]
	if (error_code == STATUS_ERR_NO_MEMORY) {
		while (true) {
			/* No EEPROM section has been set in the device's fuses */
		}
	}
//! [check_init_ok]
//! [check_re-init]
	else if (error_code != STATUS_OK) {
		/* Erase the emulated EEPROM memory (assume it is unformatted or
		 * irrecoverably corrupt) */
		eeprom_emulator_erase_memory();
		eeprom_emulator_init();
	}
//! [check_re-init]
}

#if (SAMD || SAMR21)
void SYSCTRL_Handler(void)
{
	if (SYSCTRL->INTFLAG.reg & SYSCTRL_INTFLAG_BOD33DET) {
		SYSCTRL->INTFLAG.reg = SYSCTRL_INTFLAG_BOD33DET;
		eeprom_emulator_commit_page_buffer();
	}
}
#endif
void configure_bod(void)
{
#if (SAMD || SAMR21)
	struct bod_config config_bod33;
	bod_get_config_defaults(&config_bod33);
	config_bod33.action = BOD_ACTION_INTERRUPT;
	/* BOD33 threshold level is about 3.2V */
	config_bod33.level = 48;
	bod_set_config(BOD_BOD33, &config_bod33);
	bod_enable(BOD_BOD33);

	SYSCTRL->INTENSET.reg = SYSCTRL_INTENCLR_BOD33DET;
	system_interrupt_enable(SYSTEM_INTERRUPT_MODULE_SYSCTRL);
#endif

}

void write_eeprom(uint8_t *pwm, int tamanho)
{
	eeprom_emulator_write_buffer(offset, pwm, tamanho);
	//uint8_t vEnvio[sizeof(pwm)];
	//for (int m = 0; m<sizeof(pwm); m++)
	//{
	//	vEnvio[m] = pwm[m];
	//}
	//uint8_t escreve[5] = {0x18, 0x44, 0x45, 0x41, 0x2};
	//eeprom_emulator_write_page(0, escreve);
	
	//eeprom_emulator_write_buffer(0, vEnvio, sizeof(vEnvio));
	offset += tamanho;

	enum status_code error =  eeprom_emulator_commit_page_buffer();
	while (error != STATUS_OK) {
	}
	
	#if 0
	uint8_t *vetor;
	for (int v1 = 0; v1 <= 2*sizeof(pwm); v1+=2){
		vetor[v1] = pwm[v1/2];
	}
	
	for (int v2 = 1; v2 <= 2*sizeof(tempo); v2+=2)
	{
		vetor[v2] = tempo[(v2-1)/2];
	}
	
	return vetor;
	#endif
}

uint8_t * read_eeprom() //uint8_t *const data
{
	//res = malloc(offset);
	//enum status_code a = eeprom_emulator_read_page(0, res);
	enum status_code a = eeprom_emulator_read_buffer(0, res, offset);

	while (a != STATUS_OK){
		
	}
	return res;
}

uint16_t get_offset(){
	//res = NULL;
	uint16_t valor = offset;
	offset = 0;
	return valor;
}