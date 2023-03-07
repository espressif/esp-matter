/******************************************************************************

 @file  timac_board.h

 @brief interface definition for board dependent service for TIMAC stack.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2013-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/
#ifndef TIMACBOARD_H
#define TIMACBOARD_H

#include <stdbool.h>
#include <stdint.h>
#include <icall.h>

/** board service function to write to GPIO */
#define TIMACBOARD_FUNC_GPIO_WRITE     0

/** GPIO index for debug LED 1 */
#define TIMACBOARD_GPIO_OUTPUT_LED1    0

/** GPIO index for debug LED 2 */
#define TIMACBOARD_GPIO_OUTPUT_LED2    1

/** GPIO index for debug LED 3 */
#define TIMACBOARD_GPIO_OUTPUT_LED3    2

/** GPIO index for debug LED 2 */
#define TIMACBOARD_GPIO_OUTPUT_LED4    3

/** @internal Total number of GPIOs used */
#define TIMACBOARD_GPIO_OUTPUT_COUNT   4

/** GPIO configuration data structure */
typedef struct _timacboard_gpiocfg_t
{
  /** GPIO port.
   * Note that not all driverlib requires GPIO port,
   * but for compatibility, this field is always kept.
   */
  uint_least32_t port;

  /** GPIO pin bit field */
  uint_least32_t pin;
} TIMACBoard_GPIOCFG;

/** Board configuration data structure */
typedef struct _timacboard_cfg_t
{
  /** GPIO configuration */
  TIMACBoard_GPIOCFG gpio_outputs[TIMACBOARD_GPIO_OUTPUT_COUNT];
} TIMACBoard_CFG;

typedef struct _timacboard_set_led_args_t
{
  /** common arguments */
  ICall_FuncArgsHdr hdr;
  /** GPIO port/pin definition index mapped to LED */
  uint_least16_t index;
  /** whether to turn on or off the LED (TRUE or FALSE) */
  int_least16_t set;
} TIMACBoard_SetLEDArgs;

/** Customer defined board configuration */
extern const TIMACBoard_CFG TIMACBoard_config;

/**
 * Initializes board service module.
 * Note that @ref TIMACBoard_config must be set before calling
 * this function and all GPIO port direction must have been
 * configured.
 * This function does not configure GPIO port since
 * the same port may be used for multiple purposes and
 * hence it is not possible to configure GPIO port from a single
 * service point of view.
 */
extern void TIMACBoard_init(void);

/**
 * A function to be used by BLE stack to turn on/off debug LED.
 *
 * @param index       GPIO port/pin definition index mapped to debug LED.
 * @param set         TRUE to turn on LED.
 *                    FALSE to turn off LED.
 */
static inline ICall_Errno
TIMACBoard_setLED(size_t index, bool set)
{
  TIMACBoard_SetLEDArgs args;
  args.hdr.service = ICALL_SERVICE_CLASS_TIMAC_BOARD;
  args.hdr.func = TIMACBOARD_FUNC_GPIO_WRITE;
  args.index = (uint_least16_t) index;
  args.set = (int_least16_t) set;
  return ICall_dispatcher(&args.hdr);
}

#endif /* TIMACBOARD_H */
