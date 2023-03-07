/******************************************************************************

 @file  zcomdef.h

 @brief Type definitions and macros.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2004-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

#ifndef ZCOMDEF_H
#define ZCOMDEF_H

#ifdef __cplusplus
extern "C"
{
#endif


/*********************************************************************
 * INCLUDES
 */
#include "comdef.h"
#include "saddr.h"


/*********************************************************************
 * CONSTANTS
 */
#define osal_cpyExtAddr(a, b)   sAddrExtCpy((a), (const uint8 *)(b))
#define osal_ExtAddrEqual(a, b) sAddrExtCmp((const uint8 *)(a), (const uint8 *)(b))
#define osal_copyAddress(a, b)  sAddrCpy( (sAddr_t *)(a), (const sAddr_t *)(b) )

/*********************************************************************
 * CONSTANTS
 */

// Build Device Types - Used during compilation
//   These are the types of devices to build
//   Bit masked into ZSTACK_DEVICE_BUILD
#define DEVICE_BUILD_COORDINATOR    0x01
#define DEVICE_BUILD_ROUTER         0x02
#define DEVICE_BUILD_ENDDEVICE      0x04

/*** Return Values ***/
#define ZSUCCESS                  SUCCESS

/*** Component IDs ***/
#define COMPID_OSAL               0
#define COMPID_MTEL               1
#define COMPID_MTSPCI             2
#define COMPID_NWK                3
#define COMPID_NWKIF              4
#define COMPID_MACCB              5
#define COMPID_MAC                6
#define COMPID_APP                7
#define COMPID_TEST               8

#define COMPID_RTG                9
#define COMPID_DATA               11

/* Temp CompIDs for testing */
#define COMPID_TEST_NWK_STARTUP           20
#define COMPID_TEST_SCAN_CONFIRM          21
#define COMPID_TEST_ASSOC_CONFIRM         22
#define COMPID_TEST_REMOTE_DATA_CONFIRM   23

// OSAL NV Item IDs
#define ZCD_NV_EX_LEGACY                  0x0000
#define ZCD_NV_EX_ADDRMGR                 0x0001
#define ZCD_NV_EX_BINDING_TABLE           0x0002
#define ZCD_NV_EX_DEVICE_LIST             0x0003

// OSAL NV item IDs
#define ZCD_NV_EXTADDR                    0x0001
#define ZCD_NV_BOOTCOUNTER                0x0002
#define ZCD_NV_STARTUP_OPTION             0x0003
#define ZCD_NV_START_DELAY                0x0004

// NWK Layer NV item IDs
#define ZCD_NV_NIB                        0x0021
#define ZCD_NV_DEVICE_LIST                0x0022
#define ZCD_NV_ADDRMGR                    0x0023
#define ZCD_NV_POLL_RATE_OLD16            0x0024 // Deprecated when poll rate changed from 16 to 32 bits
#define ZCD_NV_POLL_RATE                  0x0035
#define ZCD_NV_QUEUED_POLL_RATE           0x0025
#define ZCD_NV_RESPONSE_POLL_RATE         0x0026
#define ZCD_NV_REJOIN_POLL_RATE           0x0027
#define ZCD_NV_DATA_RETRIES               0x0028
#define ZCD_NV_POLL_FAILURE_RETRIES       0x0029
#define ZCD_NV_STACK_PROFILE              0x002A
#define ZCD_NV_INDIRECT_MSG_TIMEOUT       0x002B
#define ZCD_NV_ROUTE_EXPIRY_TIME          0x002C
#define ZCD_NV_EXTENDED_PAN_ID            0x002D
#define ZCD_NV_BCAST_RETRIES              0x002E
#define ZCD_NV_PASSIVE_ACK_TIMEOUT        0x002F
#define ZCD_NV_BCAST_DELIVERY_TIME        0x0030
#define ZCD_NV_NWK_MODE                   0x0031
#define ZCD_NV_CONCENTRATOR_ENABLE        0x0032
#define ZCD_NV_CONCENTRATOR_DISCOVERY     0x0033
#define ZCD_NV_CONCENTRATOR_RADIUS        0x0034
                                       // 0x0035 used above for new 32 bit Poll Rate
#define ZCD_NV_CONCENTRATOR_RC            0x0036
#define ZCD_NV_NWK_MGR_MODE               0x0037
#define ZCD_NV_SRC_RTG_EXPIRY_TIME        0x0038
#define ZCD_NV_ROUTE_DISCOVERY_TIME       0x0039
#define ZCD_NV_NWK_ACTIVE_KEY_INFO        0x003A
#define ZCD_NV_NWK_ALTERN_KEY_INFO        0x003B
#define ZCD_NV_ROUTER_OFF_ASSOC_CLEANUP   0x003C
#define ZCD_NV_NWK_LEAVE_REQ_ALLOWED      0x003D
#define ZCD_NV_NWK_CHILD_AGE_ENABLE       0x003E
#define ZCD_NV_DEVICE_LIST_KA_TIMEOUT     0x003F

// APS Layer NV item IDs
#define ZCD_NV_BINDING_TABLE              0x0041
#define ZCD_NV_GROUP_TABLE                0x0042
#define ZCD_NV_APS_FRAME_RETRIES          0x0043
#define ZCD_NV_APS_ACK_WAIT_DURATION      0x0044
#define ZCD_NV_APS_ACK_WAIT_MULTIPLIER    0x0045
#define ZCD_NV_BINDING_TIME               0x0046
#define ZCD_NV_APS_USE_EXT_PANID          0x0047
#define ZCD_NV_APS_USE_INSECURE_JOIN      0x0048
#define ZCD_NV_COMMISSIONED_NWK_ADDR      0x0049

#define ZCD_NV_APS_NONMEMBER_RADIUS       0x004B     // Multicast non_member radius
#define ZCD_NV_APS_LINK_KEY_TABLE         0x004C
#define ZCD_NV_APS_DUPREJ_TIMEOUT_INC     0x004D
#define ZCD_NV_APS_DUPREJ_TIMEOUT_COUNT   0x004E
#define ZCD_NV_APS_DUPREJ_TABLE_SIZE      0x004F

// System statistics and metrics NV ID
#define ZCD_NV_DIAGNOSTIC_STATS           0x0050

// Additional NWK Layer NV item IDs
#define ZCD_NV_NWK_PARENT_INFO            0x0051
#define ZCD_NV_NWK_ENDDEV_TIMEOUT_DEF     0x0052
#define ZCD_NV_END_DEV_TIMEOUT_VALUE      0x0053
#define ZCD_NV_END_DEV_CONFIGURATION      0x0054

// Security NV Item IDs
#define ZCD_NV_SECURITY_LEVEL             0x0061
#define ZCD_NV_PRECFGKEY                  0x0062
#define ZCD_NV_PRECFGKEYS_ENABLE          0x0063
#define ZCD_NV_SECURITY_MODE              0x0064
#define ZCD_NV_SECURE_PERMIT_JOIN         0x0065
#define ZCD_NV_APS_LINK_KEY_TYPE          0x0066
#define ZCD_NV_APS_ALLOW_R19_SECURITY     0x0067

#define ZCD_NV_IMPLICIT_CERTIFICATE       0x0069
#define ZCD_NV_DEVICE_PRIVATE_KEY         0x006A
#define ZCD_NV_CA_PUBLIC_KEY              0x006B
#define ZCD_NV_KE_MAX_DEVICES             0x006C

#define ZCD_NV_USE_DEFAULT_TCLK           0x006D
//deprecated: TRUSTCENTER_ADDR (16-bit)   0x006E
#define ZCD_NV_RNG_COUNTER                0x006F
#define ZCD_NV_RANDOM_SEED                0x0070
#define ZCD_NV_TRUSTCENTER_ADDR           0x0071

#define ZCD_NV_CERT_283                   0x0072
#define ZCD_NV_PRIVATE_KEY_283            0x0073
#define ZCD_NV_PUBLIC_KEY_283             0x0074

// ZDO NV Item IDs
#define ZCD_NV_USERDESC                   0x0081
#define ZCD_NV_NWKKEY                     0x0082
#define ZCD_NV_PANID                      0x0083
#define ZCD_NV_CHANLIST                   0x0084
#define ZCD_NV_LEAVE_CTRL                 0x0085
#define ZCD_NV_SCAN_DURATION              0x0086
#define ZCD_NV_LOGICAL_TYPE               0x0087
#define ZCD_NV_NWKMGR_MIN_TX              0x0088
#define ZCD_NV_NWKMGR_ADDR                0x0089

#define ZCD_NV_ZDO_DIRECT_CB              0x008F

// ZCL NV item IDs
#define ZCD_NV_SCENE_TABLE                0x0091
#define ZCD_NV_MIN_FREE_NWK_ADDR          0x0092
#define ZCD_NV_MAX_FREE_NWK_ADDR          0x0093
#define ZCD_NV_MIN_FREE_GRP_ID            0x0094
#define ZCD_NV_MAX_FREE_GRP_ID            0x0095
#define ZCD_NV_MIN_GRP_IDS                0x0096
#define ZCD_NV_MAX_GRP_IDS                0x0097
#define ZCD_NV_OTA_BLOCK_REQ_DELAY        0x0098

// Non-standard NV item IDs
#define ZCD_NV_SAPI_ENDPOINT              0x00A1

// NV Items Reserved for Commissioning Cluster Startup Attribute Set (SAS):
// 0x00B1 - 0x00BF: Parameters related to APS and NWK layers
// 0x00C1 - 0x00CF: Parameters related to Security
// 0x00D1 - 0x00DF: Current key parameters
#define ZCD_NV_SAS_SHORT_ADDR             0x00B1
#define ZCD_NV_SAS_EXT_PANID              0x00B2
#define ZCD_NV_SAS_PANID                  0x00B3
#define ZCD_NV_SAS_CHANNEL_MASK           0x00B4
#define ZCD_NV_SAS_PROTOCOL_VER           0x00B5
#define ZCD_NV_SAS_STACK_PROFILE          0x00B6
#define ZCD_NV_SAS_STARTUP_CTRL           0x00B7

#define ZCD_NV_SAS_TC_ADDR                0x00C1
#define ZCD_NV_SAS_TC_MASTER_KEY          0x00C2
#define ZCD_NV_SAS_NWK_KEY                0x00C3
#define ZCD_NV_SAS_USE_INSEC_JOIN         0x00C4
#define ZCD_NV_SAS_PRECFG_LINK_KEY        0x00C5
#define ZCD_NV_SAS_NWK_KEY_SEQ_NUM        0x00C6
#define ZCD_NV_SAS_NWK_KEY_TYPE           0x00C7
#define ZCD_NV_SAS_NWK_MGR_ADDR           0x00C8

#define ZCD_NV_SAS_CURR_TC_MASTER_KEY     0x00D1
#define ZCD_NV_SAS_CURR_NWK_KEY           0x00D2
#define ZCD_NV_SAS_CURR_PRECFG_LINK_KEY   0x00D3

// NV Items Reserved for Trust Center Link Key Table entries
// 0x0101 - 0x01FF
#define ZCD_NV_TCLK_TABLE_START           0x0101
#define ZCD_NV_TCLK_TABLE_END             0x01FF

// NV Items Reserved for APS Link Key Table entries
// 0x0201 - 0x02FF
#define ZCD_NV_APS_LINK_KEY_DATA_START    0x0201     // APS key data
#define ZCD_NV_APS_LINK_KEY_DATA_END      0x02FF

// NV items used to duplicate system elements
#define ZCD_NV_DUPLICATE_BINDING_TABLE            0x0300
#define ZCD_NV_DUPLICATE_DEVICE_LIST              0x0301
#define ZCD_NV_DUPLICATE_DEVICE_LIST_KA_TIMEOUT   0x0302

// NV Items Reserved for applications (user applications)
// 0x0401 - 0x0FFF


// ZCD_NV_STARTUP_OPTION values
//   These are bit weighted - you can OR these together.
//   Setting one of these bits will set their associated NV items
//   to code initialized values.
#define ZCD_STARTOPT_DEFAULT_CONFIG_STATE  0x01
#define ZCD_STARTOPT_DEFAULT_NETWORK_STATE 0x02
#define ZCD_STARTOPT_AUTO_START            0x04
#define ZCD_STARTOPT_CLEAR_CONFIG   ZCD_STARTOPT_DEFAULT_CONFIG_STATE
#define ZCD_STARTOPT_CLEAR_STATE    ZCD_STARTOPT_DEFAULT_NETWORK_STATE


#define ZCL_KE_IMPLICIT_CERTIFICATE_LEN    48
#define ZCL_KE_CA_PUBLIC_KEY_LEN           22
#define ZCL_KE_DEVICE_PRIVATE_KEY_LEN      21

/*********************************************************************
 * TYPEDEFS
 */

/*** Data Types ***/
typedef uint8   byte;
typedef uint16  UINT16;
typedef int16   INT16;

enum
{
  AddrNotPresent = 0,
  AddrGroup = 1,
  Addr16Bit = 2,
  Addr64Bit = 3,
  AddrBroadcast = 15
};

#define Z_EXTADDR_LEN   8

typedef byte ZLongAddr_t[Z_EXTADDR_LEN];

typedef struct
{
  union
  {
    uint16      shortAddr;
    ZLongAddr_t extAddr;
  } addr;
  byte addrMode;
} zAddrType_t;

// Redefined Generic Status Return Values for code backwards compatibility
#define ZSuccess                    SUCCESS
#define ZFailure                    FAILURE
#define ZInvalidParameter           INVALIDPARAMETER

// ZStack status values must start at 0x10, after the generic status values (defined in comdef.h)
#define ZMemError                   0x10
#define ZBufferFull                 0x11
#define ZUnsupportedMode            0x12
#define ZMacMemError                0x13

#define ZSapiInProgress             0x20
#define ZSapiTimeout                0x21
#define ZSapiInit                   0x22

#define ZNotAuthorized              0x7E

#define ZMalformedCmd               0x80
#define ZUnsupClusterCmd            0x81

// OTA Status values
#define ZOtaAbort                   0x95
#define ZOtaImageInvalid            0x96
#define ZOtaWaitForData             0x97
#define ZOtaNoImageAvailable        0x98
#define ZOtaRequireMoreImage        0x99

// APS status values
#define ZApsFail                    0xb1
#define ZApsTableFull               0xb2
#define ZApsIllegalRequest          0xb3
#define ZApsInvalidBinding          0xb4
#define ZApsUnsupportedAttrib       0xb5
#define ZApsNotSupported            0xb6
#define ZApsNoAck                   0xb7
#define ZApsDuplicateEntry          0xb8
#define ZApsNoBoundDevice           0xb9
#define ZApsNotAllowed              0xba
#define ZApsNotAuthenticated        0xbb

	// Security status values
#define ZSecNoKey                   0xa1
#define ZSecOldFrmCount             0xa2
#define ZSecMaxFrmCount             0xa3
#define ZSecCcmFail                 0xa4

	// NWK status values
#define ZNwkInvalidParam            0xc1
#define ZNwkInvalidRequest          0xc2
#define ZNwkNotPermitted            0xc3
#define ZNwkStartupFailure          0xc4
#define ZNwkAlreadyPresent          0xc5
#define ZNwkSyncFailure             0xc6
#define ZNwkTableFull               0xc7
#define ZNwkUnknownDevice           0xc8
#define ZNwkUnsupportedAttribute    0xc9
#define ZNwkNoNetworks              0xca
#define ZNwkLeaveUnconfirmed        0xcb
#define ZNwkNoAck                   0xcc  // not in spec
#define ZNwkNoRoute                 0xcd

	// MAC status values
#define ZMacSuccess                 0x00
#define ZMacBeaconLoss              0xe0
#define ZMacChannelAccessFailure    0xe1
#define ZMacDenied                  0xe2
#define ZMacDisableTrxFailure       0xe3
#define ZMacFailedSecurityCheck     0xe4
#define ZMacFrameTooLong            0xe5
#define ZMacInvalidGTS              0xe6
#define ZMacInvalidHandle           0xe7
#define ZMacInvalidParameter        0xe8
#define ZMacNoACK                   0xe9
#define ZMacNoBeacon                0xea
#define ZMacNoData                  0xeb
#define ZMacNoShortAddr             0xec
#define ZMacOutOfCap                0xed
#define ZMacPANIDConflict           0xee
#define ZMacRealignment             0xef
#define ZMacTransactionExpired      0xf0
#define ZMacTransactionOverFlow     0xf1
#define ZMacTxActive                0xf2
#define ZMacUnAvailableKey          0xf3
#define ZMacUnsupportedAttribute    0xf4
#define ZMacUnsupported             0xf5
#define ZMacSrcMatchInvalidIndex    0xff

typedef Status_t ZStatus_t;

typedef struct
{
  uint8  txCounter;    // Counter of transmission success/failures
  uint8  txCost;       // Average of sending rssi values if link staus is enabled
                       // i.e. NWK_LINK_STATUS_PERIOD is defined as non zero
  uint8  rxLqi;        // average of received rssi values
                       // needs to be converted to link cost (1-7) before used
  uint8  inKeySeqNum;  // security key sequence number
  uint32 inFrmCntr;    // security frame counter..
  uint16 txFailure;    // higher values indicate more failures
} linkInfo_t;

/*********************************************************************
 * Global System Messages
 */

#define SPI_INCOMING_ZTOOL_PORT   0x21    // Raw data from ZTool Port (not implemented)
#define SPI_INCOMING_ZAPP_DATA    0x22    // Raw data from the ZAPP port (see serialApp.c)
#define MT_SYS_APP_MSG            0x23    // Raw data from an MT Sys message
#define MT_SYS_APP_RSP_MSG        0x24    // Raw data output for an MT Sys message
#define MT_SYS_OTA_MSG            0x25    // Raw data output for an MT OTA Rsp
#define MT_SYS_APP_PB_ZCL_CMD     0x26    // MT APP PB ZCL command

#define AF_DATA_CONFIRM_CMD       0xFD    // Data confirmation
#define AF_REFLECT_ERROR_CMD      0xFE    // Reflected message error message
#define AF_INCOMING_MSG_CMD       0x1A    // Incoming MSG type message
#define AF_INCOMING_KVP_CMD       0x1B    // Incoming KVP type message
#define AF_INCOMING_GRP_KVP_CMD   0x1C    // Incoming Group KVP type message

//#define KEY_CHANGE                0xC0    // Key Events

#define ZDO_NEW_DSTADDR           0xD0    // ZDO has received a new DstAddr for this app
#define ZDO_STATE_CHANGE          0xD1    // ZDO has changed the device's network state
#define ZDO_MATCH_DESC_RSP_SENT   0xD2    // ZDO match descriptor response was sent
#define ZDO_CB_MSG                0xD3    // ZDO incoming message callback
#define ZDO_NETWORK_REPORT        0xD4    // ZDO received a Network Report message
#define ZDO_NETWORK_UPDATE        0xD5    // ZDO received a Network Update message
#define ZDO_ADDR_CHANGE_IND       0xD6    // ZDO was informed of device address change

#define NM_CHANNEL_INTERFERE      0x31    // NwkMgr received a Channel Interference message
#define NM_ED_SCAN_CONFIRM        0x32    // NwkMgr received an ED Scan Confirm message
#define SAPS_CHANNEL_CHANGE       0x33    // Stub APS has changed the device's channel
#define ZCL_INCOMING_MSG          0x34    // Incoming ZCL foundation message
#define ZCL_KEY_ESTABLISH_IND     0x35    // ZCL Key Establishment Completion Indication
#define ZCL_OTA_CALLBACK_IND      0x36    // ZCL OTA Completion Indication


// OSAL System Message IDs/Events Reserved for applications (user applications)
// 0xE0 - 0xFC

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ZCOMDEF_H */
