/*
 * tcc_driver.c
 *
 * Created: 06/06/2017 16:06:54
 *  Author: Vinícius
 */ 

//! [setup]
#include <asf.h>
#include <tcc_driver.h>

//! [module_inst]
struct tcc_module tcc_pwm;
//! [module_inst]

void configure_tcc(void)
{
	//! [setup_config]
	struct tcc_config config_tcc_pwm;
	//! [setup_config]
	//! [setup_config_defaults]
	tcc_get_config_defaults(&config_tcc_pwm, TCC0);
	//! [setup_config_defaults]

	//! [setup_change_config]	
	config_tcc_pwm.counter.clock_source = GCLK_GENERATOR_1; //clock = 8000000  system_gclk_chan_get_hz(GCLK_GENERATOR_1)
	config_tcc_pwm.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV64; // divide o clock por 64
	config_tcc_pwm.counter.period =   1250000; //125000 equivale a 1 s
	//! [setup_change_config]

	//! [setup_set_config]
	tcc_init(&tcc_pwm, TCC0, &config_tcc_pwm);
	//! [setup_set_config]

	//! [setup_enable]
	tcc_enable(&tcc_pwm);
	//! [setup_enable]

}

void configure_tcc_callbacks(void)
{
	//! [setup_register_callback]
	tcc_register_callback(&tcc_pwm, tcc_callback_pwm, TCC_CALLBACK_OVERFLOW);
	//! [setup_register_callback]

	//! [setup_enable_callback]
	tcc_enable_callback(&tcc_pwm, TCC_CALLBACK_OVERFLOW);
	//! [setup_enable_callback]
}

int getTime(void)
{
	return tcc_get_count_value(&tcc_pwm);
}

void setTime(int tempo)
{
	tcc_set_count_value(&tcc_pwm, tempo);
}