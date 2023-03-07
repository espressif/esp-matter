/***************************************************************************//**
 * @file
 * @brief This file implements a simple API for configuring FEM control signals
 * via PRS.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_fem_util_config.h"
#include "sl_fem_util.h"

#define SL_FEM_UTIL_GLOBAL_ENABLE (SL_FEM_UTIL_RX_ENABLE        \
                                   || SL_FEM_UTIL_TX_ENABLE     \
                                   || SL_FEM_UTIL_BYPASS_ENABLE \
                                   || SL_FEM_UTIL_TX_HIGH_POWER_ENABLE)

#if SL_FEM_UTIL_GLOBAL_ENABLE == 1
  #include "em_device.h"
  #include "em_assert.h"
  #include "em_gpio.h"
  #include "em_bus.h"
  #include "em_cmu.h"
  #include "em_prs.h"

#if SL_FEM_UTIL_RX_ENABLE == 1
  #ifdef _SILICON_LABS_32B_SERIES_1
    #if (!defined(SL_FEM_UTIL_RX_CHANNEL) \
  || !defined(SL_FEM_UTIL_RX_PORT)        \
  || !defined(SL_FEM_UTIL_RX_PIN)         \
  || !defined(SL_FEM_UTIL_RX_LOC))
      #error "SL_FEM_UTIL_RX_CHANNEL/PORT/PIN/LOC must be defined."
    #endif
  #else //!_SILICON_LABS_32B_SERIES_1
    #if (!defined(SL_FEM_UTIL_RX_CHANNEL) \
  || !defined(SL_FEM_UTIL_RX_PORT)        \
  || !defined(SL_FEM_UTIL_RX_PIN))
      #error "SL_FEM_UTIL_RX_CHANNEL/PORT/PIN must be defined."
    #endif
  #endif //!_SILICON_LABS_32B_SERIES_1
#endif // SL_FEM_UTIL_RX_ENABLE == 1

#if SL_FEM_UTIL_TX_ENABLE == 1
// if no separate CTX pin is defined, CRX is a combined RX-TX pin
  #if !defined(SL_FEM_UTIL_TX_CHANNEL) && (SL_FEM_UTIL_RX_ENABLE == 1)
    #define SL_FEM_UTIL_TX_PORT SL_FEM_UTIL_RX_PORT
    #define SL_FEM_UTIL_TX_PIN SL_FEM_UTIL_RX_PIN
    #define SL_FEM_UTIL_TX_CHANNEL SL_FEM_UTIL_RX_CHANNEL
    #if _SILICON_LABS_32B_SERIES_1
      #define SL_FEM_UTIL_TX_LOC SL_FEM_UTIL_RX_LOC
    #endif
  #elif !defined(SL_FEM_UTIL_TX_CHANNEL)
    #error "SL_FEM_UTIL_TX_CHANNEL must be defined."
  #endif
#endif // SL_FEM_UTIL_TX_ENABLE == 1

  #if SL_FEM_UTIL_TX_ENABLE == 1 && SL_FEM_UTIL_RX_ENABLE == 1
    #if SL_FEM_UTIL_RX_CHANNEL == SL_FEM_UTIL_TX_CHANNEL
      #error The RX and TX PRS channels cannot be equal.
    #endif
  #endif

  #if (SL_FEM_UTIL_RX_ENABLE == 1) && defined(SL_FEM_UTIL_SLEEP_CHANNEL)
    #if (SL_FEM_UTIL_RX_CHANNEL + 1) != SL_FEM_UTIL_SLEEP_CHANNEL
      #error "SL_FEM_UTIL_SLEEP_CHANNEL must immediately follow SL_FEM_UTIL_RX_CHANNEL"
    #endif
  #endif // SL_FEM_UTIL_RX_ENABLE

  #ifndef PRS_CHAN_COUNT
    #define PRS_CHAN_COUNT PRS_ASYNC_CHAN_COUNT
  #endif

  #if defined(SL_FEM_UTIL_SLEEP_CHANNEL) && (SL_FEM_UTIL_SLEEP_CHANNEL >= PRS_CHAN_COUNT)
    #error "SL_FEM_UTIL_SLEEP_CHANNEL number higher than number of PRS channels"
  #endif

  #if SL_FEM_UTIL_TX_ENABLE
    #if SL_FEM_UTIL_TX_CHANNEL >= PRS_CHAN_COUNT
      #error "SL_FEM_UTIL_TX_CHANNEL number higher than number of PRS channels"
    #endif
  #endif
#endif // SL_FEM_UTIL_GLOBAL_ENABLE

void sl_fem_util_init(void)
{
#if SL_FEM_UTIL_GLOBAL_ENABLE == 1
  // Turn on the GPIO clock so that we can turn on GPIOs
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_PRS, true);

#ifdef _SILICON_LABS_32B_SERIES_1
  volatile uint32_t * routeRegister;
#endif //_SILICON_LABS_32B_SERIES_1
#if SL_FEM_UTIL_TX_ENABLE == 1
  //Enable the output of TX based on a specific port and pin
  //Configure the tx gpio to be an output (FEM pin CTX)
  GPIO_PinModeSet(SL_FEM_UTIL_TX_PORT, SL_FEM_UTIL_TX_PIN, gpioModePushPull, 0);
  //Setup the PRS to output TX on the channel and location chosen.
#ifdef _SILICON_LABS_32B_SERIES_1
  PRS->CH[SL_FEM_UTIL_TX_CHANNEL].CTRL = PRS_RAC_PAEN;

  // Configure TX/RX PRS output to selected channel and location
  if (SL_FEM_UTIL_TX_CHANNEL < 4) {
    routeRegister = &PRS->ROUTELOC0;
  } else if (SL_FEM_UTIL_TX_CHANNEL < 8) {
    routeRegister = &PRS->ROUTELOC1;
  } else if (SL_FEM_UTIL_TX_CHANNEL < 12) {
    routeRegister = &PRS->ROUTELOC2;
  } else {
    EFM_ASSERT(0);
    return; // error
  }

  BUS_RegMaskedClear(routeRegister, 0xFF << ((SL_FEM_UTIL_TX_CHANNEL % 4) * 8));
  BUS_RegMaskedSet(routeRegister, SL_FEM_UTIL_TX_LOC << ((SL_FEM_UTIL_TX_CHANNEL % 4) * 8));

  BUS_RegMaskedSet(&PRS->ROUTEPEN, (1 << SL_FEM_UTIL_TX_CHANNEL));
#else //!_SILICON_LABS_32B_SERIES_1
  PRS_SourceAsyncSignalSet(SL_FEM_UTIL_TX_CHANNEL,
                           0U,
#ifdef PRS_RACL_PAEN
                           PRS_RACL_PAEN);
#elif defined(PRS_RAC_PAEN)
                           PRS_RAC_PAEN);
#else
#error "No PRS setting defined for Source=RAC, Signal=PAEN"
#endif //PRS_RACL_PAEN
  PRS_PinOutput(SL_FEM_UTIL_TX_CHANNEL, prsTypeAsync, SL_FEM_UTIL_TX_PORT, SL_FEM_UTIL_TX_PIN);
#endif //_SILICON_LABS_32B_SERIES_1
#endif //SL_FEM_UTIL_TX_ENABLE == 1

#if SL_FEM_UTIL_RX_ENABLE == 1
  //Enable the output of RX based on a specific port and pin
  //Configure the rx gpio to be an output (FEM pin CRX)
  GPIO_PinModeSet(SL_FEM_UTIL_RX_PORT, SL_FEM_UTIL_RX_PIN, gpioModePushPull, 0);

#ifdef _SILICON_LABS_32B_SERIES_1
  //Setup the PRS to output RX on the channel and location chosen.
  PRS->CH[SL_FEM_UTIL_RX_CHANNEL].CTRL = PRS_RAC_LNAEN;

  // Configure TX/RX PRS output to selected channel and location
  if (SL_FEM_UTIL_RX_CHANNEL < 4) {
    routeRegister = &PRS->ROUTELOC0;
  } else if (SL_FEM_UTIL_RX_CHANNEL < 8) {
    routeRegister = &PRS->ROUTELOC1;
  } else if (SL_FEM_UTIL_RX_CHANNEL < 12) {
    routeRegister = &PRS->ROUTELOC2;
  } else {
    EFM_ASSERT(0);
    return; // error
  }

  BUS_RegMaskedClear(routeRegister, 0xFF << ((SL_FEM_UTIL_RX_CHANNEL % 4) * 8));
  BUS_RegMaskedSet(routeRegister, SL_FEM_UTIL_RX_LOC << ((SL_FEM_UTIL_RX_CHANNEL % 4) * 8));

  BUS_RegMaskedSet(&PRS->ROUTEPEN, (1 << SL_FEM_UTIL_RX_CHANNEL));
#else //!_SILICON_LABS_32B_SERIES_1
  PRS_SourceAsyncSignalSet(SL_FEM_UTIL_RX_CHANNEL,
                           0U,
#ifdef PRS_RACL_LNAEN
                           PRS_RACL_LNAEN);
#elif defined(PRS_RAC_LNAEN)
                           PRS_RAC_LNAEN);
#else
#error "No PRS setting defined for Source=RAC, Signal=LNAEN"
#endif //PRS_RACL_LNAEN
  PRS_PinOutput(SL_FEM_UTIL_RX_CHANNEL, prsTypeAsync, SL_FEM_UTIL_RX_PORT, SL_FEM_UTIL_RX_PIN);
#endif //_SILICON_LABS_32B_SERIES_1
#endif //SL_FEM_UTIL_RX_ENABLE == 1

#if defined(SL_FEM_UTIL_SLEEP_CHANNEL)
// initialize sleep as output (FEM pin CSD)
  GPIO_PinModeSet(SL_FEM_UTIL_SLEEP_PORT, SL_FEM_UTIL_SLEEP_PIN, gpioModePushPull, 0);

#ifdef _SILICON_LABS_32B_SERIES_1
// set up the CSD to be active whenever the PA or LNA are enabled
// its signal is PA enable ORed with the RX channel's signal (LNA enable)
#if SL_FEM_UTIL_RX_ENABLE == 1
  PRS->CH[SL_FEM_UTIL_SLEEP_CHANNEL].CTRL = PRS_RAC_PAEN | PRS_CH_CTRL_ORPREV;
#else
  PRS->CH[SL_FEM_UTIL_SLEEP_CHANNEL].CTRL = PRS_RAC_PAEN;
#endif

// Configure CSD PRS output to selected channel and location
  if (SL_FEM_UTIL_SLEEP_CHANNEL < 4) {
    routeRegister = &PRS->ROUTELOC0;
  } else if (SL_FEM_UTIL_SLEEP_CHANNEL < 8) {
    routeRegister = &PRS->ROUTELOC1;
  } else if (SL_FEM_UTIL_SLEEP_CHANNEL < 12) {
    routeRegister = &PRS->ROUTELOC2;
  } else {
    EFM_ASSERT(0);
    return; // error
  }

  BUS_RegMaskedClear(routeRegister, 0xFF << ((SL_FEM_UTIL_SLEEP_CHANNEL % 4) * 8));
  BUS_RegMaskedSet(routeRegister, SL_FEM_UTIL_SLEEP_LOC << ((SL_FEM_UTIL_SLEEP_CHANNEL % 4) * 8));

// Enable CSD PRS output on both output and input
  BUS_RegMaskedSet(&PRS->ROUTEPEN, (1 << SL_FEM_UTIL_SLEEP_CHANNEL));
#else //!_SILICON_LABS_32B_SERIES_1
// set up the CSD to be active whenever the PA or LNA are enabled
// its signal is PA enable ORed with the RX channel's signal (LNA enable)
  PRS_SourceAsyncSignalSet(SL_FEM_UTIL_SLEEP_CHANNEL,
                           0U,
#ifdef PRS_RACL_PAEN
                           PRS_RACL_PAEN);
#elif defined(PRS_RAC_PAEN)
                           PRS_RAC_PAEN);
#else
#error "No PRS setting defined for Source=RAC, Signal=PAEN"
#endif //PRS_RACL_PAEN
#if SL_FEM_UTIL_RX_ENABLE == 1
  PRS_Combine(SL_FEM_UTIL_SLEEP_CHANNEL,
              SL_FEM_UTIL_RX_CHANNEL,
              prsLogic_A_OR_B);
#endif
// Configure CSD PRS output to selected channel
  PRS_PinOutput(SL_FEM_UTIL_SLEEP_CHANNEL, prsTypeAsync, SL_FEM_UTIL_SLEEP_PORT, SL_FEM_UTIL_SLEEP_PIN);

#endif //_SILICON_LABS_32B_SERIES_1
#endif // SL_FEM_UTIL_SLEEP_CHANNEL

// if fem has a bypass pin (FEM pin CPS)
#ifdef SL_FEM_UTIL_BYPASS_PORT
  // set up bypass pin
  #if SL_FEM_UTIL_BYPASS_ENABLE
  GPIO_PinModeSet(SL_FEM_UTIL_BYPASS_PORT, SL_FEM_UTIL_BYPASS_PIN, gpioModePushPull, 1);
  #else
  GPIO_PinModeSet(SL_FEM_UTIL_BYPASS_PORT, SL_FEM_UTIL_BYPASS_PIN, gpioModePushPull, 0);
  #endif
#endif

// if fem has a tx power pin (FEM pin CHL)
#ifdef SL_FEM_UTIL_TX_HIGH_POWER_PORT
  // set up tx power pin
  #if SL_FEM_UTIL_TX_HIGH_POWER_ENABLE
  GPIO_PinModeSet(SL_FEM_UTIL_TX_HIGH_POWER_PORT, SL_FEM_UTIL_TX_HIGH_POWER_PIN, gpioModePushPull, 1);
  #else
  GPIO_PinModeSet(SL_FEM_UTIL_TX_HIGH_POWER_PORT, SL_FEM_UTIL_TX_HIGH_POWER_PIN, gpioModePushPull, 0);
  #endif
#endif
#endif // SL_FEM_UTIL_GLOBAL_ENABLE
}

void sl_fem_util_wakeup(void)
{
#if SL_FEM_UTIL_GLOBAL_ENABLE == 1
// if fem has a bypass pin (FEM pin CPS)
#ifdef SL_FEM_UTIL_BYPASS_PORT
  #if SL_FEM_UTIL_BYPASS_ENABLE
  GPIO_PinOutSet(SL_FEM_UTIL_BYPASS_PORT, SL_FEM_UTIL_BYPASS_PIN);
  #else
  GPIO_PinOutClear(SL_FEM_UTIL_BYPASS_PORT, SL_FEM_UTIL_BYPASS_PIN);
  #endif
#endif

// if fem has a tx power pin (FEM pin CHL)
#ifdef SL_FEM_UTIL_TX_HIGH_POWER_PORT
  #if SL_FEM_UTIL_TX_HIGH_POWER_ENABLE
  GPIO_PinOutSet(SL_FEM_UTIL_TX_HIGH_POWER_PORT, SL_FEM_UTIL_TX_HIGH_POWER_PIN);
  #else
  GPIO_PinOutClear(SL_FEM_UTIL_TX_HIGH_POWER_PORT, SL_FEM_UTIL_TX_HIGH_POWER_PIN);
  #endif
#endif
#endif // SL_FEM_UTIL_GLOBAL_ENABLE
}

void sl_fem_util_shutdown(void)
{
#if SL_FEM_UTIL_GLOBAL_ENABLE == 1
// if fem has a bypass pin (FEM pin CPS)
#ifdef SL_FEM_UTIL_BYPASS_PORT
  GPIO_PinOutClear(SL_FEM_UTIL_BYPASS_PORT, SL_FEM_UTIL_BYPASS_PIN);
#endif

// if fem has a tx power pin (FEM pin CHL)
#ifdef SL_FEM_UTIL_TX_HIGH_POWER_PORT
  GPIO_PinOutClear(SL_FEM_UTIL_TX_HIGH_POWER_PORT, SL_FEM_UTIL_TX_HIGH_POWER_PIN);
#endif
#endif // SL_FEM_UTIL_GLOBAL_ENABLE
}

#if SL_FEM_UTIL_RUNTIME_PHY_SELECT
#if SL_FEM_UTIL_OPTIMIZED_PHY_ENABLE
static volatile bool fem_util_phy_enabled = true;
#else //!SL_FEM_UTIL_OPTIMIZED_PHY_ENABLE
static volatile bool fem_util_phy_enabled = false;
#endif //SL_FEM_UTIL_OPTIMIZED_PHY_ENABLE
#elif SL_FEM_UTIL_OPTIMIZED_PHY_ENABLE
#define fem_util_phy_enabled (true)
#else //!SL_FEM_UTIL_OPTIMIZED_PHY_ENABLE
#define fem_util_phy_enabled (false)
#endif //SL_FEM_UTIL_OPTIMIZED_PHY_ENABLE

bool sl_fem_util_get_phy_select(void)
{
  return fem_util_phy_enabled;
}

void sl_fem_util_enable_phy(bool enable)
{
#if SL_FEM_UTIL_RUNTIME_PHY_SELECT
  fem_util_phy_enabled = enable;
#else //!SL_FEM_UTIL_RUNTIME_PHY_SELECT
  (void)enable;
#endif //SL_FEM_UTIL_RUNTIME_PHY_SELECT
}
