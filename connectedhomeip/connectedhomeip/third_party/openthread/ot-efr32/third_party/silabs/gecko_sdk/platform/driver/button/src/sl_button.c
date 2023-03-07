/***************************************************************************//**
 * @file
 * @brief Button Driver
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "sl_button.h"
#include <stddef.h>

sl_status_t sl_button_init(const sl_button_t *handle)
{
  if (handle->init != NULL) {
    return handle->init(handle);
  } else {
    return SL_STATUS_NULL_POINTER;
  }
}

sl_button_state_t sl_button_get_state(const sl_button_t *handle)
{
  if (handle->get_state != NULL) {
    return handle->get_state(handle);
  } else {
    return (sl_button_state_t)BUTTON_ERROR;
  }
}

void sl_button_poll_step(const sl_button_t *handle)
{
  if (handle->poll != NULL) {
    handle->poll(handle);
  }
}

void sl_button_enable(const sl_button_t *handle)
{
  if (handle->enable != NULL) {
    handle->enable(handle);
  }
}

void sl_button_disable(const sl_button_t *handle)
{
  if (handle->disable != NULL) {
    handle->disable(handle);
  }
}

SL_WEAK void sl_button_on_change(const sl_button_t *handle)
{
  (void)handle;
}
