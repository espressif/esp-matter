/******************************************************************************

 @file  icall_hci_tl.c

 @brief This file contains the HCI TL implementation to transpose HCI serial
        packets into HCI function calls to the Stack.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2022, Texas Instruments Incorporated
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

/*********************************************************************
 * INCLUDES
 */
#include <ti/sysbios/knl/Task.h>
#include <string.h>

#include "comdef.h"

#include <icall.h>
#include "util.h"
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"
#include "icall_hci_tl.h"

#if defined(GAP_BOND_MGR)
#include "gapbondmgr_internal.h"
#endif //GAP_BOND_MGR
#include "gap_internal.h"

#if (defined(HCI_TL_FULL) || defined(PTM_MODE))
#include "inc/npi_ble.h"
#include "inc/npi_task.h"
#endif

#if  (!((defined(STACK_LIBRARY)) || defined (PTM_MODE))) && (defined(HOST_CONFIG))
#error "This file is meant to be used with STACK_LIBRARY define at the project level. Not doing so will lead to unresolve utility API not being linked with the stack library"
#endif /*  (!((defined(STACK_LIBRARY)) || defined (PTM_MODE)) && (defined(HOST_CONFIG)) */

#if  (defined(PTM_MODE) && defined(HCI_TL_FULL))
#error "Either PTM_MODE or HCI_TL_FULL should be enable, not both! (Both are TL defines)"
#endif /*  defined (PTM_MODE))) && (defined(PTM_MODE)) */

#if  defined(TESTMODES) && (!(HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG )))
#error " TESTMODES should not be defined if CENTRAL_CFG or PERIPHERAL_CFG is not defined"
#endif /* defined(TESTMODES) && (!(HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ))) */

/*********************************************************************
 * MACROS
 */

/**
 * 32 bits of data are used to describe the arguments of each HCI command.
 * a total of 8 arguments are allowed and the bits are divided equally between
 * each argument, thus allocating 4 bits to describe all possible rules for
 * interpreting the serialized arguments of an HCI buffer into a parameterized
 * list for calling the variadic function macro which maps a serial packet's
 * opcode to the corresponding HCI function in the Stack. 4 bits allow for 16
 * different rules, with the used bit permutations described below.
 *
 * Multi-octet values are little-endian unless otherwise specified.
 *
 * Value  | Rule
 *********|*********************************************************************
 * 0b0000 | No Parameter, signifies end of parsing
 * 0b0001 | 1 byte parameter
 * 0b0010 | 2 byte parameter
 * 0b0011 | 4 byte parameter (not used)
 * 0b0100 | reserved
 * 0b0101 | pointer to a single byte parameter
 * 0b0110 | pointer to 2 byte parameter
 * 0b0111 | reserved
 * 0b1000 | reserved
 * 0b1001 | 1 byte parameter implying the len (in units specified by the rule)
 *          of upcoming buffer(s)
 * 0b1010 | reserved
 * 0b1011 | reserved
 * 0b1100 | pointer to offset 0 of a 2 byte buffer
 * 0b1101 | pointer to offset 0 of a 6 byte buffer (BLE Address)
 * 0b1110 | pointer to offset 0 of a 8 byte buffer
 * 0b1111 | pointer to offset 0 of a 16 byte buffer (key buffer)
 *******************************************************************************
 *
 * Alternate meanings:
 *                     The command index field is a 16 bit field which may
 * have greater than 256 indexes, but would never require the full range.  The
 * highest order bit of this field can be used to switch the rules parameter
 * from specifying a pointer to an extended array of rules for large commands.
 */
// Number of parameters supported
#define HCI_MAX_NUM_ARGS                          12
#define HCI_DEFAULT_NUM_RULES                      8

// number of bits used to describe each parameter
#define PARAM_BIT_WIDTH                           4
#define PARAM_BIT_MASK                            0x0000000F

// Macro for condensing rules into a 32 bit value.
#define PARAM(x, i)                               ((x) << ((i)))
#define PARAM0(x)                                 PARAM(x, 0)
#define PARAM1(x)                                 PARAM(x, 1 * PARAM_BIT_WIDTH)
#define PARAM2(x)                                 PARAM(x, 2 * PARAM_BIT_WIDTH)
#define PARAM3(x)                                 PARAM(x, 3 * PARAM_BIT_WIDTH)
#define PARAM4(x)                                 PARAM(x, 4 * PARAM_BIT_WIDTH)
#define PARAM5(x)                                 PARAM(x, 5 * PARAM_BIT_WIDTH)
#define PARAM6(x)                                 PARAM(x, 6 * PARAM_BIT_WIDTH)
#define PARAM7(x)                                 PARAM(x, 7 * PARAM_BIT_WIDTH)

#define HPARAMS(x0, x1, x2, x3, x4, x5, x6, x7)   (PARAM7(x7) | \
                                                   PARAM6(x6) | \
                                                   PARAM5(x5) | \
                                                   PARAM4(x4) | \
                                                   PARAM3(x3) | \
                                                   PARAM2(x2) | \
                                                   PARAM1(x1) | \
                                                   PARAM0(x0))

#define HPARAMS_DUPLE(x0, x1)                     (PARAM1(x1) | PARAM0(x0))

#ifdef STACK_LIBRARY
#define EXTENDED_PARAMS_FLAG                      0x80000000
#else
#define EXTENDED_PARAMS_FLAG                      0x8000
#endif
#define PARAM_FLAGS                               (EXTENDED_PARAMS_FLAG)
#define EXTENDED_PARAMS_CMD_IDX_GROUP(idx)        ((EXTENDED_PARAMS_FLAG) | (idx))
#define IS_EXTENDED_PARAMS(cmdIdxGroup)           ((cmdIdxGroup) & (EXTENDED_PARAMS_FLAG))
#define GET_CMD_IDX(cmdIdxGroup)                  ((cmdIdxGroup) & ~(PARAM_FLAGS))
#define GET_RULE(pRules, arg_i)                   ((uint8_t)(((pRules)[(arg_i)/2] >> ((arg_i) % 2) * PARAM_BIT_WIDTH) & PARAM_BIT_MASK))
#define HOST_OPCODE(csg, opcode)                  ((((csg) & 0x7) << 7) | ((opcode) & 0x7F))


#ifdef STACK_LIBRARY
#define HCI_EXTENDED_ENTRY(op, idx, ruleBuffer) \
{(op), 1, ((uint32_t) idx), (uint32_t)(ruleBuffer)}
// Abstraction of a HCI translation table entry.
#define HCI_TRANSLATION_ENTRY(op, idx, r0, r1, r2, r3, r4, r5, r6, r7) \
{ (op), 0,  ((uint32_t)idx), HPARAMS(r0, r1, r2, r3, r4, r5, r6, r7) }

#else

#define HCI_EXTENDED_ENTRY(op, idx, ruleBuffer) \
{(op), EXTENDED_PARAMS_CMD_IDX_GROUP(idx), (uint32_t)(ruleBuffer)}
// Abstraction of a HCI translation table entry.
#define HCI_TRANSLATION_ENTRY(op, idx, r0, r1, r2, r3, r4, r5, r6, r7) \
{ (op), (idx), HPARAMS(r0, r1, r2, r3, r4, r5, r6, r7) }

#endif

// HCI Rule Codes.
#define HLEN                                      0x8 // buffer len type.
#define HPTR                                      0x4 // pointer type.
#define HU32                                      0x3 // 4 octets. Intentionally uses 2 bits.
#define HU16                                      0x2 // 2 octets.
#define HU8                                       0x1 // 1 octet.
#define HNP                                       0x0 // No Parameter. Encountering this rule ends the
                                                      // translation a call to the stack is made.

// Pointers.
#define HU8PTR                                    (HPTR | HU8)  // Pointer to buffer of 1 byte values.
#define HU16PTR                                   (HPTR | HU16) // Pointer to buffer of 2 byte values.

// Pointer mask.
#define HPTRMASK                                  0xB

// Buf counters.
#define HU8LEN                                    (HLEN | HU8)         // 1 byte parameter buffer length counter.
#define H2B                                       (HLEN | HPTR)        // implicit  2 byte count.
#define HAB                                       (HLEN | HPTR | HU8)  // implicit  6 byte count.
#define H8B                                       (HLEN | HPTR | HU16) // implicit  8 byte count.
#define HKB                                       (HLEN | HPTR | HU32) // implicit 16 byte count.

#define HCI_TL_LE_CREATE_CONN_NUM_PARAMS          12

#define RSP_PAYLOAD_IDX                           6
#define MAX_RSP_DATA_LEN                          100
#define MAX_RSP_BUF                               (RSP_PAYLOAD_IDX + MAX_RSP_DATA_LEN)

#define GET_HEAP_STATS_LIGHT                      0x1
#define GET_HEAP_STATS_FULL                       0x2
#define GET_THREAD_STATS                          0x4

#if !defined(HCI_EXT_APP_OUT_BUF)
  #define HCI_EXT_APP_OUT_BUF                     44
#endif // !HCI_EXT_APP_OUT_BUF

#define KEYDIST_SENC                              0x01
#define KEYDIST_SID                               0x02
#define KEYDIST_SSIGN                             0x04
#define KEYDIST_SLINK                             0x08
#define KEYDIST_MENC                              0x10
#define KEYDIST_MID                               0x20
#define KEYDIST_MSIGN                             0x40
#define KEYDIST_MLINK                             0x80

// Maximum number of reliable writes supported by Attribute Client
#define GATT_MAX_NUM_RELIABLE_WRITES              5

#if !defined(STACK_REVISION)
  #define STACK_REVISION                          0x010100
#endif // STACK_REVISION

#define PROXY_ID(id) (ICall_getLocalMsgEntityId(ICALL_SERVICE_CLASS_BLE_MSG, (id)))

// Note: Size of Extended Adv Report Less Appended two byte Vendor Specific DID field.
#define HCI_AE_EVENT_LENGTH                      (sizeof(aeExtAdvRptEvt_t) - sizeof(((aeExtAdvRptEvt_t *)0)->pData) - sizeof(uint16))
#define MAX_REPORT_DATA_SIZE                     200
#define SCAN_EVENT_DATA_COMPLETE_MASK            0xFF9F
#define SCAN_EVENT_INCOMPLETE_WITH_MORE          0x0020

#define GAP_SCAN_EVENT_LENGTH                    (sizeof(GapScan_Evt_AdvRpt_t) - sizeof(((GapScan_Evt_AdvRpt_t *)0)->pData))
#define GAP_SCAN_EVENT_HEADER                    7

#define HCI_MAX_TL_AE_DATA_LEN                   251

#define ADV_LEGACY_SET_HANDLE                    0

#ifdef BLE3_CMD
#define GAP_MAKE_ADDR_RS(pAddr)  ((pAddr[B_ADDR_LEN-1] |= STATIC_ADDR_HDR))
#endif
/*********************************************************************
 * TYPEDEFS
 */
#ifdef STACK_LIBRARY
typedef struct
{
  uint16_t opcode;
  uint8_t  extFlag;
  uint32_t cmdIdx;
  uint32_t paramRules;
} hciTranslatorEntry_t;
#define IDX_CAST (uint32_t)

#else
typedef struct
{
  uint16_t opcode;
  uint16_t cmdIdx;
  uint32_t paramRules;
} hciTranslatorEntry_t;
#define IDX_CAST
#endif

typedef const hciTranslatorEntry_t hciEntry_t;

typedef uint32_t hci_arg_t;

typedef struct
{
  uint8_t  pktType;
  uint16_t opCode;
  uint16_t  len;
  uint8_t  *pData;
} hciExtCmd_t;

/* Advertising Extension management */
typedef struct hci_tl_advSet_t hci_tl_advSet_t;

// Extended Advertising Set Entry
struct hci_tl_advSet_t
{
    hci_tl_advSet_t       *next;            // ptr to next handle in list, if any
    uint8_t               handle;
    aeRandAddrCmd_t       setAddrCmd;
    aeEnableCmd_t         enableCmdParams;
    aeSetParamCmd_t       advCmdParams;
    aeSetDataCmd_t        advCmdData;
    aeEnableScanCmd_t     enableScanCmdParams;
    aeSetDataCmd_t        scanCmdData;
};

// Structure containing the parameter for the GAP Scanner Event callback
typedef struct
{
  uint32_t event;
  uint8_t* pData;
}scanEvtCallback_t;

// Structure containing the parameter for the GAP advertiser Event callback
typedef struct
{
  uint32_t event;
  uint8_t* pData;
}advEvtCallback_t;

// Structure containing the parameter for the HCI(LL) Event callback
typedef struct
{
  uint8_t event;
  union
  {
    uint8_t handle;
    void*   pData;
  }data;
}hci_tl_AdvEvtCallback_t;

// Structure containing the parameter for the HCI(LL) Scan Event callback
typedef struct
{
  uint8_t event;
  void *  pData;
}hci_tl_ScanEvtCallback_t;

// Structure containing the parameter for the passcode Event callback
typedef struct
{
  uint8_t connectionHandle;
} passcodeEvtCallback_t;

/*********************************************************************
 * EXTERNS
 */

extern uint32         lastAppOpcodeIdxSent;
extern const uint16_t ll_buildRevision;
extern gattAttribute_t gattAttrTbl[];
/*********************************************************************
 * LOCAL VARIABLES
 */
#if (defined(HCI_TL_FULL) || defined(PTM_MODE)) && !defined(HOST_CONFIG)

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
static hci_tl_advSet_t      *hci_tl_advSetList = NULL;
#endif // ADV_NCONN_CFG || ADV_CONN_CFG

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
static aeSetScanParamCmd_t  hci_tl_cmdScanParams;
static aeEnableScanCmd_t    hci_tl_cmdScanEnable;
#endif // SCAN_CFG
#endif // (HCI_TL_FULL || PTM_MODE) && (!HOST_CONFIG)


#if defined(HCI_TL_FULL)

#if ( HOST_CONFIG & ( CENTRAL_CFG | OBSERVER_CFG ) )
static uint8_t              host_tl_gapScannerInitialized;
#endif

#ifdef BLE3_CMD
#if ( HOST_CONFIG & ( PERIPHERAL_CFG | BROADCASTER_CFG ) )
// Default - infinite advertising
uint16_t advDuration = 0;
uint8_t advHandleLegacy = 0xFF;
uint8_t advEventCntr = 0;
uint8_t maxConnReached = 0;
#endif

#if ( HOST_CONFIG & ( CENTRAL_CFG | OBSERVER_CFG ) )
// Default - infinite scan
uint16_t scanDuration = 0;
uint8_t numDev = 0;
uint8_t scanSummarySent = 0;
#endif

uint8_t advNotice = 0;
uint8_t scanNotice = 0;
uint8_t sendEstEvt = 0;
uint8_t maxNumReports = 0;
uint8_t legacyConnCancel = 0;
deviceInfo_t *deviceInfoArr = NULL;
uint8_t makeDiscFlag = 0;
uint8_t endDiscFlag = 0;
#endif // BLE3_CMD

#if defined(GAP_BOND_MGR)
uint32_t host_tl_defaultPasscode = B_APP_DEFAULT_PASSCODE;
// Passcode.
static void host_tl_passcodeCB(uint8_t *deviceAddr, uint16_t connectionHandle,
                               uint8_t uiInputs, uint8_t uiOutputs,
                               uint32_t numComparison);
// Bond Manager Callbacks
static const gapBondCBs_t host_tl_bondCB =
{
  (pfnPasscodeCB_t)host_tl_passcodeCB,  //  Passcode callback
  NULL		                        //  Pairing state callback
};

#endif //GAP_BOND_MGR

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
#ifndef LEGACY_CMD
static uint8_t hci_tl_le_create_conn_params[] =
{
  HCI_TL_LE_CREATE_CONN_NUM_PARAMS,
  HPARAMS_DUPLE(HU16, HU16),
  HPARAMS_DUPLE(HU8, HU8),
  HPARAMS_DUPLE(HAB, HU8),
  HPARAMS_DUPLE(HU16, HU16),
  HPARAMS_DUPLE(HU16, HU16),
  HPARAMS_DUPLE(HU16, HU16)
};
#endif // LEGACY_CMD
#endif /* defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG) */

static hciEntry_t hciTranslationTable[] =
{
  // Opcode                                                            Command function index                               Param 0        1        2        3        4        5        6        7
  HCI_TRANSLATION_ENTRY(HCI_READ_RSSI,                                 IDX_CAST IDX_HCI_ReadRssiCmd,                              HU16,    HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_SET_EVENT_MASK,                            IDX_CAST IDX_HCI_SetEventMaskCmd,                          HU8PTR,  HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_SET_EVENT_MASK_PAGE_2,                     IDX_CAST IDX_HCI_SetEventMaskPage2Cmd,                     HU8PTR,  HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
#ifndef LEGACY_CMD
  HCI_TRANSLATION_ENTRY(HCI_RESET,                                     IDX_CAST IDX_HCI_ResetCmd,                                 HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
#endif // LEGACY_CMD
  HCI_TRANSLATION_ENTRY(HCI_READ_LOCAL_VERSION_INFO,                   IDX_CAST IDX_HCI_ReadLocalVersionInfoCmd,                  HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_READ_LOCAL_SUPPORTED_COMMANDS,             IDX_CAST IDX_HCI_ReadLocalSupportedCommandsCmd,            HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_READ_LOCAL_SUPPORTED_FEATURES,             IDX_CAST IDX_HCI_ReadLocalSupportedFeaturesCmd,            HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_READ_BDADDR,                               IDX_CAST IDX_HCI_ReadBDADDRCmd,                            HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_RECEIVER_TEST,                          IDX_CAST IDX_HCI_LE_ReceiverTestCmd,                       HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_TRANSMITTER_TEST,                       IDX_CAST IDX_HCI_LE_TransmitterTestCmd,                    HU8,     HU8,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_TEST_END,                               IDX_CAST IDX_HCI_LE_TestEndCmd,                            HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_ENCRYPT,                                IDX_CAST IDX_HCI_LE_EncryptCmd,                            HKB,     HKB,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_RAND,                                   IDX_CAST IDX_HCI_LE_RandCmd,                               HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_READ_SUPPORTED_STATES,                  IDX_CAST IDX_HCI_LE_ReadSupportedStatesCmd,                HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_READ_WHITE_LIST_SIZE,                   IDX_CAST IDX_HCI_LE_ReadWhiteListSizeCmd,                  HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_CLEAR_WHITE_LIST,                       IDX_CAST IDX_HCI_LE_ClearWhiteListCmd,                     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_ADD_WHITE_LIST,                         IDX_CAST IDX_HCI_LE_AddWhiteListCmd,                       HU8,     HAB,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_REMOVE_WHITE_LIST,                      IDX_CAST IDX_HCI_LE_RemoveWhiteListCmd,                    HU8,     HAB,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_EVENT_MASK,                         IDX_CAST IDX_HCI_LE_SetEventMaskCmd,                       HU8PTR,  HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_READ_LOCAL_SUPPORTED_FEATURES,          IDX_CAST IDX_HCI_LE_ReadLocalSupportedFeaturesCmd,         HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_READ_BUFFER_SIZE,                       IDX_CAST IDX_HCI_LE_ReadBufSizeCmd,                        HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_RANDOM_ADDR,                        IDX_CAST IDX_HCI_LE_SetRandomAddressCmd,                   HU8PTR,  HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
  HCI_TRANSLATION_ENTRY(HCI_LE_READ_ADV_CHANNEL_TX_POWER,              IDX_CAST IDX_HCI_LE_ReadAdvChanTxPowerCmd,                 HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
#endif // (defined(CTRL_CONFIG) && ((CTRL_CONFIG & ADV_NCONN_CFG) || (CTRL_CONFIG & ADV_CONN_CFG)))

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  HCI_TRANSLATION_ENTRY(HCI_DISCONNECT,                                IDX_CAST IDX_HCI_DisconnectCmd,                            HU16,    HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_READ_CHANNEL_MAP,                       IDX_CAST IDX_HCI_LE_ReadChannelMapCmd,                     HU16,    HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_READ_REMOTE_USED_FEATURES,              IDX_CAST IDX_HCI_LE_ReadRemoteUsedFeaturesCmd,             HU16,    HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_READ_REMOTE_VERSION_INFO,                  IDX_CAST IDX_HCI_ReadRemoteVersionInfoCmd,                 HU16,    HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_READ_TRANSMIT_POWER,                       IDX_CAST IDX_HCI_ReadTransmitPowerLevelCmd,                HU16,    HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_SET_CONTROLLER_TO_HOST_FLOW_CONTROL,       IDX_CAST IDX_HCI_SetControllerToHostFlowCtrlCmd,           HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_HOST_BUFFER_SIZE,                          IDX_CAST IDX_HCI_HostBufferSizeCmd,                        HU16,    HU8,     HU16,    HU16,    HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_HOST_NUM_COMPLETED_PACKETS,                IDX_CAST IDX_HCI_HostNumCompletedPktCmd,                   HU8LEN,  HU16PTR, HU16PTR, HNP,     HNP,     HNP,     HNP,     HNP),
#endif // (defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG)))

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (INIT_CFG))
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_HOST_CHANNEL_CLASSIFICATION,        IDX_CAST IDX_HCI_LE_SetHostChanClassificationCmd,          HU8PTR,  HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
#endif // (defined(CTRL_CONFIG) && (CTRL_CONFIG & (INIT_CFG)))
  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_HOST_DEFAULT_CHANNEL_CLASSIFICATION,    IDX_CAST IDX_HCI_EXT_SetHostDefChanClassificationCmd, HU8PTR,  HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_HOST_CONNECTION_CHANNEL_CLASSIFICATION, IDX_CAST IDX_HCI_EXT_SetHostConnChanClassificationCmd,HU8PTR,  HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),


  // V4.2 - Extended Data Length
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  HCI_TRANSLATION_ENTRY(HCI_LE_READ_MAX_DATA_LENGTH,                   IDX_CAST IDX_HCI_LE_ReadMaxDataLenCmd,                     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_DATA_LENGTH,                        IDX_CAST IDX_HCI_LE_SetDataLenCmd,                         HU16,    HU16,    HU16,    HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_WRITE_SUGGESTED_DEFAULT_DATA_LENGTH,    IDX_CAST IDX_HCI_LE_WriteSuggestedDefaultDataLenCmd,       HU16,    HU16,    HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_READ_SUGGESTED_DEFAULT_DATA_LENGTH,     IDX_CAST IDX_HCI_LE_ReadSuggestedDefaultDataLenCmd,        HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
#endif // defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))

  // V4.1
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  HCI_TRANSLATION_ENTRY(HCI_READ_AUTH_PAYLOAD_TIMEOUT,                 IDX_CAST IDX_HCI_ReadAuthPayloadTimeoutCmd,                HU16,    HU16PTR, HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_WRITE_AUTH_PAYLOAD_TIMEOUT,                IDX_CAST IDX_HCI_WriteAuthPayloadTimeoutCmd,               HU16,    HU16,    HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_REMOTE_CONN_PARAM_REQ_REPLY,            IDX_CAST IDX_HCI_LE_RemoteConnParamReqReplyCmd,            HU16,    HU16,    HU16,    HU16,    HU16,    HU16,    HU16,    HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_REMOTE_CONN_PARAM_REQ_NEG_REPLY,        IDX_CAST IDX_HCI_LE_RemoteConnParamReqNegReplyCmd,         HU16,    HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
#endif // (defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG)))

  // V4.2 - Privacy 1.2
  HCI_TRANSLATION_ENTRY(HCI_LE_ADD_DEVICE_TO_RESOLVING_LIST,           IDX_CAST IDX_HCI_LE_AddDeviceToResolvingListCmd,           HU8,     HAB,     HKB,     HKB,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_REMOVE_DEVICE_FROM_RESOLVING_LIST,      IDX_CAST IDX_HCI_LE_RemoveDeviceFromResolvingListCmd,      HU8,     HAB,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_CLEAR_RESOLVING_LIST,                   IDX_CAST IDX_HCI_LE_ClearResolvingListCmd,                 HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_READ_RESOLVING_LIST_SIZE,               IDX_CAST IDX_HCI_LE_ReadResolvingListSizeCmd,              HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_READ_PEER_RESOLVABLE_ADDRESS,           IDX_CAST IDX_HCI_LE_ReadPeerResolvableAddressCmd,          HU8,     HAB,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_READ_LOCAL_RESOLVABLE_ADDRESS,          IDX_CAST IDX_HCI_LE_ReadLocalResolvableAddressCmd,         HU8,     HAB,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_ADDRESS_RESOLUTION_ENABLE,          IDX_CAST IDX_HCI_LE_SetAddressResolutionEnableCmd,         HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_RESOLVABLE_PRIVATE_ADDRESS_TIMEOUT, IDX_CAST IDX_HCI_LE_SetResolvablePrivateAddressTimeoutCmd, HU16,    HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_PRIVACY_MODE,                       IDX_CAST IDX_HCI_LE_SetPrivacyModeCmd,                     HU8,     HAB,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP),

  // V4.2 - Secure Connections
  HCI_TRANSLATION_ENTRY(HCI_LE_READ_LOCAL_P256_PUBLIC_KEY,             IDX_CAST IDX_HCI_LE_ReadLocalP256PublicKeyCmd,             HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_GENERATE_DHKEY,                         IDX_CAST IDX_HCI_LE_GenerateDHKeyCmd,                      HU8PTR,  HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),

  // V5.0 - 2M and Coded PHY
#if defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG)) && defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  HCI_TRANSLATION_ENTRY(HCI_LE_READ_PHY,                               IDX_CAST IDX_HCI_LE_ReadPhyCmd,                            HU16,    HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_DEFAULT_PHY,                        IDX_CAST IDX_HCI_LE_SetDefaultPhyCmd,                      HU8,     HU8,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_PHY,                                IDX_CAST IDX_HCI_LE_SetPhyCmd,                             HU16,    HU8,     HU8,     HU8,     HU16,    HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_ENHANCED_RECEIVER_TEST,                 IDX_CAST IDX_HCI_LE_EnhancedRxTestCmd,                     HU8,     HU8,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_ENHANCED_TRANSMITTER_TEST,              IDX_CAST IDX_HCI_LE_EnhancedTxTestCmd,                     HU8,     HU8,     HU8,     HU8,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_ENHANCED_CTE_RECEIVER_TEST,             IDX_CAST IDX_HCI_LE_EnhancedCteRxTestCmd,                  HU8,     HU8,     HU8,     HU8,     HU8,     HU8,     HU8,     HU8PTR),
  HCI_TRANSLATION_ENTRY(HCI_LE_ENHANCED_CTE_TRANSMITTER_TEST,          IDX_CAST IDX_HCI_LE_EnhancedCteTxTestCmd,                  HU8,     HU8,     HU8,     HU8,     HU8,     HU8,     HU8,     HU8PTR),
#endif // (defined(CTRL_V50_CONFIG) && (CTRL_V50_CONFIG & (PHY_2MBPS_CFG | PHY_LR_CFG)) && defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG)))

  // Advertiser
#ifndef LEGACY_CMD
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_ADV_PARAM,                          IDX_CAST IDX_HCI_LE_SetAdvParamCmd,                        HU16,    HU16,    HU8,     HU8,     HU8,     HAB,     HU8,     HU8),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_ADV_DATA,                           IDX_CAST IDX_HCI_LE_SetAdvDataCmd,                         HU8,     HU8PTR,  HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_SCAN_RSP_DATA,                      IDX_CAST IDX_HCI_LE_SetScanRspDataCmd,                     HU8,     HU8PTR,  HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_ADV_ENABLE,                         IDX_CAST IDX_HCI_LE_SetAdvEnableCmd,                       HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
#endif // (defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG)))

  // Scanner
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_SCAN_PARAM,                         IDX_CAST IDX_HCI_LE_SetScanParamCmd,                       HU8,     HU16,    HU16,    HU8,     HU8,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_SCAN_ENABLE,                        IDX_CAST IDX_HCI_LE_SetScanEnableCmd,                      HU8,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
#endif // (defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG))
#endif // LEGACY_CMD

  // Initiator
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
#ifndef LEGACY_CMD
  HCI_EXTENDED_ENTRY(HCI_LE_CREATE_CONNECTION,                         IDX_CAST IDX_HCI_LE_CreateConnCmd,                         hci_tl_le_create_conn_params),
#endif // LEGACY_CMD
  HCI_TRANSLATION_ENTRY(HCI_LE_CREATE_CONNECTION_CANCEL,               IDX_CAST IDX_HCI_LE_CreateConnCancelCmd,                   HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_START_ENCRYPTION,                       IDX_CAST IDX_HCI_LE_StartEncyptCmd,                        HU16,    H8B,     H2B,     HKB,     HNP,     HNP,     HNP,     HNP),
#endif // (defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG))

  // Connection Updates
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  HCI_TRANSLATION_ENTRY(HCI_LE_CONNECTION_UPDATE,                      IDX_CAST IDX_HCI_LE_ConnUpdateCmd,                         HU16,    HU16,    HU16,    HU16,    HU16,    HU16,    HU16,    HNP),
#endif // (defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG)))

  // Security Responder
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)
  HCI_TRANSLATION_ENTRY(HCI_LE_LTK_REQ_REPLY,                          IDX_CAST IDX_HCI_LE_LtkReqReplyCmd,                        HU16,    HKB,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_LTK_REQ_NEG_REPLY,                      IDX_CAST IDX_HCI_LE_LtkReqNegReplyCmd,                     HU16,    HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
#endif // (defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG))

  HCI_TRANSLATION_ENTRY(HCI_LE_READ_TX_POWER,                          IDX_CAST IDX_HCI_LE_ReadTxPowerCmd,                        HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_READ_RF_PATH_COMPENSATION,              IDX_CAST IDX_HCI_LE_ReadRfPathCompCmd,                     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_WRITE_RF_PATH_COMPENSATION,             IDX_CAST IDX_HCI_LE_WriteRfPathCompCmd,                    HU16,    HU16,    HNP,     HNP,     HNP,     HNP,     HNP,     HNP),

  // CTE Connection commands  
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_CONNECTION_CTE_RECEIVE_PARAMS,      IDX_CAST IDX_HCI_LE_SetConnectionCteReceiveParamsCmd,      HU16,    HU8,     HU8,     HU8,     HU8PTR,  HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_CONNECTION_CTE_TRANSMIT_PARAMS,     IDX_CAST IDX_HCI_LE_SetConnectionCteTransmitParamsCmd,     HU16,    HU8,     HU8,     HU8PTR,  HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_CONNECTION_CTE_REQUEST_ENABLE,      IDX_CAST IDX_HCI_LE_SetConnectionCteRequestEnableCmd,      HU16,    HU8,     HU16,    HU8,     HU8,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_CONNECTION_CTE_RESPONSE_ENABLE,     IDX_CAST IDX_HCI_LE_SetConnectionCteResponseEnableCmd,     HU16,    HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_READ_ANTENNA_INFORMATION,               IDX_CAST IDX_HCI_LE_ReadAntennaInformationCmd,             HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_PERIODIC_ADV_PARAMETERS,            IDX_CAST IDX_HCI_LE_SetPeriodicAdvParamsCmd,               HU8,     HU16,    HU16,    HU16,    HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_PERIODIC_ADV_DATA,                  IDX_CAST IDX_HCI_LE_SetPeriodicAdvDataCmd,                 HU8,     HU8,     HU8,     HU8PTR,  HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_PERIODIC_ADV_ENABLE,                IDX_CAST IDX_HCI_LE_SetPeriodicAdvEnableCmd,               HU8,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_CONNECTIONLESS_CTE_TRANSMIT_PARAMS, IDX_CAST IDX_HCI_LE_SetConnectionlessCteTransmitParamsCmd, HU8,     HU8,     HU8,     HU8,     HU8,     HU8PTR,  HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_CONNECTIONLESS_CTE_TRANSMIT_ENABLE, IDX_CAST IDX_HCI_LE_SetConnectionlessCteTransmitEnableCmd, HU8,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
#endif
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
  HCI_TRANSLATION_ENTRY(HCI_LE_PERIODIC_ADV_CREATE_SYNC,               IDX_CAST IDX_HCI_LE_PeriodicAdvCreateSyncCmd,              HU8,     HU8,     HU8,     HAB,     HU16,    HU16,    HU8,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_PERIODIC_ADV_CREATE_SYNC_CANCEL,        IDX_CAST IDX_HCI_LE_PeriodicAdvCreateSyncCancelCmd,        HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_PERIODIC_ADV_TERMINATE_SYNC,            IDX_CAST IDX_HCI_LE_PeriodicAdvTerminateSyncCmd,           HU16,    HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_ADD_DEVICE_TO_PERIODIC_ADV_LIST,        IDX_CAST IDX_HCI_LE_AddDeviceToPeriodicAdvListCmd,         HU8,     HAB,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_REMOVE_DEVICE_FROM_PERIODIC_ADV_LIST,   IDX_CAST IDX_HCI_LE_RemoveDeviceFromPeriodicAdvListCmd,    HU8,     HAB,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_CLEAR_PERIODIC_ADV_LIST,                IDX_CAST IDX_HCI_LE_ClearPeriodicAdvListCmd,               HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_READ_PERIODIC_ADV_LIST_SIZE,            IDX_CAST IDX_HCI_LE_ReadPeriodicAdvListSizeCmd,            HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_PERIODIC_ADV_RECEIVE_ENABLE,        IDX_CAST IDX_HCI_LE_SetPeriodicAdvReceiveEnableCmd,        HU16,    HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_SET_CONNECTIONLESS_IQ_SAMPLING_ENABLE,  IDX_CAST IDX_HCI_LE_SetConnectionlessIqSamplingEnableCmd,  HU16,    HU8,     HU8,     HU8,     HU8,     HU8PTR,  HNP,     HNP),
#endif

  // Vendor Specific HCI Commands
  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_TX_POWER,                          IDX_CAST IDX_HCI_EXT_SetTxPowerCmd,                        HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_BUILD_REVISION,                        IDX_CAST IDX_HCI_EXT_BuildRevisionCmd,                     HU8,     HU16,    HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_DELAY_SLEEP,                           IDX_CAST IDX_HCI_EXT_DelaySleepCmd,                        HU16,    HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_DECRYPT,                               IDX_CAST IDX_HCI_EXT_DecryptCmd,                           HKB,     HKB,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_ENABLE_PTM,                            IDX_CAST IDX_HCI_EXT_EnablePTMCmd,                         HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_MODEM_TEST_TX,                         IDX_CAST IDX_HCI_EXT_ModemTestTxCmd,                       HU8,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_MODEM_HOP_TEST_TX,                     IDX_CAST IDX_HCI_EXT_ModemHopTestTxCmd,                    HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_MODEM_TEST_RX,                         IDX_CAST IDX_HCI_EXT_ModemTestRxCmd,                       HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_ENHANCED_MODEM_TEST_TX,                IDX_CAST IDX_HCI_EXT_EnhancedModemTestTxCmd,               HU8,     HU8,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_ENHANCED_MODEM_HOP_TEST_TX,            IDX_CAST IDX_HCI_EXT_EnhancedModemHopTestTxCmd,            HU8,     HU8,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_ENHANCED_MODEM_TEST_RX,                IDX_CAST IDX_HCI_EXT_EnhancedModemTestRxCmd,               HU8,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_END_MODEM_TEST,                        IDX_CAST IDX_HCI_EXT_EndModemTestCmd,                      HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_BDADDR,                            IDX_CAST IDX_HCI_EXT_SetBDADDRCmd,                         HAB,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_RESET_SYSTEM,                          IDX_CAST IDX_HCI_EXT_ResetSystemCmd,                       HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_LOCAL_SUPPORTED_FEATURES,          IDX_CAST IDX_HCI_EXT_SetLocalSupportedFeaturesCmd,         H8B,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_MAX_DTM_TX_POWER,                  IDX_CAST IDX_HCI_EXT_SetMaxDtmTxPowerCmd,                  HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_RX_GAIN,                           IDX_CAST IDX_HCI_EXT_SetRxGainCmd,                         HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_EXTEND_RF_RANGE,                       IDX_CAST IDX_HCI_EXT_ExtendRfRangeCmd,                     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_HALT_DURING_RF,                        IDX_CAST IDX_HCI_EXT_HaltDuringRfCmd,                      HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_CLK_DIVIDE_ON_HALT,                    IDX_CAST IDX_HCI_EXT_ClkDivOnHaltCmd,                      HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_DECLARE_NV_USAGE,                      IDX_CAST IDX_HCI_EXT_DeclareNvUsageCmd,                    HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_MAP_PM_IO_PORT,                        IDX_CAST IDX_HCI_EXT_MapPmIoPortCmd,                       HU8,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_FREQ_TUNE,                         IDX_CAST IDX_HCI_EXT_SetFreqTuneCmd,                       HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_SAVE_FREQ_TUNE,                        IDX_CAST IDX_HCI_EXT_SaveFreqTuneCmd,                      HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_QOS_PARAMETERS,                    IDX_CAST IDX_HCI_EXT_SetQOSParameters,                     HU8,     HU8,     HU32,    HU16,    HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_QOS_DEFAULT_PARAMETERS,            IDX_CAST IDX_HCI_EXT_SetQOSDefaultParameters,              HU32,    HU8,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP),
#ifndef HOST_CONFIG
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_NCONN_CFG )
  HCI_TRANSLATION_ENTRY(HCI_EXT_LE_SET_EXT_VIRTUAL_ADV_ADDRESS,        IDX_CAST IDX_HCI_EXT_SetVirtualAdvAddrCmd,                 HU8,     HAB,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
#endif
#endif

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  HCI_TRANSLATION_ENTRY(HCI_EXT_DISCONNECT_IMMED,                      IDX_CAST IDX_HCI_EXT_DisconnectImmedCmd,                   HU16,    HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_PER,                                   IDX_CAST IDX_HCI_EXT_PacketErrorRateCmd,                   HU16,    HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_NUM_COMPLETED_PKTS_LIMIT,              IDX_CAST IDX_HCI_EXT_NumComplPktsLimitCmd,                 HU8,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_ONE_PKT_PER_EVT,                       IDX_CAST IDX_HCI_EXT_OnePktPerEvtCmd,                      HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_SCA,                               IDX_CAST IDX_HCI_EXT_SetSCACmd,                            HU16,    HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_GET_CONNECTION_INFO,                   IDX_CAST IDX_HCI_EXT_GetConnInfoCmd,                       HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_GET_ACTIVE_CONNECTION_INFO,            IDX_CAST IDX_HCI_EXT_GetActiveConnInfoCmd,                 HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_OVERLAPPED_PROCESSING,                 IDX_CAST IDX_HCI_EXT_OverlappedProcessingCmd,              HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
#endif // (defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG)))

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & ADV_CONN_CFG)
  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_FAST_TX_RESP_TIME,                 IDX_CAST IDX_HCI_EXT_SetFastTxResponseTimeCmd,             HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_OVERRIDE_SL,                           IDX_CAST IDX_HCI_EXT_SetSlaveLatencyOverrideCmd,           HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
#endif // (defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG)))

  // Set max data len
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))
  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_MAX_DATA_LENGTH,                   IDX_CAST IDX_HCI_EXT_SetMaxDataLenCmd,                     HU16,    HU16,    HU16,    HU16,    HNP,     HNP,     HNP,     HNP),
#endif // defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_CONN_CFG | INIT_CFG))

  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_DTM_TX_PKT_CNT,                    IDX_CAST IDX_HCI_EXT_SetDtmTxPktCntCmd,                    HU16,    HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_PIN_OUTPUT,                        IDX_CAST IDX_HCI_EXT_SetPinOutputCmd,                      HU8,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_LOCATIONING_ACCURACY,              IDX_CAST IDX_HCI_EXT_SetLocationingAccuracyCmd,            HU16,    HU8,     HU8,     HU8,     HU8,     HU8,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_COEX_ENABLE,                           IDX_CAST IDX_HCI_EXT_CoexEnableCmd,                        HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),

  // LL Test Mode
#ifdef LL_TEST_MODE
  HCI_TRANSLATION_ENTRY(HCI_EXT_LL_TEST_MODE,                          IDX_CAST IDX_HCI_EXT_LLTestModeCmd,                        HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP)
#endif
};

#elif defined (PTM_MODE)
static hciEntry_t hciTranslationTable[] =
{
#ifndef LEGACY_CMD
  // Controller and Baseband Commands
  HCI_TRANSLATION_ENTRY(HCI_RESET,                                     IDX_CAST IDX_HCI_ResetCmd,                                 HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
#endif // LEGACY_CMD

  // LE Commands - Direct Test Mode
  HCI_TRANSLATION_ENTRY(HCI_LE_RECEIVER_TEST,                          IDX_CAST IDX_HCI_LE_ReceiverTestCmd,                       HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_TRANSMITTER_TEST,                       IDX_CAST IDX_HCI_LE_TransmitterTestCmd,                    HU8,     HU8,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_TEST_END,                               IDX_CAST IDX_HCI_LE_TestEndCmd,                            HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_MODEM_TEST_TX,                         IDX_CAST IDX_HCI_EXT_ModemTestTxCmd,                       HU8,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_MODEM_HOP_TEST_TX,                     IDX_CAST IDX_HCI_EXT_ModemHopTestTxCmd,                    HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_MODEM_TEST_RX,                         IDX_CAST IDX_HCI_EXT_ModemTestRxCmd,                       HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_ENHANCED_MODEM_TEST_TX,                IDX_CAST IDX_HCI_EXT_EnhancedModemTestTxCmd,               HU8,     HU8,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_ENHANCED_MODEM_HOP_TEST_TX,            IDX_CAST IDX_HCI_EXT_EnhancedModemHopTestTxCmd,            HU8,     HU8,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_ENHANCED_MODEM_TEST_RX,                IDX_CAST IDX_HCI_EXT_EnhancedModemTestRxCmd,               HU8,     HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_END_MODEM_TEST,                        IDX_CAST IDX_HCI_EXT_EndModemTestCmd,                      HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_DTM_TX_PKT_CNT,                    IDX_CAST IDX_HCI_EXT_SetDtmTxPktCntCmd,                    HU16,    HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),

  // LE Commands - General
  HCI_TRANSLATION_ENTRY(HCI_READ_BDADDR,                               IDX_CAST IDX_HCI_ReadBDADDRCmd,                            HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_BDADDR,                            IDX_CAST IDX_HCI_EXT_SetBDADDRCmd,                         HAB,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_TX_POWER,                          IDX_CAST IDX_HCI_EXT_SetTxPowerCmd,                        HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_MAX_DTM_TX_POWER,                  IDX_CAST IDX_HCI_EXT_SetMaxDtmTxPowerCmd,                  HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_EXTEND_RF_RANGE,                       IDX_CAST IDX_HCI_EXT_ExtendRfRangeCmd,                     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_HALT_DURING_RF,                        IDX_CAST IDX_HCI_EXT_HaltDuringRfCmd,                      HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_READ_TRANSMIT_POWER,                       IDX_CAST IDX_HCI_ReadTransmitPowerLevelCmd,                HU16,    HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_BUILD_REVISION,                        IDX_CAST IDX_HCI_EXT_BuildRevisionCmd,                     HU8,     HU16,    HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_READ_TX_POWER,                          IDX_CAST IDX_HCI_LE_ReadTxPowerCmd,                        HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_READ_RF_PATH_COMPENSATION,              IDX_CAST IDX_HCI_LE_ReadRfPathCompCmd,                     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_LE_WRITE_RF_PATH_COMPENSATION,             IDX_CAST IDX_HCI_LE_WriteRfPathCompCmd,                    HU16,    HU16,    HNP,     HNP,     HNP,     HNP,     HNP,     HNP),

  // LE Commands - Production Test Mode
  HCI_TRANSLATION_ENTRY(HCI_EXT_ENABLE_PTM,                            IDX_CAST IDX_HCI_EXT_EnablePTMCmd,                         HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_SET_FREQ_TUNE,                         IDX_CAST IDX_HCI_EXT_SetFreqTuneCmd,                       HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_SAVE_FREQ_TUNE,                        IDX_CAST IDX_HCI_EXT_SaveFreqTuneCmd,                      HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),
  HCI_TRANSLATION_ENTRY(HCI_EXT_RESET_SYSTEM,                          IDX_CAST IDX_HCI_EXT_ResetSystemCmd,                       HU8,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP,     HNP),

};

#else // !defined (PTM_MODE) or !defined (HCI_TL_FULL)
static hciEntry_t hciTranslationTable[] = { 0 };
#endif // defined (PTM_MODE) or defined (HCI_TL_FULL)

// Callback for overriding contents of serial buffer.
HCI_TL_ParameterOverwriteCB_t HCI_TL_ParameterOverwriteCB = NULL;

#if (defined(HCI_TL_FULL) || defined(PTM_MODE))  && defined(HOST_CONFIG)
static ICall_EntityID appTaskID;


#ifdef HCI_TL_FULL
// Outgoing response
static uint8_t rspBuf[MAX_RSP_BUF];

// Outgoing event
static uint8_t out_msg[HCI_EXT_APP_OUT_BUF];
uint8_t *pLongMsg;
#endif /* HCI_TL_FULL */

#endif // (HCI_TL_FULL || PTM_MODE)  && HOST_CONFIG

// Callback for sending command status.
static HCI_TL_CommandStatusCB_t HCI_TL_CommandStatusCB = NULL;

// Callback for posting callback event (switching from SWI to Thread).
static HCI_TL_CalllbackEvtProcessCB_t HCI_TL_CallbackEvtProcessCB = NULL;

#ifdef LEGACY_CMD
// Store the type of commmand that are used.
// Once it is set, it can only be change by reset HCI (HCI_RESET command)
uint8_t legacyCmdStatus = HCI_LEGACY_CMD_STATUS_UNDEFINED;
#endif // LEGACY_CMD

/*********************************************************************
 * LOCAL FUNCTIONS
 */
#if (defined(HCI_TL_FULL) || defined(PTM_MODE))
static void HCI_TL_SendCommandPkt(hciPacket_t *pMsg);
static void HCI_TL_SendDataPkt(uint8_t *pMsg);

//HOST
#ifdef HOST_CONFIG

#if !defined(PTM_MODE)
// PTM_MODE: No Host processing required (GAP, GATT, L2CAP, Etc...)

static void HCI_TL_SendExtensionCmdPkt(hciPacket_t *pMsg);
static uint8_t processExtMsg(hciPacket_t *pMsg);

#if (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
static uint8_t processExtMsgL2CAP(uint8_t cmdID, hciExtCmd_t *pCmd, uint8_t *pRspDataLen);
static uint8_t processExtMsgATT(uint8_t cmdID, hciExtCmd_t *pCmd);
static uint8_t processExtMsgGATT(uint8_t cmdID, hciExtCmd_t *pCmd, uint8_t *pRspDataLen);
#endif /* (CENTRAL_CFG | PERIPHERAL_CFG */

static uint8_t processExtMsgGAP(uint8_t cmdID, hciExtCmd_t *pCmd, uint8_t *pRspDataLen);
static uint8_t processExtMsgUTIL(uint8_t cmdID, hciExtCmd_t *pCmd, uint8_t *pRspDataLen);

#if (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
static uint8_t *createMsgPayload(uint8_t *pBuf, uint16_t len);
static uint8_t *createSignedMsgPayload(uint8_t sig, uint8_t cmd, uint8_t *pBuf, uint16_t len);
static uint8_t *createPayload(uint8_t *pBuf, uint16_t len, uint8_t sigLen);
static uint8_t mapATT2BLEStatus(uint8_t status);
static uint8_t buildHCIExtHeader(uint8_t *pBuf, uint16_t event, uint8_t status, uint16_t connHandle);
#endif /* (CENTRAL_CFG | PERIPHERAL_CFG */

static uint8_t checkNVLen(osalSnvId_t id, osalSnvLen_t len);

static uint8_t processEvents(ICall_Hdr *pMsg);
static uint8_t *processEventsGAP(gapEventHdr_t *pMsg, uint8_t *pOutMsg,
                               uint16_t *pMsgLen, uint8_t *pAllocated,
                               uint8_t *pDeallocate);
#if (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
static uint8_t *processEventsSM(smEventHdr_t *pMsg, uint8_t *pOutMsg,
                               uint8_t *pMsgLen, uint8_t *pAllocated);
static uint8_t *processEventsL2CAP(l2capSignalEvent_t *pPkt, uint8_t *pOutMsg,
                                  uint8_t *pMsgLen);
static uint8_t *processDataL2CAP(l2capDataEvent_t *pPkt, uint8_t *pOutMsg,
                                uint16_t *pMsgLen, uint8_t *pAllocated);
static uint8_t *processEventsGATT(gattMsgEvent_t *pPkt, uint8_t *pOutMsg,
                                 uint8_t *pMsgLen, uint8_t *pAllocated);
#if !defined(GATT_DB_OFF_CHIP)
static uint8_t *processEventsGATTServ(gattEventHdr_t *pPkt, uint8_t *pMsg,
                                     uint8_t *pMsgLen);
#endif /* GATT_DB_OFF_CHIP */

#endif /* (CENTRAL_CFG | PERIPHERAL_CFG) */

#if defined(BLE_V41_FEATURES) && (BLE_V41_FEATURES & L2CAP_COC_CFG)
static uint16_t l2capVerifySecCB(uint16_t connHandle, uint8_t id, l2capConnectReq_t *pReq);
static uint8_t buildCoChannelInfo(uint16_t CID, l2capCoCInfo_t *pInfo, uint8_t *pRspBuf);
#endif /* BLE_V41_FEATURES & L2CAP_COC_CFG */

#if defined(GATT_DB_OFF_CHIP)
static uint8_t addAttrRec(gattService_t *pServ, uint8_t *pUUID, uint8_t len,
                         uint8_t permissions, uint16_t *pTotalAttrs, uint8_t *pRspDataLen);
static const uint8_t *findUUIDRec(uint8_t *pUUID, uint8_t len);
static void freeAttrRecs(gattService_t *pServ);
#endif // GATT_DB_OFF_CHIP

#if ( HOST_CONFIG & ( PERIPHERAL_CFG | CENTRAL_CFG ) )
#if defined(GAP_BOND_MGR)
// Passcode.
static void host_tl_passcodeCB(uint8_t *deviceAddr, uint16_t connectionHandle,
                               uint8_t uiInputs, uint8_t uiOutputs,
                               uint32_t numComparison);
static void host_tl_passcodeCBProcess(passcodeEvtCallback_t *pData);
#endif //GAP_BOND_MGR
#endif // PERIPHERAL_CFG | CENTRAL_CFG

#endif /* (PTM_MODE) */

#endif /* HOST_CONFIG */

#endif /* (defined(HCI_TL_FULL) || defined(PTM_MODE) ) */

#if (defined(HCI_TL_FULL) || defined(PTM_MODE))
static void HCI_TL_SendVSEvent(uint8_t *pBuf, uint16_t dataLen);
static void HCI_TL_getMemStats(uint8_t mask, uint8_t* pData, uint8_t *len);

#ifdef HOST_CONFIG

#ifndef PTM_MODE
#if ( HOST_CONFIG & ( PERIPHERAL_CFG | BROADCASTER_CFG ) )
static void      host_tl_advEvtCallback(uint32_t event, void *pData, uintptr_t arg);
static void      host_tl_advEvtCallbackProcess(advEvtCallback_t *advEvtCallback);
#endif // PERIPHERAL_CFG | BROADCASTER_CFG

#if ( HOST_CONFIG & ( CENTRAL_CFG | OBSERVER_CFG ) )
static void      host_tl_scanEvtCallback(uint32_t event, void *pData, uintptr_t arg);
static void      host_tl_scanEvtCallbackProcess(scanEvtCallback_t * scanEvtCallback);
static void      host_tl_sendAdvReport(uint32_t event, GapScan_Evt_AdvRpt_t * advRpt);
#endif // CENTRAL_CFG | OBSERVER_CFG

#if ( HOST_CONFIG & ( PERIPHERAL_CFG | CENTRAL_CFG ) )
static void      host_tl_connEvtCallback(Gap_ConnEventRpt_t *pReport);
static void      host_tl_connEvtCallbackProcess(Gap_ConnEventRpt_t *pReport);
#endif // PERIPHERAL_CFG | CENTRAL_CF
#endif /* (PTM_MODE) */

#else //!HOST_CONFIG

static void processExtraHCICmd(uint16_t cmdOpCode, uint8_t *param);

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
static hci_tl_advSet_t *hci_tl_GetAdvSet(uint8_t handle);
static void      hci_tl_RemoveAdvSet(uint8_t handle);
static void      hci_tl_ClearAdvSet(void);
static void      hci_tl_removePendingData(uint8_t* pendingAdvData);
static uint8_t*  hci_tl_createPendingData(uint8_t *pNewData, uint16_t len, uint8_t* pData);
static void      hci_tl_managedAEdata(uint16_t mode, aeSetDataCmd_t * pCmddata, uint8_t *pData);
static uint8_t*  hci_tl_appendPendingData(uint8_t *pStorage, uint16_t lenStorage,
                                          uint8_t* pData, int16_t len);
static void      hci_tl_aeAdvCback(uint8_t event, void *pData);
static void      hci_tl_aeAdvCbackSendEvent(uint8_t eventId, uint8_t  handle);
static void      hci_tl_aeAdvCbackProcess(hci_tl_AdvEvtCallback_t*  evtCallback);
#endif // ADV_NCONN_CFG || CTRL_CONFIG & ADV_CONN_CFG

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
static void      hci_tl_aeScanCback(uint8_t event, aeExtAdvRptEvt_t *extAdvRpt);
static void      hci_tl_aeScanEventCallbackProcess(hci_tl_ScanEvtCallback_t *extAdvRpt);
#ifdef LEGACY_CMD
static void      hci_tl_legacyScanCback(uint8_t event, aeExtAdvRptEvt_t *extAdvRpt);
static void      hci_tl_legacyScanEventCallbackProcess(hci_tl_ScanEvtCallback_t *extAdvRpt);
#endif // LEGACY_CMD
#endif // SCAN_CFG

#endif //HOST_CONFIG


#endif /* (defined(HCI_TL_FULL) || defined(PTM_MODE)) */

/*********************************************************************
 * @fn      HCI_TL_Init
 *
 * @brief   Initialize HCI TL.
 *
 * @param   overwriteCB     - callback used to allow custom override the contents of the
 *                            serial buffer.
 *          csCB            - Callback to process command status
 *          evtCB           - Callback to post event related to Callback from LL
 *          taskID - Task ID of task to process and forward messages to the TL.
 *
 * @return  none.
 */
void HCI_TL_Init(HCI_TL_ParameterOverwriteCB_t overwriteCB,
                 HCI_TL_CommandStatusCB_t csCB,
                 HCI_TL_CalllbackEvtProcessCB_t evtCB,
                 ICall_EntityID taskID)
{
  HCI_TL_ParameterOverwriteCB = overwriteCB;

  HCI_TL_CommandStatusCB = csCB;
  (void) HCI_TL_CommandStatusCB;

  HCI_TL_CallbackEvtProcessCB = evtCB;
  (void) HCI_TL_CallbackEvtProcessCB;

#if (defined(HCI_TL_FULL) || defined(PTM_MODE))  && defined(HOST_CONFIG)
  appTaskID = taskID;
#endif

#if ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ) )
#if (defined(HCI_TL_FULL) || defined(PTM_MODE)) && defined(HOST_CONFIG)

  // Register for GATT local events and ATT Responses pending for transmission
  GATT_RegisterForMsgs(appTaskID);

  // Register to receive incoming ATT Indications/Notifications
  GATT_RegisterForInd(appTaskID);

  #if !defined(GATT_DB_OFF_CHIP)
    // Register with GATT Server App for event messages
    GATTServApp_RegisterForMsg(appTaskID);
  #else
    // Register with GATT Server for GATT messages
    GATT_RegisterForReq(appTaskID);
  #endif // GATT_DB_OFF_CHIP
#endif // (HCI_TL_FULL || PTM_MODE) && HOST_CONFIG
#endif // (CENTRAL_CFG | PERIPHERAL_CFG)
}


#if (defined(HCI_TL_FULL) || defined(PTM_MODE))
/*********************************************************************
 * @fn      HCI_TL_sendSystemReport
 *
 * @brief   Used to return specific system error over UART.
 *
 * @param   id - id of the module reporting the error.
 *          status - type of error
 *          info -   more information linked to the error or the module
 * @return  none.
 */
void HCI_TL_sendSystemReport(uint8_t id, uint8_t status, uint16_t info)
{
    // Unsupported Event for now...
    uint8_t data[6];
    data[0] = LO_UINT16(HCI_EXT_UTIL_SYSTEM_ERROR);
    data[1] = HI_UINT16(HCI_EXT_UTIL_SYSTEM_ERROR);
    data[2] = status;          //Status
    data[3] = id;  //Module
    data[4] = LO_UINT16(info);  //Event Generating the error
    data[5] = HI_UINT16(info);
    HCI_TL_SendVSEvent(data, sizeof(data));
}

/*********************************************************************
 * @fn      HCI_TL_sendMemoryReport
 *
 * @brief   Used to return memory usage statistic.
 *
 * @param   reportMask - bitfield of the different memory stat to return.
 * @return  none.
 */
void HCI_TL_sendMemoryReport(uint8_t reportMask)
{
    // Unsupported Event for now...
    uint8_t data[30];
    uint8_t len;
    //Gather Memory Stat Buffer
    data[0] = LO_UINT16(HCI_EXT_UTIL_MEM_STATS_EVENT);
    data[1] = HI_UINT16(HCI_EXT_UTIL_MEM_STATS_EVENT);
    data[2] = 0;                //Status
    data[3] = HOST_TL_ID;       //Module
    HCI_TL_getMemStats((reportMask & 0x3), &data[4], &len);
    HCI_TL_SendVSEvent(data, len+4);
}

/*********************************************************************
 * @fn      HCI_TL_getMemStats
 *
 * @brief   fill a buffer with memory statistic
 *
 * @param   mask - bitfield of the different memory stat to return.
 *          rspBuf: buffer where the information will be stored, must be allocated by the caller
 *          len:    length of the filled data: must be allcoated by the caller.
 * @return  none.
 */
static void HCI_TL_getMemStats(uint8_t memStatCmd, uint8_t* rspBuf, uint8_t *len)
{
    uint8_t rspIndex = 0;

    rspBuf[rspIndex++] = 0;
    if (memStatCmd & GET_HEAP_STATS_LIGHT)
    {
        ICall_heapStats_t stats;
        ICall_getHeapStats(&stats);
        rspBuf[0] |= GET_HEAP_STATS_LIGHT;
        rspBuf[rspIndex++]   = GET_HEAP_STATS_LIGHT;
        //  Heap Size
        rspBuf[rspIndex++] = BREAK_UINT32(stats.totalSize, 0);
        rspBuf[rspIndex++] = BREAK_UINT32(stats.totalSize, 1);
        rspBuf[rspIndex++] = BREAK_UINT32(stats.totalSize, 2);
        rspBuf[rspIndex++] = BREAK_UINT32(stats.totalSize, 3);

        // Heap size free
        rspBuf[rspIndex++] = BREAK_UINT32(stats.totalFreeSize, 0);
        rspBuf[rspIndex++] = BREAK_UINT32(stats.totalFreeSize, 1);
        rspBuf[rspIndex++] = BREAK_UINT32(stats.totalFreeSize, 2);
        rspBuf[rspIndex++] = BREAK_UINT32(stats.totalFreeSize, 3);

        // Biggest free buffer available
        rspBuf[rspIndex++]  = BREAK_UINT32(stats.largestFreeSize, 0);
        rspBuf[rspIndex++] = BREAK_UINT32(stats.largestFreeSize, 1);
        rspBuf[rspIndex++] = BREAK_UINT32(stats.largestFreeSize, 2);
        rspBuf[rspIndex++] = BREAK_UINT32(stats.largestFreeSize, 3);
    }
#ifdef HEAPMGR_METRICS
    if (memStatCmd & GET_HEAP_STATS_FULL)
    {
        rspBuf[0] |= GET_HEAP_STATS_FULL;
        rspBuf[rspIndex++]   = GET_HEAP_STATS_FULL;
        // return number of memory allocation failure
        extern uint16_t heapmgrMemFail;
        rspBuf[rspIndex++] = BREAK_UINT32(heapmgrMemFail, 0);
        rspBuf[rspIndex++] = BREAK_UINT32(heapmgrMemFail, 1);
        // return current number of heap allocated
        extern uint32_t heapmgrMemAlo;
        rspBuf[rspIndex++] = BREAK_UINT32(heapmgrMemAlo, 0);
        rspBuf[rspIndex++] = BREAK_UINT32(heapmgrMemAlo, 1);
        rspBuf[rspIndex++] = BREAK_UINT32(heapmgrMemAlo, 2);
        rspBuf[rspIndex++] = BREAK_UINT32(heapmgrMemAlo, 3);

        // Return max allocated heap size.
        extern uint32_t heapmgrMemMax;
        rspBuf[rspIndex++]  = BREAK_UINT32(heapmgrMemMax, 0);
        rspBuf[rspIndex++]  = BREAK_UINT32(heapmgrMemMax, 1);
        rspBuf[rspIndex++]  = BREAK_UINT32(heapmgrMemMax, 2);
        rspBuf[rspIndex++] =  BREAK_UINT32(heapmgrMemMax, 3);
    }
    if (memStatCmd & GET_THREAD_STATS)
    {
        uint8_t staticThreadNb = Task_Object_count(); // + 1 for the idle task.
        Task_Stat stats;
        uint8_t nbThoffset;
        Task_Handle tempHandle, tempHandle2;
        tempHandle = Task_Object_first();

        rspBuf[0] |= GET_THREAD_STATS;
        rspBuf[rspIndex++]   = GET_THREAD_STATS;
        // Number of Thread will be contains in the second  byte, set it to 0
        nbThoffset = rspIndex;
        rspBuf[rspIndex++] = 0;

        do
        {
            if(staticThreadNb)
            {
                //Go through all static thread first
                //Get Stats for this thread.
                if (rspIndex <= ( MAX_RSP_BUF - 8))
                {
                    tempHandle2 = Task_Object_get(NULL, staticThreadNb-1);
                    if (tempHandle2)
                    {
                        Task_stat(tempHandle2, &stats);
                        rspBuf[rspIndex++] = stats.priority;
                        rspBuf[rspIndex++] = BREAK_UINT32((uint32_t)tempHandle2, 0);
                        rspBuf[rspIndex++] = BREAK_UINT32((uint32_t)tempHandle2, 1);
                        rspBuf[rspIndex++] = BREAK_UINT32(stats.used, 0);
                        rspBuf[rspIndex++] = BREAK_UINT32(stats.used, 1);
                        rspBuf[rspIndex++] = BREAK_UINT32(stats.stackSize, 0);
                        rspBuf[rspIndex++] = BREAK_UINT32(stats.stackSize, 1);
                        rspBuf[nbThoffset]++;
                    }
                }
                else
                {
                    break;
                }
                staticThreadNb--;
                continue;
            }

            // no more static thread, continue with dynamic thread.
            if ((tempHandle) && (rspIndex <= ( MAX_RSP_BUF - 8)))
            {
                Task_stat(tempHandle, &stats);
                rspBuf[rspIndex++] = stats.priority;
                rspBuf[rspIndex++] = BREAK_UINT32((uint32_t)tempHandle, 0);
                rspBuf[rspIndex++] = BREAK_UINT32((uint32_t)tempHandle, 1);
                rspBuf[rspIndex++] = BREAK_UINT32(stats.used, 0);
                rspBuf[rspIndex++] = BREAK_UINT32(stats.used, 1);
                rspBuf[rspIndex++] = BREAK_UINT32(stats.stackSize, 0);
                rspBuf[rspIndex++] = BREAK_UINT32(stats.stackSize, 1);
                rspBuf[nbThoffset]++;
                tempHandle = Task_Object_next(tempHandle);
            }
            else
            {
                break;
            }
        }while(1);
    }
#endif
    *len = rspIndex;
}
#endif /* (defined(HCI_TL_FULL) || defined(PTM_MODE)) */

/*********************************************************************
 * @fn      HCI_TL_compareAppLastOpcodeSent
 *
 * @brief   check if the opcode of an event received matches the last
 *          opcode of an HCI command called from the embedded application.
 *
 * @param   opcode - opcode of the received Stack event.
 *
 * @return  TRUE if opcode matches, FALSE otherwise.
 */
uint8_t HCI_TL_compareAppLastOpcodeSent(uint16_t opcode)
{
  uint8_t isMatch = FALSE;

  // See if the opcode received matches the ICall LITE ID last sent from the stack.
  // Last last ICall ID sent from the stack should be invalid if it was already matched.

  // If the application is looking for an event
  if (lastAppOpcodeIdxSent != 0xFFFFFFFF)
  {
    // Iterate over the hci table.
    for (uint16_t idx = 0;
         idx < sizeof(hciTranslationTable)/sizeof(hciEntry_t);
         idx++)
    {
      // If an entry matching the opcode event is found and the ICall Lite ID matches that of the last we sent
      if (hciTranslationTable[idx].opcode == opcode &&
          GET_CMD_IDX(hciTranslationTable[idx].cmdIdx) == lastAppOpcodeIdxSent)
      {
        // A match is found.
        isMatch = TRUE;
        break;
      }
    }
  }

  return(isMatch);
}

/*********************************************************************
 * @fn      HCI_TL_SendToStack
 *
 * @brief   Translate serial buffer into it's corresponding function and
 *          parameterize the arguments to send to the Stack.
 *
 * @param   msgToParse - pointer to a serialized HCI command or data packet.
 *
 * @return  none.
 */
void HCI_TL_SendToStack(uint8_t *msgToParse)
{
#if (defined(HCI_TL_FULL) || defined(PTM_MODE))
  hciPacket_t *pMsg;

  pMsg = (hciPacket_t *)msgToParse;

  // if there is a host configuration
#ifdef HOST_CONFIG
#if !defined (PTM_MODE)
  if (pMsg->hdr.event == HCI_EXT_CMD_EVENT)
  {
    HCI_TL_SendExtensionCmdPkt(pMsg);
  }
  else
#endif /* defined (PTM_MODE) */
#endif /* HOST_CONFIG */
  // What type of packet is this?
  if (pMsg->hdr.event == HCI_HOST_TO_CTRL_CMD_EVENT)
  {
    // Command.  Interpret the command and send to Host.
    HCI_TL_SendCommandPkt(pMsg);
  }
  else if (pMsg->hdr.event == HCI_HOST_TO_CTRL_DATA_EVENT)
  {
    HCI_TL_SendDataPkt(msgToParse);
  }
#endif /* (defined(HCI_TL_FULL) || defined(PTM_MODE)) */
}

#if (defined(HCI_TL_FULL) || defined(PTM_MODE))
/*********************************************************************
 * @fn      HCI_TL_SendCommandPkt
 *
 * @brief   Interprets an HCI command packet based off the translation table and
 *          calls the corresponding BLE API with the arguments provided.
 *
 * @param   pMsg - HCI Command serial packet to translate.
 *
 * @return  none.
 */
static void HCI_TL_SendCommandPkt(hciPacket_t *pMsg)
{
  uint16_t cmdOpCode;
  uint8_t  packetType;
  uint32_t  i = 0;
  uint8_t *param; // pointer into HCI command parameters.

  // retrieve packetType
  packetType = pMsg->pData[0];
  // retrieve opcode
  cmdOpCode = BUILD_UINT16(pMsg->pData[1], pMsg->pData[2]);

  // retrieve pointer to parameter
  if (packetType == HCI_EXTENDED_CMD_PACKET)
   {
       param = &pMsg->pData[5];
   }
   else
   {
       param = &pMsg->pData[4];
   }

  // If this is a command which the embedded "Host" must modify the parameters
  // to handle properly (e.g. taskEvent, taskID), it is handled here.
  if (HCI_TL_ParameterOverwriteCB)
  {
    HCI_TL_ParameterOverwriteCB(pMsg->pData);
  }

  // Loop over table to find a matching opcode
  for(; i < sizeof(hciTranslationTable) / sizeof(hciEntry_t) ; i++)
  {
    // Search for opcode that matches cmdOpCode
    if (hciTranslationTable[i].opcode == cmdOpCode)
    {
      // Found it.
      break;
    }
  }

  // If we found a matching opcode
  if (i != sizeof(hciTranslationTable) / sizeof(hciEntry_t))
  {
    uint8_t numRules;
    uint8_t *pRules;
    hci_arg_t args[HCI_MAX_NUM_ARGS] = {0,};
    uint8_t bufCount = 1;

    // Determine where the rules are and number of rules
#ifdef STACK_LIBRARY
    if (hciTranslationTable[i].extFlag)
#else
    if (IS_EXTENDED_PARAMS(hciTranslationTable[i].cmdIdx))
#endif
    {
      pRules = ((uint8_t *)(hciTranslationTable[i].paramRules)) + 1;
      numRules = *(pRules - 1);
    }
    else
    {
      pRules = (uint8_t *)(&hciTranslationTable[i].paramRules);
      numRules = HCI_DEFAULT_NUM_RULES;
    }

    // Parse the arguments according to the rules of the matching opcode
    for (uint8_t arg_i = 0; arg_i < numRules; arg_i++)
    {
      // Get this parameter's rule.
      uint8_t rule  = GET_RULE(pRules, arg_i);

      // If this a buffer counter
      if (rule & HLEN)
      {
        // What type of buffer counter is this?
        switch(rule)
        {
          // Check implicit counts.
          case H2B:
            bufCount = 2;
            break;

          case HAB:
            bufCount = 6;
            break;

          case H8B:
            bufCount = 8;
            break;

          case HKB:
            bufCount = 16;
            break;

          // Default to explicit count.
          default:
            bufCount = *param;
            break;
        }
      }

      // If this is a pointer
      if (rule & HPTR)
      {
        // Note that a pointer means the argument is the address of the byte at
        // offset 0 from param.

        // Store the address.
        args[arg_i] = (hci_arg_t)param;

        // Because the argument is stored in the buffer, param is moved forward
        // by the byte width of the argument stored there.
        switch (rule & HPTRMASK)
        {
          case HU32:
            param += 4 * bufCount;
            break;

          case HU16:
            param += 2 * bufCount;
            break;

          case HU8:
            param += 1 * bufCount;
            break;

          // Implicit buffer counters fall into here.
          default:
            param += bufCount;
            break;
        }
      }
      else if (rule == HU32)
      {
        // 32 bit value
        args[arg_i] = BUILD_UINT32(param[0], param[1], param[2], param[3]);
        param += 4;
      }
      else if (rule == HU16)
      {
        // 16 bit value
        args[arg_i] = BUILD_UINT16(param[0], param[1]);
        param += 2;
      }
      else if (rule == HU8)
      {
        // 8 bit value
        args[arg_i] = *param++;
      }
      else if (rule == HNP)
      {
        // No parameters remaining.
        break;
      }
    }

    // Call the function using the translated parameters.
    icall_directAPI(ICALL_SERVICE_CLASS_BLE,
                    (uint32_t) GET_CMD_IDX(hciTranslationTable[i].cmdIdx),
                    args[0],
                    args[1],
                    args[2],
                    args[3],
                    args[4],
                    args[5],
                    args[6],
                    args[7],
                    args[8],
                    args[9],
                    args[10],
                    args[11]);
  }
#ifdef BLE3_CMD
  else
  {
    // adv and scan notice hci command doesn't return command complete or command status event.
    if( cmdOpCode == HCI_EXT_ADV_EVENT_NOTICE )
    {
      advNotice = 1;
    }
    else if( cmdOpCode == HCI_EXT_SCAN_EVENT_NOTICE )
    {
      scanNotice = 1;
    }
  }
#endif // BLE3_CMD
#ifndef HOST_CONFIG
  else
  {
      // Some HCI command are not send directly to the Controller and are
      // instead interpreted in the application
      processExtraHCICmd(cmdOpCode, param);
  }
#endif //!HOST_CONFIG
}

#ifndef HOST_CONFIG
/*********************************************************************
 * @fn      checkLegacyHCICmdStatus
 *
 * @brief   Check and set the legacy comand operation mode.
 *          if the mode is already set to a different mode, return failure (1)
 *          if the mode is not set, set it and return success (0)
 *          if the same mode is already set, return success (0)
 * @param   mode - operation mode to check and set
 *
 * @return  0: SUCCESS,
 *          1: failure, wrong mode, the command should be rejected.
 */
static uint8_t checkLegacyHCICmdStatus(uint8_t mode)
{
#ifdef LEGACY_CMD
    if (legacyCmdStatus == HCI_LEGACY_CMD_STATUS_UNDEFINED)
    {
      legacyCmdStatus = mode;
      return(0);
    }
    else
    {
      return(legacyCmdStatus != mode);
    }
#else
    return(0);
#endif // LEGACY_CMD
}
/*********************************************************************
 * @fn      processExtraHCICmd
 *
 * @brief   Process all HCI command that was not found in the predefined table
 *
 * @param   cmdOpCode - operation code of the command to process
 *          param  -    pointer to the parameter of the command
 * @return  none
 */
static void processExtraHCICmd(uint16_t cmdOpCode, uint8_t *param)
{

    switch(cmdOpCode)
    {
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
        case HCI_LE_SET_ADV_SET_RANDOM_ADDRESS:
        {
            llStatus_t status;
            hci_tl_advSet_t *pAdvSet;

            pAdvSet = hci_tl_GetAdvSet(param[0]);
            if (pAdvSet)
            {
                pAdvSet->setAddrCmd = *((aeRandAddrCmd_t *)param);

                status = LE_SetAdvSetRandAddr(&pAdvSet->setAddrCmd);

                if (status == LL_STATUS_ERROR_UNKNOWN_ADVERTISING_IDENTIFIER)
                {
                    hci_tl_RemoveAdvSet(param[0]);
                }
            }
            else
            {
                status = LL_STATUS_ERROR_OUT_OF_HEAP;
            }

            HCI_CommandCompleteEvent(cmdOpCode,
                                      sizeof(status),
                                      &status);
            break;
        }
#endif //ADV_NCONN_CFG + ADV_CONN_CFG
#ifdef LEGACY_CMD
        case HCI_RESET:
        {
            // Since HCI_ResetCmd is doing more than just
            // calling a LL function, it is being called directly.
            // The status will be returned directly through a
            // HCI_CommandCompleteEvent call performed by HCI_ResetCmd().
            legacyCmdStatus = HCI_LEGACY_CMD_STATUS_UNDEFINED;
            hci_tl_ClearAdvSet();
            HCI_ResetCmd();
            return;
        }

//Advertiser
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
        case HCI_LE_SET_ADV_PARAM:
        {
            if(checkLegacyHCICmdStatus(HCI_LEGACY_CMD_STATUS_BT4))
            {
                HCI_CommandStatusEvent(LL_STATUS_ERROR_COMMAND_DISALLOWED,
                                       cmdOpCode );
                return;
            }
            // Translate this legacy API to a call to the new AE API.

            // This information needs to be saved on behalf of the LL.
            // A linked List will be created to save this information.
            llStatus_t status;
            hci_tl_advSet_t *pAdvSet;
            aeSetParamRtn_t retParams;

            // 1st check if there is not already an existing adv set with the same handle.
            pAdvSet = hci_tl_GetAdvSet(ADV_LEGACY_SET_HANDLE);
            if (pAdvSet)
            {
                // If set already exist, the parameters will be updated,
                // even if the call to  LE_SetExtAdvParams fails
                pAdvSet->advCmdParams.handle = ADV_LEGACY_SET_HANDLE;
                switch (param[4])
                {
                  case LL_ADV_CONNECTABLE_UNDIRECTED_EVT :
                    pAdvSet->advCmdParams.eventProps[0]   = LO_UINT16(AE_EVT_TYPE_CONN_SCAN_ADV_IND);
                    pAdvSet->advCmdParams.eventProps[1]   = HI_UINT16(AE_EVT_TYPE_CONN_SCAN_ADV_IND);
                    break;
                  case LL_ADV_CONNECTABLE_HDC_DIRECTED_EVT :
                    pAdvSet->advCmdParams.eventProps[0]   = LO_UINT16(AE_EVT_TYPE_CONN_DIR_HDC_ADV_DIRECT_IND);
                    pAdvSet->advCmdParams.eventProps[1]   = HI_UINT16(AE_EVT_TYPE_CONN_DIR_HDC_ADV_DIRECT_IND);
                    break;
                  case LL_ADV_SCANNABLE_UNDIRECTED_EVT :
                    pAdvSet->advCmdParams.eventProps[0]   = LO_UINT16(AE_EVT_TYPE_SCAN_UNDIR_ADV_SCAN_IND);
                    pAdvSet->advCmdParams.eventProps[1]   = HI_UINT16(AE_EVT_TYPE_SCAN_UNDIR_ADV_SCAN_IND);
                    break;
                  case LL_ADV_NONCONNECTABLE_UNDIRECTED_EVT :
                    pAdvSet->advCmdParams.eventProps[0]   = LO_UINT16(AE_EVT_TYPE_NONCONN_NONSCAN_ADV_NONCONN_IND);
                    pAdvSet->advCmdParams.eventProps[1]   = HI_UINT16(AE_EVT_TYPE_NONCONN_NONSCAN_ADV_NONCONN_IND);
                    break;
                  case LL_ADV_CONNECTABLE_LDC_DIRECTED_EVT :
                    pAdvSet->advCmdParams.eventProps[0]   = LO_UINT16(AE_EVT_TYPE_CONN_DIR_LDC_ADV_DIRECT_IND);
                    pAdvSet->advCmdParams.eventProps[1]   = HI_UINT16(AE_EVT_TYPE_CONN_DIR_LDC_ADV_DIRECT_IND);
                    break;
                  default:
                    status = LL_STATUS_ERROR_BAD_PARAMETER;
                    HCI_CommandCompleteEvent(cmdOpCode,
                                             sizeof(status),
                                             &status);
                    return;
                }

                pAdvSet->advCmdParams.primIntMin[0]   = param[0];
                pAdvSet->advCmdParams.primIntMin[1]   = param[1];
                pAdvSet->advCmdParams.primIntMin[2]   = 0;
                pAdvSet->advCmdParams.primIntMax[0]   = param[2];
                pAdvSet->advCmdParams.primIntMax[1]   = param[3];
                pAdvSet->advCmdParams.primIntMax[2]   = 0;
                pAdvSet->advCmdParams.primChanMap     = param[13];
                pAdvSet->advCmdParams.ownAddrType     = param[5];
                pAdvSet->advCmdParams.peerAddrType    = param[6];
                memcpy(pAdvSet->advCmdParams.peerAddr, &param[7], B_ADDR_LEN) ;
                pAdvSet->advCmdParams.filterPolicy    = param[14];
                pAdvSet->advCmdParams.txPower         = 127;            // Forced
                pAdvSet->advCmdParams.primPhy         = AE_PHY_1_MBPS;  // Forced to 1mbps
                pAdvSet->advCmdParams.secMaxSkip      = 0;              // N/A
                pAdvSet->advCmdParams.secPhy          = AE_PHY_1_MBPS;  // N/A
                pAdvSet->advCmdParams.sid             = 0;              // N/A
                pAdvSet->advCmdParams.notifyEnableFlags = 0;         // N/A

                status = LE_SetExtAdvParams(&pAdvSet->advCmdParams, &retParams);
                if (status != LL_STATUS_SUCCESS)
                {
                    hci_tl_RemoveAdvSet(ADV_LEGACY_SET_HANDLE);
                }
            }
            else
            {
                status = LL_STATUS_ERROR_OUT_OF_HEAP;
            }
            HCI_CommandCompleteEvent(cmdOpCode,
                                     sizeof(status),
                                     &status);
            break;
        }
        case HCI_LE_SET_ADV_DATA:
        {
            if(checkLegacyHCICmdStatus(HCI_LEGACY_CMD_STATUS_BT4))
            {
                HCI_CommandStatusEvent(LL_STATUS_ERROR_COMMAND_DISALLOWED,
                                       cmdOpCode );
                return;
            }
            // Translate this legacy API to a call to the new AE API.
            hci_tl_advSet_t *pAdvSet;
            llStatus_t status;

            if ( param[0] > LL_MAX_ADV_DATA_LEN )
            {
                status = LL_STATUS_ERROR_BAD_PARAMETER;
                HCI_CommandCompleteEvent(cmdOpCode,
                                         sizeof(status),
                                         &status);
                return;
            }
            // 1st check if there is not already an existing adv set with the same handle.
            pAdvSet = hci_tl_GetAdvSet(ADV_LEGACY_SET_HANDLE);

            if (pAdvSet)
            {
               // If set already exist, the parameters will be updated,
               // even if the call to  LE_SetExtAdvParams fails
               pAdvSet->advCmdData.handle    = ADV_LEGACY_SET_HANDLE;  //Forced
               pAdvSet->advCmdData.operation = AE_DATA_OP_COMPLETE;  //Forced
               pAdvSet->advCmdData.fragPref  = 0;                    //Forced
               pAdvSet->advCmdData.dataLen   = param[0];

               hci_tl_managedAEdata(cmdOpCode , &pAdvSet->advCmdData, &param[1]);
            }
            else
            {
                status = LL_STATUS_ERROR_OUT_OF_HEAP;
                HCI_CommandCompleteEvent(cmdOpCode,
                                         sizeof(status),
                                         &status);
            }
            break;
        }
        case HCI_LE_SET_SCAN_RSP_DATA:
        {
            if(checkLegacyHCICmdStatus(HCI_LEGACY_CMD_STATUS_BT4))
            {
                HCI_CommandStatusEvent( LL_STATUS_ERROR_COMMAND_DISALLOWED,
                                        cmdOpCode );
                return;
            }
            // Translate this legacy to a call to the new AE API.
            hci_tl_advSet_t *pAdvSet;
            llStatus_t status;
            if ( param[0] > LL_MAX_ADV_DATA_LEN )
            {
                status = LL_STATUS_ERROR_BAD_PARAMETER;
                HCI_CommandCompleteEvent(cmdOpCode,
                                         sizeof(status),
                                         &status);
                return;
            }
            // 1st check if there is not already an existing adv set with the same handle.
            pAdvSet = hci_tl_GetAdvSet(ADV_LEGACY_SET_HANDLE);

            if (pAdvSet)
            {
               // If set already exist, the parameters will be updated,
               // even if the call to  LE_SetExtAdvParams fails
               pAdvSet->scanCmdData.handle    = ADV_LEGACY_SET_HANDLE;  //Forced
               pAdvSet->scanCmdData.operation = AE_DATA_OP_COMPLETE;  //Forced
               pAdvSet->scanCmdData.fragPref  = 0;                    //Forced
               pAdvSet->scanCmdData.dataLen   = param[0];

               hci_tl_managedAEdata(cmdOpCode, &pAdvSet->scanCmdData, &param[1]);
            }
            else
            {
                status = LL_STATUS_ERROR_OUT_OF_HEAP;
                HCI_CommandCompleteEvent(cmdOpCode,
                                         sizeof(status),
                                         &status);
            }
            break;
        }
        case HCI_LE_SET_ADV_ENABLE:
        {
            if(checkLegacyHCICmdStatus(HCI_LEGACY_CMD_STATUS_BT4))
            {
                HCI_CommandStatusEvent(LL_STATUS_ERROR_COMMAND_DISALLOWED,
                                       cmdOpCode );
                return;
            }
            // Translate this legacy to a call to the new AE API.
            hci_tl_advSet_t *pAdvSet;
            llStatus_t status;

            // Check number of Set to enable, only one supported by TL.
            pAdvSet = hci_tl_GetAdvSet(ADV_LEGACY_SET_HANDLE);

            if(pAdvSet)
            {
                LL_AE_RegCBack(LL_CBACK_ADV_START_AFTER_ENABLE, (void *)NULL);
                LL_AE_RegCBack(LL_CBACK_ADV_END_AFTER_DISABLE, (void *)NULL);
                LL_AE_RegCBack(LL_CBACK_ADV_START, (void *)NULL);
                LL_AE_RegCBack(LL_CBACK_ADV_END, (void *)NULL);
                LL_AE_RegCBack(LL_CBACK_EXT_SCAN_REQ_RECEIVED, (void *)NULL);
                LL_AE_RegCBack(LL_CBACK_ADV_SET_TERMINATED, (void *)NULL);
                LL_AE_RegCBack(LL_CBACK_EXT_ADV_DATA_TRUNCATED, (void *)NULL);
                LL_AE_RegCBack(LL_CBACK_OUT_OF_MEMORY, (void *)hci_tl_aeAdvCback);

                pAdvSet->enableCmdParams.enable    = param[0];
                pAdvSet->enableCmdParams.numSets   = 1;
                pAdvSet->enableCmdParams.handle    = ADV_LEGACY_SET_HANDLE;
                pAdvSet->enableCmdParams.duration  = 0;
                pAdvSet->enableCmdParams.maxEvents = 0;

                status = LE_SetExtAdvEnable(&pAdvSet->enableCmdParams);
                if (status == LL_STATUS_ERROR_UNKNOWN_ADVERTISING_IDENTIFIER)
                {
                    hci_tl_RemoveAdvSet(ADV_LEGACY_SET_HANDLE);
                }
            }
            else
            {
                status = LL_STATUS_ERROR_OUT_OF_HEAP;
            }
            HCI_CommandCompleteEvent(cmdOpCode,
                                     sizeof(status),
                                     &status);
            break;
        }
#endif // (defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG)))
// Scanner
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
        case HCI_LE_SET_SCAN_PARAM:
        {
            if(checkLegacyHCICmdStatus(HCI_LEGACY_CMD_STATUS_BT4))
            {
                HCI_CommandStatusEvent( LL_STATUS_ERROR_COMMAND_DISALLOWED,
                                        cmdOpCode );
                return;
            }
            // Translate this legacy API to a call to the new AE API.
            llStatus_t status;

            hci_tl_cmdScanParams.ownAddrType                  = param[5];
            hci_tl_cmdScanParams.scanFilterPolicy             = param[6];
            hci_tl_cmdScanParams.scanPhys                     = LL_PHY_1_MBPS;  //1mbps
            hci_tl_cmdScanParams.extScanParam[0].scanType     = param[0];
            hci_tl_cmdScanParams.extScanParam[0].scanInterval = BUILD_UINT16(param[1], param[2]);
            hci_tl_cmdScanParams.extScanParam[0].scanWindow   = BUILD_UINT16(param[3], param[4]);

            status = LE_SetExtScanParams(&hci_tl_cmdScanParams);

            HCI_CommandCompleteEvent(cmdOpCode,
                                     sizeof(status),
                                     &status);
            break;

          break;
        }
        case HCI_LE_SET_SCAN_ENABLE:
        {
            if(checkLegacyHCICmdStatus(HCI_LEGACY_CMD_STATUS_BT4))
            {
                HCI_CommandStatusEvent(LL_STATUS_ERROR_COMMAND_DISALLOWED,
                                       cmdOpCode );
                return;
            }
            // Translate this legacy API to a call to the new AE API.
            llStatus_t status;

            // Need to register the callback Function.
            LL_AE_RegCBack(LL_CBACK_EXT_ADV_REPORT,        (void *)hci_tl_legacyScanCback);
            LL_AE_RegCBack(LL_CBACK_EXT_SCAN_TIMEOUT,      (void *)NULL);
#ifdef ENABLE_SCAN_CALLBACKS
            LL_AE_RegCBack(LL_CBACK_EXT_SCAN_START,        (void *)NULL);
            LL_AE_RegCBack(LL_CBACK_EXT_SCAN_END,          (void *)NULL);
            LL_AE_RegCBack(LL_CBACK_EXT_SCAN_WINDOW_END,   (void *)NULL);
            LL_AE_RegCBack(LL_CBACK_EXT_SCAN_INTERVAL_END, (void *)NULL);
            LL_AE_RegCBack(LL_CBACK_EXT_SCAN_DURATION_END, (void *)NULL);
            LL_AE_RegCBack(LL_CBACK_EXT_SCAN_PERIOD_END,   (void *)NULL);
#endif // ENABLE_SCAN_CALLBACKS

            hci_tl_cmdScanEnable.enable       = param[0];
            hci_tl_cmdScanEnable.dupFiltering = param[1];
            hci_tl_cmdScanEnable.duration     = 0;
            hci_tl_cmdScanEnable.period       = 0;

            status = LE_SetExtScanEnable(&hci_tl_cmdScanEnable);

            HCI_CommandCompleteEvent(cmdOpCode,
                                     sizeof(status),
                                     &status);
            break;
        }
#endif // (defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG))

 // Initiator
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)

        case HCI_LE_CREATE_CONNECTION:
          {
            if(checkLegacyHCICmdStatus(HCI_LEGACY_CMD_STATUS_BT4))
            {
                HCI_CommandStatusEvent(LL_STATUS_ERROR_COMMAND_DISALLOWED,
                                       cmdOpCode );
                return;
            }
            // Translate this legacy API to a call to the new AE API.
            llStatus_t status;
            static aeCreateConnCmd_t HCI_TL_createConnParam;

            HCI_TL_createConnParam.initFilterPolicy = param[4];
            HCI_TL_createConnParam.ownAddrType      = param[12];
            HCI_TL_createConnParam.peerAddrType     = param[5];
            memcpy(HCI_TL_createConnParam.peerAddr, &param[6], B_ADDR_LEN) ;
            HCI_TL_createConnParam.initPhys         = LL_PHY_1_MBPS;  //Forced
            HCI_TL_createConnParam.extInitParam[0].scanInterval = BUILD_UINT16(param[0], param[1]);
            HCI_TL_createConnParam.extInitParam[0].scanWindow   = BUILD_UINT16(param[2], param[3]);
            HCI_TL_createConnParam.extInitParam[0].connIntMin   = BUILD_UINT16(param[13], param[14]);
            HCI_TL_createConnParam.extInitParam[0].connIntMax   = BUILD_UINT16(param[15], param[16]);
            HCI_TL_createConnParam.extInitParam[0].connLatency  = BUILD_UINT16(param[17], param[18]);
            HCI_TL_createConnParam.extInitParam[0].connTimeout  = BUILD_UINT16(param[19], param[20]);
            HCI_TL_createConnParam.extInitParam[0].minLength    = BUILD_UINT16(param[21], param[22]);
            HCI_TL_createConnParam.extInitParam[0].maxLength    = BUILD_UINT16(param[23], param[24]);

            status = LE_ExtCreateConn(&HCI_TL_createConnParam);

            HCI_CommandStatusEvent(status, cmdOpCode);

            break;
        }
#endif // defined(CTRL_CONFIG) && (CTRL_CONFIG & INIT_CFG)
#endif // LEGACY_CMD

// Extended Initiator
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (INIT_CFG))
        case HCI_LE_EXT_CREATE_CONN:
        {
            llStatus_t status;
            static aeCreateConnCmd_t HCI_TL_createConnParam;

            if(checkLegacyHCICmdStatus(HCI_LEGACY_CMD_STATUS_BT5))
            {
                HCI_CommandStatusEvent(LL_STATUS_ERROR_COMMAND_DISALLOWED,
                                        cmdOpCode);
                return;
            }
            HCI_TL_createConnParam = *((aeCreateConnCmd_t *)param);

            status = LE_ExtCreateConn(&HCI_TL_createConnParam);

            HCI_CommandStatusEvent(status, HCI_LE_EXT_CREATE_CONN);
            break;
        }
#endif // INIT_CFG

// Extended Advertiser
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
        case HCI_LE_SET_EXT_ADV_PARAMETERS:
        {
            // This information needs to be saved on behalf of the LL.
            // A linked List will be created to save this information.
            hci_tl_advSet_t *pAdvSet;
            aeSetParamRtn_t retParams;
            int8_t rsp[2];

            if(checkLegacyHCICmdStatus(HCI_LEGACY_CMD_STATUS_BT5))
            {
                HCI_CommandStatusEvent(LL_STATUS_ERROR_COMMAND_DISALLOWED,
                                        cmdOpCode);
                return;
            }
            // 1st check if there is not already an existing adv set with the same handle.
            pAdvSet = hci_tl_GetAdvSet(param[0]);
            if (pAdvSet)
            {
                // If set already exist, parameters will be updated,
                // even if the call to  LE_SetExtAdvParams fails
                pAdvSet->advCmdParams = *((aeSetParamCmd_t *)param);

                rsp[0] = LE_SetExtAdvParams(&pAdvSet->advCmdParams, &retParams);
                rsp[1] = retParams.txPower;
                if (rsp[0] != LL_STATUS_SUCCESS)
                {
                    hci_tl_RemoveAdvSet(param[0]);
                }
            }
            else
            {
                rsp[0] = LL_STATUS_ERROR_OUT_OF_HEAP;
            }
            HCI_CommandCompleteEvent(cmdOpCode,
                                      2,
                                      rsp);
            break;
        }
        case HCI_LE_SET_EXT_ADV_ENABLE:
        {
            hci_tl_advSet_t *pAdvSet;
            llStatus_t status;
            uint8 i;

            if(checkLegacyHCICmdStatus(HCI_LEGACY_CMD_STATUS_BT5))
            {
                HCI_CommandStatusEvent(LL_STATUS_ERROR_COMMAND_DISALLOWED,
                                       cmdOpCode);
                return;
            }
            // Enable all Sets one by one
            for (i=0; i< param[1]; i++)
            {
                pAdvSet = hci_tl_GetAdvSet(param[2 + (i * 4)]);

                if(pAdvSet)
                {
                    LL_AE_RegCBack(LL_CBACK_ADV_START_AFTER_ENABLE, (void *)hci_tl_aeAdvCback);
                    LL_AE_RegCBack(LL_CBACK_ADV_END_AFTER_DISABLE, (void *)hci_tl_aeAdvCback);
                    LL_AE_RegCBack(LL_CBACK_ADV_START, (void *)hci_tl_aeAdvCback);
                    LL_AE_RegCBack(LL_CBACK_ADV_END, (void *)hci_tl_aeAdvCback);
                    LL_AE_RegCBack(LL_CBACK_EXT_SCAN_REQ_RECEIVED, (void *)hci_tl_aeAdvCback);
                    LL_AE_RegCBack(LL_CBACK_ADV_SET_TERMINATED, (void *)hci_tl_aeAdvCback);
                    LL_AE_RegCBack(LL_CBACK_EXT_ADV_DATA_TRUNCATED, (void *)hci_tl_aeAdvCback);
                    LL_AE_RegCBack(LL_CBACK_OUT_OF_MEMORY, (void *)hci_tl_aeAdvCback);

                    // Set command parameters
                    pAdvSet->enableCmdParams.enable = param[0];
                    pAdvSet->enableCmdParams.numSets = 1;
                    pAdvSet->enableCmdParams.handle = param[2 + (i * 4)];
                    // For the duration calc we use byte 3 as LSB and byte 4 as MSB of param, therefore we need to perform
                    // 8 bits shift for the forth byte and bitwise or to the third byte.
                    pAdvSet->enableCmdParams.duration = (uint16)(param[4 + (i * 4)] << 8) | (param[3 + (i * 4)]);
                    pAdvSet->enableCmdParams.maxEvents = param[5 + (i * 4)];

                    status = LE_SetExtAdvEnable(&pAdvSet->enableCmdParams);
                    if (status == LL_STATUS_ERROR_UNKNOWN_ADVERTISING_IDENTIFIER)
                    {
                        hci_tl_RemoveAdvSet(param[0]);
                    }
                }
                else
                {
                    status = LL_STATUS_ERROR_OUT_OF_HEAP;
                }
            }

            HCI_CommandCompleteEvent(cmdOpCode,
                                      sizeof(status),
                                      &status);
            break;
        }
        case HCI_LE_REMOVE_ADV_SET:
        {
            llStatus_t status = LE_RemoveAdvSet(param[0]);

            if(checkLegacyHCICmdStatus(HCI_LEGACY_CMD_STATUS_BT5))
            {
                HCI_CommandStatusEvent(LL_STATUS_ERROR_COMMAND_DISALLOWED,
                                       cmdOpCode);
                return;
            }
            // Remove it from our linked list also
            if (status == LL_STATUS_SUCCESS)
            {
                hci_tl_RemoveAdvSet(param[0]);
            }
            HCI_CommandCompleteEvent(cmdOpCode,
                                      sizeof(status),
                                      &status);
            break;
        }
        case HCI_LE_CLEAR_ADV_SETS:
        {
            llStatus_t status = LE_ClearAdvSets();

            if(checkLegacyHCICmdStatus(HCI_LEGACY_CMD_STATUS_BT5))
            {
                HCI_CommandStatusEvent(LL_STATUS_ERROR_COMMAND_DISALLOWED,
                                       cmdOpCode);
                return;
            }
            if (status == LL_STATUS_SUCCESS)
            {
                hci_tl_ClearAdvSet();
            }
            HCI_CommandCompleteEvent(cmdOpCode,
                                     sizeof(status),
                                     &status);
            break;
        }
        case HCI_LE_SET_EXT_SCAN_RESPONSE_DATA:
        {
            hci_tl_advSet_t *pAdvSet;
            llStatus_t status;

            if(checkLegacyHCICmdStatus(HCI_LEGACY_CMD_STATUS_BT5))
            {
                HCI_CommandStatusEvent(LL_STATUS_ERROR_COMMAND_DISALLOWED,
                                       cmdOpCode);
                return;
            }
            pAdvSet = hci_tl_GetAdvSet(param[0]);
            if (pAdvSet)
            {
                uint8_t idx = 0;
                pAdvSet->scanCmdData.handle    = param[idx++];
                pAdvSet->scanCmdData.operation = param[idx++];
                pAdvSet->scanCmdData.fragPref  = param[idx++];
                pAdvSet->scanCmdData.dataLen   = param[idx++];
                hci_tl_managedAEdata(cmdOpCode, &pAdvSet->scanCmdData, &param[idx]);
            }
            else
            {
                status = LL_STATUS_ERROR_OUT_OF_HEAP;
                HCI_CommandCompleteEvent(cmdOpCode,
                                         sizeof(status),
                                         &status);
            }
            break;
        }
        case HCI_LE_SET_EXT_ADV_DATA:
        {
            hci_tl_advSet_t *pAdvSet;
            llStatus_t status;

            if(checkLegacyHCICmdStatus(HCI_LEGACY_CMD_STATUS_BT5))
            {
                HCI_CommandStatusEvent(LL_STATUS_ERROR_COMMAND_DISALLOWED,
                                        cmdOpCode);
                return;
            }
            // 1st check if there is not already an existing adv set with the same handle.
            pAdvSet = hci_tl_GetAdvSet(param[0]);

            if (pAdvSet)
            {
               uint8_t idx = 0;
               // If set already exist, the parameters will be updated,
               // even if the call to  LE_SetExtAdvParams fails
               pAdvSet->advCmdData.handle    = param[idx++];
               pAdvSet->advCmdData.operation = param[idx++];
               pAdvSet->advCmdData.fragPref  = param[idx++];
               pAdvSet->advCmdData.dataLen   = param[idx++];
               hci_tl_managedAEdata(cmdOpCode, &pAdvSet->advCmdData, &param[idx]);
            }
            else
            {
                status = LL_STATUS_ERROR_OUT_OF_HEAP;
                HCI_CommandCompleteEvent(cmdOpCode,
                                         sizeof(status),
                                         &status);
            }
            break;
        }
        case HCI_EXT_LE_SET_EXT_ADV_DATA:
        {
            hci_tl_advSet_t *pAdvSet;
            llStatus_t status;

            if(checkLegacyHCICmdStatus(HCI_LEGACY_CMD_STATUS_BT5))
            {
                HCI_CommandStatusEvent(LL_STATUS_ERROR_COMMAND_DISALLOWED,
                                        cmdOpCode);
                return;
            }
            pAdvSet = hci_tl_GetAdvSet(param[0]);
            if (pAdvSet)
            {
                uint8_t idx = 0;
                uint16_t length;
                // If set already exist, the parameters will be updated,
                // even if the call to  LE_SetExtAdvParams fails
                pAdvSet->advCmdData.handle   = param[idx++];
                pAdvSet->advCmdData.fragPref = param[idx++];
                length = BUILD_UINT16(param[idx], param[idx+1]);
                idx+=2; // Skip the embedded Length Parameter
                pAdvSet->advCmdData.operation = AE_DATA_OP_COMPLETE;
                pAdvSet->advCmdData.dataLen   = length;
                // The real Data length of the buffer is contain in the first 2 bytes of the payload.
                hci_tl_managedAEdata(cmdOpCode, &pAdvSet->advCmdData, &param[idx]);
            }
            else
            {
                status = LL_STATUS_ERROR_OUT_OF_HEAP;
                HCI_CommandCompleteEvent(cmdOpCode,
                                         sizeof(status),
                                         &status);
            }
            break;
        }
        case HCI_EXT_LE_SET_EXT_SCAN_RESPONSE_DATA:
        {
            hci_tl_advSet_t *pAdvSet;
            llStatus_t status;

            if(checkLegacyHCICmdStatus(HCI_LEGACY_CMD_STATUS_BT5))
            {
                HCI_CommandStatusEvent(LL_STATUS_ERROR_COMMAND_DISALLOWED,
                                       cmdOpCode);
                return;
            }
            pAdvSet = hci_tl_GetAdvSet(param[0]);
            if (pAdvSet)
            {
                uint8_t idx = 0;
                uint16_t length;
                // If set already exist, the parameters will be updated,
                // even if the call to  LE_SetExtAdvParams fails
                pAdvSet->scanCmdData.handle   = param[idx++];
                pAdvSet->scanCmdData.fragPref = param[idx++];
                length = BUILD_UINT16(param[idx], param[idx+1]);
                idx+=2; // Skip the embedded Length Parameter
                pAdvSet->scanCmdData.operation = AE_DATA_OP_COMPLETE;
                pAdvSet->scanCmdData.dataLen   = length;
                // The real Data length of the buffer is contain in the first 2 bytes of the payload.
                hci_tl_managedAEdata(cmdOpCode, &pAdvSet->scanCmdData, &param[idx]);
            }
            else
            {
                status = LL_STATUS_ERROR_OUT_OF_HEAP;
                HCI_CommandCompleteEvent(cmdOpCode,
                                         sizeof(status),
                                         &status);
            }
            break;
        }
#endif // (ADV_NCONN_CFG | ADV_CONN_CFG)

// Extended Scanner
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
        case HCI_LE_SET_EXT_SCAN_ENABLE:
        {
            llStatus_t status;

            if(checkLegacyHCICmdStatus(HCI_LEGACY_CMD_STATUS_BT5))
            {
                HCI_CommandStatusEvent(LL_STATUS_ERROR_COMMAND_DISALLOWED,
                                       cmdOpCode);
                return;
            }
            // Need to register the callback Function.
            LL_AE_RegCBack(LL_CBACK_EXT_ADV_REPORT,        (void *)hci_tl_aeScanCback);
            LL_AE_RegCBack(LL_CBACK_EXT_SCAN_TIMEOUT,      (void *)hci_tl_aeScanCback);
#ifdef ENABLE_SCAN_CALLBACKS
            LL_AE_RegCBack(LL_CBACK_EXT_SCAN_START,        (void *)hci_tl_aeScanCback);
            LL_AE_RegCBack(LL_CBACK_EXT_SCAN_END,          (void *)hci_tl_aeScanCback);
            LL_AE_RegCBack(LL_CBACK_EXT_SCAN_WINDOW_END,   (void *)hci_tl_aeScanCback);
            LL_AE_RegCBack(LL_CBACK_EXT_SCAN_INTERVAL_END, (void *)hci_tl_aeScanCback);
            LL_AE_RegCBack(LL_CBACK_EXT_SCAN_DURATION_END, (void *)hci_tl_aeScanCback);
            LL_AE_RegCBack(LL_CBACK_EXT_SCAN_PERIOD_END,   (void *)hci_tl_aeScanCback);
#endif // ENABLE_SCAN_CALLBACKS

            hci_tl_cmdScanEnable = *((aeEnableScanCmd_t *)param);
            status = LE_SetExtScanEnable(&hci_tl_cmdScanEnable);

            HCI_CommandCompleteEvent(cmdOpCode,
                                     sizeof(status),
                                     &status);
            break;
        }
        case HCI_LE_SET_EXT_SCAN_PARAMETERS:
        {
            // This information needs to be saved on behalf of the LL.
            // A linked List will be created to save this information.
            llStatus_t status;

            if(checkLegacyHCICmdStatus(HCI_LEGACY_CMD_STATUS_BT5))
            {
                HCI_CommandStatusEvent(LL_STATUS_ERROR_COMMAND_DISALLOWED,
                                       cmdOpCode);
                return;
            }
            // If set already exist, the parameters will be updated,
            // even if the call to  LE_SetExtAdvParams fails
            hci_tl_cmdScanParams = *((aeSetScanParamCmd_t *)param);

            status = LE_SetExtScanParams(&hci_tl_cmdScanParams);

            HCI_CommandCompleteEvent(cmdOpCode,
                                     sizeof(status),
                                     &status);
            break;

        }
#endif /* (CTRL_CONFIG & SCAN_CFG) */
#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
        case HCI_LE_READ_MAX_ADV_DATA_LENGTH:
        {
            uint16_t value;
            uint8_t res[3];

            if(checkLegacyHCICmdStatus(HCI_LEGACY_CMD_STATUS_BT5))
            {
                HCI_CommandStatusEvent(LL_STATUS_ERROR_COMMAND_DISALLOWED,
                                       cmdOpCode);
                return;
            }
            value = LE_ReadMaxAdvDataLen();
            res[0] = LL_STATUS_SUCCESS;
            res[1] = LO_UINT16(value);
            res[2] = HI_UINT16(value);
            HCI_CommandCompleteEvent(cmdOpCode,
                                      sizeof(res),
                                      &res);
            break;
        }
        case HCI_LE_READ_NUM_SUPPORTED_ADV_SETS:
        {
            uint8_t value;
            uint8_t res[2];

            if(checkLegacyHCICmdStatus(HCI_LEGACY_CMD_STATUS_BT5))
            {
                HCI_CommandStatusEvent(LL_STATUS_ERROR_COMMAND_DISALLOWED,
                                       cmdOpCode);
                return;
            }
            value = LE_ReadMaxAdvDataLen();
            res[0] = LL_STATUS_SUCCESS;
            res[1] = value;
            HCI_CommandCompleteEvent(cmdOpCode,
                                     sizeof(res),
                                     &res);
            break;
        }
#endif // (ADV_NCONN_CFG | ADV_CONN_CFG)
        default:
        {
            // Opcode not found, return error.
            uint8_t status;
            status = HCI_ERROR_CODE_UNKNOWN_HCI_CMD;

            HCI_CommandCompleteEvent(cmdOpCode, 1, &status);
        }
    }
}

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
/*********************************************************************
 * @fn      hci_tl_aeAdvCback
 *
 * @brief   Callback for the advertising event send by LL.
 *          This function will send an event to the application so that process
 *          is execute outside of the SWI context.
 *
 * @param   event  - event trigging the callback.
 * @param   pData  - Pointer to the data that comes with the event (this is an union).
 *
 * @return  none
*/
static void hci_tl_aeAdvCback(uint8_t event, void *pData)
{
  if (HCI_TL_CallbackEvtProcessCB)
  {
    hci_tl_AdvEvtCallback_t * evtCallback;
    evtCallback = (hci_tl_AdvEvtCallback_t *) ICall_malloc(sizeof(hci_tl_AdvEvtCallback_t));
    if(evtCallback)
    {
      switch(event)
      {
        case LL_CBACK_ADV_SET_TERMINATED:
        case LL_CBACK_EXT_SCAN_REQ_RECEIVED:
        {
          evtCallback->data.pData = pData;
          break;
        }
        case LL_CBACK_ADV_START_AFTER_ENABLE:
        case LL_CBACK_ADV_END_AFTER_DISABLE:
        case LL_CBACK_ADV_START:
        case LL_CBACK_ADV_END:
        {
          uint8_t handle = (uint8_t) *((uint8_t *) pData);
          evtCallback->data.handle = handle;
          break;
        }
        case LL_CBACK_EXT_ADV_DATA_TRUNCATED:
        {
          aeAdvTrucData_t* pAllocData = (aeAdvTrucData_t *) ICall_malloc(sizeof(aeAdvTrucData_t));
          if(pAllocData)
          {
            aeAdvTrucData_t* pDataTrunc  = (aeAdvTrucData_t*) pData;
            pAllocData->handle           = pDataTrunc->handle;
            pAllocData->advDataLen       = pDataTrunc->advDataLen;
            pAllocData->availAdvDataLen  = pDataTrunc->availAdvDataLen;
            evtCallback->data.pData      = pAllocData;
          }
          else
          {
            uint8_t handle = (uint8_t) *((uint8_t *) pData);
            evtCallback->data.handle = handle;
          }
          break;
        }
        default:
        {
          evtCallback->data.pData = NULL;
          break;
        }
      }
      evtCallback->event = event;
      if(!HCI_TL_CallbackEvtProcessCB( (void*) evtCallback, (void*) hci_tl_aeAdvCbackProcess))
      {
        if ((event == LL_CBACK_ADV_SET_TERMINATED) || (event == LL_CBACK_EXT_SCAN_REQ_RECEIVED))
        {
          if (pData)
          {
            ICall_free(pData);
          }
        }
        ICall_free(evtCallback);
        HCI_TL_sendSystemReport(HCI_TL_ID, LL_STATUS_ERROR_OUT_OF_HEAP, HCI_LE_EVENT_CODE);
      }
    }
    else
    {
      if ((event == LL_CBACK_ADV_SET_TERMINATED) || (event == LL_CBACK_EXT_SCAN_REQ_RECEIVED))
      {
        if (pData)
        {
          ICall_free(pData);
        }
      }
      HCI_TL_sendSystemReport(HCI_TL_ID, LL_STATUS_ERROR_OUT_OF_HEAP, HCI_LE_EVENT_CODE);
    }
  }
}

/*********************************************************************
 * @fn      hci_tl_aeAdvCbackProcess
 *
 * @brief   Callback register with the application to handle LL callback event.
 *          Manage the following Event:
 *
 * @param   evtCallback - strucutre containing the parameter of the callback.
 *
 * @return  none
 */
void hci_tl_aeAdvCbackProcess(hci_tl_AdvEvtCallback_t* evtCallback)
{
  npiPkt_t *msg;

  if(evtCallback)
  {
    uint8_t event = evtCallback->event;
    switch(event)
    {
      case LL_CBACK_ADV_SET_TERMINATED:
      {
        aeAdvSetTerm_t* pEvtData = (aeAdvSetTerm_t* )evtCallback->data.pData;

        if ((pEvtData != NULL) && (pEvtData->subCode == HCI_BLE_ADV_SET_TERMINATED_EVENT))
        {
          uint8_t totalLength = sizeof(npiPkt_t) + HCI_EVENT_MIN_LENGTH + sizeof(aeAdvSetTerm_t);
          msg = (npiPkt_t *)ICall_allocMsg(totalLength);
          if(msg)
          {
            //Complete the packet and send it
            // Icall message event, status, and pointer to packet
            msg->hdr.event  = HCI_EVENT_PACKET;
            msg->hdr.status = 0xFF;

            // fill in length and data pointer
            msg->pktLen = HCI_EVENT_MIN_LENGTH + sizeof(aeAdvSetTerm_t);
            msg->pData  = (uint8*)(msg+1);
            // fill in BLE Complete Event data
            msg->pData[0] = HCI_EVENT_PACKET;
            msg->pData[1] = HCI_LE_EVENT_CODE;
            msg->pData[2] = sizeof(aeAdvSetTerm_t);

            // We keep all the information the same across report, only the data type will change.
            msg->pData[3]  = pEvtData->subCode;
            msg->pData[4]  = pEvtData->status;
            msg->pData[5]  = pEvtData->handle;
            msg->pData[6]  = LO_UINT16(pEvtData->connHandle);
            msg->pData[7]  = HI_UINT16(pEvtData->connHandle);
            msg->pData[8]  = pEvtData->numCompAdvEvts;
            NPITask_sendToHost((uint8_t *)msg);
          }
          else
          {
            // Out of Ressource
            HCI_TL_sendSystemReport(HCI_TL_ID, LL_STATUS_ERROR_OUT_OF_HEAP, HCI_LE_EVENT_CODE);
          }
        }
        else
        {
          HCI_TL_sendSystemReport(HCI_TL_ID, LL_STATUS_ERROR_UNKNOWN_ADV_EVT_TYPE, HCI_BLE_EXTENDED_ADV_REPORT_EVENT);
        }
        //Free the message
        if (pEvtData != NULL)
        {
          ICall_free(pEvtData);
        }
        break;
      }
      case LL_CBACK_EXT_SCAN_REQ_RECEIVED:
      {
        aeScanReqReceived_t *extAdvRpt;
        extAdvRpt = (aeScanReqReceived_t*) evtCallback->data.pData;
        if ((extAdvRpt != NULL) && (extAdvRpt->subCode == HCI_BLE_SCAN_REQUEST_RECEIVED_EVENT))
        {
          uint8_t totalLength = sizeof(npiPkt_t) + HCI_EVENT_MIN_LENGTH + sizeof(aeScanReqReceived_t) ;
          msg = (npiPkt_t *)ICall_allocMsg(totalLength);
          if(msg)
          {
            //Complete the packet and send it
            // Icall message event, status, and pointer to packet
            msg->hdr.event  = HCI_EVENT_PACKET;
            msg->hdr.status = 0xFF;

            // fill in length and data pointer

#ifdef QUAL_TEST
            // exclude the channel and rssi
            msg->pktLen = HCI_EVENT_MIN_LENGTH + sizeof(aeScanReqReceived_t) - 2;
#else
            msg->pktLen = HCI_EVENT_MIN_LENGTH + sizeof(aeScanReqReceived_t);
#endif

            msg->pData  = (uint8*)(msg+1);
            // fill in BLE Complete Event data
            msg->pData[0] = HCI_EVENT_PACKET;
            msg->pData[1] = HCI_LE_EVENT_CODE;

#ifdef QUAL_TEST
            // exclude the channel and rssi
            msg->pData[2] = sizeof(aeScanReqReceived_t) - 2;
#else
            msg->pData[2] = sizeof(aeScanReqReceived_t);
#endif
            // We keep all the information the same across report, only the data type will change.
            msg->pData[3] = extAdvRpt->subCode;
            msg->pData[4] = extAdvRpt->handle;
            msg->pData[5] = extAdvRpt->scanAddrType;
            memcpy(&msg->pData[6], extAdvRpt->scanAddr, B_ADDR_LEN);

#ifndef QUAL_TEST
            // exclude the channel and rssi
            msg->pData[12] = extAdvRpt->channel;
            msg->pData[13] = extAdvRpt->rssi;
#endif
            NPITask_sendToHost((uint8_t *)msg);
          }
        }
        else
        {
          HCI_TL_sendSystemReport(HCI_TL_ID, LL_STATUS_ERROR_UNKNOWN_ADV_EVT_TYPE, HCI_BLE_EXTENDED_ADV_REPORT_EVENT);
        }
        //Free the message
        if (extAdvRpt != NULL)
        {
          ICall_free(extAdvRpt);
        }
        break;
      }
      case LL_CBACK_ADV_START_AFTER_ENABLE:
      case LL_CBACK_ADV_END_AFTER_DISABLE:
      case LL_CBACK_ADV_START:
      case LL_CBACK_ADV_END:
      {
        hci_tl_aeAdvCbackSendEvent(evtCallback->event, (uint8_t ) evtCallback->data.handle);
        break;
      }
      case LL_CBACK_EXT_ADV_DATA_TRUNCATED:
      {
        aeAdvTrucData_t *extAdvTrunc;
        extAdvTrunc = (aeAdvTrucData_t*) evtCallback->data.pData;
        if (extAdvTrunc)
        {
          uint8_t data[7];

          data[0] = LO_UINT16(HCI_EXT_LE_ADV_EVENT);
          data[1] = HI_UINT16(HCI_EXT_LE_ADV_EVENT);
          data[2] = LL_CBACK_EXT_ADV_DATA_TRUNCATED;
          data[3] = extAdvTrunc->handle;
          data[4] = LO_UINT16(extAdvTrunc->advDataLen);
          data[5] = HI_UINT16(extAdvTrunc->advDataLen);
          data[6] = extAdvTrunc->availAdvDataLen;

          HCI_TL_SendVSEvent(data, sizeof(data));
          ICall_free(extAdvTrunc);
        }
        else
        {
        hci_tl_aeAdvCbackSendEvent(evtCallback->event, (uint8_t ) evtCallback->data.handle);
        }
        break;
      }
      case LL_CBACK_OUT_OF_MEMORY:
      {
        HCI_TL_sendSystemReport(HCI_TL_ID, LL_STATUS_ERROR_OUT_OF_HEAP, HCI_LE_EVENT_CODE);
      }

      default:
      {
        HCI_TL_sendSystemReport(HCI_TL_ID, LL_STATUS_ERROR_UNKNOWN_ADV_EVT_TYPE, HCI_BLE_EXTENDED_ADV_REPORT_EVENT);
      }

    }
    ICall_free(evtCallback);
  }
}

/*********************************************************************
 * @fn      hci_tl_aeAdvCbackSendEvent
 *
 * @brief   Create and send vendor specific event for Advertising Extension.
 *
 * @param   pParams - data of the report.
 *
 * @return  none
 */
static void hci_tl_aeAdvCbackSendEvent(uint8_t eventId, uint8_t  handle)
{
    uint8_t data[4];

    data[0] = LO_UINT16(HCI_EXT_LE_ADV_EVENT);
    data[1] = HI_UINT16(HCI_EXT_LE_ADV_EVENT);
    data[2] = eventId;
    data[3] = handle;

    HCI_TL_SendVSEvent(data, sizeof(data));
}
#endif // (ADV_NCONN_CFG | ADV_CONN_CFG)

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & SCAN_CFG)
/*********************************************************************
 * @fn      hci_tl_aeScanCback
 *
 * @brief   Callback for the advertising report send by LL.
 *          This callback is executed in SWI context
 *
 * @param   extAdvRpt - data of the Advertising report.
 *
 * @return  none
 */
static void hci_tl_aeScanCback(uint8_t event, aeExtAdvRptEvt_t *extAdvRpt)
{
  if (HCI_TL_CallbackEvtProcessCB)
  {
    // Decide what to do with the pointer based on Event.
    hci_tl_ScanEvtCallback_t * evtCallback;
    evtCallback = (hci_tl_ScanEvtCallback_t *) ICall_malloc(sizeof(hci_tl_ScanEvtCallback_t));
    if(evtCallback)
    {
      switch(event)
      {
        case LL_CBACK_EXT_ADV_REPORT:
        {
          evtCallback->pData = extAdvRpt;
          break;
        }
        case LL_CBACK_EXT_SCAN_TIMEOUT:
        case LL_CBACK_EXT_SCAN_START:
        case LL_CBACK_EXT_SCAN_PERIOD_END:
        case LL_CBACK_EXT_SCAN_END:
        case LL_CBACK_EXT_SCAN_WINDOW_END:
        case LL_CBACK_EXT_SCAN_INTERVAL_END:
        case LL_CBACK_EXT_SCAN_DURATION_END:
        default:
        {
          evtCallback->pData = NULL; // Value of handle
          break;
        }
      }
      evtCallback->event = event;
      if(!HCI_TL_CallbackEvtProcessCB( (void*) evtCallback, (void*) hci_tl_aeScanEventCallbackProcess))
      {
        // Not enough Heap...
        if ((event == LL_CBACK_EXT_ADV_REPORT) )
        {
          if (extAdvRpt->pData)
          {
            ICall_free(extAdvRpt->pData);
          }
          ICall_free(extAdvRpt);
        }
        ICall_free(evtCallback);
        HCI_TL_sendSystemReport(HCI_TL_ID, LL_STATUS_ERROR_OUT_OF_HEAP, HCI_LE_EVENT_CODE);
      }
    }
    else
    {
      if ((event == LL_CBACK_EXT_ADV_REPORT) )
      {
        if (extAdvRpt->pData)
        {
          ICall_free(extAdvRpt->pData);
        }
        ICall_free(extAdvRpt);
      }
      HCI_TL_sendSystemReport(HCI_TL_ID, LL_STATUS_ERROR_OUT_OF_HEAP, HCI_LE_EVENT_CODE);
    }
  }
}

#ifdef LEGACY_CMD
/*********************************************************************
 * @fn      hci_tl_legacyScanCback
 *
 * @brief   Callback for the legacy advertising report send by LL.
 *          This callback is executed in SWI context
 *
 * @param   extAdvRpt - data of the Advertising report.
 *
 * @return  none
 */
static void hci_tl_legacyScanCback(uint8_t event, aeExtAdvRptEvt_t *extAdvRpt)
{
  if (HCI_TL_CallbackEvtProcessCB)
  {
    // Decide what to do with the pointer based on Event.
    hci_tl_ScanEvtCallback_t * evtCallback;
    evtCallback = (hci_tl_ScanEvtCallback_t *) ICall_malloc(sizeof(hci_tl_ScanEvtCallback_t));
    if(evtCallback)
    {
      switch(event)
      {
        case LL_CBACK_EXT_ADV_REPORT:
        {
          evtCallback->pData = extAdvRpt;
          break;
        }
        case LL_CBACK_EXT_SCAN_TIMEOUT:
        case LL_CBACK_EXT_SCAN_START:
        case LL_CBACK_EXT_SCAN_PERIOD_END:
        case LL_CBACK_EXT_SCAN_END:
        case LL_CBACK_EXT_SCAN_WINDOW_END:
        case LL_CBACK_EXT_SCAN_INTERVAL_END:
        case LL_CBACK_EXT_SCAN_DURATION_END:
        default:
        {
          evtCallback->pData = NULL; // Value of handle
          break;
        }
      }
      evtCallback->event = event;
      HCI_TL_CallbackEvtProcessCB( (void*) evtCallback, (void*) hci_tl_legacyScanEventCallbackProcess);
    }
    else
    {
      if ((event == LL_CBACK_EXT_ADV_REPORT) )
      {
        ICall_free(extAdvRpt);
      }
      HCI_TL_sendSystemReport(HCI_TL_ID, LL_STATUS_ERROR_OUT_OF_HEAP, HCI_LE_EVENT_CODE);
    }
  }
}
#endif // LEGACY_CMD

/*********************************************************************
 * @fn      hci_tl_aeScanEventCallbackProcess
 *
 * @brief   Process the advertising report send by LL.
 *          This function will send the extended advertising report (per core spec).
 *
 * @param   extAdvRpt - data of the Advertising report.
 *
 * @return  none
 */
static void hci_tl_aeScanEventCallbackProcess(hci_tl_ScanEvtCallback_t *evtCallback)
{
  uint8_t dataLen;
  uint8_t remainingLength;
  npiPkt_t *msg;
  uint8_t totalLength = 0;

  if (evtCallback != NULL)
  {
    uint8_t event = evtCallback->event;

    switch(event)
    {
      case LL_CBACK_EXT_ADV_REPORT:
      {
          aeExtAdvRptEvt_t *extAdvRpt;
          extAdvRpt = (aeExtAdvRptEvt_t*) evtCallback->pData;

          // DEBUG CODE.
          // Add a Filter on RSSI to avoid being flooded and doomed by
          // all the crazy IOT devicein the world.
          // Uncoment the following to only report Adv with RSSI high enough
          // so they match teh board that are less than 1m from each other.
          //if (extAdvRpt->rssi < -37)
          //{
          //  // Reject this report...
          //
          //}
          //else
          // END of Debug Code

          if ((extAdvRpt != NULL) && (extAdvRpt->subCode == HCI_BLE_EXTENDED_ADV_REPORT_EVENT))
          {
            remainingLength = extAdvRpt->dataLen;
            // Got the Report, Map it to the Extended Report Event...
            do
            {
                //Check Length, if bigger than MAX_REPORT_DATA_SIZE, split it...
                if (remainingLength > MAX_REPORT_DATA_SIZE)
                {
                    dataLen = MAX_REPORT_DATA_SIZE;
                }
                else
                {
                    dataLen = remainingLength;
                }
                totalLength = sizeof(npiPkt_t) + HCI_EVENT_MIN_LENGTH + HCI_AE_EVENT_LENGTH + dataLen;
                msg = (npiPkt_t *)ICall_allocMsg(totalLength);
                if(msg)
                {
                  //Complete the packet and send it
                  // Icall message event, status, and pointer to packet
                  msg->hdr.event  = HCI_EVENT_PACKET;
                  msg->hdr.status = 0xFF;

                  // fill in length and data pointer
                  msg->pktLen = HCI_EVENT_MIN_LENGTH + HCI_AE_EVENT_LENGTH + dataLen;
                  msg->pData  = (uint8*)(msg+1);
                  // fill in BLE Complete Event data
                  msg->pData[0] = HCI_EVENT_PACKET;
                  msg->pData[1] = HCI_LE_EVENT_CODE;
                  msg->pData[2] = HCI_AE_EVENT_LENGTH + dataLen;

                  // We keep all the information the same across report, only the data type will change.
                  msg->pData[3]  = extAdvRpt->subCode;
                  msg->pData[4]  = extAdvRpt->numRpts;
                  msg->pData[5]  = LO_UINT16(extAdvRpt->evtType);
                  msg->pData[6]  = HI_UINT16(extAdvRpt->evtType);
                  msg->pData[7]  = extAdvRpt->addrType;
                  memcpy(&msg->pData[8], extAdvRpt->addr, B_ADDR_LEN);
                  msg->pData[14] = extAdvRpt->primPhy;
                  msg->pData[15] = extAdvRpt->secPhy;
                  msg->pData[16] = extAdvRpt->advSid;
                  msg->pData[17] = extAdvRpt->txPower;
                  msg->pData[18] = extAdvRpt->rssi;
                  msg->pData[19] = LO_UINT16(extAdvRpt->periodicAdvInt);
                  msg->pData[20] = HI_UINT16(extAdvRpt->periodicAdvInt);
                  msg->pData[21] = extAdvRpt->directAddrType;
                  memcpy(&msg->pData[22], extAdvRpt->directAddr, B_ADDR_LEN);
                  msg->pData[28] = dataLen;

                  if (dataLen)
                  {
                    memcpy( &msg->pData[HCI_AE_EVENT_LENGTH+HCI_EVENT_MIN_LENGTH],
                            extAdvRpt->pData + (extAdvRpt->dataLen - remainingLength),
                            dataLen);
                  }

                  // Update the type to incomplete with more data to follow
                  if (remainingLength > MAX_REPORT_DATA_SIZE)
                  {
                      // This is not the last packet
                     ((aeExtAdvRptEvt_t *) &(msg->pData[3]))->evtType &= AE_EVT_TYPE_COMPLETE_MASK;
                     ((aeExtAdvRptEvt_t *) &(msg->pData[3]))->evtType |= AE_EVT_TYPE_INCOMPLETE_MORE_TO_COME;
                  }

                  NPITask_sendToHost((uint8_t *)msg);

                }
                else
                {
                  HCI_TL_sendSystemReport(HCI_TL_ID, LL_STATUS_ERROR_OUT_OF_HEAP, HCI_BLE_EXTENDED_ADV_REPORT_EVENT);
                  HCI_TL_sendMemoryReport(0x3);
                  break;
                }
                // Update the local variable to send the rest of the payload.
                remainingLength-=dataLen;
            }while(remainingLength > 0);
          }
          else
          {
              HCI_TL_sendSystemReport(HCI_TL_ID, LL_STATUS_ERROR_UNKNOWN_ADV_EVT_TYPE, HCI_BLE_EXTENDED_ADV_REPORT_EVENT);
          }
          //Free the message and the payload
          if (extAdvRpt != NULL)
          {
            if (extAdvRpt->pData != NULL)
            {
              ICall_free(extAdvRpt->pData);
            }
            ICall_free(extAdvRpt);
          }
        break;
      }
      case LL_CBACK_EXT_SCAN_TIMEOUT:
      {
        uint8_t totalLength = sizeof(npiPkt_t) + HCI_EVENT_MIN_LENGTH + sizeof(aeScanTimeout_t) ;
        msg = (npiPkt_t *)ICall_allocMsg(totalLength);
        if(msg)
        {
          //Complete the packet and send it
          // Icall message event, status, and pointer to packet
          msg->hdr.event  = HCI_EVENT_PACKET;
          msg->hdr.status = 0xFF;

          // fill in length and data pointer
          msg->pktLen = HCI_EVENT_MIN_LENGTH + sizeof(aeScanTimeout_t);
          msg->pData  = (uint8*)(msg+1);
          // fill in BLE Complete Event data
          msg->pData[0] = HCI_EVENT_PACKET;
          msg->pData[1] = HCI_LE_EVENT_CODE;
          msg->pData[2] = sizeof(aeScanTimeout_t);

          // We keep all the information the same across report, only the data type will change.
          msg->pData[3]  = HCI_BLE_SCAN_TIMEOUT_EVENT;  // Only one possible Subcode
          NPITask_sendToHost((uint8_t *)msg);
        }
        break;
      }
      case LL_CBACK_EXT_SCAN_START:
      case LL_CBACK_EXT_SCAN_PERIOD_END:
      case LL_CBACK_EXT_SCAN_END:
      case LL_CBACK_EXT_SCAN_WINDOW_END:
      case LL_CBACK_EXT_SCAN_INTERVAL_END:
      case LL_CBACK_EXT_SCAN_DURATION_END:
      {
        // For those event, return a vendor specific Event.
        uint8_t data[3];
        data[0] = LO_UINT16(HCI_EXT_LE_SCAN_EVENT);
        data[1] = HI_UINT16(HCI_EXT_LE_SCAN_EVENT);
        data[2] = event;
        HCI_TL_SendVSEvent(data, sizeof(data));
        break;
      }
      default:
      {
        //Unknow Event, just free the message.
        HCI_TL_sendSystemReport(HCI_TL_ID, LL_STATUS_ERROR_UNKNOWN_ADV_EVT_TYPE, HCI_EXT_LE_SCAN_EVENT);
        break;
      }
    } // end Switch

    ICall_free(evtCallback);
  }
}

#ifdef LEGACY_CMD
/*********************************************************************
 * @fn      hci_tl_legacyScanEventCallbackProcess
 *
 * @brief   Process the legacy advertising report send by LL.
 *          This function will send the advertising report (per core spec).
 *
 * @param   extAdvRpt - data of the Advertising report.
 *
 * @return  none
 */
static void hci_tl_legacyScanEventCallbackProcess(hci_tl_ScanEvtCallback_t *evtCallback)
{
  uint8_t dataLen;
  npiPkt_t *msg;
  uint8_t totalLength = 0;
  uint8_t status = LL_STATUS_SUCCESS;

  if (evtCallback != NULL)
  {
    uint8_t event = evtCallback->event;

    switch(event)
    {
      case LL_CBACK_EXT_ADV_REPORT:
      {
          // Translate the report to the legacy report
          aeExtAdvRptEvt_t *extAdvRpt;
          extAdvRpt = (aeExtAdvRptEvt_t*) evtCallback->pData;

          // DEBUG CODE.
          // Add a Filter on RSSI to avoid being flooded and doomed by
          // all the crazy IOT devicein the world.
          // Uncoment the following to only report Adv with RSSI high enough
          // so they match teh board that are less than 1m from each other.
          //if (extAdvRpt->rssi < -37)
          //{
          //  // Reject this report...
          //  //Free the message and the payload
          //
          //}
          //else
          // END of Debug Code

          if ((extAdvRpt != NULL) && (extAdvRpt->subCode == HCI_BLE_EXTENDED_ADV_REPORT_EVENT))
          {
#ifdef QUAL_TEST
              if (extAdvRpt->directAddrType == AE_EXT_ADV_RPT_DIR_ADDR_TYPE_UNRESOLVED_RPA)
              {
                  // Got the Report, Map it to the LEGACY Directed Report Event...

                  totalLength = sizeof(npiPkt_t) + HCI_EVENT_MIN_LENGTH + HCI_ADV_DIRECTED_REPORT_EVENT_LEN;
                  msg = (npiPkt_t *)ICall_allocMsg(totalLength);
                  if(msg)
                  {
                    //Complete the packet and send it
                    // Icall message event, status, and pointer to packet
                    msg->hdr.event  = HCI_EVENT_PACKET;
                    msg->hdr.status = 0xFF;

                    // fill in length and data pointer
                    msg->pktLen = HCI_EVENT_MIN_LENGTH + HCI_ADV_DIRECTED_REPORT_EVENT_LEN;
                    msg->pData  = (uint8*)(msg+1);
                    // fill in BLE Complete Event data
                    msg->pData[0] = HCI_EVENT_PACKET;
                    msg->pData[1] = HCI_LE_EVENT_CODE;
                    msg->pData[2] = HCI_ADV_DIRECTED_REPORT_EVENT_LEN;

                    // We keep all the information the same across report, only the data type will change.
                    msg->pData[3]  = HCI_BLE_DIRECT_ADVERTISING_REPORT_EVENT;  //Forced
                    msg->pData[4]  = extAdvRpt->numRpts;
                    msg->pData[5]  = 1; //Connectable directed legacy advertising

                    msg->pData[6]  = extAdvRpt->addrType;
                    memcpy(&msg->pData[7], extAdvRpt->addr, B_ADDR_LEN);
                    msg->pData[13] = LL_DEV_ADDR_TYPE_RANDOM;
                    memcpy(&msg->pData[14], extAdvRpt->directAddr, B_ADDR_LEN);
                    msg->pData[20] = extAdvRpt->rssi;

                    NPITask_sendToHost((uint8_t *)msg);
                  }
                  else
                  {
                    HCI_TL_sendSystemReport(HCI_TL_ID, LL_STATUS_ERROR_OUT_OF_HEAP, HCI_BLE_EXTENDED_ADV_REPORT_EVENT);
                    HCI_TL_sendMemoryReport(0x3);
                    break;
                  }
              }
              else
#endif
              {
                  // Got the Report, Map it to the LEGACY Report Event...
                  dataLen = extAdvRpt->dataLen;

                  totalLength = sizeof(npiPkt_t) + HCI_EVENT_MIN_LENGTH + HCI_ADV_REPORT_EVENT_LEN + dataLen;
                  msg = (npiPkt_t *)ICall_allocMsg(totalLength);
                  if(msg)
                  {
                    //Complete the packet and send it
                    // Icall message event, status, and pointer to packet
                    msg->hdr.event  = HCI_EVENT_PACKET;
                    msg->hdr.status = 0xFF;

                    // fill in length and data pointer
                    msg->pktLen = HCI_EVENT_MIN_LENGTH + HCI_ADV_REPORT_EVENT_LEN + dataLen;
                    msg->pData  = (uint8*)(msg+1);
                    // fill in BLE Complete Event data
                    msg->pData[0] = HCI_EVENT_PACKET;
                    msg->pData[1] = HCI_LE_EVENT_CODE;
                    msg->pData[2] = HCI_ADV_REPORT_EVENT_LEN + dataLen;

                    // We keep all the information the same across report, only the data type will change.
                    msg->pData[3]  = HCI_BLE_ADV_REPORT_EVENT;  //Forced
                    msg->pData[4]  = extAdvRpt->numRpts;
                    switch (extAdvRpt->evtType)
                    {
                      case AE_EXT_ADV_RPT_EVT_TYPE_ADV_IND:
                      {
                        msg->pData[5] = 0;
                        break;
                      }
                      case AE_EXT_ADV_RPT_EVT_TYPE_DIRECT_IND:
                      {
                        msg->pData[5] = 1;
                        break;
                      }
                      case AE_EXT_ADV_RPT_EVT_TYPE_SCAN_IND:
                      {
                        msg->pData[5] = 2;
                        break;
                      }
                      case AE_EXT_ADV_RPT_EVT_TYPE_NONCONN_IND:
                      {
                        msg->pData[5] = 3;
                        break;
                      }
                      case AE_EXT_ADV_RPT_EVT_TYPE_SCAN_RSP_ADV_IND:
                      case AE_EXT_ADV_RPT_EVT_TYPE_SCAN_RSP_ADV_SCAN_IND:
                      case AE_EXT_ADV_RPT_EVT_TYPE_SCAN_RSP:
                      {
                        msg->pData[5] = 4;
                        break;
                      }
                      default:
                      {
                        // Ignore any other event type....
                        ICall_freeMsg(msg);
                        status = LL_STATUS_ERROR_UNEXPECTED_PARAMETER;
                      }
                    }
                    if (status == LL_STATUS_SUCCESS)
                    {
                      msg->pData[6]  = extAdvRpt->addrType;
                      memcpy(&msg->pData[7], extAdvRpt->addr, B_ADDR_LEN);
                      msg->pData[13] = dataLen;

                      if (dataLen)
                      {
                        memcpy( &msg->pData[14],
                                extAdvRpt->pData,
                                dataLen);
                      }
                      msg->pData[14 + dataLen] = extAdvRpt->rssi;

                      NPITask_sendToHost((uint8_t *)msg);
                    }
                  }
                  else
                  {
                    HCI_TL_sendSystemReport(HCI_TL_ID, LL_STATUS_ERROR_OUT_OF_HEAP, HCI_BLE_EXTENDED_ADV_REPORT_EVENT);
                    HCI_TL_sendMemoryReport(0x3);
                    break;
                  }
              }
          }
          else
          {
              HCI_TL_sendSystemReport(HCI_TL_ID, LL_STATUS_ERROR_UNKNOWN_ADV_EVT_TYPE, HCI_BLE_EXTENDED_ADV_REPORT_EVENT);
          }

          //Free the message and the payload
          if (extAdvRpt != NULL)
          {
            if (extAdvRpt->pData != NULL)
            {
              ICall_free(extAdvRpt->pData);
            }
            ICall_free(extAdvRpt);
          }
        break;
      }
      default:
      {
        //Unknow Event, just free the message.
        HCI_TL_sendSystemReport(HCI_TL_ID, LL_STATUS_ERROR_UNKNOWN_ADV_EVT_TYPE, HCI_EXT_LE_SCAN_EVENT);
        break;
      }
    } // end Switch

    ICall_free(evtCallback);
  }
}
#endif //LEGACY_CMD

#endif //SCAN_CFG
#endif //!HOST_CONFIG

#ifndef PTM_MODE
// Following function are not used by PTM

#if ( HOST_CONFIG & ( CENTRAL_CFG | OBSERVER_CFG ) )
/*********************************************************************
 * @fn      host_tl_scanEvtCallback
 *
 * @brief   Function callback for the HOST GAP Scanner module.
 *
 * @param   event - the event that trig this callback
 *          length - length of the data link to the event
 *          pData  - pointer to the data linked to the event.
 *          arg    - user argument
 *
 * @return  none
 */
static void host_tl_scanEvtCallback(uint32_t event, void *pData, uintptr_t arg)
{
  (void) arg;
  if (HCI_TL_CallbackEvtProcessCB)
  {
    scanEvtCallback_t * scanEvtCallback = NULL;

    if (event != GAP_EVT_INSUFFICIENT_MEMORY)
    {
      scanEvtCallback = ICall_malloc(sizeof(scanEvtCallback_t));
    }

    if (scanEvtCallback)
    {
      scanEvtCallback->event = event;
      scanEvtCallback->pData = pData;

      if( !HCI_TL_CallbackEvtProcessCB((void*) scanEvtCallback, (void*) host_tl_scanEvtCallbackProcess))
      {
        // Not enough Heap...
        if (event == GAP_EVT_ADV_REPORT)
        {
          if(pData)
          {
            if (((GapScan_Evt_AdvRpt_t*)pData)->pData)
            {
              ICall_free(((GapScan_Evt_AdvRpt_t*)pData)->pData);
            }
            ICall_free(pData);
          }
        }
        else
        {
          if(pData)
          {
            ICall_free(pData);
          }
        }
        ICall_free(scanEvtCallback);
        HCI_TL_sendSystemReport(HCI_TL_ID, LL_STATUS_ERROR_OUT_OF_HEAP, HCI_EXT_GAP_ADV_SCAN_EVENT);
      }
    }
    else
    {
      if (event == GAP_EVT_ADV_REPORT)
      {
        if(pData)
        {
          if (((GapScan_Evt_AdvRpt_t*)pData)->pData)
          {
            ICall_free(((GapScan_Evt_AdvRpt_t*)pData)->pData);
          }
          ICall_free(pData);
        }
      }
      else
      {
        if(pData)
        {
          ICall_free(pData);
        }
      }
      HCI_TL_sendSystemReport(HOST_TL_ID, LL_STATUS_ERROR_OUT_OF_HEAP, HCI_EXT_GAP_ADV_SCAN_EVENT);
    }
  }

}

/*********************************************************************
 * @fn      host_tl_scanEvtCallbackProcess
 *
 * @brief   Function callback for the HOST GAP Scanner module.
 *
 * @param   event - the event that trig this callback
 *          length - length of the data link to the event
 *          pData  - pointer to the data linked to the event.
 *
 * @return  none
 */
static void host_tl_scanEvtCallbackProcess(scanEvtCallback_t * scanEvtCallback)
{
  uint32_t event;
  uint8_t *pData;

  if (scanEvtCallback)
  {
    event = scanEvtCallback->event;
    pData = scanEvtCallback->pData;

    //Only One event can be signal at a time
    switch(event)
    {
      case GAP_EVT_SCAN_ENABLED:
      case GAP_EVT_SCAN_PRD_ENDED:
      case GAP_EVT_SCAN_DUR_ENDED:
      case GAP_EVT_SCAN_INT_ENDED:
      case GAP_EVT_SCAN_WND_ENDED:
      {
#ifdef BLE3_CMD
        if ( (event == GAP_EVT_SCAN_INT_ENDED) ||
             (event == GAP_EVT_SCAN_WND_ENDED) )
        {
          // send scan notice event
          uint8_t data[5] = {0};
          data[0] = LO_UINT16(HCI_EXT_SCAN_EVENT_NOTICE_EVENT);
          data[1] = HI_UINT16(HCI_EXT_SCAN_EVENT_NOTICE_EVENT);
          data[2] = SUCCESS;
          data[3] = LO_UINT16(HCI_EXT_SCAN_EVENT_NOTICE);
          data[4] = HI_UINT16(HCI_EXT_SCAN_EVENT_NOTICE);

          HCI_TL_SendVSEvent(data, 5);
        }
        else
        {
          uint8_t *pDataOut = NULL;
          if( maxNumReports )
          {
            uint8_t pktLen = 4 + 8*numDev;
            uint8_t i;

            pDataOut = ICall_malloc(pktLen);
            if( pDataOut )
            {
              uint8_t ind = 4;
              pDataOut[0] = LO_UINT16(HCI_EXT_GAP_DEVICE_DISCOVERY_EVENT);
              pDataOut[1] = HI_UINT16(HCI_EXT_GAP_DEVICE_DISCOVERY_EVENT);
              pDataOut[2] = 0;      // Status
              pDataOut[3] = numDev; // Number of devices discovered
              // Copy the devices information
              for (i = 0; i < numDev; i++)
              {
                pDataOut[ind] = deviceInfoArr[i].evtType;
                ind++;
                pDataOut[ind] = deviceInfoArr[i].addrType;
                ind++;
                VOID memcpy(&pDataOut[ind], deviceInfoArr[i].addr, B_ADDR_LEN);
                ind += B_ADDR_LEN;
              }
              HCI_TL_SendVSEvent(pDataOut, pktLen);
              scanSummarySent = 1;
              numDev = 0;
            }
          }
          if( pDataOut )
          {
            ICall_free(pDataOut);
          }
        }
#else // !BLE3_CMD
        uint8_t data[7];
        data[0] = LO_UINT16(HCI_EXT_GAP_ADV_SCAN_EVENT);
        data[1] = HI_UINT16(HCI_EXT_GAP_ADV_SCAN_EVENT);
        data[2] = 0;  // Status
        data[3] = BREAK_UINT32(event, 0);
        data[4] = BREAK_UINT32(event, 1);
        data[5] = BREAK_UINT32(event, 2);
        data[6] = BREAK_UINT32(event, 3);
        HCI_TL_SendVSEvent(data, sizeof(data));
#endif // BLE3_CMD
        if (pData)
        {
          ICall_free(pData);
        }
        break;
      }

      case GAP_EVT_SCAN_DISABLED:
      {
#ifdef BLE3_CMD
        if( !scanSummarySent )
        {
          uint8_t data[4];
          data[0] = LO_UINT16(HCI_EXT_GAP_DEVICE_DISCOVERY_EVENT);
          data[1] = HI_UINT16(HCI_EXT_GAP_DEVICE_DISCOVERY_EVENT);
          data[2] = bleGAPUserCanceled; // Status
          data[3] = 0; // numDev;
          // Send the event
          HCI_TL_SendVSEvent(data, sizeof(data));
          numDev = 0;
        }
        else
        {
          scanSummarySent = 0;
        }
#else // !BLE3_CMD
        if (pData)
        {
          GapScan_Evt_End_t * advEndRpt = (GapScan_Evt_End_t *) pData;
          uint8_t data[9];
          data[0] = LO_UINT16(HCI_EXT_GAP_ADV_SCAN_EVENT);
          data[1] = HI_UINT16(HCI_EXT_GAP_ADV_SCAN_EVENT);
          data[2] = 0; // Status
          data[3] = BREAK_UINT32(event, 0);
          data[4] = BREAK_UINT32(event, 1);
          data[5] = BREAK_UINT32(event, 2);
          data[6] = BREAK_UINT32(event, 3);
          data[7] = advEndRpt->reason;    //Reason
          data[8] = advEndRpt->numReport; //Number of Report collected
          HCI_TL_SendVSEvent(data, sizeof(data));
          ICall_free(pData);
        }
#endif // BLE_CMD
        if (pData)
        {
          ICall_free(pData);
        }
        break;
      }

      case GAP_EVT_ADV_REPORT:
      {
        host_tl_sendAdvReport(event, (GapScan_Evt_AdvRpt_t *) pData);
        break;
      }

      default:
      {
        if (pData)
        {
          ICall_free(pData);
        }
        break;
      }
    }
    ICall_free(scanEvtCallback);
  }
}


/*********************************************************************
 * @fn      host_tl_sendAdvReport
 *
 * @brief   Send a advertisement report as a vendor specific event.
 *
 * @param   advRpt - poiner to the report.
 *
 * @return  none
 */
static void host_tl_sendAdvReport(uint32_t event, GapScan_Evt_AdvRpt_t * advRpt)
{
#ifndef BLE3_CMD
  hciPacket_t *msg;
  uint16_t totalLength = 0;
#endif
  uint16_t remainingLength;
  uint8_t dataLen;

  if (advRpt == NULL)
  {
      return;
  }
  remainingLength = advRpt->dataLen;

  // Got the Report, Map it to the Extended Report Event...
  do
  {
    //Check Length, if bigger than MAX_REPORT_DATA_SIZE, split it...
    if (remainingLength > MAX_REPORT_DATA_SIZE)
    {
      dataLen = MAX_REPORT_DATA_SIZE;
    }
    else
    {
      dataLen = remainingLength;
    }
#ifdef BLE3_CMD
    // BLE3 reports only Legacy advertisement
    if( advRpt->evtType & 0x10 )
    {
      uint8_t rptLen = 13 + dataLen;
      uint8_t *dataOut = NULL;

      dataOut = (uint8_t *)osal_mem_alloc(rptLen);
      if( dataOut )
      {
        if( (maxNumReports) && (numDev < maxNumReports) ||
            (!maxNumReports))
        {
          // Build the event
          dataOut[0] = LO_UINT16(HCI_EXT_GAP_DEVICE_INFO_EVENT);
          dataOut[1] = HI_UINT16(HCI_EXT_GAP_DEVICE_INFO_EVENT);
          dataOut[2] = 0; // status
          dataOut[3] = getAgamaToBLE3EventProp(advRpt->evtType); // adv type
          dataOut[4] = advRpt->addrType;
          // Peer's address
          osal_memcpy(&dataOut[5], advRpt->addr, B_ADDR_LEN);
          dataOut[11] = (uint8_t)advRpt->rssi;
          dataOut[12] = dataLen;
          // Copy the advData
          osal_memcpy(&dataOut[13], advRpt->pData, dataLen);

          HCI_TL_SendVSEvent(dataOut, rptLen);

          // Saving the device info
          deviceInfoArr[numDev].evtType = dataOut[3];
          deviceInfoArr[numDev].addrType = dataOut[4];
          osal_memcpy(deviceInfoArr[numDev].addr, advRpt->addr, B_ADDR_LEN);
          numDev++;
        }
      }
      //Free the message and the payload
      if (dataOut)
      {
        ICall_free(dataOut);
      }
    }
#else // !BLE3_CMD
    // OSAL message header + HCI event header + data
    totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + \
                  GAP_SCAN_EVENT_HEADER + GAP_SCAN_EVENT_LENGTH + dataLen;

    // allocate memory for OSAL hdr + packet
    msg = (hciPacket_t *)ICall_allocMsg(totalLength);
    if (msg)
    {
      // message type, HCI event type
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // packet
      msg->pData    = (uint8_t*)(msg+1);
      msg->pData[0] = HCI_EVENT_PACKET;
      msg->pData[1] = HCI_VE_EVENT_CODE;
      msg->pData[2] = GAP_SCAN_EVENT_LENGTH + GAP_SCAN_EVENT_HEADER + dataLen;

      msg->pData[3] = LO_UINT16(HCI_EXT_GAP_ADV_SCAN_EVENT);
      msg->pData[4] = HI_UINT16(HCI_EXT_GAP_ADV_SCAN_EVENT);
      msg->pData[5] = 0; // Status
      msg->pData[6] = BREAK_UINT32(event, 0);
      msg->pData[7] = BREAK_UINT32(event, 1);
      msg->pData[8] = BREAK_UINT32(event, 2);
      msg->pData[9] = BREAK_UINT32(event, 3);
      msg->pData[10] = advRpt->evtType;  //Start of the event structure.
      msg->pData[11] = advRpt->addrType;
      memcpy(&msg->pData[12], advRpt->addr, B_ADDR_LEN);
      msg->pData[18] = advRpt->primPhy;
      msg->pData[19] = advRpt->secPhy;
      msg->pData[20] = advRpt->advSid;
      msg->pData[21] = advRpt->txPower;
      msg->pData[22] = advRpt->rssi;
      msg->pData[23] = advRpt->directAddrType;
      memcpy(&msg->pData[24], advRpt->directAddr, B_ADDR_LEN);
      msg->pData[30] = LO_UINT16(advRpt->periodicAdvInt);
      msg->pData[31] = HI_UINT16(advRpt->periodicAdvInt);
      msg->pData[32] = LO_UINT16(dataLen);
      msg->pData[33] = HI_UINT16(dataLen);
      // copy data
      if (advRpt->dataLen)
      {
          memcpy(&msg->pData[34], advRpt->pData + (advRpt->dataLen - remainingLength) , dataLen);
      }

      if (remainingLength > MAX_REPORT_DATA_SIZE)
      {
          // This is not the last packet
          msg->pData[10] &= AE_EVT_TYPE_COMPLETE_MASK;
          msg->pData[10] |= AE_EVT_TYPE_INCOMPLETE_MORE_TO_COME;
      }

      // Send to High Layer.
      if (HCI_TL_CommandStatusCB)
      {
        HCI_TL_CommandStatusCB(msg->pData, HCI_EVENT_MIN_LENGTH + msg->pData[2]);
      }

      // We're done with this message.
      ICall_freeMsg(msg);
    }
    else
    {
      HCI_TL_sendSystemReport(HOST_TL_ID, LL_STATUS_ERROR_OUT_OF_HEAP, HCI_EXT_GAP_ADV_SCAN_EVENT);
    }
#endif // BLE3_CMD
    // Update the local variable to send the rest of the payload.
    remainingLength-=dataLen;
  }while(remainingLength > 0);

  //Free the message and the payload
  if (advRpt)
  {
    if (advRpt->pData)
    {
      ICall_free(advRpt->pData);
    }
    ICall_free(advRpt);
  }
}
#endif //CENTRAL_CFG | OBSERVER_CFG

#if ( HOST_CONFIG & ( PERIPHERAL_CFG | BROADCASTER_CFG ) )
/*********************************************************************
 * @fn      host_tl_advEvtCallback
 *
 * @brief   Function callback for the HOST GAP advertiser module.
 *
 * @param   handle - handle of the advertisement set trigging the event.
 *          event - the event that trig this callback
 *          length - length of the data link to the event
 *          pData  - pointer to the data linked to the event.
 *
 * @return  none
 */
static void host_tl_advEvtCallback(uint32_t event, void *pData, UArg arg)
{
  if (HCI_TL_CallbackEvtProcessCB)
  {
    advEvtCallback_t * advEvtCallback = ICall_malloc(sizeof(advEvtCallback_t));
    if (advEvtCallback)
    {
      advEvtCallback->event = event;
      advEvtCallback->pData = pData;
      if (!HCI_TL_CallbackEvtProcessCB((void*) advEvtCallback, (void*) host_tl_advEvtCallbackProcess))
      {
        // Not enough Heap...
        ICall_free(pData);
        ICall_free(advEvtCallback);
        HCI_TL_sendSystemReport(HCI_TL_ID, LL_STATUS_ERROR_OUT_OF_HEAP, HCI_LE_EVENT_CODE);
      }
    }
    else
    {
     ICall_free(pData);
     HCI_TL_sendSystemReport(HOST_TL_ID, LL_STATUS_ERROR_OUT_OF_HEAP, HCI_EXT_GAP_ADV_SCAN_EVENT);
    }
  }
}

/*********************************************************************
 * @fn      host_tl_advEvtCallback
 *
 * @brief   Function callback for the HOST GAP advertiser module.
 *
 * @param   advEvtCallback - structure containing all info from the event.
 *
 * @return  none
 */
static void host_tl_advEvtCallbackProcess(advEvtCallback_t *advEvtCallback)
{
  if (advEvtCallback)
  {
#ifndef BLE3_CMD
    uint8_t status = SUCCESS;
    uint8_t dataLen = 0;
    // Handle is always the first byte
    uint8_t advHandle = *(uint8_t *)(advEvtCallback->pData);
    // Don't include handle since it was already extracted
    void *pData = (char *)advEvtCallback->pData + 1;
#endif

    uint8_t *dataOut = NULL;
    uint32_t event = advEvtCallback->event;

    switch(event)
    {
#ifdef BLE3_CMD
      case GAP_EVT_ADV_END:
      {
        // Allocate data to send over TL
        uint8_t totalLength = 5;
        dataOut = (uint8_t *)osal_mem_alloc(totalLength);
        dataOut[0] = LO_UINT16(HCI_EXT_ADV_EVENT_NOTICE_EVENT);
        dataOut[1] = HI_UINT16(HCI_EXT_ADV_EVENT_NOTICE_EVENT);
        dataOut[2] = SUCCESS;
        dataOut[3] = LO_UINT16(HCI_EXT_ADV_EVENT_NOTICE);
        dataOut[4] = HI_UINT16(HCI_EXT_ADV_EVENT_NOTICE);

        HCI_TL_SendVSEvent(dataOut, totalLength);
        break;
      }

      case GAP_EVT_ADV_START_AFTER_ENABLE:
      case GAP_EVT_ADV_END_AFTER_DISABLE:
      {
        // Do not use these events for BLE3 Commands
        // for BLE3 use GAP_ADV_MAKE_DISCOVERABLE_DONE_EVENT and GAP_ADV_END_DISCOVERABLE_DONE_EVENT
        break;
      }
#else // !BLE3_CMD
      case GAP_EVT_ADV_SET_TERMINATED:
        // Extract status
        status = ((GapAdv_setTerm_t *)pData)->status;

        // Don't include status as part of data
        pData = &(((GapAdv_setTerm_t *)pData)->connHandle);

        dataLen = sizeof(uint16_t) + // 2 bytes for connHandle
                  sizeof(uint8_t);   // 1 byte for numComAdvEvts
        break;

      case GAP_EVT_SCAN_REQ_RECEIVED:
        dataLen = sizeof(uint8_t) +  // 1 byte for scanAddrType
                  B_ADDR_LEN      +  // Address length
                  sizeof(uint8_t) +  // 1 byte for channel
                  sizeof(uint8_t);   // 1 byte for rssi
        break;

      case GAP_EVT_ADV_DATA_TRUNCATED:
        dataLen = sizeof(uint8_t) +   // 1 byte for availAdvDataLen
                  sizeof(uint16_t);   // 2 byte for advDataLen
        break;

      case GAP_EVT_INSUFFICIENT_MEMORY:
        // TODO
        break;
#endif // BLE3_CMD

      default:
        // The default values set above will be used
        break;
    }

#ifndef BLE3_CMD
    // Allocate data to send over TL
    uint8_t totalLength = HCI_EXT_GAP_ADV_EVENT_MIN_LENGTH + dataLen;
    dataOut = (uint8_t *)osal_mem_alloc(totalLength);

    // If data was allocated
    if(dataOut)
    {
      dataOut[0] = LO_UINT16(HCI_EXT_GAP_ADV_SCAN_EVENT);
      dataOut[1] = HI_UINT16(HCI_EXT_GAP_ADV_SCAN_EVENT);
      dataOut[2] = status;
      dataOut[3] = BREAK_UINT32(event, 0);
      dataOut[4] = BREAK_UINT32(event, 1);
      dataOut[5] = BREAK_UINT32(event, 2);
      dataOut[6] = BREAK_UINT32(event, 3);
      dataOut[7] = advHandle;
      dataOut[8] = dataLen;
      osal_memcpy(&dataOut[9], (uint8_t *) pData, dataLen);

      HCI_TL_SendVSEvent(dataOut, totalLength);
    }
#endif // !BLE3_CMD

    //Free the message and the payload
    if (dataOut)
    {
      ICall_free(dataOut);
    }

    if (advEvtCallback->pData)
    {
      ICall_free(advEvtCallback->pData);
    }

    if (advEvtCallback)
    {
      ICall_free(advEvtCallback);
    }
  }
}
#endif // ( PERIPHERAL_CFG | BROADCASTER_CFG )

#endif /* (PTM_MODE) */

#ifndef HOST_CONFIG

#if defined(CTRL_CONFIG) && (CTRL_CONFIG & (ADV_NCONN_CFG | ADV_CONN_CFG))
/*********************************************************************
 * @fn      hci_tl_managedAEdata
 *
 * @brief   perform Adv and Scan data update.
 *
 * @param   mode - opCode of the command setting the data.
 *          pCmdData - pointer to the parameter structure describing the data to set.
 *          pData    - pointer to the data to set.
 *
 * @return  none
 */
static void hci_tl_managedAEdata(uint16_t mode, aeSetDataCmd_t *pCmdData, uint8_t *pData)
{
    llStatus_t status = LL_STATUS_ERROR_BAD_PARAMETER;

    // The hypothesis is that setting fragmentat of advertisement payload will be
    // done for only one Advertisement Set at a time:
    // - no interleaving between adv set
    // - no interleaving between the mode (adv data or scan response data)
    static uint8_t *pFragmentedData = NULL;
    static uint16_t fragmentedDataLen = 0;

    {
        switch(pCmdData->operation)
        {
            case AE_DATA_OP_NEXT_FRAG:
            {
                if ( (pCmdData->dataLen == 0) || (!pFragmentedData))
                {
                  status = LL_STATUS_ERROR_BAD_PARAMETER;
                  break;
                }

                pFragmentedData = hci_tl_appendPendingData(pFragmentedData, fragmentedDataLen, pData, pCmdData->dataLen);
                if (!pFragmentedData)
                {
                    status = LL_STATUS_ERROR_OUT_OF_HEAP;
                    fragmentedDataLen = 0;
                    break;
                }
                fragmentedDataLen += pCmdData->dataLen;

                // Mock the Link Layer Status
                status = LL_STATUS_SUCCESS;
                break;
            }
            case AE_DATA_OP_FIRST_FRAG:
            {
                if (pCmdData->dataLen == 0)
                {
                  status = LL_STATUS_ERROR_BAD_PARAMETER;
                  break;
                }

                if (pFragmentedData)
                {
                  // The buffer was already preallocated, free it and restart from the beginning.
                  hci_tl_removePendingData(pFragmentedData);
                  fragmentedDataLen = 0;
                }

                // Passing NULL as the first parameter prevent to free any existing data.
                // Data will be used only if the fragmentation is successfull, meaning:
                // First --> intermediate --> Last segment  happens.
                // if the sequence does not happens, all pre-allocated buffer will be free
                // but the existing data will still exist and be used.
                pFragmentedData = hci_tl_createPendingData(NULL, pCmdData->dataLen, pData);
                if (!pFragmentedData)
                {
                    status = LL_STATUS_ERROR_OUT_OF_HEAP;
                    break;
                }
                fragmentedDataLen = pCmdData->dataLen;

                // Mock the Link Layer Status
                status = LL_STATUS_SUCCESS;
                break;
            }
            case AE_DATA_OP_LAST_FRAG:
            {
                // @todo: Need to track if Adv is enable,
                // if Adv. is enable, this command should be rejected...
                if ((pCmdData->dataLen == 0) || (!pFragmentedData))
                {
                  status = LL_STATUS_ERROR_BAD_PARAMETER;
                  break;
                }

                pFragmentedData = hci_tl_appendPendingData(pFragmentedData, fragmentedDataLen, pData, pCmdData->dataLen);
                if (!pFragmentedData)
                {
                    status = LL_STATUS_ERROR_OUT_OF_HEAP;
                    fragmentedDataLen = 0;
                    break;
                }
                fragmentedDataLen += pCmdData->dataLen;

                // Assign the data to the set
                if (pCmdData->pData)
                {
                  ICall_free(pCmdData->pData);
                }
                pCmdData->pData = pFragmentedData;

                //Overwrite the command parameter structure.
                pCmdData->dataLen = fragmentedDataLen;
                pCmdData->operation = AE_DATA_OP_COMPLETE;

                // Reset the temporary buffer to NULL, so it can be reused properly
                pFragmentedData = NULL;
                fragmentedDataLen = 0;

                if (mode == HCI_LE_SET_EXT_ADV_DATA)
                {
                    status = LE_SetExtAdvData(pCmdData);
                }
                else if (mode == HCI_LE_SET_EXT_SCAN_RESPONSE_DATA)
                {
                    // Scan Response Data
                    status = LE_SetExtScanRspData(pCmdData);
                }
                else
                {
                    // other mode like HCI_EXT_LE_SET_EXT_ADV_DATA should
                    // never be used with fragmentation
                    status = LL_STATUS_ERROR_BAD_PARAMETER;
                    // Failure, free the pre-allocated buffer...
                    hci_tl_removePendingData(pCmdData->pData);
                    break;
                }

                if (status != LL_STATUS_SUCCESS)
                {
                    // Failure, free the pre-allocated buffer...
                    hci_tl_removePendingData(pCmdData->pData);
                }
                break;
            }
            case AE_DATA_OP_UNCHANGED:
            {
                if ((mode == HCI_LE_SET_EXT_ADV_DATA) ||
                    (mode == HCI_EXT_LE_SET_EXT_ADV_DATA))
                {
                  status = LE_SetExtAdvData(pCmdData);
                }
                else
                {
                  status = LL_STATUS_ERROR_BAD_PARAMETER;
                }
                break;
            }
            case AE_DATA_OP_COMPLETE:
            {
                pCmdData->pData = hci_tl_createPendingData(pCmdData->pData, pCmdData->dataLen, pData);

                if(pCmdData->pData)
                {
                    if ((mode == HCI_LE_SET_EXT_ADV_DATA)     ||
                        (mode == HCI_EXT_LE_SET_EXT_ADV_DATA) ||
                        (mode == HCI_LE_SET_ADV_DATA))
                    {
                        status = LE_SetExtAdvData(pCmdData);
                    }
                    else
                    {
                        // Scan Response Data
                        status = LE_SetExtScanRspData(pCmdData);
                    }

                    if (status != LL_STATUS_SUCCESS)
                    {
                        // Failure, free the pre-allocated buffer...
                        hci_tl_removePendingData(pCmdData->pData);
                    }
                }
                else
                {
                    status = LL_STATUS_ERROR_OUT_OF_HEAP;
                }
                break;
            }
            default:
                status = LL_STATUS_ERROR_BAD_PARAMETER;
                break;
        }
    }
    HCI_CommandCompleteEvent(mode,
                             sizeof(status),
                             &status);
}

/*********************************************************************
 * @fn      hci_tl_removePendingData
 *
 * @brief   remove any pending Adv data setup that was not complete..
 *
 * @param   pendingAdvData - pointer to teh data to remove
 *
 * @return  none
 */
static void hci_tl_removePendingData(uint8_t* pendingAdvData)
{
    if (pendingAdvData)
    {
        ICall_free(pendingAdvData);
    }
}

/*********************************************************************
 * @fn      hci_tl_createPendingData
 *
 * @brief   Copy the payload to a new allocated buffer.
 *          if the pointer to the previous data is not NULL, then it
 *          will be free first then reassign.
 *
 * @param   pStorage - pointer to the previous storage of the data
 *          len - length of the data payload
 *          pData - pointer to the data payload
 *
 *
 * @return  return new pointer to the new allocated buffer
 */
static uint8_t* hci_tl_createPendingData(uint8_t *pStorage, uint16_t len, uint8_t* pData)
{
    uint8_t* pDataStore;
    pDataStore = ICall_malloc(len);
    if (pDataStore)
    {
        memcpy(pDataStore, pData, len);
    }
    if(pStorage)
    {
        ICall_free(pStorage);
    }
    return pDataStore;

}
/*********************************************************************
 * @fn      hci_tl_appendPendingData
 *
 * @brief   append a data buffer after another one.
 *          a new buffer will be allocated, then the initial buffer will be copy into it.
 *          the new data will be appended, then the initial buffer will be free.
 *
 * @param   pStorage - pointer to the initial storage of the data
 *          len - length of the initial storage
 *          pData - pointer to the data payload to append
 *          len - length of the data payload to append
 *
 *
 * @return  return new pointer to the new allocated buffer
 */
static uint8_t* hci_tl_appendPendingData(uint8_t *pStorage, uint16_t lenStorage,
                                         uint8_t* pData, int16_t len)
{
    uint8_t* pDataStore;

    if(pStorage)
    {
        pDataStore = ICall_malloc(lenStorage + len);
        if (!pDataStore)
        {
            // Not enough Heap, free everything.
            ICall_free(pStorage);
            return pDataStore;
        }
        memcpy(pDataStore, pStorage, lenStorage);
        memcpy(pDataStore + lenStorage, pData, len);
        ICall_free(pStorage);
    }
    else
    {
        // NULL pointer pass has paramameter, wrong behavior, free everything.
        return NULL;
    }
    return pDataStore;
}

/*********************************************************************
 * @fn      hci_tl_GetAdvSet
 *
 * @brief   Get a set from the AE linked list. if the set does not exist,
 *          creates one.
 *
 * @param   handle - handle of the set to add.
 *
 * @return  the pointer to the set, or NULL if out of memory to allocated the set.
 */
static hci_tl_advSet_t *hci_tl_GetAdvSet(uint8_t handle)
{
    hci_tl_advSet_t *pAdvSet = hci_tl_advSetList;

    // search for handle
    while(pAdvSet)
    {
      if (handle == pAdvSet->handle)
      {
        return(pAdvSet);
      }

      if (pAdvSet->next == NULL)
      {
        break;
      }

      // advance to next entry
      pAdvSet = pAdvSet->next;
    }

    // check if we have a valid pointer
    // Note: A valid pointer means the list wasn't empty, and we have last entry.
    //       A NULL pointer means the list was empty.
    if (pAdvSet)
    {
        pAdvSet->next = ICall_malloc(sizeof(hci_tl_advSet_t));
        pAdvSet = pAdvSet->next;
        if (pAdvSet)
        {
            memset(pAdvSet, 0, sizeof(hci_tl_advSet_t));
        }
    }
    else // Adv Set List is empty
    {
        pAdvSet = (hci_tl_advSet_t *) ICall_malloc(sizeof(hci_tl_advSet_t));
        hci_tl_advSetList = pAdvSet;
        if (pAdvSet)
        {
            memset(pAdvSet, 0, sizeof(hci_tl_advSet_t));
        }
    }
    if (pAdvSet)
    {
      // set the handle
      pAdvSet->handle = handle;
    }
    return(pAdvSet);

}
/*********************************************************************
 * @fn      hci_tl_RemoveAdvSet
 *
 * @brief   remove a set from the AE linked list.
 *
 * @param   handle - handle of the set to remove.
 *
 * @return  none.
 */
static void hci_tl_RemoveAdvSet(uint8_t handle)
{
  hci_tl_advSet_t *pPre = hci_tl_advSetList;
  hci_tl_advSet_t *pCur = hci_tl_advSetList;

  while(pCur)
  {
    if (pCur->handle == handle)
    {
      // chain over the handle to be removed
      pPre->next = pCur->next;

      if (pCur == hci_tl_advSetList)
      {
          hci_tl_advSetList = pCur->next;
      }

      // remove advertising set
      ICall_free(pCur);
      if (pCur->advCmdData.pData)
      {
          ICall_free(pCur->advCmdData.pData);
      }
      if (pCur->scanCmdData.pData)
      {
          ICall_free(pCur->scanCmdData.pData);
      }
      return;
    }

    // not found yet, so on to next entry in list
    pPre = pCur;
    pCur = pCur->next;
  }
}
/*********************************************************************
 * @fn      hci_tl_ClearAdvSet
 *
 * @brief   remove a set from the AE linked list.
 *
 * @param   handle - handle of the set to remove.
 *
 * @return  none.
 */
static void hci_tl_ClearAdvSet(void)
{
  hci_tl_advSet_t *pCurr = hci_tl_advSetList;

  while(pCurr)
  {
    // move head to next, if any
    hci_tl_advSetList = pCurr->next;

    if (pCurr->advCmdData.pData)
    {
        ICall_free(pCurr->advCmdData.pData);
    }

    if (pCurr->scanCmdData.pData)
    {
        ICall_free(pCurr->scanCmdData.pData);
    }

    // free what was at head
    ICall_free(pCurr);

    // point to next head, if any
    pCurr = hci_tl_advSetList;
  }

  hci_tl_advSetList = NULL;
}
#endif // ADV_NCONN_CFG || ADV_CONN_CFG
#endif // !HOST_CONFIG

/*********************************************************************
 * @fn      HCI_TL_SendDataPkt
 *
 * @brief   Interprets an HCI data packet and sends to the Stack.
 *
 * @param   pMsg - HCI data serial packet to translate.
 *
 * @return  none.
 */
static void HCI_TL_SendDataPkt(uint8_t *pMsg)
{
#ifndef HOST_CONFIG
  /* Host configurstion should never access directly HCI_TL_SendDataPkt */
  hciDataPacket_t *pDataPkt = (hciDataPacket_t *) pMsg;

  // LE only accepts Data packets of type ACL.
  if (pDataPkt->pktType == HCI_ACL_DATA_PACKET)
  {
    uint8_t *pData = pDataPkt->pData;

    // Replace data with bm data
    pDataPkt->pData = (uint8_t *) HCI_bm_alloc(pDataPkt->pktLen);

    if (pDataPkt->pData)
    {
      memcpy(pDataPkt->pData, pData, pDataPkt->pktLen);

      HCI_SendDataPkt(pDataPkt->connHandle,
                      pDataPkt->pbFlag,
                      pDataPkt->pktLen,
                      pDataPkt->pData);
    }
  }
#endif /* HOST_CONFIG */
}

static void HCI_TL_SendVSEvent(uint8_t *pBuf, uint16_t dataLen)
{
  hciPacket_t *msg;
  uint16_t totalLength;

  // OSAL message header + HCI event header + data
  totalLength = sizeof(hciPacket_t) + HCI_EVENT_MIN_LENGTH + dataLen;

  // Check for overlap - dataLen is too big, drop packet
  if (totalLength < dataLen)
  {
    return;
  }

  // allocate memory for OSAL hdr + packet
  msg = (hciPacket_t *)ICall_allocMsg(totalLength);

  if (msg)
  {
    // If data length is smaller than 0x100 bytes, process as regular event
    if (dataLen < 0x100)
    {
      // message type, HCI event type
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      // packet
      msg->pData    = (uint8_t*)(msg+1);
      msg->pData[0] = HCI_EVENT_PACKET;
      msg->pData[1] = HCI_VE_EVENT_CODE;
      msg->pData[2] = dataLen;
      // copy data
      if (dataLen)
      {
        memcpy(&(msg->pData[3]), pBuf, dataLen);
      }
    }
    // If data length is larger than 0x00FF bytes, process as extended event
    else
    {
      // message type, HCI extended event type
      msg->hdr.event  = HCI_CTRL_TO_HOST_EVENT;
      msg->hdr.status = 0xFF;

      msg->pData    = (uint8_t*)(msg+1);
      msg->pData[0] = HCI_EXTENDED_EVENT_PACKET;
      msg->pData[1] = LO_UINT16(dataLen);
      msg->pData[2] = HI_UINT16(dataLen);
      memcpy(&(msg->pData[3]), pBuf, dataLen);
    }
    // Send to High Layer.
    if (HCI_TL_CommandStatusCB)
    {
      HCI_TL_CommandStatusCB(msg->pData, HCI_EVENT_MIN_LENGTH + dataLen);
    }

    // We're done with this message.
    ICall_freeMsg(msg);
  }
}

/************************** Host Command Translation **************************/
#ifdef HOST_CONFIG

#if !defined (PTM_MODE)
// PTM_MODE does not interact with teh host (L2CAP, GAP, ATT, Etc...)
/*********************************************************************
 * @fn      HCI_TL_processStructuredEvent
 *
 * @brief   Interprets a structured Event from the BLE Host and serializes it.
 *
 * @param   pEvt - structured event to serialize.
 *
 * @return  TRUE to deallocate pEvt message, False otherwise.
 */
uint8_t HCI_TL_processStructuredEvent(ICall_Hdr *pEvt)
{
  return(processEvents(pEvt));
}

/*********************************************************************
 * @fn      HCI_TL_SendExtensionCmdPkt
 *
 * @brief   Interprets an HCI extension command packet and sends to the Stack.
 *
 * @param   pMsg - HCI command serial packet to translate.
 *
 * @return  none.
 */
static void HCI_TL_SendExtensionCmdPkt(hciPacket_t *pMsg)
{
  // Process Extension messages not bound for the HCI layer.
  processExtMsg(pMsg);
}


/*********************************************************************
 * @fn      processExtMsg
 *
 * @brief   Parse and process incoming HCI extension messages.
 *
 * @param   pMsg - incoming HCI extension message.
 *
 * @return  none
 */
static uint8_t processExtMsg(hciPacket_t *pMsg)
{
  uint8_t deallocateIncoming;
  bStatus_t stat = SUCCESS;
  uint8_t rspDataLen = 0;
  hciExtCmd_t msg;
  uint8_t *pBuf = pMsg->pData;

  // Parse the header
  msg.pktType = *pBuf++;
  msg.opCode = BUILD_UINT16(pBuf[0], pBuf[1]);
  pBuf += 2;

  if (msg.pktType == HCI_EXTENDED_CMD_PACKET)
  {
    msg.len = BUILD_UINT16(*pBuf,*(pBuf+1)) ;
    pBuf+=2;
  }
  else
  {
    msg.len = *pBuf++;
  }
  msg.pData = pBuf;

  switch(msg.opCode >> 7)
  {
#if defined(HOST_CONFIG)
#if (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
    case HCI_EXT_L2CAP_SUBGRP:
      stat = processExtMsgL2CAP((msg.opCode & 0x007F), &msg, &rspDataLen);
      break;

    case HCI_EXT_ATT_SUBGRP:
      stat = processExtMsgATT((msg.opCode & 0x007F), &msg);
      break;

    case HCI_EXT_GATT_SUBGRP:
      stat = processExtMsgGATT((msg.opCode & 0x007F), &msg, &rspDataLen);
      break;
#endif /* (CENTRAL_CFG | PERIPHERAL_CFG) */
    case HCI_EXT_GAP_SUBGRP:
      stat = processExtMsgGAP((msg.opCode & 0x007F), &msg, &rspDataLen);
      break;
#endif // HOST_CONFIG

    case HCI_EXT_UTIL_SUBGRP:
      stat = processExtMsgUTIL((msg.opCode & 0x007F), &msg, &rspDataLen);
      break;

    default:
      stat = FAILURE;
      break;
  }

  deallocateIncoming = FALSE;

  // Send back an immediate response
  rspBuf[0] = LO_UINT16(HCI_EXT_GAP_CMD_STATUS_EVENT);
  rspBuf[1] = HI_UINT16(HCI_EXT_GAP_CMD_STATUS_EVENT);
  rspBuf[2] = stat;
  rspBuf[3] = LO_UINT16(0xFC00 | msg.opCode);
  rspBuf[4] = HI_UINT16(0xFC00 | msg.opCode);
  rspBuf[5] = rspDataLen;

  // IMPORTANT!! Fill in Payload (if needed) in case statement

  HCI_TL_SendVSEvent(rspBuf, (6 + rspDataLen));

#ifdef BLE3_CMD
  if( sendEstEvt )
  {
    // An error occured when "HCI_EXT_GAP_EST_LINK_REQ" was called
    uint8_t pDataOut[20] = {0};

    pDataOut[0] = LO_UINT16(HCI_EXT_GAP_LINK_ESTABLISHED_EVENT);
    pDataOut[1] = HI_UINT16(HCI_EXT_GAP_LINK_ESTABLISHED_EVENT);
    pDataOut[2] = bleIncorrectMode;
    pDataOut[12] = GAP_PROFILE_CENTRAL;

    HCI_TL_SendVSEvent(pDataOut, 20);
    sendEstEvt = 0;
  }

#if ( HOST_CONFIG & ( PERIPHERAL_CFG | BROADCASTER_CFG ) )
  if( maxConnReached )
  {
    uint8_t pDataOut[3] = {0};

    // Send Make discoverable done with max connection reached
    pDataOut[0] = LO_UINT16(HCI_EXT_GAP_MAKE_DISCOVERABLE_DONE_EVENT);
    pDataOut[1] = HI_UINT16(HCI_EXT_GAP_MAKE_DISCOVERABLE_DONE_EVENT);
    pDataOut[2] = HCI_ERROR_CODE_CONN_LIMIT_EXCEEDED;

    HCI_TL_SendVSEvent(pDataOut, 3);
    maxConnReached = 0;
  }
#endif // HOST_CONFIG & ( PERIPHERAL_CFG | BROADCASTER_CFG )

#endif// BLE3_CMD

  return(deallocateIncoming);
}

/*********************************************************************
 * @fn      processExtMsgUTIL
 *
 * @brief   Parse and process incoming HCI extension UTIL messages.
 *
 * @param   cmdID - incoming HCI extension command ID.
 * @param   pCmd - incoming HCI extension message.
 * @param   pRspDataLen - response data length to be returned.
 *
 * @return  SUCCESS, INVALIDPARAMETER and FAILURE.
 */
static uint8_t processExtMsgUTIL(uint8_t cmdID, hciExtCmd_t *pCmd, uint8_t *pRspDataLen)
{
  bStatus_t stat = SUCCESS;

  *pRspDataLen = 0;

  switch(cmdID)
  {
    case HCI_EXT_UTIL_NV_READ:
      {
        uint8_t *pBuf = pCmd->pData;
        osalSnvId_t id  = BUILD_UINT16(pBuf[0], pBuf[1]);
        osalSnvLen_t len = pBuf[2];

        // This has a limitation of only allowing a max data length because of the fixed buffer.
        if ((len < MAX_RSP_DATA_LEN) && (checkNVLen(id, len) == SUCCESS))
        {
          stat = osal_snv_read(id, len, &rspBuf[RSP_PAYLOAD_IDX]);
          if (stat == SUCCESS)
          {
            *pRspDataLen = pBuf[2];
          }
        }
        else
        {
          stat = INVALIDPARAMETER;
        }
      }
      break;

    case HCI_EXT_UTIL_NV_WRITE:
      {
        uint8_t *pBuf = pCmd->pData;
        osalSnvId_t id  = BUILD_UINT16(pBuf[0], pBuf[1]);
        osalSnvLen_t len = pBuf[2];
        if (checkNVLen(id, len) == SUCCESS)
        {
          stat = osal_snv_write(id, len, &pBuf[3]);
        }
        else
        {
          stat = INVALIDPARAMETER;
        }
      }
      break;

    case HCI_EXT_UTIL_BUILD_REV:
      {
        ICall_BuildRevision buildRev;

        VOID buildRevision(&buildRev);

        // Stack revision
        //  Byte 0: Major
        //  Byte 1: Minor
        //  Byte 2: Patch
        rspBuf[RSP_PAYLOAD_IDX]   = BREAK_UINT32(buildRev.stackVersion, 0);
        rspBuf[RSP_PAYLOAD_IDX+1] = BREAK_UINT32(buildRev.stackVersion, 1);
        rspBuf[RSP_PAYLOAD_IDX+2] = BREAK_UINT32(buildRev.stackVersion, 2);

        // Build revision
        rspBuf[RSP_PAYLOAD_IDX+3] = LO_UINT16(buildRev.buildVersion);
        rspBuf[RSP_PAYLOAD_IDX+4] = HI_UINT16(buildRev.buildVersion);

        // Stack info (Byte 5)
        rspBuf[RSP_PAYLOAD_IDX+5] = buildRev.stackInfo;

        // Controller info - part 1 (Byte 6)
        rspBuf[RSP_PAYLOAD_IDX+6] = LO_UINT16(buildRev.ctrlInfo);

        // Controller info - part 2 (Byte 7)
        rspBuf[RSP_PAYLOAD_IDX+7] = 0; // reserved

        // Host info - part 1 (Byte 8)
        rspBuf[RSP_PAYLOAD_IDX+8] = LO_UINT16(buildRev.hostInfo);

        // Host info - part 2 (Byte 9)
        rspBuf[RSP_PAYLOAD_IDX+9] = 0; // reserved

        *pRspDataLen = 10;
      }
      break;
    case HCI_EXT_UTIL_GET_MEM_STATS:
      {
        uint8_t memStatCmd  = pCmd->pData[0];
        HCI_TL_getMemStats(memStatCmd, &rspBuf[RSP_PAYLOAD_IDX], pRspDataLen);
      }
      break;


    default:
      stat = FAILURE;
      break;
  }

  return(stat);
}



#if (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
/*********************************************************************
 * @fn      processExtMsgL2CAP
 *
 * @brief   Parse and process incoming HCI extension L2CAP messages.
 *
 * @param   pCmd - incoming HCI extension message.
 *
 * @return  SUCCESS or FAILURE
 */
static uint8_t processExtMsgL2CAP(uint8_t cmdID, hciExtCmd_t *pCmd, uint8_t *pRspDataLen)
{
  uint8_t *pBuf = pCmd->pData;
  uint16_t connHandle = BUILD_UINT16(pBuf[0], pBuf[1]); // connHandle, CID or PSM
  l2capSignalCmd_t cmd;
  bStatus_t stat;

  switch(cmdID)
  {
#if defined(BLE_V41_FEATURES) && (BLE_V41_FEATURES & L2CAP_COC_CFG)
    case HCI_EXT_L2CAP_DATA:
      if (pCmd->len > 2)
      {
        uint8_t *pPayload = createMsgPayload(&pBuf[2], pCmd->len-2);
        if (pPayload != NULL)
        {
          l2capPacket_t pkt;

          pkt.CID = connHandle; // connHandle is CID here
          pkt.pPayload = pPayload;
          pkt.len = pCmd->len-2;

          // Send SDU over dynamic channel
          stat = L2CAP_SendSDU(&pkt);
          if (stat != SUCCESS)
          {
            VOID ICall_free(pPayload);
          }
        }
        else
        {
          stat = bleMemAllocError;
        }
      }
      else
      {
        stat = FAILURE;
      }
      break;

    case HCI_EXT_L2CAP_REGISTER_PSM:
      if (pCmd->len == 10)
      {
        l2capPsm_t psm;

        psm.psm = connHandle; // connHandle is PSM here
        psm.mtu = BUILD_UINT16(pBuf[2], pBuf[3]);
        psm.initPeerCredits = BUILD_UINT16(pBuf[4], pBuf[5]);
        psm.peerCreditThreshold = BUILD_UINT16(pBuf[6], pBuf[7]);
        psm.maxNumChannels = pBuf[8];
        psm.pfnVerifySecCB = pBuf[9] ? l2capVerifySecCB : NULL;
        psm.taskId = PROXY_ID(appTaskID);

        stat = L2CAP_RegisterPsm(&psm);
      }
      else
      {
        stat = FAILURE;
      }
      break;

    case HCI_EXT_L2CAP_DEREGISTER_PSM:
      if (pCmd->len == 2)
      {
        stat = L2CAP_DeregisterPsm(appTaskID, connHandle); // connHandle is PSM here
      }
      else
      {
        stat = FAILURE;
      }
      break;

    case HCI_EXT_L2CAP_PSM_INFO:
      if (pCmd->len == 2)
      {
        l2capPsmInfo_t info;

        stat = L2CAP_PsmInfo(connHandle, &info); // connHandle is PSM here
        if (stat == SUCCESS)
        {
          *pRspDataLen = 10;
          rspBuf[RSP_PAYLOAD_IDX]   = LO_UINT16(info.mtu);
          rspBuf[RSP_PAYLOAD_IDX+1] = HI_UINT16(info.mtu);

          rspBuf[RSP_PAYLOAD_IDX+2] = LO_UINT16(info.mps);
          rspBuf[RSP_PAYLOAD_IDX+3] = HI_UINT16(info.mps);

          rspBuf[RSP_PAYLOAD_IDX+4] = LO_UINT16(info.initPeerCredits);
          rspBuf[RSP_PAYLOAD_IDX+5] = HI_UINT16(info.initPeerCredits);

          rspBuf[RSP_PAYLOAD_IDX+6] = LO_UINT16(info.peerCreditThreshold);
          rspBuf[RSP_PAYLOAD_IDX+7] = HI_UINT16(info.peerCreditThreshold);

          rspBuf[RSP_PAYLOAD_IDX+8] = info.maxNumChannels;
          rspBuf[RSP_PAYLOAD_IDX+9] = info.numActiveChannels;
        }
      }
      else
      {
        stat = FAILURE;
      }
      break;

    case HCI_EXT_L2CAP_PSM_CHANNELS:
      if (pCmd->len == 2)
      {
        l2capPsmInfo_t info;

        stat = L2CAP_PsmInfo(connHandle, &info); // connHandle is PSM here
        if ((stat == SUCCESS) && (info.numActiveChannels > 0))
        {
          uint8_t numCIDs = info.numActiveChannels;

          uint16_t *pCIDs = (uint16_t *)ICall_malloc(sizeof(uint16_t) * numCIDs);
          if (pCIDs != NULL)
          {
            stat = L2CAP_PsmChannels(connHandle, numCIDs, pCIDs);
            if (stat == SUCCESS)
            {
              uint8_t *pRspBuf = &rspBuf[RSP_PAYLOAD_IDX];
              uint8_t i;

              for (i = 0; i < numCIDs; i++)
              {
                *pRspBuf++ = LO_UINT16(pCIDs[i]);
                *pRspBuf++ = HI_UINT16(pCIDs[i]);
              }

              *pRspDataLen = numCIDs * 2;
            }

            VOID ICall_free(pCIDs);
          }
          else
          {
            stat = bleMemAllocError;
          }
        }
      }
      else
      {
        stat = FAILURE;
      }
      break;

    case HCI_EXT_L2CAP_CHANNEL_INFO:
      if (pCmd->len == 2)
      {
        l2capChannelInfo_t channelInfo;

        stat = L2CAP_ChannelInfo(connHandle, &channelInfo); // connHandle is CID here
        if (stat == SUCCESS)
        {
          rspBuf[RSP_PAYLOAD_IDX] = channelInfo.state;

          *pRspDataLen = 1 + buildCoChannelInfo(connHandle, &channelInfo.info,
                                                 &rspBuf[RSP_PAYLOAD_IDX+1]);
        }
      }
      else
      {
        stat = FAILURE;
      }
      break;

    case L2CAP_CONNECT_IND:
      if (pCmd->len == 6)
      {
        uint16_t psm = BUILD_UINT16(pBuf[2], pBuf[3]);
        uint16_t peerPsm = BUILD_UINT16(pBuf[4], pBuf[5]);

        stat = L2CAP_ConnectReq(connHandle, psm, peerPsm);
      }
      else
      {
        stat = FAILURE;
      }
      break;

    case L2CAP_CONNECT_RSP:
      if (pCmd->len == 5)
      {
        uint16_t result = BUILD_UINT16(pBuf[3], pBuf[4]);

        stat = L2CAP_ConnectRsp(connHandle, pBuf[2], result);
      }
      else
      {
        stat = FAILURE;
      }
      break;

    case L2CAP_DISCONNECT_REQ:
      if (pCmd->len == 2)
      {
        stat = L2CAP_DisconnectReq(connHandle); // connHandle is CID here
      }
      else
      {
        stat = FAILURE;
      }
      break;

    case L2CAP_FLOW_CTRL_CREDIT:
      stat = L2CAP_ParseFlowCtrlCredit(&cmd, pBuf, pCmd->len);
      if (stat == SUCCESS)
      {
        stat = L2CAP_FlowCtrlCredit(cmd.credit.CID, cmd.credit.credits);
      }
      break;
#endif //(BLE_V41_FEATURES & L2CAP_COC_CFG)

    case L2CAP_PARAM_UPDATE_REQ:
      stat = L2CAP_ParseParamUpdateReq(&cmd, &pBuf[2], pCmd->len-2);
      if (stat == SUCCESS)
      {
        stat =  L2CAP_ConnParamUpdateReq(connHandle, &cmd.updateReq, appTaskID);
      }
      break;

    case L2CAP_INFO_REQ:
      stat = L2CAP_ParseInfoReq(&cmd, &pBuf[2], pCmd->len-2);
      if (stat == SUCCESS)
      {
        stat = L2CAP_InfoReq(connHandle, &cmd.infoReq, appTaskID);
      }
      break;

    default:
      stat = FAILURE;
      break;
  }

  return(stat);
}

/*********************************************************************
 * @fn      processExtMsgATT
 *
 * @brief   Parse and process incoming HCI extension ATT messages.
 *
 * @param   pCmd - incoming HCI extension message.
 *
 * @return  SUCCESS, INVALIDPARAMETER, FAILURE,
 *          bleInvalidPDU, bleInsufficientAuthen,
 *          bleInsufficientKeySize, bleInsufficientEncrypt or bleMemAllocError
 */
static uint8_t processExtMsgATT(uint8_t cmdID, hciExtCmd_t *pCmd)
{
  static uint8_t numPrepareWrites = 0;
  static attPrepareWriteReq_t *pPrepareWrites = NULL;
  uint8_t *pBuf, *pPayload = NULL, safeToDealloc = TRUE;
  uint16_t connHandle;
  attMsg_t msg;
  bStatus_t stat = bleInvalidPDU;

  // Make sure received buffer contains at lease connection handle (2 otects)
  if (pCmd->len < 2)
  {
    return(stat);
  }

  pBuf = pCmd->pData;
  connHandle = BUILD_UINT16(pBuf[0], pBuf[1]);

  switch(cmdID)
  {
    case ATT_ERROR_RSP:
      if (ATT_ParseErrorRsp(&pBuf[2], pCmd->len-2, &msg) == SUCCESS)
      {
        stat = ATT_ErrorRsp(connHandle, &msg.errorRsp);
      }
      break;

    case ATT_EXCHANGE_MTU_REQ:
      if (ATT_ParseExchangeMTUReq(ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                    pCmd->len-2, &msg) == SUCCESS)
      {
        stat = GATT_ExchangeMTU(connHandle, &msg.exchangeMTUReq, appTaskID);
      }
      break;

    case ATT_EXCHANGE_MTU_RSP:
      if (ATT_ParseExchangeMTURsp(&pBuf[2], pCmd->len-2, &msg) == SUCCESS)
      {
        stat = ATT_ExchangeMTURsp(connHandle, &msg.exchangeMTURsp);
      }
      break;

    case ATT_FIND_INFO_REQ:
      if (ATT_ParseFindInfoReq(ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                 pCmd->len-2, &msg) == SUCCESS)
      {
        stat = GATT_DiscAllCharDescs(connHandle, msg.findInfoReq.startHandle,
                                      msg.findInfoReq.endHandle, appTaskID);
      }
      break;

    case ATT_FIND_INFO_RSP:
      pPayload = createMsgPayload(&pBuf[2], pCmd->len-2);
      if (pPayload != NULL)
      {
        if (ATT_ParseFindInfoRsp(pPayload, pCmd->len-2, &msg) == SUCCESS)
        {
          stat = ATT_FindInfoRsp(connHandle, &msg.findInfoRsp);
          if (stat == SUCCESS)
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_FIND_BY_TYPE_VALUE_REQ:
      if (ATT_ParseFindByTypeValueReq(ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                        pCmd->len-2, &msg) == SUCCESS)
      {
        attFindByTypeValueReq_t *pReq = &msg.findByTypeValueReq;

        // Find out what's been requested
        if (gattPrimaryServiceType(pReq->type)     &&
             (pReq->startHandle == GATT_MIN_HANDLE) &&
             (pReq->endHandle   == GATT_MAX_HANDLE))
        {
          // Discover primary service by service UUID
          stat = GATT_DiscPrimaryServiceByUUID(connHandle, pReq->pValue,
                                                pReq->len, appTaskID);
        }
        else
        {
          stat = INVALIDPARAMETER;
        }
      }
      break;

    case ATT_FIND_BY_TYPE_VALUE_RSP:
      pPayload = createMsgPayload(&pBuf[2], pCmd->len-2);
      if (pPayload != NULL)
      {
        if (ATT_ParseFindByTypeValueRsp(pPayload, pCmd->len-2, &msg) == SUCCESS)
        {
          stat = ATT_FindByTypeValueRsp(connHandle, &msg.findByTypeValueRsp);
          if (stat == SUCCESS)
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_READ_BY_TYPE_REQ:
      if (ATT_ParseReadByTypeReq(ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                   pCmd->len-2, &msg) == SUCCESS)
      {
        attReadByTypeReq_t *pReq = &msg.readByTypeReq;

        // Find out what's been requested
        if (gattIncludeType(pReq->type))
        {
          // Find included services
          stat = GATT_FindIncludedServices(connHandle, pReq->startHandle,
                                            pReq->endHandle, appTaskID);
        }
        else if (gattCharacterType(pReq->type))
        {
          // Discover all characteristics of a service
          stat = GATT_DiscAllChars(connHandle, pReq->startHandle,
                                    pReq->endHandle, appTaskID);
        }
        else
        {
          // Read using characteristic UUID
          stat = GATT_ReadUsingCharUUID(connHandle, pReq, appTaskID);
        }
      }
      break;

    case ATT_READ_BY_TYPE_RSP:
      pPayload = createMsgPayload(&pBuf[2], pCmd->len-2);
      if (pPayload != NULL)
      {
        if (ATT_ParseReadByTypeRsp(pPayload, pCmd->len-2, &msg) == SUCCESS)
        {
          stat = ATT_ReadByTypeRsp(connHandle, &msg.readByTypeRsp);
          if (stat == SUCCESS)
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_READ_REQ:
      if (ATT_ParseReadReq(ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                             pCmd->len-2, &msg) == SUCCESS)
      {
        // Read Characteristic Value or Read Characteristic Descriptor
        stat = GATT_ReadCharValue(connHandle, &msg.readReq, appTaskID);
      }
      break;

    case ATT_READ_RSP:
      // Create payload if attribute value is present
      if (pCmd->len > 2)
      {
        pPayload = createMsgPayload(&pBuf[2], pCmd->len-2);
      }

      if ((pCmd->len == 2) || (pPayload != NULL))
      {
        if (ATT_ParseReadRsp(pPayload, pCmd->len-2, &msg) == SUCCESS)
        {
          stat = ATT_ReadRsp(connHandle, &msg.readRsp);
          if  (stat == SUCCESS)
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_READ_BLOB_REQ:
      if (ATT_ParseReadBlobReq(ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                 pCmd->len-2, &msg) == SUCCESS)
      {
        // Read long characteristic value
        stat = GATT_ReadLongCharValue(connHandle, &msg.readBlobReq, appTaskID);
      }
      break;

    case ATT_READ_BLOB_RSP:
      // Create payload if attribute value is present
      if (pCmd->len > 2)
      {
        pPayload = createMsgPayload(&pBuf[2], pCmd->len-2);
      }

      if ((pCmd->len == 2) || (pPayload != NULL))
      {
        if (ATT_ParseReadBlobRsp(pPayload, pCmd->len-2, &msg) == SUCCESS)
        {
          stat = ATT_ReadBlobRsp(connHandle, &msg.readBlobRsp);
          if (stat == SUCCESS)
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_READ_MULTI_REQ:
      pPayload = createMsgPayload(&pBuf[2], pCmd->len-2);
      if (pPayload != NULL)
      {
        if (ATT_ParseReadMultiReq(ATT_SIG_NOT_INCLUDED, FALSE, pPayload,
                                    pCmd->len-2, &msg) == SUCCESS)
        {
          stat = GATT_ReadMultiCharValues(connHandle, &msg.readMultiReq, appTaskID);
          if (stat == SUCCESS)
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_READ_MULTI_RSP:
      // Create payload if attribute value is present
      if (pCmd->len > 2)
      {
        pPayload = createMsgPayload(&pBuf[2], pCmd->len-2);
      }

      if ((pCmd->len == 2) || (pPayload != NULL))
      {
        if (ATT_ParseReadMultiRsp(pPayload, pCmd->len-2, &msg) == SUCCESS)
        {
          stat = ATT_ReadMultiRsp(connHandle, &msg.readMultiRsp);
          if (stat == SUCCESS)
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_READ_BY_GRP_TYPE_REQ:
      if (ATT_ParseReadByTypeReq(ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                   pCmd->len-2, &msg) == SUCCESS)
      {
        attReadByGrpTypeReq_t *pReq = &msg.readByGrpTypeReq;

        // Find out what's been requested
        if (gattPrimaryServiceType(pReq->type)     &&
             (pReq->startHandle == GATT_MIN_HANDLE) &&
             (pReq->endHandle   == GATT_MAX_HANDLE))
        {
          // Discover all primary services
          stat = GATT_DiscAllPrimaryServices(connHandle, appTaskID);
        }
        else
        {
          stat = INVALIDPARAMETER;
        }
      }
      break;

    case ATT_READ_BY_GRP_TYPE_RSP:
      pPayload = createMsgPayload(&pBuf[2], pCmd->len-2);
      if (pPayload != NULL)
      {
        if (ATT_ParseReadByGrpTypeRsp(pPayload, pCmd->len-2, &msg) == SUCCESS)
        {
          stat = ATT_ReadByGrpTypeRsp(connHandle, &msg.readByGrpTypeRsp);
          if (stat == SUCCESS)
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_WRITE_REQ:
      pPayload = createSignedMsgPayload(pBuf[2], pBuf[3], &pBuf[4], pCmd->len-4);
      if (pPayload != NULL)
      {
        if (ATT_ParseWriteReq(pBuf[2], pBuf[3], pPayload,
                                pCmd->len-4, &msg) == SUCCESS)
        {
          attWriteReq_t *pReq = &msg.writeReq;

          if (pReq->cmd == FALSE)
          {
            // Write Characteristic Value or Write Characteristic Descriptor
            stat = GATT_WriteCharValue(connHandle, pReq, appTaskID);
          }
          else
          {
            if (pReq->sig == FALSE)
            {
              // Write Without Response
              stat = GATT_WriteNoRsp(connHandle, pReq);
            }
            else
            {
              // Signed Write Without Response
              stat = GATT_SignedWriteNoRsp(connHandle, pReq);
            }
          }

          if ((stat == SUCCESS) && (pReq->pValue != NULL))
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_WRITE_RSP:
      if (ATT_ParseWriteRsp(&pBuf[2], pCmd->len-2, &msg) == SUCCESS)
      {
        stat = ATT_WriteRsp(connHandle);
      }
      break;

    case ATT_PREPARE_WRITE_REQ:
      pPayload = createMsgPayload(&pBuf[2], pCmd->len-2);
      if (pPayload != NULL)
      {
        if (ATT_ParsePrepareWriteReq(ATT_SIG_NOT_INCLUDED, FALSE, pPayload,
                                       pCmd->len-2, &msg) == SUCCESS)
        {
          attPrepareWriteReq_t *pReq = &msg.prepareWriteReq;

#if !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES)
          if (GATTServApp_GetParamValue() == GATT_TESTMODE_PREPARE_WRITE)
          {
            // Send the Prepare Write Request right away - needed for GATT testing
            stat = GATT_PrepareWriteReq(connHandle, pReq, appTaskID);
            if ((stat == SUCCESS) && (pReq->pValue != NULL))
            {
              safeToDealloc = FALSE; // payload passed to GATT
            }
          }
          else
#endif // !GATT_DB_OFF_CHIP && TESTMODE
          {
            // GATT Reliable Writes
            if (pPrepareWrites == NULL)
            {
              // First allocated buffer for the Prepare Write Requests
              pPrepareWrites = ICall_malloc(GATT_MAX_NUM_RELIABLE_WRITES * sizeof(attPrepareWriteReq_t));
            }

            if (pPrepareWrites != NULL)
            {
              if (numPrepareWrites < GATT_MAX_NUM_RELIABLE_WRITES)
              {
                // Save the Prepare Write Request for now
                VOID memcpy(&(pPrepareWrites[numPrepareWrites++]),
                                  pReq, sizeof(attPrepareWriteReq_t));

                // Request was parsed and saved successfully
                stat = SUCCESS;
                safeToDealloc = FALSE; // payload saved for later
              }
              else
              {
                stat = INVALIDPARAMETER;
              }
            }
            else
            {
              stat = bleMemAllocError;
            }
          }
        }
      }
      break;

    case ATT_PREPARE_WRITE_RSP:
      pPayload = createMsgPayload(&pBuf[2], pCmd->len-2);
      if (pPayload != NULL)
      {
        if (ATT_ParsePrepareWriteRsp(pPayload, pCmd->len-2, &msg) == SUCCESS)
        {
          attPrepareWriteRsp_t *pRsp = &msg.prepareWriteRsp;

          stat = ATT_PrepareWriteRsp(connHandle, pRsp);
          if ((stat == SUCCESS) && (pRsp->pValue != NULL))
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_EXECUTE_WRITE_REQ:
      if (ATT_ParseExecuteWriteReq(ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                     pCmd->len-2, &msg) == SUCCESS)
      {
#if !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES)
        if (GATTServApp_GetParamValue() == GATT_TESTMODE_PREPARE_WRITE)
        {
          // Send the Execute Write Request right away - needed for GATT testing
          stat = GATT_ExecuteWriteReq(connHandle, &msg.executeWriteReq, appTaskID);
        }
        else
#endif // !GATT_DB_OFF_CHIP && TESTMODE
        if (pPrepareWrites != NULL)
        {
          // GATT Reliable Writes - send all saved Prepare Write Requests
          stat = GATT_ReliableWrites(connHandle, pPrepareWrites, numPrepareWrites,
                                      msg.executeWriteReq.flags, appTaskID);
          if (stat != SUCCESS)
          {
            uint8_t i;

            // Free saved payload buffers
            for (i = 0; i < numPrepareWrites; i++)
            {
              if (pPrepareWrites[i].pValue != NULL)
              {
                BM_free(pPrepareWrites[i].pValue);
              }
            }

            ICall_free(pPrepareWrites);
          }
          // else pPrepareWrites will be freed by GATT Client

          // Reset GATT Reliable Writes variables
          pPrepareWrites = NULL;
          numPrepareWrites = 0;
        }
        else
        {
          stat = INVALIDPARAMETER;
        }
      }
      break;

    case ATT_EXECUTE_WRITE_RSP:
      if (ATT_ParseExecuteWriteRsp(&pBuf[2], pCmd->len-2, &msg) == SUCCESS)
      {
        stat = ATT_ExecuteWriteRsp(connHandle);
      }
      break;

    case ATT_HANDLE_VALUE_NOTI:
      pPayload = createMsgPayload(&pBuf[3], pCmd->len-3);
      if (pPayload != NULL)
      {
        if (ATT_ParseHandleValueInd(ATT_SIG_NOT_INCLUDED, FALSE, pPayload,
                                      pCmd->len-3, &msg) == SUCCESS)
        {
          attHandleValueNoti_t *pNoti = &msg.handleValueNoti;

          stat = GATT_Notification(connHandle, pNoti, pBuf[2]);
          if ((stat == SUCCESS) && (pNoti->pValue!= NULL))
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_HANDLE_VALUE_IND:
      pPayload = createMsgPayload(&pBuf[3], pCmd->len-3);
      if (pPayload != NULL)
      {
        if (ATT_ParseHandleValueInd(ATT_SIG_NOT_INCLUDED, FALSE, pPayload,
                                      pCmd->len-3, &msg) == SUCCESS)
        {
          attHandleValueInd_t *pInd = &msg.handleValueInd;
#ifndef BLE3_CMD
          gattCharCfg_t *indCharCfg = (gattCharCfg_t *)*(uint32_t*)(gattAttrTbl[3].pValue);
          if(indCharCfg == NULL)
          {
            stat = INVALIDPARAMETER;
            break;
          }
          uint16 value = GATTServApp_ReadCharCfg( connHandle, indCharCfg );
          if(value & GATT_CLIENT_CFG_INDICATE)
          {
#endif
            stat = GATT_Indication(connHandle, pInd, pBuf[2], appTaskID);
            if ((stat == SUCCESS) && (pInd->pValue != NULL))
            {
              safeToDealloc = FALSE; // payload passed to GATT
            }
          }
#ifndef BLE3_CMD
          else
          {
            stat = FAILURE;
          }
        }
#endif
      }
      break;

    case ATT_HANDLE_VALUE_CFM:
      if (ATT_ParseHandleValueCfm(&pBuf[2], pCmd->len-2, &msg) == SUCCESS)
      {
        stat = ATT_HandleValueCfm(connHandle);
      }
      break;

    default:
      stat = FAILURE;
      break;
  }

  if ((pPayload != NULL) && safeToDealloc)
  {
    // Free payload
    BM_free(pPayload);
  }

  return(mapATT2BLEStatus(stat));
}

/*********************************************************************
 * @fn      processExtMsgGATT
 *
 * @brief   Parse and process incoming HCI extension GATT messages.
 *
 * @param   cmdID - incoming HCI extension command ID.
 * @param   pCmd - incoming HCI extension message.
 * @param   pRspDataLen - response data length to be returned.
 *
 * @return  SUCCESS, INVALIDPARAMETER, FAILURE,
 *          bleInvalidPDU or bleMemAllocError
 */
static uint8_t processExtMsgGATT(uint8_t cmdID, hciExtCmd_t *pCmd, uint8_t *pRspDataLen)
{
#if defined(GATT_DB_OFF_CHIP)
  static uint16_t totalAttrs = 0;
  static gattService_t service = { 0, NULL };
#endif // GATT_DB_OFF_CHIP
  uint8_t *pBuf, *pPayload = NULL, safeToDealloc = TRUE;
  uint16_t connHandle;
  attMsg_t msg;
  bStatus_t stat = bleInvalidPDU;

  // Make sure received buffer is at lease 2-otect long
  if (pCmd->len < 2)
  {
    return(stat);
  }

  pBuf = pCmd->pData;
  connHandle = BUILD_UINT16(pBuf[0], pBuf[1]);

  switch(cmdID)
  {
    case ATT_EXCHANGE_MTU_REQ: // GATT Exchange MTU
      if (ATT_ParseExchangeMTUReq(ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                    pCmd->len-2, &msg) == SUCCESS)
      {
        stat = GATT_ExchangeMTU(connHandle, &msg.exchangeMTUReq, appTaskID);
      }
      break;

    case ATT_READ_BY_GRP_TYPE_REQ: // GATT Discover All Primary Services
      stat = GATT_DiscAllPrimaryServices(connHandle, appTaskID);
      break;

    case ATT_FIND_BY_TYPE_VALUE_REQ: // GATT Discover Primary Service By UUID
      stat = GATT_DiscPrimaryServiceByUUID(connHandle, &pBuf[2],
                                            pCmd->len-2, appTaskID);
      break;

    case GATT_FIND_INCLUDED_SERVICES: // GATT Find Included Services
    case GATT_DISC_ALL_CHARS: // GATT Discover All Characteristics
      if ((pCmd->len-2) == ATT_READ_BY_TYPE_REQ_FIXED_SIZE)
      {
        // First requested handle number
        uint16_t startHandle = BUILD_UINT16(pBuf[2], pBuf[3]);

        // Last requested handle number
        uint16_t endHandle = BUILD_UINT16(pBuf[4], pBuf[5]);

        if (cmdID == GATT_FIND_INCLUDED_SERVICES)
        {
          stat = GATT_FindIncludedServices(connHandle, startHandle,
                                            endHandle, appTaskID);
        }
        else
        {
          stat = GATT_DiscAllChars(connHandle, startHandle,
                                    endHandle, appTaskID);
        }
      }
      break;

    case ATT_READ_BY_TYPE_REQ: // GATT Discover Characteristics by UUID
      if (ATT_ParseReadByTypeReq(ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                   pCmd->len-2, &msg) == SUCCESS)
      {
        stat = GATT_DiscCharsByUUID(connHandle, &msg.readByTypeReq, appTaskID);
      }
      break;

    case ATT_FIND_INFO_REQ: // GATT Discover All Characteristic Descriptors
      if (ATT_ParseFindInfoReq(ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                 pCmd->len-2, &msg) == SUCCESS)
      {
        stat = GATT_DiscAllCharDescs(connHandle, msg.findInfoReq.startHandle,
                                      msg.findInfoReq.endHandle, appTaskID);
      }
      break;

    case ATT_READ_REQ: // GATT Read Characteristic Value
      if (ATT_ParseReadReq(ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                             pCmd->len-2, &msg) == SUCCESS)
      {
        stat = GATT_ReadCharValue(connHandle, &msg.readReq, appTaskID);
      }
      break;

    case GATT_READ_USING_CHAR_UUID: // GATT Read Using Characteristic UUID
      if (ATT_ParseReadByTypeReq(ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                   pCmd->len-2, &msg) == SUCCESS)
      {
        stat = GATT_ReadUsingCharUUID(connHandle, &msg.readByTypeReq, appTaskID);
      }
      break;

    case ATT_READ_BLOB_REQ: // GATT Read Long Characteristic Value
      if (ATT_ParseReadBlobReq(ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                 pCmd->len-2, &msg) == SUCCESS)
      {
        stat = GATT_ReadLongCharValue(connHandle, &msg.readBlobReq, appTaskID);
      }
      break;

    case ATT_READ_MULTI_REQ: // GATT Read Multiple Characteristic Values
      pPayload = createMsgPayload(&pBuf[2], pCmd->len-2);
      if (pPayload != NULL)
      {
        if (ATT_ParseReadMultiReq(ATT_SIG_NOT_INCLUDED, FALSE, pPayload,
                                    pCmd->len-2, &msg) == SUCCESS)
        {
          stat = GATT_ReadMultiCharValues(connHandle, &msg.readMultiReq, appTaskID);
          if (stat == SUCCESS)
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case GATT_WRITE_NO_RSP: // GATT Write Without Response
      pPayload = createMsgPayload(&pBuf[2], pCmd->len-2);
      if (pPayload != NULL)
      {
        if (ATT_ParseWriteReq(FALSE, TRUE, pPayload, pCmd->len-2, &msg) == SUCCESS)
        {
          attWriteReq_t *pReq = &msg.writeReq;

          stat = GATT_WriteNoRsp(connHandle, pReq);
          if ((stat == SUCCESS) && (pReq->pValue != NULL))
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case GATT_SIGNED_WRITE_NO_RSP: // GATT Signed Write Without Response
      pPayload = createSignedMsgPayload(TRUE, TRUE, &pBuf[2], pCmd->len-2);
      if (pPayload != NULL)
      {
        if (ATT_ParseWriteReq(TRUE, TRUE, pPayload, pCmd->len-2, &msg) == SUCCESS)
        {
          attWriteReq_t *pReq = &msg.writeReq;

          stat = GATT_SignedWriteNoRsp(connHandle, pReq);
          if ((stat == SUCCESS) && (pReq->pValue != NULL))
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_WRITE_REQ: // GATT Write Characteristic Value
      pPayload = createMsgPayload(&pBuf[2], pCmd->len-2);
      if (pPayload != NULL)
      {
        if (ATT_ParseWriteReq(FALSE, FALSE, pPayload, pCmd->len-2, &msg) == SUCCESS)
        {
          attWriteReq_t *pReq = &msg.writeReq;

          stat = GATT_WriteCharValue(connHandle, pReq, appTaskID);
          if ((stat == SUCCESS) && (pReq->pValue != NULL))
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_PREPARE_WRITE_REQ: // GATT Write Long Characteristic Value
    case GATT_WRITE_LONG_CHAR_DESC: // GATT Write Long Characteristic Descriptor
      pPayload = createMsgPayload(&pBuf[2], pCmd->len-2);
      if (pPayload != NULL)
      {
        if (ATT_ParsePrepareWriteReq(ATT_SIG_NOT_INCLUDED, FALSE, pPayload,
                                       pCmd->len-2, &msg) == SUCCESS)
        {
          attPrepareWriteReq_t *pReq = &msg.prepareWriteReq;
          if (cmdID == ATT_PREPARE_WRITE_REQ)
          {
            stat = GATT_WriteLongCharValue(connHandle, pReq, appTaskID);
          }
          else
          {
            stat = GATT_WriteLongCharDesc(connHandle, pReq, appTaskID);
          }

          if ((stat == SUCCESS) && (pReq->pValue != NULL))
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case GATT_RELIABLE_WRITES: // GATT Reliable Writes
      if (pCmd->len-2 > 0)
      {
        uint8_t numReqs = pBuf[2];

        if ((numReqs > 0) && (numReqs <= GATT_MAX_NUM_RELIABLE_WRITES))
        {
          // First allocated buffer for the Prepare Write Requests
          attPrepareWriteReq_t *pReqs = ICall_malloc(numReqs * sizeof(attPrepareWriteReq_t));
          if (pReqs != NULL)
          {
            uint8_t i;

            VOID memset(pReqs, 0, numReqs * sizeof(attPrepareWriteReq_t));

            pBuf += 3; // pass connHandle and numReqs

            // Create payload buffer for each Prepare Write Request
            for (i = 0; i < numReqs; i++)
            {
              // length of request is length of attribute value plus fixed fields.
              // request format: length (1) + handle (2) + offset (2) + attribute value
              uint8_t reqLen = ATT_PREPARE_WRITE_REQ_FIXED_SIZE;

              if (*pBuf > 0)
              {
                reqLen += *pBuf++;
                pPayload = createMsgPayload(pBuf, reqLen);
                if (pPayload == NULL)
                {
                  stat = bleMemAllocError;
                  break;
                }

                VOID ATT_ParsePrepareWriteReq(ATT_SIG_NOT_INCLUDED, FALSE, pPayload,
                                               reqLen, (attMsg_t *)&(pReqs[i]));
              }
              else // no attribute value
              {
                pBuf++;
                VOID ATT_ParsePrepareWriteReq(ATT_SIG_NOT_INCLUDED, FALSE, pBuf,
                                               reqLen, (attMsg_t *)&(pReqs[i]));
              }

              // Next request
              pBuf += reqLen;
            }

            // See if all requests were parsed successfully
            if (i == numReqs)
            {
              // Send all saved Prepare Write Requests
              stat = GATT_ReliableWrites(connHandle, pReqs, numReqs,
                                          ATT_WRITE_PREPARED_VALUES, appTaskID);
            }

            if (stat != SUCCESS)
            {
              // Free payload buffers first
              for (i = 0; i < numReqs; i++)
              {
                if (pReqs[i].pValue != NULL)
                {
                  BM_free(pReqs[i].pValue);
                }
              }

              ICall_free(pReqs);
            }
            // else pReqs will be freed by GATT Client
          }
          else
          {
            stat = bleMemAllocError;
          }
        }
        else
        {
          stat = INVALIDPARAMETER;
        }
      }
      break;

    case GATT_READ_CHAR_DESC: // GATT Read Characteristic Descriptor
      if (ATT_ParseReadReq(ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                             pCmd->len-2, &msg) == SUCCESS)
      {
        stat = GATT_ReadCharDesc(connHandle, &msg.readReq, appTaskID);
      }
      break;

    case GATT_READ_LONG_CHAR_DESC: // GATT Read Long Characteristic Descriptor
      if (ATT_ParseReadBlobReq(ATT_SIG_NOT_INCLUDED, FALSE, &pBuf[2],
                                 pCmd->len-2, &msg) == SUCCESS)
      {
        stat = GATT_ReadLongCharDesc(connHandle, &msg.readBlobReq, appTaskID);
      }
      break;

    case GATT_WRITE_CHAR_DESC: // GATT Write Characteristic Descriptor
      pPayload = createMsgPayload(&pBuf[2], pCmd->len-2);
      if (pPayload != NULL)
      {
        if (ATT_ParseWriteReq(FALSE, FALSE, pPayload, pCmd->len-2, &msg) == SUCCESS)
        {
          attWriteReq_t *pReq = &msg.writeReq;

          stat = GATT_WriteCharDesc(connHandle, pReq, appTaskID);
          if ((stat == SUCCESS) && (pReq->pValue != NULL))
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_HANDLE_VALUE_NOTI:
      pPayload = createMsgPayload(&pBuf[3], pCmd->len-3);
      if (pPayload != NULL)
      {
        if (ATT_ParseHandleValueInd(ATT_SIG_NOT_INCLUDED, FALSE, pPayload,
                                      pCmd->len-3, &msg) == SUCCESS)
        {
          attHandleValueNoti_t *pNoti = &msg.handleValueNoti;

          stat = GATT_Notification(connHandle, pNoti, pBuf[2]);
          if ((stat == SUCCESS) && (pNoti->pValue != NULL))
          {
            safeToDealloc = FALSE; // payload passed to GATT
          }
        }
      }
      break;

    case ATT_HANDLE_VALUE_IND:
      pPayload = createMsgPayload(&pBuf[3], pCmd->len-3);
      if (pPayload != NULL)
      {
        if (ATT_ParseHandleValueInd(ATT_SIG_NOT_INCLUDED, FALSE, pPayload,
                                      pCmd->len-3, &msg) == SUCCESS)
        {
          attHandleValueInd_t *pInd = &msg.handleValueInd;
          gattCharCfg_t *indCharCfg = (gattCharCfg_t *)*(uint32_t*)(gattAttrTbl[3].pValue);
          if(indCharCfg == NULL)
          {
            stat = INVALIDPARAMETER;
            break;
          }
          uint16 value = GATTServApp_ReadCharCfg( connHandle, indCharCfg );
          if(value & GATT_CLIENT_CFG_INDICATE)
          {
            stat = GATT_Indication(connHandle, pInd, pBuf[2], appTaskID);
            if ((stat == SUCCESS) && (pInd->pValue != NULL))
            {
              safeToDealloc = FALSE; // payload passed to GATT
            }
          }
          else
          {
            stat = FAILURE;
          }
        }
      }
      break;

#if defined(GATT_DB_OFF_CHIP) // These GATT commands don't include connHandle field
    case HCI_EXT_GATT_ADD_SERVICE:
      if (service.attrs == NULL)
      {
        // Service type must be 2 octets (Primary or Secondary)
        if (pCmd->len-3 == ATT_BT_UUID_SIZE)
        {
          uint16_t uuid = BUILD_UINT16(pBuf[0], pBuf[1]);
          uint16_t numAttrs = BUILD_UINT16(pBuf[2], pBuf[3]);

          if (((uuid == GATT_PRIMARY_SERVICE_UUID)     ||
                 (uuid == GATT_SECONDARY_SERVICE_UUID)) &&
               (numAttrs > 0))
          {
            uint8_t encKeySize = pBuf[4];

            if ((encKeySize >= GATT_MIN_ENCRYPT_KEY_SIZE) &&
                 (encKeySize <= GATT_MAX_ENCRYPT_KEY_SIZE))
            {
              // Allocate buffer for the attribute table
              service.attrs = ICall_malloc(numAttrs * sizeof(gattAttribute_t));
              if (service.attrs != NULL)
              {
                // Zero out all attribute fields
                VOID memset(service.attrs, 0, numAttrs * sizeof(gattAttribute_t));

                totalAttrs = numAttrs;
                service.encKeySize = encKeySize;

                // Set up service record
                stat = addAttrRec(&service, pBuf, ATT_BT_UUID_SIZE,
                                   GATT_PERMIT_READ, &totalAttrs, pRspDataLen);
              }
              else
              {
                stat = bleMemAllocError;
              }
            }
            else
            {
              stat = bleInvalidRange;
            }
          }
          else
          {
            stat = INVALIDPARAMETER;
          }
        }
      }
      else
      {
        stat = blePending;
      }
      break;

    case HCI_EXT_GATT_DEL_SERVICE:
      {
        uint16_t handle = BUILD_UINT16(pBuf[0], pBuf[1]);

        if (handle == 0x0000)
        {
          // Service is not registered with GATT yet
          freeAttrRecs(&service);

          totalAttrs = 0;
        }
        else
        {
          gattService_t serv;

          // Service is already registered with the GATT Server
          stat = GATT_DeregisterService(handle, &serv);
          if (stat == SUCCESS)
          {
            freeAttrRecs(&serv);
          }
        }

        stat = SUCCESS;
      }
      break;

    case HCI_EXT_GATT_ADD_ATTRIBUTE:
      if (service.attrs != NULL)
      {
        if ((pCmd->len-1 == ATT_UUID_SIZE) ||
             (pCmd->len-1 == ATT_BT_UUID_SIZE))
        {
          // Add attribute record to the service being added
          stat = addAttrRec(&service, pBuf, pCmd->len-1,
                             pBuf[pCmd->len-1], &totalAttrs, pRspDataLen);
        }
      }
      else // no corresponding service
      {
        stat = INVALIDPARAMETER;
      }
      break;
    case HCI_EXT_GATT_UPDATE_MTU:
      stat = GATT_UpdateMTU(BUILD_UINT16(pBuf[0], pBuf[1]),  // connHandle
                              BUILD_UINT16(pBuf[2], pBuf[3])); // mtuSize
      break;
#endif // GATT_DB_OFF_CHIP

    default:
      stat = FAILURE;
      break;
  }

  if ((pPayload != NULL) && safeToDealloc)
  {
    // Free payload
    BM_free(pPayload);
  }

  return(mapATT2BLEStatus(stat));
}
#endif /* (CENTRAL_CFG | PERIPHERAL_CFG) */

/*********************************************************************
 * @fn      processExtMsgGAP
 *
 * @brief   Parse and process incoming HCI extension GAP messages.
 *
 * @param   cmdID - incoming HCI extension command ID.
 * @param   pCmd - incoming HCI extension message.
 * @param   pRspDataLen - response data length to be returned.
 *
 * @return  SUCCESS, INVALIDPARAMETER, FAILURE,
 *          or bleMemAllocError
 */
static uint8_t processExtMsgGAP(uint8_t cmdID, hciExtCmd_t *pCmd, uint8_t *pRspDataLen)
{
  uint8_t *pBuf = pCmd->pData;
  bStatus_t stat = SUCCESS;

  switch(cmdID)
  {
    case HCI_EXT_GAP_DEVICE_INIT:
    {
#if BLE3_CMD
      uint8_t profileRole = pBuf[0];
      uint8_t IRK[KEYLEN] = {0};
      uint8_t SRK[KEYLEN] = {0};
	  uint8_t addr[B_ADDR_LEN] = {0}; // The decvice address type and address must be configures with GAP_configDeviceAddr

#if defined(GAP_BOND_MGR)
      // Register with bond manager after starting device - not to prevent using GapBondMgr with BLE3_CMD flag
      GAPBondMgr_Register((gapBondCBs_t *) &host_tl_bondCB);
#endif

      if( !osal_memcmp(&pBuf[2+KEYLEN], SRK, KEYLEN) )
      {
        // Copy and set the SRK
        osal_memcpy(SRK, &pBuf[2+KEYLEN], KEYLEN);
        GapConfig_SetParameter(GAP_CONFIG_PARAM_SRK, SRK);
      }

      if( !osal_memcmp(&pBuf[1], IRK, KEYLEN) )
      {
        // Copy and set the IRK
        osal_memcpy(IRK, &pBuf[2], KEYLEN);
        GapConfig_SetParameter(GAP_CONFIG_PARAM_IRK, IRK);

        // This mode is set to enable saving the IRK given
        stat = GAP_DeviceInit(profileRole, appTaskID, ADDRMODE_RP_WITH_PUBLIC_ID, addr);
      }
      else
      {
        // IRK was all 0's - the address mode can be changed with HCI_EXT_GAP_CONFIG_DEVICE_ADDR
        stat = GAP_DeviceInit(profileRole, appTaskID, ADDRMODE_PUBLIC, addr);
      }
      // Maximum number of adv reports
      maxNumReports  = pBuf[1];

      if( maxNumReports )
      {
        // Allocating the memory for device info
        deviceInfoArr = (deviceInfo_t *)osal_mem_alloc(maxNumReports*sizeof(deviceInfo_t));
        osal_memset(deviceInfoArr, 0, maxNumReports*sizeof(deviceInfo_t));
      }

      // ignoring pSignCounter

#else // !BLE3_CMD
      uint8_t zeroAddr[B_ADDR_LEN] = {0};

#if defined(GAP_BOND_MGR)
      // Register with bond manager after starting device.
      GAPBondMgr_Register((gapBondCBs_t *) &host_tl_bondCB);
#endif

      // if the specified random addr is all zeros, replace it with NULL
      stat = GAP_DeviceInit(pBuf[0], appTaskID, pBuf[1],
                            memcmp(&pBuf[2], zeroAddr, B_ADDR_LEN) ?
                              &pBuf[2] : NULL);
#endif // BLE3_CMD
      break;
    }

    case HCI_EXT_GAP_CONFIG_SET_PARAM:
    {
      stat = GapConfig_SetParameter(pBuf[0],            // param
                                    &pBuf[1]);         // pValue
      break;
    }

#if (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
    case HCI_EXT_GAP_TERMINATE_LINK:
    {
#ifndef BLE3_CMD
      stat = GAP_TerminateLinkReq(BUILD_UINT16(pBuf[0], pBuf[1]), pBuf[2]);
#else //BLE3_CMD
      uint16_t connHandle = BUILD_UINT16(pBuf[0], pBuf[1]);
      if( connHandle != 0xFFFE )
      {
        stat = GAP_TerminateLinkReq(BUILD_UINT16(pBuf[0], pBuf[1]), pBuf[2]);
      }
      else
      {
#if (HOST_CONFIG & (CENTRAL_CFG))
        stat = GapInit_cancelConnect();
        legacyConnCancel = 1;
#else
        stat = FAILURE;
#endif
      }
#endif
      break;
    }

    case HCI_EXT_GAP_UPDATE_LINK_PARAM_REQ:
    {
      gapUpdateLinkParamReq_t updateLinkReq;

      updateLinkReq.connectionHandle = BUILD_UINT16(pBuf[0], pBuf[1]);
      pBuf += 2;
      updateLinkReq.intervalMin = BUILD_UINT16(pBuf[0], pBuf[1]);
      pBuf += 2;
      updateLinkReq.intervalMax = BUILD_UINT16(pBuf[0], pBuf[1]);
      pBuf += 2;
      updateLinkReq.connLatency = BUILD_UINT16(pBuf[0], pBuf[1]);
      pBuf += 2;
      updateLinkReq.connTimeout = BUILD_UINT16(pBuf[0], pBuf[1]);

      stat = GAP_UpdateLinkParamReq(&updateLinkReq);
      break;
    }

    case HCI_EXT_GAP_UPDATE_LINK_PARAM_REQ_REPLY:
    {
      gapUpdateLinkParamReqReply_t updateLinkReply;

      updateLinkReply.connectionHandle = BUILD_UINT16(pBuf[0], pBuf[1]);
      pBuf += 2;
      updateLinkReply.intervalMin = BUILD_UINT16(pBuf[0], pBuf[1]);
      pBuf += 2;
      updateLinkReply.intervalMax = BUILD_UINT16(pBuf[0], pBuf[1]);
      pBuf += 2;
      updateLinkReply.connLatency = BUILD_UINT16(pBuf[0], pBuf[1]);
      pBuf += 2;
      updateLinkReply.connTimeout = BUILD_UINT16(pBuf[0], pBuf[1]);
      pBuf += 2;
      updateLinkReply.signalIdentifier = *pBuf++;
      updateLinkReply.accepted    = *pBuf;

      stat = GAP_UpdateLinkParamReqReply(&updateLinkReply);
      break;
    }

    case HCI_EXT_GAP_REGISTER_CONN_EVT:
    {
      stat = Gap_RegisterConnEventCb(host_tl_connEvtCallback,
                                     pBuf[0],                         // action
                                     BUILD_UINT16(pBuf[1], pBuf[2])); // connHandle
      break;
    }

    case HCI_EXT_GAP_AUTHENTICATE:
    {
      uint8_t tmp;
      gapAuthParams_t params;
      gapPairingReq_t pairReq;
      gapPairingReq_t *pPairReq = NULL;

      VOID memset(&params, 0, sizeof (gapAuthParams_t));

      params.connectionHandle = BUILD_UINT16(pBuf[0], pBuf[1]);
      pBuf += 2;

      params.secReqs.ioCaps = *pBuf++;
      params.secReqs.oobAvailable = *pBuf++;
      VOID memcpy(params.secReqs.oob, pBuf, KEYLEN);
      pBuf += KEYLEN;

      VOID memcpy(params.secReqs.oobConfirm, pBuf, KEYLEN);
      pBuf += KEYLEN;
      params.secReqs.localOobAvailable = *pBuf++;
      VOID memcpy(params.secReqs.localOob, pBuf, KEYLEN);
      pBuf += KEYLEN;
      params.secReqs.isSCOnlyMode = *pBuf++;
      params.secReqs.eccKeys.isUsed = *pBuf++;
      VOID memcpy(params.secReqs.eccKeys.sK, pBuf, SM_ECC_KEY_LEN);
      pBuf += SM_ECC_KEY_LEN;
      VOID memcpy(params.secReqs.eccKeys.pK_x, pBuf, SM_ECC_KEY_LEN);
      pBuf += SM_ECC_KEY_LEN;
      VOID memcpy(params.secReqs.eccKeys.pK_y, pBuf, SM_ECC_KEY_LEN);
      pBuf += SM_ECC_KEY_LEN;

      params.secReqs.authReq = *pBuf++;
      params.secReqs.maxEncKeySize = *pBuf++;

      tmp = *pBuf++;
      params.secReqs.keyDist.sEncKey = (tmp & KEYDIST_SENC) ? TRUE : FALSE;
      params.secReqs.keyDist.sIdKey = (tmp & KEYDIST_SID) ? TRUE : FALSE;
      params.secReqs.keyDist.sSign = (tmp & KEYDIST_SSIGN) ? TRUE : FALSE;
      params.secReqs.keyDist.sLinkKey = (tmp & KEYDIST_SLINK) ? TRUE : FALSE;
      params.secReqs.keyDist.sReserved = 0;
      params.secReqs.keyDist.mEncKey = (tmp & KEYDIST_MENC) ? TRUE : FALSE;
      params.secReqs.keyDist.mIdKey = (tmp & KEYDIST_MID) ? TRUE : FALSE;
      params.secReqs.keyDist.mSign = (tmp & KEYDIST_MSIGN) ? TRUE : FALSE;
      params.secReqs.keyDist.mLinkKey = (tmp & KEYDIST_MLINK) ? TRUE : FALSE;
      params.secReqs.keyDist.mReserved = 0;

      tmp = *pBuf++; // enable disable
      if (tmp)
      {
        pairReq.ioCap = *pBuf++;
        pairReq.oobDataFlag = *pBuf++;
        pairReq.authReq = *pBuf++;
        pairReq.maxEncKeySize = *pBuf++;
        tmp = *pBuf++;
        pairReq.keyDist.sEncKey = (tmp & KEYDIST_SENC) ? TRUE : FALSE;
        pairReq.keyDist.sIdKey = (tmp & KEYDIST_SID) ? TRUE : FALSE;
        pairReq.keyDist.sSign = (tmp & KEYDIST_SSIGN) ? TRUE : FALSE;
        pairReq.keyDist.sLinkKey = (tmp & KEYDIST_SLINK) ? TRUE : FALSE;
        pairReq.keyDist.sReserved = 0;
        pairReq.keyDist.mEncKey = (tmp & KEYDIST_MENC) ? TRUE : FALSE;
        pairReq.keyDist.mIdKey = (tmp & KEYDIST_MID) ? TRUE : FALSE;
        pairReq.keyDist.mSign = (tmp & KEYDIST_MSIGN) ? TRUE : FALSE;
        pairReq.keyDist.mLinkKey = (tmp & KEYDIST_MLINK) ? TRUE : FALSE;
        pairReq.keyDist.mReserved = 0;
        pPairReq = &pairReq;
      }

      stat = GAP_Authenticate(&params, pPairReq);

      break;
    }

    case HCI_EXT_GAP_TERMINATE_AUTH:
      stat = GAP_TerminateAuth(BUILD_UINT16(pBuf[0], pBuf[1]), pBuf[2]);
      break;

    case HCI_EXT_GAP_BOND:
    {
      uint16_t connectionHandle;
      smSecurityInfo_t securityInfo;
      uint8_t authenticated;
      uint8_t secureConnections;

      // Do Security Information part
      connectionHandle = BUILD_UINT16(pBuf[0], pBuf[1]);
      pBuf += 2;
      authenticated = *pBuf++;
      secureConnections = *pBuf++;
      VOID memcpy(securityInfo.ltk, pBuf, KEYLEN);
      pBuf += KEYLEN;
      securityInfo.div = BUILD_UINT16(pBuf[0], pBuf[1]);
      pBuf += 2;
      VOID memcpy(securityInfo.rand, pBuf, B_RANDOM_NUM_SIZE);
      pBuf += B_RANDOM_NUM_SIZE;
      securityInfo.keySize = *pBuf++;

      stat = GAP_Bond(connectionHandle, authenticated, secureConnections,
                       &securityInfo, TRUE);
      break;
    }

    case HCI_EXT_GAP_SIGNABLE:
    {
      uint16_t connectionHandle;
      uint8_t authenticated;
      smSigningInfo_t signing;

      connectionHandle = BUILD_UINT16(pBuf[0], pBuf[1]);
      pBuf += 2;

      authenticated = *pBuf++;

      VOID memcpy(signing.srk, pBuf, KEYLEN);
      pBuf += KEYLEN;

      signing.signCounter = BUILD_UINT32(pBuf[0], pBuf[1], pBuf[2], pBuf[3]);

      stat = GAP_Signable(connectionHandle, authenticated, &signing);
      break;
    }

    case HCI_EXT_GAP_PASSKEY_UPDATE:
      stat = GAP_PasskeyUpdate(&pBuf[2], BUILD_UINT16(pBuf[0], pBuf[1]));
      break;

#endif // CENTRAL_CFG | PERIPHERAL_CFG

    case HCI_EXT_GAP_SET_PARAM:
    {
      uint16_t id = (uint16_t)pBuf[0];
      uint16_t value = BUILD_UINT16(pBuf[1], pBuf[2]);

#ifdef BLE3_CMD
      if( (id != TGAP_AUTH_TASK_ID) && (id < TGAP_PARAMID_MAX) )
      {
        stat = BLE3ToAgama_setParam(id, value);
      }
      else
      {
        stat = INVALIDPARAMETER;
      }
#else // !BLE3_CMD
#if !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES)
      if (id == GAP_PARAM_GATT_TESTCODE)
      {
        GATTServApp_SetParamValue(value);
      }
      else if (id == GAP_PARAM_ATT_TESTCODE)
      {
        ATT_SetParamValue(value);
      }
      else if (id == GAP_PARAM_L2CAP_TESTCODE)
      {
        L2CAP_SetParamValue(value);
      }
      else
#endif // !GATT_DB_OFF_CHIP && TESTMODES
      if (id == GAP_PARAM_GGS_PARAMS)
      {
        GGS_SetParamValue(value);
      }
      else if ((id != GAP_PARAM_AUTH_TASK_ID) && (id < GAP_PARAMID_MAX))
      {
        stat = GAP_SetParamValue(id, value);
      }
      else
      {
        stat = INVALIDPARAMETER;
      }
#endif // BLE3_CMD
      break;
    }

    case HCI_EXT_GAP_GET_PARAM:
    {
      uint16_t paramValue = 0xFFFF;
      uint16_t param = (uint16_t)pBuf[0];

      if (param < 0x00FF)
      {
#if !defined(GATT_DB_OFF_CHIP) && defined(TESTMODES)
        if (param == GAP_PARAM_GATT_TESTCODE)
        {
          paramValue = GATTServApp_GetParamValue();
        }
        else if (param == GAP_PARAM_ATT_TESTCODE)
        {
          paramValue = ATT_GetParamValue();
        }
        else if (param == GAP_PARAM_GGS_PARAMS)
        {
          paramValue = GGS_GetParamValue();
        }
        else if (param == GAP_PARAM_L2CAP_TESTCODE)
        {
          paramValue = L2CAP_GetParamValue();
        }
        else
#endif // !GATT_DB_OFF_CHIP && TESTMODES
        if ((param != GAP_PARAM_AUTH_TASK_ID) && (param < GAP_PARAMID_MAX))
        {
          paramValue = GAP_GetParamValue(param);
        }
      }
      if (paramValue != 0xFFFF)
      {
        stat = SUCCESS;
      }
      else
      {
        stat = INVALIDPARAMETER;
      }
      *pRspDataLen = 2;
      rspBuf[RSP_PAYLOAD_IDX] = LO_UINT16(paramValue);
      rspBuf[RSP_PAYLOAD_IDX+1] = HI_UINT16(paramValue);

      break;
    }

#if (HOST_CONFIG & PERIPHERAL_CFG)
    case HCI_EXT_GAP_SLAVE_SECURITY_REQ_UPDATE:
#ifndef EXCLUDE_SM
      stat = GAP_SendSlaveSecurityRequest(BUILD_UINT16(pBuf[0], pBuf[1]), pBuf[2]);
#else
      stat = bleIncorrectMode;
#endif // EXCLUDE_SM
      break;
#endif // PERIPHERAL_CFG

    case HCI_EXT_GAP_BOND_SET_PARAM:
    {
#if defined(GAP_BOND_MGR)
      uint16_t id = BUILD_UINT16(pBuf[0], pBuf[1]);
      uint8_t *pValue;

      if(id == GAPBOND_DEFAULT_PASSCODE)
      {
        // First build passcode
        if(pBuf[2] == 2)
          host_tl_defaultPasscode = (BUILD_UINT32(pBuf[3], pBuf[3 + 1], 0L, 0L));
        else if(pBuf[2] == 3)
          host_tl_defaultPasscode = (BUILD_UINT32(pBuf[3], pBuf[3 + 1], pBuf[3 + 2], 0L));
        else if(pBuf[2] == 4)
          host_tl_defaultPasscode = (BUILD_UINT32(pBuf[3], pBuf[3 + 1], pBuf[3 + 2],
          pBuf[3 + 3]));
        else
          host_tl_defaultPasscode = ((uint32)pBuf[3]);

        stat = SUCCESS;
      }
      else
      {
        pValue = &pBuf[3];
        stat = GAPBondMgr_SetParameter(id, pBuf[2],  pValue);
      }
#else
      stat = INVALIDPARAMETER;
#endif
      break;
    }

    case HCI_EXT_GAP_BOND_GET_PARAM:
    {
      uint8_t len = 0;
#if defined(GAP_BOND_MGR)
      uint16_t id = BUILD_UINT16(pBuf[0], pBuf[1]);

      switch (id)
      {
        case GAPBOND_OOB_DATA:
          stat = GAPBondMgr_GetParameter(id, &rspBuf[RSP_PAYLOAD_IDX+2]);
          len = KEYLEN;
          break;

        case GAPBOND_ECC_KEYS:
          stat = GAPBondMgr_GetParameter(id, &rspBuf[RSP_PAYLOAD_IDX+2]);
          len = sizeof(gapBondEccKeys_t);
          break;

        case GAPBOND_DEFAULT_PASSCODE:
          stat =  SUCCESS;
          len = sizeof(host_tl_defaultPasscode);
          memcpy(&rspBuf[RSP_PAYLOAD_IDX+2], &host_tl_defaultPasscode, len);
          break;

        default:
          stat = GAPBondMgr_GetParameter(id, &rspBuf[RSP_PAYLOAD_IDX+2]);
          len = 1;
          break;
      }
#else
      stat = INVALIDPARAMETER;
#endif
      if (stat == SUCCESS)
      {
        rspBuf[RSP_PAYLOAD_IDX] = pBuf[0];  // paramID byte 0
        rspBuf[RSP_PAYLOAD_IDX+1] = pBuf[1];  // paramID byte 1
        *pRspDataLen = len + 2;
      }
      else
      {
        *pRspDataLen = 0;
      }
      break;
    }

#if !defined(GATT_NO_SERVICE_CHANGED)
    case HCI_EXT_GAP_BOND_SERVICE_CHANGE:
#if defined(GAP_BOND_MGR)
      stat = GAPBondMgr_ServiceChangeInd(BUILD_UINT16(pBuf[0], pBuf[1]), pBuf[2]);
#else
      stat = INVALIDPARAMETER;
#endif
      break;
#endif // !GATT_NO_SERVICE_CHANGED

    case HCI_EXT_GAP_BOND_GET_OOB_PARAMS:
    {
      uint8_t len = 0;
#if defined(GAP_BOND_MGR)
      stat = GAPBondMgr_SCGetLocalOOBParameters((gapBondOOBData_t *)&rspBuf[RSP_PAYLOAD_IDX]);
      len = sizeof(gapBondOOBData_t);
#else
      stat = INVALIDPARAMETER;
#endif
      if (stat == SUCCESS)
      {
        *pRspDataLen = len;
      }
      else
      {
        *pRspDataLen = 0;
      }
      break;
    }

    case HCI_EXT_GAP_BOND_SET_OOB_PARAMS:
    {
#if defined(GAP_BOND_MGR)
      stat = GAPBondMgr_SCSetRemoteOOBParameters((gapBondOOBData_t *)&pBuf[0], pBuf[2*KEYLEN]);
#else
      stat = INVALIDPARAMETER;
#endif
      break;
    }

    case HCI_EXT_GAP_BOND_PAIR:
    {
#if defined(GAP_BOND_MGR)
      stat = GAPBondMgr_Pair(BUILD_UINT16(pBuf[0], pBuf[1]));
#else
      stat = INVALIDPARAMETER;
#endif
    }
    break;

    case HCI_EXT_GAP_BOND_FIND_ADDR:
    {
#if defined(GAP_BOND_MGR)
      stat = GAPBondMgr_FindAddr(&pBuf[0],                    // input addr
                                 pBuf[B_ADDR_LEN],            // addr Type
                                 &rspBuf[RSP_PAYLOAD_IDX],    // bond index
                                 &rspBuf[RSP_PAYLOAD_IDX+1],  // id type
                                 &rspBuf[RSP_PAYLOAD_IDX+2]); // id addr

      // If a bond was found
      if (stat == SUCCESS)
      {
        *pRspDataLen = B_ADDR_LEN + 2;
      }
      else
      {
        *pRspDataLen = 0;
      }
#else
        stat = INVALIDPARAMETER;
#endif
    }
    break;

    case HCI_EXT_GAP_BOND_PASSCODE_RSP:
    {
#if defined(GAP_BOND_MGR)
      stat = GAPBondMgr_PasscodeRsp(BUILD_UINT16(pBuf[0], pBuf[1]),  // conn handle
                                    pBuf[2],                         // status
                                    BUILD_UINT32(pBuf[3], pBuf[4],
                                                 pBuf[5], pBuf[6])); // passcode
#else
      stat = INVALIDPARAMETER;
#endif
    }
    break;

    case HCI_EXT_GAP_SET_PRIVACY_MODE:
      stat = GAP_SetPrivacyMode((GAP_Peer_Addr_Types_t) pBuf[0],  // addrType
                                                       &pBuf[1],  // address
                                                        pBuf[7]); // mode
      break;
#if ( HOST_CONFIG & ( CENTRAL_CFG | PERIPHERAL_CFG ))
    case HCI_EXT_SM_REGISTER_TASK:
      SM_RegisterTask(appTaskID);
      break;

    case HCI_EXT_SM_GET_ECCKEYS:
      stat = SM_GetEccKeys();
      break;

    case HCI_EXT_SM_GET_DHKEY:
      // each field is 32 bytes
      stat = SM_GetDHKey(pBuf, &pBuf[32], &pBuf[64]);
      break;

    case HCI_EXT_SM_GET_CONFIRM_OOB:
      stat = SM_GetScConfirmOob(pBuf,
                                &pBuf[32],
                                &rspBuf[RSP_PAYLOAD_IDX]);

      *pRspDataLen = 16;
      break;
#endif /* #if ( HOST_CONFIG & ( CENTRAL_CFG | OBSERVER_CFG )) */

#if ( HOST_CONFIG & ( PERIPHERAL_CFG | BROADCASTER_CFG ) )
    case HCI_EXT_GAP_ADV_CREATE:
    {
      // Temporary memory for adv params
      GapAdv_params_t advParams;
      advParams.eventProps = BUILD_UINT16(pBuf[0], pBuf[1]);
      advParams.primIntMin = BUILD_UINT32(pBuf[2], pBuf[3], pBuf[4], 0x00);
      advParams.primIntMax = BUILD_UINT32(pBuf[5], pBuf[6], pBuf[7], 0x00);
      advParams.primChanMap = (GapAdv_advChannels_t) pBuf[8];
      advParams.peerAddrType = (GAP_Peer_Addr_Types_t) pBuf[9];
      advParams.peerAddr[0] = pBuf[10];
      advParams.peerAddr[1] = pBuf[11];
      advParams.peerAddr[2] = pBuf[12];
      advParams.peerAddr[3] = pBuf[13];
      advParams.peerAddr[4] = pBuf[14];
      advParams.peerAddr[5] = pBuf[15];
      advParams.filterPolicy = (GapAdv_filterPolicy_t) pBuf[16];
      advParams.txPower = (int8_t) pBuf[17];
      advParams.primPhy = (GapAdv_primaryPHY_t) pBuf[18];
      advParams.secPhy = (GapAdv_secondaryPHY_t) pBuf[19];
      advParams.sid = pBuf[20];

      stat = GapAdv_create(host_tl_advEvtCallback, &advParams,
                           &rspBuf[RSP_PAYLOAD_IDX]);
      *pRspDataLen = 1;
      break;
    }
    case HCI_EXT_GAP_ADV_ENABLE:
    {
      stat = GapAdv_enable(pBuf[0],                         // handle
                           pBuf[1],                         // enable options
                           BUILD_UINT16(pBuf[2], pBuf[3])); // durationOrMaxEvents
      break;
    }
    case HCI_EXT_GAP_ADV_DISABLE:
    {
      stat = GapAdv_disable(pBuf[0]);
      break;
    }
    case HCI_EXT_GAP_ADV_DESTROY:
    {
      stat = GapAdv_destroy(pBuf[0], GAP_ADV_FREE_OPTION_ADV_DATA|GAP_ADV_FREE_OPTION_SCAN_RESP_DATA);
      break;
    }
    case HCI_EXT_GAP_ADV_SET_PARAM:
    {
      stat = GapAdv_setParam(pBuf[0],   // handle
                             pBuf[1],   // ParamID
                             &pBuf[2]); // pData
      break;
    }
    case HCI_EXT_GAP_ADV_GET_PARAM:
    {
      stat = GapAdv_getParam(pBuf[0],                    // handle
                             pBuf[1],                    // paramId
                             &rspBuf[RSP_PAYLOAD_IDX+1], // pValue
                             pRspDataLen);               // pLen

      if(stat == SUCCESS)
      {
        rspBuf[RSP_PAYLOAD_IDX] = pBuf[1];  // paramID
        (*pRspDataLen)++;                   // Add 1 for paramId
      }
      else
      {
        *pRspDataLen = 0;
      }

      break;
    }
    case HCI_EXT_GAP_ADV_LOAD_DATA:
    {
      uint8_t  handle = pBuf[0];
      // Add 1 to map data type to GapAdv_freeBufferOptions_t
      GapAdv_freeBufferOptions_t freeOption = (GapAdv_freeBufferOptions_t) (pBuf[1] + 1);
      uint16_t length = BUILD_UINT16(pBuf[2], pBuf[3]);

      // Pause advertising and free old buffer if it exists
      GapAdv_prepareLoadByHandle(handle, freeOption);

      // Allocate memory for the new buffers
      uint8_t *pData = ICall_mallocLimited(length);

      // If successfully allocated
      if(pData)
      {
        // Copy data from transport layer to new buffer
        memcpy(pData, &pBuf[4], length);

        // Load new buffer and restart advertising if it was paused
        stat = GapAdv_loadByHandle(handle, pBuf[1], length, pData);
      }
      else
      {
        stat = bleMemAllocError;
      }
    }
    break;

   case HCI_EXT_GAP_ADV_SET_EVENT_MASK:
   {
     stat = GapAdv_setEventMask(pBuf[0],                             // handle
                               (GapAdv_eventMaskFlags_t)BUILD_UINT16(pBuf[1], pBuf[2])); // mask
     break;
   }
   case HCI_EXT_GAP_ADV_SET_VIRTUAL_ADV_ADDR:
   {
     stat = GapAdv_setVirtualAdvAddr(pBuf[0],   // advHandle
                                     &pBuf[1]); // bdAddr
     break;
   }

   case HCI_EXT_GAP_ADV_SET_PERIODIC_ADV_PARAMS:
   {
     GapAdv_periodicAdvParams_t pPriodicParams;

     pPriodicParams.periodicAdvIntervalMin = BUILD_UINT16(pBuf[1],pBuf[2]);
     pPriodicParams.periodicAdvIntervalMax = BUILD_UINT16(pBuf[3],pBuf[4]);
     pPriodicParams.periodicAdvProp = BUILD_UINT16(pBuf[5],pBuf[6]);
     stat = GapAdv_SetPeriodicAdvParams(pBuf[0], &pPriodicParams);
     break;
   }

   case HCI_EXT_GAP_ADV_SET_PERIODIC_ADV_DATA:
   {
     GapAdv_periodicAdvData_t pPeriodicData;

     pPeriodicData.operation = pBuf[1];
     pPeriodicData.dataLength = pBuf[2];
     pPeriodicData.pData = MAP_osal_mem_alloc(pPeriodicData.dataLength);

     if(pPeriodicData.pData)
     {
       // Copy the data
       MAP_osal_memcpy(pPeriodicData.pData, &pBuf[3], pPeriodicData.dataLength);
       stat = GapAdv_SetPeriodicAdvData( pBuf[0], &pPeriodicData);
       // Free the allocated memory
       ICall_free(pPeriodicData.pData);
     }
     break;
   }

   case HCI_EXT_GAP_ADV_SET_PERIODIC_ADV_ENABLE:
   {
     stat = GapAdv_SetPeriodicAdvEnable(pBuf[0], pBuf[1]);
     break;
   }
#endif // (PERIPHERAL_CFG | BROADCASTER_CFG )

#if ( HOST_CONFIG & ( CENTRAL_CFG | OBSERVER_CFG ) )
   case HCI_EXT_GAP_SCAN_ENABLE:
   {
     if (!host_tl_gapScannerInitialized)
     {
       stat = GapScan_registerCb(host_tl_scanEvtCallback, 0);
       if (!stat)
       {
           host_tl_gapScannerInitialized = 1;
       }
     }
     stat = GapScan_enable(BUILD_UINT16(pBuf[0], pBuf[1]), // Period
                           BUILD_UINT16(pBuf[2], pBuf[3]), // duration
                           pBuf[4]);                       // record
     break;
   }
   case HCI_EXT_GAP_SCAN_GET_ADV_REPORT:
   {
     stat = GapScan_getAdvReport(pBuf[0],
                        (GapScan_Evt_AdvRpt_t *) &rspBuf[RSP_PAYLOAD_IDX]);

     *pRspDataLen = sizeof(GapScan_Evt_AdvRpt_t) - 4;
     break;
   }
   case HCI_EXT_GAP_SCAN_DISABLE:
   {
     stat = GapScan_disable();
     break;
   }
   case HCI_EXT_GAP_SCAN_SET_PHY_PARAMS:
   {
     stat = GapScan_setPhyParams(pBuf[0],                         // PHY
                                 pBuf[1],                         // type
                                 BUILD_UINT16(pBuf[2], pBuf[3]),  // interval
                                 BUILD_UINT16(pBuf[4], pBuf[5])); // window
     break;
   }
   case HCI_EXT_GAP_SCAN_GET_PHY_PARAMS:
   {
     stat = GapScan_getPhyParams(pBuf[0],                         // PHY
                                 &rspBuf[RSP_PAYLOAD_IDX],        // type
                                 &rspBuf[RSP_PAYLOAD_IDX+1],      // interval
                                 &rspBuf[RSP_PAYLOAD_IDX+3]);     // window
     *pRspDataLen = 5;
     break;
   }
   case HCI_EXT_GAP_SCAN_SET_PARAM:
   {
     stat = GapScan_setParam(pBuf[0], &pBuf[1]);
     break;
   }
   case HCI_EXT_GAP_SCAN_GET_PARAM:
   {
     stat = GapScan_getParam(pBuf[0],                    // paramId
                             &rspBuf[RSP_PAYLOAD_IDX+1], // pValue
                             pRspDataLen);               // pLen
     if (stat == SUCCESS)
     {
       rspBuf[RSP_PAYLOAD_IDX] = pBuf[0];  // paramID
        (*pRspDataLen)++;                   // Add 1 for paramId
     }
     else
     {
       *pRspDataLen = 0;
     }
     break;
   }
   case HCI_EXT_GAP_SCAN_SET_EVENT_MASK:
   {
     GapScan_setEventMask(BUILD_UINT32(pBuf[0], pBuf[1],
                                       pBuf[2], pBuf[3]));

     // GapScan_setEventMask is a void function
     stat = SUCCESS;
     break;
   }

   case HCI_EXT_GAP_SCAN_CREATE_SYNC:
   {
     GapScan_PeriodicAdvCreateSyncParams_t syncParams;

     syncParams.options = pBuf[1];
     syncParams.advAddrType = pBuf[2];
     MAP_osal_memcpy(syncParams.advAddress, &pBuf[3], B_ADDR_LEN);
     syncParams.skip = BUILD_UINT16(pBuf[9],pBuf[10]);
     syncParams.syncTimeout = BUILD_UINT16(pBuf[11],pBuf[12]);
     syncParams.syncCteType = pBuf[13];

     stat = GapScan_PeriodicAdvCreateSync(pBuf[0], &syncParams);
     break;
    }

     case HCI_EXT_GAP_SCAN_CREATE_SYNC_CANCEL:
     {
       stat = GapScan_PeriodicAdvCreateSyncCancel();
       break;
     }

     case HCI_EXT_GAP_SCAN_TERMINATE_SYNC:
     {
       uint16_t syncHandle = BUILD_UINT16(pBuf[0],pBuf[1]);
       stat = GapScan_PeriodicAdvTerminateSync(syncHandle);
       break;
     }

     case HCI_EXT_GAP_SCAN_RECEIVE_PERIODIC_ADV_REPORT:
     {
       uint16_t syncHandle = BUILD_UINT16(pBuf[0],pBuf[1]);
       stat = GapScan_SetPeriodicAdvReceiveEnable(syncHandle, pBuf[2]);
       break;
     }

     case HCI_EXT_GAP_SCAN_ADD_DEVICE_PERIODIC_ADV_LIST:
     {
       uint8_t advAddr[B_ADDR_LEN];
       MAP_osal_memcpy(advAddr, &pBuf[1], B_ADDR_LEN);
       stat = GapScan_AddDeviceToPeriodicAdvList(pBuf[0], advAddr, pBuf[7]);
       break;
     }

     case HCI_EXT_GAP_SCAN_REMOVE_DEVICE_PERIODIC_ADV_LIST:
     {
       uint8_t advAddr[B_ADDR_LEN];
       MAP_osal_memcpy(advAddr, &pBuf[1], B_ADDR_LEN);
       stat = GapScan_RemoveDeviceFromPeriodicAdvList(pBuf[0], advAddr, pBuf[7]);
       break;
     }

     case HCI_EXT_GAP_SCAN_READ_ERIODIC_ADV_LIST_SIZE:
     {
       stat = GapScan_ReadPeriodicAdvListSize();
       break;
     }

     case HCI_EXT_GAP_SCAN_CLEAR_PERIODIC_ADV_LIST:
     {
       stat = GapScan_ClearPeriodicAdvList();
       break;
     }
#endif // ( CENTRAL_CFG | OBSERVER_CFG )

#if ( HOST_CONFIG & CENTRAL_CFG )
 case HCI_EXT_GAP_INIT_SET_PHY_PARAM:
   {
     stat = GapInit_setPhyParam(pBuf[0],                         // PHY
                                pBuf[1],                         // paramID
                                BUILD_UINT16(pBuf[2], pBuf[3])); // value
     break;
   }
   case HCI_EXT_GAP_INIT_GET_PHY_PARAM:
   {
     stat = GapInit_getPhyParam(pBuf[0],                         // PHY
                                pBuf[1],                         // paramID
                                &rspBuf[RSP_PAYLOAD_IDX+1]);     // value
     rspBuf[RSP_PAYLOAD_IDX] = pBuf[1];
     *pRspDataLen = 3;
     break;
   }
   case HCI_EXT_GAP_INIT_CONNECT:
   {
     stat = GapInit_connect(pBuf[0],                             // peerAddrType
                            &pBuf[1],                            // peerAddress
                            pBuf[7],                             // phys
                            BUILD_UINT16(pBuf[8], pBuf[9]));     // timeout
     break;
   }
   case HCI_EXT_GAP_INIT_CONNECT_WL:
   {
     stat = GapInit_connectWl(pBuf[0],                             // phys
                              BUILD_UINT16(pBuf[1], pBuf[2]));     // timeout
     break;
   }
   case HCI_EXT_GAP_INIT_CANCEL_CONNECT:
   {
     stat = GapInit_cancelConnect();
     break;
   }
#endif // CENTRAL_CFG


#ifdef BLE3_CMD
    case HCI_EXT_GAP_CONFIG_DEVICE_ADDR:
    {
      GAP_Addr_Modes_t addrType = (GAP_Addr_Modes_t)pBuf[0];
      uint8_t addr[B_ADDR_LEN] = {0};

      // When the device address is ADDRMODE_PUBLIC or ADDRMODE_RANDOM the IRK shall be all 0's
      if( (addrType == ADDRMODE_PUBLIC) ||
	   (addrType == ADDRMODE_RANDOM) )
      {
        uint8_t tempKey[KEYLEN] = {0};
        GapConfig_SetParameter(GAP_CONFIG_PARAM_IRK, tempKey);
      }

      // if address type is random or public set irk to 0 if not 0 already
      // If own address type is random static or RPA with random static...
      if( addrType == ADDRMODE_RANDOM
         || addrType == ADDRMODE_RP_WITH_RANDOM_ID )
      {
        osal_memcpy(addr, &pBuf[1], B_ADDR_LEN);

        // Check if the random address is valid
        // If all bits excluding the 2 MSBs are all 0's...
        if(((MAP_osal_isbufset(addr, 0x00, B_ADDR_LEN - 1) &&
           ((addr[B_ADDR_LEN - 1] & 0x3F) == 0)) ||
            // Or all bites are 1's
            MAP_osal_isbufset(addr, 0xFF, B_ADDR_LEN)))
        {
          return INVALIDPARAMETER;
        }
        else if(!(GAP_IS_ADDR_RS(addr)))
        {
           // This is an invalid ramdom static address
           GAP_MAKE_ADDR_RS(addr);
         }
         // If valid random static address, put it to the controller
         stat = HCI_LE_SetRandomAddressCmd(addr);
      }
      // Set the internal GAP address mode
      gapSetDevAddressMode(addrType);
      break;
    }

#if ( HOST_CONFIG & ( PERIPHERAL_CFG | BROADCASTER_CFG ) )
    case HCI_EXT_GAP_MAKE_DISCOVERABLE:
    {
      uint16_t eventProps = 0;
      GapAdv_advChannels_t chanMap = (GapAdv_advChannels_t)pBuf[8];
      GAP_Peer_Addr_Types_t addrType = (GAP_Peer_Addr_Types_t)pBuf[1];
      GapAdv_filterPolicy_t fltPolicy = (GapAdv_filterPolicy_t)pBuf[9];

      // Fix the advertising parameter
      eventProps = getBLE3ToAgamaEventProp(pBuf[0]);

      if( eventProps & GAP_ADV_PROP_CONNECTABLE )
      {
        // This is a connectable advertising
        // check if we havn't reached the maximum number of connections
        uint8_t numConns = linkDB_NumActive();

        if( numConns >= MAX_NUM_BLE_CONNS )
        {
          maxConnReached = 1;
          return SUCCESS;
        }
      }

      if( advHandleLegacy == 0xFF )
      {
        // Sanity check. In BLE3 GAP_UpdateAdvertisingData must be called first
        GapAdv_params_t advParamLegacy = GAPADV_PARAMS_LEGACY_SCANN_CONN;

        // Update the advertising parameters struct
        advParamLegacy.eventProps = eventProps;
        advParamLegacy.peerAddrType = addrType;
        advParamLegacy.primChanMap = chanMap;
        advParamLegacy.filterPolicy = fltPolicy;

        // Create Advertisement set and assign handle
        stat = GapAdv_create(&host_tl_advEvtCallback, &advParamLegacy,
                             &advHandleLegacy);
      }
      else
      {
        // Updating the advertising parameters
        stat = GapAdv_setParam(advHandleLegacy, GAP_ADV_PARAM_PROPS, &eventProps);
        if( stat != SUCCESS )
        {
          if( stat == bleNotReady )
          {
            // The advertising is already enabled
            return bleAlreadyInRequestedMode;
          }
          return stat;
        }

        stat = GapAdv_setParam(advHandleLegacy, GAP_ADV_PARAM_PEER_ADDRESS_TYPE, &addrType);
        if( stat != SUCCESS )
        {
          if( stat == bleNotReady )
          {
            return bleAlreadyInRequestedMode;
          }
          return stat;
        }

        uint8_t addr[B_ADDR_LEN];
        osal_memcpy(addr, &pBuf[2], B_ADDR_LEN);
        stat = GapAdv_setParam(advHandleLegacy, GAP_ADV_PARAM_PEER_ADDRESS, addr);
        if( stat != SUCCESS )
        {
          if( stat == bleNotReady )
          {
            return bleAlreadyInRequestedMode;
          }
          return stat;
        }

        stat = GapAdv_setParam(advHandleLegacy, GAP_ADV_PARAM_PRIMARY_CHANNEL_MAP, &chanMap);
        if( stat != SUCCESS )
        {
          if( stat == bleNotReady )
          {
            return bleAlreadyInRequestedMode;
          }
          return stat;
        }

        stat = GapAdv_setParam(advHandleLegacy, GAP_ADV_PARAM_FILTER_POLICY, &fltPolicy);
        if( stat != SUCCESS )
        {
          if( stat == bleNotReady )
          {
            return bleAlreadyInRequestedMode;
          }
          return stat;
        }
      }

      if( advNotice )
      {
        // Set event mask - Adv Notice Events enabled
        stat = GapAdv_setEventMask(advHandleLegacy, GAP_ADV_EVT_MASK_END );
      }
      else
      {
        stat = GapAdv_setEventMask(advHandleLegacy, 0 );
      }

      // To enable adv notice event HCI_EXT_AdvEventNotice should be called before each GAP_MakeDiscoverable
      advNotice = 0;

      if( stat != SUCCESS )
      {
        return stat;
      }

      // Enable this flag to mark that GapAdv_enable was called.
      makeDiscFlag = 1;

      if( advDuration == 0 )
      {
        // Infinite advertisement
        stat = GapAdv_enable(advHandleLegacy, GAP_ADV_ENABLE_OPTIONS_USE_MAX , 0);
      }
      else
      {
        // Time limeted advertising
        stat = GapAdv_enable(advHandleLegacy, GAP_ADV_ENABLE_OPTIONS_USE_DURATION, advDuration);
      }
      if( (stat != SUCCESS) && (stat != bleInvalidRange) )
      {
        // With the above GapAdv_params we should not receive bleInvalidRange from the host
        // We should not expected to receive GAP_ADV_MAKE_DISCOVERABLE_DONE_EVENT
        makeDiscFlag = 0;
      }
      break;
    }

    case HCI_EXT_GAP_END_DISC:
    {
      // Enable this flag to mark that GapAdv_disable was called.
      endDiscFlag = 1;
      stat = GapAdv_disable(advHandleLegacy);
      if( (stat != SUCCESS)  && (stat != bleInternalError) )
      {
        // The error returned from the host. We should not expected to receive GAP_ADV_END_DISCOVERABLE_DONE_EVENT
        endDiscFlag = 0;
      }
      break;
    }

    case HCI_EXT_GAP_UPDATE_ADV_DATA:
    {
      uint8_t dataType = pBuf[0];
      uint8_t dataLen = pBuf[1];

      if( advHandleLegacy == 0xFF )
      {
        GapAdv_params_t advParamLegacy = GAPADV_PARAMS_LEGACY_SCANN_CONN;

        // Create Advertisement set and assign handle
        stat = GapAdv_create(&host_tl_advEvtCallback, &advParamLegacy,
                             &advHandleLegacy);
      }

      if( stat != SUCCESS )
      {
        return stat;
      }

      uint8_t *pData = ICall_malloc(dataLen);

      if( pData == NULL )
      {
        return bleMemAllocError;
      }

      osal_memcpy(pData, &pBuf[2], dataLen);

      if( dataType == 1 )
      {
        // Advertising Data
        // Pause advertising and free old buffer if it exists
        GapAdv_prepareLoadByHandle(advHandleLegacy, GAP_ADV_FREE_OPTION_ADV_DATA);
        stat = GapAdv_loadByHandle(advHandleLegacy, GAP_ADV_DATA_TYPE_ADV, dataLen, pData);
      }
      else
      {
        // Scan response Data
        // Pause advertising and free old buffer if it exists
        GapAdv_prepareLoadByHandle(advHandleLegacy, GAP_ADV_FREE_OPTION_SCAN_RESP_DATA);
        stat = GapAdv_loadByHandle(advHandleLegacy, GAP_ADV_DATA_TYPE_SCAN_RSP, dataLen, pData);
      }
      break;
    }
#endif // ( HOST_CONFIG & ( PERIPHERAL_CFG | BROADCASTER_CFG ) )

#if ( HOST_CONFIG & ( CENTRAL_CFG | OBSERVER_CFG ) )
    case HCI_EXT_GAP_DEVICE_DISC_REQ:
    {
      uint8_t discMode = pBuf[0];
      uint8_t activeScan = pBuf[1];
      uint8_t whiteList = pBuf[2];
      uint16_t scanWin;
      uint16_t scanInt;
      GapScan_ScanType_t scanType;

      if (!host_tl_gapScannerInitialized)
      {
        stat = GapScan_registerCb(host_tl_scanEvtCallback, 0);
        if (stat != SUCCESS)
        {
          stat = bleIncorrectMode;
          break;
        }
        host_tl_gapScannerInitialized = 1;
      }

      if( scanNotice )
      {
        // With scan notice events
        GapScan_setEventMask( GAP_EVT_SCAN_DISABLED |
                              GAP_EVT_SCAN_DUR_ENDED |
                              GAP_EVT_ADV_REPORT |
                              GAP_EVT_SCAN_INT_ENDED); // could be  changed to GAP_EVT_SCAN_WND_ENDED
      }
      else
      {
        GapScan_setEventMask( GAP_EVT_SCAN_DISABLED |
                              GAP_EVT_SCAN_DUR_ENDED |
                              GAP_EVT_ADV_REPORT );
      }

      // To enable scan notice event HCI_EXT_ScanEventNotice should be called before each GAP_DeviceDiscoveryRequest
      scanNotice = 0;

      // whitelist filter options
      stat = GapScan_setParam(SCAN_PARAM_FLT_POLICY, &whiteList);
      if( stat == bleIncorrectMode )
      {
        stat = bleAlreadyInRequestedMode;
        break;
      }
      else if( (stat != SUCCESS) && (stat != bleIncorrectMode) )
      {
        stat = bleIncorrectMode;
        break;
      }

      // Discovery mode
      if( discMode == 0x03 )
      {
        discMode = SCAN_FLT_DISC_DISABLE;
      }
      stat = GapScan_setParam(SCAN_PARAM_FLT_DISC_MODE, &discMode);
      if( stat == bleIncorrectMode )
      {
        stat = bleAlreadyInRequestedMode;
        break;
      }
      else if( (stat != SUCCESS) && (stat != bleIncorrectMode) )
      {
        stat = bleIncorrectMode;
        break;
      }

      // Get the phy scanning values
      stat = GapScan_getPhyParams(SCAN_PRIM_PHY_1M, &scanType, &scanInt, &scanWin);
      if( stat != SUCCESS )
      {
        stat = bleIncorrectMode;
		break;
      }

      // Update scan type
      stat = GapScan_setPhyParams(SCAN_PRIM_PHY_1M,(GapScan_ScanType_t)activeScan, scanInt, scanWin);
      if( stat != SUCCESS )
      {
        stat = bleIncorrectMode;
        break;
      }

      stat = GapScan_enable(0, scanDuration, maxNumReports);
      if( stat == SUCCESS )
      {
        // Do nothing
      }
	  else
      {
        if( (stat != bleAlreadyInRequestedMode) && (stat != bleMemAllocError) )
        {
          stat = bleIncorrectMode;
          break;
        }
      }
      break;
    }

    case HCI_EXT_GAP_DEVICE_DISC_CANCEL:
    {
      GapScan_disable();
      break;
    }

    case HCI_EXT_GAP_EST_LINK_REQ:
    {
      // highDutyCycle - not relevant. The scan setting need to be change using GAP_SetParam with the relevant parameters ID
      uint8_t whiteList = pBuf[1];

      if( whiteList == 1 )
      {
        stat = GapInit_connectWl(INIT_PHY_1M, 0);
      }
      else if( whiteList == 0 )
      {
        GAP_Peer_Addr_Types_t addrType = (GAP_Peer_Addr_Types_t)pBuf[2];
        uint8_t addr[B_ADDR_LEN] = {0};
        // Copy the peer's addr
        osal_memcpy(addr, &pBuf[3], B_ADDR_LEN);

        stat = GapInit_connect(addrType, addr, INIT_PHY_1M, 0);
      }
	  else
	  {
	  stat = bleIncorrectMode;
	  }

      if( stat != SUCCESS)
      {
        sendEstEvt = 1;
        stat = SUCCESS;
      }
      break;
    }
#endif // ( HOST_CONFIG & ( CENTRAL_CFG | OBSERVER_CFG ) )

#endif // BLE3_CMD

    default:
      stat = FAILURE;
      break;
  }

  return(stat);
}

#if (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
/*********************************************************************
 * @fn      createMsgPayload
 *
 * @brief   Create payload buffer for OTA message to be sent.
 *
 * @param   pBuf - pointer to message data
 * @param   len - length of message data
 *
 * @return  pointer to payload. NULL if alloc fails.
 */
static uint8_t *createMsgPayload(uint8_t *pBuf, uint16_t len)
{
  return(createPayload(pBuf, len, 0));
}
#endif /* (CENTRAL_CFG | PERIPHERAL_CFG) */

#if (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
/**
 * Receive connection events
 *
 * @param pReport connection event report
 */
static void host_tl_connEvtCallback(Gap_ConnEventRpt_t *pReport)
{
  if (HCI_TL_CallbackEvtProcessCB((void *) pReport,
                                  (void *) host_tl_connEvtCallbackProcess) ==
                                  FALSE)
  {
    // Allocation of event to switch to app context failed. Consider this event
    // lost so free the corresponding data.
    ICall_free(pReport);
  }
}

/**
 * Process connection event callback in app context
 *
 * @param pReport connection event report
 */
static void host_tl_connEvtCallbackProcess(Gap_ConnEventRpt_t *pReport)
{
  uint8_t index = 0;
  // Fill up event
#ifdef BLE3_CMD
  uint8_t data[16];
  data[index++] = LO_UINT16(HCI_EXT_GAP_BLE3_CONN_EVT_NOTICE);
  data[index++] = HI_UINT16(HCI_EXT_GAP_BLE3_CONN_EVT_NOTICE);
#else
  uint8_t data[17];
  data[index++] = LO_UINT16(HCI_EXT_GAP_CONN_EVT_NOTICE);
  data[index++] = HI_UINT16(HCI_EXT_GAP_CONN_EVT_NOTICE);
#endif
  data[index++] = pReport->status;
  data[index++] = LO_UINT16(pReport->handle);
  data[index++] = HI_UINT16(pReport->handle);
  data[index++] = pReport->channel;
#ifndef BLE3_CMD
  data[index++] = pReport->phy;
#endif
  data[index++] = pReport->lastRssi;
  data[index++] = LO_UINT16(pReport->packets);
  data[index++] = HI_UINT16(pReport->packets);
  data[index++] = LO_UINT16(pReport->errors);
  data[index++] = HI_UINT16(pReport->errors);
  data[index++] = pReport->nextTaskType;
  data[index++] = BREAK_UINT32(pReport->nextTaskTime, 0);
  data[index++] = BREAK_UINT32(pReport->nextTaskTime, 1);
  data[index++] = BREAK_UINT32(pReport->nextTaskTime, 2);
  data[index++] = BREAK_UINT32(pReport->nextTaskTime, 3);

  // Send Connection Event information over transport layer
  HCI_TL_SendVSEvent(data, sizeof(data));

  // Free connection report since it has been sent over TL
  ICall_free(pReport);
}
#endif /* (CENTRAL_CFG | PERIPHERAL_CFG) */

#if defined(GAP_BOND_MGR)
/*********************************************************************
 * @fn      host_tl_passcodeCB
 *
 * @brief   Passcode callback.
 *
 * @return  none
 */
static void host_tl_passcodeCB(uint8_t *deviceAddr, uint16_t connectionHandle,
                               uint8_t uiInputs, uint8_t uiOutputs,
                               uint32_t numComparison)
{
  // If numComparison is zero and uiInputs are TRUE,
  // Send_GAPBondMgr_SetParameter shall be called
  // by the script in response to GAP_PasskeyNeeded event
  if ((numComparison != 0 || !uiInputs) && HCI_TL_CallbackEvtProcessCB)
  {
    passcodeEvtCallback_t * passcodeEvtCallback = ICall_malloc(sizeof(passcodeEvtCallback_t));

    if (passcodeEvtCallback)
    {
        passcodeEvtCallback->connectionHandle = connectionHandle;

        HCI_TL_CallbackEvtProcessCB( (void*) passcodeEvtCallback, (void*) host_tl_passcodeCBProcess);
     }
  }
}

/*********************************************************************
 * @fn      host_tl_passcodeCBProcess
 *
 * @brief   Process passcode event (translation from the callback).
 *
 * @return  none
 */
static void host_tl_passcodeCBProcess(passcodeEvtCallback_t * pPasscodeEvt)
{
  // Send passcode response right away, without going through an event...
  GAPBondMgr_PasscodeRsp(pPasscodeEvt->connectionHandle, SUCCESS, host_tl_defaultPasscode);
}
#endif //GAP_BOND_MGR

#if (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
/*********************************************************************
 * @fn      createMsgPayload
 *
 * @brief   Create payload buffer for OTA message to be sent.
 *
 * @param   sig - whether to include authentication signature
 * @param   cmd - whether it's write command
 * @param   pBuf - pointer to message data
 * @param   len - length of message data
 *
 * @return  pointer to payload. NULL if alloc fails.
 */
static uint8_t *createSignedMsgPayload(uint8_t sig, uint8_t cmd, uint8_t *pBuf, uint16_t len)
{
  if (sig && cmd)
  {
    // Need to allocate space for Authentication Signature
    return(createPayload(pBuf, len, ATT_AUTHEN_SIG_LEN));
  }

  return(createPayload(pBuf, len, 0));
}

/*********************************************************************
 * @fn      createPayload
 *
 * @brief   Create payload buffer for OTA message plus authentication signature
 *          to be sent.
 *
 * @param   pBuf - pointer to message data
 * @param   len - length of message data
 * @param   sigLen - length of authentication signature
 *
 * @return  pointer to payload. NULL if alloc fails.
 */
static uint8_t *createPayload(uint8_t *pBuf, uint16_t len, uint8_t sigLen)
{
  // Create payload buffer for OTA message to be sent
  uint8_t *pPayload = (uint8_t *) L2CAP_bm_alloc(len + ATT_OPCODE_SIZE + sigLen);
  if (pPayload != NULL)
  {

    ICall_CSState key;

    /* Critical section is entered to disable interrupts that might cause call
     * to callback due to race condition */
    key = ICall_enterCriticalSection();
    pPayload = osal_bm_adjust_header(pPayload, -ATT_OPCODE_SIZE);
    ICall_leaveCriticalSection(key);

    // Copy received data over
    VOID memcpy(pPayload, pBuf, len);

    return(pPayload);
  }

  return(NULL);
}
#endif /* (CENTRAL_CFG | PERIPHERAL_CFG) */

// Do not use SNV drivers with FPGA. Unverified behavior
/*********************************************************************
 * @fn      checkNVLen
 *
 * @brief   Checks the size of NV items.
 *
 * @param   id - NV ID.
 * @param   len - lengths in bytes of item.
 *
 * @return  SUCCESS, INVALIDPARAMETER or FAILURE
 */
static uint8_t checkNVLen(osalSnvId_t id, osalSnvLen_t len)
{
  uint8_t stat = SUCCESS;

  switch (id)
  {
    case BLE_NVID_CSRK:
    case BLE_NVID_IRK:
      if (len != KEYLEN)
      {
        stat = INVALIDPARAMETER;
      }
      break;

    default:
#if defined(GAP_BOND_MGR)
      if ((id >= BLE_NVID_GAP_BOND_START) && (id <= BLE_NVID_GAP_BOND_END))
      {
        stat = gapBondMgr_CheckNVLen(id, len);
      }
      else
#endif
#if !defined(NO_OSAL_SNV)
      if ((id >= BLE_NVID_CUST_START) && (id <= BLE_NVID_CUST_END))
      {
        stat = SUCCESS;
      }
      else
#endif
      {
        stat = INVALIDPARAMETER;  // Initialize status to failure
      }
      break;
  }

  return(stat);
}

#if (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
/*********************************************************************
 * @fn      mapATT2BLEStatus
 *
 * @brief   Map ATT error code to BLE Generic status code.
 *
 * @param   status - ATT status
 *
 * @return  BLE Generic status
 */
static uint8_t mapATT2BLEStatus(uint8_t status)
{
  uint8_t stat;

  switch (status)
  {
    case ATT_ERR_INSUFFICIENT_AUTHEN:
      // Returned from Send routines
      stat = bleInsufficientAuthen;
      break;

    case ATT_ERR_INSUFFICIENT_ENCRYPT:
      // Returned from Send routines
      stat = bleInsufficientEncrypt;
      break;

    case ATT_ERR_INSUFFICIENT_KEY_SIZE:
      // Returned from Send routines
      stat = bleInsufficientKeySize;
      break;

    default:
      stat = status;
  }

  return(stat);
}
#endif /* (CENTRAL_CFG | PERIPHERAL_CFG) */

/*********************************************************************
 * @fn      processEvents
 *
 * @brief   Process an incoming Event messages.
 *
 * @param   pMsg - message to process
 *
 * @return  TRUE to deallocate, FALSE otherwise
 */
static uint8_t processEvents(ICall_Hdr *pMsg)
{
  uint16_t msgLen = 0;
  uint8_t *pBuf = NULL;
  uint8_t allocated = FALSE;
  uint8_t deallocateIncoming = TRUE;

  VOID memset(out_msg, 0, sizeof (out_msg));

  switch (pMsg->event)
  {
    case GAP_MSG_EVENT:
      pBuf = processEventsGAP((gapEventHdr_t *)pMsg, out_msg, (uint16_t *)&msgLen, &allocated, &deallocateIncoming);
      break;

#if (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
    case SM_MSG_EVENT:
      pBuf = processEventsSM((smEventHdr_t *)pMsg, out_msg, (uint8_t *)&msgLen, &allocated);
      break;

    case L2CAP_SIGNAL_EVENT:
      pBuf = processEventsL2CAP((l2capSignalEvent_t *)pMsg, out_msg, (uint8_t *)&msgLen);
      break;

    case L2CAP_DATA_EVENT:
      pBuf = processDataL2CAP((l2capDataEvent_t *)pMsg, out_msg, &msgLen, &allocated);
      break;

    case GATT_MSG_EVENT:
      pBuf = processEventsGATT((gattMsgEvent_t *)pMsg, out_msg, (uint8_t *)&msgLen, &allocated);
      break;
#if !defined(GATT_DB_OFF_CHIP)
    case GATT_SERV_MSG_EVENT:
      pBuf = processEventsGATTServ((gattEventHdr_t *)pMsg, out_msg, (uint8_t *)&msgLen);
      break;
#endif
#endif /*  (CENTRAL_CFG | PERIPHERAL_CFG) */
    default:
      break; // ignore
  }

  if (msgLen)
  {
    HCI_TL_SendVSEvent(pBuf, msgLen);
    //HCI_SendControllerToHostEvent(HCI_VE_EVENT_CODE,  msgLen, pBuf);
  }

  if ((pBuf != NULL) && (allocated == TRUE))
  {
    ICall_free(pBuf);

    if (!deallocateIncoming)
    {
      return(FALSE);
    }
  }

  // Free the pLongMsg allocation
  if(pLongMsg)
  {
    MAP_osal_mem_free(pLongMsg);
  }

  return(TRUE);
}

/*********************************************************************
 * @fn      processEventsGAP
 *
 * @brief   Process an incoming GAP Event messages.
 *
 * @param   pMsg - message to process
 * @param   pOutMsg - outgoing message to be built
 * @param   pMsgLen - length of outgoing message
 * @param   pAllocated - whether outgoing message is locally allocated
 * @param   pDeallocate - whether it's safe to deallocate incoming message
 *
 * @return  outgoing message
 */
static uint8_t *processEventsGAP(gapEventHdr_t *pMsg, uint8_t *pOutMsg, uint16_t *pMsgLen,
                                uint8_t *pAllocated, uint8_t *pDeallocate)
{
  uint16_t msgLen = 0;
  uint8_t *pBuf = NULL;

  *pDeallocate = TRUE;

  switch (pMsg->opcode)
  {
    case GAP_DEVICE_INIT_DONE_EVENT:
      {
        gapDeviceInitDoneEvent_t *pPkt = (gapDeviceInitDoneEvent_t *)pMsg;

        pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_DEVICE_INIT_DONE_EVENT);
        pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_DEVICE_INIT_DONE_EVENT);
        pOutMsg[2] = pPkt->hdr.status;
        VOID memcpy(&pOutMsg[3], pPkt->devAddr, B_ADDR_LEN);
        pOutMsg[9]  = LO_UINT16(pPkt->dataPktLen);
        pOutMsg[10] = HI_UINT16(pPkt->dataPktLen);
        pOutMsg[11] = pPkt->numDataPkts;

        // Copy the Device's local keys
        VOID memcpy(&pOutMsg[12], (uint8_t*) GAP_GetIRK(), KEYLEN);
        VOID memcpy(&pOutMsg[12+KEYLEN], (uint8_t*) GAP_GetSRK(), KEYLEN);

        pBuf = pOutMsg;
        msgLen = 44;
      }
      break;

    case GAP_LINK_ESTABLISHED_EVENT:
      {
        gapEstLinkReqEvent_t *pPkt = (gapEstLinkReqEvent_t *)pMsg;

        pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_LINK_ESTABLISHED_EVENT);
        pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_LINK_ESTABLISHED_EVENT);
        pOutMsg[2] = pPkt->hdr.status;
        pOutMsg[3] = pPkt->devAddrType;
        VOID memcpy(&(pOutMsg[4]), pPkt->devAddr, B_ADDR_LEN);
        pOutMsg[10] = LO_UINT16(pPkt->connectionHandle);
        pOutMsg[11] = HI_UINT16(pPkt->connectionHandle);
        pOutMsg[12] = pPkt->connRole;
        pOutMsg[13] = LO_UINT16(pPkt->connInterval);
        pOutMsg[14] = HI_UINT16(pPkt->connInterval);
        pOutMsg[15] = LO_UINT16(pPkt->connLatency);
        pOutMsg[16] = HI_UINT16(pPkt->connLatency);
        pOutMsg[17] = LO_UINT16(pPkt->connTimeout);
        pOutMsg[18] = HI_UINT16(pPkt->connTimeout);
        pOutMsg[19] = pPkt->clockAccuracy;
        pBuf = pOutMsg;
        msgLen = 20;
      }
      break;

    case GAP_LINK_TERMINATED_EVENT:
      {
        gapTerminateLinkEvent_t *pPkt = (gapTerminateLinkEvent_t *)pMsg;

        pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_LINK_TERMINATED_EVENT);
        pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_LINK_TERMINATED_EVENT);
        pOutMsg[2] = pPkt->hdr.status;
        pOutMsg[3] = LO_UINT16(pPkt->connectionHandle);
        pOutMsg[4] = HI_UINT16(pPkt->connectionHandle);
        pOutMsg[5] = pPkt->reason;
        pBuf = pOutMsg;
        msgLen = 6;
      }
      break;

    case GAP_UPDATE_LINK_PARAM_REQ_EVENT:
      {
        gapUpdateLinkParamReqEvent_t *pPkt =
                                           (gapUpdateLinkParamReqEvent_t *)pMsg;

        pOutMsg[0]  = LO_UINT16(HCI_EXT_GAP_LINK_PARAM_UPDATE_REQ_EVENT);
        pOutMsg[1]  = HI_UINT16(HCI_EXT_GAP_LINK_PARAM_UPDATE_REQ_EVENT);
        pOutMsg[2]  = pPkt->hdr.status;
        pOutMsg[3]  = LO_UINT16(pPkt->req.connectionHandle);
        pOutMsg[4]  = HI_UINT16(pPkt->req.connectionHandle);
        pOutMsg[5]  = LO_UINT16(pPkt->req.intervalMin);
        pOutMsg[6]  = HI_UINT16(pPkt->req.intervalMin);
        pOutMsg[7]  = LO_UINT16(pPkt->req.intervalMax);
        pOutMsg[8]  = HI_UINT16(pPkt->req.intervalMax);
        pOutMsg[9]  = LO_UINT16(pPkt->req.connLatency);
        pOutMsg[10] = HI_UINT16(pPkt->req.connLatency);
        pOutMsg[11] = LO_UINT16(pPkt->req.connTimeout);
        pOutMsg[12] = HI_UINT16(pPkt->req.connTimeout);
        pOutMsg[13] = pPkt->req.signalIdentifier;
        pBuf = pOutMsg;
        msgLen = 14;
      }
      break;

    case GAP_LINK_PARAM_UPDATE_EVENT:
      {
        gapLinkUpdateEvent_t *pPkt = (gapLinkUpdateEvent_t *)pMsg;

        pOutMsg[0]  = LO_UINT16(HCI_EXT_GAP_LINK_PARAM_UPDATE_EVENT);
        pOutMsg[1]  = HI_UINT16(HCI_EXT_GAP_LINK_PARAM_UPDATE_EVENT);
        pOutMsg[2]  = pPkt->hdr.status;
        pOutMsg[3]  = LO_UINT16(pPkt->connectionHandle);
        pOutMsg[4]  = HI_UINT16(pPkt->connectionHandle);
        pOutMsg[5]  = LO_UINT16(pPkt->connInterval);
        pOutMsg[6]  = HI_UINT16(pPkt->connInterval);
        pOutMsg[7]  = LO_UINT16(pPkt->connLatency);
        pOutMsg[8]  = HI_UINT16(pPkt->connLatency);
        pOutMsg[9]  = LO_UINT16(pPkt->connTimeout);
        pOutMsg[10] = HI_UINT16(pPkt->connTimeout);
        pBuf = pOutMsg;
        msgLen = 11;
      }
      break;

    case GAP_SIGNATURE_UPDATED_EVENT:
      {
        gapSignUpdateEvent_t *pPkt = (gapSignUpdateEvent_t *)pMsg;

        pOutMsg[0]  = LO_UINT16(HCI_EXT_GAP_SIGNATURE_UPDATED_EVENT);
        pOutMsg[1]  = HI_UINT16(HCI_EXT_GAP_SIGNATURE_UPDATED_EVENT);
        pOutMsg[2]  = pPkt->hdr.status;
        pOutMsg[3]  = pPkt->addrType;
        VOID memcpy(&(pOutMsg[4]), pPkt->devAddr, B_ADDR_LEN);
        pOutMsg[10] = BREAK_UINT32(pPkt->signCounter, 0);
        pOutMsg[11] = BREAK_UINT32(pPkt->signCounter, 1);
        pOutMsg[12] = BREAK_UINT32(pPkt->signCounter, 2);
        pOutMsg[13] = BREAK_UINT32(pPkt->signCounter, 3);
        pBuf = pOutMsg;
        msgLen = 14;
      }
      break;

    case GAP_PASSKEY_NEEDED_EVENT:
      {
        gapPasskeyNeededEvent_t *pPkt = (gapPasskeyNeededEvent_t *)pMsg;

        pOutMsg[0]  = LO_UINT16(HCI_EXT_GAP_PASSKEY_NEEDED_EVENT);
        pOutMsg[1]  = HI_UINT16(HCI_EXT_GAP_PASSKEY_NEEDED_EVENT);
        pOutMsg[2]  = pPkt->hdr.status;
        VOID memcpy(&(pOutMsg[3]), pPkt->deviceAddr, B_ADDR_LEN);
        pOutMsg[9] = LO_UINT16(pPkt->connectionHandle);
        pOutMsg[10] = HI_UINT16(pPkt->connectionHandle);
        pOutMsg[11] = pPkt->uiInputs;
        pOutMsg[12] = pPkt->uiOutputs;
        VOID memcpy(&(pOutMsg[13]), &pPkt->numComparison, 4);
        pBuf = pOutMsg;
        msgLen = 17;
      }
      break;

    case GAP_AUTHENTICATION_COMPLETE_EVENT:
      {
        gapAuthCompleteEvent_t *pPkt = (gapAuthCompleteEvent_t *)pMsg;

        msgLen = 107;

        pBuf = ICall_malloc(msgLen);
        if (pBuf)
        {
          uint8_t *buf = pBuf;

          *pAllocated = TRUE;

          VOID memset(buf, 0, msgLen);

          *buf++  = LO_UINT16(HCI_EXT_GAP_AUTH_COMPLETE_EVENT);
          *buf++  = HI_UINT16(HCI_EXT_GAP_AUTH_COMPLETE_EVENT);
          *buf++  = pPkt->hdr.status;
          *buf++  = LO_UINT16(pPkt->connectionHandle);
          *buf++  = HI_UINT16(pPkt->connectionHandle);
          *buf++  = pPkt->authState;

          if (pPkt->pSecurityInfo)
          {
            *buf++ = TRUE;
            *buf++ = pPkt->pSecurityInfo->keySize;
            VOID memcpy(buf, pPkt->pSecurityInfo->ltk, KEYLEN);
            buf += KEYLEN;
            *buf++ = LO_UINT16(pPkt->pSecurityInfo->div);
            *buf++ = HI_UINT16(pPkt->pSecurityInfo->div);
            VOID memcpy(buf, pPkt->pSecurityInfo->rand, B_RANDOM_NUM_SIZE);
            buf += B_RANDOM_NUM_SIZE;
          }
          else
          {
            // Skip securityInfo
            buf += 1 + KEYLEN + B_RANDOM_NUM_SIZE + 2 + 1;
          }

          if (pPkt->pDevSecInfo)
          {
            *buf++ = TRUE;
            *buf++ = pPkt->pDevSecInfo->keySize;
            VOID memcpy(buf, pPkt->pDevSecInfo->ltk, KEYLEN);
            buf += KEYLEN;
            *buf++ = LO_UINT16(pPkt->pDevSecInfo->div);
            *buf++ = HI_UINT16(pPkt->pDevSecInfo->div);
            VOID memcpy(buf, pPkt->pDevSecInfo->rand, B_RANDOM_NUM_SIZE);
            buf += B_RANDOM_NUM_SIZE;
          }
          else
          {
            // Skip securityInfo
            buf += 1 + KEYLEN + B_RANDOM_NUM_SIZE + 2 + 1;
          }

          if (pPkt->pIdentityInfo)
          {
            *buf++ = TRUE;
            VOID memcpy(buf, pPkt->pIdentityInfo->irk, KEYLEN);
            buf += KEYLEN;
            VOID memcpy(buf, pPkt->pIdentityInfo->bd_addr, B_ADDR_LEN);
            buf += B_ADDR_LEN;
            *buf++ = pPkt->pIdentityInfo->addrType;
          }
          else
          {
            // Skip identityInfo
            buf += KEYLEN + B_ADDR_LEN + 2;
          }

          if (pPkt->pSigningInfo)
          {
            *buf++ = TRUE;
            VOID memcpy(buf, pPkt->pSigningInfo->srk, KEYLEN);
            buf += KEYLEN;

            *buf++ = BREAK_UINT32(pPkt->pSigningInfo->signCounter, 0);
            *buf++ = BREAK_UINT32(pPkt->pSigningInfo->signCounter, 1);
            *buf++ = BREAK_UINT32(pPkt->pSigningInfo->signCounter, 2);
            *buf = BREAK_UINT32(pPkt->pSigningInfo->signCounter, 3);
          }
        }
        else
        {
          pOutMsg[0]  = LO_UINT16(HCI_EXT_GAP_AUTH_COMPLETE_EVENT);
          pOutMsg[1]  = HI_UINT16(HCI_EXT_GAP_AUTH_COMPLETE_EVENT);
          pOutMsg[2]  = bleMemAllocError;
          pOutMsg[3]  = LO_UINT16(pPkt->connectionHandle);
          pOutMsg[4]  = HI_UINT16(pPkt->connectionHandle);
          pBuf = pOutMsg;
          msgLen = 5;
        }
      }
      break;

    case GAP_BOND_COMPLETE_EVENT:
      {
        gapBondCompleteEvent_t *pPkt = (gapBondCompleteEvent_t *)pMsg;

        pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_BOND_COMPLETE_EVENT);
        pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_BOND_COMPLETE_EVENT);
        pOutMsg[2] = pPkt->hdr.status;
        pOutMsg[3] = LO_UINT16(pPkt->connectionHandle);
        pOutMsg[4] = HI_UINT16(pPkt->connectionHandle);
        pBuf = pOutMsg;
        msgLen = 5;
      }
      break;

    case GAP_PAIRING_REQ_EVENT:
      {
        gapPairingReqEvent_t *pPkt = (gapPairingReqEvent_t *)pMsg;
        uint8_t tmp = 0;

        pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_PAIRING_REQ_EVENT);
        pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_PAIRING_REQ_EVENT);
        pOutMsg[2] = pPkt->hdr.status;
        pOutMsg[3] = LO_UINT16(pPkt->connectionHandle);
        pOutMsg[4] = HI_UINT16(pPkt->connectionHandle);
        pOutMsg[5] = pPkt->pairReq.ioCap;
        pOutMsg[6] = pPkt->pairReq.oobDataFlag;
        pOutMsg[7] = pPkt->pairReq.authReq;
        pOutMsg[8] = pPkt->pairReq.maxEncKeySize;

        tmp |= (pPkt->pairReq.keyDist.sEncKey) ? KEYDIST_SENC : 0;
        tmp |= (pPkt->pairReq.keyDist.sIdKey) ? KEYDIST_SID : 0;
        tmp |= (pPkt->pairReq.keyDist.sSign) ? KEYDIST_SSIGN : 0;
        tmp |= (pPkt->pairReq.keyDist.sLinkKey) ? KEYDIST_SLINK : 0;
        tmp |= (pPkt->pairReq.keyDist.mEncKey) ? KEYDIST_MENC : 0;
        tmp |= (pPkt->pairReq.keyDist.mIdKey) ? KEYDIST_MID : 0;
        tmp |= (pPkt->pairReq.keyDist.mSign) ? KEYDIST_MSIGN : 0;
        tmp |= (pPkt->pairReq.keyDist.mLinkKey) ? KEYDIST_MLINK : 0;
        pOutMsg[9] = tmp;

        pBuf = pOutMsg;
        msgLen = 10;
      }
      break;

    case GAP_SLAVE_REQUESTED_SECURITY_EVENT:
      {
        gapSlaveSecurityReqEvent_t *pPkt = (gapSlaveSecurityReqEvent_t *)pMsg;

        pOutMsg[0]  = LO_UINT16(HCI_EXT_GAP_SLAVE_REQUESTED_SECURITY_EVENT);
        pOutMsg[1]  = HI_UINT16(HCI_EXT_GAP_SLAVE_REQUESTED_SECURITY_EVENT);
        pOutMsg[2]  = pPkt->hdr.status;
        pOutMsg[3]  = LO_UINT16(pPkt->connectionHandle);
        pOutMsg[4]  = HI_UINT16(pPkt->connectionHandle);
        VOID memcpy(&(pOutMsg[5]), pPkt->deviceAddr, B_ADDR_LEN);
        pOutMsg[11] = pPkt->authReq;
        pBuf = pOutMsg;
        msgLen = 12;
      }
      break;

    case GAP_CONNECTING_CANCELLED_EVENT:
      {
#ifdef BLE3_CMD
        if( !legacyConnCancel )
        {
#endif
          gapConnCancelledEvent_t *pPkt = (gapConnCancelledEvent_t *)pMsg;

          pOutMsg[0]  = LO_UINT16(HCI_EXT_GAP_CONNECTING_CANCELLED_EVENT);
          pOutMsg[1]  = HI_UINT16(HCI_EXT_GAP_CONNECTING_CANCELLED_EVENT);
          pOutMsg[2]  = pPkt->hdr.status;
          pBuf = pOutMsg;
          msgLen = 3;
#ifdef BLE3_CMD
        }
        else
        {
          // Send Link establish event with status "bleGAPConnNotAcceptable"

          pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_LINK_ESTABLISHED_EVENT);
          pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_LINK_ESTABLISHED_EVENT);
          pOutMsg[2] = bleGAPConnNotAcceptable;
          pOutMsg[3] = 0;
          memset(&(pOutMsg[4]), 0, B_ADDR_LEN);
          pOutMsg[10] = 0;
          pOutMsg[11] = 0;
          pOutMsg[12] = GAP_PROFILE_CENTRAL;
          pOutMsg[13] = 0;
          pOutMsg[14] = 0;
          pOutMsg[15] = 0;
          pOutMsg[16] = 0;
          pOutMsg[17] = 0;
          pOutMsg[18] = 0;
          pOutMsg[19] = 0;
          pBuf = pOutMsg;
          msgLen = 20;

          legacyConnCancel = 0;
        }
#endif
      }
      break;

    case GAP_BOND_LOST_EVENT:
      {
        gapBondLostEvent_t *pPkt = (gapBondLostEvent_t *)pMsg;

        pOutMsg[0]  = LO_UINT16(HCI_EXT_GAP_BOND_LOST_EVENT);
        pOutMsg[1]  = HI_UINT16(HCI_EXT_GAP_BOND_LOST_EVENT);
        pOutMsg[2]  = pPkt->hdr.status;
        pOutMsg[3]  = LO_UINT16(pPkt->connectionHandle);
        pOutMsg[4]  = HI_UINT16(pPkt->connectionHandle);
        VOID memcpy(&(pOutMsg[5]), pPkt->deviceAddr, B_ADDR_LEN);
        pBuf = pOutMsg;
        msgLen = 11;
      }
      break;

      case GAP_ADV_SET_PERIODIC_ADV_PARAMS_EVENT:
      {
        GapAdv_periodicAdvEvt_t *pPkt = (GapAdv_periodicAdvEvt_t*)pMsg;

        pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_ADV_SET_PERIODIC_ADV_PARAMS_EVENT);
        pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_ADV_SET_PERIODIC_ADV_PARAMS_EVENT);
        pOutMsg[2] = pPkt->status;
        pBuf = pOutMsg;
        msgLen = 3;

        break;
      }

      case GAP_ADV_SET_PERIODIC_ADV_DATA_EVENT:
      {
        GapAdv_periodicAdvEvt_t *pPkt = (GapAdv_periodicAdvEvt_t*)pMsg;

        pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_ADV_SET_PERIODIC_ADV_DATA_EVENT);
        pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_ADV_SET_PERIODIC_ADV_DATA_EVENT);
        pOutMsg[2] = pPkt->status;
        pBuf = pOutMsg;
        msgLen = 3;
        break;
      }

      case GAP_ADV_SET_PERIODIC_ADV_ENABLE_EVENT:
      {
        GapAdv_periodicAdvEvt_t *pPkt = (GapAdv_periodicAdvEvt_t*)pMsg;

        pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_ADV_SET_PERIODIC_ADV_ENABLE_EVENT);
        pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_ADV_SET_PERIODIC_ADV_ENABLE_EVENT);
        pOutMsg[2] = pPkt->status;
        pBuf = pOutMsg;
        msgLen = 3;
        break;
      }

      case GAP_SCAN_PERIODIC_ADV_SYNC_EST_EVENT:
      {
        GapScan_Evt_PeriodicAdvSyncEst_t *pEvt = (GapScan_Evt_PeriodicAdvSyncEst_t *)pMsg;

        if( pEvt != NULL )
        {
          pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_SCAN_PERIODIC_SYNC_EST_EVENT);
          pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_SCAN_PERIODIC_SYNC_EST_EVENT);
          pOutMsg[2] = pEvt->BLEEventCode;
          pOutMsg[3] = pEvt->status;
          pOutMsg[4] = LO_UINT16(pEvt->syncHandle);
          pOutMsg[5] = HI_UINT16(pEvt->syncHandle);
          pOutMsg[6] = pEvt->advSid;
          pOutMsg[7] = pEvt->advPhy;
          pOutMsg[8] = LO_UINT16(pEvt->periodicAdvInt);
          pOutMsg[9] = HI_UINT16(pEvt->periodicAdvInt);
          pOutMsg[10] = pEvt->advClockAccuracy;
          pOutMsg[11] = pEvt->advAddrType;
          MAP_osal_memcpy(&(pOutMsg[12]), pEvt->advAddress, B_ADDR_LEN);

          pBuf = pOutMsg;
          msgLen = 18;
        }
        break;
      }

      case GAP_SCAN_PERIODIC_ADV_SYNC_LOST_EVENT:
      {
        GapScan_PeriodicAdvSyncLostEvt_t *pEvt = (GapScan_PeriodicAdvSyncLostEvt_t *)pMsg;

        pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_SCAN_PERIODIC_ADV_SYNC_LOST_EVENT);
        pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_SCAN_PERIODIC_ADV_SYNC_LOST_EVENT);
        pOutMsg[2] = LO_UINT16(pEvt->syncHandle);
        pOutMsg[3] = HI_UINT16(pEvt->syncHandle);

        pBuf = pOutMsg;
        msgLen = 4;
        break;
      }

      case GAP_SCAN_PERIODIC_ADV_REPORT_EVENT:
      {
        GapScan_Evt_PeriodicAdvRpt_t *pEvt = (GapScan_Evt_PeriodicAdvRpt_t*)pMsg;
        uint8_t dataLen = pEvt->dataLen;
        uint16_t totalLen = sizeof(GapScan_Evt_PeriodicAdvRpt_t)-sizeof(((GapScan_Evt_PeriodicAdvRpt_t *)0)->pData) + dataLen;
        pLongMsg = (uint8_t*)MAP_osal_mem_alloc(totalLen);

        if(pLongMsg)
        {
          pLongMsg[0] = LO_UINT16(HCI_EXT_GAP_SCAN_PERIODIC_ADV_REPORT_EVENT);
          pLongMsg[1] = HI_UINT16(HCI_EXT_GAP_SCAN_PERIODIC_ADV_REPORT_EVENT);
          pLongMsg[2] = LO_UINT16(pEvt->syncHandle);
          pLongMsg[3] = HI_UINT16(pEvt->syncHandle);
          pLongMsg[4] = pEvt->txPower;
          pLongMsg[5] = pEvt->rssi;
          pLongMsg[6] = pEvt->cteType;
          pLongMsg[7] = pEvt->dataStatus;
          pLongMsg[8] = pEvt->dataLen;
          MAP_osal_memcpy(&pLongMsg[9], pEvt->pData, pEvt->dataLen);

          MAP_osal_mem_free(pEvt->pData);
          pBuf = pLongMsg;
          msgLen = 9 + pEvt->dataLen;
        }
        else
        {
          HCI_TL_sendSystemReport(HOST_TL_ID, LL_STATUS_ERROR_OUT_OF_HEAP, HCI_EXT_GAP_ADV_SCAN_EVENT);
        }
        break;
      }

      case GAP_SCAN_CREATE_SYNC_EVENT:
      {
        GapScan_PeriodicAdvEvt_t *pPkt = (GapScan_PeriodicAdvEvt_t*)pMsg;

        pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_SCAN_CREATE_SYNC_EVENT);
        pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_SCAN_CREATE_SYNC_EVENT);
        pOutMsg[2] = pPkt->status;
        pBuf = pOutMsg;
        msgLen = 3;
        break;
      }

      case GAP_SCAN_SYNC_CANCEL_EVENT:
      {
        GapScan_PeriodicAdvEvt_t *pPkt = (GapScan_PeriodicAdvEvt_t*)pMsg;

        pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_SCAN_CREATE_SYNC_CANCEL_EVENT);
        pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_SCAN_CREATE_SYNC_CANCEL_EVENT);
        pOutMsg[2] = pPkt->status;
        pBuf = pOutMsg;
        msgLen = 3;
        break;
      }

      case GAP_SCAN_TERMINATE_SYNC_EVENT:
      {
        GapScan_PeriodicAdvEvt_t *pPkt = (GapScan_PeriodicAdvEvt_t*)pMsg;

        pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_SCAN_TERMINATE_SYNC_EVENT);
        pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_SCAN_TERMINATE_SYNC_EVENT);
        pOutMsg[2] = pPkt->status;
        pBuf = pOutMsg;
        msgLen = 3;
        break;
      }

      case GAP_SCAN_PERIODIC_RECEIVE_EVENT:
      {
        GapScan_PeriodicAdvEvt_t *pPkt = (GapScan_PeriodicAdvEvt_t*)pMsg;

        pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_SCAN_RECEIVE_PERIODIC_ADV_EVENT);
        pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_SCAN_RECEIVE_PERIODIC_ADV_EVENT);
        pOutMsg[2] = pPkt->status;
        pBuf = pOutMsg;
        msgLen = 3;
        break;
      }

      case GAP_SCAN_ADD_DEVICE_ADV_LIST_EVENT:
      {
        GapScan_PeriodicAdvEvt_t *pPkt = (GapScan_PeriodicAdvEvt_t*)pMsg;

        pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_SCAN_ADD_DEVICE_PERIODIC_ADV_LIST_EVENT);
        pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_SCAN_ADD_DEVICE_PERIODIC_ADV_LIST_EVENT);
        pOutMsg[2] = pPkt->status;
        pBuf = pOutMsg;
        msgLen = 3;
        break;
      }

      case GAP_SCAN_REMOVE_DEVICE_ADV_LIST_EVENT:
      {
        GapScan_PeriodicAdvEvt_t *pPkt = (GapScan_PeriodicAdvEvt_t*)pMsg;

        pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_SCAN_REMOVE_DEVICE_PERIODIC_ADV_LIST_EVENT);
        pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_SCAN_REMOVE_DEVICE_PERIODIC_ADV_LIST_EVENT);
        pOutMsg[2] = pPkt->status;
        pBuf = pOutMsg;
        msgLen = 3;
        break;
      }

      case GAP_SCAN_CLEAR_ADV_LIST_EVENT:
      {
        GapScan_PeriodicAdvEvt_t *pPkt = (GapScan_PeriodicAdvEvt_t*)pMsg;

        pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_SCAN_CLEAR_PERIODIC_ADV_LIST_EVENT);
        pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_SCAN_CLEAR_PERIODIC_ADV_LIST_EVENT);
        pOutMsg[2] = pPkt->status;
        pBuf = pOutMsg;
        msgLen = 3;
        break;
      }

      case GAP_SCAN_READ_ADV_LIST_SIZE_EVENT:
      {
        GapScan_ReadAdvSizeListEvt_t *pPkt = (GapScan_ReadAdvSizeListEvt_t*)pMsg;

        pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_SCAN_READ_PERIODIC_ADV_LIST_SIZE_EVENT);
        pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_SCAN_READ_PERIODIC_ADV_LIST_SIZE_EVENT);
        pOutMsg[2] = pPkt->status;
        pOutMsg[3] = pPkt->listSize;

        pBuf = pOutMsg;
        msgLen = 4;
        break;
      }

#ifdef BLE3_CMD
#if ( HOST_CONFIG & ( PERIPHERAL_CFG | BROADCASTER_CFG ) )
      case GAP_ADV_DATA_UPDATE_DONE_EVENT:
      {
        msgLen = 0;

        if (advEventCntr == 2)
        {
          GapAdv_dataUpdateEvent_t *pPkt = (GapAdv_dataUpdateEvent_t *)pMsg;

          pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_ADV_DATA_UPDATE_DONE_EVENT);
          pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_ADV_DATA_UPDATE_DONE_EVENT);
          pOutMsg[2] = pPkt->hdr.status;
          pOutMsg[3] = pPkt->dataType;
          pBuf = pOutMsg;
          msgLen = 4;
        }
        else
        {
          advEventCntr++;
        }
        break;
      }

      case GAP_ADV_MAKE_DISCOVERABLE_DONE_EVENT:
      {
        if( makeDiscFlag )
        {
          // This event is a response to GapAdv_enable
          GapAv_setAdvStatusDoneEvent_t *pPkt = (GapAv_setAdvStatusDoneEvent_t*)pMsg;

          pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_MAKE_DISCOVERABLE_DONE_EVENT);
          pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_MAKE_DISCOVERABLE_DONE_EVENT);
          pOutMsg[2] = pPkt->hdr.status;
          pBuf = pOutMsg;
          msgLen = 3;

          makeDiscFlag = 0;
        }
        break;
      }

      case GAP_ADV_END_DISCOVERABLE_DONE_EVENT:
      {
        if( endDiscFlag )
        {
          // This event is a response to GapAdv_disable
          GapAv_setAdvStatusDoneEvent_t *pPkt = (GapAv_setAdvStatusDoneEvent_t*)pMsg;

          pOutMsg[0] = LO_UINT16(HCI_EXT_GAP_END_DISCOVERABLE_DONE_EVENT);
          pOutMsg[1] = HI_UINT16(HCI_EXT_GAP_END_DISCOVERABLE_DONE_EVENT);
          pOutMsg[2] = pPkt->hdr.status;
          pBuf = pOutMsg;
          msgLen = 3;

          endDiscFlag = 0;
        }
        break;
      }
#endif // HOST_CONFIG & ( PERIPHERAL_CFG | BROADCASTER_CFG)
#endif // BLE3_CMD

    default:
      // Unknown command
      break;
  }

  *pMsgLen = msgLen;

  return(pBuf);
}

#if (HOST_CONFIG & (CENTRAL_CFG | PERIPHERAL_CFG))
/*********************************************************************
 * @fn      processEventsSM
 *
 * @brief   Process an incoming SM Event messages.
 *
 * @param   pMsg       - packet to process
 * @param   pOutMsg    - outgoing message to be built
 * @param   pMsgLen    - length of outgoing message
 * @param   pAllocated - whether outgoing message is locally allocated
 *
 * @return  outgoing message
 */
static uint8_t *processEventsSM(smEventHdr_t *pMsg, uint8_t *pOutMsg,
                               uint8_t *pMsgLen, uint8_t *pAllocated)
{
  uint8_t msgLen = 0;
  uint8_t *pBuf = NULL;

  switch (pMsg->opcode)
  {
    case SM_ECC_KEYS_EVENT:
        {
          smEccKeysEvt_t *keys = (smEccKeysEvt_t *)pMsg;

          msgLen = (SM_ECC_KEY_LEN * 3) + 3;

          pBuf = ICall_malloc(msgLen);
          if (pBuf)
          {
            uint8_t *buf = pBuf;

            *buf++ = LO_UINT16(HCI_EXT_SM_GET_ECC_KEYS_EVENT);
            *buf++ = HI_UINT16(HCI_EXT_SM_GET_ECC_KEYS_EVENT);
            *buf++ = keys->hdr.status;
            memcpy(buf, keys->privateKey, SM_ECC_KEY_LEN);
            buf+=32;
            memcpy(buf, keys->publicKeyX, SM_ECC_KEY_LEN);
            buf+=32;
            memcpy(buf, keys->publicKeyY, SM_ECC_KEY_LEN);

            *pAllocated = TRUE;
          }
          else
          {
            keys->hdr.status = bleMemAllocError;
            msgLen = 0;
          }
        }
        break;

      case SM_DH_KEY_EVENT:
        {
          smDhKeyEvt_t *dhKey = (smDhKeyEvt_t *)pMsg;

          msgLen = SM_ECC_KEY_LEN + 3;

          pBuf = ICall_malloc(msgLen);
          if (pBuf)
          {
            uint8_t *buf = pBuf;
            *buf++ = LO_UINT16(HCI_EXT_SM_GET_DH_KEY_EVENT);
            *buf++ = HI_UINT16(HCI_EXT_SM_GET_DH_KEY_EVENT);
            *buf++ = dhKey->hdr.status;
            memcpy(buf, dhKey->dhKey, SM_ECC_KEY_LEN);

            *pAllocated = TRUE;
          }
          else
          {
            dhKey->hdr.status = bleMemAllocError;
            msgLen = 0;
          }
        }
        break;

      default:
        break;
  }

  *pMsgLen = msgLen;

  return(pBuf);
}


/*********************************************************************
 * @fn      processEventsL2CAP
 *
 * @brief   Process an incoming L2CAP Event messages.
 *
 * @param   pPkt - packet to process
 * @param   pOutMsg - outgoing message to be built
 * @param   pMsgLen - length of outgoing message
 *
 * @return  outgoing message
 */
static uint8_t *processEventsL2CAP(l2capSignalEvent_t *pPkt, uint8_t *pOutMsg, uint8_t *pMsgLen)
{
  uint8_t msgLen;

  // Build the message header first
  msgLen = buildHCIExtHeader(pOutMsg, (HCI_EXT_L2CAP_EVENT | pPkt->opcode),
                              pPkt->hdr.status, pPkt->connHandle);
#if defined(BLE_V41_FEATURES) && (BLE_V41_FEATURES & L2CAP_COC_CFG)
  // Build Send SDU Done event regardless of status
  if (pPkt->opcode == L2CAP_SEND_SDU_DONE_EVT)
  {
    l2capSendSduDoneEvt_t *pSduEvt = &(pPkt->cmd.sendSduDoneEvt);

    pOutMsg[msgLen++] = LO_UINT16(pSduEvt->CID);
    pOutMsg[msgLen++] = HI_UINT16(pSduEvt->CID);

    pOutMsg[msgLen++] = LO_UINT16(pSduEvt->credits);
    pOutMsg[msgLen++] = HI_UINT16(pSduEvt->credits);

    pOutMsg[msgLen++] = LO_UINT16(pSduEvt->peerCID);
    pOutMsg[msgLen++] = HI_UINT16(pSduEvt->peerCID);

    pOutMsg[msgLen++] = LO_UINT16(pSduEvt->peerCredits);
    pOutMsg[msgLen++] = HI_UINT16(pSduEvt->peerCredits);

    pOutMsg[msgLen++] = LO_UINT16(pSduEvt->totalLen);
    pOutMsg[msgLen++] = HI_UINT16(pSduEvt->totalLen);

    pOutMsg[msgLen++] = LO_UINT16(pSduEvt->txLen);
    pOutMsg[msgLen++] = HI_UINT16(pSduEvt->txLen);
  }
  else
#endif //(BLE_V41_FEATURES & L2CAP_COC_CFG)
  if (pPkt->hdr.status == SUCCESS) // Build all other messages if status is success
  {
    switch (pPkt->opcode)
    {
      case L2CAP_CMD_REJECT:
        msgLen += L2CAP_BuildCmdReject(&pOutMsg[msgLen], (uint8_t *)&(pPkt->cmd.cmdReject));
        break;

      case L2CAP_PARAM_UPDATE_RSP:
        msgLen += L2CAP_BuildParamUpdateRsp(&pOutMsg[msgLen], (uint8_t *)&(pPkt->cmd.updateRsp));
        break;

      case L2CAP_INFO_RSP:
        msgLen += L2CAP_BuildInfoRsp(&pOutMsg[msgLen], (uint8_t *)&(pPkt->cmd.infoRsp));
        break;

#if defined(BLE_V41_FEATURES) && (BLE_V41_FEATURES & L2CAP_COC_CFG)
      case L2CAP_CONNECT_IND:
        pOutMsg[msgLen++] = pPkt->id;
        msgLen += L2CAP_BuildConnectReq(&pOutMsg[msgLen], (uint8_t *)&(pPkt->cmd.connectReq));
        break;

      case L2CAP_CHANNEL_ESTABLISHED_EVT:
        {
          l2capChannelEstEvt_t *pEstEvt = &(pPkt->cmd.channelEstEvt);

          pOutMsg[msgLen++] = LO_UINT16(pEstEvt->result);
          pOutMsg[msgLen++] = HI_UINT16(pEstEvt->result);

          if (pEstEvt->result == L2CAP_CONN_SUCCESS)
          {
            msgLen += buildCoChannelInfo(pEstEvt->CID, &pEstEvt->info, &pOutMsg[msgLen]);
          }
        }
        break;

       case L2CAP_CHANNEL_TERMINATED_EVT:
        {
          l2capChannelTermEvt_t *pTermEvt = &(pPkt->cmd.channelTermEvt);

          pOutMsg[msgLen++] = LO_UINT16(pTermEvt->CID);
          pOutMsg[msgLen++] = HI_UINT16(pTermEvt->CID);

          pOutMsg[msgLen++] = LO_UINT16(pTermEvt->peerCID);
          pOutMsg[msgLen++] = HI_UINT16(pTermEvt->peerCID);

          pOutMsg[msgLen++] = LO_UINT16(pTermEvt->reason);
          pOutMsg[msgLen++] = HI_UINT16(pTermEvt->reason);
        }
        break;

      case L2CAP_OUT_OF_CREDIT_EVT:
      case L2CAP_PEER_CREDIT_THRESHOLD_EVT:
        {
          l2capCreditEvt_t *pCreditEvt = &(pPkt->cmd.creditEvt);

          pOutMsg[msgLen++] = LO_UINT16(pCreditEvt->CID);
          pOutMsg[msgLen++] = HI_UINT16(pCreditEvt->CID);

          pOutMsg[msgLen++] = LO_UINT16(pCreditEvt->peerCID);
          pOutMsg[msgLen++] = HI_UINT16(pCreditEvt->peerCID);

          pOutMsg[msgLen++] = LO_UINT16(pCreditEvt->credits);
          pOutMsg[msgLen++] = HI_UINT16(pCreditEvt->credits);
        }
        break;
#endif //(BLE_V41_FEATURES & L2CAP_COC_CFG)

      default:
        // Unknown command
        break;
    }
  }

  *pMsgLen = msgLen;

  return(pOutMsg);
}

/*********************************************************************
 * @fn      processDataL2CAP
 *
 * @brief   Process an incoming L2CAP Data message.
 *
 * @param   pPkt - packet to process
 * @param   pOutMsg - outgoing message to be built
 * @param   pMsgLen - length of outgoing message
 *
 * @return  outgoing message
 */
static uint8_t *processDataL2CAP(l2capDataEvent_t *pPkt, uint8_t *pOutMsg,
                                uint16_t *pMsgLen, uint8_t *pAllocated)
{
  uint8_t *pBuf;
  uint16_t msgLen = HCI_EXT_HDR_LEN + 2 + 2; // hdr + CID + packet length;
  uint8_t status = pPkt->hdr.status;

  *pAllocated = FALSE;

  msgLen += pPkt->pkt.len;
  if (msgLen > HCI_EXT_APP_OUT_BUF)
  {
    pBuf = ICall_malloc(msgLen);
    if (pBuf)
    {
      *pAllocated = TRUE;
    }
    else
    {
      pBuf = pOutMsg;
      msgLen -= pPkt->pkt.len;

      status = bleMemAllocError;
    }
  }
  else
  {
    pBuf = pOutMsg;
  }

  // Build the message header first
  VOID buildHCIExtHeader(pBuf, (HCI_EXT_L2CAP_EVENT | HCI_EXT_L2CAP_DATA),
                          status, pPkt->connHandle);
  // Add CID
  pBuf[HCI_EXT_HDR_LEN] = LO_UINT16(pPkt->pkt.CID);
  pBuf[HCI_EXT_HDR_LEN+1] = HI_UINT16(pPkt->pkt.CID);

  // Add data length
  pBuf[HCI_EXT_HDR_LEN+2] = LO_UINT16(pPkt->pkt.len);
  pBuf[HCI_EXT_HDR_LEN+3] = HI_UINT16(pPkt->pkt.len);

  // Add payload
  if (pPkt->pkt.pPayload != NULL)
  {
    if (status == SUCCESS)
    {
      VOID memcpy(&pBuf[HCI_EXT_HDR_LEN+4], pPkt->pkt.pPayload, pPkt->pkt.len);
    }

    // Received buffer is processed so it's safe to free it
    BM_free(pPkt->pkt.pPayload);
  }

  *pMsgLen = msgLen;

  return(pBuf);
}

/*********************************************************************
 * @fn      processEventsGATT
 *
 * @brief   Process an incoming GATT Event messages.
 *
 * @param   pPkt - packet to process
 * @param   pOutMsg - outgoing message to be built
 * @param   pMsgLen - length of outgoing message
 * @param   pAllocated - whether outgoing message is locally allocated
 *
 * @return  outgoing message
 */
static uint8_t *processEventsGATT(gattMsgEvent_t *pPkt, uint8_t *pOutMsg,
                                 uint8_t *pMsgLen, uint8_t *pAllocated)
{
  uint8_t msgLen = 0, attHdrLen = 0, hdrLen = HCI_EXT_HDR_LEN + 1; // hdr + event length
  uint8_t *pBuf, *pPayload = NULL;
  uint8_t status = pPkt->hdr.status;

  *pAllocated = FALSE;

  if ((status == SUCCESS) || (status == blePending))
  {
    // Build the ATT header first
    switch (pPkt->method)
    {
      case ATT_ERROR_RSP:
        attHdrLen = ATT_BuildErrorRsp(&pOutMsg[hdrLen], (uint8_t *)&(pPkt->msg.errorRsp));
        break;

      case ATT_EXCHANGE_MTU_REQ:
      case ATT_MTU_UPDATED_EVENT:
        attHdrLen = ATT_BuildExchangeMTUReq(&pOutMsg[hdrLen], (uint8_t *)&(pPkt->msg.exchangeMTUReq));
        break;

      case ATT_EXCHANGE_MTU_RSP:
        attHdrLen = ATT_BuildExchangeMTURsp(&pOutMsg[hdrLen], (uint8_t *)&(pPkt->msg.exchangeMTURsp));
        break;

      case ATT_FIND_INFO_REQ:
        attHdrLen = ATT_BuildFindInfoReq(&pOutMsg[hdrLen], (uint8_t *)&(pPkt->msg.findInfoReq));
        break;

      case ATT_FIND_INFO_RSP:
        {
          attFindInfoRsp_t *pRsp = &pPkt->msg.findInfoRsp;

          attHdrLen = ATT_FIND_INFO_RSP_FIXED_SIZE;

          // Copy response header over
          msgLen = ATT_BuildFindInfoRsp(&pOutMsg[hdrLen], (uint8_t *)pRsp) - attHdrLen;
          pPayload = pRsp->pInfo;
        }
        break;

      case ATT_FIND_BY_TYPE_VALUE_REQ:
        {
          attFindByTypeValueReq_t *pReq = &pPkt->msg.findByTypeValueReq;

          attHdrLen = ATT_FIND_BY_TYPE_VALUE_REQ_FIXED_SIZE;

          // Copy request header over
          msgLen = ATT_BuildFindByTypeValueReq(&pOutMsg[hdrLen], (uint8_t *)pReq) - attHdrLen;
          pPayload = pReq->pValue;
        }
        break;

      case ATT_FIND_BY_TYPE_VALUE_RSP:
        {
          attFindByTypeValueRsp_t *pRsp = &pPkt->msg.findByTypeValueRsp;

          msgLen = ATT_BuildFindByTypeValueRsp(&pOutMsg[hdrLen], (uint8_t *)pRsp);
          pPayload = pRsp->pHandlesInfo;
        }
        break;

      case ATT_READ_BY_TYPE_REQ:
        attHdrLen = ATT_BuildReadByTypeReq(&pOutMsg[hdrLen], (uint8_t *)&(pPkt->msg.readByTypeReq));
        break;

      case ATT_READ_BY_TYPE_RSP:
        {
          attReadByTypeRsp_t *pRsp = &pPkt->msg.readByTypeRsp;

          attHdrLen = ATT_READ_BY_TYPE_RSP_FIXED_SIZE;

          // Copy request header over
          msgLen = ATT_BuildReadByTypeRsp(&pOutMsg[hdrLen], (uint8_t *)pRsp) - attHdrLen;
          pPayload = pRsp->pDataList;
        }
        break;

      case ATT_READ_REQ:
        attHdrLen = ATT_BuildReadReq(&pOutMsg[hdrLen], (uint8_t *)&(pPkt->msg.readReq));
        break;

      case ATT_READ_RSP:
        {
          attReadRsp_t *pRsp = &pPkt->msg.readRsp;

          msgLen = ATT_BuildReadRsp(&pOutMsg[hdrLen], (uint8_t *)pRsp);
          pPayload = pRsp->pValue;
        }
        break;

      case ATT_READ_BLOB_REQ:
        attHdrLen = ATT_BuildReadBlobReq(&pOutMsg[hdrLen], (uint8_t *)&(pPkt->msg.readBlobReq));
        break;

      case ATT_READ_BLOB_RSP:
        {
          attReadBlobRsp_t *pRsp = &pPkt->msg.readBlobRsp;

          msgLen = ATT_BuildReadBlobRsp(&pOutMsg[hdrLen], (uint8_t *)pRsp);
          pPayload = pRsp->pValue;
        }
        break;

      case ATT_READ_MULTI_REQ:
        {
          attReadMultiReq_t *pReq = &pPkt->msg.readMultiReq;

          msgLen = ATT_BuildReadMultiReq(&pOutMsg[hdrLen], (uint8_t *)pReq);
          pPayload = pReq->pHandles;
        }
        break;

      case ATT_READ_MULTI_RSP:
        {
          attReadMultiRsp_t *pRsp = &pPkt->msg.readMultiRsp;

          msgLen = ATT_BuildReadMultiRsp(&pOutMsg[hdrLen], (uint8_t *)pRsp);
          pPayload = pRsp->pValues;
        }
        break;

      case ATT_READ_BY_GRP_TYPE_REQ:
        attHdrLen = ATT_BuildReadByTypeReq(&pOutMsg[hdrLen], (uint8_t *)&(pPkt->msg.readByGrpTypeReq));
        break;

      case ATT_READ_BY_GRP_TYPE_RSP:
        {
          attReadByGrpTypeRsp_t *pRsp = &pPkt->msg.readByGrpTypeRsp;

          attHdrLen = ATT_READ_BY_GRP_TYPE_RSP_FIXED_SIZE;

          // Copy request header over
          msgLen = ATT_BuildReadByGrpTypeRsp(&pOutMsg[hdrLen], (uint8_t *)pRsp) - attHdrLen;
          pPayload = pRsp->pDataList;
        }
        break;

      case ATT_WRITE_REQ:
        {
          attWriteReq_t *pReq = &pPkt->msg.writeReq;

          pOutMsg[hdrLen] = pReq->sig;
          pOutMsg[hdrLen+1] = pReq->cmd;

          attHdrLen = ATT_WRITE_REQ_FIXED_SIZE;

          // Copy request header over
          msgLen = ATT_BuildWriteReq(&pOutMsg[hdrLen+2], (uint8_t *)pReq) - attHdrLen;
          pPayload = pReq->pValue;

          attHdrLen += 2; // sig + cmd
        }
        break;

      case ATT_PREPARE_WRITE_REQ:
        {
          attPrepareWriteReq_t *pReq = &pPkt->msg.prepareWriteReq;

          attHdrLen = ATT_PREPARE_WRITE_REQ_FIXED_SIZE;

          // Copy request header over
          msgLen = ATT_BuildPrepareWriteReq(&pOutMsg[hdrLen], (uint8_t *)pReq) - attHdrLen;
          pPayload = pReq->pValue;
        }
        break;

      case ATT_PREPARE_WRITE_RSP:
        {
          attPrepareWriteRsp_t *pRsp = &pPkt->msg.prepareWriteRsp;

          attHdrLen = ATT_PREPARE_WRITE_RSP_FIXED_SIZE;

          // Copy request header over
          msgLen = ATT_BuildPrepareWriteRsp(&pOutMsg[hdrLen], (uint8_t *)pRsp) - attHdrLen;
          pPayload = pRsp->pValue;
        }
        break;

      case ATT_EXECUTE_WRITE_REQ:
        attHdrLen = ATT_BuildExecuteWriteReq(&pOutMsg[hdrLen], (uint8_t *)&(pPkt->msg.executeWriteReq));
        break;

      case ATT_HANDLE_VALUE_IND:
      case ATT_HANDLE_VALUE_NOTI:
        {
          attHandleValueInd_t *pInd = &pPkt->msg.handleValueInd;

          attHdrLen = ATT_HANDLE_VALUE_IND_FIXED_SIZE;

          // Copy request header over
          msgLen = ATT_BuildHandleValueInd(&pOutMsg[hdrLen], (uint8_t *)pInd) - attHdrLen;
          pPayload = pInd->pValue;
        }
        break;

      case ATT_FLOW_CTRL_VIOLATED_EVENT:
        {
          attFlowCtrlViolatedEvt_t *pEvt = &pPkt->msg.flowCtrlEvt;

          pOutMsg[hdrLen]   = pEvt->opcode;
          pOutMsg[hdrLen+1] = pEvt->pendingOpcode;

          attHdrLen = 2;
        }
        break;

      default:
        // Unknown command
        break;
    }
  }

  // Event format: HCI Ext hdr + event len + ATT hdr + ATT PDU
  if ((hdrLen + attHdrLen + msgLen) > HCI_EXT_APP_OUT_BUF)
  {
    pBuf = ICall_malloc(hdrLen + attHdrLen + msgLen);
    if (pBuf)
    {
      *pAllocated = TRUE;

      // Copy the ATT header over
      if (attHdrLen > 0)
      {
        VOID memcpy(&pBuf[hdrLen], &pOutMsg[hdrLen], attHdrLen);
      }
    }
    else
    {
      pBuf = pOutMsg;
      msgLen = 0;

      status = bleMemAllocError;
    }
  }
  else
  {
    pBuf = pOutMsg;
  }

  // Build the message PDU
  if (pPayload != NULL)
  {
    if (msgLen > 0)
    {
      // Copy the message payload over
      VOID memcpy(&pBuf[hdrLen+attHdrLen], pPayload, msgLen);
    }

    // Free the payload buffer
    BM_free(pPayload);
  }

  // Build the message header
  VOID buildHCIExtHeader(pBuf, (HCI_EXT_ATT_EVENT | pPkt->method), status, pPkt->connHandle);

  // Add the event (PDU) length for GATT events for now!
  pBuf[HCI_EXT_HDR_LEN] = attHdrLen + msgLen;

  *pMsgLen = hdrLen + attHdrLen + msgLen;

  return(pBuf);
}

#ifdef BLE3_CMD
/*********************************************************************
 * @fn      BLE3ToAgama_setParam
 *
 * @brief   Translating BLE3 param ID to a set of Agama commands
 *
 * @param   id - Parameter ID
 * @param   value - Parameter value
 *
 * @return  SUCCESS, bleInvalidRange, INVALIDPARAMETER.
 */
status_t BLE3ToAgama_setParam( uint16_t id, uint16_t value )
{
  uint8_t stat = SUCCESS;
  switch(id)
  {
#if ( HOST_CONFIG & ( CENTRAL_CFG | OBSERVER_CFG ) )
    // Scan parameters
    case TGAP_CONN_SCAN_INT:
    case TGAP_CONN_HIGH_SCAN_INT:
    case TGAP_CONN_EST_SCAN_INT:
    {
      // Update the scan interval for the establish connection scanning
      stat = GapInit_setPhyParam(SCAN_PRIM_PHY_1M, INIT_PHYPARAM_SCAN_INTERVAL, value);
      break;
    }

    case TGAP_GEN_DISC_SCAN_INT:
    case TGAP_LIM_DISC_SCAN_INT:
    {
      uint16_t scanWin;
      uint16_t scanInt;
      GapScan_ScanType_t scanType;
      stat = GapScan_getPhyParams(SCAN_PRIM_PHY_1M, &scanType, &scanInt, &scanWin);
      if( stat == SUCCESS )
      {
        // Scan interval update
        stat = GapScan_setPhyParams(SCAN_PRIM_PHY_1M, scanType, value, scanWin);
      }
      break;
    }

    case TGAP_CONN_SCAN_WIND:
    case TGAP_CONN_EST_SCAN_WIND:
    case TGAP_CONN_HIGH_SCAN_WIND:
    {
      // Update the scan window for the establish connection scanning
      stat = GapInit_setPhyParam(SCAN_PRIM_PHY_1M, INIT_PHYPARAM_SCAN_WINDOW, value);
      break;
    }

    case TGAP_GEN_DISC_SCAN_WIND:
    case TGAP_LIM_DISC_SCAN_WIND:
    {
      uint16_t scanWin;
      uint16_t scanInt;
      GapScan_ScanType_t scanType;
      stat = GapScan_getPhyParams(SCAN_PRIM_PHY_1M, &scanType, &scanInt, &scanWin);
      if( stat == SUCCESS )
      {
        // Scan window update
        stat = GapScan_setPhyParams(SCAN_PRIM_PHY_1M, scanType, scanInt, value);
      }
      break;
    }

    // Scan duration update
    case TGAP_GEN_DISC_SCAN:
    case TGAP_LIM_DISC_SCAN:
    {
      scanDuration = value/10;
      break;
    }

    // Scan channels update
    case TGAP_SET_SCAN_CHAN:
    {
      stat = GAP_SetParamValue(GAP_PARAM_SET_SCAN_CHAN, value);
      break;
    }

    case TGAP_FILTER_ADV_REPORTS:
    {
      uint8_t fltValue = (uint8_t)value;
      stat = GapScan_setParam(SCAN_PARAM_FLT_DUP, &fltValue);
      break;
    }

    case TGAP_SCAN_RSP_RSSI_MIN:
    {
      int8_t rssiValue = (int8_t)value;
      stat = GapScan_setParam(SCAN_PARAM_FLT_MIN_RSSI, &rssiValue);
      break;
    }

    case TGAP_CONN_EST_INT_MIN:
    {
      stat = GapInit_setPhyParam(INIT_PHY_1M, INIT_PHYPARAM_CONN_INT_MIN, value);
      break;
    }

    case TGAP_CONN_EST_INT_MAX:
    {
      stat = GapInit_setPhyParam(INIT_PHY_1M, INIT_PHYPARAM_CONN_INT_MAX, value);
      break;
    }

        case TGAP_CONN_EST_SUPERV_TIMEOUT:
    {
      stat = GapInit_setPhyParam(INIT_PHY_1M, INIT_PHYPARAM_SUP_TIMEOUT, value);
      break;
    }

    case TGAP_CONN_EST_LATENCY:
    {
      stat = GapInit_setPhyParam(INIT_PHY_1M, INIT_PHYPARAM_CONN_LATENCY, value);
      break;
    }

    case TGAP_CONN_EST_MIN_CE_LEN:
    {
      stat = GapInit_setPhyParam(INIT_PHY_1M, INIT_PHYPARAM_MIN_CE_LEN, value);
      break;
    }

    case TGAP_CONN_EST_MAX_CE_LEN:
    {
      stat = GapInit_setPhyParam(INIT_PHY_1M, INIT_PHYPARAM_MAX_CE_LEN, value);
      break;
    }
#endif

#if ( HOST_CONFIG & ( PERIPHERAL_CFG | BROADCASTER_CFG ) )
    // Advertisement parameters
    // Update the minimum advertising interval
    case TGAP_GEN_DISC_ADV_INT_MIN:
    case TGAP_CONN_ADV_INT_MIN:
    case TGAP_FAST_INTERVAL_2_INT_MIN:
    case TGAP_LIM_DISC_ADV_INT_MIN:
    {
      if(advHandleLegacy == 0xFF)
      {
        GapAdv_params_t advParamLegacy = GAPADV_PARAMS_LEGACY_SCANN_CONN;
        stat = GapAdv_create(host_tl_advEvtCallback, &advParamLegacy, &advHandleLegacy);
      }

      if( stat != SUCCESS )
      {
        // Failed to create a valid advertising set
        return stat;
      }

      // There is a valid advertising set
      uint32_t advIntMin = (uint32_t)value;
      stat = GapAdv_setParam(advHandleLegacy, GAP_ADV_PARAM_PRIMARY_INTERVAL_MIN, &advIntMin);
      break;
    }

    // Update the maximum advertising interval
    case TGAP_LIM_DISC_ADV_INT_MAX:
    case TGAP_GEN_DISC_ADV_INT_MAX:
    case TGAP_CONN_ADV_INT_MAX:
    case TGAP_FAST_INTERVAL_2_INT_MAX:
    {
      if(advHandleLegacy == 0xFF)
      {
        GapAdv_params_t advParamLegacy = GAPADV_PARAMS_LEGACY_SCANN_CONN;
        stat = GapAdv_create(host_tl_advEvtCallback, &advParamLegacy, &advHandleLegacy);
      }

      if( stat != SUCCESS )
      {
        // Failed to create a valid advertising set
        return stat;
      }

      // There is a valid advertising set
      uint32_t advIntMax = (uint32_t)value;
      stat = GapAdv_setParam(advHandleLegacy, GAP_ADV_PARAM_PRIMARY_INTERVAL_MAX, &advIntMax);
      break;
    }

    case TGAP_GEN_DISC_ADV_MIN:
    case TGAP_LIM_ADV_TIMEOUT:
    {
      advDuration = value;
      break;
    }
#endif

    case TGAP_CONN_PARAM_TIMEOUT:
    {
      stat = GAP_SetParamValue(GAP_PARAM_CONN_PARAM_TIMEOUT, value);
      break;
    }

    case TGAP_REJECT_CONN_PARAMS:
    {
      stat = GAP_SetParamValue(GAP_PARAM_LINK_UPDATE_DECISION, value);
      break;
    }

    case TGAP_PRIVATE_ADDR_INT:
    {
      stat = GAP_SetParamValue(GAP_PARAM_PRIVATE_ADDR_INT, value);
      break;
    }

    case TGAP_SM_TIMEOUT:
    {
      stat = GAP_SetParamValue(GAP_PARAM_SM_TIMEOUT, value);
      break;
    }

    case TGAP_SM_MIN_KEY_LEN:
    {
      stat = GAP_SetParamValue(GAP_PARAM_SM_MIN_KEY_LEN, value);
      break;
    }

    case TGAP_SM_MAX_KEY_LEN:
    {
      stat = GAP_SetParamValue(GAP_PARAM_SM_MAX_KEY_LEN, value);
      break;
    }

    default:
      stat = SUCCESS;
      break;
  }

  return stat;
}

/*********************************************************************
 * @fn      getBLE3ToAgamaEventProp
 *
 * @brief   Translating BLE3 advertising properties to Agama advertising
 *          properties
 *
 * @param   eventType - BLE3 advertising properties
 *
 * @return  0xFFFF if the event properties wasn't found, else Agama
 *          event properties
 */
uint16_t getBLE3ToAgamaEventProp( uint8_t eventType)
{
  uint16_t eventProp = 0xFFFF;

  if( eventType == GAP_ADTYPE_ADV_IND )
  {
    eventProp = GAP_ADV_PROP_LEGACY | GAP_ADV_PROP_CONNECTABLE | GAP_ADV_PROP_SCANNABLE;
  }

  if( eventType == GAP_ADTYPE_ADV_HDC_DIRECT_IND )
  {
    eventProp = GAP_ADV_PROP_LEGACY | GAP_ADV_PROP_CONNECTABLE | GAP_ADV_PROP_DIRECTED | GAP_ADV_PROP_HDC;
  }

  if( eventType == GAP_ADTYPE_ADV_SCAN_IND )
  {
    eventProp = GAP_ADV_PROP_LEGACY | GAP_ADV_PROP_SCANNABLE;
  }

  if( eventType == GAP_ADTYPE_ADV_NONCONN_IND )
  {
    eventProp = GAP_ADV_PROP_LEGACY;
  }

  if( eventType == GAP_ADTYPE_ADV_LDC_DIRECT_IND )
  {
    eventProp = GAP_ADV_PROP_LEGACY | GAP_ADV_PROP_CONNECTABLE | GAP_ADV_PROP_DIRECTED;
  }

  return eventProp;
}

/*********************************************************************
 * @fn      getAgamaToBLE3EventProp
 *
 * @brief   Translating Agama advertising properties to BLE3 advertising
 *          properties
 *
 * @param   eventType - Agama advertising properties
 *
 * @return  0xFFFF if the event properties wasn't found, else BLE3
 *          event properties
 */
uint8_t getAgamaToBLE3EventProp( uint8_t eventType )
{
  uint8_t eventProp = 0xFF;

  if( eventType == GAP_AGAMA_BLE3_TYPE_ADV_IND )
  {
    eventProp = GAP_ADTYPE_ADV_IND;
  }

  if( eventType == GAP_AGAMA_BLE3_TYPE_ADV_HDC_DIRECT_IND )
  {
   eventProp = GAP_ADTYPE_ADV_HDC_DIRECT_IND;
  }

  if( eventType == GAP_AGAMA_BLE3_TYPE_ADV_SCAN_IND )
  {
   eventProp = GAP_ADTYPE_ADV_SCAN_IND;
  }

  if( eventType == GAP_AGAMA_BLE3_TYPE_ADV_NONCONN_IND )
  {
   eventProp = GAP_ADTYPE_ADV_NONCONN_IND;
  }

  if( eventType == GAP_AGAMA_BLE3_TYPE_ADV_LDC_DIRECT_IND )
  {
   eventProp = GAP_ADTYPE_ADV_LDC_DIRECT_IND;
  }

  return eventProp;
}

#endif // BLE3_CMD

#if !defined(GATT_DB_OFF_CHIP)
/*********************************************************************
 * @fn      processEventsGATTServ
 *
 * @brief   Process an incoming GATT Server Event messages.
 *
 * @param   pPkt - packet to process
 * @param   pMsg - outgoing message to be built
 * @param   pMsgLen - length of outgoing message
 *
 * @return  outgoing message
 */
static uint8_t *processEventsGATTServ(gattEventHdr_t *pPkt, uint8_t *pMsg, uint8_t *pMsgLen)
{
  uint8_t hdrLen = HCI_EXT_HDR_LEN + 1; // hdr + event length
  uint8_t msgLen = 0;
  uint8_t *pBuf = pMsg;

  if (pPkt->hdr.status == SUCCESS)
  {
    // Build the message first
    switch (pPkt->method)
    {
      case GATT_CLIENT_CHAR_CFG_UPDATED_EVENT:
        {
          gattClientCharCfgUpdatedEvent_t *pEvent = (gattClientCharCfgUpdatedEvent_t *)pPkt;

          // Attribute handle
          pMsg[hdrLen]   = LO_UINT16(pEvent->attrHandle);
          pMsg[hdrLen+1] = HI_UINT16(pEvent->attrHandle);

          // Attribute value
          pMsg[hdrLen+2] = LO_UINT16(pEvent->value);
          pMsg[hdrLen+3] = HI_UINT16(pEvent->value);

          msgLen = 4;
        }
        break;

      default:
        // Unknown command
        break;
    }
  }

  // Build the message header
  VOID buildHCIExtHeader(pBuf, (HCI_EXT_GATT_EVENT | pPkt->method), pPkt->hdr.status, pPkt->connHandle);

  // Add the event (PDU) length for GATT events for now!
  pBuf[HCI_EXT_HDR_LEN] = msgLen;

  *pMsgLen = hdrLen + msgLen;

  return(pBuf);
}
#endif // !GATT_DB_OFF_CHIP

/*********************************************************************
 * @fn      buildHCIExtHeader
 *
 * @brief   Build an HCI Extension header.
 *
 * @param   pBuf - header to be built
 * @param   event - event id
 * @param   status - event status
 * @param   connHandle - connection handle
 *
 * @return  header length
 */
static uint8_t buildHCIExtHeader(uint8_t *pBuf, uint16_t event, uint8_t status, uint16_t connHandle)
{
  pBuf[0] = LO_UINT16(event);
  pBuf[1] = HI_UINT16(event);
  pBuf[2] = status;
  pBuf[3] = LO_UINT16(connHandle);
  pBuf[4] = HI_UINT16(connHandle);

  return(HCI_EXT_HDR_LEN);
}
#endif /* (CENTRAL_CFG | PERIPHERAL_CFG)*/

#endif /* (PTM_MODE) */

#if defined(BLE_V41_FEATURES) && (BLE_V41_FEATURES & L2CAP_COC_CFG)
/*********************************************************************
  * @fn      l2capVerifySecCB
 *
 * @brief   Callback function to verify security when a Connection
 *          Request is received.
 *
 * @param   connHandle - connection handle request was received on
 * @param   id - identifier matches responses with requests
 * @param   pReq - received connection request
 *
 * @return  See L2CAP Connection Response: Result values
 */
static uint16_t l2capVerifySecCB(uint16_t connHandle, uint8_t id, l2capConnectReq_t *pReq)
{
  uint8_t msgLen = 0;

  // Build the message header first
  msgLen = buildHCIExtHeader(out_msg, (HCI_EXT_L2CAP_EVENT | L2CAP_CONNECT_IND),
                              SUCCESS, connHandle);

  out_msg[msgLen++] = id;
  msgLen += L2CAP_BuildConnectReq(&out_msg[msgLen], (uint8_t *)pReq);

  // Send out the Connection Request
  HCI_SendControllerToHostEvent(HCI_VE_EVENT_CODE,  msgLen, out_msg);

  return(L2CAP_CONN_PENDING_SEC_VERIFY);
}

/*********************************************************************
 * @fn      buildCoChannelInfo
 *
 * @brief   Build Connection Oriented Channel info.
 *
 * @param   CID - local CID
 * @param   pInfo - pointer to CoC info
 * @param   pRspBuf - buffer to copy CoC info into
 *
 * @return  length of data copied
 */
static uint8_t buildCoChannelInfo(uint16_t CID, l2capCoCInfo_t *pInfo, uint8_t *pRspBuf)
{
  uint8_t msgLen = 0;

  pRspBuf[msgLen++] = LO_UINT16(pInfo->psm);
  pRspBuf[msgLen++] = HI_UINT16(pInfo->psm);

  pRspBuf[msgLen++] = LO_UINT16(CID);
  pRspBuf[msgLen++] = HI_UINT16(CID);

  pRspBuf[msgLen++] = LO_UINT16(pInfo->mtu);
  pRspBuf[msgLen++] = HI_UINT16(pInfo->mtu);

  pRspBuf[msgLen++] = LO_UINT16(pInfo->mps);
  pRspBuf[msgLen++] = HI_UINT16(pInfo->mps);

  pRspBuf[msgLen++] = LO_UINT16(pInfo->credits);
  pRspBuf[msgLen++] = HI_UINT16(pInfo->credits);

  pRspBuf[msgLen++] = LO_UINT16(pInfo->peerCID);
  pRspBuf[msgLen++] = HI_UINT16(pInfo->peerCID);

  pRspBuf[msgLen++] = LO_UINT16(pInfo->peerMtu);
  pRspBuf[msgLen++] = HI_UINT16(pInfo->peerMtu);

  pRspBuf[msgLen++] = LO_UINT16(pInfo->peerMps);
  pRspBuf[msgLen++] = HI_UINT16(pInfo->peerMps);

  pRspBuf[msgLen++] = LO_UINT16(pInfo->peerCredits);
  pRspBuf[msgLen++] = HI_UINT16(pInfo->peerCredits);

  pRspBuf[msgLen++] = LO_UINT16(pInfo->peerCreditThreshold);
  pRspBuf[msgLen++] = HI_UINT16(pInfo->peerCreditThreshold);

  return(msgLen);
}

#endif //(BLE_V41_FEATURES & L2CAP_COC_CFG)

#if defined(GATT_DB_OFF_CHIP)
/*********************************************************************
 * @fn      addAttrRec
 *
 * @brief   Add attribute record to its service.
 *
 * @param   pServ - GATT service
 * @param   pUUID - attribute UUID
 * @param   len - length of UUID
 * @param   permissions - attribute permissions
 * @param   pTotalAttrs - total number of attributes
 * @param   pRspDataLen - response data length to be returned
 *
 * @return  status
 */
static uint8_t addAttrRec(gattService_t *pServ, uint8_t *pUUID, uint8_t len,
                         uint8_t permissions, uint16_t *pTotalAttrs, uint8_t *pRspDataLen)
{
  gattAttribute_t *pAttr = &(pServ->attrs[pServ->numAttrs]);
  uint8_t stat = SUCCESS;

  // Set up attribute record
  pAttr->type.uuid = findUUIDRec(pUUID, len);
  if (pAttr->type.uuid != NULL)
  {
    pAttr->type.len = len;
    pAttr->permissions = permissions;

    // Are all attributes added to the service yet?
    if (++pServ->numAttrs == *pTotalAttrs)
    {
      // Register the service with the GATT Server
      stat = GATT_RegisterService(pServ);
      if (stat == SUCCESS)
      {
        *pRspDataLen = 4;

        // Service startHandle
        uint16_t handle = pServ->attrs[0].handle;
        rspBuf[RSP_PAYLOAD_IDX] = LO_UINT16(handle);
        rspBuf[RSP_PAYLOAD_IDX+1] = HI_UINT16(handle);

        // Service endHandle
        handle = pServ->attrs[pServ->numAttrs-1].handle;
        rspBuf[RSP_PAYLOAD_IDX+2] = LO_UINT16(handle);
        rspBuf[RSP_PAYLOAD_IDX+3] = HI_UINT16(handle);

        // Service is registered with GATT; clear its info
        pServ->attrs = NULL;
        pServ->numAttrs = 0;
      }
      else
      {
        freeAttrRecs(pServ);
      }

      // We're done with this service
      *pTotalAttrs = 0;
    }
  }
  else
  {
    stat = INVALIDPARAMETER;
  }

  return(stat);
}

/*********************************************************************
 * @fn      freeAttrRecs
 *
 * @brief   Free attribute records. Also, free UUIDs that were
 *          allocated dynamically.
 *
 * @param   pServ - GATT service
 *
 * @return  none
 */
static void freeAttrRecs(gattService_t *pServ)
{
  if (pServ->attrs != NULL)
  {
    for (uint8_t i = 0; i < pServ->numAttrs; i++)
    {
      gattAttrType_t *pType = &pServ->attrs[i].type;
      if (pType->uuid != NULL)
      {
        if (GATT_FindUUIDRec((uint8_t *)pType->uuid, pType->len) == NULL)
        {
          // UUID was dynamically allocated; free it
          ICall_free((uint8_t *)pType->uuid);
        }
      }
    }

    ICall_free(pServ->attrs);

    pServ->attrs = NULL;
    pServ->numAttrs = 0;
  }
}

/*********************************************************************
 * @fn      findUUIDRec
 *
 * @brief   Find UUID record. If the record is not found, create one
 *          dynamically.
 *
 * @param   pUUID - UUID to look for
 * @param   len - length of UUID
 *
 * @return  UUID record
 */
static const uint8_t *findUUIDRec(uint8_t *pUUID, uint8_t len)
{
  const uint8_t *pUuid = (uint8_t*) GATT_FindUUIDRec(pUUID, len);
  if (pUuid == NULL)
  {
    // UUID not found; allocate space for it
    pUuid = ICall_malloc(len);
    if (pUuid != NULL)
    {
      VOID memcpy((uint8_t *)pUuid, pUUID, len);
    }
    else
    {
#ifdef EXT_HAL_ASSERT
      HAL_ASSERT(HAL_ASSERT_CAUSE_OUT_OF_MEMORY);
#else /* !EXT_HAL_ASSERT */
      HAL_ASSERT(FALSE);
#endif /* EXT_HAL_ASSERT */
    }
  }

  return(pUuid);
}
#endif // GATT_DB_OFF_CHIP
#endif // HOST_CONFIG
#endif // #endif /* (defined(HCI_TL_FULL) || defined(PTM_MODE)) */

/*********************************************************************
*********************************************************************/
