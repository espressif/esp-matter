/******************************************************************************

 @file  linkdb_internal.h

 @brief This file contains internal linkDB interfaces and defines.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2022, Texas Instruments Incorporated

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

/*********************************************************************
 *
 * WARNING!!!
 *
 * THE API'S FOUND IN THIS FILE ARE FOR INTERNAL STACK USE ONLY!
 * FUNCTIONS SHOULD NOT BE CALLED DIRECTLY FROM APPLICATIONS, AND ANY
 * CALLS TO THESE FUNCTIONS FROM OUTSIDE OF THE STACK MAY RESULT IN
 * UNEXPECTED BEHAVIOR
 *
 */


#ifndef LINKDB_INTERNAL_H
#define LINKDB_INTERNAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// Secure Connection Only Mode Encryption Key Size
#define SCOM_ENCRYPT_KEY_SIZE           16      //!<  Encryption Key size (128 bits)

/**
 * @defgroup LinkdDB_ChangeTypes LinkDB Change Types
 *
 * Link Database Status callback changeTypes
 * @{
 */
#define LINKDB_STATUS_UPDATE_NEW        0       //!< New connection created
#define LINKDB_STATUS_UPDATE_REMOVED    1       //!< Connection was removed
#define LINKDB_STATUS_UPDATE_STATEFLAGS 2       //!< Connection state flag changed
/** @} End LinkdDB_ChangeTypes */

/*********************************************************************
 * TYPEDEFS
 */

/// Link Security information
typedef struct
{
  uint8 srk[KEYLEN];  //!< Signature Resolving Key
  uint32 signCounter; //!< Sign Counter
} linkSec_t;

/// Encryption Params
typedef struct
{
  uint8 ltk[KEYLEN];             //!< short term key or long term key.
                                 //!< TODO: rename this to "key" for ROM freeze
  uint16 div;                    //!< Diversifier
  uint8 rand[B_RANDOM_NUM_SIZE]; //!< random number
  uint8 keySize;                 //!< encryption Key Size
} encParams_t;

/// Additional internal information pertaining to the link
typedef struct
{
  uint8 taskID;                  //!< Application that controls the link
  uint16 connectionHandle;       //!< Controller connection handle
  uint8 stateFlags;              //!< LINK_CONNECTED, LINK_AUTHENTICATED...
  uint8 addrType;                //!< Address type of connected device
  uint8 addr[B_ADDR_LEN];        //!< Other Device's address
  uint8 addrPriv[B_ADDR_LEN];    //!< Other Device's Private address
  uint8 connRole;                //!< Connection formed as Master or Slave
  uint16 connInterval;           //!< The connection's interval (n * 1.23 ms)
  uint16 MTU;                    //!< The connection's MTU size
  linkSec_t sec;                 //!< Connection Security related items
  encParams_t *pEncParams;       //!< pointer to STK / LTK, ediv, rand
  uint16 connTimeout;            //!< Supervision timeout
  uint16 connLatency;            //!< Slave latency
} linkDBItem_t;

typedef struct
{
  uint8 addr[B_ADDR_LEN];        //!< This device's Private addresses during connection establishment
} linkDBLocalAddrDuringConnEst_t;

/// Function pointer used to perform specialized link database searches
typedef void (*pfnPerformFuncCB_t)
(
  linkDBItem_t *pLinkItem   //!< linkDB item
);

/// Function pointer used to register for a status callback
typedef void (*pfnLinkDBCB_t)
(
  uint16 connectionHandle,  //!< connection handle
  uint8 changeTypes         //!< @ref LinkdDB_ChangeTypes
);

/*********************************************************************
 * GLOBAL VARIABLES
 */

/// @cond NODOC
extern uint8 linkDBNumConns;
/// @endcond NODOC


/*********************************************************************
 * FUNCTIONS
 */

extern void linkDB_reportStatusChange( uint16 connectionHandle, uint8 changeType );

extern void linkDB_Init( void );

/**
 * Register with linkDB
 *
 * Register with this function to receive a callback when the status changes on
 * a connection. If the stateflag == 0, then the connection has been
 * disconnected.
 *
 * @param pFunc function pointer to callback function
 *
 * @return @ref SUCCESS if successful
 * @return @ref bleMemAllocError if no table space available
 *
 */
extern uint8 linkDB_Register( pfnLinkDBCB_t pFunc );

/**
 * Adds a record to the link database.
 *
 * @param taskID - Application task ID. OBSOLETE.
 * @param connectionHandle - new record connection handle
 * @param stateFlags - @ref LinkDB_States
 * @param addrType - @ref Addr_type
 * @param pAddr - new address
 * @param pAddrPriv - private address (only if addrType is 0x02 or 0x03)
 * @param connRole - @ref GAP_Profile_Roles
 * @param connInterval - connection's communications interval (n * 1.23 ms)
 * @param MTU - connection's MTU size
 *
 * @return      @ref SUCCESS if successful
 * @return      @ref bleIncorrectMode - hasn't been initialized.
 * @return      @ref bleNoResources - table full
 * @return      @ref bleAlreadyInRequestedMode - already exist connectionHandle
 *
 */
  extern uint8 linkDB_Add( uint8 taskID, uint16 connectionHandle,
                           uint8 stateFlags, uint8 addrType, uint8 *pAddr,
                           uint8 *pAddrPriv, uint8 connRole, uint16 connInterval,
                           uint16 MTU, uint16 connTimeout, uint16 connLatency );

  extern uint8 linkDB_Add_sPatch( uint8 taskID, uint16 connectionHandle,
                           uint8 stateFlags, uint8 addrType, uint8 *pAddr,
                           uint8 *pAddrPriv, uint8 connRole, uint16 connInterval,
                           uint16 MTU, uint16 connTimeout, uint16 connLatency );
/**
 * Remove a record from the link database.
 *
 * @param  pItem - pointer to the record item to remove
 *
 * @return @ref SUCCESS if successful
 * @return @ref INVALIDPARAMETER - pItem is invalid.
 */
  extern uint8 linkDB_Remove( linkDBItem_t* pItem );

/**
 * Update the stateFlags of a link record.
 *
 * @param  connectionHandle - maximum number of connections.
 * @param  newState - @ref LinkDB_States.  This value is OR'd in
 *                    to this field.
 * @param  add - TRUE to set state into flags, FALSE to remove.
 *
 * @return @ref SUCCESS if successful
 * @return @ref bleNoResources - connectionHandle not found.
 *
 */
  extern uint8 linkDB_Update( uint16 connectionHandle, uint8 newState,
                              uint8 add );

/**
 * This function is used to update the connection parameter of a link record.
 *
 * @param connectionHandle - new record connection handle
 * @param connInterval - connection's communications interval (n * 1.23 ms)
 * @param connTimeout - Connection supervision timeout.
 * @param connLatency - Number of skipped events (slave latency).
 *
 * @return SUCCESS if successful
 * @return bleNoResources - connectionHandle not found.
 */
 extern uint8 linkDB_updateConnParam ( uint16 connectionHandle,
                                       uint16 connInterval,
                                       uint16 connTimeout,
                                       uint16 connLatency );

/**
 * Update the MTU size of a link or record.
 *
 * @param  connectionHandle - controller link connection handle.
 * @param  newMtu - new MTU size.
 *
 * @return @ref SUCCESS or failure
 */
extern uint8 linkDB_UpdateMTU( uint16 connectionHandle, uint16 newMtu );

/**
 * This function is used to get the MTU size of a link.
 *
 * @param connectionHandle controller link connection handle.
 *
 * @return link MTU size
 */
extern uint16 linkDB_MTU( uint16 connectionHandle );

  /**
 * Find a link in the link database.
 *
 * Uses the connection handle to search the link database.
 *
 * @param       connectionHandle - controller link connection handle.
 *
 * @return      a pointer to the found link item
 * @return      NULL if not found
 */
extern linkDBItem_t *linkDB_Find( uint16 connectionHandle );

/**
 * Check to see if the physical link is encrypted and authenticated.
 *
 * @param  connectionHandle - controller link connection handle.
 * @param  keySize - size of encryption keys.
 * @param  mitmRequired - TRUE (yes) or FALSE (no).
 *
 * @return @ref SUCCESS if the link is authenticated
 * @return @ref bleNotConnected - connection handle is invalid
 * @return @ref LINKDB_ERR_INSUFFICIENT_AUTHEN - link is not encrypted
 * @return @ref LINBDB_ERR_INSUFFICIENT_KEYSIZE - key size encrypted is not large enough
 * @return @ref LINKDB_ERR_INSUFFICIENT_ENCRYPTION - link is encrypted, but not authenticated
 */
  extern uint8 linkDB_Authen( uint16 connectionHandle, uint8 keySize,
                              uint8 mitmRequired );

/**
 * Get the role of a physical link.
 *
 * @param  connectionHandle - controller link connection handle.
 *
 * @return @ref GAP_Profile_Roles
 * @return 0 - unknown
 */
  extern uint8 linkDB_Role( uint16 connectionHandle );

/**
 * Perform a function of each connection in the link database.
 *
 * @param cb - connection callback function.
 */
  extern void linkDB_PerformFunc( pfnPerformFuncCB_t cb );

/**
 * Set a device into Secure Connection Only Mode.
 *
 * @param  state -  TRUE for Secure Connections Only Mode. FALSE to disable
 *         Secure Connections Only Mode.
 */
  extern void linkDB_SecurityModeSCOnly( uint8 state );

/**
 * Sets the local device's address used during connection establishment.
 * WARNING: never use it before connection establish complete evt, must be called with valid connectionHandle
 *
 * @param       connHandle - record connection handle
 *              addr       - The address to set
 *
 */
  extern void linkDB_setAddrByConnHandle( uint16 connHandle, uint8_t *addr );

/**
 * Returns the local device's address used during connection establishment
 * WARNING: never use it before connection establish complete evt, must be called with valid connectionHandle

 * @param       connHandle - record connection handle
 *
 * @return      Device's address used during connection establishment.
 *              NULL if BDAddrPerConnHandle isn't initialized.
 *              Unknown behaviour If no connection for this connection handle.
 */
  extern uint8_t *linkDB_getAddrByConnHandle( uint16 connHandle );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* LINKDB_INTERNAL_H */
