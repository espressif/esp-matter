/***************************************************************************//**
 * @file
 * @brief This file specifies GPIO pin information for RAILtest applications.
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

#include <string.h>

#if !defined(__ICCARM__)
// IAR doesn't have strings.h and puts those declarations in string.h
#include <strings.h>
#endif

#include "em_core.h"

#include "response_print.h"

#include "rail.h"
#include "app_common.h"
#include "app_trx.h"

#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"

void setGpioOutPin(sl_cli_command_arg_t *args)
{
  //check number of arguments
  if (sl_cli_get_argument_count(args) < 3) {
    responsePrint(sl_cli_get_command_string(args, 0), "Error:Not enough inputs.");
    return;
  }

  GPIO_Port_TypeDef port;
  uint16_t portMask;

  char portChar = *(sl_cli_get_argument_string(args, 0));
  int16_t diff = portChar - 'a';
  int16_t diffCap = portChar - 'A';
  int16_t diffNum = portChar - '0';

  //find port name from input
  if (diff >= 0 && diff <= GPIO_PORT_MAX) {
    port = (GPIO_Port_TypeDef)diff;
  } else if (diffCap >= 0 && diffCap <= GPIO_PORT_MAX) {
    port = (GPIO_Port_TypeDef)diffCap;
  } else if (diffNum >= 0 && diffNum <= GPIO_PORT_MAX) {
    port = (GPIO_Port_TypeDef)diffNum;
  } else {
    responsePrint(sl_cli_get_command_string(args, 0), "Error:%s is not a valid port.", sl_cli_get_argument_string(args, 0));
    return;
  }

  portMask = _GPIO_PORT_MASK(port);
  uint8_t pin = sl_cli_get_argument_uint8(args, 1);

  if (((portMask >> pin) & 0x1) == 0U) {
    responsePrint(sl_cli_get_command_string(args, 0), "Error:The pin you are configuring is unavailable. \
                            Port %s has the following port mask: %x",
                  sl_cli_get_argument_string(args, 0), portMask);
    return;
  }

  //set the pin based on the input value
  uint8_t val = sl_cli_get_argument_uint8(args, 2);
  GPIO_PinModeSet(port,
                  pin,
                  gpioModePushPull,
                  val);

  responsePrint(sl_cli_get_command_string(args, 0), "Status:Success.");
}
