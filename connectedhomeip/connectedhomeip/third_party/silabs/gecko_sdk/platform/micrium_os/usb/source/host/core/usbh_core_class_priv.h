/***************************************************************************//**
 * @file
 * @brief USB Host Class Operations
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

#ifndef  _USBH_CORE_CLASS_PRIV_H_
#define  _USBH_CORE_CLASS_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>

#include  <usb/source/host/core/usbh_core_types_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

CPU_BOOLEAN USBH_ClassProbeDev(USBH_DEV *p_dev,
                               RTOS_ERR *p_err);

CPU_BOOLEAN USBH_ClassProbeDevFnct(USBH_DEV *p_dev);

CPU_BOOLEAN USBH_ClassProbeFnct(USBH_DEV_HANDLE dev_handle,
                                USBH_CONFIG     *p_config);

CPU_BOOLEAN USBH_ClassEP_Open(USBH_DEV *p_dev);

void USBH_ClassEP_Close(USBH_DEV *p_dev);

void USBH_ClassConn(USBH_CONFIG *p_config);

void USBH_ClassDisconn(USBH_CONFIG *p_config);

void USBH_ClassIF_DfltAltSet(USBH_CONFIG *p_config);

void USBH_ClassSuspend(USBH_CONFIG *p_config);

void USBH_ClassResume(USBH_CONFIG *p_config);

#if (USBH_CFG_UNINIT_EN == DEF_ENABLED)
void USBH_ClassesUnInit(void);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
