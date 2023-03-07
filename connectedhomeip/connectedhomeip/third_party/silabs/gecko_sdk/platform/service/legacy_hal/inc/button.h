/***************************************************************************//**
 * @file
 * @brief See @ref button for documentation.
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

/***************************************************************************//**
 * @addtogroup legacyhal
 * @{
 ******************************************************************************/

/** @addtogroup button
 * @brief Sample API functions for using push-buttons.
 *
 * See button.h for source code.
 *@{
 */

/** @name Button State Definitions
 * A set of numerical definitions for use with the button APIs indicating the
 * state of a button.
 *@{
 */

/** @brief Button state is pressed.
 */
#define BUTTON_PRESSED  1

/** @brief Button state is released.
 */
#define BUTTON_RELEASED 0

/**@} END Button State Definitions */

/** @brief Initializes the buttons. This function is automatically called
 * by ::halInit().
 */
void halInternalInitButton(void);

/** @brief Returns the current state (pressed or released) of a button.
 *
 * @note This function is correlated with ::halButtonIsr() and so returns the
 * shadow state rather than reading the actual state of the pin.
 *
 * @param button  The button being queried, either BUTTON0 or BUTTON1 as
 * defined in the appropriate BOARD_HEADER.
 *
 * @return ::BUTTON_PRESSED if the button is pressed or ::BUTTON_RELEASED
 * if the button is not pressed.
 */
uint8_t halButtonState(uint8_t button);

/** @brief Returns the current state (pressed or released) of the
 * pin associated with a button.
 *
 * This reads the actual state of the pin and can be used on startup to
 * determine the initial position of the buttons.
 *
 * @param button  The button being queried, either BUTTON0 or BUTTON1 as
 * defined in the appropriate BOARD_HEADER.
 *
 * @return  ::BUTTON_PRESSED if the button is pressed or ::BUTTON_RELEASED
 * if the button is not pressed.
 */
uint8_t halButtonPinState(uint8_t button);

/** @brief A callback called in interrupt context whenever a button
 * changes its state.
 *
 * @appusage Must be implemented by the application.  This function should
 * contain the functionality to be executed in response to changes of state
 * in each of the buttons, or callbacks to the appropriate functionality.
 *
 * @param button  The button which has changed state, either BUTTON0 or BUTTON1
 * as defined in the appropriate BOARD_HEADER.
 *
 * @param state   The new state of the button referenced by the button parameter,
 * either ::BUTTON_PRESSED if the button has been pressed or ::BUTTON_RELEASED if
 * the button has been released.
 */
void halButtonIsr(uint8_t button, uint8_t state);

/** @} END addtogroup
 */
/** @} (end addtogroup legacyhal) */
