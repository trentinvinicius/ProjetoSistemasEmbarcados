#ifndef TCC_DRIVER_H_
#define TCC_DRIVER_H_

void configure_tcc(void);
void configure_tcc_callbacks(void);
void tcc_callback_timer(struct tcc_module *const module_inst);
int getTime(void);
void stopTime(void);
void setTime(int tempo);


#endif