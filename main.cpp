/**
 * ************************************************************
 * @file   : main.cpp
 * @author : suffiyan shaikh
 * @brief  : main source code
 * @version: 0.1
 * @date   : Jul, 2023
 * ************************************************************
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "mbed.h"

/* Private includes ----------------------------------------------------------*/
#include "spi_dma.hpp"
#include "sharp_lcd.hpp"
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
char data[100];

/* Private function prototypes -----------------------------------------------*/
void print_data(const char* data);

/* Peripherals Init ---------------------------------------------------------*/
static BufferedSerial serial_port(PA_9, PA_10);
DigitalOut debug_led(PA_5);

/* Private user code ---------------------------------------------------------*/

// main() runs in its own thread in the OS
int main()
{
    
    serial_port.set_baud(9600);
    serial_port.set_format(
        /* bits */ 8,
        /* parity */ BufferedSerial::None,
        /* stop bit */ 1
    );


    //Test debug led
    debug_led.write(1);
    ThisThread::sleep_for(500ms);
    debug_led.write(0);

    print_data("SHARP_MEM_LCD\n");
    debug_led.write(0);


    //---- display begin---// 

    if(lcd_mem_init())
        print_data("Memory Init OK\n");


    //-----init SPI_DMA---//

    if(spi_dma_init())
        print_data("SPI_DMA Init OK\n");

    
    clear_display();

    while (true) {

    draw_lines(); 
    draw_rectangle();

    }
}

void print_data(const char* data)
{
    serial_port.write(data,strlen(data));
}



