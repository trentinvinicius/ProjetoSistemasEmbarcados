#include <asf.h>
#include <stdio.h>
#include <usart_driver.h>

/**Estrutura que contem a instancia do modulo USART */
struct usart_module usart_instance;

/**
* \fn usartConfig()
* \Configura e habilita o modulo USART definindo a baudrate como 9600
* \param[out] &usart_instance Endereco de memoria da instancia da USART configurada
*/
int usartConfig(){
	
	struct usart_config usart_conf;
	
	usart_get_config_defaults(&usart_conf);
	usart_conf.baudrate    = 9600;
	usart_conf.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	usart_conf.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	usart_conf.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	usart_conf.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	usart_conf.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	
	while (usart_init(&usart_instance,
	EDBG_CDC_MODULE, &usart_conf) != STATUS_OK) {
	}
	stdio_serial_init(&usart_instance, EDBG_CDC_MODULE, &usart_conf);
	
	usart_enable(&usart_instance);
	return &usart_instance;
}

/**
* \fn configure_usart_callbacks()
* \brief Funcao que define e inicializa a interrupcao causada pelo recebimento de 5 caracteres pela USART
*/
void configure_usart_callbacks(void)
{
	usart_register_callback(&usart_instance, usart_read_callback, USART_CALLBACK_BUFFER_RECEIVED);
	usart_enable_callback(&usart_instance, USART_CALLBACK_BUFFER_RECEIVED);
}
