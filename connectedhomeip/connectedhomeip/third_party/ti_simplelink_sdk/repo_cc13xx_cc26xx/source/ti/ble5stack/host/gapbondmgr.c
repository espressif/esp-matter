/******************************************************************************

 @file  gapbondmgr.c

 @brief GAP peripheral profile manages bonded connections

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2011-2022, Texas Instruments Incorporated
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

#if ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )

#if defined(GAP_BOND_MGR)

#if (defined(NO_OSAL_SNV))
#error "Bond Manager cannot be used since NO_OSAL_SNV used! Disable in buildConfig.opt"
#endif

/*********************************************************************
 * INCLUDES
 */
#include "bcomdef.h"
#include "osal.h"
#include "osal_snv.h"
#include "ll_enc.h"
#include "ll_privacy.h"
#include "l2cap.h"
#include "sm.h"
#include "linkdb.h"
#include "linkdb_internal.h"
#include "gap.h"
#include "gap_internal.h"
#include "gatt.h"
#include "gatt_uuid.h"
#include "hci.h"
#include "gattservapp.h"
#include "gapgattserver.h"
#include "gapbondmgr.h"
#include "gapbondmgr_internal.h"
#include "rom_jt.h"

#ifdef SYSCFG
#include "ti_ble_config.h"
#endif //SYSCFG

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
// Task event types
#define GAP_BOND_SYNC_CC_EVT                            BV(0) // Sync char config
#define GAP_BOND_SAVE_RCA_EVT                           BV(1) // Save reconnection address in NV
#define GAP_BOND_POP_PAIR_QUEUE_EVT                     BV(2) // Begin pairing with the next queued device
#define GAP_BOND_IDLE_EVT                               BV(3) // Resolve address if still idle

// Once NV usage reaches this percentage threshold, NV compaction gets triggered.
#define NV_COMPACT_THRESHOLD                            80

// Secure Connections minimum MTU size
#define SECURECONNECTION_MIN_MTU_SIZE                   65

// Central Device should only contain one Central Address Resolution
// Characteristic
#define NUM_CENT_ADDR_RES_CHAR                          1

// There only exists at most one instance of the Resolvable Private Address Only
// Characteristic.
#define NUM_RPAO_CHAR                                   1

// Central Address Resolution value shall be one octet in length. The Read by
// Type Value Response should be 3 total octets: 2 octets for attribute handle
// and 1 octet for value.
#define CENT_ADDR_RES_RSP_LEN                           3

// Resolvable Private Address Only value shall be one octet in length.  the Read
// by Type Value Response should be 3 total octets: 2 octets for attribute
// handle and 1 octet for value.
#define RPAO_RSP_LEN                                    3

// Resolvable Private Address Only has just one value:
// 0 - only Resolvable Private Addresses will be used as local addresses after
//      bonding
#define RPAO_MODE                                       0

// Key Size Limits
#define MIN_ENC_KEYSIZE                     7  //!< Minimum number of bytes for the encryption key
#define MAX_ENC_KEYSIZE                     16 //!< Maximum number of bytes for the encryption key

// Pairing Queue States
typedef enum gbmPairFSM
{
  GBM_STATE_WAIT_PAIRING = 0, // Waiting to begin pairing.
  GBM_STATE_IS_PAIRING,          // Waiting for pairing to complete
  GBM_STATE_WAIT_GATT_RPAO,      // Waiting for the RPAO Characteristic Read to complete.
#if ( HOST_CONFIG & PERIPHERAL_CFG )
  GBM_STATE_WAIT_GATT_CAR,       // Waiting for CAR Characteristic Read to complete.
#endif // PERIPHERAL_CFG
  GBM_STATE_END                  // Halt.
} gbmState_t;

#define GBM_STATE_INIT NULL

/*********************************************************************
 * TYPEDEFS
 */

typedef struct gapBondStateQueueNode
{
  struct gapBondStateQueueNode *pNext;
  gapPairingReq_t              *pPairReq;
  uint16_t                      connHandle;
  uint8_t                       addrType;
  gbmState_t                    state;
} gapBondStateNode_t;

typedef gapBondStateNode_t *gapBondStateNodePtr_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

extern uint8_t gapState;

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static uint8_t gapBondMgr_TaskID;    // Task ID for internal task/event processing

// GAPBonding Parameters
static uint8_t gapBond_PairingMode = GAPBOND_PAIRING_MODE_WAIT_FOR_REQ;
static uint8_t gapBond_MITM = FALSE;
static uint8_t gapBond_IOCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
static uint8_t gapBond_Bonding = FALSE;
static uint8_t gapBond_sc_host_debug = FALSE;
static uint8_t gapBond_allow_debug_keys = TRUE;
static uint8_t gapBond_maxBonds = GAP_BONDINGS_MAX;
static uint8_t gapBond_maxCharCfg = 4;
static uint8_t gapBond_gatt_no_client = 0;
static uint8_t gapBond_gatt_no_service_changed = 1;
static uint8_t gapBond_eraseBondInConnFlag = FALSE;
//Local OOB parameters
static uint8_t localOobAvailable = FALSE;
gapBondOOBData_t gapBond_localOobData;
//Remote OOB parameters
static uint8_t gapBond_OOBDataFlag = FALSE;
gapBondOOBData_t gapBond_remoteOobData = {0};
#if defined ( HCI_TL_FULL )
 /* for test mode only */
static uint8_t gapBond_AutoFail = FALSE;
static uint8_t gapBond_AutoFailReason = SMP_PAIRING_FAILED_NOT_SUPPORTED;
#endif // HCI_TL_FULL

static uint8_t gapBond_KeyDistList =
  (
    GAPBOND_KEYDIST_SENCKEY     // sEncKey enabled, to send the encryption key
    | GAPBOND_KEYDIST_SIDKEY   // sIdKey enabled, to send the IRK, and BD_ADDR
    | GAPBOND_KEYDIST_SSIGN    // sSign enabled, to send the CSRK
    | GAPBOND_KEYDIST_MENCKEY  // mEncKey enabled, to get the master's encryption key
    | GAPBOND_KEYDIST_MIDKEY   // mIdKey enabled, to get the master's IRK and BD_ADDR
    | GAPBOND_KEYDIST_MSIGN    // mSign enabled, to get the master's CSRK
  );
static uint8_t  gapBond_KeySize = MAX_ENC_KEYSIZE;

static uint8_t gapBond_secureConnection = GAPBOND_SECURE_CONNECTION_ALLOW;

static uint8_t gapBond_authenPairingOnly = FALSE;

// These are the "Debug Mode" keys as defined in Vol 3, Part H, section 2.3.5.6.1 of the BLE 5.2 Core spec
static gapBondEccKeys_t gapBond_eccKeys_sc_host_debug =
{
  { 0xBD, 0x1A, 0x3C, 0xCD, 0xA6, 0xB8, 0x99, 0x58, 0x99, 0xB7, 0x40, 0xEB,
    0x7B, 0x60, 0xFF, 0x4A, 0x50, 0x3F, 0x10, 0xD2, 0xE3, 0xB3, 0xC9, 0x74,
    0x38, 0x5F, 0xC5, 0xA3, 0xD4, 0xF6, 0x49, 0x3F
  },

  { 0xE6, 0x9D, 0x35, 0x0E, 0x48, 0x01, 0x03, 0xCC, 0xDB, 0xFD, 0xF4, 0xAC,
    0x11, 0x91, 0xF4, 0xEF, 0xB9, 0xA5, 0xF9, 0xE9, 0xA7, 0x83, 0x2C, 0x5E,
    0x2C, 0xBE, 0x97, 0xF2, 0xD2, 0x03, 0xB0, 0x20
  },

  { 0x8B, 0xD2, 0x89, 0x15, 0xD0, 0x8E, 0x1C, 0x74, 0x24, 0x30, 0xED, 0x8F,
    0xC2, 0x45, 0x63, 0x76, 0x5C, 0x15, 0x52, 0x5A, 0xBF, 0x9A, 0x32, 0x63,
    0x6D, 0xEB, 0x2A, 0x65, 0x49, 0x9C, 0x80, 0xDC
  }
};

static uint8 gapBond_useEccKeys = FALSE;
static gapBondEccKeys_t gapBond_eccKeys =
{
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  },
  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  }
};

#if ( HOST_CONFIG & CENTRAL_CFG )
static uint8_t  gapBond_BondFailOption = GAPBOND_FAIL_TERMINATE_LINK;
#endif

static const gapBondCBs_t *pGapBondCB = NULL;

// Local RAM shadowed bond records
static gapBondRec_t *bonds = NULL;      //will hold gapBond_maxBonds elements

static uint8_t autoSyncWhiteList = FALSE;

static uint8_t eraseAllBonds = FALSE;
static uint8_t eraseLocalInfo = FALSE;

static uint8_t *bondsToDelete = NULL;   //will hold gapBond_maxBonds elements

gapBondStateNodePtr_t gapBondStateNodeHead = GBM_STATE_INIT;

uint8_t gapBond_removeLRUBond = FALSE;
uint8_t *gapBond_lruBondList = NULL;    //will hold gapBond_maxBonds elements

// Global used to indicate whether Resolving List must be resynched with
// bond records once controller is no longer adv/init/scanning
uint8_t gapBond_syncRL = FALSE;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void gapBondMgr_LinkEst(GAP_Peer_Addr_Types_t addrType, uint8_t *pDevAddr,
                               uint16_t connHandle, uint8_t role);
static void gapBondMgr_LinkTerm(uint16_t connHandle);
static uint8_t gapBondMgrUpdateCharCfg(uint8_t idx, uint16_t attrHandle,
                                       uint16_t value);
static gapBondCharCfg_t *gapBondMgrFindCharCfgItem(uint16_t attrHandle,
                                                   gapBondCharCfg_t *charCfgTbl);
static void gapBondMgrInvertCharCfgItem(gapBondCharCfg_t *charCfgTbl);
static uint8_t gapBondMgrSaveBond(uint8_t bondIdx, gapBondRec_t* pBondRec,
                                  gapBondLTK_t* pLocalLtk, gapBondLTK_t* pDevLtk,
                                  uint8_t* pIRK, uint8_t* pSRK, uint32_t signCount,
                                  int8_t syncRL);
static uint8_t gapBondMgrAddBond(gapBondRec_t *pBondRec,
                                 gapAuthCompleteEvent_t *pPkt);
static uint8_t gapBondMgrGetStateFlags(uint8_t idx);

static void gapBondMgrReadBonds(void);
static uint8_t gapBondMgrFindEmpty(void);
static uint8_t gapBondMgrBondTotal(void);
static bStatus_t gapBondMgrEraseAllBondings(void);
static bStatus_t gapBondMgrEraseBonding(uint8_t idx);
static bStatus_t gapBondMgrEraseLocalInfo( void );
static uint8_t gapBondMgr_ProcessOSALMsg(osal_event_hdr_t *pMsg);
static void gapBondMgr_ProcessGATTMsg(gattMsgEvent_t *pMsg);
static uint8_t gapBondMgr_ProcessSMMsg(smEventHdr_t *pMsg);
static void gapBondMgr_ProcessGATTServMsg(gattEventHdr_t *pMsg);
static void gapBondMgrBondReq(uint16_t connHandle, uint8_t idx,
                              uint8_t stateFlags, uint8_t role,
                              uint8_t startEncryption);
static bStatus_t gapBondMgrAuthenticate(uint16_t connHandle, uint8_t addrType,
                                        gapPairingReq_t *pPairReq);
static void gapBondMgr_SyncWhiteList(void);
static uint8_t gapBondMgr_SyncCharCfg(uint16_t connHandle);

static void gapBondMgrReadLruBondList(void);
static uint8_t gapBondMgrGetLruBondIndex(void);
static void gapBondMgrUpdateLruBondList(uint8_t bondIndex);

//#if !defined GATT_NO_SERVICE_CHANGED || !defined GATT_NO_CLIENT
static uint8_t gapBondMgrChangeState(uint8_t idx, uint16_t state, uint8_t set);
//#endif // (!GATT_NO_SERVICE_CHANGED || !GATT_NO_CLIENT)

#if ( HOST_CONFIG & PERIPHERAL_CFG )
static bStatus_t gapBondMgrSlaveSecurityReq(uint16_t connHandle);
#endif // PERIPHERAL_CFG
#if ( HOST_CONFIG & CENTRAL_CFG )
static void gapBondMgr_SlaveReqSecurity(uint16_t connHandle, uint8_t authReq);
#endif // CENTRAL_CFG

//#ifndef GATT_NO_SERVICE_CHANGED
static void gapBondMgrSendServiceChange(linkDBItem_t *pLinkItem);
//#endif // GATT_NO_SERVICE_CHANGED

static void gapBondMgr_gapIdle(void);
static uint8_t gapBondGenerateUniqueIdentifier(GAP_Peer_Addr_Types_t addrType,
                                               uint8_t *pAddr);
static uint8_t gapBondPreprocessIdentityInformation(gapAuthCompleteEvent_t
                                                    *pPkt, gapBondRec_t *pRec);
static uint8_t gapBondMgr_isDevicePrivacyModeSupported(GAP_Peer_Addr_Types_t addrType,
                                                       uint8_t *pAddr);
static uint8_t gapBondMgr_SupportsCentAddrRes(uint8_t *pPeerAddr,
                                              GAP_Peer_Addr_Types_t addrType);

//#ifndef GATT_NO_CLIENT
static void gapBondMgrReadRPAORsp(uint16_t connHandle,
                                  attReadByTypeRsp_t *pRsp);
static void gapBondMgr_ProcessAttErrRsp(uint16_t connHandle,
                                        attErrorRsp_t *pRsp);
static bStatus_t gapBondMgrSetStateFlagFromConnhandle(uint16_t connHandle,
                                                      uint8_t stateFlag,
                                                      uint8_t setParam);

#if defined (HOST_CONFIG) && (HOST_CONFIG & PERIPHERAL_CFG)
static void gapBondMgrReadCARRsp(uint16_t connHandle, attReadByTypeRsp_t *pRsp);
#endif // HOST_CONFIG & PERIPHERAL_CFG
//#endif // !GATT_NO_CLIENT

static gapBondStateNodePtr_t gapBondMgrQueuePairing(uint16_t connHandle,
                                                    uint8_t addrType,
                                                    gapPairingReq_t *pPairReq);
static gapBondStateNodePtr_t gapBondFindPairReadyNode(void);

static bStatus_t gapBondStateStartSecurity(uint16_t connHandle,
                                           uint8_t addrType,
                                           gapPairingReq_t *pPairReq);

//#if !defined ( GATT_NO_CLIENT )
static bStatus_t gapBondStateStartNextGATTDisc(uint16_t connHandle);
static gbmState_t gapBondGetStateFromConnHandle(uint16_t connHandle);
static void gapBondStateDoGATTDisc(uint16_t connHandle, gbmState_t state);
//#endif //defined !GATT_NO_CLIENT

static bStatus_t gapBondStateEnd(uint16_t connHandle);
static void gapBondStateSetState(gapBondStateNode_t *pNode, gbmState_t state);

/*********************************************************************
 * HOST and GAP CALLBACKS
 */

static gapBondMgrCBs_t gapCBs =
{
  .suppCentAddrResCB = gapBondMgr_SupportsCentAddrRes,
  .gapIdleCB = gapBondMgr_gapIdle,
  .gapDevPrivModeCB = gapBondMgr_isDevicePrivacyModeSupported
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @brief   Set a GAP Bond Manager parameter.
 *
 * Public function defined in gapbondmgr.h.
 */
bStatus_t GAPBondMgr_SetParameter(uint16_t param, uint8_t len, void *pValue)
{
  bStatus_t ret = SUCCESS;  // return value

  switch(param)
  {
    case GAPBOND_PAIRING_MODE:
      if((len == sizeof(uint8_t)) &&
          (*((uint8_t *)pValue) <= GAPBOND_PAIRING_MODE_INITIATE))
      {
        gapBond_PairingMode = *((uint8_t *)pValue);
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;

    case GAPBOND_MITM_PROTECTION:
      if((len == sizeof(uint8_t)) && (*((uint8_t *)pValue) <= TRUE))
      {
        gapBond_MITM = *((uint8_t *)pValue);

        //For Authenticated pairing MITM bit must be set to TRUE
        if (gapBond_MITM == FALSE)
        {
          gapBond_authenPairingOnly = FALSE;
          SM_SetAuthenPairingOnlyMode(gapBond_authenPairingOnly);
        }
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;

    case GAPBOND_IO_CAPABILITIES:
      if((len == sizeof(uint8_t)) &&
          (*((uint8_t *)pValue) <= GAPBOND_IO_CAP_KEYBOARD_DISPLAY))
      {
        gapBond_IOCap = *((uint8_t *)pValue);
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;

    case GAPBOND_OOB_ENABLED:
      if((len == sizeof(uint8_t)) && (*((uint8_t *)pValue) <= TRUE))
      {
        gapBond_OOBDataFlag = *((uint8_t *)pValue);
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;

    case GAPBOND_OOB_DATA:
      if(len == KEYLEN)
      {
        VOID MAP_osal_memcpy(gapBond_remoteOobData.rand, pValue, KEYLEN) ;
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;

    case GAPBOND_BONDING_ENABLED:
      if((len == sizeof(uint8_t)) && (*((uint8_t *)pValue) <= TRUE))
      {
        gapBond_Bonding = *((uint8_t *)pValue);
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;

    case GAPBOND_SC_HOST_DEBUG:
      if((len == sizeof(uint8_t)) && (*((uint8_t *)pValue) <= TRUE))
      {
        gapBond_sc_host_debug = *((uint8_t *)pValue);
        if (gapBond_sc_host_debug)
        {
          gapBond_useEccKeys = TRUE;
          // Overide ecc Keys with local sc_host_debug keys
          MAP_osal_memcpy(&gapBond_eccKeys, &gapBond_eccKeys_sc_host_debug,
                    sizeof(gapBondEccKeys_t));
        }
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;

    case GAPBOND_ALLOW_DEBUG_KEYS:
      if((len == sizeof(uint8_t)) && (*((uint8_t *)pValue) <= TRUE))
      {
        gapBond_allow_debug_keys = *((uint8_t *)pValue);
        SM_SetAllowDebugKeysMode(gapBond_allow_debug_keys);
      }
      break;

    case GAPBOND_ERASE_BOND_IN_CONN:
      if((len == sizeof(uint8_t)) && (*((uint8_t *)pValue) <= TRUE))
      {
        gapBond_eraseBondInConnFlag = *((uint8_t *)pValue);
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;

    case GAPBOND_KEY_DIST_LIST:
      if(len == sizeof(uint8_t))
      {
        gapBond_KeyDistList = *((uint8_t *)pValue);
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;

    case GAPBOND_ERASE_ALLBONDS:
      if(len == 0)
      {
        // Make sure there's no active connection
        if(GAP_NumActiveConnections() == 0)
        {
          // Erase all bonding records
          ret =  gapBondMgrEraseAllBondings();

          // See if NV needs a compaction
          VOID osal_snv_compact(NV_COMPACT_THRESHOLD);

          // Make sure Bond RAM Shadow is up-to-date
          gapBondMgrReadBonds();
        }
        else
        {
          eraseAllBonds = TRUE;
        }
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;

    case GAPBOND_ERASE_SINGLEBOND:
      if(len == (B_ADDR_LEN + sizeof(uint8_t)))
      {
        uint8_t idx;
        uint8_t devAddr[B_ADDR_LEN];

        // Reverse bytes
        VOID MAP_osal_revmemcpy(devAddr, (uint8_t *)pValue + 1, B_ADDR_LEN);

        // Find index of address in bonding table
        if(GAPBondMgr_FindAddr(devAddr, *(GAP_Peer_Addr_Types_t *)(pValue),
                       		   &idx, NULL, NULL) == SUCCESS)
        {
          // In case the gapBond_eraseBondInConnFlag is disabled,
          // We would erase the bond only if there's no active connection.
          // In case the gapBond_eraseBondInConnFlag is enabled,
          // We can erase the bond even when there are active connections.
          if( ((gapBond_eraseBondInConnFlag == FALSE) &&
             (MAP_GAP_NumActiveConnections() == 0)) ||
             (gapBond_eraseBondInConnFlag == TRUE) )
          {
            // Erase bond
            ret = gapBondMgrEraseBonding(idx);

            // See if NV needs a compaction
            VOID osal_snv_compact(NV_COMPACT_THRESHOLD);

            // Make sure Bond RAM Shadow is up-to-date
            gapBondMgrReadBonds();
          }
          else
          {
            // Mark entry to be deleted when disconnected
            bondsToDelete[idx] = TRUE;
          }
        }
        else
        {
          ret = bleGAPNotFound;
        }
      }
      else
      {
        // Parameter is not the correct length
        ret = bleInvalidRange;
      }

      break;

#if defined ( HCI_TL_FULL )
    /* for test mode only */
    case GAPBOND_AUTO_FAIL_PAIRING:
      if((len == sizeof(uint8_t)) && (*((uint8_t *)pValue) <= TRUE))
      {
        gapBond_AutoFail = *((uint8_t *)pValue);
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;

    case GAPBOND_AUTO_FAIL_REASON:
      if((len == sizeof(uint8_t)) &&
          (*((uint8_t *)pValue) <= SMP_PAIRING_FAILED_REPEATED_ATTEMPTS))
      {
        gapBond_AutoFailReason = *((uint8_t *)pValue);
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;
#endif // HCI_TL_FULL

    case GAPBOND_KEYSIZE:
      if(len == sizeof(uint8_t) &&
         ((gapBond_secureConnection != GAPBOND_SECURE_CONNECTION_ONLY &&
         (*((uint8_t *)pValue) >= MIN_ENC_KEYSIZE) &&
         (*((uint8_t *)pValue) <= MAX_ENC_KEYSIZE)) ||
         // secure connections pairing must use 128-bit key
         (gapBond_secureConnection == GAPBOND_SECURE_CONNECTION_ONLY &&
         (*((uint8_t *)pValue) == MAX_ENC_KEYSIZE))))
      {
        gapBond_KeySize = *((uint8_t *)pValue);
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;

    case GAPBOND_AUTO_SYNC_WL:
      if(len == sizeof(uint8_t))
      {
        uint8_t oldVal = autoSyncWhiteList;

        autoSyncWhiteList = *((uint8_t *)pValue);

        // only call if parameter changes from FALSE to TRUE
        if((oldVal == FALSE) && (autoSyncWhiteList == TRUE))
        {
          // make sure bond is updated from NV
          gapBondMgrReadBonds();
        }
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;

#if ( HOST_CONFIG & CENTRAL_CFG )
    case GAPBOND_BOND_FAIL_ACTION:
      if((len == sizeof(uint8_t)) &&
         (*((uint8_t *)pValue) <= GAPBOND_FAIL_TERMINATE_ERASE_BONDS))
      {
        gapBond_BondFailOption = *((uint8_t *)pValue);
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;
#endif

    case GAPBOND_SECURE_CONNECTION:
      if((len == sizeof(uint8_t)) &&
         (*((uint8_t *)pValue) <= GAPBOND_SECURE_CONNECTION_ONLY)
#ifndef GAPBONDMGR_TESTMODE
         && (*((uint8_t *)pValue) != GAPBOND_SECURE_CONNECTION_NONE)
#endif // GAPBONDMGR_TESTMODE
         )
      {
        gapBond_secureConnection = *((uint8_t *)pValue);

        // Update linkDB so the security mode can be enforced.
        if(gapBond_secureConnection == GAPBOND_SECURE_CONNECTION_ONLY)
        {
          // Secure Connections pairing must use 128-bit key
          gapBond_KeySize = MAX_ENC_KEYSIZE;
          MAP_linkDB_SecurityModeSCOnly(TRUE);
        }
        else
        {
          MAP_linkDB_SecurityModeSCOnly(FALSE);
        }
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;

    case GAPBOND_ECCKEY_REGEN_POLICY:
      if(len == sizeof(uint8_t))
      {
        MAP_SM_SetECCRegenerationCount(*(uint8_t *)pValue);
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;

    case GAPBOND_ECC_KEYS:
      if (gapBond_sc_host_debug)
      {
         ret = bleInvalidRange;
         break;
      }

      if((len == sizeof(gapBondEccKeys_t) && pValue != 0))
      {
        // Copy.
        MAP_osal_memcpy(&gapBond_eccKeys, (gapBondEccKeys_t *)pValue,
                    sizeof(gapBondEccKeys_t));

        // Use these keys for future pairings.
        gapBond_useEccKeys = TRUE;
      }
      else if(len == sizeof(uint8_t) && pValue != 0)
      {
        gapBond_useEccKeys = *((uint8_t *)pValue);
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;

    case GAPBOND_LRU_BOND_REPLACEMENT:
      if(len == sizeof(uint8_t) && ((uint8_t *)pValue != NULL))
      {
        gapBond_removeLRUBond = *(uint8_t *)pValue;
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;

    case GAPBOND_ERASE_LOCAL_INFO:
      if(len == 0)
      {
        // Make sure there's no active connection
        if(GAP_NumActiveConnections() == 0)
        {
          // Erase all bonding records
          ret =  gapBondMgrEraseLocalInfo();

          // See if NV needs a compaction
          VOID osal_snv_compact(NV_COMPACT_THRESHOLD);
        }
        else
        {
          eraseLocalInfo = TRUE;
        }
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;

    case GAPBOND_AUTHEN_PAIRING_ONLY:
      if((len == sizeof(uint8_t)) && (*((uint8_t *)pValue) <= TRUE))
      {
        if (*((uint8_t *)pValue) == TRUE)
        {
          //For Authenticated pairing MITM shall be set to TRUE
          if (gapBond_MITM == FALSE)
          {
            ret = INVALIDPARAMETER;
            break;
          }
        }

        gapBond_authenPairingOnly = *((uint8_t *)pValue);
        SM_SetAuthenPairingOnlyMode(gapBond_authenPairingOnly);
      }
      else
      {
        ret = bleInvalidRange;
      }

      break;

    default:

      // The param value isn't part of this profile, try the GAP.
      if((param < GAP_PARAMID_MAX) && (len == sizeof(uint16_t)))
      {
        ret = (bStatus_t) MAP_GAP_SetParamValue((uint16_t)param,
                                            *((uint16_t *)pValue));
      }
      else
      {
        ret = INVALIDPARAMETER;
      }

      break;
  }

  return (ret);
}

/*********************************************************************
 * @brief   Get a GAP Bond Manager parameter.
 *
 * Public function defined in gapbondmgr.h.
 */
bStatus_t GAPBondMgr_GetParameter(uint16_t param, void *pValue)
{
  bStatus_t ret = SUCCESS;  // return value

  switch(param)
  {
    case GAPBOND_PAIRING_MODE:
      *((uint8_t *)pValue) = gapBond_PairingMode;
      break;

    case GAPBOND_MITM_PROTECTION:
      *((uint8_t *)pValue) = gapBond_MITM;
      break;

    case GAPBOND_IO_CAPABILITIES:
      *((uint8_t *)pValue) = gapBond_IOCap;
      break;

    case GAPBOND_OOB_ENABLED:
      *((uint8_t *)pValue) = gapBond_OOBDataFlag;
      break;

    case GAPBOND_OOB_DATA:
      VOID MAP_osal_memcpy(pValue, gapBond_remoteOobData.rand, KEYLEN) ;
      break;

    case GAPBOND_BONDING_ENABLED:
      *((uint8_t *)pValue) = gapBond_Bonding;
      break;

    case GAPBOND_KEY_DIST_LIST:
      *((uint8_t *)pValue) = gapBond_KeyDistList;
      break;

#if defined ( HCI_TL_FULL )
    /* for test mode only */
    case GAPBOND_AUTO_FAIL_PAIRING:
      *((uint8_t *)pValue) = gapBond_AutoFail;
      break;

    case GAPBOND_AUTO_FAIL_REASON:
      *((uint8_t *)pValue) = gapBond_AutoFailReason;
      break;
#endif // HCI_TL_FULL

    case GAPBOND_KEYSIZE:
      *((uint8_t *)pValue) = gapBond_KeySize;
      break;

    case GAPBOND_AUTO_SYNC_WL:
      *((uint8_t *)pValue) = autoSyncWhiteList;
      break;

    case GAPBOND_BOND_COUNT:
      *((uint8_t *)pValue) = gapBondMgrBondTotal();
      break;

#if ( HOST_CONFIG & CENTRAL_CFG )
    case GAPBOND_BOND_FAIL_ACTION:
      *((uint8_t *)pValue) = gapBond_BondFailOption;
      break;
#endif // ( HOST_CONFIG & CENTRAL_CFG )

    case GAPBOND_SECURE_CONNECTION:
      *((uint8_t *)pValue) = gapBond_secureConnection;
      break;

    case GAPBOND_ECC_KEYS:
      VOID MAP_osal_memcpy(pValue, &gapBond_eccKeys, sizeof(gapBondEccKeys_t));
      break;

    case GAPBOND_ALLOW_DEBUG_KEYS:
      *((uint8_t *)pValue) = gapBond_allow_debug_keys;
      break;

    case GAPBOND_ERASE_BOND_IN_CONN:
      *((uint8_t *)pValue) = gapBond_eraseBondInConnFlag;
      break;

    case GAPBOND_LRU_BOND_REPLACEMENT:
      *((uint8_t *)pValue) = gapBond_removeLRUBond;
      break;

    case GAPBOND_AUTHEN_PAIRING_ONLY:
      *((uint8_t *)pValue) = gapBond_authenPairingOnly;
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }

  return (ret);
}

/*********************************************************************
 * @brief   Resolve an address from bonding information.
 *
 * Public function defined in gapbondmgr.h.
 */
bStatus_t GAPBondMgr_FindAddr(uint8_t *pDevAddr,
                              GAP_Peer_Addr_Types_t addrType,
                              uint8_t *pIdx,
                              GAP_Peer_Addr_Types_t *pIdentityAddrType,
                              uint8_t *pIdentityAddr)
{
  // Ensure an address was passed in
  if (pDevAddr == NULL)
  {
    return (INVALIDPARAMETER);
  }

  // Check for valid address type
  if (addrType > PEER_ADDRTYPE_RANDOM_OR_RANDOM_ID)
  {
    return ( bleInvalidRange );
  }

  uint8_t idx = gapBond_maxBonds;

  // Search through bonding table
  for(idx = 0; idx < gapBond_maxBonds; idx++)
  {
    // Compare against address stored in bonding record.
    if(MAP_osal_memcmp(bonds[idx].addr, pDevAddr, B_ADDR_LEN) &&
       (addrType == bonds[idx].addrType))
    {
      if(pIdx != NULL)
      {
        *pIdx = idx;
      }
      return (SUCCESS);
    }

    // The public address in the bonding table could be an identity address if we
    // received the identity information during pairing so try to resolve the
    // address if it is random private resolvable.
    if ((addrType == PEER_ADDRTYPE_RANDOM_OR_RANDOM_ID) &&
		(GAP_IS_ADDR_RPR(pDevAddr)))
    {
      // Read the IRK from NV
      uint8_t tempIRK[KEYLEN];
      osal_snv_read(DEV_IRK_NV_ID(idx), KEYLEN, tempIRK);

      // Reverse it (in place) to pass to controller
      MAP_LL_ENC_ReverseBytes(tempIRK, KEYLEN);

      // check if the RPA resolves against the IRK read from the bond
      if(MAP_LL_PRIV_ResolveRPA(pDevAddr, tempIRK) == TRUE)
      {
        // Set output params
        if(pIdx != NULL)
        {
          *pIdx = idx;
        }

        if (pIdentityAddrType != NULL)
        {
          *pIdentityAddrType = bonds[idx].addrType;
        }

        if (pIdentityAddr != NULL)
        {
          MAP_osal_memcpy(pIdentityAddr, bonds[idx].addr, B_ADDR_LEN);
        }

        return (SUCCESS);
      }
    }
  }

  // The address wasn't found in the bonding table if we get here
  return (bleGAPNotFound);
}

//#ifndef GATT_NO_SERVICE_CHANGED
/*********************************************************************
 * @brief   Set/clear the service change indication in a bond record.
 *
 * Public function defined in gapbondmgr.h.
 */
bStatus_t GAPBondMgr_ServiceChangeInd(uint16_t connectionHandle,
                                      uint8_t setParam)
{
  bStatus_t ret = bleNoResources; // return value

  if(connectionHandle == 0xFFFF)
  {
    uint8_t idx;  // loop counter

    // Run through the bond database and update the Service Change indication
    for(idx = 0; idx < gapBond_maxBonds; idx++)
    {
      if(gapBondMgrChangeState(idx, GAP_BONDED_STATE_SERVICE_CHANGED, setParam))
      {
        ret = SUCCESS;
      }
    }

    // If the service change indication is TRUE, tell the connected devices
    if(setParam)
    {
      // Run connected database
      linkDB_PerformFunc(gapBondMgrSendServiceChange);
    }
  }
  else
  {
    // Find connection information
    linkDBItem_t *pLinkItem = MAP_linkDB_Find(connectionHandle);

    if(pLinkItem)
    {
      uint8_t idx; // loop counter

      // Find address in bonding table
      if(GAPBondMgr_FindAddr(pLinkItem->addr,
                             (GAP_Peer_Addr_Types_t)(pLinkItem->addrType &
                                                     MASK_ADDRTYPE_ID),
                             &idx, NULL, NULL) == SUCCESS)
      {
        // Bond found, update it.
        VOID gapBondMgrChangeState(idx, GAP_BONDED_STATE_SERVICE_CHANGED,
                                   setParam);

        ret = SUCCESS;
      }

      // If the service change indication is TRUE, tell the connected device
      if(setParam)
      {
        gapBondMgrSendServiceChange(pLinkItem);
      }
    }
    else
    {
      ret = bleNotConnected;
    }
  }

  return (ret);
}
//#endif // GATT_NO_SERVICE_CHANGED

/*********************************************************************
 * @brief   Register callback functions with the bond manager.
 *
 * Public function defined in gapbondmgr.h.
 */
void GAPBondMgr_Register(gapBondCBs_t *pCB)
{
  pGapBondCB = pCB;

  // Take over the processing of Authentication messages
  VOID MAP_GAP_SetParamValue(GAP_PARAM_AUTH_TASK_ID, gapBondMgr_TaskID);

  // Register with GATT Server App for event messages
  GATTServApp_RegisterForMsg(gapBondMgr_TaskID);

  //Register for SM event messages
  SM_RegisterTask(gapBondMgr_TaskID);

}

/*********************************************************************
 * @brief   Respond to a passcode request.
 *
 * Public function defined in gapbondmgr.h.
 */
bStatus_t GAPBondMgr_PasscodeRsp(uint16_t connectionHandle, uint8_t status,
                                 uint32_t passcode)
{
  bStatus_t ret = SUCCESS;

  if(status == SUCCESS)
  {
    // Truncate the passcode
    passcode = passcode % (GAP_PASSCODE_MAX + 1);

    ret = MAP_GAP_PasscodeUpdate(passcode, connectionHandle);

    if(ret != SUCCESS)
    {
      VOID MAP_GAP_TerminateAuth(connectionHandle,
                                 SMP_PAIRING_FAILED_PASSKEY_ENTRY_FAILED);
    }
  }
  else
  {
    VOID MAP_GAP_TerminateAuth(connectionHandle, status);
  }

  return ret;
}

/*********************************************************************
 * @brief   Asynchronously start pairing
 *
 * Public function defined in gapbondmgr.h.
 */
extern bStatus_t GAPBondMgr_Pair(uint16_t connHandle)
{
  bStatus_t status = FAILURE;

  // Find connection role
  linkDBInfo_t linkInfo;
  linkDB_GetInfo(connHandle, &linkInfo);
  uint8_t role = linkInfo.connRole;

#if ( HOST_CONFIG & CENTRAL_CFG )
  if(role == GAP_PROFILE_CENTRAL)
  {
    // If Central and initiating and not bonded, then initiate pairing
    status = gapBondStateStartSecurity(connHandle, linkInfo.addrType, NULL);
  }
#endif // HOST_CONFIG & CENTRAL_CFG

#if ( HOST_CONFIG & PERIPHERAL_CFG )
  // If Peripheral and initiating, send a slave security request to
  // initiate either pairing or encryption
  if(role == GAP_PROFILE_PERIPHERAL)
  {
    // Ensure that we're not already pairing
    if (GAP_isPairing())
    {
      return bleAlreadyInRequestedMode;
    }
    status = gapBondMgrSlaveSecurityReq(connHandle);
  }
#endif //HOST_CONFIG & PERIPHERAL_CFG

  return(status);
}

/*********************************************************************
 * @brief   This is a bypass mechanism to allow the bond manager to process
 *              GAP messages.
 *
 * Public function defined in gapbondmgr.h.
 */
uint8_t GAPBondMgr_ProcessGAPMsg(gapEventHdr_t *pMsg)
{
  bool safeToDealloc = FALSE;

  switch(pMsg->opcode)
  {
    case GAP_DEVICE_INIT_DONE_EVENT:
    {
      gapDeviceInitDoneEvent_t *pPkt = (gapDeviceInitDoneEvent_t *)pMsg;

      if(pPkt->hdr.status == SUCCESS)
      {
        // Save off the generated keys
        uint8_t *pIRK = MAP_GAP_GetIRK();
        uint8_t *pSRK = MAP_gapGetSRK();
        GAP_Addr_Modes_t addrMode = MAP_gapGetDevAddressMode();

        // Write keys to NV
        VOID osal_snv_write(BLE_NVID_IRK, KEYLEN, pIRK);
        VOID osal_snv_write(BLE_NVID_CSRK, KEYLEN, pSRK);

        // Write address mode to NV
        VOID osal_snv_write(BLE_NVID_ADDR_MODE, sizeof(uint8_t), &addrMode);

        // Write random address to NV if using a random address mode
        if ((addrMode == ADDRMODE_RANDOM)
            || (addrMode == ADDRMODE_RP_WITH_RANDOM_ID)
           )
        {
          uint8_t *pRandAddr = MAP_GAP_GetDevAddress(TRUE);

          VOID osal_snv_write(BLE_NVID_RANDOM_ADDR, B_ADDR_LEN, pRandAddr);
        }
      }
    }
    break;

    case GAP_LINK_ESTABLISHED_EVENT:
    {
      gapEstLinkReqEvent_t *pPkt = (gapEstLinkReqEvent_t *)pMsg;

      if(pPkt->hdr.status == SUCCESS)
      {
        // Notify the Bond Manager to the connection
        VOID gapBondMgr_LinkEst((GAP_Peer_Addr_Types_t)(pPkt->devAddrType &
                                                        MASK_ADDRTYPE_ID),
                                pPkt->devAddr,
                                pPkt->connectionHandle, pPkt->connRole);
      }
    }
    break;

    case GAP_LINK_TERMINATED_EVENT:
    {
      gapTerminateLinkEvent_t *pPkt = (gapTerminateLinkEvent_t *)pMsg;

      // Notify the Bond Manager of the connection termination
      gapBondMgr_LinkTerm(pPkt->connectionHandle);
    }
    break;

    case GAP_PASSKEY_NEEDED_EVENT:
    {
      gapPasskeyNeededEvent_t *pPkt = (gapPasskeyNeededEvent_t *)pMsg;

      // Check that the pairing is still active.
      if(GAP_isPairing())
      {
        if(pGapBondCB && pGapBondCB->passcodeCB)
        {
          // Ask app for a passcode
          pGapBondCB->passcodeCB(pPkt->deviceAddr, pPkt->connectionHandle,
                                 pPkt->uiInputs, pPkt->uiOutputs,
                                 pPkt->numComparison);
        }
        else
        {
          // There is no passcode. End pairing.
          VOID MAP_GAP_TerminateAuth(pPkt->connectionHandle,
                                     SMP_PAIRING_FAILED_PASSKEY_ENTRY_FAILED);
        }
      }

      safeToDealloc = TRUE;
    }
    break;

    case GAP_AUTHENTICATION_COMPLETE_EVENT:
    {
      gapAuthCompleteEvent_t *pPkt = (gapAuthCompleteEvent_t *)pMsg;
      uint8_t saveStatus = SUCCESS;

      // Should we save bonding information (one save at a time)
      if((pPkt->hdr.status == SUCCESS) &&
         (pPkt->authState & SM_AUTH_STATE_BONDING))
      {
        gapBondRec_t bondRec;

        VOID MAP_osal_memset(&bondRec, 0, sizeof(gapBondRec_t));

        linkDBItem_t *pItem;

        pItem = MAP_linkDB_Find(pPkt->connectionHandle);

        if(pItem)
        {
          // Do we have a public address in the data?
          if((pPkt->pIdentityInfo == NULL)
            || (gapBondPreprocessIdentityInformation(pPkt, &bondRec)
            == FAILURE)
            )
          {
            // The Identity Information was not usable, use the over the air
            // address and address type.
            // Note: the pIdentityInfo structure has been nullified in this case.
            bondRec.addrType = (GAP_Peer_Addr_Types_t)(pItem->addrType &
                                                       MASK_ADDRTYPE_ID);
            VOID MAP_osal_memcpy(bondRec.addr, pItem->addr, B_ADDR_LEN);
          }
          else if(pPkt->pIdentityInfo)
          {
            smIdentityInfo_t *pInfo = pPkt->pIdentityInfo;

            // Store identity address type and address in bonding table
            bondRec.addrType = (GAP_Peer_Addr_Types_t)(pInfo->addrType &
                                                       MASK_ADDRTYPE_ID);
            VOID MAP_osal_memcpy(bondRec.addr, pInfo->bd_addr, B_ADDR_LEN);

            // Update LinkDB information.
            // If the device was using an RPA, move it to the private address
            // field in linkdb
            if (pItem->addrType == ADDRTYPE_RANDOM)
            {
              MAP_osal_memcpy(pItem->addrPriv, pItem->addr, B_ADDR_LEN);
            }

            // Move the known Identity Address into the device address field.
            MAP_osal_memcpy(pItem->addr, pInfo->bd_addr, B_ADDR_LEN);

            // Move the Identity Address type into the address type.
            pItem->addrType = pInfo->addrType | SET_ADDRTYPE_ID;
          }
          // The connection stateFlags are read early in GAP_Bond() stage from the SNV using old connection flags
          // but if the current connection (on AUTH complete event) has different flags, the link flags must be changed
          // before using them and before writing the new flags to the SNV
          // Set the authentication flag
          if (pPkt->authState & SM_AUTH_STATE_AUTHENTICATED)
          {
            pItem->stateFlags |= LINK_AUTHENTICATED;
          }
          else
          {
            pItem->stateFlags &= ~LINK_AUTHENTICATED;
          }
          // Set the secure flag
          if (pPkt->authState & SM_AUTH_STATE_SECURECONNECTION)
          {
            pItem->stateFlags |= LINK_SECURE_CONNECTIONS;
          }
          else
          {
            pItem->stateFlags &= ~LINK_SECURE_CONNECTIONS;
          }
        }
        else
        {
          // Check if any pairing requests were queued.
          if(gapBondFindPairReadyNode() != NULL)
          {
            // Set an event to start pairing.
            MAP_osal_set_event(gapBondMgr_TaskID, GAP_BOND_POP_PAIR_QUEUE_EVT);
          }
        }

        // Save off of the authentication state
        bondRec.stateFlags |= (pPkt->authState & SM_AUTH_STATE_AUTHENTICATED) ?
                              GAP_BONDED_STATE_AUTHENTICATED : 0;
        bondRec.stateFlags |= (pPkt->authState & SM_AUTH_STATE_SECURECONNECTION)
                              ? GAP_BONDED_STATE_SECURECONNECTION : 0;

        // Add bond
        saveStatus = gapBondMgrAddBond(&bondRec, pPkt);

        if(saveStatus == SUCCESS)
        {
          if (!gapBond_gatt_no_client) //#if !defined ( GATT_NO_CLIENT )
          {
            // Begin GATT discovery state
            gapBondStateStartNextGATTDisc(pPkt->connectionHandle);
          }
          else //#else //defined GATT_NO_CLIENT
          {
            // End pairing state machine
            gapBondStateEnd(pPkt->connectionHandle);
          } //#endif //!GATT_NO_CLIENT
        }
        else
        {
          // Reaching this points implies that bonding did not occur, so GATT
          // Discovery is unnecessary.  Terminate GATT Discovery
          gapBondStateEnd(pPkt->connectionHandle);
        }
      }

      // Call app state callback
      if ( pGapBondCB && pGapBondCB->pairStateCB )
      {
        // Report to app
        pGapBondCB->pairStateCB( pPkt->connectionHandle,
                                 GAPBOND_PAIRING_STATE_COMPLETE,
                                 pPkt->hdr.status );

        if((pPkt->hdr.status == SUCCESS) &&
           (pPkt->authState & SM_AUTH_STATE_BONDING))

        {
          // Report to app
          pGapBondCB->pairStateCB( pPkt->connectionHandle,
                                   GAPBOND_PAIRING_STATE_BOND_SAVED,
                                   saveStatus );
        }
      }
      // Check if any pairing requests were queued.
        if(gapBondFindPairReadyNode() != NULL)
        {
          // Set an event to start pairing.
          MAP_osal_set_event(gapBondMgr_TaskID, GAP_BOND_POP_PAIR_QUEUE_EVT);
        }
      safeToDealloc = TRUE;
    }

    break;

    case GAP_BOND_COMPLETE_EVENT:
      // This message is received when the bonding is complete.  If hdr.status
      // is SUCCESS then call app state callback. If hdr.status is NOT SUCCESS,
      // the connection will be dropped at the LL because of a MIC failure,
      // so again nothing to do.
    {
      gapBondCompleteEvent_t *pPkt = (gapBondCompleteEvent_t *)pMsg;

#if ( HOST_CONFIG & CENTRAL_CFG )
      if(pPkt->hdr.status == LL_ENC_KEY_REQ_REJECTED)
      {
        // LTK not found on peripheral device (Pin or Key Missing). See which
        // option was configured for unsuccessful bonding.
        linkDBItem_t *pLinkItem = MAP_linkDB_Find(pPkt->connectionHandle);

        if(pLinkItem)
        {
          switch(gapBond_BondFailOption)
          {
            case GAPBOND_FAIL_INITIATE_PAIRING:
              // Initiate pairing
              gapBondStateStartSecurity(pPkt->connectionHandle,
                                        pLinkItem->addrType, NULL);
              break;

            case GAPBOND_FAIL_TERMINATE_LINK:
              // Drop connection
              MAP_GAP_TerminateLinkReq(pPkt->connectionHandle,
                                       HCI_DISCONNECT_AUTH_FAILURE);
              break;

            case GAPBOND_FAIL_TERMINATE_ERASE_BONDS:
              // Set up bond manager to erase all existing bonds after
              // connection terminates
              VOID GAPBondMgr_SetParameter(GAPBOND_ERASE_ALLBONDS, 0, NULL);

              // Drop connection
              MAP_GAP_TerminateLinkReq(pPkt->connectionHandle,
                                       HCI_DISCONNECT_AUTH_FAILURE);
              break;

            case GAPBOND_FAIL_NO_ACTION:

            // fall through
            default:
              // do nothing
              break;
          }
        }
      }
#endif

      if(pGapBondCB && pGapBondCB->pairStateCB)
      {
        pGapBondCB->pairStateCB(pPkt->connectionHandle,
                                GAPBOND_PAIRING_STATE_ENCRYPTED,
                                pMsg->hdr.status);
      }

      safeToDealloc = TRUE;
    }
    break;

    case GAP_SIGNATURE_UPDATED_EVENT:
    {
      uint8_t idx;
      gapSignUpdateEvent_t *pPkt = (gapSignUpdateEvent_t *)pMsg;

      // Find address in bonding table
      if(GAPBondMgr_FindAddr(pPkt->devAddr,
                             (GAP_Peer_Addr_Types_t)(pPkt->addrType &
                                                     MASK_ADDRTYPE_ID),
                             &idx, NULL, NULL) == SUCCESS)
      {
        // Save the sign counter
        VOID osal_snv_write(DEV_SIGN_COUNTER_NV_ID(idx), sizeof(uint32_t),
                            &(pPkt->signCounter));
      }

      safeToDealloc = TRUE;
    }
    break;

#if ( HOST_CONFIG & PERIPHERAL_CFG )

    case GAP_PAIRING_REQ_EVENT:
    {
      gapPairingReqEvent_t *pPkt = (gapPairingReqEvent_t *)pMsg;

      // Call app state callback
      if(pGapBondCB && pGapBondCB->pairStateCB)
      {
        pGapBondCB->pairStateCB(pPkt->connectionHandle,
                                GAPBOND_PAIRING_STATE_STARTED, SUCCESS);
      }

#ifdef HCI_TL_FULL
      /* for test mode only */
      if(gapBond_AutoFail != FALSE)
      {
        // Auto Fail TEST MODE (DON'T USE THIS) - Sends pre-setup reason
        VOID MAP_GAP_TerminateAuth(pPkt->connectionHandle, gapBond_AutoFailReason);
      }
      else if(gapBond_PairingMode == GAPBOND_PAIRING_MODE_NO_PAIRING)
#else // !HCI_TL_FULL
      if(gapBond_PairingMode == GAPBOND_PAIRING_MODE_NO_PAIRING)
#endif // HCI_TL_FULL
      {
        // No Pairing - Send error
        VOID MAP_GAP_TerminateAuth(pPkt->connectionHandle,
                                  SMP_PAIRING_FAILED_NOT_SUPPORTED);
      }
      else
      {
        linkDBItem_t *pLinkItem = MAP_linkDB_Find(pPkt->connectionHandle);

        if(pLinkItem == NULL)
        {
          // Can't find the connection, ignore the message
          break;
        }

        // Requesting bonding?
        if(pPkt->pairReq.authReq & SM_AUTH_STATE_BONDING)
        {
          // Check if we already have the address in NV
          if(GAPBondMgr_FindAddr(pLinkItem->addr,
                                 (GAP_Peer_Addr_Types_t)(pLinkItem->addrType &
                                                         MASK_ADDRTYPE_ID),
                                 NULL, NULL, NULL) == SUCCESS)
          {
#ifdef GAPBONDMGR_TESTMODE
            // For PTS - GAP/SEC/AUT/BV-22-C [Lost Bond � Responder Role]
            gapSendBondLostEvent(pPkt->connectionHandle,pLinkItem->addr);
#endif
          }
          else if((pLinkItem->addrType != ADDRTYPE_PUBLIC) &&
                 (pLinkItem->addrType != ADDRTYPE_RANDOM) &&
                 (pPkt->pairReq.keyDist.mIdKey == FALSE))
          {
              // Can't bond to a non-public address if the peer won't send
			  // the identity address and we don't already have it
              VOID MAP_GAP_TerminateAuth(pPkt->connectionHandle,
                                         SMP_PAIRING_FAILED_AUTH_REQ);

              // Ignore the message
              break;
          }
        }

        // Section 2.3.5.1: "If the key generation method does not result in a
        // key that provides sufficient security properties then the device
        // shall send the Pairing Failed command with the error code
        // "Authentication Requirements." This supports Secure Connections Only
        // Mode, where if the remote device does not support Secure Connections
        // and the local device requires Secure Connections, then pairing can be
        // aborted here with a Pairing Failed command with error code
        // "Authentication requirements".
        if(gapBond_secureConnection == GAPBOND_SECURE_CONNECTION_ONLY)
        {
          if(!(pPkt->pairReq.authReq & SM_AUTH_STATE_SECURECONNECTION))
          {
            // The remote device does not meet the security requirements.
            VOID MAP_GAP_TerminateAuth(pPkt->connectionHandle,
                                   SMP_PAIRING_FAILED_AUTH_REQ);

            break;
          }
        }

        // Send a response to the Pairing Request.
        gapBondStateStartSecurity(pPkt->connectionHandle, pLinkItem->addrType,
                                  &(pPkt->pairReq));
      }

      safeToDealloc = TRUE;
    }
    break;
#endif

#if ( HOST_CONFIG & CENTRAL_CFG )
    case GAP_SLAVE_REQUESTED_SECURITY_EVENT:
    {
      uint16_t connHandle =
                         ((gapSlaveSecurityReqEvent_t *)pMsg)->connectionHandle;
      uint8_t authReq = ((gapSlaveSecurityReqEvent_t *)pMsg)->authReq;

      gapBondMgr_SlaveReqSecurity(connHandle, authReq);

      safeToDealloc = TRUE;
    }
    break;
#endif

    case GAP_AUTHENTICATION_FAILURE_EVT:
    {
      // If there are any queued devices waiting, try pairing with them
      if(gapBondFindPairReadyNode() != NULL)
      {
        MAP_osal_set_event(gapBondMgr_TaskID, GAP_BOND_POP_PAIR_QUEUE_EVT);
      }

      safeToDealloc = TRUE;
    }
    break;

    // Pass any unprocessed events to the app
    default:
      break;
  }

  return safeToDealloc;
}

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */

//#ifndef GATT_NO_CLIENT
/*********************************************************************
 * @brief   Send Read By Type Request to get value of a GATT Characteristic
 *          on the remote device.
 *
 * Public function defined in gapbondmgr.h.
 */
static bStatus_t gapBondMgr_ReadGattChar(uint16_t connectionHandle, uint16_t uuid)
{
  uint8_t status;
  attReadByTypeReq_t req;

  req.startHandle = 0x0001;
  req.endHandle   = 0xFFFF;

  req.type.len = ATT_BT_UUID_SIZE;
  req.type.uuid[0] = LO_UINT16(uuid);
  req.type.uuid[1] = HI_UINT16(uuid);


  // Send Read by Type Request
  status = GATT_ReadUsingCharUUID(connectionHandle, &req, gapBondMgr_TaskID);

  // If GATT Transaction is already in progress
  if(status == blePending)
  {
    // Request an event to interleave this in between whatever the application
    // is doing.
    // NOTE: This only works because the Bond Manager runs in a higher priority
    // task than the Application.
    status = GATT_RequestNextTransaction(connectionHandle, gapBondMgr_TaskID);
  }

  return status;
}
//#endif //!GATT_NO_CLIENT

/*********************************************************************
 * @brief   Determine if peer device supports central address resolution
 *
 * Public function defined in gapbondmgr.h.
 */
static uint8_t gapBondMgr_SupportsCentAddrRes(uint8_t *pPeerAddr,
                                              GAP_Peer_Addr_Types_t addrType)
{
  uint8_t idx;

  // Check for valid input address
  if(pPeerAddr == NULL)
  {
    return FALSE;
  }

  // Try to find this device in the resolving list to see if an RPA will be
  // used
  if(MAP_LL_PRIV_FindPeerInRL(resolvingList, addrType, pPeerAddr) !=
     INVALID_RESOLVE_LIST_INDEX)
  {
    // Device was found with a valid IRK so RPA will be used. Now check to see
    // if the device supports central address resolution (i.e. it has the CAR
    // char).
    // First, find the bonding table index:
    if(GAPBondMgr_FindAddr(pPeerAddr, addrType,
                           &idx, NULL, NULL) == SUCCESS)
    {
      //Check if the peer device has RPAO disabled, which means it is
      //considered as Device Privacy Mode. If so, it doesn't matter if the peer
      //device has CAR char enabled in order to direct advertise as needed by the spec.
      if((gapBondMgrGetStateFlags(idx) & GAP_BONDED_STATE_RPA_ONLY) == 0)
      {
        return TRUE;
      }

      // Check that the device has CAR enabled
      if(gapBondMgrGetStateFlags(idx) & GAP_BONDED_STATE_CAR)
      {
        // Device has CAR enabled so OK to use RPA
        return TRUE;
      }
      else
      {
        // Device doesn't have CAR enabled so can't use RPA
        return FALSE;
      }
    }
    else
    {
      // Device wasn't found in bonding table. This means that the device was
      // manually added to the resolving list by the application and it is the
      // application's responsibility to check the CAR.
      return TRUE;
    }
  }
  else
  {
    // An RPA won't be used so no need to check for CAR char
    return TRUE;
  }
}

/*********************************************************************
 * @brief   Update the Characteristic Configuration in a bond record.
 *
 * Public function defined in gapbondmgr.h.
 */
static bStatus_t gapBondMgr_UpdateCharCfg(uint16_t connectionHandle,
                                          uint16_t attrHandle, uint16_t value)
{
  bStatus_t ret = bleNoResources; // return value
  uint8_t idx;

  if(connectionHandle == LINKDB_CONNHANDLE_INVALID)
  {
    // Run through the bond database and update the Characteristic Configuration
    for(idx = 0; idx < gapBond_maxBonds; idx++)
    {
      if(gapBondMgrUpdateCharCfg(idx, attrHandle, value))
      {
        ret = SUCCESS;
      }
    }
  }
  else
  {
    // Find connection information
    linkDBItem_t *pLinkItem = MAP_linkDB_Find(connectionHandle);

    if(pLinkItem)
    {
      // Try to find address in bonding table
      if(GAPBondMgr_FindAddr(pLinkItem->addr,
                             (GAP_Peer_Addr_Types_t)(pLinkItem->addrType &
                                                     MASK_ADDRTYPE_ID),
                             &idx, NULL, NULL) == SUCCESS)
      {
        // Bond found, update it.
        VOID gapBondMgrUpdateCharCfg(idx, attrHandle, value);
        ret = SUCCESS;
      }
    }
    else
    {
      ret = bleNotConnected;
    }
  }

  return (ret);
}

/*********************************************************************
 * @brief   Notify the Bond Manager that a connection has been made.
 *
 * Public function defined in gapbondmgr.h.
 */
static void gapBondMgr_LinkEst(GAP_Peer_Addr_Types_t addrType, uint8_t *pDevAddr,
                               uint16_t connHandle, uint8_t role)
{
  // NV Index
  uint8_t idx;
  uint8_t pair = TRUE;

  // If a pre-existing bond was found
  if(GAPBondMgr_FindAddr(pDevAddr, addrType, &idx, NULL, NULL) == SUCCESS)
  {
    uint8_t stateFlags = gapBondMgrGetStateFlags(idx);

    if (role == GAP_PROFILE_CENTRAL)
    {
      // If connection role is central and pre-existing bond was found , don't pair.
      pair = FALSE;
    }

    // Space to read a char cfg record from NV
    gapBondCharCfg_t *charCfg = (gapBondCharCfg_t *)MAP_osal_mem_alloc( sizeof (gapBondCharCfg_t) * gapBond_maxCharCfg );
    if (charCfg == NULL)
    {
      HAL_ASSERT( HAL_ASSERT_CAUSE_OUT_OF_MEMORY );
      return;
    }
    
    smSigningInfo_t signingInfo;

    // On peripheral, load the key information for the bonding
    // On central and initiaiting security, load key to initiate encyption
    gapBondMgrBondReq(connHandle, idx, stateFlags, role,
                      ((gapBond_PairingMode == GAPBOND_PAIRING_MODE_INITIATE)
                      ? TRUE : FALSE));

    // Load the Signing Key
    VOID MAP_osal_memset(&signingInfo, 0, sizeof(smSigningInfo_t));

    if(osal_snv_read(DEV_CSRK_NV_ID(idx), KEYLEN, signingInfo.srk) == SUCCESS)
    {
      if(osal_isbufset(signingInfo.srk, 0xFF, KEYLEN) == FALSE)
      {
        // Load the signing information for this connection
        VOID osal_snv_read(DEV_SIGN_COUNTER_NV_ID(idx), sizeof(uint32_t),
                           &(signingInfo.signCounter));
        VOID MAP_GAP_Signable(connHandle,
                          ((stateFlags & GAP_BONDED_STATE_AUTHENTICATED)
                          ? TRUE : FALSE), &signingInfo);
      }
    }

    // Load the characteristic configuration
    if(osal_snv_read(GATT_CFG_NV_ID(idx), sizeof(gapBondCharCfg_t) * gapBond_maxCharCfg, charCfg) == SUCCESS)
    {
      uint8_t i;

      gapBondMgrInvertCharCfgItem(charCfg);

      for(i = 0; i < gapBond_maxCharCfg; i++)
      {
        gapBondCharCfg_t *pItem = &(charCfg[i]);

        // Apply the characteristic configuration for this connection
        if(pItem->attrHandle != GATT_INVALID_HANDLE)
        {
          VOID GATTServApp_UpdateCharCfg(connHandle, pItem->attrHandle,
                                         (uint16_t)(pItem->value));
        }
      }
    }

    if (!gapBond_gatt_no_service_changed) //#ifndef GATT_NO_SERVICE_CHANGED
    {
      // Has there been a service change?
      if(stateFlags & GAP_BONDED_STATE_SERVICE_CHANGED)
      {
        VOID GATTServApp_SendServiceChangedInd(connHandle, gapBondMgr_TaskID);
      }
    } // #endif // GATT_NO_SERVICE_CHANGED

    // Update LRU Bond list
    gapBondMgrUpdateLruBondList(idx);
    MAP_osal_mem_free( charCfg );
  }

  // If connection role is peripheral and pairing mode is initiate then pair always.
  // If connection role is central and pairing mode is initiate then pair only if
  // there is no pre-existing bond.
  if((gapBond_PairingMode == GAPBOND_PAIRING_MODE_INITIATE) && (pair == TRUE))
  {
    VOID GAPBondMgr_Pair(connHandle);
  }
}

/*********************************************************************
 * @brief   Notify the Bond Manager that a connection has been terminated.
 *
 * Public function defined in gapbondmgr.h.
 */
static void gapBondMgr_LinkTerm(uint16_t connHandle)
{
  // Removing state information.
  gapBondStateEnd(connHandle);

  if(MAP_GAP_NumActiveConnections() == 0)
  {
    // See if we're asked to erase all bonding records
    if(eraseAllBonds == TRUE)
    {
      VOID gapBondMgrEraseAllBondings();
      eraseAllBonds = FALSE;

      // Reset bonds to delete table
      MAP_osal_memset(bondsToDelete, FALSE, sizeof (uint8_t) * gapBond_maxBonds);
    }
    // See if we are to erase the local bond info
    else if(eraseLocalInfo == TRUE)
    {
      VOID gapBondMgrEraseLocalInfo();
      eraseLocalInfo = FALSE;
    }
    else
    {
      // See if we're asked to erase any single bonding records
      uint8_t idx;

      for(idx = 0; idx < gapBond_maxBonds; idx++)
      {
        if(bondsToDelete[idx] == TRUE)
        {
          VOID gapBondMgrEraseBonding(idx);
          bondsToDelete[idx] = FALSE;
        }
      }
    }

    // See if NV needs a compaction
    VOID osal_snv_compact(NV_COMPACT_THRESHOLD);

    // Make sure Bond RAM Shadow is up-to-date
    gapBondMgrReadBonds();
  }
}

#if ( HOST_CONFIG & CENTRAL_CFG )
/*********************************************************************
 * @brief   Process a slave security request as a master
 *
 * Public function defined in gapbondmgr.h.
 */
static void gapBondMgr_SlaveReqSecurity(uint16_t connHandle, uint8_t authReq)
{
  uint8_t idx;
  linkDBItem_t *pLink = MAP_linkDB_Find(connHandle);

  // If link found and not already initiating security.
  if((pLink != NULL) && (GAP_isPairing() == FALSE))
  {
    // Check to see if we are already bonded (i.e. have the LTK)
    if(GAPBondMgr_FindAddr(pLink->addr,
                           (GAP_Peer_Addr_Types_t)(pLink->addrType &
                                                   MASK_ADDRTYPE_ID),
                           &idx, NULL, NULL) == SUCCESS)
    {
      // Get state flags of bonding
      uint8_t prevAuthReq = gapBondMgrGetStateFlags(idx);
      bool repair = FALSE;

      // Is LTK >= Requested Security Level where the relevant security levels
      // are defined as such:
      // 2. Unauthenticated pairing
      // 3. Authenticated pairing
      // 4. Authenticated LE Secure Connections pairing
      switch (authReq & (SM_AUTH_STATE_AUTHENTICATED |
                         SM_AUTH_STATE_SECURECONNECTION))
      {
        // Slave is asking for authenticated legacy pairing
        case SM_AUTH_STATE_AUTHENTICATED:
        {
          // Repair if not currently authenticated
          if (!(prevAuthReq & GAP_BONDED_STATE_AUTHENTICATED))
          {
            repair = TRUE;
          }
        }
        break;

        // Slave is asking for unauthenticated secure connections
        case SM_AUTH_STATE_SECURECONNECTION:
        {
          // Repair if not currently authenticated or secure connections
          if (!(prevAuthReq & (GAP_BONDED_STATE_AUTHENTICATED |
                               GAP_BONDED_STATE_SECURECONNECTION)))
          {
            repair = TRUE;
          }
        }
        break;

        // Slave is asking for authenticated secure connections
        case (SM_AUTH_STATE_AUTHENTICATED | SM_AUTH_STATE_SECURECONNECTION):
        {
          // Repair unless currently authenticated with secure connections
          if (!((prevAuthReq & GAP_BONDED_STATE_AUTHENTICATED ) &&
                (prevAuthReq & GAP_BONDED_STATE_SECURECONNECTION )))
          {
            repair = TRUE;
          }
        }
        break;

        default:
        break;
      }

      // Repair if it was found to be needed above
      if (repair == TRUE)
      {
        // Start Pairing
        gapBondStateStartSecurity(connHandle, pLink->addrType, NULL);
      }
      else
      {
        // Otherwise, start encryption if not already encrypted or refresh the
        // encryption keys if currently encrypted. Both cases are handled by
        // this API
        gapBondMgrBondReq(connHandle, idx, prevAuthReq, pLink->connRole, TRUE);
      }
    }
    // Otherwise we don't have the LTK
    else
    {
      // Reject if no pairing is desired
      if (gapBond_PairingMode == GAPBOND_PAIRING_MODE_NO_PAIRING)
      {
        VOID MAP_GAP_TerminateAuth(connHandle,
                                   SMP_PAIRING_FAILED_NOT_SUPPORTED);
      }
      else
      {
        // Start Pairing
        gapBondStateStartSecurity(connHandle, pLink->addrType, NULL);
      }
    }
  }
}
#endif

//#if !defined GATT_NO_SERVICE_CHANGED || !defined GATT_NO_CLIENT
/*********************************************************************
 * @fn      gapBondMgrChangeState
 *
 * @brief   Change a state flag in the stateFlags field of the bond record.
 *
 * @param   idx - Bond NV index
 * @param   state - state flage to set or clear
 * @param   set - TRUE to set the flag, FALSE to clear the flag
 *
 * @return  TRUE if NV Record exists, FALSE if NV Record is empty
 */
static uint8_t gapBondMgrChangeState(uint8_t idx, uint16_t state, uint8_t set)
{
  gapBondRec_t bondRec;   // Space to read a Bond record from NV

  // Look for public address that is used (not all 0xFF's)
  if((osal_snv_read(MAIN_RECORD_NV_ID(idx), sizeof(gapBondRec_t),
     &bondRec) == SUCCESS) &&
     (osal_isbufset(bondRec.addr, 0xFF, B_ADDR_LEN) == FALSE))
  {
    // Update the state of the bonded device.
    uint8_t stateFlags = bondRec.stateFlags;

    if(set)
    {
      stateFlags |= state;
    }
    else
    {
      stateFlags &= ~(state);
    }

    if(stateFlags != bondRec.stateFlags)
    {
      bondRec.stateFlags = stateFlags;
      VOID osal_snv_write(MAIN_RECORD_NV_ID(idx), sizeof(gapBondRec_t),
                          &bondRec);
    }

    return (TRUE);
  }

  return (FALSE);
}
//#endif // !GATT_NO_SERVICE_CHANGED || !GATT_NO_CLIENT

/*********************************************************************
 * @fn      gapBondMgrUpdateCharCfg
 *
 * @brief   Update the Characteristic Configuration of the bond record.
 *
 * @param   idx - Bond NV index
 * @param   attrHandle - attribute handle (0 means all handles)
 * @param   value - characteristic configuration value
 *
 * @return  TRUE if NV Record exists, FALSE if NV Record is empty
 */
static uint8_t gapBondMgrUpdateCharCfg(uint8_t idx, uint16_t attrHandle,
                                       uint16_t value)
{
  gapBondRec_t bondRec;   // Space to read a Bond record from NV

  // Look for public address that is used (not all 0xFF's)
  if((osal_snv_read(MAIN_RECORD_NV_ID(idx), sizeof(gapBondRec_t),
     &bondRec) == SUCCESS) &&
     (osal_isbufset(bondRec.addr, 0xFF, B_ADDR_LEN) == FALSE))
  {
    // Space to read a char cfg record from NV
    gapBondCharCfg_t *charCfg = (gapBondCharCfg_t *)MAP_osal_mem_alloc( sizeof (gapBondCharCfg_t) * gapBond_maxCharCfg );
    if (charCfg == NULL)
    {
      HAL_ASSERT( HAL_ASSERT_CAUSE_OUT_OF_MEMORY );
      return (FALSE);
    }

    if(osal_snv_read(GATT_CFG_NV_ID(idx), sizeof(gapBondCharCfg_t) * gapBond_maxCharCfg, charCfg) == SUCCESS)
    {
      uint8_t update = FALSE;

      gapBondMgrInvertCharCfgItem(charCfg);

      if(attrHandle == GATT_INVALID_HANDLE)
      {
        if(MAP_osal_isbufset((uint8_t *)charCfg, 0x00, sizeof(gapBondCharCfg_t) * gapBond_maxCharCfg) == FALSE)
        {
          // Clear all characteristic configuration for this device
          VOID MAP_osal_memset((void *)charCfg, 0x00, sizeof(gapBondCharCfg_t) * gapBond_maxCharCfg);
          update = TRUE;
        }
      }
      else
      {
        gapBondCharCfg_t *pItem = gapBondMgrFindCharCfgItem(attrHandle, charCfg);

        if(pItem == NULL)
        {
          // Must be a new item; ignore if the value is no operation (default)
          if((value == GATT_CFG_NO_OPERATION) ||
             ((pItem = gapBondMgrFindCharCfgItem(GATT_INVALID_HANDLE, charCfg))
             == NULL))
          {
            MAP_osal_mem_free( charCfg );
            return (FALSE);   // No empty entry found
          }

          pItem->attrHandle = attrHandle;
        }

        if(pItem->value != value)
        {
          // Update characteristic configuration
          pItem->value = (uint8_t)value;

          if(value == GATT_CFG_NO_OPERATION)
          {
            // Erase the item
            pItem->attrHandle = GATT_INVALID_HANDLE;
          }

          update = TRUE;
        }
      }

      // Update the characteristic configuration of the bonded device.
      if(update)
      {
        gapBondMgrInvertCharCfgItem(charCfg);

        VOID osal_snv_write(GATT_CFG_NV_ID(idx), sizeof(gapBondCharCfg_t) * gapBond_maxCharCfg, charCfg);
      }
    }
    MAP_osal_mem_free( charCfg );

    return (TRUE);
  }

  return (FALSE);
}

/*********************************************************************
 * @fn      gapBondMgrFindCharCfgItem
 *
 * @brief   Find the Characteristic Configuration for a given attribute.
 *          Uses the attribute handle to search the characteristic
 *          configuration table of a bonded device.
 *
 * @param   attrHandle - attribute handle.
 * @param   charCfgTbl - characteristic configuration table.
 *
 * @return  pointer to the found item. NULL, otherwise.
 */
static gapBondCharCfg_t *gapBondMgrFindCharCfgItem(uint16_t attrHandle,
    gapBondCharCfg_t *charCfgTbl)
{
  uint8_t i;

  for(i = 0; i < gapBond_maxCharCfg; i++)
  {
    if(charCfgTbl[i].attrHandle == attrHandle)
    {
      return (&(charCfgTbl[i]));
    }
  }

  return ((gapBondCharCfg_t *)NULL);
}

/*********************************************************************
 * @fn      gapBondMgrInvertCharCfgItem
 *
 * @brief   Invert the Characteristic Configuration for a given client.
 *
 * @param   charCfgTbl - characteristic configuration table.
 *
 * @return  none.
 */
static void gapBondMgrInvertCharCfgItem(gapBondCharCfg_t *charCfgTbl)
{
  uint8_t i;

  for(i = 0; i < gapBond_maxCharCfg; i++)
  {
    charCfgTbl[i].attrHandle = ~(charCfgTbl[i].attrHandle);
    charCfgTbl[i].value = ~(charCfgTbl[i].value);
  }
}

/*********************************************************************
 * @fn      gapBondMgrSaveBond
 *
 * @brief   Save a bond record to NV
 *
 * @param   pBondRec - basic bond record
 * @param   pLocalLTK - LTK used by this device during pairing
 * @param   pDevLTK - LTK used by the connected device during pairing
 * @param   pIRK - IRK used by the connected device during pairing
 * @param   pSRK - SRK used by the connected device during pairing
 * @param   signCounter - Sign counter used by the connected device during
 *                        pairing
 * @param   syncRL - Flag indicating whether Resolving List must be resynched
 *
 * @return  SUCCESS if bond was added
 *          NV_OPER_FAILED for failure
 */
static uint8_t gapBondMgrSaveBond(uint8_t bondIdx,
                                  gapBondRec_t* pBondRec,
                                  gapBondLTK_t* pLocalLtk,
                                  gapBondLTK_t* pDevLtk,
                                  uint8_t* pIRK,
                                  uint8_t* pSRK,
                                  uint32_t signCount,
                                  int8_t syncRL)
{
  uint8_t snvErrorCode = SUCCESS;
  gapBondCharCfg_t *charCfg = (gapBondCharCfg_t *)MAP_osal_mem_alloc(sizeof (gapBondCharCfg_t) * gapBond_maxCharCfg);
  if (charCfg == NULL)
  {
    HAL_ASSERT( HAL_ASSERT_CAUSE_OUT_OF_MEMORY );
    return (NV_OPER_FAILED);
  }

  // Save the main information
  snvErrorCode |= osal_snv_write(MAIN_RECORD_NV_ID(bondIdx), sizeof(gapBondRec_t), pBondRec);

  // Once main information is saved, it qualifies to update the LRU table.
  gapBondMgrUpdateLruBondList(bondIdx);

  // Write out FF's over the characteristic configuration entry, to overwrite
  // any previous bond data that may have been stored
  VOID MAP_osal_memset(charCfg, 0xFF, sizeof(gapBondCharCfg_t) * gapBond_maxCharCfg);

  snvErrorCode |= osal_snv_write(GATT_CFG_NV_ID(bondIdx), sizeof(gapBondCharCfg_t) * gapBond_maxCharCfg, charCfg);

  // Update Bond RAM Shadow just with the newly added bond entry
  VOID MAP_osal_memcpy(&(bonds[bondIdx]), pBondRec, sizeof(gapBondRec_t));

  if(pIRK)
  {
    // Add device to resolving list
    if ((HCI_LE_AddDeviceToResolvingListCmd((pBondRec->addrType & MASK_ADDRTYPE_ID),
                                             pBondRec->addr, pIRK, NULL) != SUCCESS) && syncRL)
    {
      gapBond_syncRL = TRUE;
    }

    // If available, save the connected device's IRK
    snvErrorCode |= osal_snv_write(DEV_IRK_NV_ID(bondIdx), KEYLEN, pIRK);
  }

  // If available, save the LTK information
  if(pLocalLtk)
  {
    snvErrorCode |= osal_snv_write(LOCAL_LTK_NV_ID(bondIdx), sizeof(gapBondLTK_t), pLocalLtk);
  }

  // If available, save the connected device's LTK information
  if(pDevLtk)
  {
	snvErrorCode |= osal_snv_write(DEV_LTK_NV_ID(bondIdx), sizeof(gapBondLTK_t), pDevLtk);
  }

  // If available, save the connected device's Signature information
  if(pSRK)
  {
    snvErrorCode |= osal_snv_write(DEV_CSRK_NV_ID(bondIdx), KEYLEN, pSRK);
    snvErrorCode |= osal_snv_write(DEV_SIGN_COUNTER_NV_ID(bondIdx), sizeof(uint32_t), &signCount);
  }

  if(autoSyncWhiteList)
  {
    gapBondMgr_SyncWhiteList();
  }

  MAP_osal_mem_free(charCfg);
  return (snvErrorCode);
}

/*********************************************************************
 * @fn      gapBondMgrAddBond
 *
 * @brief   Save a bond from a GAP Auth Complete Event
 *
 * @param   pBondRec - basic bond record
 * @param   pLocalLTK - LTK used by this device during pairing
 * @param   pDevLTK - LTK used by the connected device during pairing
 * @param   pIRK - IRK used by the connected device during pairing
 * @param   pSRK - SRK used by the connected device during pairing
 * @param   signCounter - Sign counter used by the connected device during
 *                        pairing
 *
 * @return SUCCESS if bond was added
 *         bleNoResources if there are no empty slots
 */
static uint8_t gapBondMgrAddBond(gapBondRec_t *pBondRec,
                                 gapAuthCompleteEvent_t *pPkt)
{
  uint8_t bondIdx;
  uint8_t snvErrorCode = SUCCESS;

  // First see if we already have an existing bond for this device
  if(GAPBondMgr_FindAddr(pBondRec->addr, pBondRec->addrType,
                         &bondIdx, NULL, NULL) != SUCCESS)
  {
    bondIdx = gapBondMgrFindEmpty();
  }
  else if(pPkt->pIdentityInfo)
  {
    uint8_t oldIrk[KEYLEN];

    // Verify that that a previous bond had an IRK before attempting to
    // remove it from the Controller's resolving list.
    if((osal_snv_read(DEV_IRK_NV_ID(bondIdx), KEYLEN, oldIrk) == SUCCESS) &&
        (MAP_osal_isbufset(oldIrk, 0xFF, KEYLEN) == FALSE))
    {
      // If a current record is simply being updated then erase previous
      // entry in resolving list for this peer. Will subsequently update
      // RL with new ID info for peer
      MAP_HCI_LE_RemoveDeviceFromResolvingListCmd((pBondRec->addrType &
                                                    MASK_ADDRTYPE_ID),
                                                   pBondRec->addr);
    }
  }

  // If an empty slot was found
  if(bondIdx < gapBond_maxBonds)
  {
    snvErrorCode = gapBondMgrSaveBond(bondIdx, pBondRec,
                                      (gapBondLTK_t*) pPkt->pSecurityInfo,
                                      (gapBondLTK_t*) pPkt->pDevSecInfo,
                                      pPkt->pIdentityInfo ? pPkt->pIdentityInfo->irk : NULL,
                                      pPkt->pSigningInfo ? pPkt->pSigningInfo->srk : NULL,
                                      pPkt->pSigningInfo ? pPkt->pSigningInfo->signCounter : 0,
                                      FALSE);

    // Update NV to have same CCC values as GATT database
    gapBondMgr_SyncCharCfg(pPkt->connectionHandle);
  }
  else
  {
    return(bleNoResources);
  }

  // Check for there was an error when writing to the NV area
  if (snvErrorCode != SUCCESS)
  {
    gapBondMgrEraseBonding(bondIdx);
    gapBondMgrReadBonds();
  }

  return (snvErrorCode);
}

/*********************************************************************
 * @fn      gapBondMgrImportBond
 *
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
 * @return SUCCESS if bond was imported
 * @return bleNoResources if there are no empty slots
 */
uint8_t gapBondMgrImportBond(gapBondRec_t* pBondRec,
                             gapBondLTK_t* pLocalLtk,
                             gapBondLTK_t* pDevLtk,
                             uint8_t* pIRK,
                             uint8_t* pSRK,
                             uint32_t signCount,
                             gapBondCharCfg_t* charCfg)
{
  uint8_t bondIdx;
  uint8_t snvErrorCode = SUCCESS;

  // First see if we already have an existing bond for this device
  if(GAPBondMgr_FindAddr(pBondRec->addr, pBondRec->addrType,
                         &bondIdx, NULL, NULL) != SUCCESS)
  {
    bondIdx = gapBondMgrFindEmpty();
  }
  else if(pIRK)
  {
    uint8_t oldIrk[KEYLEN];

    // Verify that that a previous bond had an IRK before attempting to
    // remove it from the Controller's resolving list.
    if((osal_snv_read(DEV_IRK_NV_ID(bondIdx), KEYLEN, oldIrk) == SUCCESS) &&
       (MAP_osal_isbufset(oldIrk, 0xFF, KEYLEN) == FALSE))
    {
      // If a current record is simply being updated then erase previous
      // entry in resolving list for this peer. Will subsequently update
      // RL with new ID info for peer
      if (MAP_HCI_LE_RemoveDeviceFromResolvingListCmd((pBondRec->addrType &MASK_ADDRTYPE_ID),
          pBondRec->addr) != SUCCESS)
      {
        gapBond_syncRL = TRUE;
      }
    }
  }

  // If an empty slot was found
  if(bondIdx < gapBond_maxBonds)
  {
    snvErrorCode = gapBondMgrSaveBond(bondIdx, pBondRec, pLocalLtk, pDevLtk, pIRK, pSRK,
                                      signCount, TRUE);

    // If available, save the connected device's GATT configurations
    if (charCfg)
    {
      snvErrorCode |=  osal_snv_write(GATT_CFG_NV_ID(bondIdx),
                          sizeof(gapBondCharCfg_t) * gapBond_maxCharCfg,
                          charCfg);
    }
  }
  else
  {
   return(bleNoResources);
  }

  // Check for there was an error when writing to the NV area
  if (snvErrorCode != SUCCESS)
  {
    gapBondMgrEraseBonding(bondIdx);
    gapBondMgrReadBonds();
  }
  return (snvErrorCode);
}

/*********************************************************************
 * @fn      gapBondMgrReadBondRec
 *
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
uint8_t gapBondMgrReadBondRec(GAP_Peer_Addr_Types_t addrType,
                              uint8_t *pDevAddr,
                              gapBondRec_t* pBondRec,
                              gapBondLTK_t* pLocalLtk,
                              gapBondLTK_t* pDevLtk,
                              uint8_t* pIRK,
                              uint8_t* pSRK,
                              uint32_t signCount,
                              gapBondCharCfg_t* charCfg)
{
  // NV Index
  uint8_t idx;

  // If a pre-existing bond was found
  if(GAPBondMgr_FindAddr(pDevAddr, addrType, &idx, NULL, NULL) == SUCCESS)
  {
    // See if the entry exists in NV
    if(osal_snv_read(MAIN_RECORD_NV_ID(idx), sizeof(gapBondRec_t), pBondRec) != SUCCESS)
    {
      // Can't read the entry, assume that it doesn't exist
      return (bleGAPNotFound);
    }

    // Load the local LTK information
    VOID osal_snv_read(LOCAL_LTK_NV_ID(idx), sizeof(gapBondLTK_t), pLocalLtk);

    // Load the connected device's LTK information
    VOID osal_snv_read(DEV_LTK_NV_ID(idx), sizeof(gapBondLTK_t), pDevLtk);

    // Load in NV IRK Record
    VOID osal_snv_read(DEV_IRK_NV_ID(idx), KEYLEN, pIRK);

    // Load the Signing Key
    if(osal_snv_read(DEV_CSRK_NV_ID(idx), KEYLEN, pSRK) == SUCCESS)
    {
      if(osal_isbufset(pSRK, 0xFF, KEYLEN) == FALSE)
      {
        // Load the signing information for this connection
        VOID osal_snv_read(DEV_SIGN_COUNTER_NV_ID(idx), sizeof(uint32_t),
	                           &(signCount));
      }
    }

    // Load the characteristic configuration
    VOID osal_snv_read(GATT_CFG_NV_ID(idx), sizeof(gapBondCharCfg_t) * gapBond_maxCharCfg, charCfg);
  }
  else
  {
    // The address wasn't found in the bonding table if we get here
    return (bleGAPNotFound);
  }

  return (SUCCESS);
}

/*********************************************************************
 * @fn      gapBondMgrGetStateFlags
 *
 * @brief   Gets the state flags field of a bond record in NV
 *
 * @param   idx
 *
 * @return  stateFlags field
 */
static uint8_t gapBondMgrGetStateFlags(uint8_t idx)
{
  gapBondRec_t bondRec;

  if(osal_snv_read(MAIN_RECORD_NV_ID(idx), sizeof(gapBondRec_t),
                   &bondRec) == SUCCESS)
  {
    return (bondRec.stateFlags);
  }

  return (0);
}

/*********************************************************************
 * @fn      gapBondMgr_gapIdle
 *
 * @brief   Callback for GAP to notify bond manager whenever it has
 *          finished all adv/scan/init
 */
void gapBondMgr_gapIdle(void)
{
  // Post event to own task to get out of ISR / LL context
  MAP_osal_set_event(gapBondMgr_TaskID, GAP_BOND_IDLE_EVT);
}

/*********************************************************************
 * @fn      gapBondMgrReadBonds
 *
 * @brief   Read through NV and store them in RAM.
 *
 * @param   none
 *
 * @return  none
 */
static void gapBondMgrReadBonds(void)
{
  uint8_t idx;

  for(idx = 0; idx < gapBond_maxBonds; idx++)
  {
    // See if the entry exists in NV
    if(osal_snv_read(MAIN_RECORD_NV_ID(idx), sizeof(gapBondRec_t),
       &(bonds[idx])) != SUCCESS)
    {
      // Can't read the entry, assume that it doesn't exist
      VOID MAP_osal_memset(bonds[idx].addr, 0xFF, B_ADDR_LEN);
      bonds[idx].stateFlags = 0;
    }
  }

  if(autoSyncWhiteList)
  {
    gapBondMgr_SyncWhiteList();
  }
}

/*********************************************************************
 * @fn      gapBondMgrReadLruBondList
 *
 * @brief   Find List of LRU Bond in NV or create one.
 *
 * @param   none
 *
 * @return  none
 */
static void gapBondMgrReadLruBondList(void)
{
  // See if the LRU list exists in NV
  if(osal_snv_read(BLE_LRU_BOND_LIST, sizeof(uint8_t) * gapBond_maxBonds,
     gapBond_lruBondList) != SUCCESS)
  {
    // If it doesn't, add the list using the LRU values
    for(uint8_t idx = 0; idx < gapBond_maxBonds; idx++)
    {
      // Store in reverse order (bond record 0 will automatically be
      // most recently used)
      gapBond_lruBondList[idx] = gapBond_maxBonds - idx - 1;
    }
  }
}

/*********************************************************************
 * @fn      gapBondMgrGetLruBondIndex
 *
 * @brief   Get the least recently used bond then set it as the most recently
 *          used bond.  This algorithm expects that this bond will immediately
 *          be used to reference the most recently used bond.
 *
 * @param   none
 *
 * @return  the least recently used bond.
 */
static uint8_t gapBondMgrGetLruBondIndex(void)
{
  // Return bond record index found in the LRU position.
  return gapBond_lruBondList[0];
}

/*********************************************************************
 * @fn      gapBondMgrUpdateLruBondList
 *
 * @brief   Make bond record index the most recently used bond, updating the
 *          list accordingly.
 *
 * @param   the bond record index.
 *
 * @return  none
 */
static void gapBondMgrUpdateLruBondList(uint8_t bondIndex)
{
  uint8_t updateIdx = gapBond_maxBonds;

  // Find the index
  for(uint8_t i = gapBond_maxBonds; i > 0; i--)
  {
    if(gapBond_lruBondList[i - 1] == bondIndex)
    {
      // Found.
      updateIdx = i - 1;
      break;
    }
  }

  // Update list to make bondIndex the most recently used.
  for(uint8_t i = updateIdx ; i < gapBond_maxBonds - 1; i++)
  {
    uint8_t swap = gapBond_lruBondList[i];
    gapBond_lruBondList[i] = gapBond_lruBondList[i + 1];
    gapBond_lruBondList[i + 1] = swap;
  }

  // If there was any change to the list
  if(updateIdx < gapBond_maxBonds - 1)
  {
    // Store updated list in NV
    osal_snv_write(BLE_LRU_BOND_LIST, sizeof(uint8_t) * gapBond_maxBonds,
                   gapBond_lruBondList);
  }
}

/*********************************************************************
 * @fn      gapBondMgrFindEmpty
 *
 * @brief   Look through the bonding NV entries to find an empty.
 *
 * @param   none
 *
 * @return  index to empty bonding (0 - (gapBond_maxBonds-1),
 *          gapBond_maxBonds if no empty entries
 */
static uint8_t gapBondMgrFindEmpty(void)
{
  // Item doesn't exist, so create all the items
  uint8_t idx;

  for(idx = 0; idx < gapBond_maxBonds; idx++)
  {
    // Look for public address of all 0xFF's
    if(MAP_osal_isbufset(bonds[idx].addr, 0xFF, B_ADDR_LEN))
    {
      return (idx);   // Found one
    }
  }

  // If all bonding records are used and LRU bond removal is permitted,
  // use the LRU bond.
  if(gapBond_removeLRUBond)
  {
    idx = gapBondMgrGetLruBondIndex();
    gapBondMgrEraseBonding(idx);
    return (idx);
  }

  return (gapBond_maxBonds);
}

/*********************************************************************
 * @fn      gapBondMgrBondTotal
 *
 * @brief   Look through the bonding NV entries calculate the number
 *          entries.
 *
 * @param   none
 *
 * @return  total number of bonds found
 */
static uint8_t gapBondMgrBondTotal(void)
{
  uint8_t idx;
  uint8_t numBonds = 0;

  // Item doesn't exist, so create all the items
  for(idx = 0; idx < gapBond_maxBonds; idx++)
  {
    // Look for public address that are not 0xFF's
    if(MAP_osal_isbufset(bonds[idx].addr, 0xFF, B_ADDR_LEN) == FALSE)
    {
      numBonds++; // Found one
    }
  }

  return (numBonds);
}

/*********************************************************************
 * @fn      gapBondMgrEraseAllBondings
 *
 * @brief   Write all 0xFF's to all of the bonding entries
 *
 * @param   none
 *
 * @return  SUCCESS if successful.
 *          Otherwise, NV_OPER_FAILED for failure.
 */
static bStatus_t gapBondMgrEraseAllBondings(void)
{
  uint8_t idx;
  bStatus_t stat = SUCCESS;  // return value

  // Item doesn't exist, so create all the items
  for(idx = 0; (idx < gapBond_maxBonds) && (stat == SUCCESS); idx++)
  {
    // Erasing will write/create a bonding entry
    stat = gapBondMgrEraseBonding(idx);
  }

  return (stat);
}

/*********************************************************************
 * @fn      gapBondMgrEraseBonding
 *
 * @brief   Write all 0xFF's to the complete bonding record
 *
 * @param   idx - bonding index
 *
 * @return  SUCCESS if successful.
 *          Otherwise, NV_OPER_FAILED for failure.
 */
static bStatus_t gapBondMgrEraseBonding(uint8_t idx)
{
  bStatus_t ret;
  gapBondRec_t bondRec;

  // First see if bonding record exists in NV, then write all 0xFF's to it
  if((osal_snv_read(MAIN_RECORD_NV_ID(idx), sizeof(gapBondRec_t),
     &bondRec) == SUCCESS) &&
     (MAP_osal_isbufset(bondRec.addr, 0xFF, B_ADDR_LEN) == FALSE))
  {
    gapBondLTK_t ltk;
    gapBondCharCfg_t *charCfg = (gapBondCharCfg_t *)MAP_osal_mem_alloc( sizeof (gapBondCharCfg_t) * gapBond_maxCharCfg );
    if (charCfg == NULL)
    {
      HAL_ASSERT( HAL_ASSERT_CAUSE_OUT_OF_MEMORY );
      return(NV_OPER_FAILED);
    }

    // Initialize memory of temporary variables
    VOID MAP_osal_memset(&ltk, 0xFF, sizeof(gapBondLTK_t));
    VOID MAP_osal_memset(charCfg, 0xFF, sizeof(gapBondCharCfg_t) * gapBond_maxCharCfg);

    {
      uint8_t oldIrk[KEYLEN];

      // Verify that that a previous bond had an IRK before attempting to
      // remove it from the Controller's resolving list.
      if((osal_snv_read(DEV_IRK_NV_ID(idx), KEYLEN, oldIrk) == SUCCESS) &&
         (MAP_osal_isbufset(oldIrk, 0xFF, KEYLEN) == FALSE))
      {
        // Remove device from Resolving List prior to erasing bonding info
        MAP_HCI_LE_RemoveDeviceFromResolvingListCmd((bondRec.addrType &
                                                      MASK_ADDRTYPE_ID),
                                                     bondRec.addr);
      }
    }

    // Erase bond record shadow copy
    VOID MAP_osal_memset(&bondRec, 0xFF, sizeof(gapBondRec_t));

    // Write out FF's over the entire bond entry.
    ret = osal_snv_write(MAIN_RECORD_NV_ID(idx), sizeof(gapBondRec_t),
                         &bondRec);
    ret |= osal_snv_write(LOCAL_LTK_NV_ID(idx), sizeof(gapBondLTK_t), &ltk);
    ret |= osal_snv_write(DEV_LTK_NV_ID(idx), sizeof(gapBondLTK_t), &ltk);
    ret |= osal_snv_write(DEV_IRK_NV_ID(idx), KEYLEN, ltk.LTK);
    ret |= osal_snv_write(DEV_CSRK_NV_ID(idx), KEYLEN, ltk.LTK);
    ret |= osal_snv_write(DEV_SIGN_COUNTER_NV_ID(idx), sizeof(uint32_t),
                          ltk.LTK);

    // Write out FF's over the characteristic configuration entry.
    ret |= osal_snv_write(GATT_CFG_NV_ID(idx), sizeof(gapBondCharCfg_t) * gapBond_maxCharCfg, charCfg);
    MAP_osal_mem_free( charCfg );
  }
  else
  {
    ret = SUCCESS;
  }

  return (ret);
}

/*********************************************************************
 * @fn      gapBondMgrEraseLocalInfo
 *
 * @brief   Write all 0xFF's to all local device information stored in NV: IRK,
 * CSRK, address mode, bonding LRU indices, and random address
 *
 * @return  @ref SUCCESS
 * @return  @ref NV_OPER_FAILED
 */
static bStatus_t gapBondMgrEraseLocalInfo( void )
{
  bStatus_t ret;

  // Create temporary memory to write to NV
  uint8_t tempKey[KEYLEN];
  uint8_t tempByte = 0xFF;
  uint8_t *tempLRUList = (uint8_t *)MAP_osal_mem_alloc( sizeof (uint8_t) * gapBond_maxBonds );
  if (tempLRUList == NULL)
  {
    HAL_ASSERT( HAL_ASSERT_CAUSE_OUT_OF_MEMORY );
    return (NV_OPER_FAILED); 
  }
  
  uint8_t tempAddr[B_ADDR_LEN];

  VOID MAP_osal_memset(tempKey, 0xFF, KEYLEN);
  VOID MAP_osal_memset(tempLRUList, 0xFF, gapBond_maxBonds);
  VOID MAP_osal_memset(tempAddr, 0xFF, B_ADDR_LEN);

  // Erase all local info
  ret = osal_snv_write(BLE_NVID_IRK, KEYLEN, tempKey);
  ret |= osal_snv_write(BLE_NVID_CSRK, KEYLEN, tempKey);
  ret |= osal_snv_write(BLE_NVID_ADDR_MODE, sizeof(uint8_t), &tempByte);
  ret |= osal_snv_write(BLE_LRU_BOND_LIST, gapBond_maxBonds, tempLRUList);
  ret |= osal_snv_write(BLE_NVID_RANDOM_ADDR, B_ADDR_LEN, tempAddr);

  MAP_osal_mem_free( tempLRUList );
  return (ret);
}

/*********************************************************************
 * @brief   Task Initialization function.
 *
 * Internal function defined in gapbondmgr.h.
 */
void GAPBondMgr_Init(uint8_t task_id, uint8_t cfg_gapBond_maxBonds, uint8_t cfg_gapBond_maxCharCfg, uint8_t cfg_gapBond_gatt_no_client, uint8_t cfg_gapBond_gatt_no_service_changed)
{
  gapBondMgr_TaskID = task_id;  // Save task ID

  // Allocate gapBondMgr local data base according to the configured sizes
  gapBond_maxBonds = cfg_gapBond_maxBonds;
  gapBond_maxCharCfg = cfg_gapBond_maxCharCfg;
  gapBond_gatt_no_client = cfg_gapBond_gatt_no_client;
  gapBond_gatt_no_service_changed = cfg_gapBond_gatt_no_service_changed;
  
  //static gapBondRec_t bonds[GAP_BONDINGS_MAX] = {0};
  bonds = (gapBondRec_t *)MAP_osal_mem_alloc( sizeof (gapBondRec_t) * gapBond_maxBonds );
  if (bonds == NULL)
  {
    HAL_ASSERT( HAL_ASSERT_CAUSE_OUT_OF_MEMORY );
    return; 
  }
  MAP_osal_memset(bonds, 0, sizeof (gapBondRec_t) * gapBond_maxBonds);

  //static uint8_t bondsToDelete[GAP_BONDINGS_MAX] = {FALSE};
  bondsToDelete = (uint8_t *)MAP_osal_mem_alloc( sizeof (uint8_t) * gapBond_maxBonds );
  if (bondsToDelete == NULL)
  {
    MAP_osal_mem_free(bonds);
    HAL_ASSERT( HAL_ASSERT_CAUSE_OUT_OF_MEMORY );
    return; 
  }
  MAP_osal_memset(bondsToDelete, FALSE, sizeof (uint8_t) * gapBond_maxBonds);
  
  //uint8_t gapBond_lruBondList[GAP_BONDINGS_MAX] = {0};
  gapBond_lruBondList = (uint8_t *)MAP_osal_mem_alloc( sizeof (uint8_t) * gapBond_maxBonds );
  if (gapBond_lruBondList == NULL)
  {
    MAP_osal_mem_free(bonds);
    MAP_osal_mem_free(bondsToDelete);
    HAL_ASSERT( HAL_ASSERT_CAUSE_OUT_OF_MEMORY );
    return; 
  }
  MAP_osal_memset(gapBond_lruBondList, 0, sizeof (uint8_t) * gapBond_maxBonds);
  
  // Register Call Back functions for GAP
  MAP_GAP_RegisterBondMgrCBs(&gapCBs);

  // Register as auth task
  MAP_GAP_SetParamValue(GAP_PARAM_AUTH_TASK_ID, gapBondMgr_TaskID);

  // Setup Bond RAM Shadow
  gapBondMgrReadBonds();

  // Setup LRU Bond List
  gapBondMgrReadLruBondList();
}

/*********************************************************************
 * @brief   Task Event Processor function.
 *
 * Internal function defined in gapbondmgr.h.
 */
uint16_t GAPBondMgr_ProcessEvent(uint8_t task_id, uint16_t events)
{
  VOID task_id; // OSAL required parameter that isn't used in this function

  if(events & SYS_EVENT_MSG)
  {
    uint8_t *pMsg;

    if((pMsg = MAP_osal_msg_receive(gapBondMgr_TaskID)) != NULL)
    {
// Pass all events to app in test mode
#ifdef GAPBONDMGR_TESTMODE
      gapBondMgr_ProcessOSALMsg((osal_event_hdr_t *)pMsg);
      // In TESTMODE pass through all GAP_MSG_EVENTs to Application
      osal_event_hdr_t *pTmpMsg = (osal_event_hdr_t *)pMsg;
      if(pTmpMsg->event == GAP_MSG_EVENT)
#else // GAPBONDMGR_TESTMODE
      if(!gapBondMgr_ProcessOSALMsg((osal_event_hdr_t *)pMsg))
#endif // GAPBONDMGR_TESTMODE
      {
        // Send it to the registered application.
        if(gapEndAppTaskID != INVALID_TASK_ID)
        {
          // This message wasn't processed in the Bond Manager, send it
          // to an application that wants it.
          VOID MAP_osal_msg_send(gapEndAppTaskID, pMsg);
        }
      }
      else
      {
        // Release the OSAL message
        VOID MAP_osal_msg_deallocate( pMsg );
      }
    }

    // return unprocessed events
    return (events ^ SYS_EVENT_MSG);
  }

  if(events & GAP_BOND_POP_PAIR_QUEUE_EVT)
  {
    if(gapBondStateNodeHead)
    {
      // Find first node to request a pairing.
      gapBondStateNodePtr_t pNode = gapBondFindPairReadyNode();

      if(pNode)
      {
        if(pNode->state == GBM_STATE_WAIT_PAIRING)
        {
          // Set state.
          gapBondStateSetState(pNode, GBM_STATE_IS_PAIRING);

          // Start Pairing.
          gapBondMgrAuthenticate(pNode->connHandle, pNode->addrType,
                                 pNode->pPairReq);

          //  Free remote pair request data if applicable.
          if(pNode->pPairReq)
          {
            MAP_osal_mem_free(pNode->pPairReq);
          }
        }
      }
    }

    return (events ^ GAP_BOND_POP_PAIR_QUEUE_EVT);
  }

  // This is received when the stack has finished all adv/scan/init. These
  // are states where the resolving list cannot be updated so once GAP is no
  // longer in these states the Bond Mgr should sync the resolving list to bond
  // records if there is a pending update.
  if(events & GAP_BOND_IDLE_EVT)
  {
    // Check if the GAP state is still idle since it is possible it was changed
    // in another context. If the GAP state is not idle, this function will be
    // called again automatically by the host (by gapClrState) when gap state is
    // again idle.
    if((gapBond_syncRL == TRUE) && (gapState == GAP_STATE_IDLE))
    {
      gapBondMgr_syncResolvingList();
    }

    return (events ^ GAP_BOND_IDLE_EVT);
  }

  // Discard unknown events
  return 0;
}

/*********************************************************************
 * @fn      gapBondMgr_ProcessOSALMsg
 *
 * @brief   Process an incoming task message.
 *
 * @param   pMsg - message to process
 *
 * @return  TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
static uint8_t gapBondMgr_ProcessOSALMsg(osal_event_hdr_t *pMsg)
{
  uint8_t safeToDealloc = TRUE;

  switch(pMsg->event)
  {
    case GAP_MSG_EVENT:
      safeToDealloc = GAPBondMgr_ProcessGAPMsg((gapEventHdr_t *)pMsg);
      break;

    case SM_MSG_EVENT:
      safeToDealloc = gapBondMgr_ProcessSMMsg((smEventHdr_t*)pMsg);
      break;

    case GATT_MSG_EVENT:
      gapBondMgr_ProcessGATTMsg((gattMsgEvent_t *)pMsg);
      break;

    case GATT_SERV_MSG_EVENT:
      gapBondMgr_ProcessGATTServMsg((gattEventHdr_t *)pMsg);
      break;

    default:
      break;
  }

  return (safeToDealloc);
}

/*********************************************************************
 * @fn      gapBondMgr_CheckNVLen
 *
 * @brief   This function will check the length of an NV Item.
 *
 * @param   id - NV ID.
 * @param   len - lengths in bytes of item.
 *
 * @return  SUCCESS or FAILURE
 */
uint8_t gapBondMgr_CheckNVLen(uint16_t id, uint8_t len)
{
  uint8_t stat = FAILURE;

  // Convert to index
  switch((id - BLE_NVID_GAP_BOND_START) % GAP_BOND_REC_IDS)
  {
    case GAP_BOND_REC_ID_OFFSET:
      if(len == sizeof(gapBondRec_t))
      {
        stat = SUCCESS;
      }

      break;

    case GAP_BOND_LOCAL_LTK_OFFSET:
    case GAP_BOND_DEV_LTK_OFFSET:
      if(len == sizeof(gapBondLTK_t))
      {
        stat = SUCCESS;
      }

      break;

    case GAP_BOND_DEV_IRK_OFFSET:
    case GAP_BOND_DEV_CSRK_OFFSET:
      if(len == KEYLEN)
      {
        stat = SUCCESS;
      }

      break;

    case GAP_BOND_DEV_SIGN_COUNTER_OFFSET:
      if(len == sizeof(uint32_t))
      {
        stat = SUCCESS;
      }

      break;

    default:
      break;
  }

  return (stat);
}

/*********************************************************************
 * @fn      gapBondMgr_syncResolvingList
 *
 * @brief   Add all devices found in bond record to the resolving list in
 *          the controller.
 *
 * public function defined in gap_internal.h
 */
bStatus_t gapBondMgr_syncResolvingList(void)
{
  uint8_t i;

  // If GAP is not idle then modifications to the Controller Resolving List
  // will not be allowed
  if(gapState != GAP_STATE_IDLE)
  {
    return FAILURE;
  }

  // Clear current contents of resolving list
  MAP_HCI_LE_ClearResolvingListCmd();

  // Add device's local IRK
  HCI_LE_AddDeviceToResolvingListCmd(0, 0, 0, MAP_GAP_GetIRK());

  // Write bond addresses into the Resolving List
  for(i = 0; i < gapBond_maxBonds; i++)
  {
    // Make sure empty addresses are not added to the Resolving List
    if(MAP_osal_isbufset(bonds[i].addr, 0xFF, B_ADDR_LEN) == FALSE)
    {
      uint8_t IRK[KEYLEN];

      // Read in NV IRK Record
      if(osal_snv_read(DEV_IRK_NV_ID(i), KEYLEN, IRK) == SUCCESS)
      {
        // Make sure read NV IRK is valid
        if(MAP_osal_isbufset(IRK, 0xFF, KEYLEN) == FALSE)
        {
          // Resolving list does not use ID addr types so must mask away bit
          // prior to adding device to list
          HCI_LE_AddDeviceToResolvingListCmd((bonds[i].addrType &
                                             MASK_ADDRTYPE_ID),
                                             bonds[i].addr, IRK, NULL);

          // If not explicitly Network Privacy Mode, set to Device Privacy Mode
          if(!(bonds[i].stateFlags & GAP_BONDED_STATE_RPA_ONLY))
          {
            MAP_HCI_LE_SetPrivacyModeCmd((bonds[i].addrType &
                                           MASK_ADDRTYPE_ID),
                                          bonds[i].addr,
                                          GAP_PRIVACY_MODE_DEVICE);
          }
        }
      }
    }
  }

  // Clear any flag noting that the resolving list must be re-synced since
  // that has now complete
  gapBond_syncRL = FALSE;

  return SUCCESS;
}

/*********************************************************************
 * @fn          gapBondMgr_ProcessSMMsg
 *
 * @brief       Process an incoming SM messages.
 *
 * @param       pMsg - pointer to received message
 *
 * @return      TRUE if safe to deallocate incoming message, FALSE otherwise.
 */
static uint8_t gapBondMgr_ProcessSMMsg(smEventHdr_t *pMsg)
{
  uint8_t safeToDealloc = FALSE;
  switch(pMsg->opcode)
  {
    case SM_ECC_KEYS_EVENT:
    {
      smEccKeysEvt_t *Keys= (smEccKeysEvt_t *)pMsg;
      gapBond_useEccKeys = TRUE;
      MAP_osal_memcpy(gapBond_eccKeys.privateKey, Keys->privateKey, ECC_KEYLEN);
      MAP_osal_memcpy(gapBond_eccKeys.publicKeyX, Keys->publicKeyX, ECC_KEYLEN);
      MAP_osal_memcpy(gapBond_eccKeys.publicKeyY, Keys->publicKeyY, ECC_KEYLEN);
      // Call app state callback. In this case connectionHandle is not relevant (always 0)
      if(pGapBondCB && pGapBondCB->pairStateCB)
      {
            pGapBondCB->pairStateCB(0, GAPBOND_GENERATE_ECC_DONE, Keys->hdr.status);
      }
    }
      break;
  }
  return safeToDealloc;
}

/*********************************************************************
 * @fn      GAPBondMgr_GenerateEccKeys
 *
 * @brief   Generates ECC keys.
 *
 * @return  SUCCESS - if processing.
 *          FAILURE - if SM is pairing
 */
bStatus_t GAPBondMgr_GenerateEccKeys( void )
{
	return  MAP_SM_GetEccKeys();
}

/*********************************************************************
 * @fn      GAPBondMgr_SCGetLocalOOBParameters
 *
 * @brief   Get local random number and confirm value for secure connection
 *
 * @param   localOobData - Struct holding local's random number and confirm value
 *
 * @return  SUCCESS - Finished successfully.
 *          FAILURE - Failed to generate the local random or confirmation value
 */
bStatus_t GAPBondMgr_SCGetLocalOOBParameters(gapBondOOBData_t *localOobData)
{
  bStatus_t stat;
  /*Generating the local random number - ra*/
  SM_GenerateRandBuf(localOobData->rand, KEYLEN);
  MAP_osal_memcpy(gapBond_localOobData.rand, localOobData->rand, KEYLEN);
  /*Generating the local confirmation value - ca*/
  stat = MAP_SM_GetScConfirmOob(gapBond_eccKeys.publicKeyX, localOobData->rand, localOobData->confirm);
  if(stat != SUCCESS)
  {
	return FAILURE;
  }
  MAP_osal_memcpy(gapBond_localOobData.confirm, localOobData->confirm, KEYLEN);
  localOobAvailable = TRUE;
  return SUCCESS;
}

/*********************************************************************
 * @fn      GAPBondMgr_SCSetRemoteOOBParameters
 *
 * @brief   Set remote's OOB parameters for secure connection
 *
 * @param   remoteOobData - struct holding remote random number and confirm value
 * @param   OOBDataFlag - 1 - OOB Authentication data from remote device present, else 0
 *
 * @return  SUCCESS - Finished successfully.
 *
 */
 bStatus_t GAPBondMgr_SCSetRemoteOOBParameters(gapBondOOBData_t *remoteOobData,
                                              uint8 OOBDataFlag)
{
  gapBond_OOBDataFlag = OOBDataFlag;
  MAP_osal_memcpy(gapBond_remoteOobData.confirm, remoteOobData->confirm, KEYLEN);
  MAP_osal_memcpy(gapBond_remoteOobData.rand, remoteOobData->rand, KEYLEN);
  return SUCCESS;
}

/*********************************************************************
 * @fn          gapBondMgr_ProcessGATTMsg
 *
 * @brief       Process an incoming GATT message.
 *
 * @param       pMsg - pointer to received message
 *
 * @return      none
 */
static void gapBondMgr_ProcessGATTMsg(gattMsgEvent_t *pMsg)
{
  // Process the GATT message
  switch(pMsg->method)
  {

    case ATT_HANDLE_VALUE_CFM:
      if (!gapBond_gatt_no_service_changed) //#ifndef GATT_NO_SERVICE_CHANGED
      {
        // Clear Service Changed flag for this client
        VOID GAPBondMgr_ServiceChangeInd(pMsg->connHandle, 0x00);
      } // #endif // GATT_NO_SERVICE_CHANGED
      break;

//#ifndef GATT_NO_CLIENT

    case ATT_READ_BY_TYPE_RSP:
    {
      if (!gapBond_gatt_no_client)
      {
        if(pMsg->hdr.status == bleProcedureComplete)
        {
          // Progress to next GATT Discovery State.
          gapBondStateStartNextGATTDisc(pMsg->connHandle);

          break;
        }

        switch(gapBondGetStateFromConnHandle(pMsg->connHandle))
        {
          case GBM_STATE_WAIT_GATT_RPAO:
          {
            // Read of Resolvable Private Address Only complete, store in bond
            // record.
            gapBondMgrReadRPAORsp(pMsg->connHandle, &pMsg->msg.readByTypeRsp);
          }
          break;

  #if ( HOST_CONFIG & PERIPHERAL_CFG )
          case GBM_STATE_WAIT_GATT_CAR:
            // Read of Central Address Resolution complete, store in bond record.
            gapBondMgrReadCARRsp(pMsg->connHandle, &pMsg->msg.readByTypeRsp);
            break;
  #endif // PERIPHERAL_CFG

          default:
            // Not expected!
            break;
        }
      }
    }
    break;

    case ATT_ERROR_RSP:
      if (!gapBond_gatt_no_client)
      {
        gapBondMgr_ProcessAttErrRsp(pMsg->connHandle, &pMsg->msg.errorRsp);

        // Progress to next GATT Discovery State.
        gapBondStateStartNextGATTDisc(pMsg->connHandle);
      }
      break;

    case ATT_TRANSACTION_READY_EVENT:
    {
      if (!gapBond_gatt_no_client)
      {
        gbmState_t state;

        // Get state of GATT Discovery on this connection handle.
        state = gapBondGetStateFromConnHandle(pMsg->connHandle);

        // Resume GATT Discovery on this connection handle.
        gapBondStateDoGATTDisc(pMsg->connHandle, state);
      }
    }
    break;
//#endif //!GATT_NO_CLIENT

    default:
      // Unknown message
      break;
  }

  MAP_GATT_bm_free(&pMsg->msg, pMsg->method);
}

//#ifndef GATT_NO_CLIENT
/*********************************************************************
 * @fn          gapBondMgr_ProcessAttErrRsp
 *
 * @brief       Process an incoming ATT Error Response message
 *
 * @param       connHandle - connection handle this message was received on.
 * @param       rsp        - ATT Error message received
 *
 * @return      none
 */
static void gapBondMgr_ProcessAttErrRsp(uint16_t connHandle,attErrorRsp_t *pRsp)
{
  switch(pRsp->reqOpcode)
  {
    case ATT_READ_BY_TYPE_REQ:
    {
      switch(gapBondGetStateFromConnHandle(connHandle))
      {
        case GBM_STATE_WAIT_GATT_RPAO:
        {
          // If Resolvable Private Address Only Characteristic does not
          // exist then treat as Device Privacy Mode.
          bStatus_t ret;

          // Set flag in bond record and notify application
          ret = gapBondMgrSetStateFlagFromConnhandle(connHandle,
                GAP_BONDED_STATE_RPA_ONLY, FALSE);

          // RPAO Char not found.
          linkDBItem_t *pItem;

          pItem = MAP_linkDB_Find(connHandle);

          if(pItem)
          {
            if(gapState == GAP_STATE_IDLE)
            {
              MAP_HCI_LE_SetPrivacyModeCmd(pItem->addrType & MASK_ADDRTYPE_ID,
                                           pItem->addr,
                                           GAP_PRIVACY_MODE_DEVICE);
            }
            else
            {
              gapBond_syncRL = TRUE;
            }
          }

          // Notify application of unsuccessful read of RPAO Characteristic
          if(pGapBondCB && pGapBondCB->pairStateCB)
          {
            pGapBondCB->pairStateCB(connHandle,
                                    GAPBOND_PAIRING_STATE_RPAO_READ, ret);
          }
        }
        break;

#if ( HOST_CONFIG & PERIPHERAL_CFG )
        case GBM_STATE_WAIT_GATT_CAR:
        {
          // If Central Address Resolution does not exist
          bStatus_t ret = bleIncorrectMode;

          // Set flag in bond record and notify application
          ret = gapBondMgrSetStateFlagFromConnhandle(connHandle,
                                                     GAP_BONDED_STATE_CAR,
                                                     GAP_PRIVACY_DISABLED);

          // Notify application of unsuccessful read of CAR characteristic
          if(pGapBondCB && pGapBondCB->pairStateCB)
          {
            pGapBondCB->pairStateCB(connHandle,
                                    GAPBOND_PAIRING_STATE_CAR_READ, ret);
          }
        }
        break;
#endif // PERIPHERAL_CFG

        default:
          // Should not reach here.
          break;
      }

    }
    break;

    default:
      // Error Response to Unknown Request
      break;
  }
}
//#endif //!GATT_NO_CLIENT

/*********************************************************************
 * @fn          gapBondMgr_ProcessGATTServMsg
 *
 * @brief       Process an incoming GATT Server App message.
 *
 * @param       pMsg - pointer to received message
 *
 * @return      none
 */
static void gapBondMgr_ProcessGATTServMsg(gattEventHdr_t *pMsg)
{
  // Process the GATT Server App message
  switch(pMsg->method)
  {
    case GATT_CLIENT_CHAR_CFG_UPDATED_EVENT:
    {
      gattClientCharCfgUpdatedEvent_t *pEvent =
      (gattClientCharCfgUpdatedEvent_t *) pMsg;

      VOID gapBondMgr_UpdateCharCfg(pEvent->connHandle, pEvent->attrHandle,
                                    pEvent->value);
    }
    break;

    default:
      // Unknown message
      break;
  }
}

//#ifndef GATT_NO_SERVICE_CHANGED
/*********************************************************************
 * @fn      gapBondMgrSendServiceChange
 *
 * @brief   Tell the GATT that a service change is needed.
 *
 * @param   pLinkItem - pointer to connection information
 *
 * @return  none
 */
static void gapBondMgrSendServiceChange(linkDBItem_t *pLinkItem)
{
  VOID GATTServApp_SendServiceChangedInd(pLinkItem->connectionHandle,
                                         gapBondMgr_TaskID);
}
//#endif // GATT_NO_SERVICE_CHANGED

/*********************************************************************
 * @fn      gapBondMgrAuthenticate
 *
 * @brief   Initiate authentication
 *
 * @param   connHandle - connection handle
 * @param   addrType - peer address type
 * @param   pPairReq - Enter these parameters if the Pairing Request was already received.
 *          NULL, if waiting for Pairing Request or if initiating.
 *
 * @return  SUCCESS
 *          bleIncorrectMode: Not correct profile role,
 *          INVALIDPARAMETER,
 *          bleNotConnected,
 *          bleAlreadyInRequestedMode,
 *          blePairingTimedOut: Already timed out connection
 *          FAILURE - not workable.
 */
static bStatus_t gapBondMgrAuthenticate(uint16_t connHandle, uint8_t addrType,
                                        gapPairingReq_t *pPairReq)
{
  gapAuthParams_t params;
  linkDBItem_t *pLinkItem;

  // Find the connection
  pLinkItem = MAP_linkDB_Find(connHandle);

  if(pLinkItem == NULL)
  {
    return bleNotConnected;
  }

  VOID MAP_osal_memset(&params, 0, sizeof(gapAuthParams_t));

  // Setup the pairing parameters
  params.connectionHandle = connHandle;
  params.secReqs.ioCaps = gapBond_IOCap;
  params.secReqs.maxEncKeySize = gapBond_KeySize;

  // Setup key distribution bits.
  params.secReqs.keyDist.sEncKey = (gapBond_KeyDistList &
                                    GAPBOND_KEYDIST_SENCKEY) ? TRUE : FALSE;
  params.secReqs.keyDist.sIdKey  = (gapBond_KeyDistList &
                                    GAPBOND_KEYDIST_SIDKEY) ? TRUE : FALSE;
  params.secReqs.keyDist.mEncKey = (gapBond_KeyDistList &
                                    GAPBOND_KEYDIST_MENCKEY) ? TRUE : FALSE;
  params.secReqs.keyDist.mIdKey  = (gapBond_KeyDistList &
                                    GAPBOND_KEYDIST_MIDKEY) ? TRUE : FALSE;
  params.secReqs.keyDist.mSign   = (gapBond_KeyDistList &
                                    GAPBOND_KEYDIST_MSIGN) ? TRUE : FALSE;
  params.secReqs.keyDist.sSign   = (gapBond_KeyDistList &
                                    GAPBOND_KEYDIST_SSIGN) ? TRUE : FALSE;

  // If Secure Connections Only Mode
  params.secReqs.isSCOnlyMode = (gapBond_secureConnection ==
                                 GAPBOND_SECURE_CONNECTION_ONLY) ? TRUE : FALSE;

  // Add Secure Connections to the authentication requirements if enabled and
  // if supported by the MTU size.
  if(L2CAP_GetMTU() >= SECURECONNECTION_MIN_MTU_SIZE)
  {
    params.secReqs.authReq |= (gapBond_secureConnection) ?
                              SM_AUTH_STATE_SECURECONNECTION : 0;
  }

  // If we want to specify the keys
  if(gapBond_useEccKeys)
  {
    params.secReqs.eccKeys.isUsed = TRUE;
    MAP_osal_memcpy(params.secReqs.eccKeys.sK, gapBond_eccKeys.privateKey,
                ECC_KEYLEN);
    MAP_osal_memcpy(params.secReqs.eccKeys.pK_x, gapBond_eccKeys.publicKeyX,
                ECC_KEYLEN);
    MAP_osal_memcpy(params.secReqs.eccKeys.pK_y, gapBond_eccKeys.publicKeyY,
                ECC_KEYLEN);
  }
  else
  {
    params.secReqs.eccKeys.isUsed = FALSE;
  }

  params.secReqs.oobAvailable = gapBond_OOBDataFlag;
  if(params.secReqs.oobAvailable)
  {
    //copy the remotes random number
    VOID MAP_osal_memcpy(params.secReqs.oob, gapBond_remoteOobData.rand, KEYLEN);
    if(params.secReqs.authReq == SM_AUTH_STATE_SECURECONNECTION)
    {
      //copy the remotes confirm value
      MAP_osal_memcpy(params.secReqs.oobConfirm, gapBond_remoteOobData.confirm, KEYLEN);
    }
  }

  params.secReqs.localOobAvailable = localOobAvailable;
  if(params.secReqs.localOobAvailable)
  {
    //copy the local random number - ra
    MAP_osal_memcpy(params.secReqs.localOob, gapBond_localOobData.rand, KEYLEN);
  }
  if(gapBond_Bonding &&
     (addrType == ADDRTYPE_PUBLIC_ID || addrType == ADDRTYPE_RANDOM_ID))
  {
    // Force a slave ID key
    params.secReqs.keyDist.sIdKey = TRUE;
  }
  params.secReqs.authReq |= (gapBond_Bonding) ? SM_AUTH_STATE_BONDING : 0;
  params.secReqs.authReq |= (gapBond_MITM) ? SM_AUTH_STATE_AUTHENTICATED : 0;

  return MAP_GAP_Authenticate(&params, pPairReq);
}

//#ifndef GATT_NO_CLIENT
#if ( HOST_CONFIG & PERIPHERAL_CFG )
/*********************************************************************
 * @fn      gapBondMgrReadCARRsp
 *
 * @brief   Determine whether or not Privacy 1.2 is supported on peer
 *          device through Central Address Resolution characteristic read
 *          response.
 *
 * @param   connHandle -  the connection handle.
 * @param   pRsp       -  Read by Type response message for central
 *                        address resolution UUID.
 *
 * @return  None
 */
static void gapBondMgrReadCARRsp(uint16_t connHandle, attReadByTypeRsp_t *pRsp)
{
  uint8_t value;
  bStatus_t ret = bleNoResources; // return value

  // Parameter Check:
  // Should only exist one Central Address Resolution characteristic on
  // central peer device with a one octet value
  // Vol. 3, Part C, Section 12.4
  if(pRsp->numPairs != NUM_CENT_ADDR_RES_CHAR ||
     pRsp->len != CENT_ADDR_RES_RSP_LEN)
  {
    ret = bleInvalidRange;
  }
  else
  {
    // Read By Type response data list should only contain one handle-value pair
    // First two octets are att handle, third octet is value of characteristic
    value = pRsp->pDataList[2];

    // Set Central Address Resolution state flag for this connection
    ret = gapBondMgrSetStateFlagFromConnhandle(connHandle,
          GAP_BONDED_STATE_CAR, value);
  }

  // Notify application if CAR was unsuccessfully read or if there
  // was an error when storing in NV
  if(pGapBondCB && pGapBondCB->pairStateCB)
  {
    pGapBondCB->pairStateCB(connHandle, GAPBOND_PAIRING_STATE_CAR_READ, ret);
  }
}
#endif // PERIPHERAL_CFG

/*********************************************************************
 * @fn      gapBondMgrReadRPAORsp
 *
 * @brief   Determine whether or not Privacy 1.2 is supported on peer
 *          device through Central Address Resolution characteristic read
 *          response.
 *
 * @param   connHandle -  the connection handle.
 * @param   rsp        -  Read by Type response message for central
 *                        address resolution UUID.
 *
 * @return  None
 */
static void gapBondMgrReadRPAORsp(uint16_t connHandle, attReadByTypeRsp_t *pRsp)
{
  bStatus_t ret = FAILURE; // return value

  // Parameter Check:
  // Should only exist one Resolvable Private Address Only characteristic on
  // central peer device with a one octet value
  // Vol. 3, Part C, Section 12.5
  if(pRsp->numPairs == NUM_RPAO_CHAR && pRsp->len == RPAO_RSP_LEN)
  {
    uint8_t value;

    // Read By Type response data list should only contain one handle-value pair
    // First two octets are att handle, third octet is value of characteristic
    value = pRsp->pDataList[2];

    if(value == RPAO_MODE)
    {
      // Set Bond Resolvable Private Address only flag for this connection
      ret = gapBondMgrSetStateFlagFromConnhandle(connHandle,
            GAP_BONDED_STATE_RPA_ONLY, TRUE);
    }
  }

  // Notify application if CAR was unsuccessfully read or if there
  // was an error when storing in NV
  if(pGapBondCB && pGapBondCB->pairStateCB)
  {
    pGapBondCB->pairStateCB(connHandle, GAPBOND_PAIRING_STATE_RPAO_READ, ret);
  }
}

//#endif //!GATT_NO_CLIENT

//#ifndef GATT_NO_CLIENT
/*********************************************************************
 * @fn      gapBondMgrSetStateFlagFromConnhandle
 *
 * @brief   Set the state flag in the bond record corresponding
 *          to the connection handle provided.
 *
 * @param   connHandle -  the connection handle.
 * @param   stateFlag  -  the state flag.
 * @param   setParam   -  value to update flag with
 *
 * @return      SUCCESS - bond record found and changed,<BR>
 *              bleNoResources - bond record not found (for 0xFFFF connHandle),<BR>
 *              bleNotConnected - connection not found - connHandle is invalid
 *                                (for non-0xFFFF connHandle).
 */
static bStatus_t gapBondMgrSetStateFlagFromConnhandle(uint16_t connHandle,
                                                      uint8_t  stateFlag,
                                                      uint8_t  setParam)
{
  bStatus_t ret = bleNoResources; // return value

  if(connHandle == LINKDB_CONNHANDLE_INVALID)
  {
    uint8_t idx;  // loop counter

    // Run through the bond database and update the flag
    for(idx = 0; idx < gapBond_maxBonds; idx++)
    {
      if(gapBondMgrChangeState(idx, stateFlag, setParam))
      {
        ret = SUCCESS;
      }
    }
  }
  else
  {
    // Find connection information
    linkDBItem_t *pLinkItem = MAP_linkDB_Find(connHandle);

    if(pLinkItem)
    {
      uint8_t idx; // loop counter

      // Search for bond
      if(GAPBondMgr_FindAddr(pLinkItem->addr,
                             (GAP_Peer_Addr_Types_t)(pLinkItem->addrType &
                                                     MASK_ADDRTYPE_ID),
                             &idx, NULL, NULL) == SUCCESS)
      {
        // Bond found, update it.
        gapBondMgrChangeState(idx, stateFlag, setParam);
        ret = SUCCESS;
      }
    }
    else
    {
      ret = bleNotConnected;
    }
  }

  return (ret);
}
//#endif //!GATT_NO_CLIENT

#if ( HOST_CONFIG & PERIPHERAL_CFG )
/*********************************************************************
 * @fn      gapBondMgrSlaveSecurityReq
 *
 * @brief   Send a slave security request
 *
 * @param   connHandle - connection handle
 *
 * @return  SUCCESS, bleNotConnected, blePairingTimedOut
 */
static bStatus_t gapBondMgrSlaveSecurityReq(uint16_t connHandle)
{
  uint8_t authReq = 0;

  authReq |= (gapBond_Bonding) ? SM_AUTH_STATE_BONDING : 0;
  authReq |= (gapBond_MITM) ? SM_AUTH_STATE_AUTHENTICATED : 0;

  //Secure Connection
  authReq |= (gapBond_secureConnection) ? SM_AUTH_STATE_SECURECONNECTION : 0;

  return MAP_GAP_SendSlaveSecurityRequest(connHandle, authReq);
}

#endif // PERIPHERAL_CFG

/*********************************************************************
 * @fn      gapBondMgrBondReq
 *
 * @brief   Initiate a GAP bond request
 *
 * @param   connHandle - connection handle
 * @param   idx - NV index of bond entry
 * @param   stateFlags - bond state flags
 * @param   role - master or slave role
 * @param   startEncryption - whether or not to start encryption
 *
 * @return  none
 */
static void gapBondMgrBondReq(uint16_t connHandle, uint8_t idx,
                              uint8_t stateFlags, uint8_t role,
                              uint8_t startEncryption)
{
  smSecurityInfo_t ltk;
  osalSnvId_t      nvId;

  if(role == GAP_PROFILE_CENTRAL &&
     !(stateFlags & GAP_BONDED_STATE_SECURECONNECTION))
  {
    nvId = DEV_LTK_NV_ID(idx);
  }
  else
  {
    nvId = LOCAL_LTK_NV_ID(idx);
  }

  // Initialize the NV structures
  VOID MAP_osal_memset(&ltk, 0, sizeof(smSecurityInfo_t));

  if(osal_snv_read(nvId, sizeof(smSecurityInfo_t), &ltk) == SUCCESS)
  {
    if((ltk.keySize >= MIN_ENC_KEYSIZE) && (ltk.keySize <= MAX_ENC_KEYSIZE))
    {
      uint8_t authenticated;
      uint8_t secureConnections;

      authenticated = (stateFlags & GAP_BONDED_STATE_AUTHENTICATED)
                      ? TRUE : FALSE;
      secureConnections = (stateFlags & GAP_BONDED_STATE_SECURECONNECTION)
                          ? TRUE : FALSE;

      VOID MAP_GAP_Bond(connHandle, authenticated, secureConnections, &ltk,
                    startEncryption);
    }
  }
}

/*********************************************************************
 * @fn      gapBondMgr_SyncWhiteList
 *
 * @brief   synchronize the White List with the bonds
 *
 * @param   none
 *
 * @return  none
 */
static void gapBondMgr_SyncWhiteList(void)
{
  uint8_t i;

  //erase the White List
  VOID MAP_HCI_LE_ClearWhiteListCmd();

  // Write bond addresses into the White List
  for(i = 0; i < gapBond_maxBonds; i++)
  {
    // Make sure empty addresses are not added to the White List
    if(osal_isbufset(bonds[i].addr, 0xFF, B_ADDR_LEN) == FALSE)
    {
      VOID MAP_HCI_LE_AddWhiteListCmd(HCI_PUBLIC_DEVICE_ADDRESS, bonds[i].addr);
    }
  }
}

/*********************************************************************
 * @fn          gapBondMgr_SyncCharCfg
 *
 * @brief       Update the Bond Manager to have the same configurations as
 *              the GATT database.
 *
 * @param       connHandle - the current connection handle to find client configurations for
 *
 * @return      TRUE if sync done. FALSE, otherwise.
 */
static uint8_t gapBondMgr_SyncCharCfg(uint16_t connHandle)
{
  static gattAttribute_t *pAttr = NULL;
  static uint16_t service;

  // Only attributes with attribute handles between and including the Starting
  // Handle parameter and the Ending Handle parameter that match the requested
  // attribute type and the attribute value will be returned.

  // All attribute types are effectively compared as 128-bit UUIDs,
  // even if a 16-bit UUID is provided in this request or defined
  // for an attribute.
  if(pAttr == NULL)
  {
    pAttr = MAP_GATT_FindHandleUUID(GATT_MIN_HANDLE, GATT_MAX_HANDLE,
                                    clientCharCfgUUID, ATT_BT_UUID_SIZE,
                                    &service);
  }

  if(pAttr != NULL)
  {
    uint16_t len;
    uint8_t attrVal[ATT_BT_UUID_SIZE];

    // It is not possible to use this request on an attribute that has a value
    // that is longer than 2.
    if(MAP_GATTServApp_ReadAttr(connHandle, pAttr, service, attrVal,
                            &len, 0, ATT_BT_UUID_SIZE, 0xFF) == SUCCESS)
    {
      uint16_t value = BUILD_UINT16(attrVal[0], attrVal[1]);

      if(value != GATT_CFG_NO_OPERATION)
      {
        // NV must be updated to meet configuration of the database
        VOID gapBondMgr_UpdateCharCfg(connHandle, pAttr->handle, value);
      }
    }

    // Try to find the next attribute
    pAttr = MAP_GATT_FindNextAttr(pAttr, GATT_MAX_HANDLE, service, NULL);
  }

  return (pAttr == NULL);
}

/*********************************************************************
 * @fn          gapBondPreprocessIdentityInformation
 *
 * @brief       Preprocess Identity Information to prepare for storage.
 *
 * @param       pPkt  - pointer to the GAP Authentication Complete Event
 *                      structure, containing the Identity Information
 * @param       pRec  - output buffer to contain 6 byte unique identifier.  It
 *                      is assumed that the buffer has been zeroed out.
 *
 * @return      SUCCESS if a Identity Information is usable, FAILURE if not.
 */
static uint8_t gapBondPreprocessIdentityInformation(gapAuthCompleteEvent_t
                                                    *pPkt, gapBondRec_t *pRec)
{
  uint8_t ret = SUCCESS;
  smIdentityInfo_t *pInfo = pPkt->pIdentityInfo;

  // If Identity Address is all zeroes
  if(MAP_osal_isbufset(pInfo->bd_addr, 0x00, B_ADDR_LEN))
  {
    // If the IRK is all zeroes
    if(MAP_osal_isbufset(pInfo->irk, 0x00, B_ADDR_LEN))
    {
      ret = FAILURE;
    }
    // IRK is not all zeroes, but a "Unique Identifier" (Volume 3 Part C Section
    // 10.7) is required when the remote device distributes an all zero Identity
    // Address.
    else if(gapBondGenerateUniqueIdentifier((GAP_Peer_Addr_Types_t)(pInfo->addrType &
                                                                    MASK_ADDRTYPE_ID),
                                            pInfo->bd_addr) == FAILURE)
    {
      // No available unique addresses return FAILURE.
      // Note: this error is extremely unlikely.
      ret = FAILURE;
    }

    if(ret == FAILURE)
    {
      // Nullify the Identity Information.
      // Either both the Identity Address and the IRK are all zeroes, or
      // no Unique Identity could be produced.
      pPkt->pIdentityInfo = NULL;
    }
  }

  return ret;
}

/*********************************************************************
 * @fn          gapBondGenerateUniqueIdentifier
 *
 * @brief       Generate a unique identifier address.
 *
 * @param       addrType - address type.
 * @param       pAddr    - output buffer to contain 6 byte unique identifier.
 *                         It is assumed that the buffer has been zeroed out.
 *
 * @return      SUCCESS if a unique address was created, FAILURE if none was
 *              found.
 */
static uint8_t gapBondGenerateUniqueIdentifier(GAP_Peer_Addr_Types_t addrType,
                                               uint8_t *pAddr)
{
  // Unique identifier counter.  Note that this will not cover the full address
  // range of a Bluetooth address, but it is unlikely that a device will come
  // in contact with 2^8 different addresses and bond with all of them.
  // This counter starts over from one on each call, as it is agnostic of the
  // deletion of previously used records.
  uint8_t uniqueID = 0;

  // Search for a unique Identifier.
  do
  {
    // Set unique identifier in buffer.
    // Use pre-increment to disallow an all-zero address.
    *pAddr = ++uniqueID;

    // Check that the Identifier is unique to the set of known addresses: i.e.,
    // the address does not exist in the bonding table
    if ((GAPBondMgr_FindAddr(pAddr, addrType, NULL, NULL, NULL) != SUCCESS))
    {
      // Success...address wasn't found
      break;
    }

    // Rolling over indicates no unique addresses are available.
  } while(uniqueID);


  if(uniqueID)
  {
    return SUCCESS;
  }
  else
  {
    return FAILURE;
  }
}

/*********************************************************************
 * @fn          gapBondMgr_isDevicePrivacyModeSupported
 *
 * @brief       Determine if Device Privacy Mode can be used for the device
 *              associated with the address provided based on the requirements
 *              that no Resolvable Private Address Only Characteristic exists
 *              within the remote device's GATT Server.
 *
 * @param       addrType - address type of remote device.
 * @param       pAddr    - identity address of device to check.
 *
 * @return      TRUE if Device Privacy Mode is permitted, FALSE otherwise.
 */
uint8_t gapBondMgr_isDevicePrivacyModeSupported(GAP_Peer_Addr_Types_t addrType,
                                                uint8_t *pAddr)
{
  uint8_t idx;

  // If bonding information exists
  if(GAPBondMgr_FindAddr(pAddr, addrType, &idx, NULL, NULL) == SUCCESS)
  {
    // If state flags indicate the remote does not have an instance of the
    // Resolvable Private Address Only (RPAO) Characteristic then Device Privacy Mode
    // is permitted.
    if((gapBondMgrGetStateFlags(idx) & GAP_BONDED_STATE_RPA_ONLY) == 0)
    {
      return (TRUE);
    }
  }

  // Not bonded or bonded but RPAO Characteristic was found, so Device Privacy
  // Mode cannot be set.
  return (FALSE);
}

/*********************************************************************
 * @fn          gapBondStateStartSecurity
 *
 * @brief       Start a pairing and create a Node at the beginning of the state
 *              machine.
 *
 * @param       connHandle - the connnection handle associated with the node.
 * @param       addrType   - over the air address type of device to pair with.
 * @param       pPairReq   - pairing information of Remote device if called in
 *                           response to recieving a SMP Pairing Request, else
 *                           NULL.
 *
 * @return      bStatus_t
 */
static bStatus_t gapBondStateStartSecurity(uint16_t connHandle,
                                           uint8_t addrType,
                                           gapPairingReq_t *pPairReq)
{
  uint8_t ret = SUCCESS;
  uint8_t isHandleExist = FALSE;
  // check if connHandle already in the queue
  gapBondStateNodePtr_t qNode = gapBondStateNodeHead;
  while(qNode)
  {
    if(qNode->connHandle == connHandle)
    {
      //found handle in list
      isHandleExist = TRUE;
      break;
    }
    qNode = qNode->pNext;
  }

  if(isHandleExist == FALSE)
  {
    // Enqueue
    qNode = gapBondMgrQueuePairing(connHandle, addrType, pPairReq);
    if(qNode == NULL)
    {
      return(bleNoResources);
    }
  }

  if(GAP_isPairing())
  {
	return ret;
  }

  // Set the state.
  gapBondStateSetState(qNode, GBM_STATE_IS_PAIRING);

  // Start Pairing.
  ret = gapBondMgrAuthenticate(connHandle, addrType, pPairReq);

  // Call app state callback
  if ( pGapBondCB && pGapBondCB->pairStateCB )
  {
    pGapBondCB->pairStateCB( connHandle, GAPBOND_PAIRING_STATE_STARTED, SUCCESS );
  }
  return (ret);
}

//#if !defined ( GATT_NO_CLIENT )
/*********************************************************************
 * @fn          gapBondStateStartNextGATTDisc
 *
 * @brief       Find a Node from the connHandle and perform the next GATT
 *              Discovery in the state machine.
 *
 * @param       connHandle - the connnection handle associated with the node.
 *
 * @return      bStatus_t
 */
static bStatus_t gapBondStateStartNextGATTDisc(uint16_t connHandle)
{
  gbmState_t state;

  // Find the active node
  {
    gapBondStateNodePtr_t pCurr = gapBondStateNodeHead;

    // Find the node
    while(pCurr)
    {
      if(pCurr->connHandle == connHandle)
      {
        break;
      }

      pCurr = pCurr->pNext;
    }

    if(pCurr == NULL)
    {
      return (FAILURE);
    }

    if((pCurr->state == GBM_STATE_IS_PAIRING) &&
       (gapBondFindPairReadyNode() != NULL))
    {
      // set an event to start pairing.
      MAP_osal_set_event(gapBondMgr_TaskID, GAP_BOND_POP_PAIR_QUEUE_EVT);
    }

    // Move to next state
    state = ++pCurr->state;
  }

  // Do GATT Discovery
  gapBondStateDoGATTDisc(connHandle, state);

  return (SUCCESS);
}

/*********************************************************************
 * @fn          gapBondStateDoGATTDisc
 *
 * @brief       Perform the requested GATT Discovery in the state machine.
 *
 * @param       connHandle - the connnection handle associated with the node.
 * @param       state      - GATT Discovery state to execute.
 *
 * @return      None.
 */
static void gapBondStateDoGATTDisc(uint16_t connHandle, gbmState_t state)
{
  uint8_t ret;

  switch(state)
  {
    case GBM_STATE_WAIT_GATT_RPAO:
      // Read peer's Resolvable Private Address Only Characteristic.
      // Required before a device can assume the remote will only send
      // resolvable private addresses or before attempting to set Device
      // Privacy Mode with the remote device. Vol. 3, Part C, Section 12.5
      ret = gapBondMgr_ReadGattChar(connHandle,
                                    RESOLVABLE_PRIVATE_ADDRESS_ONLY_UUID);

      // If read fails notify application that read failed
      if(ret != SUCCESS && pGapBondCB && pGapBondCB->pairStateCB)
      {
        pGapBondCB->pairStateCB(connHandle, GAPBOND_PAIRING_STATE_RPAO_READ,
                                ret);
      }

      break;

#if ( HOST_CONFIG & PERIPHERAL_CFG )
    case GBM_STATE_WAIT_GATT_CAR:
      // Read peer's Central Address Resolution characteristic
      // Required before a peripheral device can use RPA for initiator
      // address within directed advertisements. Vol. 3, Part C,
      // Section 12.4
      ret = gapBondMgr_ReadGattChar(connHandle,
                                    CENTRAL_ADDRESS_RESOLUTION_UUID);

      // If read fails notify application that read failed
      if(ret != SUCCESS && pGapBondCB && pGapBondCB->pairStateCB)
      {
        pGapBondCB->pairStateCB(connHandle, GAPBOND_PAIRING_STATE_CAR_READ,
                                ret);
      }
      break;
#endif // PERIPHERAL_CFG

    case GBM_STATE_END:
    default:
      gapBondStateEnd(connHandle);
      break;
  }
}
//#endif //defined !GATT_NO_CLIENT

/*********************************************************************
 * @fn          gapBondStateEnd
 *
 * @brief       Find Node from connHandle and remove from queue, it has
 *              completed the state machine.
 *
 * @param       connHandle - the connnection handle associated with the node.
 *
 * @return      bStatus_t
 */
static bStatus_t gapBondStateEnd(uint16_t connHandle)
{
  gapBondStateNodePtr_t pPrev = NULL;
  gapBondStateNodePtr_t pCurr = gapBondStateNodeHead;

  // Find the node
  while(pCurr)
  {
    if(pCurr->connHandle == connHandle)
    {
      // Handle case where node is head of queue.
      if(pPrev == NULL)
      {
        gapBondStateNodeHead = pCurr->pNext;
      }
      else
      {
        pPrev->pNext = pCurr->pNext;
      }

      // Free the node.
      if(pCurr->pPairReq)
      {
        MAP_osal_mem_free(pCurr->pPairReq);
      }

      MAP_osal_mem_free(pCurr);

      // We're done.
      break;
    }
    else
    {
      // Save the previous node.
      pPrev = pCurr;

      // Look in the next.
      pCurr = pCurr->pNext;
    }
  }

  return (SUCCESS);
}

//#if !defined ( GATT_NO_CLIENT )
/*********************************************************************
 * @fn          gapBondGetStateFromConnHandle
 *
 * @brief       Find Node and determinate state machine state from the
 *              connhandle.
 *
 * @param       connHandle - the connnection handle associated with the node.
 *
 * @return      gbmState_t value for the node associated with connHandle.
 */
static gbmState_t gapBondGetStateFromConnHandle(uint16_t connHandle)
{
  gbmState_t state = GBM_STATE_END;

  gapBondStateNodePtr_t pCurr = gapBondStateNodeHead;

  while(pCurr)
  {
    if(pCurr->connHandle == connHandle)
    {
      state = pCurr->state;

      break;
    }

    pCurr = pCurr->pNext;
  }

  return state;
}
//#endif //defined !GATT_NO_CLIENT

/*********************************************************************
 * @fn          gapBondStateSetState
 *
 * @brief       Set the state of the node.  Assumes that pNode is not NULL.
 *
 * @param       pNode - the node whose state shall be changed.
 * @param       state - the new state.
 *
 * @return      None
 */
static void gapBondStateSetState(gapBondStateNode_t *pNode, gbmState_t state)
{
  pNode->state = state;
}

/*********************************************************************
 * @fn          gapBondMgrQueuePairing
 *
 * @brief       Add a pairing to the queue.
 *
 * @param       connHandle - the connection handle associated the remote device.
 * @param       addrType   - peer over the air address type.
 * @param       pPairReq   - pairing information from remote device when this
 *                           node was created after receiving a SMP Pairing
 *                           Request.
 *
 * @return      TRUE if pairing in progress, FALSE Otherwise.
 */
static gapBondStateNodePtr_t gapBondMgrQueuePairing(uint16_t connHandle,
    uint8_t addrType,
    gapPairingReq_t *pPairReq)
{
  // Allocated space on the queue
  gapBondStateNodePtr_t pNewNode = (gapBondStateNode_t *) MAP_osal_mem_alloc(sizeof(
                                     gapBondStateNode_t));

  if(pNewNode)
  {
    pNewNode->connHandle = connHandle;
    pNewNode->addrType = addrType;
    pNewNode->pNext = NULL;
    pNewNode->state = GBM_STATE_WAIT_PAIRING;

    // If there was a pairing request message.
    if(pPairReq)
    {
      pNewNode->pPairReq = (gapPairingReq_t *)MAP_osal_memdup(pPairReq,
                           sizeof(gapPairingReq_t));

      if(!pNewNode->pPairReq)
      {
        // malloc failed, free pairing and return
        MAP_osal_mem_free(pNewNode);

        return NULL;
      }
    }
    else
    {
      pNewNode->pPairReq = NULL;
    }

    // Check if queue is empty
    if(gapBondStateNodeHead == NULL)
    {
      gapBondStateNodeHead = pNewNode;
    }
    else
    {
      gapBondStateNodePtr_t qNode = gapBondStateNodeHead;

      while(qNode->pNext != NULL)
      {
        qNode = qNode->pNext;
      }

      qNode->pNext = pNewNode;
    }
  }

  return pNewNode;
}

/*********************************************************************
 * @fn          gapBondFindPairReadyNode
 *
 * @brief       Find the first node to be ready to pair and return it.  Nodes
 *              are processed in FIFO order.
 *
 * @param       None
 *
 * @return      gapBondStateNodePtr_t if a device is waiting to pair,
 *              NULL otherwise.
 */
static gapBondStateNodePtr_t gapBondFindPairReadyNode(void)
{
  // Find the first node ready to pair and set event.
  gapBondStateNodePtr_t pCurr = gapBondStateNodeHead;

  if(pCurr)
  {
    while((pCurr = pCurr->pNext))
    {
      // If a pairing request queued
      if(pCurr->state == GBM_STATE_WAIT_PAIRING)
      {
        return pCurr;
      }
    }
  }

  return NULL;
}

#endif // GAP_BOND_MGR

#endif // ( CENTRAL_CFG | PERIPHERAL_CFG )

/*********************************************************************
*********************************************************************/
