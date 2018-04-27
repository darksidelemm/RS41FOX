#ifndef __DELAY_H_
#define __DELAY_H_
#ifdef __cplusplus
extern "C" {
#endif

/** Initialize delay core - configure SysTick timer */
void delay_init();

void _delay_us(uint16_t us, uint8_t precise);

void _delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif
#endif
