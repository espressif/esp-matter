/***************************************************************************//**
 * @file
 * @brief Test harness code for validating the behavior of the key establishment
 *  cluster and modifying the normal behavior of App. Framework.
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

// this file contains all the common includes for clusters in the zcl-util
#include "app/framework/include/af.h"
#include "app/framework/util/af-main.h"
#include "app/framework/util/common.h"
#include "app/framework/util/util.h"
#include "app/util/serial/sl_zigbee_command_interpreter.h"
#include "test-harness.h"
#ifdef SL_CATALOG_CLI_PRESENT
#include "test-harness-cli.h"
#endif

#ifdef UC_BUILD
#include "sl_component_catalog.h"
#include "test-harness-config.h"
#if (EMBER_AF_PLUGIN_TEST_HARNESS_AUTO_REGISTRATION_START == 1)
#define AUTO_REGISTRATION_START
#endif
#ifdef SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT
#include "app/framework/plugin/key-establishment/key-establishment.h"
#if defined(ZCL_TERMINATE_KEY_ESTABLISHMENT_FROM_SERVER_COMMAND_ID)
#define ZCL_TERMINATE_KEY_ESTABLISHMENT_COMMAND_ID ZCL_TERMINATE_KEY_ESTABLISHMENT_FROM_SERVER_COMMAND_ID
#elif defined(ZCL_TERMINATE_KEY_ESTABLISHMENT_FROM_CLIENT_COMMAND_ID)
#define ZCL_TERMINATE_KEY_ESTABLISHMENT_COMMAND_ID ZCL_TERMINATE_KEY_ESTABLISHMENT_FROM_CLIENT_COMMAND_ID
#else
#error "ZCL TerminateKeyEstablishment Command ID not defined"
#endif
#endif // ZCL_TERMINATE_KEY_ESTABLISHMENT_FROM_SERVER_COMMAND_ID || ZCL_TERMINATE_KEY_ESTABLISHMENT_FROM_CLIENT_COMMAND_ID
#ifdef SL_CATALOG_ZIGBEE_PRICE_SERVER_PRESENT
#include "price-server.h"
#endif
#ifdef SL_CATALOG_ZIGBEE_TRUST_CENTER_KEEPALIVE_PRESENT
#include "trust-center-keepalive.h"
#endif
#ifdef SL_CATALOG_ZIGBEE_OTA_STORAGE_SIMPLE_RAM_PRESENT
#include "ota-storage-ram.h"
#endif
#ifdef SL_CATALOG_ZIGBEE_TRUST_CENTER_NWK_KEY_UPDATE_BROADCAST_PRESENT
#include "app/framework/plugin/trust-center-nwk-key-update-broadcast/trust-center-nwk-key-update-broadcast.h"
#endif
#ifdef SL_CATALOG_ZIGBEE_TRUST_CENTER_NWK_KEY_UPDATE_UNICAST_PRESENT
#include "app/framework/plugin/trust-center-nwk-key-update-unicast/trust-center-nwk-key-update-unicast.h"
#endif
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_TEST_HARNESS_AUTO_REGISTRATION_START
#define AUTO_REGISTRATION_START
#endif
#ifdef EMBER_AF_PLUGIN_KEY_ESTABLISHMENT
#define SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT
#include "app/framework/plugin/key-establishment/key-establishment.h"
#endif
#ifdef EMBER_AF_PLUGIN_PRICE_SERVER
#define SL_CATALOG_ZIGBEE_PRICE_SERVER_PRESENT
#include "app/framework/plugin/price-server/price-server.h"
#endif
#ifdef EMBER_AF_PLUGIN_TRUST_CENTER_KEEPALIVE
#define SL_CATALOG_ZIGBEE_TRUST_CENTER_KEEPALIVE_PRESENT
#include "app/framework/plugin/trust-center-keepalive/trust-center-keepalive.h"
#endif
#ifdef EMBER_AF_PLUGIN_OTA_STORAGE_SIMPLE_RAM
#define SL_CATALOG_ZIGBEE_OTA_STORAGE_SIMPLE_RAM_PRESENT
#include "app/framework/plugin/ota-storage-simple-ram/ota-storage-ram.h"
#endif
#ifdef EMBER_AF_PLUGIN_TRUST_CENTER_NWK_KEY_UPDATE_BROADCAST
#define SL_CATALOG_ZIGBEE_TRUST_CENTER_NWK_KEY_UPDATE_BROADCAST_PRESENT
#include "app/framework/plugin/trust-center-nwk-key-update-broadcast/trust-center-nwk-key-update-broadcast.h"
#endif
#ifdef EMBER_AF_PLUGIN_TRUST_CENTER_NWK_KEY_UPDATE_UNICAST
#define SL_CATALOG_ZIGBEE_TRUST_CENTER_NWK_KEY_UPDATE_UNICAST_PRESENT
#include "app/framework/plugin/trust-center-nwk-key-update-unicast/trust-center-nwk-key-update-unicast.h"
#endif
#ifdef EMBER_AF_PLUGIN_TRUST_CENTER_NWK_KEY_UPDATE_PERIODIC
#define SL_CATALOG_ZIGBEE_TRUST_CENTER_NWK_KEY_UPDATE_PERIODIC_PRESENT
#endif
#ifdef EMBER_AF_PLUGIN_CONCENTRATOR
#define SL_CATALOG_ZIGBEE_CONCENTRATOR_PRESENT
#endif
#ifdef EMBER_AF_PLUGIN_NETWORK_FIND
#define SL_CATALOG_ZIGBEE_NETWORK_FIND_PRESENT
#endif
#ifdef EMBER_AF_PLUGIN_ZLL_LIBRARY
#define SL_CATALOG_ZIGBEE_LIGHT_LINK_PRESENT
#endif
#endif // UC_BUILD

#include "stack/include/source-route.h"

#if !defined(EZSP_HOST)
  #include "stack/include/cbke-crypto-engine.h"
#endif

#if !defined(EZSP_HOST)
extern uint8_t emRouterNeighborTableSize;
  #if defined(EMBER_TEST) || defined(EMBER_STACK_TEST_HARNESS)
extern uint8_t emRouterNeighborTableSize;
    #define STACK_TEST_HARNESS
  #endif
#endif

//------------------------------------------------------------------------------
// Globals

typedef uint8_t TestHarnessMode;

#define MODE_NORMAL           0
#define MODE_CERT_MANGLE      1
#define MODE_OUT_OF_SEQUENCE  2
#define MODE_NO_RESOURCES     3
#define MODE_TIMEOUT          4
#define MODE_DELAY_CBKE       5
#define MODE_DEFAULT_RESPONSE 6
#define MODE_KEY_MANGLE       7

static const char * modeText[] = {
  "Normal",
  "Cert Mangle",
  "Out of Sequence",
  "No Resources",
  "Cause Timeout",
  "Delay CBKE operation",
  "Default Response",
  "Key Mangle"
};

// Workaround for EMAPPFWKV2-1559.
#ifndef UNUSED
  #define UNUSED
#endif

static TestHarnessMode testHarnessMode = MODE_NORMAL;
static uint8_t respondToCommandWithOutOfSequence = ZCL_INITIATE_KEY_ESTABLISHMENT_REQUEST_COMMAND_ID;
static int8_t certLengthMod = 0;
static UNUSED int8_t keyLengthMod  = 0;
#define CERT_MANGLE_NONE    0
#define CERT_MANGLE_LENGTH  1
#define CERT_MANGLE_ISSUER  2
#define CERT_MANGLE_CORRUPT 3
#define CERT_MANGLE_SUBJECT 4
#define CERT_MANGLE_VALUE   5

static const char * certMangleText[] = {
  "None",
  "Mangle Length",
  "Rewrite Issuer",
  "Corrupt Cert",
  "Change byte",
};

#if defined(EMBER_AF_HAS_SECURITY_PROFILE_SE_TEST)
  #define DEFAULT_POLICY true
#else
  #define DEFAULT_POLICY false
#endif
bool emKeyEstablishmentPolicyAllowNewKeyEntries = DEFAULT_POLICY;
bool emAfTestHarnessSupportForNewPriceFields = true;

typedef uint8_t CertMangleType;

static CertMangleType certMangleType = CERT_MANGLE_NONE;

// Offset from start of ZCL header
//   - ZCL overhead (3 bytes)
//   - KE suite (2 bytes)
//   - Ephemeral Data Generate Time (1 byte)
//   - Confirm Key Generate Time (1 byte)
#define CERT_OFFSET_IN_INITIATE_KEY_ESTABLISHMENT_MESSAGE (3 + 2 + 1 + 1)

static uint8_t invalidEui64[] = {
  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
};

extern uint32_t testHarnessChannelMask;
extern const uint32_t testHarnessOriginalChannelMask;

extern EmberStatus emberAddChild(EmberNodeId shortId,
                                 EmberEUI64 longId,
                                 uint8_t childType);
#if !defined(EZSP_HOST) && defined(EMBER_TEST)
extern uint8_t emTestStackComplianceRevision;
#endif

#ifndef UC_BUILD

void emberAfPluginTestHarnessChannelMaskAddOrRemoveCommand(void);
void emberAfPluginTestHarnessKeyEstablishmentSetModeCommand(void);
void emberAfPluginTestHarnessCertMangleCommand(void);
void emberAfPluginTestHarnessKeyEstablishmentKeyMangleCommand(void);
void emberAfPluginTestHarnessStatusCommand(void);
void emberAfPluginTestHarnessSetRegistrationCommand(void);
void emberAfPluginTestHarnessSetApsSecurityForClusterCommand(void);
void emberAfPluginTestHarnessPriceSendNewFieldsCommand(void);
void emberAfPluginTestHarnessTcKeepaliveSendCommand(void);
void emberAfPluginTestHarnessTcKeepaliveStartStopCommand(void);
void emberAfPluginTestHarnessOtaImageMangleCommand(void);
void emberAfPluginTestHarnessRadioOnOffCommand(void);
void emberAfPluginTestHarnessSelectSuiteCommand(void);

#if defined(SL_CATALOG_ZIGBEE_CONCENTRATOR_PRESENT)
void emberAfPluginTestHarnessConcentratorStartStopCommand(void);
#endif

#if defined(SL_CATALOG_ZIGBEE_NETWORK_FIND_PRESENT)
void emberAfPluginTestHarnessChannelMaskResetClearAllCommand(void);
#endif

void emberAfPluginTestHarnessEnableDisableEndpointCommand(void);
void emberAfPluginTestHarnessEndpointStatusCommand(void);
void emberAfPluginTestHarnessClusterEndpointIndexCommand(void);

extern EmberCommandEntry emAfReadWriteAttributeTestCommands[];

#if defined(SL_CATALOG_ZIGBEE_TRUST_CENTER_NWK_KEY_UPDATE_PERIODIC_PRESENT)
void emberAfPluginTestHarnessKeyUpdateCommand(void);
#endif

// TODO: this should be modified once we've upgraded the generated CLI
// mechanism to conditionally generate CLI based on macros such as these
#if defined(STACK_TEST_HARNESS) || defined(EMBER_AF_GENERATE_CLI)
void emberAfPluginTestHarnessLimitBeaconsOnOffCommand(void);
#endif

#if !defined(EMBER_AF_GENERATE_CLI)

static EmberCommandEntry certMangleCommands[] = {
  emberCommandEntryAction("length", emberAfPluginTestHarnessCertMangleCommand, "s", "Mangles the length of the certificate"),
  emberCommandEntryAction("issuer", emberAfPluginTestHarnessCertMangleCommand, "b", "Changes the issuer in the certificate"),
  emberCommandEntryAction("corrupt", emberAfPluginTestHarnessCertMangleCommand, "u", "Corrupts a single byte in the cert"),
  emberCommandEntryAction("subject", emberAfPluginTestHarnessCertMangleCommand, "b", "Changes the subject (IEEE) of the cert"),
  emberCommandEntryAction("change-byte", emberAfPluginTestHarnessCertMangleCommand, "uu", "Changes the value of a single byte of the cert"),

  emberCommandEntryTerminator(),
};

static EmberCommandEntry registrationCommands[] = {
  emberCommandEntryAction("on", emberAfPluginTestHarnessSetRegistrationCommand, "", "Turns automatic SE registration on."),
  emberCommandEntryAction("off", emberAfPluginTestHarnessSetRegistrationCommand, "", "Turns automatic SE registration off."),
  emberCommandEntryTerminator(),
};

static EmberCommandEntry apsSecurityForClusterCommands[] = {
  emberCommandEntryAction("on", emberAfPluginTestHarnessSetApsSecurityForClusterCommand, "v", "Turns on automatic APS security based on cluster."),
  emberCommandEntryAction("off", emberAfPluginTestHarnessSetApsSecurityForClusterCommand, "", "Turns off automatic APS security based on cluster."),
  emberCommandEntryTerminator(),
};

static EmberCommandEntry keyEstablishmentCommands[] = {
  emberCommandEntryAction("normal-mode", emberAfPluginTestHarnessKeyEstablishmentSetModeCommand, "", "Sets key establishment to normal, compliant mode."),
  emberCommandEntryAction("no-resources", emberAfPluginTestHarnessKeyEstablishmentSetModeCommand, "", "All received KE requests will be responded with 'no resources'"),
  emberCommandEntryAction("out-of-sequence", emberAfPluginTestHarnessKeyEstablishmentSetModeCommand, "u", "Sends an out-of-sequence KE message based on the passed command ID."),
  emberCommandEntryAction("timeout", emberAfPluginTestHarnessKeyEstablishmentSetModeCommand, "", "Artificially creates a timeout by delaying an outgoing message."),
  emberCommandEntryAction("delay-cbke", emberAfPluginTestHarnessKeyEstablishmentSetModeCommand, "vv", "Changes the advertised delays by the local device for CBKE."),
  emberCommandEntrySubMenu("cert-mangle", certMangleCommands, "Commands to mangle the certificate sent by the local device."),
  emberCommandEntryAction("default-resp", emberAfPluginTestHarnessKeyEstablishmentSetModeCommand, "", "Sends a default response error message in response to initate KE."),
  emberCommandEntryAction("new-key-policy", emberAfPluginTestHarnessKeyEstablishmentSetModeCommand, "u", "Sets the policy of whether the TC allows new KE requests."),
  emberCommandEntryAction("reset-aps-fc", emberAfPluginTestHarnessKeyEstablishmentSetModeCommand, "", "Forces the local device to reset its outgoing APS FC."),
  emberCommandEntryAction("adv-aps-fc", emberAfPluginTestHarnessKeyEstablishmentSetModeCommand, "", "Advances the local device's outgoing APS FC by 4096."),
  emberCommandEntryTerminator(),
};

static EmberCommandEntry priceCommands[] = {
  emberCommandEntryAction("send-new-fields", emberAfPluginTestHarnessPriceSendNewFieldsCommand, "u", "Controls whether the new SE 1.1 price fields are included."),
  emberCommandEntryTerminator(),
};

static EmberCommandEntry tcKeepaliveCommands[] = {
  emberCommandEntryAction("send", emberAfPluginTestHarnessTcKeepaliveSendCommand, "", "Sends a new TC keepalive."),
  emberCommandEntryAction("start", emberAfPluginTestHarnessTcKeepaliveStartStopCommand, "", "Starts the TC keepalive state machine."),
  emberCommandEntryAction("stop", emberAfPluginTestHarnessTcKeepaliveStartStopCommand, "", "Stops the TC keepalive state machine."),
  emberCommandEntryTerminator(),
};

static EmberCommandEntry otaCommands[] = {
  emberCommandEntryAction("image-mangle",
                          emberAfPluginTestHarnessOtaImageMangleCommand,
                          "v",
                          "Mangles the Simple Storage RAM OTA image."),
  emberCommandEntryTerminator(),
};

#if defined(SL_CATALOG_ZIGBEE_TRUST_CENTER_NWK_KEY_UPDATE_PERIODIC_PRESENT)
static EmberCommandEntry keyUpdateCommands[] = {
  emberCommandEntryAction("unicast", emberAfPluginTestHarnessKeyUpdateCommand, "", "Changes TC NWK key update mechanism to unicast with APS security."),
  emberCommandEntryAction("broadcast", emberAfPluginTestHarnessKeyUpdateCommand, "", "Changes TC NWK key update mechanism to broadcast."),
  emberCommandEntryAction("now", emberAfPluginTestHarnessKeyUpdateCommand, "", "Starts a TC NWK key update now"),
  emberCommandEntryTerminator(),
};
#endif

#if defined(SL_CATALOG_ZIGBEE_CONCENTRATOR_PRESENT)
static EmberCommandEntry concentratorCommands[] = {
  emberCommandEntryAction("start",
                          emberAfPluginTestHarnessConcentratorStartStopCommand,
                          "",
                          "Starts the concentrator's periodic broadcasts."),
  emberCommandEntryAction("stop",
                          emberAfPluginTestHarnessConcentratorStartStopCommand,
                          "",
                          "Stops the concentrator's periodic broadcasts."),
  emberCommandEntryTerminator(),
};
#endif

#if defined(STACK_TEST_HARNESS)

static EmberCommandEntry limitBeaconsCommands[] = {
  emberCommandEntryAction("on",
                          emberAfPluginTestHarnessLimitBeaconsOnOffCommand,
                          "",
                          "Enables a limit to the max number of outgoing beacons."),
  emberCommandEntryAction("off",
                          emberAfPluginTestHarnessLimitBeaconsOnOffCommand,
                          "",
                          "Disables a limit to the max number of outgoing beacons."),
  emberCommandEntryTerminator(),
};

static EmberCommandEntry stackTestHarnessCommands[] = {
  emberCommandEntrySubMenu("limit-beacons",
                           limitBeaconsCommands,
                           "Commands to limit the outgoing beacons."),
  emberCommandEntryTerminator(),
};
#endif

#if defined(SL_CATALOG_ZIGBEE_NETWORK_FIND_PRESENT)
static EmberCommandEntry channelMaskCommands[] = {
  emberCommandEntryAction("clear",
                          emberAfPluginTestHarnessChannelMaskResetClearAllCommand,
                          "",
                          "Clears the channel mask used by network find."),

  emberCommandEntryAction("reset",
                          emberAfPluginTestHarnessChannelMaskResetClearAllCommand,
                          "",
                          "Resets the channel mask back to the app default"),

  emberCommandEntryAction("all",
                          emberAfPluginTestHarnessChannelMaskResetClearAllCommand,
                          "",
                          "Sets the channel mask to all channels"),

  emberCommandEntryAction("add",
                          emberAfPluginTestHarnessChannelMaskAddOrRemoveCommand,
                          "u",
                          "Add a channel to the mask"),

  emberCommandEntryAction("remove",
                          emberAfPluginTestHarnessChannelMaskAddOrRemoveCommand,
                          "u",
                          "Removes a channel from the mask"),

  emberCommandEntryTerminator(),
};
#endif

static EmberCommandEntry endpointCommands[] = {
  emberCommandEntryAction("enable",
                          emberAfPluginTestHarnessEnableDisableEndpointCommand,
                          "u",
                          "Enables the endpont to receive messages and be discovered"),

  emberCommandEntryAction("disable",
                          emberAfPluginTestHarnessEnableDisableEndpointCommand,
                          "u",
                          "Disables the endpont to receive messages and be discovered"),

  emberCommandEntryAction("status",
                          emberAfPluginTestHarnessEndpointStatusCommand,
                          "",
                          "Prints the status of all endpoints"),

  emberCommandEntryAction("cluster-endpoint-index",
                          emberAfPluginTestHarnessClusterEndpointIndexCommand,
                          "uvu",
                          "Get cluster endpoint index"),

  emberCommandEntryTerminator(),
};

static EmberCommandEntry radioCommands[] = {
  emberCommandEntryAction("on", emberAfPluginTestHarnessRadioOnOffCommand, "",
                          "Turns on the radio if it was previously turned off"),
  emberCommandEntryAction("off", emberAfPluginTestHarnessRadioOnOffCommand, "",
                          "Turns off the radio so that no messages are sent."),
  emberCommandEntryTerminator(),
};

EmberCommandEntry emberAfPluginTestHarnessCommands[] = {
  emberCommandEntryAction("status",
                          emberAfPluginTestHarnessStatusCommand,
                          "",
                          "Prints the status of the test harness plugin"),

  emberCommandEntrySubMenu("aps-sec-for-clust",
                           apsSecurityForClusterCommands,
                           "APS Security on clusters"),
  emberCommandEntrySubMenu("registration",
                           registrationCommands,
                           "Enables/disables auto SE registration"),

#if defined(SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT)
  emberCommandEntrySubMenu("ke",
                           keyEstablishmentCommands,
                           "Key establishment commands"),
#endif

#if defined(SL_CATALOG_ZIGBEE_PRICE_SERVER_PRESENT)
  emberCommandEntrySubMenu("price",
                           priceCommands,
                           "Commands for manipulating price server"),
#endif

#if defined(SL_CATALOG_ZIGBEE_TRUST_CENTER_KEEPALIVE_PRESENT)
  emberCommandEntrySubMenu("tc-keepalive",
                           tcKeepaliveCommands,
                           "Commands for Keepalive messages to TC"),
#endif

#if defined(SL_CATALOG_ZIGBEE_TRUST_CENTER_NWK_KEY_UPDATE_PERIODIC_PRESENT)
  emberCommandEntrySubMenu("key-update",
                           keyUpdateCommands,
                           "Trust Center Key update commands"),
#endif

  emberCommandEntrySubMenu("ota",
                           otaCommands,
                           "OTA cluster commands"),

#if defined(SL_CATALOG_ZIGBEE_CONCENTRATOR_PRESENT)
  emberCommandEntrySubMenu("concentrator",
                           concentratorCommands,
                           "Concentrator commands"),
#endif

#if defined(STACK_TEST_HARNESS)
  emberCommandEntrySubMenu("stack",
                           stackTestHarnessCommands,
                           "Stack test harness commands"),
#endif

#if defined(SL_CATALOG_ZIGBEE_NETWORK_FIND_PRESENT)
  emberCommandEntrySubMenu("channel-mask",
                           channelMaskCommands,
                           "Manipulates the Network Find channel mask"),
#endif

  emberCommandEntrySubMenu("endpoint",
                           endpointCommands,
                           "Commands for manipulating endpoint messaging/discovery."),
  emberCommandEntrySubMenu("radio",
                           radioCommands,
                           "Commands to turn on/off the radio"),
  emberCommandEntrySubMenu("attributes",
                           emAfReadWriteAttributeTestCommands,
                           "Commands to test attributes"),

  emberCommandEntryTerminator(),
};

#endif // !defined(EMBER_AF_GENERATE_CLI)
#endif //UC_BUILD

static uint8_t certIndexToCorrupt = 0;
static uint8_t certIndexToChange  = 0;
static uint8_t certNewByteValue   = 0;
#define TEST_HARNESS_BACK_OFF_TIME_SECONDS 30

#define CBKE_KEY_ESTABLISHMENT_SUITE         0x0001   // per the spec

#define TEST_HARNESS_PRINT_PREFIX "TEST HARNESS: "

// Holds either the ephemeral public key or the 2 SMAC values.
// The SMAC values are the biggest piece of data.
static uint8_t* delayedData[EMBER_SMAC_SIZE * 2];
static bool stopRecursion = false;
static uint16_t cbkeDelaySeconds = 0;
static uint8_t delayedCbkeOperation = CBKE_OPERATION_GENERATE_KEYS;

#ifdef UC_BUILD
extern sl_zigbee_event_t emberAfPluginTestHarnessReadWriteAttributesTimeoutEvent;
extern void emberAfPluginTestHarnessReadWriteAttributesTimeoutEventHandler(SLXU_UC_EVENT);
sl_zigbee_event_t emAfKeyEstablishmentTestHarnessEvent;
#define testHarnessEventControl (&emAfKeyEstablishmentTestHarnessEvent)
void emAfKeyEstablishmentTestHarnessEventHandler(SLXU_UC_EVENT);
#else
EmberEventControl emAfKeyEstablishmentTestHarnessEventControl;
#define testHarnessEventControl emAfKeyEstablishmentTestHarnessEventControl
#endif

#ifdef SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT
// This is what is reported to the partner of key establishment.
uint16_t emAfKeyEstablishmentTestHarnessGenerateKeyTime = DEFAULT_EPHEMERAL_DATA_GENERATE_TIME_SECONDS;
uint16_t emAfKeyEstablishmentTestHarnessConfirmKeyTime = DEFAULT_GENERATE_SHARED_SECRET_TIME_SECONDS;
uint16_t emAfKeyEstablishmentTestHarnessAdvertisedGenerateKeyTime = DEFAULT_EPHEMERAL_DATA_GENERATE_TIME_SECONDS;
#endif // SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT

#define NULL_CLUSTER_ID 0xFFFF
static EmberAfClusterId clusterIdRequiringApsSecurity = NULL_CLUSTER_ID;

#if defined(AUTO_REGISTRATION_START)
  #define AUTO_REG_START true
#else
  #define AUTO_REG_START false
#endif

bool emAfTestHarnessAllowRegistration = AUTO_REG_START;

//------------------------------------------------------------------------------

static void testHarnessPrintVarArgs(const char * format,
                                    va_list vargs)
{
  emberAfCoreFlush();
  emberAfCorePrint(TEST_HARNESS_PRINT_PREFIX);
  (void) emberSerialPrintfVarArg(EMBER_AF_PRINT_OUTPUT, format, vargs);
  emberAfCoreFlush();
}

static void testHarnessPrintln(const char * format,
                               ...)
{
  va_list vargs;
  va_start(vargs, format);
  testHarnessPrintVarArgs(format, vargs);
  va_end(vargs);
  emberSerialPrintCarriageReturn(EMBER_AF_PRINT_OUTPUT);
}

static void resetTimeouts(void)
{
#ifdef SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT
  emAfKeyEstablishmentTestHarnessGenerateKeyTime = DEFAULT_EPHEMERAL_DATA_GENERATE_TIME_SECONDS;
  emAfKeyEstablishmentTestHarnessConfirmKeyTime = DEFAULT_GENERATE_SHARED_SECRET_TIME_SECONDS;
  emAfKeyEstablishmentTestHarnessAdvertisedGenerateKeyTime = DEFAULT_EPHEMERAL_DATA_GENERATE_TIME_SECONDS;
#endif // SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT
}

#if defined(SL_CATALOG_ZIGBEE_TRUST_CENTER_NWK_KEY_UPDATE_PERIODIC_PRESENT)
static bool unicastKeyUpdate = false;

extern EmberStatus emberAfTrustCenterStartUnicastNetworkKeyUpdate(void);
extern EmberStatus emberAfTrustCenterStartBroadcastNetworkKeyUpdate(void);

EmberStatus emberAfTrustCenterStartNetworkKeyUpdate(void)
{
  testHarnessPrintln("Using %p key update method",
                     (unicastKeyUpdate
                      ? "unicast"
                      : "broadcast"));
  return (unicastKeyUpdate
          ? emberAfTrustCenterStartUnicastNetworkKeyUpdate()
          : emberAfTrustCenterStartBroadcastNetworkKeyUpdate());
}
#endif

//------------------------------------------------------------------------------
// Misc. forward declarations

#ifdef UC_BUILD
#if defined(SL_CATALOG_ZIGBEE_TRUST_CENTER_NWK_KEY_UPDATE_PERIODIC_PRESENT)
extern sl_zigbee_event_t emberAfPluginTrustCenterNwkKeyUpdatePeriodicMyEvent;
#endif // SL_CATALOG_ZIGBEE_TRUST_CENTER_NWK_KEY_UPDATE_PERIODIC_PRESENT
#endif // UC_BUILD

void emberAfPluginTrustCenterNwkKeyUpdatePeriodicMyEventHandler(SLXU_UC_EVENT);

#if defined(STACK_TEST_HARNESS)
void emTestHarnessBeaconSuppressSet(bool enable);
uint8_t emTestHarnessBeaconSuppressGet(void);
#endif

#ifdef UC_BUILD
void emAfPluginTestHarnessInitCallback(SLXU_INIT_ARG)
{
  SLXU_INIT_UNUSED_ARG;

  slxu_zigbee_network_event_init(&emAfKeyEstablishmentTestHarnessEvent,
                                 emAfKeyEstablishmentTestHarnessEventHandler);
  slxu_zigbee_network_event_init(&emberAfPluginTestHarnessReadWriteAttributesTimeoutEvent,
                                 emberAfPluginTestHarnessReadWriteAttributesTimeoutEventHandler);
}
#endif
void emAfKeyEstablishmentTestHarnessEventHandler(SLXU_UC_EVENT)
{
  slxu_zigbee_event_set_inactive(testHarnessEventControl);
  testHarnessPrintln("Test Harness Event Handler fired, Tick: 0x%4X",
                     halCommonGetInt32uMillisecondTick());

  stopRecursion = true;
#if defined(SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT)
  testHarnessPrintln("Generating %p callback.",
                     ((delayedCbkeOperation
                       == CBKE_OPERATION_GENERATE_KEYS)
                      ? "emberGenerateCbkeKeysHandler()"
                      : "emberCalculateSmacsHandler()"));

  if (delayedCbkeOperation == CBKE_OPERATION_GENERATE_KEYS) {
    emAfPluginKeyEstablishmentGenerateCbkeKeysHandler(EMBER_SUCCESS,
                                                      (EmberPublicKeyData*)delayedData);
  } else {
    emAfPluginKeyEstablishmentCalculateSmacsHandler(EMBER_SUCCESS,
                                                    (EmberSmacData*)delayedData,
                                                    (EmberSmacData*)(delayedData + EMBER_SMAC_SIZE));
  }
#endif

  stopRecursion = false;
}

bool emAfKeyEstablishmentTestHarnessCbkeCallback(uint8_t cbkeOperation,
                                                 uint8_t* data1,
                                                 uint8_t* data2)
{
  if (testHarnessMode != MODE_DELAY_CBKE) {
    return false;
  }

  if (stopRecursion) {
    return false;
  }

  testHarnessPrintln("Delaying %p key callback for %d seconds",
                     (cbkeOperation == CBKE_OPERATION_GENERATE_KEYS
                      ? "ephemeral"
                      : "confirm"),
                     cbkeDelaySeconds);

  MEMMOVE(delayedData,
          data1,
          (cbkeOperation == CBKE_OPERATION_GENERATE_KEYS
           ? sizeof(EmberPublicKeyData)
           : sizeof(EmberSmacData)));

  if (data2 != NULL) {
    MEMMOVE(delayedData + sizeof(EmberSmacData),
            data2,
            sizeof(EmberSmacData));
  }

  delayedCbkeOperation = cbkeOperation;

  testHarnessPrintln("Test Harness Tick: 0x%4X",
                     halCommonGetInt32uMillisecondTick());
  uint32_t delayMs = (((uint32_t)(cbkeDelaySeconds))
                      * MILLISECOND_TICKS_PER_SECOND);
  if (delayMs <= EMBER_MAX_EVENT_DELAY_MS) {
    slxu_zigbee_event_set_delay_ms(testHarnessEventControl, delayMs);
  }
  return true;
}

bool emAfKeyEstablishmentTestHarnessMessageSendCallback(uint8_t message)
{
#if defined(SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT)
  uint8_t i;
  uint8_t* ptr = (appResponseData
                  + CERT_OFFSET_IN_INITIATE_KEY_ESTABLISHMENT_MESSAGE);
  uint8_t direction = (*appResponseData & ZCL_FRAME_CONTROL_DIRECTION_MASK);

  if (MODE_NORMAL == testHarnessMode) {
    return true;
  }

  if (testHarnessMode == MODE_CERT_MANGLE) {
    if (message == ZCL_INITIATE_KEY_ESTABLISHMENT_REQUEST_COMMAND_ID) {
      if (certMangleType == CERT_MANGLE_LENGTH) {
        if (certLengthMod > 0) {
          ptr = appResponseData + appResponseLength;
          for (i = 0; i < certLengthMod; i++) {
            *ptr = i;
            ptr++;
          }
        }
        appResponseLength += certLengthMod;

        testHarnessPrintln("Mangling certificate length by %p%d bytes",
                           (certLengthMod > 0
                            ? "+"
                            : ""),
                           certLengthMod);
      } else if (certMangleType == CERT_MANGLE_ISSUER
                 || certMangleType == CERT_MANGLE_SUBJECT) {
        testHarnessPrintln("appRespLen=%d, certLenMod=%d", appResponseLength, certLengthMod);
        ptr += (certMangleType == CERT_MANGLE_ISSUER
                ? ((appResponseLength == 81) ? CERT_ISSUER_OFFSET_283K1 : CERT_ISSUER_OFFSET)
                : ((appResponseLength == 81) ? CERT_SUBJECT_OFFSET_283K1 : CERT_SUBJECT_OFFSET));

        MEMMOVE(ptr, invalidEui64, EUI64_SIZE);
        testHarnessPrintln("Mangling certificate %p to be (>)%X%X%X%X%X%X%X%X",
                           (certMangleType == CERT_MANGLE_ISSUER
                            ? "issuer"
                            : "subject"),
                           invalidEui64[0],
                           invalidEui64[1],
                           invalidEui64[2],
                           invalidEui64[3],
                           invalidEui64[4],
                           invalidEui64[5],
                           invalidEui64[6],
                           invalidEui64[7]);
      } else if (certMangleType == CERT_MANGLE_CORRUPT) {
        // We bit flip the byte to make sure it is different than
        // its real value.
        ptr += certIndexToCorrupt;
        *ptr = ~(*ptr);
        testHarnessPrintln("Mangling byte at index %d in certificate.",
                           certIndexToCorrupt);
      } else if (certMangleType == CERT_MANGLE_VALUE) {
        ptr += certIndexToChange;
        *ptr = certNewByteValue;
        testHarnessPrintln("Changing byte at index %u to %u in certificate.",
                           certIndexToChange, certNewByteValue);
      }
    } else if (message == ZCL_TERMINATE_KEY_ESTABLISHMENT_COMMAND_ID) {
      if (certMangleType == CERT_MANGLE_CORRUPT || certMangleType == CERT_MANGLE_VALUE) {
        // To simulate that the test harness has NOT cancelled
        // key establishment due to a problem with calculating
        // or comparing the SMAC (acting as KE server), we
        // send a confirm key message instead.  The local server
        // has really cancelled KE but we want to test that the other
        // side will cancel it and send its own terminate message.
        ptr = appResponseData + 2; // jump over frame control and sequence num
        *ptr++ = ZCL_CONFIRM_KEY_DATA_RESPONSE_COMMAND_ID;
        for (i = 0; i < EMBER_SMAC_SIZE; i++) {
          *ptr++ = i;
        }
        appResponseLength = (EMBER_AF_ZCL_OVERHEAD
                             + EMBER_SMAC_SIZE);
        testHarnessPrintln("Rewriting Terminate Message as Confirm Key Message");
      }
    }
  } else if (testHarnessMode == MODE_OUT_OF_SEQUENCE) {
    if (message == respondToCommandWithOutOfSequence) {
      uint8_t *ptr = appResponseData + 2;  // ZCL Frame control and sequence number

      testHarnessPrintln("Sending out-of-sequence message");

      ptr[0] = (message == ZCL_CONFIRM_KEY_DATA_REQUEST_COMMAND_ID
                ? ZCL_INITIATE_KEY_ESTABLISHMENT_REQUEST_COMMAND_ID
                : (message + 1u));

      // Setting the outgoing message to the right length without really
      // filling the message with valid data means there would
      // probably be garbage or bad data.  However the receiving device should
      // check for an invalid command ID first before parsing the specific
      // fields in the message.
      appResponseLength = EMBER_AF_ZCL_OVERHEAD + emAfKeyEstablishMessageToDataSize[message];
    }
  } else if (testHarnessMode == MODE_NO_RESOURCES) {
    // If we are the client then we have to wait until after the first
    // message to send the 'no resources' message.
    if (!(direction == ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
          && message == ZCL_INITIATE_KEY_ESTABLISHMENT_REQUEST_COMMAND_ID)) {
      uint8_t *ptr = appResponseData + 2;  // ZCL Frame control and sequence number
      *ptr++ = ZCL_TERMINATE_KEY_ESTABLISHMENT_COMMAND_ID;
      *ptr++ = EMBER_ZCL_AMI_KEY_ESTABLISHMENT_STATUS_NO_RESOURCES;
      *ptr++ = TEST_HARNESS_BACK_OFF_TIME_SECONDS;
      *ptr++ = LOW_BYTE(emAfCurrentCbkeSuite);
      *ptr++ = HIGH_BYTE(emAfCurrentCbkeSuite);

      appResponseLength = ptr - appResponseData;

      testHarnessPrintln("Sending Terminate: NO_RESOURCES");
    }
  } else if (testHarnessMode == MODE_DEFAULT_RESPONSE) {
    // If we are the client then we have to wait until after the first
    // message to send the Default Response message.
    if (!(direction == ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
          && message == ZCL_INITIATE_KEY_ESTABLISHMENT_REQUEST_COMMAND_ID)) {
      uint8_t *ptr = appResponseData;
      uint8_t oldCommandId;
      *ptr++ = direction;
      ptr++; // skip sequence number, it was already written correctly.

      oldCommandId = *ptr;
      *ptr++ = ZCL_DEFAULT_RESPONSE_COMMAND_ID;

      // If we are the client, we send a response to the previous command ID.
      // If we are the server, we send a response to the current command ID.
      // This works because the client -> server and server -> client commands
      // are identical.  The client code will be sending the NEXT command ID,
      // which we are rewriting into a default response.  The server is sending
      // a command ID for the previous client command (a response ID to the
      // request), which has the same command ID as we are rewriting.
      *ptr++ = (direction == ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
                ? oldCommandId - 1
                : oldCommandId);
      *ptr++ = EMBER_ZCL_STATUS_FAILURE;

      appResponseLength = ptr - appResponseData;

      testHarnessPrintln("Sending Default Response: FAILURE");
    }
  } else if (testHarnessMode == MODE_TIMEOUT) {
    // If we are the client then we have to wait until after the first
    // message to induce a timeout.
    if (!(direction == ZCL_FRAME_CONTROL_CLIENT_TO_SERVER
          && message == ZCL_INITIATE_KEY_ESTABLISHMENT_REQUEST_COMMAND_ID)) {
      testHarnessPrintln("Suppressing message to induce timeout.");
      return false;
    }
  } else if (testHarnessMode == MODE_KEY_MANGLE) {
    //Change the length of the emphemeral Key.
    if (message == ZCL_EPHEMERAL_DATA_REQUEST_COMMAND_ID) {
      if (keyLengthMod > 0) {
        ptr = appResponseData + appResponseLength;
        for (i = 0; i < keyLengthMod; i++) {
          *ptr = i;
          ptr++;
        }
      }
      appResponseLength += keyLengthMod;

      testHarnessPrintln("Mangling key length by %p%d bytes",
                         (keyLengthMod > 0
                          ? "+"
                          : ""),
                         keyLengthMod);
    }
  }

  // Send the message
  return true;
#endif
  return false;
}

#if defined(UC_BUILD) && defined(SL_CATALOG_CLI_PRESENT)

void emberAfPluginTestHarnessCertMangleCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t position = sl_cli_get_command_count(arguments) - 1;
  uint8_t commandChar = sl_cli_get_command_string(arguments, position)[0];
  uint8_t secondChar  = sl_cli_get_command_string(arguments, position)[1];
  testHarnessMode = MODE_CERT_MANGLE;

  if (commandChar == 'l') {
    certMangleType = CERT_MANGLE_LENGTH;
    certLengthMod = (int8_t)sl_cli_get_argument_int8(arguments, 0);
  } else if (commandChar == 'i') {
    certMangleType = CERT_MANGLE_ISSUER;
    emberCopyEui64Argument(0, invalidEui64);
  } else if (commandChar == 'c' ) {
    if (secondChar == 'o') {
      certMangleType = CERT_MANGLE_CORRUPT;
      certIndexToCorrupt = (uint8_t)sl_cli_get_argument_uint8(arguments, 0);
    } else {
      certMangleType = CERT_MANGLE_VALUE;
      certIndexToChange  = (uint8_t)sl_cli_get_argument_uint8(arguments, 0);
      certNewByteValue   = (uint8_t)sl_cli_get_argument_uint8(arguments, 1);
    }
  } else if (commandChar == 's' ) {
    certMangleType = CERT_MANGLE_SUBJECT;
    emberCopyEui64Argument(0, invalidEui64);
  } else {
    testHarnessPrintln("Error: Unknown command.");
    return;
  }

  // Reset this value back to its normal value in case
  // it was set.
  resetTimeouts();
}

void emberAfPluginTestHarnessKeyEstablishmentSetModeCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t position = sl_cli_get_command_count(arguments) - 1;
  uint8_t commandChar0 = sl_cli_get_command_string(arguments, position)[0];
  uint8_t commandChar2   = sl_cli_get_command_string(arguments, position)[2];

  if (commandChar0 == 'o') {
    testHarnessMode = MODE_OUT_OF_SEQUENCE;
    respondToCommandWithOutOfSequence = (uint8_t)sl_cli_get_argument_uint8(arguments, 0);
  } else if (commandChar0 == 'n') {
    if (commandChar2 == '-') {
      testHarnessMode = MODE_NO_RESOURCES;
    } else if (commandChar2 == 'r') {
      testHarnessMode = MODE_NORMAL;
    } else if (commandChar2 == 'w') {
      testHarnessMode = MODE_NORMAL;
      emKeyEstablishmentPolicyAllowNewKeyEntries =
        (uint8_t)sl_cli_get_argument_uint8(arguments, 0);
    }
  } else if (commandChar0 == 't') {
    testHarnessMode = MODE_TIMEOUT;
  } else if (commandChar2 == 'l' ) {  // delay-cbke
    uint16_t delay = sl_cli_get_argument_uint16(arguments, 0);
    testHarnessMode = MODE_DELAY_CBKE;
    cbkeDelaySeconds = delay;
#ifdef SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT
    uint16_t advertisedDelay = sl_cli_get_argument_uint16(arguments, 1);
    emAfKeyEstablishmentTestHarnessGenerateKeyTime = delay;
    emAfKeyEstablishmentTestHarnessConfirmKeyTime  = advertisedDelay;
    emAfKeyEstablishmentTestHarnessAdvertisedGenerateKeyTime = advertisedDelay;
#endif // SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT
    return;
  } else if (commandChar2 == 'f') { // default-resp
    testHarnessMode = MODE_DEFAULT_RESPONSE;
    return;
  } else if (commandChar0 == 'r') {
    testHarnessMode = MODE_NORMAL;
    emAfTestHarnessResetApsFrameCounter();
  } else if (commandChar0 == 'a') {
    testHarnessMode = MODE_NORMAL;
    emAfTestHarnessAdvanceApsFrameCounter();
  } else {
    testHarnessPrintln("Error: Unknown command.");
    return;
  }

  resetTimeouts();
}

void emberAfPluginTestHarnessSetRegistrationCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t position = sl_cli_get_command_count(arguments) - 1;
  uint8_t commandChar1 = sl_cli_get_command_string(arguments, position)[1];
  if (commandChar1 == 'n') {
    emAfTestHarnessAllowRegistration = true;
  } else if (commandChar1 == 'f') {
    emAfTestHarnessAllowRegistration = false;
  } else {
    testHarnessPrintln("Error: Unknown command.");
  }
}

void emberAfPluginTestHarnessSetApsSecurityForClusterCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t position = sl_cli_get_command_count(arguments) - 1;
  uint8_t commandChar1 = sl_cli_get_command_string(arguments, position)[1];
  if (commandChar1 == 'n') {
    clusterIdRequiringApsSecurity = sl_cli_get_argument_uint16(arguments, 0);
  } else if (commandChar1 == 'f') {
    clusterIdRequiringApsSecurity = NULL_CLUSTER_ID;
  } else {
    testHarnessPrintln("Error: Unknown command.");
  }
}

void emberAfPluginTestHarnessKeyEstablishmentKeyMangleCommand(sl_cli_command_arg_t *arguments)
{
#if defined(SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT)
  testHarnessMode = MODE_KEY_MANGLE;
  keyLengthMod = sl_cli_get_argument_int8(arguments, 0);
#endif
}

void emberAfPluginTestHarnessKeyEstablishmentSelectSuiteCommand(sl_cli_command_arg_t *arguments)
{
#if defined(SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT)
  uint16_t suite = sl_cli_get_argument_uint16(arguments, 0);
  emAfSkipCheckSupportedCurves(suite);
#endif
}

void emberAfPluginTestHarnessKeyEstablishmentSetAvailableSuiteCommand(sl_cli_command_arg_t *arguments)
{
#if defined(SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT)
  uint16_t suite = sl_cli_get_argument_uint16(arguments, 0);
  emAfSetAvailableCurves(suite);
#endif
}

void emberAfPluginTestHarnessStatusCommand(sl_cli_command_arg_t *arguments)
{
  emberAfKeyEstablishmentClusterPrint("Test Harness Mode: %p", modeText[testHarnessMode]);
  if (testHarnessMode == MODE_CERT_MANGLE) {
    emberAfKeyEstablishmentClusterPrintln("");
    emberAfKeyEstablishmentClusterPrint("Cert Mangling Type: %p", certMangleText[certMangleType]);
    if (certMangleType == CERT_MANGLE_LENGTH) {
      emberAfKeyEstablishmentClusterPrint(" (%p%d bytes)",
                                          ((certLengthMod > 0)
                                           ? "+"
                                           : ""),
                                          certLengthMod);
    } else if (certMangleType == CERT_MANGLE_CORRUPT) {
      emberAfKeyEstablishmentClusterPrint(" (index: %d)",
                                          certIndexToCorrupt);
    }
  } else if (testHarnessMode == MODE_DELAY_CBKE) {
    emberAfKeyEstablishmentClusterPrint(" (by %d seconds",
                                        cbkeDelaySeconds);
  }
  emberAfKeyEstablishmentClusterPrintln("");

  emberAfKeyEstablishmentClusterPrintln("Auto SE Registration: %p",
                                        (emAfTestHarnessAllowRegistration
                                         ? "On"
                                         : "Off"));
  emberAfKeyEstablishmentClusterPrint("Additional Cluster Security: ");
  if (clusterIdRequiringApsSecurity == NULL_CLUSTER_ID) {
    emberAfKeyEstablishmentClusterPrintln("off");
  } else {
    emberAfKeyEstablishmentClusterPrintln("0x%2X",
                                          clusterIdRequiringApsSecurity);
  }

  emberAfKeyEstablishmentClusterPrintln("Publish Price includes SE 1.1 fields: %p",
                                        (emAfTestHarnessSupportForNewPriceFields
                                         ? "yes"
                                         : "no"));
  emberAfKeyEstablishmentClusterFlush();

#if defined(STACK_TEST_HARNESS)
  {
    uint8_t beaconsLeft = emTestHarnessBeaconSuppressGet();
    emberAfKeyEstablishmentClusterPrint("Beacon Suppress: %p",
                                        (beaconsLeft == 255
                                         ? "Disabled "
                                         : "Enabled "));
    if (beaconsLeft != 255) {
      emberAfKeyEstablishmentClusterPrint(" (%d left to be sent)",
                                          beaconsLeft);
    }
    emberAfKeyEstablishmentClusterFlush();
    emberAfKeyEstablishmentClusterPrintln("");
  }
#endif

#if defined(SL_CATALOG_ZIGBEE_NETWORK_FIND_PRESENT)
  {
    emberAfKeyEstablishmentClusterPrint("Channel Mask: ");
    emberAfPrintChannelListFromMask(testHarnessChannelMask);
    emberAfKeyEstablishmentClusterPrintln("");
  }
#endif
}

bool emberAfClusterSecurityCustomCallback(EmberAfProfileId profileId,
                                          EmberAfClusterId clusterId,
                                          bool incoming,
                                          uint8_t commandId)
{
  return (clusterIdRequiringApsSecurity != NULL_CLUSTER_ID
          && clusterId == clusterIdRequiringApsSecurity);
}

void emberAfPluginTestHarnessPriceSendNewFieldsCommand(sl_cli_command_arg_t *arguments)
{
#if defined(SL_CATALOG_ZIGBEE_PRICE_SERVER_PRESENT)
  emAfTestHarnessSupportForNewPriceFields = (bool)emberUnsignedCommandArgument(0);
#else
  testHarnessPrintln("No Price server plugin included.");
#endif
}

void emberAfPluginTestHarnessTcKeepaliveSendCommand(sl_cli_command_arg_t *arguments)
{
#if defined(SL_CATALOG_ZIGBEE_TRUST_CENTER_KEEPALIVE_PRESENT)
  emAfSendKeepaliveSignal();
#else
  testHarnessPrintln("No TC keepalive plugin included.");
#endif
}

void emberAfPluginTestHarnessTcKeepaliveStartStopCommand(sl_cli_command_arg_t *arguments)
{
#if defined(SL_CATALOG_ZIGBEE_TRUST_CENTER_KEEPALIVE_PRESENT)

  uint8_t position = sl_cli_get_command_count(arguments) - 1;
  uint8_t commandChar2 = sl_cli_get_command_string(arguments, position)[2];
  if (commandChar2 == 'o') {  // stop
    emberAfTrustCenterKeepaliveAbortCallback();
  } else if (commandChar2 == 'a') { // start
    emberAfTrustCenterKeepaliveUpdateCallback(true); // registration complete?
                                                     // assume this is only called when device is done with that
  } else {
    testHarnessPrintln("Unknown keepalive command.");
  }

#else
  testHarnessPrintln("No TC keepalive plugin included.");
#endif
}

void emberAfPluginTestHarnessOtaImageMangleCommand(sl_cli_command_arg_t *arguments)
{
#if defined (SL_CATALOG_ZIGBEE_OTA_STORAGE_SIMPLE_RAM_PRESENT)
  uint16_t index = sl_cli_get_argument_uint16(arguments, 0);
  if (index >= emAfOtaStorageDriveGetImageSize()) {
    testHarnessPrintln("Error: Index %d > image size of %d",
                       index,
                       emAfOtaStorageDriveGetImageSize());
  } else {
    emAfOtaStorageDriverCorruptImage(index);
  }
#else
  testHarnessPrintln("No OTA Storage Simple RAM plugin included");
#endif
}

#if defined(SL_CATALOG_ZIGBEE_TRUST_CENTER_NWK_KEY_UPDATE_PERIODIC_PRESENT)
void emberAfPluginTestHarnessKeyUpdateCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t position = sl_cli_get_command_count(arguments) - 1;
  uint8_t commandChar0 = sl_cli_get_command_string(arguments, position)[0];

  if (commandChar0 == 'u') {
    unicastKeyUpdate = true;
  } else if (commandChar0 == 'b') {
    unicastKeyUpdate = false;
  } else if (commandChar0 == 'n') {
    emberAfPluginTrustCenterNwkKeyUpdatePeriodicMyEventHandler(&emberAfPluginTrustCenterNwkKeyUpdatePeriodicMyEvent);
  }
}

#else

void emberAfPluginTestHarnessKeyUpdateCommand(sl_cli_command_arg_t *arguments)
{
  testHarnessPrintln("NWK Key Update Plugin not enabled.");
}

#endif // SL_CATALOG_ZIGBEE_TRUST_CENTER_NWK_KEY_UPDATE_PERIODIC_PRESENT

void emberAfPluginTestHarnessConcentratorStartStopCommand(sl_cli_command_arg_t *arguments)
{
#if defined(SL_CATALOG_ZIGBEE_CONCENTRATOR_PRESENT)
  uint8_t position = sl_cli_get_command_count(arguments) - 1;
  uint8_t commandChar2 = sl_cli_get_command_string(arguments, position)[2];
  if (commandChar2 == 'o') {
    emberSetSourceRouteDiscoveryMode(EMBER_SOURCE_ROUTE_DISCOVERY_OFF);
  } else if (commandChar2 == 'a') {
    emberSetSourceRouteDiscoveryMode(EMBER_SOURCE_ROUTE_DISCOVERY_ON);
  } else {
    testHarnessPrintln("Error: Unknown command.");
  }
#endif // SL_CATALOG_ZIGBEE_CONCENTRATOR_PRESENT
}

#if defined(STACK_TEST_HARNESS)
void emberAfPluginTestHarnessLimitBeaconsOnOffCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t position = sl_cli_get_command_count(arguments) - 1;
  uint8_t commandChar1 = sl_cli_get_command_string(arguments, position)[1];

  if (commandChar1 == 'f') {
    emTestHarnessBeaconSuppressSet(255);
  } else if (commandChar1 == 'n') {
    emTestHarnessBeaconSuppressSet(1);
  } else {
    testHarnessPrintln("Error: Unknown command.");
  }
}
// TODO: this should be modified once we've upgraded the generated CLI
// mechanism to conditionally generate CLI based on macros such as these
#elif defined(EMBER_AF_GENERATE_CLI)
void emberAfPluginTestHarnessLimitBeaconsOnOffCommand(sl_cli_command_arg_t *arguments)
{
}
#endif

#if defined(SL_CATALOG_ZIGBEE_NETWORK_FIND_PRESENT)

void emberAfPluginTestHarnessChannelMaskAddOrRemoveCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t position = sl_cli_get_command_count(arguments) - 1;
  uint8_t commandChar0 = sl_cli_get_command_string(arguments, position)[0];
  uint8_t channel = sl_cli_get_argument_uint8(arguments, 0);

  if (channel < 11 || channel > 26) {
    testHarnessPrintln("Error: Invalid channel '%d'.", channel);
    return;
  }
  if (commandChar0 == 'a') {
    testHarnessChannelMask |= (1 << channel);
  } else if (commandChar0 == 'r') {
    testHarnessChannelMask &= ~(1 << channel);
  } else {
    testHarnessPrintln("Error: Unknown command.");
  }
}

void emberAfPluginTestHarnessChannelMaskResetClearAllCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t position = sl_cli_get_command_count(arguments) - 1;
  uint8_t commandChar0 = sl_cli_get_command_string(arguments, position)[0];

  if (commandChar0 == 'c') {
    testHarnessChannelMask = 0;
  } else if (commandChar0 == 'r') {
    testHarnessChannelMask = testHarnessOriginalChannelMask;
  } else if (commandChar0 == 'a') {
    testHarnessChannelMask = EMBER_ALL_802_15_4_CHANNELS_MASK;
  } else {
    testHarnessPrintln("Error: Unknown command.");
  }
}

#endif

void emberAfPluginTestHarnessEnableDisableEndpointCommand(sl_cli_command_arg_t *arguments)
{
#if defined(EZSP_HOST)
  uint8_t endpoint = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t position = sl_cli_get_command_count(arguments) - 1;
  uint8_t commandChar0 = sl_cli_get_command_string(arguments, position)[0];
  bool disable = commandChar0 == 'd';

  ezspSetEndpointFlags(endpoint,
                       (disable
                        ? EZSP_ENDPOINT_DISABLED
                        : EZSP_ENDPOINT_ENABLED));
#else

  testHarnessPrintln("Unsupported on SOC.");

#endif
}

void emberAfPluginTestHarnessEndpointStatusCommand(sl_cli_command_arg_t *arguments)
{
#if defined(EZSP_HOST)
  uint8_t i;

  for (i = 0; i < emberAfEndpointCount(); i++) {
    uint8_t endpoint = emberAfEndpointFromIndex(i);
    EzspEndpointFlags flags;
    ezspGetEndpointFlags(endpoint, &flags);
    testHarnessPrintln("EP %d, Flags 0x%2X [%s]",
                       endpoint,
                       flags,
                       ((flags & EZSP_ENDPOINT_ENABLED)
                        ? "Enabled"
                        : "Disabled"));
#ifdef SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT
    if (delayedCbkeOperation == CBKE_OPERATION_GENERATE_KEYS) {
      emAfPluginKeyEstablishmentGenerateCbkeKeysHandler(EMBER_SUCCESS,
                                                        (EmberPublicKeyData*)delayedData);
    } else if (delayedCbkeOperation == CBKE_OPERATION_GENERATE_KEYS_283K1) {
      emAfPluginKeyEstablishmentGenerateCbkeKeysHandler283k1(EMBER_SUCCESS,
                                                             (EmberPublicKey283k1Data*)delayedData);
    } else if (delayedCbkeOperation == CBKE_OPERATION_GENERATE_SECRET) {
      emAfPluginKeyEstablishmentCalculateSmacsHandler(EMBER_SUCCESS,
                                                      (EmberSmacData*)delayedData,
                                                      (EmberSmacData*)(delayedData + EMBER_SMAC_SIZE));
    } else if (delayedCbkeOperation == CBKE_OPERATION_GENERATE_SECRET_283K1) {
      emAfPluginKeyEstablishmentCalculateSmacsHandler283k1(EMBER_SUCCESS,
                                                           (EmberSmacData*)delayedData,
                                                           (EmberSmacData*)(delayedData + EMBER_SMAC_SIZE));
    } else {
      testHarnessPrintln("Test Harness Event Handler: Unknown operation 0x%4X", delayedCbkeOperation);
    }
#endif // SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT
  }

#else

  testHarnessPrintln("Unsupported on SOC");

#endif
}

void emberAfPluginTestHarnessClusterEndpointIndexCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint, mask, clusterEndpointIndex;
  EmberAfClusterId clusterId;

  endpoint = sl_cli_get_argument_uint8(arguments, 0);
  clusterId = (EmberAfClusterId)sl_cli_get_argument_uint16(arguments, 1);
  mask = sl_cli_get_argument_uint8(arguments, 2);

  clusterEndpointIndex = (uint8_t)(mask == 0
                                   ? emberAfFindClusterClientEndpointIndex(endpoint, clusterId)
                                   : emberAfFindClusterServerEndpointIndex(endpoint, clusterId));

  testHarnessPrintln("endpoint: 0x%2x cluster: 0x%2x clusterEndpointIndex: 0x%2x %s",
                     endpoint,
                     clusterId,
                     clusterEndpointIndex,
                     ((mask == 0)
                      ? "(client)"
                      : "(server)"));
}

void emberAfPluginTestHarnessRadioOnOffCommand(sl_cli_command_arg_t *arguments)
{
  uint8_t position = sl_cli_get_command_count(arguments) - 1;
  uint8_t commandChar1 = sl_cli_get_command_string(arguments, position)[1];
  bool radioOff = (commandChar1 == 'f');
  EmberStatus status;
  if (radioOff) {
    status = emberStartScan(EMBER_START_RADIO_OFF_SCAN,
                            0,   // channels (doesn't matter)
                            0);  // duration (doesn't matter)

#ifdef SL_CATALOG_ZIGBEE_LIGHT_LINK_PRESENT
    if (emberZllRxOnWhenIdleGetActive()) {
      // Ensure radio doesn't get turned back on while it's supposed to be off.
      emberZllCancelRxOnWhenIdle();
    }
#endif
  } else {
    status = emberStopScan();
  }
  emberAfCorePrintln("Radio %s status: 0x%X",
                     (radioOff ? "OFF" : "ON"),
                     status);
}

void emberAfPluginTestHarnessSetRadioPower(sl_cli_command_arg_t *arguments)
{
  int8_t val = (int8_t)sl_cli_get_argument_int8(arguments, 0);
  emberSetRadioPower(val);
  emberAfCorePrintln("radio power %d", val);
}

void emberAfPluginTestHarnessAddChildCommand(sl_cli_command_arg_t *arguments)
{
#if !defined(EZSP_HOST)
  EmberNodeId shortId;
  EmberEUI64 longId;
  uint8_t nodeType;
  EmberStatus status;
  shortId = sl_cli_get_argument_uint16(arguments, 0);
  sl_zigbee_copy_eui64_arg(arguments, 1, longId, false);
  nodeType = sl_cli_get_argument_uint16(arguments, 2);

  status = emberAddChild(shortId, longId, nodeType);
  emberAfCorePrintln("status 0x%x", status);
#endif
}

#elif !defined(UC_BUILD)

void emberAfPluginTestHarnessCertMangleCommand(void)
{
  uint8_t commandChar = emberCurrentCommand->name[0];
  uint8_t secondChar  = emberCurrentCommand->name[1];
  testHarnessMode = MODE_CERT_MANGLE;

  if (commandChar == 'l') {
    certMangleType = CERT_MANGLE_LENGTH;
    certLengthMod = (int8_t)emberSignedCommandArgument(0);
  } else if (commandChar == 'i') {
    certMangleType = CERT_MANGLE_ISSUER;
    emberCopyEui64Argument(0, invalidEui64);
  } else if (commandChar == 'c' ) {
    if (secondChar == 'o') {
      certMangleType = CERT_MANGLE_CORRUPT;
      certIndexToCorrupt = (uint8_t)emberUnsignedCommandArgument(0);
    } else {
      certMangleType = CERT_MANGLE_VALUE;
      certIndexToChange  = (uint8_t)emberUnsignedCommandArgument(0);
      certNewByteValue   = (uint8_t)emberUnsignedCommandArgument(1);
    }
  } else if (commandChar == 's' ) {
    certMangleType = CERT_MANGLE_SUBJECT;
    emberCopyEui64Argument(0, invalidEui64);
  } else {
    testHarnessPrintln("Error: Unknown command.");
    return;
  }

  // Reset this value back to its normal value in case
  // it was set.
  resetTimeouts();
}

void emberAfPluginTestHarnessKeyEstablishmentSetModeCommand(void)
{
  uint8_t commandChar0 = emberCurrentCommand->name[0];
  uint8_t commandChar2 = emberCurrentCommand->name[2];

  if (commandChar0 == 'o') {
    testHarnessMode = MODE_OUT_OF_SEQUENCE;
    respondToCommandWithOutOfSequence = (uint8_t)emberUnsignedCommandArgument(0);
  } else if (commandChar0 == 'n') {
    if (commandChar2 == '-') {
      testHarnessMode = MODE_NO_RESOURCES;
    } else if (commandChar2 == 'r') {
      testHarnessMode = MODE_NORMAL;
    } else if (commandChar2 == 'w') {
      testHarnessMode = MODE_NORMAL;
      emKeyEstablishmentPolicyAllowNewKeyEntries =
        (uint8_t)emberUnsignedCommandArgument(0);
    }
  } else if (commandChar0 == 't') {
    testHarnessMode = MODE_TIMEOUT;
  } else if (commandChar2 == 'l' ) {  // delay-cbke
    uint16_t delay = (uint16_t)emberUnsignedCommandArgument(0);
    testHarnessMode = MODE_DELAY_CBKE;
    cbkeDelaySeconds = delay;
#ifdef SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT
    uint16_t advertisedDelay = (uint16_t)emberUnsignedCommandArgument(1);
    emAfKeyEstablishmentTestHarnessGenerateKeyTime = delay;
    emAfKeyEstablishmentTestHarnessConfirmKeyTime  = advertisedDelay;
    emAfKeyEstablishmentTestHarnessAdvertisedGenerateKeyTime = advertisedDelay;
#endif // SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT
    return;
  } else if (commandChar2 == 'f') { // default-resp
    testHarnessMode = MODE_DEFAULT_RESPONSE;
    return;
  } else if (commandChar0 == 'r') {
    testHarnessMode = MODE_NORMAL;
    emAfTestHarnessResetApsFrameCounter();
  } else if (commandChar0 == 'a') {
    testHarnessMode = MODE_NORMAL;
    emAfTestHarnessAdvanceApsFrameCounter();
  } else {
    testHarnessPrintln("Error: Unknown command.");
    return;
  }

  resetTimeouts();
}

void emberAfPluginTestHarnessSetRegistrationCommand(void)
{
  uint8_t commandChar1 = emberCurrentCommand->name[1];
  if (commandChar1 == 'n') {
    emAfTestHarnessAllowRegistration = true;
  } else if (commandChar1 == 'f') {
    emAfTestHarnessAllowRegistration = false;
  } else {
    testHarnessPrintln("Error: Unknown command.");
  }
}

void emberAfPluginTestHarnessSetApsSecurityForClusterCommand(void)
{
  uint8_t commandChar1 = emberCurrentCommand->name[1];
  if (commandChar1 == 'n') {
    clusterIdRequiringApsSecurity = (uint16_t)emberUnsignedCommandArgument(0);
  } else if (commandChar1 == 'f') {
    clusterIdRequiringApsSecurity = NULL_CLUSTER_ID;
  } else {
    testHarnessPrintln("Error: Unknown command.");
  }
}

void emberAfPluginTestHarnessKeyEstablishmentKeyMangleCommand(void)
{
#if defined(SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT)
  testHarnessMode = MODE_KEY_MANGLE;
  keyLengthMod = (int8_t)emberSignedCommandArgument(0);
#endif
}

void emberAfPluginTestHarnessKeyEstablishmentSelectSuiteCommand(void)
{
#if defined(SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT)
  uint16_t suite = (uint16_t)emberUnsignedCommandArgument(0);
  emAfSkipCheckSupportedCurves(suite);
#endif
}

void emberAfPluginTestHarnessKeyEstablishmentSetAvailableSuiteCommand(void)
{
#if defined(SL_CATALOG_ZIGBEE_KEY_ESTABLISHMENT_PRESENT)
  uint16_t suite = (uint16_t)emberUnsignedCommandArgument(0);
  emAfSetAvailableCurves(suite);
#endif
}

void emberAfPluginTestHarnessStatusCommand(void)
{
  emberAfKeyEstablishmentClusterPrint("Test Harness Mode: %p", modeText[testHarnessMode]);
  if (testHarnessMode == MODE_CERT_MANGLE) {
    emberAfKeyEstablishmentClusterPrintln("");
    emberAfKeyEstablishmentClusterPrint("Cert Mangling Type: %p", certMangleText[certMangleType]);
    if (certMangleType == CERT_MANGLE_LENGTH) {
      emberAfKeyEstablishmentClusterPrint(" (%p%d bytes)",
                                          ((certLengthMod > 0)
                                           ? "+"
                                           : ""),
                                          certLengthMod);
    } else if (certMangleType == CERT_MANGLE_CORRUPT) {
      emberAfKeyEstablishmentClusterPrint(" (index: %d)",
                                          certIndexToCorrupt);
    }
  } else if (testHarnessMode == MODE_DELAY_CBKE) {
    emberAfKeyEstablishmentClusterPrint(" (by %d seconds",
                                        cbkeDelaySeconds);
  }
  emberAfKeyEstablishmentClusterPrintln("");

  emberAfKeyEstablishmentClusterPrintln("Auto SE Registration: %p",
                                        (emAfTestHarnessAllowRegistration
                                         ? "On"
                                         : "Off"));
  emberAfKeyEstablishmentClusterPrint("Additional Cluster Security: ");
  if (clusterIdRequiringApsSecurity == NULL_CLUSTER_ID) {
    emberAfKeyEstablishmentClusterPrintln("off");
  } else {
    emberAfKeyEstablishmentClusterPrintln("0x%2X",
                                          clusterIdRequiringApsSecurity);
  }

  emberAfKeyEstablishmentClusterPrintln("Publish Price includes SE 1.1 fields: %p",
                                        (emAfTestHarnessSupportForNewPriceFields
                                         ? "yes"
                                         : "no"));
  emberAfKeyEstablishmentClusterFlush();

#if defined(STACK_TEST_HARNESS)
  {
    uint8_t beaconsLeft = emTestHarnessBeaconSuppressGet();
    emberAfKeyEstablishmentClusterPrint("Beacon Suppress: %p",
                                        (beaconsLeft == 255
                                         ? "Disabled "
                                         : "Enabled "));
    if (beaconsLeft != 255) {
      emberAfKeyEstablishmentClusterPrint(" (%d left to be sent)",
                                          beaconsLeft);
    }
    emberAfKeyEstablishmentClusterFlush();
    emberAfKeyEstablishmentClusterPrintln("");
  }
#endif

#if defined(SL_CATALOG_ZIGBEE_NETWORK_FIND_PRESENT)
  {
    emberAfKeyEstablishmentClusterPrint("Channel Mask: ");
    emberAfPrintChannelListFromMask(testHarnessChannelMask);
    emberAfKeyEstablishmentClusterPrintln("");
  }
#endif
}

bool emberAfClusterSecurityCustomCallback(EmberAfProfileId profileId,
                                          EmberAfClusterId clusterId,
                                          bool incoming,
                                          uint8_t commandId)
{
  return (clusterIdRequiringApsSecurity != NULL_CLUSTER_ID
          && clusterId == clusterIdRequiringApsSecurity);
}

void emberAfPluginTestHarnessPriceSendNewFieldsCommand(void)
{
#if defined(SL_CATALOG_ZIGBEE_PRICE_SERVER_PRESENT)
  emAfTestHarnessSupportForNewPriceFields = (bool)emberUnsignedCommandArgument(0);
#else
  testHarnessPrintln("No Price server plugin included.");
#endif
}

void emberAfPluginTestHarnessTcKeepaliveSendCommand(void)
{
#if defined(SL_CATALOG_ZIGBEE_TRUST_CENTER_KEEPALIVE_PRESENT)
  emAfSendKeepaliveSignal();
#else
  testHarnessPrintln("No TC keepalive plugin included.");
#endif
}

void emberAfPluginTestHarnessTcKeepaliveStartStopCommand(void)
{
#if defined(SL_CATALOG_ZIGBEE_TRUST_CENTER_KEEPALIVE_PRESENT)

  uint8_t commandChar2 = emberCurrentCommand->name[2];
  if (commandChar2 == 'o') {  // stop
    emberAfTrustCenterKeepaliveAbortCallback();
  } else if (commandChar2 == 'a') { // start
    emberAfTrustCenterKeepaliveUpdateCallback(true); // registration complete?
                                                     // assume this is only called when device is done with that
  } else {
    testHarnessPrintln("Unknown keepalive command.");
  }

#else
  testHarnessPrintln("No TC keepalive plugin included.");
#endif
}

void emberAfPluginTestHarnessOtaImageMangleCommand(void)
{
#if defined (SL_CATALOG_ZIGBEE_OTA_STORAGE_SIMPLE_RAM_PRESENT)
  uint16_t index = (uint16_t)emberUnsignedCommandArgument(0);
  if (index >= emAfOtaStorageDriveGetImageSize()) {
    testHarnessPrintln("Error: Index %d > image size of %d",
                       index,
                       emAfOtaStorageDriveGetImageSize());
  } else {
    emAfOtaStorageDriverCorruptImage(index);
  }
#else
  testHarnessPrintln("No OTA Storage Simple RAM plugin included");
#endif
}

#if defined(SL_CATALOG_ZIGBEE_TRUST_CENTER_NWK_KEY_UPDATE_PERIODIC_PRESENT)
void emberAfPluginTestHarnessKeyUpdateCommand(void)
{
  uint8_t commandChar0 = emberCurrentCommand->name[0];

  if (commandChar0 == 'u') {
    unicastKeyUpdate = true;
  } else if (commandChar0 == 'b') {
    unicastKeyUpdate = false;
  } else if (commandChar0 == 'n') {
    emberAfPluginTrustCenterNwkKeyUpdatePeriodicMyEventHandler();
  }
}

#else

void emberAfPluginTestHarnessKeyUpdateCommand(void)
{
  testHarnessPrintln("NWK Key Update Plugin not enabled.");
}

#endif // SL_CATALOG_ZIGBEE_TRUST_CENTER_NWK_KEY_UPDATE_PERIODIC_PRESENT

void emberAfPluginTestHarnessConcentratorStartStopCommand(void)
{
#if defined(SL_CATALOG_ZIGBEE_CONCENTRATOR_PRESENT)
  uint8_t commandChar2 = emberCurrentCommand->name[2];
  if (commandChar2 == 'o') {
    emberSetSourceRouteDiscoveryMode(EMBER_SOURCE_ROUTE_DISCOVERY_OFF);
  } else if (commandChar2 == 'a') {
    emberSetSourceRouteDiscoveryMode(EMBER_SOURCE_ROUTE_DISCOVERY_ON);
  } else {
    testHarnessPrintln("Error: Unknown command.");
  }
#endif // SL_CATALOG_ZIGBEE_CONCENTRATOR_PRESENT
}

#if defined(STACK_TEST_HARNESS)
void emberAfPluginTestHarnessLimitBeaconsOnOffCommand(void)
{
  uint8_t commandChar1 = emberCurrentCommand->name[1];
  if (commandChar1 == 'f') {
    emTestHarnessBeaconSuppressSet(255);
  } else if (commandChar1 == 'n') {
    emTestHarnessBeaconSuppressSet(1);
  } else {
    testHarnessPrintln("Error: Unknown command.");
  }
}
// TODO: this should be modified once we've upgraded the generated CLI
// mechanism to conditionally generate CLI based on macros such as these
#elif defined(EMBER_AF_GENERATE_CLI)
void emberAfPluginTestHarnessLimitBeaconsOnOffCommand(void)
{
}
#endif

#if defined(SL_CATALOG_ZIGBEE_NETWORK_FIND_PRESENT)

void emberAfPluginTestHarnessChannelMaskAddOrRemoveCommand(void)
{
  uint8_t commandChar0 = emberCurrentCommand->name[0];
  uint8_t channel = (uint8_t)emberUnsignedCommandArgument(0);

  if (channel < 11 || channel > 26) {
    testHarnessPrintln("Error: Invalid channel '%d'.", channel);
    return;
  }
  if (commandChar0 == 'a') {
    testHarnessChannelMask |= (1 << channel);
  } else if (commandChar0 == 'r') {
    testHarnessChannelMask &= ~(1 << channel);
  } else {
    testHarnessPrintln("Error: Unknown command.");
  }
}

void emberAfPluginTestHarnessChannelMaskResetClearAllCommand(void)
{
  uint8_t commandChar0 = emberCurrentCommand->name[0];

  if (commandChar0 == 'c') {
    testHarnessChannelMask = 0;
  } else if (commandChar0 == 'r') {
    testHarnessChannelMask = testHarnessOriginalChannelMask;
  } else if (commandChar0 == 'a') {
    testHarnessChannelMask = EMBER_ALL_802_15_4_CHANNELS_MASK;
  } else {
    testHarnessPrintln("Error: Unknown command.");
  }
}

#endif

void emberAfPluginTestHarnessEnableDisableEndpointCommand(void)
{
#if defined(EZSP_HOST)
  uint8_t endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  uint8_t commandChar0 = emberCurrentCommand->name[0];
  bool disable = commandChar0 == 'd';

  ezspSetEndpointFlags(endpoint,
                       (disable
                        ? EZSP_ENDPOINT_DISABLED
                        : EZSP_ENDPOINT_ENABLED));
#else

  testHarnessPrintln("Unsupported on SOC.");

#endif
}

void emberAfPluginTestHarnessEndpointStatusCommand(void)
{
#if defined(EZSP_HOST)
  uint8_t i;

  for (i = 0; i < emberAfEndpointCount(); i++) {
    uint8_t endpoint = emberAfEndpointFromIndex(i);
    EzspEndpointFlags flags;
    ezspGetEndpointFlags(endpoint, &flags);
    testHarnessPrintln("EP %d, Flags 0x%2X [%p]",
                       endpoint,
                       flags,
                       ((flags & EZSP_ENDPOINT_ENABLED)
                        ? "Enabled"
                        : "Disabled"));
  }

#else

  testHarnessPrintln("Unsupported on SOC");

#endif
}

void emberAfPluginTestHarnessClusterEndpointIndexCommand(void)
{
  uint8_t endpoint, mask, clusterEndpointIndex;
  EmberAfClusterId clusterId;

  endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  clusterId = (EmberAfClusterId)emberUnsignedCommandArgument(1);
  mask = (uint8_t)emberUnsignedCommandArgument(2);

  clusterEndpointIndex = (uint8_t)(mask == 0
                                   ? emberAfFindClusterClientEndpointIndex(endpoint, clusterId)
                                   : emberAfFindClusterServerEndpointIndex(endpoint, clusterId));

  testHarnessPrintln("endpoint: 0x%2x cluster: 0x%2x clusterEndpointIndex: 0x%2x %p",
                     endpoint,
                     clusterId,
                     clusterEndpointIndex,
                     ((mask == 0)
                      ? "(client)"
                      : "(server)"));
}

void emberAfPluginTestHarnessRadioOnOffCommand(void)
{
  bool radioOff = (emberCurrentCommand->name[1] == 'f');
  EmberStatus status;
  if (radioOff) {
    status = emberStartScan(EMBER_START_RADIO_OFF_SCAN,
                            0,   // channels (doesn't matter)
                            0);  // duration (doesn't matter)

#ifdef SL_CATALOG_ZIGBEE_LIGHT_LINK_PRESENT
    if (emberZllRxOnWhenIdleGetActive()) {
      // Ensure radio doesn't get turned back on while it's supposed to be off.
      emberZllCancelRxOnWhenIdle();
    }
#endif
  } else {
    status = emberStopScan();
  }
  emberAfCorePrintln("Radio %p status: 0x%X",
                     (radioOff ? "OFF" : "ON"),
                     status);
}

void emberAfPluginTestHarnessSetRadioPower(void)
{
  int8_t val = (int8_t)emberUnsignedCommandArgument(0);
  emberSetRadioPower(val);
  emberAfCorePrintln("radio power %d", val);
}

void emberAfPluginTestHarnessAddChildCommand(void)
{
#if !defined(EZSP_HOST)
  EmberNodeId shortId;
  EmberEUI64 longId;
  uint8_t nodeType;
  EmberStatus status;
  shortId = emberUnsignedCommandArgument(0);
  emberCopyEui64Argument(1, longId);
  nodeType = emberUnsignedCommandArgument(2);

  status = emberAddChild(shortId, longId, nodeType);
  emberAfCorePrintln("status 0x%x", status);
#endif
}

#endif //UC_BUILD

#if (!defined(UC_BUILD) || defined(SL_CATALOG_CLI_PRESENT))
void emberAfPluginTestHarnessSetNodeDescriptorComplianceRevision(SL_CLI_COMMAND_ARG)
{
  uint8_t val = (uint8_t)emberUnsignedCommandArgument(0);
#ifdef EZSP_HOST
  EzspStatus status;
  status = ezspSetValue(EZSP_VALUE_ENABLE_R21_BEHAVIOR, 1, &val);
  if (status == EZSP_SUCCESS) {
    emberAfCorePrintln("The compliance revision of the device has been changed to R%d (0x%X)", val, status);
  }
#elif defined(EMBER_TEST)
  emTestStackComplianceRevision = val;
  emberAfCorePrintln("The compliance revision of the device has been changed to R%d", val);
#else
  (void)val;
#endif
}

void emberAfPluginTestHarnessSetMaxChildren(SL_CLI_COMMAND_ARG)
{
  uint8_t maxChildren = (uint8_t)emberUnsignedCommandArgument(0);
#ifdef EZSP_HOST
  EzspStatus status;
  status = emberAfSetEzspConfigValue(EZSP_CONFIG_MAX_END_DEVICE_CHILDREN,
                                     maxChildren,
                                     "max end device children");
  if (status != EZSP_SUCCESS) {
    return;
  }
#else
  emMaxEndDeviceChildren = maxChildren;
#endif
  emberAfCorePrintln("Set maximum children to %d", maxChildren);
}

void emberAfPluginTestHarnessSetNeighborTableSize(SL_CLI_COMMAND_ARG)
{
  // CAUTION : This function does not cause memory to be allocated toward the
  // neighbor table, therefore, the set value needs to be under the compile
  // time size (1,16,26 - depending on EMBER_NEIGHBOR_TABLE_SIZE)
  uint8_t neighborTableSize = (uint8_t)emberUnsignedCommandArgument(0);
#ifdef EZSP_HOST
  EzspStatus status;
  status = emberAfSetEzspConfigValue(EZSP_CONFIG_NEIGHBOR_TABLE_SIZE,
                                     neighborTableSize,
                                     "neighbor table size");
  if (status != EZSP_SUCCESS) {
    return;
  }
#else
  emRouterNeighborTableSize = neighborTableSize;
#endif
  emberAfCorePrintln("Set neighbor table size to %d", neighborTableSize);
}
#endif
