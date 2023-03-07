/***************************************************************************//**
 * @file
 * @brief Routines for timers used by the GPD.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef __SILABS_GPD_COMPONENTS_COMMON_H__
#define __SILABS_GPD_COMPONENTS_COMMON_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef UC_BUILD
#include PLATFORM_HEADER
#include CONFIGURATION_HEADER
#else
#include "gpd-apps-config.h"
#endif
#include "rail.h"
#include "rail_types.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_emu.h"
#ifndef UC_BUILD
#include "bsp.h"
#include "gpiointerrupt.h"

//power management
#include  "sleep.h"
#endif
//rail ieee 802.15.4
#include  "rail_ieee802154.h"

//mbedtls
#include "mbedtls/ccm.h"

// Common defines
#define FAILED  (-1)
#define SUCCESS (0)

#define HI_UINT16(a) ((uint8_t)(((uint16_t)(a) >> 8) & 0xFF))
#define LO_UINT16(a) ((uint8_t)((uint16_t)(a) & 0xFF))

#define GP_FIFO_SIZE 128

#define EMBER_GPD_TX_MPDU_SIZE 128
#define EMBER_GPD_RX_MPDU_SIZE 128

#define EMBER_GPD_EUI64_SIZE 8

#define EMBER_GPD_SECURITY_KEY_TYPE_INDICATION_MASK 0x04
#define EMBER_GPD_SECURITY_KEY_TYPE_INDICATION_MASK_OFFSET 2
// GPD max payload sizes
#define EMBER_GPD_SRC_ID_MAX_PAYLOAD_SIZE   59
#define EMBER_GPD_IEEE_ID_MAX_PAYLOAD_SIZE  54

// GPD Application Type
#define EMBER_GPD_APP_ID_SRC_ID    0
#define EMBER_GPD_APP_ID_IEEE_ID   2

//GPD MAC Sequence Capability Type
#define EMBER_GPD_MAC_SEQ_RANDOM     0
#define EMBER_GPD_MAC_SEQ_INCREMENTAL 1

// Offsets in incoming frame
#define EMBER_GPD_MAC_LONG_ADDRESS_OFFSET 6
#define EMBER_GPD_NWK_FC_OFFSET_FOR_LONG_ADDRESS 14
#define EMBER_GPD_NWK_FC_OFFSET_FOR_SHORT_ADDRESS 8

// GPD Security Level Type
#define EMBER_GPD_SECURITY_LEVEL_NONE           0x00 // No security
#define EMBER_GPD_SECURITY_LEVEL_RESERVED       0x01 // Reserved
#define EMBER_GPD_SECURITY_LEVEL_LONG_AUTH_ONLY 0x02 // long 4 bytes Frame counter, 4 bytes MIC, only authentication no encryption
#define EMBER_GPD_SECURITY_LEVEL_LONG_FULL      0x03 // long 4 bytes Frame counter, 4 bytes MIC, authentication and encryption
#define TAG_SIZE                                4
//GPD Security Key Type
#define EMBER_GPD_SECURITY_KEY_TYPE_NOKEY         0x00   // No key
#define EMBER_GPD_SECURITY_KEY_TYPE_NWK           0x01   // NWK Key
#define EMBER_GPD_SECURITY_KEY_TYPE_GROUP         0x02   // ZGP group key
#define EMBER_GPD_SECURITY_KEY_TYPE_GROUP_DERIVED 0x03   // ZGPD Derived group key
#define EMBER_GPD_SECURITY_KEY_TYPE_OOB           0x04   // (individual) OOB ZGPD Key
#define EMBER_GPD_SECURITY_KEY_TYPE_DERIVED       0x07   // derived individual ZGPD key

//GPD Security Key Indication Flag
#define EMBER_GPD_SECURITY_KEY_INDICATION_SHARED     0x00
#define EMBER_GPD_SECURITY_KEY_INDICATION_INDIVIDUAL 0x01

// NV Storage macros
#define EMBER_GPD_NV_DATA_GPD_FRAME_COUNTER_INDEX  0
#define EMBER_GPD_NV_DATA_GPD_KEY_INDEX            4
#define EMBER_GPD_NV_DATA_GPD_SEC_LEVEL_INDEX      20
#define EMBER_GPD_NV_DATA_GPD_SEC_KEY_TYPE_INDEX   21
#define EMBER_GPD_NV_DATA_GPD_CHANNEL_INDEX        22
#define EMBER_GPD_NV_DATA_GPD_STATE_INDEX          23

#define EMBER_GPD_NV_DATA_SIZE                     32

////////////////////////////
// GPD Commands
#define GP_CMD_COMMISSIONING            0xE0
#define GP_CMD_DECOMMISSIONING          0xE1
#define GP_CMD_COMMISSIONING_SUCCESS    0xE2
#define GP_CMD_CHANNEL_RQST             0xE3
#define GP_CMD_COMMISSIONING_APP_DESCR  0xE4

#define GP_CMD_COMMISSIONING_REPLY      0xF0
#define GP_CMD_CHANNEL_CONFIG           0xF3

#define GP_CMD_TOGGLE                   0x22

#define GP_CMD_ATTRIBUTE_REPORTING      0xA0

// MAC Frame control field
// MAC FC bits
//-----------------------------------------------------------------------------
//0-2  | 3       | 4      | 5 | 6          | 7-9| 10-11      | 12-13  | 14-15
//-----+---------+--------+---+------------+----+------------+--------+--------
//Frame| Security| Frame  | AR| PAN ID     | Res| Destination| Frame  | Source
//Type | Enabled | Pending|   | Compression|    | Addressing | Version| Addressing
//     |         |        |   |            |    | Mode       |        | Mode
//-----------------------------------------------------------------------------
#define EMBER_GPD_MAC_FC_WORD                     0x0801
#define EMBER_GPD_MAC_FC_IEEE_WORD                0xC841
#define EMBER_GPD_MAC_FC_IEEE_WORD_INCOMING       0x0C01
#define EMBER_GPD_MAC_FOOTER_SIZE                 2
#define EMBER_GPD_MAC_HEADER_SIZE                 8
#define EMBER_GPD_MAC_HEADER_SIZE_IEEE_RX         14
#define EMBER_GPD_MAC_HEADER_SIZE_IEEE_TX         16

// Authentication Macro
#define EMBER_GPD_AUTH_FIXED_LEN_SRC_ID    10
#define EMBER_GPD_AUTH_START_SRC_ID        8
#define EMBER_GPD_AUTH_START_IEEE          16
#define EMBER_GPD_AUTH_FIXED_LEN_IEEE      7

// Network Frame Control Fields
// Frame Type sub-field
#define EMBER_GPD_NWK_FC_FRAME_TYPE_DATA     0x00
#define EMBER_GPD_NWK_FC_FRAME_TYPE_MAINT    0x01
#define EMBER_GPD_NWK_FC_FRAME_TYPE_MASK     0x03

// Zigbee Protocol Version sub-field
#define EMBER_GPD_NWK_FC_ZIGBEE_VER          0x03
#define EMBER_GPD_NWK_FC_ZIGBEE_VER_MASK     0x1C
#define EMBER_GPD_NWK_FC_ZIGBEE_VER_OFFSET   2

// Autocommissioning sub-field
#define EMBER_GPD_NWK_FC_AUTOCOM_MASK   0x40
// Extended Network Frame Control sub-field
#define EMBER_GPD_NWK_FC_EXT_FC_MASK       0x80

//Extended Network Frame Control Fields
// Application Id sub-field
#define EMBER_GPD_EXT_NWK_FC_APP_ID_MASK         0x07
// Security Level sub-field
#define EMBER_GPD_EXT_NWK_FC_SECURITY_LEVEL_MASK 0x18
#define EMBER_GPD_EXT_NWK_FC_SECURITY_LEVEL_OFFSET 3
// Security Key Type sub-field
#define EMBER_GPD_EXT_NWK_FC_SEC_KEY_TYPE_MASK   0x20
// Rx After Tx sub-field
#define EMBER_GPD_EXT_NWK_FC_RX_AFTER_TX_MASK    0x40

// Direction sub-field
#define EMBER_GPD_EXT_NWK_FC_DIR_ZGPD_TO_ZGPP 0
#define EMBER_GPD_EXT_NWK_FC_DIR_ZGPP_TO_ZGPD 1
#define EMBER_GPD_EXT_NWK_FC_DIRECTION_MASK   0x80

/*******************************************************************************
* Commissioning Option Fields
*******************************************************************************/
// MAC Sequence Number Capability sub-field
#define EMBER_GPD_COM_OPT_MAC_SEQ_NBR_CAP_MASK    0x01
// Rx Capability sub-field
#define EMBER_GPD_COM_OPT_RX_CAP_MASK    0x02
// Application Information sub-field
#define EMBER_GPD_COM_OPT_APP_INFO_MASK   0x04
// PanID Request sub-field
#define EMBER_GPD_COM_OPT_PANID_REQ_MASK    0x10
// Security Key Request sub-field
#define EMBER_GPD_COM_OPT_KEY_REQ_MASK    0x20
// Fixed Location sub-field
#define EMBER_GPD_COM_OPT_FIXED_LOC_MASK    0x40
// Extended Option sub-field
#define EMBER_GPD_COM_OPT_EXT_OPT_MASK    0x80

/*******************************************************************************
* Extended Commissioning Option Fields
*******************************************************************************/
// Security Level Capability sub-field
#define EMBER_GPD_EXT_COM_OPT_SECURITY_LEVEL_CAP_MASK    0x03
// Key type sub-field
#define EMBER_GPD_EXT_COM_OPT_KEY_TYPE_MASK    0x1C
#define EMBER_GPD_EXT_COM_OPT_KEY_TYPE_OFFSET  2
// Key Present sub-field
#define EMBER_GPD_EXT_COM_OPT_KEY_PRESENT_MASK    0x20
// Key Encrypted sub-field
#define EMBER_GPD_EXT_COM_OPT_KEY_ENCRYPT_MASK    0x40
// Outgoing Counter Present sub-field
#define EMBER_GPD_EXT_COM_OPT_OUT_COUNTER_PRESENT_MASK    0x80

// Application Information Commissioning Fields
//
// MS ID sub field
#define EMBER_GPD_APP_INFO_COM_MS_ID_MASK         0x01
// Model ID sub field
#define EMBER_GPD_APP_INFO_COM_MODEL_ID_MASK      0x02
// CMD list sub field
#define EMBER_GPD_APP_INFO_COM_CMDLIST_MASK       0x04
// CLuster list sub field
#define EMBER_GPD_APP_INFO_COM_CLUSTERLIST_MASK   0x08
// Switch information mask
#define EMBER_GPD_APP_INFO_COM_SWITCH_INFORMATION_PRESENT_MASK 0x10
// Application Description Follows mask
#define EMBER_GPD_APP_INFO_COM_APP_DESCRIPTION_FOLLOWS_MASK 0x20
//

//Commissioning Reply Option Flag
//
// PanId Present sub-field
#define EMBER_GPD_COM_REP_OPT_PANID_PRESENT_MASK   0x01
// Key Present sub-field
#define EMBER_GPD_COM_REP_OPT_KEY_PRESENT_MASK     0x02
// Security Key Encrypted
#define EMBER_GPD_COM_REP_OPT_KEY_ENCRYPT_MASK     0x04
// Security Level sub-field
#define EMBER_GPD_COM_REP_OPT_SECURITY_LEVEL_MASK  0x18
#define EMBER_GPD_COM_REP_OPT_SECURITY_LEVEL_OFFSET 0x03
// Haskey sub-field
#define EMBER_GPD_COM_REP_OPT_KEY_TYPE_MASK        0xE0
#define EMBER_GPD_COM_REP_OPT_KEY_TYPE_OFFSET      0x05

#define EMBER_GPD_MIC_LENGTH            4

#define EMBER_GPD_CMD_MAX_PAYLOAD_SIZE  40

// SecurityLevel enumeration for type EmberGpdSecurityLevel_t
typedef uint8_t EmberGpdSecurityLevel_t;
// Security Key enumeration for type EmberGpdSecurityKeyType_t
typedef uint8_t EmberGpdSecurityKeyType_t;
// Security Key Indication values
typedef uint8_t EmberGpdSecurityKeyIndicationType_t;
// Application Radio State
typedef uint8_t EmberGpdRadioState_t;
enum {
  GPD_RADIO_STATE_IDLE         = 0x00,
  GPD_RADIO_STATE_TRANSMITTING = 0x01,
  GPD_RADIO_STATE_RECEIVING    = 0x02
};

typedef uint8_t EmberGpdApplicationState_t;
enum {
  EMBER_GPD_APP_STATE_NOT_COMMISSIONED,
  EMBER_GPD_APP_STATE_CHANNEL_REQUEST,
  EMBER_GPD_APP_STATE_CHANNEL_RECEIVED,
  EMBER_GPD_APP_STATE_COMMISSIONING_REQUEST,
  EMBER_GPD_APP_STATE_COMMISSIONING_REPLY_RECIEVED,
  EMBER_GPD_APP_STATE_COMMISSIONING_SUCCESS_REQUEST,
  EMBER_GPD_APP_STATE_OPERATIONAL,
  EMBER_GPD_APP_STATE_OPERATIONAL_COMMAND_REQUEST,
  EMBER_GPD_APP_STATE_OPERATIONAL_COMMAND_RECEIVED,
  EMBER_GPD_APP_STATE_INVALID
};

// Callback type defines
typedef uint8_t EmebrGpdNvLoadStore_t;
enum {
  EMEBER_GPD_AF_CALLBACK_LOAD_GPD_FROM_NVM,
  EMEBER_GPD_AF_CALLBACK_STORE_GPD_TO_NVM,
};

#define EMBER_AP_PLUGIN_APPS_APP_NEXT_RX_CHANNEL 11
#define EMBER_AP_PLUGIN_APPS_APP_SECOND_NEXT_RX_CHANNEL 11
#define EMBER_AF_PLUGIN_APPS_CMD_RESEND_NUMBER 1

// GPD address structure
typedef struct {
  union {
    uint32_t srcId;
    uint8_t ieee[8];
  }id;
  uint8_t gpdEndpoint;
  uint8_t appId;
}EmberGpdAddr_t;

// GPD device
typedef struct {
  EmberGpdAddr_t addr;
  bool autoCommissioning;
  uint8_t rxAfterTx;
  uint8_t rxOffset;
  uint8_t minRxWindow;
  EmberGpdSecurityLevel_t securityLevel;
  EmberGpdSecurityKeyType_t securityKeyType;
  uint8_t securityKey[16];
  uint32_t securityFrameCounter;
  bool skipCca;
  uint8_t channel;
  EmberGpdRadioState_t radioState;
  EmberGpdApplicationState_t gpdState;
}EmberGpd_t;

// Function Prototypes
void emberGpdRadioInit();
int8_t emberAfGpdfSend(uint8_t frameType,
                       EmberGpd_t * gpd,
                       uint8_t* payload,
                       uint8_t payloadLength,
                       uint8_t repetition_nb);

uint8_t emberBuildMaintMpdu(uint8_t * mpdu,
                            EmberGpd_t * gpd,
                            uint8_t * pPayload,
                            uint8_t payloadLength);

int8_t emberGpdProcessIncommingCommand(uint8_t * gpdCommandBuffer,
                                       uint8_t length);

int8_t emberGpdParseAndProcessRxGpdf(uint8_t * buffer,
                                     uint16_t length);

// Radio Function Prototypes
void emberGpdRailStartTxWrapper(bool skipCca,
                                uint16_t channel);
void emberGpdRailWriteTxFifoWrapper(const uint8_t *dataPtr,
                                    uint16_t writeLength);
void emberGpdRailStartRxWrapper(uint8_t channel);
void emberGpdRailIdleWrapper(void);
uint16_t emberGpdRailGetRadioEntropyWrapper(uint8_t *dataPtr, uint16_t dataLength);
void emberGpdRailProvideRailHandle(RAIL_Handle_t handle);

// Security Function Prototypes
int8_t emberGpdSecurityEncryptKey(EmberGpdAddr_t * addr,
                                  const uint8_t* oobKey,
                                  uint8_t * oobScured,
                                  uint8_t * mic);
void emberGpdSecurityInit(EmberGpdAddr_t * addr,
                          uint8_t* securityKey,
                          uint32_t frameCounter);
void emberGpdSetFcIntoTxNonce(uint32_t value);
void emberGpdSetFcIntoRxNonce(uint32_t value);

int8_t emberTagTxBuff(uint8_t * data,
                      uint8_t length,
                      uint8_t authStart,
                      uint8_t authLength);

int8_t emberEncryptAndTagTxBuff(uint8_t* data,
                                uint8_t length,
                                uint8_t payloadLength,
                                uint8_t authStart,
                                uint8_t authLength);

int8_t emberGpdMbedtlsCcmSetkeyWraper(const uint8_t * key);

int8_t emberGpdSecurityDecryptKey(EmberGpdAddr_t * addr,
                                  uint8_t * pKeyEncrypted,
                                  uint8_t * pKeyMic,
                                  uint8_t * pSecCounter,
                                  uint8_t * decryptedKey);

int8_t emberDecryptRxBuffer(uint8_t * rxMpdu,
                            uint8_t authStart,
                            uint8_t encrLength,
                            uint8_t authLength);
// Timer Functions
void emberGpdLeTimerInit(void);
void emberGpdLoadLeTimer(uint32_t timeInUs);
bool emberGpdLeTimerRunning(void);

// Utility Functions
void emberGpdUtilityCopy4Bytes(uint8_t * dst, uint32_t value);
bool emberGpdUtilityAddrMatch(EmberGpdAddr_t * a1, EmberGpdAddr_t * a2);

// AF Send Receive functions
uint8_t * emberGpdGetRxMpdu(void);

// Nwk Functions
uint8_t emberGpdBuildMpdu(uint8_t * mpdu,
                          uint8_t * payload,
                          uint8_t payloadLength,
                          EmberGpd_t * gpd);
uint8_t emberGpdSecureOutgoingMPdu(uint8_t * mpdu,
                                   uint8_t mpduLength,
                                   uint8_t payloadLength,
                                   EmberGpdSecurityLevel_t securityLevel);
uint8_t emberBuildOutGoingPdu(uint8_t frameType,
                              uint8_t * mpdu,
                              uint8_t * payload,
                              uint8_t payloadLength,
                              EmberGpd_t * gpd);
// Commissioning
int8_t emberGpdProcessCommissioningReply(EmberGpd_t * gpd,
                                         uint8_t payload_size,
                                         uint8_t * pRxbuffer);
// Gpd Node
EmberGpd_t * emberGpdInit(void);
EmberGpd_t * emberGpdGetGpd(void);
void emberGpdSetChannel(uint8_t channel);
void emberGpdSetState(uint8_t gpdState);
EmberGpdAddr_t * emberGpdGetAddr(void);
void emberGpdLoadGpdDefaultConfiguration(EmberGpd_t * gpd);

// NV functions
void emberGpdNvInit(void);
void emberGpdSaveGpdFrameCounterToShadow(uint32_t frameCounter);
void emberGpdCopyToShadow(EmberGpd_t * gpd);
void emberGpdCopyFromShadow(EmberGpd_t * gpd);
void emberGpdStoreSecDataToNV(EmberGpd_t * gpd);
void emberGpdStoreSecDataFromNV(EmberGpd_t * gpd);
void emberGpdLoadSecDataFromNV(EmberGpd_t * gpd);

//Incomming Command and Message Handler
void emberGpdIncomingCommandHandler(uint8_t *gpdCommandBuffer,
                                    uint8_t length);
void emberGpdIncomingMessageHandler(uint8_t * buffer,
                                    uint16_t length);

void emberGpdAfPluginCommission(EmberGpd_t * gpd);
void emberGpdAfPluginDeCommission(EmberGpd_t * gpd);
#endif
