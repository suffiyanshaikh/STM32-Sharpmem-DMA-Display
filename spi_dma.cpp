/**
 * ************************************************************
 * @file   : spi_dma.cpp
 * @author : suffiyan shaikh
 * @brief  : spi_dma source file
 * @version: 0.1
 * @date   : Jul, 2023
 * ************************************************************
 *
 */
/* Includes ------------------------------------------------------------------*/

/* Private includes ----------------------------------------------------------*/
#include "spi_dma.hpp"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_dma.h"
#include "stm32l4xx_hal_spi.h"

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
EventFlags dma_flag;

/**-----SPI Parameters --------**/

SPI_HandleTypeDef hspi;
DMA_HandleTypeDef hspi_dma_tx;
DMA_HandleTypeDef hspi_dma_rx;

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

/* User defined functions ----------------------------------------------------*/

int spi_dma_init() {

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* Enable SPI3 clock */
    __HAL_RCC_SPI3_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/
    /**SPI3 GPIO Configuration
    PC10     ------> SPI3_SCK
    PC12     ------> SPI3_MOSI
    */
    GPIO_InitStruct.Pin =       GPIO_PIN_10 | GPIO_PIN_12;
    GPIO_InitStruct.Mode =      GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull =      GPIO_NOPULL;
    GPIO_InitStruct.Speed =     GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;

    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*##-3- Configure the SPI peripheral #######################################*/
    /* Set the SPI parameters */
    hspi.Instance =                 SPI3;
    hspi.Init.Mode =                SPI_MODE_MASTER;
    hspi.Init.Direction =           SPI_DIRECTION_2LINES;
    hspi.Init.DataSize =            SPI_DATASIZE_8BIT;
    hspi.Init.CLKPolarity =         SPI_POLARITY_LOW;
    hspi.Init.CLKPhase =            SPI_PHASE_1EDGE;
    hspi.Init.NSS =                 SPI_NSS_SOFT;
    hspi.Init.BaudRatePrescaler =   SPI_BAUDRATEPRESCALER_16;
    hspi.Init.FirstBit =            SPI_FIRSTBIT_MSB;
    hspi.Init.TIMode =              SPI_TIMODE_DISABLE;
    hspi.Init.CRCCalculation =      SPI_CRCCALCULATION_DISABLE;
    hspi.Init.CRCPolynomial =       7;
    hspi.Init.CRCLength =           SPI_CRC_LENGTH_DATASIZE;
    hspi.Init.NSSPMode =            SPI_NSS_PULSE_ENABLE;

    if ((HAL_SPI_Init(&hspi)) != HAL_OK) {
        return 0;
    }
    

    /* SPI3 interrupt Init */
    HAL_NVIC_SetPriority(SPI3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SPI3_IRQn);

#if DMA_SPI

    /*##-4- Configure the DMA ##################################################*/
    /* Associate the initialized DMA handle to the the SPI handle */

    /* Set Interrupt Group Priority */
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    /* DMA2_Channel2_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Channel2_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(DMA2_Channel2_IRQn);

    /*##-4- Configure the DMA ##################################################*/

    /* DMA controller clock enable */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* Configure the DMA handler for Transmission process */
    hspi_dma_tx.Instance =                  DMA2_Channel2;
    hspi_dma_tx.Init.Request =              DMA_REQUEST_3;
    hspi_dma_tx.Init.Direction =            DMA_MEMORY_TO_PERIPH;
    hspi_dma_tx.Init.PeriphInc =            DMA_PINC_DISABLE;
    hspi_dma_tx.Init.MemInc =               DMA_MINC_ENABLE;
    hspi_dma_tx.Init.PeriphDataAlignment =  DMA_PDATAALIGN_BYTE;
    hspi_dma_tx.Init.MemDataAlignment =     DMA_MDATAALIGN_BYTE;
    hspi_dma_tx.Init.Mode =                 DMA_NORMAL;
    hspi_dma_tx.Init.Priority =             DMA_PRIORITY_HIGH;

    if (HAL_DMA_Init(&hspi_dma_tx) != HAL_OK) return false;

    __HAL_LINKDMA(&hspi, hdmatx, hspi_dma_tx);
    __HAL_DMA_ENABLE_IT(&hspi_dma_tx, DMA_IT_TC);

#endif

    return 1;
}

/**
 * @brief  TxRx Transfer completed callback.
 * @param  hspi: SPI handle
 * @note   This example shows a simple way to report end of DMA TxRx transfer, and
 *         you can add your own implementation.
 * @retval None
 */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) { 

    dma_flag.set(DMA_Tx_cplt);
}

/**
 * @brief This function handles SPI3 global interrupt.
 */
extern "C" void SPI3_IRQHandler(void) {
    // Call the HAL_DMA_IRQHandler function using the hspi_dma_tx member of the active instance.
    HAL_SPI_IRQHandler(&hspi);
}

/**
 * @brief This function handles DMA2 channel2 global interrupt.
 */
extern "C" void DMA2_Channel2_IRQHandler(void) {
    HAL_DMA_IRQHandler(&hspi_dma_tx);
}

/**
 * @brief  SPI error callbacks.
 * @param  hspi: SPI handle
 * @note   This example shows a simple way to report transfer error, and you can
 *         add your own implementation.
 * @retval None
 */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {}
