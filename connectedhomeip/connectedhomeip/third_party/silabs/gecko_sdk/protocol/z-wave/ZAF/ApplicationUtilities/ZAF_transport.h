/**
 * @file
 * @brief Handles multicast frames in the Z-Wave Framework.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef ZAF_APPLICATIONUTILITIES_ZAF_TRANSPORT_H_
#define ZAF_APPLICATIONUTILITIES_ZAF_TRANSPORT_H_

#include <stdbool.h>

/**
 * Instructs Z-Wave protocol to Abort an Application initiated transmit if any inprogress.
 *
 */
bool
ZAF_transportSendDataAbort(void);

#endif /* ZAF_APPLICATIONUTILITIES_ZAF_TRANSPORT_H_ */
