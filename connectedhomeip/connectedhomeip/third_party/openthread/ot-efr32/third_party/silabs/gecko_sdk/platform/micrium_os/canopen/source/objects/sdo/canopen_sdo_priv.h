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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef _CANOPEN_SDO_PRIV_H_
#define _CANOPEN_SDO_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>

#include  <canopen_cfg.h>
#include  <canopen/include/canopen_if.h>
#include  <canopen/include/canopen_obj.h>
#include  <canopen/source/core/canopen_core_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  CANOPEN_SDO_RD                       1                 // Object read access
#define  CANOPEN_SDO_WR                       2                 // Object write access

#define  CANOPEN_SDO_XFER_EXPEDITED_MAX_DATA_PAYLOAD                4u
#define  CANOPEN_SDO_XFER_SEGMENTED_MAX_DATA_PAYLOAD                7u

#define  CANOPEN_SDO_COB_ID_VALID    DEF_BIT_31                 // Disabled SDO server / COBID

/********************************************************************************************************
 *                                SDO CLIENT/SERVER REQUESTS/RESPONSES
 *
 * Note(s) : (1) More details about SDO client/server requests/responses in section 7.2.4.3 of
 *               specification CiA-301 "CANopen application layer and communication profile, Version:
 *               4.2.0, 21 February 2011"
 *******************************************************************************************************/

#define  CANOPEN_SDO_CMD_SPECIFIER_MSK                              (DEF_BIT_07 | DEF_BIT_06 | DEF_BIT_05)
#define  CANOPEN_SDO_CMD_SPECIFIER_BIT_SHIFT                        (5u)
//                                                                 ---------------- DOWNLOAD INITIATE -----------------
#define  CANOPEN_SDO_CLIENT_CMD_EXPEDITED_NBR_BYTES_UNUSED_MSK      (DEF_BIT_03 | DEF_BIT_02)
#define  CANOPEN_SDO_CLIENT_CMD_XFER_TYPE_MSK                       (DEF_BIT_01)
#define  CANOPEN_SDO_CLIENT_CMD_DATA_SET_SIZE_MSK                   (DEF_BIT_00)
//                                                                 ----------------- DOWNLOAD SEGMENT -----------------
#define  CANOPEN_SDO_CLIENT_CMD_TOGGLE_BIT_MSK                      (DEF_BIT_04)
#define  CANOPEN_SDO_CLIENT_CMD_NBR_BYTES_UNUSED_MSK                (DEF_BIT_03 | DEF_BIT_02 | DEF_BIT_01)
#define  CANOPEN_SDO_CLIENT_CMD_MORE_SEG_MSK                        (DEF_BIT_00)
//                                                                 ----------------- UPLOAD INITIATE ------------------
#define  CANOPEN_SDO_SERVER_CMD_EXPEDITED_NBR_BYTES_UNUSED_MSK      (DEF_BIT_03 | DEF_BIT_02)
#define  CANOPEN_SDO_SERVER_CMD_XFER_TYPE_MSK                       (DEF_BIT_01)
#define  CANOPEN_SDO_SERVER_CMD_DATA_SET_SIZE_MSK                   (DEF_BIT_00)
//                                                                 ------------------ UPLOAD SEGMENT ------------------
#define  CANOPEN_SDO_SERVER_CMD_TOGGLE_BIT_MSK                      (DEF_BIT_04)
#define  CANOPEN_SDO_SERVER_CMD_NBR_BYTES_UNUSED_MSK                (DEF_BIT_03 | DEF_BIT_02 | DEF_BIT_01)
#define  CANOPEN_SDO_SERVER_CMD_MORE_SEG_MSK                        (DEF_BIT_00)
#define  CANOPEN_SDO_CLIENT_CMD_RESERVED_BITS_MSK                   (DEF_BIT_03 | DEF_BIT_02 | DEF_BIT_01 | DEF_BIT_00)
#define  CANOPEN_SDO_CLIENT_CMD_REQ_UPLOAD_SEG_MSK_NO_TOGGLE        (CANOPEN_SDO_CMD_SPECIFIER_MSK \
                                                                     | CANOPEN_SDO_CLIENT_CMD_RESERVED_BITS_MSK)

#define  CANOPEN_SDO_CLIENT_CMD_XFER_TYPE_NORMAL                    0u
#define  CANOPEN_SDO_CLIENT_CMD_XFER_TYPE_EXPEDITED                 1u
#define  CANOPEN_SDO_CLIENT_CMD_DATA_SET_SIZE_NOT_INDICATED         0u
#define  CANOPEN_SDO_CLIENT_CMD_DATA_SET_SIZE_INDICATED             1u
#define  CANOPEN_SDO_CLIENT_CMD_MORE_SEG                            0u
#define  CANOPEN_SDO_CLIENT_CMD_NO_MORE_SEG                         1u

#define  CANOPEN_SDO_SERVER_CMD_XFER_TYPE_NORMAL                    0u
#define  CANOPEN_SDO_SERVER_CMD_XFER_TYPE_EXPEDITED                 1u
#define  CANOPEN_SDO_SERVER_CMD_DATA_SET_SIZE_NOT_INDICATED         0u
#define  CANOPEN_SDO_SERVER_CMD_DATA_SET_SIZE_INDICATED             1u
#define  CANOPEN_SDO_SERVER_CMD_MORE_SEG                            0u
#define  CANOPEN_SDO_SERVER_CMD_NO_MORE_SEG                         1u

#define  CANOPEN_SDO_CLIENT_CMD_REQ_DOWNLOAD_SEG                    0u
#define  CANOPEN_SDO_CLIENT_CMD_REQ_DOWNLOAD_INITIATE               1u
#define  CANOPEN_SDO_CLIENT_CMD_REQ_UPLOAD_INITIATE                 2u
#define  CANOPEN_SDO_CLIENT_CMD_REQ_UPLOAD_SEG                      3u
#define  CANOPEN_SDO_CLIENT_CMD_REQ_BLK_UPLOAD                      5u
#define  CANOPEN_SDO_CLIENT_CMD_REQ_BLK_DOWNLOAD                    6u
#define  CANOPEN_SDO_CLIENT_SUBCMD_REQ_BLK_UPLOAD_INITIATE          0u
#define  CANOPEN_SDO_CLIENT_SUBCMD_REQ_BLK_DOWNLOAD_END             1u
#define  CANOPEN_SDO_CLIENT_SUBCMD_RESP_BLK_UPLOAD_END              1u
#define  CANOPEN_SDO_CLIENT_SUBCMD_RESP_BLK_UPLOAD                  2u
#define  CANOPEN_SDO_CLIENT_SUBCMD_REQ_BLK_UPLOAD_START             3u

#define  CANOPEN_SDO_SERVER_CMD_RESP_UPLOAD_SEG                     0u
#define  CANOPEN_SDO_SERVER_CMD_RESP_DOWNLOAD_SEG                   1u
#define  CANOPEN_SDO_SERVER_CMD_RESP_UPLOAD_INITIATE                2u
#define  CANOPEN_SDO_SERVER_CMD_RESP_DOWNLOAD_INITIATE              3u
#define  CANOPEN_SDO_SERVER_CMD_RESP_BLK_DOWNLOAD                   5u
#define  CANOPEN_SDO_SERVER_CMD_RESP_BLK_UPLOAD                     6u
#define  CANOPEN_SDO_SERVER_SUBCMD_RESP_BLK_UPLOAD_INITIATE         0u
#define  CANOPEN_SDO_SERVER_SUBCMD_RESP_BLK_DOWNLOAD_END            1u
#define  CANOPEN_SDO_SERVER_SUBCMD_REQ_BLK_UPLOAD_END               1u
#define  CANOPEN_SDO_SERVER_SUBCMD_RESP_BLK_DOWNLOAD_SUBBLK         2u

#define  CANOPEN_SDO_CMD_ABORT_XFER_REQ                             4u

#define  CANOPEN_SDO_CMD_ABORT_XFER_REQ_FMTED                   (CANOPEN_SDO_CMD_ABORT_XFER_REQ << CANOPEN_SDO_CMD_SPECIFIER_BIT_SHIFT)
#define  CANOPEN_SDO_CLIENT_CMD_REQ_UPLOAD_INITIATE_FMTED       (CANOPEN_SDO_CLIENT_CMD_REQ_UPLOAD_INITIATE << CANOPEN_SDO_CMD_SPECIFIER_BIT_SHIFT)
#define  CANOPEN_SDO_SERVER_CMD_RESP_DOWNLOAD_INITIATE_FMTED    (CANOPEN_SDO_SERVER_CMD_RESP_DOWNLOAD_INITIATE << CANOPEN_SDO_CMD_SPECIFIER_BIT_SHIFT)
#define  CANOPEN_SDO_CLIENT_CMD_REQ_UPLOAD_SEG_FMTED            (CANOPEN_SDO_CLIENT_CMD_REQ_UPLOAD_SEG << CANOPEN_SDO_CMD_SPECIFIER_BIT_SHIFT)

/********************************************************************************************************
 *                                     SDO PROTOCOL ERROR CODES
 *******************************************************************************************************/

#define  CANOPEN_SDO_ERR_TBIT               0x05030000          ///< Toggle bit not alternated
#define  CANOPEN_SDO_ERR_CMD                0x05040001          ///< SDO command specifier invalid/unknown
#define  CANOPEN_SDO_ERR_BLK_SIZE           0x05040002          ///< Invalid block size
#define  CANOPEN_SDO_ERR_SEQ_NUM            0x05040003          ///< Invalid Sequence number
#define  CANOPEN_SDO_ERR_RD                 0x06010001          ///< Attempt to read a write only object
#define  CANOPEN_SDO_ERR_WR                 0x06010002          ///< Attempt to write a read only object
#define  CANOPEN_SDO_ERR_OBJ                0x06020000          ///< Object doesn't exist in directory
#define  CANOPEN_SDO_ERR_ACCESS_HW          0x06060000          ///< Access failed due to an hardware error
#define  CANOPEN_SDO_ERR_LEN_HIGH           0x06070012          ///< Length of parameter too high
#define  CANOPEN_SDO_ERR_LEN_SMALL          0x06070013          ///< Length of parameter too small
#define  CANOPEN_SDO_ERR_SUB_IX             0x06090011          ///< Sub-index doesn't exist in directory
#define  CANOPEN_SDO_ERR_RANGE              0x06090030          ///< Value range of parameter exceeded
#define  CANOPEN_SDO_ERR_TOS                0x08000020          ///< Data can't be transfered or stored
#define  CANOPEN_SDO_ERR_TOS_STATE          0x08000022          ///< Data can't be transfered or stored because of the present device state
#define  CANOPEN_SDO_ERR_GEN_ERR            0x08000000          ///< General error
#define  CANOPEN_SDO_ERR_OBJ_MAP            0x06040041          ///< Object cannot be mapped to the PDO
#define  CANOPEN_SDO_ERR_OBJ_MAP_N          0x06040042          ///< Number and length exceed PDO
#define  CANOPEN_SDO_ERR_PARA_INCOMP        0x06040043          ///< parameter incompatibility reason

/********************************************************************************************************
 ********************************************************************************************************
 *                                              DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ********************************************************************************************************
 *                                    CANopen response statuses
 *******************************************************************************************************/

typedef enum canopen_response_status {
  CANOPEN_RESPONSE_STATUS_NO_ERROR,
  CANOPEN_RESPONSE_STATUS_ABORTED,
  CANOPEN_RESPONSE_STATUS_OTHER
} CANOPEN_RESPONSE_STATUS;

/*
 ********************************************************************************************************
 *                                    SDO transfer buffer structure
 *******************************************************************************************************/

typedef struct canopen_sdo_buf {
  CPU_INT32U Len;                                               ///< Number of bytes in transfer buffer.
  CPU_INT08U *StartPtr;                                         ///< Pointer to start of transfer buffer.
  CPU_INT08U *CurPtr;                                           ///< Pointer to next free buffer location.
} CANOPEN_SDO_BUF;

/*
 ********************************************************************************************************
 *                                Segmented SDO transfer data structure
 *******************************************************************************************************/

typedef struct canopen_sdo_seg {
  CPU_INT32U XferLen;                                           ///< Transfer length of object entry
  CPU_INT32U Num;                                               ///< Number of transfered bytes
  CPU_INT08U ToggleBit;                                         ///< Segment toggle bit
} CANOPEN_SDO_SEG;

/*
 ********************************************************************************************************
 *                                      SDO block transfer states
 *******************************************************************************************************/

typedef enum canopen_sdo_blk_state {
  SDO_BLK_STATE_IDLE,                                           ///< no block transfer ongoing
  SDO_BLK_STATE_DOWNLOAD,                                       ///< block download active
  SDO_BLK_STATE_UPLOAD,                                         ///< block upload active
  SDO_BLK_STATE_REPEAT,                                         ///< block upload repeat request active
  SDO_BLK_STATE_DOWNLOAD_WAIT                                   ///< block download wait for next block/end
} CANOPEN_SDO_BLK_STATE;

/*
 ********************************************************************************************************
 *                                  SDO block transfer data structure
 *******************************************************************************************************/

typedef struct canopen_sdo_blk {
  CANOPEN_SDO_BLK_STATE State;                                  ///< block transfer state
  CPU_INT32U            Size;                                   ///< Size of object entry
  CPU_INT32U            Len;                                    ///< remaining block length in byte
  CPU_INT08U            SegNumMax;                              ///< number of segments in block
  CPU_INT08U            SegCnt;                                 ///< current segment number
  CPU_INT08U            LastValid;                              ///< number of valid bytes in last segment
} CANOPEN_SDO_BLK;

/*
 ********************************************************************************************************
 *                                        SDO server structure
 *******************************************************************************************************/

typedef struct canopen_sdo {
  CANOPEN_NODE    *NodePtr;                                     ///< Pointer to node info structure
  CANOPEN_OBJ     *ObjPtr;                                      ///< Addressed Object Entry Reference
  CANOPEN_IF_FRM  *FrmPtr;                                      ///< SDO request/response CAN frame
  CPU_INT32U      RxId;                                         ///< SDO request CAN identifier
  CPU_INT32U      TxId;                                         ///< SDO response CAN identifier
  CPU_INT16U      Idx;                                          ///< Extracted Multiplexer Index
  CPU_INT08U      Sub;                                          ///< Extracted Multiplexer Subindex
#if ((CANOPEN_SDO_SEG_EN == DEF_ENABLED) \
  || (CANOPEN_SDO_BLK_EN == DEF_ENABLED))
  CANOPEN_SDO_BUF Buf;                                          ///< Transfer buffer management structure
#endif
#if (CANOPEN_SDO_SEG_EN == DEF_ENABLED)
  CANOPEN_SDO_SEG Seg;                                          ///< Segmented transfer control structure
#endif
#if (CANOPEN_SDO_BLK_EN == DEF_ENABLED)
  CANOPEN_SDO_BLK Blk;                                          ///< Block transfer control structure
#endif
} CANOPEN_SDO;

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void CANopen_SdoInit(CANOPEN_SDO  *p_srv,
                     CANOPEN_NODE *p_node,
                     RTOS_ERR     *p_err);

void CANopen_SdoReset(CANOPEN_SDO  *p_srv,
                      CPU_INT08U   nbr,
                      CANOPEN_NODE *p_node);

void CANopen_SdoEn(CANOPEN_SDO *p_srv,
                   CPU_INT08U  nbr,
                   RTOS_ERR    *p_err);

CANOPEN_SDO *CANopen_SdoCheck(CANOPEN_SDO    *p_srv,
                              CANOPEN_IF_FRM *p_frm);

CANOPEN_RESPONSE_STATUS CANopen_SdoResp(CANOPEN_SDO *p_srv,
                                        RTOS_ERR    *p_err);

void CANopen_SdoAbortReq(CANOPEN_SDO *p_srv);

void CANopen_SdoAbort(CANOPEN_SDO *p_srv,
                      CPU_INT32U  err);

void CANopen_SdoObjGet(CANOPEN_SDO *p_srv,
                       CPU_INT16U  mode,
                       RTOS_ERR    *p_err);

CPU_INT32U CANopen_SdoObjSizeGet(CANOPEN_SDO *p_srv,
                                 CPU_INT32U  width,
                                 RTOS_ERR    *p_err);

CANOPEN_RESPONSE_STATUS CANopen_SdoExpeditedUpload(CANOPEN_SDO *p_srv,
                                                   RTOS_ERR    *p_err);

CANOPEN_RESPONSE_STATUS CANopen_SdoExpeditedDownload(CANOPEN_SDO *p_srv,
                                                     RTOS_ERR    *p_err);

#if (CANOPEN_SDO_SEG_EN == DEF_ENABLED)
void CANopen_SdoSegmentedInitUpload(CANOPEN_SDO *p_srv,
                                    CPU_INT32U  width,
                                    RTOS_ERR    *p_err);

CANOPEN_RESPONSE_STATUS CANopen_SdoSegmentedInitDownload(CANOPEN_SDO *p_srv,
                                                         RTOS_ERR    *p_err);

CANOPEN_RESPONSE_STATUS CANopen_SdoSegmentedDownload(CANOPEN_SDO *p_srv,
                                                     RTOS_ERR    *p_err);

CANOPEN_RESPONSE_STATUS CANopen_SdoSegmentedUpload(CANOPEN_SDO *p_srv,
                                                   RTOS_ERR    *p_err);
#endif

#if (CANOPEN_SDO_BLK_EN == DEF_ENABLED)
CANOPEN_RESPONSE_STATUS CANopen_SdoBlkInitDownload(CANOPEN_SDO *p_srv,
                                                   RTOS_ERR    *p_err);

CANOPEN_RESPONSE_STATUS CANopen_SdoBlkEndDownload(CANOPEN_SDO *p_srv,
                                                  RTOS_ERR    *p_err);

CANOPEN_RESPONSE_STATUS CANopen_SdoBlkDownload(CANOPEN_SDO *p_srv,
                                               RTOS_ERR    *p_err);

CANOPEN_RESPONSE_STATUS CANopen_SdoBlkInitUpload(CANOPEN_SDO *p_srv,
                                                 RTOS_ERR    *p_err);

CANOPEN_RESPONSE_STATUS CANopen_SdoBlkUpload(CANOPEN_SDO *p_srv,
                                             RTOS_ERR    *p_err);

CANOPEN_RESPONSE_STATUS CANopen_SdoBlkAckUpload(CANOPEN_SDO *p_srv,
                                                RTOS_ERR    *p_err);

CANOPEN_RESPONSE_STATUS CANopen_SdoBlkEndUpload(CANOPEN_SDO *p_srv);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _CANOPEN_SDO_PRIV_H_
