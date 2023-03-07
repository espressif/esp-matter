/**
 * @file
 * @brief Small module for turning on all LEDs and enable all buttons.
 *
 * @details Used to test GPIO and wiring connections and settings.
 *
 * @copyright 2021 Silicon Laboratories Inc.
 */

#ifndef _HMI_CHECKER_H_
#define _HMI_CHECKER_H_

#include <ev_man.h>


/**
 * @brief Initializes the module for operation.
 */
void HMIChecker_Init(void);

/**
 * @brief A function to capture all button events for proper operation.
 */
void HMIChecker_buttonEventHandler(BUTTON_EVENT btnEvent);

#endif /* _HMI_CHECKER_H_ */
