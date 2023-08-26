/**
 * ************************************************************
 * @file   : sharp_lcd.cpp
 * @author : suffiyan shaikh
 * @brief  : sharp lcd source file
 * @version: 0.1
 * @date   : Aug, 2023
 * ************************************************************
 */

/* Includes ------------------------------------------------------------------*/


/* Private includes ----------------------------------------------------------*/
#include "sharp_lcd.hpp"
#include "spi_dma.hpp"
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/


uint8_t *sharpmem_buffer = NULL;
uint8_t sharpmem_vcom = 0;

HAL_StatusTypeDef err;
extern SPI_HandleTypeDef hspi;
extern EventFlags dma_flag;


/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/
DigitalOut sharplcd_cs(SHARP_SS);


/* User defined functions ----------------------------------------------------*/

int lcd_mem_init(void) {

  // Set the vcom bit to a defined state
  sharpmem_vcom = SHARPMEM_BIT_VCOM; //0x02

  sharpmem_buffer = (uint8_t *)malloc((SHARPMEM_WIDTH * SHARPMEM_HEIGHT) / 8);
  MBED_ASSERT(sharpmem_buffer);
  
  //put cs in default state
  sharplcd_cs.write(0);

  return true;
}

void clear_display() {

    memset(sharpmem_buffer, 0xff, (SHARPMEM_WIDTH * SHARPMEM_HEIGHT) / 8);
    sharplcd_cs.write(1);

    sharpmem_vcom = SHARPMEM_BIT_VCOM; //0x02

    /**
    * clear screen command
    *   0V10 0000 0000 0000  
    *   command    trailer     
    */
    uint8_t clear_data[2] = {(uint8_t)(sharpmem_vcom | SHARPMEM_BIT_CLEAR),0x00};
    
    err = HAL_SPI_Transmit(&hspi,clear_data,sizeof(clear_data),100);
    if(err)
        print_data("lcd clear!");

    TOGGLE_VCOM;// 0x00
    sharplcd_cs.write(0);
}

/**
* @brief copy the image bitmap data into memory display
*
* @param pBMP
* @return int
*/
void draw_bitmap(const uint8_t *pBMP) {

    clear_display_buffer();
    memcpy(sharpmem_buffer, pBMP, (SHARPMEM_WIDTH * SHARPMEM_HEIGHT) / 8);
}

/**
* @brief clear memory buffer of lcd
*
*/
void clear_display_buffer() {

    memset(sharpmem_buffer, 0xff, (SHARPMEM_WIDTH * SHARPMEM_HEIGHT) / 8);
}

/**
* @brief set pixels to print boxes
*
* @param bin
* @param colour
*/
void clear_pixels(uint16_t x_start, uint16_t x_end, uint16_t y_start, uint16_t y_end, uint8_t colour) {

    for (uint16_t column = y_start; column <= y_end; column++) {

        // print horizontal lines keeping y constant
        for (uint16_t row = x_start; row <= x_end; row++) {
            drawPixel(row, column, colour);
        }
    }
}

void refresh_display() {

    uint16_t i, currentline;

    sharplcd_cs.write(1); // CS High

    uint8_t write_cmd[1] = {(uint8_t)(sharpmem_vcom | SHARPMEM_BIT_WRITECMD)}; //01

    err = HAL_SPI_Transmit(&hspi,write_cmd,sizeof(write_cmd),100);

    TOGGLE_VCOM;//0x02

    uint8_t bytes_per_line = SHARPMEM_WIDTH / 8; //50 bytes
    uint16_t totalbytes = (SHARPMEM_WIDTH * SHARPMEM_HEIGHT) / 8; //12k
    uint8_t* line = (uint8_t*)malloc((bytes_per_line + 2) * sizeof(uint8_t));


    //Transmit data line by line using DMA
    for (i = 0; i < totalbytes; i += bytes_per_line) {
        uint8_t line[bytes_per_line + 2];

        // Send address byte
        currentline = ((i + 1) / (SHARPMEM_WIDTH / 8)) + 1;
        
        line[0] = swapByte(currentline);


        // copy over this line
        memcpy(line + 1, sharpmem_buffer + i, bytes_per_line);
        
        // alternate way to Copy data from sharpmem_buffer to line buffer
        // for (int j = 0; j < bytes_per_line; j++) {
        //     line[j + 1] = sharpmem_buffer[i + j];
        // }

        // Send end of line
        line[bytes_per_line + 1] = 0x00;


        //Start DMA Transfer
        err = HAL_SPI_Transmit_DMA(&hspi,line,bytes_per_line+2);
            
            dma_flag.wait_any(DMA_Tx_cplt);

    
        //Wait to end DMA Transfer

    } 

    free(line);


    // Send another trailing 8 bits for the last line
    uint8_t end_cmd[1] = {0x00};

    err = HAL_SPI_Transmit(&hspi,end_cmd,1,100);
    sharplcd_cs.write(0);
}

/**
 * @brief draw single pixel in memory
 *
 * @param x
 * @param y
 * @param color
 */
void drawPixel(uint16_t x, uint16_t y, uint16_t color) {

    uint16_t buffer_index = (y * SHARPMEM_WIDTH + x) / 8;
    uint8_t bit_position = x % 8;

    // Calculate the bit mask for the specific pixel position
    uint8_t pixel_mask = (uint8_t)(1 << (7 - bit_position));

    if (color) {
        // Set the pixel by ORing with the set bit mask
        sharpmem_buffer[buffer_index] |= pixel_mask;
    } else {
        // Clear the pixel by ANDing with the clear bit mask
        // sharpmem_buffer[buffer_index] &= clr[bit_position];
        sharpmem_buffer[buffer_index] &= ~pixel_mask;
    }
}

uint8_t swapByte(uint8_t value) {
  uint8_t result = 0;
  for (int i = 0; i < 8; i++) {
    result |= ((value >> i) & 0x01) << (7 - i);
  }
  return result;
}

void draw_bar() {

    //Trace vertical line
    for(int y=30;y<240;y++){
    drawPixel(30,y, BLACK);}
    refresh_display();

    //Trace bottom horizontal line
    for(int x=30;x<100;x++){
    drawPixel(x,30, BLACK);}
    refresh_display();


    //Trace vertical line
    for(int y=30;y<240;y++){
    drawPixel(100,y, BLACK);}
    refresh_display();
    //---------------

    //Trace vertical line
    for(int y=30;y<240;y++){
    drawPixel(150,y, BLACK);}
    refresh_display();

    //Trace Top horizontal line
    for(int x=150;x<220;x++){
    drawPixel(x,30, BLACK);}
    refresh_display();


    //Trace vertical line
    for(int y=30;y<240;y++){
    drawPixel(220,y, BLACK);}
    refresh_display();

//-----------------------------

    //Trace vertical line
    for(int y=100;y<240;y++){
    drawPixel(270,y, BLACK);}
    refresh_display();

    //Trace Top horizontal line
    for(int x=270;x<340;x++){
    drawPixel(x,100, BLACK);}
    refresh_display();

    //Trace vertical line
    for(int y=100;y<240;y++){
    drawPixel(340,y, BLACK);}
    refresh_display();

    thread_sleep_for(500);
    clear_display();
}

void draw_lines() 
{

// plot xlines
for(int x=1;x<400;x++){
drawPixel(x,20, BLACK);}
refresh_display();

for(int x=1;x<400;x++){
drawPixel(x,40, BLACK);}
refresh_display();
    
for(int x=1;x<400;x++){
drawPixel(x,60, BLACK);}
refresh_display();

for(int x=1;x<400;x++){
drawPixel(x,80, BLACK);}
refresh_display();

for(int x=1;x<400;x++){
drawPixel(x,100, BLACK);}
refresh_display();

for(int x=1;x<400;x++){
drawPixel(x,120, BLACK);}
refresh_display();

for(int x=1;x<400;x++){
drawPixel(x,140, BLACK);}
refresh_display();

for(int x=1;x<400;x++){
drawPixel(x,160, BLACK);}
refresh_display();

for(int x=1;x<400;x++){
drawPixel(x,180, BLACK);}
refresh_display();

for(int x=1;x<400;x++){
drawPixel(x,200, BLACK);}
refresh_display();

for(int x=1;x<400;x++){
drawPixel(x,240, BLACK);}
refresh_display();

//plot ylines
for(int y=1;y<240;y++){
drawPixel(30,y, BLACK);}
refresh_display();


for(int y=1;y<240;y++){
drawPixel(60,y, BLACK);}
refresh_display();


for(int y=1;y<240;y++){
drawPixel(90,y, BLACK);}
refresh_display();


for(int y=1;y<240;y++){
drawPixel(120,y, BLACK);}
refresh_display();

for(int y=1;y<240;y++){
drawPixel(150,y, BLACK);}
refresh_display();

for(int y=1;y<240;y++){
drawPixel(180,y, BLACK);}
refresh_display();

for(int y=1;y<240;y++){
drawPixel(210,y, BLACK);}
refresh_display();

for(int y=1;y<240;y++){
drawPixel(240,y, BLACK);}
refresh_display();

for(int y=1;y<240;y++){
drawPixel(270,y, BLACK);}
refresh_display();

for(int y=1;y<240;y++){
drawPixel(300,y, BLACK);}
refresh_display();

ThisThread::sleep_for(500ms);
clear_display();

}

void draw_rectangle() {

//Trace vertical line
for(int y=50;y<150;y++){
drawPixel(50,y, BLACK);}
refresh_display();

for(int y=50;y<150;y++){
drawPixel(250,y, BLACK);}
refresh_display();

//Trace Top horizontal line
for(int x=50;x<250;x++){
drawPixel(x,50, BLACK);}
refresh_display();

for(int x=50;x<250;x++){
drawPixel(x,150, BLACK);}
refresh_display();

ThisThread::sleep_for(500ms);
clear_display();

}


/**
* @brief display single character on screen
*
* @param x
* @param y
* @param character
*/
void drawChar(uint16_t x, uint16_t y, char character) {

    if (character >= 32 && character <= 126) {
        const uint8_t *charData = Font7x10[character - 32]; // Adjust ASCII offset (Font7x10 starts from ASCII 32)

        for (uint16_t row = 0; row < 10; row++) { // 10 rows for 7x10 font

            for (uint16_t col = 0; col < 7; col++) { // 7 columns for 7x10 font
                if (charData[row] & (1 << col)) {
                    drawPixel(x + col, y + row, 0); // Draw a pixel at (x + col, y + row)
                } else {
                    drawPixel(x + col, y + row, 1); // Clear a pixel at (x + col, y + row)
                }
            }
        }
    }
}


/**
* @brief display int value on screen
*
* @param x
* @param y
* @param value
*/
void drawInt(uint16_t x, uint16_t y, int value) {

    // Convert the float value to a string using snprintf
    char valueString[10]; // Assuming the string won't exceed 10 characters
    snprintf(valueString, sizeof(valueString), "%d", value);

    // Display each character of the value string one at a time
    for (int i = 0; valueString[i] != '\0'; i++) {

        drawChar(x, y, valueString[i]);
        x += 8; // Move to the next position for the next character
    }
}

// Function to display a float value on the Sharp Memory LCD
void drawFloat(uint16_t x, uint16_t y, float value, int numDecimals) {

    // Convert the float value to a string using our custom function
    char valueString[20]; // You can adjust the size based on your requirement
    floatToString(value, valueString, numDecimals);

    // Display each character of the value string one at a time
    for (int i = 0; valueString[i] != '\0'; i++) {
        drawChar(x, y, valueString[i]);
        x += 8; // Move to the next position for the next character
    }
}

/**
* @brief  Function to convert a float value to a string
*
* @param value
* @param result
* @param numDecimals
*/
void floatToString(float value, char *result, int numDecimals) {
    int intValue = (int)value;
    int decimalValue = (int)((value - intValue) * pow(10, numDecimals));

    // Convert the integer part to a string
    int len = sprintf(result, "%d", intValue);

    // If there are decimals, add the decimal point and convert the decimal part to a string
    if (numDecimals > 0) {
        result[len++] = '.';
        int i;
        for (i = 0; i < numDecimals; i++) {
            result[len + i] = '0' + (decimalValue / (int)pow(10, numDecimals - i - 1)) % 10;
        }
        len += i;
    }

    // Null-terminate the string
    result[len] = '\0';
}

/**
 * @brief draw string on the display
 *
 * @param x
 * @param y
 * @param str
 */
void drawString(uint16_t x, uint16_t y, const char *str) {
    uint16_t currentX = x;

    while (*str) {
        drawChar(currentX, y, *str);
        currentX += 8; // Increment x position to leave space between characters (assuming 7x10 font)
        str++;
    }
}