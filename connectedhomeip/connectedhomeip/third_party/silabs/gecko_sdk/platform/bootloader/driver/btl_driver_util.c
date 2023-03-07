/***************************************************************************//**
 * @file
 * @brief Gecko bootloader driver utility functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include "btl_driver_util.h"

uint32_t util_getClockFreq(void)
{
#if defined(_SILICON_LABS_32B_SERIES_2)
  const uint8_t frequencies[] = { 4, 0, 0, 7, 0, 0, 13, 16, 19, 0, 26, 32, 38, 48, 56, 64, 80 };
#else
  const uint8_t frequencies[] = { 4, 0, 0, 7, 0, 0, 13, 16, 19, 0, 26, 32, 38, 48, 56, 64, 72 };
#endif
  uint32_t clockFreq;
#if defined(_SILICON_LABS_32B_SERIES_2)
  if ((CMU->SYSCLKCTRL & _CMU_SYSCLKCTRL_CLKSEL_MASK) == CMU_SYSCLKCTRL_CLKSEL_HFXO) {
    #if defined(BSP_CLK_HFXO_FREQ)
    clockFreq = BSP_CLK_HFXO_FREQ;
    #else
    clockFreq = 38400000UL;
    #endif
  } else {
#if defined(_CMU_CLKEN0_MASK)
    CMU->CLKEN0_SET = CMU_CLKEN0_HFRCO0;
#endif
    clockFreq = (HFRCO0->CAL & _HFRCO_CAL_FREQRANGE_MASK) >> _HFRCO_CAL_FREQRANGE_SHIFT;
    if (clockFreq > 16) {
      clockFreq = 19000000UL;
    } else {
      clockFreq = frequencies[clockFreq] * 1000000UL;
    }
    if (clockFreq == 4000000UL) {
      clockFreq /= (0x1 << ((HFRCO0->CAL & _HFRCO_CAL_CLKDIV_MASK) >> _HFRCO_CAL_CLKDIV_SHIFT));
    }
  }
  clockFreq /= (1U + ((CMU->SYSCLKCTRL & _CMU_SYSCLKCTRL_HCLKPRESC_MASK)
                      >> _CMU_SYSCLKCTRL_HCLKPRESC_SHIFT));
#else
  if ((CMU->HFCLKSTATUS & _CMU_HFCLKSTATUS_SELECTED_MASK) == CMU_HFCLKSTATUS_SELECTED_HFXO) {
    #if defined(BSP_CLK_HFXO_FREQ)
    clockFreq = BSP_CLK_HFXO_FREQ;
    #else
    clockFreq = 38400000UL;
    #endif
  } else {
    clockFreq = (CMU->HFRCOCTRL & _CMU_HFRCOCTRL_FREQRANGE_MASK) >> _CMU_HFRCOCTRL_FREQRANGE_SHIFT;
    if (clockFreq > 16) {
      clockFreq = 19000000UL;
    } else {
      clockFreq = frequencies[clockFreq] * 1000000UL;
    }
    if (clockFreq == 4000000UL) {
      clockFreq /= (0x1 << ((CMU->HFRCOCTRL & _CMU_HFRCOCTRL_CLKDIV_MASK) >> _CMU_HFRCOCTRL_CLKDIV_SHIFT));
    }
  }
  clockFreq /= (1U + ((CMU->HFPRESC & _CMU_HFPRESC_PRESC_MASK)
                      >> _CMU_HFPRESC_PRESC_SHIFT));
#endif
  return clockFreq;
}
#if defined(BTL_UART_ENABLE) || defined(BTL_SPI_USART_ENABLE)
void util_deinitUsart(USART_TypeDef *btlUsart, uint8_t usartNum, CMU_Clock_TypeDef btlUsartClock)
{
  // Void casting to prevent warning.
  (void)btlUsartClock;
  (void)usartNum;
  btlUsart->CMD = USART_CMD_RXDIS
                  | USART_CMD_TXDIS
                  | USART_CMD_CLEARTX
                  | USART_CMD_CLEARRX;
#if defined(_USART_ROUTEPEN_RESETVALUE)
  btlUsart->ROUTEPEN = _USART_ROUTEPEN_RESETVALUE;
#else
  GPIO->USARTROUTE[usartNum].ROUTEEN = 0;
#endif

#if defined(CMU_CTRL_HFPERCLKEN)
  CMU_ClockEnable(btlUsartClock, false);
#endif

#if defined(CMU_CLKEN0_USART0)
  CMU->CLKEN0_CLR = CMU_CLKEN0_USART0;
#endif
#if defined(CMU_CLKEN0_USART1)
  CMU->CLKEN0_CLR = CMU_CLKEN0_USART1;
#endif
}
#endif
