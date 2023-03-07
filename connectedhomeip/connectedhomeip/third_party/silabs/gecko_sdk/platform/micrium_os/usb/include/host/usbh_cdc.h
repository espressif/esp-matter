/***************************************************************************//**
 * @file
 * @brief USB Host - Communications Device Class (CDC)
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
 * @defgroup USBH_CDC USB Host CDC API
 * @ingroup USBH
 * @brief   USB Host CDC API
 *
 * @addtogroup USBH_CDC
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBH_CDC_H_
#define  _USBH_CDC_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_class.h>
#include  <usb/include/host/usbh_core_handle.h>
#include  <usb/include/host/usbh_core_utils.h>
#include  <usb/include/host/usbh_core_opt_def.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  USBH_CDC_STD_REQ_TIMEOUT_DFLT                  5000u

/********************************************************************************************************
 *                           SUBCLASS CODES FOR COMMUNICATION INTERFACE CLASS
 *
 * Note(s) : (1) See "USB Class Definitiopns for Communication Devices Specification", version 1.2,
 *               Section 4.3, Table 4.
 *******************************************************************************************************/

#define  USBH_CDC_CTRL_SUBCLASS_CODE_DLCM               0x01u   // Direct Line         Control Model.
#define  USBH_CDC_CTRL_SUBCLASS_CODE_ACM                0x02u   // Abstract            Control Model.
#define  USBH_CDC_CTRL_SUBCLASS_CODE_TCM                0x03u   // Telephone           Control Model.
#define  USBH_CDC_CTRL_SUBCLASS_CODE_MCCM               0x04u   // Multi-Channel       Control Model.
#define  USBH_CDC_CTRL_SUBCLASS_CODE_CAPICM             0x05u   // CAPI                Control Model.
#define  USBH_CDC_CTRL_SUBCLASS_CODE_ENCM               0x06u   // Ethernet Networking Control Model.
#define  USBH_CDC_CTRL_SUBCLASS_CODE_ATMNCM             0x07u   // ATM Networking      Control Model.
#define  USBH_CDC_CTRL_SUBCLASS_CODE_WHCM               0x08u   // Wireless Handset    Control Model.
#define  USBH_CDC_CTRL_SUBCLASS_CODE_DEV_MGMT           0x09u   // Device Management.
#define  USBH_CDC_CTRL_SUBCLASS_CODE_MDLM               0x0Au   // Mobile Direct Line Model.
#define  USBH_CDC_CTRL_SUBCLASS_CODE_OBEX               0x0Bu   // OBEX.
#define  USBH_CDC_CTRL_SUBCLASS_CODE_EEM                0x0Cu   // Ethernet Emulation         Model.
#define  USBH_CDC_CTRL_SUBCLASS_CODE_NCM                0x0Du   // Network Control            Model.
#define  USBH_CDC_CTRL_SUBCLASS_CODE_MBIM               0x0Eu   // Mobile Broadband Interface Model.

/********************************************************************************************************
 *                           PROTOCOL CODES FOR COMMUNICATION INTERFACE CLASS
 *
 * Note(s) : (1) See "USB Class Definitiopns for Communication Devices Specification", version 1.2,
 *               Section 4.4, Table 5.
 *******************************************************************************************************/

#define  USBH_CDC_CTRL_PROTOCOL_CODE_USB                0x00u   // No class specific protocol.
#define  USBH_CDC_CTRL_PROTOCOL_CODE_AT_V25             0x01u   // AT Commands V.250 etc.
#define  USBH_CDC_CTRL_PROTOCOL_CODE_AT_PCCA101         0x02u   // AT Commands defined by PCCA-101.
#define  USBH_CDC_CTRL_PROTOCOL_CODE_AT_PCCA101_AO      0x03u   // AT Commands defined by PCCA-101 & annex O.
#define  USBH_CDC_CTRL_PROTOCOL_CODE_AT_GSM0707         0x04u   // AT Commands defined by GSM 07.07.
#define  USBH_CDC_CTRL_PROTOCOL_CODE_AT_3GPP27007       0x05u   // AT Commands defined by 3GPP 27.007.
#define  USBH_CDC_CTRL_PROTOCOL_CODE_AT_TIACDMA         0x06u   // AT Commands defined by TIA for CDMA.
#define  USBH_CDC_CTRL_PROTOCOL_CODE_EEM                0x07u   // Ethernet Emulation Model.
#define  USBH_CDC_CTRL_PROTOCOL_CODE_EXT                0xFEu   // External Protocol.
#define  USBH_CDC_CTRL_PROTOCOL_CODE_VENDOR             0xFFu   // Vendor-Specific.

/********************************************************************************************************
 *                               PROTOCOL CODES FOR DATA INTERFACE CLASS
 *
 * Note(s) : (1) See "USB Class Definitiopns for Communication Devices Specification", version 1.2,
 *               Section 4.7, Table 7.
 *******************************************************************************************************/

#define  USBH_CDC_DATA_PROTOCOL_CODE_USB                0x00u   // No class specific protocol.
#define  USBH_CDC_DATA_PROTOCOL_CODE_NTB                0x01u   // Network Transfer Block.
#define  USBH_CDC_DATA_PROTOCOL_CODE_NTB_IPDSS          0x02u   // Network Transfer Block (IP + DSS).
#define  USBH_CDC_DATA_PROTOCOL_CODE_ISDN_BRI           0x30u   // Physical Interface Protocol for ISDN BRI.
#define  USBH_CDC_DATA_PROTOCOL_CODE_HDLC               0x31u   // HDLC.
#define  USBH_CDC_DATA_PROTOCOL_CODE_TRANSPARENT        0x32u   // Transparent.
#define  USBH_CDC_DATA_PROTOCOL_CODE_Q_921M             0x50u   // Management Protocol for Q.921.
#define  USBH_CDC_DATA_PROTOCOL_CODE_Q_921              0x51u   // Data Link  Protocol for Q.921.
#define  USBH_CDC_DATA_PROTOCOL_CODE_Q921TM             0x52u   // TEI-multiplexor for Q.921 Data Link Protocol.
#define  USBH_CDC_DATA_PROTOCOL_CODE_V42BIS             0x90u   // Data Compression Procedures.
#define  USBH_CDC_DATA_PROTOCOL_CODE_E_ISDN             0x91u   // Euro-ISDN Protocol Control.
#define  USBH_CDC_DATA_PROTOCOL_CODE_V24_ISDN           0x92u   // V.24 Rate Adaptation to ISDN.
#define  USBH_CDC_DATA_PROTOCOL_CODE_CAPI               0x93u   // CAPI Commands.
#define  USBH_CDC_DATA_PROTOCOL_CODE_HOST               0xFDu   // Host Based Driver.
#define  USBH_CDC_DATA_PROTOCOL_CODE_CDC                0xFEu   // Protocol(s) described using PUF Desc on CCI.
#define  USBH_CDC_DATA_PROTOCOL_CODE_VENDOR             0xFFu   // Vendor-Specific.

/********************************************************************************************************
 *                                           CDC RELEASE VERSIONS
 *
 * Note(s) : (1) CDC release version is reported via the header functional descriptor.
 *******************************************************************************************************/

#define  USBH_CDC_REL_NBR_1_09                        0x0109u
#define  USBH_CDC_REL_NBR_1_0                         0x0100u
#define  USBH_CDC_REL_NBR_1_1                         0x0110u
#define  USBH_CDC_REL_NBR_1_2                         0x0120u

/********************************************************************************************************
 *                                       CLASS-SPECIFIC REQUEST CODES
 *
 * Note(s) : (1) For more information on CDC specific request codes, see 'USB Class Definitions for
 *               Communication Devices Specification", version 1.2, Section 6.2, Table 19'.
 *******************************************************************************************************/

#define  USBH_CDC_REQ_SEND_ENCAPSULATED_CMD             0x00u
#define  USBH_CDC_REQ_GET_ENCAPSULATED_RESP             0x01u
#define  USBH_CDC_REQ_SET_COMM_FEATURE                  0x02u
#define  USBH_CDC_REQ_GET_COMM_FEATURE                  0x03u
#define  USBH_CDC_REQ_CLR_COMM_FEATURE                  0x04u
#define  USBH_CDC_RESET_FNCT                            0x05u

#define  USBH_CDC_REQ_SET_AUX_LINE_STATE                0x10u
#define  USBH_CDC_REQ_SET_HOOK_STATE                    0x11u
#define  USBH_CDC_REQ_PULSE_SETUP                       0x12u
#define  USBH_CDC_REQ_SEND_PULSE                        0x13u
#define  USBH_CDC_REQ_SET_PULSE_TIME                    0x14u
#define  USBH_CDC_REQ_RING_AUX_JACK                     0x15u

#define  USBH_CDC_REQ_SET_LINE_CODING                   0x20u
#define  USBH_CDC_REQ_GET_LINE_CODING                   0x21u
#define  USBH_CDC_REQ_SET_CTRL_LINESTATE                0x22u
#define  USBH_CDC_REQ_SEND_BREAK                        0x23u

#define  USBH_CDC_REQ_SET_RINGER_PARMS                  0x30u
#define  USBH_CDC_REQ_GET_RINGER_PARMS                  0x31u
#define  USBH_CDC_REQ_SET_OPER_PARMS                    0x32u
#define  USBH_CDC_REQ_GET_OPER_PARMS                    0x33u
#define  USBH_CDC_REQ_SET_LINE_PARMS                    0x34u
#define  USBH_CDC_REQ_GET_LINE_PARMS                    0x35u
#define  USBH_CDC_REQ_DIAL_DIG                          0x36u
#define  USBH_CDC_REQ_SET_UNIT_PARAM                    0x37u
#define  USBH_CDC_REQ_GET_UNIT_PARAM                    0x38u
#define  USBH_CDC_REQ_CLR_UNIT_PARAM                    0x39u
#define  USBH_CDC_REQ_GET_PROFILE                       0x3Au

#define  USBH_CDC_REQ_SET_ETHER_MULTICAST_FILTERS       0x40u
#define  USBH_CDC_REQ_SET_ETHER_PWR_MGMT_PATTERN_FILTER 0x41u
#define  USBH_CDC_REQ_GET_ETHER_PWR_MGMT_PATTERN_FILTER 0x42u
#define  USBH_CDC_REQ_SET_ETHER_PKT_FILTER              0x43u
#define  USBH_CDC_REQ_GET_ETHER_STAT                    0x44u

#define  USBH_CDC_REQ_SET_ATM_DATA_FMT                  0x50u
#define  USBH_CDC_REQ_SET_ATM_DEV_STAT                  0x51u
#define  USBH_CDC_REQ_SET_ATM_DFLT_VC                   0x52u
#define  USBH_CDC_REQ_SET_ATM_VC_STAT                   0x53u

#define  USBH_CDC_REQ_GET_NTB_PARAMS                    0x80u
#define  USBH_CDC_REQ_GET_NET_ADDR                      0x81u
#define  USBH_CDC_REQ_SET_NET_ADDR                      0x82u
#define  USBH_CDC_REQ_GET_NTB_FMT                       0x83u
#define  USBH_CDC_REQ_SET_NTB_FMT                       0x84u
#define  USBH_CDC_REQ_GET_NTB_INPUT_SIZE                0x85u
#define  USBH_CDC_REQ_SET_NTB_INPUT_SIZE                0x86u
#define  USBH_CDC_REQ_GET_MAX_DATAGRAM_SIZE             0x87u
#define  USBH_CDC_REQ_SET_MAX_DATAGRAM_SIZE             0x88u
#define  USBH_CDC_REQ_GET_CRC_MODE                      0x89u
#define  USBH_CDC_REQ_SET_CRC_MODE                      0x8Au

/********************************************************************************************************
 *                               CLASS-SPECIFIC REQUEST DATA PHASE LENGTH
 *******************************************************************************************************/

#define  USBH_CDC_REQ_LEN_SET_COMM_FEATURE                 2u
#define  USBH_CDC_REQ_LEN_GET_COMM_FEATURE                 2u
#define  USBH_CDC_REQ_LEN_SET_LINE_CODING                  7u
#define  USBH_CDC_REQ_LEN_GET_LINE_CODING                  7u

/********************************************************************************************************
 *                                           COMM FEATURE TYPES
 *
 * Note(s) : (1) See 'Communications Class Subclass Specification for PSTN Devices, version 1.2, Section
 *               6.3.2, Table 14'.
 *******************************************************************************************************/

#define  USBH_CDC_COMM_FEATURE_ABSTRACT_STATE           0x01u
#define  USBH_CDC_COMM_FEATURE_COUNTRY_SETTING          0x02u

/********************************************************************************************************
 *                                       COMM FEATURE ABSTRACT STATES
 *
 * Note(s) : (1) See 'Communications Class Subclass Specification for PSTN Devices, version 1.2, Section
 *               6.3.2, Table 15'.
 *******************************************************************************************************/

#define  USBH_CDC_COMM_FEATURE_ABSTRACT_STATE_IDLE      DEF_BIT_00
#define  USBH_CDC_COMM_FEATURE_ABSTRACT_STATE_DATA_MUX  DEF_BIT_01

/********************************************************************************************************
 *                                       SET/GET LINE CODING VALUES
 *
 * Note(s) : (1) See "Universal Serial Bus Communications Class Subclass Specification for PSTN Devices",
 *               version 1.2, February 9, 2007. Section 6.3.11 Table 17.
 *******************************************************************************************************/
//                                                                 Baud rate (in baud/sec).
#define  USBH_CDC_LINECODING_RATE_110                    110u
#define  USBH_CDC_LINECODING_RATE_300                    300u
#define  USBH_CDC_LINECODING_RATE_1200                  1200u
#define  USBH_CDC_LINECODING_RATE_2400                  2400u
#define  USBH_CDC_LINECODING_RATE_4800                  4800u
#define  USBH_CDC_LINECODING_RATE_9600                  9600u
#define  USBH_CDC_LINECODING_RATE_19200                19200u
#define  USBH_CDC_LINECODING_RATE_38400                38400u
#define  USBH_CDC_LINECODING_RATE_56700                56700u
#define  USBH_CDC_LINECODING_RATE_115200              115200u
#define  USBH_CDC_LINECODING_RATE_230400              230400u
#define  USBH_CDC_LINECODING_RATE_460800              460800u
#define  USBH_CDC_LINECODING_RATE_921600              921600u

//                                                                 Stop bit.
#define  USBH_CDC_LINECODING_STOP_BIT_1                    0u
#define  USBH_CDC_LINECODING_STOP_BIT_1_5                  1u
#define  USBH_CDC_LINECODING_STOP_BIT_2                    2u

//                                                                 Parity check type.
#define  USBH_CDC_LINECODING_PARITY_NONE                   0u
#define  USBH_CDC_LINECODING_PARITY_ODD                    1u
#define  USBH_CDC_LINECODING_PARITY_EVEN                   2u
#define  USBH_CDC_LINECODING_PARITY_MARK                   3u
#define  USBH_CDC_LINECODING_PARITY_SPACE                  4u

//                                                                 Data bit qty.
#define  USBH_CDC_LINECODING_DATA_BIT_5                    5u
#define  USBH_CDC_LINECODING_DATA_BIT_6                    6u
#define  USBH_CDC_LINECODING_DATA_BIT_7                    7u
#define  USBH_CDC_LINECODING_DATA_BIT_8                    8u

/********************************************************************************************************
 *                                       SET CONTROL LINE STATE BITS
 *
 * Note(s) : (1) See "Universal Serial Bus Communications Class Subclass Specification for PSTN Devices",
 *               version 1.2, February 9, 2007. Section 6.3.13 Table 18.
 *******************************************************************************************************/

#define  USBH_CDC_CTRL_LINE_STATE_DTR                   DEF_BIT_00
#define  USBH_CDC_CTRL_LINE_STATE_CARRIER               DEF_BIT_01

/********************************************************************************************************
 *                                       CONTROL LINE STATE BITS
 *
 * Note(s) : (1) See 'Communications Class Subclass Specification for PSTN Devices, version 1.2, Section
 *               6.3.13, Table 18'.
 *******************************************************************************************************/

#define  USBH_CDC_CTRL_LINE_STATE_DTE                   DEF_BIT_00
#define  USBH_CDC_CTRL_LINE_STATE_CARRIER               DEF_BIT_01

/********************************************************************************************************
 *                                           SERIAL STATE BITS
 *
 * Note(s) : (1) See 'Communications Class Subclass Specification for PSTN Devices, version 1.2, Section
 *               6.5.4, Table 31'.
 *******************************************************************************************************/

#define  USBH_CDC_SERIAL_STATE_RXCARRIER                DEF_BIT_00
#define  USBH_CDC_SERIAL_STATE_TXCARRIER                DEF_BIT_01
#define  USBH_CDC_SERIAL_STATE_BRK                      DEF_BIT_02
#define  USBH_CDC_SERIAL_STATE_RINGSIGNAL               DEF_BIT_03
#define  USBH_CDC_SERIAL_STATE_FRAMING                  DEF_BIT_04
#define  USBH_CDC_SERIAL_STATE_PARITY                   DEF_BIT_05
#define  USBH_CDC_SERIAL_STATE_OVERRUN                  DEF_BIT_06

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           CDC FUNCTION HANDLE
 *******************************************************************************************************/

typedef USBH_CLASS_FNCT_HANDLE USBH_CDC_FNCT_HANDLE;

/****************************************************************************************************//**
 *                                       CDC CONFIGURATION STRUCTURES
 *
 * @note     (1) Can be DEF_NULL if USBH_CFG_OPTIMIZE_SPD_EN is set to DEF_DISABLED
 *
 * @note     (2) Can be DEF_NULL if USBH_CFG_INIT_ALLOC_EN is set to DEF_DISABLED
 *******************************************************************************************************/

//                                                                 -- USB HOST CDC CLASS OPTIMIZE SPD CONFIGURATIONS --
typedef struct usbh_cdc_cfg_optimize_spd {
  CPU_INT08U FnctQty;                                           ///< Quantity of CDC functions.
  CPU_INT08U IF_PerFnctQty;                                     ///< Quantity of IF per CDC function (including CCI).
} USBH_CDC_CFG_OPTIMIZE_SPD;

//                                                                 --- USB HOST CDC CLASS INIT ALLOC CONFIGURATIONS ---
typedef struct usbh_cdc_cfg_init_alloc {
  CPU_INT08U FnctQty;                                           ///< Quantity of CDC functions (total).
  CPU_INT08U AsyncXferQty;                                      ///< Quantity of async xfer on all DCI at any time.
  CPU_INT16U DCI_Qty;                                           ///< Quantity of DCI (total).
} USBH_CDC_CFG_INIT_ALLOC;

//                                                                 ------------- CDC INIT CONFIGURATIONS --------------
typedef struct usbh_cdc_init_cfg {
  CPU_SIZE_T                BufAlignOctets;                     ///< Indicates desired mem alignment for internal buf.
  CPU_INT08U                EventURB_Qty;                       ///< Quantity of event URB to submit per CDC function.

  MEM_SEG                   *MemSegPtr;                         ///< Ptr to mem segment to use when allocating ctrl data.
  MEM_SEG                   *MemSegBufPtr;                      ///< Ptr to mem segment to use when allocating data buf.

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  USBH_CDC_CFG_OPTIMIZE_SPD OptimizeSpd;                        ///< Configurations for optimize speed mode.
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  USBH_CDC_CFG_INIT_ALLOC InitAlloc;                            ///< Configurations for allocations at init mode.
#endif
} USBH_CDC_INIT_CFG;

/****************************************************************************************************//**
 *                                           CDC LINE CODING
 *
 * @note     (1) Values to be set here are defined in 'Communications Class Subclass Specification for
 *               PSTN Devices, version 1.2, Section 6.3.11 Table 17.'
 *
 * @note     (2) Possible values are:
 *               - (a) for 'Rate'
 *                   - USBH_CDC_LINECODING_RATE_110
 *                   - USBH_CDC_LINECODING_RATE_300
 *                   - USBH_CDC_LINECODING_RATE_1200
 *                   - USBH_CDC_LINECODING_RATE_2400
 *                   - USBH_CDC_LINECODING_RATE_4800
 *                   - USBH_CDC_LINECODING_RATE_9600
 *                   - USBH_CDC_LINECODING_RATE_19200
 *                   - USBH_CDC_LINECODING_RATE_38400
 *                   - USBH_CDC_LINECODING_RATE_56700
 *                   - USBH_CDC_LINECODING_RATE_115200
 *                   - USBH_CDC_LINECODING_RATE_230400
 *                   - USBH_CDC_LINECODING_RATE_460800
 *                   - USBH_CDC_LINECODING_RATE_921600
 *               - (b) for 'CharFmt'
 *                   - USBH_CDC_LINECODING_STOP_BIT_1
 *                   - USBH_CDC_LINECODING_STOP_BIT_1_5
 *                   - USBH_CDC_LINECODING_STOP_BIT_2
 *               - (c) for 'ParityType'
 *                   - USBH_CDC_LINECODING_PARITY_NONE
 *                   - USBH_CDC_LINECODING_PARITY_ODD
 *                   - USBH_CDC_LINECODING_PARITY_EVEN
 *                   - USBH_CDC_LINECODING_PARITY_MARK
 *                   - USBH_CDC_LINECODING_PARITY_SPACE
 *               - (d) for 'DataBit'
 *                   - USBH_CDC_LINECODING_DATA_BIT_5
 *                   - USBH_CDC_LINECODING_DATA_BIT_6
 *                   - USBH_CDC_LINECODING_DATA_BIT_7
 *                   - USBH_CDC_LINECODING_DATA_BIT_8
 *******************************************************************************************************/

typedef struct usbh_cdc_linecoding {
  CPU_INT32U Rate;
  CPU_INT08U CharFmt;
  CPU_INT08U ParityType;
  CPU_INT08U DataBit;
} USBH_CDC_LINECODING;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

extern const USBH_CDC_INIT_CFG USBH_CDC_InitCfgDflt;            // USBH CDC class dflt configurations.

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
void USBH_CDC_ConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets);

void USBH_CDC_ConfigureEventURB_Qty(CPU_INT08U event_urb_qty);

void USBH_CDC_ConfigureMemSeg(MEM_SEG *p_mem_seg,
                              MEM_SEG *p_mem_seg_buf);

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
void USBH_CDC_ConfigureOptimizeSpdCfg(const USBH_CDC_CFG_OPTIMIZE_SPD *p_optimize_spd_cfg);
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
void USBH_CDC_ConfigureInitAllocCfg(const USBH_CDC_CFG_INIT_ALLOC *p_init_alloc_cfg);
#endif
#endif

/********************************************************************************************************
 *                                               CDC FUNCTIONS
 *******************************************************************************************************/

void USBH_CDC_Init(RTOS_ERR *p_err);

void USBH_CDC_StdReqTimeoutSet(CPU_INT32U std_req_timeout_ms,
                               RTOS_ERR   *p_err);

void USBH_CDC_PostInit(RTOS_ERR *p_err);

USBH_DEV_HANDLE USBH_CDC_DevHandleGet(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                                      RTOS_ERR             *p_err);

USBH_FNCT_HANDLE USBH_CDC_FnctHandleGet(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                                        RTOS_ERR             *p_err);

CPU_INT08U USBH_CDC_DCI_QtyGet(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                               RTOS_ERR             *p_err);

CPU_INT16U USBH_CDC_RelNbrGet(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                              RTOS_ERR             *p_err);

CPU_INT16U USBH_CDC_EncapsulatedCmdTx(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                                      CPU_INT08U           *p_buf,
                                      CPU_INT16U           buf_len,
                                      CPU_INT32U           timeout,
                                      RTOS_ERR             *p_err);

CPU_INT16U USBH_CDC_EncapsulatedRespRx(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                                       CPU_INT08U           *p_buf,
                                       CPU_INT16U           buf_len,
                                       CPU_INT32U           timeout,
                                       RTOS_ERR             *p_err);

void USBH_CDC_CommFeatureSet(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                             CPU_INT08U           feature,
                             CPU_INT16U           data,
                             CPU_INT32U           timeout,
                             RTOS_ERR             *p_err);

CPU_INT16U USBH_CDC_CommFeatureGet(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                                   CPU_INT08U           feature,
                                   CPU_INT32U           timeout,
                                   RTOS_ERR             *p_err);

void USBH_CDC_CommFeatureClr(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                             CPU_INT08U           feature,
                             CPU_INT32U           timeout,
                             RTOS_ERR             *p_err);

void USBH_CDC_LineCodingSet(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                            USBH_CDC_LINECODING  *p_line_coding,
                            CPU_INT32U           timeout,
                            RTOS_ERR             *p_err);

void USBH_CDC_LineCodingGet(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                            USBH_CDC_LINECODING  *p_line_coding,
                            CPU_INT32U           timeout,
                            RTOS_ERR             *p_err);

void USBH_CDC_CtrlLineStateSet(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                               CPU_INT16U           ctrl_signal,
                               CPU_INT32U           timeout,
                               RTOS_ERR             *p_err);

void USBH_CDC_BrkSend(USBH_CDC_FNCT_HANDLE cdc_fnct_handle,
                      CPU_INT16U           dur,
                      CPU_INT32U           timeout,
                      RTOS_ERR             *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                           MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
