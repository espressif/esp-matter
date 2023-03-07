/***************************************************************************//**
 * @file
 * @brief Utilities for forming and joining networks.
 *
 * See @ref networks for documentation.
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

#ifndef SILABS_FORM_AND_JOIN_H
#define SILABS_FORM_AND_JOIN_H

/**
 * @addtogroup networks
 * Functions for finding an existing network to join and for
 * finding an unused PAN id with which to form a network.
 *
 * Summary of application requirements:
 *
 * For the SoC:
 * - Define ::EMBER_APPLICATION_HAS_ENERGY_SCAN_RESULT_HANDLER
 *   in the configuration header.
 * - Call ::emberFormAndJoinTick() regularly in the main loop.
 * - Include form-and-join.c and form-and-join-node-adapter.c in the build.
 * - Optionally include form-and-join-node-callbacks.c in the build.
 * - If processor idling is desired:
 *   -- Call ::emberFormAndJoinTaskInit() to initialize the form and join task
 *   -- Call ::emberFormAndJoinRunTask() regularly in the main loop instead of
 *      ::emberFormAndJoinTick()
 *
 * For an EZSP Host:
 * - Define ::EZSP_APPLICATION_HAS_ENERGY_SCAN_RESULT_HANDLER
 *   in the configuration header.
 * - Include form-and-join.c and form-and-join-host-adapter.c in the build.
 * - Optionally include form-and-join-host-callbacks.c in the build.
 *
 * For either platform, the application can omit the
 * form-and-join-*-callback.c file from the build and implement
 * the callbacks itself if necessary.  In this case, the appropriate
 * form-and-join callback function must be called from within each callback
 * as within the form-and-join-*-callback.c files.
 *
 * On either platform, FORM_AND_JOIN_MAX_NETWORKS can be explicitly defined to
 * limit (or expand) the number of joinable networks that the library will
 * save for consideration during the scan process.
 *
 * The library can resume scanning for joinable networks from where it
 * left off, via a call to emberScanForNextJoinableNetwork(). Therefore, if the
 * first joinable network found is not the correct one, the application can
 * continue scanning without starting from the beginning and without finding
 * the same network that it has already rejected.  The library can also be used
 * on the host processor.
 *
 * @{
 */

/** @brief Number of bytes required to store relevant info for a saved network.
 *
 * This constant represents the minimum number of bytes required to store all
 * members of the NetworkInfo struct used in the adapter code.  Its value should
 * not be changed unless the underlying adapter code is updated accordingly.
 * Note that this constant's value may be different than sizeof(NetworkInfo)
 * because some compilers pad the structs to align on word boundaries.
 * Thus, the adapter code stores/retrieves these pieces of data individually
 * (to be platform-agnostic) rather than as a struct.
 *
 * For efficiency's sake, this number should be kept to a power of 2 and not
 * and not exceed 32 (PACKET_BUFFER_SIZE).
 */
#define NETWORK_STORAGE_SIZE  16

/** @brief Log_base2 of ::NETWORK_STORAGE_SIZE
 */
#define NETWORK_STORAGE_SIZE_SHIFT 4

/** @brief Number of joinable networks that can be remembered during the scan
 * process.
 *
 * Note for SoC Platforms: This is currently limited to a maximum
 * of 15 due to the size of each network entry (16 bytes) and the
 * EmberMessageBuffer API's requirement that total buffer storage length be
 * kept to an 8-bit quantity (less than 256).
 *
 * Note for EZSP Host Platforms: In the host implementation of this library,
 * the storage size for the detected networks buffer is controlled by
 * ::EZSP_HOST_FORM_AND_JOIN_BUFFER_SIZE, so that limits the highest value that
 * the host can set for FORM_AND_JOIN_MAX_NETWORKS.
 */
#ifndef FORM_AND_JOIN_MAX_NETWORKS
  #ifdef EZSP_HOST
// the host's buffer is 16-bit array, so translate to bytes for comparison
    #define FORM_AND_JOIN_MAX_NETWORKS \
  (EZSP_HOST_FORM_AND_JOIN_BUFFER_SIZE * 2 / NETWORK_STORAGE_SIZE)
  #else
// use highest value that won't exceed max EmberMessageBuffer length
    #define FORM_AND_JOIN_MAX_NETWORKS 15
  #endif
#endif

// Check that this value isn't too large for the SoC implementation to handle
#ifndef EZSP_HOST
  #if (FORM_AND_JOIN_MAX_NETWORKS > 15)
    #error "FORM_AND_JOIN_MAX_NETWORKS can't exceed 15 on SoC platform"
  #endif
#endif

/** @brief Find an unused PAN ID.
 *
 * Does an energy scan on the indicated channels and randomly chooses
 * one from amongst those with the least average energy. Then
 * picks a short PAN ID that does not appear during an active
 * scan on the chosen channel.  The chosen PAN ID and channel are returned
 * via the ::emberUnusedPanIdFoundHandler() callback.  If an error occurs, the
 * application is informed via the ::emberScanErrorHandler().
 *
 * @param channelMask
 * @param duration  The duration of the energy scan.  See the documentation
 * for ::emberStartScan() in stack/include/network-formation.h for information
 * on duration values.
 * @return EMBER_LIBRARY_NOT_PRESENT if the form and join library is not
 * available.
 */
EmberStatus emberScanForUnusedPanId(uint32_t channelMask, uint8_t duration);

/** @brief Find a joinable network.
 *
 * Performs an active scan on the specified channels looking for networks that:
 * -# currently permit joining,
 * -# match the stack profile of the application,
 * -# match the extended PAN id argument if it is not NULL.
 *
 * Upon finding a matching network, the application is notified via the
 * emberJoinableNetworkFoundHandler() callback, and scanning stops.
 * If an error occurs during the scanning process, the application is
 * informed via the emberScanErrorHandler(), and scanning stops.
 *
 * If the application determines that the discovered network is not the correct
 * one, it may call emberScanForNextJoinableNetwork() to continue the scanning
 * process where it was left off and find a different joinable network.  If the
 * next network is not the correct one, the application can continue to call
 * emberScanForNextJoinableNetwork().  Each call must
 * occur within 30 seconds of the previous one, otherwise the state of the scan
 * process is deleted to free up memory.  Calling emberScanForJoinableNetwork()
 * causes any old state to be forgotten and starts scanning from the beginning.
 *
 * @param channelMask
 * @param extendedPanId
 * @return EMBER_LIBRARY_NOT_PRESENT if the form and join library is not
 * available.
 */
EmberStatus emberScanForJoinableNetwork(uint32_t channelMask, uint8_t* extendedPanId);

/** @brief See emberScanForJoinableNetwork(). */
EmberStatus emberScanForNextJoinableNetwork(void);

/** @brief Return true if and only if the form and join library is in the
 * process of scanning and is therefore expecting scan results to be passed
 * to it from the application.
 */
bool emberFormAndJoinIsScanning(void);

/** @brief Return true if and only if the application can continue a joinable
 * network scan by calling emberScanForNextJoinableNetwork().  See
 * emberScanForJoinableNetwork().
 */
bool emberFormAndJoinCanContinueJoinableNetworkScan(void);

//------------------------------------------------------------------------------
// Callbacks the application needs to implement.

/** @brief A callback the application needs to implement.
 *
 * Notifies the application of the network found after a call
 * to ::emberScanForJoinableNetwork() or
 * ::emberScanForNextJoinableNetwork().
 *
 * @param networkFound
 * @param lqi  The LQI value of the received beacon.
 * @param rssi The RSSI value of the received beacon.
 */
void emberJoinableNetworkFoundHandler(EmberZigbeeNetwork *networkFound,
                                      uint8_t lqi,
                                      int8_t rssi);

/** @brief A callback the application needs to implement.
 *
 * If an error occurs while scanning,
 * this function is called and the scan effort is aborted.
 *
 * Possible return status values are:
 * <ul>
 * <li> EMBER_INVALID_CALL: if emberScanForNextJoinableNetwork() is called
 * more than 30 seconds after a previous call to emberScanForJoinableNetwork()
 * or emberScanForNextJoinableNetwork().
 * <li> EMBER_NO_BUFFERS: if there is not enough memory to start a scan.
 * <li> EMBER_NO_BEACONS: if no joinable beacons are found.
 * <li> EMBER_MAC_SCANNING: if a scan is already in progress.
 * </ul>
 *
 * @param status
 */
void emberScanErrorHandler(EmberStatus status);

//------------------------------------------------------------------------------
// Library functions the application must call from within the
// corresponding EmberZNet or EZSP callback.

/** @brief The application must call this function from within its
 * emberScanCompleteHandler() (on the node) or ezspScanCompleteHandler()
 * (on an EZSP host).  Default callback implementations are provided
 * in the form-and-join-*-callbacks.c files.
 *
 * @return true if the library made use of the call.
 */
bool emberFormAndJoinScanCompleteHandler(uint8_t channel, EmberStatus status);

/** @brief The application must call this function from within its
 * emberNetworkFoundHandler() (on the node) or ezspNetworkFoundHandler()
 * (on an EZSP host).  Default callback implementations are provided
 * in the form-and-join-*-callbacks.c files.
 *
 * @return true if the library made use of the call.
 */
bool emberFormAndJoinNetworkFoundHandler(EmberZigbeeNetwork *networkFound,
                                         uint8_t lqi,
                                         int8_t rssi);

/** @brief The application must call this function from within its
 * emberEnergyScanResultHandler() (on the node) or ezspEnergyScanResultHandler()
 * (on an EZSP host).  Default callback implementations are provided
 * in the form-and-join-*-callbacks.c files.
 *
 * @return true if the library made use of the call.
 */
bool emberFormAndJoinEnergyScanResultHandler(uint8_t channel, int8_t maxRssiValue);

/** @brief The application must call this function from within its
 * emberUnusedPandIdFoundHandler() (on the node) or ezspEnergyScanResultHandler()
 * (on an EZSP host). Default callback implementations are provided
 * in the form-and-join-*-callbacks.c files.
 */
bool emberFormAndJoinUnusedPanIdFoundHandler(EmberPanId panId, uint8_t channel);

/** Used by the form and join code on the node to time out a joinable scan after
 * 30 seconds of inactivity. The application must call emberFormAndJoinTick()
 * regularly.  This function does not exist for the EZSP host library.
 */
void emberFormAndJoinTick(void);

/** @brief When processor idling is desired on the SOC, this must be called to
 *   properly initialize the form and join library
 */
void emberFormAndJoinTaskInit(void);

/** @brief When processor idling is desired on the SOC, this should be called
 *   regularly instead of ::emberFormAndJoinTick()
 */
void emberFormAndJoinRunTask(void);

/** @brief When form-and-join state is no longer needed, the application
 *   can call this routine to cleanup and free resources.  On the SOC
 *   platforms this will free the allocated message buffer.
 */
void emberFormAndJoinCleanup(EmberStatus status);

/** @brief Network found.
 *
 * This is called by the form-and-join library to notify the application of the
 * network found after a call to ::emberScanForJoinableNetwork() or
 * ::emberScanForNextJoinableNetwork(). See form-and-join documentation for
 * more information.
 *
 * @param networkFound   Ver.: always
 * @param lqi   Ver.: always
 * @param rssi   Ver.: always
 */
void emberAfPluginFormAndJoinNetworkFoundCallback(EmberZigbeeNetwork *networkFound,
                                                  uint8_t lqi,
                                                  int8_t rssi);

/** @brief Unused Pan ID found.
 *
 * This function is called when the form-and-join library finds an unused PAN
 * ID that can be used to form a new network on.
 *
 * @param panId A randomly generated PAN ID without other devices on it.
 * Ver.: always
 * @param channel The channel where the PAN ID can be used to form a new
 * network. Ver.: always
 */
void emberAfPluginFormAndJoinUnusedPanIdFoundCallback(EmberPanId panId,
                                                      uint8_t channel);

/** @} END addtogroup
 */

#endif // SILABS_FORM_AND_JOIN_H
