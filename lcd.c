#include "lcd.h"
// Port B is output port to the LCD
#define GPIO_PORTB_DATA_R (*((volatile unsigned long *)0x400053FC))
#define GPIO_PORTB_DIR_R (*((volatile unsigned long *)0x40005400))
#define GPIO_PORTB_AFSEL_R (*((volatile unsigned long *)0x40005420))
#define GPIO_PORTB_PUR_R (*((volatile unsigned long *)0x40005510))
#define GPIO_PORTB_DEN_R (*((volatile unsigned long *)0x4000551C))
#define GPIO_PORTB_CR_R (*((volatile unsigned long *)0x40005524))
#define GPIO_PORTB_AMSEL_R (*((volatile unsigned long *)0x40005528))
#define GPIO_PORTB_PCTL_R (*((volatile unsigned long *)0x4000552C))

// Port A is output port to the LCD
#define GPIO_PORTA_DATA_R (*((volatile unsigned long *)0x400043FC))
#define GPIO_PORTA_DIR_R (*((volatile unsigned long *)0x40004400))
#define GPIO_PORTA_AFSEL_R (*((volatile unsigned long *)0x40004420))
#define GPIO_PORTA_PUR_R (*((volatile unsigned long *)0x40004510))
#define GPIO_PORTA_DEN_R (*((volatile unsigned long *)0x4000451C))
#define GPIO_PORTA_CR_R (*((volatile unsigned long *)0x40004524))
#define GPIO_PORTA_AMSEL_R (*((volatile unsigned long *)0x40004528))
#define GPIO_PORTA_PCTL_R (*((volatile unsigned long *)0x4000452C))
#define SYSCTL_RCGC2_R (*((volatile unsigned long *)0x400FE108))

#define lcdRS (*((volatile unsigned long *)0x40004020)) //RS - PA3
#define lcdEN (*((volatile unsigned long *)0x40004010)) //EN - PA2
#define lcdDB (*((volatile unsigned long *)0x400050F0)) //DB4-7 are mapped to PB2-5
#define lcdDB (*((volatile unsigned long *)0x400050F0)) //DB4-7 are mapped to PB2-5

void LCD_port_init()
{
	unsigned long delay;
	//Init for PortB
	SYSCTL_RCGC2_R |= 0x00000002;	 // 1) b clock
	delay = SYSCTL_RCGC2_R;			 // delay
	GPIO_PORTB_CR_R |= 0x3C;		 // allow changes to PB2-PB5
	GPIO_PORTB_AMSEL_R &= 0x00;		 // 3) disable analog function
	GPIO_PORTB_PCTL_R &= 0x00000000; // 4) GPIO clear bit PCTL
	GPIO_PORTB_DIR_R |= 0x3C;		 // 5.2) PB2-PB5 as OUTPUTS
	GPIO_PORTB_AFSEL_R &= 0x00;		 // 6) no alternate function
	GPIO_PORTB_DEN_R |= 0x3C;		 // 7) enable digital pins to PB2-PB5

	//Init for PortA2-3
	SYSCTL_RCGC2_R |= 0x00000001;	 // 1) A clock
	delay = SYSCTL_RCGC2_R;			 // delay
	GPIO_PORTA_CR_R |= 0x0C;		 // allow changes to PA2-3
	GPIO_PORTA_AMSEL_R &= 0x00;		 // 3) disable analog function
	GPIO_PORTA_PCTL_R &= 0x00000000; // 4) GPIO clear bit PCTL
	GPIO_PORTA_DIR_R |= 0x0C;		 // 5.1) PA2-3 OUTPUTS
	GPIO_PORTA_AFSEL_R &= 0x00;		 // 6) no alternate function
	GPIO_PORTA_DEN_R |= 0x0C;		 // 7) enable digital pins PA2-3
}

//clear LCD
void LCD_clear()
{
	LCD_write_cmd(0x01);
	LCD_write_cmd(0x02);
	SysTick_Wait_ms(50); //"Home" LCD
}
unsigned char address_table[] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07}; //?????????http://www.51hei.com/bbs/dpj-129626-1.html

void LCD_en_write()
{ //set EN high for x time
	lcdEN |= 0x04;
	SysTick_Wait_ms(50);
	lcdEN &= ~0x04; //return en to 0
}

void LCD_write_cmd(unsigned char c)
{
	lcdRS = 0x00; //line low
	SysTick_Wait_ms(10);
	lcdDB = (c & 0xF0) >> 2; //send first high nible to PB2-5
	LCD_en_write();
	SysTick_Wait_ms(10);	 //10ms delay
	lcdDB = (c & 0x0F) << 2; //set low niblle
	LCD_en_write();
	SysTick_Wait_ms(10); //50ms delay
}
//write data to LCD
void LCD_write_data(unsigned char c)
{
	lcdRS = 0x08; //RS on for sending data
	SysTick_Wait_ms(10);
	lcdDB = (c & 0xF0) >> 2; //high nibble
	LCD_en_write();
	SysTick_Wait_ms(10);	 //50ms delay
	lcdDB = (c & 0x0F) << 2; //low nibble
	LCD_en_write();
	SysTick_Wait_ms(10); //50ms delay
	lcdRS = 0x00;
}
//write address
void LCD_set_xy(unsigned char x, unsigned char y)
{
	unsigned char address;
	if (y == 0)
	{
		address = 0x80 + x; //the first line
	}
	else
	{
		address = 0xc0 + x; // the second line
	}
	LCD_write_cmd(address);
	SysTick_Wait_ms(50);
}
//Write char to LCD
void LCD_print_char(unsigned char x, unsigned char y, unsigned char c)
{
	LCD_set_xy(x, y);
	LCD_write_data(c);
}
//write string to LCD
void LCD_print_string(unsigned char x, unsigned char y, unsigned char *s)
{
	if (x == 3)
	{
		LCD_clear();
		SysTick_Wait_ms(2); //delay 2 ms
		LCD_set_xy(0, y);
	}
	else
		LCD_set_xy(x, y);
	while (*s)
	{
		LCD_write_data(*s);
		s++;
	}
}

//intiate LCD
void LCD_init()
{
	lcdEN = 0x00 << 2;	 //set enable 0
	lcdDB = 0x00 << 2;	 //set data 0
	lcdRS = 0x00 << 2;	 //set RS 0
	SysTick_Wait_us(50); 
	lcdDB = 0x3 << 2;	 // cmd 1
	LCD_en_write();
	SysTick_Wait_us(50);
	lcdDB = 0x2 << 2; // cmd 2
	LCD_en_write();
	SysTick_Wait_us(40); //4 us
	//LCD_write_cmd(0x02); // //AC=0,光标、画面回HOME位, DDRAM内容不变
	SysTick_Wait_us(50); //wait 37us
	LCD_write_cmd(0x28); ////两行显示4-bits
	SysTick_Wait_us(50); //wait 37us
	//LCD_write_cmd(0x0c); //LCD开显示
	SysTick_Wait_us(50); //wait 37us
	LCD_write_cmd(0x06); //光标右移 字符不移动
	SysTick_Wait_us(50); //wait 37us
	LCD_write_cmd(0x0F);
	SysTick_Wait_us(50);
	LCD_write_cmd(0x00); //sets display pn, cursor on, blink on
	SysTick_Wait_us(50);
	LCD_write_cmd(0x04);
	SysTick_Wait_us(50);
	LCD_write_cmd(0x00); //entry mode: cursor to increment and display not to shift
	SysTick_Wait_us(40); //4 us
}

// extra part: display images
void LCD_display_image()
{
	unsigned char image_table[] = {
		0x03, 0x07, 0x0f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
		0x18, 0x1E, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
		0x07, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f,
		0x10, 0x18, 0x1c, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
		0x0f, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00,
		0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x0f, 0x07, 0x01,
		0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1c, 0x18, 0x00,
		0x1c, 0x18, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00}; // a heart image
	SysTick_Wait_ms(2);
	unsigned char address_table[] = {
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

	LCD_write_cmd(0x40); // input the image table
	int i;
	for (i = 0; i < 64; i++)
	{
		LCD_write_data(image_table[i]);
		SysTick_Wait_ms(2);
	}
	LCD_write_cmd(0x80);
	for (i = 0; i < 4; i++)
	{
		LCD_write_data(address_table[i]);
		SysTick_Wait_ms(2);
	} // the upper part of the image
	LCD_write_cmd(0xc0);
	for (i = 4; i < 8; i++)
	{
		LCD_write_data(address_table[i]);
		SysTick_Wait_ms(2);
	} // the lower part of image
}

void LCD_erase()
{
	LCD_write_cmd(0x10);
	LCD_write_data(' ');
	LCD_write_cmd(0x10);
}