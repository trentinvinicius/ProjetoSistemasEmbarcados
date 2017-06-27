#include <asf.h>
#include <eeprom_driver.h>
#include <stdlib.h>

/// \var offset Posicao na memoria a partir do qual sera escrito ou numero de bytes a serem lidos
uint16_t offset = 0;

/// \var *res Ponteiro que conterá os bytes lidos da memoria EEPROM
uint8_t *res;

/**
*\fn configure_eeprom()
* \brief Configura a memoria EEPROM da placa
*/
void configure_eeprom(void)
{
	enum status_code error_code = eeprom_emulator_init();
	if (error_code == STATUS_ERR_NO_MEMORY) {
		while (true) {
			/* No EEPROM section has been set in the device's fuses */
		}
	}
	else if (error_code != STATUS_OK) {
		/* Erase the emulated EEPROM memory (assume it is unformatted or irrecoverably corrupt) */
		eeprom_emulator_erase_memory();
		eeprom_emulator_init();
	}
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
	config_bod33.level = 48;
	bod_set_config(BOD_BOD33, &config_bod33);
	bod_enable(BOD_BOD33);

	SYSCTRL->INTENSET.reg = SYSCTRL_INTENCLR_BOD33DET;
	system_interrupt_enable(SYSTEM_INTERRUPT_MODULE_SYSCTRL);
#endif

}

/**
* \fn write_eeprom(uint8_t *conteudo, int tamanho)
* \param[in] conteudo Ponteiro que sera escrito na memoria
* \param[in] tamanho Quantidade de bytes que sera escrito
* \brief Funcao para para escrever na memoria EEPROM os valores de PWM e TEMPO
*/
void write_eeprom(uint8_t *conteudo, int tamanho)
{
	if (offset + tamanho >= 4000)
	{
		offset = 0;
	}

	eeprom_emulator_write_buffer(offset, conteudo, tamanho);
	
	offset += tamanho;
	
	enum status_code error =  eeprom_emulator_commit_page_buffer();
	while (error != STATUS_OK) {
	}
}

/**
* \fn read_eeprom()
* \param[out] res Ponteiro que contem os bytes lidos
* \brief Funcao para para ler a memoria EEPROM
*/
uint8_t * read_eeprom() 
{
	res = malloc(offset);
	enum status_code a = eeprom_emulator_read_buffer(0, res, offset);
	while (a != STATUS_OK){		
	}
	
	return res;
}


/**
* \fn get_offset()
* \param[out] valor Quantidade de bytes lidos da memoria
* \brief Funcao que retorna a quantidade de bytes lidos da memoria EEPROM
*/
uint16_t get_offset(){
	res = NULL;
	uint16_t valor = offset;
	offset = 0;
	return valor;
}
