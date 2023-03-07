/***************************************************************************//**
 * @file
 * @brief Button Driver
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

#ifndef SL_BUTTON_H
#define SL_BUTTON_H

#include "sl_common.h"
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup button Button API
 * @brief Generic Button API
 * @{
 ******************************************************************************/

/*******************************************************************************
 ******************************   DEFINES   ************************************
 ******************************************************************************/

#define BUTTON_ERROR  0xFFFF              ///< Error when trying to return state

/*******************************************************************************
 *****************************   DATA TYPES   *********************************
 ******************************************************************************/

typedef uint8_t sl_button_mode_t;       ///< BUTTON mode
typedef uint8_t sl_button_state_t;      ///< BUTTON state
typedef struct sl_button sl_button_t;

/// A BUTTON instance
typedef struct sl_button {
  void                  *context;                       ///< The context for this BUTTON instance
  sl_status_t           (*init)(const sl_button_t *handle);   ///< Member function to initialize BUTTON instance
  void                  (*poll)(const sl_button_t *handle);         ///< Member function to poll BUTTON
  void                  (*enable)(const sl_button_t *handle);       ///< Member function to enable BUTTON
  void                  (*disable)(const sl_button_t *handle);      ///< Member function to disable BUTTON
  sl_button_state_t     (*get_state)(const sl_button_t *handle);    ///< Member function to retrieve BUTTON state
} sl_button;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * Button driver init. This function should be called before calling any other
 * button function. Sets up the GPIO. Sets the mode of operation. Sets up the
 * interrupts based on the mode of operation.
 *
 * @param[in] handle            Pointer to button instance
 *
 * @return    Status Code:
 *              - SL_STATUS_OK
 ******************************************************************************/
sl_status_t sl_button_init(const sl_button_t *handle);

/***************************************************************************//**
 * Get button state.
 *
 * @param[in] handle            Pointer to button instance
 *
 * @return    Button state      Current state of the button
 ******************************************************************************/
sl_button_state_t sl_button_get_state(const sl_button_t *handle);

/***************************************************************************//**
 * Enable the button.
 *
 * @param[in] handle            Pointer to button instance
 *
 ******************************************************************************/
void sl_button_enable(const sl_button_t *handle);

/***************************************************************************//**
 * Disable the button.
 *
 * @param[in] handle            Pointer to button instance
 *
 ******************************************************************************/
void sl_button_disable(const sl_button_t *handle);

/***************************************************************************//**
 * Poll the button.
 *
 * @param[in] handle            Pointer to button instance
 ******************************************************************************/
void sl_button_poll_step(const sl_button_t *handle);

/***************************************************************************//**
 * A callback called in interrupt context whenever a button changes its state.
 *
 * @remark Can be implemented by the application if required. This function
 * can contain the functionality to be executed in response to changes of state
 * in each of the buttons, or callbacks to appropriate functionality.
 *
 * @note The button state should not be updated in this function, it is updated
 * by specific button driver prior to arriving here
 *
   @param[out] handle             Pointer to button instance
 ******************************************************************************/
void sl_button_on_change(const sl_button_t *handle);

/** @} (end addtogroup button) */

// ******** THE REST OF THE FILE IS DOCUMENTATION ONLY !***********************
/// @addtogroup button Button API
/// @{
///
///   @details
///
///   @li @ref buttondrv_intro
///   @li @ref buttondrv_config
///   @li @ref buttondrv_usage
///
///   @n @section buttondrv_intro Introduction
///
///   The button driver is a platfom level software module that manages the initialization
///   and reading of various types of buttons. There is currently one type of button
///   supported by the button driver:
///
///   @li @ref simple_button
///
///   All button functions are called through the generic driver, which then references
///   functions in the simple button and other potential future button drivers.
///
///   @n @section buttondrv_config Configuration
///
///   All button instances are configured with an @ref sl_button_t struct and a type specific
///   context struct. These structs are automatically generated after a button is set up
///   using Simplicity Studio's wizard, along with a function definition for initializing all
///   LEDs of that type. Specific setup for the simple button is in the following section.
///
///   @li @ref simple_button_config
///
///   @n @section buttondrv_usage Usage
///
///   Once the button structs are defined, the common button functions can be called being
///   passed an instance of sl_button_t, which will be redirected to calling the type specific
///   version of that function. The common functions include the following:
///
///   @li @ref sl_button_init
///   @li @ref sl_button_get_state
///   @li @ref sl_button_poll_step
///   @li @ref sl_button_on_change
///
///   @ref sl_button_init must be called before attempting to read the state of the button.
///
///   The button driver can either be used with interrupt mode, polling or polling with debounce.
///   In the case of using interrupt mode, @ref sl_button_on_change can be implemented by the
///   application if required. This function can contain functionality to be executed in response
///   to button event or callbacks to appropriate functionality.
///   In the case of polling and polling with debounce mode, @ref sl_button_poll_step is used to
///   update the state, and needs to be called from a tick function or similar by the user.
///   These mode can be configured per button instance in the instance specific config file.
///
///   Both the interrupt and polling methods obtain the button state for the user by calling
///   @ref sl_button_get_state.
///
/// @} end group button ********************************************************/

#ifdef __cplusplus
}
#endif

#endif // SL_BUTTON_H
