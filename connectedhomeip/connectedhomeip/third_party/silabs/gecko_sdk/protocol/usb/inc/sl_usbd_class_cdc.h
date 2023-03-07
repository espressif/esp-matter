/***************************************************************************//**
 * @file
 * @brief USB Device - USB Communications Device Class (CDC)
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef  SL_USBD_CLASS_CDC_H
#define  SL_USBD_CLASS_CDC_H

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "sl_usbd_core.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   CDC INVALID INSTANCE NUMBERS DEFINES
 *******************************************************************************************************/

#define  SL_USBD_CDC_NBR_NONE                      255u
#define  SL_USBD_CDC_DATA_IF_NBR_NONE              255u

/********************************************************************************************************
 *                               COMMUNICATIONS CLASS SUBCLASS CODES DEFINES
 *
 * Note(s) : (1) Communication class subclass codes are defined in table 4 from the CDC specification
 *               revision 1.2.
 *******************************************************************************************************/

#define  SL_USBD_CDC_SUBCLASS_RSVD                        0x00u    // Reserved.
#define  SL_USBD_CDC_SUBCLASS_DLCM                        0x01u    // Direct line         control model.
#define  SL_USBD_CDC_SUBCLASS_ACM                         0x02u    // Abstract            control model.
#define  SL_USBD_CDC_SUBCLASS_TCM                         0x03u    // Telephone           control model.
#define  SL_USBD_CDC_SUBCLASS_MCCM                        0x04u    // Multi-channel       control model.
#define  SL_USBD_CDC_SUBCLASS_CAPICM                      0x05u    // CAPI                control model.
#define  SL_USBD_CDC_SUBCLASS_ENCM                        0x06u    // Ethernet networking control model.
#define  SL_USBD_CDC_SUBCLASS_ATM                         0x07u    // ATM networking      control model.
#define  SL_USBD_CDC_SUBCLASS_WHCM                        0x08u    // Wireless handset    control model.
#define  SL_USBD_CDC_SUBCLASS_DEV_MGMT                    0x09u    // Device management.
#define  SL_USBD_CDC_SUBCLASS_MDLM                        0x0Au    // Mobile direct line model.
#define  SL_USBD_CDC_SUBCLASS_OBEX                        0x0Bu    // OBEX.
#define  SL_USBD_CDC_SUBCLASS_EEM                         0x0Cu    // Ethernet emulation model.
#define  SL_USBD_CDC_SUBCLASS_NCM                         0x0Du    // Network             control model.
#define  SL_USBD_CDC_SUBCLASS_VENDOR                      0xFEu    // Vendor specific.

/********************************************************************************************************
 *                               COMMUNICATION CLASS PROTOCOL CODES DEFINES
 *
 * Note(s) : (1) Communication class protocol codes are defined in table 5 from the CDC specification
 *               revision 1.2.
 *******************************************************************************************************/

#define  SL_USBD_CDC_COMM_PROTOCOL_NONE                   0x00u    // No class specific protocol required.
#define  SL_USBD_CDC_COMM_PROTOCOL_AT_V250                0x01u    // AT Commands: V.250 etc.
#define  SL_USBD_CDC_COMM_PROTOCOL_AT_PCCA_101            0x02u    // AT Commands defined by PCCA-101.
#define  SL_USBD_CDC_COMM_PROTOCOL_AT_PCCA_101_ANNEX      0x03u    // AT Commands defined by PCCA-101 & Annex O.
#define  SL_USBD_CDC_COMM_PROTOCOL_AT_GSM_7_07            0x04u    // AT Commands defined by GSM 07.07
#define  SL_USBD_CDC_COMM_PROTOCOL_AT_3GPP_27_07          0x05u    // AT Commands defined by 3GPP 27.007
#define  SL_USBD_CDC_COMM_PROTOCOL_AT_TIA_CDMA            0x06u    // AT Commands defined by TIA for CDMA
#define  SL_USBD_CDC_COMM_PROTOCOL_EEM                    0x07u    // Ethernet Emulation Model.
//                                                                 External protocol: Commands defined by command set functional descriptor.
#define  SL_USBD_CDC_COMM_PROTOCOL_EXT                    0xFEu
#define  SL_USBD_CDC_COMM_PROTOCOL_VENDOR                 0xFFu    // Vendor-specific.

/********************************************************************************************************
 *                               DATA INTERFACE CLASS PROTOCOL CODES DEFINES
 *
 * Note(s) : (1) Data interface class protocol codes are defined in table 7 from the CDC specification
 *               revision 1.2.
 *******************************************************************************************************/

#define  SL_USBD_CDC_DATA_PROTOCOL_NONE                   0x00u    // No class specific protocol required.
#define  SL_USBD_CDC_DATA_PROTOCOL_NTB                    0x01u    // Network transfer block.
#define  SL_USBD_CDC_DATA_PROTOCOL_PHY                    0x30u    // Physical interface protocol for ISDN BRI.
#define  SL_USBD_CDC_DATA_PROTOCOL_HDLC                   0x31u    // Physical interface protocol for ISDN BRI.
#define  SL_USBD_CDC_DATA_PROTOCOL_TRANS                  0x32u    // Transparent.
#define  SL_USBD_CDC_DATA_PROTOCOL_Q921M                  0x50u    // Management protocol for Q.921 data link protocol.
#define  SL_USBD_CDC_DATA_PROTOCOL_Q921                   0x51u    // Data link protocol for Q.931.
#define  SL_USBD_CDC_DATA_PROTOCOL_Q921TM                 0x52u    // TEI-multiplexor for Q.921 data link protocol.
#define  SL_USBD_CDC_DATA_PROTOCOL_COMPRES                0x90u    // Data compression procedures.
#define  SL_USBD_CDC_DATA_PROTOCOL_Q9131                  0x91u    // Q.931/Euro- ISDN Euro-ISDN protocol control.
#define  SL_USBD_CDC_DATA_PROTOCOL_V24                    0x92u    // V.24 rate adaptation to ISDN.
#define  SL_USBD_CDC_DATA_PROTOCOL_CAPI                   0x93u    // CAPI commands.
#define  SL_USBD_CDC_DATA_PROTOCOL_HOST                   0xFDu    // Host based drivers.
//                                                                 The protocol(s) are described using a protocol ...
//                                                                 ... unit functional descriptors on communications...
//                                                                 ... class.
#define  SL_USBD_CDC_DATA_PROTOCOL_CDC                    0xFEu
#define  SL_USBD_CDC_DATA_PROTOCOL_VENDOR                 0xFFu    // Vendor-specific.

/********************************************************************************************************
 *                                   MANAGEMENT ELEMENTS REQUESTS DEFINES
 *
 * Note(s) : (1) Management elements requests are defined in table 19 from the CDC specification
 *               revision 1.2.
 *******************************************************************************************************/

#define  SL_USBD_CDC_REQ_SEND_ENCAP_COMM                  0x00u
#define  SL_USBD_CDC_REQ_GET_ENCAP_RESP                   0x01u
#define  SL_USBD_CDC_REQ_SET_COMM_FEATURE                 0x02u
#define  SL_USBD_CDC_REQ_GET_COMM_FEATURE                 0x03u
#define  SL_USBD_CDC_REQ_CLR_COMM_FEATURE                 0x04u
#define  SL_USBD_CDC_REQ_SET_AUX_LINE_STATE               0x10u
#define  SL_USBD_CDC_REQ_SET_HOOK_STATE                   0x11u
#define  SL_USBD_CDC_REQ_PULSE_SETUP                      0x12u
#define  SL_USBD_CDC_REQ_SEND_PULSE                       0x13u
#define  SL_USBD_CDC_REQ_SET_PULSE_TIME                   0x14u
#define  SL_USBD_CDC_REQ_RING_AUX_JACK                    0x15u
#define  SL_USBD_CDC_REQ_SET_LINE_CODING                  0x20u
#define  SL_USBD_CDC_REQ_GET_LINE_CODING                  0x21u
#define  SL_USBD_CDC_REQ_SET_CTRL_LINE_STATE              0x22u
#define  SL_USBD_CDC_REQ_SEND_BREAK                       0x23u
#define  SL_USBD_CDC_REQ_SET_RINGER_PAARMS                0x30u
#define  SL_USBD_CDC_REQ_GET_RINGER_PARAMS                0x31u
#define  SL_USBD_CDC_REQ_SET_OPERATION_PARAMS             0x32u
#define  SL_USBD_CDC_REQ_GET_OPERATION_PARAMS             0x33u
#define  SL_USBD_CDC_REQ_SET_LINE_PARAMS                  0x34u
#define  SL_USBD_CDC_REQ_GET_LINE_PARAMS                  0x35u
#define  SL_USBD_CDC_REQ_DIAL_DIGITS                      0x36u
#define  SL_USBD_CDC_REQ_SET_UNIT_PARAM                   0x37u
#define  SL_USBD_CDC_REQ_GET_UNIT_PARAM                   0x38u
#define  SL_USBD_CDC_REQ_CLR_UNUT_PARAM                   0x39u
#define  SL_USBD_CDC_REQ_GET_PROFILE                      0x3Au
#define  SL_USBD_CDC_REQ_SET_ETHER_MULTI_FILTER           0x40u
#define  SL_USBD_CDC_REQ_SET_ETHER_PWR_MGT_FILTER         0x41u
#define  SL_USBD_CDC_REQ_GET_ETHER_PWR_MGT_FILTER         0x42u
#define  SL_USBD_CDC_REQ_SET_ETHER_PKT_FILTER             0x43u
#define  SL_USBD_CDC_REQ_GET_ETHER_STAT                   0x44u
#define  SL_USBD_CDC_REQ_SET_ATM_DATA_FMT                 0x50u
#define  SL_USBD_CDC_REQ_GET_ATM_DATA_FMT                 0x51u
#define  SL_USBD_CDC_REQ_GET_ATM_DEV_STAT                 0x52u
#define  SL_USBD_CDC_REQ_SET_ATM_DFLT_VC                  0x53u
#define  SL_USBD_CDC_REQ_GET_ATM_VC_STAT                  0x54u
#define  SL_USBD_CDC_REQ_GET_NTB_PARAM2                   0x80u
#define  SL_USBD_CDC_REQ_GET_NET_ADDR                     0x81u
#define  SL_USBD_CDC_REQ_SET_NET_ADDR                     0x82u
#define  SL_USBD_CDC_REQ_GET_NTB_FMT                      0x83u
#define  SL_USBD_CDC_REQ_SET_NTB_FMT                      0x84u
#define  SL_USBD_CDC_REQ_GET_NTB_INPUT_SIZE               0x85u
#define  SL_USBD_CDC_REQ_SET_NTB_INPUT_SIZE               0x86u
#define  SL_USBD_CDC_REQ_GET_MAX_DATAGRAM_SIZE            0x87u
#define  SL_USBD_CDC_REQ_SET_MAX_DATAGRAM_SIZE            0x88u
#define  SL_USBD_CDC_REQ_GET_CRC_MODE                     0x89u
#define  SL_USBD_CDC_REQ_SET_CRC_MODE                     0x8Au

/********************************************************************************************************
 *                               CDC FUNCTIONAL DESCRIPTOR TYPE DEFINES
 *
 * Note(s) : (1) Functional descriptors types are defined in table 12 from the CDC specification revision
 *               1.2.
 *******************************************************************************************************/

#define  SL_USBD_CDC_DESC_TYPE_CS_IF                      0x24u
#define  SL_USBD_CDC_DESC_TYPE_CS_EP                      0x25u

/********************************************************************************************************
 *                               CDC FUNCTIONAL DESCRIPTOR SUBTYPE DEFINES
 *
 * Note(s) : (1) Functional descriptors subtypes are defined in table 13 from the CDC specification
 *               revision 1.2.
 *******************************************************************************************************/
//                                                                 Header functional descriptor, which marks the ...
//                                                                 ... beginning of the concatenated set of      ...
//                                                                 ... functional descriptors for the interface.
#define  SL_USBD_CDC_DESC_SUBTYPE_HEADER                  0x00u
#define  SL_USBD_CDC_DESC_SUBTYPE_CALL_MGMT               0x01u    // Call management                functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_ACM                     0x02u    // Abstract control management    functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_DLM                     0x03u    // Direct line management         functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_TEL_RINGER              0x04u    // Telephone Ringer               functional descriptor.
//                                                                 Telephone call and line state reporting ...
//                                                                 ... capabilities functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_TEL_CALL_LINE           0x05u
#define  SL_USBD_CDC_DESC_SUBTYPE_UNION                   0x06u    // Union functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_COUNTRY_SEL             0x07u    // Country selection              functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_TEL_OPERA               0x08u    // Telephone operational modes    functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_USB_TERM                0x09u    // USB terminal                   functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_NET_CH_TERM             0x0Au    // Network channel terminal descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_PROTOCOL_UNIT           0x0Bu    // protocol unit                  functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_EXT_UNIT                0x0Cu    // Extension unit                 functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_MULTI_CH_MGMT           0x0Du    // Multi-Channel management       functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_CAPI_CTRL               0x0Eu    // CAPI control management        functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_ETHER_NET               0x0Fu    // Ethernet networking            functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_ATM_NET                 0x10u    // ATM networking                 functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_WIRELESS_CTRL           0x11u    // Wireless handset control model functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_MOBILE DLM              0x12u    // Mobile Direct Line Model       functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_DETAIL                  0x13u    // MDLM Detail                    functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_DEV_MGMT                0x14u    // Device management model        functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_OBEX                    0x15u    // OBEX                           functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_CMD_SET                 0x16u    // Command set                    functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_CMD_SET_DETAIL          0x17u    // Command set detail             functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_TCM                     0x18u    // Telephone control model        functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_OBEX_SERV               0x19u    // OBEX service identifier        functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_NCM                     0x1Au    // NCM                            functional descriptor.
#define  SL_USBD_CDC_DESC_SUBTYPE_VENDOR                  0xFEu    // Vendor specific.

/********************************************************************************************************
 *                                       CDC NOTIFICATION DEFINES
 *******************************************************************************************************/

#define  SL_USBD_CDC_NOTIFICATION_HEADER                     8u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/
/// CDC Subclass Driver
typedef const struct {
  void (*enable)(void *p_subclass_arg);                         ///< Callback for enable event

  void (*disable)(void *p_subclass_arg);                        ///< Callback for disable event

  bool (*mgmt_req)(const sl_usbd_setup_req_t *p_setup_req,
                   void                      *p_subclass_arg);  ///< Callback to handle management requests.

  void (*notify_cmpl)(void       *p_subclass_arg);              ///< Callback to handle notification completion.

  void (*fnct_descriptor)(void       *p_subclass_arg,
                          uint8_t    first_dci_if_nbr);         ///< Callback to build functional desc.

  uint16_t (*fnct_get_descriptor_size)(void  *p_subclass_arg);  ///< Callback to get the size of the functional desc.
} sl_usbd_cdc_subclass_driver_t;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************************************
 *                                               CDC FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 * @brief    Initializes CDC class.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_init(void);

/****************************************************************************************************//**
 * @brief    Add a new instance of the CDC class.
 *
 * @param    subclass            Communication class subclass subcode (see Note #1).
 *
 * @param    p_subclass_drv      Pointer to the CDC subclass driver.
 *
 * @param    p_subclass_arg      Pointer to the CDC subclass driver argument.
 *
 * @param    protocol            Communication class protocol code.
 *
 * @param    notify_en           Notification enabled :
 *                               true   Enable  CDC class notifications.
 *                               false  Disable CDC class notifications.
 *
 * @param    notify_interval     Notification interval in milliseconds (must be a power of 2).
 *
 * @param    p_class_nbr         Pointer to a variable that will receive class instance number.
 *                               The variable is set to CDC class instance number if no errors are returned.,
 *                               otherwise it is set to SL_USBD_CDC_NBR_NONE.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Communication class subclass codes are defined in 'sl_usbd_class_cdc.h'
 *               'SL_USBD_CDC_SUBCLASS_XXXX'.
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_create_instance(uint8_t                       subclass,
                                        sl_usbd_cdc_subclass_driver_t *p_subclass_drv,
                                        void                          *p_subclass_arg,
                                        uint8_t                       protocol,
                                        bool                          notify_en,
                                        uint16_t                      notify_interval,
                                        uint8_t                       *p_class_nbr);

/****************************************************************************************************//**
 * @brief    Add a CDC instance into the USB device configuration.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    config_nbr  Configuration index to which to add the new CDC class interface.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_add_to_configuration(uint8_t  class_nbr,
                                             uint8_t  config_nbr);

/****************************************************************************************************//**
 * @brief    Get the CDC class enable state.
 *
 * @param    class_nbr    Class instance number.
 *
 * @param    p_enabled    Pointer to a boolean variable that will receive enable state.
 *                        The variable is set to true if CDC class is enabled.
 *                        The variable is set to false  if CDC class is not enabled.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_is_enabled(uint8_t  class_nbr,
                                   bool     *p_enabled);

/****************************************************************************************************//**
 * @brief    Add a data interface class to the CDC communication interface class.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    isoc_en     Data interface isochronous enable (see Note #1) :
 *                           - true   Data interface uses isochronous EPs.
 *                           - false  Data interface uses bulk        EPs.
 *
 * @param    protocol    Data interface protocol code.
 *
 * @param    p_if_nbr    Pointer to a variable that will receive data interface number.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) The value of 'isoc_en' must be false. Isochronous EPs are not supported.
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_add_data_interface(uint8_t  class_nbr,
                                           bool     isoc_en,
                                           uint8_t  protocol,
                                           uint8_t  *p_if_nbr);

/****************************************************************************************************//**
 * @brief    Receive data on the CDC data interface.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    data_if_nbr     CDC data interface number.
 *
 * @param    p_buf           Pointer to the destination buffer to receive data.
 *
 * @param    buf_len         Number of octets to receive.
 *
 * @param    timeout         Timeout in milliseconds.
 *
 * @param    p_xfer_len      Pointer to a variable that will receive transfer length.
 *                           If no errors are returned, the variable is set to number of octets received,
 *                           otherwise it is set to 0.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_read_data(uint8_t  class_nbr,
                                  uint8_t  data_if_nbr,
                                  uint8_t  *p_buf,
                                  uint32_t buf_len,
                                  uint16_t timeout,
                                  uint32_t *p_xfer_len);

/****************************************************************************************************//**
 * @brief    Send data on the CDC data interface.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    data_if_nbr     CDC data interface number.
 *
 * @param    p_buf           Pointer to the buffer of data that will be transmitted.
 *
 * @param    buf_len         Number of octets to transmit.
 *
 * @param    timeout         Timeout in milliseconds.
 *
 * @param    p_xfer_len      Pointer to a variable that will receive transfer length.
 *                           If no errors are returned, the variable is set to number of octets transmitted,
 *                           otherwise it is set to 0.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_write_data(uint8_t  class_nbr,
                                   uint8_t  data_if_nbr,
                                   uint8_t  *p_buf,
                                   uint32_t buf_len,
                                   uint16_t timeout,
                                   uint32_t *p_xfer_len);

/****************************************************************************************************//**
 * @brief    Send a communication interface class notification to the host.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    notification    Notification code.
 *
 * @param    value           Notification value.
 *
 * @param    p_buf           Pointer to the notification buffer (see Note #1).
 *
 * @param    data_len        Length of the data portion of the notification.
 *
 * @param    p_result        Pointer to a boolean variable that will receive the result of the operation.
 *                           The variable is set to true if no errors are encountered, otherwise
 *                           it is set to false.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) The notification buffer MUST contain space for the notification header
 *               'SL_USBD_CDC_NOTIFICATION_HEADER' plus the variable-length data portion.
 *******************************************************************************************************/
sl_status_t sl_usbd_cdc_notify_host(uint8_t   class_nbr,
                                    uint8_t   notification,
                                    uint16_t  value,
                                    uint8_t   *p_buf,
                                    uint16_t  data_len,
                                    bool      *p_result);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
