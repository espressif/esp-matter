/***************************************************************************//**
 * @file
 * @brief USB Host Device Declarations
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

#ifndef  _USBH_CORE_DEV_PRIV_H_
#define  _USBH_CORE_DEV_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>

#include  <usb/include/host/usbh_core_handle.h>

#include  <usb/source/host/core/usbh_core_types_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void USBH_DevInit(CPU_INT16U dev_qty,
                  RTOS_ERR   *p_err);

USBH_DEV *USBH_DevConn(USBH_DEV_SPD  dev_spd,
                       USBH_HOST     *p_host,
                       USBH_HC       *p_hc,
                       USBH_HUB_FNCT *p_hub_fnct,
                       CPU_INT08U    port_nbr);

void USBH_DevDisconn(USBH_DEV_HANDLE dev_handle);

void USBH_DevSuspend(USBH_DEV_HANDLE dev_handle);

void USBH_DevResume(USBH_DEV_HANDLE dev_handle);

void USBH_DevAddrSet(USBH_DEV *p_dev,
                     RTOS_ERR *p_err);

void USBH_DevConfigSetProcess(USBH_DEV_HANDLE dev_handle,
                              CPU_INT08U      cfg_nbr,
                              RTOS_ERR        *p_err);

CPU_INT08U USBH_DevStrRd(USBH_DEV   *p_dev,
                         CPU_INT08U str_ix,
                         void       *p_buf,
                         CPU_INT08U buf_len,
                         RTOS_ERR   *p_err);

CPU_INT16U USBH_DevDescRdLocked(USBH_DEV   *p_dev,
                                CPU_INT08U recipient,
                                CPU_INT08U type,
                                CPU_INT08U desc_type,
                                CPU_INT08U desc_ix,
                                CPU_INT16U desc_len_req,
                                CPU_INT08U *p_desc_buf,
                                CPU_INT16U desc_buf_len,
                                RTOS_ERR   *p_err);

USBH_DEV *USBH_DevGet(USBH_DEV_HANDLE dev_handle);

USBH_DEV *USBH_DevAcquireShared(USBH_DEV_HANDLE dev_handle,
                                RTOS_ERR        *p_err);

USBH_DEV *USBH_DevAcquireCompleting(USBH_DEV_HANDLE dev_handle,
                                    RTOS_ERR        *p_err);

void USBH_DevRelShared(USBH_DEV *p_dev);

CPU_BOOLEAN USBH_DevConnAcceptNotify(void);

void USBH_DevConnFailNotify(CPU_INT08U hub_addr,
                            CPU_INT08U port_nbr,
                            RTOS_ERR   err);

#if 0
void USBH_DevResumeNotify(USBH_DEV_HANDLE dev_handle);
#endif

void USBH_DevDisconnNotify(USBH_DEV_HANDLE dev_handle);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
