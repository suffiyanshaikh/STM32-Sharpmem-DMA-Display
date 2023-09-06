# STM32-Sharpmem-DMA-Display
The Git repository showcases the integration of a Sharp Memory Display with an STM32 microcontroller in the Mbed OS environment. 
The project aims to enhance the display refresh rate by utilizing Direct Memory Access (DMA) in conjunction with Serial Peripheral Interface (SPI) communication. The main focus of this repository is to demonstrate how to efficiently drive the Sharp Memory Display using DMA-enabled SPI, resulting in faster screen updates and improved performance. For demonstration purposes, CPU stats are read using MBED OS APIs and displayed on LCD in regular intervals.

### Hardware details

* Adafruit SHARP Memory Display Breakout - 2.7" 400x240 Monochrome
* Nucleo-L476RG Board

*  [Nucleo-L476RG datasheet](https://www.st.com/resource/en/user_manual/um1724-stm32-nucleo64-boards-mb1136-stmicroelectronics.pdf).
*  [Adafruit SHARP Memory Display Breakout](https://www.adafruit.com/product/4694)

**Connections**
|LCD Pins |STM32L476 Pins|
|:-----------|:-------------:|
|CS          | PA_15         |
|SCK         | PC_10         |
|MOSI        | PC_12         |
|VCC         | 3.3V          |
|GND         |Gnd/0V

### How to Use the project
* Clone the repository and include it in EMBED OS IDE

**Example Output**
![image](https://github.com/suffiyanshaikh/STM32-Sharpmem-DMA-Display/assets/42150715/375f89d1-ceac-4335-a51f-2430b8e84904)
