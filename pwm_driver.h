/*
 * pwm_driver.h
 *
 * Created: 08/06/2017 09:45:04
 *  Author: Vinícius
 */ 


#ifndef PWM_DRIVER_H_
#define PWM_DRIVER_H_


void configure_pwm(void);
void set_pwm_compare_value(int32_t valor);
int get_pwm_compare_value(void);




#endif /* PWM_DRIVER_H_ */