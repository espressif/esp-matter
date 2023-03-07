/***************************************************************************//**
 * @file
 * @brief See @ref child for documentation.
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

#ifndef SILABS_CHILD_H
#define SILABS_CHILD_H

#include "mac-child.h"
/**
 * @addtogroup child
 * @brief EmberZNet API relating to end device children.
 *
 * See child.h for source code.
 * @{
 */

/** @brief Convert a child index to a node ID.
 *
 * @param childIndex  The index.
 *
 * @return The node ID of the child or ::EMBER_NULL_NODE_ID if there isn't a
 *         child at the childIndex specified.
 */
EmberNodeId emberChildId(uint8_t childIndex);

/** @brief Return radio power value of the child from the given
 * childIndex.
 * @param childIndex  The index.
 *
 * @return The power of the child or maximum radio power, which is the power value
 *         provided by the user while forming/joining a network if there isn't a
 *         child at the childIndex specified.
 */
int8_t emberChildPower(uint8_t childIndex);

/** @brief Set the radio power value for a given child index.
 *
 * @param childIndex The index.
 * @param newPower The new power value.
 */
void emberSetChildPower(uint8_t childIndex, int8_t newPower);

/** @brief Convert a node ID to a child index.
 *
 * @param childId  The node ID of the child.
 *
 * @return The child index or 0xFF if the node ID does not belong to a child.
 */
uint8_t emberChildIndex(EmberNodeId childId);

/** @brief Get the EUI64 and node type of the child at the provided index.
 * If there is no child at 'index', it returns ::EMBER_NOT_JOINED.
 * Possible child indexes run from zero to  ::emberMaxChildCount() - 1.
 *
 * @param index            The index of the child of interest.
 *
 * @param childData        The child's data is copied here.
 *
 * @return Returns ::EMBER_SUCCESS if a child is found at that index,
 *         ::EMBER_NOT_JOINED if not.
 */
EmberStatus emberGetChildData(uint8_t index,
                              EmberChildData *childData);

/** @brief Sets the EUI64, node ID, and node type of the child at the provided index in
 * nonvolatile memory. No other data from the childData argument is used or copied.
 * The user is responsible for rebooting the device after calling this API in order to
 * initialize RAM-based child data.
 * Possible child indexes run from zero to ::emberMaxChildCount() - 1.
 *
 * @param index            The index of the child.
 *
 * @param childData        The child's data.
 *
 * @return Returns ::EMBER_SUCCESS if successfully setting child at that index,
 *         ::EMBER_INDEX_OUT_OF_RANGE if provided index is out of range.
 *
 * @note This API only sets the child data to nonvolatile memory. A reboot is
 * required after calling this API to read the nonvolatile child data and set it to
 * RAM structures. This API does not set other child RAM data, such as power and
 * timeout values.
 */
EmberStatus emberSetChildData(uint8_t index,
                              EmberChildData *childData);

/** @brief Called by the stack when a child joins or leaves.
 * 'Joining' is true if the child is joining and false if leaving.
 *
 * The index is the same as the value that should be passed to
 * ::emberGetChildData() to get this child's data.
 * Note that if the child is leaving, ::emberGetChildData()
 * will return ::EMBER_NOT_JOINED if called with this index.
 * If the application includes ::emberChildJoinHandler(), it must
 * define EMBER_APPLICATION_HAS_CHILD_JOIN_HANDLER in its
 * CONFIGURATION_HEADER
 *
 * @param index    The index of the child of interest.
 *
 * @param joining  True if the child is joining, false if the child is leaving.
 */
void emberChildJoinHandler(uint8_t index, bool joining);

/** @ brief Set the keep alive mode.
 *
 *  @param mode One of the four configurations in the EmberKeepAliveMode.
 *
 */

EmberStatus emberSetKeepAliveMode(EmberKeepAliveMode mode);

/** @ brief Get the keep alive mode.
 *
 *  @return mode One of the four configurations in the EmberKeepAliveMode.
 *
 */

EmberKeepAliveMode emberGetKeepAliveMode(void);

/** @brief Request any pending data from the parent node.
 * This function allows an end device to query its parent for any pending data.
 *
 * End devices must call this function periodically to maintain contact with
 * their parent.  The parent will remove a child if it has not received a poll
 * from it within the configured timeout.
 *
 * If the end device has lost contact with its parent, ::emberPollForData()
 * calls ::emberRejoinNetwork(true) and returns ::EMBER_ERR_FATAL.
 *
 * The default values for the timeouts are set in
 * config/ember-configuration-defaults.h and can be overridden in the
 * application's configuration header.
 *
 * @return An EmberStatus value:
 * - ::EMBER_SUCCESS - The poll message has been submitted for transmission.
 * - ::EMBER_INVALID_CALL - The node is not an end device.
 * - ::EMBER_NOT_JOINED - The node is not part of a network.
 * - ::EMBER_MAC_TRANSMIT_QUEUE_FULL - The MAC layer transmit queue is full.
 * - ::EMBER_NO_BUFFERS - No buffers available to create the
 *     data poll message.
 * - ::EMBER_ERR_FATAL - Too much time has elapsed since the
 *     last successful poll. A rejoin attempt has been initiated.
 *     This error is not "fatal". The command can be retried until successful.
 */
EmberStatus emberPollForData(void);

/** @ brief Set the retry interval (in milliseconds) for
 * mac data poll.
 * This function is useful for sleepy end devices.
 * The retry interval is the time in milliseconds the device
 * waits before retrying a data poll when a MAC level data poll fails
 * for any reason. The retry feature can be disabled by setting
 * the retry interval to zero.
 *
 *  @param waitBeforeRetryIntervalMs  Time in milliseconds the device waits before
 *                                    retrying a data poll when a MAC level data
 *                                    poll fails for any reason. The retry feature
 *                                    can be disabled by setting this value to zero.
 *
 */
void emberSetMacPollFailureWaitTime(uint32_t waitBeforeRetryIntervalMs);

/** @ brief Called by the stack when a data poll to the parent is
 * complete.
 *
 * If the application includes ::emberPollCompleteHandler(),
 * it must define EMBER_APPLICATION_HAS_POLL_COMPLETE_HANDLER within
 * its CONFIGURATION_HEADER
 *
 * @param status  An \c EmberStatus value:
 * - ::EMBER_SUCCESS - Data was received in response to the poll.
 * - ::EMBER_MAC_NO_DATA - No data was pending.
 * - ::EMBER_PHY_TX_CCA_FAIL - The poll message could not be sent over
 *     the air.
 * - ::EMBER_MAC_NO_ACK_RECEIVED - The poll message was sent but
 *     not acknowledged by the parent.
 */
void emberPollCompleteHandler(EmberStatus status);

/** @brief Set a flag to indicate that a message is pending for a child.
 * The next time that the child polls, it will be informed that it has
 * a pending message. The message is sent from emberPollHandler, which is called
 * when the child requests data.
 *
 * @param childId  The ID of the child that just polled for data.
 *
 * @return An ::EmberStatus value.
 * - ::EMBER_SUCCESS - The next time that the child polls, it will be informed
 *     that it has pending data.
 * - ::EMBER_NOT_JOINED - The child identified by childId is not our child.
 */
EmberStatus emberSetMessageFlag(EmberNodeId childId);

/** @brief Clear a flag to indicate that there are no more messages for a
 * child. The next time the child polls, it will be informed that it does not
 * have any pending messages.
 *
 * @param childId  The ID of the child that no longer has pending messages.
 *
 * @return An ::EmberStatus value.
 * - ::EMBER_SUCCESS - The next time that the child polls, it will be informed
 *     that it does not have any pending messages.
 * - ::EMBER_NOT_JOINED - The child identified by childId is not our child.
 */
EmberStatus emberClearMessageFlag(EmberNodeId childId);

/** @brief Called to allow the application to send a message in
 * response to a poll from a child.
 *
 * This function is called when a child polls,
 * provided that the pending message flag is set for that child
 * (see ::emberSetMessageFlag(). The message should be sent to the child
 * using ::emberSendUnicast() with the ::EMBER_APS_OPTION_POLL_RESPONSE option.
 *
 * If the application includes ::emberPollHanlder(), it must
 * define EMBER_APPLICATION_HAS_POLL_HANDLER in its CONFIGURATION_HEADER.
 *
 * @param childId           The ID of the child that is requesting data.
 *
 * @param transmitExpected  True if the child is expecting an application-
 *     supplied data message.  False otherwise.
 *
 */
void emberPollHandler(EmberNodeId childId, bool transmitExpected);

/** @brief Add a child to the child/neighbor table only on SoC,
 * allowing direct manipulation of these tables by the application.
 * This can affect the network functionality, and needs to be used wisely.
 * If used appropriately, the application can maintain more than the
 * maximum of children provided by the stack.
 *
 * @param shortId  The preferred short ID of the node.
 *
 * @param longId  The long ID of the node.
 *
 * @param nodeType The nodetype e.g., EMBER_ROUTER defining,
 * if this would be added to the child table or neighbor table.
 *
 * @return An ::EmberStatus value.
 * - ::EMBER_SUCCESS - This node has been successfully added.
 * - ::EMBER_ERR_FATAL - The child was not added to the child/neighbor table.
 */
EmberStatus emberAddChild(EmberNodeId shortId,
                          EmberEUI64 longId,
                          EmberNodeType nodeType);

/** @brief Remove a node from child/neighbor table only on SoC,
 * allowing direct manipulation of these tables by the application.
 * This can affect the network functionality, and needs to be used wisely.
 *
 * @param childEui64  The long ID of the node.
 *
 * @return An ::EmberStatus value.
 * - ::EMBER_SUCCESS - This node has been successfully removed.
 * - ::EMBER_ERR_FATAL - The node was not found in either of the child or neighbor tables.
 */
EmberStatus emberRemoveChild(EmberEUI64 childEui64);

/** @brief Remove a neighbor from neighbor table only on SoC,
 * allowing direct manipulation of neighbor table by the application.
 * This can affect the network functionality, and needs to be used wisely.
 *
 * @param shortId  The short ID of the neighbor.
 *
 * @param longId  The long ID of the neighbor.
 *
 */
void emberRemoveNeighbor(EmberNodeId shortId,
                         EmberEUI64 longId);

#ifdef DOXYGEN_SHOULD_SKIP_THIS
/** @brief Return the number of children the node currently has.
 *
 * @return The number of children.
 */
uint8_t emberChildCount(void);

/** @brief Return the number of router children that the node
 * currently has.
 *
 * @return The number of router children.
 */
uint8_t emberRouterChildCount(void);

/** @brief Return the maximum number of children for this node.
 * The return value is undefined for nodes that are not joined to a network.
 *
 * @return The maximum number of children.
 */
uint8_t emberMaxChildCount(void);

/** @brief Return the maximum number of router children for this node.
 * The return value is undefined for nodes that are not joined to a network.
 *
 * @return The maximum number of router children.
 */
uint8_t emberMaxRouterChildCount(void);

/** @brief Return the parent's node ID.  The return value is undefined
 * for nodes without parents (coordinators and nodes that are not joined
 * to a network).
 *
 * @return The parent's node ID.
 */
EmberNodeId emberGetParentNodeId(void);

/** @brief Return the parent's EUI64.  The return value is undefined
 * for nodes without parents (coordinators and nodes that are not joined
 * to a network).
 *
 * @return The parent's EUI64.
 */
EmberEUI64 emberGetParentEui64(void);

#else // Doxygen ignores the following

extern uint8_t emMaxEndDeviceChildren;    // A maximum for this node.

// The '+ 0' prevents anyone from accidentally assigning to these.
#define emberRouterChildCount()    0
#define emberMaxChildCount()       (emMaxEndDeviceChildren + 0)
#define emberMaxRouterChildCount() 0

// Implemented in ember-stack-common.c.
uint32_t emberGetParentIncomingNwkFrameCounter(void);
EmberStatus emberSetParentIncomingNwkFrameCounter(uint32_t value);
EmberNodeId emberGetParentNodeId(void);
uint8_t* emberGetParentEui64(void);
uint8_t emberChildCount(void);

#endif

/**@name Power Management
 */
//@{

/** @brief Define tasks that are taken into account when dealing with
 *  sleep related behavior.
 *
 *  As opposed to its subset of EMBER_HIGH_PRIORITY_TASKS, these tasks do not
 *  necessarily prevent the device from going to sleep. However, they are
 *  taken into account when calculating how long we can sleep until the next event.
 */
enum {
  /** Messages are waiting for transmission. */
  EMBER_OUTGOING_MESSAGES = 0x0001,
  /** One or more incoming messages are being processed. */
  EMBER_INCOMING_MESSAGES = 0x0002,
  /** The radio is currently powered on.  On sleepy devices, the radio is
   *  turned off when not in use.  On non-sleepy devices (::EMBER_COORDINATOR,
   *  ::EMBER_ROUTER, or ::EMBER_END_DEVICE), the radio is always on.
   */
  EMBER_RADIO_IS_ON = 0x0004,
  /** The transport layer has messages awaiting an ACK. */
  EMBER_TRANSPORT_ACTIVE = 0x0008,
  /** The ZigBee APS layer has messages awaiting an ACK. */
  EMBER_APS_LAYER_ACTIVE = 0x0010,
  /** The node is currently trying to associate with a ZigBee PRO network. */
  EMBER_ASSOCIATING = 0x0020,
  /** The node is currently touch linking. */
  EMBER_ZLL_TOUCH_LINKING = 0x0040,
  /** Network Timeout Request Event. */
  EMBER_NETWORK_TIMEOUT_REQUEST = 0x0200,
  /** Sending Orphan Notification Event. */
  EMBER_SEND_ORPHAN_NOTIFICATION = 0x0400,
  /** MAC Data Poll Super Retry Event . */
  EMBER_MAC_DATA_POLL_SUPER_RETRY = 0x0800,
};

/** @brief A mask of the tasks that prevent a device from sleeping.
 */
#define EMBER_HIGH_PRIORITY_TASKS \
  (EMBER_OUTGOING_MESSAGES | EMBER_INCOMING_MESSAGES | EMBER_RADIO_IS_ON)

/** @brief Return a bitmask indicating the stack's current tasks.
 *
 *  The mask ::EMBER_HIGH_PRIORITY_TASKS defines which tasks are high
 *  priority.  Devices should not sleep if any high priority tasks are active.
 *  Active tasks that are not high priority are waiting for
 *  messages to arrive from other devices.  If there are active tasks,
 *  but no high priority ones, the device may sleep but should periodically
 *  wake up and call ::emberPollForData() in order to receive messages.  Parents
 *  will hold messages for ::EMBER_INDIRECT_TRANSMISSION_TIMEOUT milliseconds
 *  before discarding them.
 *
 * @return A bitmask of the stack's active tasks.
 */
uint16_t emberCurrentStackTasks(void);

/** @brief Indicate whether the stack is currently in a state where
 * there are no high-priority tasks, allowing the device to sleep.
 *
 * There may be tasks expecting incoming messages, in which case the device should
 * periodically wake up and call ::emberPollForData() in order to receive messages.
 * This function can only be called when the node type is
 * ::EMBER_SLEEPY_END_DEVICE
 *
 * @return True if the application may sleep but the stack may be expecting
 * incoming messages.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
bool emberOkToNap(void);
#else
#define emberOkToNap() \
  (!(emberCurrentStackTasks() & EMBER_HIGH_PRIORITY_TASKS))
#endif

/** @brief Indicate whether the parent token has been set by association.
 *
 * @return True if the parent token has been set.
 */
bool emberParentTokenSet(void);

/** @brief Indicate whether the stack currently has any tasks pending.
 *
 * If no tasks are pending, ::emberTick() does not need to be called until
 * the next time a stack API function is called. This function can only be
 * called when the node type is ::EMBER_SLEEPY_END_DEVICE or
 *
 * @return True if the application may sleep for as long as it wishes.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
bool emberOkToHibernate(void);
#else
#define emberOkToHibernate() (!emberCurrentStackTasks())
#endif

/** @brief Indicate whether the stack is currently in a state that does not
 * require the application to periodically poll.
 *
 * @return True if the application may stop polling periodically.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
bool emberOkToLongPoll(void);
#else
#define emberOkToLongPoll() (!emberPendingAckedMessages())
#endif

/** @brief Immediately turn the radio power completely off.
 *
 * Calling this function will render all other stack functions except
 * emberStackPowerUp() non-functional until the radio is powered back on.
 */
void emberStackPowerDown(void);

/** @brief Initialize the radio.  Typically called coming out of deep sleep.
 *
 * For non-sleepy devices, also turns the radio on and leaves it in RX mode.
 */
void emberStackPowerUp(void);

// @} END Power Management

// @} END addtogroup

/** @brief Send a link power delta request to the parent.
 */
EmberStatus emberSendLinkPowerDeltaRequest (void);

/** @brief Accessor function for recent average RSSI value between this device and the parent (SoC only).
 */
int8_t emberGetAvgParentRssi(void);

/** @brief Accessor function for the parent's node ID if applicable (SoC only).
 */
EmberNodeId emberGetParentId(void);

/** @brief: Bit mask that decides which timeout values are acceptable for a child.
 *  It defaults to 0xFFFF which covers all the following 15 options:
 *
 *  #define SECONDS_10              0
 *  #define MINUTES_2               1
 *  #define MINUTES_4               2
 *  #define MINUTES_8               3
 *  #define MINUTES_16              4
 *  #define MINUTES_32              5
 *  #define MINUTES_64              6
 *  #define MINUTES_128             7
 *  #define MINUTES_256             8
 *  #define MINUTES_512             9
 *  #define MINUTES_1024            10
 *  #define MINUTES_2048            11
 *  #define MINUTES_4096            12
 *  #define MINUTES_8192            13
 *  #define MINUTES_16384           14 // ~11 days
 *
 */
EmberStatus emberSetChildTimeoutOptionMask(uint16_t mask);

/**
 * <!-- HIDDEN
 * @page 2p5_to_3p0
 * <hr>
 * The file child.h is new and is described in @ref child.
 *
 * - <b>Items moved from ember.h</b>
 *   - emberChildCount()
 *   - emberChildId()
 *   - emberChildIndex()
 *   - emberChildJoinHandler()
 *   - emberClearMessageFlag()
 *   - emberCurrentStackTasks()
 *   - emberGetChildData()
 *   - emberGetParentEui64()
 *   - emberGetParentNodeId()
 *   - emberMaxChildCount()
 *   - emberMaxRouterChildCount()
 *   - emberOkToHibernate()
 *   - emberOkToNap()
 *   - emberPollCompleteHandler()
 *   - emberPollForData()
 *   - emberPollHandler()
 *   - emberRouterChildCount()
 *   - emberSetMessageFlag()
 *   - emberStackPowerDown()
 *   - emberStackPowerUp()
 *   .
 * .
 * HIDDEN -->
 */

#endif // SILABS_CHILD_H
