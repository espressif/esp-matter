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
 *  @file  sm.h
 *  @brief    This file contains the interface to the SM.
 */

#ifndef SM_H
#define SM_H

#ifdef __cplusplus
extern "C"
{
#endif

/*-------------------------------------------------------------------
 * INCLUDES
 */
#include "bcomdef.h"
#include "osal.h"

#include "hci.h"

/*-------------------------------------------------------------------
 * MACROS
 */

/*-------------------------------------------------------------------
 * CONSTANTS
 */
/** @addtogroup GAPBondMgr_Constants
 *  @{
 */

/**
 * @defgroup SM_IO_CAP_DEFINES SM I/O Capabilities
 * @{
 */
#define DISPLAY_ONLY              0x00  //!< Display Only Device
#define DISPLAY_YES_NO            0x01  //!< Display and Yes and No Capable
#define KEYBOARD_ONLY             0x02  //!< Keyboard Only
#define NO_INPUT_NO_OUTPUT        0x03  //!< No Display or Input Device
#define KEYBOARD_DISPLAY          0x04  //!< Both Keyboard and Display Capable
/** @} End SM_IO_CAP_DEFINES */

#define SM_AUTH_MITM_MASK(a)    (((a) & 0x04) >> 2)   //!< MITM Mask

/**
 * @defgroup SM_PASSKEY_TYPE_DEFINES SM Passkey Types (Bit Masks)
 * @{
 */
#define SM_PASSKEY_TYPE_INPUT   0x01    //!< Input the passkey
#define SM_PASSKEY_TYPE_DISPLAY 0x02    //!< Display the passkey
/** @} End SM_PASSKEY_TYPE_DEFINES */

/**
 * @defgroup SM_BONDING_FLAGS_DEFINES SM AuthReq Bonding Flags
 * Bonding flags 0x02 and 0x03 are reserved.
 * @{
 */
#define SM_AUTH_REQ_NO_BONDING    0x00  //!< No bonding
#define SM_AUTH_REQ_BONDING       0x01  //!< Bonding
/** @} End SM_BONDING_FLAGS_DEFINES */

#define PASSKEY_LEN     6   //!< Passkey Character Length (ASCII Characters)

#define SM_AUTH_STATE_AUTHENTICATED       0x04  //!< Authenticate requested
#define SM_AUTH_STATE_BONDING             0x01  //!< Bonding requested
#define SM_AUTH_STATE_SECURECONNECTION    0x08  //!< Secure Connection requested

#define SM_ECC_KEY_LEN 32  //!< ECC Key length in bytes

/* SM private/public key regeneration policy */
#define SM_ECC_KEYS_NOT_AVAILABLE     0xFF  //!< Initial state of recycled keys before they exist.
#define SM_ECC_KEYS_REGNENERATE_NEVER 0xFF  //!< Never regenerate the keys.
#define SM_ECC_KEYS_REGENERATE_ALWAYS 0x00  //!< Always regenerate the keys.

/**
 * @defgroup SM_MESSAGE_EVENT_OPCODES SM Message opcocdes
 * @{
 */
#define SM_ECC_KEYS_EVENT 0x00 //!< ECC Keys
#define SM_DH_KEY_EVENT   0x01 //!< Diffie-Hellman key
/** @} End SM_MESSAGE_EVENT_OPCODES */

/** @} */ // end of GAPBondMgr_Constants

/*-------------------------------------------------------------------
 * General TYPEDEFS
 */

/**
 * SM_NEW_RAND_KEY_EVENT message format.  This message is sent to the
 * requesting task.
 */
typedef struct
{
  osal_event_hdr_t  hdr;      //!< SM_NEW_RAND_KEY_EVENT and status
  uint8 newKey[KEYLEN];       //!< New key value - if status is SUCCESS
} smNewRandKeyEvent_t;

/**
 * header type for ECC and ECDH commands
 */
typedef struct
{
  osal_event_hdr_t hdr;     //!< osal event header
  uint8 opcode;             //!< op code
} smEventHdr_t;

/**
 * SM_ECC_KEYS_EVENT message format for ECC keys.  This message is sent to
 * the request task.
 */
typedef struct
{
  osal_event_hdr_t hdr;             //!< SM_MSG_EVENT and status
  uint8 opcode;                     //!< SM_ECC_KEYS_EVENT
  uint8 privateKey[SM_ECC_KEY_LEN]; //!< ECC private key.
  uint8 publicKeyX[SM_ECC_KEY_LEN]; //!< ECC public key X-coordinate.
  uint8 publicKeyY[SM_ECC_KEY_LEN]; //!< ECC public key Y-coordinate.
} smEccKeysEvt_t;

/**
 * SM_DH_KEY_EVENT message format for ECDH keys.
 * This message is sent to the request task.
 */
typedef struct
{
  osal_event_hdr_t hdr;        //!< SM_MSG_EVENT and status
  uint8 opcode;                //!< SM_DH_KEY_EVENT
  uint8 dhKey[SM_ECC_KEY_LEN]; //!< ECC Diffie-Hellman key
} smDhKeyEvt_t;

/**
 * Key Distribution field  - True or False fields
 */
typedef struct
{
  unsigned int sEncKey:1;    //!< Set to distribute slave encryption key
  unsigned int sIdKey:1;     //!< Set to distribute slave identity key
  unsigned int sSign:1;      //!< Set to distribute slave signing key
  unsigned int sLinkKey:1;   //!< Set to derive slave link key from slave LTK
  unsigned int sReserved:4;  //!< Reserved for slave - don't use
  unsigned int mEncKey:1;    //!< Set to distribute master encryption key
  unsigned int mIdKey:1;     //!< Set to distribute master identity key
  unsigned int mSign:1;      //!< Set to distribute master signing key
  unsigned int mLinkKey:1;   //!< Set to derive master link key from master LTK
  unsigned int mReserved:4;  //!< Reserved for master - don't use
} keyDist_t;

/**
 * ECC keys for pairing.
 */
typedef struct
{
  uint8 isUsed;               //!< FALSE if not used.  USE_PKEYS if public keys only.  USE_ALL_KEYS if public and private keys.
  uint8 sK[SM_ECC_KEY_LEN];   //!< private key (only used if supplied by Bond Manager)
  uint8 pK_x[SM_ECC_KEY_LEN]; //!< public key X-coordinate
  uint8 pK_y[SM_ECC_KEY_LEN]; //!< public key Y-coordinate
} smEccKeys_t;

/**
 * Link Security Requirements
 */
typedef struct
{
  uint8 ioCaps;                 //!< I/O Capabilities (ie.
  uint8 oobAvailable;           //!< True if remote Out-of-band key available
  uint8 oob[KEYLEN];            //!< Out-Of-Bounds key from remote device
  uint8 oobConfirm[KEYLEN];     //!< Out-Of-Bounds confirm from remote device. Secure Connections only.
  uint8 localOobAvailable;      //!< True if local Out-of-band key available. Secure Connections only.
  uint8 localOob[KEYLEN];       //!< Out-Of-Bounds local data. Secure Connections only.
  uint8 isSCOnlyMode;           //!< TRUE if Secure Connections Only Mode. Secure Connections only.
  smEccKeys_t eccKeys;          //!< Optionally specified ECC keys for pairing. Secure Connections only.
  uint8 authReq;                //!< Authentication Requirements
  keyDist_t keyDist;            //!< Key Distribution mask
  uint8 maxEncKeySize;          //!< Maximum Encryption Key size (7-16 bytes)
} smLinkSecurityReq_t;

/**
 * Link Security Information
 */
typedef struct
{
  uint8 ltk[KEYLEN];              //!< Long Term Key (LTK)
  uint16 div;                     //!< LTK Diversifier
  uint8 rand[B_RANDOM_NUM_SIZE];  //!< LTK random number
  uint8 keySize;                  //!< LTK Key Size (7-16 bytes)
} smSecurityInfo_t;

/**
 * Link Identity Information
 */
typedef struct
{
  uint8 irk[KEYLEN];          //!< Identity Resolving Key (IRK)
  uint8 addrType;             //!< Address type for BD_ADDR
  uint8 bd_addr[B_ADDR_LEN];  //!< The advertiser may set this to zeroes to not disclose its BD_ADDR (public address).
} smIdentityInfo_t;

/**
 * Signing Information
 */
typedef struct
{
  uint8  srk[KEYLEN]; //!< Signature Resolving Key (CSRK)
  uint32 signCounter; //!< Sign Counter
} smSigningInfo_t;

/**
 * Pairing Request & Response - authReq field
 */
typedef struct
{
  unsigned int bonding:2;    //!< Bonding flags
  unsigned int mitm:1;       //!< Man-In-The-Middle (MITM)
  unsigned int sc:1;         //!< LE Secure Connections (SC)
  unsigned int kp:1;         //!< LE SC Keypress Notifications
  unsigned int ct2:1;        //!< h7 Link Key conversion support (not used in LE)
  unsigned int reserved:2;   //!< Reserved - don't use
} authReq_t;

/**
 * Application callback to the SM for ECC keys and Diffie-Hellman Shared Secret (ECDH).
 */
typedef void (*smCB_t)
(
uint8 *secretKey,  //!< Private key when ECC keys were requested.  ECDH shared secret when DHKey is requested.
uint8 *publicKeyX, //!< Local Public Key X-Coordinate.  Not used when DHKey is returned.
uint8 *publicKeyY  //!< Local Public Key Y-Coordinate when ECC keys were requested.  Not used when DHKey is returned.
);

/*-------------------------------------------------------------------
 * GLOBAL VARIABLES
 */

/*-------------------------------------------------------------------
 * FUNCTIONS - APPLICATION API - Functions the application may call
 */

/**
 * @brief   Register Task with the Security Manager to receive
 *          ECC keys or a ECDH shared secret.
 *
 * @param  taskID - Application's task ID.
 */
extern void SM_RegisterTask( uint8 taskID );

/**
 * @brief  Get ECC private and public keys.  Keys are returned through the
 *         callback registered in SM_RegisterCb.
 *
 * @return @ref SUCCESS if processing
 * @return @ref FAILURE if SM is pairing
 * @return @ref HCI_ERROR_CODE_MEM_CAP_EXCEEDED if out of heap memory.
 */
extern bStatus_t SM_GetEccKeys( void );

/**
 * @brief   Request a Diffie-Hellman key from the ECC engine. Key is returned
 *          through the callback registered in SM_RegisterCb, in the first
 *          parameter.
 *
 * @param   secretKey - the local private key
 * @param   publicKeyX - the remote public key, X-Coordinate.
 * @param   publicKeyY - the remote public key, Y-Coordinate.
 *
 * @return @ref SUCCESS if processing
 * @return @ref FAILURE if SM is pairing
 * @return @ref HCI_ERROR_CODE_MEM_CAP_EXCEEDED if out of heap memory.
 */
extern bStatus_t SM_GetDHKey( uint8 *secretKey, uint8 *publicKeyX,
                              uint8 *publicKeyY );

/**
 * @brief   Generate a confirm value for OOB data.
 *
 * @param   publicKey - the local public key X-coordinate
 * @param   oob       - the random string to be sent to the remote device via
 *                      oob
 * @param   pOut      - pointer to the generated confirm value.  The calling
 *                      application must copy this data into its own buffer.
 *
 * @return @ref SUCCESS
 * @return @ref INVALIDPARAMETER
 * @return @ref bleMemAllocError
 */
extern bStatus_t SM_GetScConfirmOob( uint8 *publicKey, uint8 *oob, uint8 *pOut );

/**
* @fn          SM_GenerateRandBuf
*
* @brief       Generate a buffer with random numbers.
*
* @param       random - pointer to buffer
* @param       len - length of buffer.
*
* @return      none
*/
extern void SM_GenerateRandBuf( uint8 *random, uint8 len );

/**
* @fn          SM_SetAuthenPairingOnlyMode
*
* @brief       Set the device into Authenticated Pairing Only Mode.
*              If enabled and the key generation method does not result
*              in a key that provides Authenticated MITM protection then
*              the device will send the Pairing Failed command with the
*              error code Authentication Requirements.
*
* @param       state - set TRUE to enable Authenticated Pairing Only Mode
*                      set FALSE to disable
*
* @return      none
*/
extern void SM_SetAuthenPairingOnlyMode( uint8 state );

/**
* @fn          SM_SetAllowDebugKeysMode
*
* @brief       Set the device policy for allowance of receiving the debug keys
*              defined in the spec. If not allowed, the pairing will be rejected
*              with code of invalid parameters. If allowed, pairing will
*              conttinue noramlly.
*
* @param       state - set TRUE to allow debug keys as peer public keys
*                      set FALSE to disallow
*
* @return      none
*/
extern void SM_SetAllowDebugKeysMode( uint8 state );
/**
* @fn          SM_GetAuthenPairingOnlyMode
*
* @brief       Get the device into Authenticated Pairing Only Mode.
*              If enabled and the key generation method does not result
*              in a key that provides Authenticated MITM protection then
*              the device will send the Pairing Failed command with the
*              error code Authentication Requirements.
*
* @param       none
*
* @return      state - TRUE Authenticated Pairing Only Mode is enabled
*                      FALSE when disabled
*/
extern uint8 SM_GetAuthenPairingOnlyMode( void );
/**
* @fn          SM_GetAllowDebugKeysMode
*
* @brief       Get the device debug keys allownce policy.
*              If not allowed, the pairing will be rejected
*              with code of invalid parameters. If allowed, pairing will
*              continue noramlly.
*
* @param       none
*
* @return      state - TRUE debug keys as peer public keys are allowed
*                      FALSE when disallowed
*/
extern uint8 SM_GetAllowDebugKeysMode( void );
/*-------------------------------------------------------------------
 * FUNCTIONS - MASTER API - Only use these in a master device
 */

/**
 * @brief Initialize SM Initiator on a master device.
 *
 * @warning Only use this API on a master device
 *
 * @return @ref SUCCESS
 */
extern bStatus_t SM_InitiatorInit( void );

/**
 * @brief Start the pairing process.
 *
 * This function is also called if the device is already bound.
 *
 * @note  Only one pairing process at a time per device.
 *
 * @warning Only use this API on a master device
 *
 * @param       initiator - TRUE to start pairing as Initiator.
 * @param       taskID - task ID to send results.
 * @param       connectionHandle - Link's connection handle
 * @param       pSecReqs - Security parameters for pairing
 *
 * @return @ref SUCCESS
 * @return @ref INVALIDPARAMETER
 * @return @ref bleAlreadyInRequestedMode
 */
extern bStatus_t SM_StartPairing(  uint8 initiator,
                                   uint8 taskID,
                                   uint16 connectionHandle,
                                   smLinkSecurityReq_t *pSecReqs );

/**
 * @brief       Send Start Encrypt through HCI
 *
 * @warning Only use this API on a master device
 *
 * @param       connHandle - Connection Handle
 * @param       pLTK - pointer to 16 byte lkt
 * @param       div - div or ediv
 * @param       pRandNum - pointer to 8 byte random number
 * @param       keyLen - length of LTK (bytes)
 *
 * @return @ref SUCCESS
 * @return @ref INVALIDPARAMETER
 */
extern bStatus_t SM_StartEncryption( uint16 connHandle, uint8 *pLTK,
                                     uint16 div, uint8 *pRandNum,
                                     uint8 keyLen );


/*-------------------------------------------------------------------
 * FUNCTIONS - SLAVE API - Only use these in a slave device
 */

/**
 * @brief       Initialize SM Responder on a slave device.
 *
 * @warning Only use this API on a slave device
 *
 * @return @ref SUCCESS
 */
extern bStatus_t SM_ResponderInit( void );

/*-------------------------------------------------------------------
 * FUNCTIONS - GENERAL API - both master and slave
 */

/**
 * @brief       Generate a key with a random value.
 *
 * @param       taskID - task ID to send results.
 *
 * @return @ref SUCCESS
 * @return @ref bleNotReady
 * @return @ref bleMemAllocError
 * @return @ref FAILURE
 */
extern bStatus_t SM_NewRandKey( uint8 taskID );

/**
 * @brief       Encrypt the plain text data with the key..
 *
 * @param       pKey - key data
 * @param       pPlainText - Plain text data
 * @param       pResult - place to put the encrypted result
 *
 * @return @ref SUCCESS - if started
 * @return @ref INVALIDPARAMETER - one of the parameters are NULL
 * @return @ref bleAlreadyInRequestedMode
 * @return @ref bleMemAllocError
 */
extern bStatus_t SM_Encrypt( uint8 *pKey, uint8 *pPlainText, uint8 *pResult );

/**
 * Generate an outgoing Authentication Signature.
 *
 * @param pData message data
 * @param len length of pData
 * @param pAuthenSig place to put new signature
 * @param connHandle connection handle
 *
 * @return @ref SUCCESS - signature authentication generated
 * @return @ref INVALIDPARAMETER - pData or pAuthenSig is NULL
 * @return @ref bleMemAllocError
 */
extern bStatus_t SM_GenerateAuthenSig( uint8 *pData, uint16 len,
                                      uint8 *pAuthenSig, uint16 connHandle );

/**
 * @brief       Verify an Authentication Signature.
 *
 * @param       connHandle - connection to verify against.
 * @param       authentication - TRUE if requires an authenticated CSRK, FALSE if not
 * @param       pData - message data
 * @param       len - length of pData
 * @param       pAuthenSig - message signature to verify
 *
 * @return @ref SUCCESS - signature authentication verified
 * @return @ref FAILURE - if not verified
 * @return @ref bleNotConnected - Connection not found
 * @return @ref INVALIDPARAMETER - pData or pAuthenSig is NULL, or signCounter is invalid
 * @return @ref bleMemAllocError
 */
extern bStatus_t SM_VerifyAuthenSig( uint16 connHandle,
                                     uint8 authentication,
                                     uint8 *pData,
                                     uint16 len,
                                     uint8 *pAuthenSig );

/**
 * @brief       Update the passkey for the pairing process.
 *
 * @param       pPasskey - pointer to the 6 digit passkey
 * @param       connectionHandle - connection handle to link.
 *
 * @return @ref SUCCESS
 * @return @ref bleIncorrectMode - Not pairing
 * @return @ref INVALIDPARAMETER - link is incorrect
 */
extern bStatus_t SM_PasskeyUpdate( uint8 *pPasskey, uint16 connectionHandle );


/**
 * @brief       Update the regeneration count for ECC key re-usage.
 *
 * @param       count - number of times a keypair can be reused before regeneration.
 */
extern void SM_SetECCRegenerationCount(uint8 count);

/// @cond NODOC

/*-------------------------------------------------------------------
 * TASK API - These functions must only be called by OSAL.
 */

  /**
   * @internal
   *
   * @brief       SM Task Initialization Function.
   *
   * @param       taskID - SM task ID.
   */
  extern void SM_Init( uint8 task_id );

  /**
   * @internal
   *
   * @brief       SM Task event processing function.
   *
   * @param       taskID - SM task ID
   * @param       events - SM events.
   *
   * @return      events not processed
   */
  extern uint16 SM_ProcessEvent( uint8 task_id, uint16 events );

  /**
   * @internal
   *
   * @brief   Read P-256 Key Event Callback from the Controller.  The secret key
   *          used to generate the public key pair is also input here.
   *
   * @params  pK         - pointer to HCI BLE Read P256 Public Key Complete struct
   *                       which contain the X and Y Coordinates of the ECC Public
   *                       Keys.
   *
   * @param   privateKey - the private key used to generated the ECC Public Keys.
   */
  extern void SM_p256KeyCB( hciEvt_BLEReadP256PublicKeyComplete_t *pK,
                            uint8 *privateKey );

  /**
   * @internal
   *
   * @brief   BLE Generate Diffie-Hellman Key Event Callback from the controller.
   *
   * @params         - pointer to HCI BLE Read P256 Public Key Complete struct
   *                   which contain the X and Y Coordinates of the ECC Public
   *                   Keys.
   *
   * @param   pDhKey - the Diffie-Hellman X and Y Coordinates output from the
   *                   ECDH operation.
   */
  extern void SM_dhKeyCB( hciEvt_BLEGenDHKeyComplete_t *pDhKey );

/// @endcond //NODOC

/*-------------------------------------------------------------------
-------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* SM_H */
