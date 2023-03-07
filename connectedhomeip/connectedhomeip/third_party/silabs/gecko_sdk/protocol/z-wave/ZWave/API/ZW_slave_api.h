/**
 * @file
 * Z-Wave Slave node application interface
 * @copyright 2019 Silicon Laboratories Inc.
 */
#ifndef _ZW_SLAVE_API_H_
#define _ZW_SLAVE_API_H_

#ifdef ZW_SLAVE

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZW_basis_api.h>

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

/* ApplicationUpdate status */
#define UPDATE_STATE_NODE_INFO_RECEIVED         0x84
#define UPDATE_STATE_NOP_POWER_RECEIVED         0x83
#define UPDATE_STATE_REQUEST_NODE_INFO_RECEIVED 0x82

/* ZW_REQUEST_NETWORK_UPDATE callback values */
#define ZW_ROUTE_UPDATE_DONE      0x00
#define ZW_ROUTE_UPDATE_ABORT     0x01
#define ZW_ROUTE_UPDATE_WAIT      0x02
#define ZW_ROUTE_UPDATE_DISABLED  0x03

#endif /* ZW_SLAVE */

#endif /* _ZW_SLAVE_API_H_ */

