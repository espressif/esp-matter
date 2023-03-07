/***************************************************************************//**
 * @file fem-control.c
 * @brief This file implements a simple API for configuring FEM control signals via PRS.
 * @copyright Copyright 2018 Silicon Laboratories, Inc. www.silabs.com
 ******************************************************************************/

#include "hal-config.h"
#include "em_device.h"
#include "em_assert.h"
#include "em_gpio.h"
#include "em_bus.h"
#include "em_cmu.h"
#include "em_prs.h"

// If RX FEM control is specified ensure a proper mapping is given
#if HAL_FEM_RX_ACTIVE == 1
#ifdef _SILICON_LABS_32B_SERIES_1
#if (!defined(BSP_FEM_RX_CHANNEL) \
  || !defined(BSP_FEM_RX_PORT)    \
  || !defined(BSP_FEM_RX_PIN)     \
  || !defined(BSP_FEM_RX_LOC))
  #error "BSP_FEM_RX_CHANNEL/PORT/PIN/LOC must be defined."
#endif
#else //_SILICON_LABS_32B_SERIES_1
#if (!defined(BSP_FEM_RX_CHANNEL) \
  || !defined(BSP_FEM_RX_PORT)    \
  || !defined(BSP_FEM_RX_PIN))
  #error "BSP_FEM_RX_CHANNEL/PORT/PIN must be defined."
#endif
#endif //!_SILICON_LABS_32B_SERIES_1
#endif // HAL_FEM_RX_ACTIVE

#if HAL_FEM_TX_ACTIVE == 1
// if no separate CTX pin is defined, CRX is a combined rx tx pin
#if !defined(BSP_FEM_TX_CHANNEL) && (HAL_FEM_RX_ACTIVE == 1)
  #define BSP_FEM_TX_PORT BSP_FEM_RX_PORT
  #define BSP_FEM_TX_PIN BSP_FEM_RX_PIN
  #define BSP_FEM_TX_CHANNEL BSP_FEM_RX_CHANNEL
#if _SILICON_LABS_32B_SERIES_1
  #define BSP_FEM_TX_LOC BSP_FEM_RX_LOC
#endif //_SILICON_LABS_32B_SERIES_1
#elif !defined(BSP_FEM_TX_CHANNEL)
  #error "BSP_FEM_TX_CHANNEL/PORT/PIN must be defined."
#endif
#endif // HAL_FEM_TX_ACTIVE

#if HAL_FEM_TX_ACTIVE == 1 && HAL_FEM_RX_ACTIVE == 1
  #if BSP_FEM_RX_CHANNEL == BSP_FEM_TX_CHANNEL
    #error The RX and TX PRS channels cannot be equal.
  #endif
#endif

#if (HAL_FEM_RX_ACTIVE == 1) && defined(BSP_FEM_SLEEP_CHANNEL)
#if (BSP_FEM_RX_CHANNEL + 1) != BSP_FEM_SLEEP_CHANNEL
  #error "Sleep pin channel must immediately follow RX channel"
#endif
#endif // HAL_FEM_RX_ACTIVE

#ifndef PRS_CHAN_COUNT
#define PRS_CHAN_COUNT PRS_ASYNC_CHAN_COUNT
#endif

#if defined(BSP_FEM_SLEEP_CHANNEL) && BSP_FEM_SLEEP_CHANNEL >= PRS_CHAN_COUNT
  #error "FEM_SLEEP channel number higher than number of PRS channels"
#endif

#if HAL_FEM_TX_ACTIVE
#if BSP_FEM_TX_CHANNEL >= PRS_CHAN_COUNT
  #error "FEM_TX channel number higher than number of PRS channels"
#endif
#endif

// Find the signals for PA enable based on the chip
#if defined(PRS_RAC_PAEN)
#define PA_ENABLE_PRS_SIGNAL PRS_RAC_PAEN
#elif defined(PRS_RACL_PAEN)
#define PA_ENABLE_PRS_SIGNAL PRS_RACL_PAEN
#else
#error "PA enable signal not defined for this device"
#endif

// Find the signals for PA enable based on the chip
#if defined(PRS_RAC_LNAEN)
#define LNA_ENABLE_PRS_SIGNAL PRS_RAC_LNAEN
#elif defined(PRS_RACL_LNAEN)
#define LNA_ENABLE_PRS_SIGNAL PRS_RACL_LNAEN
#else
#error "LNA enable signal not defined for this device"
#endif

void initFem(void)
{
  // Turn on the GPIO clock so that we can turn on GPIOs
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_PRS, true);

#ifdef _SILICON_LABS_32B_SERIES_1
  volatile uint32_t * routeRegister;
#endif //_SILICON_LABS_32B_SERIES_1
#if HAL_FEM_TX_ACTIVE == 1
  //Enable the output of TX_ACTIVE based on a specific port and pin
  //Configure the tx gpio to be an output (FEM pin CTX)
  GPIO_PinModeSet(BSP_FEM_TX_PORT, BSP_FEM_TX_PIN, gpioModePushPull, 0);
  //Setup the PRS to output TX_ACTIVE on the channel and location chosen.
#ifdef _SILICON_LABS_32B_SERIES_1
  PRS->CH[BSP_FEM_TX_CHANNEL].CTRL = PA_ENABLE_PRS_SIGNAL;

  // Configure TX/RX PRS output to selected channel and location
  if (BSP_FEM_TX_CHANNEL < 4) {
    routeRegister = &PRS->ROUTELOC0;
  } else if (BSP_FEM_TX_CHANNEL < 8) {
    routeRegister = &PRS->ROUTELOC1;
  } else if (BSP_FEM_TX_CHANNEL < 12) {
    routeRegister = &PRS->ROUTELOC2;
  } else {
    EFM_ASSERT(0);
    return; // error
  }

  BUS_RegMaskedWrite(routeRegister,
                     0xFF << ((BSP_FEM_TX_CHANNEL % 4) * 8),
                     BSP_FEM_TX_LOC << ((BSP_FEM_TX_CHANNEL % 4) * 8));

  BUS_RegMaskedSet(&PRS->ROUTEPEN, (1 << BSP_FEM_TX_CHANNEL));
#else //!_SILICON_LABS_32B_SERIES_1
  PRS_SourceAsyncSignalSet(BSP_FEM_TX_CHANNEL,
                           0U,
                           PA_ENABLE_PRS_SIGNAL);
  PRS_PinOutput(BSP_FEM_TX_CHANNEL, prsTypeAsync, BSP_FEM_TX_PORT, BSP_FEM_TX_PIN);
#endif //_SILICON_LABS_32B_SERIES_1
#endif //HAL_FEM_TX_ACTIVE == 1

#if HAL_FEM_RX_ACTIVE == 1
  //Enable the output of RX_ACTIVE based on a specific port and pin
  //Configure the rx gpio to be an output (FEM pin CRX)
  GPIO_PinModeSet(BSP_FEM_RX_PORT, BSP_FEM_RX_PIN, gpioModePushPull, 0);

#ifdef _SILICON_LABS_32B_SERIES_1
  //Setup the PRS to output RX_ACTIVE on the channel and location chosen.
  PRS->CH[BSP_FEM_RX_CHANNEL].CTRL = LNA_ENABLE_PRS_SIGNAL;

  // Configure TX/RX PRS output to selected channel and location
  if (BSP_FEM_RX_CHANNEL < 4) {
    routeRegister = &PRS->ROUTELOC0;
  } else if (BSP_FEM_RX_CHANNEL < 8) {
    routeRegister = &PRS->ROUTELOC1;
  } else if (BSP_FEM_RX_CHANNEL < 12) {
    routeRegister = &PRS->ROUTELOC2;
  } else {
    EFM_ASSERT(0);
    return; // error
  }

  BUS_RegMaskedWrite(routeRegister,
                     0xFF << ((BSP_FEM_RX_CHANNEL % 4) * 8),
                     BSP_FEM_RX_LOC << ((BSP_FEM_RX_CHANNEL % 4) * 8));

  BUS_RegMaskedSet(&PRS->ROUTEPEN, (1 << BSP_FEM_RX_CHANNEL));
#else //!_SILICON_LABS_32B_SERIES_1
  PRS_SourceAsyncSignalSet(BSP_FEM_RX_CHANNEL,
                           0U,
                           LNA_ENABLE_PRS_SIGNAL);
  PRS_PinOutput(BSP_FEM_RX_CHANNEL, prsTypeAsync, BSP_FEM_RX_PORT, BSP_FEM_RX_PIN);
#endif //_SILICON_LABS_32B_SERIES_1
#endif //HAL_FEM_RX_ACTIVE == 1

#if defined(BSP_FEM_SLEEP_CHANNEL)
// initialize sleep as output (FEM pin CSD)
  GPIO_PinModeSet(BSP_FEM_SLEEP_PORT, BSP_FEM_SLEEP_PIN, gpioModePushPull, 0);

#ifdef _SILICON_LABS_32B_SERIES_1
// set up the CSD to be active whenever the PA or LNA are enabled
// its signal is PA enable ORed with the RX channel's signal (LNA enable)
#if HAL_FEM_RX_ACTIVE == 1
  PRS->CH[BSP_FEM_SLEEP_CHANNEL].CTRL = PA_ENABLE_PRS_SIGNAL | PRS_CH_CTRL_ORPREV;
#else
  PRS->CH[BSP_FEM_SLEEP_CHANNEL].CTRL = PA_ENABLE_PRS_SIGNAL;
#endif

// Configure CSD PRS output to selected channel and location
  if (BSP_FEM_SLEEP_CHANNEL < 4) {
    routeRegister = &PRS->ROUTELOC0;
  } else if (BSP_FEM_SLEEP_CHANNEL < 8) {
    routeRegister = &PRS->ROUTELOC1;
  } else if (BSP_FEM_SLEEP_CHANNEL < 12) {
    routeRegister = &PRS->ROUTELOC2;
  } else {
    EFM_ASSERT(0);
    return; // error
  }

  BUS_RegMaskedWrite(routeRegister,
                     0xFF << ((BSP_FEM_SLEEP_CHANNEL % 4) * 8),
                     BSP_FEM_SLEEP_LOC << ((BSP_FEM_SLEEP_CHANNEL % 4) * 8));

// Enable CSD PRS output on both output and input
  BUS_RegMaskedSet(&PRS->ROUTEPEN, (1 << BSP_FEM_SLEEP_CHANNEL));
#else //!_SILICON_LABS_32B_SERIES_1
// set up the CSD to be active whenever the PA or LNA are enabled
// its signal is PA enable ORed with the RX channel's signal (LNA enable)
  PRS_SourceAsyncSignalSet(BSP_FEM_SLEEP_CHANNEL,
                           0U,
                           PA_ENABLE_PRS_SIGNAL);
#if HAL_FEM_RX_ACTIVE == 1
  PRS_Combine(BSP_FEM_SLEEP_CHANNEL,
              BSP_FEM_RX_CHANNEL,
              prsLogic_A_OR_B);
#endif
// Configure CSD PRS output to selected channel
  PRS_PinOutput(BSP_FEM_SLEEP_CHANNEL, prsTypeAsync, BSP_FEM_SLEEP_PORT, BSP_FEM_SLEEP_PIN);

#endif //_SILICON_LABS_32B_SERIES_1
#endif // BSP_FEM_SLEEP_CHANNEL

// if fem has a bypass pin (FEM pin CPS)
#ifdef BSP_FEM_BYPASS_PORT
  // set up bypass pin
  #if HAL_FEM_BYPASS_ENABLE
  GPIO_PinModeSet(BSP_FEM_BYPASS_PORT, BSP_FEM_BYPASS_PIN, gpioModePushPull, 1);
  #else
  GPIO_PinModeSet(BSP_FEM_BYPASS_PORT, BSP_FEM_BYPASS_PIN, gpioModePushPull, 0);
  #endif
#endif

// if fem has a tx power pin (FEM pin CHL)
#ifdef BSP_FEM_TXPOWER_PORT
  // set up tx power pin
  #if HAL_FEM_TX_HIGH_POWER
  GPIO_PinModeSet(BSP_FEM_TXPOWER_PORT, BSP_FEM_TXPOWER_PIN, gpioModePushPull, 1);
  #else
  GPIO_PinModeSet(BSP_FEM_TXPOWER_PORT, BSP_FEM_TXPOWER_PIN, gpioModePushPull, 0);
  #endif
#endif
}

void wakeupFem(void)
{
// if fem has a bypass pin (FEM pin CPS)
#ifdef BSP_FEM_BYPASS_PORT
  #if HAL_FEM_BYPASS_ENABLE
  GPIO_PinOutSet(BSP_FEM_BYPASS_PORT, BSP_FEM_BYPASS_PIN);
  #else
  GPIO_PinOutClear(BSP_FEM_BYPASS_PORT, BSP_FEM_BYPASS_PIN);
  #endif
#endif

// if fem has a tx power pin (FEM pin CHL)
#ifdef BSP_FEM_TXPOWER_PORT
  #if HAL_FEM_TX_HIGH_POWER
  GPIO_PinOutSet(BSP_FEM_TXPOWER_PORT, BSP_FEM_TXPOWER_PIN);
  #else
  GPIO_PinOutClear(BSP_FEM_TXPOWER_PORT, BSP_FEM_TXPOWER_PIN);
  #endif
#endif
}

void shutdownFem(void)
{
  #ifdef BSP_FEM_BYPASS_PORT
  GPIO_PinOutClear(BSP_FEM_BYPASS_PORT, BSP_FEM_BYPASS_PIN);
  #endif
  #ifdef BSP_FEM_TXPOWER_PORT
  GPIO_PinOutClear(BSP_FEM_TXPOWER_PORT, BSP_FEM_TXPOWER_PIN);
  #endif
}
