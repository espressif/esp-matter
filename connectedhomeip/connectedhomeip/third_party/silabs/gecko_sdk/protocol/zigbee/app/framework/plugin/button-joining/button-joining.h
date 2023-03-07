/***************************************************************************/ /**
 * @file
 * @brief APIs for the Button Form/Join plugin.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

/**
 * @defgroup button-joining Button Joining
 * @ingroup component
 * @brief API and Callbacks for the Button Joining Component
 *
 * This component requires Network Creator Security + Network Creator or Network
 * Steering component to function correctly in Zigbee 3.0 networks. Otherwise,
 * the Network Find component is strongly recommended but not mandatory. This
 * code hooks up button 0 to have specific behavior based on the current network
 * state. The behavior is as follows: If the device is not joined to a network,
 * it either forms a Zigbee 3.0 network (via the Network Creator component) if
 * it is configured as a coordinator, or joins a Zigbee 3.0 or ZHA legacy
 * network (via the Network Steering component) if it is configured as a router.
 * If the device is joined to a network, pressing the button broadcasts a ZDO
 * permit join to allow new devices to join. Holding the button for 5 seconds
 * and releasing causes the device to leave the network. Button 1 is not used
 * and a callback is provided to another module wishing to use it.
 *
 */

/**
 * @addtogroup button-joining
 * @{
 */

/**
 * @name API
 * @{
 */

/** @brief Press a button.
 *
 * Presses a button indicated by the button argument.
 *
 * @param button The button that was pressed.
 *
 */
void emberAfPluginButtonJoiningPressButton(uint8_t button);

/** @} */ // end of name APIs

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup button_joining_cb Button Joining
 * @ingroup af_callback
 * @brief Callbacks for Button Joining Component
 *
 */

/**
 * @addtogroup button_joining_cb
 * @{
 */

/** @brief Button Event
 *
 * This allows another module to get notification when a button is pressed and
 * released but the button joining plugin did not handle it. This callback is
 * NOT called in the ISR context so there are no restrictions on what code can
 * execute.
 *
 * @param buttonNumber The button number that was pressed. Ver.: always
 * @param buttonPressDurationMs The length of time button was held down before
 * it was released. Ver.: always
 */
void emberAfPluginButtonJoiningButtonEventCallback(uint8_t buttonNumber,
                                                   uint32_t buttonPressDurationMs);

/** @} */ // end of button_joining_cb
/** @} */ // end of name Callbacks
/** @} */ // end of button-joining
