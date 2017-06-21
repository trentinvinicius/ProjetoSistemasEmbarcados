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
	STLerSensor = 0, STPWM, STGravar, STTimer
} States;

typedef void (*Action)(unsigned char data);

struct StateMachine {
	States state;
	unsigned int valorAnterior;
	unsigned int valorAtual;
	unsigned int acaoControle;
	uint8_t PWM[11];
	uint8_t TEMPO[11];
	unsigned int indice;
	unsigned int incremento;
	Action action[7];
} sm;



/// Funções executadas quando as interrupções são geradas
void tcc_callback_pwm(struct tcc_module *const module_inst)
{
	/** Função chamada a cada 10 segundos e quando chamada,
	 *  incrementa o valor da variável incremento
	 */
	sm.incremento ++;
}

void usart_read_callback(struct usart_module *const usart_module)
{
	/** Quando chamada, verifica se a mensagem recebida pela serial é "envie"
	 *  Caso verdadeiro, acende o LED, chama a função EnviarDados e apaga o LED    
	 *  quando todos os dados foram enviados
	*/
	if(strcmp(rx_buffer, "envie") == 0)
	{
		port_pin_set_output_level(LED_0_PIN, 0);
		EnviarDados();
		port_pin_set_output_level(LED_0_PIN, 1);
	}
}
/// Fim das funções de interrupção


void EnviarDados()
{
	/** Função chamada quando ocorre uma interrupção gerada pelo recebimento da palavra "envie"
	 *  através da USART
	 */
	uint8_t *resultado;
	resultado = read_eeprom();  /// Recebe os dados gravados na memória EEPROM
	uint16_t offset = get_offset(); /// Recebe a quantidade de dados gravados na memória
	printf("Dados lidos:\n");  /// Mensagem que caracteriza o início da transmissão
	for (int a = 0; a<= offset-1; a++)
	{
		printf("%d\n", resultado[a]);
	}
	if (sm.indice != 1)
	{
		for (int b = 0; b<= sm.indice-2; b++)
		{
			printf("%d\n", sm.PWM[b]);
			printf("%d\n", sm.TEMPO[b]);
		}
		sm.PWM[0] = sm.PWM[sm.indice-1];  /// Realoca a última posição do vetor PWM como sendo a primeira
		sm.indice = 1;   /// Reinicia o índice		
	}
	
	
	printf("fim");    /// Mensagem que caracteriza o fim da transmissão
}

void LerSensor(){
	/** Função que lê o valor do sensor
	 * se houver uma diferença superior a 200 entre a leitura atual e a última leitura armazenada
	 * prossegue para a próxima etapa da máquina de estados
	 * caso contrário, permanece nesse estado
	 */
	sm.valorAtual = read();
	//printf("Valores: atual: %d e anterior %d\n", sm.valorAtual, sm.valorAnterior);
	if(abs(sm.valorAtual - sm.valorAnterior)>200){
		sm.state = STPWM;
		sm.valorAnterior = sm.valorAtual;
	}
}


void SetPWM()
{
	/** Função que atualiza a luminosidade do LED.
	 *  Isso ocorre alterando o duty-cicle do PWM, cujo valor máximo é 4096 que equivale ao LED totalmente apagado.
	 *  Feito isso, prossegue para o próximo estado
	 */
	set_pwm_compare_value(sm.valorAtual);
	sm.state = STGravar;
}


void Gravar()
{	
	/** Função que armazena o novo percentual de luminosidade do LED e a duração em minutos do percentual anterior
	*/
	float pwmPercentual = (100 - sm.valorAtual * 0.0244140625);   /// converte a ação de controle em percentual
	sm.PWM[sm.indice] = (int)pwmPercentual;   //grava no vetor PWM a ação de controle em percentual
	int tempoMinutos = (sm.incremento*1250000 + getTime())/7500000; /// por simplificacao, adota-se o tempo em minutos
	int tempoSegundos = (sm.incremento*1250000 + getTime())/125000; /// por simplificacao, adota-se o tempo em segundos
	//printf("Percentual PWM: %d Tempo: %d\n", (int)pwmPercentual, tempoSegundos);
	//printf ("Valor atual na posição %d: %d\n", sm.indice -1 , sm.TEMPO[sm.indice - 1]);
	sm.TEMPO[sm.indice-1] = (int)tempoSegundos;   //armazena o tempo calculado no vetor tempo
	//printf("Indice sendo gravado %d, valor sendo gravado %d e valor gravado %d\n", (sm.indice - 1), (int)tempoSegundos, sm.TEMPO[sm.indice - 1]);
	sm.indice ++;
	if (sm.indice == 11)
	{
		uint8_t gravarEeprom[18];
		for (int v1 = 0; v1<18; v1+=2)
		{
			gravarEeprom[v1] = sm.PWM[(v1/2)];
		}
		for (int v2 = 1; v2<19; v2+=2)
		{
			gravarEeprom[v2] = sm.TEMPO[((v2-1)/2)];
		}
		for (int beta = 0; beta<10; beta++){
			//printf("PWM e Tempo na posicao %d: %d e %d\n", beta, sm.PWM[beta], sm.TEMPO[beta]);
		}
		//printf("GravarEeprom: \n");
		for (int aba = 0; aba <18; aba++){
			//printf("%d ", gravarEeprom[aba]);
		}
		//printf("\n");
		
		write_eeprom(&gravarEeprom, 18);
				
		sm.PWM[0] = sm.PWM[9];
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
	sm.PWM[0] = 0;
	sm.TEMPO[0] = 0;
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
	
	printf ("INICIO\n");
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
