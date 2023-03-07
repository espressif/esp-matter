/***************************************************************************//**
 * @file
 * @brief CANopen Service Data Object (Sdo) Service - Sdo Server
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

#include  <cpu/include/cpu.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/rtos_err.h>
#include  <common/include/lib_utils.h>

#include  <canopen_cfg.h>
#include  <canopen/include/canopen_core.h>
#include  <canopen/include/canopen_dict.h>
#include  <canopen/source/core/canopen_core_priv.h>
#include  <canopen/source/objects/sdo/canopen_sdo_priv.h>
#include  <canopen/source/core/canopen_obj_priv.h>
#include  <canopen/source/core/canopen_if_priv.h>
#include  <canopen/source/core/canopen_dict_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (CAN, CANOPEN)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_CANOPEN

//                                                                 Transfer buffer size in bytes.
#if (CANOPEN_SDO_BLK_EN == DEF_ENABLED)
#define  CANOPEN_SDO_BUF_BYTE              (CANOPEN_SDO_MAX_SEG_QTY * 7)
#else
#define  CANOPEN_SDO_BUF_BYTE                         7
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                         INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           CANopen_SdoInit()
 *
 * @brief    Initializes the SDO server.
 *
 * @param    p_srv   Pointer to root element of SDO server array.
 *
 * @param    p_node  Pointer to parent CANopen node.
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) This function reads the content of the CANopen object directory with index 1200+[n]
 *               subindex 1 (for RX) and subindex 2 (for TX), where n is a counter from 0 to the
 *               maximal number of supported SDO servers (CO_SDO_N).
 *******************************************************************************************************/
void CANopen_SdoInit(CANOPEN_SDO  *p_srv,
                     CANOPEN_NODE *p_node,
                     RTOS_ERR     *p_err)
{
  CPU_INT08U n;                                                 // Local: Loop counter for SDO Lists

  for (n = 0; n < CANOPEN_SDO_MAX_SERVER_QTY; n++) {            // Loop through all SDO servers
    CANopen_SdoReset(p_srv, n, p_node);                         // reset SDO server internal data fields
    CANopen_SdoEn(p_srv, n, p_err);                             // enable SDO server acc. object directory
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      CANOPEN_CTR_ERR_INC(p_srv->NodePtr->CntErrPtr->Sdo.EnFailCtr);
      return;
    }
  }
}

/****************************************************************************************************//**
 *                                          CANopen_SdoReset()
 *
 * @brief    Resets the SDO server.
 *
 * @param    p_srv   Pointer to root element of SDO server array.
 *
 * @param    nbr     SDO server number.
 *
 * @param    p_node  Pointer to parent CANopen node.
 *
 * @note     (1) This function resets all internal data of the given SDO server.
 *******************************************************************************************************/
void CANopen_SdoReset(CANOPEN_SDO  *p_srv,
                      CPU_INT08U   nbr,
                      CANOPEN_NODE *p_node)
{
  CANOPEN_SDO *p_srvnum;                                        // Local: pointer to SDO server #num
#if ((CANOPEN_SDO_SEG_EN == DEF_ENABLED) || (CANOPEN_SDO_BLK_EN == DEF_ENABLED))
  CPU_INT32U offset;                                            // Local: Offset within SDO buffer memory
#endif

  RTOS_ASSERT_DBG((nbr < CANOPEN_SDO_MAX_SERVER_QTY), RTOS_ERR_INVALID_ARG,; );

  p_srvnum = &p_srv[nbr];                                       // get ptr to addressed SDO server
  p_srvnum->NodePtr = p_node;                                   // link node to this server
  p_srvnum->RxId = CANOPEN_SDO_COB_ID_VALID;                    // clear request identifier
  p_srvnum->TxId = CANOPEN_SDO_COB_ID_VALID;                    // clear response identifier
  p_srvnum->FrmPtr = DEF_NULL;                                  // clear CAN frame reference
  p_srvnum->ObjPtr = DEF_NULL;                                  // clear object entry reference
#if ((CANOPEN_SDO_SEG_EN == DEF_ENABLED) || (CANOPEN_SDO_BLK_EN == DEF_ENABLED))
  offset = nbr * CANOPEN_SDO_BUF_BYTE;                          // calculate offset of SDO buffer in memory
  p_srvnum->Buf.StartPtr = &p_node->SdoBufPtr[offset];          // set start address of transfer buffer
  p_srvnum->Buf.CurPtr = &p_node->SdoBufPtr[offset];            // set next free buffer location
  p_srvnum->Buf.Len = 0;                                        // clear number of bytes in buffer
#endif
#if (CANOPEN_SDO_SEG_EN == DEF_ENABLED)
  p_srvnum->Seg.ToggleBit = 0;                                  // clear segmented transfer toggle bit
  p_srvnum->Seg.Num = 0;                                        // clear segmented transfer length
  p_srvnum->Seg.XferLen = 0;                                    // set segmented transfer counter inactive
#endif
#if (CANOPEN_SDO_BLK_EN == DEF_ENABLED)
  p_srvnum->Blk.State = SDO_BLK_STATE_IDLE;                     // set block transfer not active
#endif
}

/****************************************************************************************************//**
 *                                            CANopen_SdoEn()
 *
 * @brief    Enables the SDO server.
 *
 * @param    p_srv   Pointer to root element of SDO server array.
 *
 * @param    nbr     SDO server number.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void CANopen_SdoEn(CANOPEN_SDO *p_srv,
                   CPU_INT08U  nbr,
                   RTOS_ERR    *p_err)
{
  CANOPEN_NODE_HANDLE handle;
  CPU_INT32U          rx_id;                                    // Local: CAN identifier for SDO requests
  CPU_INT32U          tx_id;                                    // Local: CAN identifier for SDO response
  CANOPEN_NODE        *p_node;                                  // Local: pointer to parent node
  CANOPEN_SDO         *p_srvnum;                                // Local: pointer to SDO server #num

  RTOS_ASSERT_DBG_ERR_SET((nbr < CANOPEN_SDO_MAX_SERVER_QTY), *p_err, RTOS_ERR_INVALID_ARG,; );

  p_srvnum = &p_srv[nbr];                                       // get ptr to addressed SDO server
  p_srvnum->RxId = CANOPEN_SDO_COB_ID_VALID;                    // disable SDO server for function abort
  p_srvnum->TxId = CANOPEN_SDO_COB_ID_VALID;

  p_node = p_srv->NodePtr;                                      // get parent node

  CANOPEN_NODE_HANDLE_SET(handle, p_node);

  CANopen_DictLongRd(handle,                                    // read object entry with RX identifier
                     CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_SDO_SERVER_PARAM_START + nbr, 1),
                     &rx_id,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CANopen_DictLongRd(handle,                                    // read object entry with TX identifier
                     CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_SDO_SERVER_PARAM_START + nbr, 2),
                     &tx_id,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if ((DEF_BIT_IS_CLR(rx_id, CANOPEN_SDO_COB_ID_VALID))         // see, if both identifiers are valid
      && (DEF_BIT_IS_CLR(tx_id, CANOPEN_SDO_COB_ID_VALID))) {
    p_srvnum->RxId = rx_id;                                     // yes: set request and response ID's
    p_srvnum->TxId = tx_id;
  }
}

/****************************************************************************************************//**
 *                                          CANopen_SdoCheck()
 *
 * @brief    Check for SDO frame.
 *
 * @param    p_srv   Pointer to root element of SDO server array.
 *
 * @param    p_frm   Frame received from CAN bus.
 *
 * @return   Pointer to addressed SDO server.
 *           DEF_NULL, if an error is detected or the frame is not a SDO request.
 *
 * @note     (1) This function checks the given frame to be a SDO request. If the frame is identified
 *               to be a SDO request, the identifier in the frame will be modified to be the
 *               corresponding SDO response.
 *******************************************************************************************************/
CANOPEN_SDO *CANopen_SdoCheck(CANOPEN_SDO    *p_srv,
                              CANOPEN_IF_FRM *p_frm)
{
  CANOPEN_SDO *p_sdo = DEF_NULL;                                // Local: function result
  CPU_INT08U  n;                                                // Local: SDO counter

  if (p_frm != DEF_NULL) {                                      // see, if CAN frame is a valid pointer
    n = 0;                                                      // reset SDO counter
                                                                // Loop through SDO server list
    while ((n < CANOPEN_SDO_MAX_SERVER_QTY) && (p_sdo == DEF_NULL)) {
      //                                                           see, if received ID is equal request ID
      if (CANOPEN_FRM_GET_COB_ID(p_frm) == p_srv[n].RxId) {
        CANOPEN_FRM_SET_COB_ID(p_frm, p_srv[n].TxId);           // set corresponding response ID
        p_srv[n].FrmPtr = p_frm;                                // store request in SDO server object
        if (p_srv[n].ObjPtr == DEF_NULL) {
          p_srv[n].Idx = CANOPEN_FRM_GET_WORD(p_frm, 1);
          p_srv[n].Sub = CANOPEN_FRM_GET_BYTE(p_frm, 3);
        }
        p_sdo = &p_srv[n];                                      // return pointer to SDO server
      }
      n++;                                                      // increment SDO identifier
    }
  }

  return (p_sdo);
}

/****************************************************************************************************//**
 *                                           CANopen_SdoResp()
 *
 * @brief    Generates SDO response.
 *
 * @param    p_srv   Pointer to the root element of SDO server array.
 *
 * @param    p_err   Error pointer.
 *
 * @return   Response status:
 *               - CANOPEN_RESPONSE_STATUS_NO_ERROR
 *               - CANOPEN_RESPONSE_STATUS_ABORTED
 *               - CANOPEN_RESPONSE_STATUS_OTHER
 *
 * @note     (1) This function interprets the data byte #0 of the SDO request and organizes the
 *               generation of the corresponding response.
 *******************************************************************************************************/
CANOPEN_RESPONSE_STATUS CANopen_SdoResp(CANOPEN_SDO *p_srv,
                                        RTOS_ERR    *p_err)
{
  CANOPEN_RESPONSE_STATUS status;
  CPU_INT08U              cmd;                                  // Local: command byte
  CPU_INT08U              cmd_code;
  CPU_INT08U              xfer_type;

  cmd = CANOPEN_FRM_GET_BYTE(p_srv->FrmPtr, 0);                 // request: get command byte
                                                                // ---------------------- ABORT -----------------------
  if (cmd == CANOPEN_SDO_CMD_ABORT_XFER_REQ_FMTED) {            // cs=4: abort request
#if ((CANOPEN_SDO_SEG_EN == DEF_ENABLED) || (CANOPEN_SDO_BLK_EN == DEF_ENABLED))
    CANopen_SdoAbortReq(p_srv);                                 // perform abort of SDO transfer
#endif
    return (CANOPEN_RESPONSE_STATUS_OTHER);                     // no transmission of response frame
  }

  cmd_code = DEF_BIT_FIELD_RD(cmd, CANOPEN_SDO_CMD_SPECIFIER_MSK);
  xfer_type = DEF_BIT_FIELD_RD(cmd, CANOPEN_SDO_CLIENT_CMD_XFER_TYPE_MSK);

#if (CANOPEN_SDO_BLK_EN == DEF_ENABLED)
  switch (p_srv->Blk.State) {
    case SDO_BLK_STATE_DOWNLOAD:                                // see, if block download is active
      status = CANopen_SdoBlkDownload(p_srv, p_err);            // perform block download
      return (status);

    case SDO_BLK_STATE_DOWNLOAD_WAIT:                           // otherwise see, if download is waiting

      if ((cmd & 0xE3) == 0xC1) {                               // ---------------- END BLOCK DOWNLOAD ----------------
        status = CANopen_SdoBlkEndDownload(p_srv, p_err);       // perform end block download
      } else {                                                  // otherwise: within block segments
        p_srv->Blk.State = SDO_BLK_STATE_DOWNLOAD;              // set to download active
        status = CANopen_SdoBlkDownload(p_srv, p_err);          // perform block download
      }
      return (status);                                          // return function result

    case SDO_BLK_STATE_UPLOAD:                                  // otherwise see, if block upload is active
                                                                // ----------------- END BLOCK UPLOAD -----------------
      if (cmd == 0xA1) {
        status = CANopen_SdoBlkEndUpload(p_srv);                // perform end block upload
                                                                // ----------------- ACK BLOCK UPLOAD -----------------
      } else if ((cmd & 0xE3) == 0xA2) {
        status = CANopen_SdoBlkAckUpload(p_srv, p_err);         // otherwise: within block segments
      } else {                                                  // otherwise: unknown response in upload
        CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_CMD);           // Abort message: command invalid/unknown
        status = CANOPEN_RESPONSE_STATUS_ABORTED;
      }
      return (status);                                          // return function result

    case SDO_BLK_STATE_IDLE:
    default:
      break;
  }
#endif
  //                                                               ---------------- EXPEDITED DOWNLOAD ----------------
  if ((cmd_code == CANOPEN_SDO_CLIENT_CMD_REQ_DOWNLOAD_INITIATE)
      && (xfer_type == CANOPEN_SDO_CLIENT_CMD_XFER_TYPE_EXPEDITED)) {
    CANopen_SdoObjGet(p_srv, CANOPEN_SDO_WR, p_err);            // yes: link addressed object entry
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      status = CANopen_SdoExpeditedDownload(p_srv,              // perform expedited download transfer
                                            p_err);
    } else {
      status = CANOPEN_RESPONSE_STATUS_ABORTED;
    }
    //                                                             ---------------------- UPLOAD ----------------------
  } else if (cmd == CANOPEN_SDO_CLIENT_CMD_REQ_UPLOAD_INITIATE_FMTED) {
    CANopen_SdoObjGet(p_srv, CANOPEN_SDO_RD, p_err);            // yes: link addressed object entry
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      status = CANopen_SdoExpeditedUpload(p_srv,                // perform the upload transfer
                                          p_err);
    } else {
      status = CANOPEN_RESPONSE_STATUS_ABORTED;
    }

#if (CANOPEN_SDO_SEG_EN == DEF_ENABLED)
    //                                                             ------------ INITIATE SEGMENT DOWNLOAD -------------
  } else if ((cmd_code == CANOPEN_SDO_CLIENT_CMD_REQ_DOWNLOAD_INITIATE)
             && (xfer_type == CANOPEN_SDO_CLIENT_CMD_XFER_TYPE_NORMAL)) {
    CANopen_SdoObjGet(p_srv, CANOPEN_SDO_WR, p_err);            // yes: link addressed object entry
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      status = CANopen_SdoSegmentedInitDownload(p_srv, p_err);  // perform init of download transfer
    } else {
      status = CANOPEN_RESPONSE_STATUS_ABORTED;
    }
    //                                                             ----------------- SEGMENT DOWNLOAD -----------------
  } else if (cmd_code == CANOPEN_SDO_CLIENT_CMD_REQ_DOWNLOAD_SEG) {
    status = CANopen_SdoSegmentedDownload(p_srv, p_err);        // perform segmented download
                                                                // ------------------ SEGMENT UPLOAD ------------------
  } else if ((cmd & CANOPEN_SDO_CLIENT_CMD_REQ_UPLOAD_SEG_MSK_NO_TOGGLE) == CANOPEN_SDO_CLIENT_CMD_REQ_UPLOAD_SEG_FMTED) {
    status = CANopen_SdoSegmentedUpload(p_srv, p_err);          // perform segmented upload
#endif

#if (CANOPEN_SDO_BLK_EN == DEF_ENABLED)
    //                                                             --------------- INIT BLOCK DOWNLOAD ----------------
  } else if ((cmd & 0xF9) == 0xC0) {
    CANopen_SdoObjGet(p_srv, CANOPEN_SDO_WR, p_err);            // yes: link addressed object entry
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      status = CANopen_SdoBlkInitDownload(p_srv, p_err);        // perform init block download
    } else {
      status = CANOPEN_RESPONSE_STATUS_ABORTED;
    }

    //                                                             ---------------- INIT BLOCK UPLOAD -----------------
  } else if ((cmd & 0xE3) == 0xA0) {
    status = CANopen_SdoBlkInitUpload(p_srv, p_err);            // perform init block upload

    //                                                             ------------------- BLOCK UPLOAD -------------------
  } else if (cmd == 0xA3) {
    status = CANopen_SdoBlkUpload(p_srv, p_err);                // perform block upload
#endif
  } else {                                                      // otherwise: not supported/invalid command
    CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_CMD);               // Abort message: command invalid/unknown
    status = CANOPEN_RESPONSE_STATUS_ABORTED;
  }

  return (status);                                              // return function result
}

/****************************************************************************************************//**
 *                                          CANopen_SdoAbort()
 *
 * @brief    Aborts a SDO operation in case of error.
 *
 * @param    p_srv   Pointer to SDO server object.
 *
 * @param    err     Error pointer.
 *
 * @note     (1) This function generates the response for a SDO abort.
 *******************************************************************************************************/
void CANopen_SdoAbort(CANOPEN_SDO *p_srv,
                      CPU_INT32U  err)
{
  CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr,                           // set response: cs=4 (Abort SDO Transfer)
                       CANOPEN_SDO_CMD_ABORT_XFER_REQ_FMTED,
                       0);

  CANOPEN_FRM_SET_WORD(p_srv->FrmPtr, p_srv->Idx, 1);           // set index
  CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, p_srv->Sub, 3);           // set subindex
  CANOPEN_FRM_SET_LONG(p_srv->FrmPtr, err, 4);                  // set error code into data bytes #4..#7

  p_srv->ObjPtr = DEF_NULL;                                     // indicates: transfer finished
}

/****************************************************************************************************//**
 *                                          CANopen_SdoObjGet()
 *
 * @brief    Gets the addressed object.
 *
 * @param    p_srv   Pointer to the root element of SDO server array.
 *
 * @param    mode    Access mode:
 *                       - CANOPEN_SDO_RD
 *                       - CANOPEN_SDO_WR
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) This function looks for the addressed object in the object directory and generates
 *               several abort messages on bad addresses. If everything is fine, this function
 *               stores the corresponding object entry reference in the SDO server object.
 *******************************************************************************************************/
void CANopen_SdoObjGet(CANOPEN_SDO *p_srv,
                       CPU_INT16U  mode,
                       RTOS_ERR    *p_err)
{
  CANOPEN_OBJ *p_obj;                                           // Local: reference to object entry
  CPU_INT32U  key;                                              // Local: object entry key
  RTOS_ERR    local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  key = CANOPEN_DEV(p_srv->Idx, p_srv->Sub);                                     // construct key of object entry
  p_obj = CANopen_DictFind(&p_srv->NodePtr->Dict, key, p_err);                   // search addressed object entry
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {                              // see, if object entry is found
    if (((mode == CANOPEN_SDO_RD) && (CANOPEN_OBJ_IS_RD(p_obj->Key) != 0))
        || ((mode == CANOPEN_SDO_WR) && (CANOPEN_OBJ_IS_WR(p_obj->Key) != 0))) { // see, if req can be done on obj
      p_srv->ObjPtr = p_obj;                                                     // store found object entry
    } else {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
      CANopen_SdoAbort(p_srv, (mode == CANOPEN_SDO_RD) ? CANOPEN_SDO_ERR_RD : CANOPEN_SDO_ERR_WR);
    }
  } else {                                                              // otherwise: object is not found
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    if (p_srv->Sub == 0) {                                              // see, if accessed subindex was 0
      CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_OBJ);                     // Abort 'object not existant'
    } else {                                                            // otherwise: access subindex > 0
      key = CANOPEN_DEV(p_srv->Idx, 0);                                 // construct key of object entry main index
      p_obj = CANopen_DictFind(&p_srv->NodePtr->Dict, key, &local_err); // search main index object entry
      CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE, p_srv->NodePtr->CntErrPtr->Sdo.DictFindFailCtr);
      if (p_obj != DEF_NULL) {                                          // see, if main index is existing in OD
        CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_SUB_IX);                // yes: Abort 'subindex not existant'
      } else {                                                          // otherwise: object is not existing in OD
        CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_OBJ);                   // Abort 'object not existant'
      }
    }
  }
}

/****************************************************************************************************//**
 *                                        CANopen_SdoObjSizeGet()
 *
 * @brief    Gets the size of an object.
 *
 * @param    p_srv   Pointer to SDO server object.
 *
 * @param    width   Parameter size in bytes (or 0 if unknown).
 *
 * @param    p_err   Error pointer.
 *
 * @return   Size of the object, in bytes.
 *           0, if an error occurs.
 *
 * @note     (1) This function checks the given size according to the addressed object size in the object
 *               directory and generates several abort messages on bad size information. If everything is
 *               fine, this function returns the size of the object in bytes.
 *******************************************************************************************************/
CPU_INT32U CANopen_SdoObjSizeGet(CANOPEN_SDO *p_srv,
                                 CPU_INT32U  width,
                                 RTOS_ERR    *p_err)
{
  CPU_INT32U result = 0;                                        // Local: function result
  CPU_INT32U size;                                              // Local: object size in byte

  size = CANopen_ObjSizeGet(p_srv->ObjPtr, width, p_err);       // Get size of object in byte
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {             // object access failed (bad config)
    CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_TOS);               // Abort message: Data cannot be transfered
    return (0u);                                                // abort function with error indication
  }

  if (width != 0) {                                             // see, if a data width is given
    if (size == width) {                                        // yes: see, if object size = data width
      result = size;                                            // yes: data ok
    } else {                                                    // otherwise: data width mismatch error
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      if (size < width) {                                       // see if object is < data bytes
        CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_LEN_HIGH);      // yes: Abort 'datatype length too high'
      } else {                                                  // otherwise: object is > data bytes
        CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_LEN_SMALL);     // Abort 'datatype length too small'
      }
    }
  } else {                                                      // otherwise: data width unknown
    result = size;                                              // assume data ok
  }

  return (result);                                              // return function result
}

/****************************************************************************************************//**
 *                                     CANopen_SdoExpeditedUpload()
 *
 * @brief    Generates the response for a SDO expedited upload request.
 *
 * @param    p_srv   Pointer to SDO server object.
 *
 * @param    p_err   Error pointer.
 *
 * @return   Response status:
 *               - CANOPEN_RESPONSE_STATUS_NO_ERROR
 *               - CANOPEN_RESPONSE_STATUS_ABORTED
 *               - CANOPEN_RESPONSE_STATUS_OTHER
 *
 * @note     (1) The function checks the object size for matching into a simple expedited transfer.
 *               If possible, this transfer is used. The segmented transfer (if support available) is
 *               initiated in any other cases.
 *******************************************************************************************************/
CANOPEN_RESPONSE_STATUS CANopen_SdoExpeditedUpload(CANOPEN_SDO *p_srv,
                                                   RTOS_ERR    *p_err)
{
  CPU_INT32U              size;                                 // Local: object size in byte
  CPU_INT32U              data = 0;                             // Local: object entry value
  CANOPEN_RESPONSE_STATUS result;                               // Local: function result
  CPU_INT08U              cmd;                                  // Local: SDO command byte

  size = CANopen_SdoObjSizeGet(p_srv, 0, p_err);                // check data width acc. object
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    result = CANOPEN_RESPONSE_STATUS_ABORTED;
    //                                                             ---------------- EXPEDITED TRANSFER ----------------
    //                                                             see, if size is in valid range
  } else if ((size > 0) && (size <= CANOPEN_SDO_XFER_EXPEDITED_MAX_DATA_PAYLOAD)) {
    if (!CANOPEN_OBJ_IS_RD_ONLY(p_srv->ObjPtr->Key)) {
      KAL_LockAcquire(p_srv->NodePtr->ObjLock,
                      KAL_OPT_PEND_NONE,
                      p_srv->NodePtr->LockTimeoutMs,
                      p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        RTOS_ERR_CONTEXT_REFRESH(*p_err);
        result = CANOPEN_RESPONSE_STATUS_ABORTED;
      }
    }

    CANopen_ObjValRd(p_srv->NodePtr,
                     p_srv->ObjPtr,                             // read value from object entry
                     (void *)&data,
                     CANOPEN_OBJ_LONG,
                     p_srv->NodePtr->NodeId,
                     p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      CPU_INT32U err_code;

      switch (RTOS_ERR_CODE_GET(*p_err)) {
        case RTOS_ERR_CANOPEN_GEN_ERR:
          err_code = CANOPEN_SDO_ERR_GEN_ERR;
          break;

        case RTOS_ERR_CANOPEN_SDO_ERR_SUB_IX:
          err_code = CANOPEN_SDO_ERR_SUB_IX;
          break;

        default:
          err_code = CANOPEN_SDO_ERR_TOS;
          break;
      }

      if (!CANOPEN_OBJ_IS_RD_ONLY(p_srv->ObjPtr->Key)) {
        RTOS_ERR local_err;

        RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
        KAL_LockRelease(p_srv->NodePtr->ObjLock,
                        &local_err);
        RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
      }

      CANopen_SdoAbort(p_srv, err_code);

      return (CANOPEN_RESPONSE_STATUS_ABORTED);
    } else {                                                    // otherwise: reading successful
                                                                // response: scs=2, n=4-width, e=1, s=1
      cmd = (CANOPEN_SDO_SERVER_CMD_RESP_UPLOAD_INITIATE << CANOPEN_SDO_CMD_SPECIFIER_BIT_SHIFT);
      DEF_BIT_FIELD_WR(cmd,
                       (CPU_INT08U)(CANOPEN_SDO_XFER_EXPEDITED_MAX_DATA_PAYLOAD - size),
                       (CPU_INT08U)CANOPEN_SDO_SERVER_CMD_EXPEDITED_NBR_BYTES_UNUSED_MSK);
      DEF_BIT_SET(cmd, CANOPEN_SDO_SERVER_CMD_XFER_TYPE_MSK);
      DEF_BIT_SET(cmd, CANOPEN_SDO_SERVER_CMD_DATA_SET_SIZE_MSK);

      CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, cmd, 0);              // response: command byte
      CANOPEN_FRM_SET_LONG(p_srv->FrmPtr, data, 4);             // data in byte #4..#7
      result = CANOPEN_RESPONSE_STATUS_NO_ERROR;                // indicates: transfer accepted
    }

    if (!CANOPEN_OBJ_IS_RD_ONLY(p_srv->ObjPtr->Key)) {
      RTOS_ERR local_err;

      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      KAL_LockRelease(p_srv->NodePtr->ObjLock,
                      &local_err);
      RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }

    p_srv->ObjPtr = DEF_NULL;                                   // indicates: transfer finished

#if (CANOPEN_SDO_SEG_EN == DEF_ENABLED)
    //                                                             ----------------- NORMAL TRANSFER ------------------
    //                                                             see, if size is in valid range
  } else if (size > CANOPEN_SDO_XFER_EXPEDITED_MAX_DATA_PAYLOAD) {
    CANopen_SdoSegmentedInitUpload(p_srv, size, p_err);         // initialize segmented transfer
    result = CANOPEN_RESPONSE_STATUS_NO_ERROR;
  } else {                                                      // otherwise: no transfer possible
#endif
    CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_CMD);               // Abort 'command invalid/unknown'
    result = CANOPEN_RESPONSE_STATUS_ABORTED;
  }

  return (result);                                              // return function result
}

/****************************************************************************************************//**
 *                                    CANopen_SdoExpeditedDownload()
 *
 * @brief    Generates the response for a SDO expedited download request.
 *
 * @param    p_srv   Pointer to SDO server object.
 *
 * @param    p_err   Error pointer.
 *
 * @return   Response status:
 *               - CANOPEN_RESPONSE_STATUS_NO_ERROR
 *               - CANOPEN_RESPONSE_STATUS_ABORTED
 *               - CANOPEN_RESPONSE_STATUS_OTHER
 *******************************************************************************************************/
CANOPEN_RESPONSE_STATUS CANopen_SdoExpeditedDownload(CANOPEN_SDO *p_srv,
                                                     RTOS_ERR    *p_err)
{
  CPU_INT32U              size;                                 // Local: object entry size in byte
  CPU_INT32U              data;                                 // Local: object entry value
  CPU_INT32U              width = 0;                            // Local: number of data bytes
  CANOPEN_RESPONSE_STATUS result;                               // Local: function result
  CPU_INT08U              cmd;                                  // Local: SDO command byte

  cmd = CANOPEN_FRM_GET_BYTE(p_srv->FrmPtr, 0);                 // extract request command byte
                                                                // see, if size is indicated (s=1)
  if (DEF_BIT_IS_SET(cmd, CANOPEN_SDO_CLIENT_CMD_DATA_SET_SIZE_MSK)) {
    //                                                             calculate number of data bytes
    width = CANOPEN_SDO_XFER_EXPEDITED_MAX_DATA_PAYLOAD - DEF_BIT_FIELD_RD(cmd, CANOPEN_SDO_CLIENT_CMD_EXPEDITED_NBR_BYTES_UNUSED_MSK);
  }

  size = CANopen_SdoObjSizeGet(p_srv, width, p_err);            // check data width acc. object
                                                                // ---------------- EXPEDITED TRANSFER ----------------
  if ((size > 0)
      && (size <= CANOPEN_SDO_XFER_EXPEDITED_MAX_DATA_PAYLOAD)
      && (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)) {        // see, if size is in valid range
    data = CANOPEN_FRM_GET_LONG(p_srv->FrmPtr, 4);              // extract data from CAN frame

    if (!CANOPEN_OBJ_IS_RD_ONLY(p_srv->ObjPtr->Key)) {
      KAL_LockAcquire(p_srv->NodePtr->ObjLock,
                      KAL_OPT_PEND_NONE,
                      p_srv->NodePtr->LockTimeoutMs,
                      p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        RTOS_ERR_CONTEXT_REFRESH(*p_err);
        return (CANOPEN_RESPONSE_STATUS_ABORTED);
      }
    }

    CANopen_ObjValWr(p_srv->NodePtr,
                     p_srv->ObjPtr,                             // write value to object entry
                     (void *)&data,
                     CANOPEN_OBJ_LONG,
                     p_srv->NodePtr->NodeId,
                     p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {           // see, if write detects an error
      CPU_INT32U err_code;

      switch (RTOS_ERR_CODE_GET(*p_err)) {
        case RTOS_ERR_CANOPEN_PARAM_STORE:
        case RTOS_ERR_CANOPEN_PARAM_RESTORE:
          err_code = CANOPEN_SDO_ERR_ACCESS_HW;
          break;

        case RTOS_ERR_CANOPEN_PARAM_INCOMP:
          err_code = CANOPEN_SDO_ERR_PARA_INCOMP;
          break;

        case RTOS_ERR_CANOPEN_SDO_ERR_RANGE:
        case RTOS_ERR_CANOPEN_xPDO_MAP_INVALID_CFG:
          err_code = CANOPEN_SDO_ERR_RANGE;
          break;

        case RTOS_ERR_CANOPEN_PDO_INVALID:
          err_code = CANOPEN_SDO_ERR_OBJ;
          break;

        case RTOS_ERR_CANOPEN_SDO_ERR_SUB_IX:
          err_code = CANOPEN_SDO_ERR_SUB_IX;
          break;

        case RTOS_ERR_CANOPEN_INVALID_ENTRY:
          err_code = CANOPEN_SDO_ERR_TOS_STATE;
          break;

        default:
          if (size < CANOPEN_OBJ_LONG) {
            err_code = CANOPEN_SDO_ERR_LEN_SMALL;
          } else if (size > CANOPEN_OBJ_LONG) {
            err_code = CANOPEN_SDO_ERR_LEN_HIGH;
          } else {
            err_code = CANOPEN_SDO_ERR_TOS_STATE;
          }
          break;
      }

      result = CANOPEN_RESPONSE_STATUS_ABORTED;
      CANopen_SdoAbort(p_srv, err_code);

      CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE, p_srv->NodePtr->CntErrPtr->Sdo.ObjWrFailCtr);
    } else {                                                    // otherwise: write successful
      CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr,                       // scs=3: initiate download response
                           CANOPEN_SDO_SERVER_CMD_RESP_DOWNLOAD_INITIATE_FMTED,
                           0);

      CANOPEN_FRM_SET_LONG(p_srv->FrmPtr, 0, 4);                // clear all 4 data bytes with 0
      p_srv->ObjPtr = DEF_NULL;                                 // indicates: transfer finished
      result = CANOPEN_RESPONSE_STATUS_NO_ERROR;                // indicates: transfer accepted
    }

    if (!CANOPEN_OBJ_IS_RD_ONLY(p_srv->ObjPtr->Key)) {
      RTOS_ERR local_err;

      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      KAL_LockRelease(p_srv->NodePtr->ObjLock,
                      &local_err);
      RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }
  } else {
    result = CANOPEN_RESPONSE_STATUS_ABORTED;
  }

  return (result);                                              // return function result
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_CANOPEN_AVAIL
