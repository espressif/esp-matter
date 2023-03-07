/**
 * @file
 * SWO debug print module
 * @copyright 2019 Silicon Laboratories Inc.
 */


#include "SWO_Debug.h"
#include "hal-config.h"
#include "em_gpio.h"
#include <em_dbg.h>
#include <em_cmu.h>
#include "DebugPrintConfig.h"

#define SWO_OUTPUT_FREQ   875000   /*SWO clock frequency in Hz*/

void SWO_Flush(void)
{
  while (((ITM->TCR & ITM_TCR_ITMENA_Msk) != 0UL)        /* ITM enabled */
         && ((ITM->TER & 1UL) != 0UL)                    /* ITM Port #0 enabled */
         && ((ITM->TCR & ITM_TCR_BUSY_Msk) != 0UL) ) {
    __NOP();
  }
}

static void ZW_SWO_Print(const uint8_t * pData,uint32_t iLength)
{
  for(uint32_t i=0; i < iLength; i++)
  {
    ITM_SendChar(pData[i]);
  }
  SWO_Flush();  
}

#ifdef ZWAVE_SERIES_800
void SWO_DebugPrintInit() {
  static uint8_t buffer[64];
  uint32_t div;
 /*Enable GPIO clock*/
  CMU_ClockEnable(cmuClock_GPIO, true);
 /*Enable SWO output pin SWO PF2*/
  GPIO_DbgSWOEnable(true);
  GPIO->P[GPIO_SWV_PORT].MODEL &= ~(_GPIO_P_MODEL_MODE0_MASK << (GPIO_SWV_PIN * 4));
  GPIO->P[GPIO_SWV_PORT].MODEL |= _GPIO_P_MODEL_MODE0_PUSHPULL << (GPIO_SWV_PIN * 4);
  #if defined(_CMU_TRACECLKCTRL_CLKSEL_MASK)
  // Select HFRCOEM23 as source for TRACECLK 
  CMU_ClockSelectSet(cmuClock_TRACECLK, cmuSelect_HFRCOEM23);
  #endif
  /* Configure SWO pin for output */
  GPIO_PinModeSet(BSP_TRACE_SWO_PORT, BSP_TRACE_SWO_PIN, gpioModePushPull, 0);
  /* Ensure auxiliary clock going to the Cortex debug trace module is enabled */
  CMU_OscillatorEnable(cmuOsc_HFRCOEM23, true, true);
  /* Enable trace in core debug */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  /* Set TPIU prescaler for the current debug clock frequency. Target frequency
     is swo_freq Hz so we choose a divider that gives us the closest match.
     Actual divider is TPI->ACPR + 1. */
  div  = (CMU_ClockFreqGet(cmuClock_TRACECLK) + (SWO_OUTPUT_FREQ / 2)) / SWO_OUTPUT_FREQ;
  //div = CMU_ClockFreqGet(cmuClock_TRACECLK) / 863630;
  TPI->SPPR = 2; /* Set protocol to NRZ */
  TPI->ACPR = div - 1;   /* "Async Clock Prescaler Register". Scale the baud rate of the asynchronous output */
  /* Unlock ITM and output data */
  ITM->LAR  = 0xC5ACCE55;
  ITM->TCR  = ITM_TCR_TRACEBUSID_Msk | ITM_TCR_SWOENA_Msk | ITM_TCR_SYNCENA_Msk | ITM_TCR_ITMENA_Msk; /* ITM Trace Control Register */
  ITM->TPR  = ITM_TPR_PRIVMASK_Msk; /* ITM Trace Privilege Register make channel 0 accessible by user code*/
  /* ITM Channel 0 is used for UART output */
  ITM->TER  |= 0x1; /* ITM Trace Enable Register. Enabled tracing on stimulus ports. One bit per stimulus port. */
  DWT->CTRL = 0x400003FE;
  /* Disable continuous formatting */
  TPI->FFCR = 0x00000100; /* Formatter and Flush Control Register */
  DebugPrintConfig(buffer,sizeof(buffer) ,ZW_SWO_Print);
}

#else // 700s
void SWO_DebugPrintInit() {
  static uint8_t buffer[64];
  uint32_t div;
 /*Enable GPIO clock*/
  CMU_ClockEnable(cmuClock_GPIO, true);
 /*Enable SWO output pin SWO PF2*/
  GPIO_DbgSWOEnable(true);
  GPIO_DbgLocationSet(BSP_TRACE_SWO_LOC);
  /* Configure SWO pin for output */
  GPIO_PinModeSet(BSP_TRACE_SWO_PORT, BSP_TRACE_SWO_PIN, gpioModePushPull, 0);
  /* Ensure auxiliary clock going to the Cortex debug trace module is enabled */
  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, true, true);
  /* Enable trace in core debug */
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  /* Set TPIU prescaler for the current debug clock frequency. Target frequency
     is swo_freq Hz so we choose a divider that gives us the closest match.
     Actual divider is TPI->ACPR + 1. */
  div  = (CMU_ClockFreqGet(cmuClock_DBG) + (SWO_OUTPUT_FREQ / 2)) / SWO_OUTPUT_FREQ;

  TPI->SPPR = 2; /* Set protocol to NRZ */
  TPI->ACPR = div - 1;   /* "Async Clock Prescaler Register". Scale the baud rate of the asynchronous output */
  /* Unlock ITM and output data */
  ITM->LAR  = 0xC5ACCE55;
  ITM->TCR  = ITM_TCR_TraceBusID_Msk | ITM_TCR_SWOENA_Msk | ITM_TCR_SYNCENA_Msk | ITM_TCR_ITMENA_Msk; /* ITM Trace Control Register */
  ITM->TPR  = ITM_TPR_PRIVMASK_Msk; /* ITM Trace Privilege Register make channel 0 accessible by user code*/
  /* ITM Channel 0 is used for UART output */
  ITM->TER  = 0x1; /* ITM Trace Enable Register. Enabled tracing on stimulus ports. One bit per stimulus port. */
  DWT->CTRL = 0x400003FE;
  /* Disable continuous formatting */
  TPI->FFCR = 0x00000100; /* Formatter and Flush Control Register */
  DebugPrintConfig(buffer,sizeof(buffer) ,ZW_SWO_Print);
}
#endif // ZWAVE_SERIES_800
