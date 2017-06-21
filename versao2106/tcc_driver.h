/*
 * tcc_driver.h
 *
 * Created: 06/06/2017 16:05:38
 *  Author: Vinícius
 */ 


#ifndef TCC_DRIVER_H_
#define TCC_DRIVER_H_

void configure_tcc(void);
void configure_tcc_callbacks(void);
void tcc_callback_pwm(struct tcc_module *const module_inst);
int getTime(void);
void stopTime(void);
void setTime(int tempo);



#endif /* TCC_DRIVER_H_ */