/******************************************************************************

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated

 All rights reserved not granted herein.
 Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free,
 non-exclusive license under copyrights and patents it now or hereafter
 owns or controls to make, have made, use, import, offer to sell and sell
 ("Utilize") this software subject to the terms herein. With respect to the
 foregoing patent license, such license is granted solely to the extent that
 any such patent is necessary to Utilize the software alone. The patent
 license shall not apply to any combinations which include this software,
 other than combinations with devices manufactured by or for TI ("TI
 Devices"). No hardware patent is licensed hereunder.

 Redistributions must preserve existing copyright notices and reproduce
 this license (including the above copyright notice and the disclaimer and
 (if applicable) source code license limitations below) in the documentation
 and/or other materials provided with the distribution.

 Redistribution and use in binary form, without modification, are permitted
 provided that the following conditions are met:

   * No reverse engineering, decompilation, or disassembly of this software
     is permitted with respect to any software provided in binary form.
   * Any redistribution and use are licensed by TI for use only with TI Devices.
   * Nothing shall obligate TI to provide you with source code for the software
     licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution
 of the source code are permitted provided that the following conditions are
 met:

   * Any redistribution and use of the source code, including any resulting
     derivative works, are licensed by TI for use only with TI Devices.
   * Any redistribution and use of any object code compiled from the source
     code and any resulting derivative works, are licensed by TI for use
     only with TI Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its
 suppliers may be used to endorse or promote products derived from this
 software without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/**
 *  @defgroup L2CAP L2CAP
 *  @brief This module implements the Logical Link Control and Adaption Protocol
 *  @{
 *  @file  l2cap.h
 *  @brief      L2CAP layer interface
 */

#ifndef L2CAP_H
#define L2CAP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "osal.h"

/*********************************************************************
 * CONSTANTS
 */

/**
 * @defgroup L2CAP_CONSTANTS L2CAP Constants
 * @{
 */

#define L2CAP_MTU_SIZE                   23 //!< Minimum supported information payload for the Basic information frame (B-frame)

#define L2CAP_SDU_SIZE                   65527 //!< Maximum SDU size (PSM's MTU) that can be received on a Connection Oriented Channel is 65527.

#define L2CAP_NUM_PSM_DEFAULT            3 //!< Default maximum number of L2CAP Protocol/Service Multiplexers (PSM)

#define L2CAP_NUM_CO_CHANNELS_DEFAULT    3 //!< Default maximum number of L2CAP Connection Oriented Channels

#define L2CAP_SIG_MTU_SIZE               23 //!< Minimum supported information payload for the Control frame (C-frame)

#define L2CAP_HDR_SIZE                   4 //!< Basic L2CAP header: Length (2 bytes) + Channel ID (2 bytes)

/**
 * @brief L2CAP PDU Size
 *
 * Minimum size of PDU received from lower layer protocol (incoming
 * packet), or delivered to lower layer protocol (outgoing packet).
 */
#define L2CAP_PDU_SIZE                   ( L2CAP_HDR_SIZE + L2CAP_MTU_SIZE )

#define L2CAP_LEN_FIELD_SIZE             2 //!< SDU Length field size

/**
 * @defgroup L2CAP_PSM LE Protocol/Service Multiplexer values
 * @{
 * LE PSM values are separated into
 * two ranges. Values in the first range are assigned by the Bluetooth SIG
 * and indicate protocols. Values in the second range are dynamically
 * allocated and used in conjunction with services defined in the GATT server.
 */
#define L2CAP_INVALID_PSM                0x0000 //!< Invalid PSM

#define L2CAP_FIXED_PSM_MIN              0x0001 //!< Fixed LE PSM min
#define L2CAP_FIXED_PSM_MAX              0x007F //!< Fixed LE PSM max

#define L2CAP_DYNAMIC_PSM_MIN            0x0080 //!< Dynamic PSM min
#define L2CAP_DYNAMIC_PSM_MAX            0x00FF //!< Dynamic PSM max

/** @} End L2CAP_PSM */

/**
 * @defgroup L2CAP_CIDS L2CAP Channel Identifiers
 * @{
 * Identifiers from 0x0001 to 0x003F are
 * reserved for specific L2CAP functions. Identifiers 0x0001-0x0003
 * are reserved by BR/EDR.
 */
#define L2CAP_CID_NULL                   0x0000 //!< Illegal Identifier

#define L2CAP_CID_ATT                    0x0004 //!< Attribute Protocol
#define L2CAP_CID_SIG                    0x0005 //!< L2CAP Signaling
#define L2CAP_CID_SMP                    0x0006 //!< Security Management Protocol
#define L2CAP_CID_GENERIC                0x0007 //!< Generic (proprietary channel)

#define L2CAP_DYNAMIC_CID_MIN            0x0040 //!< Dynamic Channel Identifier Min
#define L2CAP_DYNAMIC_CID_MAX            0x007F //!< Dynamic Channel Identifier Max

#define L2CAP_NUM_FIXED_CHANNELS         4 //!< Number of Fixed channels: one for each of ATT, Signaling, SMP and Generic channels

#define L2CAP_NUM_SIG_COMMANDS           1 //!< Number of Signaling Commands: one for Connection Parameter Update Request

/** @} End L2CAP_CIDS */

/**
 * @brief L2CAP Response Timeout Expired (RTX)
 *
 * The RTX value is for Signaling commands (in seconds).
 * The RTX timer is used for response timeout or to terminate a dynamic channel
 * when the remote device is unresponsive to signaling requests. Its value may
 * range from 1 to 60 seconds.
 */
#define L2CAP_RTX_TIMEOUT                30

/**
 * @defgroup L2CAP_SIGNALING_CODES_DEFINES L2CAP Signaling Codes (type of commands)
 * @{
 */
#define L2CAP_CMD_REJECT                 0x01 //!< Command Reject
#define L2CAP_DISCONNECT_REQ             0x06 //!< Disconnection Request
#define L2CAP_DISCONNECT_RSP             0x07 //!< Disconnection Response
#define L2CAP_INFO_REQ                   0x0a //!< Information Request - not supported
#define L2CAP_INFO_RSP                   0x0b //!< Information Response - not supported
#define L2CAP_PARAM_UPDATE_REQ           0x12 //!< Connection Parameter Update Request
#define L2CAP_PARAM_UPDATE_RSP           0x13 //!< Connection Parameter Update Response
#define L2CAP_CONNECT_IND                0x14 //!< LE Credit Based Connection Indication
#define L2CAP_CONNECT_RSP                0x15 //!< LE Credit Based Connection Response
#define L2CAP_FLOW_CTRL_CREDIT           0x16 //!< LE Flow Control Credit

/** @} End L2CAP_SIGNALING_CODES_DEFINES */

/**
 * @defgroup L2CAP_EVENT_CODES_DEFINES L2CAP Proprietary Event Codes (type of events)
 * @{
 */

#define L2CAP_CHANNEL_ESTABLISHED_EVT    0x60 //!< Channel Established Event
#define L2CAP_CHANNEL_TERMINATED_EVT     0x61 //!< Channel Terminated Event
#define L2CAP_OUT_OF_CREDIT_EVT          0x62 //!< Out Of Credit Event
#define L2CAP_PEER_CREDIT_THRESHOLD_EVT  0x63 //!< Peer Credit Threshold Event
#define L2CAP_SEND_SDU_DONE_EVT          0x64 //!< Send SDU Done Event
#define L2CAP_NUM_CTRL_DATA_PKT_EVT      0x65 //!< Available Ctrl Data Packets Event

/** @} End L2CAP_EVENT_CODES_DEFINES */

/**
 * @defgroup L2CAP_REJECT_REASON_CODES L2CAP Command Reject Reason Codes
 * @{
 */

#define L2CAP_REJECT_CMD_NOT_UNDERSTOOD  0x0000 //!< Command not understood
#define L2CAP_REJECT_SIGNAL_MTU_EXCEED   0x0001 //!< Signaling MTU exceeded
#define L2CAP_REJECT_INVALID_CID         0x0002 //!< Invalid CID in request

/** @} End L2CAP_REJECT_REASON_CODES */

/**
 * @defgroup L2CAP_CONN_RESULT_VALUES L2CAP Connection Response Result Values
 * @{
 */

#define L2CAP_CONN_SUCCESS               0x0000 //!< Connection successful
#define L2CAP_CONN_PSM_NOT_SUPPORTED     0x0002 //!< Connection refused - LE_PSM not supported
#define L2CAP_CONN_NO_RESOURCES          0x0004 //!< Connection refused - no resources available
#define L2CAP_CONN_INSUFFICIENT_AUTHEN   0x0005 //!< Connection refused - insufficient authentication
#define L2CAP_CONN_INSUFFICIENT_AUTHOR   0x0006 //!< Connection refused - insufficient authorization
#define L2CAP_CONN_INSUFFICIENT_KEY_SIZE 0x0007 //!< Connection refused - insufficient encryption key size
#define L2CAP_CONN_INSUFFICIENT_ENCRYPT  0x0008 //!< Connection refused - insufficient encryption
#define L2CAP_CONN_INVALID_SRC_CID       0x0009 //!< Connection refused - invalid Source CID
#define L2CAP_CONN_SRC_CID_ALREADY_ALLOC 0x000A //!< Connection refused - Source CID already allocated
#define L2CAP_CONN_UNACCEPTABLE_PARAMS   0x000B //!< Connection refused - unacceptable parameters

#define L2CAP_CONN_PENDING_SEC_VERIFY    0xFFFF //!< Connection security verification pending (used locally). Note - PSM must send back a Connection Response

/** @} End L2CAP_CONN_RESULT_VALUES */

/**
 * @defgroup L2CAP_PARAMS_RESULT_VALUES L2CAP Connection Parameter Update Response Result Values
 * @{
 */

#define L2CAP_CONN_PARAMS_ACCEPTED       0x0000 //!< Connection Parameters accepted
#define L2CAP_CONN_PARAMS_REJECTED       0x0001 //!< Connection Parameters rejected

/** @} End L2CAP_PARAMS_RESULT_VALUES */

/**
 * @defgroup L2CAP_TERM_REASON_VALUES L2CAP Channel Terminated Event Reason Values
 * @{
 */

#define L2CAP_TERM_LINK_DOWN             0x0000 //!< Physical link gone down
#define L2CAP_TERM_BY_PSM                0x0001 //!< Channel terminated by local PSM
#define L2CAP_TERM_BY_PEER_PSM           0x0002 //!< Channel terminated by peer PSM
#define L2CAP_TERM_MAX_CREDIT_EXCEED     0x0003 //!< Credit count exceeded 65535
#define L2CAP_TERM_SDU_LEN_EXCEED        0x0004 //!< Total length of payload received exceeded SDU length specified in first LE-frame of SDU
#define L2CAP_TERM_PSM_MTU_EXCEED        0x0005 //!< Total length of payload received exceeded local PSM's MTU
#define L2CAP_TERM_RX_PKT_NO_CREDIT      0x0006 //!< LE-frame was received from peer device that has a credit of count zero
#define L2CAP_TERM_RX_ERROR              0x0007 //!< LE-frame was received with error
#define L2CAP_TERM_TX_ERROR              0x0008 //!< Error happened while trying to send LE-frame
#define L2CAP_TERM_MEM_ALLOC_ERROR       0x0009 //!< Memory allocation error occurred

/** @} End L2CAP_TERM_REASON_VALUES */

/**
 * @defgroup L2CAP_CHANNEL_STATES L2CAP Channel States
 * @{
 */

#define L2CAP_CLOSED                     0x00 //!< Closed - no channel associated with this CID
#define L2CAP_OPEN                       0x01 //!< Open - channel is open
#define L2CAP_PENDING_CONN_RSP           0x02 //!< Waiting for Connection Response
#define L2CAP_PENDING_DISCONN_RSP        0x03 //!< Waiting for Disconnection Response
#define L2CAP_PENDING_PARAM_UPDATE_RSP   0x04 //!< Waiting for Connection Parameter Update Response
#define L2CAP_PENDING_INFO_RSP           0x05 //!< Waiting for Info Response
#define L2CAP_PENDING_SEC_VERIFY         0x06 //!< Waiting for Security Verification from local PSM

/** @} End L2CAP_CHANNEL_STATES */

/**
 * @defgroup L2CAP_INFO_TYPES L2CAP Information Request/Response Info Types
 * @{
 */

#define L2CAP_INFO_CONNLESS_MTU          0x0001 //!< Connectionless MTU
#define L2CAP_INFO_EXTENDED_FEATURES     0x0002 //!< Extended features supported
#define L2CAP_INFO_FIXED_CHANNELS        0x0003 //!< Fixed channels supported

/** @} End L2CAP_INFO_TYPES */

/*********************************************************************
 * Information Response: Extended Features Mask Values
 */
#define L2CAP_FIXED_CHANNELS             0x00000080 //!< Fixed channels are supported

#define L2CAP_EXTENDED_FEATURES_SIZE     4 //!< Length of Extended Features bit mask

/*********************************************************************
 * Information Response: Fixed Channels Mask Values
 */
#define L2CAP_FIXED_CHANNELS_ATT         0x10 //!< Fixed Channel ATT is supported

#define L2CAP_FIXED_CHANNELS_SIG         0x20 //!< Fixed Channel L2CAP Signaling is supported

#define L2CAP_FIXED_CHANNELS_SMP         0x40 //!< Fixed Channel SMP is supported

#define L2CAP_FIXED_CHANNELS_SIZE        8 //!< Length of Fixed Channels bit mask

/*********************************************************************
 * Information Response: Result Values
 */
#define L2CAP_INFO_SUCCESS               0x0000 //!< L2CAP Success

#define L2CAP_INFO_NOT_SUPPORTED         0x0001 //!< L2CAP Not supported

/// @cond NODOC

/*********************************************************************
 * L2CAP Test Modes: Test code values
 */
#if defined ( TESTMODES )
  // Test modes 0 through 10 are reserved for L2CAP Connection Request.
  #define L2CAP_TESTMODE_OFF                   0  //!< Test mode off
  #define L2CAP_TESTMODE_PSM_NOT_SUPPORTED     2  //!< Refuse connection - LE_PSM not supported
  #define L2CAP_TESTMODE_NO_RESOURCES          4  //!< Refuse connection - no resources available
  #define L2CAP_TESTMODE_INSUFF_AUTHEN         5  //!< Refuse connection - insufficient authentication
  #define L2CAP_TESTMODE_INSUFF_AUTHOR         6  //!< Refuse connection - insufficient authorization
  #define L2CAP_TESTMODE_INSUFF_KEY_SIZE       7  //!< Refuse connection - insufficient encryption key size
  #define L2CAP_TESTMODE_INSUFF_ENCRYPT        8  //!< Refuse connection - insufficient encryption
  #define L2CAP_TESTMODE_INVALID_SRC_CID       9  //!< Refuse connection - invalid Source CID
  #define L2CAP_TESTMODE_SRC_CID_ALREADY_ALLOC 10 //!< Refuse connection - Source CID already allocated
  #define L2CAP_TESTMODE_OUT_OF_RANGE_CREDITS  11 //!< Allow out of range credits to be sent (0 or exceeding 65535)
  #define L2CAP_TESTMODE_SDU_LEN_EXCEED        12 //!< Send SDU larger than SDU length specified in first LE-frame of SDU
  #define L2CAP_TESTMODE_TX_PKT_NO_CREDIT      13 //!< Send LE-frame when local device has a credit count of zero
  #define L2CAP_TESTMODE_BLOCK_DATA_TX         14 //!< Block data packet transmission
#endif

/// @endcond // NODOC

/** @} End L2CAP_CONSTANTS */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/**
 * @defgroup L2CAP_Structs L2CAP Data Structures
 * @{
 */

/// @brief User configurable variables format.
typedef struct
{
  uint8 maxNumPSM;        //!< Max number of Protocol/Service Multiplexers (PSM): @ref L2CAP_NUM_PSM_DEFAULT
  uint8 maxNumCoChannels; //!< Max number of Connection Oriented Channels: @ref L2CAP_NUM_CO_CHANNELS_DEFAULT
} l2capUserCfg_t;

/// @brief Invalid CID in Request format.
typedef struct
{
  uint16 localCID;  //!< Destination CID from the rejected command
  uint16 remoteCID; //!< Source CID from the rejected command
} l2capInvalidCID_t;

/// @brief Command Reject Reason Data format.
typedef union
{
  uint16 signalMTU;             //!< Maximum Signaling MTU
  l2capInvalidCID_t invalidCID; //!< Invalid CID in Request
} l2capReasonData_t;

/// @brief Command Reject format.
typedef struct
{
  uint16 reason;                //!< Reason: @ref L2CAP_REJECT_REASON_CODES
  l2capReasonData_t reasonData; //!< Reason Data

  // Shorthand access for union members
  #define maxSignalMTU     reasonData.signalMTU             //!< max signal MTU
  #define invalidLocalCID  reasonData.invalidCID.localCID   //!< local CID
  #define invalidRemoteCID reasonData.invalidCID.remoteCID  //!< remote CID
} l2capCmdReject_t;

/// @brief Connection Parameter Update Request format.
typedef struct
{
  uint16 intervalMin;       //!< Minimum Interval
  uint16 intervalMax;       //!< Maximum Interval
  uint16 slaveLatency;      //!< Slave Latency
  uint16 timeoutMultiplier; //!< Timeout Multiplier
} l2capParamUpdateReq_t;

/// @brief Connection Parameter Update Response format.
typedef struct
{
  uint16 result; //!< Result: @ref L2CAP_PARAMS_RESULT_VALUES
} l2capParamUpdateRsp_t;

/// @brief Information Request format.
typedef struct
{
  uint16 infoType; //!< Information type: @ref L2CAP_INFO_TYPES
} l2capInfoReq_t;

/// @brief Information Response Data field.
typedef union
{
  uint16 connectionlessMTU;                       //!< Connectionless MTU
  uint32 extendedFeatures;                        //!< Extended features supported
  uint8 fixedChannels[L2CAP_FIXED_CHANNELS_SIZE]; //!< Fixed channels supported
} l2capInfo_t;

/// @brief Information Response format.
typedef struct
{
  uint16 result;    //!< Result
  uint16 infoType;  //!< Information type: @ref L2CAP_INFO_TYPES
  l2capInfo_t info; //!< Content of Info field depends on infoType
} l2capInfoRsp_t;

/// @brief Connection Request format.
typedef struct
{
  uint16 psm;         //!< LE PSM
  uint16 srcCID;      //!< Represents CID on device sending request and receiving response
  uint16 mtu;         //!< Specifies maximum SDU size that can be received on this channel
  uint16 mps;         //!< Specifies maximum payload size that can be received on this channel
  uint16 initCredits; //!< Indicates number of LE-frames that peer device can send
} l2capConnectReq_t;

/// @brief Connection Response format.
typedef struct
{
  uint16 dstCID;      //!< Represents CID on device receiving request and sending response
  uint16 mtu;         //!< Specifies maximum SDU size that can be received on this channel
  uint16 mps;         //!< Specifies maximum payload size that can be received on this channel
  uint16 initCredits; //!< Indicates number of LE-frames that peer device can send
  uint16 result;      //!< Indicates outcome of connection request: @ref L2CAP_CONN_RESULT_VALUES
} l2capConnectRsp_t;

/// @brief Disconnection Request format (src/dst CIDs are relative to sender of request).
typedef struct
{
  uint16 dstCID; //!< Specifies CID to be disconnected on device receiving request
  uint16 srcCID; //!< Specifies CID to be disconnected on device sending request
} l2capDisconnectReq_t;

/// @brief Disconnection Response format (src/dst CIDs are relative to sender of request).
typedef struct
{
  uint16 dstCID; //!< Identifies CID on device sending response
  uint16 srcCID; //!< Identifies CID on device receiving response
} l2capDisconnectRsp_t;

/// @brief Flow Control Credit format.
typedef struct
{
  uint16 CID;     //!< Represents Source CID of device sending credit packet
  uint16 credits; //!< Number of LE-frames that can be sent to local device
} l2capFlowCtrlCredit_t;

/// @brief PSM information format.
typedef struct
{
  uint16 mtu;                 //!< Maximum SDU size that can be received by local device
  uint16 mps;                 //!< Maximum payload size that can be received by local device
  uint16 initPeerCredits;     //!< Number of LE-frames that peer device can send
  uint16 peerCreditThreshold; //!< Low threshold for number of peer credits
  uint8 maxNumChannels;       //!< Maximum number of CO Channels supported by PSM
  uint8 numActiveChannels;    //!< Number of active CO Channels
} l2capPsmInfo_t;

/// @brief Connection oriented channel information format.
typedef struct
{
  uint16 psm;                 //!< PSM that channel belongs to
  uint16 mtu;                 //!< Maximum SDU size that can be received by local device
  uint16 mps;                 //!< Maximum payload size that can be received by local device
  uint16 credits;             //!< Number of LE-frames that local device can send
  uint16 peerCID;             //!< Remote channel id
  uint16 peerMtu;             //!< Maximum SDU size that can be received by peer device
  uint16 peerMps;             //!< Maximum payload size that can be received by peer device
  uint16 peerCredits;         //!< Number of LE-frames that peer device can send
  uint16 peerCreditThreshold; //!< Low threshold for peer credit count
} l2capCoCInfo_t;

/// @brief Local channel information format.
typedef struct
{
  uint8 state;         //!< Channel connection state: @ref L2CAP_CHANNEL_STATES
  l2capCoCInfo_t info; //!< Channel info
} l2capChannelInfo_t;

/** @} End L2CAP_Structs */

/**
 * @defgroup L2CAP_Events L2CAP Events
 * @{
 */

/**
 * @brief L2CAP_CHANNEL_ESTABLISHED_EVT message format.
 *
 * This message is sent to the
 * app when an L2CAP Connection Oriented Channel is established.
 */
typedef struct
{
  uint16 result;       //!< Indicates outcome of connection request: @ref L2CAP_CONN_RESULT_VALUES
  uint16 CID;          //!< Local channel id
  l2capCoCInfo_t info; //!< Channel info
} l2capChannelEstEvt_t;

/**
 * @brief L2CAP_CHANNEL_TERMINATED_EVT message format.
 *
 * This message is sent to the
 * app when an L2CAP Connection Oriented Channel is terminated.
 */
typedef struct
{
  uint16 CID;     //!< Local channel id
  uint16 peerCID; //!< Peer channel id
  uint16 reason;  //!< Indicates reason for termination: @ref L2CAP_TERM_REASON_VALUES
} l2capChannelTermEvt_t;

/**
 * @brief L2CAP_OUT_OF_CREDIT_EVT or L2CAP_PEER_CREDIT_THRESHOLD_EVT message format.
 *
 * This message is sent to the app to indicate the number of available credits
 * on a channel.
 */
typedef struct
{
  uint16 CID;     //!< Local channel id
  uint16 peerCID; //!< Peer channel id
  uint16 credits; //!< Credits needed to transmit (L2CAP_OUT_OF_CREDIT_EVT) or
                  //!< Peer credit threshold (L2CAP_PEER_CREDIT_THRESHOLD_EVT)
} l2capCreditEvt_t;

/**
 * @brief L2CAP_SEND_SDU_DONE_EVT message format.
 *
 * This message is sent to the
 * app when an SDU transmission is completed or aborted.
 */
typedef struct
{
  uint16 CID;         //!< Local channel id
  uint16 credits;     //!< Local credits
  uint16 peerCID;     //!< Peer channel id
  uint16 peerCredits; //!< Peer credits
  uint16 totalLen;    //!< Total length of SDU being transmitted
  uint16 txLen;       //!< Total number of octets transmitted
} l2capSendSduDoneEvt_t;

/**
 * @brief L2CAP_NUM_CTRL_DATA_PKT_EVT message format.
 *
 * This message is sent to the
 * app when a controller data buffers become available for sending another packet
 */
typedef struct
{
  uint8 numDataPkt;   //!< Number of Data Packets Available
} l2capNumCtrlDataPktEvt_t;

/// @brief Union of all L2CAP Signaling commands.
typedef union
{
  // Requests
  l2capParamUpdateReq_t updateReq;              //!< Connection Parameter Update Request
  l2capInfoReq_t infoReq;                       //!< Information Request - not supported
  l2capConnectReq_t connectReq;                 //!< LE Credit Based Connection Request
  l2capDisconnectReq_t disconnectReq;           //!< Disconnection Request
  l2capFlowCtrlCredit_t credit;                 //!< LE Flow Control Credit

  // Responses
  l2capParamUpdateRsp_t updateRsp;              //!< Connection Parameter Update Response
  l2capInfoRsp_t infoRsp;                       //!< Information Response - not supported
  l2capCmdReject_t cmdReject;                   //!< Command Reject
  l2capConnectRsp_t connectRsp;                 //!< LE Credit Based Connection Response
  l2capDisconnectRsp_t disconnectRsp;           //!< Disconnection Response

  // Proprietary local events
  l2capChannelEstEvt_t channelEstEvt;           //!< Channel Established Event
  l2capChannelTermEvt_t channelTermEvt;         //!< Channel Terminated Event
  l2capCreditEvt_t creditEvt;                   //!< Out Of Credit Event or Peer Credit Threshold Event
  l2capSendSduDoneEvt_t sendSduDoneEvt;         //!< Send SDU Done Event
  l2capNumCtrlDataPktEvt_t numCtrlDataPktEvt;       //!< Number of Available Ctrl Data Packets Event
} l2capSignalCmd_t;

/**
 * @brief OSAL L2CAP_SIGNAL_EVENT message format.
 *
 * This message is used to deliver an
 * incoming Signaling command up to an upper layer application.
 */
typedef struct
{
  osal_event_hdr_t hdr; //!< L2CAP_SIGNAL_EVENT and status
  uint16 connHandle;    //!< connection message was received on
  uint8 id;             //!< identifier to match responses with requests
  uint8 opcode;         //!< type of command: @ref L2CAP_SIGNALING_CODES_DEFINES
  l2capSignalCmd_t cmd; //!< command data
} l2capSignalEvent_t;

/// @brief L2CAP packet structure.
typedef struct
{
  uint16 CID;      //!< local channel id
  uint8 *pPayload; //!< pointer to information payload. This contains the payload
                   //!< received from the upper layer protocol (outgoing packet),
                   //!< or delivered to the upper layer protocol (incoming packet).
  uint16 len;      //!< length of information payload
} l2capPacket_t;

/**
 * @brief OSAL L2CAP_DATA_EVENT message format.
 *
 * This message is used to forward an
 * incoming data packet up to an upper layer application.
 *
 * @note
 * Received packet 'pPayload' must be freed using BM_free().
 *
 */
typedef struct
{
  osal_event_hdr_t hdr; //!< L2CAP_DATA_EVENT and status
  uint16 connHandle;    //!< connection packet was received on
  l2capPacket_t pkt;    //!< received packet
} l2capDataEvent_t;

/** @} End L2CAP_Events */

/**
 * @defgroup L2CAP_CBS L2CAP Callbacks
 *
 * @{
 */

/**
 * @brief   Callback function prototype to verify security when a Connection
 *          Request is received.
 *
 * @note
 * It can be specified as NULL by PSM if there's no security requirements.
 *
 * @param   connHandle - connection handle request was received on
 * @param   id - identifier matches responses with requests
 * @param   pReq - received connection request
 *
 * @return  @ref L2CAP_CONN_RESULT_VALUES
 */
typedef uint16 (*pfnVerifySecCB_t)( uint16 connHandle, uint8 id, l2capConnectReq_t *pReq );

/** @} End L2CAP_CBS */

 /**
 *  @addtogroup L2CAP_Structs
 *  @{
 */

/// @brief L2CAP PSM structure. Allocated one for each registered PSM.
typedef struct
{
  uint16 psm;                      //!< Registered PSM
  uint16 mtu;                      //!< Maximum SDU size that can be received by local device
  uint16 initPeerCredits;          //!< Number of LE-frames that peer device can send
  uint16 peerCreditThreshold;      //!< Low threshold for peer credit count
  uint8 maxNumChannels;            //!< Maximum number of CO Channels supported by PSM
  uint8 taskId;                    //!< Task registered with PSM
  pfnVerifySecCB_t pfnVerifySecCB; //!< PSM security verification callback (can be NULL)
} l2capPsm_t;

/** @} */ // end of L2CAP_Structs

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

/**
 * @brief   Register a protocol/application with an L2CAP fixed channel.
 *
 * @param   taskId - task to be registered with channel.
 * @param   CID - fixed channel ID.
 *
 * @return  @ref SUCCESS : Registration was successful.
 * @return  @ref INVALIDPARAMETER : Channel ID is invalid.
 */
extern bStatus_t L2CAP_RegisterApp( uint8 taskId, uint16 CID );

/**
 * @brief   Register task to recevie L2CAP Flow Control events when
 *          there are data buffers available for sending messages
 *
 * @param  taskId - Application's task ID.
 */
extern void L2CAP_RegisterFlowCtrlTask( uint8 taskId );

/**
 * @brief   Send data packet over an L2CAP fixed channel established over
 *          a physical connection.
 *
 * @note
 * Packet 'pPayload' must be allocated using L2CAP_bm_alloc().
 *
 * @param   connHandle - connection to be used.
 * @param   pPkt - pointer to packet to be sent.
 *
 * @return  @ref SUCCESS : Data was sent successfully.
 * @return  @ref INVALIDPARAMETER : Channel ID is invalid or payload is null.
 * @return  @ref MSG_BUFFER_NOT_AVAIL.
 * @return  @ref bleNotConnected
 * @return  @ref bleMemAllocError
 * @return  @ref blePending : In the middle of another transmit.
 * @return  @ref bleInvalidMtuSize : Packet length is larger than MTU size.
 */
extern bStatus_t L2CAP_SendData( uint16 connHandle, l2capPacket_t *pPkt );

/**
 * @brief   Register a Protocol/Service Multiplexer (PSM) with L2CAP.
 *
 * @param   pPsm - pointer to PSM structure.
 *
 * @return  @ref SUCCESS : Registration was successful.
 * @return  @ref INVALIDPARAMETER : Max number of channels is greater than total supported.
 * @return  @ref bleInvalidRange :PSM value is out of range.
 * @return  @ref bleInvalidMtuSize : MTU size is out of range.
 * @return  @ref bleNoResources
 * @return  @ref bleAlreadyInRequestedMode : PSM already registered.
 */
extern bStatus_t L2CAP_RegisterPsm( l2capPsm_t *pPsm );

/**
 * @brief   Deregister a Protocol/Service Multiplexer with L2CAP.
 *
 * @param   taskId - task PSM belongs to.
 * @param   psm - PSM to deregister.
 *
 * @return  @ref SUCCESS : Registration was successful.
 * @return  @ref INVALIDPARAMETER : PSM or task Id is invalid.
 * @return  @ref bleIncorrectMode : PSM is in use.
 */
extern bStatus_t L2CAP_DeregisterPsm( uint8 taskId, uint16 psm );

/**
 * @brief   Get information about a given registered PSM.
 *
 * @param   psm - PSM Id.
 * @param   pInfo - structure to copy PSM info into.
 *
 * @return  @ref SUCCESS : Operation was successful.
 * @return  @ref INVALIDPARAMETER : PSM is not registered.
 */
extern bStatus_t L2CAP_PsmInfo( uint16 psm, l2capPsmInfo_t *pInfo );

/**
 * @brief   Get all active channels for a given registered PSM.
 *
 * @param   psm - PSM Id.
 * @param   numCIDs - number of CIDs can be copied.
 * @param   pCIDs - structure to copy CIDs into.
 *
 * @return  @ref SUCCESS : Operation was successful.
 * @return  @ref INVALIDPARAMETER : PSM is not registered.
 */
extern bStatus_t L2CAP_PsmChannels( uint16 psm, uint8 numCIDs, uint16 *pCIDs );

/**
 * @brief   Get information about a given active Connection Oriented Channel.
 *
 * @param   CID - local channel id.
 * @param   pInfo - structure to copy channel info into.
 *
 * @return  @ref SUCCESS : Operation was successful.
 * @return  @ref INVALIDPARAMETER : No such a channel.
 */
extern bStatus_t L2CAP_ChannelInfo( uint16 CID, l2capChannelInfo_t *pInfo );

/**
 * @brief   Send Connection Request.
 *
 * @param   connHandle - connection to create channel on
 * @param   psm - local PSM
 * @param   peerPsm - peer PSM
 *
 * @return  @ref SUCCESS : Request was sent successfully.
 * @return  @ref INVALIDPARAMETER : PSM is not registered.
 * @return  @ref MSG_BUFFER_NOT_AVAIL.
 * @return  @ref bleIncorrectMode : PSM not registered.
 * @return  @ref bleNotConnected
 * @return  @ref bleNoResources
 * @return  @ref bleMemAllocError
 */
extern bStatus_t L2CAP_ConnectReq( uint16 connHandle, uint16 psm, uint16 peerPsm );

/**
 * @brief   Send Connection Response.
 *
 * @param   connHandle - connection handle
 * @param   id - identifier received in connection request
 * @param   result - @ref L2CAP_CONN_RESULT_VALUES
 *
 * @return  @ref SUCCESS : Request was sent successfully.
 * @return  @ref INVALIDPARAMETER : PSM is not registered or Channel is not open.
 * @return  @ref MSG_BUFFER_NOT_AVAIL.
 * @return  @ref bleNotConnected
 * @return  @ref bleMemAllocError
 */
extern bStatus_t L2CAP_ConnectRsp( uint16 connHandle, uint8 id, uint16 result );

/**
 * @brief   Send Disconnection Request.
 *
 * @param   CID - local CID to disconnect
 *
 * @return  @ref SUCCESS : Request was sent successfully.
 * @return  @ref INVALIDPARAMETER : Channel id is invalid.
 * @return  @ref MSG_BUFFER_NOT_AVAIL.
 * @return  @ref bleNotConnected
 * @return  @ref bleNoResources
 * @return  @ref bleMemAllocError
 */
extern bStatus_t L2CAP_DisconnectReq( uint16 CID );

/**
 * @brief   Send Flow Control Credit.
 *
 * @param   CID - local CID
 * @param   peerCredits - number of credits to give to peer device
 *
 * @return  @ref SUCCESS : Request was sent successfully.
 * @return  @ref INVALIDPARAMETER : Channel is not open.
 * @return  @ref MSG_BUFFER_NOT_AVAIL.
 * @return  @ref bleNotConnected
 * @return  @ref bleInvalidRange : Credits is out of range.
 * @return  @ref bleMemAllocError
 */
extern bStatus_t L2CAP_FlowCtrlCredit( uint16 CID, uint16 peerCredits );

/**
 * @brief   Send data packet over an L2CAP connection oriented channel
 *          established over a physical connection.
 *
 *          Note: Packet 'pPayload' must be allocated using L2CAP_bm_alloc().
 *
 * @param   pPkt - pointer to packet to be sent.
 *
 * @return  @ref SUCCESS : Data was sent successfully.
 * @return  @ref INVALIDPARAMETER : SDU payload is null.
 * @return  @ref bleNotConnected : Connection or Channel is down.
 * @return  @ref bleMemAllocError
 * @return  @ref blePending : In the middle of another transmit.
 * @return  @ref bleInvalidMtuSize : SDU size is larger than peer MTU.
 */
extern bStatus_t L2CAP_SendSDU( l2capPacket_t *pPkt );

/**
 * @brief   Send Command Reject.
 *
 * @param   connHandle - connection to use
 * @param   id - identifier of the request packet being rejected
 * @param   pCmdReject - pointer to Command Reject to be sent
 *
 * @return  @ref SUCCESS : Request was sent successfully.
 * @return  @ref INVALIDPARAMETER : Data can not fit into one packet.
 * @return  @ref MSG_BUFFER_NOT_AVAIL.
 * @return  @ref bleNotConnected
 * @return  @ref bleMemAllocError
 */
extern bStatus_t L2CAP_CmdReject( uint16 connHandle, uint8 id, l2capCmdReject_t *pCmdReject );

/**
 * @brief   Send Information Request.
 *
 * @param   connHandle - connection to use
 * @param   pInfoReq - pointer to Info Request to be sent
 * @param   taskId - task to be notified about result
 *
 * @return  @ref SUCCESS : Request was sent successfully.
 * @return  @ref INVALIDPARAMETER : Data can not fit into one packet.
 * @return  @ref MSG_BUFFER_NOT_AVAIL.
 * @return  @ref bleNotConnected
 * @return  @ref bleNoResources
 * @return  @ref bleMemAllocError
 */
extern bStatus_t L2CAP_InfoReq( uint16 connHandle, l2capInfoReq_t *pInfoReq, uint8 taskId );

/**
 * @brief   Send Connection Parameter Update Request.
 *
 * @param   connHandle - connection to use
 * @param   pUpdateReq - pointer to Update Request to be sent
 * @param   taskId - task to be notified about result
 *
 * @return  @ref SUCCESS : Request was sent successfully.
 * @return  @ref INVALIDPARAMETER : Data can not fit into one packet.
 * @return  @ref MSG_BUFFER_NOT_AVAIL.
 * @return  @ref bleMemAllocError
 * @return  @ref bleNotConnected
 * @return  @ref bleNoResources
 * @return  @ref bleInvalidRange :a parameter isout of range.
 */
extern bStatus_t L2CAP_ConnParamUpdateReq( uint16 connHandle, l2capParamUpdateReq_t *pUpdateReq, uint8 taskId );

/**
 * @brief   Send Connection Parameter Update Response.
 *
 * @param   connHandle - connection to use
 * @param   id - identifier received in request
 * @param   pUpdateRsp - pointer to Update Response to be sent
 *
 * @return  @ref SUCCESS : Request was sent successfully.
 * @return  @ref INVALIDPARAMETER : Data can not fit into one packet.
 * @return  @ref MSG_BUFFER_NOT_AVAIL.
 * @return  @ref bleNotConnected
 * @return  @ref bleMemAllocError
 */
extern bStatus_t L2CAP_ConnParamUpdateRsp( uint16 connHandle, uint8 id, l2capParamUpdateRsp_t *pUpdateRsp );

/**
 * @brief   Set the user configurable variables for the L2CAP layer.
 *
 * @note
 * This function should be called BEFORE osal_init_system().
 *
 * @param   pUserCfg - pointer to user configuration
 */
extern void L2CAP_SetUserConfig( l2capUserCfg_t *pUserCfg );

/**
 * @brief   This API is used by the upper layer to set the maximum data
 *          packet size and the number of data packets allowed by the
 *          Controller.
 *
 * @param   dataPktLen - data packet size
 * @param   numDataPkts - number of data packets
 */
extern void L2CAP_SetBufSize( uint16 dataPktLen, uint8 numDataPkts );

/**
 * @brief   Get the maximum payload size that L2CAP is capable of receiving.
 *
 * @return  MTU size
 */
extern uint16 L2CAP_GetMTU( void );

/**
 * @brief   L2CAP implementation of the allocator functionality.
 *
 * @note
 * This function should only be called by L2CAP and
 *                the upper layer protocol/application.
 *
 * @param   size - number of bytes to allocate from the heap.
 *
 * @return  pointer to the heap allocation
 * @return NULL if error or failure.
 */
extern void *L2CAP_bm_alloc( uint16 size );

/**
 * @brief   Build Connection Request.
 *
 * @param   pBuf - pointer to buffer to hold command data
 * @param   pData - pointer to command data
 *
 * @return  length of the command data
 */
extern uint16 L2CAP_BuildConnectReq( uint8 *pBuf, uint8 *pData );

/**
 * @brief   Parse Connection Request message.
 *
 * @param   pCmd - pointer to command data to be built
 * @param   pData - pointer to incoming command data to be parsed
 * @param   len - length of incoming command data
 *
 * @return  @ref SUCCESS : Command was parsed successfully.
 * @return  @ref FAILURE : Command length is invalid.
 */
extern bStatus_t L2CAP_ParseConnectReq( l2capSignalCmd_t *pCmd, uint8 *pData, uint16 len );

/**
 * @brief   Parse Flow Control Credit message.
 *
 * @param   pCmd - pointer to command data to be built
 * @param   pData - pointer to incoming command data to be parsed
 * @param   len - length of incoming command data
 *
 * @return  @ref SUCCESS : Command was parsed successfully.
 * @return  @ref FAILURE : Command length is invalid.
 */
extern bStatus_t L2CAP_ParseFlowCtrlCredit( l2capSignalCmd_t *pCmd, uint8 *pData, uint16 len );

/**
 * @brief   Build Command Reject.
 *
 * @param   pBuf - pointer to buffer to hold command data
 * @param   pCmd - pointer to command data
 *
 * @return  length of the command data
 */
extern uint16 L2CAP_BuildCmdReject( uint8 *pBuf, uint8 *pCmd );

/**
 * @brief   Parse Information Request message.
 *
 * @param   pCmd - pointer to command data to be built
 * @param   pData - pointer to incoming command data to be parsed
 * @param   len - length of incoming command data
 *
 * @return  @ref SUCCESS : Command was parsed successfully.
 * @return  @ref FAILURE : Command length is invalid.
 */
extern bStatus_t L2CAP_ParseInfoReq( l2capSignalCmd_t *pCmd, uint8 *pData, uint16 len );

/**
 * @brief   Parse Connection Parameter Update Request.
 *
 * @param   pCmd - pointer to command data to be built
 * @param   pData - pointer to incoming command data to be parsed
 * @param   len - length of incoming command data
 *
 * @return  @ref SUCCESS : Command was parsed successfully.
 * @return  @ref FAILURE : Command length is invalid.
 */
extern bStatus_t L2CAP_ParseParamUpdateReq( l2capSignalCmd_t *pCmd, uint8 *pData, uint16 len );

/**
 * @brief   Build Connection Parameter Update Response.
 *
 * @param   pBuf - pointer to buffer to hold command data
 * @param   pData - pointer to command data
 *
 * @return  length of the command data
 */
extern uint16 L2CAP_BuildParamUpdateRsp( uint8 *pBuf, uint8 *pData );

/**
 * @brief   Build Information Response.
 *
 * @param   pBuf - pointer to buffer to hold command data
 * @param   pData - pointer to command data
 *
 * @return  length of the command data
 */
extern uint16 L2CAP_BuildInfoRsp( uint8 *pBuf, uint8 *pData );

/**
 * @brief   Turn flow control on
 *          or off for data packets sent from the Controller to the Host.
 *
 * @param   hostBuffSize - total data buffer available on Host
 * @param   flowCtrlMode - flow control mode: TRUE or FALSE
 */
extern void L2CAP_SetControllerToHostFlowCtrl( uint16 hostBuffSize, uint8 flowCtrlMode );

/**
 * @brief Host Number of Completed Packets
 *
 * This API is used by the upper layer to notify L2CAP of the
 *          number of data packets that have been completed for connection
 *          handle since this API was previously called.
 *
 * @param   connHandle - connection handle
 * @param   numCompletedPkts - number of completed packets
 */
extern void L2CAP_HostNumCompletedPkts( uint16 connHandle, uint16 numCompletedPkts );

/**
 * @brief   Set the L2CAP parameter value.
 *
 *          Use this function to change the default L2CAP parameter value.
 *
 * @param   value - new parameter value
 */
extern void L2CAP_SetParamValue( uint16 value );

/**
 * @brief   Get the L2CAP parameter value.
 *
 * @return  L2CAP parameter value
 */
extern uint16 L2CAP_GetParamValue( void );

/*-------------------------------------------------------------------
 * TASK API - These functions must only be called by OSAL.
 */

/// @cond NODOC

/**
 * @internal
 *
 * @brief   Initialize the L2CAP layer.
 *
 * @param   taskId - Task identifier for the desired task
 */
extern void L2CAP_Init( uint8 taskId );

/**
 * @internal
 *
 * @brief   L2CAP Task event processing function. This function should
 *          be called at periodic intervals when event occur.
 *
 * @param   taskId - Task ID
 * @param   events  - Bitmap of events
 */
extern uint16 L2CAP_ProcessEvent( uint8 taskId, uint16 events );

/// @endcond // NODOC

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* L2CAP_H */

/** @} End L2CAP */
