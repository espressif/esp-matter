/***************************************************************************//**
 * @file
 * @brief USB Host Device Operations
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

#ifndef  _USBH_CORE_DEV_H_
#define  _USBH_CORE_DEV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_core_utils.h>
#include  <usb/include/host/usbh_core_opt_def.h>
#include  <usb/include/host/usbh_core_handle.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  USBH_DEV_ADDR_RH                               0xEEu

#define  USBH_DEV_NBR_EP                                  32u

/********************************************************************************************************
 *                                       USB SPECIFICATION NUMBER
 *
 * Note(s) : (1) See 'Universal Serial Bus Specification Revision 2.0', Section 9.6.1, Table 9-8.
 *
 *           (2) The field "bcdUSB" is part of the device descriptor and indicates the release number of the
 *               USB specification to which the device complies.
 *******************************************************************************************************/

#define  USBH_DEV_SPEC_NBR_1_0                        0x0100u
#define  USBH_DEV_SPEC_NBR_1_1                        0x0110u
#define  USBH_DEV_SPEC_NBR_2_0                        0x0200u
#define  USBH_DEV_SPEC_NBR_2_1                        0x0210u
#define  USBH_DEV_SPEC_NBR_3_0                        0x0300u

/********************************************************************************************************
 *                                       REQUEST CHARACTERISTICS
 *
 * Note(s) : (1) See 'Universal Serial Bus Specification Revision 2.0', Section 9.3, Table 9-2.
 *
 *           (2) The 'bmRequestType' field of a setup request is a bit-mapped datum with three subfields :
 *
 *               (a) Bit  7  : Data transfer direction.
 *               (b) Bits 6-5: Type.
 *               (c) Bits 4-0: Recipient.
 *******************************************************************************************************/

#define  USBH_DEV_REQ_DIR_MASK                          0x80u
#define  USBH_DEV_REQ_DIR_HOST_TO_DEV                   0x00u
#define  USBH_DEV_REQ_DIR_DEV_TO_HOST                   0x80u

#define  USBH_DEV_REQ_TYPE_STD                          0x00u
#define  USBH_DEV_REQ_TYPE_CLASS                        0x20u
#define  USBH_DEV_REQ_TYPE_VENDOR                       0x40u
#define  USBH_DEV_REQ_TYPE_RSVD                         0x60u

#define  USBH_DEV_REQ_RECIPIENT_DEV                     0x00u
#define  USBH_DEV_REQ_RECIPIENT_IF                      0x01u
#define  USBH_DEV_REQ_RECIPIENT_EP                      0x02u
#define  USBH_DEV_REQ_RECIPIENT_OTHER                   0x03u

/********************************************************************************************************
 *                                           STANDARD REQUESTS
 *
 * Note(s) : (1) See 'Universal Serial Bus Specification Revision 2.0', Section 9.4, Table 9-4.
 *
 *           (2) The 'bRequest' field of a standard setup request may contain one of these values.
 *******************************************************************************************************/

#define  USBH_DEV_REQ_GET_STATUS                        0x00u
#define  USBH_DEV_REQ_CLR_FEATURE                       0x01u
#define  USBH_DEV_REQ_SET_FEATURE                       0x03u
#define  USBH_DEV_REQ_SET_ADDR                          0x05u
#define  USBH_DEV_REQ_GET_DESC                          0x06u
#define  USBH_DEV_REQ_SET_DESC                          0x07u
#define  USBH_DEV_REQ_GET_CFG                           0x08u
#define  USBH_DEV_REQ_SET_CFG                           0x09u
#define  USBH_DEV_REQ_GET_IF                            0x0Au
#define  USBH_DEV_REQ_SET_IF                            0x0Bu
#define  USBH_DEV_REQ_SYNCH_FRAME                       0x0Cu

/********************************************************************************************************
 *                                           DESCRIPTOR TYPES
 *
 * Note(s) : (1) See 'Universal Serial Bus Specification Revision 2.0', Section 9.4, Table 9-5, and
 *               Section 9.4.3.
 *
 *           (2) For a 'get descriptor' setup request, the low byte of the 'wValue' field may contain
 *               one of these values.
 *******************************************************************************************************/

#define  USBH_DESC_TYPE_DEV                                1u
#define  USBH_DESC_TYPE_DEV_QUALIFIER                      6u

#define  USBH_DESC_LEN_DEV                                18u
#define  USBH_DESC_LEN_DEV_QUALIFIER                      10u

/********************************************************************************************************
 *                                           DEVICE ADDRESSES
 *******************************************************************************************************/

#define  USBH_DEV_ADDR_INVALID                           255u

#define  USBH_DEV_HANDLE_IS_RH(dev_handle)                  ((((dev_handle) & 0x000000FFu) == USBH_DEV_ADDR_RH) ? DEF_YES : DEF_NO)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               DEVICE SPEED
 *******************************************************************************************************/

typedef enum usbh_dev_spd {
  USBH_DEV_SPD_NONE = 0u,

  USBH_DEV_SPD_LOW = 1u,
  USBH_DEV_SPD_FULL = 2u,
  USBH_DEV_SPD_HIGH = 3u
} USBH_DEV_SPD;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

USBH_DEV_SPD USBH_DevSpdGet(USBH_DEV_HANDLE dev_handle,
                            RTOS_ERR        *p_err);

CPU_INT08U USBH_DevAddrGet(USBH_DEV_HANDLE dev_handle,
                           RTOS_ERR        *p_err);

CPU_INT08U USBH_DevHostNbrGet(USBH_DEV_HANDLE dev_handle,
                              RTOS_ERR        *p_err);

CPU_INT08U USBH_DevHC_NbrGet(USBH_DEV_HANDLE dev_handle,
                             RTOS_ERR        *p_err);

USBH_HC_HANDLE USBH_DevHC_HandleGet(USBH_DEV_HANDLE dev_handle);

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_SPEC_NBR) == DEF_YES)
CPU_INT16U USBH_DevSpecNbrGet(USBH_DEV_HANDLE dev_handle,
                              RTOS_ERR        *p_err);
#endif

CPU_INT08U USBH_DevClassGet(USBH_DEV_HANDLE dev_handle,
                            RTOS_ERR        *p_err);

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_SUBCLASS) == DEF_YES)
CPU_INT08U USBH_DevSubclassGet(USBH_DEV_HANDLE dev_handle,
                               RTOS_ERR        *p_err);
#endif

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_PROTOCOL) == DEF_YES)
CPU_INT08U USBH_DevProtocolGet(USBH_DEV_HANDLE dev_handle,
                               RTOS_ERR        *p_err);
#endif

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_VENDOR_ID) == DEF_YES)
CPU_INT16U USBH_DevVendorID_Get(USBH_DEV_HANDLE dev_handle,
                                RTOS_ERR        *p_err);
#endif

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_PRODUCT_ID) == DEF_YES)
CPU_INT16U USBH_DevProductID_Get(USBH_DEV_HANDLE dev_handle,
                                 RTOS_ERR        *p_err);
#endif

#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_REL_NBR) == DEF_YES)
CPU_INT16U USBH_DevRelNbrGet(USBH_DEV_HANDLE dev_handle,
                             RTOS_ERR        *p_err);
#endif

#if (USBH_CFG_STR_EN == DEF_ENABLED)
CPU_INT08U USBH_DevManufacturerStrGet(USBH_DEV_HANDLE dev_handle,
                                      CPU_CHAR        *p_str_buf,
                                      CPU_INT08U      str_buf_len,
                                      RTOS_ERR        *p_err);

CPU_INT08U USBH_DevProductStrGet(USBH_DEV_HANDLE dev_handle,
                                 CPU_CHAR        *p_str_buf,
                                 CPU_INT08U      str_buf_len,
                                 RTOS_ERR        *p_err);

CPU_INT08U USBH_DevSerNbrStrGet(USBH_DEV_HANDLE dev_handle,
                                CPU_CHAR        *p_str_buf,
                                CPU_INT08U      str_buf_len,
                                RTOS_ERR        *p_err);
#endif

CPU_INT08U USBH_DevConfigQtyGet(USBH_DEV_HANDLE dev_handle,
                                RTOS_ERR        *p_err);

void USBH_DevConfigSet(USBH_DEV_HANDLE dev_handle,
                       CPU_INT08U      cfg_nbr,
                       RTOS_ERR        *p_err);

CPU_INT08U USBH_DevConfigGet(USBH_DEV_HANDLE dev_handle,
                             RTOS_ERR        *p_err);

CPU_INT08U USBH_DevPortNbrGet(USBH_DEV_HANDLE dev_handle,
                              RTOS_ERR        *p_err);

USBH_DEV_HANDLE USBH_DevHubHandleGet(USBH_DEV_HANDLE dev_handle,
                                     RTOS_ERR        *p_err);

USBH_DEV_HANDLE USBH_DevHS_HubNearestHandleGet(USBH_DEV_HANDLE dev_handle,
                                               CPU_INT08U      *p_port_nbr,
                                               RTOS_ERR        *p_err);

CPU_INT16U USBH_DevDescRd(USBH_DEV_HANDLE dev_handle,
                          CPU_INT08U      recipient,
                          CPU_INT08U      type,
                          CPU_INT08U      desc_type,
                          CPU_INT08U      desc_ix,
                          CPU_INT16U      desc_len_req,
                          CPU_INT08U      *desc_buf,
                          CPU_INT16U      desc_buf_len,
                          RTOS_ERR        *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
