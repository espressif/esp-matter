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

#ifndef SL_SIMPLE_BUTTON_H
#define SL_SIMPLE_BUTTON_H

#include "sl_button.h"
#include "em_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup button
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup simple_button Simple Button Driver
 * @details Simple Button Driver module provides APIs to initalize and read
 *          simple buttons. Subsequent sections provide more insight into button
 *          driver configuration and usage.
 * @{
 ******************************************************************************/

/*******************************************************************************
 ******************************   DEFINES   ************************************
 ******************************************************************************/

#define SL_SIMPLE_BUTTON_MODE_POLL               0U   ///< BUTTON input capture using polling
#define SL_SIMPLE_BUTTON_MODE_POLL_AND_DEBOUNCE  1U   ///< BUTTON input capture using polling and debouncing
#define SL_SIMPLE_BUTTON_MODE_INTERRUPT          2U   ///< BUTTON input capture using interrupt

#define SL_SIMPLE_BUTTON_DISABLED                2U   ///< BUTTON state is disabled
#define SL_SIMPLE_BUTTON_PRESSED                 1U   ///< BUTTON state is pressed
#define SL_SIMPLE_BUTTON_RELEASED                0U   ///< BUTTON state is released

#define SL_SIMPLE_BUTTON_GET_STATE(context) (((sl_simple_button_context_t *)(context))->state)  ///< BUTTON member function to get state
#define SL_SIMPLE_BUTTON_GET_PORT(context)  (((sl_simple_button_context_t *)(context))->port)   ///< BUTTON member function to get port
#define SL_SIMPLE_BUTTON_GET_PIN(context)   (((sl_simple_button_context_t *)(context))->pin)    ///< BUTTON member function to get pin
#define SL_SIMPLE_BUTTON_GET_MODE(context)  (((sl_simple_button_context_t *)(context))->mode)   ///< BUTTON member function to get mode

/*******************************************************************************
 *****************************   DATA TYPES   *********************************
 ******************************************************************************/

/// A Simple BUTTON instance
typedef struct {
  sl_button_state_t     state;          ///< Current button state
  uint16_t              history;        ///< History of button states
  GPIO_Port_TypeDef     port;           ///< Button port
  uint8_t               pin;            ///< Button pin
  sl_button_mode_t      mode;           ///< Mode of operation */
} sl_simple_button_context_t;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * Initialize the simple button driver.
 *
 * @param[in] handle           Pointer to button handle:
 *                                - sl_button_t
 *
 * @return    Status Code:
 *              - SL_STATUS_OK
 ******************************************************************************/
sl_status_t sl_simple_button_init(const sl_button_t *handle);

/***************************************************************************//**
 * Get the current state of the simple button.
 *
 * @param[in] handle           Pointer to button handle:
 *                                - sl_button_t
 *
 * @return    Button State:     Current state of the button
 *              - SL_SIMPLE_BUTTON_PRESSED
 *              - SL_SIMPLE_BUTTON_RELEASED
 ******************************************************************************/
sl_button_state_t sl_simple_button_get_state(const sl_button_t *handle);

/***************************************************************************//**
 * Poll the simple button. (button mode - poll / poll and debonuce)
 *
 * @param[in] handle           Pointer to button handle:
 *                                - sl_button_t
 ******************************************************************************/
void sl_simple_button_poll_step(const sl_button_t *handle);

/***************************************************************************//**
 * Enable the simple button.
 *
 * @param[in] handle           Pointer to button handle:
 *                                - sl_button_t
 ******************************************************************************/
void sl_simple_button_enable(const sl_button_t *handle);

/***************************************************************************//**
 * Disable the simple button.
 *
 * @param[in] handle           Pointer to button handle:
 *                                - sl_button_t
 ******************************************************************************/
void sl_simple_button_disable(const sl_button_t *handle);

/** @} (end addtogroup simple_button) */
/** @} (end addtogroup button) */

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup simple_button Simple Button Driver
/// @{
///
///   @details
///
///   @li @ref simple_button_intro
///   @li @ref simple_button_config
///   @li @ref simple_button_usage
///
///   @n @section simple_button_intro Introduction
///
///   The Simple Button driver is a module of the button driver that provides the functionality
///   to initialize and read simple buttons.
///
///   @n @section simple_button_config Simple Button Configuration
///
///   Simple buttons use the @ref sl_button_t struct and their @ref sl_simple_button_context_t
///   struct. These are automatically generated into the following files, as well as
///   instance specific headers with macro definitions in them. The samples below
///   are for a single instance called "inst0".
///
///   @code{.c}
///// sl_simple_button_instances.c
///
///#include "sl_simple_button.h"
///#include "sl_simple_button_inst0_config.h"
///
///sl_simple_button_context_t simple_inst0_context = {
///  .state = 0,
///  .history = 0,
///  .port = SL_SIMPLE_BUTTON_INST0_PORT,
///  .pin = SL_SIMPLE_BUTTON_INST0_PIN,
///  .mode = SL_SIMPLE_BUTTON_INST0_MODE,
///};
///
///const sl_button_t sl_button_inst0 = {
///  .context = &simple_inst0_context,
///  .init = sl_simple_button_init,
///  .get_state = sl_simple_button_get_state,
///  .poll = sl_simple_button_poll_step,
///};
///
///const sl_button_t *sl_simple_button_array[] = {&sl_button_inst0};
///const uint8_t simple_button_count = 1;
///
///void sl_simple_button_init_instances(void)
///{
///  sl_button_init(&sl_button_inst0);
///}
///
///void sl_simple_button_poll_instances(void)
///{
///  sl_button_poll_step(&sl_button_inst0);
///}
///   @endcode
///
///   @note The sl_simple_button_instances.c file is shown with only one instance, but if more
///         were in use they would all appear in this .c file.
///
///   @code{.c}
///// sl_simple_button_instances.h
///
///#ifndef SL_SIMPLE_BUTTON_INSTANCES_H
///#define SL_SIMPLE_BUTTON_INSTANCES_H
///
///#include "sl_simple_button.h"
///
///extern const sl_button_t sl_button_inst0;
///
///void sl_simple_button_init_instances(void);
///void sl_simple_button_poll_instances(void);
///
///#endif // SL_SIMPLE_BUTTON_INSTANCES_H
///   @endcode
///
///   @note The sl_simple_button_instances.h file is shown with only one instance, but if more
///         were in use they would all appear in this .h file.
///
///   @n @section simple_button_usage Simple Button Usage
///
///   The simple button driver has no differences in its usage from the common button driver.
///   See @ref buttondrv_usage.
///
/// @} end group simple_button ********************************************************/

#ifdef __cplusplus
}
#endif

#endif // SL_SIMPLE_BUTTON_H
