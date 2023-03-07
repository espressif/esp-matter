/***************************************************************************//**
 * @file
 * @brief USB Device - USB Hid Class Operating System Layer
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

#ifndef  _USBD_HID_OS_PRIV_H_
#define  _USBD_HID_OS_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <usb/include/device/usbd_core.h>
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void USBD_HID_OS_Init(MEM_SEG          *p_mem_seg,
                      USBD_HID_QTY_CFG *p_cfg,
                      CPU_INT32U       stk_size_elements,
                      void             *p_stk,
                      RTOS_ERR         *p_err);

void USBD_HID_OS_TmrTaskPrioSet(RTOS_TASK_PRIO prio,
                                RTOS_ERR       *p_err);

void USBD_HID_OS_InputLock(CPU_INT08U class_nbr,
                           RTOS_ERR   *p_err);

void USBD_HID_OS_InputUnlock(CPU_INT08U class_nbr);

void USBD_HID_OS_InputDataPend(CPU_INT08U class_nbr,
                               CPU_INT16U timeout_ms,
                               RTOS_ERR   *p_err);

void USBD_HID_OS_InputDataPendAbort(CPU_INT08U class_nbr);

void USBD_HID_OS_InputDataPost(CPU_INT08U class_nbr);

void USBD_HID_OS_OutputLock(CPU_INT08U class_nbr,
                            RTOS_ERR   *p_err);

void USBD_HID_OS_OutputUnlock(CPU_INT08U class_nbr);

void USBD_HID_OS_OutputDataPendAbort(CPU_INT08U class_nbr);

void USBD_HID_OS_OutputDataPend(CPU_INT08U class_nbr,
                                CPU_INT16U timeout_ms,
                                RTOS_ERR   *p_err);

void USBD_HID_OS_OutputDataPost(CPU_INT08U class_nbr);

void USBD_HID_OS_TxLock(CPU_INT08U class_nbr,
                        RTOS_ERR   *p_err);

void USBD_HID_OS_TxUnlock(CPU_INT08U class_nbr);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
