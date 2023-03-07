/***************************************************************************//**
 * @file
 * @brief APIs and defines for the IAS Zone Client plugin, which keeps track of
 *        IAS Zone servers.
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
 * @defgroup ias-zone-client IAS Zone Client
 * @ingroup component cluster
 * @brief API and Callbacks for the IAS Zone Cluster Client Component
 *
 * An implementation of a client for IAS Zone that configures and keeps
 * track of IAS Zone Servers. It requires extending to handle server
 * notifications that might trigger some behavior on the
 * client (e.g., alarm).
 *
 */

/**
 * @addtogroup ias-zone-client
 * @{
 */

typedef struct {
  EmberEUI64 ieeeAddress;
  EmberNodeId nodeId;
  uint16_t zoneType;
  uint16_t zoneStatus;
  uint8_t zoneState;
  uint8_t endpoint;
  uint8_t zoneId;
} IasZoneDevice;

extern IasZoneDevice emberAfIasZoneClientKnownServers[];

#define NO_INDEX 0xFF
#define UNKNOWN_ENDPOINT 0

#define UNKNOWN_ZONE_ID 0xFF

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup ias_zon_client_cb IAS Zone Client
 * @ingroup af_callback
 * @brief Callbacks for IAS Zone Client Component
 *
 */

/**
 * @addtogroup ias_zon_client_cb
 * @{
 */

/** @brief Callback for IAS zone client ZDO
 *
 * @param emberNodeID node ID Ver.: always
 * @param apsFrame Ver.: always
 * @param message Ver.: always
 * @param length Ver.: always
 *
 */
void emberAfPluginIasZoneClientZdoCallback(EmberNodeId emberNodeId,
                                           EmberApsFrame* apsFrame,
                                           uint8_t* message,
                                           uint16_t length);

/** @brief Callback for IAS zone client write attributes
 *
 * @param clusterId Ver.: always
 * @param bufffer Ver.: always
 * @param bufLen
 *
 */
void emberAfPluginIasZoneClientWriteAttributesResponseCallback(EmberAfClusterId clusterId,
                                                               uint8_t * buffer,
                                                               uint16_t bufLen);

/** @brief Callback for IAS zone read attributes
 *
 * @param clusterId Ver.: always
 * @param buffer Ver.: always
 * @param bufLen Ver.: always
 *
 */
void emberAfPluginIasZoneClientReadAttributesResponseCallback(EmberAfClusterId clusterId,
                                                              uint8_t * buffer,
                                                              uint16_t bufLen);

/** @} */ // end of ias_zon_client_cb
/** @} */ // end of name Callbacks
/** @} */ // end of ias-zone-client

void emAfClearServers(void);
