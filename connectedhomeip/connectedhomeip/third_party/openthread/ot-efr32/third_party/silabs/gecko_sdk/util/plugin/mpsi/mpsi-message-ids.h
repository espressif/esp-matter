/**************************************************************************//**
 * Copyright 2017 Silicon Laboratories, Inc.
 *
 *****************************************************************************/

#ifndef MPSI_MESSAGE_IDS_H
#define MPSI_MESSAGE_IDS_H

// Application IDs
#define MPSI_APP_ID_NONE        0x00
#define MPSI_APP_ID_MOBILE_APP  0x01
#define MPSI_APP_ID_ZIGBEE      0x02
#define MPSI_APP_ID_THREAD      0x03
#define MPSI_APP_ID_CONNECT     0x04
#define MPSI_APP_ID_BLE         0x05
#define MPSI_APP_ID_MCU         0x06
#define MPSI_APP_ID_ANY         0xFF

// Define the local application ID
#if defined (EMBER_STACK_BLE)

#if defined(BLE_NCP_MOBILE_APP)
 #define MPSI_APP_ID    MPSI_APP_ID_MOBILE_APP
#else // BLE_NCP_MOBILE_APP
 #define MPSI_APP_ID    MPSI_APP_ID_BLE
#endif // BLE_NCP_MOBILE_APP

#elif defined (EMBER_STACK_ZIGBEE)
 #define MPSI_APP_ID    MPSI_APP_ID_ZIGBEE

#elif defined (EMBER_STACK_IP)
 #define MPSI_APP_ID    MPSI_APP_ID_THREAD

#else // EMBER_STACK_BLE || EMBER_STACK_ZIGBEE || EMBER_STACK_IP
#error "MPSI application ID is undefined"
#endif // EMBER_STACK_BLE || EMBER_STACK_ZIGBEE || EMBER_STACK_IP

// Pre-defined messages
#define MPSI_MESSAGE_ID_GET_APPS_INFO                       0
#define MPSI_MESSAGE_ID_APPS_INFO                           1
#define MPSI_MESSAGE_ID_BOOTLOAD_SLOT                       2
#define MPSI_MESSAGE_ID_ERROR                               3
#define MPSI_MESSAGE_ID_INITIATE_JOINING                    4
#define MPSI_MESSAGE_ID_GET_ZIGBEE_JOINING_DEVICE_INFO      5
#define MPSI_MESSAGE_ID_ZIGBEE_JOINING_DEVICE_INFO          6
#define MPSI_MESSAGE_ID_SET_ZIGBEE_JOINING_DEVICE_INFO      7
#define MPSI_MESSAGE_ID_GET_ZIGBEE_TC_JOINING_CREDENTIALS   8
#define MPSI_MESSAGE_ID_ZIGBEE_TC_JOINING_CREDENTIALS       9
#define MPSI_MESSAGE_ID_SET_ZIGBEE_TC_JOINING_CREDENTIALS   10
#define MPSI_MESSAGE_ID_GET_THREAD_JOINING_DEVICE_INFO      11
#define MPSI_MESSAGE_ID_THREAD_JOINING_DEVICE_INFO          12
#define MPSI_MESSAGE_ID_SET_THREAD_JOINING_DEVICE_INFO      13

// First release supports until MPSI_MESSAGE_ID_SET_ZIGBEE_JOINING_DEVICE_INFO
// Second release supports until
// MPSI_MESSAGE_ID_SET_ZIGBEE_TC_JOINING_CREDENTIALS
// NOTE: any change made here must be made in mpsi-metadata.bin as well!
#define MPSI_MESSAGE_ID_MAX_ID \
  MPSI_MESSAGE_ID_SET_ZIGBEE_TC_JOINING_CREDENTIALS

// The very first MPSI version understood up until (and including) this message
#define MPSI_INITIAL_MAX_MESSAGE_ID_SUPPORTED \
  MPSI_MESSAGE_ID_SET_ZIGBEE_JOINING_DEVICE_INFO

#define INVALID_SLOT                                (uint8_t)-1
#define APPLICATION_VERSION_DIGITS                  4
#define EUI64_SIZE                                  8
#define MIN_INSTALL_CODE_LENGTH                     8   // includes 2 byte CRC
#define MAX_INSTALL_CODE_LENGTH                     18  // includes 2 byte CRC
#define MIN_PSKD_LENGTH                             6
#define MAX_PSKD_LENGTH                             32
#define ZIGBEE_EXT_PAN_LENGTH                       8
#define ZIGBEE_KEY_LENGTH                           16

#define MPSI_CUSTOM_MESSAGE_BIT                     0x8000
#define MPSI_MAX_PAYLOAD_LENGTH                     0xFF
#define MPSI_MESSAGE_OVERHEAD                       (4)
#define MPSI_MAX_MESSAGE_LENGTH \
  ((uint16_t)(MPSI_MAX_PAYLOAD_LENGTH + MPSI_MESSAGE_OVERHEAD))

// MPSI message struct
typedef struct {
  uint8_t   destinationAppId;
  uint16_t  messageId;
  uint8_t   payloadLength;
  uint8_t   payload[MPSI_MAX_PAYLOAD_LENGTH];
} MpsiMessage_t;

// Message payload structs

// Get Apps Info message has no payload

typedef struct {
  uint8_t  slotId;
  uint8_t  applicationId;
  uint32_t applicationVersion;
  uint16_t maxMessageIdSupported;
} MpsiAppsInfoMessage_t;

typedef struct {
  uint8_t slotId;
} MpsiBootloadSlotMessage_t;

typedef struct {
  uint8_t  errorCode;
  uint8_t  sourceApplicationId;
  uint16_t messageIdInError;
} MpsiErrorMessage_t;

typedef struct {
  uint8_t option;
} MpsiInitiateJoiningMessage_t;

typedef struct {
  uint8_t eui64[EUI64_SIZE];
  uint8_t installCodeLength;
  uint8_t installCode[MAX_INSTALL_CODE_LENGTH];
} MpsiZigbeeJoiningDeviceInfoMessage_t;

typedef struct {
  uint8_t eui64[EUI64_SIZE];
  uint8_t pskdLength;
  uint8_t pskd[MAX_PSKD_LENGTH];
} MpsiThreadJoiningDeviceInfoMessage_t;

typedef struct {
  uint32_t channelMask;
  uint8_t  extendedPanId[ZIGBEE_EXT_PAN_LENGTH];
  uint8_t  preconfiguredKey[ZIGBEE_KEY_LENGTH];
} MpsiZigbeeTrustCenterJoiningCredentialsMessage_t;

// Message callback structs
typedef uint8_t (*MpsiMessageHandlerFunction)(MpsiMessage_t*);

typedef struct MpsiMessageHandlerMapping {
  uint16_t                    messageId;
  MpsiMessageHandlerFunction  function;
} MpsiMessageHandlerMapping_t;

// Internal APIs

/**************************************************************************//**
 * Serialize an MpsiMessage_t struct to a buffer.
 *
 * @param[in] MpsiMessage_t struct to serialize
 * @param[out] buffer that contains the serialized MpsiMessage_t struct
 *
 * @return number of bytes written
 *
 * @note When copying the MpsiMessage_t's payload, we only copy as many bytes as
 *       specified by the payloadLength field.
 *****************************************************************************/
uint16_t emberAfPluginMpsiSerialize(MpsiMessage_t *mpsiMessage,
                                    uint8_t       *buffer);

/**************************************************************************//**
 * Deserialize a buffer to an MpsiMessage_t struct.
 *
 * @param[in] buffer that contains the serialized MpsiMessage_t struct
 * @param[out] MpsiMessage_t struct to fill in
 *
 * @return number of bytes written
 *
 * @note When copying from the buffer to the MpsiMessage_t's payload field, we
 *       only copy as many bytes as specified by the payloadLength field.
 *****************************************************************************/
uint16_t emberAfPluginMpsiDeserialize(uint8_t       *buffer,
                                      MpsiMessage_t *mpsiMessage);

/**************************************************************************//**
 * Serialize a specific message struct to a buffer.
 *
 * @param[in] specific message struct to serialize
 * @param[in] the message ID that corresponds to the struct to serialize
 * @param[out] buffer that contains the serialized message struct
 *
 * @return number of bytes written
 *****************************************************************************/
uint8_t emAfPluginMpsiSerializeSpecificMessage(void    *specificMpsiMessage,
                                               uint16_t messageId,
                                               uint8_t *buffer);

/**************************************************************************//**
 * Deserialize a buffer to a specific message struct.
 *
 * @param[in] buffer that contains the serialized specific message struct
 * @param[in] the message ID that corresponds to the struct to fill in
 * @param[out] the message ID that corresponds to the struct to fill in
 *
 * @return number of bytes written
 *
 * @note It is expected that the caller already knows the message type and casts
 *       the returned specificMpsiMessage pointer to the correct struct pointer.
 *       The caller must know the message type since the message ID was passed
 *       in as a parameter.
 *****************************************************************************/
uint8_t emAfPluginMpsiDeserializeSpecificMessage(uint8_t *buffer,
                                                 uint16_t messageId,
                                                 void    *specificMpsiMessage);

#endif  // MPSI_MESSAGE_IDS_H
