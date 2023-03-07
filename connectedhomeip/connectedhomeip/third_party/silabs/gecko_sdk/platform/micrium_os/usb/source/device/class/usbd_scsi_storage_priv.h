/***************************************************************************//**
 * @file
 * @brief USB Device Msc Class Storage Driver - Ramdisk
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

#ifndef  _USBD_SCSI_STORAGE_PRIV_H_
#define  _USBD_SCSI_STORAGE_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <usb/include/device/usbd_core.h>
#include  <usb/include/device/usbd_msc.h>
#include  <usb/source/device/class/usbd_scsi_types_priv.h>
#include  <common/include/rtos_err.h>
#include  <common/include/lib_mem.h>
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------------- STORAGE API --------------------
struct usbd_scsi_storage_api {                                  // Initialize storage driver.
  void (*Init)       (MEM_SEG               *p_mem_seg,
                      USBD_SCSI_STORAGE_LUN *p_storage_lun,
                      RTOS_ERR              *p_err);

  //                                                               Request exclusive access to media.
  void (*Open)       (USBD_SCSI_STORAGE_LUN *p_storage_lun,
                      CPU_CHAR              *media_name,
                      RTOS_ERR              *p_err);

  //                                                               Release exclusive access to media.
  void (*Close)      (USBD_SCSI_STORAGE_LUN *p_storage_lun,
                      RTOS_ERR              *p_err);

  //                                                               Retrieve storage media capacity.
  CPU_BOOLEAN (*CapacityGet)(USBD_SCSI_STORAGE_LUN *p_storage_lun,
                             USBD_SCSI_LB_QTY      *p_nbr_blks,
                             CPU_INT32U            *p_blk_size);

  //                                                               Read data from storage media.
  CPU_BOOLEAN (*Rd)         (USBD_SCSI_STORAGE_LUN *p_storage_lun,
                             USBD_SCSI_LBA         blk_addr,
                             CPU_INT32U            nbr_blks,
                             CPU_INT08U            *p_data_buf);

  //                                                               Write data to storage media.
  CPU_BOOLEAN (*Wr)         (USBD_SCSI_STORAGE_LUN *p_storage_lun,
                             USBD_SCSI_LBA         blk_addr,
                             CPU_INT32U            nbr_blks,
                             CPU_INT08U            *p_data_buf);
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
