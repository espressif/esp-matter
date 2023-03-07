/*******************************************************************************
* @file  rsi_hal_mcu_platform_init.c
* @brief
*******************************************************************************
* # License
* <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "rsi_driver.h"
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "em_ldma.h"
#include "em_core.h"
#include "sl_status.h"
#include "sl_device_init_clocks.h"
#include "sl_device_init_hfrco_config.h"
#include "sl_device_init_lfxo_config.h"
#include "sl_device_init_emu_config.h"
#include "rsi_board_configuration.h"

#ifndef WEAK
#define WEAK  __attribute__((weak))
#endif

#ifndef RSI_HAL_NO_COM_PORT
static void rsi_hal_com_port_init(void);
#endif

#ifdef RSI_CALIB_MODE_EN
void rsi_calib_uart_recv_isr(uint8_t cmd_char);
#endif

//! systick interrupt priority
#define SYSTICK_INTR_PRI	((1<<__NVIC_PRIO_BITS)-1)

//! packet pending interrupt priority
#define	PACKET_PENDING_INT_PRI	3

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  // Enable clock (not needed on xG21)
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure SPI bus pins
  GPIO_PinModeSet(SPI_MISO_PIN.port, SPI_MISO_PIN.pin, gpioModeInput, 0);
  GPIO_PinModeSet(SPI_MOSI_PIN.port, SPI_MOSI_PIN.pin, gpioModePushPull, 0);
  GPIO_PinModeSet(SPI_CLOCK_PIN.port, SPI_CLOCK_PIN.pin, gpioModePushPullAlternate, 0);
  GPIO_PinModeSet(SPI_CS_PIN.port, SPI_CS_PIN.pin, gpioModePushPull, 1);

}
/**************************************************************************//**
 * @brief
 *    USART0 initialization
 *****************************************************************************/
void initUsart2(void)
{
  // Enable clock (not needed on xG21)
  CMU_ClockEnable(cmuClock_USART2, true);

  // Default asynchronous initializer (master mode, 1 Mbps, 8-bit data)
  USART_InitSync_TypeDef init = USART_INITSYNC_DEFAULT;

  init.msbf = true;           // MSB first transmission for SPI compatibility
  init.autoCsEnable = true;   // Allow the USART to assert CS
  init.baudrate = 8000000; // 12500000;
  /*
   * Route USART0 RX, TX, and CLK to the specified pins.  Note that CS is
   * not controlled by USART0 so there is no write to the corresponding
   * USARTROUTE register to do this.
   */
  GPIO->USARTROUTE[2].RXROUTE = (SPI_MISO_PIN.port << _GPIO_USART_RXROUTE_PORT_SHIFT)
      | (SPI_MISO_PIN.pin << _GPIO_USART_RXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[2].TXROUTE = (SPI_MOSI_PIN.port << _GPIO_USART_TXROUTE_PORT_SHIFT)
      | (SPI_MOSI_PIN.pin << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[2].CLKROUTE = (SPI_CLOCK_PIN.port << _GPIO_USART_CLKROUTE_PORT_SHIFT)
      | (SPI_CLOCK_PIN.pin << _GPIO_USART_CLKROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[2].CSROUTE = (SPI_CS_PIN.port << _GPIO_USART_CSROUTE_PORT_SHIFT)
      | (SPI_CS_PIN.pin << _GPIO_USART_CSROUTE_PIN_SHIFT);

  // Enable USART interface pins
  GPIO->USARTROUTE[2].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN |    // MISO
                                GPIO_USART_ROUTEEN_TXPEN |    // MOSI
                                GPIO_USART_ROUTEEN_CLKPEN |
                                GPIO_USART_ROUTEEN_CSPEN;

  // Set slew rate for alternate usage pins
  GPIO_SlewrateSet(SPI_CLOCK_PIN.port, 6, 6);

  // Configure and enable USART0
  USART_InitSync(USART2, &init);
}
/**************************************************************************//**
 * @brief
 *    LDMA initialization
 *****************************************************************************/
void initLdma(void)
{
  // First, initialize the LDMA unit itself
  LDMA_Init_t ldmaInit = LDMA_INIT_DEFAULT;
  LDMA_Init(&ldmaInit);
}

#if 0
/***************************************************************************//**
 * Configure SWO - serial wire output
 ******************************************************************************/
static void swo_setup(void)
{
  uint32_t tpiu_prescaler_val;

#if defined(_CMU_HFBUSCLKEN0_GPIO_MASK)
  CMU->HFBUSCLKEN0 |= CMU_HFBUSCLKEN0_GPIO;
#endif
#if defined(_CMU_HFPERCLKEN0_GPIO_MASK)
  CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;
#endif

  // Enable Serial wire output pin
#if defined (_GPIO_ROUTE_MASK)
  GPIO->ROUTE |= GPIO_ROUTE_SWOPEN;
#endif
#if defined (_GPIO_ROUTEPEN_MASK)
  GPIO->ROUTEPEN |= GPIO_ROUTEPEN_SWVPEN;
#endif

#if defined(_EFM32_GECKO_FAMILY) || defined(_EFM32_TINY_FAMILY)
  // Set location 1
  GPIO->ROUTE = (GPIO->ROUTE & ~(_GPIO_ROUTE_SWLOCATION_MASK)) | GPIO_ROUTE_SWLOCATION_LOC1;
  // Enable output on pin
  GPIO->P[2].MODEH &= ~(_GPIO_P_MODEH_MODE15_MASK);
  GPIO->P[2].MODEH |= GPIO_P_MODEH_MODE15_PUSHPULL;
  // Set TPIU prescaler to 16 (14 MHz / 16 = 875 kHz SWO speed)
  tpiu_prescaler_val = 15;
#elif defined(_SILICON_LABS_32B_SERIES_0) \
    && (defined(_EFM32_GIANT_FAMILY) || defined(_EFM32_WONDER_FAMILY))
  // Set location 0
  GPIO->ROUTE = (GPIO->ROUTE & ~(_GPIO_ROUTE_SWLOCATION_MASK)) | GPIO_ROUTE_SWLOCATION_LOC0;
  // Enable output on pin
  GPIO->P[5].MODEL &= ~(_GPIO_P_MODEL_MODE2_MASK);
  GPIO->P[5].MODEL |= GPIO_P_MODEL_MODE2_PUSHPULL;
  // Set TPIU prescaler to 16 (14 MHz / 16 = 875 kHz SWO speed)
  tpiu_prescaler_val = 16 - 1;
#elif defined(_SILICON_LABS_32B_SERIES_1)
  // Set location 0
  GPIO->ROUTELOC0 = (GPIO->ROUTELOC0 & ~(_GPIO_ROUTELOC0_SWVLOC_MASK)) | GPIO_ROUTELOC0_SWVLOC_LOC0;
  // Enable output on pin
  GPIO->P[5].MODEL &= ~(_GPIO_P_MODEL_MODE2_MASK);
  GPIO->P[5].MODEL |= 4 << 8;
#if defined(EMDK1001_FPGA)
  // Set TPIU prescaler to get a 875 kHz swo speed
  tpiu_prescaler_val = SystemCoreClockGet() / 875000 - 1;
#else
  // Set TPIU prescaler to 22 (19 MHz / 22 = 863.63 kHz SWO speed)
  tpiu_prescaler_val = 22 - 1;
#endif

#elif (_SILICON_LABS_32B_SERIES == 2)

  // Enable output on pin
#if defined (_CMU_CLKEN0_MASK)
  CMU->CLKEN0 |= CMU_CLKEN0_GPIO;
#endif
  GPIO->P[GPIO_SWV_PORT].MODEL &= ~(_GPIO_P_MODEL_MODE0_MASK << (GPIO_SWV_PIN * 4));
  GPIO->P[GPIO_SWV_PORT].MODEL |= _GPIO_P_MODEL_MODE0_PUSHPULL << (GPIO_SWV_PIN * 4);
  GPIO->TRACEROUTEPEN |= GPIO_TRACEROUTEPEN_SWVPEN;
#if defined(HFRCOEM23_PRESENT)
  // Select HFRCOEM23 as source for TRACECLK
  CMU_ClockSelectSet(cmuClock_TRACECLK, cmuSelect_HFRCOEM23);
#endif
  // Set TPIU prescaler to get a 863.63 kHz SWO speed
  tpiu_prescaler_val = CMU_ClockFreqGet(cmuClock_TRACECLK) / 863630 - 1;

#else
#error Unknown device family!
#endif

#if (_SILICON_LABS_32B_SERIES != 2)
  // Enable debug clock AUXHFRCO
  CMU->OSCENCMD = CMU_OSCENCMD_AUXHFRCOEN;

  while (!(CMU->STATUS & CMU_STATUS_AUXHFRCORDY)) ;
#endif

  // Enable trace in core debug
  CoreDebug->DHCSR |= 1;
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

  // Enable Cycle Counter
  DWT->CTRL = 0x3FF;

  // Set TPIU Prescaler
  TPI->ACPR = tpiu_prescaler_val;

  // Set protocol to NRZ
  TPI->SPPR = 2;

  // Disable continuous formatting
  TPI->FFCR = 0x100;

  // Unlock ITM and output data
  ITM->LAR = 0xC5ACCE55;
  ITM->TCR = 0x10009;

  // ITM Channel 0 is used for print-style output
  ITM->TER |= (1UL << 0);
}
#endif

void rsi_hal_board_init(void)
{
#ifndef RSI_HAL_NO_CLOCK_INIT
  // Initializes the platform
  CHIP_Init();

  // Initialize and enable the HFXO for the crystal on the radio board
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;
  CMU_HFXOInit(&hfxoInit);
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);

  // Select the DPLL as the SYSCLK
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_HFRCODPLL);

  /*
   * Initialize the DPLL to run at 50 MHz from the 38.4 MHz HFXO
   * reference.  The HFXO is designed to provide an accurate reference
   * to the radio vs. the LFXO which is designed for low-power.
   *
   * fDPLL = fREF * (N + 1) / (M + 1), where N > 300
   *
   * In this case:
   *
   * fDPLL = 38,400,000 * (3749 + 1) / (2879 + 1) = 50,000,000 MHz
   */
  CMU_DPLLInit_TypeDef dpllInit = CMU_DPLLINIT_DEFAULT;
  dpllInit.frequency = 50000000;
  dpllInit.n = (3750 - 1);
  dpllInit.m = (2880 - 1);
  dpllInit.refClk = cmuSelect_HFXO;

  // Attempt DPLL lock; halt on failure
  if (CMU_DPLLLock(&dpllInit) == false)
    __BKPT(0);

  SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000);

#ifdef RSI_WITH_OS
  // Set Systick Intr priority
  NVIC_SetPriority(SysTick_IRQn, SYSTICK_INTR_PRI);
#endif
#endif

//  swo_setup();
  // Initialize GPIO and USART0
  initGpio();
  initUsart2();
  initLdma();
#ifndef RSI_HAL_NO_COM_PORT
  rsi_hal_com_port_init();

#if defined(SL_BOARD_ENABLE_VCOM_PIN)
  GPIO_PinModeSet(SL_BOARD_ENABLE_VCOM_PIN.port, SL_BOARD_ENABLE_VCOM_PIN.pin, gpioModePushPull, 1);
#endif
#endif

  //! configure packet pending interrupt priority
  NVIC_SetPriority(GPIO_ODD_IRQn, PACKET_PENDING_INT_PRI);
  
  // Configure interrupt pin
  GPIO_PinModeSet(INTERRUPT_PIN.port, INTERRUPT_PIN.pin, gpioModeInput, 0);
  GPIO_ExtIntConfig(INTERRUPT_PIN.port, INTERRUPT_PIN.pin, INTERRUPT_PIN.pin, true, false, true);

#ifdef LOGGING_STATS
  // Configure logging stats gpio pin
  GPIO_PinModeSet(LOGGING_WAKE_INDICATOR_PIN.port, LOGGING_WAKE_INDICATOR_PIN.pin, gpioModeInput, 1);
  GPIO_ExtIntConfig(LOGGING_WAKE_INDICATOR_PIN.port, LOGGING_WAKE_INDICATOR_PIN.pin, LOGGING_WAKE_INDICATOR_PIN.pin, true, true, true);
#endif
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
  // Initializes the high speed clock
//  USART2->CTRL_SET = USART_CTRL_SMSDELAY;
//  GPIO_SlewrateSet(SPI_CLOCK_PIN.port, 6, 7);
//  USART_BaudrateSyncSet(USART2, 0, 25000000);
}

#ifndef RSI_HAL_NO_COM_PORT
static void rsi_hal_com_port_init(void)
{
  USART_InitAsync_TypeDef init_vcom = USART_INITASYNC_DEFAULT;
  init_vcom.baudrate = COM_PORT_BAUDRATE;
  init_vcom.parity = COM_PORT_PARITY;
  init_vcom.stopbits = COM_PORT_STOP_BITS;
#if (_SILICON_LABS_32B_SERIES > 0)
  init_vcom.hwFlowControl = COM_PORT_FLOW_CONTROL_TYPE;
#endif

#if (_SILICON_LABS_32B_SERIES > 0)
  #if defined(COM_PORT_CTS_PIN)
    bool cts = false;
  #endif
  #if defined(COM_PORT_RTS_PIN)
    bool rts = false;
#endif
#endif

  // Enable peripheral clocks
#if defined(_CMU_HFPERCLKEN0_MASK)
  CMU_ClockEnable(cmuClock_HFPER, true);
#endif

  // Configure TX and RX GPIOs
  GPIO_PinModeSet(COM_PORT_TX_PIN.port, COM_PORT_TX_PIN.pin, gpioModePushPull, 1);
  GPIO_PinModeSet(COM_PORT_RX_PIN.port, COM_PORT_RX_PIN.pin, gpioModeInputPull, 1);

  CMU_ClockEnable(COM_PORT_CLOCK, true);

  // Configure USART for basic async operation
  init_vcom.enable = usartDisable;
  USART_InitAsync(COM_PORT_PERIPHERAL, &init_vcom);

#if defined(GPIO_USART_ROUTEEN_TXPEN)
  // Enable pins at correct USART/USART location
  GPIO->USARTROUTE[COM_PORT_PERIPHERAL_NO].ROUTEEN = GPIO_USART_ROUTEEN_TXPEN | GPIO_USART_ROUTEEN_RXPEN;
  GPIO->USARTROUTE[COM_PORT_PERIPHERAL_NO].TXROUTE = (COM_PORT_TX_PIN.port << _GPIO_USART_TXROUTE_PORT_SHIFT)
                                                                   | (COM_PORT_TX_PIN.pin << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[COM_PORT_PERIPHERAL_NO].RXROUTE = (COM_PORT_RX_PIN.port << _GPIO_USART_RXROUTE_PORT_SHIFT)
                                                                   | (COM_PORT_RX_PIN.pin << _GPIO_USART_RXROUTE_PIN_SHIFT);

#elif defined(USART_ROUTEPEN_RXPEN)
  // Enable pins at correct USART/USART location
  COM_PORT_PERIPHERAL->ROUTEPEN |= USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;
  COM_PORT_PERIPHERAL->ROUTELOC0 = (COM_PORT_PERIPHERAL->ROUTELOC0 & ~(_USART_ROUTELOC0_TXLOC_MASK | _USART_ROUTELOC0_RXLOC_MASK))
                                                 | (COM_PORT_TX_LOC << _USART_ROUTELOC0_TXLOC_SHIFT)
                                                 | (COM_PORT_RX_LOC << _USART_ROUTELOC0_RXLOC_SHIFT);
  COM_PORT_PERIPHERAL->ROUTEPEN = USART_ROUTEPEN_RXPEN | USART_ROUTEPEN_TXPEN;
#else
  COM_PORT_PERIPHERAL->ROUTE = USART_ROUTE_RXPEN | USART_ROUTE_TXPEN | (COM_PORT_ROUTE_LOC << _USART_ROUTE_LOCATION_SHIFT);
#endif

  // Configure GPIOs for hardware flow control
#if (_SILICON_LABS_32B_SERIES > 0)
//    usart_context->flags = 0;
    switch (COM_PORT_FLOW_CONTROL_TYPE) {
      case usartHwFlowControlNone:
        break;
      case usartHwFlowControlCts:
      #if defined(COM_PORT_CTS_PIN)
        cts = true;
      #endif
 //        usart_context->flags = SLI_IOSTREAM_UART_FLAG_CTS;
        break;
      case usartHwFlowControlRts:
      #if defined(COM_PORT_RTS_PIN)
         rts = true;
      #endif
        //        usart_context->flags = SLI_IOSTREAM_UART_FLAG_RTS;
        break;
      case usartHwFlowControlCtsAndRts:
      #if defined(COM_PORT_CTS_PIN)
        cts = true;
      #endif
      #if defined(COM_PORT_RTS_PIN)
        rts = true;
      #endif
//   usart_context->flags = SLI_IOSTREAM_UART_FLAG_CTS | SLI_IOSTREAM_UART_FLAG_RTS;
        break;
      default:
        return;    // SL_STATUS_INVALID_CONFIGURATION;
    }

#if defined(COM_PORT_CTS_PIN)
    if (cts == true) {
      GPIO_PinModeSet(COM_PORT_CTS_PIN.port, COM_PORT_CTS_PIN.pin, gpioModeInputPull, 0);

   #if defined(_USART_ROUTEPEN_RTSPEN_MASK) && defined(_USART_ROUTEPEN_CTSPEN_MASK)
      COM_PORT_PERIPHERAL->ROUTELOC1 = (COM_PORT_CTS_LOC << _USART_ROUTELOC1_CTSLOC_SHIFT);
      COM_PORT_PERIPHERAL->CTRLX    |= USART_CTRLX_CTSEN;
      COM_PORT_PERIPHERAL->ROUTEPEN |= USART_ROUTEPEN_CTSPEN;
   #elif defined(_GPIO_USART_ROUTEEN_MASK)
      GPIO->USARTROUTE_SET[COM_PORT_PERIPHERAL_NO].CTSROUTE = (COM_PORT_CTS_PIN.port << _GPIO_USART_CTSROUTE_PORT_SHIFT)
                                                                            | (COM_PORT_CTS_PIN.pin << _GPIO_USART_CTSROUTE_PIN_SHIFT);
      COM_PORT_PERIPHERAL->CTRLX_SET = USART_CTRLX_CTSEN;
   #endif
    }
#endif
#if defined(COM_PORT_RTS_PIN)
    if (rts == true) {
      GPIO_PinModeSet(COM_PORT_RTS_PIN.port, COM_PORT_RTS_PIN.pin, gpioModePushPull, 0);
   #if defined(_USART_ROUTEPEN_RTSPEN_MASK) && defined(_USART_ROUTEPEN_CTSPEN_MASK)
      COM_PORT_PERIPHERAL->ROUTELOC1 |= (COM_PORT_RTS_LOC << _USART_ROUTELOC1_RTSLOC_SHIFT);
      COM_PORT_PERIPHERAL->ROUTEPEN |= USART_ROUTEPEN_RTSPEN;

   #elif defined(_GPIO_USART_ROUTEEN_MASK)
      GPIO->USARTROUTE_SET[COM_PORT_PERIPHERAL_NO].ROUTEEN = GPIO_USART_ROUTEEN_RTSPEN;
      GPIO->USARTROUTE_SET[COM_PORT_PERIPHERAL_NO].RTSROUTE = (COM_PORT_RTS_PIN.port << _GPIO_USART_RTSROUTE_PORT_SHIFT)
                                                                            | (COM_PORT_RTS_PIN.pin << _GPIO_USART_RTSROUTE_PIN_SHIFT);
   #endif
    }
#endif
   #endif  // Configure GPIOs for hardware flow control

  // Enable RX interrupts
  USART_IntClear(COM_PORT_PERIPHERAL, USART_IF_RXDATAV);
  USART_IntEnable(COM_PORT_PERIPHERAL, USART_IF_RXDATAV);
  NVIC_EnableIRQ(COM_PORT_NVIC);

  // Finally enable it
  USART_Enable(COM_PORT_PERIPHERAL, usartEnable);
}



int _write(int file, const char *ptr, int len)
{
  int txCount;
  (void)file;

  for (txCount = 0; txCount < len; txCount++) {
    USART_Tx(COM_PORT_PERIPHERAL, *ptr++);
  }

  return len;
}

WEAK void uart_rx_handler(uint8_t character)
{
#ifndef RSI_CALIB_MODE_EN
  UNUSED_PARAMETER(character); //This statement is added only to resolve compilation warnings, value is unchanged
#endif

#ifdef RSI_CALIB_MODE_EN
  rsi_calib_uart_recv_isr(character);
#endif
}

// Com Port RX interrupt
void USART0_RX_IRQHandler(void)
{
    uart_rx_handler(COM_PORT_PERIPHERAL->RXDATA);
}

#endif
