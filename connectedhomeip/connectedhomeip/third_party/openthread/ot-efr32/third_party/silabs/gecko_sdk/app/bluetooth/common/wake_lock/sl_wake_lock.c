/***************************************************************************//**
 * @file
 * @brief Wake and sleep functionality
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
// Define module name for Power Manager debuging feature.
#define CURRENT_MODULE_NAME    "APP_BLUETOOTH_EXAMPLE_WAKE_LOCK"

#include "em_gpio.h"
#include "gpiointerrupt.h"
#include "sl_wake_lock.h"
#include "sl_power_manager.h"
#include "uartdrv_config.h"
#include "app_assert.h"

#if SL_WAKE_LOCK_ENABLE_LOCAL == 1
// global mutexes for wake-up handling
static bool wake_lock_em_req_added = false;
// Interrupt number assigned to the input pin
static unsigned int wake_lock_input_intno = INTERRUPT_UNAVAILABLE;

// Callback function for wake-up functionality
static void wake_lock_cb(uint8_t irq_idx, void *ctx);

 #if UARTDRV_RESTRICT_ENERGY_MODE_TO_ALLOW_RECEPTION != 0
 #warning Please set "Enable reception when sleeping" to 'Disable' in 'UARTDRV Core' component to enable sleep!
 #endif
#else //SL_WAKE_LOCK_ENABLE_LOCAL
 #if UARTDRV_RESTRICT_ENERGY_MODE_TO_ALLOW_RECEPTION != 1
 #warning Please set "Enable reception when sleeping" to 'Enable' in 'UARTDRV Core' component to disable sleep!
 #endif
#endif //SL_WAKE_LOCK_ENABLE_LOCAL

/**************************************************************************//**
 * Wake and sleep (lock) initialization function
 *****************************************************************************/
void sl_wake_lock_init(void)
{
#if SL_WAKE_LOCK_ENABLE_REMOTE == 1
  // Set output pin to wake-up remote controller
  GPIO_PinModeSet(SL_WAKE_LOCK_OUTPUT_PORT, SL_WAKE_LOCK_OUTPUT_PIN, gpioModePushPull, SL_WAKE_LOCK_OUTPUT_POLARITY_ACTIVE_HIGH);
  // Set pin to disable wake-up
  if (SL_WAKE_LOCK_OUTPUT_POLARITY_ACTIVE_HIGH) {
    GPIO_PinOutSet(SL_WAKE_LOCK_OUTPUT_PORT, SL_WAKE_LOCK_OUTPUT_PIN);
  } else {
    GPIO_PinOutClear(SL_WAKE_LOCK_OUTPUT_PORT, SL_WAKE_LOCK_OUTPUT_PIN);
  }
#endif // SL_WAKE_LOCK_ENABLE_REMOTE

#if SL_WAKE_LOCK_ENABLE_LOCAL == 1
  wake_lock_em_req_added = false;
  // Set input pin to wake up from sleep and configure interrupt and call-back function
  GPIOINT_Init();
  GPIO_PinModeSet(SL_WAKE_LOCK_INPUT_PORT,
                  SL_WAKE_LOCK_INPUT_PIN,
                  gpioModeInput,
                  SL_WAKE_LOCK_INPUT_POLARITY_ACTIVE_HIGH);
  wake_lock_input_intno =
    GPIOINT_CallbackRegisterExt(SL_WAKE_LOCK_INPUT_PIN,
                                (GPIOINT_IrqCallbackPtrExt_t)wake_lock_cb,
                                NULL);
  if (wake_lock_input_intno != INTERRUPT_UNAVAILABLE) {
    GPIO_ExtIntConfig(SL_WAKE_LOCK_INPUT_PORT,
                      SL_WAKE_LOCK_INPUT_PIN,
                      wake_lock_input_intno,
                      true,
                      true,
                      true);
  } else {
    app_assert(false, "Failed to register callback function for wake-lock.\n");
  }
#endif //SL_WAKE_LOCK_ENABLE_LOCAL
}

#if SL_WAKE_LOCK_ENABLE_REMOTE == 1
/**************************************************************************//**
 * Signal wake-up to remote controller.
 *
 * Set output pin to signal remote controller to wake-up.
 *
 * @note Wait 1ms before sending out command to ensure that remote end has fully
 * woken up.
 *****************************************************************************/
void sl_wake_lock_set_remote_req(void)
{
  // set pin to wake up remote controller
  if (SL_WAKE_LOCK_OUTPUT_POLARITY_ACTIVE_HIGH) {
    GPIO_PinOutSet(SL_WAKE_LOCK_OUTPUT_PORT, SL_WAKE_LOCK_OUTPUT_PIN);
  } else {
    GPIO_PinOutClear(SL_WAKE_LOCK_OUTPUT_PORT, SL_WAKE_LOCK_OUTPUT_PIN);
  }
  // wait until remote side wakes up (typically it takes 600us)
  sl_sleeptimer_delay_millisecond(1);
}

/**************************************************************************//**
 * Signal go to sleep (lock) to remote controller.
 *
 * Signal to remote controller that the transmission ended (or close to end)
 * and it can go to sleep after.
 *****************************************************************************/
void sl_wake_lock_clear_remote_req(void)
{
  // clear pin to sleep remote controller
  if (SL_WAKE_LOCK_OUTPUT_POLARITY_ACTIVE_HIGH) {
    GPIO_PinOutClear(SL_WAKE_LOCK_OUTPUT_PORT, SL_WAKE_LOCK_OUTPUT_PIN);
  } else {
    GPIO_PinOutSet(SL_WAKE_LOCK_OUTPUT_PORT, SL_WAKE_LOCK_OUTPUT_PIN);
  }
}
#else // SL_WAKE_LOCK_ENABLE_REMOTE
// unused dummy functions without wake-up functionality enabled
void sl_wake_lock_set_remote_req(void)
{
}
void sl_wake_lock_clear_remote_req(void)
{
}
#endif // SL_WAKE_LOCK_ENABLE_REMOTE

/**************************************************************************//**
 * Wake-up signal arrived from remote controller.
 *
 * @note Dummy function.
 *****************************************************************************/
SL_WEAK void sl_wake_lock_set_req_rx_cb(void)
{
}

/**************************************************************************//**
 * Go to sleep (lock) signal arrived from remote controller.
 *
 * @note Dummy function.
 *****************************************************************************/
SL_WEAK void sl_wake_lock_clear_req_rx_cb(void)
{
}

#if SL_WAKE_LOCK_ENABLE_LOCAL == 1
/**************************************************************************//**
 * Enable sleep mode locally.
 *
 * Enable sleep mode for this controller. Only enables it if not yet enabled.
 *
 * @note It is called automatically by wake-up event handling function.
 *****************************************************************************/
void sl_wake_lock_set_local(void)
{
  // Only add requirement if not added before.
  if (wake_lock_em_req_added) {
    sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
    wake_lock_em_req_added = false;
  }
}

/**************************************************************************//**
 * Disable sleep mode locally.
 *
 * Disable sleep mode for this controller. Only disables it if not yet disabled.
 *
 * @note Have to be called from either wake-up or go sleep callback functions
 * before more time consuming functions.
 *****************************************************************************/
void sl_wake_lock_clear_local(void)
{
  // Only remove requirement if not removed before
  if (!wake_lock_em_req_added) {
    sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
    wake_lock_em_req_added = true;
  }
}

/**************************************************************************//**
 * Wake-up event handling function.
 *
 * Checks the arrived event and calls the appropriate wake-up or go to sleep
 * function callbacks. Wakes up controller if wake-up signal arrived.
 *
 * @note: It is the responsibility of the application to call
 * sl_wake_lock_set_local() function.
 *****************************************************************************/
static void wake_lock_cb(uint8_t irq_idx, void *ctx)
{
  (void)ctx; // Context is not used here
  if (irq_idx == wake_lock_input_intno) {
    if ( GPIO_PinInGet(SL_WAKE_LOCK_INPUT_PORT, SL_WAKE_LOCK_INPUT_PIN) != SL_WAKE_LOCK_INPUT_POLARITY_ACTIVE_HIGH) {
      // Go to sleep signal arrived from remote controller.
      sl_wake_lock_clear_req_rx_cb();
    } else {
      // Wake-up signal arrived from remote controller.
      sl_wake_lock_clear_local();
      sl_wake_lock_set_req_rx_cb();
    }
  }
}
#else //SL_WAKE_LOCK_ENABLE_LOCAL
// unused dummy without deep sleep function enabled
void sl_wake_lock_set_local(void)
{
}
void sl_wake_lock_clear_local(void)
{
}
#endif //SL_WAKE_LOCK_ENABLE_LOCAL
