/*
 * usart_driver.c
 *
 * Created: 06/06/2017 15:49:58
 *  Author: Vinícius
 */ 


#include <asf.h>
#include <stdio.h>
#include <usart_driver.h>

struct usart_module usart_instance;
struct usart_config usart_conf;



#define MAX_RX_BUFFER_LENGTH   5

volatile uint8_t rx_buffer[5];
//! [rx_buffer_var]


int usartConfig(){
	
	usart_get_config_defaults(&usart_conf);
	usart_conf.baudrate    = 9600;
	usart_conf.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	usart_conf.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	usart_conf.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	usart_conf.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	usart_conf.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	
	//! [setup_set_config]
	while (usart_init(&usart_instance,
	EDBG_CDC_MODULE, &usart_conf) != STATUS_OK) {
	}
	//! [setup_set_config]
	stdio_serial_init(&usart_instance, EDBG_CDC_MODULE, &usart_conf);
	
	usart_enable(&usart_instance);
	return &usart_instance;
}

void configure_usart_callbacks(void)
{
	//! [setup_register_callbacks]
	//usart_register_callback(&usart_instance,
	//usart_write_callback, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_register_callback(&usart_instance,
	usart_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
	//! [setup_register_callbacks]

	//! [setup_enable_callbacks]
	usart_enable_callback(&usart_instance, USART_CALLBACK_BUFFER_TRANSMITTED);
	usart_enable_callback(&usart_instance, USART_CALLBACK_BUFFER_RECEIVED);
	//! [setup_enable_callbacks]
	
	//uint8_t string[] = "Hello World!\r\n";
	//usart_write_buffer_wait(&usart_instance, string, sizeof(string));
}

#if 0
void usart_read_callback(struct usart_module *const usart_module)
{
	if(rx_buffer == "envie"){
		printf("TRESSTET\n\n\n\n\n\n\n\n\n\n\n\n\n\n\");
		port_pin_toggle_output_level(LED_0_PIN);
	}
	usart_write_buffer_job(&usart_instance,
	(uint8_t *)rx_buffer, MAX_RX_BUFFER_LENGTH);
}

void usart_write_callback(struct usart_module *const usart_module)
{
	//port_pin_toggle_output_level(LED_0_PIN);
}
#endif