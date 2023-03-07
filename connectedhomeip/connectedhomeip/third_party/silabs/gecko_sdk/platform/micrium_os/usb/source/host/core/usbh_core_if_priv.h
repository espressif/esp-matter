/***************************************************************************//**
 * @file
 * @brief USB Host Interface Declarations
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

#ifndef  _USBH_CORE_IF_PRIV_H_
#define  _USBH_CORE_IF_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/source/host/core/usbh_core_types_priv.h>

#include  <usb/include/host/usbh_core_handle.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void USBH_IF_Init(CPU_INT16U if_qty,
                  RTOS_ERR   *p_err);

USBH_IF *USBH_IF_Create(CPU_INT08U *p_desc_buf,
                        RTOS_ERR   *p_err);

void USBH_IF_Del(USBH_IF *p_if);

void USBH_IF_EP_Add(USBH_IF  *p_if,
                    USBH_EP  *p_ep,
                    RTOS_ERR *p_err);

USBH_IF *USBH_IF_PtrGet(USBH_CONFIG      *p_config,
                        USBH_FNCT_HANDLE fnct_handle,
                        CPU_INT08U       if_ix,
                        RTOS_ERR         *p_err);

USBH_IF *USBH_IF_PtrAtIxGet(USBH_FNCT  *p_fnct,
                            CPU_INT08U if_ix);

#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
CPU_INT08U USBH_IF_AltSetProcess(USBH_DEV_HANDLE  dev_handle,
                                 USBH_FNCT_HANDLE fnct_handle,
                                 CPU_INT08U       if_ix,
                                 CPU_INT08U       if_alt_ix,
                                 RTOS_ERR         *p_err);
#endif

void USBH_IF_AltEP_Open(USBH_DEV   *p_dev,
                        USBH_FNCT  *p_fnct,
                        CPU_INT08U if_alt_ix,
                        RTOS_ERR   *p_err);

void USBH_IF_AltEP_Close(USBH_DEV   *p_dev,
                         USBH_FNCT  *p_fnct,
                         CPU_INT08U if_alt_ix,
                         RTOS_ERR   *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
