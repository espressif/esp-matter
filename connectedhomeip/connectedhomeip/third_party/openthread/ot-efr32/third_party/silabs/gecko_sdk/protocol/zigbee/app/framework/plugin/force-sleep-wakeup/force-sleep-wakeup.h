/***************************************************************************//**
 * @file
 * @brief Zigbee Force sleep component header
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SILABS_FORCE_SLEEP_WAKEUP_H
#define SILABS_FORCE_SLEEP_WAKEUP_H

/**
 * @defgroup force-sleep-wakeup Force sleep and Wakeup
 * @ingroup component
 * @brief API and Callbacks for the Zigbee Force sleep and Wakeup Component
 *
 * This component can put the zigbee device to sleep immediately through CLI
 * and wakeup with a GPIO interrupt (by pressing Button 1 of WSTK)
 *
 */

/**
 * @addtogroup force-sleep-wakeup
 * @{
 */

/**
 * @name API
 * @{
 */

/**
 * @brief Puts Zigbee device to sleep immediately
 *
 * This API will purge queues, cancel all polls & events and calls rail idle to satisfy all conditions needed to go to EM2.
 * Removes EM1 requirement if set which allows power manager to sleep.
 */
void sl_zigbee_app_framework_force_stop(void);

/**
 * @brief Makes Zigbee device to undo force sleep
 *
 * This API will check all conditions, bring back the radio in RX mode, then
 * calls the power manager and makes sure the correct EM requirements are set
 * which will cause a proper wakeup
 */
void sl_zigbee_app_framework_force_wakeup(void);

/** @} */ // end of name APIs

bool sli_zigbee_app_framework_get_force_sleep_flag (void);

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup force_sleep_cb Force Sleep callback
 * @ingroup af_callback
 * @brief Callbacks for Force Sleep and Wakeup Component
 *
 */

/**
 * @addtogroup force_sleep_cb
 * @{
 */

/** @brief Force sleep callback
 *
 * This callabck is fired when force-sleep is triggered from cli. This can be
 * implemented in the app.c if other things have to be done before force-sleep API is called (like turn off LEDs)
 * Ver.: always
 */

void sli_zigbee_app_framework_force_sleep_callback(void);

/** @} */ // end of force_sleep_cb
/** @} */ // end of name Callbacks

/** @} */ // end of group force-sleep-wakeup

#endif //SILABS_FORCE_SLEEP_WAKEUP_H
