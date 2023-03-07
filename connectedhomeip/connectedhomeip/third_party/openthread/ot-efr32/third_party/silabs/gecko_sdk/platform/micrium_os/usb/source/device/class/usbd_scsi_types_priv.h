/***************************************************************************//**
 * @file
 * @brief USB Device Scsi - Types
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

#ifndef  _USBD_SCSI_TYPES_PRIV_H_
#define  _USBD_SCSI_TYPES_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_path.h>
#include  <common/source/kal/kal_priv.h>

#include  <usbd_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (USBD_SCSI_CFG_64_BIT_LBA_EN == DEF_ENABLED)
typedef CPU_INT64U USBD_SCSI_LBA;
#else
typedef CPU_INT32U USBD_SCSI_LBA;
#endif

#if (USBD_SCSI_CFG_64_BIT_LBA_EN == DEF_ENABLED)
typedef CPU_INT64U USBD_SCSI_LB_QTY;
#else
typedef CPU_INT32U USBD_SCSI_LB_QTY;
#endif

//                                                                 ---------------- SCSI MEDIUM STATUS ----------------
typedef enum usbd_scsi_medium_status {
  USBD_SCSI_MEDIUM_STATUS_CONNECTED,                            // Medium present and ready.
  USBD_SCSI_MEDIUM_STATUS_NOT_CONNECTED,                        // Medium not present.
  USBD_SCSI_MEDIUM_STATUS_RECONNECTED                           // Medium present, but is a new one.
} USBD_SCSI_MEDIUM_STATUS;

/********************************************************************************************************
 *                                           STORAGE UNIT CONTROL
 *******************************************************************************************************/

typedef struct usbd_scsi_storage_lun {
  const CPU_CHAR        *VendorIdStr;                           // Dev vendor info.
  const CPU_CHAR        *ProdIdStr;                             // Dev prod ID.
  CPU_INT32U            ProdRevisionLevel;                      // Revision level of product.
  CPU_BOOLEAN           IsRdOnly;                               // Wr protected or not.
  CPU_BOOLEAN           IsRemovable;                            // Flag that indicates if logical unit is removeable.

  CPU_INT08U            LU_Nbr;                                 // Logical unit nbr.
  USBD_SCSI_LU_FNCTS    *LU_FnctsPtr;                           // Pointer to logical unit callback functions structure.

  USBD_SCSI_STORAGE_API *SCSI_StorageApiPtr;                    // Pointer to SCSI storage driver API.
  void                  *DataPtr;                               // Pointer to storage driver's data.

  USBD_SCSI_LB_QTY      NbrBlocks;                              // Quantity of blocks.
  CPU_INT32U            BlockSize;                              // Size of blocks.

  USBD_SCSI_LBA         LBAddr;                                 // Logical Blk Addr.
  CPU_INT32U            LBCnt;                                  // Nbr of mem blks.
  CPU_INT08U            SenseKey;                               // Sense key describing an err or exception cond.
  CPU_INT08U            ASC;                                    // Additional Sense Code describing sense key in detail.

  KAL_LOCK_HANDLE       StateLock;                              // Lock protecting state flags.
  CPU_BOOLEAN           OpenedFlag;                             // Flag indicating logical unit locked or not.
  CPU_BOOLEAN           PresentFlag;
} USBD_SCSI_STORAGE_LUN;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
