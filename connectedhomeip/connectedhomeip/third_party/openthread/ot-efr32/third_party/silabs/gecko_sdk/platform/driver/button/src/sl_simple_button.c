/***************************************************************************//**
 * @file
 * @brief Simple Button Driver
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "sl_simple_button.h"
#include "sl_simple_button_config.h"

#include "gpiointerrupt.h"
#include "em_cmu.h"

#if (SL_SIMPLE_BUTTON_DEBOUNCE_BITS < 1U)
#undef SL_SIMPLE_BUTTON_DEBOUNCE_BITS
#define SL_SIMPLE_BUTTON_DEBOUNCE_BITS  1U
#endif
#if (SL_SIMPLE_BUTTON_DEBOUNCE_BITS > 15U)
#undef SL_SIMPLE_BUTTON_DEBOUNCE_BITS
#define SL_SIMPLE_BUTTON_DEBOUNCE_BITS  15U
#endif

static const uint16_t check_press = (uint16_t)(0xffff << SL_SIMPLE_BUTTON_DEBOUNCE_BITS);
static const uint16_t check_release = (uint16_t)(~(0x1 << SL_SIMPLE_BUTTON_DEBOUNCE_BITS));
static const uint16_t debounce_window = (uint16_t)(0xffff << (SL_SIMPLE_BUTTON_DEBOUNCE_BITS + 1));

/***************************************************************************//**
 * An internal callback called in interrupt context whenever a button changes
 * its state. (mode - SL_SIMPLE_BUTTON_MODE_INTERRUPT)
 *
 * @note The button state is updated by this function. The application callback
 * should not update it again.
 *
 * @param[in] interrupt_no      Interrupt number (pin number)
 * @param[in] ctx               Pointer to button handle
 ******************************************************************************/
static void sli_simple_button_on_change(uint8_t interrupt_no, void *ctx)
{
  (void)interrupt_no;
  sl_button_t *button = (sl_button_t *)ctx;
  sl_simple_button_context_t *simple_button = button->context;

  if (simple_button->state != SL_SIMPLE_BUTTON_DISABLED) {
    simple_button->state = ((bool)GPIO_PinInGet(simple_button->port, simple_button->pin) == SL_SIMPLE_BUTTON_POLARITY);
    sl_button_on_change(button);
  }
}

sl_status_t sl_simple_button_init(const sl_button_t *handle)
{
  unsigned int interrupt;
  sl_button_t *button = (sl_button_t *)handle;
  sl_simple_button_context_t *simple_button = button->context;

  CMU_ClockEnable(cmuClock_GPIO, true);

  GPIO_PinModeSet(simple_button->port,
                  simple_button->pin,
                  SL_SIMPLE_BUTTON_GPIO_MODE,
                  SL_SIMPLE_BUTTON_GPIO_DOUT);

  simple_button->state = ((bool)GPIO_PinInGet(simple_button->port, simple_button->pin) == SL_SIMPLE_BUTTON_POLARITY);

  if (simple_button->mode == SL_SIMPLE_BUTTON_MODE_INTERRUPT) {
    GPIOINT_Init();
    interrupt = GPIOINT_CallbackRegisterExt(simple_button->pin,
                                            (GPIOINT_IrqCallbackPtrExt_t)sli_simple_button_on_change,
                                            button);
    EFM_ASSERT(interrupt != INTERRUPT_UNAVAILABLE);
    GPIO_ExtIntConfig(simple_button->port,
                      simple_button->pin,
                      interrupt,
                      true,
                      true,
                      true);
  }

  return SL_STATUS_OK;
}

sl_button_state_t sl_simple_button_get_state(const sl_button_t *handle)
{
  sl_button_t *button = (sl_button_t *)handle;
  sl_simple_button_context_t *simple_button = button->context;

  return simple_button->state;
}

void sl_simple_button_poll_step(const sl_button_t *handle)
{
  sl_button_t *button = (sl_button_t *)handle;
  sl_simple_button_context_t *simple_button = button->context;

  if (simple_button->state == SL_SIMPLE_BUTTON_DISABLED) {
    return;
  }

  bool button_press = (bool)GPIO_PinInGet(simple_button->port, simple_button->pin);

  if (simple_button->mode == SL_SIMPLE_BUTTON_MODE_POLL_AND_DEBOUNCE) {
    uint16_t history = simple_button->history;
    history = (history << 1) | (button_press ^ SL_SIMPLE_BUTTON_POLARITY) | (debounce_window);

    if (history == check_press) {
      simple_button->state = SL_SIMPLE_BUTTON_PRESSED;
    }
    if (history == check_release) {
      simple_button->state = SL_SIMPLE_BUTTON_RELEASED;
    }

    simple_button->history = history;
  } else if (simple_button->mode == SL_SIMPLE_BUTTON_MODE_POLL) {
    simple_button->state = (button_press == SL_SIMPLE_BUTTON_POLARITY);
  }
}

void sl_simple_button_enable(const sl_button_t *handle)
{
  sl_button_t *button = (sl_button_t *)handle;
  sl_simple_button_context_t *simple_button = button->context;

  // Return if the button is not disabled
  if (simple_button->state != SL_SIMPLE_BUTTON_DISABLED) {
    return;
  }

  // Clear history
  simple_button->history = 0;
  // Reinit button
  sl_simple_button_init(handle);
}

void sl_simple_button_disable(const sl_button_t *handle)
{
  sl_button_t *button = (sl_button_t *)handle;
  sl_simple_button_context_t *simple_button = button->context;

  // Return if the button is disabled
  if (simple_button->state == SL_SIMPLE_BUTTON_DISABLED) {
    return;
  }
  if (simple_button->mode == SL_SIMPLE_BUTTON_MODE_INTERRUPT) {
    GPIOINT_CallbackUnRegister(simple_button->pin);
    // Disable interrupts
    GPIO_ExtIntConfig(simple_button->port,
                      simple_button->pin,
                      simple_button->pin,
                      false,
                      false,
                      false);
  }
  // Disable the button
  simple_button->state = SL_SIMPLE_BUTTON_DISABLED;
}
