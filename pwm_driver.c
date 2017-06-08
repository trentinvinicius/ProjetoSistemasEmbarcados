/*
 * pwm_driver.c
 *
 * Created: 08/06/2017 09:47:03
 *  Author: Vinícius
 *
 */
#include <asf.h>
#include <pwm_driver.h>


struct tcc_module pwm_instance;

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

	
	//return &pwm_instance;
}

void set_pwm_compare_value(int32_t valor){
	tcc_set_compare_value(&pwm_instance, 0, valor);	
}

int get_pwm_compare_value(void){
	return tcc_get_capture_value(&pwm_instance, 0);
}