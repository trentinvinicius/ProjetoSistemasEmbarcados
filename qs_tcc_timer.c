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

#include <asf.h>
#include <stdio.h>
#include <stdint.h>

static void configure_tcc(void);
static void configure_tcc_callbacks(void);
static void tcc_callback_usart(
		struct tcc_module *const module_inst);
static void tcc_callback_pwm(
struct tcc_module *const module_inst);
void LerSensor();

typedef enum {
	STLerSensor = 0, STControle, STGravar, STPWM, STTimer, STUsart, STEnviar
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
	unsigned uint32_t incremento;
	Action action[7];
} sm;

//! [module_inst]
struct tcc_module tcc_usart, tcc_pwm;
//! [module_inst]

//! [callback_funcs]
static void tcc_callback_usart(
		struct tcc_module *const module_inst)
{
	port_pin_toggle_output_level(LED0_PIN);
	printf("LED MUDA\n");
	sm.state = STLerSensor;
	
}

static void tcc_callback_pwm(
struct tcc_module *const module_inst)
{
	sm.incremento ++;
	printf("Incremento : %d\n", sm.incremento);
}
//! [callback_funcs]

//! [setup]
static void configure_tcc(void)
{
	//! [setup_config]
	struct tcc_config config_tcc_usart, config_tcc_pwm;
	//! [setup_config]
	//! [setup_config_defaults]
	tcc_get_config_defaults(&config_tcc_usart, TCC0);
	tcc_get_config_defaults(&config_tcc_pwm, TCC1);
	//! [setup_config_defaults]

	//! [setup_change_config]
	config_tcc_usart.counter.clock_source = GCLK_GENERATOR_1;
	config_tcc_usart.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV64;
	config_tcc_usart.counter.period =   4000;
	
	config_tcc_pwm.counter.clock_source = GCLK_GENERATOR_1;
	config_tcc_pwm.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV64;
	config_tcc_pwm.counter.period =   2000;
	//! [setup_change_config]

	//! [setup_set_config]
	tcc_init(&tcc_usart, TCC0, &config_tcc_usart);
	tcc_init(&tcc_pwm, TCC1, &config_tcc_pwm);
	//! [setup_set_config]

	//! [setup_enable]
	tcc_enable(&tcc_usart);
	tcc_enable(&tcc_pwm);
	//! [setup_enable]
	
	tcc_stop_counter(&tcc_usart);
	//tcc_stop_counter(&tcc_pwm);
}

static void configure_tcc_callbacks(void)
{
	//! [setup_register_callback]
	tcc_register_callback(&tcc_usart, tcc_callback_usart,TCC_CALLBACK_OVERFLOW);
	tcc_register_callback(&tcc_pwm, tcc_callback_pwm,TCC_CALLBACK_OVERFLOW);	
	//! [setup_register_callback]

	//! [setup_enable_callback]
	tcc_enable_callback(&tcc_usart, TCC_CALLBACK_OVERFLOW);
	tcc_enable_callback(&tcc_pwm, TCC_CALLBACK_OVERFLOW);
	//! [setup_enable_callback]
}

struct usart_module usart_instance;
struct usart_config usart_conf;
void usartConfig(){
	
	usart_get_config_defaults(&usart_conf);
	usart_conf.baudrate    = 9600;
	usart_conf.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	usart_conf.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	usart_conf.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	usart_conf.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	usart_conf.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	stdio_serial_init(&usart_instance, EDBG_CDC_MODULE, &usart_conf);
	
	usart_enable(&usart_instance);
}

struct adc_module adc_instance;
void adcConfig(){
	struct adc_config config_adc;
	adc_get_config_defaults(&config_adc);
	config_adc.gain_factor     = ADC_GAIN_FACTOR_DIV2;
	config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV16;
	config_adc.reference       = ADC_REFERENCE_INTVCC1;
	config_adc.positive_input  = ADC_POSITIVE_INPUT_PIN0;
	config_adc.resolution      = ADC_RESOLUTION_12BIT;
	config_adc.freerunning     = true;
	config_adc.left_adjust     = false;
	adc_init(&adc_instance, ADC, &config_adc);
	adc_enable(&adc_instance);
	
	adc_start_conversion(&adc_instance);
}

void LerSensor(){
	printf("LENDO SENSOR");
	do {
		/* Wait for conversion to be done and read out result */
	} while (adc_read(&adc_instance, &sm.valorAtual) == STATUS_BUSY);
	printf("Valores: atual: %d e anterior %d\n", sm.valorAtual, sm.valorAnterior);
	if(abs(sm.valorAtual - sm.valorAnterior)>1){
		sm.state = STControle;
		}else{
		sm.state = STUsart;
	}
	sm.valorAnterior = sm.valorAtual;
}

void Controle(){
	printf("CONTROLANDO");
	sm.acaoControle = sm.valorAtual*100;	
	sm.state = STPWM;
}

void SetPWM(){
	printf("SETANDO PWM");
	//liga led com acao
	sm.state = STGravar;
}

void Gravar(){
	printf("GRAVANDO");
	sm.PWM[sm.indice] = sm.acaoControle;
	
	sm.TEMPO[sm.indice-1] =sm.incremento * 4000 + tcc_get_count_value(&tcc_pwm);
	sm.indice ++;
	if (sm.indice == 1000){
		sm.indice = 0;
	}
	sm.state = STTimer;
}


void Timer(){
	printf("INICIANDO TIMER\n");
	tcc_set_count_value(&tcc_pwm, 0);
	sm.incremento = 0;
	sm.state = STUsart;
}

void Usart(){
	tcc_set_count_value(&tcc_usart, 0);
	tcc_restart_counter(&tcc_usart);
	printf("ESPERANDO USART");
	char msg;
	scanf("%c",&msg);
	if (msg == 'a'){
		sm.state = STEnviar;
		}else{
		sm.state = STLerSensor;
	}
}

void EnviarDados(){
	tcc_stop_counter(&tcc_usart);
	printf("qnt %d", sm.indice);
	for(int i=1; i<=sm.indice; i++){
		printf("\nTempo %d Controle %d \n", sm.TEMPO[i], sm.PWM[i]);
	}
	sm.indice = 1;
	sm.state = STLerSensor;
}

void initSM()
{
	sm.state = STLerSensor;
	sm.indice = 1;
	sm.acaoControle = 0;
	sm.incremento = 0;
	sm.action[STLerSensor] = LerSensor;
	sm.action[STControle] = Controle;
	sm.action[STGravar] = Gravar;
	sm.action[STPWM] = SetPWM;
	sm.action[STTimer] = Timer;
	sm.action[STUsart] = Usart;
	sm.action[STEnviar] = EnviarDados;
	
}

//! [setup]

int main(void)
{
	system_init();

//! [setup_init]
	usartConfig();
	printf("Iniciando\n");
	configure_tcc();
	configure_tcc_callbacks();
	printf("CONf\n");
	adcConfig();
	initSM();
	
//! [setup_init]

//! [main]
	//! [enable_global_interrupts]
	system_interrupt_enable_global();
	//! [enable_global_interrupts]

	//! [main_loop]
	while (true) {
		sm.action[sm.state](0);
		int beta =  tcc_get_count_value(&tcc_pwm);
		printf("\n\nTEMPO ATUAL: %d",beta);
		printf("\n");
	}
	//! [main_loop]
//! [main]
}
