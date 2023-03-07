/******************************************************************************

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2010-2022, Texas Instruments Incorporated
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

/**
 *  @defgroup GAPBondMgr GAPBondMgr
 *  This module implements the GAP Bond Manager
 *  @{
 *  @file  gapbondmgr.h
 *       GAPBondMgr layer interface
 */

#ifndef GAPBONDMGR_H
#define GAPBONDMGR_H

#ifdef __cplusplus
extern "C"
{
#endif

/*-------------------------------------------------------------------
 * INCLUDES
 */
#include "gap.h"

/*-------------------------------------------------------------------
 * CONSTANTS
 */

/**
 * @defgroup GAPBondMgr_Constants GAP Bond Manager Constants
 * @{
 */

// This is a common file for the legacy and sysconfig examples,
// the parameters under ifndef SYSCFG are defined in this file for
// the legacy examples and generated using the sysconfig tool for
// the sysconfig examples
#ifndef SYSCFG
#if !defined ( GAP_BONDINGS_MAX )
/// Maximum number of bonds that can be saved in NV.
#define GAP_BONDINGS_MAX    10
#endif

#if !defined ( GAP_CHAR_CFG_MAX )
/// Maximum number of characteristic configuration that can be saved in NV.
#define GAP_CHAR_CFG_MAX    4
#endif
#endif //SYSCFG

/** @} End GAPBondMgr_Constants */

/**
 * @defgroup GAPBondMgr_Params GAP Bond Manager Parameters
 *
 * Parameters set via @ref GAPBondMgr_SetParameter
 * @{
 */

/**
 * Whether to allow pairing, and if so, whether to initiate pairing.
 * (Read/Write)
 *
 * size: uint8_t
 *
 * default: @ref GAPBOND_PAIRING_MODE_WAIT_FOR_REQ
 *
 * range: @ref GAPBondMgr_Pairing_Modes
 */
#define GAPBOND_PAIRING_MODE          0x400

/**
 * Whether to use authenticated pairing (Read/Write)
 *
 * size: uint8_t
 *
 * default: TRUE
 *
 * range: TRUE (use) or FALSE (do not use)
 */
#define GAPBOND_MITM_PROTECTION       0x402

/**
 * The I/O capabilities of the local device (Read/Write)
 *
 * size: uint8_t
 *
 * default: @ref GAPBOND_IO_CAP_DISPLAY_ONLY
 *
 * range: @ref GAPBondMgr_IO_Caps
 */
#define GAPBOND_IO_CAPABILITIES       0x403

/**
 * Whether to use OOB for pairing (Read/Write)
 *
 * size: uint8_t
 *
 * default: FALSE
 *
 * range: TRUE (use) or FALSE (do not use)
 */
#define GAPBOND_OOB_ENABLED           0x404

/**
 * OOB data to use for pairing (Read/Write)
 *
 * size: uint8_t[16]
 *
 * default: 0x00000000000000000000000000000000 -
 *
 * range: 0x00000000000000000000000000000000 -
 * 0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
 */
#define GAPBOND_OOB_DATA              0x405

/**
 * Whether to bond after pairing (Read/Write)
 *
 * size: uint8_t
 *
 * default: FALSE
 *
 * range: TRUE (request) or FALSE (do not request)
 */
#define GAPBOND_BONDING_ENABLED       0x406

/**
 * Key distribution list for pairing (Read/Write)
 *
 * Multiple values can be used via bitwise ORing
 *
 * size: uint8_t
 *
 * default: @ref GAPBOND_KEYDIST_SENCKEY | @ref GAPBOND_KEYDIST_SIDKEY
 * | @ref GAPBOND_KEYDIST_MIDKEY | @ref GAPBOND_KEYDIST_MSIGN
 *
 * range: @ref GAPBondMgr_Key_Distr
 */
#define GAPBOND_KEY_DIST_LIST         0x407

/**
 * Erase all bonds from SNV and remove all bonded devices (Write)
 *
 * @note The erase won't happen until there are no active connections.
 */
#define GAPBOND_ERASE_ALLBONDS        0x409

/// @cond NODOC
#define GAPBOND_AUTO_FAIL_PAIRING     0x40A
#define GAPBOND_AUTO_FAIL_REASON      0x40B
/// @endcond // NODOC

/**
 * Key Size used in pairing. (Read/Write)
 *
 * size: uint8_t
 *
 * default: 16
 *
 * range: @ref GAP_PARAM_SM_MIN_KEY_LEN @ref GAP_PARAM_SM_MAX_KEY_LEN
 */
#define GAPBOND_KEYSIZE               0x40C

/**
 * Synchronize the whitelist with bonded devices (Read/Write)
 *
 * If TRUE, the whitelist will first be cleared. Then, each unique address
 * stored by bonds in SNV will be synched with the whitelist indefinitely or
 * until this is set to FALSE
 *
 * size: uint8_t
 *
 * default: FALSE
 *
 * range: TRUE (sync) or FALSE (don't sync)
 */
#define GAPBOND_AUTO_SYNC_WL          0x40D

/**
 * Gets the total number of bonds stored in NV (Read-only)
 *
 * size: uint8_t
 *
 * range: 0 255
 */
#define GAPBOND_BOND_COUNT            0x40E

/**
 * The action that the device takes after an unsuccessful bonding
 * attempt (Read / Write)
 *
 * size: uint8_t
 *
 * default: @ref GAPBOND_FAIL_TERMINATE_LINK
 *
 * range: @ref GAPBondMgr_Bond_Failure_Actions
 */
#define GAPBOND_BOND_FAIL_ACTION      0x40F

/**
 * Erase a single bonded device (Write-only)
 *
 * @note In case @ref GAPBOND_ERASE_BOND_IN_CONN is disabled,
 *       The erase won't happen until there are no active connections.
 *       In case @ref GAPBOND_ERASE_BOND_IN_CONN is enabled,
 *       the bond erase can happen also when if there are active connections.
 *
 * size: uint8_t[B_ADDR_LEN]
 *
 * range: A one-byte address type followed by a 6 byte address
 */
#define GAPBOND_ERASE_SINGLEBOND      0x410

/**
 * Define Secure Connections Usage during Pairing (Read/Write)
 *
 * @note Per ESR 11 E7226, "If LE Secure Connections pairing is supported
 * by the device, then the SC field shall be set to 1, otherwise it shall be set
 * to 0." Therefore, only testmode can choose GAPBOND_SECURE_CONNECTION_NONE.
 *
 * size: uint8_t
 *
 * default: @ref GAPBOND_SECURE_CONNECTION_ALLOW
 *
 * range: @ref GAPBondMgr_Secure_Cxns
 */
#define GAPBOND_SECURE_CONNECTION     0x411

/**
 * ECC Key Regeneration Policy (Write Only)
 *
 * Only applicable for Secure Connections. Define reuse of the private and
 * public ECC keys for multiple pairings.The default is to always regenerate the
 * keys upon each new pairing. This parameter has no effect when the application
 * specifies the keys using the @ref GAPBOND_ECC_KEYS parameter. The behavior
 * is that upon each pairing the number of recycles remaining is decremented by
 * 1, but if the pairing fails the count is decremented by 3. The specification
 * recommends that this value be set to no higher than 10 to avoid an attacker
 * from learning too much about a private key before it is regenerated.
 *
 * size: uint8_t
 *
 * default: 2
 *
 * range: 0 256
 */
#define GAPBOND_ECCKEY_REGEN_POLICY   0x412

/**
 * The private and public keys to use for Secure Connections pairing
 * (Read/Write)
 *
 * Only applicable for Secure Connections. Allows the application to specify the
 * private and public keys to use pairing. When this is set, the keys are used
 * indefinitely even if a regeneration policy was set with
 * @ref GAPBOND_ECCKEY_REGEN_POLICY. To make the Bond Manager stop using these
 * keys, pass a 1 byte value of 0x00. These keys are stored in RAM and are not
 * retained in non-volatile memory. These keys can be defined by the
 * application, or the application can request them using the
 * @ref SM_GetEccKeys command.
 *
 * size: gapBondEccKeys_t
 *
 * default: by default, the keys are generated using
 * @ref GAPBOND_ECCKEY_REGEN_POLICY
 *
 * range: A valid @ref gapBondEccKeys_t structure <br>
 * 0x00: previously passed keys will no longer be used
 */
#define GAPBOND_ECC_KEYS              0x413

/**
 * Whether to enable SC_HOST_DEBUG
 *
 * size: uint8_t
 *
 * default: FALSE
 *
 * range: TRUE (enabled) or FALSE (do not enabled)
 */
#define GAPBOND_SC_HOST_DEBUG          0x414

/**
 * Allows to erase a bond even if there are active connections
 *
 * If TRUE,  a bond can be erased even if there are active connections.
 * If FALSE, a bond can be erased only when there are no active connections.
 *
 * @note Once @ref GAPBOND_ERASE_SINGLEBOND is called the bond would be erased
 *       according to the value of the flag and the number of connections.
 *
 * size: uint8_t
 *
 * default: FALSE
 *
 * range: TRUE (erase a bond when there are active connections).
 *        FALSE(erase a bond when there are no active connections).
 */
#define GAPBOND_ERASE_BOND_IN_CONN     0x415

/**
 * Allows to receive debug keys during pairing, in the public keys exchange phase.
 *
 * If TRUE,  receiving debug keys is allowed.
 * If FALSE, receiving debug keys is not allowed, pairing will fail with invalid
 *           paramters code.
 *
 * @note if both devices are allowing it, and both are sending debug keys, it
 *       will fail due to mirror attack suspicion
 *
 * size: uint8_t
 *
 * default: TRUE
 *
 * range: TRUE (allow receiving debug keys).
 *        FALSE(reject debug keys).
 */
#define GAPBOND_ALLOW_DEBUG_KEYS       0x416

/**
 * Enable / Disable LRU Bond Replacement Scheme (Read/Write)
 *
 * Whether to enable the least recently used bond scheme so that, when a  newly
 * bonded device is added and all the entries are full, the least recently used
 * device’s bond is deleted to make room.
 *
 * size: uint8_t
 *
 * default: FALSE
 *
 * range: TRUE (enable) or FALSE (disable)
 */
#define GAPBOND_LRU_BOND_REPLACEMENT  0x418

/**
 * Erase Local NV Info (Write-only)
 *
 * Erase local IRK, CSRK, address mode, LRU list, random address.
 *
 * @note The erase won't happen until there are no active connections.
 *
 */
#define GAPBOND_ERASE_LOCAL_INFO      0x41A

/**
 * Whether to enfore authenticated pairing (Read/Write)
 *
 * size: uint8_t
 *
 * default: FALSE
 *
 * range: TRUE (enforce) or FALSE (do not enforce)
 */
#define GAPBOND_AUTHEN_PAIRING_ONLY   0x41B

/** @} End GAPBondMgr_Params */

/**
 *  @addtogroup GAPBondMgr_Constants
 *  @{
 */

/**
 * @defgroup GAPBondMgr_Pairing_Modes GAP Bond Manager Pairing Modes
 * @{
 */
/// Pairing is not allowed
#define GAPBOND_PAIRING_MODE_NO_PAIRING          0x00
/// Wait for a pairing request or slave security request
#define GAPBOND_PAIRING_MODE_WAIT_FOR_REQ        0x01
/// Don't wait, initiate a pairing request or slave security request
#define GAPBOND_PAIRING_MODE_INITIATE            0x02
/** @} End GAPBondMgr_Pairing_Modes */

/**
 * @defgroup GAPBondMgr_IO_Caps GAP Bond Manager I/O Capabilities
 * @{
 */
/// Display Only Device
#define GAPBOND_IO_CAP_DISPLAY_ONLY              0x00
/// Display and Yes and No Capable
#define GAPBOND_IO_CAP_DISPLAY_YES_NO            0x01
/// Keyboard Only
#define GAPBOND_IO_CAP_KEYBOARD_ONLY             0x02
/// No Display or Input Device
#define GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT        0x03
/// Both Keyboard and Display Capable
#define GAPBOND_IO_CAP_KEYBOARD_DISPLAY          0x04
/** @} End GAPBondMgr_IO_Caps */

/**
 * @defgroup GAPBondMgr_Key_Distr GAP Bond Manager Key Distribution
 * @{
 */
/// Slave Encryption Key
#define GAPBOND_KEYDIST_SENCKEY                  0x01
/// Slave IRK and ID information
#define GAPBOND_KEYDIST_SIDKEY                   0x02
/// Slave CSRK
#define GAPBOND_KEYDIST_SSIGN                    0x04
/// Slave Link Key
#define GAPBOND_KEYDIST_SLINK                    0x08
/// Master Encryption Key
#define GAPBOND_KEYDIST_MENCKEY                  0x10
/// Master IRK and ID information
#define GAPBOND_KEYDIST_MIDKEY                   0x20
/// Master CSRK
#define GAPBOND_KEYDIST_MSIGN                    0x40
/// Master Link Key
#define GAPBOND_KEYDIST_MLINK                    0x80
/** @} End GAPBondMgr_Key_Distr */

/**
 * GAP Bond Manager NV layout
 *
 * The NV definitions:
 *     BLE_NVID_GAP_BOND_START - starting NV ID
 *     gapBond_maxBonds - Maximum number of bonding allowed (32 is max for
 *                        number of NV IDs allocated in bcomdef.h).
 *
 * A single bonding entry consists of 6 components (NV items):
 *     Bond Record - defined as gapBondRec_t and uses GAP_BOND_REC_ID_OFFSET
 *         for an NV ID
 *     local LTK Info - defined as gapBondLTK_t and uses
 *         GAP_BOND_LOCAL_LTK_OFFSET for an NV ID
 *     device LTK Info - defined as gapBondLTK_t and uses
 *         GAP_BOND_DEV_LTK_OFFSET for an NV ID
 *     device IRK - defined as "uint8_t devIRK[KEYLEN]" and uses
 *         GAP_BOND_DEV_IRK_OFFSET for an NV ID
 *     device CSRK - defined as "uint8_t devCSRK[KEYLEN]" and uses
 *        GAP_BOND_DEV_CSRK_OFFSET for an NV ID
 *     device Sign Counter - defined as a uint32_t and uses
 *        GAP_BOND_DEV_SIGN_COUNTER_OFFSET for an NV ID
 *
 * When the device is initialized for the first time, all (gapBond_maxBonds) NV
 * items are created and initialized to all 0xFF's. A bonding record of all
 * 0xFF's indicates that the bonding record is empty and free to use.
 *
 * The calculation for each bonding records NV IDs:
 *    MAIN_RECORD_NV_ID = ((bondIdx * GAP_BOND_REC_IDS) +
 *                         BLE_NVID_GAP_BOND_START)
 *    LOCAL_LTK_NV_ID = (((bondIdx * GAP_BOND_REC_IDS) +
 *                       GAP_BOND_LOCAL_LTK_OFFSET) + BLE_NVID_GAP_BOND_START)
 *
 */

#define GAP_BOND_REC_ID_OFFSET              0 //!< NV ID for the main bonding record
#define GAP_BOND_LOCAL_LTK_OFFSET           1 //!< NV ID for the bonding record's local LTK information
#define GAP_BOND_DEV_LTK_OFFSET             2 //!< NV ID for the bonding records' device LTK information
#define GAP_BOND_DEV_IRK_OFFSET             3 //!< NV ID for the bonding records' device IRK
#define GAP_BOND_DEV_CSRK_OFFSET            4 //!< NV ID for the bonding records' device CSRK
#define GAP_BOND_DEV_SIGN_COUNTER_OFFSET    5 //!< NV ID for the bonding records' device Sign Counter
#define GAP_BOND_REC_IDS                    6

// Bonded State Flags
#define GAP_BONDED_STATE_AUTHENTICATED                  0x01
#define GAP_BONDED_STATE_SERVICE_CHANGED                0x02
#define GAP_BONDED_STATE_CAR                            0x04
#define GAP_BONDED_STATE_SECURECONNECTION               0x08
#define GAP_BONDED_STATE_RPA_ONLY                       0x10

/** @} End GAPBondMgr_Constants */

/**
 * @defgroup GAPBondMgr_Events GAP Bond Manager Pairing State Events
 *
 * These are the states that are returned through the @ref pfnPairStateCB_t.
 * See the specific state for its possible statuses.
 * @{
 */

/**
 * Pairing Started
 *
 * The following statuses are possible for this state:
 * @ref SUCCESS : pairing has been initiated. A pairing request has been either
 *      sent or received.
 */
#define GAPBOND_PAIRING_STATE_STARTED             0x00

/**
 * Pairing Complete
 *
 * The following statuses are possible for this state:
 * @ref SUCCESS : pairing pairing is complete (Session keys have been exchanged)
 * @ref SMP_PAIRING_FAILED_PASSKEY_ENTRY_FAILED : user input failed
 * @ref SMP_PAIRING_FAILED_OOB_NOT_AVAIL : Out-of-band data not available
 * @ref SMP_PAIRING_FAILED_AUTH_REQ : Input and output capabilities of devices
 * do not allow for authentication
 * @ref SMP_PAIRING_FAILED_CONFIRM_VALUE : the confirm value does not match the
 * calculated compare value
 * @ref SMP_PAIRING_FAILED_NOT_SUPPORTED : pairing is unsupported
 * @ref SMP_PAIRING_FAILED_ENC_KEY_SIZE : encryption key size is insufficient
 * @ref SMP_PAIRING_FAILED_CMD_NOT_SUPPORTED : The SMP command received is
 * unsupported on this device
 * @ref SMP_PAIRING_FAILED_UNSPECIFIED : encryption failed to start
 * @ref bleTimeout : pairing failed to complete before timeout
 * @ref bleGAPBondRejected : keys did not match
 */
#define GAPBOND_PAIRING_STATE_COMPLETE            0x01

/**
 * Encryption has started
 *
 * This event is returned after encryption has started, either using the keys
 * from the pairing that has just completed, or using keys from NV for a bonded
 * device.
 *
 * The following statuses are possible for this state:
 * @ref SUCCESS : pairing has been initiated. A pairing request has been either
 *      sent or received.
 * @ref HCI_ERROR_CODE_PIN_KEY_MISSING : encryption key is missing
 * @ref HCI_ERROR_CODE_UNSUPPORTED_REMOTE_FEATURE : feature is unsupported by
 *      the remote device
 * @ref HCI_ERROR_CODE_LMP_LL_RESP_TIMEOUT : Timeout waiting for response
 * @ref bleGAPBondRejected (0x32): this is received due to one of the previous
 *      three errors
 */
#define GAPBOND_PAIRING_STATE_ENCRYPTED           0x02
/// Bonding record saved in NV
#define GAPBOND_PAIRING_STATE_BOND_SAVED          0x03
/// Central Address Resolution Characteristic Read
#define GAPBOND_PAIRING_STATE_CAR_READ            0x04
/// Resolvable Private Address Only Characteristic Read
#define GAPBOND_PAIRING_STATE_RPAO_READ           0x05
/// Get ECC keys done
#define GAPBOND_GENERATE_ECC_DONE				  0x06
/** @} End GAPBondMgr_Events */

/**
 *  @addtogroup GAPBondMgr_Constants
 *  @{
 */

/**
 * @defgroup GAPBondMgr_Pairing_Failed Pairing failure status values
 * @{
 */
/**
 * The user input of the passkey failed, for example, the user canceled the
 * operation.
 */
#define SMP_PAIRING_FAILED_PASSKEY_ENTRY_FAILED   0x01
/// The OOB data is not available
#define SMP_PAIRING_FAILED_OOB_NOT_AVAIL          0x02
/**
 * The pairing procedure can't be performed as authentication requirements
 * can't be met due to IO capabilities of one or both devices
 */
#define SMP_PAIRING_FAILED_AUTH_REQ               0x03
/// The confirm value doesn't match the calculated compare value
#define SMP_PAIRING_FAILED_CONFIRM_VALUE          0x04
/// Pairing isn't supported by the device
#define SMP_PAIRING_FAILED_NOT_SUPPORTED          0x05
/**
 * The resultant encryption key size is insufficient for the security
 * requirements of this device.
 */
#define SMP_PAIRING_FAILED_ENC_KEY_SIZE           0x06
/// The SMP command received is not supported on this device.
#define SMP_PAIRING_FAILED_CMD_NOT_SUPPORTED      0x07
/// Pairing failed due to an unspecified reason
#define SMP_PAIRING_FAILED_UNSPECIFIED            0x08
/**
 * Pairing or authentication procedure is disallowed because too little time has
 * elapsed since the last pairing request or security request.
 */
#define SMP_PAIRING_FAILED_REPEATED_ATTEMPTS      0x09
/**
 * Indicates that the command length is invalid or that a parameter is outside of
 * the specified range.
 */
#define SMP_PAIRING_FAILED_INVALID_PARAMETERS     0x0A
/**
 * Indicates to the remote device that the DHKey Check value received does not
 * match the one calculated by the local device.
 */
#define SMP_PAIRING_FAILED_DHKEY_CHECK_FAILED     0x0B
/**
 * Indicates that the confirm value in the numeric comparison protocol do not
 * match.
 */
#define SMP_PAIRING_FAILED_NUM_COMPARISON_FAILED  0x0C
/** @} End GAPBondMgr_Pairing_Failed */

/**
 * @defgroup GAPBondMgr_Bond_Failure_Actions Bonding Failure Actions
 * @{
 */
/// Take no action upon unsuccessful bonding
#define GAPBOND_FAIL_NO_ACTION                         0x00
/// Initiate pairing upon unsuccessful bonding
#define GAPBOND_FAIL_INITIATE_PAIRING                  0x01
/// Terminate link upon unsuccessful bonding
#define GAPBOND_FAIL_TERMINATE_LINK                    0x02
/**
 * Terminate link and erase all existing bonds on device upon unsuccessful
 * bonding
 */
#define GAPBOND_FAIL_TERMINATE_ERASE_BONDS             0x03
/** @} End GAPBondMgr_Bond_Failure_Actions */

/**
 * @defgroup GAPBondMgr_Secure_Cxns GAP Bond Manager Secure Connections options
 * @{
 */
/// Secure Connections not supported
#define GAPBOND_SECURE_CONNECTION_NONE                 0x00
/// Secure Connections are supported
#define GAPBOND_SECURE_CONNECTION_ALLOW                0x01
/// Secure Connections Only Mode
#define GAPBOND_SECURE_CONNECTION_ONLY                 0x02
/** @} End GAPBondMgr_Secure_Cxns */

/**
 * @defgroup GAPBondMgr_ECC_Policies GAP Bond Manager ECC Re-use count before
 * regeneration
 *
 * Any value in this range is acceptable, but the higher re-use count, the more
 * an attacker can learn about the keys in each attack.
 * @{
 */
/// Regenerate after each usage
#define GAPBOND_REGENERATE_ALWAYS                       0x00
/// Never regenerate, always use the same pair.
#define GAPBOND_REGENERATE_NEVER                        0xFF
/** @} End GAPBondMgr_ECC_Policies */

/// ECC Key Length: 256 bit keys
#define ECC_KEYLEN                                     32

/** @} End GAPBondMgr_Constants */

/*-------------------------------------------------------------------
 * TYPEDEFS
 */

/**
 * @defgroup GAPBondMgr_CBs GAP Bond Manager Callbacks
 * @{
 * These are functions whose pointers are passed from the application
 * to the GAPBondMgr so that the GAPBondMgr can send events to the application
 */

/**
 * Passcode and Numeric Comparison Callback Function
 *
 * This callback returns to the application the peer device information when a
 * passcode is requested during the paring process or when numeric comparison is
 * used
 *
 * @param deviceAddr Pointer to 6-byte device address which the current pairing
 *        process relates to
 * @param connectionHandle Connection handle of the current pairing process
 * @param uiInputs if TRUE, the local device should accept a passcode input.
 * @param uiOutputs if TRUE, the local device should display the passcode.
 * @param numComparison If this is a nonzero value, then it is the code that
 *        should be displayed for numeric comparison pairing. If this is zero,
 *        then passkey entry pairing is occurring.
*/
typedef void (*pfnPasscodeCB_t)
(
  uint8_t  *deviceAddr,
  uint16_t connectionHandle,
  uint8_t  uiInputs,
  uint8_t  uiOutputs,
  uint32_t numComparison
);

/**
 * Pairing State Callback Function
 *
 * This callback returns the current pairing state to the application whenever
 * the state changes and the current status of the pairing or bonding process
 * associated with the current state.
 *
 * @param connectionHandle connection handle of current pairing process
 * @param state @ref GAPBondMgr_Events
 * @param status pairing status
 */
typedef void (*pfnPairStateCB_t)
(
  uint16_t connectionHandle,
  uint8_t  state,
  uint8_t  status
);

/**
 * GAPBondMgr Callback Structure
 *
 * This must be setup by the application and passed to the GAPBondMgr when
 * @ref GAPBondMgr_Register is called.
 */
typedef struct
{
  pfnPasscodeCB_t     passcodeCB;       //!< Passcode callback
  pfnPairStateCB_t    pairStateCB;      //!< Pairing state callback
} gapBondCBs_t;

/** @} End GAPBondMgr_CBs */

/**
 * @defgroup GAPBondMgr_Structs GAP Bond Manager Structures
 * @{
 */

/// @brief Public and Private ECC Keys
typedef struct
{
  uint8_t privateKey[ECC_KEYLEN];     //!< private key
  uint8_t publicKeyX[ECC_KEYLEN];     //!< public key X
  uint8_t publicKeyY[ECC_KEYLEN];     //!< prublic key Y
} gapBondEccKeys_t;

//structure of OOB data
typedef struct gapBondOOBData
{
    uint8 confirm[KEYLEN];              //calculated/received confirm value
    uint8 rand[KEYLEN];                 //calculated/received random number
} gapBondOOBData_t;

/// @brief Structure of NV data for the connected device's encryption information
typedef struct
{
  uint8_t   LTK[KEYLEN];              // Long Term Key (LTK)
  uint16_t  div;  //lint -e754        // LTK eDiv
  uint8_t   rand[B_RANDOM_NUM_SIZE];  // LTK random number
  uint8_t   keySize;                  // LTK key size
} gapBondLTK_t;

/// @brief Structure of NV data for the connected device's address information
typedef struct
{
  /**
   * Peer's address
   *
   * If identity information exists for this bond, this will be an
   * identity address
   */
  uint8_t               addr[B_ADDR_LEN];
  /**
   * Peer's address type
   */
  GAP_Peer_Addr_Types_t addrType;
  /**
   * State flags of bond
   *
   * @ref GAP_BONDED_STATE_FLAGS
   */
  uint8_t               stateFlags;
} gapBondRec_t;

/// @brief Structure of NV data for the connected device's characteristic configuration
typedef struct
{
  uint16_t attrHandle;  // attribute handle
  uint8_t  value;       // attribute value for this device
} gapBondCharCfg_t;

/** @} End GAPBondMgr_Structs */
/*-------------------------------------------------------------------
 * API's
 */

/**
 * Register callback functions with the bond manager.
 *
 * @param pCB pointer to callback function structure.
 */
extern void GAPBondMgr_Register(gapBondCBs_t *pCB);

/**
 * Set a GAP Bond Manager parameter.
 *
 * @note The "len" field must be set to the size of a "uint16_t" and the
 * "pValue" field must point to a "uint16_t".
 *
 * @param param @ref GAPBondMgr_Params
 * @param len length of data to write
 * @param pValue pointer to data to write.  This is dependent on
 *        the parameter ID and WILL be cast to the appropriate
 *        data type (example: data type of uint16_t will be cast to
 *        uint16_t pointer).
 *
 * @return @ref SUCCESS
 * @return @ref INVALIDPARAMETER
 */
extern bStatus_t GAPBondMgr_SetParameter(uint16_t param, uint8_t len,
    void *pValue);

/**
 * Get a GAP Bond Manager parameter.
 *
 * @note The "pValue" field must point to a "uint16_t".
 *
 * @param param Profile parameter ID: @ref GAPBondMgr_Params
 * @param pValue pointer to location to get the value.  This is dependent on
 *        the parameter ID and WILL be cast to the appropriate
 *        data type (example: data type of uint16_t will be cast to
 *        uint16_t pointer).
 *
 * @return @ref SUCCESS
 * @return @ref INVALIDPARAMETER
 */
extern bStatus_t GAPBondMgr_GetParameter(uint16_t param, void *pValue);

/**
 * Start (or re-start) the pairing process
 *
 * This API can be used to initiate pairing in cases where it is not started
 * automatically by the gapbondmgr or to re-pair after the initial pairing has
 * occurred if, for example, the pairing requirements have changed.
 *
 * @note Pairing will occur automatically if @ref GAPBOND_PAIRING_MODE is set to
 * @ref GAPBOND_PAIRING_MODE_INITIATE or @ref GAPBOND_PAIRING_MODE is set to
 * @ref GAPBOND_PAIRING_MODE_WAIT_FOR_REQ and a pairing request is received. In
 * these cases, this API is not needed and will fail since pairing is already
 * in progress.
 *
 * @param connHandle connection handle to initiate pairing with
 *
 * @return @ref SUCCESS
 * @return @ref bleAlreadyInRequestedMode already pairing
 */
extern bStatus_t GAPBondMgr_Pair(uint16_t connHandle);

/**
 * @brief  Search for an address in the bonding table. If the address is a
 * random private resolvable address, attempt to resolve the random address
 * against all IRK's in bonding table. As output parameters, return the
 * following:
 * - index into bonding table if the address (or resolved address) is found
 * - identity address type if a resolved address was found. Only valid if the
 *   input address is random private resolvable
 * - identity address if a resolved address was found. Only valid if the
 *   input address is random private resolvable
 *
 * Note that the output parameters are optional. If it is not desired for them
 * to be returned, a NULL pointer can safely be passed.
 *
 * @param pDevAddr peer's address
 * @param addrType peer's address type
 * @param pIdx pointer to byte to put the bond index if found
 * @param pIdentityAddrType pointer to byte to put the identity address type
 * if applicable
 * @param pIdentityAddr pointer to buffer to put the identity address type
 * if applicable
 *
 * @return SUCCESS if the address was found or resolved to an address in the
 * bonding table
 * @return INVALIDPARAMETER if a NULL address is passed in
 * @return bleGAPNotFound if the address was not found and didn't resolve to any
 * addresses in the bonding table
 */
extern bStatus_t GAPBondMgr_FindAddr(uint8_t *pDevAddr,
                                     GAP_Peer_Addr_Types_t addrType,
                                     uint8_t *pIdx,
                                     GAP_Peer_Addr_Types_t *pIdentityAddrType,
                                     uint8_t *pIdentityAddr);

/**
 * Set/clear the service change indication in a bond record.
 *
 * @param connectionHandle connection handle of the connected device or 0xFFFF
 *                                 if all devices in database.
 * @param setParam TRUE to set the service change indication, FALSE to clear it.
 *
 * @return @ref SUCCESS bond record found and changed,
 * @return @ref bleNoResources bond record not found
 * @return @ref bleNotConnected connection not found connectionHandle is invalid
 *         (for non-0xFFFF connectionHandle).
 */
extern bStatus_t GAPBondMgr_ServiceChangeInd(uint16_t connectionHandle,
                                             uint8_t setParam);

/**
 * Respond to a passcode request.
 *
 * @param connectionHandle connection handle of the connected device or 0xFFFF
 *        if all devices in database.
 * @param status SUCCESS if passcode is available, otherwise see
 *        @ref GAPBondMgr_Pairing_Failed.
 * @param passcode integer value containing the passcode.
 *
 * @return @ref SUCCESS bond record found and changed,
 * @return @ref bleIncorrectMode Link not found.
 * @return @ref INVALIDPARAMETER : passcode is out of range
 * @return @ref bleMemAllocError : heap is out of memory
 */
extern bStatus_t GAPBondMgr_PasscodeRsp(uint16_t connectionHandle,
                                        uint8_t status, uint32_t passcode);

/**
 * @brief   Get local Random number and confirm value for secure connection
 *
 * @design /ref did_244389729
 *
 * @param   localOobData - Struct holding local's random number and confirm value
 *
 * @return  SUCCESS - Finished successfully.
 *          FAILURE - One of the parameters returned FAILURE
 */
extern bStatus_t GAPBondMgr_SCGetLocalOOBParameters(gapBondOOBData_t *localOobData);

/**
 * @brief   Set remote's OOB parameters for secure connection
 *
 * @design /ref did_244389729
 *
 * @param   remoteOobData - struct holding remote random number and confirm value
 * @param   OOBDataFlag - 1 - OOB Authentication data from remote device present, else 0
 *
 * @return  SUCCESS - Finished successfully.
 *          FAILURE - If OOBDataFlag is set but remote's OOB parameters are not
 *          or if OOBDataFlag is not set but OOB parameters are set.
 */
extern bStatus_t GAPBondMgr_SCSetRemoteOOBParameters(gapBondOOBData_t *remoteOobData,
                                                     uint8 OOBDataFlag);

/**
 * @brief	Generates ECC keys.
 *
 * @design /ref did_244389729
 *
 * @return	SUCCESS - if processing.
 *	        FAILURE - if SM is pairing
 */
extern bStatus_t GAPBondMgr_GenerateEccKeys( void );

/**
 * @brief   Read bond record from NV
 *
 * @param   pBondRec - basic bond record
 * @param   pLocalLTK - LTK used by this device during pairing
 * @param   pDevLTK - LTK used by the peer device during pairing
 * @param   pIRK - IRK used by the peer device during pairing
 * @param   pSRK - SRK used by the peer device during pairing
 * @param   signCounter - Sign counter used by the peer device during pairing
 * @param   charCfg - GATT characteristic configuration
 *
 * @return  SUCCESS if bond was extracted
 *          bleGAPNotFound if there is no bond record
 */
extern uint8_t gapBondMgrReadBondRec(GAP_Peer_Addr_Types_t addrType,
                                     uint8_t *pDevAddr,
                                     gapBondRec_t* pBondRec,
                                     gapBondLTK_t* pLocalLtk,
                                     gapBondLTK_t* pDevLtk,
                                     uint8_t* pIRK,
                                     uint8_t* pSRK,
                                     uint32_t signCount,
                                     gapBondCharCfg_t* charCfg);

/**
 * @brief   Import bond record to NV
 *
 * @param   pBondRec - basic bond record
 * @param   pLocalLTK - LTK used by the device that has the same public address as current device
 * @param   pDevLTK - LTK used by the peer device during pairing
 * @param   pIRK - IRK used by the peer device during pairing
 * @param   pSRK - SRK used by the peer device during pairing
 * @param   signCounter - Sign counter used by the peer device during pairing
 * @param   charCfg - GATT characteristic configuration
 *
 * @return  SUCCESS if bond was imported
 * @        bleNoResources if there are no empty slots
 */
extern uint8_t gapBondMgrImportBond(gapBondRec_t* pBondRec,
                              gapBondLTK_t* pLocalLtk,
                              gapBondLTK_t* pDevLtk,
                              uint8_t* pIRK,
                              uint8_t* pSRK,
                              uint32_t signCount,
                              gapBondCharCfg_t* charCfg);
/*-------------------------------------------------------------------
-------------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif /* GAPBONDMGR_H */

/** @} End GAPBondMgr */
