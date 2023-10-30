#ifndef LCD
#define LCD

#include "PLL.h"

//enable the lcd
void LCD_en_write(void);
//current address
unsigned char LCD_current_add(void);
//write command to LCD
void LCD_write_cmd(unsigned char);
//write data to LCD
void LCD_write_data(unsigned char);
//write address 
void LCD_set_xy(unsigned char, unsigned char);
//Write char to LCD
void LCD_print_char(unsigned char, unsigned char, unsigned char);
//write string to LCD
void LCD_print_string(unsigned char, unsigned char, unsigned char*);
//clear LCD
void LCD_clear(void);
//intiate LCD
void LCD_init(void);
//initiate the port
void LCD_port_init(void);
//erase the last input
void LCD_erase();
// extra part: display images
void LCD_display_image(void);

#endif