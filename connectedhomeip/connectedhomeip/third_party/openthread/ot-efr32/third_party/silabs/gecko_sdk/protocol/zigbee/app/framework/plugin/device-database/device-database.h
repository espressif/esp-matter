/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Device Database plugin.
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

/**
 * @defgroup device-database Device Database
 * @ingroup component
 * @brief API and Callbacks for the Device Database Component
 *
 * This component provides an API to add/remove a device
 * from a list of known devices, and to record their list of endpoints and clusters.
 *
 */

/**
 * @addtogroup device-database
 * @{
 */

/**
 * @name API
 * @{
 */
/** @brief  Get device by index
 *
 * @param index Ver.: always
 *
 * @return EmberAfDeviceInfo Device Information struct
 *
 */
const EmberAfDeviceInfo* emberAfPluginDeviceDatabaseGetDeviceByIndex(uint16_t index);

/** @brief Find device by status
 *
 * @param status Ver.: always
 *
 * @return EmberAfDeviceInfo Device Information struct
 *
 */
const EmberAfDeviceInfo* emberAfPluginDeviceDatabaseFindDeviceByStatus(EmberAfDeviceDiscoveryStatus status);

/** @brief Find device by EUI64
 *
 * @param eui64
 *
 * @return EmberAfDeviceInfo Device Information struct
 *
 */
const EmberAfDeviceInfo* emberAfPluginDeviceDatabaseFindDeviceByEui64(EmberEUI64 eui64);

/** @brief add to the device database
 *
 * @param eui64 Ver.: always
 * @param zigbeeCapabilities Ver.: always
 *
 * @return EmberDeviceInfo
 *
 */
const EmberAfDeviceInfo* emberAfPluginDeviceDatabaseAdd(EmberEUI64 eui64, uint8_t zigbeeCapabilities);

/** @brief Erase device from device database
 *
 * @param eui64 Ver.: always
 *
 * @return bool true is success
 *
 */
bool emberAfPluginDeviceDatabaseEraseDevice(EmberEUI64 eui64);

/** @brief Set device endpoints
 *
 * @param eui64
 * @param endpointList
 * @param endpointCount
 *
 * @return bool true is success
 *
 */
bool emberAfPluginDeviceDatabaseSetEndpoints(const EmberEUI64 eui64,
                                             const uint8_t* endpointList,
                                             uint8_t endpointCount);

/** @brief Get device endpoint from index
 *
 * @param eui64
 * @param index
 *
 * @return uint8_t
 *
 */
uint8_t emberAfPluginDeviceDatabaseGetDeviceEndpointFromIndex(const EmberEUI64 eui64,
                                                              uint8_t index);

/** @brief Get index from endpoint
 *
 * @param endpoint
 * @param eui64
 *
 * @note Explicitly made the eui64 the second argument to prevent confusion between
 * this function and the emberAfPluginDeviceDatabaseGetDeviceEndpointsFromIndex()
 *
 */
uint8_t emberAfPluginDeviceDatabaseGetIndexFromEndpoint(uint8_t endpoint,
                                                        const EmberEUI64 eui64);

/** @brief Set clusters from endpoint
 *
 * @param eui64
 * @param clusterList
 *
 * @return bool true is success
 *
 */
bool emberAfPluginDeviceDatabaseSetClustersForEndpoint(const EmberEUI64 eui64,
                                                       const EmberAfClusterList* clusterList);
/** @brief Clears the failed DB discovery count
 *
 * @param maxFailureCount
 *
 * @return bool true is success
 */
bool emberAfPluginDeviceDatabaseClearAllFailedDiscoveryStatus(uint8_t maxFailureCount);

/** @brief Get device database status string
 *
 * @param status
 *
 * @return const char* status string
 *
 */
const char* emberAfPluginDeviceDatabaseGetStatusString(EmberAfDeviceDiscoveryStatus status);

/** @brief Set device database status
 *
 * @param deviceEui64
 * @param newStatus
 *
 * @return bool true is success.
 *
 */
bool emberAfPluginDeviceDatabaseSetStatus(const EmberEUI64 deviceEui64, EmberAfDeviceDiscoveryStatus newStatus);

/** @brief Add device to database with all information
 *
 * @param newDevice Struct pointer to device infor
 *
 * @return EmberDeviceInfo
 *
 */
const EmberAfDeviceInfo* emberAfPluginDeviceDatabaseAddDeviceWithAllInfo(const EmberAfDeviceInfo* newDevice);

/** @brief Checks if a device has cluster
 *
 * @param deviceEui64
 * @param clusterToFind
 * @param server
 * @param returnEndpoint
 *
 * @return EmberStatus status code
 *
 */
EmberStatus emberAfPluginDeviceDatabaseDoesDeviceHaveCluster(EmberEUI64 deviceEui64,
                                                             EmberAfClusterId clusterToFind,
                                                             bool server,
                                                             uint8_t* returnEndpoint);

/** @brief create a new search
 *
 * @param iterator
 *
 */
void emberAfPluginDeviceDatabaseCreateNewSearch(EmberAfDeviceDatabaseIterator* iterator);

/** @brief Find device supporting cluster
 *
 * @param iterator
 * @param clusterToFind
 * @param server
 * @param returnEndpoint
 *
 * @return EmberStatus status code
 */
EmberStatus emberAfPluginDeviceDatabaseFindDeviceSupportingCluster(EmberAfDeviceDatabaseIterator* iterator,
                                                                   EmberAfClusterId clusterToFind,
                                                                   bool server,
                                                                   uint8_t* returnEndpoint);

/** @} */ // end of name API
/** @} */ // end of device-database

void emAfPluginDeviceDatabaseUpdateNodeStackRevision(EmberEUI64 eui64,
                                                     uint8_t stackRevision);
