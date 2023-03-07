/***************************************************************************//**
 * @file
 * @brief USB Host - Communications Device Class (Cdc) - Subclass Interface
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

#ifndef  _USBH_CDC_SUBCLASS_PRIV_H_
#define  _USBH_CDC_SUBCLASS_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>

#include  <usb/source/host/class/usbh_class_core_priv.h>
#include  <usb/source/host/class/usbh_class_common_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  USBH_CDC_LEN_NOTIFICATION_HDR                     8u

/********************************************************************************************************
 *                                       FUNCTIONAL DESCRIPTORS CODES
 *
 * Note(s) : (1) For more information on functional descriptor type codes, see 'USB Class Definitions for
 *               Communication Devices Specification", version 1.2, Section 5.2.3, Table 12'.
 *******************************************************************************************************/

#define  USBH_CDC_FNCTL_DESC_IF                         0x24u   // Interface Functional Descriptor.
#define  USBH_CDC_FNCTL_DESC_EP                         0x25u   // Endpoint  Functional Descriptor.

/********************************************************************************************************
 *                                   FUNCTIONAL DESCRIPTORS SUBTYPE CODES
 *
 * Note(s) : (1) For more information on functional descriptor subtype codes, see 'USB Class Definitions
 *               for Communication Devices Specification", version 1.2, Section 5.2.3, Table 13'.
 *******************************************************************************************************/

#define  USBH_CDC_FNCTL_DESC_SUB_HDR                    0x00u   // Header.
#define  USBH_CDC_FNCTL_DESC_SUB_CM                     0x01u   // Call             Management.
#define  USBH_CDC_FNCTL_DESC_SUB_ACM                    0x02u   // Abstract Control Management.
#define  USBH_CDC_FNCTL_DESC_SUB_DLM                    0x03u   // Direct Line      Management.
#define  USBH_CDC_FNCTL_DESC_SUB_TR                     0x04u   // Telephone Ringer.
#define  USBH_CDC_FNCTL_DESC_SUB_TCLSRC                 0x05u   // Telephone Call and Line State Reporting Capabilities.
#define  USBH_CDC_FNCTL_DESC_SUB_UNION                  0x06u   // Union.
#define  USBH_CDC_FNCTL_DESC_SUB_CS                     0x07u   // Country Selection.
#define  USBH_CDC_FNCTL_DESC_SUB_TOM                    0x08u   // Telephone Operational Modes.
#define  USBH_CDC_FNCTL_DESC_SUB_USB_TERMINAL           0x09u   // USB Terminal.
#define  USBH_CDC_FNCTL_DESC_SUB_NCTD                   0x0Au   // Network Channel.
#define  USBH_CDC_FNCTL_DESC_SUB_PU                     0x0Bu   // Protocol  Unit.
#define  USBH_CDC_FNCTL_DESC_SUB_EU                     0x0Cu   // Extension Unit.
#define  USBH_CDC_FNCTL_DESC_SUB_MCM                    0x0Du   // Multi-Channel Management.
#define  USBH_CDC_FNCTL_DESC_SUB_CAPI_CM                0x0Eu   // CAPI Control  Management.
#define  USBH_CDC_FNCTL_DESC_SUB_EN                     0x0Fu   // Ethernet Networking.
#define  USBH_CDC_FNCTL_DESC_SUB_ATM_NET                0x10u   // ATM Networking.
#define  USBH_CDC_FNCTL_DESC_SUB_WHCM                   0x11u   // Wireless Handset Control Model.
#define  USBH_CDC_FNCTL_DESC_SUB_MDLM                   0x12u   // Mobile Direct Line Model.
#define  USBH_CDC_FNCTL_DESC_SUB_MDLM_DETAIL            0x13u   // Mobile Direct Line Model Detail.
#define  USBH_CDC_FNCTL_DESC_SUB_DMM                    0x14u   // Device Management  Model.
#define  USBH_CDC_FNCTL_DESC_SUB_OBEX                   0x15u   // OBEX.
#define  USBH_CDC_FNCTL_DESC_SUB_CMD_SET                0x16u   // Command Set.
#define  USBH_CDC_FNCTL_DESC_SUB_CMD_SET_DETAIL         0x17u   // Command Set Detail.
#define  USBH_CDC_FNCTL_DESC_SUB_TCM                    0x18u   // Telephone Control Model.
#define  USBH_CDC_FNCTL_DESC_SUB_OBEX_SI                0x19u   // OBEX Service Identifier.
#define  USBH_CDC_FNCTL_DESC_SUB_NCM                    0x1Au   // NCM.
#define  USBH_CDC_FNCTL_DESC_SUB_MBIM                   0x1Bu   // MBIM.
#define  USBH_CDC_FNCTL_DESC_SUB_MBIM_EXT               0x1Cu   // MBIM Extended.

/********************************************************************************************************
 *                                       FUNCTIONAL DESCRIPTORS LENGTH
 *******************************************************************************************************/

#define  USBH_CDC_FNCTL_DESC_LEN_HDR                       5u

/********************************************************************************************************
 *                                   CDC CALL MANAGEMENT CAPABILITIES BITS
 *
 * Note(s) : (1) See 'Communications Class Subclass Specification for PSTN Devices, version 1.2, Section
 *               5.3.1, Table 3'.
 *******************************************************************************************************/

#define  USBH_CDC_CALL_MGMT_CAP_HANDLE               DEF_BIT_00
#define  USBH_CDC_CALL_MGMT_CAP_DCI                  DEF_BIT_01

/********************************************************************************************************
 *                           CDC ABSTRACT CONTROL MANAGEMENT CAPABILITIES BITS
 *
 * Note(s) : (1) See 'Communications Class Subclass Specification for PSTN Devices, version 1.2, Section
 *               5.3.2, Table 4'.
 *******************************************************************************************************/

#define  USBH_CDC_ACM_CAP_COMM_FEATURE               DEF_BIT_00
#define  USBH_CDC_ACM_CAP_LINE_CODING                DEF_BIT_01
#define  USBH_CDC_ACM_CAP_SEND_BREAK                 DEF_BIT_02
#define  USBH_CDC_ACM_CAP_NET_CONN                   DEF_BIT_03

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       CDC ASYNCHRONOUS FUNCTION
 *******************************************************************************************************/

typedef void (*USBH_CDC_ASYNC_FNCT) (USBH_CDC_FNCT_HANDLE cdc_handle,
                                     CPU_INT08U           dci_ix,
                                     CPU_INT08U           *p_buf,
                                     CPU_INT32U           buf_len,
                                     CPU_INT32U           xfer_len,
                                     void                 *p_arg,
                                     RTOS_ERR             err);

/********************************************************************************************************
 *                                           CDC SUBCLASS DRIVER
 *******************************************************************************************************/

typedef struct usbh_cdc_subclass_drv {
  //                                                               Subclass probe.
  CPU_BOOLEAN (*Probe)(USBH_DEV_HANDLE  dev_handle,
                       USBH_FNCT_HANDLE fnct_handle,
                       CPU_INT08U       class_code,
                       void             **pp_cdc_sublass,
                       RTOS_ERR         *p_err);

  //                                                               Notify of CDC function connection.
  void (*Conn)(USBH_DEV_HANDLE      dev_handle,
               USBH_FNCT_HANDLE     fnct_handle,
               USBH_CDC_FNCT_HANDLE cdc_handle,
               void                 **pp_cdc_sublass);

  //                                                               Notify of CDC function disconnection.
  void (*Disconn)(USBH_CDC_FNCT_HANDLE cdc_handle,
                  void                 *p_cdc_sublass);

  //                                                               Notify of CDC function network connection.
  void (*NetConn)(USBH_CDC_FNCT_HANDLE cdc_handle,
                  void                 *p_cdc_sublass,
                  CPU_BOOLEAN          is_conn);

  //                                                               Notify of CDC function response available.
  void (*RespAvail)(USBH_CDC_FNCT_HANDLE cdc_handle,
                    void                 *p_cdc_sublass);

  //                                                               Notify of CDC function aux jack hoos tate chng.
  void (*AuxJackHookState)(USBH_CDC_FNCT_HANDLE cdc_handle,
                           void                 *p_cdc_sublass,
                           CPU_BOOLEAN          is_on_hook);

  //                                                               Notify of CDC function ring detect.
  void (*RingDetect)(USBH_CDC_FNCT_HANDLE cdc_handle,
                     void                 *p_cdc_sublass);

  //                                                               Notify of CDC function serial state chng.
  void (*SerialState)(USBH_CDC_FNCT_HANDLE cdc_handle,
                      void                 *p_cdc_sublass,
                      CPU_INT08U           serial_state);

  //                                                               Notify of CDC function call state chng.
  void (*CallStateChng)(USBH_CDC_FNCT_HANDLE cdc_handle,
                        void                 *p_cdc_sublass,
                        CPU_INT08U           ix,
                        CPU_INT08U           state_chng,
                        void                 *p_buf,
                        CPU_INT16U           buf_len);

  //                                                               Notify of CDC function line state chng.
  void (*LineStateChng)(USBH_CDC_FNCT_HANDLE cdc_handle,
                        void                 *p_cdc_sublass,
                        CPU_INT16U           value,
                        void                 *p_buf,
                        CPU_INT16U           buf_len);

  //                                                               Notify of CDC function connection spd chng.
  void (*ConnSpdChng)(USBH_CDC_FNCT_HANDLE cdc_handle,
                      void                 *p_cdc_sublass,
                      CPU_INT32U           upstream_bit_rate,
                      CPU_INT32U           downstream_bit_rate);

  CPU_INT16U BufLenNotifications;                               // Buffer length needed to handle notifications.
} USBH_CDC_SUBCLASS_DRV;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void USBH_CDC_SubclassDrvReg(USBH_CLASS_DRV        *p_subclass_drv,
                             USBH_CDC_SUBCLASS_DRV *p_cdc_subclass_drv,
                             RTOS_ERR              *p_err);

void USBH_CDC_DCI_RxAsync(USBH_CDC_FNCT_HANDLE cdc_handle,
                          CPU_INT08U           dci_ix,
                          CPU_INT08U           *p_buf,
                          CPU_INT32U           buf_len,
                          USBH_CDC_ASYNC_FNCT  async_fnct,
                          void                 *p_arg,
                          RTOS_ERR             *p_err);

void USBH_CDC_DCI_TxAsync(USBH_CDC_FNCT_HANDLE cdc_handle,
                          CPU_INT08U           dci_ix,
                          CPU_INT08U           *p_buf,
                          CPU_INT32U           buf_len,
                          USBH_CDC_ASYNC_FNCT  async_fnct,
                          void                 *p_arg,
                          RTOS_ERR             *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
