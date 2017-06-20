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
	uint8_t PWM[10];
	uint8_t TEMPO[10];
	unsigned int indice;
	unsigned int incremento;
	Action action[7];
} sm;



//! [callback_funcs]
void tcc_callback_pwm(struct tcc_module *const module_inst)
{
	port_pin_toggle_output_level(LED_0_PIN);
	sm.incremento ++;
}

void usart_read_callback(struct usart_module *const usart_module)
{
	if(strcmp(rx_buffer, "envie") == 0)			// verifica se a msg recebi pela serial é "envie"
	{
		port_pin_set_output_level(LED_0_PIN, 0);
		EnviarDados();
		port_pin_set_output_level(LED_0_PIN, 1);
	}
}
//! [callback_funcs]


void EnviarDados()
{
	uint8_t *resultado;
	resultado = read_eeprom();
	uint16_t offset = get_offset();
	printf("Dados lidos:\n");
	for (int a = 0; a<= offset-1; a++)
	{
		printf("%d\n", resultado[a]);
	}
	printf("fim");
}

void LerSensor(){
	//printf("LENDO SENSOR");
	sm.valorAtual = read();
	//printf("Valores: atual: %d e anterior %d\n", sm.valorAtual, sm.valorAnterior);
	if(abs(sm.valorAtual - sm.valorAnterior)>200){
		sm.state = STPWM;
		sm.valorAnterior = sm.valorAtual;
	}
}


void SetPWM()
{
	set_pwm_compare_value(sm.valorAtual);
	sm.state = STGravar;
}


void Gravar()
{	
	float pwmPercentual = (100 - sm.valorAtual * 0.0244140625);   // converte a ação de controle em percentual
	sm.PWM[sm.indice] = (int)pwmPercentual;   //grava no vetor PWM a ação de controle em percentual
	int tempoSegundos = (sm.incremento*1250000 + getTime())/125000; // por simplificacao, adota-se o tempo em minutos
	printf("Percentual PWM: %d Tempo: %d\n", (int)pwmPercentual, tempoSegundos);

	sm.TEMPO[sm.indice-1] = (int)tempoSegundos;   //armazena o tempo calculado no vetor tempo
	sm.indice ++;
	if (sm.indice == 10)
	{
		uint8_t gravarEeprom[18];
		for (int v1 = 0; v1<18; v1+=2)
		{
			gravarEeprom[v1] = sm.PWM[(v1/2)+1];
		}
		for (int v2 = 1; v2<18; v2+=2)
		{
			gravarEeprom[v2] = sm.TEMPO[((v2-1)/2)+1];
		}
		printf("GravarEeprom: \n");
		for (int aba = 0; aba <18; aba++){
			printf("%d ", gravarEeprom[aba]);
		}
		printf("\n");
		
		write_eeprom(&gravarEeprom, 20);
		sm.indice = 1;			 
	}
	sm.state = STTimer;
}


void Timer()
{
	setTime(0);
	sm.incremento = 0;
	sm.state = STLerSensor;
}

void initSM()
{
	sm.state = STLerSensor;
	sm.indice = 1;
	sm.incremento = 0;
	sm.valorAnterior = 0;
	sm.valorAtual = 0;
	sm.action[STLerSensor] = LerSensor;
	sm.action[STGravar] = Gravar;
	sm.action[STPWM] = SetPWM;
	sm.action[STTimer] = Timer;	
}

int main(void)
{

	system_init();

//! [setup_init]
	instancia_usart = usartConfig();
	configure_usart_callbacks();
	//printf("Iniciando\n");
	configure_tcc();
	configure_tcc_callbacks();
	adcConfig();
	configure_pwm();
	
	configure_eeprom();
	configure_bod();
	
	
	initSM();
	
//! [setup_init]

//! [main]
	//! [enable_global_interrupts]
	system_interrupt_enable_global();
	//! [enable_global_interrupts]

	//! [main_loop]
	while (true) {
		
		sm.action[sm.state](0);
		//printf("\n\nTEMPO ATUAL: %d",getTime());
		//printf("\n");
		usart_read_buffer_job(instancia_usart,
		(uint8_t *)rx_buffer, 5);
		//printf("%s\n\n",rx_buffer);
		
	
	}
	//! [main_loop]
//! [main]
}
