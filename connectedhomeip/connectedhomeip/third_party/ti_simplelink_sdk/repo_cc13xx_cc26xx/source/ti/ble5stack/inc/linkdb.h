/******************************************************************************

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated

 All rights reserved not granted herein.
 Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free,
 non-exclusive license under copyrights and patents it now or hereafter
 owns or controls to make, have made, use, import, offer to sell and sell
 ("Utilize") this software subject to the terms herein. With respect to the
 foregoing patent license, such license is granted solely to the extent that
 any such patent is necessary to Utilize the software alone. The patent
 license shall not apply to any combinations which include this software,
 other than combinations with devices manufactured by or for TI ("TI
 Devices"). No hardware patent is licensed hereunder.

 Redistributions must preserve existing copyright notices and reproduce
 this license (including the above copyright notice and the disclaimer and
 (if applicable) source code license limitations below) in the documentation
 and/or other materials provided with the distribution.

 Redistribution and use in binary form, without modification, are permitted
 provided that the following conditions are met:

   * No reverse engineering, decompilation, or disassembly of this software
     is permitted with respect to any software provided in binary form.
   * Any redistribution and use are licensed by TI for use only with TI Devices.
   * Nothing shall obligate TI to provide you with source code for the software
     licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution
 of the source code are permitted provided that the following conditions are
 met:

   * Any redistribution and use of the source code, including any resulting
     derivative works, are licensed by TI for use only with TI Devices.
   * Any redistribution and use of any object code compiled from the source
     code and any resulting derivative works, are licensed by TI for use
     only with TI Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its
 suppliers may be used to endorse or promote products derived from this
 software without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/**
 *  @defgroup LinkDB LinkDB
 *  This module implements the Link Database Module
 *  @{
 *  @file  linkdb.h
 *  LinkDB layer interface
 */

#ifndef LINKDB_H
#define LINKDB_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#include "ll_common.h"

/*********************************************************************
 * MACROS
 */

/**
 * Check to see if a physical link is up (connected).
 *
 * @param connectionHandle controller link connection handle.
 *
 * @return TRUE if the link is up
 * @return FALSE, otherwise.
 */
#define linkDB_Up( connectionHandle )  linkDB_State( (connectionHandle), LINK_CONNECTED )

/**
 * Check to see if the physical link is encrypted.
 *
 * @param connectionHandle controller link connection handle.
 *
 * @return TRUE if the link is encrypted
 * @return FALSE, otherwise.
 */
#define linkDB_Encrypted( connectionHandle )  linkDB_State( (connectionHandle), LINK_ENCRYPTED )

/**
 * Check to see if the physical link is authenticated.
 *
 * @param connectionHandle controller link connection handle.
 *
 * @return TRUE if the link is authenticated
 * @return FALSE, otherwise.
 */
#define linkDB_Authenticated( connectionHandle )  linkDB_State( (connectionHandle), LINK_AUTHENTICATED )

/**
 * Check to see if the physical link is bonded.
 *
 * @param connectionHandle controller link connection handle.
 *
 * @return TRUE if the link is bonded
 * @return FALSE, otherwise.
 */
#define linkDB_Bonded( connectionHandle )  linkDB_State( (connectionHandle), LINK_BOUND )

/*********************************************************************
 * CONSTANTS
 */

/**
 * @defgroup GapInit_Constants GapInit Constants
 *
 * Other defines used in the GapInit module
 * @{
 */

/**
 * @defgroup LinkDB_Conn_Handle LinkDB Connection Handle
 * @{
 */
/// Terminates all links
#define LINKDB_CONNHANDLE_ALL                  LL_CONNHANDLE_ALL
/// Loopback connection handle, used to loopback a message
#define LINKDB_CONNHANDLE_LOOPBACK             LL_CONNHANDLE_LOOPBACK
/// Invalid connection handle, used for no connection handle
#define LINKDB_CONNHANDLE_INVALID              LL_CONNHANDLE_INVALID
/** @} End LinkDB_Conn_Handle */

/**
 * @defgroup LinkDB_States LinkDB Connection State Flags
 * @{
 */
#define LINK_NOT_CONNECTED              0x00    //!< Link isn't connected
#define LINK_CONNECTED                  0x01    //!< Link is connected
#define LINK_AUTHENTICATED              0x02    //!< Link is authenticated
#define LINK_BOUND                      0x04    //!< Link is bonded
#define LINK_ENCRYPTED                  0x10    //!< Link is encrypted
#define LINK_SECURE_CONNECTIONS         0x20    //!< Link uses Secure Connections
#define LINK_IN_UPDATE                  0x40    //!< Link is in update procedure
#define LINK_PAIR_TIMEOUT               0x80    //!< Pairing attempt has been timed out
/** @} End LinkDB_States  */

/**
 * @defgroup LinkdDB_AuthErrors Link Authentication Errors
 * @{
 */
#define LINKDB_ERR_INSUFFICIENT_AUTHEN      0x05  //!< Link isn't even encrypted
#define LINBDB_ERR_INSUFFICIENT_KEYSIZE     0x0c  //!< Link is encrypted but the key size is too small
#define LINKDB_ERR_INSUFFICIENT_ENCRYPTION  0x0f  //!< Link is encrypted but it's not authenticated
/** @} End LinkdDB_AuthErrors  */


/** @} End GapInit_Constants */

/*********************************************************************
 * STRUCTURES
 */

/**
 * @defgroup LinkDB_Structures LinkDB Structures
 *
 * Data structures used in the LinkDB module
 * @{
 */

/// Information pertaining to the linklinkDB info
typedef struct
{
  uint8 stateFlags;             //!< @ref LinkDB_States
  uint8 addrType;               //!< Address type of connected device
  uint8 addr[B_ADDR_LEN];       //!< Other Device's address
  uint8 addrPriv[B_ADDR_LEN];   //!< Other Device's Private address
  uint8 connRole;               //!< Connection formed as Master or Slave
  uint16 connInterval;          //!< The connection's interval (n * 1.23 ms)
  uint16 MTU;                   //!< The connection's MTU size
  uint16 connTimeout;           //!< current connection timeout
  uint16 connLatency;           //!< current connection latency
} linkDBInfo_t;

/** @} End LinkDB_Structures  */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * Return the number of active connections
 *
 * @return the number of active connections
 */
extern uint8 linkDB_NumActive( void );

/**
 * Return the maximum number of connections supported.
 *
 * @return the number of connections supported
 */
extern uint8 linkDB_NumConns( void );

/**
 * Get information about a link
 *
 * Copies all of the link information into pInfo.  Uses the connection handle to
 * search the link database.
 *
 * @param connectionHandle controller link connection handle.
 * @param pInfo output parameter to copy the link information
 *
 * @return @ref SUCCESS
 * @return @ref FAILURE connection wasn't found
 */
  extern uint8 linkDB_GetInfo( uint16 connectionHandle, linkDBInfo_t *pInfo );

/**
 * Check to see if a physical link is in a specific state.
 *
 * @param connectionHandle controller link connection handle.
 * @param state @ref LinkDB_States state to look for.
 *
 * @return TRUE if the link is found and state is set in state flags.
 * @return FALSE otherwise.
 */
  extern uint8 linkDB_State( uint16 connectionHandle, uint8 state );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* LINKDB_H */

/** @} End LinkDB */
