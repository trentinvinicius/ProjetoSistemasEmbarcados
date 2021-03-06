﻿#ifndef USART_DRIVER_H_
#define USART_DRIVER_H_

int usartConfig(void);
void configure_usart_callbacks(void);
void usart_read_callback(struct usart_module *const usart_module);
void usart_write_callback(struct usart_module *const usart_module);

#endif 