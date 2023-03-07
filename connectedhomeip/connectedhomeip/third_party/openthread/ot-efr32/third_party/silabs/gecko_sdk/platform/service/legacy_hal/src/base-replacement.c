/***************************************************************************//**
 * @file base-replacement.c
 * @brief Legacy HAL
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories, Inc, www.silabs.com</b>
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

#include PLATFORM_HEADER

#include <stdio.h>
#include "hal.h"
#include "em_chip.h"
#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_80)
#include "tempdrv.h"
#endif
#include "em_emu.h"
#include "em_cmu.h"
#include "pa_conversions_efr32.h"
#include "sl_sleeptimer.h"
#include "em_wdog.h"
#include "em_rmu.h"
#include "sl_power_manager.h"
#include "rail.h"
#include "sl_status.h"
#include "sl_component_catalog.h"
#include "sl_token_api.h"

#if defined(SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT)
#include "sl_iostream.h"
#include "sl_iostream_usart.h"
#include "sl_iostream_handles.h"
#endif

#if defined(SL_CATALOG_IOSTREAM_VUART_PRESENT)
#include "sl_iostream_vuart.h"
#endif

#if defined(SL_CATALOG_LED_PRESENT)
#include "sl_led.h"
#endif

#if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT)
#include "sl_button.h"
#include "sl_simple_button.h"
#include "sl_simple_button_config.h"
#include "sl_simple_button_instances.h"
#endif

#if defined(SL_CATALOG_LEGACY_HAL_WDOG_PRESENT)
#include "sl_legacy_hal_wdog_config.h"
#endif

#define EMBER_SUCCESS             (0x00u)
#define EMBER_ERR_FATAL           (0x01u)
#define EMBER_SLEEP_INTERRUPTED   (0x85u)

// System timer / sleeptimer
uint16_t halCommonGetInt16uMillisecondTick(void)
{
  return (uint16_t)halCommonGetInt64uMillisecondTick();
}

uint32_t halCommonGetInt32uMillisecondTick(void)
{
  return (uint32_t)halCommonGetInt64uMillisecondTick();
}

uint64_t halCommonGetInt64uMillisecondTick(void)
{
  uint64_t ms;

  assert(sl_sleeptimer_tick64_to_ms(sl_sleeptimer_get_tick_count64(),
                                    &ms) == SL_STATUS_OK);
  return ms;
}

uint16_t halCommonGetInt16uQuarterSecondTick(void)
{
  uint32_t ticks = halCommonGetInt32uMillisecondTick();
  ticks /= MILLISECOND_TICKS_PER_QUARTERSECOND;
  return (uint16_t)ticks;
}

bool sl_legacy_hal_is_ok_to_sleep(void)
{
  return true;
}

sl_power_manager_on_isr_exit_t sl_legacy_hal_sleep_on_isr_exit(void)
{
  return SL_POWER_MANAGER_WAKEUP;
}

void halCommonDelayMicroseconds(uint16_t us)
{
  // Note: If you find that your software is hanging in this spot, then
  // You most likely haven't run RAIL_Init() yet. If you really need
  // us precision timings before RAIL_Init() is run, consider using
  // the ustimer component instead of halCommonDelayMicroseconds().

  RAIL_DelayUs(us);
}

void halCommonDelayMilliseconds(uint16_t ms)
{
  sl_sleeptimer_delay_millisecond(ms);
}

uint16_t halInternalStartSystemTimer(void)
{
  if (sl_sleeptimer_init() != SL_STATUS_OK) {
    assert(0);
  }
  return 0;
}

void halInit(void)
{
  halInternalClassifyReset();

  // To be able to give more details on errors, we want faults enabled so
  // they're not all forced into hard faults.
  SCB->SHCSR |= (SCB_SHCSR_BUSFAULTENA_Msk
                 | SCB_SHCSR_MEMFAULTENA_Msk
#ifdef SCB_SHCSR_SECUREFAULTENA_Msk
                 | SCB_SHCSR_SECUREFAULTENA_Msk
#endif
                 | SCB_SHCSR_USGFAULTENA_Msk);

  //Fill the unused portion of the memory reserved for the stack.
  //memset() is not being used to do this in case it uses the stack
  //to store the return address.
  volatile uint32_t *dataDestination;
  //This code assumes that the __get_MSP() return value and
  //_CSTACK_SEGMENT_BEGIN are both 32-bit aligned values.
  dataDestination = (uint32_t*) (__get_MSP() - 4U);
  //Start at current stack ptr fill up until CSTACK_SEGMENT_BEGIN
  while (dataDestination >= _CSTACK_SEGMENT_BEGIN) {
    //Fill with magic value interpreted by C-SPY's Stack View
    *dataDestination-- = STACK_FILL_VALUE;
  }

  // TEMPDRV fixes an errata that only affects SDID 80
#if defined(_SILICON_LABS_GECKO_INTERNAL_SDID_80)
  TEMPDRV_Init();
#endif

  EMU_UnlatchPinRetention();

#if ((SL_LEGACY_HAL_ENABLE_WATCHDOG == 1) && (SL_LEGACY_HAL_DISABLE_WATCHDOG == 0))
  halInternalEnableWatchDog();
#endif // ((SL_LEGACY_HAL_ENABLE_WATCHDOG == 1) && (SL_LEGACY_HAL_DISABLE_WATCHDOG == 0))

  halInternalStartSystemTimer();

  RAIL_InitTxPowerCurvesAlt(&RAIL_TxPowerCurvesVbat);
}

void halReboot(void)
{
  halInternalSysReset(RESET_SOFTWARE_REBOOT);
}

// Memory functions

void halCommonMemMove(void *dest, const void *src, uint16_t bytes)
{
  uint8_t *d = (uint8_t *)dest;
  uint8_t *s = (uint8_t *)src;

  if (d > s) {
    d += bytes - 1;
    s += bytes - 1;
    #ifndef _HAL_SMALL_MEMUTILS_
    while (bytes >= 4) {
      bytes -= 4;
      *d-- = *s--;
      *d-- = *s--;
      *d-- = *s--;
      *d-- = *s--;
    }
    #endif // _HAL_SMALL_MEMUTILS_
    for (; bytes != 0U; bytes--) {
      *d-- = *s--;
    }
  } else {
    #ifndef _HAL_SMALL_MEMUTILS_
    while (bytes >= 4) {
      bytes -= 4;
      *d++ = *s++;
      *d++ = *s++;
      *d++ = *s++;
      *d++ = *s++;
    }
    #endif // _HAL_SMALL_MEMUTILS_
    for (; bytes != 0U; bytes--) {
      *d++ = *s++;
    }
  }
}

int16_t halCommonMemCompare(const void *source0, const void *source1, uint16_t bytes)
{
  uint8_t *s0 = (uint8_t *)source0;
  uint8_t *s1 = (uint8_t *)source1;

  while (0 < bytes) {
    uint8_t b0 = *s0;
    uint8_t b1 = *s1;
    if (b0 != b1) {
      return b0 - b1;
    }
    bytes--;
    s0++;
    s1++;
  }
  return 0;
}

void halCommonMemSet(void *dest, uint8_t val, uint16_t bytes)
{
  uint8_t *d = (uint8_t *)dest;

  for (; bytes != 0U; bytes--) {
    *d++ = val;
  }
}

int8_t halCommonMemPGMCompare(const void *source0, const void PGM_NO_CONST *source1, uint16_t bytes)
{
  return halCommonMemCompare(source0, source1, bytes);
}

void halCommonMemPGMCopy(void* dest, const void PGM_NO_CONST *source, uint16_t bytes)
{
  halCommonMemMove(dest, source, bytes);
}

#ifndef EMBER_TEST
uint32_t halInternalGetCStackBottom(void)
{
  return (uint32_t)(uint8_t *)_CSTACK_SEGMENT_BEGIN;
}
#endif
// These variables are defined by the stack in ember-configuration.c
extern uint16_t heapMemory[];
extern const uint32_t heapMemorySize;

uint32_t halInternalGetHeapTop(void)
{
  return (uint32_t)((uint8_t *)heapMemory) + heapMemorySize;
}

uint32_t halInternalGetHeapBottom(void)
{
  return (uint32_t)(uint8_t *)heapMemory;
}

WEAK(void halRadioPowerUpHandler(void))
{
  // Intentionally empty.
  // This function is implemented here weakly as a fallback when it is not
  // overridden. It is called below by halStackRadioPowerMainControl.
}

WEAK(void halRadioPowerDownHandler(void))
{
  // Intentionally empty.
  // This function is implemented here weakly as a fallback when it is not
  // overridden. It is called below by halStackRadioPowerMainControl.
}

void halStackRadioPowerMainControl(bool powerUp)
{
  if (powerUp) {
    halRadioPowerUpHandler();
  } else {
    halRadioPowerDownHandler();
  }
}

void halStackRadioPowerUpBoard(void)
{
  // Intentionally empty.
  // This function is defined here only to provide compatibility for code that
  // may also link against legacy RF coexistence implementations.
}

void halStackRadioPowerDownBoard(void)
{
  // Intentionally empty.
  // This function is defined here only to provide compatibility for code that
  // may also link against legacy RF coexistence implementations.
}

#if !defined(BOARD_ACTIVITY_LED)
  #if defined(SL_CATALOG_BOARD_ACTIVITY_PRESENT)
    #define BOARD_ACTIVITY_LED  sl_led_board_activity
  #elif defined(SL_CATALOG_LED0_PRESENT)
    #define BOARD_ACTIVITY_LED  sl_led_led0
  #endif
#endif

void halStackIndicateActivity(bool turnOn)
{
  (void)turnOn;

#if defined(SL_CATALOG_LED_PRESENT) && defined(BOARD_ACTIVITY_LED)
  if (turnOn) {
    sl_led_turn_on(&BOARD_ACTIVITY_LED);
  } else {
    sl_led_turn_off(&BOARD_ACTIVITY_LED);
  }
#endif
}

void halSetLed(HalBoardLed led)
{
  (void)led;
}

void halClearLed(HalBoardLed led)
{
  (void)led;
}

void halToggleLed(HalBoardLed led)
{
  (void)led;
}

void halStackProcessBootCount(void)
{
#if defined(CREATOR_STACK_BOOT_COUNTER)
  halCommonIncrementCounterToken(TOKEN_STACK_BOOT_COUNTER);
#endif
}

EmberStatus halCommonIdleForMilliseconds(uint32_t *duration)
{
  (void)duration;

  // Sleeping is now handled by the Power Manager. Closest we can get to the old
  // behavior is to tell the caller that sleep was interrupted.
  return EMBER_SLEEP_INTERRUPTED;
}

void halStackSymbolDelayAIsr(void)
{
  // Intentionally empty.
  // This function is defined here only to provide compatibility for code that
  // may also be built for legacy parts.
}

EmberStatus emDebugInit(void)
{
#if defined(SL_CATALOG_IOSTREAM_VUART_PRESENT)
  sl_status_t status = sl_iostream_vuart_init();

  if (status == SL_STATUS_OK) {
    return EMBER_SUCCESS;
  }
#endif

  return EMBER_ERR_FATAL;
}

void emRadioSeedRandom(void)
{
  uint32_t seed;
  uint16_t bytes = RAIL_GetRadioEntropy(NULL, (uint8_t *)(&seed), sizeof(seed));
  assert(bytes == sizeof(seed));
  halStackSeedRandom(seed);
}

//------------------------------------------------------------------------------

EmberStatus emberSerialInit(uint8_t port,
                            SerialBaudRate rate,
                            SerialParity parity,
                            uint8_t stopBits)
{
  (void) port;
  (void) rate;
  (void) parity;
  (void) stopBits;
  return EMBER_SUCCESS;
}

EmberStatus emberSerialWaitSend(uint8_t port)
{
  (void) port;
  return EMBER_SUCCESS;
}

#ifdef SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT
const sl_iostream_instance_info_t * sli_legacy_hal_get_iostream_info(uint8_t port)
{
  sl_iostream_type_t type = SL_IOSTREAM_TYPE_UNDEFINED;
  uint8_t periph = (uint8_t)(-1);

  switch (port) {
    case COM_VCP:
    case comPortVcp:
      type = SL_IOSTREAM_TYPE_VUART;
      periph = 0;
      break;
    case COM_USART0:
    case comPortUsart0:
      type = SL_IOSTREAM_TYPE_UART;
      periph = 0;
      break;
    case COM_USART1:
    case comPortUsart1:
      type = SL_IOSTREAM_TYPE_UART;
      periph = 1;
      break;
    case COM_USART2:
    case comPortUsart2:
      type = SL_IOSTREAM_TYPE_UART;
      periph = 2;
      break;
    case comPortUsart3:
      type = SL_IOSTREAM_TYPE_UART;
      periph = 3;
      break;
    default:
      return NULL;
  }

  for (uint8_t i = 0; i < sl_iostream_instances_count; i++) {
    if (sl_iostream_instances_info[i] != NULL
        && sl_iostream_instances_info[i]->type == type
        && sl_iostream_instances_info[i]->periph_id == periph) {
      return sl_iostream_instances_info[i];
    }
  }

  return NULL;
}
#endif // SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT

EmberStatus emberSerialReadByte(uint8_t port, uint8_t *dataByte)
{
  (void)port;
  (void)dataByte;

#ifdef SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT
  const sl_iostream_instance_info_t * info;
  info = sli_legacy_hal_get_iostream_info(port);

  if (info != NULL && info->handle != NULL) {
    sl_status_t status = sl_iostream_getchar(info->handle, (char *)dataByte);

    if (status == SL_STATUS_OK) {
      return EMBER_SUCCESS;
    } else if (status == SL_STATUS_EMPTY) {
      return EMBER_SERIAL_RX_EMPTY;
    }
  }
#endif // SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT

  return EMBER_ERR_FATAL;
}

EmberStatus emberSerialReadData(uint8_t port,
                                uint8_t *data,
                                uint16_t length,
                                uint16_t *bytesRead)
{
  (void)port;
  (void)data;
  (void)length;
  (void)bytesRead;

#ifdef SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT
  const sl_iostream_instance_info_t * info;
  info = sli_legacy_hal_get_iostream_info(port);

  if (info == NULL || info->handle == NULL) {
    return EMBER_ERR_FATAL;
  }

  uint16_t bytesReadInternal = 0;
  sl_status_t status;

  while (bytesReadInternal < length) {
    status = sl_iostream_getchar(info->handle, (char *)data);
    switch (status) {
      case SL_STATUS_OK:
        ++data;
        ++bytesReadInternal;
        break;

      case SL_STATUS_EMPTY:
        // empty queue is not an error for us, we just want to keep waiting
        break;

      default:
        // only store number of bytes read if the caller provided a non-NULL pointer
        if (bytesRead) {
          *bytesRead = bytesReadInternal;
        }
        return EMBER_ERR_FATAL;
    }
  }

  // only store number of bytes read if the caller provided a non-NULL pointer
  if (bytesRead) {
    *bytesRead = bytesReadInternal;
  }
  return EMBER_SUCCESS;
#else // !SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT

  return EMBER_ERR_FATAL;
#endif // SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT
}

EmberStatus emberSerialWriteByte(uint8_t port, uint8_t dataByte)
{
  (void)port;
  (void)dataByte;

#ifdef SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT
  const sl_iostream_instance_info_t * info;
  info = sli_legacy_hal_get_iostream_info(port);

  if (info != NULL && info->handle != NULL) {
    sl_status_t status = sl_iostream_putchar(info->handle, (char)dataByte);

    if (status == SL_STATUS_OK) {
      return EMBER_SUCCESS;
    }
  }
#endif // SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT

  return EMBER_ERR_FATAL;
}

EmberStatus emberSerialGuaranteedPrintf(uint8_t port,
                                        const char * formatString, ...)
{
  (void)port;
  (void)formatString;

#ifdef SL_CATALOG_LEGACY_PRINTF_PRESENT
  EmberStatus status;
  va_list ap;
  va_start(ap, formatString);
  status = emberSerialPrintfVarArg(port, formatString, ap);
  va_end(ap);
  printf("\n");
  return status;
#else // !SL_CATALOG_LEGACY_PRINTF_PRESENT

  return EMBER_ERR_FATAL;
#endif // SL_CATALOG_LEGACY_PRINTF_PRESENT
}

bool halInternalUartXonRefreshDone(uint8_t port)
{
  (void)port;

  // SL_IOSTREAM does not currently support software flow control
  return true;
}

bool halInternalUartTxIsIdle(uint8_t port)
{
  (void)port;

#ifdef SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT
  const sl_iostream_instance_info_t * info;
  info = sli_legacy_hal_get_iostream_info(port);

  assert(info != NULL
         && info->type == SL_IOSTREAM_TYPE_UART
         && info->handle != NULL
         && info->handle->context != NULL);

  sl_iostream_usart_context_t * context = info->handle->context;
  uint32_t status = context->usart->STATUS;

  return ((status & _USART_STATUS_TXIDLE_MASK) == _USART_STATUS_TXIDLE_MASK
          && (status & _USART_STATUS_TXBUFCNT_MASK) == 0);
#else // !SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT

  return true;
#endif // SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT
}

bool halInternalUartFlowControlRxIsEnabled(uint8_t port)
{
  (void)port;

#ifdef SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT
  const sl_iostream_instance_info_t * info;
  info = sli_legacy_hal_get_iostream_info(port);

  assert(info != NULL
         && info->type == SL_IOSTREAM_TYPE_UART
         && info->handle != NULL
         && info->handle->context != NULL);

  sl_iostream_usart_context_t * context = info->handle->context;
  uint32_t status = context->usart->STATUS;

  return ((status & _USART_STATUS_RXFULL_MASK) == 0);
#else // !SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT

  return true;
#endif // SL_CATALOG_IOSTREAM_UART_COMMON_PRESENT
}

void halInternalSetCtune(uint16_t tune)
{
  #if defined(_CMU_HFXOSTEADYSTATECTRL_CTUNE_MASK) // series 1
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);

  CMU_OscillatorEnable(cmuOsc_HFXO, false, false);
  CMU->HFXOSTEADYSTATECTRL &= ~_CMU_HFXOSTEADYSTATECTRL_CTUNE_MASK;
  CMU->HFXOSTEADYSTATECTRL |= (tune << _CMU_HFXOSTEADYSTATECTRL_CTUNE_SHIFT)
                              & _CMU_HFXOSTEADYSTATECTRL_CTUNE_MASK;
  CMU_OscillatorEnable(cmuOsc_HFXO, true, true);

  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);

  #elif defined(_HFXO_XTALCTRL_CTUNEXIANA_MASK) // series 2
  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_FSRCO);

  // clear FORCEEN bit and set DISONDEMAND bit to enable writing of ctune values
  HFXO0->CTRL &= ~_HFXO_CTRL_FORCEEN_MASK;
  HFXO0->CTRL |= _HFXO_CTRL_DISONDEMAND_MASK;

  // FSMLOCK only on series 2 config 1 and 2
  #if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1) || defined(_SILICON_LABS_32B_SERIES_2_CONFIG_2)
  // wait until FSMLOCK becomes 0
  while ((HFXO0->STATUS & _HFXO_STATUS_FSMLOCK_MASK) == HFXO_STATUS_FSMLOCK) {
  }
  #endif

  // check that the HFXO is off
  if ((HFXO0->STATUS & _HFXO_STATUS_ENS_MASK) != HFXO_STATUS_ENS) {
    HFXO0->XTALCTRL &= ~_HFXO_XTALCTRL_CTUNEXIANA_MASK;
    HFXO0->XTALCTRL |= (tune << _HFXO_XTALCTRL_CTUNEXIANA_SHIFT)
                       & _HFXO_XTALCTRL_CTUNEXIANA_MASK;
    HFXO0->XTALCTRL &= ~_HFXO_XTALCTRL_CTUNEXOANA_MASK;
    HFXO0->XTALCTRL |= (tune << _HFXO_XTALCTRL_CTUNEXOANA_SHIFT)
                       & _HFXO_XTALCTRL_CTUNEXOANA_MASK;
  }

  // clear DISONDEMAND and set FORCEEN to reenable HFXO
  HFXO0->CTRL &= ~_HFXO_CTRL_DISONDEMAND_MASK;
  HFXO0->CTRL |= _HFXO_CTRL_FORCEEN_MASK;

  CMU_ClockSelectSet(cmuClock_SYSCLK, cmuSelect_HFXO);

  #endif // _CMU_HFXOSTARTUPCTRL_CTUNE_MASK
}

uint16_t halInternalGetCtune(void)
{
  #if defined(_CMU_HFXOSTARTUPCTRL_CTUNE_MASK) // series 1
  return (CMU->HFXOSTEADYSTATECTRL & _CMU_HFXOSTEADYSTATECTRL_CTUNE_MASK)
         >> _CMU_HFXOSTEADYSTATECTRL_CTUNE_SHIFT;
  #elif defined(_HFXO_XTALCTRL_CTUNEXIANA_MASK) // series 2
  return (HFXO0->XTALCTRL & _HFXO_XTALCTRL_CTUNEXIANA_MASK)
         >> _HFXO_XTALCTRL_CTUNEXIANA_SHIFT;
  #else //!_CMU_HFXOSTEADYSTATECTRL_CTUNE_MASK
  return 0U;
  #endif //_CMU_HFXOSTEADYSTATECTRL_CTUNE_MASK
}

#if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT)
  #define BUTTON_COUNT (SL_SIMPLE_BUTTON_COUNT)
#else
  #define BUTTON_COUNT (0U)
#endif

// Make sure our data type is large enough to handle all buttons
#if BUTTON_COUNT > 256U
  #error Legacy HAL can not accomodate more than 256 buttons!
#endif

void halInternalInitButton(void)
{
  #if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT)
  sl_simple_button_init_instances();
  #endif
}

uint8_t halButtonState(uint8_t button)
{
  #if BUTTON_COUNT > 0
  assert(button < BUTTON_COUNT);
  const sl_button_t * ptr = SL_SIMPLE_BUTTON_INSTANCE(button);
  sl_button_state_t state = sl_button_get_state(ptr);
  assert(state != (sl_button_state_t)BUTTON_ERROR
         && state != SL_SIMPLE_BUTTON_DISABLED);

  return (state == SL_SIMPLE_BUTTON_PRESSED) ? BUTTON_PRESSED : BUTTON_RELEASED;
  #else
  (void)button;
  assert(false);
  // The assert above should prevent us from getting here, but this return
  // prevents warnings or errors from compilers and static analyzers.
  return (0U);
  #endif
}

uint8_t halButtonPinState(uint8_t button)
{
  #if BUTTON_COUNT > 0
  assert(button < BUTTON_COUNT);
  assert(SL_SIMPLE_BUTTON_INSTANCE(button) != NULL);
  sl_simple_button_context_t * context = SL_SIMPLE_BUTTON_INSTANCE(button)->context;
  assert(context != NULL);
  GPIO_Port_TypeDef port = SL_SIMPLE_BUTTON_GET_PORT(context);
  uint8_t pin = SL_SIMPLE_BUTTON_GET_PIN(context);
  uint8_t state = (uint8_t)GPIO_PinInGet(port, pin);
  return (state == SL_SIMPLE_BUTTON_POLARITY) ? BUTTON_PRESSED : BUTTON_RELEASED;
  #else
  (void)button;
  assert(false);
  // The assert above should prevent us from getting here, but this return
  // prevents warnings or errors from compilers and static analyzers.
  return (0U);
  #endif
}

#if (BUTTON_COUNT > 0) && LEGACY_HAL_TRANSLATE_BUTTON_INTERRUPT

SL_WEAK void sl_button_on_change(const sl_button_t *handle)
{
  sl_button_state_t sl_state = sl_button_get_state(handle);

  assert(sl_state != (sl_button_state_t)BUTTON_ERROR
         && sl_state != SL_SIMPLE_BUTTON_DISABLED);

  uint8_t em_state = (sl_state == SL_SIMPLE_BUTTON_PRESSED)
                     ? BUTTON_PRESSED
                     : BUTTON_RELEASED;

  int button;

  for (button = 0; button < BUTTON_COUNT; button++) {
    if (handle == SL_SIMPLE_BUTTON_INSTANCE(button)) {
      halButtonIsr(button, em_state);
      break;
    }
  }

  assert(button < BUTTON_COUNT);
}

SL_WEAK void halButtonIsr(uint8_t button, uint8_t state)
{
  // Weak stub implementation that can be overridden by the application
  // Will only be called for changes in buttons configured for interrupt mode
  (void) button;
  (void) state;
}

#endif // SL_CATALOG_SIMPLE_BUTTON_PRESENT + LEGACY_HAL_TRANSLATE_BUTTON_INTERRUPT
