/***************************************************************************//**
 * @file
 * @brief USB Device Msc Scsi
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
 *                                                   MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBD_SCSI_PRIV_H_
#define  _USBD_SCSI_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/include/lib_mem.h>

#include  <usb/include/device/usbd_core.h>
#include  <usb/include/device/usbd_msc.h>

#include  <usb/source/device/class/usbd_scsi_storage_priv.h>
#include  <usb/source/device/class/usbd_scsi_types_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------ SCSI MODULE CONFIGURATION -------------
typedef struct usbd_scsi_cfg {
  CPU_INT08U LUN_Qty;                                           // Total quantity of Logical Unit.
} USBD_SCSI_CFG;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void USBD_SCSI_Init(USBD_SCSI_CFG *p_cfg,
                    MEM_SEG       *p_mem_seg,
                    RTOS_ERR      *p_err);

void *USBD_SCSI_LunAdd(CPU_INT08U         lu_nbr,
                       USBD_SCSI_LU_INFO  *p_lu_info,
                       USBD_SCSI_LU_FNCTS *p_lu_fncts,
                       RTOS_ERR           *p_err);

void USBD_SCSI_LunAttach(void     *p_lun_data,
                         CPU_CHAR *media_name,
                         RTOS_ERR *p_err);

void USBD_SCSI_LunDetach(void     *p_lun_data,
                         RTOS_ERR *p_err);

CPU_BOOLEAN USBD_SCSI_CmdProcess(void             *p_lun_data,
                                 const CPU_INT08U *p_cbwcb,
                                 CPU_INT08U       *p_data_buf,
                                 CPU_INT32U       *p_resp_len,
                                 CPU_INT08U       *p_data_dir);

CPU_BOOLEAN USBD_SCSI_DataRd(void       *p_lun_data,
                             CPU_INT08U scsi_cmd,
                             CPU_INT08U *p_data_buf,
                             CPU_INT32U data_len);

CPU_BOOLEAN USBD_SCSI_DataWr(void       *p_lun_data,
                             CPU_INT08U scsi_cmd,
                             void       *p_data_buf,
                             CPU_INT32U data_len);

void USBD_SCSI_Conn(void *p_lun_data);

void USBD_SCSI_Disconn(void *p_lun_data);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/
#endif
