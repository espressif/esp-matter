/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Green Power Translation Table plugin.
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

#ifndef _SILABS_GREEN_POWER_TRANSLATION_TABLE_H_
#define _SILABS_GREEN_POWER_TRANSLATION_TABLE_H_

#ifdef UC_BUILD
#include "sl_component_catalog.h"
// This translation table header is by the green power server to perform default
// command translation, even without the translation table component. Hence the
// the folowing configuration header inclussion is conditionalised.
#ifdef SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
#include "green-power-translation-table-config.h"
#endif // SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
#else
// The following flag is defined for non-UC builds.
#define SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
#endif // UC_BUILD

/**
 * @defgroup green-power-translation-table Green Power Translation Table
 * @ingroup component
 * @brief API and Callbacks for the Green Power Translation Table Component
 *
 * A component implementing the translation table functionality of the Green Power cluster.
 *
 */

/**
 * @addtogroup green-power-translation-table
 * @{
 */
#ifndef UC_BUILD
#ifndef EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_CUSTOMIZED_GPD_TRANSLATION_TABLE_SIZE
#define EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_CUSTOMIZED_GPD_TRANSLATION_TABLE_SIZE (20)
#endif

#ifndef EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_TRANSLATION_TABLE_SIZE
#define EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_TRANSLATION_TABLE_SIZE 30
#endif
#endif // !UC_BUILD
#define EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ADDITIONALINFO_TABLE_SIZE EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_TRANSLATION_TABLE_SIZE

#define EMBER_AF_GREEN_POWER_SERVER_TRANSLATION_TABLE_ENTRY_ZCL_PAYLOAD_LEN (7)

// defines macros for GPD To ZCL CMD Mapping Payload SRC
#define  EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_PRECONFIGURED 1
#define  EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_NA            2
#define  EMBER_AF_GREEN_POWER_ZCL_PAYLOAD_SRC_GPD_CMD       4

#define GREEN_POWER_SERVER_GPD_COMMAND_PAYLOAD_UNSPECIFIED_LENGTH 0xFF
#define GREEN_POWER_SERVER_GPD_COMMAND_PAYLOAD_TT_DERIVED_LENGTH  0xFE

#define GP_TRANSLATION_TABLE_STATUS_SUCCESS                0x00
#define GP_TRANSLATION_TABLE_STATUS_FAILED                 0xFF
#define GP_TRANSLATION_TABLE_STATUS_FULL                   0xFE
#define GP_TRANSLATION_TABLE_STATUS_EMPTY                  0xFD
#define GP_TRANSLATION_TABLE_STATUS_ENTRY_EMPTY            0xFC
#define GP_TRANSLATION_TABLE_STATUS_ENTRY_NOT_EMPTY        0xFB
#define GP_TRANSLATION_TABLE_STATUS_PARAM_DOES_NOT_MATCH   0xFA
#define GP_TRANSLATION_TABLE_STATUS_CUSTOMIZED_TABLE_FULL  0xF9

#define GP_TRANSLATION_TABLE_ENTRY_INVALID_INDEX 0xFF

typedef uint8_t GpTableType;
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum EmGpTableType
#else
enum
#endif //DOXYGEN_SHOULD_SKIP_THIS
{
  NO_ENTRY,
  DEFAULT_TABLE_ENTRY,
  CUSTOMIZED_TABLE_ENTRY,
}; //EmGpTableType

#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum IncomingReqType
#else
enum
#endif //DOXYGEN_SHOULD_SKIP_THIS
{
  ADD_PAIRED_DEVICE = 1,
  DELETE_PAIRED_DEVICE = 1,
  TRANSLATION_TABLE_UPDATE = 2,
}; //IncommingReqType

typedef struct {
  bool            validEntry;
  uint8_t         gpdCommand;
  uint8_t         endpoint;
  uint16_t        zigbeeProfile;
  uint16_t        zigbeeCluster;
  uint8_t         serverClient;
  uint8_t         zigbeeCommandId;
  uint8_t         payloadSrc;

  // This is a Zigbee Octate String, fist byte is length sub-field followed by payload.
  // The length sub-field has following special meanings
  // 1. If the Length sub-field of the ZigBee Command payload field is set to 0x00,
  //    the Payload sub-field is not present, and the ZigBee command is sent without payload.
  // 2. If the Length sub-field of the ZigBee Command payload field is set to 0xff,
  //    the Payload sub-field is not present, and the payload from the triggering
  //    GPD command is to be copied verbatim into the ZigBee command.
  // 3. If the Length sub-field of the ZigBee Command payload field is set to 0xfe,
  //    the Payload sub-field is not present, and the payload from the triggering GPD command
  //    needs to be parsed. For all other values of the Length sub-field,
  uint8_t zclPayloadDefault[EMBER_AF_GREEN_POWER_SERVER_TRANSLATION_TABLE_ENTRY_ZCL_PAYLOAD_LEN];
} EmberAfGreenPowerServerGpdSubTranslationTableEntry;

// The following declaration/definitions are only needed if the translation table is installed
// by the user.
#ifdef SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
typedef struct {
  uint8_t applicationId;
  uint8_t additionalInfoBlockPresent;
} GpTranslationTableOptionField;

typedef struct {
  uint8_t       switchType;
  uint8_t       nbOfIdentifiedContacts;
  uint8_t       nbOfTTEntriesNeeded;
  uint8_t       indicativeBitmask;
} EmGpSwitchTypeData;

typedef struct {
  bool            infoBlockPresent;
  uint8_t         gpdCommand;
  uint8_t         zbEndpoint;
  uint8_t         offset;
  GpTableType     entry;
  EmberGpAddress  gpAddr;
  EmberGpApplicationId gpApplicationId;
  uint8_t         additionalInfoOffset;
} EmGpCommandTranslationTableEntry;

typedef struct {
  uint8_t       endpoint;
  uint8_t       gpdCommand;
  uint16_t      zigbeeProfile;
} EmberAfGreenPowerServerMultiSensorTranslation;

typedef struct {
  EmGpCommandTranslationTableEntry TableEntry[EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE_TRANSLATION_TABLE_SIZE];
  uint8_t totalNoOfEntries;
} EmGpCommandTranslationTable;

typedef struct {
  EmberGpTranslationTableAdditionalInfoBlockOptionRecordField additionalInfoBlock[EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ADDITIONALINFO_TABLE_SIZE];
  uint8_t validEntry[EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ADDITIONALINFO_TABLE_SIZE];
  uint8_t totlaNoOfEntries;
} EmberGpTranslationTableAdditionalInfoBlockField;

typedef struct {
  EmGpSwitchTypeData  SwitchType;
  EmberAfGreenPowerServerGpdSubTranslationTableEntry genericSwitchDefaultTableEntry;
} EmberAfGreenPowerServerDefautGenericSwTranslation;

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup gp_translation_table_cb Green Power Translation Table
 * @ingroup af_callback
 * @brief Callbacks for Green Power Translation Table Component
 *
 */

/**
 * @addtogroup gp_translation_table_cb
 * @{
 */

/** @brief Green power server translation table init callback.
 *
 * This function is called by the green power server translation table plugin
 * upon initializing the translation tables.
 *
 */
void emberAfPluginTableInitCallback(void);

/** @brief Green power server pass frame without translation callback
 *
 * This function is called by the green power server translation table plugin
 * to notify the application of a green power notification without translation
 * (when the endpoint is set as 0xFC).
 *
 * @param addr GPD address in the notification   Ver.: always
 * @param gpdCommandId GPD command id Ver.: always
 * @param gpdCommandPayload GPD command payload Ver.: always
 */
void emberAfGreenPowerClusterPassFrameWithoutTranslationCallback(EmberGpAddress * addr,
                                                                 uint8_t gpdCommandId,
                                                                 uint8_t * gpdCommandPayload);

/** @} */ // end of gp_translation_table_cb
/** @} */ // end of name Callbacks
/** @} */ // end of green-power-translation-table

EmGpCommandTranslationTable* emGpTransTableGetTranslationTable(void);

void emGpTransTableClearTranslationTable(void);
void emGpSetTranslationTableEntry(uint8_t index);
EmberAfGreenPowerServerGpdSubTranslationTableEntry* emGpGetCustomizedTable(void);
void emGpClearCustomizedTable(void);
void emGpSetCustomizedTableEntry(uint8_t index);
EmberGpTranslationTableAdditionalInfoBlockField * emGpGetAdditionalInfoTable(void);
void embGpClearAdditionalInfoBlockTable(void);
void emGpSetAdditionalInfoBlockTableEntry(uint8_t index);
uint8_t emGpTransTableGetTranslationTableEntry(uint8_t entryIndex,
                                               EmberAfGreenPowerServerGpdSubTranslationTableEntry *TranslationTableEntry);
uint8_t emGpTransTableReplaceTranslationTableEntryUpdateCommand(uint8_t Index,
                                                                bool infoBlockPresent,
                                                                EmberGpAddress * gpdAddr,
                                                                uint8_t gpdCommandId,
                                                                uint8_t ZbEndpoint,
                                                                uint16_t zigbeeProfile,
                                                                uint16_t zigbeeCluster,
                                                                uint8_t  zigbeeCommandId,
                                                                uint8_t payloadLength,
                                                                uint8_t* payload,
                                                                uint8_t payloadSrc,
                                                                uint8_t additionalInfoLength,
                                                                EmberGpTranslationTableAdditionalInfoBlockOptionRecordField* AdditionalInfoBlock);
uint8_t emGpTransTableAddTranslationTableEntryUpdateCommand(uint8_t Index,
                                                            bool infoBlockPresent,
                                                            EmberGpAddress * gpdAddr,
                                                            uint8_t gpdCommandId,
                                                            uint8_t ZbEndpoint,
                                                            uint16_t zigbeeProfile,
                                                            uint16_t zigbeeCluster,
                                                            uint8_t  zigbeeCommandId,
                                                            uint8_t payloadLength,
                                                            uint8_t* payload,
                                                            uint8_t payloadSrc,
                                                            uint8_t additionalInfoLength,
                                                            EmberGpTranslationTableAdditionalInfoBlockOptionRecordField* AdditionalInfoBlock);
uint16_t emGpCopyAdditionalInfoBlockArrayToStructure(uint8_t * additionalInfoBlockIn,
                                                     EmberGpTranslationTableAdditionalInfoBlockOptionRecordField * additionalInfoBlockOut,
                                                     uint8_t gpdCommandId);
uint8_t emGpTransTableFindMatchingTranslationTableEntry(uint8_t levelOfScan,
                                                        bool infoBlockPresent,
                                                        EmberGpAddress  * gpAddr,
                                                        uint8_t gpdCommandId,
                                                        uint8_t zbEndpoint,
                                                        uint8_t * gpdCmdPayload,
                                                        EmberGpTranslationTableAdditionalInfoBlockOptionRecordField* additionalInfoBlock,
                                                        uint8_t *outIndex,
                                                        uint8_t startIndex);
uint8_t emGpTransTableDeletePairedDevicefromTranslationTableEntry(EmberGpAddress * gpdAddr);
void emGpRemoveGpdEndpointFromTranslationTable (EmberGpAddress *gpdAddr, uint8_t zbEndpoint);
void emGpForwardGpdCommandBasedOnTranslationTable(EmberGpAddress *addr,
                                                  uint8_t gpdCommandId,
                                                  uint8_t * gpdCommandPayload);
uint8_t emGpFindMatchingGenericTranslationTableEntry(uint8_t entryType,
                                                     uint8_t incomingReqType,
                                                     uint8_t offset,
                                                     bool infoBlockPresent,
                                                     uint8_t gpdCommandId,
                                                     uint16_t zigbeeProfile,
                                                     uint16_t zigbeeCluster,
                                                     uint8_t  zigbeeCommandId,
                                                     uint8_t payloadLength,
                                                     uint8_t* payload,
                                                     uint8_t* outIndex);

const EmberAfGreenPowerServerGpdSubTranslationTableEntry* emGpGetDefaultTable(void);
void emGpPairingDoneThusSetCustomizedTranslationTable(EmberGpAddress * gpdAddr,
                                                      uint8_t gpdCommandId,
                                                      uint8_t endpoint);
extern EmberStatus emGpForwardGpdToMappedEndpoint(EmberGpAddress *addr,
                                                  uint8_t gpdCommandId,
                                                  uint8_t * gpdCommandPayload,
                                                  const EmberAfGreenPowerServerGpdSubTranslationTableEntry * genericTranslationTable,
                                                  uint8_t endpoint);
#endif // SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
#endif //_SILABS_GREEN_POWER_TRANSLATION_TABLE_H_
