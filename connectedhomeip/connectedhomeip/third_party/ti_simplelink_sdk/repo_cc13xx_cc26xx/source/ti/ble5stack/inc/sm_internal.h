/******************************************************************************

 @file  sm_internal.h

 @brief This file contains internal interfaces for the SM.

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

/*********************************************************************
 *
 * WARNING!!!
 *
 * THE API'S FOUND IN THIS FILE ARE FOR INTERNAL STACK USE ONLY!
 * FUNCTIONS SHOULD NOT BE CALLED DIRECTLY FROM APPLICATIONS, AND ANY
 * CALLS TO THESE FUNCTIONS FROM OUTSIDE OF THE STACK MAY RESULT IN
 * UNEXPECTED BEHAVIOR
 *
 */


#ifndef SM_INTERNAL_H
#define SM_INTERNAL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "l2cap.h"
#include "sm.h"
#include "smp.h"
#include "linkdb.h"
#include "linkdb_internal.h"

/*********************************************************************
 * MACROS
 */

#define PASSKEYBIT(z, i)             ((((*(uint32 *)z) >> (i)) & 0x00000001)  | 0x80)

/*********************************************************************
 * CONSTANTS
 */

// Security Manager Task Events
#define SM_TIMEOUT_EVT            0x0001    // Message timeout event
#define SM_PAIRING_STATE_EVT      0x0002    // Event used to progress to the next pairing state
#define SM_PAIRING_SEND_RSP_EVT   0x0004    // Send a Pairing Response and begin pairing
#define SM_P256_RETRY_EVT         0x0008    // Retry generating a P256 Key
#define SM_DHKEY_RETRY_EVT        0x0010    // Retry generating a Diffie-Hellman Key

// Security Manager ECC retry timeout
#define SM_ECC_RETRY_TIMEOUT      200       // 200 milliseconds

// Pairing states
#define SM_PAIRING_STATE_INITIALIZE                        0  // Pairing has started
#define SM_PAIRING_STATE_PAIRING_REQ_SENT                  1  // Initiator: Pairing Request has been sent, Responder: waiting for Pairing Request.
#define SM_PAIRING_STATE_WAIT_CONFIRM                      2  // Waiting for Confirm message
#define SM_PAIRING_STATE_WAIT_PASSKEY                      3  // Waiting for Passkey from app/profile
#define SM_PAIRING_STATE_WAIT_CONFIRM_PASSKEY              4  // Received Initiator Confirm message and waiting for Passkey from app/profile (responder only)
#define SM_PAIRING_STATE_WAIT_RANDOM                       5  // Waiting for Random message
#define SM_PAIRING_STATE_WAIT_STK                          6  // Waiting for STK process to finish
#define SM_PAIRING_STATE_WAIT_SLAVE_ENCRYPTION_INFO        7  // Waiting for Slave Encryption Info to be sent
#define SM_PAIRING_STATE_WAIT_SLAVE_MASTER_INFO            8  // Waiting for Slave Master Info to be sent
#define SM_PAIRING_STATE_WAIT_SLAVE_IDENTITY_INFO          9  // Waiting for Slave Identity Info to be sent
#define SM_PAIRING_STATE_WAIT_SLAVE_IDENTITY_ADDR_INFO     10 // Waiting for Slave Identity Addr Info to be sent
#define SM_PAIRING_STATE_WAIT_SLAVE_SIGNING_INFO           11 // Waiting for Slave Signing Info to be sent
#define SM_PAIRING_STATE_WAIT_MASTER_ENCRYPTION_INFO       12 // Waiting for Master Encryption Info to be sent
#define SM_PAIRING_STATE_WAIT_MASTER_MASTER_INFO           13 // Waiting for Master Master Info to be sent
#define SM_PAIRING_STATE_WAIT_MASTER_IDENTITY_INFO         14 // Waiting for Master Identity Info to be sent
#define SM_PAIRING_STATE_WAIT_MASTER_IDENTITY_ADDR_INFO    15 // Waiting for Master Identity Addr Info to be sent
#define SM_PAIRING_STATE_WAIT_MASTER_SIGNING_INFO          16 // Waiting for Master Signing Info to be sent
#define SM_PAIRING_STATE_WAIT_ENCRYPT                      17 // Waiting for LTK process to finish
#define SM_PAIRING_STATE_DONE                              18 // Closing out the pairing process

// LE Secure Connection Pairing States
#define SM_PAIRING_STATE_WAIT_LOCAL_REMOTE_PUBLICKEY       19 // Waiting for the Public Key of the local device
#define SM_PAIRING_STATE_WAIT_LOCAL_PUBLICKEY              20
#define SM_PAIRING_STATE_WAIT_REMOTE_PUBLICKEY             21 // Waiting for the Public Key of the remote device or Waiting to handle this case while local key is generated
#define SM_PAIRING_STATE_WAIT_LOCAL_DHKEY                  22 // Waiting for the DH Key of the local device
#define SM_PAIRING_STATE_WAIT_DHKEY_CHECK                  23 // Waiting for the DHKey Check value (Ea/Eb) to be sent from the remote device
#define SM_PAIRING_STATE_WAIT_NUMERIC_COMPARISON           24 // Waiting for the numeric comparison verdict from the app
#define SM_PAIRING_STATE_WAIT_MASTER_PASSKEY               25 // Waiting for the Initiator to send his passkey
#define SM_PAIRING_STATE_WAIT_SLAVE_PASSKEY                26 // Waiting for the Responder to inject secret rb.
#define SM_PAIRING_STATE_WAIT_OOB                          27 // Waiting for the Responder to catch up in OOB protocol (Responder only)
#define SM_PAIRING_STATE_FAIL_CONFIRM                      28 // For OOB on responder, waiting to receive a Random from the initiator only to send a pairing failed for the confirmation.
#define SM_PAIRING_STATE_FAIL_OOB_NOT_FOUND                29 // For OOB on responder, waiting to receive a Random from the initiator only to send a pairing failed for lack of local OOB data.
#define SM_PAIRING_STATE_WAIT_DHKEY_CHECK_FAIL_UNSPECIFIED 30 // Waiting for DHKey Check, Auth Stage 2 failed due to malloc failure (Responder only)

#if defined ( TESTMODES )
  // SM TestModes
  #define SM_TESTMODE_OFF                           0   // No Test mode
  #define SM_TESTMODE_NO_RESPONSE                   1   // Don't respond to any SM message
  #define SM_TESTMODE_SEND_BAD_CONFIRM              2   // Force a bad confirm value in the Confirm Message
  #define SM_TESTMODE_BAD_CONFIRM_VERIFY            3   // Force a bad confirm check of the received Confirm Message
  #define SM_TESTMODE_SEND_CONFIRM                  4   // Force a SMP Confirm message
  #define SM_TESTMODE_USE_PA_WITH_VALID_IRK         5   // Use Public Address and exchange valid IRK in the pairing process
#endif  // TESTMODES

// Pairing Types
#define SM_PAIRING_TYPE_INIT                        0 // Pairing has been started but the type hasn't been determined yet
#define SM_PAIRING_TYPE_JUST_WORKS                  1 // Pairing is Just Works
#define SM_PAIRING_TYPE_PASSKEY_INITIATOR_INPUTS    2 // Pairing is MITM Passkey with initiator inputs passkey
#define SM_PAIRING_TYPE_PASSKEY_RESPONDER_INPUTS    3 // Pairing is MITM Passkey with responder inputs passkey
#define SM_PAIRING_TYPE_PASSKEY_BOTH_INPUTS         4 // Pairing is MITM Passkey with both initiator and responder input passkey
#define SM_PAIRING_TYPE_OOB                         5 // Pairing is MITM OOB
#define SM_PAIRING_TYPE_NUMERIC_COMPARISONS         6 // Pairing is LE Secure Connections with Numeric Comparison

#define SM_PAIRING_STATE_WAIT                       500 // The default wait time between key distribution messages.

#define NUM_PASSKEY_BITS                            20 // number of bits in a passkey.

#define NO_NUMCMP                                   0  // Authenticated pairing is not Numeric Comparison

/*********************************************************************
 * TYPEDEFS
 */

typedef struct
{
  // Optimization: moving dHKeyCheck into out of scope confirm field.
  uint8  confirm[KEYLEN];       // calculated confirm value
  uint8  rand[SMP_RANDOM_LEN];  // First MRand or Srand, then RAND. Nonce for SC
//  uint8  dHKeyCheck[KEYLEN];    // DH Key check Ea/Eb.
} devPairing_t;

typedef struct
{
  uint8 sK[SM_ECC_KEY_LEN];      // private key (only used if supplied by Bond Manager)
  uint8 pK_x[SM_ECC_KEY_LEN];    // public key X-coordinate
  uint8 pK_y[SM_ECC_KEY_LEN];    // public key Y-coordinate
  uint8 dHKey[SM_ECC_KEY_LEN];   // shared secret (Diffie-Hellman key).
  uint8 macKey[KEYLEN];          // MacKey used during authentication stage 2.
} smpPairingSCKeys_t;

typedef struct
{
  // Variables
  uint8                 initiator;        // TRUE if initiator
  uint8                 state;            // Pairing state
  uint8                 taskID;           // Task ID of the app/profile that requested the pairing
  uint8                 authState;        // uses SM_AUTH_STATE_AUTHENTICATED & SM_AUTH_STATE_BONDING & SM_AUTH_STATE_SECURECONNECTION
  uint8                 type;             // ie. SM_PAIRING_TYPE_JUST_WORKS
  uint16                connectionHandle; // Connection Handle from controller,
  uint8                 tk[KEYLEN];       // Holds tk from app.  remote OOB data in OOB protocol.

  // Pairing components
  devPairing_t          myComp;           // This device's pairing components
  devPairing_t          devComp;          // The other device's components

  // Pointer structures (security keys, pairing data)
  smSecurityInfo_t      *pEncParams;      // Your (device's) encryption parameters
  smSecurityInfo_t      *pDevEncParams;   // Connected device's encryption parameters
  smIdentityInfo_t      *pIdInfo;         // Connected device's identity parameters
  smSigningInfo_t       *pSigningInfo;    // Connected device's signing parameters
  smLinkSecurityReq_t   *pSecReqs;        // Pairing Control info (local device pairing req/rsp data)
  smpPairingReq_t       *pPairDev;        // remote devices pairing req/rsp data

  // Secure Connections
  uint8                 passkeyCount;     // Counts passskey iterations. NUM_PASSKEY_BITS is maximum.
  smpPairingSCKeys_t    *pLocalSCKeys;    // local device's SC public/private keypair.
  smpPairingPublicKey_t *pRemoteSCKeys;   // remote devices SC public key pair.

} smPairingParams_t;

// Callback when an SMP message has been received on the Initiator or Responder.
typedef uint8 (*smProcessMsg_t)( linkDBItem_t *pLinkItem, uint8 cmdID, smpMsgs_t *pParsedMsg );

// Callback to send next key message, and sets state for next event on the Initiator or Responder.
typedef void (*smSendNextKeyInfo_t)( void );

// Callback to send Start Encrypt through HCI on the Initiator.
typedef bStatus_t (*smStartEncryption_t)( uint16 connHandle, uint8 *pLTK, uint16 div,
                                          uint8 *pRandNum, uint8 keyLen );

// Callback when an HCI BLE LTK Request has been received on the Responder.
typedef uint8 (*smProcessLTKReq_t)( uint16 connectionHandle, uint8 *pRandom, uint16 encDiv );

// Callback when an When public key exchange must wait until public keys are generated on the Responder.
typedef uint8 (*smFinishPKEx_t)( void );

// Callback when authentication stage 2 after a passkey/numeric comparison is received.
typedef void (*smAuthStageTwo_t)( void );

// Callback where a Responder will send its pairing response to the initiator.
// Note: this callback helps decrease the runtime call stack's peak.
typedef void (*smProcessPairRsp_t)( void );

// Initiator callback structure - must be setup by the Initiator.
typedef struct
{
  smProcessMsg_t      pfnProcessMsg;      // When SMP message received
  smSendNextKeyInfo_t pfnSendNextKeyInfo; // When need to send next key message
  smStartEncryption_t pfnStartEncryption; // When Start Encrypt requested
  smAuthStageTwo_t    pfnAuthStageTwo;    // When Secure Connection Authentication stage 2 begins
} smInitiatorCBs_t;

// Responder callback structure - must be setup by the Initiator.
typedef struct
{
  smProcessMsg_t      pfnProcessMsg;       // When SMP message received
  smSendNextKeyInfo_t pfnSendNextKeyInfo;  // When need to send next key message
  smProcessPairRsp_t  pfnProcessPairRsp;   // When a SMP pairing Response is to be sent
  smProcessLTKReq_t   pfnProcessLTKReq;    // When HCI BLE LTK Request received
  smFinishPKEx_t      pfnFinishPKExchange; // When public key exchange must wait until public keys are generated.
  smAuthStageTwo_t    pfnAuthStageTwo;     // When Secure Connections Authentication stage 2 begins
} smResponderCBs_t;

// Container for keys when Bond Manager does not want regeneration on every pairing.
typedef struct
{
  uint8 maxNumRecycles;
  uint8 numRecycles;
  uint8 pK_x[SM_ECC_KEY_LEN];
  uint8 pK_y[SM_ECC_KEY_LEN];
} smEccRecycleKeys_t;

// Encrypt function structure
typedef struct
{
  uint8 key[KEYLEN];              // Key to encrypt with
  uint8 plainTextData[KEYLEN];    // Plain Text data to encrypt
  uint8 result[KEYLEN];           // Result of encrypt (key + Plain Text data)
} sm_Encrypt_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Security Manager's OSAL task ID
extern uint8 smTaskID;

extern smPairingParams_t *pPairingParams;

extern smResponderCBs_t *pfnResponderCBs;

// Connection handle waiting for a SM response
extern uint16 smRspWaitingConnHandle;

extern smEccRecycleKeys_t localEccKeys;
// These are the "Debug Mode" keys as defined in Vol 3, Part H, section 2.3.5.6.1 of the BLE 5.2 Core spec
extern smpPairingPublicKey_t sm_eccKeys_sc_host_debug;

/*********************************************************************
 * SM Task Internal Functions
 */

extern uint8 smProcessHCIBLEEventCode( hciEvt_CmdComplete_t *pMsg );
extern uint8 smProcessHCIBLEMetaEventCode( hciEvt_CmdComplete_t *pMsg );
extern uint8 smProcessOSALMsg( osal_event_hdr_t *pMsg );
extern uint8 smpResponderProcessIncoming_hook( linkDBItem_t *pLinkItem, uint8 cmdID, smpMsgs_t *pParsedMsg );

/*********************************************************************
 * SM Manager Internal Functions
 */

extern bStatus_t generate_subkey( uint8 *pKey, uint8 *pK1, uint8 *pK2 );
extern void leftshift_onebit( uint8 *pInp, uint8 *pOutp );
extern void padding ( uint8 *pLastb, uint8 *pPad, uint8 length );
extern uint8 smAuthReqToUint8( authReq_t *pAuthReq );
extern bStatus_t smEncrypt( sm_Encrypt_t *pParam );
extern bStatus_t smEncryptLocal( uint8 *pKey, uint8 *pPlaintextData, uint8 *pEncryptedData );
extern void smGenerateRandBuf( uint8 *pRandNum, uint8 len );
extern void smStartRspTimer( uint16 connHandle );
extern void smStopRspTimer( void );
extern void smUint8ToAuthReq( authReq_t *pAuthReq, uint8 authReqUint8 );
extern bStatus_t sm_c1new( uint8 *pK, uint8 *pR, uint8 *pRes, uint8 *pReq, uint8 iat, uint8 *pIA, uint8 rat, uint8 *pRA, uint8 *pC1 );
extern bStatus_t sm_CMAC( uint8 *pK, uint8 *pM, uint16 mLen, uint8 *pMac, uint8 macLen );
extern bStatus_t sm_d1( uint8 *pK, uint16 d, uint8 *pD1 );
extern bStatus_t sm_dm( uint8 *pK, uint8 *pR, uint16 *pDm );
extern bStatus_t sm_f4( uint8 *U, uint8 *V, uint8 *X, uint8 Z, uint8 *output );
extern bStatus_t sm_f5( uint8 *W, uint8 *N1, uint8 *N2, uint8 *A1, uint8 *A2, uint8 *output );
extern bStatus_t sm_f6( uint8 *W, uint8 *N1, uint8 *N2, uint8 *R, uint8 * IOCap, uint8 *A1, uint8 *A2, uint8 *output );
extern bStatus_t sm_g2( uint8 *U, uint8 *V, uint8 *X, uint8 *Y, uint32 *output );
extern bStatus_t sm_s1( uint8 *pK, uint8 *pR1, uint8 *pR2, uint8 *pS1 );
extern void sm_xor( uint8 *p1, uint8* p2 );
extern void xor_128( uint8 *pA, CONST uint8 *pB, uint8 *pOutcome );

/*********************************************************************
 * SM Pairing Manager Internal Functions
 */

extern bStatus_t smDetermineIOCaps( uint8 initiatorIO, uint8 responderIO );
extern uint8 smDetermineKeySize( void );
extern void smEndPairing( uint8 status );
extern void smFreePairingParams( void );
extern uint8 smF5Wrapper( uint8 *nonceA, uint8 *nonceB );
extern bStatus_t smGenerateAddrInput(uint8 *addrA, uint8 *addrB);
extern bStatus_t smGenerateAddrInput_sPatch(uint8 *addrA, uint8 *addrB);
extern bStatus_t smGenerateConfirm( void );
extern uint8 smGenerateDHKeyCheck( uint8 *nonce1, uint8 *nonce2, uint8 *pR, uint8 isLocalIOCap );
extern bStatus_t smGeneratePairingReqRsp( void );
extern bStatus_t smGenerateRandMsg( void );
extern uint8 smGetECCKeys( void );
extern void smIncrementEccKeyRecycleCount(uint8 status);
extern void smLinkCheck( uint16 connectionHandle, uint8 changeType );
extern void smNextPairingState( void );
extern bStatus_t smOobSCAuthentication( void );
extern void smPairingSendEncInfo( uint16 connHandle, uint8 *pLTK );
extern void smPairingSendIdentityAddrInfo( uint16 connHandle, uint8 addrType, uint8 *pMACAddr );
extern void smPairingSendIdentityInfo( uint16 connHandle, uint8 *pIRK );
extern void smPairingSendMasterID( uint16 connHandle, uint16 ediv, uint8 *pRand );
extern void smPairingSendSigningInfo( uint16 connHandle, uint8 *pSRK );
extern void smProcessDataMsg( l2capDataEvent_t *pMsg );
extern uint8 smProcessEncryptChange( uint16 connectionHandle, uint8 reason );
extern void smUpdateEncInfo(uint16 connectionHandle, uint8 reason);
extern void smProcessPairingReq( linkDBItem_t *pLinkItem, gapPairingReq_t *pPairReq );
extern void smRegisterInitiator( smInitiatorCBs_t *pfnCBs );
extern void smRegisterResponder( smResponderCBs_t *pfnCBs );
extern bStatus_t smSavePairInfo( smpPairingReq_t *pPair );
extern void smSaveRemotePublicKeys( smpPairingPublicKey_t *pParsedMsg );
extern void smSendDHKeyCheck( void );
extern bStatus_t smSendFailAndEnd( uint16 connHandle, smpPairingFailed_t *pFailedMsg );
extern void smSendFailureEvt(void);
extern void smSetPairingReqRsp( smpPairingReq_t *pReq );
extern bStatus_t smSendPublicKeys( void );
extern bStatus_t smStartEncryption( uint16 connHandle, uint8 *pLTK, uint16 div, uint8 *pRandNum, uint8 keyLen );
extern void smTimedOut( void );
extern void sm_allocateSCParameters( void );
extern void sm_computeDHKey( void );
extern bStatus_t sm_c1( uint8 *pK, uint8 *pR, uint8 *pC1 );

extern uint8 smpProcessIncoming( uint16 connHandle, uint8 cmdID, smpMsgs_t *pParsedMsg );

/*********************************************************************
 * SM Initiatior Pairing Manager Internal Functions
 */

extern void setupInitiatorKeys( void );
extern void smEncLTK( void );
extern void smInitiatorAuthStageTwo( void );
extern void smInitiatorSendNextKeyInfo( void );

extern uint8 smpInitiatorProcessEncryptionInformation( smpEncInfo_t *pParsedMsg );
extern uint8 smpInitiatorProcessIdentityAddrInfo( smpIdentityAddrInfo_t *pParsedMsg );
extern uint8 smpInitiatorProcessIdentityInfo( smpIdentityInfo_t *pParsedMsg );
extern uint8 smpInitiatorProcessIncoming( linkDBItem_t *pLinkItem, uint8 cmdID, smpMsgs_t *pParsedMsg );
extern uint8 smpInitiatorProcessMasterID( smpMasterID_t *pParsedMsg );
extern uint8 smpInitiatorProcessPairingConfirm( smpPairingConfirm_t *pParsedMsg );
extern uint8 smpInitiatorProcessPairingDHKeyCheck( smpPairingDHKeyCheck_t *pParsedMsg );
extern uint8 smpInitiatorProcessPairingPubKey( smpPairingPublicKey_t *pParsedMsg );
extern uint8 smpInitiatorProcessPairingPubKey_sPatch( smpPairingPublicKey_t *pParsedMsg );
extern uint8 smpInitiatorProcessPairingRandom( smpPairingRandom_t *pParsedMsg,
                                               uint16_t connHandle);
extern uint8 smpInitiatorProcessPairingRsp( smpPairingRsp_t *parsedMsg );
extern uint8 smpInitiatorProcessPairingRsp_sPatch( smpPairingRsp_t *parsedMsg );
extern uint8 smpInitiatorProcessSigningInfo( smpSigningInfo_t *pParsedMsg );

/*********************************************************************
 * SM Response Pairing Manager Internal Functions
 */

extern uint8 smFinishPublicKeyExchange( void );
extern void smResponderAuthStageTwo( void );
extern uint8 smResponderProcessLTKReq( uint16 connectionHandle, uint8 *pRandom, uint16 encDiv );
extern void smResponderSendNextKeyInfo( void );

extern uint8 smpResponderProcessEncryptionInformation( smpEncInfo_t *pParsedMsg );
extern uint8 smpResponderProcessIdentityAddrInfo( smpIdentityAddrInfo_t *pParsedMsg );
extern uint8 smpResponderProcessIdentityInfo( smpIdentityInfo_t *pParsedMsg );
extern uint8 smpResponderProcessIncoming( linkDBItem_t *pLinkItem, uint8 cmdID, smpMsgs_t *pParsedMsg );
extern uint8 smpResponderProcessIncoming_sPatch( linkDBItem_t *pLinkItem, uint8 cmdID, smpMsgs_t *pParsedMsg );
extern uint8 smpResponderProcessMasterID( smpMasterID_t *pParsedMsg );
extern uint8 smpResponderProcessPairingConfirm( smpPairingConfirm_t *pParsedMsg );
extern uint8 smpResponderProcessPairingDHKeyCheck( smpPairingDHKeyCheck_t *pParsedMsg );
extern uint8 smpResponderProcessPairingPublicKey( smpPairingPublicKey_t *pParsedMsg );
extern uint8 smpResponderProcessPairingRandom( smpPairingRandom_t *pParsedMsg );
extern uint8 smpResponderProcessPairingReq( smpPairingReq_t *pParsedMsg );
extern uint8 smpResponderProcessPairingReq_sPatch( smpPairingReq_t *pParsedMsg );
extern uint8 smpResponderProcessSigningInfo( smpSigningInfo_t *pParsedMsg );
extern void smpResponderSendPairRspEvent(void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SM_INTERNAL_H */
