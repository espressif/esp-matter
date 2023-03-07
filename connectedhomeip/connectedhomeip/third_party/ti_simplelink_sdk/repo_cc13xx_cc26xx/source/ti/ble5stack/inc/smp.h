/******************************************************************************

 @file  smp.h

 @brief This file contains the interface to the Security Manager Protocol (SMP).

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

#ifndef SMP_H
#define SMP_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"

#include "sm_internal.h"

#include "rom_jt.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

// Code field of the SMP Command format
#define SMP_PAIRING_REQ                           0x01
#define SMP_PAIRING_RSP                           0x02
#define SMP_PAIRING_CONFIRM                       0x03
#define SMP_PAIRING_RANDOM                        0x04
#define SMP_PAIRING_FAILED                        0x05
#define SMP_ENCRYPTION_INFORMATION                0x06
#define SMP_MASTER_IDENTIFICATION                 0x07
#define SMP_IDENTITY_INFORMATION                  0x08
#define SMP_IDENTITY_ADDR_INFORMATION             0x09
#define SMP_SIGNING_INFORMATION                   0x0A
#define SMP_SECURITY_REQUEST                      0x0B
// Bluetooth 4.2 Secure Connections
#define SMP_PAIRING_PUBLIC_KEY                    0x0C
#define SMP_PAIRING_DHKEY_CHECK                   0x0D
#define SMP_PAIRING_KEYPRESS_NOTIFICATION         0x0E

// Pairing Request & Response - IO Capabilities
#define SMP_IO_CAP_DISPLAY_ONLY                   0x00
#define SMP_IO_CAP_DISPLAY_YES_NO                 0x01
#define SMP_IO_CAP_KEYBOARD_ONLY                  0x02
#define SMP_IO_CAP_NO_INPUT_NO_OUTPUT             0x03
#define SMP_IO_CAP_KEYBOARD_DISPLAY               0x04

// Pairing Request & Response - Out Of Bound (OOB) data flag values
#define SMP_OOB_AUTH_DATA_NOT_PRESENT             0x00
#define SMP_OOB_AUTH_DATA_REMOTE_DEVICE_PRESENT   0x01

// Pairing Request & Response - authReq field
//   - This field contains 2 sub-fields:
//      bonding flags - bits 1 & 0
#define SMP_AUTHREQ_BONDING                       0x01
//      Man-In-The-Middle (MITM) - bit 2
#define SMP_AUTHREQ_MITM                          0x04
//      Secure Connection (SC) - bit 3
#define SMP_AUTHREQ_SC                            0x08
//      Keypress Notification (SC) - bit 5
#define SMP_AUTHREQ_SC_KEYPRESS_NOTI              0x10

#define SMP_CONFIRM_LEN                           16
#define SMP_RANDOM_LEN                            16
#define SMP_DHKEY_CHECK_LEN                       16
#define SMP_PUBLIC_KEY_LEN                        32
#define SMP_MAX_ENC_KEY_SIZE                      16

// Pairing Failed - "reason" field
#define SMP_PAIRING_FAILED_PASSKEY_ENTRY_FAILED   0x01 //!< The user input of the passkey failed, for example, the user cancelled the operation.
#define SMP_PAIRING_FAILED_OOB_NOT_AVAIL          0x02 //!< The OOB data is not available
#define SMP_PAIRING_FAILED_AUTH_REQ               0x03 //!< The pairing procedure can't be performed as authentication requirements can't be met due to IO capabilities of one or both devices
#define SMP_PAIRING_FAILED_CONFIRM_VALUE          0x04 //!< The confirm value doesn't match the calculated compare value
#define SMP_PAIRING_FAILED_NOT_SUPPORTED          0x05 //!< Pairing isn't supported by the device
#define SMP_PAIRING_FAILED_ENC_KEY_SIZE           0x06 //!< The resultant encryption key size is insufficient for the security requirements of this device.
#define SMP_PAIRING_FAILED_CMD_NOT_SUPPORTED      0x07 //!< The SMP command received is not supported on this device.
#define SMP_PAIRING_FAILED_UNSPECIFIED            0x08 //!< Pairing failed due to an unspecified reason
#define SMP_PAIRING_FAILED_REPEATED_ATTEMPTS      0x09 //!< Pairing or authenication procedure is disallowed because too little time has elapsed since the last pairing request or security request.
#define SMP_PAIRING_FAILED_INVALID_PARAMETERS     0x0A //!< Indicates that the command length is invalid or that a parameter is outside of the specified range.
#define SMP_PAIRING_FAILED_DHKEY_CHECK_FAILED     0x0B //!< Indicates to the remote device that the DHKey Check value received does not match the one calculated by the local device.
#define SMP_PAIRING_FAILED_NUM_COMPARISON_FAILED  0x0C //!< Indicates that the confirm value in the numeric comparison protocol do not match.
#define SMP_PAIRING_FAILED_BREDR_PAIR_IN_PROGRESS 0x0D //!< Indicates that the pairing over the LE transport failed due to a Pairing Request sent over the BR/EDR transport in process.
#define SMP_PAIRING_FAILED_CT_KEYDER_NOT_ALLOWED  0x0E //!< Indicates that the BR/EDR Link Key generated on the BR/EDR transport cannot be used to derive and distribute keys for the LE transport.

#define SMP_PAIRING_FAILED_LOCAL_KEY_FAILURE      0x0F    // Local value - not sent over the air

// LE Secure Connections Keypress Notification values
#define SMP_KEYPRESS_NOTI_STARTED                 0x00
#define SMP_KEYPRESS_NOTI_DIGIT_ENTERED           0x01
#define SMP_KEYPRESS_NOTI_DIGIT_ERASED            0x02
#define SMP_KEYPRESS_NOTI_CLEARED                 0x03
#define SMP_KEYPRESS_NOTI_ENTRY_COMPLETED         0x04

// Message lengths
#define SMP_PAIRING_REQ_LEN                       7
#define SMP_PAIRING_RSP_LEN                       7
#define SMP_PAIRING_CONFIRM_LEN                   17
#define SMP_PAIRING_RANDOM_LEN                    17
#define SMP_PAIRING_FAILED_LEN                    2
#define SMP_ENCRYPTION_INFORMATION_LEN            17
#define SMP_MASTER_IDENTIFICATION_LEN             11
#define SMP_IDENTITY_INFORMATION_LEN              17
#define SMP_IDENTITY_ADDR_INFORMATION_LEN         8
#define SMP_SIGNING_INFORMATION_LEN               17
#define SMP_SECURITY_REQUEST_LEN                  2
#define SMP_PAIRING_PUBLIC_KEY_LEN                65
#define SMP_PAIRING_DHKEY_CHECK_LEN               17

// Macros to use the smSendSMMsg() function to send all of the Security Manager Protocol messages
#define smSendPairingReq( connHandle, msgStruct )  \
        MAP_smSendSMMsg( (connHandle), SMP_PAIRING_REQ_LEN, \
                         (smpMsgs_t *)(msgStruct), \
                         (pfnSMBuildCmd_t)(MAP_smpBuildPairingReq) )

#define smSendPairingRsp( connHandle, msgStruct )  \
        MAP_smSendSMMsg( (connHandle), SMP_PAIRING_RSP_LEN, \
                         (smpMsgs_t *)(msgStruct), \
                         (pfnSMBuildCmd_t)(MAP_smpBuildPairingRsp) )

#define smSendPairingConfirm( connHandle, msgStruct )  \
        MAP_smSendSMMsg( (connHandle), SMP_PAIRING_CONFIRM_LEN, \
                         (smpMsgs_t *)(msgStruct), \
                         (pfnSMBuildCmd_t)(MAP_smpBuildPairingConfirm) )

#define smSendPairingRandom( connHandle, msgStruct )  \
        MAP_smSendSMMsg( (connHandle), SMP_PAIRING_RANDOM_LEN, \
                         (smpMsgs_t *)(msgStruct), \
                         (pfnSMBuildCmd_t)(MAP_smpBuildPairingRandom) )

#define smSendPairingFailed( connHandle, msgStruct )  \
        MAP_smSendSMMsg( (connHandle), SMP_PAIRING_FAILED_LEN, \
                         (smpMsgs_t *)(msgStruct), \
                         (pfnSMBuildCmd_t)(MAP_smpBuildPairingFailed) )

#define smSendEncInfo( connHandle, msgStruct )  \
        MAP_smSendSMMsg( (connHandle), SMP_ENCRYPTION_INFORMATION_LEN, \
                         (smpMsgs_t *)(msgStruct), \
                         (pfnSMBuildCmd_t)(MAP_smpBuildEncInfo) )

#define smSendMasterID( connHandle, msgStruct )  \
        MAP_smSendSMMsg( (connHandle), SMP_MASTER_IDENTIFICATION_LEN, \
                         (smpMsgs_t *)(msgStruct), \
                         (pfnSMBuildCmd_t)(MAP_smpBuildMasterID) )

#define smSendIdentityInfo( connHandle, msgStruct )  \
        MAP_smSendSMMsg( (connHandle), SMP_IDENTITY_INFORMATION_LEN, \
                         (smpMsgs_t *)(msgStruct), \
                         (pfnSMBuildCmd_t)(MAP_smpBuildIdentityInfo) )

#define smSendIdentityAddrInfo( connHandle, msgStruct )  \
        MAP_smSendSMMsg( (connHandle), SMP_IDENTITY_ADDR_INFORMATION_LEN, \
                         (smpMsgs_t *)(msgStruct), \
                         (pfnSMBuildCmd_t)(MAP_smpBuildIdentityAddrInfo) )

#define smSendSigningInfo( connHandle, msgStruct )  \
        MAP_smSendSMMsg( (connHandle), SMP_SIGNING_INFORMATION_LEN, \
                         (smpMsgs_t *)(msgStruct), \
                         (pfnSMBuildCmd_t)(MAP_smpBuildSigningInfo) )

#define smSendSecurityReq( connHandle, msgStruct )  \
        MAP_smSendSMMsg( (connHandle), SMP_SECURITY_REQUEST_LEN, \
                         (smpMsgs_t *)(msgStruct), \
                         (pfnSMBuildCmd_t)(MAP_smpBuildSecurityReq) )

// LE Secure Connections
#define smSendPairingPublicKey( connHandle, msgStruct ) \
        MAP_smSendSMMsg( (connHandle), SMP_PAIRING_PUBLIC_KEY_LEN, \
                         (smpMsgs_t *)(msgStruct), \
                         (pfnSMBuildCmd_t)(MAP_smpBuildPairingPublicKey) )

#define smSendPairingDHKeyCheck( connHandle, msgStruct ) \
        MAP_smSendSMMsg( (connHandle), SMP_PAIRING_DHKEY_CHECK_LEN, \
                         (smpMsgs_t *)(msgStruct), \
                         (pfnSMBuildCmd_t)(MAP_smpBuildPairingDHKeyCheck) )

#define smSendKeypressNoti( connHandle, msgStruct ) \
        MAP_smSendSMMsg( (connHandle), SMP_PAIRING_KEYPRESS_NOTI_LEN, \
                         (smpMsgs_t *)(msgStruct), \
                         (pfnSMBuildCmd_t)(MAP_smpBuildKeypressNoti) )
/*********************************************************************
 * TYPEDEFS
 */

// Pairing Request
typedef struct
{
  uint8 ioCapability;   // ex. SMP_IO_CAP_DISPLAY_YES_NO
  uint8 oobDataFlag;    // Out of Bound data flag
  authReq_t authReq;    // Authentication fields
  uint8 maxEncKeySize;  // Encryption Key size max bytes (7 - 16)
  keyDist_t keyDist;    // Key Distribution Field - bit struct
} smpPairingReq_t;

// Pairing Response - same as Pairing Request
typedef smpPairingReq_t smpPairingRsp_t;

// Pairing Confirm
typedef struct
{
  uint8 confirmValue[SMP_CONFIRM_LEN];
} smpPairingConfirm_t;

// Pairing Random
typedef struct
{
  uint8 randomValue[SMP_RANDOM_LEN];
} smpPairingRandom_t;

// Pairing Failed
typedef struct
{
  uint8 reason;
} smpPairingFailed_t;

// Encryption Information
typedef struct
{
  uint8  ltk[KEYLEN];
} smpEncInfo_t;

// Master Identification
typedef struct
{
  uint16  ediv;
  uint16  rand[B_RANDOM_NUM_SIZE];
} smpMasterID_t;

// Identity Information
typedef struct
{
  uint8 irk[KEYLEN];
} smpIdentityInfo_t;

// Identity Address Information
typedef struct
{
  uint8 addrType;
  uint8 bdAddr[B_ADDR_LEN];
} smpIdentityAddrInfo_t;

// Signing Information
typedef struct
{
  uint8 signature[KEYLEN];
} smpSigningInfo_t;

// Slave Security Request
typedef struct
{
  authReq_t authReq;
} smpSecurityReq_t;

// LE Secure Connections Public Key Request
typedef struct
{
  uint8 publicKeyX[SMP_PUBLIC_KEY_LEN];
  uint8 publicKeyY[SMP_PUBLIC_KEY_LEN];
} smpPairingPublicKey_t;

// LE Secure Connections Diffie-Hellman Key Check Request
typedef struct
{
  uint8 dhKeyCheck[SMP_DHKEY_CHECK_LEN];
} smpPairingDHKeyCheck_t;

// LE Secure Connections Passcode Keypress Notification
typedef struct
{
  uint8 keypress; // ex. SMP_KEYPRESS_NOTI_STARTED
} smpKeyPressNoti_t;

// Union with all of the SM messages.
typedef union
{
  smpPairingReq_t        pairingReq;
  smpPairingReq_t        pairingRsp;
  smpPairingConfirm_t    pairingConfirm;
  smpPairingRandom_t     pairingRandom;
  smpPairingFailed_t     pairingFailed;
  smpEncInfo_t           encInfo;
  smpMasterID_t          masterID;
  smpIdentityInfo_t      idInfo;
  smpIdentityAddrInfo_t  idAddrInfo;
  smpSigningInfo_t       signingInfo;
  smpSecurityReq_t       secReq;
  smpPairingPublicKey_t  pairingPubKey;
  smpPairingDHKeyCheck_t pairingDHKeyCheck;
  smpKeyPressNoti_t      keyPressNoti;
} smpMsgs_t;

typedef uint8 (*pfnSMBuildCmd_t)( smpMsgs_t *pMsgStruct, uint8 *pBuf );

/*********************************************************************
 * GLOBAL VARIABLES
 */
extern smpPairingReq_t pairingReg;

/*********************************************************************
 * FUNCTIONS
 */

  /*
   * smpBuildPairingReq - Build an SM Pairing Request
   */
  extern bStatus_t smpBuildPairingReq( smpPairingReq_t *pPairingReq, uint8 *pBuf );

  /*
   * smpBuildPairingRsp - Build an SM Pairing Response
   */
  extern bStatus_t smpBuildPairingRsp( smpPairingRsp_t *pPairingRsp, uint8 *pBuf );

  /*
   * smpBuildPairingReqRsp - Build an SM Pairing Request or Response
   */
  extern bStatus_t smpBuildPairingReqRsp( uint8 opCode, smpPairingReq_t *pPairingReq, uint8 *pBuf );

  /*
   * smpParsePairingReq - Parse an SM Pairing Request
   */
  extern bStatus_t smpParsePairingReq( uint8 *pBuf, smpPairingReq_t *pPairingReq );

  /*
   * smpParsePairingRsp - Parse an SM Pairing Response
   */
  #define smpParsePairingRsp( a, b ) smpParsePairingReq( (a), (b) )

  /*
   * smpBuildPairingConfirm - Build an SM Pairing Confirm
   */
  extern bStatus_t smpBuildPairingConfirm( smpPairingConfirm_t *pPairingConfirm,
                                 uint8 *pBuf );

  /*
   * smpParsePairingConfirm - Parse an SM Pairing Confirm
   */
  extern bStatus_t smpParsePairingConfirm( uint8 *pBuf,
                                 smpPairingConfirm_t *pPairingConfirm );

  /*
   * smpBuildPairingRandom - Build an SM Pairing Random
   */
  extern bStatus_t smpBuildPairingRandom( smpPairingRandom_t *pPairingRandom,
                                 uint8 *pBuf );

  /*
   * smpParsePairingRandom - Parse an SM Pairing Random
   */
  extern bStatus_t smpParsePairingRandom( uint8 *pBuf,
                                 smpPairingRandom_t *pPairingRandom );

  /*
   * smpBuildPairingFailed - Build an SM Pairing Failed
   */
  extern bStatus_t smpBuildPairingFailed( smpPairingFailed_t *pPairingFailed,
                                 uint8 *pBuf );

  /*
   * smpParsePairingFailed - Parse an SM Pairing Failed
   */
  extern bStatus_t smpParsePairingFailed( uint8 *pBuf,
                                 smpPairingFailed_t *pPairingFailed );

  /*
   * smpBuildEncInfo - Build an SM Encryption Information
   */
  extern bStatus_t smpBuildEncInfo( smpEncInfo_t *pEncInfo, uint8 *pBuf );

  /*
   * smpParseEncInfo - Parse an SM Encryption Information
   */
  extern bStatus_t smpParseEncInfo( uint8 *buf, smpEncInfo_t *encInfo );

  /*
   * smpBuildMasterID - Build an SM Master Identification
   */
  extern bStatus_t smpBuildMasterID( smpMasterID_t *pMasterID, uint8 *pBuf );

  /*
   * smpParseMasterID - Parse an SM Master Identification
   */
  extern bStatus_t smpParseMasterID( uint8 *pBuf, smpMasterID_t *pMasterID );

  /*
   * smpBuildIdentityInfo - Build an SM Identity Information
   */
  extern bStatus_t smpBuildIdentityInfo( smpIdentityInfo_t *pIdInfo, uint8 *pBuf );

  /*
   * smpBuildIdentityAddrInfo - Build an SM Identity Address Information
   */
  extern bStatus_t smpBuildIdentityAddrInfo( smpIdentityAddrInfo_t *pIdInfo, uint8 *pBuf );

  /*
   * smpParseIdentityInfo - Parse an SM Identity Information
   */
  extern bStatus_t smpParseIdentityInfo( uint8 *pBuf, smpIdentityInfo_t *pIdInfo );

  /*
   * smpParseIdentityAddrInfo - Parse an SM Identity Address Information
   */
  extern bStatus_t smpParseIdentityAddrInfo( uint8 *pBuf, smpIdentityAddrInfo_t *pIdInfo );

  /*
   * smpBuildSigningInfo - Build an SM Signing Information
   */
  extern bStatus_t smpBuildSigningInfo( smpSigningInfo_t *pSigningInfo, uint8 *pBuf );

  /*
   * smpParseSigningInfo - Parse an SM Signing Information
   */
  extern bStatus_t smpParseSigningInfo( uint8 *pBuf, smpSigningInfo_t *pSigningInfo );

  /*
   * smpBuildSecurityReq - Build an SM Slave Security Request
   */
  extern bStatus_t smpBuildSecurityReq( smpSecurityReq_t *pSecReq, uint8 *pBuf );

  /*
   * smpParseSecurityReq - Parse an SM Slave Security Request
   */
  extern bStatus_t smpParseSecurityReq( uint8 *pBuf, smpSecurityReq_t *pSecReq );

  /*
   * smSendSMMsg - Generic Send L2CAP SM message function
   */
  extern bStatus_t smSendSMMsg( uint16 connHandle, uint8 bufLen, smpMsgs_t *pMsg, pfnSMBuildCmd_t buildFn );


  /*
   * smpBuildPairingPublicKey - Build an SM Pairing Public Key message
   */
  extern bStatus_t smpBuildPairingPublicKey( smpPairingPublicKey_t *pPubKey, uint8 *pBuf );

  /*
   * smpParsePairingPublicKey - Parse an SM Pairing Public Key message
   */
  extern bStatus_t smpParsePairingPublicKey( uint8 *pBuf, smpPairingPublicKey_t *pPubKey );

  /*
   * smpBuildPairingDHKeyCheck - Build an SM Pairing DHKey Check message
   */
  extern bStatus_t smpBuildPairingDHKeyCheck( smpPairingDHKeyCheck_t *pDHKeyCheck, uint8 *pBuf );

  /*
   * smpParsePairingDHKeyCheckk - Parse an SM Pairing DHKey Check message
   */
  extern bStatus_t smpParsePairingDHKeyCheck( uint8 *pBuf, smpPairingDHKeyCheck_t *pDHKeyCheck );

  /*
   * smpBuildKeypressNoti - Build a Pairing Keypress Notification.
   */
  extern bStatus_t smpBuildKeypressNoti( smpKeyPressNoti_t *pKeyPress, uint8 *pBuf );

  /*
   * smpParseKeypressNoti - Parse a Pairing Keypress Notification.
   */
  extern bStatus_t smpParseKeypressNoti( uint8 *pBuf, smpKeyPressNoti_t *pKeyPress );

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SMP_H */
