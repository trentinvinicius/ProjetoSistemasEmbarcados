#include <asf.h>
#include <tcc_driver.h>
/**Estrutura que contem a instancia do modulo tcc responsavel por contar o tempo em que o LED fica ligado em uma determinada luminosidade.*/
struct tcc_module tcc_timer;

/**
* \fn configure_tcc()
* \brief Configura e inicializa o modulo tcc. Define que 125000 ciclos do clock equivalem a 1 s.
*/
void configure_tcc(void)
{
	struct tcc_config config_tcc_timer;
	tcc_get_config_defaults(&config_tcc_timer, TCC0);
		
	config_tcc_timer.counter.clock_source = GCLK_GENERATOR_1; //clock = 8000000  system_gclk_chan_get_hz(GCLK_GENERATOR_1)
	config_tcc_timer.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV64; // divide o clock por 64
	config_tcc_timer.counter.period =   1250000; //125000 equivale a 1 s
	
	tcc_init(&tcc_timer, TCC0, &config_tcc_timer);
	
	tcc_enable(&tcc_timer);
}
/**
* \fn configure_tcc_callbacks()
* \brief Funcao que define e inicializa a interrupcao gerada quando ocorre overflow do timer
*/
void configure_tcc_callbacks(void)
{
	tcc_register_callback(&tcc_timer, tcc_callback_timer, TCC_CALLBACK_OVERFLOW);
	tcc_enable_callback(&tcc_timer, TCC_CALLBACK_OVERFLOW);
}

/**
* \fn getTime()
* \brief Funcao que retorna o valor referente a contagem do timer.
*/
int getTime(void)
{
	return tcc_get_count_value(&tcc_timer);
}

/**
* \fn setTime(int tempo)
* \param[in] tempo Valor que sera setado ao timer.
* \brief Funcao que seta um valor ao timer. Utilizada para reinicializar o timer.
*/
void setTime(int tempo)
{
	tcc_set_count_value(&tcc_timer, tempo);
}