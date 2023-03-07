/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Application framework device database.
 *
 *  Copyright (c) 2011-2019 Arm Ltd. All Rights Reserved.
 *
 *  Copyright (c) 2019 Packetcraft, Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 * Copyright (c) 2021, Qorvo Inc
 *
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 */
/*************************************************************************************************/
#ifndef APP_DB_H
#define APP_DB_H

#include "wsf_os.h"
#include "dm_api.h"
#include "att_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \addtogroup APP_FRAMEWORK_DB_API
 *  \{ */

/**************************************************************************************************
  Macros
**************************************************************************************************/

/*! \brief No device database record handle  */
#define APP_DB_HDL_NONE                   NULL

/**************************************************************************************************
  Data Types
**************************************************************************************************/

/*! \brief Device database record handle type */
typedef void *appDbHdl_t;

/**************************************************************************************************
  Function Declarations
**************************************************************************************************/

/** \name App Database
 * Store known device and security information.
 */
/**@{*/

/*************************************************************************************************/
/*!
 *  \brief  Initialize the device database.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbInit(void);

/*************************************************************************************************/
/*!
 *  \brief  Create a new device database record.
 *
 *  \param  addrType  Address type.
 *  \param  pAddr     Peer device address.
 *
 *  \return Database record handle.
 */
/*************************************************************************************************/
appDbHdl_t AppDbNewRecord(uint8_t addrType, uint8_t *pAddr);

/*************************************************************************************************/
/*!
*  \brief  Get next device database record for a given database record. For the first database
*          record, the function should be called with 'hdl' set to 'APP_DB_HDL_NONE'.
*
*  \param  hdl  Database record handle.
*
*  \return Next database record handle found. APP_DB_HDL_NONE, otherwise.
*/
/*************************************************************************************************/
appDbHdl_t AppDbGetNextRecord(appDbHdl_t hdl);

/*************************************************************************************************/
/*!
 *  \brief  Delete a new device database record.
 *
 *  \param  hdl       Database record handle.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbDeleteRecord(appDbHdl_t hdl);

/*************************************************************************************************/
/*!
 *  \brief  Validate a new device database record.  This function is called when pairing is
 *          successful and the devices are bonded.
 *
 *  \param  hdl       Database record handle.
 *  \param  keyMask   Bitmask of keys to validate.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbValidateRecord(appDbHdl_t hdl, uint8_t keyMask);

/*************************************************************************************************/
/*!
 *  \brief  Check if a record has been validated.  If it has not, delete it.  This function
 *          is typically called when the connection is closed.
 *
 *  \param  hdl       Database record handle.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbCheckValidRecord(appDbHdl_t hdl);

/*************************************************************************************************/
/*!
*  \brief  Check if a database record is in use.

*  \param  hdl       Database record handle.
*
*  \return TURE if record in use. FALSE, otherwise.
*/
/*************************************************************************************************/
bool_t AppDbRecordInUse(appDbHdl_t hdl);

/*************************************************************************************************/
/*!
 *  \brief  Check if there is a stored bond with any device.
 *
 *  \return TRUE if a bonded device is found, FALSE otherwise.
 */
/*************************************************************************************************/
bool_t AppDbCheckBonded(void);

/*************************************************************************************************/
/*!
 *  \brief  Delete all database records.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbDeleteAllRecords(void);

/*************************************************************************************************/
/*!
 *  \brief  Find a device database record by peer address.
 *
 *  \param  addrType  Address type.
 *  \param  pAddr     Peer device address.
 *
 *  \return Database record handle or APP_DB_HDL_NONE if not found.
 */
/*************************************************************************************************/
appDbHdl_t AppDbFindByAddr(uint8_t addrType, uint8_t *pAddr);

/*************************************************************************************************/
/*!
 *  \brief  Find a device database record from data in an LTK request.
 *
 *  \param  encDiversifier  Encryption diversifier associated with key.
 *  \param  pRandNum        Pointer to random number associated with key.
 *
 *  \return Database record handle or APP_DB_HDL_NONE if not found.
 */
/*************************************************************************************************/
appDbHdl_t AppDbFindByLtkReq(uint16_t encDiversifier, uint8_t *pRandNum);

/*************************************************************************************************/
/*!
 *  \brief  Get the device database record handle associated with an open connection.
 *
 *  \param  connId    Connection identifier.
 *
 *  \return Database record handle or APP_DB_HDL_NONE.
 */
/*************************************************************************************************/
appDbHdl_t AppDbGetHdl(dmConnId_t connId);

/*************************************************************************************************/
/*!
 *  \brief  Get a key from a device database record.
 *
 *  \param  hdl       Database record handle.
 *  \param  type      Type of key to get.
 *  \param  pSecLevel If the key is valid, returns the security level of the key.
 *
 *  \return Pointer to the key if the key is valid or NULL if not valid.
 */
/*************************************************************************************************/
dmSecKey_t *AppDbGetKey(appDbHdl_t hdl, uint8_t type, uint8_t *pSecLevel);

/*************************************************************************************************/
/*!
 *  \brief  Set a key in a device database record.
 *
 *  \param  hdl       Database record handle.
 *  \param  pKey      Key data.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetKey(appDbHdl_t hdl, dmSecKeyIndEvt_t *pKey);

/*************************************************************************************************/
/*!
 *  \brief  Get the client characteristic configuration descriptor table.
 *
 *  \param  hdl       Database record handle.
 *
 *  \return Pointer to client characteristic configuration descriptor table.
 */
/*************************************************************************************************/
uint16_t *AppDbGetCccTbl(appDbHdl_t hdl);

/*************************************************************************************************/
/*!
 *  \brief  Set a value in the client characteristic configuration table.
 *
 *  \param  hdl       Database record handle.
 *  \param  idx       Table index.
 *  \param  value     Client characteristic configuration value.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetCccTblValue(appDbHdl_t hdl, uint16_t idx, uint16_t value);

/*************************************************************************************************/
/*!
 *  \brief  Get the change aware state and client supported features record.
 *
 *  \param  hdl             Database record handle.
 *  \param  pIsChangeAware  Pointer to peer client's change aware status to a change in the database.
 *  \param  pCsf            Pointer to csf value pointer.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbGetCsfRecord(appDbHdl_t hdl, uint8_t *pIsChangeAware, uint8_t **pCsf);

/*************************************************************************************************/
/*!
 *  \brief  Set a client supported features record.
 *
 *  \param  hdl              Database record handle.
 *  \param  changeAwareState The state of awareness to a change.
 *  \param  pCsf             pointed client supported features.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetCsfRecord(appDbHdl_t hdl, uint8_t changeAwareState, uint8_t *pCsf);

/*************************************************************************************************/
/*!
 *  \brief  Set client's state of awareness to a change in the database.
 *
 *  \param  hdl        Database record handle. If \ref hdl == \ref NULL, state is set for all
 *                     clients.
 *  \param  state      The state of awareness to a change, see ::attClientAwareStates.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetClientChangeAwareState(appDbHdl_t hdl, uint8_t state);

/*************************************************************************************************/
/*!
 *  \brief  Get device's GATT database hash.
 *
 *  \return Pointer to database hash.
 */
/*************************************************************************************************/
uint8_t *AppDbGetDbHash(void);

/*************************************************************************************************/
/*!
 *  \brief  Set device's GATT database hash.
 *
 *  \param  pHash    GATT database hash to store.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetDbHash(uint8_t *pHash);

/*************************************************************************************************/
/*!
 *  \brief  Get the peer's database hash.
 *
 *  \param  hdl       Database record handle.
 *
 *  \return Pointer to database hash.
 */
/*************************************************************************************************/
uint8_t *AppDbGetPeerDbHash(appDbHdl_t hdl);

/*************************************************************************************************/
/*!
 *  \brief  Set a new peer database hash.
 *
 *  \param  hdl       Database record handle.
 *  \param  dbHash    Pointer to new hash.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetPeerDbHash(appDbHdl_t hdl, uint8_t *dbHash);

/*************************************************************************************************/
/*!
 *  \brief  Check if cached handles' validity are ascertained by reading the peer's database hash
 *
 *  \param  hdl       Database record handle.
 *
 *  \return \ref TRUE if peer's database hash must be read to verify handles have not changed.
 */
/*************************************************************************************************/
bool_t AppDbIsCacheCheckedByHash(appDbHdl_t hdl);

/*************************************************************************************************/
/*!
 *  \brief  Set if cached handles' validity are determined by reading the peer's database hash.
 *
 *  \param  hdl            Database record handle.
 *  \param  cacheByHash   \ref TRUE if peer's database must be read to verify cached handles.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetCacheByHash(appDbHdl_t hdl, bool_t cacheByHash);

/*************************************************************************************************/
/*!
*  \brief  Get the discovery status.
*
*  \param  hdl       Database record handle.
*
*  \return Discovery status.
*/
/*************************************************************************************************/
uint8_t AppDbGetDiscStatus(appDbHdl_t hdl);

/*************************************************************************************************/
/*!
 *  \brief  Get the discovery status.
 *
 *  \param  hdl       Database record handle.
 *
 *  \return Discovery status.
 */
/*************************************************************************************************/
uint8_t AppDbGetDiscStatus(appDbHdl_t hdl);

/*************************************************************************************************/
/*!
 *  \brief  Set the discovery status.
 *
 *  \param  hdl       Database record handle.
 *  \param  status    Discovery status.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetDiscStatus(appDbHdl_t hdl, uint8_t status);

/*************************************************************************************************/
/*!
 *  \brief  Get the cached handle list.
 *
 *  \param  hdl       Database record handle.
 *
 *  \return Pointer to handle list.
 */
/*************************************************************************************************/
uint16_t *AppDbGetHdlList(appDbHdl_t hdl);

/*************************************************************************************************/
/*!
 *  \brief  Set the cached handle list.
 *
 *  \param  hdl       Database record handle.
 *  \param  pHdlList  Pointer to handle list.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetHdlList(appDbHdl_t hdl, uint16_t *pHdlList);

/*************************************************************************************************/
/*!
 *  \brief  Get the device name.
 *
 *  \param  pLen      Returned device name length.
 *
 *  \return Pointer to UTF-8 string containing the device name or NULL if not set.
 */
/*************************************************************************************************/
char *AppDbGetDevName(uint8_t *pLen);

/*************************************************************************************************/
/*!
 *  \brief  Set the device name.
 *
 *  \param  len       Device name length.
 *  \param  pStr      UTF-8 string containing the device name.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetDevName(uint8_t len, char *pStr);

/*************************************************************************************************/
/*!
*  \brief  Get address resolution attribute value read from a peer device.
*
*  \param  hdl        Database record handle.
*
*  \return TRUE if address resolution is supported in peer device. FALSE, otherwise.
*/
/*************************************************************************************************/
bool_t AppDbGetPeerAddrRes(appDbHdl_t hdl);

/*************************************************************************************************/
/*!
*
*  \brief  Get address of a peer device.
*
*  \param  hdl        Database record handle.
*
*  \return Pointer to peer address.
*/
/*************************************************************************************************/
uint8_t *AppDbGetPeerAddress(appDbHdl_t hdl);

/*************************************************************************************************/
/*!
*  \brief  Get address type of a peer device.
*
*  \param  hdl        Database record handle.
*
*  \return Peer address type.
*/
/*************************************************************************************************/
uint8_t AppDbGetPeerAddressType(appDbHdl_t hdl);


/*************************************************************************************************/
/*!
*  \brief  Set address resolution attribute value for a peer device.
*
*  \param  hdl        Database record handle.
*  \param  addrRes    Peer address resolution attribue value.
*
*  \return None.
*/
/*************************************************************************************************/
void AppDbSetPeerAddrRes(appDbHdl_t hdl, uint8_t addrRes);

/*************************************************************************************************/
/*!
*  \brief  Get sign counter for a peer device.
*
*  \param  hdl        Database record handle.
*
*  \return Sign counter for peer device.
*/
/*************************************************************************************************/
uint32_t AppDbGetPeerSignCounter(appDbHdl_t hdl);

/*************************************************************************************************/
/*!
*  \brief  Set sign counter for a peer device.
*
*  \param  hdl          Database record handle.
*  \param  signCounter  Sign counter for peer device.
*
*  \return None.
*/
/*************************************************************************************************/
void AppDbSetPeerSignCounter(appDbHdl_t hdl, uint32_t signCounter);

/*************************************************************************************************/
/*!
 *  \brief  Get the peer device added to resolving list flag value.
 *
 *  \param  hdl        Database record handle.
 *
 *  \return TRUE if peer device's been added to resolving list. FALSE, otherwise.
 */
/*************************************************************************************************/
bool_t AppDbGetPeerAddedToRl(appDbHdl_t hdl);

/*************************************************************************************************/
/*!
 *  \brief  Set the peer device added to resolving list flag to a given value.
 *
 *  \param  hdl           Database record handle.
 *  \param  peerAddedToRl Peer device added to resolving list flag value.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetPeerAddedToRl(appDbHdl_t hdl, bool_t peerAddedToRl);

/*************************************************************************************************/
/*!
 *  \brief  Get resolvable private address only attribute present flag for a peer device.
 *
 *  \param  hdl        Database record handle.
 *
 *  \return TRUE if RPA Only attribute is present on peer device. FALSE, otherwise.
 */
/*************************************************************************************************/
bool_t AppDbGetPeerRpao(appDbHdl_t hdl);

/*************************************************************************************************/
/*!
 *  \brief  Set resolvable private address only attribute present flag for a peer device.
 *
 *  \param  hdl        Database record handle.
 *  \param  peerRpao   Resolvable private address only attribute present flag.
 *
 *  \return None.
 */
/*************************************************************************************************/
void AppDbSetPeerRpao(appDbHdl_t hdl, bool_t peerRpao);

void appDbRestoreBonds(void);

/**@}*/

/*! \} */    /*! APP_FRAMEWORK_DB_API */

#ifdef __cplusplus
};
#endif

#endif /* APP_DB_H */
