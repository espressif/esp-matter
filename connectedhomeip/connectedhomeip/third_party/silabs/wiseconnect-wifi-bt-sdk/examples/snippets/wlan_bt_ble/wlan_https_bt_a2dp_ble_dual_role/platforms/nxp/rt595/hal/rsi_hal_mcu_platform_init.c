/*******************************************************************************
* @file  rsi_hal_mcu_platform_init.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/



/**
 * Includes
 */
#ifdef RSI_ENABLE_DEMOS
#include "rsi_common_app.h"
#endif
#include "rsi_driver.h"
#include "board.h"
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_spi_cmsis.h"
#include "board.h"

#include <stdio.h>
#include <string.h>
#include "fsl_sd.h"
#include "ff.h"
#include "diskio.h"
#include "fsl_sd_disk.h"

#include "pin_mux.h"
#include "fsl_inputmux.h"


#include "fsl_sdio.h"
#include "clock_config.h"
#include "fsl_power.h"
#include "fsl_pca9420.h"

 extern sdio_card_t card;
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* SPI user SignalEvent */
void SPI_MasterSignalEvent_t(uint32_t event);
#ifdef RSI_ENABLE_DEMOS
int8_t sd_filesystem_init(void);
extern int sdram_init(void);
#endif
uint8_t platform_initialised;

/*******************************************************************************
 * Variables
 *****************************************************************************/
static const sdmmchost_detect_card_t s_sdCardDetect = {
#ifndef BOARD_SD_DETECT_TYPE
    .cdType = kSDMMCHOST_DetectCardByGpioCD,
#else
    .cdType = BOARD_SD_DETECT_TYPE,
#endif
    .cdTimeOut_ms = (~0U),
};

volatile bool isTransferCompleted = false;
/*******************************************************************************
 * Code
 ******************************************************************************/
//! SD Card parameters
static FATFS g_fileSystem; /* File system object */

int8_t sd_filesystem_init(void)
{
  const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};
FRESULT error;
    if (f_mount(&g_fileSystem, driverNumberBuffer, 0U))
    {
        PRINTF("Mount volume failed.\r\n");
        return -1;
    }

#if (FF_FS_RPATH >= 2U)
    error = f_chdrive((char const *)&driverNumberBuffer[0U]);
    if (error)
    {
        PRINTF("Change drive failed.\r\n");
        return -1;
    }
#endif
	return FR_OK;  
}
uint32_t SPI5_GetFreq(void)
{
    return CLOCK_GetFlexcommClkFreq(5U);
}
void SPI_MasterSignalEvent_t(uint32_t event)
{
    /* user code */
    isTransferCompleted = true;
}



/*==============================================*/

/**
 * @fn           void rsi_hal_board_init()
 * @brief        This function Initializes the platform
 * @param[in]    none 
 * @param[out]   none
 * @return       none
 * @section description
 * This function initializes the platform
 *
 */
void rsi_hal_board_init()
{

  uint32_t errorCount;
  uint32_t i;

  if(!platform_initialised)
  {
    /* Configure DMAMUX. */
    RESET_PeripheralReset(kINPUTMUX_RST_SHIFT_RSTn);

    INPUTMUX_Init(INPUTMUX);
    CLOCK_AttachClk(kFRO48M_to_FLEXCOMM5);
    INPUTMUX_EnableSignal(INPUTMUX, kINPUTMUX_Flexcomm5RxToDmac0Ch10RequestEna, true);
    INPUTMUX_EnableSignal(INPUTMUX, kINPUTMUX_Flexcomm5TxToDmac0Ch11RequestEna, true);
    /* Turnoff clock to inputmux to save power. Clock is only needed to make changes */
    INPUTMUX_Deinit(INPUTMUX);   
    /* Define the init structure for the input switch pin */
    gpio_pin_config_t In_config = {kGPIO_DigitalInput, 0};  //power save in pin
    gpio_pin_config_t Out_config = {kGPIO_DigitalOutput, 0}; //power save out pin
    gpio_pin_config_t Reset_config = {kGPIO_DigitalOutput, 0}; //reset pin

    BOARD_InitPins(); 
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
#ifdef  RSI_WITH_OS    
    /* Set interrupt priority for UART interrupt */
    NVIC_SetPriority(BOARD_UART_IRQ,BOARD_UART_IRQ_PRIORITY_5);
#endif    
    GPIO_PortInit(GPIO, BOARD_INITPINS_PS_IN_PORT); //power save in pin
    GPIO_PinInit(GPIO, BOARD_INITPINS_PS_IN_PORT, BOARD_INITPINS_PS_IN_PIN, &In_config); //power save in pin

    GPIO_PortInit(GPIO, BOARD_INITPINS_PS_OUT_PORT); //power save out pin
    GPIO_PinInit(GPIO, BOARD_INITPINS_PS_OUT_PORT, BOARD_INITPINS_PS_OUT_PIN, &Out_config); //power save out pin

    GPIO_PortInit(GPIO, BOARD_INITPINS_RESET_PORT); //RESET pin
    GPIO_PinInit(GPIO, BOARD_INITPINS_RESET_PORT, BOARD_INITPINS_RESET_PIN, &Reset_config);//RESET pin
#if MXRT_595s_ADDONCARD                             /*RT595 ADD ON CARD GPIO HOST SELECT pins*/
    GPIO_PinInit(GPIO, BOARD_INITPINS_HOST_GPIO_SEL_OUT_PORT, BOARD_INITPINS_HOST_GPIO_SEL_OUT_PIN, &Out_config); //GPIO HOST SELECT pin
    GPIO_PinWrite(GPIO, BOARD_INITPINS_HOST_GPIO_SEL_OUT_PORT, BOARD_INITPINS_HOST_GPIO_SEL_OUT_PIN, 1);//GPIO HOST SELECT pin
#endif
#ifdef WAKEUP_GPIO_INTERRUPT_METHOD
    gpio_interrupt_config_t config = {kGPIO_PinIntEnableLevel, kGPIO_PinIntEnableHighOrRise};
    /* Enable GPIO pin interrupt */
    GPIO_SetPinInterruptConfig(GPIO, BOARD_INITPINS_PS_IN_PORT, BOARD_INITPINS_PS_IN_PIN, &config);
    GPIO_PinEnableInterrupt(GPIO, BOARD_INITPINS_PS_IN_PORT, BOARD_INITPINS_PS_IN_PIN, 0);
#ifdef  RSI_WITH_OS       
    /* Set interrupt priority for wakeup GPIO interrupt */
    NVIC_SetPriority(GPIO_INTA_IRQn,GPIO_INTA_IRQn_4);
#endif    
#endif
    //! Disable the interrupt 
    rsi_hal_intr_mask();

#ifdef RSI_ENABLE_DEMOS
#ifndef RSI_WITH_OS
    //Initialize systick
#if TICK_100_USEC
    //! Set systick reload value to generate 100us interrupt
    if(SysTick_Config(SystemCoreClock / 10000U))
#elif TICK_1_MSEC
      //! Set systick reload value to generate 1ms interrupt
      if(SysTick_Config(SystemCoreClock / 1000U))
#endif
      {
        while(1)
        {
        }
      }

#endif
#else

    //! Set systick reload value to generate 1ms interrupt
    if(SysTick_Config(SystemCoreClock / 1000U))

    {
      while(1)
      {
      }
    }
#endif
    /* Set systick reload value to generate 1ms interrupt */
    SysTick_Config(CLOCK_GetFreq(kCLOCK_CoreSysClk) / 1000U);

    platform_initialised = 1;
  }
}


/*==============================================*/
/**
 * @fn           void rsi_switch_to_high_clk_freq()
 * @brief        This function intializes SPI to high clock
 * @param[in]    none 
 * @param[out]   none
 * @return       none
 * @section description
 * This function intializes SPI to high clock
 *
 *
 */

void rsi_switch_to_high_clk_freq()
{
  //! Initializes the high speed clock
  SDIO_SwitchToHighSpeed(&card); 
}




