/***************************************************************************//**
 * @file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifdef SL_CATALOG_CONNECT_AES_SECURITY_PRESENT
#include <psa/crypto.h>
#endif

/**
 * @addtogroup stack_info
 * @brief Connect API for accessing and modifying stack states and behaviors.
 *
 * See stack-info.h for source code.
 * @{
 */

/** @brief A mask of the tasks that prevent a device from sleeping.
 */
#define EMBER_HIGH_PRIORITY_TASKS \
  (EMBER_OUTGOING_MESSAGES | EMBER_INCOMING_MESSAGES | EMBER_RADIO_IS_ON)

/**
 * @brief Invalid channel number
 *
 */
#define EMBER_INVALID_CHANNEL  65535

/** @name Handlers
 * @{
 */

/** @brief A callback invoked when the status of the stack changes.
 *
 * The application is free to begin messaging once it receives the
 * ::EMBER_NETWORK_UP status.
 *
 * @param[in] status  Stack status. One of the following:
 * - ::EMBER_NETWORK_UP
 * - ::EMBER_NETWORK_DOWN
 * - ::EMBER_NO_VALID_BEACONS
 * - ::EMBER_JOIN_SCAN_FAILED
 * - ::EMBER_JOIN_FAILED
 * - ::EMBER_JOIN_DENIED
 * - ::EMBER_JOIN_TIMEOUT
 * - ::EMBER_MAC_SYNC_TIMEOUT
 */
void emberStackStatusHandler(EmberStatus status);

/** @brief This handler is invoked in ISR context when certain stack-related ISR
 * routines fire.
 */
void emberStackIsrHandler(void);

/** @brief A callback to allow the application to manage idling the MCU.
 *
 * @param[in,out] idleTimeMs  A pointer to the time in millisecond the stack is
 * allowed to idle. If the application decides to manage idling the MCU, it
 * should update the passed value with the actual time the MCU was idled.
 *
 * @return   true if the application is managing idling the MCU, false otherwise.
 * If this function returns false, the stack will manage idling the MCU.
 */
bool emberStackIdleHandler(uint32_t *idleTimeMs);

/** @brief The radio calibration callback function.
 *
 * This handler is invoked by the stack upon receiving a "calibration needed"
 * event from the radio to inform the application that it should perform
 * calibration of the current channel as soon as possible using the
 * ::emberCalibrateCurrentChannel() API.
 * While calibration only takes tens of microseconds, the application can
 * failsafe any critical processes or peripherals before calling
 * ::emberCalibrateCurrentChannel(). The application must call
 * ::emberCalibrateCurrentChannel() in response to this callback to maintain
 * expected radio performance.
 */
void emberRadioNeedsCalibratingHandler(void);

/** @} //END Handlers
 */

/** @name APIs
 * @{
 */

/** @brief Immediately turns the radio power completely off.
 *
 * After calling this function, do not call any other stack function
 * except emberStackPowerUp() because all other stack
 * functions require that the radio is powered to
 * operate properly.
 */
void emberStackPowerDown(void);

/** @brief Power up the radio.  Typically called coming out of sleep.
 *
 * For non-sleepy devices, also turns the radio on and leaves it in RX mode.
 */
void emberStackPowerUp(void);

/** @brief Return the current join status.
 *
 *   Returns a value indicating whether the node is joining, joined to, or
 *   leaving a network.
 *
 * @return An ::EmberNetworkStatus value indicating the current join status.
 */
EmberNetworkStatus emberNetworkState(void);

/** @brief Indicate whether the stack is currently up.
 *
 *   Returns true if the stack is joined to a network and ready to send and
 *   receive messages.  This reflects only the state of the local node and does
 *   not indicate whether or not other nodes are able to communicate with this
 *   node.
 *
 * @return \b true if the stack is up, \b false otherwise.
 */
bool emberStackIsUp(void);

/** @brief Write a key at the address of the formerly used security key. The
 *  key set by this function will not be used by the stack. The API is meant to
 *  be used to erase a key as it is now managed by PSA Crypto API.
 *
 * @param[in] key   An ::EmberKeyData value containing the security key to be
 * set.
 *
 * @return An EmberStatus value of ::EMBER_SUCCESS if the key was successfully
 *   set. Otherwise, it returns an EmberStatus value of ::EMBER_INVALID_CALL.
 */
EmberStatus emberSetSecurityKey(EmberKeyData *key);

/** @brief Get the legacy security key. This function does not return the value
 *  set with the PSA Crypto API.
 *
 * @param[in] key   An ::EmberKeyData where the legcay security key will be stored
 *
 * @return An EmberStatus value of ::EMBER_SUCCESS if the key was successfully
 *   read.
 */
EmberStatus emberGetSecurityKey(EmberKeyData *key);

#ifdef SL_CATALOG_CONNECT_AES_SECURITY_PRESENT
/**
 * @brief Register a PSA Cyrpto key. It must comply to Connect stack key format:
 *  128 bits AES key with encrypt and decrypt permissions.
 *
 * @param key_id A PSA Crypto key identifier.
 * @return An EmberStatus value of ::EMBER_SUCCESS if the key was successfully
 *  imported. If the function failed, it returns:
 *  - EMBER_BAD_ARGUMENT if the key was not imported calling psa_import_key or any
 *  equivalent.
 *  - EMBER_SECURITY_DATA_INVALID if the key does not match the Connect stack
 *  key format.
 *
 */
EmberStatus emberSetPsaSecurityKey(mbedtls_svc_key_id_t key_id);

/**
 * @brief Unregister the PSA Crypto key id previously set with ::emberSetPsaSecurityKey.
 * This function does not destroy the PSA Crypto key. It is the application
 * responsibility to call psa_destroy_key.
 *
 * @return An EmberStatus value of ::EMBER_SUCCESS if the key was successfully
 *  removed.
 */
EmberStatus emberRemovePsaSecurityKey(void);
#endif

/** @brief Set the channel for sending and receiving messages on the current
 * network. The available channels depend on the radio config you use. Channels
 * can differ more than the frequency if it's a multi-PHY config.
 *
 * @note Care should be taken when using this API.
 *       All devices on a network must use the same channel.
 *
 * @param[in] channel  A desired radio channel.
 *
 * @param[in] persistent  A flag to instruct the stack to save the channel
 * setting in persistent or not. Each persistent call triggers a token write.
 * Excessive usage might cause flash to wear-out.
 *
 * @return An ::EmberStatus value of:
 * - ::EMBER_SUCCESS if the stack accepted the channel change.
 * - ::EMBER_INVALID_CALL if the node is currently performing frequency hopping.
 * - ::EMBER_PHY_INVALID_CHANNEL if the passed channel is invalid.
 * - ::EMBER_MAC_BUSY if the MAC is currently performing a high priority
 *   task.
 */
EmberStatus emberSetRadioChannelExtended(uint16_t channel, bool persistent);

#if defined(DOXYGEN_SHOULD_SKIP_THIS) || defined(UNIX_HOST)
/** @brief Set the channel for sending and receiving messages on the current
 * network. The available channels depend on the radio config you use. Channels
 * can differ more than the frequency if it's a multi-PHY config.
 *
 * @note Care should be taken when using this API.
 *       All devices on a network must use the same channel.
 *       Each call triggers a token write.
 *       Excessive usage might cause flash to wear-out.
 *
 * @param[in] channel  A desired radio channel.
 *
 * @return An ::EmberStatus value of:
 * - ::EMBER_SUCCESS if the stack accepted the channel change.
 * - ::EMBER_INVALID_CALL if the node is currently performing frequency hopping.
 * - ::EMBER_PHY_INVALID_CHANNEL if the passed channel is invalid.
 * - ::EMBER_MAC_BUSY if the MAC is currently performing a high priority
 *   task.
 */
EmberStatus emberSetRadioChannel(uint16_t channel);
#else
#define emberSetRadioChannel(channel) \
  (emberSetRadioChannelExtended((channel), 1))
#endif // DOXYGEN_SHOULD_SKIP_THIS

/** @brief Get the radio channel, to which a node is set, on the current
 * network. The available channels depend on the radio config you use.
 * Channels can differ more than the frequency if it's a multi-PHY config.
 *
 * @return The current radio channel.
 */
uint16_t emberGetRadioChannel(void);

/** @brief Get the first available channel in the current radio configuration.
 *
 * @return The first available channel in the radio configuration.
 *         0xffff if error
 */
uint16_t emberGetDefaultChannel(void);

/** @brief Indicate if the PHY configuration of the stack. Currently only
 * supporting ::EMBER_RADIO_CONFIGURATOR and ::EMBER_STANDARD_PHY_2_4GHZ.
 * It must be called before initializing the stack.
 *
 * @return ::EMBER_BAD_ARGUMENT if phyType is incorrect
 *         ::EMBER_INVALID_CALL if API is called after stack initialization
 *         ::EMBER_SUCCESS otherwise
 */
EmberStatus emberPhyConfigInit(EmberPhyType phyType);

/** @brief Perform image rejection calibration on the current channel. The
 * stack will notify the application that it needs channel calibration via the
 * ::emberRadioNeedsCalibratingHandler() callback function during
 * ::emberTick().  This function should only be called from within the context
 * of the ::emberRadioNeedsCalibratingHandler() callback function. Note if this
 * function is called when the radio is off, it will turn the radio on and leave
 * it on.
 *
 * @param[in] calValueIn  the calibration value to use.
 * Set to EMBER_CAL_INVALID_VALUE to perform automatic calibration.
 *
 * @param[out] calValueOut  a pointer to the calibration value that was used.
 * This parameter is ignored when set to NULL.
 *
 * @return An ::EmberStatus value indicating the success or failure of the
 * command.
 */
EmberStatus emberCalibrateCurrentChannelExtended(uint32_t calValueIn,
                                                 uint32_t *calValueOut);

#if defined(DOXYGEN_SHOULD_SKIP_THIS) || defined(UNIX_HOST)
/** @brief Perform image rejection calibration on the current channel. The
 * stack will notify the application that it needs channel calibration via the
 * ::emberRadioNeedsCalibratingHandler() callback function during
 * ::emberTick().  This function should only be called from within the context
 * of the ::emberRadioNeedsCalibratingHandler() callback function. Note if this
 * function is called when the radio is off, it will turn the radio on and leave
 * it on.
 */
void emberCalibrateCurrentChannel(void);
#else
#define emberCalibrateCurrentChannel() \
  (emberCalibrateCurrentChannelExtended(EMBER_CAL_INVALID_VALUE, NULL))
#endif

/** @brief Apply Image Rejection calibration on the current channel. The
 * stack will notify the application that it needs channel calibration via the
 * ::emberRadioNeedsCalibratingHandler() callback function during
 * ::emberTick().  This function should only be called from within the context
 * of the ::emberRadioNeedsCalibratingHandler() callback function. Note if this
 * function is called when the radio is off, it will turn the radio on and leave
 * it on.
 *
 * @param[in] calValueIn  the calibration value to apply. Should not be set
 * to EMBER_CAL_INVALID_VALUE.
 *
 * @return An ::EmberStatus value indicating the success or failure of the
 * command.
 */
EmberStatus emberApplyIrCalibration(uint32_t calValue);

/** @brief Perform Temperature VCO calibration calibration on the current channel. The
 * stack will notify the application that it needs channel calibration via the
 * ::emberRadioNeedsCalibratingHandler() callback function during
 * ::emberTick().  This function should only be called from within the context
 * of the ::emberRadioNeedsCalibratingHandler() callback function. Note if this
 * function is called when the radio is off, it will turn the radio on and leave
 * it on.
 *
 * @return An ::EmberStatus value indicating the success or failure of the
 * command.
 */
EmberStatus emberTempCalibration(void);

/** @brief Fetch calibration type associated to the latest
 * emberRadioNeedsCalibratingHandler() callback.
 *
 * @return An ::EmberCalType value indicating which type of calibration should
 * be performed.
 */
EmberCalType emberGetCalType(void);

/** @brief Set the radio output power at which a node is to operate for the
 * current network. The radio has a finite power resolution, so it will
 * approximate the requested power with the closest possible value at or below
 * the requested value.
 * @note Care should be taken when using this API on a running network,
 * because it directly impacts the established link qualities neighboring nodes
 * have with the node on which it is called.  This can lead to disruption of
 * existing routes and erratic network behavior.
 *
 * @param[in] power  Desired radio output power, in deci-dBm.
 *
 * @param[in] persistent  A flag to instruct the stack to save the power setting
 * in persistent or not.
 *
 * @return An ::EmberStatus value indicating the success or failure of the
 * command.  Failure indicates that the requested power level is out of range.
 */
EmberStatus emberSetRadioPower(int16_t power, bool persistent);

/** @brief Get the radio output power of the current network at which
 * a node is operating. This might be different to what you set using
 * ::emberSetRadioPower because the radio has a finite power resolution,
 * and ::emberSetRadioPower must approximate to the closest possible value at or
 * below the requested value. This API however returns with the actual setting.
 *
 * @return Current radio output power, in deci-dBm.
 */
int16_t emberGetRadioPower(void);

/** @brief Allow the application to turn the radio on/off. This API is
 * intended for use with direct devices only.
 *
 * @param[in] radioOn  If this parameter is \b true, the radio is turned on,
 * otherwise it's turned off.
 *
 * @return An ::EmberStatus value indicating the success or failure of the
 * command.  Failure indicates that the node type is a type other than
 * ::EMBER_DIRECT_DEVICE.
 */
EmberStatus emberSetRadioPowerMode(bool radioOn);

/** @brief Set the MAC layer transmission parameters.
 *
 * @param[in] ccaThreshold   The CCA RSSI threshold, in dBm, above which the
 *                         channel is considered 'busy'. This parameter is by
 *                         default set to \b ::EMBER_RADIO_CCA_THRESHOLD.
 *
 * @param[in] maxCcaAttempts   The maximum number of clear channel assessment
 *                         attempts that are performed prior to fail to
 *                         transmit a packet with ::EMBER_PHY_TX_CCA_FAIL
 *                         status. This parameter is set by default to 4.
 *                         If this parameter is set to 0, the CCA assessment
 *                         shall not be performed.
 *
 * @param[in] minBackoffExp   The backoff exponent used if the initial channel
 *                        clear assessment fails. This parameter is set by
 *                        default to 3. Note: this is meaningful only if the
 *                        \ref checkCca parameter is set to \b true.
 *
 * @param[in] maxBackoffExp   The backoff exponent used if the final channel
 *                        clear assessment fails. This parameter is set by
 *                        default to 5. Note: this is meaningful only if the
 *                        \ref checkCca parameter is set to \b true.
 *
 * @param[in] ccaBackoff   The backoff unit period in microsecond. It is
 *                     multiplied by the random backoff exponential controlled
 *                     by \ref minBackoffExp and \ref maxBackoffExp to determine
 *                     the overall backoff period. This parameter is set by
 *                     default to the PHY symbol time in microseconds multiplied
 *                     by 20.
 *
 * @param[in] ccaDuration    The minimum desired CCA check duration in
 *                      microseconds. This parameter is set by default to the
 *                      PHY symbol time in microseconds multiplied by 8.
 *
 * @param[in] maxRetries      The number of transmission retries that is
 *                        performed if no acknowledgment was received.
 *                        This parameter is set by default to 3 (which means
 *                        that a total of 4 transmission attempts will be
 *                        performed).
 *
 * @param[in] csmaTimeout    An overall timeout in microsecond time base for the
 *                        the CSMA operations. This value is set by default to
 *                        \b 0 which means that no timeout is imposed.
 *
 * @param[in] ackTimeout     The ack timeout in microseconds after which the
 *                        transmitting gives up waiting for an acknowledgment.
 *                        This parameter is set by default to
 *                        \b (::EMBER_MAC_ACK_TIMEOUT_MS * 1000).
 *
 * @note The CSMA/CA (CCA) values are directly used in RAIL's
 * @p RAIL_CsmaConfig_t and further information can be found in the RAIL API
 * documentation.
 * @return An ::EmberStatus value indicating whether the MAC parameters were
 * successfully set or the reason of failure.
 */
EmberStatus emberSetMacParams(int8_t ccaThreshold,
                              uint8_t maxCcaAttempts,
                              uint8_t minBackoffExp,
                              uint8_t maxBackoffExp,
                              uint16_t ccaBackoff,
                              uint16_t ccaDuration,
                              uint8_t maxRetries,
                              uint32_t csmaTimeout,
                              uint16_t ackTimeout);

/** @brief Retrieve the parent address. This API can be invoked only for nodes
 * of ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE type.
 *
 * @return An ::EmberStatus value of ::EMBER_SUCCESS if the parent address was
 * successfully retrieved, otherwise an ::EmberStatus value indicating the
 * reason of failure.
 */
EmberStatus emberMacGetParentAddress(EmberMacAddress *parentAddress);

/** @brief Return the time in milliseconds the stack could idle for.
 *
 * @param[in] currentStackTasks  A pointer to an integer that is written with
 *                          the active stack tasks at the time of the API call.
 *
 * @return Allowed idle time in milliseconds.
 */
uint32_t emberStackIdleTimeMs(uint16_t *currentStackTasks);

/** @brief Return a bitmask indicating the stack's current tasks.
 *
 *  The mask ::EMBER_HIGH_PRIORITY_TASKS defines which tasks are
 *  high-priority.  Devices should not sleep if any high-priority tasks
 *  are active. Active tasks that are not high-priority are waiting for
 *  messages to arrive from other devices.  If there are active tasks,
 *  but no high-priority ones, the device may sleep but should periodically
 *  wake up and call ::emberPollForData() to receive messages.  Parents
 *  will hold messages for ::EMBER_INDIRECT_TRANSMISSION_TIMEOUT_MS milliseconds
 *  before discarding them.
 *
 * @return A bitmask of the stack's active tasks.
 */
uint16_t emberCurrentStackTasks(void);

/** @brief Indicate whether the stack is currently in a state
 * with no high-priority tasks and may sleep.
 *
 * Tasks may be expecting incoming messages, in which case the device
 * should periodically wake up and call ::emberPollForData() to receive
 * messages. This function can only be called when the node type is
 * ::EMBER_STAR_SLEEPY_END_DEVICE.
 *
 * @return \b true if the application may sleep but the stack may be expecting
 * incoming messages.
 */
#if defined(DOXYGEN_SHOULD_SKIP_THIS) || defined(UNIX_HOST)
bool emberOkToNap(void);
#else
#define emberOkToNap() \
  (!(emberCurrentStackTasks() & EMBER_HIGH_PRIORITY_TASKS))
#endif

/** @brief Indicate whether the stack currently has any pending tasks.
 *
 * If no tasks are pending , ::emberTick() does not need to be called
 * until next stack API function is called. This function can only be
 * called when the node type is ::EMBER_STAR_SLEEPY_END_DEVICE.
 *
 * @return \b true if the application may sleep for as long as it wishes.
 */
#if defined(DOXYGEN_SHOULD_SKIP_THIS) || defined(UNIX_HOST)
bool emberOkToHibernate(void);
#else
#define emberOkToHibernate() (!emberCurrentStackTasks())
#endif

#if defined(DOXYGEN_SHOULD_SKIP_THIS) || defined(UNIX_HOST)
/** @brief Return the EUI64 ID of the local node.
 *
 * @return The 64-bit ID.
 */
#ifdef UNIX_HOST
uint8_t* emberGetEui64(void);
#else
EmberEUI64 emberGetEui64(void);
#endif

/** @brief Determine whether \c eui64 is the local node's EUI64 ID. EUI64 is
 * easily accessible in SoC mode, but in Host-NCP, the address is stored on the
 * NCP. This API can be used on the Host to compare a value with the locally
 * stored one.
 *
 * @param[in] eui64  An EUI64 ID.
 *
 * @return \b true if \c eui64 is the local node's ID, otherwise \b false.
 */
bool emberIsLocalEui64(EmberEUI64 eui64);

/** @brief Return the 16-bit node ID of local node on the current network.
 *
 * @return The 16-bit ID. Byte order is little endian.
 */
EmberNodeId emberGetNodeId(void);

/** @brief Return the local node's PAN ID of the current network.
 *
 * @return The PAN ID.
 */
EmberPanId emberGetPanId(void);

/** @brief Return an ::EmberNodeType value indicating the type of the node.
 *
 * @return The node type.
 */
EmberNodeType emberGetNodeType(void);

/** @brief Return the parent's node ID.
 *
 * @return The parent's node ID.
 */
EmberNodeId emberGetParentId(void);

#else // Doxygen ignores the following
extern EmberNodeId emLocalNodeId;
#define emberGetNodeId() (emLocalNodeId)
extern EmberPanId emLocalPanId;
#define emberGetPanId() (emLocalPanId)
extern EmberNodeId emParentId;
#define emberGetParentId() (emParentId)
extern EmberNodeType emNodeType;
#define emberGetNodeType() (emNodeType)
extern EmberEUI64 emLocalEui64;
#define emberGetEui64() (emLocalEui64)
#define emberIsLocalEui64(eui64) \
  (MEMCOMPARE((eui64), emLocalEui64, EUI64_SIZE) == 0)

#endif // DOXYGEN_SHOULD_SKIP_THIS

/**
 * @}
 *
 * @}
 */

/**
 * @addtogroup stack_counters
 * @brief Stack counters API
 *
 * @{
 */

/** @brief Retrieve the stack counter corresponding to the passed counter type.
 *
 * @param[in] counterType   An ::EmberCounterType value indicating the stack
 * counter to be retrieved.
 *
 * @param[out] count  The counter of the requested @p counterType is returned
 *  here
 *
 * @return An EmberStatus value of ::EMBER_SUCCESS if the stack counter was
 * successfully retrieved. An EmberStatus value of ::EMBER_INVALID_CALL if the
 * passed counterType is invalid. An EmberStatus value of
 * ::EMBER_LIBRARY_NOT_PRESENT if the stack counter library is not present.
 */
EmberStatus emberGetCounter(EmberCounterType counterType, uint32_t *count);

/**
 * @}
 */

/**
 * @addtogroup parent_support
 * @{
 *
 * @name Handlers
 * @{
 */

/** @brief Invoked at coordinator, range extender, or mac mode nodes when a new
 * child has joined the device.
 *
 * @param[in] nodeType  The role of the joining device (::EMBER_STAR_RANGE_EXTENDER,
 * ::EMBER_STAR_END_DEVICE, ::EMBER_STAR_SLEEPY_END_DEVICE,
 * ::EMBER_MAC_MODE_DEVICE or ::EMBER_MAC_MODE_SLEEPY_DEVICE).
 *
 * @param[in] nodeId    The node ID of the joining device.
 */
void emberChildJoinHandler(EmberNodeType nodeType,
                           EmberNodeId nodeId);
/**
 * @}
 */
