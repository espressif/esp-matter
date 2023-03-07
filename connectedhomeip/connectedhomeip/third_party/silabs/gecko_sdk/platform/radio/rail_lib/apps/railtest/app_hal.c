/***************************************************************************//**
 * @file
 * @brief This file handles the hardware interactions for RAILtest
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

#include <stdio.h>

#include "rail.h"

#include "em_cmu.h"
#include "em_gpio.h"

#include "gpiointerrupt.h"
#include "hal_common.h"

#include "app_common.h"
#include "sl_rail_test_config.h"

#include "sl_component_catalog.h"

#if defined(SL_RAIL_TEST_GRAPHICS_SUPPORT_ENABLE)
#include "sl_rail_test_graphics.h"
#endif

#if defined(SL_CATALOG_SIMPLE_LED_PRESENT)
#include "sl_simple_led_instances.h"
#endif

#if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT)
#include "sl_simple_button_instances.h"
#endif

#if defined(SL_CATALOG_IOSTREAM_USART_PRESENT)
#include "em_usart.h"
#include "sl_iostream_usart_vcom_config.h"
#endif

#if defined(SL_CATALOG_IOSTREAM_EUSART_PRESENT)
#include "em_eusart.h"
#include "sl_iostream_eusart_vcom_config.h"
#endif

volatile bool serEvent = false;
// Used for wakeup from sleep
volatile bool buttonWakeEvent = false;

#if defined(SL_CATALOG_IOSTREAM_USART_PRESENT) || defined(SL_CATALOG_IOSTREAM_EUSART_PRESENT)
static void gpioSerialWakeupCallback(uint8_t pin)
{
#ifdef SL_CATALOG_IOSTREAM_USART_PRESENT
  if (pin == SL_IOSTREAM_USART_VCOM_RX_PIN) {
#elif defined(SL_CATALOG_IOSTREAM_EUSART_PRESENT)
  if (pin == SL_IOSTREAM_EUSART_VCOM_RX_PIN) {
#endif
    serEvent = true;
  }
}
#endif

/******************************************************************************
 * Application HAL Initialization
 *****************************************************************************/
void appHalInit(void)
{
#if !defined(SL_RAIL_UTIL_IC_SIMULATION_BUILD)
#if defined (SL_RAIL_TEST_PER_PORT) && defined(SL_RAIL_TEST_PER_PIN)
  // For PER test
  GPIO_PinModeSet(SL_RAIL_TEST_PER_PORT, SL_RAIL_TEST_PER_PIN, gpioModePushPull, 1);
#endif // SL_RAIL_TEST_PER_PORT && SL_RAIL_TEST_PER_PIN

#if defined(SL_CATALOG_IOSTREAM_USART_PRESENT) || defined(SL_CATALOG_IOSTREAM_EUSART_PRESENT)
  // For 'sleep'
  GPIOINT_Init();
#ifdef SL_CATALOG_IOSTREAM_USART_PRESENT
  GPIOINT_CallbackRegister(SL_IOSTREAM_USART_VCOM_RX_PIN, gpioSerialWakeupCallback);
#endif
#ifdef SL_CATALOG_IOSTREAM_EUSART_PRESENT
  GPIOINT_CallbackRegister(SL_IOSTREAM_EUSART_VCOM_RX_PIN, gpioSerialWakeupCallback);
#endif
#endif // SL_CATALOG_IOSTREAM_USART_PRESENT || SL_CATALOG_IOSTREAM_EUSART_PRESENT
#endif // !SL_RAIL_UTIL_IC_SIMULATION_BUILD
}

void PeripheralDisable(void)
{
  LedsDisable();
  disableGraphics();
  // Disable the buttons on the board
  deinitButtons();
}

void PeripheralEnable(void)
{
  enableGraphics();
  updateGraphics();
  // Enable the buttons on the board
  initButtons();
}

/**
 * Delay for the specified number of microseconds by busy waiting.
 * @param microseconds Time to delay for in microseconds.
 */
void usDelay(uint32_t microseconds)
{
  uint32_t start = RAIL_GetTime();
  while ((RAIL_GetTime() - start) < microseconds) {
  }
}

void serialWaitForTxIdle(void)
{
  // Wait for the serial output to have completely cleared the UART
  // before sleeping.
#ifdef SL_CATALOG_IOSTREAM_USART_PRESENT
  while ((USART_StatusGet(SL_IOSTREAM_USART_VCOM_PERIPHERAL)
          & USART_STATUS_TXIDLE) == 0) {
  }
#endif
#ifdef SL_CATALOG_IOSTREAM_EUSART_PRESENT
  while ((EUSART_StatusGet(SL_IOSTREAM_EUSART_VCOM_PERIPHERAL)
          & EUSART_STATUS_TXIDLE) == 0) {
  }
#endif
}

/******************************************************************************
 * Configurable Peripherals
 *****************************************************************************/

// Graphics
#if defined(SL_RAIL_TEST_GRAPHICS_SUPPORT_ENABLE)

void disableGraphics(void)
{
  sl_rail_test_graphics_clear();
  sl_rail_test_graphics_sleep();

#if defined(HAL_VCOM_ENABLE) && defined(BSP_VCOM_ENABLE_PORT)
  // Some boards use the same pin for VCOM and the display so re-init that pin
  // here just to be safe.
  GPIO_PinModeSet(BSP_VCOM_ENABLE_PORT, BSP_VCOM_ENABLE_PIN, gpioModePushPull, 1);
#endif
}

void enableGraphics(void)
{
  sl_rail_test_graphics_wakeup();
}

// Update any LCD text that should be changed
void updateGraphics(void)
{
  if (logLevel & PERIPHERAL_ENABLE) {
    // Force a redraw
    sl_rail_test_graphics_update();
  }
}

#else

void updateGraphics(void)
{
}
void disableGraphics(void)
{
}
void enableGraphics(void)
{
}

#endif // defined(SL_RAIL_TEST_GRAPHICS_SUPPORT_ENABLE)

// LED's
#if (SL_SIMPLE_LED_COUNT > 0)

void LedSet(int led)
{
  // check passed argument
  if ((led < 0) || (led >= SL_SIMPLE_LED_COUNT)) {
    return;
  }

  if (logLevel & PERIPHERAL_ENABLE) {
    sl_led_turn_on(SL_SIMPLE_LED_INSTANCE(led));
  }
}

void LedToggle(int led)
{
  // check passed argument
  if ((led < 0) || (led >= SL_SIMPLE_LED_COUNT)) {
    return;
  }

  if (logLevel & PERIPHERAL_ENABLE) {
    sl_led_toggle(SL_SIMPLE_LED_INSTANCE(led));
  }
}

void LedsDisable(void)
{
  for (int i = 0; i < SL_SIMPLE_LED_COUNT; i++) {
    sl_led_turn_off(SL_SIMPLE_LED_INSTANCE(i));
  }
}

#else // !(SL_SIMPLE_LED_COUNT > 0)

void LedSet(int led)
{
  (void)led;
}
void LedToggle(int led)
{
  (void)led;
}
void LedsDisable(void)
{
}

#endif // (SL_SIMPLE_LED_COUNT > 0)

// Buttons
#if (SL_SIMPLE_BUTTON_COUNT > 0)

// Holds Enable/Disable status of the buttons on the board
static bool initButtonStatus = true;

void deinitButtons(void)
{
  initButtonStatus = false;
}

void initButtons(void)
{
  initButtonStatus = true;
}

#if defined(SL_CATALOG_BTN0_PRESENT)
static void gpio0LongPress(void)
{
  radioTransmit(0, NULL);
}

static void gpio0ShortPress(void)
{
  radioTransmit(1, NULL);
}
#endif // SL_CATALOG_BTN0_PRESENT

#if defined(SL_CATALOG_BTN1_PRESENT)
static void gpio1LongPress(void)
{
}

static void gpio1ShortPress(void)
{
  if (!inAppMode(NONE, NULL) || inRadioState(RAIL_RF_STATE_TX, NULL)) {
    return;
  }

  if (inRadioState(RAIL_RF_STATE_RX, NULL)) {
    RAIL_Idle(railHandle, RAIL_IDLE_ABORT, false);
  }

  // Check if next channel exists
  if (RAIL_IsValidChannel(railHandle, channel + 1)
      == RAIL_STATUS_NO_ERROR) {
    channel++;
  } else {
    // Find initial channel
    channel = 0;
    while (RAIL_IsValidChannel(railHandle, channel)
           != RAIL_STATUS_NO_ERROR) {
      channel++;
    }
  }

  changeChannel(channel);
}
#endif // SL_CATALOG_BTN1_PRESENT

// This is the reimplementation of a WEAK function in button.c.
void sl_button_on_change(const sl_button_t *handle)
{
  if (initButtonStatus) {
    #define GET_TIME_IN_MS() (RAIL_GetTime() / 1000)

    static uint32_t gpioTimeCapture[SL_SIMPLE_BUTTON_COUNT];
    // Hold true if a Negative Edge is encountered for the button press
    static bool gpioNegEdge[SL_SIMPLE_BUTTON_COUNT];

    void(*gpioLongPress_arr[])(void) = {
    #if defined(SL_CATALOG_BTN0_PRESENT)
      gpio0LongPress,
    #endif // SL_CATALOG_BTN0_PRESENT
    #if defined(SL_CATALOG_BTN1_PRESENT)
      gpio1LongPress
    #endif // SL_CATALOG_BTN1_PRESENT
    };
    void(*gpioShortPress_arr[])(void) = {
    #if defined(SL_CATALOG_BTN0_PRESENT)
      gpio0ShortPress,
    #endif // SL_CATALOG_BTN0_PRESENT
    #if defined(SL_CATALOG_BTN1_PRESENT)
      gpio1ShortPress
    #endif // SL_CATALOG_BTN1_PRESENT
    };

    for (uint8_t i = 0; i < SL_SIMPLE_BUTTON_COUNT; i++) {
      if (SL_SIMPLE_BUTTON_INSTANCE(i) == handle) {
        if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
          // Negative Edge
          gpioTimeCapture[i] = GET_TIME_IN_MS();
          gpioNegEdge[i] = true;
        } else {
          // Positive Edge with a preceeding Negative Edge
          if (gpioNegEdge[i]) {
            gpioNegEdge[i] = false;
            if ((elapsedTimeInt32u(gpioTimeCapture[i], GET_TIME_IN_MS())
                 > SL_RAIL_TEST_BUTTON_LONG_HOLD_DURATION_MS)) {
              (*gpioLongPress_arr[i])();
            } else {
              (*gpioShortPress_arr[i])();
            }
          }
        }
      }
    }
  } else {
    buttonWakeEvent = true;
  } // initButtonStatus
}

#else // !(SL_SIMPLE_BUTTON_COUNT > 0)

void deinitButtons(void)
{
}
void initButtons(void)
{
}

#endif // (SL_SIMPLE_BUTTON_COUNT > 0)
