/**
*\brief Trabalho final da disciplina de Sistemas Embarcados
* Academicos: Daniel Caye e Vinicius Trentin
* Placa utilizada: SAM D21
* O trabalho consiste em realizar a leitura de um sensor de luminosidade (LDR) e baseado nisso modificar a intensidade do brilho de um led
* Tanto a intensidade em percentual quanto o intervalo de tempo entre as variacoes do nivel da intensidade do led sao armazenados na memoria EEPROM
*/
#include <asf.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <adc_driver.h>
#include <tcc_driver.h>
#include <usart_driver.h>
#include <pwm_driver.h>
#include <eeprom_driver.h>

/// Declaracao de variaveis globais
int32_t instancia_usart;
volatile uint8_t rx_buffer[5];
/// Fim da declaracao de variaveis globais

/// Prototipos de funcoes
void LerSensor();
void Gravar();
void SetPWM();
void Timer();
void EnviarDados();
/// Fim dos prototipos

/*\enum possiveis estados da maquina de estados */
typedef enum {
	STLerSensor = 0, STPWM, STGravar, STTimer 
} States;

/// ponteiro de funcao
typedef void (*Action)(unsigned char data);  

/**
* Estrutura que contem os elementos referentes a maquina de estados
*/
struct StateMachine {
	States state;	/*!< estado em que a maquina de estados se encontra */
	unsigned int valorAnterior; /*!< valor armazenado para comparacao com valor atual */
	unsigned int valorAtual; /*!< valor lido pelo sensor */
	uint8_t PWM[11];	/*!< vetor que armazena o percentual de luminosidade */
	uint8_t TEMPO[11];	/*!<	vetor que armazena as medidas de tempo */
	unsigned int indice;	/*!< indice dos vetores PWM e TEMPO */
	unsigned int incremento; /*!< incremento do tempo, incrementado por interrupcao */
	Action action[4];	/*!< instancia do ponteiro de funcao */
} sm;

/// Funcoes executadas quando as interrupcoes sao geradas

/**
* \fn tcc_callback_pwm(struct tcc_module *const module_inst)
* \param *const module_inst Estrutura que contem a instancia do modulo tcc.
* \brief Funcao chamada a cada 10 segundos. Quando chamada, incrementa o valor da variavel incremento
*/
void tcc_callback_pwm(struct tcc_module *const module_inst)
{
	sm.incremento ++;
}

/** 
* \fn usart_read_callback(struct usart_module *const usart_module)
* \param *const usart_module Estrutura que contem a instancia do modulo usart.
* \brief Quando chamada, verifica se a mensagem recebida pela serial e "envie". Caso verdadeiro, acende o LED, chama a funcao EnviarDados e apaga o LED apos todos os dados serem enviados
*/
void usart_read_callback(struct usart_module *const usart_module)
{
	if(strcmp(rx_buffer, "envie") == 0)
	{
		port_pin_set_output_level(LED_0_PIN, 0);
		EnviarDados();
		port_pin_set_output_level(LED_0_PIN, 1);
	}
}
/// Fim das funcoes de interrupcao

/** 
* \fn EnviarDados()
* \brief Funcao chamada quando ocorre uma interrupcao gerada pelo recebimento da palavra "envie" atraves da USART.
*/
void EnviarDados()
{
	uint8_t *resultado;
	resultado = read_eeprom();  /*!< Recebe os dados gravados na memoria EEPROM */
	uint16_t offset = get_offset(); /*!< Recebe a quantidade de dados gravados na memoria */
	printf("Dados lidos:\n");  /*!< Mensagem que caracteriza o inicio da transmissao */
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
		sm.PWM[0] = sm.PWM[sm.indice-1];  /*!< Realoca a ultima posicao do vetor PWM como sendo a primeira */
		sm.indice = 1;   /*!< Reinicia o indice */
	}
	
	printf("fim");    /*!< Mensagem que caracteriza o fim da transmissao */
}

/*!
 \fn LerSensor()
 \brief Funcao que le o valor do sensor. Se houver uma diferenca superior a 200 entre a leitura atual e a ultima leitura armazenada, altera a variavel referente ao estado da maquina de estados. Caso contrario, permanece nesse estado.
*/
void LerSensor(){
	sm.valorAtual = read();
	//printf("Valores: atual: %d e anterior %d\n", sm.valorAtual, sm.valorAnterior);
	if(abs(sm.valorAtual - sm.valorAnterior)>200){
		sm.state = STPWM;
		sm.valorAnterior = sm.valorAtual;
	}
}

/** 
* \fn SetPWM()
* \brief Funcao que atualiza a luminosidade do LED. Isso ocorre alterando o duty-cicle do PWM, cujo valor maximo e 4096 que equivale ao LED totalmente apagado. Feito isso, altera a variavel referente ao estado da maquina de estados
*/
void SetPWM()
{
	set_pwm_compare_value(sm.valorAtual);
	sm.state = STGravar;
}

/** 
* \fn Gravar()
* \brief Funcao que armazena o novo percentual de luminosidade do LED e a duracao em minutos do percentual anterior. Caso o indice for 11 os dados dos vetores PWM e TEMPO sao combinados e gravados na memoria EEPROM. A penultima posicao do vetor PWM e realocada para a primeira posicao do vetor, pois ainda nao ha informacao de tempo disponivel referente ao valor de PWM setado. Feito isso, altera a variavel referente ao estado da maquina de estados
*/
void Gravar()
{	
	float pwmPercentual = (100 - sm.valorAtual * 0.0244140625);   /*!< converte a acao de controle em percentual */
	sm.PWM[sm.indice] = (int)pwmPercentual;   /*!< grava no vetor PWM a acao de controle em percentual */
	int tempoMinutos = (sm.incremento*1250000 + getTime())/7500000; /*!< por simplificacao, adota-se o tempo em minutos */
	int tempoSegundos = (sm.incremento*1250000 + getTime())/125000; /*!< por simplificacao, adota-se o tempo em segundos */
	sm.TEMPO[sm.indice-1] = (int)tempoSegundos;   /*!< armazena o tempo calculado no vetor tempo */
	sm.indice ++;	/*!< incrementa o indice */
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
		
		write_eeprom(&gravarEeprom, 18);
				
		sm.PWM[0] = sm.PWM[9];
		sm.indice = 1;	 
	}
	sm.state = STTimer;
}

/**
* \fn Timer()
* \brief Funcao que reinicia o temporizador. Feito isso, altera a variavel referente ao estado da maquina de estados
*/
void Timer()
{
	setTime(0);
	sm.incremento = 0;
	sm.state = STLerSensor;
}

/**
* \fn initSM()
* \brief Funcao que inicializa as variaveis da maquina de estados e referencia cada estado a sua funcao especifica
*/
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
	/**
	* Configuracoes iniciais do sistema
	*/
	system_init();
	instancia_usart = usartConfig();   /*!< inicializa a USART e retorna a instancia da mesma */
	configure_usart_callbacks();
	configure_tcc();
	configure_tcc_callbacks();
	adcConfig();
	configure_pwm();
	configure_eeprom();
	configure_bod();
 
	initSM();

	system_interrupt_enable_global();	/*!< habilita as interrupcoes */
	
	/// Fim das configuracoes iniciais
	
    /// Loop principal
	while (true) 
	{
		sm.action[sm.state](0); //!< chama a funcao da maquina de estados que esta na vez
		usart_read_buffer_job(instancia_usart, (uint8_t *)rx_buffer, 5);   //!< le a porta serial
	}
	/// Fim do loop principal
}