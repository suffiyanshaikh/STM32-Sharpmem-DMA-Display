/**
 * ************************************************************
 * @file   : sharp_lcd.hpp
 * @author : suffiyan
 * @brief  : sharp_lcd header file
 * @version: 0.1
 * @date   : Jul, 2023
 * ************************************************************
 */

#ifndef SHARP_LCD_H
#define SHARP_LCD_H 

/* Includes ------------------------------------------------------------------*/
#include "mbed.h"
#include "mbed_error.h"
#include "mbed_fault_handler.h"

/* Private includes ----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/**
* The memory LCD excepts the LSB first
**/
#define SHARPMEM_BIT_WRITECMD (0x80)   // << 7 0x01 
#define SHARPMEM_BIT_VCOM (0x40)      // << 4 0x04 
#define SHARPMEM_BIT_CLEAR (0x20)    // <<3  0x02 

#define SHARPMEM_WIDTH   400
#define SHARPMEM_HEIGHT  240

#define BLACK 0
#define WHITE 1

#define SHARP_SS    PA_15 //Sharp cs,active high

#define pgm_read_byte(addr) (*(const unsigned char *)(addr))

#define TOGGLE_VCOM                                                            \
do {                                                                         \
    sharpmem_vcom = sharpmem_vcom ? 0x00 : SHARPMEM_BIT_VCOM;                \
} while (0);

/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/

// draw patterns
void draw_bar();
void draw_lines();
void draw_rectangle();

// display_functions
int lcd_mem_init();
void clear_display();
void refresh_display();
void drawPixel(int16_t x, int16_t y, uint16_t color);
uint8_t swapByte(uint8_t value);
extern void print_data(const char* data);



#endif