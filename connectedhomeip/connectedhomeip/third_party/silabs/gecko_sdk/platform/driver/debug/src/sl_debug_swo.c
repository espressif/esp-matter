#include "em_gpio.h"
#include "em_cmu.h"

#include "sl_debug_swo.h"

#if defined(__CORTEX_M) && (__CORTEX_M >= 3)

#include "sl_debug_swo_config.h"

sl_status_t sl_debug_swo_init(void)
{
  uint32_t freq;
  unsigned int location = 0U;
  uint16_t cyctap, postpreset;
  CMU_ClockEnable(cmuClock_GPIO, true);

#if defined(_GPIO_ROUTE_SWOPEN_MASK)
  // Series 0
  location = SL_DEBUG_ROUTE_LOC;
  GPIO_PinModeSet(SL_DEBUG_SWO_PORT, SL_DEBUG_SWO_PIN, gpioModePushPull, 1);
#elif defined(_GPIO_ROUTEPEN_SWVPEN_MASK)
  // Series 1
  location = SL_DEBUG_SWV_LOC;
  GPIO_PinModeSet(SL_DEBUG_SWV_PORT, SL_DEBUG_SWV_PIN, gpioModePushPull, 1);
#elif defined(GPIO_SWV_PORT)
  // Series 2
  // SWO location is not configurable
  GPIO_PinModeSet((GPIO_Port_TypeDef)GPIO_SWV_PORT, GPIO_SWV_PIN, gpioModePushPull, 1);
#endif

  // Set SWO location
  GPIO_DbgLocationSet(location);

  // Enable SWO pin
  GPIO_DbgSWOEnable(true);

#if _SILICON_LABS_32B_SERIES < 2
  // Enable debug clock
  CMU_OscillatorEnable(cmuOsc_AUXHFRCO, true, true);

  // Get debug clock frequency
  freq = CMU_ClockFreqGet(cmuClock_DBG);
#else //_SILICON_LABS_32B_SERIES >= 2

#if defined(_CMU_TRACECLKCTRL_CLKSEL_MASK)
#if defined(_CMU_TRACECLKCTRL_CLKSEL_HFRCOEM23)
#if defined(CMU_CLKEN0_HFRCOEM23)
  CMU->CLKEN0_SET = CMU_CLKEN0_HFRCOEM23;
#endif
  // Select HFRCOEM23 as source for TRACECLK
  CMU_ClockSelectSet(cmuClock_TRACECLK, cmuSelect_HFRCOEM23);
#elif defined(_CMU_TRACECLKCTRL_CLKSEL_SYSCLK)
  // Select SYSCLK as source for TRACECLK
  CMU_ClockSelectSet(cmuClock_TRACECLK, cmuSelect_SYSCLK);
#endif
#endif

  freq = CMU_ClockFreqGet(cmuClock_TRACECLK);
#endif

  // Enable trace in core debug
  CoreDebug->DHCSR |= CoreDebug_DHCSR_C_DEBUGEN_Msk;
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  //Tap the cyctap and postpreset based on the selected interval
#ifdef SL_DEBUG_SWO_SAMPLE_INTERVAL
  if (SL_DEBUG_SWO_SAMPLE_INTERVAL <= 960) {
    cyctap = 0;
    postpreset = (SL_DEBUG_SWO_SAMPLE_INTERVAL / 64);
  } else {
    cyctap = 1;
    postpreset = (SL_DEBUG_SWO_SAMPLE_INTERVAL / 1024);
  }
#else
  cyctap = 1;
  postpreset = 0xF;
#endif
  // Enable PC and IRQ sampling output
  DWT->CTRL = ((4UL << DWT_CTRL_NUMCOMP_Pos)        // Number of comparators. Hardwired to 4.
               | (SL_DEBUG_SWO_SAMPLE_IRQ << DWT_CTRL_EXCTRCENA_Pos)  // Interrupt events
               | (SL_DEBUG_SWO_SAMPLE_PC << DWT_CTRL_PCSAMPLENA_Pos)  // PC sample events
               | (cyctap << DWT_CTRL_CYCTAP_Pos)       // Tap cycle counter at bit 10 (vs bit 6 if 0)
               | (0xFUL << DWT_CTRL_POSTINIT_Pos)   // Post-tap counter
               | (postpreset << DWT_CTRL_POSTPRESET_Pos) // Post-tap counter reload value
               | (1UL << DWT_CTRL_CYCCNTENA_Pos));  // Enable cycle counter
  // Set TPIU prescaler for the current debug clock frequency. ACPR value is div - 1.
  TPI->ACPR = ((freq + (SL_DEBUG_SWO_FREQ / 2)) / SL_DEBUG_SWO_FREQ) - 1UL;

  // Set protocol to NRZ
  TPI->SPPR = 2UL;

  // Disable continuous formatting
  TPI->FFCR = TPI_FFCR_TrigIn_Msk;

  // Unlock ITM and output data
  ITM->LAR = 0xC5ACCE55UL;
  // CMSIS bitfield naming is inconsistent - 16U maps to
  // ITM_TCR_TraceBusID_Pos (v7M) or ITM_TCR_TRACEBUSID_Pos (v8M)
  ITM->TCR = ((1UL << 16U)
              | (1UL << ITM_TCR_DWTENA_Pos)
              | (1UL << ITM_TCR_ITMENA_Pos));

  return SL_STATUS_OK;
}

sl_status_t sl_debug_swo_enable_itm(uint32_t channel)
{
  ITM->TER |= (1UL << channel);
  return SL_STATUS_OK;
}

sl_status_t sl_debug_swo_disable_itm(uint32_t channel)
{
  ITM->TER &= ~(1UL << channel);
  return SL_STATUS_OK;
}

sl_status_t sl_debug_swo_write_u8(uint32_t channel, uint8_t byte)
{
  if (ITM->TCR & ITM_TCR_ITMENA_Msk) {
    do {
      // Some versions of JLink (erroneously) disable SWO when debug connections
      // are closed. Re-enabling trace works around this.
      CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

      // Ensure ITM channel is enabled
      ITM->TER |= (1UL << channel);
    } while (ITM->PORT[channel].u32 == 0);

    ITM->PORT[channel].u8 = byte;

    return SL_STATUS_OK;
  }

  return SL_STATUS_NOT_INITIALIZED;
}

sl_status_t sl_debug_swo_write_u16(uint32_t channel, uint16_t half_word)
{
  if (ITM->TCR & ITM_TCR_ITMENA_Msk) {
    do {
      // Some versions of JLink (erroneously) disable SWO when debug connections
      // are closed. Re-enabling trace works around this.
      CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

      // Ensure ITM channel is enabled
      ITM->TER |= (1UL << channel);
    } while (ITM->PORT[channel].u32 == 0);

    ITM->PORT[channel].u16 = half_word;

    return SL_STATUS_OK;
  }

  return SL_STATUS_NOT_INITIALIZED;
}

sl_status_t sl_debug_swo_write_u32(uint32_t channel, uint32_t word)
{
  if (ITM->TCR & ITM_TCR_ITMENA_Msk) {
    do {
      // Some versions of JLink (erroneously) disable SWO when debug connections
      // are closed. Re-enabling trace works around this.
      CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

      // Ensure ITM channel is enabled
      ITM->TER |= (1UL << channel);
    } while (ITM->PORT[channel].u32 == 0);

    ITM->PORT[channel].u32 = word;

    return SL_STATUS_OK;
  }

  return SL_STATUS_NOT_INITIALIZED;
}

#else // __CORTEX_M

sl_status_t sl_debug_swo_init(void)
{
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_debug_swo_enable_itm(uint32_t channel)
{
  (void) channel;
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_debug_swo_disable_itm(uint32_t channel)
{
  (void) channel;
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_debug_swo_write_u8(uint32_t channel, uint8_t byte)
{
  (void) channel;
  (void) byte;
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_debug_swo_write_u16(uint32_t channel, uint16_t half_word)
{
  (void) channel;
  (void) half_word;
  return SL_STATUS_NOT_SUPPORTED;
}

sl_status_t sl_debug_swo_write_u32(uint32_t channel, uint32_t word)
{
  (void) channel;
  (void) word;
  return SL_STATUS_NOT_SUPPORTED;
}

#endif // __CORTEX_M
