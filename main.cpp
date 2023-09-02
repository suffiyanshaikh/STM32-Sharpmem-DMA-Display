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
#include <cstdint>
#include <inttypes.h>


/* Private includes ----------------------------------------------------------*/
#include "spi_dma.hpp"
#include "sharp_lcd.hpp"
#include "sharp_bitmap.h"

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define SAMPLE_TIME_MS   200

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


    // Request the shared queue
    EventQueue *stats_queue = mbed_event_queue();
    stats_queue->call_every(SAMPLE_TIME_MS, display_system_stats);



    // while (true) {

    // display_system_stats();
    // ThisThread::sleep_for(1000);


    // // draw_lines(); 
    // // draw_rectangle();

    // }
}

void print_data(const char* data)
{
    serial_port.write(data,strlen(data));
}

void display_system_info() {

//----------- SYS Details ---------------//
mbed_stats_sys_t sys_info;

mbed_stats_sys_get(&sys_info);

uint32_t mbed_major = sys_info.os_version / 10000;
uint32_t mbed_minor = (sys_info.os_version / 100) % 100;
uint32_t mbed_patch = sys_info.os_version % 100;

sprintf(data,"Mbed_OS_Ver: %u.%u.%u",mbed_major,mbed_minor,mbed_patch);
drawString( 10, 60,data); //y+20


sprintf(data,"CPU_ID: 0x%" PRIx32, sys_info.cpu_id);
drawString( 10, 80,data); //y+20


sprintf(data,"RAM_Size: %d KB", (sys_info.ram_size[0]+sys_info.ram_size[1])/1024);
drawString( 10, 100,data); //y+20


sprintf(data,"ROM_Size: %d MB", (sys_info.rom_size[0] + sys_info.rom_size[1])/1048576);
drawString( 10, 120,data); //Y+20

}


void display_system_stats() {

    mbed_stats_cpu_t cpu_stats;
    mbed_stats_heap_t heap_info;
    mbed_stats_stack_t stack_info;

    //----------- CPU USAGE ----------------------//
    mbed_stats_cpu_get(&cpu_stats);

    // Calculate the percentage of CPU usage
    uint64_t diff_usec = (cpu_stats.idle_time - prev_idle_time);
    uint8_t idle = (diff_usec * 100) / (SAMPLE_TIME_MS*1000);
    uint8_t usage = 100 - ((diff_usec * 100) / (SAMPLE_TIME_MS*1000));
    prev_idle_time = cpu_stats.idle_time;
    
    sprintf(data,"Up_Time: %lld sec", cpu_stats.uptime/1000000);
    drawString( 200, 60,data); //Y+20

    sprintf(data,"Idle_Time: %lld sec", cpu_stats.idle_time/1000000);
    drawString( 200, 80,data); //Y+20

    sprintf(data,"Sleep_Time: %lld sec", cpu_stats.sleep_time/1000000);
    drawString( 200, 100,data); //Y+20

    sprintf(data,"DeepSleep: %lld sec", cpu_stats.deep_sleep_time/1000000);
    drawString( 200, 120,data); //Y+20

    sprintf(data,"Idle: %d%% Usage: %d%%", idle, usage);
    drawString( 200, 140,data); //Y+20

    //----------- Memory USAGE ----------------------//

    //heap memory
    mbed_stats_heap_get(&heap_info); 

    sprintf(data,"heap size: %d B", heap_info.reserved_size );
    drawString( 10, 180,data);


    sprintf(data,"heap used: %d B", heap_info.current_size );
    drawString( 10, 200,data);

    // sprintf(data,"Max bytes allocated at a given time: %d Kb\n", heap_info.max_size / 1000);
    // serial_port.write(data,strlen(data));

    sprintf(data,"heap free: %d B", (heap_info.reserved_size - heap_info.current_size));
    drawString( 10, 220,data);

   
    //stack memory
    mbed_stats_stack_get(&stack_info); 

    sprintf(data,"stack stack: %d B", stack_info.reserved_size);
    drawString( 200, 180,data);

    sprintf(data,"stack size: %d B", stack_info.max_size);
    drawString( 200, 200,data);

    sprintf(data,"stack free: %d B", (stack_info.reserved_size - stack_info.max_size));
    drawString( 200, 220,data);

    refresh_display();
}

void display_static_page() {

drawString( 100, 10,"***** SYSTEM STATS ****");
drawString( 10, 40,"-- System Info --");
drawString( 200, 40,"-- CPU Usage --");
drawString( 100, 160,"-- Memory Usage --");

}



