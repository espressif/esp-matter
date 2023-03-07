/******************************************************************************

 @file  hal_board_cfg.h

 @brief Platform-specific definitions for the DK-LM3S9B96

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2011-2022, Texas Instruments Incorporated
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

#ifndef HAL_BOARD_CFG_H
#define HAL_BOARD_CFG_H

/*
 *     =============================================================
 *     |                  CC26xxEM EVAL BOARD                      |
 *     | --------------------------------------------------------- |
 *     |  mcu   : Texas Instruments cc26xx                         |
 *     |  clock : 24 MHz                                           |
 *     =============================================================
 */

/* ------------------------------------------------------------------------------------------------
 *                                           Includes
 * ------------------------------------------------------------------------------------------------
 */

#include "hal_mcu.h"
#include "hal_defs.h"
#include "hal_types.h"
#include "hal_assert.h"
#include <inc/hw_memmap.h>



/* ------------------------------------------------------------------------------------------------
 *                                          Clock Speed
 *
 *   Note that when the HAL_CPU_CLOCK_MHZ is changed, the HAL_CLOCK_INIT
 *   macro must also be changed.
 * ------------------------------------------------------------------------------------------------
 */

#define HAL_CPU_CLOCK_MHZ       24

/* ------------------------------------------------------------------------------------------------
 *                                        Interrupt Priorities
 * ------------------------------------------------------------------------------------------------
 */

/* sleep timer interrupt */
#define HAL_INT_PRIOR_ST        (4 << 5)

/* MAC interrupts */
#define HAL_INT_PRIOR_MAC       (4 << 5)

/* UART interrupt */
#define HAL_INT_PRIOR_UART      (5 << 5)

/* Keyboard interrupt */
#define HAL_INT_PRIOR_KEY       (5 << 5)
/* ------------------------------------------------------------------------------------------------
 *                                       KEY Configuration
 * ------------------------------------------------------------------------------------------------
 */

/* Interrupt option - Enable or disable */

// Use Interrupts rather than polling.
#ifndef BSP_KEY_MODE_ISR
#define BSP_KEY_MODE_ISR                 1
#endif
#define HAL_KEY_MODE                     BSP_KEY_MODE_ISR

/* ------------------------------------------------------------------------------------------------
 *                         OSAL NV implemented by internal flash pages.
 * ------------------------------------------------------------------------------------------------
 */
#define HAL_NV_PAGE_END           30

// Flash consists of 32 pages of 4 KB.
#if !defined(CC26X2) && !defined(CC13X2) && !defined(CC13X2P)
#define HAL_FLASH_PAGE_SIZE                             4096      // in bytes
#else // !(Agama CC26X2 || CC13X2 || CC13X2P)
#define HAL_FLASH_PAGE_SIZE                             8192      // in bytes
#endif ////Agama CC26X2 || CC13X2 || CC13X2P
#define HAL_FLASH_WORD_SIZE       4

// Z-Stack uses flash pages for NV
#define HAL_NV_PAGE_CNT           2
#define HAL_NV_PAGE_BEG           (HAL_NV_PAGE_END-HAL_NV_PAGE_CNT+1)
#define HAL_NV_START_ADDR         ((FLASHMEM_BASE) + (HAL_NV_PAGE_BEG * HAL_FLASH_PAGE_SIZE))


// First half of last page of flash is used for factory commissioning
// Commissioning items are aligned and padded to HAL_FLASH_WORD_SIZE
#define HAL_FLASH_COMM_PAGE  ( 30 )
#define HAL_FLASH_HALF_PAGE  ((HAL_FLASH_PAGE_SIZE) / 2)
#define HAL_FLASH_COMM_ADDR  ((FLASH_BASE) + (HAL_FLASH_COMM_PAGE * HAL_FLASH_PAGE_SIZE) + (HAL_FLASH_HALF_PAGE))

// 8-byte IEEE address (unique for each device)
#define HAL_FLASH_IEEE_SIZE  ( 8 )  // Re-defining Z_EXTADDR_LEN here
#define HAL_FLASH_IEEE_ADDR  (HAL_FLASH_COMM_ADDR - HAL_FLASH_IEEE_SIZE)

// 21-byte Device Private Key (for Key Establishment)
#define HAL_FLASH_DEV_PRIVATE_KEY_SIZE  ( 21 + 3 )  // 3-bytes of pad
#define HAL_FLASH_DEV_PRIVATE_KEY_ADDR  (HAL_FLASH_IEEE_ADDR - HAL_FLASH_DEV_PRIVATE_KEY_SIZE)

// 22-byte CA Public Key (for Key Establishment)
#define HAL_FLASH_CA_PUBLIC_KEY_SIZE  ( 22 + 2 )  // 2-bytes of pad
#define HAL_FLASH_CA_PUBLIC_KEY_ADDR  (HAL_FLASH_DEV_PRIVATE_KEY_ADDR - HAL_FLASH_CA_PUBLIC_KEY_SIZE)

// 48-byte Implicit Certificate (for Key Establishment)
#define HAL_FLASH_IMPLICIT_CERT_SIZE  ( 48 )
#define HAL_FLASH_IMPLICIT_CERT_ADDR  (HAL_FLASH_CA_PUBLIC_KEY_ADDR - HAL_FLASH_IMPLICIT_CERT_SIZE)


/* ----------- Board Initialization ---------- */
#define HAL_BOARD_INIT()                                                    \
{                                                                           \
  /* Turn on cache prefetch mode */                                         \
  /* Reset all the peripherals used in the mac */                           \
  /* Reset Timer0.                                                          \
   * Timer0A is used as 320us tick.                                         \
   * timer0B used to implement the timer API                                \
   * in mac_mcu_timer.h                                                     \
   */                                                                       \
  /* Reset Timer1(used as a sleep timer) */                                 \
  /* Reset the Push button ports */                                         \
  /* Reset the SPI peripheral */                                            \
  /* Enable the Led port (port f) */                                        \
  /* Configure sleep settings */                                            \
  /* Configure the LEDs as outputs */                                       \
  HAL_TURN_OFF_LED1();                                                      \
  HAL_TURN_OFF_LED2();                                                      \
  HAL_TURN_OFF_LED3();                                                      \
  HAL_TURN_OFF_LED4();                                                      \
}

/* ----------- Debounce ---------- */
#define HAL_DEBOUNCE(expr)    { int i; for (i=0; i<500; i++) { if (!(expr)) i = 0; } }
/* ----------- Push Buttons ---------- */
/*
#define HAL_PUSH_BTN_RIGHT() (PUSH_BTN_POLARITY (GPIOPinRead(BTN_PORT_R, HAL_BTN_R)))
#define HAL_PUSH_BTN_LEFT()  (PUSH_BTN_POLARITY (GPIOPinRead(BTN_PORT_L, HAL_BTN_L)))
#define HAL_PUSH_BTN_SELECT()(PUSH_BTN_POLARITY (GPIOPinRead(BTN_PORT_S, HAL_BTN_S)))
#define HAL_PUSH_BTN_UP()    (PUSH_BTN_POLARITY (GPIOPinRead(BTN_PORT_U, HAL_BTN_U)))
#define HAL_PUSH_BTN_DOWN()  (PUSH_BTN_POLARITY (GPIOPinRead(BTN_PORT_D, HAL_BTN_D)))

#define HAL_PUSH_BUTTON1     HAL_PUSH_BTN_SELECT
*/

#define HAL_PUSH_BUTTON1()   TRUE

/* ----------- LED's ---------- */

#define HAL_LED_BLINK_DELAY()   st( { volatile uint32 i; for (i=0; i<0x34000; i++) { }; } )

/* Note that only BLE stack accesses LEDs for debug purpose */
#define HAL_TURN_OFF_LED1()
#define HAL_TURN_OFF_LED2()
#define HAL_TURN_OFF_LED3()
#define HAL_TURN_OFF_LED4()

#define HAL_TURN_ON_LED1()
#define HAL_TURN_ON_LED2()
#define HAL_TURN_ON_LED3()
#define HAL_TURN_ON_LED4()

#define HAL_TOGGLE_LED1()
#define HAL_TOGGLE_LED2()
#define HAL_TOGGLE_LED3()
#define HAL_TOGGLE_LED4()

#define HAL_STATE_LED1()          (0)
#define HAL_STATE_LED2()          (0)
#define HAL_STATE_LED3()          (0)
#define HAL_STATE_LED4()          (0)

/* ------------------------------------------------------------------------------------------------
 *                                     Driver Configuration
 * ------------------------------------------------------------------------------------------------
 */

/* Set to TRUE enable H/W TIMER usage, FALSE disable it */
#ifndef HAL_TIMER
#define HAL_TIMER FALSE
#endif

/* Set to TRUE enable ADC usage, FALSE disable it */
#ifndef HAL_ADC
#define HAL_ADC FALSE
#endif

/* Set to TRUE enable LCD usage, FALSE disable it */
#ifndef HAL_LCD
#define HAL_LCD FALSE
#endif

/* Set to TRUE enable LED usage, FALSE disable it */
#ifndef HAL_LED
#define HAL_LED FALSE
#endif
#if (!defined BLINK_LEDS) && (HAL_LED == TRUE)
#define BLINK_LEDS
#endif

/* Set to TRUE enable KEY usage, FALSE disable it */
#ifndef HAL_KEY
#define HAL_KEY FALSE
#endif

/* Set to TRUE enable UART usage, FALSE disable it */
#ifndef HAL_UART
#if (defined ZAPP_P1) || (defined ZAPP_P2) || (defined ZTOOL_P1) || (defined ZTOOL_P2)
#define HAL_UART FALSE
#else
#define HAL_UART FALSE
#endif /* ZAPP, ZTOOL */
#endif /* HAL_UART */

#endif
/*******************************************************************************************************
*/
