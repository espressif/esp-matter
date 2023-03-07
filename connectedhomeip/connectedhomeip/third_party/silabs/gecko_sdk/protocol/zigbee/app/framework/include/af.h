/***************************************************************************//**
 * @file
 * @brief The master include file for the Ember ApplicationFramework  API.
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

#ifndef SILABS_AF_API
#define SILABS_AF_API

// Micro and compiler-specific typedefs and macros
#include PLATFORM_HEADER

#ifndef CONFIGURATION_HEADER
  #define CONFIGURATION_HEADER "app/framework/util/config.h"
#endif
#include CONFIGURATION_HEADER

#ifdef EZSP_HOST
// Includes needed for ember related functions for the EZSP host
  #include "stack/include/error.h"
  #include "stack/include/ember-types.h"
  #include "stack/include/ember-random-api.h"
  #include "app/util/ezsp/ezsp-protocol.h"
  #include "app/util/ezsp/ezsp.h"
  #include "app/util/ezsp/ezsp-utils.h"
  #include "app/util/ezsp/serial-interface.h"
#else
// Includes needed for ember related functions for the SoC
  #include "stack/include/ember.h"
#endif // EZSP_HOST

#include "hal/hal.h"
#include "event_queue/event-queue.h"
#include "stack/include/error.h"
#include "af-types.h"
#include "app/framework/util/print.h"
#include "app/framework/util/time-util.h"

#ifdef UC_BUILD
#include "serial/serial.h"
#include "sl_endianness.h"
// Temporary fix for UC_ALPHA_2, see EMZIGBEE-6782
#include "plugin-component-mappings.h"
#include "af-storage.h"
#include "zap-command.h"
#include "zap-id.h"
#include "zap-type.h"
#include "zap-print.h"
#include "app/framework/util/util.h"
#include "app/framework/util/global-callback.h"
#include "app/framework/util/global-other-callback.h"
#include "app/framework/service-function/sl_service_function.h"
#include "zap-enabled-incoming-commands.h"
#include "zigbee_app_framework_common.h"
#include "app/framework/util/zcl-debug-print.h"
#else // !UC_BUILD
#include "plugin/serial/serial.h"
#include "event_control/event.h"
#include "att-storage.h"
#include "call-command-handler.h"
#include "client-command-macro.h"
#include "callback.h"
#include "af-structs.h"
#include "attribute-type.h"
#include "attribute-id.h"
#include "cluster-id.h"
#include "command-id.h"
#include "enums.h"
#include "print-cluster.h"
#include "af-legacy.h"
#include "debug-printing.h"
#include "app/util/serial/command-interpreter2.h"
#include "app/framework/cli/zcl-cli.h"
#endif // UC_BUILD

#include "app/framework/util/client-api.h"

// TODO: EMZIGBEE-6322 Remove this file when doing the UC_BUILD cleanup works.
#include "app/util/common/uc-temp-macros.h"

/**
 * @defgroup attribute-storage Attribute Storage
 * @ingroup af
 * @brief API for the Attribute Storage functionality
 * in the Application Framework
 *
 * Attribute Storage description.
 *
 */

/**
 * @addtogroup attribute-storage
 * @{
 */

/** @name API */
// @{

/**
 * @brief Locate attribute metadata.
 *
 * Function returns pointer to the attribute metadata structure,
 * or NULL if attribute was not found.
 *
 * @param endpoint Zigbee endpoint number.
 * @param clusterId Cluster ID of the sought cluster.
 * @param attributeId Attribute ID of the sought attribute.
 * @param mask CLUSTER_MASK_SERVER or CLUSTER_MASK_CLIENT
 * @param manufacturerCode Manufacturer code of the sough attribute.
 *
 * @return Returns pointer to the attribute metadata location.
 */
EmberAfAttributeMetadata *emberAfLocateAttributeMetadata(uint8_t endpoint,
                                                         EmberAfClusterId clusterId,
                                                         EmberAfAttributeId attributeId,
                                                         uint8_t mask,
                                                         uint16_t manufacturerCode);

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief Return true if the attribute exists. */
bool emberAfContainsAttribute(uint8_t endpoint,
                              EmberAfClusterId clusterId,
                              EmberAfAttributeId attributeId,
                              uint8_t mask,
                              uint16_t manufacturerCode);
#else
  #define emberAfContainsAttribute(endpoint, clusterId, attributeId, mask, manufacturerCode) \
  (emberAfLocateAttributeMetadata(endpoint, clusterId, attributeId, mask, manufacturerCode) != NULL)
#endif

/**
 * @brief Return true if an endpoint contains a cluster, checking for mfg code.
 *
 * This function returns true regardless of whether
 * the endpoint contains server, client or both.
 * For standard libraries (when ClusterId < FC00),
 * the manufacturerCode is ignored.
 *
 * @param endpoint Zigbee endpoint number.
 * @param clusterId Cluster ID of the sought cluster.
 * @param manufacturerCode Manufacturer code of the sought endpoint.
 */
bool emberAfContainsClusterWithMfgCode(uint8_t endpoint, EmberAfClusterId clusterId, uint16_t manufacturerCode);

/**
 * @brief Return true if the endpoint contains the ZCL cluster with specified ID.
 *
 * This function returns true regardless of whether
 * the endpoint contains server, client or both in the Zigbee cluster Library.
 * This wraps emberAfContainsClusterWithMfgCode with
 * manufacturerCode = EMBER_AF_NULL_MANUFACTURER_CODE
 * If this function is used with a manufacturer specific clusterId,
 * it returns the first cluster that it finds in the Cluster table.
 * It does not return any other clusters that share that ID.
 *
 * @param endpoint Zigbee endpoint number.
 * @param clusterId Cluster ID of the sought cluster.
 */
bool emberAfContainsCluster(uint8_t endpoint, EmberAfClusterId clusterId);

/**
 * @brief Return true if the endpoint has a cluster server, checking for the mfg code.
 *
 * This function returns true if
 * the endpoint contains server of a given cluster.
 * For standard libraries (when ClusterId < FC00), the manufacturerCode is ignored.
 *
 * @param endpoint Zigbee endpoint number.
 * @param clusterId Cluster ID of the sought cluster.
 * @param manufacturerCode Manufacturer code of the sought cluster.
 */
bool emberAfContainsServerWithMfgCode(uint8_t endpoint, EmberAfClusterId clusterId, uint16_t manufacturerCode);

/**
 * @brief Return true if the endpoint contains the ZCL server with a specified ID.
 *
 * This function returns true if
 * the endpoint contains server of a given cluster.
 * This wraps emberAfContainsServer with
 * manufacturerCode = EMBER_AF_NULL_MANUFACTURER_CODE
 * If this function is used with a manufacturer specific clusterId,
 * it returns the first cluster that it finds in the Cluster table.
 * It does not return any other clusters that share that ID.
 *
 * @param endpoint Zigbee endpoint number.
 * @param clusterId Cluster ID of the sought cluster.
 */
bool emberAfContainsServer(uint8_t endpoint, EmberAfClusterId clusterId);

/**
 * @brief Return true if the endpoint contains the cluster client.
 *
 * This function returns true if
 * the endpoint contains client of a given cluster.
 * For standard library clusters (when ClusterId < FC00),
 * the manufacturerCode is ignored.
 *
 * @param endpoint Zigbee endpoint number.
 * @param clusterId Cluster ID of the sought cluster.
 * @param manufacturerCode Manufacturer code of the sought cluster.
 */
bool emberAfContainsClientWithMfgCode(uint8_t endpoint, EmberAfClusterId clusterId, uint16_t manufacturerCode);

/**
 * @brief Return true if the endpoint contains the ZCL client with a specified ID.
 *
 * This function returns true if
 * the endpoint contains client of a given cluster.
 * It wraps emberAfContainsClient with
 * manufacturerCode = EMBER_AF_NULL_MANUFACTURER_CODE
 * If this function is used with a manufacturer-specific clusterId,
 * it returns the first cluster that it finds in the Cluster table.
 * It does not return any other clusters that share that ID.
 *
 * @param endpoint Zigbee endpoint number.
 * @param clusterId Cluster ID of the sought cluster.
 */
bool emberAfContainsClient(uint8_t endpoint, EmberAfClusterId clusterId);

/**
 * @brief Write an attribute performing all checks.
 *
 * This function attempts to write the attribute value from
 * the provided pointer. It only checks that the
 * attribute exists. If it does, it writes the value into
 * the attribute table for the given attribute.
 *
 * This function does not check to see if the attribute is
 * writable since the read only / writable characteristic
 * of an attribute only pertains to external devices writing
 * over the air. Because this function is called locally,
 * it assumes that the device knows what it is doing and has permission
 * to perform the given operation.
 *
 * @see emberAfWriteClientAttribute, emberAfWriteServerAttribute,
 *      emberAfWriteManufacturerSpecificClientAttribute,
 *      emberAfWriteManufacturerSpecificServerAttribute
 *
 * @param endpoint Zigbee endpoint number.
 * @param cluster Cluster ID of the sought cluster.
 * @param attributeID Attribute ID of the sought attribute.
 * @param mask CLUSTER_MASK_SERVER or CLUSTER_MASK_CLIENT.
 * @param dataPtr Pointer to the ZCL attribute.
 * @param dataType ZCL attribute type.
 */
EmberAfStatus emberAfWriteAttribute(uint8_t endpoint,
                                    EmberAfClusterId cluster,
                                    EmberAfAttributeId attributeID,
                                    uint8_t mask,
                                    uint8_t* dataPtr,
                                    EmberAfAttributeType dataType);

/**
 * @brief Write a cluster server attribute.
 *
 * This function is the same as emberAfWriteAttribute
 * except that it saves having to pass the cluster mask.
 * This is useful for code saving because the write attribute
 * is used frequently throughout the framework
 *
 * @see emberAfWriteClientAttribute,
 *      emberAfWriteManufacturerSpecificClientAttribute,
 *      emberAfWriteManufacturerSpecificServerAttribute
 *
 * @param endpoint Zigbee endpoint number.
 * @param cluster Cluster ID of the sought cluster.
 * @param attributeID Attribute ID of the sought attribute.
 * @param dataPtr Pointer to the ZCL attribute.
 * @param dataType ZCL attribute type.
 */
EmberAfStatus emberAfWriteServerAttribute(uint8_t endpoint,
                                          EmberAfClusterId cluster,
                                          EmberAfAttributeId attributeID,
                                          uint8_t* dataPtr,
                                          EmberAfAttributeType dataType);

/**
 * @brief Write a cluster client attribute.
 *
 * This function is the same as emberAfWriteAttribute
 * except that it saves having to pass the cluster mask.
 * This is useful for code saving because the write attribute
 * is used frequently throughout the framework.
 *
 * @see emberAfWriteServerAttribute,
 *      emberAfWriteManufacturerSpecificClientAttribute,
 *      emberAfWriteManufacturerSpecificServerAttribute
 *
 * @param endpoint Zigbee endpoint number.
 * @param cluster Cluster ID of the sought cluster.
 * @param attributeID Attribute ID of the sought attribute.
 * @param dataPtr Pointer to the ZCL attribute.
 * @param dataType ZCL attribute type.
 */
EmberAfStatus emberAfWriteClientAttribute(uint8_t endpoint,
                                          EmberAfClusterId cluster,
                                          EmberAfAttributeId attributeID,
                                          uint8_t* dataPtr,
                                          EmberAfAttributeType dataType);

/**
 * @brief Write a manufacturer-specific server attribute.
 *
 * This function is the same as emberAfWriteAttribute
 * except that it saves having to pass the cluster mask
 * and allows passing a manufacturer code.
 * This is useful for code saving because the write attribute
 * is used frequently throughout the framework.
 *
 * @see emberAfWriteClientAttribute, emberAfWriteServerAttribute,
 *      emberAfWriteManufacturerSpecificClientAttribute
 *
 * @param endpoint Zigbee endpoint number.
 * @param cluster Cluster ID of the sought cluster.
 * @param attributeID Attribute ID of the sought attribute.
 * @param manufacturerCode Manufacturer code of the sought cluster.
 * @param dataPtr Pointer to the ZCL attribute.
 * @param dataType ZCL attribute type.
 */
EmberAfStatus emberAfWriteManufacturerSpecificServerAttribute(uint8_t endpoint,
                                                              EmberAfClusterId cluster,
                                                              EmberAfAttributeId attributeID,
                                                              uint16_t manufacturerCode,
                                                              uint8_t* dataPtr,
                                                              EmberAfAttributeType dataType);

/**
 * @brief Write a manufacturer-specific client attribute.
 *
 * This function is the same as emberAfWriteAttribute
 * except that it saves having to pass the cluster mask
 * and allows passing of a manufacturer code.
 * This is useful for code saving because the write attribute
 * is used frequently throughout the framework.
 *
 * @see emberAfWriteClientAttribute, emberAfWriteServerAttribute,
 *      emberAfWriteManufacturerSpecificServerAttribute
 *
 * @param endpoint Zigbee endpoint number.
 * @param cluster Cluster ID of the sought cluster.
 * @param attributeID Attribute ID of the sought attribute.
 * @param manufacturerCode Manufacturer code of the sought cluster.
 * @param dataPtr Pointer to the ZCL attribute.
 * @param dataType ZCL attribute type.
 */
EmberAfStatus emberAfWriteManufacturerSpecificClientAttribute(uint8_t endpoint,
                                                              EmberAfClusterId cluster,
                                                              EmberAfAttributeId attributeID,
                                                              uint16_t manufacturerCode,
                                                              uint8_t* dataPtr,
                                                              EmberAfAttributeType dataType);

/**
 * @brief Test the success of attribute write.
 *
 * This function returns success if the attribute write is successful.
 * It does not actually write anything, just validates for read-only and
 * data-type.
 *
 * @param endpoint Zigbee endpoint number
 * @param cluster Cluster ID of the sought cluster.
 * @param attributeID Attribute ID of the sought attribute.
 * @param mask CLUSTER_MASK_SERVER or CLUSTER_MASK_CLIENT
 * @param buffer Location where attribute will be written from.
 * @param dataType ZCL attribute type.
 */
EmberAfStatus emberAfVerifyAttributeWrite(uint8_t endpoint,
                                          EmberAfClusterId cluster,
                                          EmberAfAttributeId attributeID,
                                          uint8_t mask,
                                          uint16_t manufacturerCode,
                                          uint8_t* dataPtr,
                                          EmberAfAttributeType dataType);

/**
 * @brief Read the attribute value performing all the checks.
 *
 * This function attempts to read the attribute and store
 * it into the pointer. It also reads the data type.
 * Both dataPtr and dataType may be NULL, signifying that either
 * value or type is not desired.
 *
 * @see emberAfReadClientAttribute, emberAfReadServerAttribute,
 *      emberAfReadManufacturerSpecificClientAttribute,
 *      emberAfReadManufacturerSpecificServerAttribute
 *
 * @param endpoint Zigbee endpoint number
 * @param cluster Cluster ID of the sought cluster.
 * @param attributeID Attribute ID of the sought attribute.
 * @param mask CLUSTER_MASK_SERVER or CLUSTER_MASK_CLIENT
 * @param dataPtr Pointer to the ZCL attribute.
 * @param readLength Length of the attribute to be read.
 * @param dataType ZCL attribute type.
 */
EmberAfStatus emberAfReadAttribute(uint8_t endpoint,
                                   EmberAfClusterId cluster,
                                   EmberAfAttributeId attributeID,
                                   uint8_t mask,
                                   uint8_t* dataPtr,
                                   uint8_t readLength,
                                   EmberAfAttributeType* dataType);

/**
 * @brief Read the server attribute value performing all the checks.
 *
 * This function attempts to read the attribute and store
 * it into the pointer. It also reads the data type.
 * Both dataPtr and dataType may be NULL, signifying that either
 * value or type is not desired.
 *
 * @see emberAfReadClientAttribute,
 *      emberAfReadManufacturerSpecificClientAttribute,
 *      emberAfReadManufacturerSpecificServerAttribute
 *
 * @param endpoint Zigbee endpoint number
 * @param cluster Cluster ID of the sought cluster.
 * @param attributeID Attribute ID of the sought attribute.
 * @param dataPtr Pointer to the server attribute.
 * @param readLength Length of the attribute to be read.
 */
EmberAfStatus emberAfReadServerAttribute(uint8_t endpoint,
                                         EmberAfClusterId cluster,
                                         EmberAfAttributeId attributeID,
                                         uint8_t* dataPtr,
                                         uint8_t readLength);

/**
 * @brief Read the client attribute value, performing all the checks.
 *
 * This function attempts to read the attribute and store
 * it into the pointer. It also reads the data type.
 * Both dataPtr and dataType may be NULL, signifying that either
 * value or type is not desired.
 *
 * @see emberAfReadServerAttribute,
 *      emberAfReadManufacturerSpecificClientAttribute,
 *      emberAfReadManufacturerSpecificServerAttribute
 *
 * @param endpoint Zigbee endpoint number
 * @param cluster Cluster ID of the sought cluster.
 * @param attributeID Attribute ID of the sought attribute.
 * @param dataPtr Pointer to the client attribute.
 * @param readLength Length of the attribute to be read.
 */
EmberAfStatus emberAfReadClientAttribute(uint8_t endpoint,
                                         EmberAfClusterId cluster,
                                         EmberAfAttributeId attributeID,
                                         uint8_t* dataPtr,
                                         uint8_t readLength);

/**
 * @brief Read the manufacturer-specific server attribute value, performing all checks.
 *
 * This function attempts to read the attribute and store
 * it into the pointer. It also reads the data type.
 * Both dataPtr and dataType may be NULL, signifying that either
 * value or type is not desired.
 *
 * @see emberAfReadClientAttribute, emberAfReadServerAttribute,
 *      emberAfReadManufacturerSpecificClientAttribute
 *
 * @param endpoint Zigbee endpoint number
 * @param cluster Cluster ID of the sought cluster.
 * @param attributeID Attribute ID of the sought attribute.
 * @param manufacturerCode Manufacturer code of the sought cluster.
 * @param dataPtr Pointer to the server attribute.
 * @param readLength Length of the attribute to be read.
 */
EmberAfStatus emberAfReadManufacturerSpecificServerAttribute(uint8_t endpoint,
                                                             EmberAfClusterId cluster,
                                                             EmberAfAttributeId attributeID,
                                                             uint16_t manufacturerCode,
                                                             uint8_t* dataPtr,
                                                             uint8_t readLength);

/**
 * @brief Read the manufacturer-specific client attribute value, performing all checks.
 *
 * This function attempts to read the attribute and store
 * it into the pointer. It also reads the data type.
 * Both dataPtr and dataType may be NULL, signifying that either
 * value or type is not desired.
 *
 * @see emberAfReadClientAttribute, emberAfReadServerAttribute,
 *      emberAfReadManufacturerSpecificServerAttribute
 *
 * @param endpoint Zigbee endpoint number
 * @param cluster Cluster ID of the sought cluster.
 * @param attributeID Attribute ID of the sought attribute.
 * @param manufacturerCode Manufacturer code of the sought cluster.
 * @param dataPtr Pointer to the server attribute.
 * @param readLength Length of the attribute to be read.
 */
EmberAfStatus emberAfReadManufacturerSpecificClientAttribute(uint8_t endpoint,
                                                             EmberAfClusterId cluster,
                                                             EmberAfAttributeId attributeID,
                                                             uint16_t manufacturerCode,
                                                             uint8_t* dataPtr,
                                                             uint8_t readLength);

/**
 * @brief Return the size of the ZCL data in bytes.
 *
 * @param dataType ZCL data type
 * @return Size in bytes or 0 if invalid data type
 */
uint8_t emberAfGetDataSize(uint8_t dataType);

/**
 * @brief Return the number of seconds the network remains open.
 *
 * @return A return value of 0 indicates that the network is closed.
 */
uint8_t emberAfGetOpenNetworkDurationSec(void);

/**
 * @brief Return true if the cluster is in the manufacturer-specific range.
 *
 * @param cluster EmberAfCluster* to consider
 */
#define emberAfClusterIsManufacturerSpecific(cluster) ((cluster)->clusterId >= 0xFC00u)

/**
 * @brief Return true if the attribute is read only.
 *
 * @param metadata EmberAfAttributeMetadata* to consider.
 */
#define emberAfAttributeIsReadOnly(metadata) (((metadata)->mask & ATTRIBUTE_MASK_WRITABLE) == 0)

/**
 * @brief Return true if this is a client attribute and false if
 * if it is a server attribute.
 *
 * @param metadata EmberAfAttributeMetadata* to consider.
 */
#define emberAfAttributeIsClient(metadata) (((metadata)->mask & ATTRIBUTE_MASK_CLIENT) != 0)

/**
 * @brief Return true if the attribute is saved to a token.
 *
 * @param metadata EmberAfAttributeMetadata* to consider.
 */
#define emberAfAttributeIsTokenized(metadata) (((metadata)->mask & ATTRIBUTE_MASK_TOKENIZE) != 0)

/**
 * @brief Return true if the attribute is saved in external storage.
 *
 * @param metadata EmberAfAttributeMetadata* to consider.
 */
#define emberAfAttributeIsExternal(metadata) (((metadata)->mask & ATTRIBUTE_MASK_EXTERNAL_STORAGE) != 0)

/**
 * @brief Return true if the attribute is a singleton.
 *
 * @param metadata EmberAfAttributeMetadata* to consider.
 */
#define emberAfAttributeIsSingleton(metadata) (((metadata)->mask & ATTRIBUTE_MASK_SINGLETON) != 0)

/**
 * @brief Return true if the attribute is manufacturer specific.
 *
 * @param metadata EmberAfAttributeMetadata* to consider.
 */
#define emberAfAttributeIsManufacturerSpecific(metadata) (((metadata)->mask & ATTRIBUTE_MASK_MANUFACTURER_SPECIFIC) != 0)

/**
 * @brief Return the size of attribute in bytes.
 *
 * @param metadata EmberAfAttributeMetadata* to consider.
 */
#define emberAfAttributeSize(metadata) ((metadata)->size)

#if !defined(DOXYGEN_SHOULD_SKIP_THIS)
// master array of all defined endpoints
extern EmberAfDefinedEndpoint emAfEndpoints[];

// Master array of all zigbee PRO networks.
extern const EmAfZigbeeProNetwork emAfZigbeeProNetworks[];

// The current zigbee PRO network or NULL.
extern const EmAfZigbeeProNetwork *emAfCurrentZigbeeProNetwork;

// true if the current network is a zigbee PRO network.
#define emAfProIsCurrentNetwork() (emAfCurrentZigbeeProNetwork != NULL)
#endif

/**
 * @brief Take an index of an endpoint and return the Zigbee endpoint.
 *
 *@param index Index of an endpoint.
 *@return Zigbee endpoint.
 */
uint8_t emberAfEndpointFromIndex(uint8_t index);

/**
 * Return the index of a given endpoint.
 *
 *@param endpoint Zigbee endpoint.
 *@return Index of a Zigbee endpoint.
 */
uint8_t emberAfIndexFromEndpoint(uint8_t endpoint);

/**
 * Return the index of a given endpoint. It does not ignore disabled endpoints.
 *
 *@param endpoint Zigbee endpoint.
 *@return Index of a Zigbee endpoint.
 */
uint8_t emberAfIndexFromEndpointIncludingDisabledEndpoints(uint8_t endpoint);

/**
 * Return the endpoint index within a given cluster (Client-side),
 * looking only for standard clusters.
 *
 *@param endpoint Zigbee endpoint.
 *@param clusterId Cluster ID of a sought cluster.
 *@return Index of a Zigbee endpoint.
 */
uint8_t emberAfFindClusterClientEndpointIndex(uint8_t endpoint, EmberAfClusterId clusterId);

/**
 * Return the endpoint index within a given cluster (Server-side),
 * looking only for standard clusters.
 *
 *@param endpoint Zigbee endpoint.
 *@param clusterId Cluster ID of a sought cluster.
 *@return Index of a Zigbee endpoint.
 */
uint8_t emberAfFindClusterServerEndpointIndex(uint8_t endpoint, EmberAfClusterId clusterId);

/**
 * @brief Take an index of an endpoint and return the profile ID for it.
 */
#define emberAfProfileIdFromIndex(index) (emAfEndpoints[(index)].profileId)

/**
 * @brief Take an index of an endpoint and return the device ID for it.
 */
#define emberAfDeviceIdFromIndex(index) (emAfEndpoints[(index)].deviceId)

/**
 * @brief Take an index of an endpoint and return the device version for it.
 */
#define emberAfDeviceVersionFromIndex(index) (emAfEndpoints[(index)].deviceVersion)

/**
 * @brief Take an index of an endpoint and return the network index for it.
 */
#define emberAfNetworkIndexFromEndpointIndex(index) (emAfEndpoints[(index)].networkIndex)

/**
 * @brief Return the network index of a given endpoint.
 *
 *@param endpoint Zigbee endpoint.
 *@return Network index of a Zigbee endpoint.
 */
uint8_t emberAfNetworkIndexFromEndpoint(uint8_t endpoint);

/**
 * @brief Return the primary profile ID.
 *
 * Primary profile is the profile of a primary endpoint as defined
 * in AppBuilder.
 */
#define emberAfPrimaryProfileId()       emberAfProfileIdFromIndex(0)

/**
 * @brief Return the primary endpoint.
 */
#define emberAfPrimaryEndpoint() (emAfEndpoints[0].endpoint)

/**
 * @brief Return the total number of endpoints (dynamic and pre-compiled).
 */
uint8_t emberAfEndpointCount(void);

/**
 * @brief Return the number of pre-compiled endpoints.
 */
uint8_t emberAfFixedEndpointCount(void);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
/**
 * Data types are either analog or discrete. This makes a difference for
 * some of the ZCL global commands
 */
enum {
  EMBER_AF_DATA_TYPE_ANALOG     = 0,
  EMBER_AF_DATA_TYPE_DISCRETE   = 1,
  EMBER_AF_DATA_TYPE_NONE       = 2
};
#endif //DOXYGEN_SHOULD_SKIP_THIS

/**
 * @brief Return the type of the attribute, either ANALOG, DISCRETE, or NONE.
 */
uint8_t emberAfGetAttributeAnalogOrDiscreteType(uint8_t dataType);

/**
 *@brief Return true if type is signed, false otherwise.
 */
bool emberAfIsTypeSigned(EmberAfAttributeType dataType);

/**
 * @brief Extract a 32-bit integer from the message buffer.
 */
uint32_t emberAfGetInt32u(const uint8_t* message, uint16_t currentIndex, uint16_t msgLen);

/**
 * @brief Extract a 24-bit integer from the message buffer.
 */
uint32_t emberAfGetInt24u(const uint8_t* message, uint16_t currentIndex, uint16_t msgLen);

/**
 * @brief Extract a 16-bit integer from the message buffer.
 */
uint16_t emberAfGetInt16u(const uint8_t* message, uint16_t currentIndex, uint16_t msgLen);
/**
 * @brief Extract a ZCL string from the message buffer.
 */
uint8_t* emberAfGetString(uint8_t* message, uint16_t currentIndex, uint16_t msgLen);
/**
 * @brief Extract a ZCL long string from the message buffer.
 */
uint8_t* emberAfGetLongString(uint8_t* message, uint16_t currentIndex, uint16_t msgLen);
/*
 * @brief Extract a ZCL Date from the message buffer and returns it
 * in the given destination. Returns the number of bytes copied.
 */
uint8_t emberAfGetDate(uint8_t* message, uint16_t currentIndex, uint16_t msgLen, EmberAfDate *destination);

/**
 * @brief Extract a single byte out of the message.
 */
#define emberAfGetInt8u(message, currentIndex, msgLen) message[currentIndex]

/**
 * @brief Copy a uint8_t from variable into buffer.
 */
#define emberAfCopyInt8u(data, index, x) (data[index] = (x))
/**
 * @brief Copy a uint16_t value into a buffer.
 */
void emberAfCopyInt16u(uint8_t *data, uint16_t index, uint16_t x);
/**
 * @brief Copy a uint24_t value into a buffer.
 */
void emberAfCopyInt24u(uint8_t *data, uint16_t index, uint32_t x);
/**
 * @brief Copy a uint32_t value into a buffer.
 */
void emberAfCopyInt32u(uint8_t *data, uint16_t index, uint32_t x);
/*
 * @brief Copy a ZCL string type into a buffer.  The size
 * parameter should indicate the maximum number of characters to copy to the
 * destination buffer not including the length byte.
 */
void emberAfCopyString(uint8_t *dest, uint8_t *src, uint8_t size);
/*
 * @brief Copy a ZCL long string into a buffer.  The size
 * parameter should indicate the maximum number of characters to copy to the
 * destination buffer not including the length bytes.
 */
void emberAfCopyLongString(uint8_t *dest, uint8_t *src, uint16_t size);
/*
 * @brief Determine the length of a Zigbee Cluster Library string
 *   (where the first byte is assumed to be the length).
 */
uint8_t emberAfStringLength(const uint8_t *buffer);
/*
 * @brief Determine the length of a Zigbee Cluster Library long string.
 *   (where the first two bytes are assumed to be the length).
 */
uint16_t emberAfLongStringLength(const uint8_t *buffer);

/*
 * @brief Determine the size of a Zigbee Cluster Library
 * attribute value.
 *
 * The attribute could be non-string, string, or long string. For strings,
 * the size includes the length of the string plus the number of the string's
 * length prefix byte(s).
 *
 * Additionally, checks that the resulting size does not exceed the
 * specified buffer size.
 *
 * @param dataType Zigbee data type
 * @param buffer pointer to a buffer containing a string value
 * @param bufferSize size of the buffer
 *
 * @return 0 if dataType is unrecognized or data size exceeds buffer size
 *.        nonzero for valid data size
 */
uint16_t emberAfAttributeValueSize(EmberAfAttributeType dataType,
                                   const uint8_t *buffer,
                                   const uint16_t bufferSize);

/** @} */ // end of name API
/** @} */ // end of attribute-storage

/**
 * @defgroup device-control Device Control
 * @ingroup af
 * @brief API for the Device Control functionality
 * in the Application Framework
 *
 * Device Control description.
 *
 */

/**
 * @addtogroup device-control
 * @{
 */

/** @name API */
// @{

/**
 * @brief Check whether the endpoint is enabled.
 *
 * This function returns true if device at a given endpoint is
 * enabled. At startup all endpoints are enabled.
 *
 * @param endpoint Zigbee endpoint number
 */
bool emberAfIsDeviceEnabled(uint8_t endpoint);

/**
 * @brief Check whether the endpoint is identifying.
 *
 * This function returns true if the device at a given endpoint is
 * identifying.
 *
 * @param endpoint Zigbee endpoint number
 */
bool emberAfIsDeviceIdentifying(uint8_t endpoint);

/**
 * @brief Enable or disables an endpoint.
 *
 * By calling this function, you turn off all processing of incoming traffic
 * for a given endpoint.
 *
 * @param endpoint Zigbee endpoint number
 */
void emberAfSetDeviceEnabled(uint8_t endpoint, bool enabled);

/** @} */ // end of name API
/** @} */ // end of device-control

/**
 * @defgroup miscellaneous Miscellaneous
 * @ingroup af
 * @brief API for the Miscellaneous functionality
 * in the Application Framework
 *
 * Miscellaneous description.
 *
 */

/**
 * @addtogroup miscellaneous
 * @{
 */

/** @name API */
// @{

/**
 * @brief When running on an OS, this API sets the Zigbee stack task to ready.
 */
void sl_zigbee_common_rtos_wakeup_stack_task(void);

/**
 * @brief Enable/disable endpoints.
 */
bool emberAfEndpointEnableDisable(uint8_t endpoint, bool enable);

/**
 * @brief Determine if an endpoint at the specified index is enabled or disabled.
 */
bool emberAfEndpointIndexIsEnabled(uint8_t index);

/**
 * @brief Indicate a new image verification is taking place.
 */
#define EMBER_AF_NEW_IMAGE_VERIFICATION true

/**
 * @brief Indicate the continuation of an image verification already
 * in progress.
 */
#define EMBER_AF_CONTINUE_IMAGE_VERIFY  false

/**
 * @brief This variable defines an invalid image ID.  It is used
 *   to determine if a returned EmberAfOtaImageId is valid or not.
 *   This is done by passing the data to the function
 *   emberAfIsOtaImageIdValid().
 */
extern const EmberAfOtaImageId emberAfInvalidImageId;

/**
 * @brief Return true if a given ZCL data type is a string type.
 *
 * Use this function to perform a different
 * memory operation on a certain attribute because it is a string type.
 * Since ZCL strings carry length as the first byte(s), it is often required
 * to treat them differently than regular data types.
 *
 * @return true if data type is a string.
 */
bool emberAfIsThisDataTypeAStringType(EmberAfAttributeType dataType);

/** @brief Return true if the given attribute type is a string. */
bool emberAfIsStringAttributeType(EmberAfAttributeType attributeType);

/** @brief Return true if the given attribute type is a long string. */
bool emberAfIsLongStringAttributeType(EmberAfAttributeType attributeType);

/**
 * @brief The mask applied by ::emberAfNextSequence when generating ZCL
 * sequence numbers.
 */
#define EMBER_AF_ZCL_SEQUENCE_MASK 0x7Fu

/**
 * @brief The mask applied to generated message tags used by the framework when sending messages via EZSP.
 * Customers who call ezspSend functions directly must use message tags outside this mask
 */
#define EMBER_AF_MESSAGE_TAG_MASK 0x7Fu

/**
 * @brief Increment the ZCL sequence number and returns the value.
 *
 * ZCL messages have sequence numbers so that they can be matched up with other
 * messages in the transaction.  To avoid conflicts with sequence numbers
 * generated independently by the application, this API returns sequence
 * numbers with the high bit clear.  If the application generates its own
 * sequence numbers, it should use numbers with the high bit set.
 *
 * @return The next ZCL sequence number.
 */
uint8_t emberAfNextSequence(void);

/**
 * @brief Retrieve the last sequence number that was used.
 *
 */
uint8_t emberAfGetLastSequenceNumber(void);

/**
 * @brief Simple integer comparison function.
 * Compares two values of a known length as integers.
 * Signed integer comparison are supported for numbers with length of
 * 4 (bytes) or less.
 * The integers are in native endianness.
 *
 * @return -1, if val1 is smaller
 *          0, if they are the same or if two negative numbers with length
 *          greater than 4 is being compared
 *          1, if val2 is smaller.
 */
int8_t emberAfCompareValues(uint8_t* val1, uint8_t* val2, uint8_t len, bool signedNumber);

/**
 * @brief Populate the passed EUI64 with the local EUI64 MAC address.
 */
void emberAfGetEui64(EmberEUI64 returnEui64);

#ifdef EZSP_HOST
// Normally this is provided by the stack code, but on the host
// it is provided by the application code.
void emberReverseMemCopy(uint8_t* dest, const uint8_t* src, uint16_t length);
#endif // EZSP_HOST

/**
 * @brief Return the node ID of the local node.
 */
EmberNodeId emberAfGetNodeId(void);

#if defined(DOXYGEN_SHOULD_SKIP_THIS) || defined(EZSP_HOST)
/**
 * @brief Generate a random key (link, network, or master).
 */
EmberStatus emberAfGenerateRandomKey(EmberKeyData *result);
#else
  #define emberAfGenerateRandomKey(result) emberGenerateRandomKey(result)
#endif

/**
 * @brief Return the PAN ID of the local node.
 */
EmberPanId emberAfGetPanId(void);

/**
 * @brief Return the radioChannel of the current network
 */
uint8_t emberAfGetRadioChannel(void);

/*
 * @brief Return a binding index that matches the current incoming message, if
 * known.
 */
uint8_t emberAfGetBindingIndex(void);

/*
 * @brief Return an address index that matches the current incoming message,
 * if known.
 */
uint8_t emberAfGetAddressIndex(void);

/**
 * @brief Return the current network state.  This call caches the results
 *   on the host to prevent frequent EZSP transactions.
 */
EmberNetworkStatus emberAfNetworkState(void);

/**
 * @brief Get this node's radio channel for the current network.
 */
uint8_t emberAfGetRadioChannel(void);

/**
 * @brief Return the current network parameters.
 */
EmberStatus emberAfGetNetworkParameters(EmberNodeType *nodeType,
                                        EmberNetworkParameters *parameters);

/**
 * @brief Returns the current node type.
 */
EmberStatus emberAfGetNodeType(EmberNodeType *nodeType);

/**
 */
#define EMBER_AF_REJOIN_DUE_TO_END_DEVICE_MOVE            0xA0u
#define EMBER_AF_REJOIN_DUE_TO_TC_KEEPALIVE_FAILURE       0xA1u
#define EMBER_AF_REJOIN_DUE_TO_CLI_COMMAND                0xA2u
#define EMBER_AF_REJOIN_DUE_TO_WWAH_CONNECTIVITY_MANAGER  0xA3u

#define EMBER_AF_REJOIN_FIRST_REASON                EMBER_AF_REJOIN_DUE_TO_END_DEVICE_MOVE
#define EMBER_AF_REJOIN_LAST_REASON                 EMBER_AF_REJOIN_DUE_TO_END_DEVICE_MOVE

/**
 * @brief Enable local permit join and optionally broadcasts the ZDO
 * Mgmt_Permit_Join_req message. This API can be called from any device
 * type and still return EMBER_SUCCESS. If the API is called from an
 * end device, the permit association bit will just be left off.
 *
 * @param duration The duration that the permit join bit will remain on
 * and other devices will be able to join the current network.
 * @param broadcastMgmtPermitJoin whether or not to broadcast the ZDO
 * Mgmt_Permit_Join_req message.
 *
 * @returns status of whether or not permit join was enabled.
 */
EmberStatus emberAfPermitJoin(uint8_t duration,
                              bool broadcastMgmtPermitJoin);

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/**
 * @brief Enable local permit join and broadcasts the ZDO
 * Mgmt_Permit_Join_req message. This API can be called from any device
 * type and still return EMBER_SUCCESS. If the API is called from an
 * end device, the permit association bit will just be left off.
 *
 * @param duration The duration that the permit join bit will remain on
 * and other devices will be able to join the current network.
 *
 * @returns status of whether or not permit join was enabled.
 */
EmberStatus emberAfBroadcastPermitJoin(uint8_t duration);
#else
  #define emberAfBroadcastPermitJoin(duration) \
  emberAfPermitJoin((duration), true)
#endif

/** @} */ // end of name API
/** @} */ // end of miscellaneous

/**
 * @defgroup sleep-control Sleep Control
 * @ingroup af
 * @brief API for the Sleep Control functionality
 * in the Application Framework
 *
 * Sleep Control description.
 *
 */

/**
 * @addtogroup sleep-control
 * @{
 */

/** @name API */
// @{

/**
 * @brief Friendly define for use in the scheduling or canceling client events
 * with sl_zigbee_zcl_schedule_cluster_tick() and
 * sl_zigbee_zcl_deactivate_cluster_tick().
 */
#define EMBER_AF_CLIENT_CLUSTER_TICK true

/**
 * @brief Friendly define for use in the scheduling or canceling server events
 * with sl_zigbee_zcl_schedule_cluster_tick() and
 * sl_zigbee_zcl_deactivate_cluster_tick().
 */
#define EMBER_AF_SERVER_CLUSTER_TICK false

/**
 * @brief Schedule a cluster-related event inside the
 * application framework's event mechanism.  This function provides a wrapper
 * for the Ember stack event mechanism which allows the cluster code to access
 * its events by their endpoint, cluster ID, and client/server identity.  The
 * passed poll and sleep controls allow the cluster to indicate whether it
 * needs to long or short poll and whether it needs to stay awake or if it can
 * sleep.
 *
 * @param endpoint The endpoint of the event to be scheduled.
 * @param clusterId The cluster id of the event to be scheduled.
 * @param isClient ::EMBER_AF_CLIENT_CLUSTER_TICK if the event to be scheduled
 *        is associated with a client cluster or ::EMBER_AF_SERVER_CLUSTER_TICK
 *        otherwise.
 * @param delayMs The number of milliseconds until the event should be called.
 * @param pollControl ::EMBER_AF_SHORT_POLL if the cluster needs to short poll
 *        or ::EMBER_AF_LONG_POLL otherwise.
 * @param sleepControl ::EMBER_AF_STAY_AWAKE if the cluster needs to stay awake
 *        or EMBER_AF_OK_TO_SLEEP otherwise.
 *
 * @return EMBER_SUCCESS if the event was scheduled or an error otherwise.
 */
EmberStatus sl_zigbee_zcl_schedule_tick_extended(uint8_t endpoint,
                                                 EmberAfClusterId clusterId,
                                                 bool isClient,
                                                 uint32_t delayMs,
                                                 EmberAfEventPollControl pollControl,
                                                 EmberAfEventSleepControl sleepControl);

/**
 * @brief Schedule a cluster-related event inside the
 * This function is a wrapper for ::emberAfScheduleTickExtended.  The cluster
 * on the given endpoint will be set to long poll if sleepControl is set to
 * ::EMBER_AF_OK_TO_HIBERNATE or will be set to short poll otherwise.  It will
 * stay awake if sleepControl is ::EMBER_AF_STAY_AWAKE and will sleep
 * otherwise.
 *
 * @param endpoint The endpoint of the event to be scheduled.
 * @param clusterId The cluster id of the event to be scheduled.
 * @param isClient ::EMBER_AF_CLIENT_CLUSTER_TICK if the event to be scheduled
 *        is associated with a client cluster or ::EMBER_AF_SERVER_CLUSTER_TICK
 *        otherwise.
 * @param delayMs The number of milliseconds until the event should be called.
 * @param sleepControl the priority of the event, what the processor should
 *        be allowed to do in terms of sleeping while the event is active.
 *
 * @return EMBER_SUCCESS if the event was scheduled or an error otherwise.
 */
EmberStatus sl_zigbee_zcl_schedule_cluster_tick(uint8_t endpoint,
                                                EmberAfClusterId clusterId,
                                                bool isClient,
                                                uint32_t delayMs,
                                                EmberAfEventSleepControl sleepControl);

/**
 * @brief Schedule a cluster client event.  This function
 * is a wrapper for ::sl_zigbee_zcl_schedule_tick_extended.
 *
 * @param endpoint The endpoint of the event to be scheduled
 * @param clusterId The cluster id of the event to be scheduled
 * @param delayMs The number of milliseconds until the event should be called.
 * @param pollControl ::EMBER_AF_SHORT_POLL if the cluster needs to short poll
 *        or ::EMBER_AF_LONG_POLL otherwise.
 * @param sleepControl ::EMBER_AF_STAY_AWAKE if the cluster needs to stay awake
 *        or EMBER_AF_OK_TO_SLEEP otherwise.
 *
 * @return EMBER_SUCCESS if the event was scheduled or an error otherwise.
 */
EmberStatus sl_zigbee_zcl_schedule_client_tick_extended(uint8_t endpoint,
                                                        EmberAfClusterId clusterId,
                                                        uint32_t delayMs,
                                                        EmberAfEventPollControl pollControl,
                                                        EmberAfEventSleepControl sleepControl);

/**
 * @brief Schedule a cluster client event.  This function
 * is a wrapper for ::sl_zigbee_zcl_schedule_client_tick_extended.  It indicates
 * that the cluster client on the given endpoint can long poll and can sleep.
 *
 * @param endpoint The endpoint of the event to be scheduled.
 * @param clusterId The cluster id of the event to be scheduled.
 * @param delayMs The number of milliseconds until the event should be called.
 *
 * @return EMBER_SUCCESS if the event was scheduled or an error otherwise.
 */
EmberStatus sl_zigbee_zcl_schedule_client_tick(uint8_t endpoint,
                                               EmberAfClusterId clusterId,
                                               uint32_t delayMs);

/**
 * @brief Schedule a cluster server event.  This function
 * is a wrapper for ::sl_zigbee_zcl_schedule_tick_extended.
 *
 * @param endpoint The endpoint of the event to be scheduled.
 * @param clusterId The cluster id of the event to be scheduled.
 * @param delayMs The number of milliseconds until the event should be called.
 * @param pollControl ::EMBER_AF_SHORT_POLL if the cluster needs to short poll
 *        or ::EMBER_AF_LONG_POLL otherwise.
 * @param sleepControl ::EMBER_AF_STAY_AWAKE if the cluster needs to stay awake
 *        or EMBER_AF_OK_TO_SLEEP otherwise.
 *
 * @return EMBER_SUCCESS if the event was scheduled or an error otherwise.
 */
EmberStatus sl_zigbee_zcl_schedule_server_tick_extended(uint8_t endpoint,
                                                        EmberAfClusterId clusterId,
                                                        uint32_t delayMs,
                                                        EmberAfEventPollControl pollControl,
                                                        EmberAfEventSleepControl sleepControl);

/**
 * @brief Schedule a cluster server event.  This function
 * is a wrapper for ::sl_zigbee_zcl_schedule_server_tick_extended.  It indicates
 * that the cluster server on the given endpoint can long poll and can sleep.
 *
 * @param endpoint The endpoint of the event to be scheduled
 * @param clusterId The cluster id of the event to be scheduled.
 * @param delayMs The number of milliseconds until the event should be called.
 *
 * @return EMBER_SUCCESS if the event was scheduled or an error otherwise.
 */
EmberStatus sl_zigbee_zcl_schedule_server_tick(uint8_t endpoint,
                                               EmberAfClusterId clusterId,
                                               uint32_t delayMs);

/**
 * @brief Deactivate a cluster-related event.  This function
 * provides a wrapper for the Ember stack's event mechanism which allows an
 * event to be accessed by its endpoint, cluster id, and client/server
 * identity.
 *
 * @param endpoint The endpoint of the event to be deactivated.
 * @param clusterId The cluster id of the event to be deactivated.
 * @param isClient ::EMBER_AF_CLIENT_CLUSTER_TICK if the event to be
 *        deactivated is a client cluster ::EMBER_AF_SERVER_CLUSTER_TICK
 *        otherwise.
 *
 * @return EMBER_SUCCESS if the event was deactivated or an error otherwise.
 */
EmberStatus sl_zigbee_zcl_deactivate_cluster_tick(uint8_t endpoint,
                                                  EmberAfClusterId clusterId,
                                                  bool isClient);

/**
 * @brief Deactivate a cluster client event.  This function
 * is a wrapper for ::sl_zigbee_zcl_deactivate_cluster_tick.
 *
 * @param endpoint The endpoint of the event to be deactivated.
 * @param clusterId The cluster id of the event to be deactivated.
 *
 * @return EMBER_SUCCESS if the event was deactivated or an error otherwise.
 */
EmberStatus sl_zigbee_zcl_deactivate_client_tick(uint8_t endpoint,
                                                 EmberAfClusterId clusterId);

/**
 * @brief Deactivate a cluster server event.  This function
 * is a wrapper for ::sl_zigbee_zcl_deactivate_cluster_tick.
 *
 * @param endpoint The endpoint of the event to be deactivated.
 * @param clusterId The cluster id of the event to be deactivated.
 *
 * @return EMBER_SUCCESS if the event was deactivated or an error otherwise.
 */
EmberStatus sl_zigbee_zcl_deactivate_server_tick(uint8_t endpoint,
                                                 EmberAfClusterId clusterId);

/**
 * @brief Retrieve the most restrictive sleep
 * control value for all scheduled events. This function is
 * used by emberAfOkToNap and emberAfOkToHibernate to makes sure
 * that there are no events scheduled which will keep the device
 * from hibernating or napping.
 * @return The most restrictive sleep control value for all
 *         scheduled events or the value returned by
 *         emberAfGetDefaultSleepControl()
 *         if no events are currently scheduled. The default
 *         sleep control value is initialized to
 *         EMBER_AF_OK_TO_HIBERNATE but can be changed at any
 *         time using the emberAfSetDefaultSleepControl() function.
 */
#define emberAfGetCurrentSleepControl() \
  emberAfGetCurrentSleepControlCallback()

/**
 * @brief Set the default sleep control
 *        value against which all scheduled event sleep control
 *        values will be evaluated. This can be used to keep
 *        a device awake for an extended period of time by setting
 *        the default to EMBER_AF_STAY_AWAKE and then resetting
 *        the value to EMBER_AF_OK_TO_HIBERNATE once the wake
 *        period is complete.
 */
#define  emberAfSetDefaultSleepControl(x) \
  emberAfSetDefaultSleepControlCallback(x)

/**
 * @brief Retrieve the default sleep control against
 *        which all event sleep control values are evaluated. The
 *        default sleep control value is initialized to
 *        EMBER_AF_OK_TO_HIBERNATE but can be changed by the application
 *        at any time using the emberAfSetDefaultSleepControl() function.
 * @return The current default sleep control value.
 */
#define emberAfGetDefaultSleepControl() \
  emberAfGetDefaultSleepControlCallback()

#if !defined(DOXYGEN_SHOULD_SKIP_THIS)
/**
 * @brief Add a task to the task register.
 */
#define emberAfAddToCurrentAppTasks(x) \
  emberAfAddToCurrentAppTasksCallback(x)

/**
 * @brief Remove a task from the task register.
 */
#define emberAfRemoveFromCurrentAppTasks(x) \
  emberAfRemoveFromCurrentAppTasksCallback(x)

/**
 * @brief Retrieve the bitmask of all application
 * framework tasks currently in progress. This can be useful for debugging if
 * some task is holding the device out of hibernation.
 */
#define emberAfCurrentAppTasks() emberAfGetCurrentAppTasksCallback()
#endif // !DOXYGEN_SHOULD_SKIP_THIS

/** @} */ // end of name API
/** @} */ // end of sleep-control

/**
 * @defgroup messaging Messaging
 * @ingroup af
 * @brief API for the Messaging functionality
 * in the Application Framework
 *
 * Messaging description.
 *
 */

/**
 * @addtogroup messaging
 * @{
 */

/** @name API */
// @{

/**
 * @brief Send a ZCL response, based on the information
 * that is currently in the outgoing buffer. It is expected that a complete
 * ZCL message is present, including header. The application may use
 * this method directly from within the message handling function
 * and associated callbacks. However, this will result in the
 * response being sent before the APS ACK is sent which is not
 * ideal.
 *
 * NOTE:  This will overwrite the ZCL sequence number of the message
 * to use the LAST received sequence number.
 */
EmberStatus emberAfSendResponse(void);

/**
 * @brief Send ZCL response with attached message sent callback
 */
EmberStatus emberAfSendResponseWithCallback(EmberAfMessageSentFunction callback);

/**
 * @brief Sends multicast.
 */
EmberStatus emberAfSendMulticast(EmberMulticastId multicastId,
                                 EmberApsFrame *apsFrame,
                                 uint16_t messageLength,
                                 uint8_t* message);

/**
 * @brief Multicasts the message to the group in the binding table that
 * matches the cluster and source endpoint in the APS frame.  Note: if the
 * binding table contains many matching entries, calling this API cause a
 * significant amount of network traffic. Care should be taken when considering
 * the effects of broadcasts in a network.
 */
EmberStatus emberAfSendMulticastToBindings(EmberApsFrame *apsFrame,
                                           uint16_t messageLength,
                                           uint8_t* message);

/**
 * @brief Send Multicast with alias with attached message sent callback
 */
EmberStatus emberAfSendMulticastWithAliasWithCallback(EmberMulticastId multicastId,
                                                      EmberApsFrame *apsFrame,
                                                      uint16_t messageLength,
                                                      uint8_t *message,
                                                      EmberNodeId alias,
                                                      uint8_t sequence,
                                                      EmberAfMessageSentFunction callback);

/**
 * @brief Send multicast with attached message sent callback.
 */
EmberStatus emberAfSendMulticastWithCallback(EmberMulticastId multicastId,
                                             EmberApsFrame *apsFrame,
                                             uint16_t messageLength,
                                             uint8_t* message,
                                             EmberAfMessageSentFunction callback);

/**
 * @brief Send broadcast.
 */
EmberStatus emberAfSendBroadcast(EmberNodeId destination,
                                 EmberApsFrame *apsFrame,
                                 uint16_t messageLength,
                                 uint8_t* message);

/**
 * @brief Send broadcast with attached message sent callback.
 */
EmberStatus emberAfSendBroadcastWithCallback(EmberNodeId destination,
                                             EmberApsFrame *apsFrame,
                                             uint16_t messageLength,
                                             uint8_t* message,
                                             EmberAfMessageSentFunction callback);

/**
 * @brief Send broadcast with alias with attached message sent callback.
 */
EmberStatus emberAfSendBroadcastWithAliasWithCallback(EmberNodeId destination,
                                                      EmberApsFrame *apsFrame,
                                                      uint16_t messageLength,
                                                      uint8_t *message,
                                                      EmberNodeId alias,
                                                      uint8_t sequence,
                                                      EmberAfMessageSentFunction callback);

/**
 * @brief Send unicast.
 */
EmberStatus emberAfSendUnicast(EmberOutgoingMessageType type,
                               uint16_t indexOrDestination,
                               EmberApsFrame *apsFrame,
                               uint16_t messageLength,
                               uint8_t* message);

/**
 * @brief Send unicast with attached message sent callback.
 */
EmberStatus emberAfSendUnicastWithCallback(EmberOutgoingMessageType type,
                                           uint16_t indexOrDestination,
                                           EmberApsFrame *apsFrame,
                                           uint16_t messageLength,
                                           uint8_t* message,
                                           EmberAfMessageSentFunction callback);

/**
 * @brief Unicast the message to each remote node in the binding table that
 * matches the cluster and source endpoint in the APS frame.  Note: if the
 * binding table contains many matching entries, calling this API cause a
 * significant amount of network traffic.
 */
EmberStatus emberAfSendUnicastToBindings(EmberApsFrame *apsFrame,
                                         uint16_t messageLength,
                                         uint8_t* message);

/**
 * @brief emberAfSendUnicastToBindings with attached message sent callback.
 */
EmberStatus emberAfSendUnicastToBindingsWithCallback(EmberApsFrame *apsFrame,
                                                     uint16_t messageLength,
                                                     uint8_t* message,
                                                     EmberAfMessageSentFunction callback);

/**
 * @brief Send interpan message.
 */
EmberStatus emberAfSendInterPan(EmberPanId panId,
                                const EmberEUI64 destinationLongId,
                                EmberNodeId destinationShortId,
                                EmberMulticastId multicastId,
                                EmberAfClusterId clusterId,
                                EmberAfProfileId profileId,
                                uint16_t messageLength,
                                uint8_t* messageBytes);

/**
 * @brief Send end device binding request.
 */
EmberStatus emberAfSendEndDeviceBind(uint8_t endpoint);

/**
 * @brief Send the command prepared with emberAfFill.... macro.
 *
 * This function is used to send a command that was previously prepared
 * using the emberAfFill... macros from the client command API. It
 * will be sent as unicast to each remote node in the binding table that
 * matches the cluster and source endpoint in the APS frame.  Note: if the
 * binding table contains many matching entries, calling this API cause a
 * significant amount of network traffic.
 */
EmberStatus emberAfSendCommandUnicastToBindings(void);

/**
 * @brief emberAfSendCommandUnicastToBindings with attached message sent callback.
 */
EmberStatus emberAfSendCommandUnicastToBindingsWithCallback(EmberAfMessageSentFunction callback);

/**
 * @brief Send the command prepared with emberAfFill.... macro.
 *
 * This function is used to send a command that was previously prepared
 * using the emberAfFill... macros from the client command API. It
 * will be sent as multicast.
 */
EmberStatus emberAfSendCommandMulticast(EmberMulticastId multicastId);

/**
 * @brief Send the command prepared with emberAfFill.... macro.
 *
 * This function is used to send a command that was previously prepared
 * using the emberAfFill... macros from the client command API. It
 * will be sent as multicast.
 */
EmberStatus emberAfSendCommandMulticastWithAlias(EmberMulticastId multicastId, EmberNodeId alias, uint8_t sequence);

/**
 * @brief emberAfSendCommandMulticast with attached message sent callback.
 */
EmberStatus emberAfSendCommandMulticastWithCallback(EmberMulticastId multicastId,
                                                    EmberAfMessageSentFunction callback);

/**
 * @brief Send the command prepared with emberAfFill.... macro.
 *
 * This function is used to send a command that was previously prepared
 * using the emberAfFill... macros from the client command API. It
 * will be sent as multicast to the group specified in the binding table that
 * matches the cluster and source endpoint in the APS frame.  Note: if the
 * binding table contains many matching entries, calling this API cause a
 * significant amount of network traffic.
 */
EmberStatus emberAfSendCommandMulticastToBindings(void);
/**
 * @brief Send the command prepared with emberAfFill.... macro.
 *
 * This function is used to send a command that was previously prepared
 * using the emberAfFill... macros from the client command API.
 * It will be sent as unicast.
 */
EmberStatus emberAfSendCommandUnicast(EmberOutgoingMessageType type,
                                      uint16_t indexOrDestination);

/**
 * @brief emberAfSendCommandUnicast with attached message sent callback.
 */
EmberStatus emberAfSendCommandUnicastWithCallback(EmberOutgoingMessageType type,
                                                  uint16_t indexOrDestination,
                                                  EmberAfMessageSentFunction callback);

/**
 * @brief Send the command prepared with emberAfFill.... macro.
 *
 * This function is used to send a command that was previously prepared
 * using the emberAfFill... macros from the client command API.
 */
EmberStatus emberAfSendCommandBroadcast(EmberNodeId destination);

/**
 * @brief emberAfSendCommandBroadcast with attached message sent callback.
 */
EmberStatus emberAfSendCommandBroadcastWithCallback(EmberNodeId destination,
                                                    EmberAfMessageSentFunction callback);

/**
 * @brief emberAfSendCommandBroadcast from alias with attached message sent callback.
 */
EmberStatus emberAfSendCommandBroadcastWithAliasWithCallback(EmberNodeId destination,
                                                             EmberNodeId alias,
                                                             uint8_t sequence,
                                                             EmberAfMessageSentFunction callback);

/**
 * @brief Send the command prepared with emberAfFill.... macro.
 *
 * This function is used to send a command that was previously prepared
 * using the emberAfFill... macros from the client command API.
 */
EmberStatus emberAfSendCommandBroadcastWithAlias(EmberNodeId destination,
                                                 EmberNodeId alias,
                                                 uint8_t sequence);
/**
 * @brief Send the command prepared with emberAfFill.... macro.
 *
 * This function is used to send a command that was previously prepared
 * using the emberAfFill... macros from the client command API.
 * It will be sent via inter-PAN.  If destinationLongId is not NULL, the message
 * will be sent to that long address using long addressing mode; otherwise, the
 * message will be sent to destinationShortId using short address mode.  IF
 * multicastId is not zero, the message will be sent using multicast mode.
 */
EmberStatus emberAfSendCommandInterPan(EmberPanId panId,
                                       const EmberEUI64 destinationLongId,
                                       EmberNodeId destinationShortId,
                                       EmberMulticastId multicastId,
                                       EmberAfProfileId profileId);

/**
 * @brief Send a default response to a cluster command.
 *
 * This function is used to prepare and send a default response to a cluster
 * command.
 *
 * @param cmd The cluster command to which to respond.
 * @param status Status code for the default response command.
 * @return An ::EmberStatus value that indicates the success or failure of
 * sending the response.
 */
EmberStatus emberAfSendDefaultResponse(const EmberAfClusterCommand *cmd,
                                       EmberAfStatus status);

/**
 * @brief emberAfSendDefaultResponse with attached message sent callback.
 */
EmberStatus emberAfSendDefaultResponseWithCallback(const EmberAfClusterCommand *cmd,
                                                   EmberAfStatus status,
                                                   EmberAfMessageSentFunction callback);

/**
 * @brief Send a default response to a cluster command using the
 * current command.
 *
 * This function is used to prepare and send a default response to a cluster
 * command.
 *
 * @param status Status code for the default response command.
 * @return An ::EmberStatus value that indicates the success or failure of
 * sending the response.
 */
EmberStatus emberAfSendImmediateDefaultResponse(EmberAfStatus status);

/**
 * @brief emberAfSendImmediateDefaultResponse with attached message sent callback.
 */
EmberStatus emberAfSendImmediateDefaultResponseWithCallback(EmberAfStatus status,
                                                            EmberAfMessageSentFunction callback);

/**
 * @brief Return the maximum size of the payload that the Application
 * Support sub-layer will accept for the given message type, destination, and
 * APS frame.
 *
 * The size depends on multiple factors, including the security level in use
 * and additional information added to the message to support the various
 * options.
 *
 * @param type The outgoing message type.
 * @param indexOrDestination Depending on the message type, this is either the
 *  EmberNodeId of the destination, an index into the address table, an index
 *  into the binding table, the multicast identifier, or a broadcast address.
 * @param apsFrame The APS frame for the message.
 * @return The maximum APS payload length for the given message.
 */
uint8_t emberAfMaximumApsPayloadLength(EmberOutgoingMessageType type,
                                       uint16_t indexOrDestination,
                                       EmberApsFrame *apsFrame);

/**
 * @brief Access to client API APS frame.
 */
EmberApsFrame *emberAfGetCommandApsFrame(void);

/**
 * @brief Set the source and destination endpoints in the client API APS frame.
 */
void emberAfSetCommandEndpoints(uint8_t sourceEndpoint, uint8_t destinationEndpoint);

/**
 * @brief Friendly define for use in discovering client clusters with
 * ::emberAfFindDevicesByProfileAndCluster().
 */
#define EMBER_AF_CLIENT_CLUSTER_DISCOVERY false

/**
 * @brief Friendly define for use in discovering server clusters with
 * ::emberAfFindDevicesByProfileAndCluster().
 */
#define EMBER_AF_SERVER_CLUSTER_DISCOVERY true

/**
 * @brief Use this function to find devices in the network with endpoints
 *   matching a given profile ID and cluster ID in their descriptors.
 *   Target may either be a specific device, or the broadcast
 *   address EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS.
 *
 * With this function a service discovery is initiated and received
 * responses are returned by executing the callback function passed in.
 * For unicast discoveries, the callback will be executed only once.
 * Either the target will return a result or a timeout will occur.
 * For broadcast discoveries, the callback may be called multiple times
 * and after a period of time the discovery will be finished with a final
 * call to the callback.
 *
 * @param target The destination node ID for the discovery; either a specific
 *  node's ID or EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS.
 * @param profileId The application profile for the cluster being discovered.
 * @param clusterId The cluster being discovered.
 * @param serverCluster EMBER_AF_SERVER_CLUSTER_DISCOVERY (true) if discovering
 *  servers for the target cluster; EMBER_AF_CLIENT_CLUSTER_DISCOVERY (false)
 *  if discovering clients for that cluster.
 * @param callback Function pointer for the callback function triggered when
 *  a match is discovered.  (For broadcast discoveries, this is called once per
 *  matching node, even if a node has multiple matching endpoints.)
 */
EmberStatus emberAfFindDevicesByProfileAndCluster(EmberNodeId target,
                                                  EmberAfProfileId profileId,
                                                  EmberAfClusterId clusterId,
                                                  bool serverCluster,
                                                  EmberAfServiceDiscoveryCallback *callback);

/**
 * @brief Use this function to find all of the given in and out clusters
 *   implemented on a devices given endpoint. Target should only be the
 *   short address of a specific device.
 *
 * With this function a single service discovery is initiated and the response
 * is passed back to the passed callback.
 *
 * @param target The destination node ID for the discovery. This should be a
 *  specific node's ID and should not be a broadcast address.
 * @param targetEndpoint The endpoint to target with the discovery process.
 * @param callback Function pointer for the callback function triggered when
 *  the discovery is returned.
 */
EmberStatus emberAfFindClustersByDeviceAndEndpoint(EmberNodeId target,
                                                   uint8_t targetEndpoint,
                                                   EmberAfServiceDiscoveryCallback *callback);

/**
 * @brief Use this function to initiate a discovery for the IEEE address
 *   of the specified node id.  This will send a unicast sent to the target
 *   node ID.
 */
EmberStatus emberAfFindIeeeAddress(EmberNodeId shortAddress,
                                   EmberAfServiceDiscoveryCallback *callback);

/**
 * @brief Use this function to initiate a discovery for the short ID of the
 *   specified long address.  This will send a broadcast to all
 *   rx-on-when-idle devices (non-sleepies).
 */
EmberStatus emberAfFindNodeId(EmberEUI64 longAddress,
                              EmberAfServiceDiscoveryCallback *callback);

/**
 * @brief Initiate an Active Endpoint request ZDO message to the target node ID.
 */
EmberStatus emberAfFindActiveEndpoints(EmberNodeId target,
                                       EmberAfServiceDiscoveryCallback *callback);

/**
 * @brief Use this function to add an entry for a remote device to the address
 * table.
 *
 * If the EUI64 already exists in the address table, the index of the existing
 * entry will be returned.  Otherwise, a new entry will be created and the new
 * new index will be returned.  The framework will remember how many times the
 * returned index has been referenced.  When the address table entry is no
 * longer needed, the application should remove its reference by calling
 * ::emberAfRemoveAddressTableEntry.
 *
 * @param longId The EUI64 of the remote device.
 * @param shortId The node id of the remote device or ::EMBER_UNKNOWN_NODE_ID
 * if the node id is currently unknown.
 * @return The index of the address table entry for this remove device or
 * ::EMBER_NULL_ADDRESS_TABLE_INDEX if an error occurred (e.g., the address
 * table is full).
 */
uint8_t emberAfAddAddressTableEntry(EmberEUI64 longId, EmberNodeId shortId);

/**
 * @brief Use this function to add an entry for a remote device to the address
 * table at a specific location.
 *
 * The framework will remember how many times an address table index has been
 * referenced through ::emberAfAddAddressTableEntry.  If the reference count
 * for the index passed to this function is not zero, the entry will be not
 * changed.   When the address table entry is no longer needed, the application
 * should remove its reference by calling ::emberAfRemoveAddressTableEntry.
 *
 * @param index The index of the address table entry.
 * @param longId The EUI64 of the remote device.
 * @param shortId The node id of the remote device or ::EMBER_UNKNOWN_NODE_ID
 * if the node id is currently unknown.
 * @return ::EMBER_SUCCESS if the address table entry was successfully set,
 * ::EMBER_ADDRESS_TABLE_ENTRY_IS_ACTIVE if any messages are being sent using
 * the existing entry at that index or the entry is still referenced in the
 * framework, or ::EMBER_ADDRESS_TABLE_INDEX_OUT_OF_RANGE if the index is out
 * of range.
 */
EmberStatus emberAfSetAddressTableEntry(uint8_t index,
                                        EmberEUI64 longId,
                                        EmberNodeId shortId);

/**
 * @brief Use this function to remove a specific entry from the address table.
 *
 * The framework will remember how many times an address table index has been
 * referenced through ::emberAfAddAddressTableEntry and
 * ::emberAfSetAddressTableEntry.  The address table entry at this index will
 * not actually be removed until its reference count reaches zero.
 *
 * @param index The index of the address table entry.
 * @return ::EMBER_SUCCESS if the address table entry was successfully removed
 * or ::EMBER_ADDRESS_TABLE_INDEX_OUT_OF_RANGE if the index is out of range.
 */
EmberStatus emberAfRemoveAddressTableEntry(uint8_t index);

#if !defined(DOXYGEN_SHOULD_SKIP_THIS)
/**
 * @brief Use this macro to retrieve the current command. This
 * macro may only be used within the command parsing context. For instance
 * Any of the command handling callbacks may use this macro. If this macro
 * is used outside the command context, the returned EmberAfClusterCommand pointer
 * will be null.
 */
#define emberAfCurrentCommand() (emAfCurrentCommand)
extern EmberAfClusterCommand *emAfCurrentCommand;
#endif

/**
 * @brief Return the current endpoint that is being served.
 *
 * The purpose of this macro is mostly to access endpoint that
 * is being served in the command callbacks.
 */
#define emberAfCurrentEndpoint() (emberAfCurrentCommand()->apsFrame->destinationEndpoint)

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief Use this function to initiate key establishment with a remote node.
 * ::emberAfKeyEstablishmentCallback will be called as events occur and when
 * key establishment completes.
 *
 * @param nodeId The node ID of the remote device.
 * @param endpoint The endpoint on the remote device.
 * @return ::EMBER_SUCCESS if key establishment was initiated successfully
 */
EmberStatus emberAfInitiateKeyEstablishment(EmberNodeId nodeId, uint8_t endpoint);

/** @brief Use this function to initiate key establishment with a remote node on
 * a different PAN.  ::emberAfInterPanKeyEstablishmentCallback will be called
 * as events occur and when key establishment completes.
 *
 * @param panId The PAN id of the remote device.
 * @param eui64 The EUI64 of the remote device.
 * @return ::EMBER_SUCCESS if key establishment was initiated successfully
 */
EmberStatus emberAfInitiateInterPanKeyEstablishment(EmberPanId panId,
                                                    const EmberEUI64 eui64);

/** @brief Use this function to tell if the device is in the process of
 * performing key establishment.
 *
 * @return ::true if key establishment is in progress.
 */
bool emberAfPerformingKeyEstablishment(void);

/** @brief Use this function to initiate partner link key exchange with a
 * remote node.
 *
 * @param target The node id of the remote device.
 * @param endpoint The key establishment endpoint of the remote device.
 * @param callback The callback that should be called when the partner link
 * key exchange completes.
 * @return ::EMBER_SUCCESS if the partner link key exchange was initiated
 * successfully.
 */
EmberStatus emberAfInitiatePartnerLinkKeyExchange(EmberNodeId target,
                                                  uint8_t endpoint,
                                                  EmberAfPartnerLinkKeyExchangeCallback *callback);
#else
  #define emberAfInitiateKeyEstablishment(nodeId, endpoint) \
  emberAfInitiateKeyEstablishmentCallback(nodeId, endpoint)
  #define emberAfInitiateInterPanKeyEstablishment(panId, eui64) \
  emberAfInitiateInterPanKeyEstablishmentCallback(panId, eui64)
  #define emberAfPerformingKeyEstablishment() \
  emberAfPerformingKeyEstablishmentCallback()
  #define emberAfInitiatePartnerLinkKeyExchange(target, endpoint, callback) \
  emberAfInitiatePartnerLinkKeyExchangeCallback(target, endpoint, callback)
#endif

/** @brief Use this function to determine if the security profile of the
 * current network was set to Smart Energy.  The security profile is configured
 * in AppBuilder.
   @ return true if the security profile is Smart Energy or false otherwise.
 */
bool emberAfIsCurrentSecurityProfileSmartEnergy(void);

/** @} */ // end of name API
/** @} */ // end of messaging

/**
 * @defgroup zcl_commands ZCL Commands
 * @ingroup af
 * @brief API for runtime subscription to ZCL commands
 *
 */

/**
 * @addtogroup zcl_commands
 *
 * Following a brief usage example that demonstrate how to subscribe to the ZCL
 * "Level Control" cluster commands, client side, with no manufacturer ID.
 * In this example we only handle only "Stop" command.
 * The subscribed callback must return a value of:
 * - ::EMBER_ZCL_STATUS_SUCCESS if the received ZCL command was handled.
 * - ::EMBER_ZCL_STATUS_UNSUP_COMMAND if the received ZCL command was not
 * handled.
 *
 * @code
 * void my_zcl_command_handler(sl_service_opcode_t opcode,
 *                             sl_service_function_context_t *context)
 * {
 *   assert(opcode == SL_SERVICE_FUNCTION_TYPE_ZCL_COMMAND);
 *
 *   EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
 *
 *   switch (cmd->commandId) {
 *     case ZCL_STOP_COMMAND_ID:
 *     {
 *       // ZCL command structs and parsing functions are implemented in the
 *       // generated zap-cluster-command-parser.[c,h] files.
 *       sl_zcl_level_control_cluster_stop_command_t cmd_data;
 *
 *       if (zcl_decode_level_control_cluster_stop_command(cmd, &cmd_data)
 *           != EMBER_ZCL_STATUS_SUCCESS) {
 *         return EMBER_ZCL_STATUS_UNSUP_COMMAND;
 *       }
 *
 *       // Handle the command here
 *
 *       // Send a default response back to the client
 *       emberAfSendDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
 *
 *       return EMBER_ZCL_STATUS_SUCCESS;
 *     }
 *   }
 *
 *   return EMBER_ZCL_STATUS_UNSUP_COMMAND;
 * }
 *
 * void app_init(void)
 * {
 *   sl_zigbee_subscribe_to_zcl_commands(ZCL_LEVEL_CONTROL_CLUSTER_ID,
 *                                       0xFFFF, // not manufacturer specific
 *                                       ZCL_DIRECTION_SERVER_TO_CLIENT,
 *                                       my_zcl_command_handler);
 * }
 * @endcode
 *
 * @{
 */

/** @name API */
// @{

#ifdef UC_BUILD
/** @brief Runtime subscription to specific incoming ZCL commands.
 *
 * @param cluster_id    The cluster ID of the ZCL messages to subscribe to.
 *
 * @param manufacturer_id   The manufacturer ID if any. If not set, a value of
 *                          0xFFFF should be passed in.
 *
 * @param direction  A value of ::ZCL_DIRECTION_CLIENT_TO_SERVER or
 *                   ::ZCL_DIRECTION_SERVER_TO_CLIENT specifying the side
 *                   (client or server) of the ZCL messages to subscribe to.
 *
 * @param service_function   A ::sl_service_function_t function pointer
 *                           specifying the function to be invoked when an
 *                           incoming ZCL command matches the criteria specified
 *                           in this API.
 *
 * @return A ::sl_status_t value of:
 * - ::SL_STATUS_OK - If the subscription was successful.
 * - ::SL_STATUS_INVALID_PARAMETER - If one or more passed parameters are
 *   invalid.
 * - ::SL_STATUS_ALLOCATION_FAILED - If the system failed to allocate the
 *   necessary data structures.
 */
sl_status_t sl_zigbee_subscribe_to_zcl_commands(uint16_t cluster_id,
                                                uint16_t manufacturer_id,
                                                uint8_t direction,
                                                sl_service_function_t service_function);
#endif // UC_BUILD

/** @} */ // end of name API
/** @} */ // end of zcl_commands

/**
 * @defgroup zcl-macros ZCL Macros
 * @ingroup af
 * @brief ZCL Macros in the Application Framework
 *
 * ZCL Macro description.
 *
 */

/**
 * @addtogroup zcl-macros
 * @{
 */

// Frame control fields (8 bits total)
// Bits 0 and 1 are Frame Type Sub-field
#define ZCL_FRAME_CONTROL_FRAME_TYPE_MASK     (BIT(0) | BIT(1))
#define ZCL_CLUSTER_SPECIFIC_COMMAND          BIT(0)
#define ZCL_PROFILE_WIDE_COMMAND              0u
#define ZCL_GLOBAL_COMMAND                    (ZCL_PROFILE_WIDE_COMMAND)
// Bit 2 is Manufacturer Specific Sub-field
#define ZCL_MANUFACTURER_SPECIFIC_MASK        BIT(2)
// Bit 3 is Direction Sub-field
#define ZCL_FRAME_CONTROL_DIRECTION_MASK      BIT(3)
#define ZCL_FRAME_CONTROL_SERVER_TO_CLIENT    BIT(3)
#define ZCL_FRAME_CONTROL_CLIENT_TO_SERVER    0u
// Bit 4 is Disable Default Response Sub-field
#define ZCL_DISABLE_DEFAULT_RESPONSE_MASK     BIT(4)
// Bits 5 to 7 are reserved

#define ZCL_DIRECTION_CLIENT_TO_SERVER 0u
#define ZCL_DIRECTION_SERVER_TO_CLIENT 1u

// Packet must be at least 3 bytes for ZCL overhead.
//   Frame Control (1-byte)
//   Sequence Number (1-byte)
//   Command Id (1-byte)
#define EMBER_AF_ZCL_OVERHEAD                       3
#define EMBER_AF_ZCL_MANUFACTURER_SPECIFIC_OVERHEAD 5

// Permitted values for emberAfSetFormAndJoinMode
#define FIND_AND_JOIN_MODE_ALLOW_2_4_GHZ        BIT(0)
#define FIND_AND_JOIN_MODE_ALLOW_SUB_GHZ        BIT(1)
#define FIND_AND_JOIN_MODE_ALLOW_BOTH           (FIND_AND_JOIN_MODE_ALLOW_2_4_GHZ | FIND_AND_JOIN_MODE_ALLOW_SUB_GHZ)

/** @} */ // end of zcl-macros

/**
 * @defgroup network-utility-functions Network Utility Functions
 * @ingroup af
 * @brief API and Callbacks for the Network Utility functionality
 * in the Application Framework
 *
 * Network Utility Functions description.
 *
 */

/**
 * @addtogroup network-utility-functions
 * @{
 */

/** @name API */
// @{

/** @brief Use this function to form a new network using the specified network
 * parameters.
 *
 * @param parameters Specification of the new network.
 * @return An ::EmberStatus value that indicates either the successful formation
 * of the new network or the reason that the network formation failed.
 */
EmberStatus emberAfFormNetwork(EmberNetworkParameters *parameters);

/** @brief Use this function to associate with the network using the specified
 * network parameters.
 *
 * @param parameters Specification of the network with which the node should
 * associate.
 * @return An ::EmberStatus value that indicates either that the association
 * process began successfully or the reason for failure.
 */
EmberStatus emberAfJoinNetwork(EmberNetworkParameters *parameters);

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief Use this function to find an unused PAN id and form a new network.
 *
 * @return An ::EmberStatus value that indicates either the process begin
 * successfully or the reason for failure.
 */
EmberStatus emberAfFindUnusedPanIdAndForm(void);
/** @brief Use this function to find a joinable network and join it.
 *
 * @return An ::EmberStatus value that indicates either the process begin
 * successfully or the reason for failure.
 */
EmberStatus emberAfStartSearchForJoinableNetwork(void);
#else
  #define emberAfFindUnusedPanIdAndForm()        emberAfFindUnusedPanIdAndFormCallback()
  #define emberAfStartSearchForJoinableNetwork() emberAfStartSearchForJoinableNetworkCallback()
#endif

/** @brief Set the current network to that of the given index and adds it to
 * the stack of networks maintained by the framework.  Every call to this API
 * must be paired with a subsequent call to ::emberAfPopNetworkIndex.
 */

EmberStatus emberAfPushNetworkIndex(uint8_t networkIndex);
/** @brief Set the current network to the callback network and adds it to
 * the stack of networks maintained by the framework.  Every call to this API
 * must be paired with a subsequent call to ::emberAfPopNetworkIndex.
 */

EmberStatus emberAfPushCallbackNetworkIndex(void);
/** @brief Set the current network to that of the given endpoint and adds it
 * to the stack of networks maintained by the framework.  Every call to this
 * API must be paired with a subsequent call to ::emberAfPopNetworkIndex.
 */

EmberStatus emberAfPushEndpointNetworkIndex(uint8_t endpoint);
/** @brief Remove the topmost network from the stack of networks maintained by
 * the framework and sets the current network to the new topmost network.
 * Every call to this API must be paired with a prior call to
 * ::emberAfPushNetworkIndex, ::emberAfPushCallbackNetworkIndex, or
 * ::emberAfPushEndpointNetworkIndex.
 */

EmberStatus emberAfPopNetworkIndex(void);
/** @brief Return the primary endpoint of the given network index or 0xFF if
 * no endpoints belong to the network.emberAfStatus
 */

uint8_t emberAfPrimaryEndpointForNetworkIndex(uint8_t networkIndex);
/** @brief Returns the primary endpoint of the current network index or 0xFF if
 * no endpoints belong to the current network.
 */

uint8_t emberAfPrimaryEndpointForCurrentNetworkIndex(void);

#if !defined(DOXYGEN_SHOULD_SKIP_THIS)
/** @brief Initialize the stack of networks maintained by the framework,
 * including setting the default network.
 *
 * @return An ::EmberStatus value that indicates either that the network stack
 * has been successfully initialized or the reason for failure.
 */
EmberStatus emAfInitializeNetworkIndexStack(void);
void emAfAssertNetworkIndexStackIsEmpty(void);
#endif

/** @} */ // end of API
/** @} */ // end of network-utility-functions

#if !defined(DOXYGEN_SHOULD_SKIP_THIS)
  #if defined(EMBER_TEST)
    #define EMBER_TEST_ASSERT(x) assert(x)
  #else
    #define EMBER_TEST_ASSERT(x)
  #endif
#endif

/** @brief The maximum power level that can be used by the chip.
 */
// Note:  This is a #define for now but could be a real function call in the future.
#define emberAfMaxPowerLevel() (3)

#if !defined(DOXYGEN_SHOULD_SKIP_THIS)
#ifdef UC_BUILD
// This macro used to be generated by AppBuilder, defining it disables legacy CLI commands.
// Legacy CLI is no longer supported and will eventually be cleaned up (see EMZIGBEE-868),
// until then define this statically.
#define EMBER_AF_GENERATE_CLI
#endif // UC_BUILD
#endif // !DOXYGEN_SHOULD_SKIP_THIS

#endif // SILABS_AF_API
