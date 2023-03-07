/**
 * @file
 * Miscellaneous stuff.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _MISC_H_
#define _MISC_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/

#include <ZW_transport_api.h>

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

/**
 * This define holds a default set of transmit options.
 */
#define ZWAVE_PLUS_TX_OPTIONS (TRANSMIT_OPTION_ACK | TRANSMIT_OPTION_AUTO_ROUTE | TRANSMIT_OPTION_EXPLORE)

#endif /*#ifndef _MISC_H_*/
