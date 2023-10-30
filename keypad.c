#include "keypad.h"
// Port D is output port from keypad
#define GPIO_PORTD_DATA_R (*((volatile unsigned long *)0x400073FC))
#define GPIO_PORTD_DIR_R (*((volatile unsigned long *)0x40007400))
#define GPIO_PORTD_AFSEL_R (*((volatile unsigned long *)0x40007420))
#define GPIO_PORTD_PUR_R (*((volatile unsigned long *)0x40007510))
#define GPIO_PORTD_PDR_R (*((volatile unsigned long *)0x40007514))
#define GPIO_PORTD_DEN_R (*((volatile unsigned long *)0x4000751C))
#define GPIO_PORTD_LOCK_R (*((volatile unsigned long *)0x40007520))
#define GPIO_PORTD_CR_R (*((volatile unsigned long *)0x40007524))
#define GPIO_PORTD_AMSEL_R (*((volatile unsigned long *)0x40007528))
#define GPIO_PORTD_PCTL_R (*((volatile unsigned long *)0x4000752C))

#define SYSCTL_RCGC2_R (*((volatile unsigned long *)0x400FE108))

// Port E is input port to keypad
#define GPIO_PORTE_DATA_R (*((volatile unsigned long *)0x400243FC))
#define GPIO_PORTE_DIR_R (*((volatile unsigned long *)0x40024400))
#define GPIO_PORTE_AFSEL_R (*((volatile unsigned long *)0x40024420))
#define GPIO_PORTE_PUR_R (*((volatile unsigned long *)0x40024510))
#define GPIO_PORTE_PDR_R (*((volatile unsigned long *)0x40024514))
#define GPIO_PORTE_DEN_R (*((volatile unsigned long *)0x4002451C))
#define GPIO_PORTE_CR_R (*((volatile unsigned long *)0x40024524))
#define GPIO_PORTE_AMSEL_R (*((volatile unsigned long *)0x40024528))
#define GPIO_PORTE_PCTL_R (*((volatile unsigned long *)0x4002452C))
#define GPIO_PORTE_LOCK_R (*((volatile unsigned long *)0x40024520))

//Define all the PINS in PortD individually
#define PORTD0 (*((volatile unsigned long *)0x40007004))
#define PORTD1 (*((volatile unsigned long *)0x40007008))
#define PORTD2 (*((volatile unsigned long *)0x40007010))
#define PORTD3 (*((volatile unsigned long *)0x40007020))
//Define all the PINS in PortE individually
#define PORTE0 (*((volatile unsigned long *)0x40024004))
#define PORTE1 (*((volatile unsigned long *)0x40024008))
#define PORTE2 (*((volatile unsigned long *)0x40024010))
#define PORTE3 (*((volatile unsigned long *)0x40024020))
/* keypad_getkey() function returns the value of key pressed by the user by traversing columns
and rows respectivley */

void key_init()
{
  unsigned long delay;
  //Init for port D (output,columns) 0-3
  SYSCTL_RCGC2_R |= 0x00000008;    // 1) D clock
  delay = SYSCTL_RCGC2_R;          // delay
  GPIO_PORTD_LOCK_R = 0x4C4F434B;  // 2) unlock PortD
  GPIO_PORTD_CR_R |= 0x0F;         // allow changes to PD3-0
  GPIO_PORTD_AMSEL_R &= 0x00;      // 3) disable analog function
  GPIO_PORTD_PCTL_R &= 0x00000000; // 4) GPIO clear bit PCTL
  GPIO_PORTD_DIR_R |= 0x0F;        // 5.2) PD3-0 Outputs
  GPIO_PORTD_AFSEL_R &= 0x00;      // 6) no alternate function
  GPIO_PORTD_DEN_R |= 0x0F;        // 7) enable digital pins PD3-0
                                   //Init for port E (input,rows) 0-3
  SYSCTL_RCGC2_R |= 0x00000010;    // 1) E clock;
  delay = SYSCTL_RCGC2_R;          // delay
  GPIO_PORTE_LOCK_R = 0x4C4F434B;  // 2) unlock PortE
  GPIO_PORTE_CR_R |= 0x0F;         // allow changes to PE3-0
  GPIO_PORTE_AMSEL_R &= 0x00;      // 3) disable analog function
  GPIO_PORTE_PCTL_R &= 0x00000000; // 4) GPIO clear bit PCTL
  GPIO_PORTE_DIR_R &= ~0x0F;       // 5.1) PE3-0 Inputs
  GPIO_PORTE_AFSEL_R &= 0x00;      // 6) no alternate function
  GPIO_PORTE_PDR_R |= 0x0F;        // enable pulldown resistors on PFE3-0
  GPIO_PORTE_DEN_R |= 0x0F;        // 7) enable digital pins PE3-0
}
char key_scan(void)
{
  //the keypad matrix
  char symbol_table[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
    };
  for(int i = 0; i < 4; i++)    //Scan columns loop
  {
    GPIO_PORTD_DATA_R = (1U << i);
    SysTick_Wait_us(200);
    for(int j = 0; j < 4; j++)  //Scan rows
    {
      if((GPIO_PORTE_DATA_R ) & (1U << j))
        return symbol_table[j][i];
    }
  }
  return 'N';//defult return NULL key
}
// 1U << 0 = 1 = 0x01
// 1U << 1 = 2 = 0x02
// 1U << 2 = 4 = 0x04
// 1U << 3 = 8 = 0x08
// etc...