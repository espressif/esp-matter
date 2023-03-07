/***************************************************************************//**
 * @file
 * @brief CANopen Process Data Objects (Pdo) Service - Receive Pdo
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_CANOPEN_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <canopen_cfg.h>

#if (CANOPEN_RPDO_MAX_QTY > 0)
#include  <cpu/include/cpu.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/rtos_err.h>
#include  <common/include/lib_def.h>

#include  <canopen/include/canopen_if.h>
#include  <canopen/source/core/canopen_core_priv.h>
#include  <canopen/source/core/canopen_if_priv.h>
#include  <canopen/source/objects/sync/canopen_sync_priv.h>
#include  <canopen/source/objects/pdo/canopen_rpdo_priv.h>
#include  <canopen/source/core/canopen_obj_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (CAN, CANOPEN)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_CANOPEN

/********************************************************************************************************
 ********************************************************************************************************
 *                                         INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          CANopen_RPdoInit()
 *
 * @brief    Checks the object dictionary and creates the PDO related configuration data from the
 *           object dictionary settings.
 *
 * @param    p_pdo   Pointer to start of RPDO array.
 *
 * @param    p_node  Pointer to parent node object.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void CANopen_RPdoInit(CANOPEN_RPDO *p_pdo,
                      CANOPEN_NODE *p_node,
                      RTOS_ERR     *p_err)
{
  CANOPEN_NODE_HANDLE handle;
  CPU_INT16S          nbr;                                      // Local: loop through RPDO number
  CPU_INT08U          rnum;                                     // LocaL: number of PDO Com Entries

  CANOPEN_NODE_HANDLE_SET(handle, p_node);

  for (nbr = 0; nbr < CANOPEN_RPDO_MAX_QTY; nbr++) {            // loop through all possible supported RPDO
    p_pdo[nbr].MsgId = 0;                                       // clear message identifier
    p_pdo[nbr].MapObjQty = 0;                                   // indicate no configured message
    CANopen_DictByteRd(handle,                                  // check RPDO communication settings
                       CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_RPDO_COMM_PARAM_START, 0),
                       &rnum,
                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {           // RPDO Communication settings are existing
      CANopen_RPdoReset(p_pdo, nbr, p_err);                     // scan RPDO communication and mapping
    }
  }
}

/****************************************************************************************************//**
 *                                           CANopen_RPdoClr()
 *
 * @brief    Sets the PDO related configuration data to a known state (inactive).
 *
 * @param    p_pdo   Pointer to start of RPDO array.
 *
 * @param    p_node  Pointer to parent node object.
 *******************************************************************************************************/
void CANopen_RPdoClr(CANOPEN_RPDO *p_pdo,
                     CANOPEN_NODE *p_node)
{
  CPU_INT16S nbr;                                               // Local: loop through RPDO number

  RTOS_ASSERT_DBG((p_pdo != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG((p_node != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  for (nbr = 0; nbr < CANOPEN_RPDO_MAX_QTY; nbr++) {            // loop through all possible supported RPDO
    p_pdo[nbr].NodePtr = p_node;                                // link CANopen node informations
    p_pdo[nbr].MsgId = 0;                                       // clear message identifier
    p_pdo[nbr].MapObjQty = 0;                                   // indicate no configured message
  }
}

/****************************************************************************************************//**
 *                                          CANopen_RPdoReset()
 *
 * @brief    Scans the object dictionary for the RPDO communication and mapping parameter. The function will
 *           fill the RPDO info structure with the corresponding informations.
 *
 * @param    p_pdo   Pointer to start of RPDO array
 *
 * @param    nbr     RPDO Number (0..511).
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) The following list shows the considered communication parameter entries:
 *               -# 0x1400+[nbr] : 0x01 = RPDO CAN identifier
 *               -# 0x1400+[nbr] : 0x02 = Transmission type
 *
 * @note     (2) The object entry 0x1400+[nbr] : 0x03..0x05 are reserved and should be not existing.
 *               This is not checked with this function.
 *******************************************************************************************************/
void CANopen_RPdoReset(CANOPEN_RPDO *p_pdo,
                       CPU_INT16S   nbr,
                       RTOS_ERR     *p_err)
{
  CANOPEN_NODE_HANDLE handle;
  CANOPEN_RPDO        *p_rpdo;
  CPU_INT08U          map_obj_ix;
  CPU_INT32U          id = CANOPEN_RPDO_COMM_COB_ID_VALID;
  CPU_INT08U          type = 0;                                               // Local: transmission type from profile

  p_rpdo = &p_pdo[nbr];                                                       // set pointer to working pdo
  p_rpdo->MsgId = 0;                                                          // clear message identifier
  p_rpdo->MapObjQty = 0;                                                      // indicate no configured message
  for (map_obj_ix = 0; map_obj_ix < CANOPEN_RPDO_MAX_MAP_QTY; map_obj_ix++) { // loop through object map array
    p_rpdo->MapPtrTbl[map_obj_ix] = 0;                                        // clear map entry
  }

  CANOPEN_NODE_HANDLE_SET(handle, p_rpdo->NodePtr);

#if (CANOPEN_SYNC_EN == DEF_ENABLED)
  if ((p_rpdo->Flag & CANOPEN_RPDO_FLG_S_) != 0) {              // see, if RPDO is synchronized
    CANopen_SyncRemove(&p_pdo->NodePtr->Sync,                   // remove RPDO from SYNC management table
                       nbr,
                       CANOPEN_SYNC_FLG_RX);
  }
#endif
  p_rpdo->Flag = 0;                                             // clear flag attributes

  //                                                               -------------- COMUNICATION PARAMETER --------------
  CANopen_DictByteRd(handle,                                    // get RPDO transmission type
                     CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_RPDO_COMM_PARAM_START + nbr, 2),
                     &type,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#if (CANOPEN_SYNC_EN == DEF_DISABLED)
  if (type <= CANOPEN_RPDO_COMM_TYPE_SYNC_MAX_LIMIT) {          // see, if synchronous transmission type
    return;
  }
#endif

  CANopen_DictLongRd(handle,                                    // get RPDO identifier
                     CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_RPDO_COMM_PARAM_START + nbr, 1),
                     &id,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
  if ((id & CANOPEN_RPDO_COMM_COB_ID_RTR) == 0) {               // see, if PDO is configured with RTR (not supported)
    return;
  }
  if ((id & CANOPEN_RPDO_COMM_COB_ID_FRM) != 0) {               // see, if PDO is configured with ext. ID (mixed IDs...
    return;                                                     // ...not supported)
  }
  if ((id & CANOPEN_RPDO_COMM_COB_ID_VALID) == 0) {
    //                                                             set identifier in message
    p_pdo[nbr].MsgId = (id & CANOPEN_RPDO_COMM_COB_ID_CAN_ID_29_BITS_MSK);
    p_pdo[nbr].Flag = CANOPEN_RPDO_FLG__E;                      // set message type (transmit message)
#if (CANOPEN_SYNC_EN == DEF_ENABLED)
    if (type <= CANOPEN_RPDO_COMM_TYPE_SYNC_MAX_LIMIT) {        // see, if transmission type is 0..240
      p_pdo[nbr].Flag |= CANOPEN_RPDO_FLG_S_;                   // set message type to synchronous
    }
#endif
  } else {
    p_pdo[nbr].MsgId = CANOPEN_RPDO_COMM_COB_ID_VALID;          // set identifier in message
    p_pdo[nbr].Flag = 0;                                        // set message type (transmit message)
  }
  //                                                               ---------------- MAPPING PARAMETER -----------------
  CANopen_RPdoMapGet(p_pdo, nbr, p_err);                        // get RPDO mapping from object dictionary

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_CANOPEN_xPDO_MAP_INVALID_CFG);
    return;
  }
  if ((p_pdo[nbr].Flag & CANOPEN_RPDO_FLG__E) != 0) {           // see, if message configuration is valid
#if (CANOPEN_SYNC_EN == DEF_ENABLED)
    if (type <= CANOPEN_RPDO_COMM_TYPE_SYNC_MAX_LIMIT) {        // transmission type: synchronous 0 .. 240
      CANopen_SyncAdd(&p_pdo[nbr].NodePtr->Sync,
                      nbr,
                      CANOPEN_SYNC_FLG_RX,
                      type);
    }
#endif
  }
}

/****************************************************************************************************//**
 *                                         CANopen_RPdoMapGet()
 *
 * @brief    Gets the PDO mapping data out of the object dictionary and puts the pre-calculated values in
 *           the CAN message configuration.
 *
 * @param    p_pdo   Pointer to start of RPDO array.
 *
 * @param    nbr     RPDO Number (0..511).
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) The following list shows the considered mapping parameter entries:
 *               -# 0x1600+[nbr] : 0x00 = Number of mapped signals (0..8)
 *               -# 0x1600+[nbr] : 0x01..0x08 = Mapped signal
 *******************************************************************************************************/
void CANopen_RPdoMapGet(CANOPEN_RPDO *p_pdo,
                        CPU_INT16U   nbr,
                        RTOS_ERR     *p_err)
{
  CANOPEN_NODE_HANDLE handle;
  CANOPEN_OBJ         *p_obj;                                   // Local: pointer to object entry
  CPU_INT32U          mapping;                                  // Local: PDO mapping information
  CPU_INT16U          idx;                                      // Local: object entry index
  CPU_INT16U          map_obj_ix;                               // Local: object entry index
  CPU_INT08U          map_obj_cnt_ix;                           // Local: Loop counter for mapped objects
  CPU_INT08U          map_obj_qty;                              // Local: number of PDO mappings
  CPU_INT08U          dlc;                                      // Local: resulting data length code
  CPU_INT08U          dummy = 0;                                // Local: number of dummy bytes

  //                                                               set index to TPDO mapping profile
  idx = CANOPEN_COMM_PROFILE_OBJ_IX_RPDO_MAPPING_PARAM_START + nbr;

  CANOPEN_NODE_HANDLE_SET(handle, p_pdo[nbr].NodePtr);

  //                                                               get number of mapped signals
  CANopen_DictByteRd(handle, CANOPEN_DEV(idx, 0), &map_obj_qty, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;                                                                  // abort
  }
  dlc = 0;                                                                   // clear number of bytes in message

  for (map_obj_cnt_ix = 0; map_obj_cnt_ix < map_obj_qty; map_obj_cnt_ix++) { // Loop through all mapped signals
    CANopen_DictLongRd(handle,                                               // get mapping information of this PDO
                       CANOPEN_DEV(idx, 1 + map_obj_cnt_ix),
                       &mapping,
                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;                                                   // abort
    }
    //                                                             add number of bits, converted to bytes
    dlc += (CPU_INT08U)(mapping & CANOPEN_RPDO_MAPPING_OBJ_LEN_MSK) >> 3;
    if (dlc > CANOPEN_RPDO_MAX_DATA_PAYLOAD) {                  // more than 8 byte not possible in a PDO
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
      return;                                                   // abort
    }
    //                                                             get index out of the mapping
    map_obj_ix = mapping >> CANOPEN_RPDO_MAPPING_OBJ_IX_BIT_SHIFT;
    if ((map_obj_ix == 2) || (map_obj_ix == 5)) {               // see, if index is between 2 or 5; this
                                                                // indicates a dummy BYTE mapping entry
                                                                // set object pointer indicating dummy byte
      p_pdo[nbr].MapPtrTbl[map_obj_cnt_ix + dummy] = DEF_NULL;
      dummy++;                                                  // increment number of dummy bytes
    } else if ((map_obj_ix == 3) || (map_obj_ix == 6)) {        // see, if index is between 3 or 6; this
                                                                // indicates a dummy WORD mapping entry
                                                                // set object pointer indicating dummy byte
      p_pdo[nbr].MapPtrTbl[map_obj_cnt_ix + dummy] = DEF_NULL;
      dummy++;                                                  // increment number of dummy bytes
                                                                // set object pointer indicating dummy byte
      p_pdo[nbr].MapPtrTbl[map_obj_cnt_ix + dummy] = DEF_NULL;
      dummy++;                                                  // increment number of dummy bytes
    } else if ((map_obj_ix == 4) || (map_obj_ix == 7)) {        // see, if index is between 3 or 6; this
                                                                // indicates a dummy LONG mapping entry
                                                                // set object pointer indicating dummy byte
      p_pdo[nbr].MapPtrTbl[map_obj_cnt_ix + dummy] = DEF_NULL;
      dummy++;                                                  // increment number of dummy bytes
                                                                // set object pointer indicating dummy byte
      p_pdo[nbr].MapPtrTbl[map_obj_cnt_ix + dummy] = DEF_NULL;
      dummy++;                                                  // increment number of dummy bytes
      p_pdo[nbr].MapPtrTbl[map_obj_cnt_ix + dummy] = DEF_NULL;  // set object pointer indicating dummy byte
      dummy++;                                                  // increment number of dummy bytes
      p_pdo[nbr].MapPtrTbl[map_obj_cnt_ix + dummy] = DEF_NULL;  // set object pointer indicating dummy byte
      dummy++;                                                  // increment number of dummy bytes
    } else {
      //                                                           get mapping information this PDO
      p_obj = CANopen_DictFind(&p_pdo->NodePtr->Dict, mapping, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {         // see, if object entry is not found
        return;                                                 // abort
      }

      p_pdo[nbr].MapPtrTbl[map_obj_cnt_ix + dummy] = p_obj;     // add object pointer to RPDO map
    }
  }

  p_pdo[nbr].MapObjQty = map_obj_qty + dummy;                   // set number of RPDO object references
}

/****************************************************************************************************//**
 *                                          CANopen_RPdoCheck()
 *
 * @brief    Checks the received CAN message frame to be a RPDO message.
 *
 * @param    p_pdo   Pointer to start of RPDO array.
 *
 * @param    p_frm   Received CAN message frame.
 *
 * @return   if positive, the number of the destination RPDO.
 *           if negative, not a valid RPDO CAN message.
 *******************************************************************************************************/
CPU_INT16S CANopen_RPdoCheck(CANOPEN_RPDO   *p_pdo,
                             CANOPEN_IF_FRM *p_frm)
{
  CPU_INT16S result = -1;                                       // Local: function result
  CPU_INT16U n;                                                 // Local: PDO counter

  n = 0;                                                        // reset PDO counter
  while (n < CANOPEN_RPDO_MAX_QTY) {                            // Loop through whole PDO table
                                                                // Look for ID and receive type
    if ((p_pdo[n].Flag & CANOPEN_RPDO_FLG__E) != 0) {
      if (p_pdo[n].MsgId == p_frm->MsgId) {
        result = n;
        break;
      }
    }
    n++;                                                        // increment counter
  }
  return (result);                                              // return function result
}

/****************************************************************************************************//**
 *                                           CANopen_RPdoRd()
 *
 * @brief    Distributes of a RPDO into the corresponding signals within the object dictionary.
 *
 * @param    p_pdo   Pointer to start of RPDO array.
 *
 * @param    nbr     RPDO Number (0..511).
 *
 * @param    p_frm   Pointer to received CAN frame.
 *******************************************************************************************************/
void CANopen_RPdoRd(CANOPEN_RPDO   *p_pdo,
                    CPU_INT16U     nbr,
                    CANOPEN_IF_FRM *p_frm)
{
  CPU_INT16S err = 0;                                           // Local: error code
  RTOS_ERR   local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  if ((p_pdo[nbr].Flag & CANOPEN_RPDO_FLG_S_) == 0) {           // if not a synchronized PDO
    if ((p_pdo->NodePtr->EventFnctsPtr != DEF_NULL)
        && (p_pdo->NodePtr->EventFnctsPtr->RpdoOnRx != DEF_NULL)) {
      CANOPEN_NODE_HANDLE handle;

      CANOPEN_NODE_HANDLE_SET(handle, p_pdo->NodePtr);
      //                                                           Application callback function
      err = p_pdo->NodePtr->EventFnctsPtr->RpdoOnRx(handle, p_frm);
    }

    if (err == 0) {                                             // see, if msg is not consumed by callback
      CANopen_RPdoWr(&p_pdo[nbr], p_frm, &local_err);           // write content to object dictionary
      if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
        CANOPEN_CTR_ERR_INC(p_pdo->NodePtr->CntErrPtr->Pdo.RPdoWrFailCtr);
        return;
      }
    }
#if (CANOPEN_SYNC_EN == DEF_ENABLED)
  } else {                                                      // otherwise: if see if synchronized PDO
    CANopen_SyncRx(&p_pdo[nbr].NodePtr->Sync, p_frm);           // yes: handle synchronous receive PDO
#endif
  }
}

/****************************************************************************************************//**
 *                                           CANopen_RPdoWr()
 *
 * @brief    Write the received CAN message data to the object dictionary.
 *
 * @param    p_pdo   Pointer to start of RPDO array.
 *
 * @param    p_frm   Received CAN message frame.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void CANopen_RPdoWr(CANOPEN_RPDO   *p_pdo,
                    CANOPEN_IF_FRM *p_frm,
                    RTOS_ERR       *p_err)
{
  CANOPEN_OBJ *p_obj;                                           // Local: pointer to linked object
  CPU_BOOLEAN valid_size = DEF_NO;                              // Local: valid size flag
  CPU_INT32U  val;
  CPU_INT08U  map_obj_ix;                                       // Local: loop through linked objects
  CPU_INT08U  size;                                             // Local: size of object entry
  CPU_INT08U  data_len_code = 0;                                // Local: data length code
  RTOS_ERR    local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               loop through RPDO object map
  for (map_obj_ix = 0; map_obj_ix < p_pdo->MapObjQty; map_obj_ix++) {
    p_obj = p_pdo->MapPtrTbl[map_obj_ix];                       // get next object pointer
    if (p_obj != 0) {                                           // see, if not a dummy byte
      size = (CPU_INT08U)CANopen_ObjSizeGet(p_obj, 0L, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        return;
      }

      if (!CANOPEN_OBJ_IS_RD_ONLY(p_obj->Key)) {
        KAL_LockAcquire(p_pdo->NodePtr->ObjLock,
                        KAL_OPT_PEND_NONE,
                        p_pdo->NodePtr->LockTimeoutMs,
                        p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          RTOS_ERR_CONTEXT_REFRESH(*p_err);
          return;
        }
      }

      if (size == CANOPEN_OBJ_BYTE) {                           // see, if object is a byte
                                                                // get byte from payload
        val = CANOPEN_FRM_GET_BYTE(p_frm, data_len_code);
        data_len_code++;                                        // increment dlc position
        valid_size = DEF_YES;
      } else if (size == CANOPEN_OBJ_WORD) {                    // otherwise, see if object is a word
                                                                // get word from payload
        val = CANOPEN_FRM_GET_WORD(p_frm, data_len_code);
        data_len_code += 2;                                     // increment dlc position
        valid_size = DEF_YES;
      } else if (size == CANOPEN_OBJ_LONG) {                    // otherwise, see if object is a long
                                                                // get long from payload
        val = CANOPEN_FRM_GET_LONG(p_frm, data_len_code);
        data_len_code += 4;                                     // increment dlc position
        valid_size = DEF_YES;
      }

      if (valid_size == DEF_YES) {
        CANopen_ObjValWr(p_pdo->NodePtr,
                         p_obj,                                 // write object with message payload data
                         (void *)&val,
                         size,
                         p_pdo->NodePtr->NodeId,
                         p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          CANOPEN_CTR_ERR_INC(p_pdo->NodePtr->CntErrPtr->Pdo.RPdoWrFailCtr);
        }

        valid_size = DEF_NO;
      }

      if (!CANOPEN_OBJ_IS_RD_ONLY(p_obj->Key)) {
        KAL_LockRelease(p_pdo->NodePtr->ObjLock,
                        &local_err);
        RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }
    }
  }
}

#endif // CANOPEN_RPDO_MAX_QTY

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_CANOPEN_AVAIL
