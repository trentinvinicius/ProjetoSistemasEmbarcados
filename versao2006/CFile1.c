/**
 * \file
 *
 * \brief SAM TCC - Timer Counter for Control Applications Driver Quick Start
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#if 0

#include <asf.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <adc_driver.h>
#include <tcc_driver.h>
#include <usart_driver.h>
#include <pwm_driver.h>
#include <eeprom_driver.h>


int32_t instancia_usart, instancia_pwm;
volatile uint8_t rx_buffer[5];


void LerSensor();
void EnviarDados();

typedef enum {
	STLerSensor = 0, STControle, STGravar, STPWM, STTimer, STEnviar
} States;

typedef void (*Action)(unsigned char data);

struct StateMachine {
	States state;
	unsigned int valorAnterior;
	unsigned int valorAtual;
	unsigned int acaoControle;
	unsigned int PWM[1000];
	unsigned int TEMPO[1000];
	unsigned int indice;
	unsigned int incremento;
	Action action[7];
} sm;



//! [callback_funcs]
void tcc_callback_pwm(struct tcc_module *const module_inst)
{
	sm.incremento ++;
}

void usart_read_callback(struct usart_module *const usart_module)
{
	if(strcmp(rx_buffer, "envie")==0){
		port_pin_set_output_level(LED_0_PIN, 0);
		EnviarDados();
		port_pin_set_output_level(LED_0_PIN, 1);
	}
}
//! [callback_funcs]


void EnviarDados(){
	printf("qnt %d", sm.indice);
	for(int i=1; i<=sm.indice; i++){
		printf("\nTempo %d Controle %d\n", sm.TEMPO[i], sm.PWM[i]);
	}
	sm.indice = 1;
}

void LerSensor(){
	//printf("LENDO SENSOR");
	sm.valorAtual = read();
	printf("Valores: atual: %d e anterior %d\n", sm.valorAtual, sm.valorAnterior);
	if(abs(sm.valorAtual - sm.valorAnterior)>1){
		sm.state = STPWM;
		sm.valorAnterior = sm.valorAtual;
	}	
}

void LerSensor()
{
	sm.valorAtual = read();
	//printf("Valores: atual: %d e anterior %d\n", sm.valorAtual, sm.valorAnterior);
	if(abs(sm.valorAtual - sm.valorAnterior)>100){
		printf("Diferenca valida, muda estado\n");
		printf("Valores: atual: %d e anterior %d\n", sm.valorAtual, sm.valorAnterior);
		sm.state = STPWM;
		sm.valorAnterior = sm.valorAtual;
	}
}

void SetPWM(){
	//printf("SETANDO PWM");
	//liga led com acao
	set_pwm_compare_value(sm.valorAtual);
	sm.state = STGravar;
}

void Gravar(){
	//printf("GRAVANDO");
	sm.PWM[sm.indice] = sm.valorAtual;
	
	sm.TEMPO[sm.indice-1] =sm.incremento * 4000 + getTime();
	sm.indice ++;
	if (sm.indice == 1000){
		sm.indice = 0;
	}
	printf("\nResultado\n");
	
	sm.state = STTimer;
}

void Timer(){
	//printf("INICIANDO TIMER\n");
	setTime(0);
	sm.incremento = 0;
	sm.state = STLerSensor;
}

void initSM()
{
	sm.state = STLerSensor;
	sm.indice = 1;
	sm.incremento = 0;
	sm.action[STLerSensor] = LerSensor;
	sm.action[STGravar] = Gravar;
	sm.action[STPWM] = SetPWM;
	sm.action[STTimer] = Timer;	
}


#endif