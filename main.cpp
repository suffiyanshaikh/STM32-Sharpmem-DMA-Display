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
#include <inttypes.h>


/* Private includes ----------------------------------------------------------*/
#include "spi_dma.hpp"
#include "sharp_lcd.hpp"
#include "sharp_bitmap.h"

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define SAMPLE_TIME_MS   1000

/* Private variables ---------------------------------------------------------*/
char data[100];
uint64_t prev_idle_time = 0;



/* Private function prototypes -----------------------------------------------*/
void print_data(const char* data);
void display_system_info();
void display_static_page();
void display_system_stats();



/* Peripherals Init ---------------------------------------------------------*/
static BufferedSerial serial_port(USBTX, USBRX);
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

    display_static_page();

    display_system_info();


    while (true) {

    display_system_stats();
    ThisThread::sleep_for(1000);


    // draw_lines(); 
    // draw_rectangle();

    }
}

void print_data(const char* data)
{
    serial_port.write(data,strlen(data));
}

void display_system_info() {

//----------- SYS Details ---------------//
mbed_stats_sys_t sys_info;

mbed_stats_sys_get(&sys_info);

sprintf(data,"/*-------SYSTEM DETAILS-------*/\n");
serial_port.write(data,strlen(data));

sprintf(data,"Mbed OS Version: %" PRId32 "\n", sys_info.os_version);
serial_port.write(data,strlen(data));

sprintf(data,"CPU ID: 0x%" PRIx32 "\n\n", sys_info.cpu_id);
serial_port.write(data,strlen(data));

sprintf(data,"RAM Size: %d KB \n", (sys_info.ram_size[0]+sys_info.ram_size[1])/1024);
serial_port.write(data,strlen(data));

sprintf(data,"ROM Size: %d MB \n", (sys_info.rom_size[0] + sys_info.rom_size[1])/1048576);
serial_port.write(data,strlen(data));

}


void display_system_stats() {

    mbed_stats_cpu_t cpu_stats;
    mbed_stats_heap_t heap_info;
    mbed_stats_stack_t stack_info[1];


    //----------- CPU USAGE ----------------------//
    mbed_stats_cpu_get(&cpu_stats);

    sprintf(data,"/*------CPU USAGE--------*/\n");
    serial_port.write(data,strlen(data));

    // Calculate the percentage of CPU usage
    uint64_t diff_usec = (cpu_stats.idle_time - prev_idle_time);
    uint8_t idle = (diff_usec * 100) / (SAMPLE_TIME_MS*1000);
    uint8_t usage = 100 - ((diff_usec * 100) / (SAMPLE_TIME_MS*1000));
    prev_idle_time = cpu_stats.idle_time;
    
    sprintf(data,"Time(secs): Up: %lld", cpu_stats.uptime/1000000);
    serial_port.write(data,strlen(data));

    sprintf(data,"   Idle: %lld", cpu_stats.idle_time/1000000);
    serial_port.write(data,strlen(data));

    sprintf(data,"   Sleep: %lld", cpu_stats.sleep_time/1000000);
    serial_port.write(data,strlen(data));

    sprintf(data,"   DeepSleep: %lld\n", cpu_stats.deep_sleep_time/1000000);
    serial_port.write(data,strlen(data));

    sprintf(data,"Idle: %d%% Usage: %d%%\n\n", idle, usage);
    serial_port.write(data,strlen(data));

    //----------- Memory USAGE ----------------------//

    mbed_stats_heap_get(&heap_info); //heap memory
    sprintf(data,"/*----------MEMORY USAGE------*/\n");
    serial_port.write(data,strlen(data));

    sprintf(data,"Bytes allocated currently: %d Kb\n", heap_info.current_size / 1000);
    serial_port.write(data,strlen(data));

    sprintf(data,"Max bytes allocated at a given time: %d Kb\n", heap_info.max_size / 1000);
    serial_port.write(data,strlen(data));

    sprintf(data,"Cumulative sum of bytes ever allocated: %d Kb\n", heap_info.total_size / 1000);
    serial_port.write(data,strlen(data));

    sprintf(data,"Current number of bytes allocated for the heap: %d Kb\n", heap_info.reserved_size / 1000);
    serial_port.write(data,strlen(data));

    sprintf(data,"Current number of allocations: %d\n", heap_info.alloc_cnt);
    serial_port.write(data,strlen(data));

    sprintf(data,"Number of failed allocations: %d\n", heap_info.alloc_fail_cnt);
    serial_port.write(data,strlen(data));

    mbed_stats_stack_get(&stack_info[0]); //stack memory

    sprintf(data,"Maximum number of bytes used on the stack: %d kb\n", stack_info[0].max_size/1000);
    serial_port.write(data,strlen(data));

    sprintf(data,"Current number of bytes allocated for the stack: %d Kb\n\n", stack_info[0].reserved_size/1000);
    serial_port.write(data,strlen(data));

}

void display_static_page() {

drawString( 200, 5,"SYSTEM STATS"); //
drawString( 10, 25,"SYSTEM INFO");



}



