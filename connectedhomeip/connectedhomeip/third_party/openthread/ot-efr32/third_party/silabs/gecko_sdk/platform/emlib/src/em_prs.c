/***************************************************************************//**
 * @file
 * @brief Peripheral Reflex System (PRS) Peripheral API
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "em_prs.h"
#if defined(PRS_COUNT) && (PRS_COUNT > 0)

#include "sl_assert.h"

/***************************************************************************//**
 * @addtogroup prs PRS - Peripheral Reflex System
 * @brief Peripheral Reflex System (PRS) Peripheral API
 * @details
 *  This module contains functions to control the PRS peripheral of Silicon
 *  Labs 32-bit MCUs and SoCs. The PRS allows configurable, fast, and autonomous
 *  communication between peripherals on the MCU or SoC.
 * @{
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

/* Generic defines for async and sync signals applying to all TIMER instances.
 * Those defines map to TIMER2 but it could be any TIMER instance number. */
#define   _PRS_ASYNC_CH_CTRL_SIGSEL_TIMERUF   _PRS_ASYNC_CH_CTRL_SIGSEL_TIMER2UF
#define   _PRS_ASYNC_CH_CTRL_SIGSEL_TIMEROF   _PRS_ASYNC_CH_CTRL_SIGSEL_TIMER2OF
#define   _PRS_ASYNC_CH_CTRL_SIGSEL_TIMERCC0  _PRS_ASYNC_CH_CTRL_SIGSEL_TIMER2CC0
#define   _PRS_ASYNC_CH_CTRL_SIGSEL_TIMERCC1  _PRS_ASYNC_CH_CTRL_SIGSEL_TIMER2CC1
#define   _PRS_ASYNC_CH_CTRL_SIGSEL_TIMERCC2  _PRS_ASYNC_CH_CTRL_SIGSEL_TIMER2CC2

#define   _PRS_SYNC_CH_CTRL_SIGSEL_TIMERUF   _PRS_SYNC_CH_CTRL_SIGSEL_TIMER2UF
#define   _PRS_SYNC_CH_CTRL_SIGSEL_TIMEROF   _PRS_SYNC_CH_CTRL_SIGSEL_TIMER2OF
#define   _PRS_SYNC_CH_CTRL_SIGSEL_TIMERCC0  _PRS_SYNC_CH_CTRL_SIGSEL_TIMER2CC0
#define   _PRS_SYNC_CH_CTRL_SIGSEL_TIMERCC1  _PRS_SYNC_CH_CTRL_SIGSEL_TIMER2CC1
#define   _PRS_SYNC_CH_CTRL_SIGSEL_TIMERCC2  _PRS_SYNC_CH_CTRL_SIGSEL_TIMER2CC2

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Get PRS source signal for a channel.
 *
 * @param[in] type
 *   PRS channel type. This can be either @ref prsTypeAsync or
 *   @ref prsTypeSync.
 *
 * @param[in] ch
 *   channel number.
 *
 * @return
 *   PRS signal assigned to the channel.
 ******************************************************************************/
static PRS_Signal_t getSignal(unsigned int ch, PRS_ChType_t type)
{
  PRS_Signal_t signal;

#if defined(_PRS_ASYNC_CH_CTRL_SOURCESEL_MASK)
  if (type == prsTypeAsync) {
    signal = (PRS_Signal_t) (PRS->ASYNC_CH[ch].CTRL
                             & (_PRS_ASYNC_CH_CTRL_SOURCESEL_MASK | _PRS_ASYNC_CH_CTRL_SIGSEL_MASK));
  } else {
    signal = (PRS_Signal_t) (PRS->SYNC_CH[ch].CTRL
                             & (_PRS_SYNC_CH_CTRL_SOURCESEL_MASK | _PRS_SYNC_CH_CTRL_SIGSEL_MASK));
  }
#else
  (void) type;
  signal = (PRS_Signal_t) (PRS->CH[ch].CTRL
                           & (_PRS_CH_CTRL_SOURCESEL_MASK | _PRS_CH_CTRL_SIGSEL_MASK));
#endif
  return signal;
}

/** @endcond */

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

#if defined(_SILICON_LABS_32B_SERIES_2)
/***************************************************************************//**
 * @brief
 *   Convert an async PRS source to a sync source.
 *
 * @details
 *   This conversion must be done because the id's of the same peripheral
 *   source is different depending on if it's used as an asynchronous PRS source
 *   or a synchronous PRS source.
 *
 * @param[in] asyncSource
 *   The id of the asynchronous PRS source.
 *
 * @return
 *   The id of the corresponding synchronous PRS source.
 ******************************************************************************/
uint32_t PRS_ConvertToSyncSource(uint32_t asyncSource)
{
  uint32_t syncSource = 0;

  switch (asyncSource) {
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_NONE:
      syncSource = _PRS_SYNC_CH_CTRL_SOURCESEL_NONE;
      break;
#if defined(IADC_PRESENT)
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_IADC0:
      syncSource = _PRS_SYNC_CH_CTRL_SOURCESEL_IADC0;
      break;
#endif
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_TIMER0:
      syncSource = _PRS_SYNC_CH_CTRL_SOURCESEL_TIMER0;
      break;
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_TIMER1:
      syncSource = _PRS_SYNC_CH_CTRL_SOURCESEL_TIMER1;
      break;
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_TIMER2:
      syncSource = _PRS_SYNC_CH_CTRL_SOURCESEL_TIMER2;
      break;
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_TIMER3:
      syncSource = _PRS_SYNC_CH_CTRL_SOURCESEL_TIMER3;
      break;
#if defined(TIMER4)
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_TIMER4:
      syncSource = _PRS_SYNC_CH_CTRL_SOURCESEL_TIMER4;
      break;
#endif
#if defined(TIMER5)
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_TIMER5:
      syncSource = _PRS_SYNC_CH_CTRL_SOURCESEL_TIMER5;
      break;
#endif
#if defined(TIMER6)
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_TIMER6:
      syncSource = _PRS_SYNC_CH_CTRL_SOURCESEL_TIMER6;
      break;
#endif
#if defined(TIMER7)
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_TIMER7:
      syncSource = _PRS_SYNC_CH_CTRL_SOURCESEL_TIMER7;
      break;
#endif
#if defined(VDAC0)
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_VDAC0L:
      syncSource = _PRS_SYNC_CH_CTRL_SOURCESEL_VDAC0;
      break;
#endif
#if defined(VDAC1)
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_VDAC1L:
      syncSource = _PRS_SYNC_CH_CTRL_SOURCESEL_VDAC1;
      break;
#endif
    default:
      EFM_ASSERT(false);
      break;
  }
  return syncSource;
}

/***************************************************************************//**
 * @brief
 *   Convert an async PRS signal to a sync signal.
 *
 * @details
 *   PRS values for some peripherals signals differ between asynchronous and
 *   synchronous PRS channels. This function must be used to handle the
 *   conversion.
 *
 * @param[in] asyncSource
 *   The id of the asynchronous PRS source.
 *
 * @param[in] asyncSignal
 *   The id of the asynchronous PRS signal.
 *
 * @return
 *   The id of the corresponding synchronous PRS signal.
 ******************************************************************************/
uint32_t PRS_ConvertToSyncSignal(uint32_t asyncSource, uint32_t asyncSignal)
{
  uint32_t syncSignal = asyncSignal;

  switch (asyncSource) {
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_TIMER0:
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_TIMER1:
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_TIMER2:
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_TIMER3:
#if defined(_PRS_ASYNC_CH_CTRL_SOURCESEL_TIMER4)
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_TIMER4:
#endif
#if defined(_PRS_ASYNC_CH_CTRL_SOURCESEL_TIMER5)
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_TIMER5:
#endif
#if defined(_PRS_ASYNC_CH_CTRL_SOURCESEL_TIMER6)
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_TIMER6:
#endif
#if defined(_PRS_ASYNC_CH_CTRL_SOURCESEL_TIMER7)
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_TIMER7:
#endif
      /* Async and sync signal values are consistent across all timers instances.
       * Generic defines are used. */
      switch (asyncSignal) {
        case _PRS_ASYNC_CH_CTRL_SIGSEL_TIMERUF:
          syncSignal = _PRS_SYNC_CH_CTRL_SIGSEL_TIMERUF;
          break;
        case _PRS_ASYNC_CH_CTRL_SIGSEL_TIMEROF:
          syncSignal = _PRS_SYNC_CH_CTRL_SIGSEL_TIMEROF;
          break;
        case _PRS_ASYNC_CH_CTRL_SIGSEL_TIMERCC0:
          syncSignal = _PRS_SYNC_CH_CTRL_SIGSEL_TIMERCC0;
          break;
        case _PRS_ASYNC_CH_CTRL_SIGSEL_TIMERCC1:
          syncSignal = _PRS_SYNC_CH_CTRL_SIGSEL_TIMERCC1;
          break;
        case _PRS_ASYNC_CH_CTRL_SIGSEL_TIMERCC2:
          syncSignal = _PRS_SYNC_CH_CTRL_SIGSEL_TIMERCC2;
          break;
        default:
          EFM_ASSERT(false);
          break;
      }
      break;
#if defined(IADC0)
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_IADC0:
      switch (asyncSignal) {
        case _PRS_ASYNC_CH_CTRL_SIGSEL_IADC0SCANENTRYDONE:
          syncSignal = _PRS_SYNC_CH_CTRL_SIGSEL_IADC0SCANENTRYDONE;
          break;
        case _PRS_ASYNC_CH_CTRL_SIGSEL_IADC0SCANTABLEDONE:
          syncSignal = _PRS_SYNC_CH_CTRL_SIGSEL_IADC0SCANTABLEDONE;
          break;
        case _PRS_ASYNC_CH_CTRL_SIGSEL_IADC0SINGLEDONE:
          syncSignal = _PRS_SYNC_CH_CTRL_SIGSEL_IADC0SINGLEDONE;
          break;
        default:
          EFM_ASSERT(false);
          break;
      }
      break;
#endif
#if defined(VDAC0)
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_VDAC0L:
      switch (asyncSignal) {
        case _PRS_ASYNC_CH_CTRL_SIGSEL_VDAC0LCH0DONEASYNC:
          syncSignal = _PRS_SYNC_CH_CTRL_SIGSEL_VDAC0CH0DONESYNC;
          break;
        case _PRS_ASYNC_CH_CTRL_SIGSEL_VDAC0LCH1DONEASYNC:
          syncSignal = _PRS_SYNC_CH_CTRL_SIGSEL_VDAC0CH1DONESYNC;
          break;
        default:
          EFM_ASSERT(false);
          break;
      }
      break;
#endif
#if defined(VDAC1)
    case _PRS_ASYNC_CH_CTRL_SOURCESEL_VDAC1L:
      switch (asyncSignal) {
        case _PRS_ASYNC_CH_CTRL_SIGSEL_VDAC1LCH0DONEASYNC:
          syncSignal = _PRS_SYNC_CH_CTRL_SIGSEL_VDAC1CH0DONESYNC;
          break;
        case _PRS_ASYNC_CH_CTRL_SIGSEL_VDAC1LCH1DONEASYNC:
          syncSignal = _PRS_SYNC_CH_CTRL_SIGSEL_VDAC1CH1DONESYNC;
          break;
        default:
          EFM_ASSERT(false);
          break;
      }
      break;
#endif
    default:
      // No translation
      break;
  }
  return syncSignal;
}
#endif

/***************************************************************************//**
 * @brief
 *   Set a source and signal for a channel.
 *
 * @param[in] ch
 *   A channel to define the signal and source for.
 *
 * @param[in] source
 *   A source to select for the channel. Use one of PRS_CH_CTRL_SOURCESEL_x defines.
 *
 * @param[in] signal
 *   A signal (for selected @p source) to use. Use one of PRS_CH_CTRL_SIGSEL_x
 *   defines.
 *
 * @param[in] edge
 *   An edge (for selected source/signal) to generate the pulse for.
 ******************************************************************************/
void PRS_SourceSignalSet(unsigned int ch,
                         uint32_t source,
                         uint32_t signal,
                         PRS_Edge_TypeDef edge)
{
#if defined(_PRS_SYNC_CH_CTRL_MASK)
  (void) edge;
  EFM_ASSERT(ch < PRS_SYNC_CHAN_COUNT);
  PRS->SYNC_CH[ch].CTRL = (source & _PRS_SYNC_CH_CTRL_SOURCESEL_MASK)
                          | (signal & _PRS_SYNC_CH_CTRL_SIGSEL_MASK);
#else
  EFM_ASSERT(ch < PRS_CHAN_COUNT);
  PRS->CH[ch].CTRL = (source & _PRS_CH_CTRL_SOURCESEL_MASK)
                     | (signal & _PRS_CH_CTRL_SIGSEL_MASK)
                     | (uint32_t)edge << _PRS_CH_CTRL_EDSEL_SHIFT;
#endif
}

#if defined(PRS_ASYNC_SUPPORTED)
/***************************************************************************//**
 * @brief
 *   Set the source and asynchronous signal for a channel.
 *
 * @details
 *   Asynchronous reflexes are not clocked on HFPERCLK and can be used even in
 *   EM2/EM3.
 *   There is a limitation to reflexes operating in asynchronous mode in
 *   that they can only be used by a subset of the reflex consumers. See
 *   the PRS chapter in the reference manual for the complete list of
 *   supported asynchronous signals and consumers.
 *
 * @note
 *   This function is not supported on EFM32GxxxFyyy parts.
 *   In asynchronous mode, the edge detector only works in EM0 and should
 *   not be used. The EDSEL parameter in PRS_CHx_CTRL register is set to 0 (OFF)
 *   by default.
 *
 * @param[in] ch
 *   A channel to define the source and asynchronous signal for.
 *
 * @param[in] source
 *   A source to select for the channel. Use one of PRS_CH_CTRL_SOURCESEL_x defines.
 *
 * @param[in] signal
 *   An asynchronous signal (for selected @p source) to use. Use one of the
 *   PRS_CH_CTRL_SIGSEL_x defines that support asynchronous operation.
 ******************************************************************************/
void PRS_SourceAsyncSignalSet(unsigned int ch,
                              uint32_t source,
                              uint32_t signal)
{
  PRS_ConnectSignal(ch, prsTypeAsync, (PRS_Signal_t) (source | signal));
}
#endif

#if defined(_PRS_ROUTELOC0_MASK) || (_PRS_ROUTE_MASK)
/***************************************************************************//**
 * @brief
 *   Send the output of a PRS channel to a GPIO pin.
 *
 * @details
 *   This function is used to send the output of a PRS channel to a GPIO pin.
 *   Note that there are certain restrictions to where a PRS channel can be
 *   routed. Consult the datasheet of the device to see if a channel can be
 *   routed to the requested GPIO pin.
 *
 * @param[in] ch
 *   PRS channel number.
 *
 * @param[in] location
 *   PRS routing location.
 ******************************************************************************/
void PRS_GpioOutputLocation(unsigned int ch,
                            unsigned int location)
{
  EFM_ASSERT(ch < PRS_CHAN_COUNT);

#if defined(_PRS_ROUTE_MASK)
  PRS->ROUTE |= (location << _PRS_ROUTE_LOCATION_SHIFT)
                | (1 << ch);
#else
  uint32_t shift = (ch % 4) * 8;
  uint32_t mask = location << shift;
  uint32_t locationGroup = ch / 4;
  /* Since all ROUTELOCx registers are in consecutive memory locations, treat them
   * as an array starting at ROUTELOC0 and use locationGroup to index into this array */
  volatile uint32_t * routeloc = &PRS->ROUTELOC0;
  routeloc[locationGroup] |= mask;
  PRS->ROUTEPEN |= 1 << ch;
#endif
}
#endif

/***************************************************************************//**
 * @brief
 *   Search for the first free PRS channel.
 *
 * @param[in] type
 *   PRS channel type. This can be either @ref prsTypeAsync or
 *   @ref prsTypeSync.
 *
 * @return
 *   Channel number >= 0 if an unused PRS channel was found. If no free PRS
 *   channel was found then -1 is returned.
 ******************************************************************************/
int PRS_GetFreeChannel(PRS_ChType_t type)
{
  int ch = -1;
  PRS_Signal_t signal;
  int max;

  if (type == prsTypeAsync) {
    max = PRS_ASYNC_CHAN_COUNT;
  } else {
    max = PRS_SYNC_CHAN_COUNT;
  }

  for (int i = 0; i < max; i++) {
    signal = getSignal(i, type);
    if (signal == prsSignalNone) {
      ch = i;
      break;
    }
  }
  return ch;
}

/***************************************************************************//**
 * @brief
 *   Reset all PRS channels
 *
 * @details
 *   This function will reset all the PRS channel configuration.
 ******************************************************************************/
void PRS_Reset(void)
{
  unsigned int i;

#if defined(_SILICON_LABS_32B_SERIES_2)
  PRS->ASYNC_SWLEVEL = 0;
  for (i = 0; i < PRS_ASYNC_CHAN_COUNT; i++) {
    PRS->ASYNC_CH[i].CTRL = _PRS_ASYNC_CH_CTRL_RESETVALUE;
  }
  for (i = 0; i < PRS_SYNC_CHAN_COUNT; i++) {
    PRS->SYNC_CH[i].CTRL = _PRS_SYNC_CH_CTRL_RESETVALUE;
  }
#else
  PRS->SWLEVEL = 0x0;
  for (i = 0; i < PRS_CHAN_COUNT; i++) {
    PRS->CH[i].CTRL = _PRS_CH_CTRL_RESETVALUE;
  }
#endif
}

/***************************************************************************//**
 * @brief
 *   Connect a PRS signal to a channel.
 *
 * @details
 *   This function will make the PRS signal available on the specific channel.
 *   Only a single PRS signal can be connected to any given channel.
 *
 * @param[in] ch
 *   PRS channel number.
 *
 * @param[in] type
 *   PRS channel type. This can be either @ref prsTypeAsync or
 *   @ref prsTypeSync.
 *
 * @param[in] signal
 *   This is the PRS signal that should be placed on the channel.
 ******************************************************************************/
void PRS_ConnectSignal(unsigned int ch, PRS_ChType_t type, PRS_Signal_t signal)
{
#if defined(_PRS_ASYNC_CH_CTRL_MASK)
  // Series 2 devices
  uint32_t sourceField = ((uint32_t)signal & _PRS_ASYNC_CH_CTRL_SOURCESEL_MASK)
                         >> _PRS_ASYNC_CH_CTRL_SOURCESEL_SHIFT;
  uint32_t signalField = ((uint32_t)signal & _PRS_ASYNC_CH_CTRL_SIGSEL_MASK)
                         >> _PRS_ASYNC_CH_CTRL_SIGSEL_SHIFT;
  if (type == prsTypeAsync) {
    EFM_ASSERT(ch < PRS_ASYNC_CHAN_COUNT);
    PRS->ASYNC_CH[ch].CTRL = PRS_ASYNC_CH_CTRL_FNSEL_A
                             | (sourceField << _PRS_ASYNC_CH_CTRL_SOURCESEL_SHIFT)
                             | (signalField << _PRS_ASYNC_CH_CTRL_SIGSEL_SHIFT);
  } else {
    EFM_ASSERT(ch < PRS_SYNC_CHAN_COUNT);
    signalField = PRS_ConvertToSyncSignal(sourceField, signalField);
    sourceField = PRS_ConvertToSyncSource(sourceField);
    PRS->SYNC_CH[ch].CTRL = (sourceField << _PRS_SYNC_CH_CTRL_SOURCESEL_SHIFT)
                            | (signalField << _PRS_SYNC_CH_CTRL_SIGSEL_SHIFT);
  }
#else
  // Series 0 and Series 1 devices
  uint32_t signalField = (uint32_t) signal & (_PRS_CH_CTRL_SOURCESEL_MASK
                                              | _PRS_CH_CTRL_SIGSEL_MASK);
  if (type == prsTypeAsync) {
#if defined(PRS_ASYNC_SUPPORTED)
    EFM_ASSERT(ch < PRS_ASYNC_CHAN_COUNT);
    PRS->CH[ch].CTRL = PRS_CH_CTRL_EDSEL_OFF
                       | PRS_CH_CTRL_ASYNC
                       | signalField;
#endif
  } else {
    EFM_ASSERT(ch < PRS_SYNC_CHAN_COUNT);
    PRS->CH[ch].CTRL = PRS_CH_CTRL_EDSEL_OFF
                       | signalField;
  }
#endif
}

#if defined(_SILICON_LABS_32B_SERIES_2)
/***************************************************************************//**
 * @brief
 *   Connect a peripheral consumer to a PRS channel.
 *
 * @details
 *   Different peripherals can use PRS channels as their input. This function
 *   can be used to connect a peripheral consumer to a PRS channel. Multiple
 *   consumers can be connected to a single PRS channel.
 *
 * @param[in] ch
 *   PRS channel number.
 *
 * @param[in] type
 *   PRS channel type. This can be either @ref prsTypeAsync or
 *   @ref prsTypeSync.
 *
 * @param[in] consumer
 *   This is the PRS consumer.
 ******************************************************************************/
void PRS_ConnectConsumer(unsigned int ch, PRS_ChType_t type, PRS_Consumer_t consumer)
{
  EFM_ASSERT((uint32_t)consumer <= 0xFFF);
  volatile uint32_t * addr = (volatile uint32_t *) PRS;
  uint32_t offset = (uint32_t) consumer;
  addr = addr + offset / 4;

  if (consumer != prsConsumerNone) {
    if (type == prsTypeAsync) {
      *addr = ch << _PRS_CONSUMER_TIMER0_CC0_PRSSEL_SHIFT;
    } else {
      *addr = ch << _PRS_CONSUMER_TIMER0_CC0_SPRSSEL_SHIFT;
    }
  }
}

/***************************************************************************//**
 * @brief
 *   Send the output of a PRS channel to a GPIO pin.
 *
 * @details
 *   This function is used to send the output of a PRS channel to a GPIO pin.
 *   Note that there are certain restrictions to where a PRS channel can be
 *   routed. Consult the datasheet of the device to see if a channel can be
 *   routed to the requested GPIO pin. Some devices for instance can only route
 *   the async channels 0-5 on GPIO pins PAx and PBx while async channels 6-11
 *   can only be routed to GPIO pins PCx and PDx
 *
 * @param[in] ch
 *   PRS channel number.
 *
 * @param[in] type
 *   PRS channel type. This can be either @ref prsTypeAsync or
 *   @ref prsTypeSync.
 *
 * @param[in] port
 *   GPIO port
 *
 * @param[in] pin
 *   GPIO pin
 ******************************************************************************/
void PRS_PinOutput(unsigned int ch, PRS_ChType_t type, GPIO_Port_TypeDef port, uint8_t pin)
{
  volatile uint32_t * addr;
  if (type == prsTypeAsync) {
    addr = &GPIO->PRSROUTE[0].ASYNCH0ROUTE;
  } else {
    addr = &GPIO->PRSROUTE[0].SYNCH0ROUTE;
  }
  addr += ch;
  *addr = ((uint32_t)port << _GPIO_PRS_ASYNCH0ROUTE_PORT_SHIFT)
          | ((uint32_t)pin << _GPIO_PRS_ASYNCH0ROUTE_PIN_SHIFT);

  if (type == prsTypeAsync) {
    GPIO->PRSROUTE[0].ROUTEEN |= 0x1 << (ch + _GPIO_PRS_ROUTEEN_ASYNCH0PEN_SHIFT);
  } else {
    GPIO->PRSROUTE[0].ROUTEEN |= 0x1 << (ch + _GPIO_PRS_ROUTEEN_SYNCH0PEN_SHIFT);
  }
}

/***************************************************************************//**
 * @brief
 *   Combine two PRS channels using a logic function.
 *
 * @details
 *   This function allows you to combine the output of one PRS channel with the
 *   the signal of another PRS channel using various logic functions. Note that
 *   for series 2, config 1 devices, the hardware only allows a PRS channel to
 *   be combined with the previous channel. So for instance channel 5 can be
 *   combined only with channel 4.
 *
 *   The logic function operates on two PRS channels called A and B. The output
 *   of PRS channel B is combined with the PRS source configured for channel A
 *   to produce an output. This output is used as the output of channel A.
 *
 * @param[in] chA
 *   PRS Channel for the A input.
 *
 * @param[in] chB
 *   PRS Channel for the B input.
 *
 * @param[in] logic
 *   The logic function to use when combining the Channel A and Channel B. The
 *   output of the logic function is the output of Channel A. Function like
 *   AND, OR, XOR, NOT and more are available.
 ******************************************************************************/
void PRS_Combine(unsigned int chA, unsigned int chB, PRS_Logic_t logic)
{
  EFM_ASSERT(chA < PRS_ASYNC_CHAN_COUNT);
  EFM_ASSERT(chB < PRS_ASYNC_CHAN_COUNT);

#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
  EFM_ASSERT(chA == ((chB + 1) % PRS_ASYNC_CHAN_COUNT));
  PRS->ASYNC_CH[chA].CTRL = (PRS->ASYNC_CH[chA].CTRL & ~_PRS_ASYNC_CH_CTRL_FNSEL_MASK)
                            | ((uint32_t)logic << _PRS_ASYNC_CH_CTRL_FNSEL_SHIFT);

#else
  PRS->ASYNC_CH[chA].CTRL = (PRS->ASYNC_CH[chA].CTRL
                             & ~(_PRS_ASYNC_CH_CTRL_FNSEL_MASK
                                 | _PRS_ASYNC_CH_CTRL_AUXSEL_MASK))
                            | ((uint32_t)logic << _PRS_ASYNC_CH_CTRL_FNSEL_SHIFT)
                            | ((uint32_t)chB << _PRS_ASYNC_CH_CTRL_AUXSEL_SHIFT);
#endif
}
#endif

/** @} (end addtogroup prs) */
#endif /* defined(PRS_COUNT) && (PRS_COUNT > 0) */
