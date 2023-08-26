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
#include "sharp_bitmap.h"


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

// sharp_functions
int lcd_mem_init();
void clear_display();
void clear_display_buffer();
void refresh_display();
void drawPixel(uint16_t x, uint16_t y, uint16_t color);
void clear_pixels(uint16_t x_start, uint16_t x_end, uint16_t y_start, uint16_t y_end, uint8_t colour);
void draw_bitmap(const uint8_t* pBMP);
uint8_t swapByte(uint8_t value);
extern void print_data(const char* data);

//To draw character
void drawChar(uint16_t x, uint16_t y);
void drawInt(uint16_t x, uint16_t y, int value);
void drawFloat(uint16_t x, uint16_t y, float value, int numDecimals);
void floatToString(float value, char *result, int numDecimals);
void drawString(uint16_t x, uint16_t y, const char* str);




#endif