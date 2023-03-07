/***************************************************************************//**
 * @file
 * @brief This file contains EFR32 specific HAL code to handle chip startup.
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

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_chip.h"
#if defined(_SILICON_LABS_32B_SERIES_2)
#include "em_prs.h"
#endif

#include "rail.h"
#include "hal_common.h"

// Create defines for the different PRS signal sources as they vary per chip
#if _SILICON_LABS_32B_SERIES_1_CONFIG == 1
// Defines for EFR32xG1 chips
#define _PRS_CH_CTRL_SOURCESEL_FRC     0x00000025UL
#else
// Defines for EFR32xG12 and newer chips
#define _PRS_CH_CTRL_SOURCESEL_FRC     0x00000055UL
#endif

#ifdef _SILICON_LABS_32B_SERIES_1

// Provide some PRS signals missing from public headers
#if _SILICON_LABS_32B_SERIES_1_CONFIG == 1
#define PRS_PROTIMER_CC0                ((35 << 8) + 0)
#define PRS_PROTIMER_CC1                ((35 << 8) + 1)
#else
#define PRS_PROTIMER_CC0                ((84 << 8) + 0)
#define PRS_PROTIMER_CC1                ((84 << 8) + 1)
#endif
/**
 * Define the signals that are supported for debug in RAILtest. These are chip
 * specific because on some chips these are supported by the PRS while on others
 * the debugging must come from the library directly.
 */
static const debugSignal_t debugSignals[] =
{
  {
    .name = "RXACTIVE",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_RAC_RX & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_RAC_RX & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
  {
    .name = "TXACTIVE",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_RAC_TX & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_RAC_TX & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
  {
    .name = "LNAEN",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_RAC_LNAEN & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_RAC_LNAEN & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
  {
    .name = "PAEN",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_RAC_PAEN & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_RAC_PAEN & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
  {
    .name = "RACACTIVE",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_RAC_ACTIVE & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_RAC_ACTIVE & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
  {
    .name = "PTIDATA",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = 0x00,
        .source = _PRS_CH_CTRL_SOURCESEL_FRC
      }
    }
  },
  {
    .name = "FRAMEDETECT",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_MODEM_FRAMEDET & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_MODEM_FRAMEDET & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
  {
    .name = "PREAMBLEDETECT",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_MODEM_PREDET & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_MODEM_PREDET & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
  {
    .name = "TIMINGDETECT",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_MODEM_TIMDET & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_MODEM_TIMDET & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
  {
    .name = "FRAMESENT",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_MODEM_FRAMESENT & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_MODEM_FRAMESENT & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
  {
    .name = "SYNCSENT",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_MODEM_SYNCSENT & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_MODEM_SYNCSENT & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
  {
    .name = "CC0",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_PROTIMER_CC0 & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_PROTIMER_CC0 & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
  {
    .name = "CC1",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = (PRS_PROTIMER_CC1 & _PRS_CH_CTRL_SIGSEL_MASK) >> _PRS_CH_CTRL_SIGSEL_SHIFT,
        .source = (PRS_PROTIMER_CC1 & _PRS_CH_CTRL_SOURCESEL_MASK) >> _PRS_CH_CTRL_SOURCESEL_SHIFT,
      }
    }
  },
};
#elif defined(_SILICON_LABS_32B_SERIES_2)
/**
 * Define the signals that are supported for debug in RAILtest. These are chip
 * specific because on some chips these are supported by the PRS while on others
 * the debugging must come from the library directly.
 */
static const debugSignal_t debugSignals[] =
{
  {
    .name = "RXACTIVE",
    .isPrs = true,
    .loc = {
      .prs = {
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
        .signal = _PRS_ASYNC_CH_CTRL_SIGSEL_RACRX,
        .source = _PRS_ASYNC_CH_CTRL_SOURCESEL_RAC,
#else
        .signal = _PRS_ASYNC_CH_CTRL_SIGSEL_RACLRX,
        .source = _PRS_ASYNC_CH_CTRL_SOURCESEL_RACL,
#endif
      }
    }
  },
  {
    .name = "TXACTIVE",
    .isPrs = true,
    .loc = {
      .prs = {
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
        .signal = _PRS_ASYNC_CH_CTRL_SIGSEL_RACTX,
        .source = _PRS_ASYNC_CH_CTRL_SOURCESEL_RAC,
#else
        .signal = _PRS_ASYNC_CH_CTRL_SIGSEL_RACLTX,
        .source = _PRS_ASYNC_CH_CTRL_SOURCESEL_RACL,
#endif
      }
    }
  },
  {
    .name = "LNAEN",
    .isPrs = true,
    .loc = {
      .prs = {
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
        .signal = _PRS_ASYNC_CH_CTRL_SIGSEL_RACLNAEN,
        .source = _PRS_ASYNC_CH_CTRL_SOURCESEL_RAC,
#else
        .signal = _PRS_ASYNC_CH_CTRL_SIGSEL_RACLLNAEN,
        .source = _PRS_ASYNC_CH_CTRL_SOURCESEL_RACL,
#endif
      }
    }
  },
  {
    .name = "PAEN",
    .isPrs = true,
    .loc = {
      .prs = {
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
        .signal = _PRS_ASYNC_CH_CTRL_SIGSEL_RACPAEN,
        .source = _PRS_ASYNC_CH_CTRL_SOURCESEL_RAC,
#else
        .signal = _PRS_ASYNC_CH_CTRL_SIGSEL_RACLPAEN,
        .source = _PRS_ASYNC_CH_CTRL_SOURCESEL_RACL,
#endif
      }
    }
  },
  {
    .name = "RACACTIVE",
    .isPrs = true,
    .loc = {
      .prs = {
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
        .signal = _PRS_ASYNC_CH_CTRL_SIGSEL_RACACTIVE,
        .source = _PRS_ASYNC_CH_CTRL_SOURCESEL_RAC,
#else
        .signal = _PRS_ASYNC_CH_CTRL_SIGSEL_RACLACTIVE,
        .source = _PRS_ASYNC_CH_CTRL_SOURCESEL_RACL,
#endif
      }
    }
  },
  {
    .name = "PTIDATA",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = _PRS_ASYNC_CH_CTRL_SIGSEL_FRCDOUT,
        .source = _PRS_ASYNC_CH_CTRL_SOURCESEL_FRC
      }
    }
  },
  {
    .name = "FRAMEDETECT",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = _PRS_ASYNC_CH_CTRL_SIGSEL_MODEMLFRAMEDET,
        .source = _PRS_ASYNC_CH_CTRL_SOURCESEL_MODEML,
      }
    }
  },
  {
    .name = "PREAMBLEDETECT",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = _PRS_ASYNC_CH_CTRL_SIGSEL_MODEMPREDET,
        .source = _PRS_ASYNC_CH_CTRL_SOURCESEL_MODEM,
      }
    }
  },
  {
    .name = "TIMINGDETECT",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = _PRS_ASYNC_CH_CTRL_SIGSEL_MODEMHTIMDET,
        .source = _PRS_ASYNC_CH_CTRL_SOURCESEL_MODEMH,
      }
    }
  },
  {
    .name = "FRAMESENT",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = _PRS_ASYNC_CH_CTRL_SIGSEL_MODEMFRAMESENT,
        .source = _PRS_ASYNC_CH_CTRL_SOURCESEL_MODEM,
      }
    }
  },
  {
    .name = "SYNCSENT",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = _PRS_ASYNC_CH_CTRL_SIGSEL_MODEMHSYNCSENT,
        .source = _PRS_ASYNC_CH_CTRL_SOURCESEL_MODEMH,
      }
    }
  },
  {
    .name = "EOF",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = _PRS_ASYNC_CH_CTRL_SIGSEL_MODEMHEOF,
        .source = _PRS_ASYNC_CH_CTRL_SOURCESEL_MODEMH,
      }
    }
  },
#ifndef _PRS_ASYNC_CH_CTRL_SIGSEL_PROTIMERLCC0
#define _PRS_ASYNC_CH_CTRL_SIGSEL_PROTIMERLCC0 (0x00000001UL)
#endif
  {
    .name = "CC0",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = _PRS_ASYNC_CH_CTRL_SIGSEL_PROTIMERLCC0,
        .source = _PRS_ASYNC_CH_CTRL_SOURCESEL_PROTIMERL,
      }
    }
  },
#ifndef _PRS_ASYNC_CH_CTRL_SIGSEL_PROTIMERLCC1
#define _PRS_ASYNC_CH_CTRL_SIGSEL_PROTIMERLCC1 (0x00000002UL)
#endif
  {
    .name = "CC1",
    .isPrs = true,
    .loc = {
      .prs = {
        .signal = _PRS_ASYNC_CH_CTRL_SIGSEL_PROTIMERLCC1,
        .source = _PRS_ASYNC_CH_CTRL_SOURCESEL_PROTIMERL,
      }
    }
  },
};
#else
#warning "Implement debugSignals for this platform"
#endif

const debugSignal_t* halGetDebugSignals(uint32_t *size)
{
  if (size != NULL) {
    *size = sizeof(debugSignals) / sizeof(debugSignal_t);
  }
  return debugSignals;
}

#if defined(_SILICON_LABS_32B_SERIES_1)
/**
 * Define the pins that are supported for debugging on the EFR32. This includes
 * PF2, PF3, PF4, PF5, PC10, and PC11. Along with these pins there are specific
 * PRS channels that will be used to output debug information on these pins.
 * This is all for debug and very specific to the EFR32.
 */
static const debugPin_t debugPins[] = {
  {
    .name = "PC10", // EFR32xG1/EFR32xG12/EFR32xG13 - EXP_HEADER15, WSTK_P12
    .prsChannel = 9, // PRS 0/12 9/15 10/4 11/3
    .prsLocation = 15,
    .gpioPort = gpioPortC,
    .gpioPin = 10
  },
  {
    .name = "PC11", // EFR32xG1/EFR32xG12/EFR32xG13 - EXP_HEADER16, WSTK_P13
    .prsChannel = 10, // PRS 0/13 9/16 10/5 11/4
    .prsLocation = 5,
    .gpioPort = gpioPortC,
    .gpioPin = 11
  },
  {
    .name = "PF2", // EFR32xG1/EFR32xG13 - WSTK_P28 (SWO)
    .prsChannel = 0, // PRS 0/2 1/1 2/0 3/7
    .prsLocation = 2,
    .gpioPort = gpioPortF,
    .gpioPin = 2
  },
  {
    .name = "PF3", // EFR32xG1/EFR32xG13 - EXP_HEADER13, WSTK_10 (TDI) [EFR32xG12 PC9]
    .prsChannel = 1, // PRS 0/3 1/2 2/1 3/0
    .prsLocation = 2,
    .gpioPort = gpioPortF,
    .gpioPin = 3
  },
  {
    .name = "PF4", // EFR32xG1/EFR32xG13 - EXP_HEADER11, WSTK_P8 (LED0) [EFR32xG12 PD12]
    .prsChannel = 2, // PRS 0/4 1/3 2/2 3/1
    .prsLocation = 2,
    .gpioPort = gpioPortF,
    .gpioPin = 4
  },
  {
    .name = "PF5", // EFR32xG1/EFR32xG13 - WSTK_P32 (LED1)
    .prsChannel = 3, // PRS 0/5 1/4 2/3 3/2
    .prsLocation = 2,
    .gpioPort = gpioPortF,
    .gpioPin = 5
  },
  {
    .name = "PF6", // EFR32xG1/EFR32xG13 - WSTK_P32 (LED1)
    .prsChannel = 0, // PRS 0/5 1/4 2/3 3/2
    .prsLocation = 6,
    .gpioPort = gpioPortF,
    .gpioPin = 6
  },
  {
    .name = "PF7", // EFR32xG1/EFR32xG13 - WSTK_P32 (LED1)
    .prsChannel = 1, // PRS 0/5 1/4 2/3 3/2
    .prsLocation = 6,
    .gpioPort = gpioPortF,
    .gpioPin = 7
  },
  {
    .name = "PC9", // EFR32xG12 - EXP_HEADER13, WSTK_P10 [EFR32xG1/EFR32xG13 PF3]
    .prsChannel = 11, // PRS 0/11 9/14 10/3 11/2
    .prsLocation = 2,
    .gpioPort = gpioPortC,
    .gpioPin = 9
  },
  {
    .name = "PD9", // EFR32xG12 - EXP_HEADER5, WSTK_P2 [EFR32xG1/EFR32xG13 PA3 (VCOM_CTS)]
    .prsChannel = 3, // PRS 3/8 4/0 5/6 6/11
    .prsLocation = 8,
    .gpioPort = gpioPortD,
    .gpioPin = 9
  },
  {
    .name = "PD10", // EFR32xG12 - EXP_HEADER7, WSTK_P4 [EFR32xG1/EFR32xG13 PF6 (BUTTON0)]
    .prsChannel = 4, // PRS 3/9 4/1 5/0 6/12
    .prsLocation = 1,
    .gpioPort = gpioPortD,
    .gpioPin = 10
  },
  {
    .name = "PD11", // EFR32xG12 - EXP_HEADER9, WSTK_P6 [EFR32xG1/EFR32xG13 PF7 (BUTTON1)]
    .prsChannel = 5, // PRS 3/10 4/2 5/1 6/13
    .prsLocation = 1,
    .gpioPort = gpioPortD,
    .gpioPin = 11
  },
  {
    .name = "PD12", // EFR32xG12 - EXP_HEADER11, WSTK_P8 [EFR32xG1/EFR32xG13 PF4 (LED0)]
    .prsChannel = 6, // PRS 3/11 4/3 5/2 6/14
    .prsLocation = 14,
    .gpioPort = gpioPortD,
    .gpioPin = 12
  },
};
#elif defined(_SILICON_LABS_32B_SERIES_2)
// PRS channels for debug pins are allocated during runtime for series 2
// Channels 0-5 can route to any pin on PA or PB
// Channels 6-11 can route to any pin on PC or PD
#else
#warning Implement debugPins for this platform
#endif

#if defined(_SILICON_LABS_32B_SERIES_1)
const debugPin_t* halGetDebugPins(uint32_t *size)
{
  if (size != NULL) {
    *size = sizeof(debugPins) / sizeof(debugPin_t);
  }
  return debugPins;
}
#endif

void halDisablePrs(uint8_t channel)
{
#if defined(_SILICON_LABS_32B_SERIES_1)
  // Turn the specified PRS channel off
  BUS_RegBitWrite(&PRS->ROUTEPEN,
                  _PRS_ROUTEPEN_CH0PEN_SHIFT + channel,
                  0);
#elif defined(_SILICON_LABS_32B_SERIES_2)
  GPIO->PRSROUTE[0].ROUTEEN &= ~(0x1 << (channel + _GPIO_PRS_ROUTEEN_ASYNCH0PEN_SHIFT));
  // PRS_FreeChannel(unsigned int ch, PRS_ChType_t type, GPIO_Port_TypeDef port, uint8_t pin)
#else
  #error "Unsupported platform!"
#endif
}

void halEnablePrs(uint8_t channel,
                  uint8_t loc,
                  GPIO_Port_TypeDef port,
                  uint8_t pin,
                  uint8_t source,
                  uint8_t signal)
{
#if defined(_SILICON_LABS_32B_SERIES_1)
  (void)port;
  (void)pin;
  volatile uint32_t *routeLocPtr;

  // Make sure the PRS is on and clocked
  CMU_ClockEnable(cmuClock_PRS, true);

  // Make sure this PRS channel is off while reconfiguring
  halDisablePrs(channel);

  PRS->CH[channel].CTRL = signal << _PRS_CH_CTRL_SIGSEL_SHIFT
                          | source << _PRS_CH_CTRL_SOURCESEL_SHIFT
                          | PRS_CH_CTRL_EDSEL_OFF;

  // Configure the output location for this PRS channel
  routeLocPtr   = &PRS->ROUTELOC0 + (channel / 4);
  *routeLocPtr &= ~(0xFFUL << (_PRS_ROUTELOC0_CH1LOC_SHIFT
                               * (channel % 4)));
  *routeLocPtr |= loc << (_PRS_ROUTELOC0_CH1LOC_SHIFT
                          * (channel % 4));

  // Set the enable bit for this PRS channel
  BUS_RegBitWrite(&PRS->ROUTEPEN,
                  _PRS_ROUTEPEN_CH0PEN_SHIFT + channel,
                  1);

  // Configure this GPIO as an output low to finish enabling this signal
  GPIO_PinModeSet(port, pin, gpioModePushPull, 0);

#elif defined(_SILICON_LABS_32B_SERIES_2)
  (void)loc;
  // Make sure the PRS is on and clocked
  CMU_ClockEnable(cmuClock_PRS, true);

  PRS_SourceAsyncSignalSet(channel,
                           ( ( uint32_t ) source << _PRS_ASYNC_CH_CTRL_SOURCESEL_SHIFT),
                           ( ( uint32_t ) signal << _PRS_ASYNC_CH_CTRL_SIGSEL_SHIFT) );
  PRS_PinOutput(channel, prsTypeAsync, port, pin);

  // Configure this GPIO as an output low to finish enabling this signal
  GPIO_PinModeSet(port, pin, gpioModePushPull, 0);
#else
  #error "Unsupported platform!"
#endif
}
