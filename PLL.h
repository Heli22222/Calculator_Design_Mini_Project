#ifndef PLL
#define PLL

void SysTick_Init(void);
void PLL_Init(unsigned long);
// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(unsigned long delay);
void SysTick_Wait_ms(unsigned long delay_ms);
void SysTick_Wait_us(unsigned long delay_us);
int number(void);
#endif
