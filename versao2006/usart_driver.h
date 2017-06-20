/*
 * usart_driver.h
 *
 * Created: 06/06/2017 15:49:34
 *  Author: Vinícius
 */ 


#ifndef USART_DRIVER_H_
#define USART_DRIVER_H_

int usartConfig(void);
void configure_usart_callbacks(void);
void usart_read_callback(struct usart_module *const usart_module);
void usart_write_callback(struct usart_module *const usart_module);



#endif /* USART_DRIVER_H_ */