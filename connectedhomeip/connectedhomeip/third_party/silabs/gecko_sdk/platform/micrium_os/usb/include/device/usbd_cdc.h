/***************************************************************************//**
 * @file
 * @brief USB Device - USB Communications Device Class (CDC)
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
 * @defgroup USBD_CDC USB Device CDC API
 * @ingroup USBD
 * @brief   USB Device CDC API
 *
 * @addtogroup USBD_CDC
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBD_CDC_H_
#define  _USBD_CDC_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <usb/include/device/usbd_core.h>
#include  <common/include/rtos_err.h>
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   CDC INVALID INSTANCE NUMBERS DEFINES
 *******************************************************************************************************/

#define  USBD_CDC_NBR_NONE                      DEF_INT_08U_MAX_VAL
#define  USBD_CDC_DATA_IF_NBR_NONE              DEF_INT_08U_MAX_VAL

/********************************************************************************************************
 *                               COMMUNICATIONS CLASS SUBCLASS CODES DEFINES
 *
 * Note(s) : (1) Communication class subclass codes are defined in table 4 from the CDC specification
 *               revision 1.2.
 *******************************************************************************************************/

#define  USBD_CDC_SUBCLASS_RSVD                        0x00u    // Reserved.
#define  USBD_CDC_SUBCLASS_DLCM                        0x01u    // Direct line         control model.
#define  USBD_CDC_SUBCLASS_ACM                         0x02u    // Abstract            control model.
#define  USBD_CDC_SUBCLASS_TCM                         0x03u    // Telephone           control model.
#define  USBD_CDC_SUBCLASS_MCCM                        0x04u    // Multi-channel       control model.
#define  USBD_CDC_SUBCLASS_CAPICM                      0x05u    // CAPI                control model.
#define  USBD_CDC_SUBCLASS_ENCM                        0x06u    // Ethernet networking control model.
#define  USBD_CDC_SUBCLASS_ATM                         0x07u    // ATM networking      control model.
#define  USBD_CDC_SUBCLASS_WHCM                        0x08u    // Wireless handset    control model.
#define  USBD_CDC_SUBCLASS_DEV_MGMT                    0x09u    // Device management.
#define  USBD_CDC_SUBCLASS_MDLM                        0x0Au    // Mobile direct line model.
#define  USBD_CDC_SUBCLASS_OBEX                        0x0Bu    // OBEX.
#define  USBD_CDC_SUBCLASS_EEM                         0x0Cu    // Ethernet emulation model.
#define  USBD_CDC_SUBCLASS_NCM                         0x0Du    // Network             control model.
#define  USBD_CDC_SUBCLASS_VENDOR                      0xFEu    // Vendor specific.

/********************************************************************************************************
 *                               COMMUNICATION CLASS PROTOCOL CODES DEFINES
 *
 * Note(s) : (1) Communication class protocol codes are defined in table 5 from the CDC specification
 *               revision 1.2.
 *******************************************************************************************************/

#define  USBD_CDC_COMM_PROTOCOL_NONE                   0x00u    // No class specific protocol required.
#define  USBD_CDC_COMM_PROTOCOL_AT_V250                0x01u    // AT Commands: V.250 etc.
#define  USBD_CDC_COMM_PROTOCOL_AT_PCCA_101            0x02u    // AT Commands defined by PCCA-101.
#define  USBD_CDC_COMM_PROTOCOL_AT_PCCA_101_ANNEX      0x03u    // AT Commands defined by PCCA-101 & Annex O.
#define  USBD_CDC_COMM_PROTOCOL_AT_GSM_7_07            0x04u    // AT Commands defined by GSM 07.07
#define  USBD_CDC_COMM_PROTOCOL_AT_3GPP_27_07          0x05u    // AT Commands defined by 3GPP 27.007
#define  USBD_CDC_COMM_PROTOCOL_AT_TIA_CDMA            0x06u    // AT Commands defined by TIA for CDMA
#define  USBD_CDC_COMM_PROTOCOL_EEM                    0x07u    // Ethernet Emulation Model.
//                                                                 External Protocol: Commands defined by command set functional descriptor.
#define  USBD_CDC_COMM_PROTOCOL_EXT                    0xFEu
#define  USBD_CDC_COMM_PROTOCOL_VENDOR                 0xFFu    // Vendor-specific.

/********************************************************************************************************
 *                               DATA INTERFACE CLASS PROTOCOL CODES DEFINES
 *
 * Note(s) : (1) Data interface class protocol codes are defined in table 7 from the CDC specification
 *               revision 1.2.
 *******************************************************************************************************/

#define  USBD_CDC_DATA_PROTOCOL_NONE                   0x00u    // No class specific protocol required.
#define  USBD_CDC_DATA_PROTOCOL_NTB                    0x01u    // Network transfer block.
#define  USBD_CDC_DATA_PROTOCOL_PHY                    0x30u    // Physical interface protocol for ISDN BRI.
#define  USBD_CDC_DATA_PROTOCOL_HDLC                   0x31u    // Physical interface protocol for ISDN BRI.
#define  USBD_CDC_DATA_PROTOCOL_TRANS                  0x32u    // Transparent.
#define  USBD_CDC_DATA_PROTOCOL_Q921M                  0x50u    // Management protocol for Q.921 data link protocol.
#define  USBD_CDC_DATA_PROTOCOL_Q921                   0x51u    // Data link protocol for Q.931.
#define  USBD_CDC_DATA_PROTOCOL_Q921TM                 0x52u    // TEI-multiplexor for Q.921 data link protocol.
#define  USBD_CDC_DATA_PROTOCOL_COMPRES                0x90u    // Data compression procedures.
#define  USBD_CDC_DATA_PROTOCOL_Q9131                  0x91u    // Q.931/Euro- ISDN Euro-ISDN protocol control.
#define  USBD_CDC_DATA_PROTOCOL_V24                    0x92u    // V.24 rate adaptation to ISDN.
#define  USBD_CDC_DATA_PROTOCOL_CAPI                   0x93u    // CAPI commands.
#define  USBD_CDC_DATA_PROTOCOL_HOST                   0xFDu    // Host based drivers.
//                                                                 The protocol(s) are described using a protocol ...
//                                                                 ... unit functional descriptors on communications...
//                                                                 ... class.
#define  USBD_CDC_DATA_PROTOCOL_CDC                    0xFEu
#define  USBD_CDC_DATA_PROTOCOL_VENDOR                 0xFFu    // Vendor-specific.

/********************************************************************************************************
 *                                   MANAGEMENT ELEMENTS REQUESTS DEFINES
 *
 * Note(s) : (1) Management elements requests are defined in table 19 from the CDC specification
 *               revision 1.2.
 *******************************************************************************************************/

#define  USBD_CDC_REQ_SEND_ENCAP_COMM                  0x00u
#define  USBD_CDC_REQ_GET_ENCAP_RESP                   0x01u
#define  USBD_CDC_REQ_SET_COMM_FEATURE                 0x02u
#define  USBD_CDC_REQ_GET_COMM_FEATURE                 0x03u
#define  USBD_CDC_REQ_CLR_COMM_FEATURE                 0x04u
#define  USBD_CDC_REQ_SET_AUX_LINE_STATE               0x10u
#define  USBD_CDC_REQ_SET_HOOK_STATE                   0x11u
#define  USBD_CDC_REQ_PULSE_SETUP                      0x12u
#define  USBD_CDC_REQ_SEND_PULSE                       0x13u
#define  USBD_CDC_REQ_SET_PULSE_TIME                   0x14u
#define  USBD_CDC_REQ_RING_AUX_JACK                    0x15u
#define  USBD_CDC_REQ_SET_LINE_CODING                  0x20u
#define  USBD_CDC_REQ_GET_LINE_CODING                  0x21u
#define  USBD_CDC_REQ_SET_CTRL_LINE_STATE              0x22u
#define  USBD_CDC_REQ_SEND_BREAK                       0x23u
#define  USBD_CDC_REQ_SET_RINGER_PAARMS                0x30u
#define  USBD_CDC_REQ_GET_RINGER_PARAMS                0x31u
#define  USBD_CDC_REQ_SET_OPERATION_PARAMS             0x32u
#define  USBD_CDC_REQ_GET_OPERATION_PARAMS             0x33u
#define  USBD_CDC_REQ_SET_LINE_PARAMS                  0x34u
#define  USBD_CDC_REQ_GET_LINE_PARAMS                  0x35u
#define  USBD_CDC_REQ_DIAL_DIGITS                      0x36u
#define  USBD_CDC_REQ_SET_UNIT_PARAM                   0x37u
#define  USBD_CDC_REQ_GET_UNIT_PARAM                   0x38u
#define  USBD_CDC_REQ_CLR_UNUT_PARAM                   0x39u
#define  USBD_CDC_REQ_GET_PROFILE                      0x3Au
#define  USBD_CDC_REQ_SET_ETHER_MULTI_FILTER           0x40u
#define  USBD_CDC_REQ_SET_ETHER_PWR_MGT_FILTER         0x41u
#define  USBD_CDC_REQ_GET_ETHER_PWR_MGT_FILTER         0x42u
#define  USBD_CDC_REQ_SET_ETHER_PKT_FILTER             0x43u
#define  USBD_CDC_REQ_GET_ETHER_STAT                   0x44u
#define  USBD_CDC_REQ_SET_ATM_DATA_FMT                 0x50u
#define  USBD_CDC_REQ_GET_ATM_DATA_FMT                 0x51u
#define  USBD_CDC_REQ_GET_ATM_DEV_STAT                 0x52u
#define  USBD_CDC_REQ_SET_ATM_DFLT_VC                  0x53u
#define  USBD_CDC_REQ_GET_ATM_VC_STAT                  0x54u
#define  USBD_CDC_REQ_GET_NTB_PARAM2                   0x80u
#define  USBD_CDC_REQ_GET_NET_ADDR                     0x81u
#define  USBD_CDC_REQ_SET_NET_ADDR                     0x82u
#define  USBD_CDC_REQ_GET_NTB_FMT                      0x83u
#define  USBD_CDC_REQ_SET_NTB_FMT                      0x84u
#define  USBD_CDC_REQ_GET_NTB_INPUT_SIZE               0x85u
#define  USBD_CDC_REQ_SET_NTB_INPUT_SIZE               0x86u
#define  USBD_CDC_REQ_GET_MAX_DATAGRAM_SIZE            0x87u
#define  USBD_CDC_REQ_SET_MAX_DATAGRAM_SIZE            0x88u
#define  USBD_CDC_REQ_GET_CRC_MODE                     0x89u
#define  USBD_CDC_REQ_SET_CRC_MODE                     0x8Au

/********************************************************************************************************
 *                               CDC FUNCTIONAL DESCRIPTOR TYPE DEFINES
 *
 * Note(s) : (1) Functional descriptors types are defined in table 12 from the CDC specification revision
 *               1.2.
 *******************************************************************************************************/

#define  USBD_CDC_DESC_TYPE_CS_IF                      0x24u
#define  USBD_CDC_DESC_TYPE_CS_EP                      0x25u

/********************************************************************************************************
 *                               CDC FUNCTIONAL DESCRIPTOR SUBTYPE DEFINES
 *
 * Note(s) : (1) Functional descriptors subtypes are defined in table 13 from the CDC specification
 *               revision 1.2.
 *******************************************************************************************************/
//                                                                 Header functional descriptor, which marks the ...
//                                                                 ... beginning of the concatenated set of      ...
//                                                                 ... functional descriptors for the interface.
#define  USBD_CDC_DESC_SUBTYPE_HEADER                  0x00u
#define  USBD_CDC_DESC_SUBTYPE_CALL_MGMT               0x01u    // Call management                functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_ACM                     0x02u    // Abstract control management    functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_DLM                     0x03u    // Direct line management         functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_TEL_RINGER              0x04u    // Telephone Ringer               functional descriptor.
//                                                                 Telephone call and line state reporting ...
//                                                                 ... capabilities functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_TEL_CALL_LINE           0x05u
#define  USBD_CDC_DESC_SUBTYPE_UNION                   0x06u    // Union functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_COUNTRY_SEL             0x07u    // Country selection              functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_TEL_OPERA               0x08u    // Telephone operational modes    functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_USB_TERM                0x09u    // USB terminal                   functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_NET_CH_TERM             0x0Au    // Network channel terminal descriptor.
#define  USBD_CDC_DESC_SUBTYPE_PROTOCOL_UNIT           0x0Bu    // Protocol unit                  functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_EXT_UNIT                0x0Cu    // Extension unit                 functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_MULTI_CH_MGMT           0x0Du    // Multi-Channel management       functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_CAPI_CTRL               0x0Eu    // CAPI control management        functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_ETHER_NET               0x0Fu    // Ethernet networking            functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_ATM_NET                 0x10u    // ATM networking                 functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_WIRELESS_CTRL           0x11u    // Wireless handset control model functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_MOBILE DLM              0x12u    // Mobile Direct Line Model       functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_DETAIL                  0x13u    // MDLM Detail                    functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_DEV_MGMT                0x14u    // Device management model        functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_OBEX                    0x15u    // OBEX                           functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_CMD_SET                 0x16u    // Command set                    functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_CMD_SET_DETAIL          0x17u    // Command set detail             functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_TCM                     0x18u    // Telephone control model        functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_OBEX_SERV               0x19u    // OBEX service identifier        functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_NCM                     0x1Au    // NCM                            functional descriptor.
#define  USBD_CDC_DESC_SUBTYPE_VENDOR                  0xFEu    // Vendor specific.

/********************************************************************************************************
 *                                       CDC NOTIFIACTION DEFINES
 *******************************************************************************************************/

#define  USBD_CDC_NOTIFICATION_HEADER                     8u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef const struct usbd_cdc_subclass_drv {                    // --------------- CDC SUBCLASS DRIVER ----------------
                                                                // Callback to handle management requests.
  CPU_BOOLEAN (*MngmtReq)(CPU_INT08U           dev_nbr,
                          const USBD_SETUP_REQ *p_setup_req,
                          void                 *p_subclass_arg);

  //                                                               Callback to handle notification completion.
  void (*NotifyCmpl)(CPU_INT08U dev_nbr,
                     void       *p_subclass_arg);

  //                                                               Callback to build functional desc.
  void (*FnctDesc)(CPU_INT08U dev_nbr,
                   void       *p_subclass_arg,
                   CPU_INT08U first_dci_if_nbr);

  //                                                               Callback to get the size of the functional desc.
  CPU_INT16U (*FnctDescSizeGet)(CPU_INT08U dev_nbr,
                                void       *p_subclass_arg);
} USBD_CDC_SUBCLASS_DRV;

typedef struct usbd_cdc_cfg {                                   // -------- CDC CLASS CONFIGURAITON STRUCTURE ---------
  CPU_INT08U ClassInstanceQty;                                  // Quantity of class instance.
  CPU_INT08U ConfigQty;                                         // Quantity of configuration.

  CPU_INT08U DataIF_Qty;                                        // Quantity of data interfaces.
} USBD_CDC_QTY_CFG;

typedef struct usbd_cdc_init_cfg {                              // ------------- CDC INIT CONFIGURATIONS --------------
  MEM_SEG *MemSegPtr;                                           // Ptr to mem segment to use when allocating ctrl data.
} USBD_CDC_INIT_CFG;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

extern const USBD_CDC_INIT_CFG USBD_CDC_InitCfgDflt;            // USBD CDC class dflt configurations.

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   CONFIGURATION OVERRIDE FUNCTIONS
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_CDC_ConfigureMemSeg(MEM_SEG *p_mem_seg);
#endif

/********************************************************************************************************
 *                                               CDC FUNCTIONS
 *******************************************************************************************************/

void USBD_CDC_Init(USBD_CDC_QTY_CFG *p_qty_cfg,
                   RTOS_ERR         *p_err);

CPU_INT08U USBD_CDC_Add(CPU_INT08U            subclass,
                        USBD_CDC_SUBCLASS_DRV *p_subclass_drv,
                        void                  *p_subclass_arg,
                        CPU_INT08U            protocol,
                        CPU_BOOLEAN           notify_en,
                        CPU_INT16U            notify_interval,
                        RTOS_ERR              *p_err);

CPU_BOOLEAN USBD_CDC_ConfigAdd(CPU_INT08U class_nbr,
                               CPU_INT08U dev_nbr,
                               CPU_INT08U config_nbr,
                               RTOS_ERR   *p_err);

CPU_BOOLEAN USBD_CDC_IsConn(CPU_INT08U class_nbr);

//                                                                 ---------- DATA INTERFACE CLASS FUNCTIONS ----------
CPU_INT08U USBD_CDC_DataIF_Add(CPU_INT08U  class_nbr,
                               CPU_BOOLEAN isoc_en,
                               CPU_INT08U  protocol,
                               RTOS_ERR    *p_err);

CPU_INT32U USBD_CDC_DataRx(CPU_INT08U class_nbr,
                           CPU_INT08U data_if_nbr,
                           CPU_INT08U *p_buf,
                           CPU_INT32U buf_len,
                           CPU_INT16U timeout,
                           RTOS_ERR   *p_err);

CPU_INT32U USBD_CDC_DataTx(CPU_INT08U class_nbr,
                           CPU_INT08U data_if_nbr,
                           CPU_INT08U *p_buf,
                           CPU_INT32U buf_len,
                           CPU_INT16U timeout,
                           RTOS_ERR   *p_err);

//                                                                 ------------- NOTIFICATION FUNCTIONS --------------
CPU_BOOLEAN USBD_CDC_Notify(CPU_INT08U class_nbr,
                            CPU_INT08U notification,
                            CPU_INT16U value,
                            CPU_INT08U *p_buf,
                            CPU_INT16U data_len,
                            RTOS_ERR   *p_err);

#if 0
void USBD_CDC_GrpCreate(CPU_INT08 class_nbr,
                        RTOS_ERR  *p_err);

void USBD_CDC_GrpAdd(CPU_INT08U class_nbr,
                     CPU_INT08U nbr_slave,
                     RTOS_ERR   *p_err);
#endif

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
