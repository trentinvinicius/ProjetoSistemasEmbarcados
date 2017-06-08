/*
 * eeprom_driver.c
 *
 * Created: 06/06/2017 16:08:35
 *  Author: Vinícius
 */ 

#if 0
void configure_eeprom(void)
{
 /* Setup EEPROM emulator service */
 enum status_code error_code = rww_eeprom_emulator_init();
 if (error_code == STATUS_ERR_NO_MEMORY) {
 while (true) {
 /* No EEPROM section has been set in the device's fuses */
 }
 }
 else if (error_code != STATUS_OK) {
 /* Erase the emulated EEPROM memory (assume it is unformatted or
 * irrecoverably corrupt) */
 rww_eeprom_emulator_erase_memory();
 rww_eeprom_emulator_init();
 }
}
#if (SAMD21) || (SAMDA1)
void SYSCTRL_Handler(void)
{
 if (SYSCTRL->INTFLAG.reg & SYSCTRL_INTFLAG_BOD33DET) {
 SYSCTRL->INTFLAG.reg |= SYSCTRL_INTFLAG_BOD33DET;
 rww_eeprom_emulator_commit_page_buffer();
 }
}
#endif
static void configure_bod(void)
{
#if (SAMD21) || (SAMDA1)
 struct bod_config config_bod33;
 bod_get_config_defaults(&config_bod33);
 config_bod33.action = BOD_ACTION_INTERRUPT;
 /* BOD33 threshold level is about 3.2V */
 config_bod33.level = 48;
 bod_set_config(BOD_BOD33, &config_bod33);
 bod_enable(BOD_BOD33);
 SYSCTRL->INTENSET.reg |= SYSCTRL_INTENCLR_BOD33DET;
 system_interrupt_enable(SYSTEM_INTERRUPT_MODULE_SYSCTRL);
 #endif
 }
 #endif