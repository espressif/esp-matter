/***************************************************************************//**
 * @file
 * @brief Definitions for the Reporting plugin.
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

#include "af.h"

/**
 * @addtogroup global_callback
 * @{
 */

/** @brief Add To Current App Tasks
 *
 * This function is only useful to sleepy end devices.  This function will note
 * the passed item as part of a set of tasks the application has outstanding
 * (e.g. message sent requiring APS acknwoledgement).  This will affect how the
 * application behaves with regard to sleeping and polling.  Until the
 * outstanding task is completed, the device may poll more frequently and sleep
 * less often.
 *
 * @param tasks   Ver.: always
 */
void emberAfAddToCurrentAppTasksCallback(EmberAfApplicationTask tasks);
/** @brief Allow Network Write Attribute
 *
 * This function is called by the application framework before it writes an
 * attribute in response to a write attribute request from an external device.
 * The value passed into this callback is the value to which the attribute is to
 * be set by the framework.
        Example:	In mirroring simple metering data
 * on an Energy Services Interface (ESI) (formerly called Energy Service Portal
 * (ESP) in SE 1.0).), a mirrored simple meter needs to write read-only
 * attributes on its mirror. The-meter-mirror sample application, located in
 * app/framework/sample-apps, uses this callback to allow the mirrored device to
 * write simple metering attributes on the mirror regardless of the fact that
 * most simple metering attributes are defined as read-only by the ZigBee
 * specification.
        Note:	The ZCL specification does not (as of this
 * writing) specify any permission-level security for writing writeable
 * attributes. As far as the ZCL specification is concerned, if an attribute is
 * writeable, any device that has a link key for the device should be able to
 * write that attribute. Furthermore if an attribute is read only, it should not
 * be written over the air. Thus, if you implement permissions for writing
 * attributes as a feature, you MAY be operating outside the specification. This
 * is unlikely to be a problem for writing read-only attributes, but it may be a
 * problem for attributes that are writeable according to the specification but
 * restricted by the application implementing this callback.
 *
 * @param endpoint   Ver.: always
 * @param clusterId   Ver.: always
 * @param attributeId   Ver.: always
 * @param mask   Ver.: always
 * @param manufacturerCode   Ver.: always
 * @param value   Ver.: always
 * @param type   Ver.: always
 */
EmberAfAttributeWritePermission emberAfAllowNetworkWriteAttributeCallback(int8u endpoint,
                                                                          EmberAfClusterId clusterId,
                                                                          EmberAfAttributeId attributeId,
                                                                          int8u mask,
                                                                          int16u manufacturerCode,
                                                                          int8u* value,
                                                                          int8u type);
/** @brief Attribute Read Access
 *
 * This function is called whenever the Application Framework needs to check
 * access permission for an attribute read.
 *
 * @param endpoint   Ver.: always
 * @param clusterId   Ver.: always
 * @param manufacturerCode   Ver.: always
 * @param attributeId   Ver.: always
 */
bool emberAfAttributeReadAccessCallback(int8u endpoint,
                                        EmberAfClusterId clusterId,
                                        int16u manufacturerCode,
                                        int16u attributeId);
/** @brief Attribute Write Access
 *
 * This function is called whenever the Application Framework needs to check
 * access permission for an attribute write.
 *
 * @param endpoint   Ver.: always
 * @param clusterId   Ver.: always
 * @param manufacturerCode   Ver.: always
 * @param attributeId   Ver.: always
 */
bool emberAfAttributeWriteAccessCallback(int8u endpoint,
                                         EmberAfClusterId clusterId,
                                         int16u manufacturerCode,
                                         int16u attributeId);
/** @brief Clear Report Table
 *
 * This function is called by the framework when the application should clear
 * the report table.
 *
 */
EmberStatus emberAfClearReportTableCallback(void);
/** @brief Cluster Init
 *
 * This function is called when a specific cluster is initialized. It gives the
 * application an opportunity to take care of cluster initialization procedures.
 * It is called exactly once for each endpoint where cluster is present.
 *
 * @param endpoint   Ver.: always
 * @param clusterId   Ver.: always
 */
void emberAfClusterInitCallback(int8u endpoint,
                                EmberAfClusterId clusterId);
/** @brief Cluster Security Custom
 *
 * This callback is fired when determining if APS encryption is required for a
 * cluster outside of the specification's required clusters.  In other words,
 * for the Smart Energy profile this would be a cluster beyond the list that
 * normally requires APS encryption.
 *
 * @param profileId The profile ID  Ver.: always
 * @param clusterId The cluster ID  Ver.: always
 * @param incoming Whether this is an incoming or outgoing message.  Ver.:
 * always
 * @param commandId The ZCL command ID being sent/received.  Ver.: always
 */
bool emberAfClusterSecurityCustomCallback(EmberAfProfileId profileId,
                                          EmberAfClusterId clusterId,
                                          bool incoming,
                                          int8u commandId);
/** @brief Configure Reporting Command
 *
 * This function is called by the application framework when a Configure
 * Reporting command is received from an external device.  The Configure
 * Reporting command contains a series of attribute reporting configuration
 * records.  The application should return true if the message was processed or
 * false if it was not.
 *
 * @param cmd   Ver.: always
 */
bool emberAfConfigureReportingCommandCallback(const EmberAfClusterCommand *cmd);
/** @brief Configure Reporting Response
 *
 * This function is called by the application framework when a Configure
 * Reporting Response command is received from an external device.  The
 * application should return true if the message was processed or false if it
 * was not.
 *
 * @param clusterId The cluster identifier of this response.  Ver.: always
 * @param buffer Buffer containing the list of attribute status records.  Ver.:
 * always
 * @param bufLen The length in bytes of the list.  Ver.: always
 */
bool emberAfConfigureReportingResponseCallback(EmberAfClusterId clusterId,
                                               int8u *buffer,
                                               int16u bufLen);
/** @brief Default Response
 *
 * This function is called by the application framework when a Default Response
 * command is received from an external device.  The application should return
 * true if the message was processed or false if it was not.
 *
 * @param clusterId The cluster identifier of this response.  Ver.: always
 * @param commandId The command identifier to which this is a response.  Ver.:
 * always
 * @param status Specifies either SUCCESS or the nature of the error that was
 * detected in the received command.  Ver.: always
 */
bool emberAfDefaultResponseCallback(EmberAfClusterId clusterId,
                                    int8u commandId,
                                    EmberAfStatus status);
/** @brief Discover Attributes Response
 *
 * This function is called by the application framework when a Discover
 * Attributes Response or Discover Attributes Extended Response command is
 * received from an external device.  The Discover Attributes Response command
 * contains a bool indicating if discovery is complete and a list of zero or
 * more attribute identifier/type records. The final argument indicates whether
 * the response is in the extended format or not.  The application should return
 * true if the message was processed or false if it was not.
 *
 * @param clusterId The cluster identifier of this response.  Ver.: always
 * @param discoveryComplete Indicates whether there are more attributes to be
 * discovered.  true if there are no more attributes to be discovered.  Ver.:
 * always
 * @param buffer Buffer containing the list of attribute identifier/type
 * records.  Ver.: always
 * @param bufLen The length in bytes of the list.  Ver.: always
 * @param extended Indicates whether the response is in the extended format or
 * not.  Ver.: always
 */
bool emberAfDiscoverAttributesResponseCallback(EmberAfClusterId clusterId,
                                               bool discoveryComplete,
                                               int8u *buffer,
                                               int16u bufLen,
                                               bool extended);
/** @brief Discover Commands Generated Response
 *
 * This function is called by the framework when Discover Commands Generated
 * Response is received.
 *
 * @param clusterId The cluster identifier of this response.  Ver.: always
 * @param manufacturerCode Manufacturer code  Ver.: always
 * @param discoveryComplete Indicates whether there are more commands to be
 * discovered.  Ver.: always
 * @param commandIds Buffer containing the list of command identifiers.  Ver.:
 * always
 * @param commandIdCount The length of bytes of the list, whish is the same as
 * the number of identifiers.  Ver.: always
 */
bool emberAfDiscoverCommandsGeneratedResponseCallback(EmberAfClusterId clusterId,
                                                      int16u manufacturerCode,
                                                      bool discoveryComplete,
                                                      int8u *commandIds,
                                                      int16u commandIdCount);
/** @brief Discover Commands Received Response
 *
 * This function is called by the framework when Discover Commands Received
 * Response is received.
 *
 * @param clusterId The cluster identifier of this response.  Ver.: always
 * @param manufacturerCode Manufacturer code  Ver.: always
 * @param discoveryComplete Indicates whether there are more commands to be
 * discovered.  Ver.: always
 * @param commandIds Buffer containing the list of command identifiers.  Ver.:
 * always
 * @param commandIdCount The length of bytes of the list, whish is the same as
 * the number of identifiers.  Ver.: always
 */
bool emberAfDiscoverCommandsReceivedResponseCallback(EmberAfClusterId clusterId,
                                                     int16u manufacturerCode,
                                                     bool discoveryComplete,
                                                     int8u *commandIds,
                                                     int16u commandIdCount);
/** @brief Eeprom Init
 *
 * Tells the system to initialize the EEPROM if it is not already initialized.
 *
 */
void emberAfEepromInitCallback(void);
/** @brief Eeprom Note Initialized State
 *
 * Records the state of the EEPROM so that an intelligent driver (like the
 * EEPROM plugin) can re-initialize the driver prior to any calls to it.
 *
 * @param state The state of the EEPROM, false=re-initalization needed,
 * true=no-re-init needed  Ver.: always
 */
void emberAfEepromNoteInitializedStateCallback(bool state);
/** @brief Eeprom Shutdown
 *
 * Tells the system to shutdown the EEPROM if it is not already shutdown.
 *
 */
void emberAfEepromShutdownCallback(void);

/** @brief External Attribute Read
 *
 * Like emberAfExternalAttributeWriteCallback above, this function is called
 * when the framework needs to read an attribute that is not stored within the
 * Application Framework's data structures.
        All of the important
 * information about the attribute itself is passed as a pointer to an
 * EmberAfAttributeMetadata struct, which is stored within the application and
 * used to manage the attribute. A complete description of the
 * EmberAfAttributeMetadata struct is provided in
 * app/framework/include/af-types.h
        This function assumes that the
 * application is able to read the attribute, write it into the passed buffer,
 * and return immediately. Any attributes that require a state machine for
 * reading and writing are not really candidates for externalization at the
 * present time. The Application Framework does not currently include a state
 * machine for reading or writing attributes that must take place across a
 * series of application ticks. Attributes that cannot be read in a timely
 * manner should be stored within the Application Framework and updated
 * occasionally by the application code from within the
 * emberAfMainTickCallback.
        If the application was successfully able to
 * read the attribute and write it into the passed buffer, it should return a
 * value of EMBER_ZCL_STATUS_SUCCESS. Ensure that the size of the externally
 * managed attribute value is smaller than what the buffer can hold. In the case
 * of a buffer overflow throw an appropriate error such as
 * EMBER_ZCL_STATUS_INSUFFICIENT_SPACE. Any other return value indicates the
 * application was not able to read the attribute.
 *
 * @param endpoint   Ver.: always
 * @param clusterId   Ver.: always
 * @param attributeMetadata   Ver.: always
 * @param manufacturerCode   Ver.: always
 * @param buffer   Ver.: always
 * @param maxReadLength   Ver.: always
 */
EmberAfStatus emberAfExternalAttributeReadCallback(int8u endpoint,
                                                   EmberAfClusterId clusterId,
                                                   EmberAfAttributeMetadata *attributeMetadata,
                                                   int16u manufacturerCode,
                                                   int8u *buffer,
                                                   int16u maxReadLength);
/** @brief External Attribute Write
 *
 * This function is called whenever the Application Framework needs to write an
 * attribute which is not stored within the data structures of the Application
 * Framework itself. One of the new features in Version 2 is the ability to
 * store attributes outside the Framework. This is particularly useful for
 * attributes that do not need to be stored because they can be read off the
 * hardware when they are needed, or are stored in some central location used by
 * many modules within the system. In this case, you can indicate that the
 * attribute is stored externally. When the framework needs to write an external
 * attribute, it makes a call to this callback.
        This callback is very
 * useful for host micros which need to store attributes in persistent memory.
 * Because each host micro (used with an Ember NCP) has its own type of
 * persistent memory storage, the Application Framework does not include the
 * ability to mark attributes as stored in flash the way that it does for an SoC
 * architecture. On a host micro, any attributes that need to be stored in
 * persistent memory should be marked as external and accessed through the
 * external read and write callbacks. Any host code associated with the
 * persistent storage should be implemented within this callback.
        All of
 * the important information about the attribute itself is passed as a pointer
 * to an EmberAfAttributeMetadata struct, which is stored within the application
 * and used to manage the attribute. A complete description of the
 * EmberAfAttributeMetadata struct is provided in
 * app/framework/include/af-types.h.
        This function assumes that the
 * application is able to write the attribute and return immediately. Any
 * attributes that require a state machine for reading and writing are not
 * candidates for externalization at the present time. The Application Framework
 * does not currently include a state machine for reading or writing attributes
 * that must take place across a series of application ticks. Attributes that
 * cannot be written immediately should be stored within the Application
 * Framework and updated occasionally by the application code from within the
 * emberAfMainTickCallback.
        If the application was successfully able to
 * write the attribute, it returns a value of EMBER_ZCL_STATUS_SUCCESS. Any
 * other return value indicates the application was not able to write the
 * attribute.
 *
 * @param endpoint   Ver.: always
 * @param clusterId   Ver.: always
 * @param attributeMetadata   Ver.: always
 * @param manufacturerCode   Ver.: always
 * @param buffer   Ver.: always
 */
EmberAfStatus emberAfExternalAttributeWriteCallback(int8u endpoint,
                                                    EmberAfClusterId clusterId,
                                                    EmberAfAttributeMetadata *attributeMetadata,
                                                    int16u manufacturerCode,
                                                    int8u *buffer);
/** @brief Find Unused Pan Id And Form
 *
 * This function is called by the framework to search for an unused PAN id and
 * form a new network.  The application should return EMBER_SUCCESS if the
 * operation was initiated successfully.
 *
 */
EmberStatus emberAfFindUnusedPanIdAndFormCallback(void);
/** @brief Get Current App Tasks
 *
 * This function is only useful to sleepy end devices.  This function will
 * return the set of tasks the application has outstanding.  These tasks affect
 * how the application behaves with regard to sleeping and polling.
 *
 */
EmberAfApplicationTask emberAfGetCurrentAppTasksCallback(void);
/** @brief Get Current Poll Control
 *
 * This function will retrieve the current poll control that the system is using
 * for the current network.  This is determined by examining all the scheduled
 * events and obtaining the most restrictive poll control context across all
 * events.  The most restrictive poll control is EMBER_AF_SHORT_POLL followed by
 * EMBER_AF_LONG_POLL.
 *
 */
EmberAfEventPollControl emberAfGetCurrentPollControlCallback(void);
/** @brief Get Current Poll Interval Ms
 *
 * This function is only useful to end devices.  This function will return the
 * current poll interval (in milliseconds) for the current network.  This
 * interval is the maximum amount of time a child is currently waiting between
 * polls of its parent.
 *
 */
int32u emberAfGetCurrentPollIntervalMsCallback(void);
/** @brief Get Current Poll Interval Qs
 *
 * This function is only useful to end devices.  This function will return the
 * current poll interval (in quarter seconds) for the current network.  This
 * interval is the maximum amount of time a child is currently waiting between
 * polls of its parent.
 *
 */
int32u emberAfGetCurrentPollIntervalQsCallback(void);
/** @brief Get Current Sleep Control
 *
 * This function will retrieve the current sleep control that the system is
 * using.  This is determined by examining all the scheduled events and
 * obtaining the most restrictive sleep control context across all events.  The
 * most restrictive sleep control is EMBER_AF_STAY_AWAKE followed by
 * EMBER_AF_OK_TO_SLEEP.
 *
 */
EmberAfEventSleepControl emberAfGetCurrentSleepControlCallback(void);
/** @brief Get Current Time
 *
 * This callback is called when device attempts to get current time from the
 * hardware. If this device has means to retrieve exact time, then this method
 * should implement it. If the callback can't provide the exact time it should
 * return 0 to indicate failure. Default action is to return 0, which indicates
 * that device does not have access to real time.
 *
 */
int32u emberAfGetCurrentTimeCallback(void);
/** @brief Get Default Poll Control
 *
 * This function will retrieve the default poll control for the current network
 * as previously set by emberAfSetDefaultPollControlCallback().  The default
 * poll control will limit whether the network can long poll.
 *
 */
EmberAfEventPollControl emberAfGetDefaultPollControlCallback(void);
/** @brief Get Default Sleep Control
 *
 * This function will retrieve the default sleep control the system is using as
 * previously set by emberAfSetDefaultSleepControlCallback().  The default sleep
 * control will limit whether the device can sleep.
 *
 */
EmberAfEventSleepControl emberAfGetDefaultSleepControlCallback(void);
/** @brief Get Endpoint By Index
 *
 * Get the endpoint number based on the passed index.  By default the framework
 * handles this by managing endpoints based on the precompiled configuration
 * defined in AppBuilder.  This callback can override this behavior at runtime
 * and provide additional endpoints or different data than the compiled values.
 * If the index is overridden than the callback shall return true and set the
 * endpointReturn parameter accordingly. A value of 0xFF means the endpoint
 * doesn't exist at that index.
        Otherwise false must be returned by the
 * callback and the default framework behavior will be executed.  This is only
 * applicable to the SOC devices.
 *
 * @param index The index of the endpoint.  Ver.: always
 * @param endpointReturn The value of endpoint.  Ver.: always
 */
bool emberAfGetEndpointByIndexCallback(int8u index,
                                       int8u* endpointReturn);
/** @brief Get Endpoint Description
 *
 * This callback is called by the framework whenever it receives a ZDO request
 * to enumerate the details about an endpoint.  By default the framework
 * provides the information based on the precompiled endpoint information as
 * defined in AppBuilder.  This callback can override that behavior at runtime
 * and return different information.  If the endpoint information is being
 * overridden then the callback must return true.  Otherwise it should return
 * false, which allows the framework to perform its default behavior.  This is
 * only applicable to SOC devices.
 *
 * @param endpoint The endpoint number that is being queried.  Ver.: always
 * @param result This is a pointer to a data structure where the endpoint
 * information is written if the callback is providing the information.  Ver.:
 * always
 */
bool emberAfGetEndpointDescriptionCallback(int8u endpoint,
                                           EmberEndpointDescription* result);
/** @brief Get Endpoint Info
 *
 * This function is a callback to an application implemented endpoint that
 * operates outside the normal application framework.  When the framework wishes
 * to perform operations with that endpoint it uses this callback to retrieve
 * the endpoint's information.  If the endpoint exists and the application can
 * provide data then true shall be returned.  Otherwise the callback must return
 * false.
 *
 * @param endpoint The endpoint to retrieve data for.  Ver.: always
 * @param returnNetworkIndex The index corresponding to the ZigBee network the
 * endpoint belongs to.  If not using a multi-network device, 0 must be
 * returned.  Otherwise on a multi-network device the stack will switch to this
 * network before sending the message.  Ver.: always
 * @param returnEndpointInfo A pointer to a data struct that will be written
 * with information about the endpoint.  Ver.: always
 */
bool emberAfGetEndpointInfoCallback(int8u endpoint,
                                    int8u* returnNetworkIndex,
                                    EmberAfEndpointInfoStruct* returnEndpointInfo);
/** @brief Get Form And Join Extended Pan Id
 *
 * This callback is called by the framework to get the extended PAN ID used by
 * the current network for forming and joining.  The extended PAN ID used for
 * forming and joining is not necessarily the same extended PAN ID actually in
 * use on the network.
 *
 * @param resultLocation   Ver.: always
 */
void emberAfGetFormAndJoinExtendedPanIdCallback(int8u *resultLocation);
/** @brief Get Long Poll Interval Ms
 *
 * This function is only useful to end devices.  This function will return the
 * long poll interval (in milliseconds) for the current network.  This interval
 * is the maximum amount of time a child will wait between polls of its parent
 * when it is not expecting data.
 *
 */
int32u emberAfGetLongPollIntervalMsCallback(void);
/** @brief Get Long Poll Interval Qs
 *
 * This function is only useful to end devices.  This function will return the
 * long poll interval (in quarter seconds) for the current network.  This
 * interval is the maximum amount of time a child will wait between polls of its
 * parent when it is not expecting data.
 *
 */
int32u emberAfGetLongPollIntervalQsCallback(void);
/** @brief Get Short Poll Interval Ms
 *
 * This function is only useful to sleepy end devices.  This function will
 * return the short poll interval (in milliseconds) for the current network.
 * This interval is the maximum amount of time a child will wait between polls
 * of its parent when it is expecting data.
 *
 */
int16u emberAfGetShortPollIntervalMsCallback(void);
/** @brief Get Short Poll Interval Qs
 *
 * This function is only useful to sleepy end devices.  This function will
 * return the short poll interval (in quarter seconds) for the current network.
 * This interval is the maximum amount of time a child will wait between polls
 * of its parent when it is expecting data.
 *
 */
int16u emberAfGetShortPollIntervalQsCallback(void);
/** @brief Get Source Route Overhead
 *
 * This function is called by the framework to determine the overhead required
 * in the network frame for source routing to a particular destination.
 *
 * @param destination The node id of the destination  Ver.: always
 */
int8u emberAfGetSourceRouteOverheadCallback(EmberNodeId destination);
/** @brief Get Wake Timeout Bitmask
 *
 * This function is only useful to sleepy end devices.  This function will
 * return the wake timeout bitmask for the current network.  The bitmask
 * determines which tasks will timeout automatically and which tasks require
 * manual removal from the task list.
 *
 */
EmberAfApplicationTask emberAfGetWakeTimeoutBitmaskCallback(void);
/** @brief Get Wake Timeout Ms
 *
 * This function is only useful to sleepy end devices.  This function will
 * return the wake timeout (in milliseconds) for the current network.  This
 * timeout is the maximum amount of time a child will wait for a task in the
 * wake bitmask to finish.  While waiting, the device will short poll.
 *
 */
int16u emberAfGetWakeTimeoutMsCallback(void);
/** @brief Get Wake Timeout Qs
 *
 * This function is only useful to sleepy end devices.  This function will
 * return the wake timeout (in quarter seconds) for the current network.  This
 * timeout is the maximum amount of time a child will wait for a task in the
 * wake bitmask to finish.  While waiting, the device will short poll.
 *
 */
int16u emberAfGetWakeTimeoutQsCallback(void);
/** @brief Hal Button Isr
 *
 * This callback is called by the framework whenever a button is pressed on the
 * device. This callback is called within ISR context.
 *
 * @param button The button which has changed state, either BUTTON0 or BUTTON1
 * as defined in the appropriate BOARD_HEADER.  Ver.: always
 * @param state The new state of the button referenced by the button parameter,
 * either ::BUTTON_PRESSED if the button has been pressed or ::BUTTON_RELEASED
 * if the button has been released.  Ver.: always
 */
void emberAfHalButtonIsrCallback(int8u button,
                                 int8u state);
/** @brief Incoming Packet Filter
 *
 * ** REQUIRES INCLUDING THE PACKET-HANDOFF PLUGIN **

        This is called by
 * the Packet Handoff plugin when the stack receives a packet from one of the
 * protocol layers specified in ::EmberZigbeePacketType.

        The packetType
 * argument is one of the values of the ::EmberZigbeePacketType enum. If the
 * stack receives an 802.15.4 MAC beacon, it will call this function with the
 * packetType argument set to ::EMBER_ZIGBEE_PACKET_TYPE_BEACON.

        The
 * implementation of this callback may alter the data contained in packetData,
 * modify options and flags in the auxillary data, or consume the packet itself,
 * either sending the message, or discarding it as it sees fit.
 *
 * @param packetType the type of packet and associated protocol layer  Ver.:
 * always
 * @param packetData flat buffer containing the packet data associated with the
 * packet type  Ver.: always
 * @param size_p a pointer containing the size value of the packet  Ver.: always
 * @param data auxillary data included with the packet  Ver.: always
 */
EmberPacketAction emberAfIncomingPacketFilterCallback(EmberZigbeePacketType packetType,
                                                      int8u* packetData,
                                                      int8u* size_p,
                                                      void* data);
/** @brief Initiate Inter Pan Key Establishment
 *
 * This function is called by the framework to initiate key establishment with a
 * remote device on a different PAN.  The application should return
 * EMBER_SUCCESS if key establishment was initiated successfully.  The
 * application should call ::emberAfInterPanKeyEstablishmentCallback as events
 * occur.
 *
 * @param panId The PAN id of the remote device.  Ver.: always
 * @param eui64 The EUI64 of the remote device.  Ver.: always
 */
EmberStatus emberAfInitiateInterPanKeyEstablishmentCallback(EmberPanId panId,
                                                            const EmberEUI64 eui64);
/** @brief Initiate Key Establishment
 *
 * This function is called by the framework to initiate key establishment with a
 * remote device.  The application should return EMBER_SUCCESS if key
 * establishment was initiated successfully.  The application should call
 * ::emberAfKeyEstablishmentCallback as events occur.
 *
 * @param nodeId The node id of the remote device.  Ver.: always
 * @param endpoint The endpoint on the remote device.  Ver.: always
 */
EmberStatus emberAfInitiateKeyEstablishmentCallback(EmberNodeId nodeId,
                                                    int8u endpoint);
/** @brief Initiate Partner Link Key Exchange
 *
 * This function is called by the framework to initiate a partner link key
 * exchange with a remote device.  The application should return EMBER_SUCCESS
 * if the partner link key exchange was initiated successfully.  When the
 * partner link key exchange completes, the application should call the given
 * callback.
 *
 * @param target The node id of the remote device.  Ver.: always
 * @param endpoint The key establishment endpoint of the remote device.  Ver.:
 * always
 * @param callback The callback that should be called when the partner link key
 * exchange completse.  Ver.: always
 */
EmberStatus emberAfInitiatePartnerLinkKeyExchangeCallback(EmberNodeId target,
                                                          int8u endpoint,
                                                          EmberAfPartnerLinkKeyExchangeCallback *callback);
/** @brief Inter Pan Key Establishment
 *
 * A callback by the key-establishment code to indicate an event has occurred.
 * For error codes this is purely a notification.  For non-error status codes
 * (besides LINK_KEY_ESTABLISHED), it is the application's chance to allow or
 * disallow the operation.  If the application returns true then the key
 * establishment is allowed to proceed.  If it returns false, then key
 * establishment is aborted.  LINK_KEY_ESTABLISHED is a notification of success.
 *
 * @param status   Ver.: always
 * @param amInitiator   Ver.: always
 * @param panId   Ver.: always
 * @param eui64   Ver.: always
 * @param delayInSeconds   Ver.: always
 */
bool emberAfInterPanKeyEstablishmentCallback(EmberAfKeyEstablishmentNotifyMessage status,
                                             bool amInitiator,
                                             EmberPanId panId,
                                             const EmberEUI64 eui64,
                                             int8u delayInSeconds);
/** @brief Interpan Send Message
 *
 * This function will send a raw MAC message with interpan frame format using
 * the passed parameters.
 *
 * @param header Interpan header info  Ver.: always
 * @param messageLength The length of the message received or to send  Ver.:
 * always
 * @param message The message data received or to send.  Ver.: always
 */
EmberStatus emberAfInterpanSendMessageCallback(EmberAfInterpanHeader* header,
                                               int16u messageLength,
                                               int8u* message);
/** @brief Key Establishment
 *
 * A callback by the key-establishment code to indicate an event has occurred.
 * For error codes this is purely a notification.  For non-error status codes
 * (besides LINK_KEY_ESTABLISHED), it is the application's chance to allow or
 * disallow the operation.  If the application returns true then the key
 * establishment is allowed to proceed.  If it returns false, then key
 * establishment is aborted.  LINK_KEY_ESTABLISHED is a notification of success.
 *
 * @param status   Ver.: always
 * @param amInitiator   Ver.: always
 * @param partnerShortId   Ver.: always
 * @param delayInSeconds   Ver.: always
 */
bool emberAfKeyEstablishmentCallback(EmberAfKeyEstablishmentNotifyMessage status,
                                     bool amInitiator,
                                     EmberNodeId partnerShortId,
                                     int8u delayInSeconds);
/** @brief Main Init
 *
 * This function is called from the application's main function. It gives the
 * application a chance to do any initialization required at system startup. Any
 * code that you would normally put into the top of the application's main()
 * routine should be put into this function. This is called before the clusters,
 * plugins, and the network are initialized so some functionality is not yet
 * available.
        Note: No callback in the Application Framework is
 * associated with resource cleanup. If you are implementing your application on
 * a Unix host where resource cleanup is a consideration, we expect that you
 * will use the standard Posix system calls, including the use of atexit() and
 * handlers for signals such as SIGTERM, SIGINT, SIGCHLD, SIGPIPE and so on. If
 * you use the signal() function to register your signal handler, please mind
 * the returned value which may be an Application Framework function. If the
 * return value is non-null, please make sure that you call the returned
 * function from your handler to avoid negating the resource cleanup of the
 * Application Framework itself.
 *
 */
void emberAfMainInitCallback(void);
/** @brief Main Start
 *
 * This function is called at the start of main after the HAL has been
 * initialized.  The standard main function arguments of argc and argv are
 * passed in.  However not all platforms have support for main() function
 * arguments.  Those that do not are passed NULL for argv, therefore argv should
 * be checked for NULL before using it.  If the callback determines that the
 * program must exit, it should return true.  The value returned by main() will
 * be the value written to the returnCode pointer.  Otherwise the callback
 * should return false to let normal execution continue.
 *
 * @param returnCode   Ver.: always
 * @param argc   Ver.: always
 * @param argv   Ver.: always
 */
bool emberAfMainStartCallback(int* returnCode,
                              int argc,
                              char** argv);
/** @brief Main Tick
 *
 * Whenever main application tick is called, this callback will be called at the
 * end of the main tick execution.
 *
 */
void emberAfMainTickCallback(void);
/** @brief Message Sent
 *
 * This function is called by the application framework from the message sent
 * handler, when it is informed by the stack regarding the message sent status.
 * All of the values passed to the emberMessageSentHandler are passed on to this
 * callback. This provides an opportunity for the application to verify that its
 * message has been sent successfully and take the appropriate action. This
 * callback should return a bool value of true or false. A value of true
 * indicates that the message sent notification has been handled and should not
 * be handled by the application framework.
 *
 * @param type   Ver.: always
 * @param indexOrDestination   Ver.: always
 * @param apsFrame   Ver.: always
 * @param msgLen   Ver.: always
 * @param message   Ver.: always
 * @param status   Ver.: always
 */
bool emberAfMessageSentCallback(EmberOutgoingMessageType type,
                                int16u indexOrDestination,
                                EmberApsFrame* apsFrame,
                                int16u msgLen,
                                int8u* message,
                                EmberStatus status);
/** @brief Ncp Init
 *
 * This function is called when the network coprocessor is being initialized,
 * either at startup or upon reset.  It provides applications on opportunity to
 * perform additional configuration of the NCP.  The function is always called
 * twice when the NCP is initialized.  In the first invocation, memoryAllocation
 * will be true and the application should only issue EZSP commands that affect
 * memory allocation on the NCP.  For example, tables on the NCP can be resized
 * in the first call.  In the second invocation, memoryAllocation will be false
 * and the application should only issue EZSP commands that do not affect memory
 * allocation.  For example, tables on the NCP can be populated in the second
 * call.  This callback is not called on SoCs.
 *
 * @param memoryAllocation   Ver.: always
 */
void emberAfNcpInitCallback(bool memoryAllocation);
/** @brief Ncp Is Awake Isr
 *
 * This function is called IN ISR CONTEXT.  It notes that the NCP is awake after
 * sleeping.  Care should be taken to do minimal processing in this ISR handler
 * function.
 *
 */
void emberAfNcpIsAwakeIsrCallback(void);
/** @brief Network Key Update Complete
 *
 * This is called by the framework when a network key update operation started
 * by the trust center is complete.
 *
 * @param status   Ver.: always
 */
void emberAfNetworkKeyUpdateCompleteCallback(EmberStatus status);
/** @brief Ota Bootload
 *
 * The platform specific routine to bootload the device from a ZigBee
 * over-the-air upgrade file.
 *
 * @param id A pointer to the structure that contains the information about what
 * OTA image to bootload.  Ver.: always
 * @param ncpUpgradeTagId The tag ID of the upgrade data that will be used to
 * bootload the device.  Ver.: always
 */
int8u emberAfOtaBootloadCallback(const EmberAfOtaImageId* id,
                                 int16u ncpUpgradeTagId);
/** @brief Ota Client Bootload
 *
 * This callback is fired when the OTA Client recevies a command to bootload the
 * newly downloaded OTA image.  This callback will perform the platform specific
 * to bootload their device.
 *
 * @param id This is the identifier relating to the image that has been
 * downloaded and is ready for bootload.  Ver.: always
 */
void emberAfOtaClientBootloadCallback(const EmberAfOtaImageId* id);
/** @brief Ota Client Custom Verify
 *
 * This callback is executed by the OTA client after the signature verification
 * has successfully completed.  It allows the device to do its own custom
 * verification of the image (such as verifying that the EBL is intact).
 *
 * @param newVerification This indicates if a new verification should be
 * started.  Ver.: always
 * @param id This is ID of the image to be verified.  Ver.: always
 */
EmberAfImageVerifyStatus emberAfOtaClientCustomVerifyCallback(bool newVerification,
                                                              const EmberAfOtaImageId* id);
/** @brief Ota Client Download Complete
 *
 * This callback indicates that the OTA client has completed the download of a
 * file.  If the file has been completely downloaded and cryptographic checks
 * have been turned on, then those will be performed prior to this callback and
 * that outcome included in the 'success' result.  On failure, this callback is
 * merely informative, and the return type is ignored.  On succesful download,
 * this callback allows the client to perform any additional verification of the
 * downloaded image and return that result to the OTA server.
 *
 * @param success This indicates the success or failure of the download and
 * cryptographic verification process (if applicable).  Ver.: always
 * @param id This is the image identifier information that corresponds to the
 * download result.  Ver.: always
 */
bool emberAfOtaClientDownloadCompleteCallback(EmberAfOtaDownloadResult success,
                                              const EmberAfOtaImageId* id);
/** @brief Ota Client Incoming Message Raw
 *
 * This callback is for processing incoming messages for the Over-the-air
 * bootload cluster client.  ZCL will not process the message and instead hand
 * the raw over the air data to the callback for its own processing.
 *
 * @param message A pointer to the structure containing the message buffer and
 * other information about it.  Ver.: always
 */
bool emberAfOtaClientIncomingMessageRawCallback(EmberAfClusterCommand *message);
/** @brief Ota Client Start
 *
 * This callback should be called when the profile specific registration has
 * completed successfully.  It will start the client's state machine that will
 * find the OTA server, query it for the next image, download the image, wait
 * for the bootload message, and kick off the bootload.
 *
 */
void emberAfOtaClientStartCallback(void);
/** @brief Ota Client Version Info
 *
 * This function is called by the OTA client when a new query will occur to the
 * server asking what the next version of firmware is.  The client can inform
 * the cluster software as to what information to use in the query (and
 * subsequent download).
 *
 * @param currentImageInfo This is the information to use in the next query by
 * the client cluster code.  It contains the manufacturer ID, image type ID, and
 * the firmware version to be specified in the query message sent to the server.
 *  Ver.: always
 * @param hardwareVersion This is a pointer to the hardware version to use in
 * the query.  If no hardware version should be used, then
 * EMBER_AF_INVALID_HARDWARE_VERSION should be used.  Ver.: always
 */
void emberAfOtaClientVersionInfoCallback(EmberAfOtaImageId* currentImageInfo,
                                         int16u* hardwareVersion);
/** @brief Ota Page Request Server Policy
 *
 * This callback is called by the OTA server page request code when it wants to
 * determine if it is allowed for an OTA client to make a page request.  It is
 * only called if page request support has been enabled on the server.  It
 * should return EMBER_ZCL_STATUS_SUCCESS if it allows the page request, and
 * EMBER_ZCL_STATUS_UNSUP_COMMAND if it does not want to allow it.
 *
 */
int8u emberAfOtaPageRequestServerPolicyCallback(void);
/** @brief Ota Server Block Size
 *
 * This function provides a way for the server to adjust the block size of its
 * response to an Image block request by a client.
 *
 * @param clientNodeId The node Id of OTA client making an image block request.
 * Ver.: always
 */
int8u emberAfOtaServerBlockSizeCallback(EmberNodeId clientNodeId);
/** @brief Ota Server Incoming Message Raw
 *
 * This callback is for processing incoming messages for the Over-the-air
 * bootload cluster server.  ZCL will not process the message and instead hand
 * the raw over the air data to the callback for its own processing.
 *
 * @param message A pointer to the structure containing the message buffer and
 * other information about it.  Ver.: always
 */
bool emberAfOtaServerIncomingMessageRawCallback(EmberAfClusterCommand *message);
/** @brief Ota Server Query
 *
 * This callback is fired when the OTA server receives a query request by the
 * client.  The callback lets the server application indicate to the client what
 * the 'next' version of software is for the device, or if there is not one
 * available.
 *
 * @param currentImageId This is the current software image that the client
 * hase.  Ver.: always
 * @param hardwareVersion If this value is non-NULL, it indicates the hardware
 * version of the client device.  If NULL, the client did not specify a hardware
 * version.  Ver.: always
 * @param nextUpgradeImageId This is a pointer to a data structure containing
 * the 'next' software version for the client to download.  Ver.: always
 */
int8u emberAfOtaServerQueryCallback(const EmberAfOtaImageId* currentImageId,
                                    int16u* hardwareVersion,
                                    EmberAfOtaImageId* nextUpgradeImageId);
/** @brief Ota Server Send Image Notify
 *
 * This callback is an indication to the OTA server that it should send out
 * notification about an OTA file that is available for download.
 *
 * @param dest The destination of the image notify message.  May be a broadcast
 * address.  Ver.: always
 * @param endpoint The destination endpoint of the image notify message.  May be
 * a broadcast endpoint.  Ver.: always
 * @param payloadType The type of data the image notify message will contain.  0
 * = no data.  1 = Manufacturer ID.  2 = Manufacturer ID and the image type ID.
 * 3 = Manufacturer ID, image type ID, and firmware version.  Ver.: always
 * @param queryJitter The percentage of nodes that should respond to this
 * message, from 1-100.  On receipt of this message, each recipient will
 * randomly choose a percentage and only query the server if their percentage is
 * below this value.  Ver.: always
 * @param id The image information that will be put in the message.  The data
 * within this struct that will be appended to the message is determined by the
 * previous 'payloadType' argument.  Ver.: always
 */
bool emberAfOtaServerSendImageNotifyCallback(EmberNodeId dest,
                                             int8u endpoint,
                                             int8u payloadType,
                                             int8u queryJitter,
                                             const EmberAfOtaImageId* id);
/** @brief Ota Server Upgrade End Request
 *
 * This function is called when the OTA server receives a request an upgrade end
 * request.  If the request indicated a successful download by the client, the
 * server must tell the client when and if to upgrade to the downloaded image.
 *
 * @param source The node ID of the device that sent the upgrade end request.
 * Ver.: always
 * @param status This is the ZCL status sent by the client indicating the result
 * of its attempt to download the new upgrade image.  If the status is not
 * EMBER_ZCL_STATUS_SUCCESS then this callback is merely informative and no
 * response mesasge will be generated by the server.  Ver.: always
 * @param returnValue If the server returns true indicating that the client
 * should apply the upgrade, this time value indicates when in the future the
 * client should apply the upgrade.  Ver.: always
 * @param imageId This variable indicates the software version that the client
 * successfully downloaded and is asking to upgrade to.  Ver.: always
 */
bool emberAfOtaServerUpgradeEndRequestCallback(EmberNodeId source,
                                               int8u status,
                                               int32u* returnValue,
                                               const EmberAfOtaImageId* imageId);
/** @brief Ota Storage Check Temp Data
 *
 * This callback will validate temporary data in the storage device to determine
 * whether it is a complete file, a partially downloaded file, or there is no
 * file present.  When a complete or partial file is found it will return
 * EMBER_AF_OTA_STORAGE_SUCCESS or EMBER_AF_OTA_STORAGE_PARTIAL_FILE_FOUND,
 * respectively.  In that case, the currentOffset, totalImageSize, and
 * newFileInfo will be populated with data.  When EMBER_AF_OTA_STORAGE_ERROR is
 * returned, no temporary data is present.
 *
 * @param currentOffset A pointer to a value that will be written with the
 * offset within the total file size that has been successfully stored in the
 * storage device.  This will indicate how much data has been currently
 * dowloaded.  Ver.: always
 * @param totalImageSize A pointer to a value that will be written with the
 * total image size of the OTA file when a download has completed.  This does
 * not indicate how much data has actually been downloaded currently.  Ver.:
 * always
 * @param newFileInfo This is the image id of the temporary file data stored in
 * the storage device.  Ver.: always
 */
EmberAfOtaStorageStatus emberAfOtaStorageCheckTempDataCallback(int32u* currentOffset,
                                                               int32u* totalImageSize,
                                                               EmberAfOtaImageId* newFileInfo);
/** @brief Ota Storage Clear Temp Data
 *
 * This function clears any existing temp data that was downloaed.  It is used
 * immediately prior to downloading a raw image over the air.
 *
 */
EmberAfOtaStorageStatus emberAfOtaStorageClearTempDataCallback(void);
/** @brief Ota Storage Close
 *
 * This callback shuts down the ZigBee Over-the-air storage module.
 *
 */
void emberAfOtaStorageCloseCallback(void);
/** @brief Ota Storage Driver Download Finish
 *
 * This callback defines the low-level means by which a device records the final
 * offset value of the download image.
 *
 * @param offset The value of the final offset of the image download.  Ver.:
 * always
 */
void emberAfOtaStorageDriverDownloadFinishCallback(int32u offset);
/** @brief Ota Storage Driver Init
 *
 * The initialization code for the OTA storage driver.
 *
 */
bool emberAfOtaStorageDriverInitCallback(void);
/** @brief Ota Storage Driver Invalidate Image
 *
 * This callback invalidates the image stored on disk so that it will not be
 * bootloaded, and it will not be a valid image that is in the middle of
 * downloading.
 *
 */
EmberAfOtaStorageStatus emberAfOtaStorageDriverInvalidateImageCallback(void);
/** @brief Ota Storage Driver Prepare To Resume Download
 *
 * This callback allows the underlying storage driver to prepare to resume the
 * OTA file download.  For example, the driver may exceute a page erase to
 * insure the next page is ready to be written to.
 *
 */
EmberAfOtaStorageStatus emberAfOtaStorageDriverPrepareToResumeDownloadCallback(void);
/** @brief Ota Storage Driver Read
 *
 * This callback defines the low-level means by which a device reads from the
 * OTA storage device.
 *
 * @param offset The address offset from the start of the storage device where
 * data is to be read.  Ver.: always
 * @param length The length of the data to be read from the storage device.
 * Ver.: always
 * @param returnData A pointer where the data read from the device should be
 * written to.  Ver.: always
 */
bool emberAfOtaStorageDriverReadCallback(int32u offset,
                                         int32u length,
                                         int8u* returnData);
/** @brief Ota Storage Driver Retrieve Last Stored Offset
 *
 * This callback defines the low-level means by which a device retrieves the
 * last persistently recorded download offset.  This may be different than last
 * actual download offset.
 *
 */
int32u emberAfOtaStorageDriverRetrieveLastStoredOffsetCallback(void);
/** @brief Ota Storage Driver Write
 *
 * This callback defines the low-level means by which a device reads from the
 * OTA storage device.
 *
 * @param dataToWrite A pointer to the data that will be written to the storage
 * device.  Ver.: always
 * @param offset The address offset from the start of the storage device where
 * data will be written.  Ver.: always
 * @param length The length of the data to be written to the storage device.
 * Ver.: always
 */
bool emberAfOtaStorageDriverWriteCallback(const int8u* dataToWrite,
                                          int32u offset,
                                          int32u length);
/** @brief Ota Storage Finish Download
 *
 * This function indicates to the storage module that the download has finished.
 *
 * @param offset The final offset of the downloaded file (i.e. the total size)
 * Ver.: always
 */
EmberAfOtaStorageStatus emberAfOtaStorageFinishDownloadCallback(int32u offset);
/** @brief Ota Storage Get Count
 *
 * This callback returns the total number of ZigBee Over-the-air upgrade images
 * stored in the storage module.
 *
 */
int8u emberAfOtaStorageGetCountCallback(void);
/** @brief Ota Storage Get Full Header
 *
 * This callback populates the EmberAfOtaHeader structure pointed to by the
 * returnData with data about the OTA file stored in the storage module.
 *
 * @param id This is a pointer to the image id for the OTA file to retrieve
 * information about.  Ver.: always
 * @param returnData This is a pointer to the location of the structure that
 * will be populated with data.  Ver.: always
 */
EmberAfOtaStorageStatus emberAfOtaStorageGetFullHeaderCallback(const EmberAfOtaImageId* id,
                                                               EmberAfOtaHeader* returnData);
/** @brief Ota Storage Get Total Image Size
 *
 * This function returns the total size of the ZigBee Over-the-air file with the
 * passed parameters.  If no file is found with those parameters, 0 is returned.
 *
 * @param id A pointer to the image identifier for the OTA file to retrieve
 * information for.  Ver.: always
 */
int32u emberAfOtaStorageGetTotalImageSizeCallback(const EmberAfOtaImageId* id);
/** @brief Ota Storage Init
 *
 * This callback initializes the ZigBee Over-the-air storage module.
 *
 */
EmberAfOtaStorageStatus emberAfOtaStorageInitCallback(void);
/** @brief Ota Storage Iterator First
 *
 * This callback lets you walk through the list of all OTA files by jumping to
 * the first file in the list maintained by the storage module.  If there is no
 * file then emberAfOtaInvalidImageId is returned.
 *
 */
EmberAfOtaImageId emberAfOtaStorageIteratorFirstCallback(void);
/** @brief Ota Storage Iterator Next
 *
 * This callback lets you walk through the list of all OTA files by jumping to
 * the next file in the list maintained by the storage module.  If there is no
 * next file then emberAfOtaInvalidImageId is returned.
 *
 */
EmberAfOtaImageId emberAfOtaStorageIteratorNextCallback(void);
/** @brief Ota Storage Read Image Data
 *
 * This callback reads data from the specified OTA file and returns that data to
 * the caller.
 *
 * @param id This is a pointer to the image id for the OTA file to retrieve data
 * from.  Ver.: always
 * @param offset This is the offset relative to the start of the image where the
 * data should be read from.  Ver.: always
 * @param length This is the length of data that will be read.  Ver.: always
 * @param returnData This is a pointer to where the data read out of the file
 * will be written to  Ver.: always
 * @param returnedLength This is a pointer to a variable where the actual length
 * of data read will be written to.  A short read may occur if the end of file
 * was reached.  Ver.: always
 */
EmberAfOtaStorageStatus emberAfOtaStorageReadImageDataCallback(const EmberAfOtaImageId* id,
                                                               int32u offset,
                                                               int32u length,
                                                               int8u* returnData,
                                                               int32u* returnedLength);
/** @brief Ota Storage Search
 *
 * This callback searches through the list of all images for one that matches
 * the passed parameters.  On success an image identifier is returned with a
 * matching image.  On failure emberAfInvalidImageId is returned.
 *
 * @param manufacturerId The ZigBee assigned identifier of the manufacturer
 * contained in the OTA image being searched for.  Ver.: always
 * @param imageTypeId The image type identifier contained in the OTA image being
 * searched for.  Ver.: always
 * @param hardwareVersion This is a pointer to the hardware version that will be
 * used in the search.  If the pointer is NULL, hardware version will not be
 * considered when searching for matching images.  If it points to a value, the
 * search will only consider images where that value falls between the minimum
 * and maxmimum hardware version specified in the OTA file.  If no hardware
 * version is present in an OTA file but the other parameters match, the file
 * will be considered a match  Ver.: always
 */
EmberAfOtaImageId emberAfOtaStorageSearchCallback(int16u manufacturerId,
                                                  int16u imageTypeId,
                                                  const int16u* hardwareVersion);
/** @brief Ota Storage Write Temp Data
 *
 * This function writes to the temporary data in the storage device at the
 * specified offset.  It is used when downloading a raw image over the air.
 *
 * @param offset The location within the download image file where to write the
 * data.  Ver.: always
 * @param length The length of data to write.  Ver.: always
 * @param data A pointer to the temporary data that will be written to the
 * storage device.  Ver.: always
 */
EmberAfOtaStorageStatus emberAfOtaStorageWriteTempDataCallback(int32u offset,
                                                               int32u length,
                                                               const int8u* data);
/** @brief Outgoing Packet Filter
 *
 * ** REQUIRES INCLUDING THE PACKET-HANDOFF PLUGIN **

        This is called by
 * the Packet Handoff plugin when the stack prepares to send a packet from one
 * of the protocol layers specified in ::EmberZigbeePacketType.

        The
 * packetType argument is one of the values of the ::EmberZigbeePacketType enum.
 * If the stack receives an 802.15.4 MAC beacon, it will call this function with
 * the packetType argument set to ::EMBER_ZIGBEE_PACKET_TYPE_BEACON.


 * The implementation of this callback may alter the data contained in
 * packetData, modify options and flags in the auxillary data, or consume the
 * packet itself, either sending the message, or discarding it as it sees fit.
 *
 * @param packetType the type of packet and associated protocol layer  Ver.:
 * always
 * @param packetData flat buffer containing the packet data associated with the
 * packet type  Ver.: always
 * @param size_p a pointer containing the size value of the packet  Ver.: always
 * @param data auxillary data included with the packet  Ver.: always
 */
EmberPacketAction emberAfOutgoingPacketFilterCallback(EmberZigbeePacketType packetType,
                                                      int8u* packetData,
                                                      int8u* size_p,
                                                      void* data);
/** @brief Partner Link Key Exchange Request
 *
 * This function is called by the framework on SOC platforms when a remote node
 * requests a partner link key exchange.  The application should return
 * EMBER_SUCCESS to accept the request or any other status to reject it.  On
 * network coprocessor platforms, this function will not be called because the
 * NCP handles partner link key exchange requests based on the binding policy.
 *
 * @param partner The EUI of the remote node.  Ver.: always
 */
EmberZdoStatus emberAfPartnerLinkKeyExchangeRequestCallback(EmberEUI64 partner);
/** @brief Partner Link Key Exchange Response
 *
 * This function is called by the framework when a remote node requests a
 * partner link key exchange.  The application should return true to accept the
 * request or false to reject it.  On network coprocessor platforms, this
 * function will not be called because the NCP handles partner link key exchange
 * requests based on the binding policy.
 *
 * @param sender The EUI of the remote node.  Ver.: always
 * @param status The ZDO response status.  Ver.: always
 */
void emberAfPartnerLinkKeyExchangeResponseCallback(EmberNodeId sender,
                                                   EmberZdoStatus status);
/** @brief Performing Key Establishment
 *
 * This function is called by the framework to determine if the device is
 * performing key establishment.  The application should return true if key
 * establishment is in progress.
 *
 */
bool emberAfPerformingKeyEstablishmentCallback(void);
/** @brief Post Attribute Change
 *
 * This function is called by the application framework after it changes an
 * attribute value. The value passed into this callback is the value to which
 * the attribute was set by the framework.
 *
 * @param endpoint   Ver.: always
 * @param clusterId   Ver.: always
 * @param attributeId   Ver.: always
 * @param mask   Ver.: always
 * @param manufacturerCode   Ver.: always
 * @param type   Ver.: always
 * @param size   Ver.: always
 * @param value   Ver.: always
 */
void emberAfPostAttributeChangeCallback(int8u endpoint,
                                        EmberAfClusterId clusterId,
                                        EmberAfAttributeId attributeId,
                                        int8u mask,
                                        int16u manufacturerCode,
                                        int8u type,
                                        int8u size,
                                        int8u* value);
/** @brief Post Em4 Reset
 *
 * A callback called by application framework, and implemented by em4 plugin
 *
 */
void emberAfPostEm4ResetCallback(void);
/** @brief Pre Attribute Change
 *
 * This function is called by the application framework before it changes an
 * attribute value.  The value passed into this callback is the value to which
 * the attribute is to be set by the framework.  The application should return
 * ::EMBER_ZCL_STATUS_SUCCESS to permit the change or any other ::EmberAfStatus
 * to reject it.
 *
 * @param endpoint   Ver.: always
 * @param clusterId   Ver.: always
 * @param attributeId   Ver.: always
 * @param mask   Ver.: always
 * @param manufacturerCode   Ver.: always
 * @param type   Ver.: always
 * @param size   Ver.: always
 * @param value   Ver.: always
 */
EmberAfStatus emberAfPreAttributeChangeCallback(int8u endpoint,
                                                EmberAfClusterId clusterId,
                                                EmberAfAttributeId attributeId,
                                                int8u mask,
                                                int16u manufacturerCode,
                                                int8u type,
                                                int8u size,
                                                int8u* value);
/** @brief Pre Cli Send
 *
 * This function is called by the framework when it is about to pass a message
 * constructed over CLI to the stack primitives for sending. If the function
 * returns true it is assumed that the callback has consumed and processed the
 * message. The framework will not do any further processing on the message.

 *     If the function returns false then it is assumed that the callback has
 * not processed the message and the framework will continue to process
 * accordingly.
 *
 * @param apsFrame The structure containing the APS frame  Ver.: always
 * @param source Source Node Id  Ver.: always
 * @param destination Destintion Node Id  Ver.: always
 * @param message Pointer to the message payload  Ver.: always
 * @param messageLength Length of the message payload  Ver.: always
 */
bool emberAfPreCliSendCallback(EmberApsFrame* apsFrame,
                               EmberNodeId source,
                               EmberNodeId destination,
                               int8u* message,
                               int16u messageLength);
/** @brief Pre Command Received
 *
 * This callback is the second in the Application Framework's message processing
 * chain. At this point in the processing of incoming over-the-air messages, the
 * application has determined that the incoming message is a ZCL command. It
 * parses enough of the message to populate an EmberAfClusterCommand struct. The
 * Application Framework defines this struct value in a local scope to the
 * command processing but also makes it available through a global pointer
 * called emberAfCurrentCommand, in app/framework/util/util.c. When command
 * processing is complete, this pointer is cleared.
 *
 * @param cmd   Ver.: always
 */
bool emberAfPreCommandReceivedCallback(EmberAfClusterCommand* cmd);
/** @brief Pre Message Received
 *
 * This callback is the first in the Application Framework's message processing
 * chain. The Application Framework calls it when a message has been received
 * over the air but has not yet been parsed by the ZCL command-handling code. If
 * you wish to parse some messages that are completely outside the ZCL
 * specification or are not handled by the Application Framework's command
 * handling code, you should intercept them for parsing in this callback.

 *   This callback returns a Boolean value indicating whether or not the message
 * has been handled. If the callback returns a value of true, then the
 * Application Framework assumes that the message has been handled and it does
 * nothing else with it. If the callback returns a value of false, then the
 * application framework continues to process the message as it would with any
 * incoming message.
        Note:   This callback receives a pointer to an
 * incoming message struct. This struct allows the application framework to
 * provide a unified interface between both Host devices, which receive their
 * message through the ezspIncomingMessageHandler, and SoC devices, which
 * receive their message through emberIncomingMessageHandler.
 *
 * @param incomingMessage   Ver.: always
 */
bool emberAfPreMessageReceivedCallback(EmberAfIncomingMessage* incomingMessage);
/** @brief Pre Message Send
 *
 * This function is called by the framework when it is about to pass a message
 * to the stack primitives for sending.   This message may or may not be ZCL,
 * ZDO, or some other protocol.  This is called prior to
        any ZigBee
 * fragmentation that may be done.  If the function returns true it is assumed
 * the callback has consumed and processed the message.  The callback must also
 * set the EmberStatus status code to be passed back to the caller.  The
 * framework will do no further processing on the message.
        If the
 * function returns false then it is assumed that the callback has not processed
 * the mesasge and the framework will continue to process accordingly.
 *
 * @param messageStruct The structure containing the parameters of the APS
 * message to be sent.  Ver.: always
 * @param status A pointer to the status code value that will be returned to the
 * caller.  Ver.: always
 */
bool emberAfPreMessageSendCallback(EmberAfMessageStruct* messageStruct,
                                   EmberStatus* status);
/** @brief Pre Ncp Reset
 *
 * This function will be called prior to the reset of the NCP by the host.
 *
 */
void emberAfPreNcpResetCallback(void);
/** @brief Pre ZDO Message Received
 *
 * This function passes the application an incoming ZDO message and gives the
 * appictation the opportunity to handle it. By default, this callback returns
 * false indicating that the incoming ZDO message has not been handled and
 * should be handled by the Application Framework.
 *
 * @param emberNodeId   Ver.: always
 * @param apsFrame   Ver.: always
 * @param message   Ver.: always
 * @param length   Ver.: always
 */
bool emberAfPreZDOMessageReceivedCallback(EmberNodeId emberNodeId,
                                          EmberApsFrame* apsFrame,
                                          int8u* message,
                                          int16u length);
/** @brief Read Attributes Response
 *
 * This function is called by the application framework when a Read Attributes
 * Response command is received from an external device.  The application should
 * return true if the message was processed or false if it was not.
 *
 * @param clusterId The cluster identifier of this response.  Ver.: always
 * @param buffer Buffer containing the list of read attribute status records.
 * Ver.: always
 * @param bufLen The length in bytes of the list.  Ver.: always
 */
bool emberAfReadAttributesResponseCallback(EmberAfClusterId clusterId,
                                           int8u *buffer,
                                           int16u bufLen);
/** @brief Read Reporting Configuration Command
 *
 * This function is called by the application framework when a Read Reporting
 * Configuration command is received from an external device.  The application
 * should return true if the message was processed or false if it was not.
 *
 * @param cmd   Ver.: always
 */
bool emberAfReadReportingConfigurationCommandCallback(const EmberAfClusterCommand *cmd);
/** @brief Read Reporting Configuration Response
 *
 * This function is called by the application framework when a Read Reporting
 * Configuration Response command is received from an external device.  The
 * application should return true if the message was processed or false if it
 * was not.
 *
 * @param clusterId The cluster identifier of this response.  Ver.: always
 * @param buffer Buffer containing the list of attribute reporting configuration
 * records.  Ver.: always
 * @param bufLen The length in bytes of the list.  Ver.: always
 */
bool emberAfReadReportingConfigurationResponseCallback(EmberAfClusterId clusterId,
                                                       int8u *buffer,
                                                       int16u bufLen);
/** @brief Registration Abort
 *
 * This callback is called when the device should abort the registration
 * process.
 *
 */
void emberAfRegistrationAbortCallback(void);
/** @brief Registration
 *
 * This callback is called when the device joins a network and the process of
 * registration is complete. This callback provides a success value of true if
 * the registration process was successful and a value of false if registration
 * failed.
 *
 * @param success true if registration succeeded, false otherwise.  Ver.: always
 */
void emberAfRegistrationCallback(bool success);
/** @brief Registration Start
 *
 * This callback is called when the device joins a network and the registration
 * process should begin.  The application should return EMBER_SUCCESS if the
 * registration process started successfully.  When registration is complete,
 * the application should call emberAfRegistrationCallback with an indication of
 * success or failure.
 *
 */
EmberStatus emberAfRegistrationStartCallback(void);
/** @brief Remote Delete Binding Permission
 *
 * This function is called by the framework to request permission to service the
 * remote delete binding request. Return EMBER_SUCCESS to allow request,
 * anything else to disallow request.
 *
 * @param index index to an Ember binding table entry  Ver.: always
 */
EmberStatus emberAfRemoteDeleteBindingPermissionCallback(int8u index);
/** @brief Remote Set Binding Permission
 *
 * This function is called by the framework to request permission to service the
 * remote set binding request. Return EMBER_SUCCESS to allow request, anything
 * else to disallow request.
 *
 * @param entry Ember Binding Tablet Entry  Ver.: always
 */
EmberStatus emberAfRemoteSetBindingPermissionCallback(const EmberBindingTableEntry *entry);
/** @brief Remove From Current App Tasks
 *
 * This function is only useful to sleepy end devices.  This function will
 * remove the passed item from the set of tasks the application has outstanding
 * (e.g. message sent requiring APS acknwoledgement).  This will affect how the
 * application behaves with regard to sleeping and polling.  Removing the item
 * from the list of outstanding tasks may allow the device to sleep longer and
 * poll less frequently.  If there are other outstanding tasks the system may
 * still have to stay away and poll more often.
 *
 * @param tasks   Ver.: always
 */
void emberAfRemoveFromCurrentAppTasksCallback(EmberAfApplicationTask tasks);
/** @brief Report Attributes
 *
 * This function is called by the application framework when a Report Attributes
 * command is received from an external device.  The application should return
 * true if the message was processed or false if it was not.
 *
 * @param clusterId The cluster identifier of this command.  Ver.: always
 * @param buffer Buffer containing the list of attribute report records.  Ver.:
 * always
 * @param bufLen The length in bytes of the list.  Ver.: always
 */
bool emberAfReportAttributesCallback(EmberAfClusterId clusterId,
                                     int8u *buffer,
                                     int16u bufLen);
/** @brief Reporting Attribute Change
 *
 * This function is called by the framework when an attribute managed by the
 * framework changes.  The application should call this function when an
 * externally-managed attribute changes.  The application should use the change
 * notification to inform its reporting decisions.
 *
 * @param endpoint   Ver.: always
 * @param clusterId   Ver.: always
 * @param attributeId   Ver.: always
 * @param mask   Ver.: always
 * @param manufacturerCode   Ver.: always
 * @param type   Ver.: always
 * @param data   Ver.: always
 */
void emberAfReportingAttributeChangeCallback(int8u endpoint,
                                             EmberAfClusterId clusterId,
                                             EmberAfAttributeId attributeId,
                                             int8u mask,
                                             int16u manufacturerCode,
                                             EmberAfAttributeType type,
                                             int8u *data);
/** @brief Security Init
 *
 * This callback is called by the framework to give the application a chance to
 * modify the security settings of the node during network initialization.
 * Depending on the context when this callback is called, the pointer to the
 * initial security state may be NULL, which means the initial security state
 * can no longer be modified as the node is already operating on the network.
 *
 * @param state   Ver.: always
 * @param extended   Ver.: always
 * @param trustCenter   Ver.: always
 */
void emberAfSecurityInitCallback(EmberInitialSecurityState *state,
                                 EmberExtendedSecurityBitmask *extended,
                                 bool trustCenter);
/** @brief Set Default Poll Control
 *
 * This function will set the default poll control for the current network to
 * control whether or not it can long poll.
 *
 * @param control   Ver.: always
 */
void emberAfSetDefaultPollControlCallback(EmberAfEventPollControl control);
/** @brief Set Default Sleep Control
 *
 * This function will set the default behavior of a sleeping device to control
 * whether or not it must stay awake.  A device that stays awake does not sleep
 * at all.  Otherwise, the device can sleep between events when appropriate.
 *
 * @param control   Ver.: always
 */
void emberAfSetDefaultSleepControlCallback(EmberAfEventSleepControl control);
/** @brief Set Form And Join Extended Pan Id
 *
 * This callback is called by the framework to set the extended PAN ID used by
 * the current network for forming and joining.  The extended PAN ID used for
 * forming and joining is not necessarily the same extended PAN ID actually in
 * use on the network.
 *
 * @param extendedPanId   Ver.: always
 */
void emberAfSetFormAndJoinExtendedPanIdCallback(const int8u *extendedPanId);
/** @brief Set Long Poll Interval Ms
 *
 * This function is only useful to end devices.  This function will set the long
 * poll interval (in milliseconds) for the current network.  This interval is
 * the maximum amount of time a child will wait between polls of its parent when
 * it is not expecting data.
 *
 * @param longPollIntervalMs   Ver.: always
 */
void emberAfSetLongPollIntervalMsCallback(int32u longPollIntervalMs);
/** @brief Set Long Poll Interval Qs
 *
 * This function is only useful to end devices.  This function will set the long
 * poll interval (in quarter seconds) for the current network.  This interval is
 * the maximum amount of time a child will wait between polls of its parent when
 * it is not expecting data.
 *
 * @param longPollIntervalQs   Ver.: always
 */
void emberAfSetLongPollIntervalQsCallback(int32u longPollIntervalQs);
/** @brief Set Short Poll Interval Ms
 *
 * This function is only useful to sleepy end devices.  This function will set
 * the short poll interval (in milliseconds) for the current network.  This
 * interval is the maximum amount of time a child will wait between polls of its
 * parent when it is expecting data.
 *
 * @param shortPollIntervalMs   Ver.: always
 */
void emberAfSetShortPollIntervalMsCallback(int16u shortPollIntervalMs);
/** @brief Set Short Poll Interval Qs
 *
 * This function is only useful to sleepy end devices.  This function will set
 * the short poll interval (in quarter seconds) for the current network.  This
 * interval is the maximum amount of time a child will wait between polls of its
 * parent when it is expecting data.
 *
 * @param shortPollIntervalQs   Ver.: always
 */
void emberAfSetShortPollIntervalQsCallback(int16u shortPollIntervalQs);
/** @brief Set Source Route Overhead
 *
 * This function is called by the framework when it has information about the
 * source route overhead to a particular destination. The application may use
 * this information to cache the source route overhead.
 *
 * @param destination The node id of the destination  Ver.: always
 * @param overhead The overhead in bytes  Ver.: always
 */
void emberAfSetSourceRouteOverheadCallback(EmberNodeId destination,
                                           int8u overhead);
/** @brief Set Time
 *
 * This callback should be implemented, if the device has access to real time
 * clock, and has an ability to update that clock. The application framework
 * expects to be passed the utcTime which is the number of seconds since the
 * year 2000. Default implementation does nothing. Note: This function used to
 * take time in year, month, day, hour, min, sec. We have changed this to
 * utcTime in order to conserve code space.
 *
 * @param utcTime   Ver.: always
 */
void emberAfSetTimeCallback(int32u utcTime);
/** @brief Set Wake Timeout Bitmask
 *
 * This function is only useful to sleepy end devices.  This function will set
 * the wake timeout bitmask for the current network.  The bitmask determines
 * which tasks will timeout automatically and which tasks require manual removal
 * from the task list.
 *
 * @param tasks   Ver.: always
 */
void emberAfSetWakeTimeoutBitmaskCallback(EmberAfApplicationTask tasks);
/** @brief Set Wake Timeout Ms
 *
 * This function is only useful to sleepy end devices.  This function will set
 * the wake timeout (in milliseconds) for the current network.  This timeout is
 * the maximum amount of time a child will wait for a task in the wake bitmask
 * to finish.  While waiting, the device will short poll.
 *
 * @param wakeTimeoutMs   Ver.: always
 */
void emberAfSetWakeTimeoutMsCallback(int16u wakeTimeoutMs);
/** @brief Set Wake Timeout Qs
 *
 * This function is only useful to sleepy end devices.  This function will set
 * the wake timeout (in quarter seconds) for the current network.  This timeout
 * is the maximum amount of time a child will wait for a task in the wake
 * bitmask to finish.  While waiting, the device will short poll.
 *
 * @param wakeTimeoutQs   Ver.: always
 */
void emberAfSetWakeTimeoutQsCallback(int16u wakeTimeoutQs);
/** @brief Start Move
 *
 * This function is called to initiate the process for a device to move (rejoin)
 * to a new parent.
 *
 */
bool emberAfStartMoveCallback(void);
/** @brief Start Search For Joinable Network
 *
 * This function is called by the framework to search for joinable networks and
 * join a network.  The application should return EMBER_SUCCESS if the operation
 * was initiated successfully.
 *
 */
EmberStatus emberAfStartSearchForJoinableNetworkCallback(void);
/** @brief Stop Move
 *
 * This function is called to cancel a previously scheduled move (rejoin) to a
 * new parent.
 *
 */
void emberAfStopMoveCallback(void);
/** @brief Trust Center Join
 *
 * This callback is called from within the application framework's
 * implementation of emberTrustCenterJoinHandler or ezspTrustCenterJoinHandler.
 * This callback provides the same arguments passed to the
 * TrustCenterJoinHandler. For more information about the TrustCenterJoinHandler
 * please see documentation included in stack/include/trust-center.h.
 *
 * @param newNodeId   Ver.: always
 * @param newNodeEui64   Ver.: always
 * @param parentOfNewNode   Ver.: always
 * @param status   Ver.: always
 * @param decision   Ver.: always
 */
void emberAfTrustCenterJoinCallback(EmberNodeId newNodeId,
                                    EmberEUI64 newNodeEui64,
                                    EmberNodeId parentOfNewNode,
                                    EmberDeviceUpdate status,
                                    EmberJoinDecision decision);
/** @brief Trust Center Keepalive Abort
 *
 * This callback is called when the device should abort the trust center
 * keepalive process.
 *
 */
void emberAfTrustCenterKeepaliveAbortCallback(void);
/** @brief Trust Center Keepalive Update
 *
 * This callback is called when the device finishes registration (successfully
 * or otherwise) and the trust center keepalive process must be updated.  If the
 * keepalive process has not been started, then it is started.  Otherwise if the
 * keepalive is in the process of searching for the TC, it will process the
 * result of that Trust Center search operation.
 *
 * @param registrationComplete   Ver.: always
 */
void emberAfTrustCenterKeepaliveUpdateCallback(bool registrationComplete);
/** @brief Write Attributes Response
 *
 * This function is called by the application framework when a Write Attributes
 * Response command is received from an external device.  The application should
 * return true if the message was processed or false if it was not.
 *
 * @param clusterId The cluster identifier of this response.  Ver.: always
 * @param buffer Buffer containing the list of write attribute status records.
 * Ver.: always
 * @param bufLen The length in bytes of the list.  Ver.: always
 */
bool emberAfWriteAttributesResponseCallback(EmberAfClusterId clusterId,
                                            int8u *buffer,
                                            int16u bufLen);
/** @} */ // end of global_callback
