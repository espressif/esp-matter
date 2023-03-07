/***************************************************************************//**
 * @file
 * @brief USB Device - USB Msc Class - Private Header
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

#ifndef  _USBD_MSC_PRIV_H_
#define  _USBD_MSC_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <usb/include/device/usbd_core.h>
#include  <common/source/kal/kal_priv.h>
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/rtos_path.h>
#include  <rtos_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   COMMAND BLOCK WRAPPER DATA TYPE
 *
 * Note(s) : (1) See 'USB Mass Storage Class - Bulk Only Transport', Section 5.1.
 *
 *           (2) The 'Flags' field is a bit-mapped datum with three subfields :
 *
 *               (a) Bit  7  : Data transfer direction :
 *
 *                   (1) 0 = Data-out from host   to device.
 *                   (2) 1 = Data-in  from device to host.
 *
 *               (b) Bit  6  : Obsolete.  Should be set to zero.
 *               (c) Bits 5-0: Reserved.  Should be set to zero.
 *******************************************************************************************************/

typedef struct usbd_msc_cbw {
  CPU_INT32U Signature;                                         // Sig that helps identify this data pkt as CBW.
  CPU_INT32U Tag;                                               // A cmd block tag sent by the host.
  CPU_INT32U DataTransferLength;                                // Nbr of bytes of data that host expects to xfer.
  CPU_INT08U Flags;                                             // Flags (see Notes #2).
  CPU_INT08U LUN;                                               // LUN to which the cmd blk is being sent.
  CPU_INT08U CB_Len;                                            // Length of the CBWB in bytes.
  CPU_INT08U CB[16];                                            // Cmd blk to be executed by the dev.
} USBD_MSC_CBW;

/********************************************************************************************************
 *                                   COMMAND STATUS WRAPPER DATA TYPE
 *
 * Note(s) : (1) See 'USB Mass Storage Class - Bulk Only Transport', Section 5.2.
 *******************************************************************************************************/

typedef struct usbd_msc_csw {
  CPU_INT32U Signature;                                         // Sig that helps identify this data pkt as CSW.
  CPU_INT32U Tag;                                               // Dev shall set this to the value in CBW's Tag.
  CPU_INT32U DataResidue;                                       // Difference between expected & actual nbr data bytes.
  CPU_INT08U Status;                                            // Indicates the success or failure of the cmd.
} USBD_MSC_CSW;

/********************************************************************************************************
 *                                           FORWARD DECLARATIONS
 *******************************************************************************************************/

typedef struct usbd_msc_ctrl USBD_MSC_CTRL;

/********************************************************************************************************
 *                                               MSC STATES
 *******************************************************************************************************/

typedef enum usbd_msc_state {
  USBD_MSC_STATE_NONE = 0,
  USBD_MSC_STATE_INIT,
  USBD_MSC_STATE_CONFIG
} USBD_MSC_STATE;

typedef enum usbd_msc_comm_state {
  USBD_MSC_COMM_STATE_NONE = 0,
  USBD_MSC_COMM_STATE_CBW,
  USBD_MSC_COMM_STATE_DATA,
  USBD_MSC_COMM_STATE_CSW,
  USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_IN_STALL,
  USBD_MSC_COMM_STATE_RESET_RECOVERY_BULK_OUT_STALL,
  USBD_MSC_COMM_STATE_RESET_RECOVERY,
  USBD_MSC_COMM_STATE_BULK_IN_STALL,
  USBD_MSC_COMM_STATE_BULK_OUT_STALL
} USBD_MSC_COMM_STATE;

/********************************************************************************************************
 *                                       MSC EP REQUIREMENTS DATA TYPE
 *******************************************************************************************************/

typedef struct usbd_msc_comm {                                  // ---------- MSC COMMUNICATION INFORMATION -----------
  USBD_MSC_CTRL       *CtrlPtr;                                 // Ptr to ctrl information.
  CPU_INT08U          DataBulkInEpAddr;
  CPU_INT08U          DataBulkOutEpAddr;
  USBD_MSC_COMM_STATE NextCommState;                            // Next comm state of the MSC device.
  USBD_MSC_CBW        CBW;                                      // Cmd blk wrapper (CBW).
  USBD_MSC_CSW        CSW;                                      // Cmd status word (CSW).
  CPU_BOOLEAN         Stall;                                    // Used to stall the endpoints.
  CPU_INT32U          BytesToXfer;                              // Current bytes to xfer during data xfer stage.
  void                *SCSIWrBufPtr;                            // Ptr to the SCSI buf used to wr to SCSI.
  CPU_INT32U          SCSIWrBuflen;                             // SCSI buf len used to wr to SCSI.
} USBD_MSC_COMM;

struct usbd_msc_ctrl {                                          // ------------- MSC CONTROL INFORMATION --------------
  CPU_INT08U     DevNbr;                                        // MSC dev nbr.
  CPU_INT08U     ClassNbr;                                      // MSC class instance nbr.
  USBD_MSC_STATE State;                                         // MSC dev state.
  CPU_INT08U     MaxLun;                                        // Max logical unit number (LUN).
  void           **LunDataPtrTbl;                               // Array of struct pointing to LUN's
  USBD_MSC_COMM  *CommPtr;                                      // MSC comm info ptr.
  CPU_INT08U     *CBW_BufPtr;                                   // Buf to rx Cmd Blk  Wrapper.
  CPU_INT08U     *CSW_BufPtr;                                   // Buf to send Cmd Status Wrapper.
  CPU_INT08U     *DataBufPtr;                                   // Buf to handle data stage.
  CPU_INT08U     *CtrlStatusBufPtr;                             // Buf used for ctrl status xfers.
};

typedef struct usbd_msc {                                       // -------------- MSC CLASS ROOT STRUCT ---------------
  USBD_MSC_CTRL *CtrlTbl;                                       // MSC instance array.
  CPU_INT08U    CtrlNbrNext;

  USBD_MSC_COMM *CommTbl;                                       // MSC comm array.
  CPU_INT16U    CommNbrNext;

#if ((DEF_BIT_IS_SET(RTOS_CFG_ASSERT_DBG_ARG_CHK_EXT_MASK, (RTOS_CFG_MODULE_USBD))) == DEF_ENABLED)
  CPU_INT08U    ClassInstanceQty;                               // Quantity of class instance.
  CPU_INT08U    LUN_Qty;                                        // Quantity of logical unit per class instance.
#endif
} USBD_MSC;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern USBD_MSC *USBD_MSC_Ptr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void USBD_MSC_OS_Init(const USBD_MSC_QTY_CFG *p_qty_cfg,
                      MEM_SEG                *p_mem_seg,
                      RTOS_ERR               *p_err);

void USBD_MSC_OS_TaskCreate(CPU_INT08U    class_nbr,
                            RTOS_TASK_CFG *p_msc_task_cfg,
                            RTOS_ERR      *p_err);

void USBD_MSC_OS_TaskPrioSet(CPU_INT08U     class_nbr,
                             RTOS_TASK_PRIO prio,
                             RTOS_ERR       *p_err);

void USBD_MSC_OS_CommSignalPost(CPU_INT08U class_nbr,
                                RTOS_ERR   *p_err);

void USBD_MSC_OS_CommSignalPend(CPU_INT08U class_nbr,
                                CPU_INT32U timeout,
                                RTOS_ERR   *p_err);

#if 0
void USBD_MSC_OS_CommSignalDel(CPU_INT08U class_nbr);
#endif

void USBD_MSC_OS_EnumSignalPost(CPU_INT08U class_nbr,
                                RTOS_ERR   *p_err);

void USBD_MSC_OS_EnumSignalPend(CPU_INT08U class_nbr,
                                CPU_INT32U timeout,
                                RTOS_ERR   *p_err);

void USBD_MSC_TaskHandler(CPU_INT08U class_nbr);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
