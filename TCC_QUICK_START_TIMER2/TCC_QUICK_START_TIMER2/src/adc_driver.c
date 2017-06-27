#include <asf.h>
#include <adc_driver.h>

/**Estrutura que contem a instancia do modulo conversor analogico/digital*/
struct adc_module adc_instance;
/**
* \fn adcConfig()
* \brief Configura e habilita o modulo do conversor analogico/digital. O conversor e configurado com 12 bits de modo que o valor maximo lido seja 4096.
*/
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

/**
* \fn read()
* \param[out] valor Valor do sensor lido
* \brief Le o sensor LDR conectado
*/
int read(void){
	int valor;
	do {
		/* Wait for conversion to be done and read out result */
	} while (adc_read(&adc_instance, &valor) == STATUS_BUSY);
	return valor;
}
