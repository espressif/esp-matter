/***************************************************************************//**
 * @file
 * @brief USB Host Interface Operations
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

/****************************************************************************************************//**
 * @addtogroup USBH_CORE
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBH_CORE_IF_H_
#define  _USBH_CORE_IF_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_core_utils.h>
#include  <usb/include/host/usbh_core_opt_def.h>
#include  <usb/include/host/usbh_core_handle.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  USBH_IF_ALT_CUR                                 255u

/********************************************************************************************************
 *                                           DESCRIPTOR TYPES
 *
 * Note(s) : (1) See 'Universal Serial Bus Specification Revision 2.0', Section 9.4, Table 9-5, and
 *               Section 9.4.3.
 *
 *           (2) For a 'get descriptor' setup request, the low byte of the 'wValue' field may contain
 *               one of these values.
 *******************************************************************************************************/

#define  USBH_DESC_TYPE_IF                                 4u
#define  USBH_DESC_TYPE_IF_PWR                             8u
#define  USBH_DESC_TYPE_IF_ASSOCIATION                    11u

#define  USBH_DESC_LEN_IF                                  9u
#define  USBH_DESC_LEN_IF_ASSOCIATION                      8u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
typedef void (*USBH_IF_ALT_SET_CMPL) (USBH_DEV_HANDLE dev_handle,
                                      USBH_DEV_HANDLE fnct_handle,
                                      CPU_INT08U      if_ix,
                                      CPU_INT08U      if_alt_ix,
                                      CPU_INT08U      if_alt_ix_prev,
                                      void            *p_arg,
                                      RTOS_ERR        err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

CPU_INT08U USBH_IF_NbrGet(USBH_DEV_HANDLE  dev_handle,
                          USBH_FNCT_HANDLE fnct_handle,
                          CPU_INT08U       if_ix,
                          RTOS_ERR         *p_err);

#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
CPU_INT08U USBH_IF_AltNbrGet(USBH_DEV_HANDLE  dev_handle,
                             USBH_FNCT_HANDLE fnct_handle,
                             CPU_INT08U       if_ix,
                             RTOS_ERR         *p_err);

CPU_INT08U USBH_IF_AltIxCurGet(USBH_DEV_HANDLE  dev_handle,
                               USBH_FNCT_HANDLE fnct_handle,
                               CPU_INT08U       if_ix,
                               RTOS_ERR         *p_err);
#endif

CPU_INT08U USBH_IF_EP_QtyGet(USBH_DEV_HANDLE  dev_handle,
                             USBH_FNCT_HANDLE fnct_handle,
                             CPU_INT08U       if_ix,
                             RTOS_ERR         *p_err);

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_IF_CLASS) == DEF_YES)
CPU_INT08U USBH_IF_ClassGet(USBH_DEV_HANDLE  dev_handle,
                            USBH_FNCT_HANDLE fnct_handle,
                            CPU_INT08U       if_ix,
                            RTOS_ERR         *p_err);
#endif

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_IF_SUBCLASS) == DEF_YES)
CPU_INT08U USBH_IF_SubclassGet(USBH_DEV_HANDLE  dev_handle,
                               USBH_FNCT_HANDLE fnct_handle,
                               CPU_INT08U       if_ix,
                               RTOS_ERR         *p_err);
#endif

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_IF_PROTOCOL) == DEF_YES)
CPU_INT08U USBH_IF_ProtocolGet(USBH_DEV_HANDLE  dev_handle,
                               USBH_FNCT_HANDLE fnct_handle,
                               CPU_INT08U       if_ix,
                               RTOS_ERR         *p_err);
#endif

#if (USBH_CFG_STR_EN == DEF_ENABLED)
CPU_INT08U USBH_IF_StrGet(USBH_DEV_HANDLE  dev_handle,
                          USBH_FNCT_HANDLE fnct_handle,
                          CPU_INT08U       if_ix,
                          CPU_CHAR         *p_str_buf,
                          CPU_INT08U       str_buf_len,
                          RTOS_ERR         *p_err);
#endif

CPU_INT08U *USBH_IF_DescExtraGet(USBH_FNCT_HANDLE fnct_handle,
                                 CPU_INT08U       if_ix,
                                 CPU_INT16U       *p_desc_extra_len,
                                 RTOS_ERR         *p_err);

CPU_INT08U USBH_IF_EP_AddrNextGet(USBH_DEV_HANDLE  dev_handle,
                                  USBH_FNCT_HANDLE fnct_handle,
                                  CPU_INT08U       if_ix,
                                  CPU_INT08U       ep_addr_prev,
                                  RTOS_ERR         *p_err);

#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
void USBH_IF_AltSet(USBH_DEV_HANDLE      dev_handle,
                    USBH_FNCT_HANDLE     fnct_handle,
                    CPU_INT08U           if_ix,
                    CPU_INT08U           if_alt_ix,
                    USBH_IF_ALT_SET_CMPL callback_fnct,
                    void                 *p_arg,
                    RTOS_ERR             *p_err);
#endif

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                            MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
