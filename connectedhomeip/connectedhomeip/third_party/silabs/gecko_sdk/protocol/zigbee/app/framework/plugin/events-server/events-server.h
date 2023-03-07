/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Events Server plugin, which implements the
 *        server side of the Events cluster.
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

#ifdef UC_BUILD
#include "events-server-config.h"
#include "sl_component_catalog.h"
#ifdef SL_CATALOG_ZIGBEE_GAS_PROXY_FUNCTION_PRESENT
#include "gas-proxy-function-config.h"
#endif
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_GAS_PROXY_FUNCTION
#define SL_CATALOG_ZIGBEE_GAS_PROXY_FUNCTION_PRESENT
#endif
#ifdef EMBER_AF_PLUGIN_GBCS_COMPATIBILITY
#define SL_CATALOG_ZIGBEE_GBCS_COMPATIBILITY_PRESENT
#endif
#endif // UC_BUILD

/**
 * @defgroup events-server  Events Server
 * @ingroup component cluster
 * @brief API and Callbacks for the Events Server Cluster Component
 *
 */

/**
 * @addtogroup events-server
 * @{
 */

typedef struct {
  uint16_t eventId;
  uint32_t eventTime;
  uint8_t eventData[EMBER_AF_PLUGIN_EVENTS_SERVER_EVENT_DATA_LENGTH + 1];
} EmberAfEvent;

#define ZCL_EVENTS_INVALID_INDEX 0xFF

#if defined(SL_CATALOG_ZIGBEE_GAS_PROXY_FUNCTION_PRESENT)
// GBCS Alert Codes
#define GBCS_ALERT_BILLING_DATA_LOG_UPDATED       (0x800A)
#define GBCS_EVENT_ID_UNAUTHD_COMM_ACC_ATT        (0x803E)
#define GBCS_EVENT_ID_EVENT_LOG_CLEARED           (0x8052)
#define GBCS_EVENT_ID_FAILED_AUTH                 (0x8053)
#define GBCS_EVENT_ID_IMM_HAN_CMD_RXED_ACTED      (0x8054)
#define GBCS_EVENT_ID_IMM_HAN_CMD_RXED_NOT_ACTED  (0x8055)
#define GBCS_EVENT_ID_FUT_HAN_CMD_ACTED           (0x8066)
#define GBCS_EVENT_ID_FUT_HAN_CMD_NOT_ACTED       (0x8067)
#define GBCS_EVENT_ID_GPF_DEVICE_LOG_CHGD         (0x8071)
#define GBCS_EVENT_ID_GSME_CMD_NOT_RETRVD         (0x809D)
#define GBCS_EVENT_LOG_ID_GSME_EVENT_LOG          (0x6)
#define GBCS_EVENT_LOG_ID_GSME_SECURITY_EVENT_LOG (0x7)
#endif

/**
 * @name API
 * @{
 */

/**
 * @brief Clear all events in the specified event log.
 *
 * @param endpoint The endpoint for which the event log will be cleared.
 * @param logId The log to be cleared.
 * @return True if the log was successfully cleared or false if logId is invalid.
 **/
bool emberAfEventsServerClearEventLog(uint8_t endpoint,
                                      EmberAfEventLogId logId);

/**
 * @brief Print all events in the specified event log.
 *
 * @param endpoint The endpoint for which the event log will be printed.
 * @param logId The log to be printed.
 **/
void emberAfEventsServerPrintEventLog(uint8_t endpoint,
                                      EmberAfEventLogId logId);

/**
 * @brief Print an event.
 *
 * @param event The event to print.
 **/
void emberAfEventsServerPrintEvent(const EmberAfEvent *event);

/**
 * @brief Get an event from the specified event log.
 *
 * This function can be used to get an event at a specific location in
 * the specified log.
 *
 * @param endpoint The relevant endpoint.
 * @param logId The relevant log.
 * @param index The index in the event log.
 * @param event The ::EmberAfEvent structure describing the event.
 * @return True if the event was found or false if the index is invalid.
 */
bool emberAfEventsServerGetEvent(uint8_t endpoint,
                                 EmberAfEventLogId logId,
                                 uint8_t index,
                                 EmberAfEvent *event);

/**
 * @brief Store an event in the specified event log.
 *
 * This function is used to set an event at a specific location in
 * the specified log.
 *
 * @param endpoint The relevant endpoint.
 * @param logId The relevant log.
 * @param index The index in the event log.
 * @param event The ::EmberAfEvent structure describing the event.
 * If NULL, the event is removed from the server.
 * @return True if the event was set or removed or false if the index is invalid.
 */
bool emberAfEventsServerSetEvent(uint8_t endpoint,
                                 EmberAfEventLogId logId,
                                 uint8_t index,
                                 const EmberAfEvent *event);

/**
 * @brief Add an event to the specified event log.
 *
 * This function is used to add an event at the next available location in
 * the specified log. After the event log is full, new events will start
 * overwriting old events at the beginning of the table.
 *
 * @param endpoint The relevant endpoint.
 * @param logId The relevant log.
 * @param event The ::EmberAfEvent structure describing the event.
 * @return the index of the location in the log where the event was added or
 * ZCL_EVENTS_INVALID_INDEX if the specified event log is full.
 */
uint8_t emberAfEventsServerAddEvent(uint8_t endpoint,
                                    EmberAfEventLogId logId,
                                    const EmberAfEvent *event);

/**
 * @brief Publish an event.
 *
 * This function will locate the event in the specified log at the specified
 * location and using the information from the event build and send a
 * PublishEvent command.
 *
 * @param nodeId The destination nodeId.
 * @param srcEndpoint The source endpoint.
 * @param dstEndpoint The destination endpoint.
 * @param logId The relevant log.
 * @param index The index in the event log.
 * @param eventControl Actions to be taken regarding this event. For example,
 * Report event to HAN and/or Report event to WAN.
 **/
void emberAfEventsServerPublishEventMessage(EmberNodeId nodeId,
                                            uint8_t srcEndpoint,
                                            uint8_t dstEndpoint,
                                            EmberAfEventLogId logId,
                                            uint8_t index,
                                            uint8_t eventControl);
/** @} */ // end of name API

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup events_server_cb Events Server
 * @ingroup af_callback
 * @brief Callbacks for Events Server Component
 *
 */

/**
 * @addtogroup events_server_cb
 * @{
 */

/** @brief OK to clear the log.
 *
 * This function is called by the Events server plugin whenever a Clear Event
 * Log Request command is received. The application should return true if it is
 * OK to clear the given log and false otherwise. If the request is to clear
 * all logs (i.e., logId == EMBER_ZCL_EVENT_LOG_ID_ALL_LOGS) and the application
 * allows all logs to be cleared, the application should return true. If
 * the application does not allow all logs to be cleared, it should return false
 * in which case the plugin will subsequently call this callback for each event
 * log allowing the application to selectively choose which event logs are OK
 * to be cleared.
 *
 * @param logId The identifier of the log requested to be cleared. Ver.: always
 */
bool emberAfPluginEventsServerOkToClearLogCallback(EmberAfEventLogId logId);

/** @brief Log data updated.
 *
 * This function is called by the Events server plugin when any modification to
 * the plugin's event logs has been made. The argument will hint the ZCL
 * command that might be used to triggered the data change. If null,
 * logging data have been updated through other means, e.g., CLI.
 *
 * @param cmd ZCL command Ver.: always
 */
void emberAfPluginEventsServerLogDataUpdatedCallback(const EmberAfClusterCommand *cmd);
/** @} */ // end of name events_server_cb
/** @} */ // end of name Callbacks
/** @} */ // end of events-server
