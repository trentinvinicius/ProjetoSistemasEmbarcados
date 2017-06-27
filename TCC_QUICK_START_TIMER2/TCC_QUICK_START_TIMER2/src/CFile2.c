#include <asf.h>

static void configure_tcc(void);
static void configure_tcc_callbacks(void);
static void tcc_callback_to_toggle_led(
struct tcc_module *const module_inst);


//! [module_inst]
struct tcc_module tcc_instance;
//! [module_inst]

//! [callback_funcs]
static void tcc_callback_to_toggle_led(
struct tcc_module *const module_inst)
{
	port_pin_toggle_output_level(LED0_PIN);
	//int valor = tcc_get_capture_value(&tcc_instance,TCC_CALLBACK_CHANNEL_0);
}
//! [callback_funcs]

//! [setup]
static void configure_tcc(void)
{
	//! [setup_config]
	struct tcc_config config_tcc;
	//! [setup_config]
	//! [setup_config_defaults]
	tcc_get_config_defaults(&config_tcc, TCC0);
	//! [setup_config_defaults]

	//! [setup_change_config]
	config_tcc.counter.clock_source = GCLK_GENERATOR_1;
	config_tcc.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV64;
	config_tcc.counter.period =   2000;
	//! [setup_change_config]

	//! [setup_set_config]
	tcc_init(&tcc_instance, TCC0, &config_tcc);
	//! [setup_set_config]

	//! [setup_enable]
	tcc_enable(&tcc_instance);
	//! [setup_enable]
}

static void configure_tcc_callbacks(void)
{
	//! [setup_register_callback]
	tcc_register_callback(&tcc_instance, tcc_callback_to_toggle_led,
	TCC_CALLBACK_OVERFLOW);
	//! [setup_register_callback]

	//! [setup_enable_callback]
	tcc_enable_callback(&tcc_instance, TCC_CALLBACK_OVERFLOW);

	//! [setup_enable_callback]
}
//! [setup]

#if 0
int main(void)
{
	system_init();

	//! [setup_init]
	configure_tcc();
	configure_tcc_callbacks();
	//! [setup_init]

	//! [main]
	//! [enable_global_interrupts]
	system_interrupt_enable_global();
	//! [enable_global_interrupts]

	//! [main_loop]
	while (true) {
	}
	//! [main_loop]
	//! [main]
}

/*Copia TCC driver*/
struct tcc_module tcc_instance;
//! [module_inst]

//! [callback_funcs]
static void tcc_callback_to_toggle_led(
struct tcc_module *const module_inst)
{
	port_pin_toggle_output_level(LED0_PIN);
	//int valor = tcc_get_capture_value(&tcc_instance,TCC_CALLBACK_CHANNEL_0);
}
//! [callback_funcs]

static void configure_tcc(void)
{
	//! [setup_config]
	struct tcc_config config_tcc;
	//! [setup_config]
	//! [setup_config_defaults]
	tcc_get_config_defaults(&config_tcc, TCC0);
	//! [setup_config_defaults]

	//! [setup_change_config]
	config_tcc.counter.clock_source = GCLK_GENERATOR_1;
	config_tcc.counter.clock_prescaler = TCC_CLOCK_PRESCALER_DIV64;
	config_tcc.counter.period =   200000;
	//! [setup_change_config]

	//! [setup_set_config]
	tcc_init(&tcc_instance, TCC0, &config_tcc);
	//! [setup_set_config]

	//! [setup_enable]
	tcc_enable(&tcc_instance);
	//! [setup_enable]
}
static void configure_tcc_callbacks(void)
{
	//! [setup_register_callback]
	tcc_register_callback(&tcc_instance, tcc_callback_to_toggle_led,
	TCC_CALLBACK_OVERFLOW);
	//! [setup_register_callback]

	//! [setup_enable_callback]
	tcc_enable_callback(&tcc_instance, TCC_CALLBACK_OVERFLOW);

	//! [setup_enable_callback]
}
/*Fim da copia */

#endif