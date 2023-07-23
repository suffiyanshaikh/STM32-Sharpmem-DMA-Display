/**
 * ************************************************************
 * @file   : spi_dma.hpp
 * @author : suffiyan shaikh
 * @brief  : spi_driver header file
 * @version: 0.1
 * @date   : Jul, 2023
 * ************************************************************
 *
 */

#ifndef SPI_DMA_H
#define SPI_DMA_H

/* Includes ------------------------------------------------------------------*/
#include "mbed.h"
#include "mbed_error.h"
#include "mbed_fault_handler.h"

/* Private includes ----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define DMA_SPI 1
#define DMA_Tx_cplt 1

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

int spi_dma_init();

#endif