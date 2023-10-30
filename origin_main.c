#include "TExaS.h"
#include "lcd.h"
#include "PLL.h"
#include "keypad.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FLASH.h"
#define PASSWORD 123

// // //status of calculator
int shifted;     //1 means shifted , 0 means unshifted
 char OPERAND[4]; // + - x / del clear
 char  current_c;  //current char presse
 float NUMBERS[5]; //store all the numbers
 char NUMBER[6]; // store the current number
 int PRIORITY[4]; // store the priority of operand
 float ANSWER; // final value to display
 int index_operand; //index of operand vector
 int index_numbers; //index of all numbers vector
 int index_number; //index of current number vector
char password_string[10]; // the string form of password
int password_int;  // the int form of password

void global_init(void); // the function to initiate all the variables
void main_init(void); // initiate all the clock , prots and all the variables
void check_password(void); // check the entered password is correct or not
void process_input(void); // process the input number and the input operand 
void process_number(char input); // the function which push the character to the current numebr char
void calculation(void); // perform all the calculation of numbers of vector

int main()
{
  main_init();
  check_password();
  while (1)
  {
    process_input();
    calculation();
    global_init();
  }
}


void main_init()
{
  SysTick_Init();
  PLL_Init(16);
  key_init();
  LCD_port_init();
  LCD_init();
  global_init();
  Flash_Enable();
}

void process_input()
{
  int input_finished = 0;
  shifted = 0;
  while (!input_finished)
  {
    current_c = key_scan();
    SysTick_Wait_ms(3000);
    if (current_c != 'N')
    {
      switch (current_c)
      {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        LCD_write_data(current_c);
        NUMBER[index_number++] = current_c;
        break;
      case 'A':
        if (shifted == 0)
        {
          LCD_write_data('+');
          OPERAND[index_operand++] = '+';
        }
        else
        {
          LCD_write_data('x');
          OPERAND[index_operand++] = 'x';
        }
        NUMBERS[index_numbers++] = atof(NUMBER);
        memset(NUMBER, 0, sizeof(NUMBER));
        index_number = 0;
        break;
      case 'B':
        if (shifted == 0)
        {
          LCD_write_data('-');
          OPERAND[index_operand++] = '-';
        }
        else
        {
          LCD_write_data('/');
          OPERAND[index_operand++] = '/';
        }
        NUMBERS[index_numbers++] = atof(NUMBER);
        memset(NUMBER, 0, sizeof(NUMBER));
        index_number = 0;
        break;
      case 'C':
        if (shifted == 0)
        {
          LCD_write_data('.');
          NUMBER[index_number++] = '.';
        }
        else
        {
          LCD_write_data('E');
          NUMBER[index_number++] = 'E';
        }
        break;
      case 'D':
        shifted = ~shifted;
        break;
      case '*':
        NUMBERS[index_numbers++] = atof(NUMBER);
        memset(NUMBER, 0, sizeof(NUMBER));
        index_number = 0;
        input_finished = 1;
        break;
      case '#':
        if (!shifted)
        {
          if ((strcmp(NUMBER, "")) != 0)
          { // strcmp: if identical return 0
            LCD_erase();
            NUMBER[index_number - 1] = '\0';
            index_number = index_number - 1;
          }
          else
          { // if the NUMBER is empty, clear the OPERAND
            LCD_erase();
            OPERAND[index_operand - 1] = '+';
            index_operand--;
          }
        }
        else
        {
          LCD_clear();
          global_init();
        }
        break;
      default:
        break;
      }
    }
  }
}

void calculation()
{
  int calculation_finished = 0;
  char buffer[15];
  for (int i = 0; i < index_operand; i++)
  {
    if (OPERAND[i] == '-')
    {
      PRIORITY[i] = 1;
    }
    else if (OPERAND[i] == 'x')
    {
      PRIORITY[i] = 2;
    }
    else if (OPERAND[i] == '/')
    {
      PRIORITY[i] = 3;
    }
  }
  for (int i = 0; i < index_operand; i++)
  {
    if (PRIORITY[i] == 3)
    {
      NUMBERS[i] = NUMBERS[i] / NUMBERS[i + 1];
      NUMBERS[i + 1] = 0;
      OPERAND[i] = '+';
    }
    else if (PRIORITY[i] == 2)
    {
      NUMBERS[i] = NUMBERS[i] * NUMBERS[i + 1];
      NUMBERS[i + 1] = 0;
      OPERAND[i] = '+';
    }
  }
  for (int i = 0; i < index_operand; i++)
  {
    if (PRIORITY[i] == 1)
    {
      NUMBERS[i] = NUMBERS[i] - NUMBERS[i + 1];
      NUMBERS[i + 1] = 0;
      OPERAND[i] = '+';
    }
  }
  for (int i = 0; i < index_numbers; i++)
  {
    ANSWER = ANSWER + NUMBERS[i];
  }
  LCD_set_xy(0, 1);
  LCD_write_data('=');
  LCD_write_data(' ');
  sprintf(buffer, "%f", ANSWER);
  LCD_print_string(2, 1, buffer);
  while (~calculation_finished)
  {
    current_c = key_scan();
    if (current_c == '#')
    {
      calculation_finished = !calculation_finished;
      LCD_clear();
      SysTick_Wait_ms(2000);
      break;
    }
  }
}


void global_init()
{
  shifted = 0;
  ANSWER = 0;
  memset(NUMBER, 0, sizeof(NUMBER));
  memset(NUMBERS, 0.0, sizeof(NUMBERS));
  memset(OPERAND, '+', sizeof(OPERAND));
  memset(PRIORITY, 0, sizeof(PRIORITY));
  index_number = 0;
  index_numbers = 0;
  index_operand = 0;
  memset(password_string, 0, sizeof(password_string));
  password_int = -1;
}
void check_password()
{
  int right_password = 0;
  int cursor = 0;
  while (!right_password)
  {
    LCD_print_string(0, 0, "Password please:");
    SysTick_Wait_ms(1000);
    current_c = key_scan();
    SysTick_Wait_ms(2000);
    //current_c_new = key_scan();
    if (current_c != 'N')
    {
      if (current_c != '#') // # means the end of input
      {
        LCD_set_xy(cursor, 1);
        LCD_write_data('*');
        cursor++;
        strcat(password_string, &current_c);
      }
      else
      {
        LCD_clear();
        password_int = atoi(password_string);
        cursor = 0;
        if (password_int != PASSWORD)
        {
          LCD_print_string(0, 0, "Wrong password!");
          SysTick_Wait_ms(5000);
          SysTick_Wait_ms(5000);
          LCD_clear();
          password_int = 0;
          memset(password_string, 0, sizeof(password_string));
        }
        else
        {
          LCD_print_string(0, 0, "Right password!");
          SysTick_Wait_ms(5000);
          SysTick_Wait_ms(5000);
          LCD_clear();
          LCD_display_image();
          LCD_print_string(4, 0, "Welcom! :)");
          LCD_print_string(4, 1, "HFD designs");
          SysTick_Wait_ms(5000);
          SysTick_Wait_ms(5000);
          SysTick_Wait_ms(5000);
          SysTick_Wait_ms(5000);
          SysTick_Wait_ms(5000);
          SysTick_Wait_ms(5000);
          LCD_clear();
          right_password = 1;
        }
      }
    }
  }
  password_int = 0;
  memset(password_string, 0, sizeof(password_string));
}


