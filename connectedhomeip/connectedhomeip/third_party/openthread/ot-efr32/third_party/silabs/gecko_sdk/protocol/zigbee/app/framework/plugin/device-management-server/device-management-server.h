/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Device Management Server plugin.
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

#include "app/framework/plugin/device-management-server/device-management-common.h"

/**
 * @defgroup device-management-server Device Management Server
 * @ingroup component cluster
 * @brief API and Callbacks for the Device Management Cluster Server Component
 *
 * This component implements the server-side functionality of the
 * Smart Energy 1.2 Device Management cluster.
 *
 */

/**
 * @addtogroup device-management-server
 * @{
 */

/**
 * @name API
 * @{
 */

/** @brief Set tenancy
 *
 * @param tenency Ver.: always
 * @param validateOptionalFields Ver.: always
 *
 * @return bool true is success
 *
 */
bool emberAfPluginDeviceManagementSetTenancy(EmberAfDeviceManagementTenancy *tenancy,
                                             bool validateOptionalFields);

/** @brief Get tenancy
 *
 * @param tenancy tenancy Ver.: always
 *
 * @return bool true is success
 *
 */
bool emberAfPluginDeviceManagementGetTenancy(EmberAfDeviceManagementTenancy *tenancy);

/** @brief Set supplier for the device
 *
 * @param endpoint Ver.: always
 * @param supplier Ver.: always
 *
 * @return bool true is success
 *
 */
bool emberAfPluginDeviceManagementSetSupplier(uint8_t endpoint, EmberAfDeviceManagementSupplier *supplier);

/** @brief Get supplier for the device
 *
 * @param supplier Ver.: always
 *
 * @return bool true is success
 *
 */
bool emberAfPluginDeviceManagementGetSupplier(EmberAfDeviceManagementSupplier *supplier);

/** @brief Set global data device information
 *
 * @param providerId Ver.: always
 * @param issuerEventId Ver.: always
 * @param tarrifType Ver.: always
 *
 * @return bool true is success
 *
 */
bool emberAfPluginDeviceManagementSetInfoGlobalData(uint32_t providerId,
                                                    uint32_t issuerEventId,
                                                    uint8_t tariffType);

/** @brief Set device site ID
 *
 * @param siteID Ver.: always
 *
 * @return bool true is success
 *
 */
bool emberAfPluginDeviceManagementSetSiteId(EmberAfDeviceManagementSiteId *siteId);

/** @brief Get device site ID
 *
 * @param siteID Ver.: always
 *
 * @return bool true is success
 *
 */
bool emberAfPluginDeviceManagementGetSiteId(EmberAfDeviceManagementSiteId *siteId);

/** @brief Set device CIN
 *
 * @param cin Ver.: always
 *
 * @return bool true is success
 *
 */
bool emberAfPluginDeviceManagementSetCIN(EmberAfDeviceManagementCIN *cin);

/** @brief Get device CIN
 *
 * @param cin Ver.: always
 *
 * @return bool true is success
 *
 */
bool emberAfPluginDeviceManagementGetCIN(EmberAfDeviceManagementCIN *cin);

/** @brief Set device password
 *
 * @param password Ver.: always
 *
 * @return bool true is success
 *
 */
bool emberAfPluginDeviceManagementSetPassword(EmberAfDeviceManagementPassword *password);

/** @brief Get device password
 *
 * @param password Ver.: always
 *
 * @return bool true is success
 *
 */
bool emberAfPluginDeviceManagementGetPassword(EmberAfDeviceManagementPassword *password,
                                              uint8_t passwordType);

/** @brief Device managerment server print
 *
 */
void emberAfDeviceManagementServerPrint(void);

/** @brief Update device site ID
 *
 * @param dstAddr Ver.: always
 * @param srcEndpoint Ver.: always
 * @param dstEndpoint Ver.: always
 *
 * @return bool true is success
 *
 */
bool emberAfDeviceManagementClusterUpdateSiteId(EmberNodeId dstAddr,
                                                uint8_t srcEndpoint,
                                                uint8_t dstEndpoint);

/** @brief Set device provider ID
 *
 * @param providerId Ver.: always
 *
 * @return bool true is success
 *
 */
bool emberAfPluginDeviceManagementSetProviderId(uint32_t providerId);

/** @brief Set device issuer event ID
 *
 * @param issuerEventId
 *
 * @return bool true is success
 *
 */
bool emberAfPluginDeviceManagementSetIssuerEventId(uint32_t issuerEventId);

/** @brief Set device tarrif type
 *
 * @param tarrifType Ver.: always
 *
 * @return bool true is success
 *
 */
bool emberAfPluginDeviceManagementSetTariffType(EmberAfTariffType tariffType);

/** @brief Publish the change of tenancy
 *
 * @param dstAddr Ver.: always
 * @param srcEndpoint Ver.: always
 * @param dstEndpoint Ver.: always
 *
 * @return bool true is success
 *
 */
bool emberAfDeviceManagementClusterPublishChangeOfTenancy(EmberNodeId dstAddr,
                                                          uint8_t srcEndpoint,
                                                          uint8_t dstEndpoint);

/** @brief Publish the change of supplier
 *
 * @param dstAddr Ver.: always
 * @param srcEndpoint Ver.: always
 * @param dstEndpoint Ver.: always
 *
 * @return bool true is success
 *
 */
bool emberAfDeviceManagementClusterPublishChangeOfSupplier(EmberNodeId dstAddr,
                                                           uint8_t srcEndpoint,
                                                           uint8_t dstEndpoint);

/** @brief Set device pending updates
 *
 * @param pendingUpdatesMask Ver.: always
 *
 */
void emberAfDeviceManagementClusterSetPendingUpdates(EmberAfDeviceManagementChangePendingFlags pendingUpdatesMask);

/** @brief Get device pending updates
 *
 * @param pendingUpdatesMask Ver.: always
 *
 */
void emberAfDeviceManagementClusterGetPendingUpdates(EmberAfDeviceManagementChangePendingFlags *pendingUpdatesMask);

/** @brief Update CIN for the device
 *
 * @param dstAddr Ver.: always
 * @param srcEndpoint Ver.: always
 * @param dstEndpoint Ver.: always
 *
 * @return bool true is success
 *
 */
bool emberAfDeviceManagementClusterUpdateCIN(EmberNodeId dstAddr,
                                             uint8_t srcEndpoint,
                                             uint8_t dstEndpoint);

/** @brief Send the request new password response
 *
 * @param passwordType Ver.: always
 * @param dstAddr Ver.: always
 * @param srcEndpoint Ver.: always
 * @param dstEndpoint Ver.: always
 *
 * @return bool true is success
 *
 */
bool emberAfDeviceManagementClusterSendRequestNewPasswordResponse(uint8_t passwordType,
                                                                  EmberNodeId dstAddr,
                                                                  uint8_t srcEndpoint,
                                                                  uint8_t dstEndpoint);

/** @} */ // end of name API
/** @} */ // end of device-management-server
