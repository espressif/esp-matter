/***************************************************************************//**
 * @file
 * @brief SPI protocol Host functions
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "stack/include/ember-types.h"
#include "hal/hal.h"
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/ezsp-host/ezsp-host-common.h"
#include "app/ezsp-host/spi/spi-host.h"
#include "app/ezsp-host/ezsp-host-io.h"
#include "app/ezsp-host/ezsp-host-priv.h"
#include "app/ezsp-host/ezsp-host-queues.h"
#include "app/ezsp-host/ezsp-host-ui.h"
#include "app/util/ezsp/serial-interface.h"

//------------------------------------------------------------------------------
// Preprocessor definitions

//------------------------------------------------------------------------------
// Global Variables

EzspStatus ncpError = EZSP_NO_ERROR;                        // ncp error or reset code
EzspStatus hostError = EZSP_NO_ERROR;                        // host error code

bool ncpSleepEnabled;                    // ncp is enabled to sleep

// Config 0 (default) : Mostly dummy values to be compatible with ASH
#define SPI_HOST_CONFIG_DEFAULT                                                   \
  {                                                                               \
    "/dev/ttyS0",       /* serial port name                                  */   \
    115200,             /* baud rate (bits/second)                           */   \
    1,                  /* stop bits                                         */   \
    true,               /* true enables RTS/CTS flow control, false XON/XOFF */   \
    256,                /* max bytes to buffer before writing to serial port */   \
    256,                /* max bytes to read ahead from serial port          */   \
    0,                  /* trace output control bit flags                    */   \
    3,                  /* max frames sent without being ACKed (1-7)         */   \
    false,               /* enables randomizing DATA frame payloads           */  \
    800,                /* adaptive rec'd ACK timeout initial value          */   \
    400,                /*  "     "     "     "     "  minimum value         */   \
    2400,               /*  "     "     "     "     "  maximum value         */   \
    2500,               /* time allowed to receive RSTACK after ncp is reset */   \
    RX_FREE_LWM,        /* if free buffers < limit, host receiver isn't ready */  \
    RX_FREE_HWM,        /* if free buffers > limit, host receiver is ready   */   \
    480,                /* time until a set nFlag must be resent (max 2032)  */   \
    SPI_RESET_METHOD_RST  /* method used to reset ncp                          */ \
  }

// Host configuration structure
EzspHostConfig spiHostConfig = SPI_HOST_CONFIG_DEFAULT;
