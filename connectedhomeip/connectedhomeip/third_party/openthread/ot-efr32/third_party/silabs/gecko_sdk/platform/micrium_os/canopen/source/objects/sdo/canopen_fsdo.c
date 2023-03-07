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
#include  <common/source/kal/kal_priv.h>
#include  <common/include/rtos_err.h>

#include  <canopen_cfg.h>
#include  <canopen/source/core/canopen_core_priv.h>
#include  <canopen/source/core/canopen_obj_priv.h>
#include  <canopen/source/objects/sdo/canopen_sdo_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (CAN, CANOPEN)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_CANOPEN

/********************************************************************************************************
 ********************************************************************************************************
 *                                     LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (CANOPEN_SDO_DYN_ID_EN == DEF_ENABLED)
static void CANopen_SdoIdWr(CANOPEN_NODE_HANDLE node_handle,
                            CANOPEN_OBJ         *p_obj,
                            void                *p_buf,
                            CPU_INT32U          size,
                            RTOS_ERR            *p_err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                             Specialized object type for SDO identifier
 *
 * @note     (1) This object type specializes the general handling of objects for the object directory
 *               entries holding a SDO identifier. These entries are designed to provide the feature
 *               of changing a SDO identifier.
 *******************************************************************************************************/

#if (CANOPEN_SDO_DYN_ID_EN == DEF_ENABLED)
const CANOPEN_OBJ_TYPE CANopen_ObjTypeSdoId = {
  .SizeCb = DEF_NULL,
  .CtrlCb = DEF_NULL,
  .RdCb = DEF_NULL,
  .WrCb = CANopen_SdoIdWr
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                         INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

#if (CANOPEN_SDO_SEG_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                   CANopen_SdoSegmentedInitUpload()
 *
 * @brief    Generates the response for 'Initiate SDO Upload' protocol.
 *
 * @param    p_srv   Pointer to SDO server object.
 *
 * @param    size    Object size in byte.
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) Entry condition for this function is the SDO request command byte with the
 *               following condition:
 * @verbatim
 *           +---+---+---+---+---+---+---+---+
 *           |     2     | x | x | x | X | x | => condition = "command & 0xE0 must be 0x40"
 *           +---+---+---+---+---+---+---+---+
 *             7   6   5   4   3   2   1   0
 * @endverbatim
 *******************************************************************************************************/
void CANopen_SdoSegmentedInitUpload(CANOPEN_SDO *p_srv,
                                    CPU_INT32U  size,
                                    RTOS_ERR    *p_err)
{
  CPU_INT08U cmd;                                               // Local: SDO command byte

  //                                                               response info: scs=2, n=0, e=0 and s=1
  cmd = CANOPEN_SDO_SERVER_CMD_RESP_UPLOAD_INITIATE << CANOPEN_SDO_CMD_SPECIFIER_BIT_SHIFT;
  DEF_BIT_SET(cmd, CANOPEN_SDO_SERVER_CMD_DATA_SET_SIZE_MSK);
  CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, cmd, 0);                  // response: command byte
  CANOPEN_FRM_SET_LONG(p_srv->FrmPtr, size, 4);                 // object size in byte #4..#7

  p_srv->Seg.XferLen = size;                                    // store size of object entry
  p_srv->Seg.ToggleBit = 0;                                     // first segment should have toggle bit = 0
  p_srv->Seg.Num = 0;                                           // reset number of transfered bytes
  p_srv->Buf.CurPtr = p_srv->Buf.StartPtr;                      // set next free location
  p_srv->Buf.Len = 0;                                           // clear number of bytes in buffer

  if (!CANOPEN_OBJ_IS_RD_ONLY(p_srv->ObjPtr->Key)) {
    KAL_LockAcquire(p_srv->NodePtr->ObjLock,
                    KAL_OPT_PEND_NONE,
                    p_srv->NodePtr->LockTimeoutMs,
                    p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR_CONTEXT_REFRESH(*p_err);
      return;
    }
  }

  CANopen_ObjBufStartRd(p_srv->NodePtr,
                        p_srv->ObjPtr,                          // initialize object read
                        p_srv->Buf.CurPtr,
                        0,
                        p_err);
  CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE, p_srv->NodePtr->CntErrPtr->Sdo.ObjRdFailCtr);

  if (!CANOPEN_OBJ_IS_RD_ONLY(p_srv->ObjPtr->Key)) {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    KAL_LockRelease(p_srv->NodePtr->ObjLock,
                    &local_err);
    RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
}
#endif

#if (CANOPEN_SDO_SEG_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                  CANopen_SdoSegmentedInitDownload()
 *
 * @brief    Generates the response for 'Initiate SDO Download' protocol.
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
 * @note     (1) Entry condition for this function is the SDO request command byte with the
 *               following condition:
 * @verbatim
 *           +---+---+---+---+---+---+---+---+
 *           |     1     | 0 |   n   | 0 | s | => condition = "command & 0xF2 must be 0x20"
 *           +---+---+---+---+---+---+---+---+
 *             7   6   5   4   3   2   1   0
 * @endverbatim
 *******************************************************************************************************/
CANOPEN_RESPONSE_STATUS CANopen_SdoSegmentedInitDownload(CANOPEN_SDO *p_srv,
                                                         RTOS_ERR    *p_err)
{
  CPU_INT32U              size;                                 // Local: size of object entry in bytes
  CPU_INT32U              width = 0;                            // Local: data width in byte
  CANOPEN_RESPONSE_STATUS result = CANOPEN_RESPONSE_STATUS_ABORTED;
  CPU_INT08U              cmd;                                  // Local: SDO command byte

  cmd = CANOPEN_FRM_GET_BYTE(p_srv->FrmPtr, 0);                 // get command specifier
                                                                // see, if data size is indicated (s=1)
  if (DEF_BIT_IS_SET(cmd, CANOPEN_SDO_CLIENT_CMD_DATA_SET_SIZE_MSK)) {
    width = CANOPEN_FRM_GET_LONG(p_srv->FrmPtr, 4);             // yes: get data size information
  }

  size = CANopen_SdoObjSizeGet(p_srv, width, p_err);            // check data width acc. object
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {             // see, if size is valid
    cmd = CANOPEN_SDO_SERVER_CMD_RESP_DOWNLOAD_INITIATE << CANOPEN_SDO_CMD_SPECIFIER_BIT_SHIFT;
    CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, cmd, 0);                // scs=3: initiate download response
    CANOPEN_FRM_SET_LONG(p_srv->FrmPtr, 0, 4);                  // clear all 4 data bytes with 0

    p_srv->Seg.XferLen = size;                                  // store size of object entry
    p_srv->Seg.ToggleBit = 0;                                   // first segment should have toggle bit = 0
    p_srv->Seg.Num = 0;                                         // reset number of transfered bytes

    p_srv->Buf.CurPtr = p_srv->Buf.StartPtr;                    // set next free buffer location
    p_srv->Buf.Len = 0;                                         // clear number of bytes in buffer

    if (!CANOPEN_OBJ_IS_RD_ONLY(p_srv->ObjPtr->Key)) {
      KAL_LockAcquire(p_srv->NodePtr->ObjLock,
                      KAL_OPT_PEND_NONE,
                      p_srv->NodePtr->LockTimeoutMs,
                      p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        RTOS_ERR_CONTEXT_REFRESH(*p_err);
        return (CANOPEN_RESPONSE_STATUS_OTHER);
      }
    }

    CANopen_ObjBufStartWr(p_srv->NodePtr,
                          p_srv->ObjPtr,                        // initialize object write
                          p_srv->Buf.CurPtr,
                          0,
                          p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {           // see, if an error was detected
      result = CANOPEN_RESPONSE_STATUS_NO_ERROR;
    }

    if (!CANOPEN_OBJ_IS_RD_ONLY(p_srv->ObjPtr->Key)) {
      RTOS_ERR local_err;

      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

      KAL_LockRelease(p_srv->NodePtr->ObjLock,
                      &local_err);
      RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }
  }

  return (result);
}
#endif

#if (CANOPEN_SDO_SEG_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                    CANopen_SdoSegmentedDownload()
 *
 * @brief    Generates the response for 'Download SDO Segment' protocol.
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
 * @note     (1) Entry condition for this function is the SDO request command byte with the
 *               following condition:
 * @verbatim
 *           +---+---+---+---+---+---+---+---+
 *           |     0     | t |     n     | c | => condition = "command & 0xE0 must be 0x00"
 *           +---+---+---+---+---+---+---+---+
 *             7   6   5   4   3   2   1   0
 * endverbatim
 *******************************************************************************************************/
CANOPEN_RESPONSE_STATUS CANopen_SdoSegmentedDownload(CANOPEN_SDO *p_srv,
                                                     RTOS_ERR    *p_err)
{
  CPU_INT32U              num;                                       // Local: number of bytes for transfer
  CANOPEN_RESPONSE_STATUS result = CANOPEN_RESPONSE_STATUS_NO_ERROR; // Local: function result
  CPU_INT08U              n;                                         // Local: bitfield 'n' from command byte
  CPU_INT08U              cmd;                                       // Local: SDO command byte
  CPU_INT08U              byte_id;                                   // Local: byte id (loop counter)
  CPU_INT08U              toggle_bit;

  cmd = CANOPEN_FRM_GET_BYTE(p_srv->FrmPtr, 0);                      // extract SDO command byte

  toggle_bit = DEF_BIT_FIELD_RD(cmd, CANOPEN_SDO_CLIENT_CMD_TOGGLE_BIT_MSK);
  if (toggle_bit != p_srv->Seg.ToggleBit) {                          // check for bad toggle bit
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_TBIT);                   // Abort message: toggle bit error
    return (CANOPEN_RESPONSE_STATUS_ABORTED);
  }

  //                                                               get bitfield 'n' from command byte
  n = DEF_BIT_FIELD_RD(cmd, CANOPEN_SDO_CLIENT_CMD_NBR_BYTES_UNUSED_MSK);
  if (n == 0) {                                                 // if no segment size is indicated
    num = p_srv->Seg.XferLen - p_srv->Seg.Num;                  // calculate remaining bytes in object
    if (num > CANOPEN_SDO_XFER_SEGMENTED_MAX_DATA_PAYLOAD) {    // see, if transfer width is > 7
      num = CANOPEN_SDO_XFER_SEGMENTED_MAX_DATA_PAYLOAD;        // yes: limit to 7
    }
  } else {                                                      // otherwise: size is given in command
    num = CANOPEN_SDO_XFER_SEGMENTED_MAX_DATA_PAYLOAD - n;      // set number of bytes to given value
  }

  byte_id = 1;                                                  // set byte id to first data byte
  while (num > 0) {                                             // loop through all valid bytes
                                                                // copy data into transfer buffer
    *(p_srv->Buf.CurPtr) = CANOPEN_FRM_GET_BYTE(p_srv->FrmPtr, byte_id);
    p_srv->Buf.Len++;                                           // increment number of bytes in buffer
    p_srv->Buf.CurPtr++;                                        // move current to next free location
    byte_id++;                                                  // switch to next data byte
    num--;                                                      // decrement remaining valid data bytes
  }
  p_srv->Seg.Num += p_srv->Buf.Len;                             // clear number of transfered bytes

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

  if (DEF_BIT_IS_SET(cmd, CANOPEN_SDO_CLIENT_CMD_MORE_SEG_MSK)) {
    //                                                             if no more segments
    CANopen_ObjBufContWr(p_srv->NodePtr,
                         p_srv->ObjPtr,                         // last write to object entry
                         p_srv->Buf.StartPtr,
                         p_srv->Buf.Len,
                         p_err);

    p_srv->Seg.XferLen = 0;                                     // clear object entry size
    p_srv->Seg.Num = 0;                                         // clear number of transfered bytes
    p_srv->ObjPtr = 0;                                          // indicate finished transfer
  } else {                                                      // otherwise: if segments remain
    CANopen_ObjBufContWr(p_srv->NodePtr,
                         p_srv->ObjPtr,                         // buffered write to object entry
                         p_srv->Buf.StartPtr,
                         p_srv->Buf.Len,
                         p_err);

    p_srv->Buf.CurPtr = p_srv->Buf.StartPtr;                    // set next free buffer location
    p_srv->Buf.Len = 0;                                         // clear number of bytes in buffer
  }

  if (!CANOPEN_OBJ_IS_RD_ONLY(p_srv->ObjPtr->Key)) {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    KAL_LockRelease(p_srv->NodePtr->ObjLock,
                    &local_err);
    RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {             // see, if an error was detected
    CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_TOS);               // Abort 'Data cannot be transfered'
    result = CANOPEN_RESPONSE_STATUS_ABORTED;
  }

  if (result == 0) {                                            // see, if no error is detected
    cmd = CANOPEN_SDO_SERVER_CMD_RESP_DOWNLOAD_SEG << CANOPEN_SDO_CMD_SPECIFIER_BIT_SHIFT;
    DEF_BIT_FIELD_WR(cmd, p_srv->Seg.ToggleBit, (CPU_INT08U)CANOPEN_SDO_CLIENT_CMD_TOGGLE_BIT_MSK);

    CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, cmd, 0);                // set response, scs=1, t=toggle bit
    CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, 0, 1);
    CANOPEN_FRM_SET_WORD(p_srv->FrmPtr, 0, 2);
    CANOPEN_FRM_SET_LONG(p_srv->FrmPtr, 0, 4);
  }

  p_srv->Seg.ToggleBit ^= 0x01;                                 // toggle the toggle bit

  return (result);
}
#endif

#if (CANOPEN_SDO_SEG_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                     CANopen_SdoSegmentedUpload()
 *
 * @brief    Generates the response for the 'Upload SDO Segment' protocol.
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
 * @note     (1) Entry condition for this function is the SDO request command byte with the
 *               following condition:
 * @verbatim
 *           +---+---+---+---+---+---+---+---+
 *           |     3     | t |       0       | => condition = "command & 0xEF must be 0x60"
 *           +---+---+---+---+---+---+---+---+
 *             7   6   5   4   3   2   1   0
 * @endverbatim
 *******************************************************************************************************/
CANOPEN_RESPONSE_STATUS CANopen_SdoSegmentedUpload(CANOPEN_SDO *p_srv,
                                                   RTOS_ERR    *p_err)
{
  CPU_INT32U              width;                                // Local: object width in byte
  CPU_INT08U              cmd;                                  // Local: SDO command byte
  CPU_INT08U              i;                                    // Local: counter variable
  CPU_INT08U              toggle_bit;
  CPU_INT08U              c_bit = 0;                            // Local: indicator if last segment
  CANOPEN_RESPONSE_STATUS result = CANOPEN_RESPONSE_STATUS_NO_ERROR;

  if (p_srv->ObjPtr == DEF_NULL) {                              // if segmented transfer not started
    RTOS_ERR_SET(*p_err, RTOS_ERR_NULL_PTR);
    CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_CMD);               // Abort message: command error
    return (CANOPEN_RESPONSE_STATUS_ABORTED);
  }

  cmd = CANOPEN_FRM_GET_BYTE(p_srv->FrmPtr, 0);                 // get command byte

  toggle_bit = DEF_BIT_FIELD_RD(cmd, CANOPEN_SDO_CLIENT_CMD_TOGGLE_BIT_MSK);
  if (toggle_bit != p_srv->Seg.ToggleBit) {                     // check for bad toggle bit
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_TBIT);              // Abort message: toggle bit error
    return (CANOPEN_RESPONSE_STATUS_ABORTED);
  }

  width = p_srv->Seg.XferLen - p_srv->Seg.Num;                  // calculate remaining bytes
  if (width > CANOPEN_SDO_XFER_SEGMENTED_MAX_DATA_PAYLOAD) {    // if remaining data is longer than 7 bytes
    width = CANOPEN_SDO_XFER_SEGMENTED_MAX_DATA_PAYLOAD;        // use full segment size
  } else {                                                      // otherwise:
    c_bit = 1;                                                  // set c-bit, last segment to be transfered
  }

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

  CANopen_ObjBufContRd(p_srv->NodePtr,
                       p_srv->ObjPtr,                           // read data into transfer buffer
                       p_srv->Buf.StartPtr,
                       width,
                       p_err);
  if (!CANOPEN_OBJ_IS_RD_ONLY(p_srv->ObjPtr->Key)) {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    KAL_LockRelease(p_srv->NodePtr->ObjLock,
                    &local_err);
    RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_TOS);               // Abort 'Data cannot be transfered'
    return (CANOPEN_RESPONSE_STATUS_ABORTED);
  }

  p_srv->Buf.CurPtr = p_srv->Buf.StartPtr;
  for (i = 0; i < (CPU_INT08U)width; i++) {
    CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, *(p_srv->Buf.CurPtr), 1 + i);
    p_srv->Buf.CurPtr++;
  }
  //                                                               clear rest of can frame
  for (i = (CPU_INT08U)(width + 1); i <= CANOPEN_SDO_XFER_SEGMENTED_MAX_DATA_PAYLOAD; i++) {
    CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, 0, 1 + i);
  }

  //                                                               response info: scs = 0,
  cmd = CANOPEN_SDO_SERVER_CMD_RESP_UPLOAD_SEG << CANOPEN_SDO_CMD_SPECIFIER_BIT_SHIFT;
  DEF_BIT_FIELD_WR(cmd,                                         // t = togglebit,
                   (CPU_INT08U)p_srv->Seg.ToggleBit,
                   (CPU_INT08U)CANOPEN_SDO_SERVER_CMD_TOGGLE_BIT_MSK);
  DEF_BIT_FIELD_WR(cmd,                                         // n = 7 - width
                   (CPU_INT08U)(CANOPEN_SDO_XFER_SEGMENTED_MAX_DATA_PAYLOAD - width),
                   (CPU_INT08U)CANOPEN_SDO_SERVER_CMD_NBR_BYTES_UNUSED_MSK);
  cmd |= c_bit;                                                 // and c = c-bit
  CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, cmd, 0);

  if (c_bit == 1) {                                             // if last segment
    p_srv->Seg.XferLen = 0;
    p_srv->Seg.Num = 0;
    p_srv->Seg.ToggleBit = 0;
    p_srv->ObjPtr = DEF_NULL;                                   // indicate finished transfer
  } else {                                                      // otherwise: segments remaining
    p_srv->Seg.Num += width;                                    // update number of transmitted bytes
    p_srv->Seg.ToggleBit ^= 0x01;                               // toggle the toggle bit
  }

  return (result);
}
#endif

#if (CANOPEN_SDO_BLK_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                     CANopen_SdoBlkInitDownload()
 *
 * @brief    Generates the response for 'Initiate SDO Block Download' protocol.
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
 * @note     (1) Entry condition for this function is the SDO request command byte with the
 *               following condition:
 * @verbatim
 *           +---+---+---+---+---+---+---+---+
 *           |     6     |   0   | cc| s | 0 | => condition = "command & 0xF9 must be 0xC0"
 *           +---+---+---+---+---+---+---+---+
 *             7   6   5   4   3   2   1   0
 * @endverbatim
 *******************************************************************************************************/
CANOPEN_RESPONSE_STATUS CANopen_SdoBlkInitDownload(CANOPEN_SDO *p_srv,
                                                   RTOS_ERR    *p_err)
{
  CPU_INT32U              size;                                     // Local: XferLen object entry
  CPU_INT32U              width = 0;                                // Local: data width in byte
  CANOPEN_RESPONSE_STATUS result = CANOPEN_RESPONSE_STATUS_ABORTED; // Local: function result
  CPU_INT08U              cmd;                                      // Local: SDO command byte

  cmd = CANOPEN_FRM_GET_BYTE(p_srv->FrmPtr, 0);                     // get command specifier
  if ((cmd & 0x02) != 0) {                                          // if s = 1, size indicated
    width = CANOPEN_FRM_GET_LONG(p_srv->FrmPtr, 4);                 // get size information from CAN frame
  }

  size = CANopen_SdoObjSizeGet(p_srv, width, p_err);                // check data width acc. object
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {                 // see, if an error is detected
    CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_TOS);                   // Abort message: Data cannot be transfered
    return (CANOPEN_RESPONSE_STATUS_ABORTED);                       // abort with error indication
  }

  if (width <= size) {
    if (!CANOPEN_OBJ_IS_RD_ONLY(p_srv->ObjPtr->Key)) {
      KAL_LockAcquire(p_srv->NodePtr->ObjLock,
                      KAL_OPT_PEND_NONE,
                      p_srv->NodePtr->LockTimeoutMs,
                      p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        RTOS_ERR_CONTEXT_REFRESH(*p_err);
        result = CANOPEN_RESPONSE_STATUS_OTHER;
        return (result);
      }
    }

    //                                                             see, if size is valid
    CANopen_ObjBufStartWr(p_srv->NodePtr,
                          p_srv->ObjPtr,                        // initialize object write
                          p_srv->Buf.CurPtr,
                          0,
                          p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      result = CANOPEN_RESPONSE_STATUS_NO_ERROR;                // clear local error
    }

    if (!CANOPEN_OBJ_IS_RD_ONLY(p_srv->ObjPtr->Key)) {
      RTOS_ERR local_err;

      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      KAL_LockRelease(p_srv->NodePtr->ObjLock,
                      &local_err);
      RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      CANOPEN_CTR_ERR_INC(p_srv->NodePtr->CntErrPtr->Sdo.ObjWrFailCtr);

      CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_TOS);
      return (CANOPEN_RESPONSE_STATUS_ABORTED);
    }

    p_srv->Blk.SegCnt = 0;                                      // clear segment counter
    p_srv->Blk.State = SDO_BLK_STATE_DOWNLOAD;                  // set block transfer active
    p_srv->Blk.SegNumMax = CANOPEN_SDO_MAX_SEG_QTY;             // set number of segments per blocks

    CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, 0xA0, 0);               // scs=5,sc=0,ss=0: initiate download
                                                                // response, no checksum supported
    CANOPEN_FRM_SET_LONG(p_srv->FrmPtr,                         // set max. number of segments per block
                         (CPU_INT32U)CANOPEN_SDO_MAX_SEG_QTY,
                         4);
  } else {                                                      // otherwise: size is invalid
    CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_LEN_HIGH);          // Abort 'datatype length too high'
  }

  return (result);
}
#endif

#if (CANOPEN_SDO_BLK_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                      CANopen_SdoBlkEndDownload()
 *
 * @brief    generates the response for 'End SDO Block Download' protocol.
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
 * @note     (1) Entry condition for this function is the SDO request command byte with the
 *               following condition:
 * @verbatim
 *           +---+---+---+---+---+---+---+---+
 *           |     6     |     n     | 0 | 1 | => condition = "command & 0xE3 must be 0xC1"
 *           +---+---+---+---+---+---+---+---+
 *             7   6   5   4   3   2   1   0
 * @endverbatim
 *******************************************************************************************************/
CANOPEN_RESPONSE_STATUS CANopen_SdoBlkEndDownload(CANOPEN_SDO *p_srv,
                                                  RTOS_ERR    *p_err)
{
  CANOPEN_RESPONSE_STATUS result = CANOPEN_RESPONSE_STATUS_ABORTED; // Local: function result
  CPU_INT08U              cmd;                                      // Local: SDO command byte
  CPU_INT08U              n;                                        // Local: bit field 'n' in command byte

  cmd = CANOPEN_FRM_GET_BYTE(p_srv->FrmPtr, 0);                     // get command byte
  if ((cmd & 0x01) != 0) {
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

    //                                                             if cs=1, end block download
    n = (cmd & 0x1C) >> 2;                                      // get n out of command byte
    CANopen_ObjBufContWr(p_srv->NodePtr,
                         p_srv->ObjPtr,                         // last write to object entry
                         p_srv->Buf.StartPtr,
                         ((CPU_INT32U)p_srv->Buf.Len - n),
                         p_err);

    if (!CANOPEN_OBJ_IS_RD_ONLY(p_srv->ObjPtr->Key)) {
      RTOS_ERR local_err;

      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      KAL_LockRelease(p_srv->NodePtr->ObjLock,
                      &local_err);
      RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {           // see, if an error is detected
      CANOPEN_CTR_ERR_INC(p_srv->NodePtr->CntErrPtr->Sdo.ObjWrFailCtr);

      CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_TOS);
      return (CANOPEN_RESPONSE_STATUS_ABORTED);
    }

    CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, 0xA1, 0);               // scs=5,x=0,ss=1: end download
    CANOPEN_FRM_SET_WORD(p_srv->FrmPtr, 0, 1);                  // clear data
    CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, 0, 3);
    CANOPEN_FRM_SET_LONG(p_srv->FrmPtr, 0, 4);

    p_srv->Blk.State = SDO_BLK_STATE_IDLE;                      // set block transfer to idle
    p_srv->Buf.CurPtr = p_srv->Buf.StartPtr;                    // reset transfer buffer working location
    p_srv->Buf.Len = 0;                                         // clear number of bytes in transfer buffer
    p_srv->ObjPtr = DEF_NULL;                                   // clear object entry reference
    result = CANOPEN_RESPONSE_STATUS_NO_ERROR;                  // indicate success
  }

  return (result);
}
#endif

#if (CANOPEN_SDO_BLK_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                       CANopen_SdoBlkDownload()
 *
 * @brief    Generates the response for 'Download SDO Block Segment' protocol.
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
 * @note     (1) Entry condition for this function is activation of the block download state. The
 *               SDO request command byte contains the state after this CAN frame:
 * @verbatim
 *           +---+---+---+---+---+---+---+---+
 *           | c |      0 < seqno < 128      | => c=0: more segments to be downloaded
 *           +---+---+---+---+---+---+---+---+    C=1: enter END SDO block download phase
 *             7   6   5   4   3   2   1   0
 * @endverbatim
 *******************************************************************************************************/
CANOPEN_RESPONSE_STATUS CANopen_SdoBlkDownload(CANOPEN_SDO *p_srv,
                                               RTOS_ERR    *p_err)
{
  CANOPEN_RESPONSE_STATUS result = CANOPEN_RESPONSE_STATUS_OTHER; // Local: function result
  CPU_INT08U              cmd;                                    // Local: sequence number
  CPU_INT08U              i;                                      // Local: counter

  cmd = CANOPEN_FRM_GET_BYTE(p_srv->FrmPtr, 0);                   // get sequence counter
  if ((cmd & 0x7F) == (p_srv->Blk.SegCnt + 1)) {                  // if segment counter fits segment number
                                                                  // copy data to transfer buffer
    for (i = 0; i < CANOPEN_SDO_XFER_SEGMENTED_MAX_DATA_PAYLOAD; i++) {
      *(p_srv->Buf.CurPtr) = CANOPEN_FRM_GET_BYTE(p_srv->FrmPtr, 1 + i);
      p_srv->Buf.CurPtr++;
      p_srv->Buf.Len++;
    }
    p_srv->Blk.SegCnt++;                                               // increment segment counter
    if ( (p_srv->Blk.SegCnt == CANOPEN_SDO_MAX_SEG_QTY)                // if max. segment of block transfered or
         || ((cmd & 0x80) != 0)) {                                     // if last segment of last block indicated
      CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, 0xA2, 0);                    // scs=5; ss=2
      CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, p_srv->Blk.SegCnt, 1);       // set ackseq for response
      CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, CANOPEN_SDO_MAX_SEG_QTY, 2); // set blksize for response
      //                                                           clear rest of frame
      for (i = 3; i <= CANOPEN_SDO_XFER_SEGMENTED_MAX_DATA_PAYLOAD; i++) {
        CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, 0, i);
      }
      p_srv->Blk.SegCnt = 0;                                    // reset segment counter
      p_srv->Blk.State = SDO_BLK_STATE_DOWNLOAD_WAIT;           // set to waiting state
      result = CANOPEN_RESPONSE_STATUS_NO_ERROR;                // indicate block confirmation
    }

    if (result == CANOPEN_RESPONSE_STATUS_NO_ERROR) {           // if last segment of block
      if ((cmd & 0x80) == 0) {
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

        //                                                         yes, if not last segment of last block
        CANopen_ObjBufContWr(p_srv->NodePtr,
                             p_srv->ObjPtr,                     // write transfer buffer to domain
                             p_srv->Buf.StartPtr,
                             (CPU_INT32U)p_srv->Buf.Len,
                             p_err);

        result = CANOPEN_RESPONSE_STATUS_ABORTED;

        if (!CANOPEN_OBJ_IS_RD_ONLY(p_srv->ObjPtr->Key)) {
          RTOS_ERR local_err;

          RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
          KAL_LockRelease(p_srv->NodePtr->ObjLock,
                          &local_err);
          RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
        }

        p_srv->Buf.CurPtr = p_srv->Buf.StartPtr;
        p_srv->Buf.Len = 0;
      }
    }
  } else {                                                              // otherwise: sequence count mismatch
    if ((p_srv->Blk.SegCnt & 0x80) == 0) {                              // if first error
      p_srv->Blk.SegCnt |= 0x80;                                        // indicate appeared error
    }

    if ( ((cmd & 0x7F) == CANOPEN_SDO_MAX_SEG_QTY)                      // if max. segment of block transfered or
         || ((cmd & 0x80) != 0) ) {                                     // if last segment of last block indicated
      CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, 0xA2, 0);                     // scs=5; ss=2
      CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, p_srv->Blk.SegCnt & 0x7F, 1); // ackseq with last successful seg.
      CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, CANOPEN_SDO_MAX_SEG_QTY, 2);  // set blksize for response
      CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, 0, 3);                        // clear rest of message
      CANOPEN_FRM_SET_LONG(p_srv->FrmPtr, 0, 4);

      p_srv->Blk.SegCnt = 0;                                            // reset segment counter
      p_srv->Buf.CurPtr = p_srv->Buf.StartPtr;                          // reset transfer buffer
      p_srv->Buf.Len = 0;
      result = CANOPEN_RESPONSE_STATUS_NO_ERROR;                        // indicate block confirmation
    }
  }

  return (result);
}
#endif

#if (CANOPEN_SDO_BLK_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                      CANopen_SdoBlkInitUpload()
 *
 * @brief    Generates the response for 'Initiate SDO Block Upload' protocol,
 *           sub-command 'initiate upload request'.
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
 * @note     (1) Entry condition for this function is the SDO request command byte with the
 *               following condition:
 * @verbatim
 *           +---+---+---+---+---+---+---+---+
 *           |     5     | 0 | cc| 0 |   0   | => condition = "command & 0xE7 must be 0xA0"
 *           +---+---+---+---+---+---+---+---+
 *             7   6   5   4   3   2   1   0
 * @endverbatim
 *
 * @note     TODO: Implement the PST to dynamically switch to segmented or expedited transfer if
 *           number of segments is below the given threshold.
 *******************************************************************************************************/
CANOPEN_RESPONSE_STATUS CANopen_SdoBlkInitUpload(CANOPEN_SDO *p_srv,
                                                 RTOS_ERR    *p_err)
{
  CPU_INT32U              size;                                      // Local: size of object entry
  CANOPEN_RESPONSE_STATUS result = CANOPEN_RESPONSE_STATUS_NO_ERROR; // Local: function result
  CPU_INT08U              cmd;                                       // Local: SDO command byte

  CANopen_SdoGetObject(p_srv, CANOPEN_SDO_RD, p_err);                // get addressed object entry
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {                  // see, if object entry is not existing
    return (CANOPEN_RESPONSE_STATUS_ABORTED);                        // yes: stop processing
  }

  p_srv->Blk.XferLen = CANopen_SdoObjSizeGet(p_srv, 0, p_err);       // get size of object entry
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {                  // see, if an error is detected
    return (CANOPEN_RESPONSE_STATUS_ABORTED);                        // yes: stop processing
  }

  p_srv->Blk.SegNumMax = CANOPEN_FRM_GET_BYTE(p_srv->FrmPtr, 4);     // get max. number of segments per block

  if ( (p_srv->Blk.SegNumMax < 0x01)                                 // if invalid number of segments 1..0x7F
       || (p_srv->Blk.SegNumMax > 0x7F) ) {
    CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_BLK_SIZE);               // Abort 'invalid block size'
    CANopen_SdoAbortReq(p_srv);                                      // perform abort of SDO transfer
    return (CANOPEN_RESPONSE_STATUS_ABORTED);                        // send response
  } else {                                                           // otherwise: blksize is in valid range
    if (p_srv->Blk.SegNumMax > CANOPEN_SDO_MAX_SEG_QTY) {            // see, if blksize is > transfer buffer
      p_srv->Blk.SegNumMax = CANOPEN_SDO_MAX_SEG_QTY;                // yes: limit to transfer buffer
    }
  }

  size = p_srv->Blk.XferLen;                                         // setup response
  cmd = 0xC2;                                                        // scs=6,sc=0,ss=0: initiate download
                                                                     // response, no checksum supported
  p_srv->Blk.LastValid = 0xFF;                                       // set last valid bytes invalid
  p_srv->Blk.Len = p_srv->Blk.XferLen;                               // set remaining bytes to transfer

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

  CANopen_ObjBufStartRd(p_srv->NodePtr,
                        p_srv->ObjPtr,                          // initialize object read
                        p_srv->Buf.CurPtr,
                        0,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    result = CANOPEN_RESPONSE_STATUS_ABORTED;
  }

  if (!CANOPEN_OBJ_IS_RD_ONLY(p_srv->ObjPtr->Key)) {
    RTOS_ERR local_err;

    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
    KAL_LockRelease(p_srv->NodePtr->ObjLock,
                    &local_err);
    RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, cmd, 0);                  // set SDO response command byte
  CANOPEN_FRM_SET_LONG(p_srv->FrmPtr, size, 4);                 // indicate object entry size in bytes

  return (result);                                              // return function result
}
#endif

#if (CANOPEN_SDO_BLK_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                        CANopen_SdoBlkUpload()
 *
 * @brief    Generates the response for 'Initiate SDO Block Upload' protocol',
 *           sub-command 'start upload'.
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
 * @note     (1) Entry condition for this function is the SDO request command byte with the
 *               following condition, or the already started block upload state:
 * @verbatim
 *           +---+---+---+---+---+---+---+---+
 *           |     5     |     0     |   3   | => condition = "command must be 0xA3"
 *           +---+---+---+---+---+---+---+---+
 *             7   6   5   4   3   2   1   0
 * @endverbatim
 *******************************************************************************************************/
CANOPEN_RESPONSE_STATUS CANopen_SdoBlkUpload(CANOPEN_SDO *p_srv,
                                             RTOS_ERR    *p_err)
{
  CPU_INT32U size;                                              // Local: remaining object entry size
  CPU_INT08U finished = 0;                                      // Local: end of object indication
  CPU_INT08U seg;                                               // Local: segment counter
  CPU_INT08U len;                                               // Local: used bytes in frame
  CPU_INT08U i;                                                 // Local: loop counter
  RTOS_ERR   local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_srv->Buf.CurPtr = p_srv->Buf.StartPtr;                      // clr reading location in transfer buffer
                                                                // calculate number of bytes in buffer
  p_srv->Buf.Len = CANOPEN_SDO_XFER_SEGMENTED_MAX_DATA_PAYLOAD * p_srv->Blk.SegNumMax;
  if (p_srv->Blk.State == SDO_BLK_STATE_REPEAT) {               // see, if loaded block must be repeated
    p_srv->Blk.Len += p_srv->Buf.Len;                           // yes: add block again to remaining bytes
  } else {                                                      // otherwise: load new block into buffer
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

    if (p_srv->Blk.XferLen > p_srv->Buf.Len) {                  // see, if domain > transfer buffer
      p_srv->Blk.XferLen -= p_srv->Buf.Len;                     // update remaining object size
    } else {                                                    // otherwise: domain <= transfer buffer
      p_srv->Buf.Len = p_srv->Blk.XferLen;                      // set reading to object entry size
      p_srv->Blk.XferLen = 0;                                   // set remaining object size to 0
    }

    CANopen_ObjBufContRd(p_srv->NodePtr,
                         p_srv->ObjPtr,                         // fill transfer buffer for reading
                         p_srv->Buf.StartPtr,
                         (CPU_INT32U)p_srv->Buf.Len,
                         p_err);

    if (!CANOPEN_OBJ_IS_RD_ONLY(p_srv->ObjPtr->Key)) {
      KAL_LockRelease(p_srv->NodePtr->ObjLock,
                      &local_err);
      RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_TOS);
      return (CANOPEN_RESPONSE_STATUS_ABORTED);
    }
  }
  p_srv->Blk.State = SDO_BLK_STATE_UPLOAD;                      // set block transfer active
  p_srv->Blk.SegCnt = 1;                                        // restart segments in transmission buffer
  while ( (p_srv->Blk.SegCnt <= p_srv->Blk.SegNumMax)           // loop for number of segments
          && (finished == 0) ) {                                // or end of object entry reached
    seg = p_srv->Blk.SegCnt;                                    // set segment counter
    size = p_srv->Blk.Len;                                      // calculate remaining bytes in object
    if (size > CANOPEN_SDO_XFER_SEGMENTED_MAX_DATA_PAYLOAD) {   // if more than 7 bytes remain
                                                                // set actual data length
      len = CANOPEN_SDO_XFER_SEGMENTED_MAX_DATA_PAYLOAD;
      p_srv->Blk.Len -= CANOPEN_SDO_XFER_SEGMENTED_MAX_DATA_PAYLOAD;
      if (p_srv->Blk.SegCnt < p_srv->Blk.SegNumMax) {           // see, if buffer end is not reached
        p_srv->Blk.SegCnt++;                                    // yes: inc. number of transmitted segments
      } else {                                                  // otherwise: end of block reached
        finished = 1;                                           // indicate last segment in block
      }
    } else {                                                    // otherwise: last segment of last block
      len = (CPU_INT08U)size;                                   // set length
      p_srv->Blk.Len = 0;
      finished = 1;                                             // end on next loop
    }
    if (finished == 1) {                                        // see, if this is the last segment
      if (p_srv->Blk.Len == 0) {                                // yes: see, if this is the last block
        seg |= 0x80;                                            // yes: c=1, last segment in last block
      }
      p_srv->Blk.LastValid = len;                               // set valid bytes of last segment in blk
    }

    CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, seg, 0);                // set sequence byte
    for (i = 0; i < len; i++) {                                 // copy data from transfer buffer to frame
      CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr,                       // copy bytes from transfer buffer to frame
                           *(p_srv->Buf.CurPtr),
                           1 + i);
      p_srv->Buf.CurPtr++;                                      // update working read pointer in buffer
      p_srv->Buf.Len--;                                         // decrement remaining bytes in buffer
    }
    //                                                             clear rest of can frame
    for (i = (CPU_INT08U)len; i < CANOPEN_SDO_XFER_SEGMENTED_MAX_DATA_PAYLOAD; i++) {
      CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, 0, 1 + i);
    }

    CANopen_IfSend(&p_srv->NodePtr->If, p_srv->FrmPtr, &local_err); // send SDO response at once
    //                                                             possible error registered in node
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {            // if error in can message
      return (CANOPEN_RESPONSE_STATUS_OTHER);                       // set allowed objects to none
    }
  }

  return (CANOPEN_RESPONSE_STATUS_OTHER);
}
#endif

#if (CANOPEN_SDO_BLK_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                       CANopen_SdoBlkAckUpload()
 *
 * @brief    Generates the response for 'Upload SDO Block Segment' protocol.
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
 * @note     (1) Entry condition for this function is the SDO request command byte with the
 *               following condition:
 * @verbatim
 *           +---+---+---+---+---+---+---+---+
 *           |     5     | x | x | x |   2   | => condition = "command & 0xE3 must be 0xA2"
 *           +---+---+---+---+---+---+---+---+
 *             7   6   5   4   3   2   1   0
 * @endverbatim
 *******************************************************************************************************/
CANOPEN_RESPONSE_STATUS CANopen_SdoBlkAckUpload(CANOPEN_SDO *p_srv,
                                                RTOS_ERR    *p_err)
{
  CANOPEN_RESPONSE_STATUS result = CANOPEN_RESPONSE_STATUS_OTHER;
  CPU_INT08U              cmd;                                  // Local: SDO command byte
  CPU_INT08U              seq;                                  // Local: sequence counter
  CPU_INT08U              val;                                  // Local: last valid segment
  CPU_INT08U              i;                                    // Local: sequence counter

  seq = CANOPEN_FRM_GET_BYTE(p_srv->FrmPtr, 1);
  if (seq > 0x7F) {                                             // if sequence number out of range
    CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_SEQ_NUM);           // Abort message: invalid sequence number
    CANopen_SdoAbortReq(p_srv);                                 // perform abort of SDO transfer
    return (CANOPEN_RESPONSE_STATUS_ABORTED);                   // send response
  } else if (seq != p_srv->Blk.SegCnt) {                        // if frames not receive correctly
    p_srv->Blk.State = SDO_BLK_STATE_REPEAT;                    // repeat block transfer
    result = CANopen_SdoBlkUpload(p_srv, p_err);                // re-send this block
  } else if (p_srv->Blk.Len == 0) {                             // if last segment of last block
                                                                // if last valid bytes are valid
    if (p_srv->Blk.LastValid <= CANOPEN_SDO_XFER_SEGMENTED_MAX_DATA_PAYLOAD) {
      val = (CPU_INT08U)p_srv->Blk.LastValid;                   // get last valid received segments
      cmd = (CPU_INT08U)0xC0                                    // scs=6, ss=1, n=number of unused bytes
            | (CPU_INT08U)((CANOPEN_SDO_XFER_SEGMENTED_MAX_DATA_PAYLOAD - val) << 2)
            | (CPU_INT08U)0x01;
      CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, cmd, 0);
      //                                                           clear remaining bytes
      for (i = 1; i <= CANOPEN_SDO_XFER_SEGMENTED_MAX_DATA_PAYLOAD; i++) {
        CANOPEN_FRM_SET_BYTE(p_srv->FrmPtr, 0, i);
      }
      result = CANOPEN_RESPONSE_STATUS_NO_ERROR;                   // send response
    }
  } else {                                                         // otherwise: continue with next block
    p_srv->Blk.SegNumMax = CANOPEN_FRM_GET_BYTE(p_srv->FrmPtr, 2); // set number of segments for next block
    if ( (p_srv->Blk.SegNumMax < 0x01)                             // if invalid number of segments 1..0x7F
         || (p_srv->Blk.SegNumMax > 0x7F) ) {
      CANopen_SdoAbort(p_srv, CANOPEN_SDO_ERR_BLK_SIZE);           // Abort 'invalid block size'
      CANopen_SdoAbortReq(p_srv);                                  // perform abort of SDO transfer
      return (CANOPEN_RESPONSE_STATUS_ABORTED);                    // send response
    } else {                                                       // otherwise: blksize is in valid range
      if (p_srv->Blk.SegNumMax > CANOPEN_SDO_MAX_SEG_QTY) {        // see, if blksize is > transfer buffer
        p_srv->Blk.SegNumMax = CANOPEN_SDO_MAX_SEG_QTY;            // yes: limit to transfer buffer
      }
    }
    result = CANopen_SdoBlkUpload(p_srv, p_err);                   // send next block
  }

  return (result);
}
#endif

#if (CANOPEN_SDO_BLK_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                       CANopen_SdoBlkEndUpload()
 *
 * @brief    Generates the response for 'End SDO Block Upload' protocol.
 *
 * @param    p_srv   Pointer to SDO server object.
 *
 * @return   Response status:
 *               - CANOPEN_RESPONSE_STATUS_NO_ERROR
 *               - CANOPEN_RESPONSE_STATUS_ABORTED
 *               - CANOPEN_RESPONSE_STATUS_OTHER
 *
 * @note     (1) Entry condition for this function is the SDO request command byte with the
 *               following condition:
 * @verbatim
 *           +---+---+---+---+---+---+---+---+
 *           |     5     |     0     |   1   | => condition = "command must be 0xA1"
 *           +---+---+---+---+---+---+---+---+
 *             7   6   5   4   3   2   1   0
 * @endverbatim
 *******************************************************************************************************/
CANOPEN_RESPONSE_STATUS CANopen_SdoBlkEndUpload(CANOPEN_SDO *p_srv)
{
  p_srv->Blk.State = SDO_BLK_STATE_IDLE;                        // set block transfer to idle
  p_srv->ObjPtr = DEF_NULL;                                     // clear object entry reference

  return (CANOPEN_RESPONSE_STATUS_OTHER);
}
#endif

#if ((CANOPEN_SDO_BLK_EN == DEF_ENABLED) || (CANOPEN_SDO_SEG_EN == DEF_ENABLED))
/****************************************************************************************************//**
 *                                         CANopen_SdoAbortReq()
 *
 * @brief    Requests to abort an SDO transfer.
 *
 * @param    p_srv   Pointer to SDO server object
 *******************************************************************************************************/
void CANopen_SdoAbortReq(CANOPEN_SDO *p_srv)
{
#if ((CANOPEN_SDO_BLK_EN == DEF_ENABLED) || (CANOPEN_SDO_SEG_EN == DEF_ENABLED))
  p_srv->ObjPtr = DEF_NULL;
  p_srv->Idx = 0;
  p_srv->Sub = 0;
  p_srv->Buf.CurPtr = p_srv->Buf.StartPtr;                      // reset transfer buffer working location
  p_srv->Buf.Len = 0;                                           // clear number of bytes in transfer buffer
#endif
#if (CANOPEN_SDO_BLK_EN == DEF_ENABLED)
  p_srv->Blk.State = SDO_BLK_STATE_IDLE;                        // set block transfer to idle
#endif
#if (CANOPEN_SDO_SEG_EN == DEF_ENABLED)
  p_srv->Seg.Num = 0;
  p_srv->Seg.XferLen = 0;
  p_srv->Seg.ToggleBit = 0;
#endif
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

#if (CANOPEN_SDO_DYN_ID_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                           CANopen_SdoIdWr()
 *
 * @brief    Writes a value to a SDO ID.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_obj           Pointer to addressed object entry.
 *
 * @param    p_buf           Pointer to data buffer.
 *
 * @param    size            Length of data buffer.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void CANopen_SdoIdWr(CANOPEN_NODE_HANDLE node_handle,
                            CANOPEN_OBJ         *p_obj,
                            void                *p_buf,
                            CPU_INT32U          size,
                            RTOS_ERR            *p_err)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CPU_INT32U   newval;                                          // Local: write value
  CPU_INT32U   curval;                                          // Local: current CAN identifier value
  CPU_INT08U   nbr;                                             // Local: number of SDO server

  PP_UNUSED_PARAM(size);

  RTOS_ASSERT_DBG_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  newval = *(CPU_INT32U *)p_buf;                                // get write value
  CANopen_ObjDirectRd(p_obj, &curval, CANOPEN_OBJ_LONG, p_err); // get current value (existence is checked)
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    CANOPEN_CTR_ERR_INC(p_node->CntErrPtr->Sdo.ObjRdFailCtr);
    return;
  }

  //                                                               get number of SDO server
  nbr = CANOPEN_OBJ_GET_IX(p_obj->Key) & CANOPEN_COMM_PROFILE_OBJ_IX_SDO_SERVER_PARAM_RANGE;

  if ((curval & CANOPEN_SDO_COB_ID_VALID) == 0) {               // see, if write is limited to disable ID
    if ((newval & CANOPEN_SDO_COB_ID_VALID) != 0) {             // yes: see, if write access disables ID
      CANopen_ObjDirectWr(p_obj,
                          &newval,                              // yes: set write value in object entry
                          CANOPEN_OBJ_LONG,
                          p_err);
      if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {         // see, if write access was successful
        CANopen_SdoReset(p_node->Sdo, nbr, p_node);             // for safety: reset addressed SDO server
      }
    } else {                                                    // otherwise: write is not allowed
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      return;                                                   // abort write operation
    }
  } else {                                                      // otherwise: current COBID is disabled
    CANopen_ObjDirectWr(p_obj,
                        &newval,
                        CANOPEN_OBJ_LONG,
                        p_err);                                 // write new value to COBID
  }

  CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE, p_node->CntErrPtr->Sdo.ObjWrFailCtr);

  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {             // see, if no error is detected
    CANopen_SdoEn(p_node->Sdo, nbr, p_err);                     // update SDO server status
    CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE, p_node->CntErrPtr->Sdo.EnFailCtr);
  }
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_CANOPEN_AVAIL
