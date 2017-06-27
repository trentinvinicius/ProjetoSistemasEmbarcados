#include <asf.h>
#include <pwm_driver.h>

/**Estrutura que contem a instancia do modulo tcc responsavel pelo PWM*/
struct tcc_module pwm_instance;

/**
* \fn configure_pwm()
* \brief Funcao para configurar o modulo tcc responsavel pelo PWM. O periodo do PWM e definido como sendo o valor maximo lido pelo sensor LDR (4096) e o duty-cycle e o valor atual
*/
void configure_pwm(void)
{
	struct tcc_config config_tcc;
	tcc_get_config_defaults(&config_tcc, TCC1);
	config_tcc.counter.period = 4096;			//periodo do pwm, definido como sendo o valor maximo lido pelo sensor LDR
	config_tcc.compare.wave_generation = TCC_WAVE_GENERATION_SINGLE_SLOPE_PWM;
	config_tcc.compare.match[0] = 4096;			//duty cycle do pwm = valor lido pelo sensor ldr
	config_tcc.pins.enable_wave_out_pin[0] = true;
	config_tcc.pins.wave_out_pin[0]        = PIN_PA06E_TCC1_WO0;
	config_tcc.pins.wave_out_pin_mux[0]    = MUX_PA06E_TCC1_WO0;
	tcc_init(&pwm_instance, TCC1, &config_tcc);
	tcc_enable(&pwm_instance);
}
/**
* \fn set_pwm_compare_value(int32_t valor)
* \param[in] valor Valor lido pelo sensor LDR
*\ brief Altera o valor do duty-cicle do PWM e por consequencia a luminosidade do LED
*/
void set_pwm_compare_value(int32_t valor){
	tcc_set_compare_value(&pwm_instance, 0, valor);	
}
