/***************************************************************************//**
 * @file
 * @brief The main header file for the RAIL library. It describes the external
 *   APIs available to a RAIL user
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __RAIL_H__
#define __RAIL_H__

#include <string.h> // For memcpy()

// Get the RAIL-specific structures and types
#include "rail_types.h"
#include "rail_assert_error_codes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RAIL_API RAIL API
 * @brief This is the primary API layer for the Radio Abstraction Interface
 *   Layer (RAIL)
 * @{
 */

/**
 * @defgroup Protocol_Specific Protocol-specific
 * @brief Protocol-Specific RAIL APIs
 */

/******************************************************************************
 * General Radio Operation
 *****************************************************************************/
/**
 * @addtogroup General
 * @brief Basic APIs to set up and interact with the RAIL library
 * @{
 */

/**
 * Get the version information for the compiled RAIL library.
 *
 * @param[out] version A pointer to \ref RAIL_Version_t structure to
 *   populate with version information.
 * @param[in] verbose Populate \ref RAIL_Version_t struct with verbose
 *   information.
 *
 * The version information contains a major version number, a minor version
 * number, and a rev (revision) number.
 */
void RAIL_GetVersion(RAIL_Version_t *version, bool verbose);

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * A global pointer to the head of a linked list of state buffers
 * \ref RAIL_Init() can use.
 *
 * RAIL internally provides one statically-allocated RAM state buffer
 * for a single protocol and two for dynamic multiprotocol. If your
 * application needs more, they can be provided via \ref
 * RAIL_AddStateBuffer3() or RAIL_AddStateBuffer4(), which use
 * internal buffers, or the more general \ref RAIL_AddStateBuffer().
 *
 * This symbol is WEAK in the RAIL library in case an application wants
 * to allocate and provide its own buffers. However, this use is highly
 * discouraged.
 */
extern RAIL_StateBufferEntry_t *RAIL_StateBufferHead;

/**
 * Get the run-time size of the radio's state buffer.
 *
 * @param[in] genericRailHandle A generic RAIL instance handle.
 * @return Size, in bytes, of the radio's internal state buffer.
 *   If the handle is invalid, 0 is returned.
 *
 * See \ref RAIL_STATE_BUFFER_BYTES for a compile-time estimated size
 * definition, which may be larger than what this function returns.
 */
uint32_t RAIL_GetStateBufferSize(RAIL_Handle_t genericRailHandle);

/**
 * Add an app-provided state buffer to the \ref RAIL_StateBufferHead list.
 *
 * @param[in] genericRailHandle A generic RAIL instance handle.
 * @param[in] newEntry pointer to a \ref RAIL_StateBufferEntry_t to
 *   add to the liked list of state buffers headed by
 *   \ref RAIL_StateBufferHead. Both the \ref RAIL_StateBufferEntry_t
 *   to which this parameter points and the \ref
 *   RAIL_StateBufferEntry_t::buffer to which that points must be
 *   allocated in RAM and persist indefinitely beyond this call.
 * @return Status code indicating success of the function call.
 *   An error should be returned if the entry's
 *   \ref RAIL_StateBufferEntry_t::bufferBytes is too small or
 *   the RAIL_StateBufferEntry_t::buffer pointer seems invalid.
 *
 * RAIL's internal \ref RAIL_StateBufferHead should prove
 * sufficient for most applications, providing one (single protocol)
 * or two (dynamic multiprotocol) buffers preallocated in RAM for
 * use by \ref RAIL_Init(). This function exists for dynamic
 * multiprotocol applications that needs more than two protocols, or
 * that prefer to dynamically allocate RAIL state buffers just prior
 * to calling \ref RAIL_Init() rather than having them statically
 * allocated in RAM.
 */
RAIL_Status_t RAIL_AddStateBuffer(RAIL_Handle_t genericRailHandle,
                                  RAIL_StateBufferEntry_t *newEntry);

#endif//DOXYGEN_SHOULD_SKIP_THIS

/**
 * Add a 3rd multiprotocol internal state buffer for use by \ref RAIL_Init().
 *
 * @param[in] genericRailHandle A generic RAIL instance handle.
 * @return Status code indicating success of the function call.
 *   An error is returned if the 3rd state buffer was previously added
 *   or this isn't the RAIL multiprotocol library.
 */
RAIL_Status_t RAIL_AddStateBuffer3(RAIL_Handle_t genericRailHandle);

/**
 * Add a 4th multiprotocol internal state buffer for use by \ref RAIL_Init().
 *
 * @param[in] genericRailHandle A generic RAIL instance handle.
 * @return Status code indicating success of the function call.
 *   An error is returned if the 4th state buffer was previously added.
 *   or this isn't the RAIL multiprotocol library.
 */
RAIL_Status_t RAIL_AddStateBuffer4(RAIL_Handle_t genericRailHandle);

/**
 * Allocate a DMA channel for RAIL to work with.
 *
 * @param[in] channel The DMA channel to use when copying memory. If a value of
 *   RAIL_DMA_INVALID is passed, RAIL will stop using any DMA channel.
 * @return Status code indicating success of the function call.
 *
 * To use this API, the application must initialize the DMA engine
 * on the chip and allocate a DMA channel. This channel will be used
 * periodically to copy memory more efficiently. Call this function
 * before RAIL_Init to have the most benefit. If the application needs
 * to take back control of the DMA channel that RAIL is using, this API may be
 * called with a channel of RAIL_DMA_INVALID to tell RAIL to stop using DMA.
 */
RAIL_Status_t RAIL_UseDma(uint8_t channel);

/**
 * Initialize RAIL.
 *
 * @param[in,out] railCfg The configuration and state structure for setting up
 *   the library, which contains memory and other options that RAIL needs.
 *   This structure must be allocated in application global read-write
 *   memory. RAIL may modify fields within or referenced by this structure
 *   during its operation.
 * @param[in] cb A callback that notifies the application when the radio is
 *   finished initializing and is ready for further configuration. This
 *   callback is useful for potential transceiver products that require a
 *   power up sequence before further configuration is available. After the
 *   callback fires, the radio is ready for additional configuration before
 *   transmit and receive operations.
 * @return Handle for initialized rail instance or NULL if an
 *   invalid value was passed in the railCfg.
 *
 * @note Call this function only once per protocol. If called
 *   again, it will do nothing and return NULL.
 */
RAIL_Handle_t RAIL_Init(RAIL_Config_t *railCfg,
                        RAIL_InitCompleteCallbackPtr_t cb);

/**
 * Get RAIL initialization status.
 *
 * @return True if the radio has finished initializing and
 *   false otherwise.
 *
 * RAIL APIs, e.g., RAIL_GetTime(), which work only if RAIL_Init() has been called,
 * can use RAIL_IsInitialized() to determine whether RAIL has been initialized or not.
 */
bool RAIL_IsInitialized(void);

/**
 * Collect entropy from the radio if available.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[out] buffer The buffer to write the collected entropy.
 * @param[in] bytes The number of bytes to fill in the input buffer.
 * @return Returns the number of bytes of entropy collected. For
 *   chips that don't support entropy collection, the function returns 0.
 *   Values less than the requested amount may also be returned on platforms
 *   that use entropy pools to collect random data periodically.
 *
 * Attempts to fill the provided buffer with the requested number of bytes of
 * entropy. If the requested number of bytes can't be provided, as many
 * bytes as possible will be filled and returned. For chips
 * that do not support this function, 0 bytes are always returned. For
 * information about the specific mechanism for gathering entropy, see
 * documentation for the chip family.
 */
uint16_t RAIL_GetRadioEntropy(RAIL_Handle_t railHandle,
                              uint8_t *buffer,
                              uint16_t bytes);

/** @} */ // end of group General

/******************************************************************************
 * PTI
 *****************************************************************************/
/**
 * @addtogroup PTI Packet Trace (PTI)
 * @brief Basic APIs to set up and interact with PTI settings
 * @{
 */

/**
 * Configure PTI pin locations, serial protocols, and baud rates.
 *
 * @param[in] railHandle A RAIL instance handle (currently not used).
 * @param[in] ptiConfig A configuration structure applied to the
 *   relevant PTI registers. A NULL ptiConfig will produce undefined
 *   behavior.
 * @return Status code indicating success of the function call.
 *
 * This method must be called before RAIL_EnablePti() is called.
 * Although a RAIL handle is included for potential future
 * expansion of this function, it is currently not used. That is,
 * there is only one PTI configuration that can be active on a
 * chip, regardless of the number of protocols (unless the application
 * updates the configuration upon a protocol switch),
 * and the configuration is not saved in the RAIL instance. For optimal
 * future compatibility, pass in a chip-specific handle, such as
 * \ref RAIL_EFR32_HANDLE.
 */
RAIL_Status_t RAIL_ConfigPti(RAIL_Handle_t railHandle,
                             const RAIL_PtiConfig_t *ptiConfig);

/**
 * Get the currently-active PTI configuration.
 *
 * @param[in] railHandle A RAIL instance handle (currently not used).
 * @param[out] ptiConfig A configuration structure filled with the active
 *   PTI configuration.
 * @return RAIL status indicating success of the function call.
 *
 * Although most combinations of configurations can be set, it is safest
 * to call this method after configuration to confirm which values were
 * actually set. As in RAIL_ConfigPti, railHandle is not used. This function
 * always returns the single active PTI configuration regardless of the
 * active protocol. For optimal future compatibility, pass in a
 * chip-specific handle, such as \ref RAIL_EFR32_HANDLE.
 */
RAIL_Status_t RAIL_GetPtiConfig(RAIL_Handle_t railHandle,
                                RAIL_PtiConfig_t *ptiConfig);

/**
 * Enable Packet Trace Interface (PTI) output of packet data.
 *
 * @param[in] railHandle A RAIL instance handle (currently not used).
 * @param[in] enable PTI is enabled if true; disable if false.
 * @return RAIL status indicating success of the function call.
 *
 * Similarly to having only one PTI configuration per chip,
 * PTI can only be enabled or disabled for all protocols. It cannot
 * be individually set to enabled and disabled per protocol
 * (unless the application switches it when
 * the protocol switches), and enable/disable is not saved as part of the
 * RAIL instance. For optimal future compatibility, pass in a chip-specific
 * handle, such as \ref RAIL_EFR32_HANDLE.
 */
RAIL_Status_t RAIL_EnablePti(RAIL_Handle_t railHandle,
                             bool enable);

/**
 * Set a protocol that RAIL outputs on PTI.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] protocol The enumeration representing which protocol the node is using.
 * @return Status code indicating success of the function call.
 *
 * The protocol is output via PTI for each packet.
 * Before any protocol is set, the default value is \ref
 * RAIL_PTI_PROTOCOL_CUSTOM. Use one of the enumeration values so that
 * the Network Analyzer can decode the packet.
 *
 * @note This function cannot be called unless the radio is currently in the
 * \ref RAIL_RF_STATE_IDLE or \ref RAIL_RF_STATE_INACTIVE states. For this
 * reason, call this function early on before starting radio
 * operations and not changed later.
 */
RAIL_Status_t RAIL_SetPtiProtocol(RAIL_Handle_t railHandle,
                                  RAIL_PtiProtocol_t protocol);

/**
 * Get the protocol that RAIL outputs on PTI.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return PTI protocol in use.
 */
RAIL_PtiProtocol_t RAIL_GetPtiProtocol(RAIL_Handle_t railHandle);

/** @} */ // end of group PTI

/******************************************************************************
 * Antenna Control
 *****************************************************************************/
/**
 * @addtogroup Antenna_Control Antenna Control
 * @brief Basic APIs to control the antenna functionality
 * @{
 */
/**
 * Configure antenna path and pin locations.
 *
 * @warning This API must be called before any TX or RX occurs. Otherwise,
 *   the antenna configurations for those functions will not take effect.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] config A configuration structure applied to the relevant Antenna
 *   Configuration registers. A NULL configuration will produce undefined behavior.
 * @return Status code indicating success of the function call.
 *
 * This function informs RAIL how to select each antenna, but not when.
 * Antenna selection for receive is controlled by the
 * \ref RAIL_RxOptions_t::RAIL_RX_OPTION_ANTENNA0 and
 * \ref RAIL_RxOptions_t::RAIL_RX_OPTION_ANTENNA1 options
 * (and the \ref RAIL_RxOptions_t::RAIL_RX_OPTION_ANTENNA_AUTO combination).
 * Antenna selection for transmit is controlled by the
 * \ref RAIL_TxOptions_t::RAIL_TX_OPTION_ANTENNA0 and
 * \ref RAIL_TxOptions_t::RAIL_TX_OPTION_ANTENNA1 options.
 *
 * Although a RAIL handle is included for potential future
 * expansion of this function, it is currently not used. That is,
 * only one antenna configuration can be active on a
 * chip, regardless of the number of protocols (unless the application
 * updates the configuration upon a protocol switch),
 * and the configuration is not saved in the RAIL instance. For optimal
 * future compatibility, pass in a chip-specific handle, such as
 * \ref RAIL_EFR32_HANDLE.
 */
RAIL_Status_t RAIL_ConfigAntenna(RAIL_Handle_t railHandle,
                                 const RAIL_AntennaConfig_t *config);

/**
 * Get the default RF path.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[out] rfPath Pointer to RF path.
 * @return A status code indicating success of the function call.
 *
 * If multiple protocols are used, this function returns
 * \ref RAIL_STATUS_INVALID_STATE if it is called and the given railHandle is
 * not active. In that case, the caller must attempt to re-call this function later,
 * for example when \ref RAIL_EVENT_CONFIG_SCHEDULED trigger.
 */
RAIL_Status_t RAIL_GetRfPath(RAIL_Handle_t railHandle, RAIL_AntennaSel_t *rfPath);

/** @} */ // end of group Antenna_Control

/******************************************************************************
 * Radio Configuration
 *****************************************************************************/
/// @addtogroup Radio_Configuration Radio Configuration
/// @brief Routines for setting up and querying radio configuration information.
///
/// These routines allow for runtime flexibility in the radio
/// configuration. Some of the parameters, however, are meant to be generated
/// from the radio calculator in Simplicity Studio. The basic code to configure
/// the radio from this calculator output looks like the example below.
///
/// @code{.c}
/// // Associate a specific channel configuration with a particular RAIL instance and
/// // load the settings that correspond to the first usable channel.
/// RAIL_ConfigChannels(railHandle, channelConfigs[0]);
/// @endcode
///
/// For more information about the types of parameters that can be changed in
/// the other functions and how to use them, see their individual documentation.
///
/// @{

/**
 * Load a static radio configuration.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] config A pointer to a radio configuration.
 * @return Status code indicating success of the function call.
 *
 * The configuration passed into this function should be auto-generated
 * and not manually created or edited. By default, do not call this function
 * in RAIL 2.x and later unless instructed by Silicon Labs because it
 * may bypass updating certain RAIL state. In RAIL 2.x and later, the
 * RAIL_ConfigChannels function applies the default radio configuration
 * automatically.
 */
RAIL_Status_t RAIL_ConfigRadio(RAIL_Handle_t railHandle,
                               RAIL_RadioConfig_t config);

/**
 * Modify the currently configured fixed frame length in bytes.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] length The expected fixed frame length. A value of 0 is infinite.
 *   A value of RAIL_SETFIXEDLENGTH_INVALID restores the frame's length back to
 *   the length specified by the default frame type configuration.
 * @return Length configured; The new frame length configured into the hardware
 *   for use. 0 if in infinite mode, or RAIL_SETFIXEDLENGTH_INVALID if the frame
 *   length has not yet been overridden by a valid value.
 *
 * Sets the fixed-length configuration for transmit and receive.
 * Be careful when using this function in receive and transmit as this
 * function changes the default frame configuration and remains in force until
 * it is called again with an input value of RAIL_SETFIXEDLENGTH_INVALID. This
 * function will override any fixed or variable length settings from a radio
 * configuration.
 */
uint16_t RAIL_SetFixedLength(RAIL_Handle_t railHandle, uint16_t length);

/**
 * Configure the channels supported by this device.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] config A pointer to the channel configuration for your device.
 *   This pointer will be cached in the library so it must
 *   exist for the runtime of the application. Typically, this should be
 *   what is stored in Flash by the configuration tool.
 * @param[in] cb Function called whenever a radio configuration change occurs.
 * @return Returns the first available channel in the configuration.
 *
 * When configuring channels on EFR32, the radio tuner is reconfigured
 * based on the frequency and channel spacing in the channel configuration.
 *
 * @note config can be NULL to simply register or unregister the cb callback
 *   function when using RAIL internal protocol-specific radio configuration
 *   APIs for BLE, IEEE 802.15.4, or Z-Wave, which lack callback specification.
 *   In this use case, 0 is returned.
 */
uint16_t RAIL_ConfigChannels(RAIL_Handle_t railHandle,
                             const RAIL_ChannelConfig_t *config,
                             RAIL_RadioConfigChangedCallback_t cb);

/**
 * Get verbose listing of channel metadata for the current channel configuration.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[out] channelMetadata Allocated array that will be populated with
 * channel metadata.
 * @param[in,out] length Pointer to the length of the channelMetadata.
 *   This value will be updated to the number of channels written to the array.
 * @param[in] minChannel Minimum channel number about which to collect data.
 * @param[in] maxChannel Maximum channel number about which to collect data.
 * @return Status of the call. \ref RAIL_STATUS_INVALID_PARAMETER means that,
 *   based on the currently active radio configuration, there are more
 *   channels to write than there is space provided in the allocated
 *   channelMetadata. However, the channel metadata that was written is valid.
 *   \ref RAIL_STATUS_INVALID_STATE indicates that the channel configuration
 *   has not been configured. \ref RAIL_STATUS_NO_ERROR indicates complete
 *   success.
 */
RAIL_Status_t RAIL_GetChannelMetadata(RAIL_Handle_t railHandle,
                                      RAIL_ChannelMetadata_t *channelMetadata,
                                      uint16_t *length,
                                      uint16_t minChannel,
                                      uint16_t maxChannel);

/**
 * Check whether the channel exists in RAIL.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] channel A channel number to check.
 * @return Returns RAIL_STATUS_NO_ERROR if channel exists
 *
 * Returns RAIL_STATUS_INVALID_PARAMETER if the given channel does not exist
 * in the channel configuration currently used or RAIL_STATUS_NO_ERROR if the
 * channel is valid.
 */
RAIL_Status_t RAIL_IsValidChannel(RAIL_Handle_t railHandle,
                                  uint16_t channel);

/**
 * Cause radio settings associated with a particular channel to be applied to
 * hardware.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] channel The channel to prepare for use.
 * @return \ref RAIL_STATUS_NO_ERROR on success or
 *   \ref RAIL_STATUS_INVALID_PARAMETER if the given channel does not have an
 *   associated channel configuration entry or
 *   \ref RAIL_STATUS_INVALID_STATE if the specified railHandle is not the
 *   current RAIL handle in use. \ref RAIL_STATUS_INVALID_STATE is only used
 *   in the multiprotocol version of RAIL.
 *
 * This function walks the channelConfigEntry list and applies the configuration
 * associated with the specified channel. This function manually
 * changes channels without starting a TX or RX operation.
 *
 * When successful, the radio is idled.
 * When unsuccessful, the radio state will not be altered.
 */
RAIL_Status_t RAIL_PrepareChannel(RAIL_Handle_t railHandle, uint16_t channel);

/**
 * Return the current RAIL channel.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[out] channel The channel for which RAIL is currently configured.
 * @return RAIL_STATUS_NO_ERROR on success or
 *   RAIL_STATUS_INVALID_CALL if the radio is not configured for any channel or
 *   RAIL_STATUS_INVALID_PARAMETER if channel parameter is NULL.
 *
 * This function returns the channel most recently specified in API calls that
 * pass in a channel to tune to, namely \ref RAIL_PrepareChannel,
 * \ref RAIL_StartTx, \ref RAIL_StartScheduledTx, \ref RAIL_StartCcaCsmaTx,
 * \ref RAIL_StartCcaLbtTx, \ref RAIL_StartScheduledCcaCsmaTx,
 * \ref RAIL_StartScheduledCcaLbtTx, \ref RAIL_StartRx, \ref RAIL_ScheduleRx,
 * \ref RAIL_StartAverageRssi, \ref RAIL_StartTxStream, \ref RAIL_StartTxStreamAlt.
 * It doesn't follow changes RAIL performs implicitly during channel hopping and
 * mode switch.
 */
RAIL_Status_t RAIL_GetChannel(RAIL_Handle_t railHandle, uint16_t *channel);

/**
 * Return the current RAIL channel.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[out] channel The channel for which RAIL is currently configured.
 * @return RAIL_STATUS_NO_ERROR on success or
 *   RAIL_STATUS_INVALID_CALL if the radio is not configured for any channel or
 *   RAIL_STATUS_INVALID_PARAMETER if channel parameter is NULL.
 *
 * This function returns the channel the radio is currently tuned to if the
 * specified RAIL handle is active. It returns the channel it will be tuned to
 * during the next protocol switch if the handle is inactive.
 * The channel returned may be different than what \ref RAIL_GetChannel returns
 * when channel hopping or mode switch are involved.
 */
RAIL_Status_t RAIL_GetChannelAlt(RAIL_Handle_t railHandle, uint16_t *channel);

/**
 * Return the symbol rate for the current PHY.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return The symbol rate in symbols per second or 0.
 *
 * The symbol rate is the rate of symbol changes over the air. For non-DSSS
 * PHYs, this is the same as the baudrate. For DSSS PHYs, it is the baudrate
 * divided by the length of a chipping sequence. For more information,
 * see the modem calculator documentation. If the rate cannot be
 * calculated, this function returns 0.
 */
uint32_t RAIL_GetSymbolRate(RAIL_Handle_t railHandle);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
/**
 * Calculate the symbol rate for the current PHY.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return The symbol rate in symbols per second or 0.
 *
 * This function calculates the symbol rate when the radio configuration does
 * not include that information. In general, this function should be
 * implemented automatically in the radio configuration as a stub.
 */
uint32_t RAILCb_CalcSymbolRate(RAIL_Handle_t railHandle);
#endif

/**
 * Return the bit rate for the current PHY.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return The bit rate in bits per second or 0.
 *
 * The bit rate is the effective over-the-air data rate. It does not account
 * for extra spreading for forward error correction, and so on, but
 * accounts for modulation schemes, DSSS, and other configurations. For more
 * information, see the modem calculator documentation. If the rate cannot be
 * calculated, this function returns 0.
 */
uint32_t RAIL_GetBitRate(RAIL_Handle_t railHandle);

#ifndef DOXYGEN_SHOULD_SKIP_THIS
/**
 * Calculate the bit rate for the current PHY.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return The bit rate in bits per second or 0.
 *
 * This function calculates the bit rate when the radio configuration does
 * not include that information. In general, this function should be
 * implemented automatically in the radio configuration as a stub.
 */
uint32_t RAILCb_CalcBitRate(RAIL_Handle_t railHandle);
#endif
/**
 * Set the PA capacitor tune value for transmit and receive.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] txPaCtuneValue PA Ctune value for TX mode.
 * @param[in] rxPaCtuneValue PA Ctune value for RX mode.
 * @return Status code indicating success of the function call.
 *
 * Tunes the impedance of the transmit
 * and receive modes by changing the amount of capacitance at
 * the PA output.
 * Changes made to the TX Power configuration, e.g., calling \ref RAIL_ConfigTxPower,
 * will undo changes made to PA capacitor tune value for transmit and receive
 * via \ref RAIL_SetPaCTune.
 */
RAIL_Status_t RAIL_SetPaCTune(RAIL_Handle_t railHandle,
                              uint8_t txPaCtuneValue,
                              uint8_t rxPaCtuneValue);

/**
 * Get the sync words and their length.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[out] syncWordConfig An application-provided non-NULL pointer to store
 *                            \ref RAIL_SyncWordConfig_t sync word information.
 * @return Status code indicating success of the function call.
 **/
RAIL_Status_t RAIL_GetSyncWords(RAIL_Handle_t railHandle,
                                RAIL_SyncWordConfig_t *syncWordConfig);

/**
 * Set the selected sync words and their length.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] syncWordConfig A non-NULL pointer to \ref RAIL_SyncWordConfig_t
 *                           specifying the sync words and their length.
 * The desired length should be between 2 and 32 bits inclusive.
 * Other values will result in \ref RAIL_STATUS_INVALID_PARAMETER. The default
 * syncWord continues to be valid.
 * @return Status code indicating success of the function call.
 * When the custom sync word(s) applied by this API are no longer needed, or to
 * revert to default sync word, calling RAIL_ConfigChannels() will re-establish
 * the sync words specified in the radio configuration.
 *
 * This function will return \ref RAIL_STATUS_INVALID_STATE if called when BLE
 * has been enabled for this railHandle. When changing sync words in BLE mode,
 * use \ref RAIL_BLE_ConfigChannelRadioParams instead.
 **/
RAIL_Status_t RAIL_ConfigSyncWords(RAIL_Handle_t railHandle,
                                   const RAIL_SyncWordConfig_t *syncWordConfig);

/** @} */ // end of group Radio_Configuration

/******************************************************************************
 * Timing Information
 *****************************************************************************/
/// @addtogroup System_Timing System Timing
/// @brief Functionality related to the RAIL timer and general system time.
///
/// These functions can be used to get information about the current system time
/// or to manipulate the RAIL timer.
///
/// The system time returned by RAIL_GetTime() is in the same timebase that is
/// used throughout RAIL. Any callbacks or structures that provide a timestamp,
/// such as \ref RAIL_RxPacketDetails_t::timeReceived, will use the same timebase
/// as will any APIs that accept an absolute time for scheduling their action.
/// Throughout the documentation, the timebase is referred to as the RAIL
/// timebase. The timebase is currently a value in microseconds from \ref
/// RAIL_Init() time, which means that it will wrap every 1.19 hours.
/// (`(2^32 - 1) / (3600 sec/hr * 1000000 us/sec)`).
///
/// The provided timer is hardware-backed and interrupt-driven. It can be used
/// for timing any event in the system, but is especially helpful for
/// timing protocol-based state machines and other systems that interact with
/// the radio. To avoid processing the expiration in interrupt
/// context, leave the cb parameter passed to RAIL_SetTimer() as NULL and poll
/// for expiration with the RAIL_IsTimerExpired() function. See below for an
/// example of the interrupt driven method of interacting with the timer.
///
/// @code{.c}
/// void timerCb(RAIL_Handle_t cbArg)
/// {
///   // Timer callback action
/// }
///
/// void main(void)
/// {
///   // Initialize RAIL ...
///
///   // Set up a timer for 1 ms from now
///   RAIL_SetTimer(railHandle, 1000, RAIL_TIME_RELATIVE, &timerCb);
///
///   // Run main loop
///   while(1);
/// }
/// @endcode
///
/// If multiple software timers are needed to be run off of the one available
/// hardware timer, enable a software multiplexing layer within RAIL
/// using the \ref RAIL_ConfigMultiTimer() function. This will allow you to
/// set up as many timers as you want using the RAIL_*MultiTimer() functions. See
/// the example below for using the multitimer functionality.
///
/// @code{.c}
/// // Declare timer structures in global space or somewhere that will exist
/// // until the callback has fired
/// RAIL_MultiTimer_t tmr1, tmr2;
///
/// void timerCb(RAIL_MultiTimer_t *tmr,
///              RAIL_Time_t expectedTimeOfEvent,
///              void *cbArg)
/// {
///   if (tmr == tmr1) {
///     // Timer 1 action
///   } else {
///     // Timer 2 action
///   }
/// }
///
/// void main(void)
/// {
///   // Initialize RAIL ...
///
///   RAIL_ConfigMultiTimer(true);
///
///   // Set up one timer for 1 ms from now and one at time 2000000 in the RAIL
///   // timebase
///   RAIL_SetMultiTimer(&tmr1, 1000, RAIL_TIME_RELATIVE, &timerCb, NULL);
///   RAIL_SetMultiTimer(&tmr2, 2000000, RAIL_TIME_ABSOLUTE, &timerCb, NULL);
///
///   // Run main loop
///   while(1);
/// }
/// @endcode
///
/// @{

/**
 * Get the current RAIL time.
 *
 * @return Returns the RAIL timebase in microseconds. Note that this wraps
 *   after about 1.19 hours since it's stored in a 32 bit value.
 *
 * Returns the current time in the RAIL timebase (microseconds). It can be
 * used to compare with packet timestamps or to schedule transmits.
 */
RAIL_Time_t RAIL_GetTime(void);

/**
 * Set the current RAIL time.
 *
 * @warning Use this API only for testing purposes or in
 *   very limited circumstances during RAIL Timer Synchronization.
 *   Undefined behavior can result by calling it in multiprotocol or
 *   when the radio is not idle or timed events are active. Applications
 *   using \ref RAIL_GetTime() may not be designed for discontinuous
 *   changes to the RAIL time base.
 *
 * @param[in] time Set the RAIL timebase to this value in microseconds.
 * @return Status code indicating the success of the function call.
 *
 * Sets the current time in the RAIL timebase in microseconds.
 */
RAIL_Status_t RAIL_SetTime(RAIL_Time_t time);

/**
 * Blocking delay routine for a specified number of microseconds.
 *
 * @param[in] microseconds Delay duration in microseconds.
 * @return Status code indicating success of the function call.
 *
 * Use this RAIL API only for short blocking delays because it has less overhead
 * than calling RAIL_GetTime() in a loop.
 * @note
 * Passing large delay values may give unpredictable results or trigger
 * the Watchdog reset.
 * \n Also, this function will start the clocks required for the RAIL timebase if they
 * are not running, except between \ref RAIL_Sleep() and \ref RAIL_Wake()
 * where the timer must remain stopped.
 * \n Interrupts are not disabled during the delay, so the delay may be longer if an
 * interrupt extends beyond the delay duration.
 */
RAIL_Status_t RAIL_DelayUs(RAIL_Time_t microseconds);

/**
 * Schedule a timer to expire using the RAIL timebase.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] time The timer's expiration time in the RAIL timebase.
 * @param[in] mode Indicates whether the time argument is an absolute
 *   RAIL time or relative to the current RAIL time. Specifying mode
 *   \ref RAIL_TIME_DISABLED is the same as calling RAIL_CancelTimer().
 * @param[in] cb The callback for RAIL to call when the timer expires.
 * @return RAIL_STATUS_NO_ERROR on success and
 *   RAIL_STATUS_INVALID_PARAMETER if the timer can't be scheduled.
 *
 * Configures a timer to expire after a period in the RAIL timebase.
 * This timer can be used to implement low-level protocol features.
 *
 * @warning Attempting to schedule the timer when it is
 *   still running from a previous request is bad practice, unless the cb
 *   callback is identical to that used in the previous request, in which case
 *   the timer is rescheduled to the new time. Note that if the original timer
 *   expires as it is being rescheduled, the callback may or may not occur. It
 *   is generally good practice to cancel a running timer before rescheduling
 *   it to minimize ambiguity.
 */
RAIL_Status_t RAIL_SetTimer(RAIL_Handle_t railHandle,
                            RAIL_Time_t time,
                            RAIL_TimeMode_t mode,
                            RAIL_TimerCallback_t cb);

/**
 * Return the absolute time that the RAIL timer was configured to expire.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return The absolute time that this timer was set to expire.
 *
 * Provides the absolute time regardless of the \ref RAIL_TimeMode_t that
 * was passed into \ref RAIL_SetTimer. Note that the time might be in the
 * past if the timer has already expired. The return value is undefined if the
 * timer was never set.
 */
RAIL_Time_t RAIL_GetTimer(RAIL_Handle_t railHandle);

/**
 * Stop the currently scheduled RAIL timer.
 *
 * @param[in] railHandle A RAIL instance handle.
 * Cancels the timer. If this function is called before the timer expires,
 * the cb callback specified in the earlier RAIL_SetTimer() call will never
 * be called.
 */
void RAIL_CancelTimer(RAIL_Handle_t railHandle);

/**
 * Check whether the RAIL timer has expired.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return True if the previously scheduled timer has expired and false
 *   otherwise.
 *
 * Polling with this function is an alternative to the callback.
 */
bool RAIL_IsTimerExpired(RAIL_Handle_t railHandle);

/**
 * Check whether the RAIL timer is currently running.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return Returns true if the timer is running and false if
 *   the timer has expired or was never set.
 */
bool RAIL_IsTimerRunning(RAIL_Handle_t railHandle);

/**
 * Configure the RAIL software timer feature.
 *
 * @param[in] enable Enables/disables the RAIL multitimer.
 * @return True if the multitimer was successfully enabled/disabled, false
 *   otherwise.
 *
 * Turning this on will add a software timer layer above the physical RAIL timer
 * so that the user can have as many timers as desired. It is not necessary to
 * call this function if the MultiTimer APIs are not used.
 *
 * @note This function must be called before calling \ref RAIL_SetMultiTimer.
 *       This function is a no-op on multiprotocol as this layer is already used
 *       under the hood.
 *       Do not call this function while the RAIL timer is running.
 *       Call \ref RAIL_IsTimerRunning before enabling/disabling the multitimer.
 *       If the multitimer is not needed, do not call this function to
 *       allow the multitimer code to be dead stripped. If the multitimer is
 *       enabled for use, the multitimer and timer APIs can both be used.
 *       However, no timer can be in use while this function is being called.
 */
bool RAIL_ConfigMultiTimer(bool enable);

/**
 * Start a multitimer instance.
 *
 * @note
 *    It is legal to start an already running timer. If this is done, the timer
 *    will first be stopped before the new configuration is applied.
 *    If expirationTime is 0, the callback is called
 *    immediately.
 *
 * @param[in,out] tmr A pointer to the timer instance to start.
 * @param[in] expirationTime A time when the timer is set to expire.
 * @param[in] expirationMode Select mode of expirationTime. See \ref
 *            RAIL_TimeMode_t.
 * @param[in] callback A function to call on timer expiry. See \ref
 *            RAIL_MultiTimerCallback_t. NULL is a legal value.
 * @param[in] cbArg An extra callback function parameter for the user application.
 *
 * @return
 *    \ref RAIL_STATUS_NO_ERROR on success.@n
 *    \ref RAIL_STATUS_INVALID_PARAMETER if tmr has an illegal value or if
 *         timeout is in the past.
 */
RAIL_Status_t RAIL_SetMultiTimer(RAIL_MultiTimer_t *tmr,
                                 RAIL_Time_t expirationTime,
                                 RAIL_TimeMode_t expirationMode,
                                 RAIL_MultiTimerCallback_t callback,
                                 void *cbArg);

/**
 * Stop the currently scheduled RAIL multitimer.
 *
 * @param[in,out] tmr A RAIL timer instance handle.
 *
 * @return
 *    true if the timer was successfully canceled.
 *    false if the timer was not running.
 *
 * Cancels the timer. If this function is called before the timer expires,
 * the cb callback specified in the earlier RAIL_SetTimer() call will never
 * be called.
 */
bool RAIL_CancelMultiTimer(RAIL_MultiTimer_t *tmr);

/**
 * Check if a given timer is running.
 *
 * @param[in] tmr A pointer to the timer structure to query.
 *
 * @return
 *    true if the timer is running.
 *    false if the timer is not running.
 */
bool RAIL_IsMultiTimerRunning(RAIL_MultiTimer_t *tmr);

/**
 * Check if a given timer has expired.
 *
 * @param[in] tmr A pointer to the timer structure to query.
 *
 * @return
 *    true if the timer is expired.
 *    false if the timer is running.
 */
bool RAIL_IsMultiTimerExpired(RAIL_MultiTimer_t *tmr);

/**
 * Get time left before a given timer instance expires.
 *
 * @param[in] tmr A pointer to the timer structure to query.
 * @param[in] timeMode Indicates how the function provides the time
 *            remaining. By choosing \ref
 *            RAIL_TimeMode_t::RAIL_TIME_ABSOLUTE, the function returns the
 *            absolute expiration time, and by choosing \ref
 *            RAIL_TimeMode_t::RAIL_TIME_DELAY, the function returns the
 *            amount of time remaining before the timer's expiration.
 *
 * @return
 *    Time left expressed in RAIL's time units.
 *    0 if the soft timer is not running or has already expired.
 */
RAIL_Time_t RAIL_GetMultiTimer(RAIL_MultiTimer_t *tmr,
                               RAIL_TimeMode_t timeMode);

/** @} */ // end of group System_Timing

/******************************************************************************
 * Sleep APIs
 *****************************************************************************/
/// @addtogroup Sleep
/// @brief These APIs help when putting the system to an EM2/EM3/EM4 sleep
/// states where the high frequency clock is disabled.
/// @{
///
/// The RAIL library has its own timebase and the ability to schedule operations
/// into the future. When going to any power mode that disables the HF clock
/// used for the radio (EM2/EM3/EM4), it is important that this timebase is
/// synchronized to a running LFCLK and the chip is set to wake up before the
/// next scheduled event.
/// If RAIL has not been configured to use the power manager,
/// \ref RAIL_Sleep and \ref RAIL_Wake must be called for performing this
/// synchronization.
/// If RAIL has been configured to use the power manager,
/// \ref RAIL_InitPowerManager, it will automatically perform timer
/// synchronization based on the selected \ref RAIL_TimerSyncConfig_t. Calls to
/// \ref RAIL_Sleep and \ref RAIL_Wake are unsupported in such a scenario.
///
/// Following example code snippets demonstrate synchronizing the timebase
/// with and without timer synchronization:
///
/// <b>Sleep with timer synchronization:</b>
///
/// When sleeping with timer synchronization, you must first get the required
/// LFCLK up and running and leave it running across sleep so that the high
/// frequency clock that drives the RAIL time base can be synchronized to it.
/// The \ref RAIL_Sleep() API will also set up a wake event on the timer to wake
/// up wakeupTime before the next timer event so that it can run successfully.
/// See the \ref efr32_main sections on Low-Frequency Clocks and RAIL Timer
/// Synchronization for more setup details.
///
/// This is useful when maintaining packet timestamps
/// across sleep or use the scheduled RX/TX APIs while sleeping in between. It
/// does take more time and code to do the synchronization. If your
/// application does not need this, it should be avoided.
///
/// Example (without Power Manager):
/// @code{.c}
/// #include <rail.h>
/// #include <rail_types.h>
///
/// extern RAIL_Handle_t railHandle;
/// // Wakeup time for your crystal/board/chip combination
/// extern uint32_t wakeupTime;
///
/// void main(void) {
///   RAIL_Status_t status;
///   bool shouldSleep = false;
///
///   // This function depends on your board/chip but it must enable the LFCLK
///   // you intend to use for RTCC sync before we configure sleep as that function
///   // will attempt to auto detect the clock.
///   BoardSetupLFCLK()
///
///   // Configure sleep for timer synchronization
///   status = RAIL_ConfigSleep(railHandle, RAIL_SLEEP_CONFIG_TIMERSYNC_ENABLED);
///   assert(status == RAIL_STATUS_NO_ERROR);
///
///   // Application main loop
///   while(1) {
///     // ... do normal app stuff and set shouldSleep to true when we want to
///     // sleep
///     if (shouldSleep) {
///       bool sleepAllowed = false;
///
///       // Go critical to assess sleep decisions
///       CORE_ENTER_CRITICAL();
///       if (RAIL_Sleep(wakeupTime, &sleepAllowed) != RAIL_STATUS_NO_ERROR) {
///         printf("Error trying to go to sleep!");
///         CORE_EXIT_CRITICAL();
///         continue;
///       }
///       if (sleepAllowed) {
///         // Go to sleep
///       }
///       // Wakeup and sync the RAIL timebase back up
///       RAIL_Wake(0);
///       CORE_EXIT_CRITICAL();
///     }
///   }
/// }
/// @endcode
///
/// Example (with Power Manager):
/// @code{.c}
/// #include <rail.h>
/// #include <rail_types.h>
/// #include <sl_power_manager.h>
///
/// extern RAIL_Handle_t railHandle;
///
/// void main(void) {
///   RAIL_Status_t status;
///   bool shouldSleep = false;
///
///   // This function depends on your board/chip but it must enable the LFCLK
///   // you intend to use for RTCC sync before we configure sleep as that function
///   // will attempt to auto detect the clock.
///   BoardSetupLFCLK();
///   // Configure sleep for timer synchronization
///   status = RAIL_ConfigSleep(railHandle, RAIL_SLEEP_CONFIG_TIMERSYNC_ENABLED);
///   assert(status == RAIL_STATUS_NO_ERROR);
///   // Initialize application-level power manager service
///   sl_power_manager_init();
///   // Initialize RAIL library's use of the power manager
///   RAIL_InitPowerManager();
///
///   // Application main loop
///   while(1) {
///     // ... do normal app stuff and set shouldSleep to true when we want to
///     // sleep
///     if (shouldSleep) {
///       // Let the CPU go to sleep if the system allows it.
///       sl_power_manager_sleep();
///     }
///   }
/// }
/// @endcode
///
/// RAIL APIs such as, \ref RAIL_StartScheduledTx, \ref RAIL_ScheduleRx,
/// \ref RAIL_SetTimer, \ref RAIL_SetMultiTimer can be used to schedule periodic
/// wakeups to perform a scheduled operation. The call to
/// sl_power_manager_sleep() in the main loop ensures that the device sleeps
/// until the scheduled operation is due.
/// Upon completion, each instantaneous or scheduled RX/TX operation will
/// indicate radio busy to the power manager to allow the application to
/// service the RAIL event and perform subsequent operations before going to
/// sleep. Therefore, it is important that the application idle the radio by either
/// calling \ref RAIL_Idle or \ref RAIL_YieldRadio.
/// If the radio transitions to RX after an RX or TX operation,
/// always call \ref RAIL_Idle in order transition to a lower sleep state.
/// If the radio transitions to idle after an RX or TX operation,
/// \ref RAIL_YieldRadio should suffice in indicating to the power manager
/// that the radio is no longer busy and the device can sleep.
///
/// The following example shows scheduling periodic TX on getting a TX completion
/// event:
/// @code{.c}
/// void RAILCb_Event(RAIL_Handle_t railHandle, RAIL_Events_t events) {
///   // Omitting other event handlers
///   if (events & RAIL_EVENTS_TX_COMPLETION) {
///     // Schedule the next TX.
///     RAIL_ScheduleTxConfig_t config = {
///       .when = (RAIL_Time_t)parameters->startTime,
///       .mode = (RAIL_TimeMode_t)parameters->startTimeMode
///     };
///     (void)RAIL_StartScheduledTx(radio.handle, channel, 0, &config, NULL);
///   }
/// }
/// @endcode
///
/// @note The above code assumes that RAIL automatic state transitions after TX
///       are idle. Set \ref RAIL_SetTxTransitions to ensure the right state
///       transitions. Radio must be idle for the device to enter EM2 or lower
///       energy mode.
///
/// @note When using the power manager, usage of \ref RAIL_YieldRadio in
///       single protocol RAIL is similar to its usage in multiprotocol RAIL.
///       See \ref rail_radio_scheduler_yield for more details.
///
/// @note Back to back scheduled operations do not require an explicit call to
///       \ref RAIL_YieldRadio if the radio transitions to idle.
///
/// <b>Sleep without timer synchronization:</b>
///
/// When sleeping without timer synchronization, you are free to enable only the
/// LFCLKs and wake sources required by the application. RAIL will not attempt
/// to configure any wake events and may miss anything that occurs over sleep.
///
/// This is useful when your application does not care about
/// packet timestamps or scheduling operations accurately over sleep.
///
/// Example (without Power Manager):
/// @code{.c}
/// #include <rail.h>
/// #include <rail_types.h>
///
/// extern RAIL_Handle_t railHandle;
///
/// void main(void) {
///   RAIL_Status_t status;
///   bool shouldSleep = false;
///
///   // Configure sleep for timer synchronization
///   status = RAIL_ConfigSleep(railHandle, RAIL_SLEEP_CONFIG_TIMERSYNC_DISABLED);
///   assert(status == RAIL_STATUS_NO_ERROR);
///
///   // Application main loop
///   while(1) {
///     // ... do normal app stuff and set shouldSleep to true when we want to
///     // sleep
///     if (shouldSleep) {
///       bool sleepAllowed = false;
///       uint32_t sleepTime = 0;
///
///       // Go critical to assess sleep decisions
///       CORE_ENTER_CRITICAL();
///       if (RAIL_Sleep(0, &sleepAllowed) != RAIL_STATUS_NO_ERROR) {
///         printf("Error trying to go to sleep!");
///         CORE_EXIT_CRITICAL();
///         continue;
///       }
///       if (sleepAllowed) {
///         // Go to sleep and optionally update sleepTime to the correct value
///         // in microseconds
///       }
///       // Wakeup and sync the RAIL timebase back up
///       RAIL_Wake(sleepTime);
///       CORE_EXIT_CRITICAL();
///     }
///   }
/// }
/// @endcode
///
/// Example (with Power Manager):
/// @code{.c}
/// #include <rail.h>
/// #include <rail_types.h>
/// #include <sl_power_manager.h>
///
/// extern RAIL_Handle_t railHandle;
///
/// void main(void) {
///   RAIL_Status_t status;
///   bool shouldSleep = false;
///
///   // This function depends on your board/chip but it must enable the LFCLK
///   // you intend to use for RTCC sync before we configure sleep as that function
///   // will attempt to auto detect the clock.
///   BoardSetupLFCLK();
///   // Configure sleep for timer synchronization
///   status = RAIL_ConfigSleep(railHandle, RAIL_SLEEP_CONFIG_TIMERSYNC_DISABLED);
///   assert(status == RAIL_STATUS_NO_ERROR);
///   // Initialize application-level power manager service
///   sl_power_manager_init();
///   // Initialize RAIL library's use of the power manager
///   RAIL_InitPowerManager();
///
///   // Application main loop
///   while(1) {
///     // ... do normal app stuff and set shouldSleep to true when we want to
///     // sleep
///     if (shouldSleep) {
///       // Let the CPU go to sleep if the system allows it.
///       sl_power_manager_sleep();
///     }
///   }
/// }
/// @endcode
/**
 * Configure RAIL timer synchronization. This function is optional to implement.
 *
 * @param[in,out] timerSyncConfig A pointer to the \ref RAIL_TimerSyncConfig_t
 *   structure containing the configuration parameters for timer sync. The
 *   \ref RAIL_TimerSyncConfig_t::sleep field is ignored in this call.
 *
 * This function is called during \ref RAIL_ConfigSleep to allow an application
 * to configure the PRS and RTCC channels used for timer sync to values other
 * than their defaults. The default channels are populated in timerSyncConfig and
 * can be overwritten by the application. If this function is not implemented by the
 * application, a default implementation from within the RAIL library will be used
 * that simply maintains the default channel values in timerSyncConfig.
 *
 * If an unsupported channel is selected by the application, \ref RAIL_ConfigSleep
 * will return \ref RAIL_STATUS_INVALID_PARAMETER.
 *
 * @code{.c}
 * void RAILCb_ConfigSleepTimerSync(RAIL_TimerSyncConfig_t *timerSyncConfig)
 * {
 *   timerSyncConfig->prsChannel = MY_TIMERSYNC_PRS_CHANNEL;
 *   timerSyncConfig->rtccChannel = MY_TIMERSYNC_RTCC_CHANNEL;
 * }
 * @endcode
 */
void RAILCb_ConfigSleepTimerSync(RAIL_TimerSyncConfig_t *timerSyncConfig);

/**
 * Initialize RAIL timer synchronization.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] sleepConfig A sleep configuration.
 *
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_ConfigSleep(RAIL_Handle_t railHandle,
                               RAIL_SleepConfig_t sleepConfig);

/**
 * Initialize RAIL timer synchronization.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] syncConfig A pointer to the timer synchronization configuration.
 *
 * The default structure used to enable timer synchronization across sleep is
 * \ref RAIL_TIMER_SYNC_DEFAULT.
 *
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_ConfigSleepAlt(RAIL_Handle_t railHandle,
                                  RAIL_TimerSyncConfig_t *syncConfig);

/**
 * Stop the RAIL timer and prepare RAIL for sleep.
 *
 * @param[in] wakeupProcessTime Time in microseconds that the application and
 *                              hardware need to recover from sleep state.
 * @param[out] deepSleepAllowed
 *   true - system can go to deep sleep.
 *   false - system should not go to deep sleep. Deep sleep should be blocked
 *           in this case.
 *
 * @return Status code indicating success of the function call.
 *
 * @warning The active RAIL configuration must be idle to enable sleep.
 *
 * @note This API must not be called if RAIL Power Manager is initialized.
 */
RAIL_Status_t RAIL_Sleep(uint16_t wakeupProcessTime, bool *deepSleepAllowed);

/**
 * Wake RAIL from sleep and restart the RAIL timer.
 *
 * @param[in] elapsedTime Add the sleep duration to the RAIL timer
 *   before restarting the RAIL timer.
 *
 * @return Status code indicating success of the function call.
 *
 * If the timer sync was enabled by \ref RAIL_ConfigSleep, synchronize the RAIL
 * timer using an alternate timer. Otherwise, add elapsedTime to the RAIL
 * timer.
 *
 * @note This API must not be called if RAIL Power Manager is initialized.
 */
RAIL_Status_t RAIL_Wake(RAIL_Time_t elapsedTime);

/**
 * Initialize RAIL Power Manager.
 *
 * @return Status code indicating success of the function call.
 *
 * @note Call this function only when the application is built
 * and initialized with Power Manager plugin.
 * RAIL will perform timer synchronization, upon transitioning from EM2 or lower
 * to EM1 or higher energy mode or vice-versa, in the Power Manager EM
 * transition callback. Since EM transition callbacks are not called in a
 * deterministic order, it is suggested to not call any RAIL time dependent APIs
 * in an EM transition callback.
 */
RAIL_Status_t RAIL_InitPowerManager(void);

/**
 * Stop the RAIL Power Manager.
 *
 * @return Status code indicating success of the function call.
 *
 * @note The active RAIL configuration must be idle to disable radio
 * power manager and there should be no outstanding requirements by
 * radio power manager.
 */
RAIL_Status_t RAIL_DeinitPowerManager(void);

/** @} */ // end of group Sleep

/******************************************************************************
 * Events
 *****************************************************************************/
/**
 * @addtogroup Events
 * @brief APIs related to events
 * @{
 */

/**
 * Configure radio events.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] mask A bitmask of events to configure.
 * @param[in] events A bitmask of events to trigger \ref RAIL_Config_t::eventsCallback
 *   For a full list of available callbacks, see
 *   RAIL_EVENT_* set of defines.
 * @return Status code indicating success of the function call.
 *
 * Sets up which radio interrupts generate a RAIL event. The full list of
 * options is in \ref RAIL_Events_t.
 */
RAIL_Status_t RAIL_ConfigEvents(RAIL_Handle_t railHandle,
                                RAIL_Events_t mask,
                                RAIL_Events_t events);

/** @} */ // end of group Events

/******************************************************************************
 * Data Management
 *****************************************************************************/
/// @addtogroup Data_Management Data Management
/// @brief Data management functions
///
/// These functions allow the application to choose how data is presented to
/// the application. RAIL provides data in a packet-based method or in a
/// FIFO-based method. As originally conceived,
/// \ref RAIL_DataMethod_t::PACKET_MODE was designed for handling packets
/// that fit within RAIL's FIFOs while \ref RAIL_DataMethod_t::FIFO_MODE
/// was designed for handling packets larger than RAIL's FIFOs could hold.
/// Conceptually it is still useful to think of these modes this way, but
/// functionally their distinction has become blurred by improvements in
/// RAIL's flexibility -- applications now have much more control over both
/// receive and transmit FIFO sizes, and the FIFO-management and threshold
/// APIs and related events are no longer restricted to \ref
/// RAIL_DataMethod_t::FIFO_MODE operation but can be used in \ref
/// RAIL_DataMethod_t::PACKET_MODE too.
///
/// The application can configure RAIL data management through
/// RAIL_ConfigData(). This function allows the application to specify the type
/// of radio data (\ref RAIL_TxDataSource_t and \ref RAIL_RxDataSource_t) and
/// the method of interacting with data (\ref RAIL_DataMethod_t). By default,
/// RAIL configures TX and RX both with packet data source and \ref
/// RAIL_DataMethod_t::PACKET_MODE.
///
/// For transmit, \ref RAIL_DataMethod_t::PACKET_MODE and \ref
/// RAIL_DataMethod_t::FIFO_MODE are functionally the same:
///   - When not actively transmitting, load a packet's initial transmit
///     data using RAIL_WriteTxFifo() with reset set to true. Alternatively
///     this data copying can be avoided by changing the transmit FIFO to an
///     already-loaded section of memory with \ref RAIL_SetTxFifo().
///   - When actively transmitting, load remaining transmit data with
///     RAIL_WriteTxFifo() with reset set to false.
///   - If transmit packets exceed the FIFO size, set the transmit FIFO
///     threshold through RAIL_SetTxFifoThreshold(). The \ref
///     RAIL_Config_t::eventsCallback with \ref RAIL_EVENT_TX_FIFO_ALMOST_EMPTY
///     will occur telling the application to load more TX packet data, if
///     needed, to prevent a \ref RAIL_EVENT_TX_UNDERFLOW event from occurring.
///     One can get how much space is available in the transmit FIFO for more
///     transmit data through RAIL_GetTxFifoSpaceAvailable().
///   - After transmit completes, the transmit FIFO can be manually reset
///     with RAIL_ResetFifo(), but this should rarely be necessary.
///
/// The transmit FIFO is specified by the application and its size is
/// the value returned from the most recent call to RAIL_SetTxFifo().
/// The transmit FIFO is edge-based in that it only provides the \ref
/// RAIL_EVENT_TX_FIFO_ALMOST_EMPTY event once when the threshold is crossed
/// in the emptying direction.
///
/// For receive, the distinction between \ref RAIL_DataMethod_t::PACKET_MODE
/// and \ref RAIL_DataMethod_t::FIFO_MODE basically boils down to how
/// unsuccessfully-received packets are handled. In \ref
/// RAIL_DataMethod_t::PACKET_MODE, data from such packets is automatically
/// rolled back as if the packet was never received, while in \ref
/// RAIL_DataMethod_t::FIFO_MODE, rollback does not occur putting more onus
/// on the application to deal with that data.
///
/// In receive \ref RAIL_DataMethod_t::PACKET_MODE data management:
///   - Packet lengths are determined from the Radio Configurator configuration
///     and can be read out at the end using \ref RAIL_GetRxPacketInfo().
///   - Received packet data is made available on successful packet completion
///     via \ref RAIL_Config_t::eventsCallback with \ref
///     RAIL_EVENT_RX_PACKET_RECEIVED which can then use RAIL_GetRxPacketInfo()
///     and RAIL_GetRxPacketDetailsAlt() to access packet information and
///     RAIL_PeekRxPacket() to access packet data.
///   - Filtered, Aborted, or FrameError received packet data is automatically
///     rolled back (dropped) without the application needing to worry about
///     consuming it.
///     The application can choose to not even be bothered with the events
///     related to such packets: \ref RAIL_EVENT_RX_ADDRESS_FILTERED,
///     \ref RAIL_EVENT_RX_PACKET_ABORTED, or \ref RAIL_EVENT_RX_FRAME_ERROR.
///
/// In receive \ref RAIL_DataMethod_t::FIFO_MODE data management:
///   - Packet Lengths are determined from the Radio Configurator configuration
///     or by application knowledge of packet payload structure.
///   - Received data can be retrieved prior to packet completion through
///     RAIL_ReadRxFifo() and is never rolled back on Filtered, Aborted, or
///     FrameError packets. The application should enable and handle these
///     events so it can flush any packet data it's already retrieved.
///   - After packet completion, remaining packet data for Filtered, Aborted,
///     or FrameError packets remains in the FIFO and the appropriate event is
///     triggered to the user. This data may be consumed in the callback unlike
///     in packet mode where it is automatically rolled back. At the end of the
///     callback all remaining data in the FIFO will be cleaned up as usual.
///     Keep in mind that RAIL_GetRxPacketDetailsAlt() provides packet detailed
///     information only for successfully received packets.
///
/// Common receive data management features:
///   - Set the receive FIFO threshold through RAIL_SetRxFifoThreshold(). The
///     \ref RAIL_Config_t::eventsCallback with \ref RAIL_EVENT_RX_FIFO_ALMOST_FULL
///     will occur telling the application to consume some RX packet data to
///     prevent a \ref RAIL_EVENT_RX_FIFO_OVERFLOW event from occurring.
///   - Get receive FIFO count information through
///     RAIL_GetRxPacketInfo(\ref RAIL_RX_PACKET_HANDLE_NEWEST)
///     (or RAIL_GetRxFifoBytesAvailable()).
///   - After receive completes and all its data has been consumed, the receive
///     FIFO can be manually reset with RAIL_ResetFifo(), though this should
///     rarely be necessary and should only be done with the radio idle.
///
/// When trying to determine an appropriate threshold, the application needs
/// to know the size of each FIFO. The default receive FIFO is internal to RAIL
/// with a size of 512 bytes. This can be changed, however, using
/// \ref RAIL_SetRxFifo() and the default may be removed entirely by calling
/// this from the RAILCb_SetupRxFifo() callback. The receive FIFO event is
/// level-based in that the \ref RAIL_EVENT_RX_FIFO_ALMOST_FULL event will
/// constantly pend if the threshold is exceeded. This normally means that
/// inside this event's callback, the application should empty enough of the FIFO
/// to go under the threshold. To defer reading the FIFO to main context, the
/// application can disable or re-enable the receive FIFO threshold event using
/// RAIL_ConfigEvents() with the mask \ref RAIL_EVENT_RX_FIFO_ALMOST_FULL.
///
/// The receive FIFO can store multiple packets and processing of a packet can
/// be deferred from the RAIL event callback to main-loop processing
/// by using RAIL_HoldRxPacket() in the event callback and
/// RAIL_ReleaseRxPacket() in the main-loop.
/// On some platforms, the receive FIFO is supplemented by an internal
/// fixed-size packet metadata FIFO that limits the number of packets
/// RAIL and applications can hold onto for deferred processing.
/// See chip-specific documentation, such as \ref efr32_main, for more
/// information. Note that when using multiprotocol the receive FIFO is reset
/// prior to a protocol switch so held packets will be lost if not processed
/// before then.
///
/// While \ref RAIL_EVENT_RX_FIFO_ALMOST_FULL occurs solely based on the
/// state of the receive FIFO used for packet data, both
/// \ref RAIL_EVENT_RX_FIFO_FULL and \ref RAIL_EVENT_RX_FIFO_OVERFLOW
/// can occur coincident with packet completion when either that or the
/// internal packet metadata FIFO fills or overflows.
/// \ref RAIL_EVENT_RX_FIFO_FULL informs the application it should
/// immediately process and free up the oldest packets/data to make room
/// for new packets/data, reducing the possibility of packet/data loss
/// and \ref RAIL_EVENT_RX_FIFO_OVERFLOW.
///
/// Before a packet is fully received you can always use
/// RAIL_PeekRxPacket() to look at the contents. In FIFO mode, you may also
/// consume its data with \ref RAIL_ReadRxFifo(). Remember that none of these
/// APIs will read across a packet boundary (even in FIFO mode) so you will
/// need to handle each received packet individually.
///
/// While RAIL defaults to \ref RAIL_DataMethod_t::PACKET_MODE, the
/// application can explicitly initialize RAIL for \ref
/// RAIL_DataMethod_t::PACKET_MODE in the following manner:
/// @code{.c}
/// extern RAIL_Handle_t railHandle;
/// static const RAIL_DataConfig_t railDataConfig = {
///   .txSource = TX_PACKET_DATA,
///   .rxSource = RX_PACKET_DATA,
///   .txMethod = PACKET_MODE,
///   .rxMethod = PACKET_MODE,
/// };
///
/// status = RAIL_ConfigData(railHandle, &railDataConfig);
///
/// // Events that can occur in Packet Mode:
/// //    RAIL_EVENT_TX_PACKET_SENT
/// //    RAIL_EVENT_RX_PACKET_RECEIVED
/// // and optionally (packet data automatically dropped):
/// //    RAIL_EVENT_RX_ADDRESS_FILTERED
/// //    RAIL_EVENT_RX_PACKET_ABORTED
/// //    RAIL_EVENT_RX_FRAME_ERROR
/// // and if enabled:
/// //   RAIL_EVENT_TX_UNDERFLOW
/// //   RAIL_EVENT_TXACK_UNDERFLOW
/// //   RAIL_EVENT_TX_FIFO_ALMOST_EMPTY
/// //   RAIL_EVENT_RX_FIFO_ALMOST_FULL
/// @endcode
///
/// Initializing RAIL for \ref RAIL_DataMethod_t::FIFO_MODE requires a few
/// more function calls:
/// @code{.c}
/// extern RAIL_Handle_t railHandle;
/// static const RAIL_DataConfig_t railDataConfig = {
///   .txSource = TX_PACKET_DATA,
///   .rxSource = RX_PACKET_DATA,
///   .txMethod = FIFO_MODE,
///   .rxMethod = FIFO_MODE,
/// };
///
/// status = RAIL_ConfigData(railHandle, &railDataConfig);
///
/// // Gets the size of the FIFOs.
/// // Assume that the transmit and receive FIFOs are the same size
/// uint16_t fifoSize = RAIL_GetTxFifoSpaceAvailable(railHandle);
///
/// // Sets the transmit and receive FIFO thresholds.
/// // For this example, set the threshold in the middle of each FIFO.
/// RAIL_SetRxFifoThreshold(railHandle, fifoSize / 2);
/// RAIL_SetTxFifoThreshold(railHandle, fifoSize / 2);
///
/// // Events that can occur in FIFO mode:
/// //   RAIL_EVENT_TX_FIFO_ALMOST_EMPTY
/// //   RAIL_EVENT_TX_UNDERFLOW
/// //   RAIL_EVENT_TXACK_UNDERFLOW
/// //   RAIL_EVENT_TX_PACKET_SENT
/// //   RAIL_EVENT_RX_FIFO_ALMOST_FULL
/// //   RAIL_EVENT_RX_FIFO_OVERFLOW
/// //   RAIL_EVENT_RX_ADDRESS_FILTERED
/// //   RAIL_EVENT_RX_PACKET_ABORTED
/// //   RAIL_EVENT_RX_FRAME_ERROR
/// //   RAIL_EVENT_RX_PACKET_RECEIVED
/// @endcode
///
/// On receive, an application can use a different \ref RAIL_RxDataSource_t that
/// is only compatible with \ref RAIL_DataMethod_t::FIFO_MODE. All that differs
/// from the FIFO mode example above is the RAIL_DataConfig_t::rxSource setting.
/// IQ data samples are taken at the hardware's oversample rate and the amount
/// of data can easily overwhelm the CPU processing time. The sample rate
/// depends on the chosen PHY, as determined by the data rate and the decimation
/// chain. It is <b>not</b> recommended to use the IQ data source with sample
/// rates above 300 k samples/second because the CPU might not be able to keep up
/// with the data stream. Depending on the application and the needed CPU
/// bandwidth, slower data rates may be required. On EFR32xG22 and later
/// platforms, it is recommended to reset the RX buffer before initiating a
/// receive for all modes except \ref RAIL_RxDataSource_t::RX_PACKET_DATA since
/// the RX buffer has to be 32-bit aligned. If the buffer is <b>not</b> reset
/// but is 32-bit aligned, capture is performed on the remaining space available.
/// If the buffer is <b>not</b> reset and is <b>not</b> 32-bit aligned, then
/// RAIL_ConfigData() returns \ref RAIL_STATUS_INVALID_STATE.
/// @code{.c}
/// // Reset RX buffer (EFR32xG22 and later platforms)
/// RAIL_ResetFifo(railHandle, false, true);
///
/// // IQ data is provided into the receive FIFO.
/// static const RAIL_DataConfig_t railDataConfig = {
///   .txSource = TX_PACKET_DATA,
///   .rxSource = RX_IQDATA_FILTLSB,
///   .txMethod = FIFO_MODE,
///   .rxMethod = FIFO_MODE,
/// };
///
/// // IQ data comes in the following format when reading out of the FIFO:
/// //------------------------------------
/// // I[LSB] | I[MSB] | Q[LSB] | Q[MSB] |
/// //------------------------------------
/// @endcode
///
/// @{

/**
 * RAIL data management configuration
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] dataConfig RAIL data configuration structure.
 * @return Status code indicating success of the function call.
 *
 * This function configures how RAIL manages data. The application can
 * configure RAIL to receive data in a packet-based or FIFO-based manner.
 * \ref RAIL_DataMethod_t::FIFO_MODE is necessary to receive packets larger
 * than the radio's receive FIFO. It is also required for receive data
 * sources other than \ref RAIL_RxDataSource_t::RX_PACKET_DATA.
 *
 * Generally with \ref RAIL_DataMethod_t::FIFO_MODE, the application sets
 * appropriate FIFO thresholds via RAIL_SetTxFifoThreshold() and
 * RAIL_SetRxFifoThreshold() and then enables and handles the
 * \ref RAIL_EVENT_TX_FIFO_ALMOST_EMPTY event callback (to feed more packet
 * data via RAIL_WriteTxFifo() before the FIFO underflows) and the \ref
 * RAIL_EVENT_RX_FIFO_ALMOST_FULL event callback (to consume packet data
 * via RAIL_ReadRxFifo() before the receive FIFO overflows).
 *
 * When configuring TX for \ref RAIL_DataMethod_t::FIFO_MODE, this
 * function resets the transmit FIFO. When configuring TX or RX for
 * \ref RAIL_DataMethod_t::PACKET_MODE, this function will reset
 * the corresponding FIFO thresholds such that they won't trigger the
 * \ref RAIL_EVENT_RX_FIFO_ALMOST_FULL or \ref RAIL_EVENT_TX_FIFO_ALMOST_EMPTY
 * events.
 *
 * When \ref RAIL_DataConfig_t::rxMethod is set to \ref
 * RAIL_DataMethod_t::FIFO_MODE, the radio won't drop packet data of
 * aborted or CRC error packets, but will present it to the application
 * to deal with accordingly. On completion of erroneous packets, the
 * \ref RAIL_Config_t::eventsCallback with \ref RAIL_EVENT_RX_PACKET_ABORTED,
 * \ref RAIL_EVENT_RX_FRAME_ERROR, or \ref RAIL_EVENT_RX_ADDRESS_FILTERED will
 * tell the application it can drop any data it read via RAIL_ReadRxFifo() during reception.
 * For CRC error packets when the \ref RAIL_RX_OPTION_IGNORE_CRC_ERRORS
 * RX option is in effect, the application should check for that from the
 * \ref RAIL_RxPacketStatus_t obtained by calling RAIL_GetRxPacketInfo().
 * RAIL will automatically flush any remaining packet data after reporting
 * one of these packet completion events or the application can explicitly
 * flush it by calling RAIL_ReleaseRxPacket().
 *
 * When \ref RAIL_DataConfig_t::rxMethod is set to \ref
 * RAIL_DataMethod_t::PACKET_MODE, the radio will roll back (drop) all packet
 * data associated with aborted packets including those with CRC errors
 * (unless configured to ignore CRC errors via the
 * \ref RAIL_RX_OPTION_IGNORE_CRC_ERRORS RX option). The application will
 * never have to deal with packet data from these packets.
 * In either mode, the application can set RX options as needed.
 *
 * When \ref RAIL_DataConfig_t::rxSource is set to a value other than
 * \ref RX_PACKET_DATA and \ref RAIL_Config_t::eventsCallback
 * \ref RAIL_EVENT_RX_FIFO_OVERFLOW is enabled RX will be terminated
 * if a RX FIFO overflow occurs.  If \ref RAIL_EVENT_RX_FIFO_OVERFLOW
 * is not enabled, data will be discarded until the overflow condition
 * is resolved.  To continue capturing data RX must be restarted using
 * \ref RAIL_StartRx().
 *
 */
RAIL_Status_t RAIL_ConfigData(RAIL_Handle_t railHandle,
                              const RAIL_DataConfig_t *dataConfig);

/**
 * Write data to the transmit FIFO previously established by RAIL_SetTxFifo().
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] dataPtr An application-provided pointer to transmit data.
 * @param[in] writeLength A number of bytes to write to the transmit FIFO.
 * @param[in] reset If true, resets transmit FIFO before writing the data.
 * @return The number of bytes written to the transmit FIFO.
 *
 * This function reads data from the provided dataPtr and writes it to the transmit
 * FIFO that was previously established by RAIL_SetTxFifo().
 * If the requested writeLength exceeds the current number of bytes open
 * in the transmit FIFO, the function only writes until the transmit FIFO
 * is full. The function returns the number of bytes written to the transmit
 * FIFO or returns zero if railHandle is NULL or if the transmit FIFO is full.
 *
 * @note The protocol's packet configuration, as set up by the radio
 *   configurator or via RAIL_SetFixedLength(), determines how many
 *   bytes of data are consumed from the transmit FIFO for a successful transmit
 *   operation, not the writeLength value passed in. If not enough data has
 *   been put into the transmit FIFO, a \ref RAIL_EVENT_TX_UNDERFLOW event will
 *   occur. If too much data is put into the transmit FIFO, the extra data will
 *   either become the first bytes
 *   sent in a subsequent packet, or will be thrown away if the FIFO gets
 *   reset prior to the next transmit. In general, the proper number of
 *   packet bytes to put into the transmit FIFO are all payload bytes except
 *   for any CRC bytes, which the packet configuration causes to be sent
 *   automatically.
 *
 * @note This function does not create a critical section but, depending on the
 *   application, a critical section could be appropriate.
 */
uint16_t RAIL_WriteTxFifo(RAIL_Handle_t railHandle,
                          const uint8_t *dataPtr,
                          uint16_t writeLength,
                          bool reset);

/**
 * Set the address of the transmit FIFO, a circular buffer used for TX data.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in,out] addr A pointer to a read-write memory location in RAM
 *   used as the transmit FIFO. This memory must persist until the next call to
 *   this function.
 * @param[in] initLength A number of initial bytes already in the transmit FIFO.
 * @param[in] size A desired size of the transmit FIFO in bytes.
 * @return Returns the FIFO size in bytes, 0 if an error occurs.
 *
 * This function sets the memory location for the transmit FIFO. It
 * must be called at least once before any transmit operations occur.
 *
 * FIFO size can be determined by the return value of this function. The
 * chosen size is determined based on the available FIFO sizes supported by the
 * hardware. Similarly, some hardware has stricter FIFO alignment requirements;
 * 32-bit alignment provides the maximum portability across all RAIL platforms.
 * For more on supported FIFO sizes and alignments, see chip-specific
 * documentation, such as \ref efr32_main. The returned FIFO size will be the
 * closest allowed size less than or equal to the passed in size parameter,
 * unless the size parameter is smaller than the minimum FIFO size. If the
 * initLength parameter is larger than the returned size, the FIFO will be
 * filled up to its size.
 *
 * A user may write to the custom memory location directly before calling this
 * function, or use \ref RAIL_WriteTxFifo to write to the memory location after
 * calling this function. Users must specify the initLength for
 * previously-written memory to be set in the transmit FIFO.
 *
 * This function reserves the block of RAM starting at addr with a length of the
 * returned FIFO size, which is used internally as a circular buffer for the
 * transmit FIFO. It must be able to hold the entire FIFO size. The caller must
 * guarantee that the custom FIFO remains intact and unchanged (except via calls
 * to \ref RAIL_WriteTxFifo) until the next call to this function.
 *
 * @note The protocol's packet configuration, as set up by the radio
 *   configurator or via RAIL_SetFixedLength(), determines how many
 *   bytes of data are consumed from the transmit FIFO for a successful transmit
 *   operation, not the initLength value passed in. If not enough data has
 *   been put into the transmit FIFO, a \ref RAIL_EVENT_TX_UNDERFLOW event will
 *   occur. If too much data is put into the transmit FIFO, the extra data
 *   will either become the first bytes
 *   sent in a subsequent packet, or will be thrown away if the FIFO gets
 *   reset prior to the next transmit. In general, the proper number of
 *   packet bytes to put into the transmit FIFO are all payload bytes except
 *   for any CRC bytes which the packet configuration causes to be sent
 *   automatically.
 */
uint16_t RAIL_SetTxFifo(RAIL_Handle_t railHandle,
                        uint8_t *addr,
                        uint16_t initLength,
                        uint16_t size);

/**
 * Set the address of the receive FIFO, a circular buffer used for RX data.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in,out] addr A pointer to a read-write memory location in RAM used as
 *   the receive FIFO. This memory must persist until the next call to this
 *   function.
 * @param[in,out] size A desired size of the receive FIFO in bytes. This will
 *   be populated with the actual size during the function call.
 * @return Status code indicating success of the function call.
 *
 * This function sets the memory location for the receive FIFO. It
 * must be called at least once before any receive operations occur.
 *
 * @note After it is called, any prior receive FIFO is orphaned. To avoid
 *   orphaning the default internal 512-byte receive FIFO so it does
 *   not unnecessarily consume RAM resources in your application,
 *   implement \ref RAILCb_SetupRxFifo() to call this function.
 *
 * FIFO size can be determined by the return value of this function. The
 * chosen size is determined based on the available FIFO sizes supported by the
 * hardware. Similarly, some hardware has stricter FIFO alignment requirements;
 * 32-bit alignment provides the maximum portability across all RAIL platforms.
 * For more on supported FIFO sizes and alignments, see chip-specific
 * documentation, such as \ref efr32_main. The returned FIFO size will be the
 * closest allowed size less than or equal to the passed in size parameter,
 * unless the size parameter is smaller than the minimum FIFO size.
 *
 * This function reserves the block of RAM starting at addr with a length
 * of size, which is used internally as a circular buffer for the receive FIFO.
 * It must be able to hold the entire FIFO size. The caller must guarantee that
 * the custom FIFO remains intact and unchanged (except via incoming packet data
 * being written) until the next call to this function.
 *
 * In multiprotocol, RAIL currently shares one receive FIFO across all
 * protocols. This function will return \ref RAIL_STATUS_INVALID_STATE if the
 * requested \ref RAIL_Handle_t is not active.
 */
RAIL_Status_t RAIL_SetRxFifo(RAIL_Handle_t railHandle,
                             uint8_t *addr,
                             uint16_t *size);

/// Set up the receive FIFO to use. This function is optional to implement.
///
/// @param[in] railHandle A RAIL instance handle.
/// @return Status code indicating success of the function call.
///
/// This function is called during the \ref RAIL_Init process to set up the FIFO
/// to use for received packets. If not implemented by the application,
/// a default implementation from within the RAIL library will be used to
/// initialize an internal default 512-byte receive FIFO.
///
/// If this function returns an error, the RAIL_Init process will fail.
///
/// During this function, the application should generally call
/// \ref RAIL_SetRxFifo. If that does not happen, the application needs to
/// set up the receive FIFO via a call to \ref RAIL_SetRxFifo before attempting
/// to receive any packets. An example implementation may look like the following:
/// @code{.c}
/// #define RX_FIFO_SIZE 1024
/// static uint8_t rxFifo[RX_FIFO_SIZE];
///
/// RAIL_Status_t RAILCb_SetupRxFifo(RAIL_Handle_t railHandle)
/// {
///   uint16_t rxFifoSize = RX_FIFO_SIZE;
///   RAIL_Status_t status = RAIL_SetRxFifo(railHandle, &rxFifo[0], &rxFifoSize);
///   if (rxFifoSize != RX_FIFO_SIZE) {
///     // We set up an incorrect FIFO size
///     return RAIL_STATUS_INVALID_PARAMETER;
///   }
///   if (status == RAIL_STATUS_INVALID_STATE) {
///     // Allow failures due to multiprotocol
///     return RAIL_STATUS_NO_ERROR;
///   }
///   return status;
/// }
/// @endcode
RAIL_Status_t RAILCb_SetupRxFifo(RAIL_Handle_t railHandle);

/**
 * Read packet data from RAIL's internal receive FIFO.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[out] dataPtr An application-provided pointer to store data.
 *   If NULL, the data is thrown away rather than copied out.
 * @param[in] readLength A number of packet bytes to read from the FIFO.
 * @return The number of packet bytes read from the receive FIFO.
 *
 * This function reads packet data from the head of receive FIFO and
 * writes it to the provided dataPtr. It does not permit reading more
 * data than is available in the FIFO, nor does it permit reading more
 * data than remains in the oldest unreleased packet.
 *
 * Because this function does not have a critical section, use it
 * only in one context or make sure function calls are protected to prevent
 * buffer corruption.
 *
 * @warning This function is intended for use only with \ref
 *   RAIL_DataMethod_t::FIFO_MODE and should never be called in \ref
 *   RAIL_DataMethod_t::PACKET_MODE where it could lead to receive FIFO
 *   corruption.
 *
 * @note When reading data from an arriving packet that is not yet complete,
 *   keep in mind its data is highly suspect because it has not yet passed
 *   any CRC integrity checking. Also note that the packet could be aborted,
 *   canceled, or fail momentarily, invalidating its data in Packet mode.
 *   Furthermore, there is a small chance towards the end of packet reception
 *   that the receive FIFO could include not only packet data received so far,
 *   but also some raw radio-appended info detail bytes that RAIL's
 *   packet-completion processing will subsequently deal with. It's up to the
 *   application to know its packet format well enough to avoid reading this
 *   info because it will corrupt the packet's details and possibly corrupt the
 *   receive FIFO.
 */
uint16_t RAIL_ReadRxFifo(RAIL_Handle_t railHandle,
                         uint8_t *dataPtr,
                         uint16_t readLength);

/**
 * Configure the RAIL transmit FIFO almost empty threshold.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] txThreshold The threshold below which the
 *   \ref RAIL_EVENT_TX_FIFO_ALMOST_EMPTY event will fire.
 * @return Configured transmit FIFO threshold value.
 *
 * This function configures the threshold for the transmit FIFO. When the
 * number of bytes in the transmit FIFO falls below the configured threshold,
 * \ref RAIL_Config_t::eventsCallback will fire with \ref
 * RAIL_EVENT_TX_FIFO_ALMOST_EMPTY set.
 * The txThreshold value should be smaller than or equal to the transmit
 * FIFO size; higher values will be pegged to the FIFO size.
 * A value of 0 or \ref RAIL_FIFO_THRESHOLD_DISABLED will disable the
 * threshold, returning \ref RAIL_FIFO_THRESHOLD_DISABLED.
 */
uint16_t RAIL_SetTxFifoThreshold(RAIL_Handle_t railHandle,
                                 uint16_t txThreshold);

/**
 * Configure the RAIL receive FIFO almost full threshold.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] rxThreshold The threshold above which the
 *   \ref RAIL_EVENT_RX_FIFO_ALMOST_FULL event will fire.
 * @return Configured receive FIFO threshold value.
 *
 * This function configures the threshold for the receive FIFO. When the
 * number of bytes of packet data in the receive FIFO exceeds the configured
 * threshold, \ref RAIL_Config_t::eventsCallback will fire with \ref
 * RAIL_EVENT_RX_FIFO_ALMOST_FULL set.
 * The rxThreshold value should be smaller than the receive FIFO size;
 * anything else, including a
 * value of \ref RAIL_FIFO_THRESHOLD_DISABLED, will disable the threshold,
 * returning \ref RAIL_FIFO_THRESHOLD_DISABLED.
 */
uint16_t RAIL_SetRxFifoThreshold(RAIL_Handle_t railHandle,
                                 uint16_t rxThreshold);

/**
 * Get the RAIL transmit FIFO almost empty threshold value.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return Configured TX Threshold value.
 *
 * Retrieves the configured TX threshold value.
 */
uint16_t RAIL_GetTxFifoThreshold(RAIL_Handle_t railHandle);

/**
 * Get the RAIL receive FIFO almost full threshold value.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return Configured RX Threshold value.
 *
 * Retrieves the configured RX threshold value.
 */
uint16_t RAIL_GetRxFifoThreshold(RAIL_Handle_t railHandle);

/**
 * Reset the RAIL transmit and/or receive FIFOs.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] txFifo If true, reset the transmit FIFO.
 * @param[in] rxFifo If true, reset the receive FIFO.
 *
 * This function can reset each FIFO independently.
 * The application should not reset the receive FIFO while receiving a frame,
 * nor should it reset the transmit FIFO while transmitting a frame.
 */
void RAIL_ResetFifo(RAIL_Handle_t railHandle, bool txFifo, bool rxFifo);

/**
 * Get the number of bytes used in the receive FIFO.
 * Only use this function in RX \ref RAIL_DataMethod_t::FIFO_MODE.
 * Apps should use RAIL_GetRxPacketInfo() instead.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return Number of bytes used in the receive FIFO.
 *
 * This function indicates how much packet-related data exists in the receive FIFO
 * that could be read.
 *
 * @note The number of bytes returned may not just reflect the current
 *   packet's data but could also include raw appended info bytes added
 *   after successful packet reception and bytes from subsequently received
 *   packets. It is up to the app to never try to consume more than the
 *   packet's actual data when using the value returned here in a subsequent
 *   call to RAIL_ReadRxFifo(), otherwise the receive FIFO will be corrupted.
 */
uint16_t RAIL_GetRxFifoBytesAvailable(RAIL_Handle_t railHandle);

/**
 * Get the number of bytes unused in the transmit FIFO.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return Number of bytes unused in the transmit FIFO.
 *
 * This function indicates how much space is available in the transmit FIFO for writing
 * additional packet data.
 */
uint16_t RAIL_GetTxFifoSpaceAvailable(RAIL_Handle_t railHandle);

/** @} */ // end of group Data_Management

/******************************************************************************
 * State Transitions
 *****************************************************************************/
/**
 * @addtogroup State_Transitions State Transitions
 * @{
 */

/**
 * Configure RAIL automatic state transitions after RX.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] transitions The state transitions to apply after reception.
 * @return Status code indicating success of the function call.
 *
 * This function fails if unsupported transitions are passed in or if the
 * radio is currently in the RX state. Success can transition to TX, RX, or
 * IDLE, while error can transition to RX or IDLE. The timings of state
 * transitions from the RX state are not guaranteed when packets are longer
 * than 16 seconds on-air.
 */
RAIL_Status_t RAIL_SetRxTransitions(RAIL_Handle_t railHandle,
                                    const RAIL_StateTransitions_t *transitions);

/**
 * Get the current RAIL automatic state transitions after RX.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[out] transitions The state transitions that apply after receive.
 * @return Status code indicating a success of the function call.
 *
 * Retrieves the current state transitions after RX and stores them in the
 * transitions argument.
 */
RAIL_Status_t RAIL_GetRxTransitions(RAIL_Handle_t railHandle,
                                    RAIL_StateTransitions_t *transitions);

/**
 * Configure RAIL automatic state transitions after TX.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] transitions The state transitions to apply after transmission.
 * @return Status code indicating a success of the function call.
 *
 * This function fails if unsupported transitions are passed in or if the
 * radio is currently in the TX state. Success and error can each transition
 * to RX or IDLE. For the ability to run repeated transmits, see
 * \ref RAIL_SetNextTxRepeat.
 */
RAIL_Status_t RAIL_SetTxTransitions(RAIL_Handle_t railHandle,
                                    const RAIL_StateTransitions_t *transitions);

/**
 * Get the current RAIL automatic state transitions after TX.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[out] transitions The state transitions that apply after transmission.
 * @return Status code indicating a success of the function call.
 *
 * Retrieves the current state transitions after TX and stores them in the
 * transitions argument.
 */
RAIL_Status_t RAIL_GetTxTransitions(RAIL_Handle_t railHandle,
                                    RAIL_StateTransitions_t *transitions);

/**
 * Set up automatic repeated transmits after the next transmit.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] repeatConfig The configuration structure for repeated transmits.
 * @return Status code indicating a success of the function call.
 *
 * Repeated transmits will occur after an application-initiated transmit caused
 * by calling one of the \ref Packet_TX APIs. The repetition will only occur
 * after the first application-initiated transmit after this function is
 * called. Future repeated transmits must be requested by calling this function
 * again.
 *
 * Each repeated transmit that occurs will have full \ref PTI information, and
 * will receive events such as \ref RAIL_EVENT_TX_PACKET_SENT as normal.
 *
 * If a TX error occurs during the repetition, the process will abort and the
 * TX error transition from \ref RAIL_SetTxTransitions will be used. If the
 * repetition completes successfully, then the TX success transition from
 * \ref RAIL_SetTxTransitions will be used.
 *
 * Use \ref RAIL_GetTxPacketsRemaining() if need to know how many transmit
 * completion events are expected before the repeating sequence is done, or
 * how many were not performed due to a transmit error.
 *
 * Any call to \ref RAIL_Idle or \ref RAIL_StopTx will clear the pending
 * repeated transmits. The state will also be cleared by another call to this
 * function. To clear the repeated transmits before they've started without
 * stopping other radio actions, call this function with a \ref
 * RAIL_TxRepeatConfig_t::iterations count of 0. A DMP switch will clear this
 * state only if the initial transmit triggering the repeated transmits has
 * started.
 *
 * The application is responsible for populating the transmit data to be used
 * by the repeated transmits via \ref RAIL_SetTxFifo or \ref RAIL_WriteTxFifo.
 * Data will be transmitted from the Transmit FIFO. If the Transmit FIFO does
 * not have sufficient data to transmit, a TX error will be caused and a \ref
 * RAIL_EVENT_TX_UNDERFLOW will occur. In order to avoid an underflow, the
 * application should queue data to be transmitted as early as possible.
 * Consider using \ref RAIL_TX_OPTION_RESEND if the same packet data is to
 * be repeated: then the Transmit FIFO only needs to be set/written once.
 *
 * Do not call this function after starting a transmit operation or before
 * processing the final transmit completion event of a prior transmit.
 * This function will fail to configure the repetition if a transmit of any
 * kind is ongoing, including during the time between an initial transmit and
 * the end of a previously-configured repetition.
 *
 * @note This feature/API is not supported on the EFR32XG1 family of chips.
 *        Use the compile time symbol \ref RAIL_SUPPORTS_TX_TO_TX or the runtime
 *        call \ref RAIL_SupportsTxToTx() to check whether the platform supports
 *        this feature.
 */
RAIL_Status_t RAIL_SetNextTxRepeat(RAIL_Handle_t railHandle,
                                   const RAIL_TxRepeatConfig_t *repeatConfig);

/**
 * Get the number of transmits remaining in a repeat operation.
 * Must only be called from within event callback context when handling
 * one of the \ref RAIL_EVENTS_TX_COMPLETION events.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return transmits remaining as described below.
 *
 * If the TX completion event is \ref RAIL_EVENT_TX_PACKET_SENT the
 * returned value indicates how many more such events are expected
 * before the repeat transmit operation is done. Due to interrupt
 * latency and timing, this may be an overcount if greater than 0
 * but is guaranteed to be accurate when 0.
 *
 * If the TX completion event is an error, the returned value indicates
 * the number of requested transmits that were not performed. For
 * \ref RAIL_EVENT_TX_ABORTED and \ref RAIL_EVENT_TX_UNDERFLOW the
 * count does not include the failing transmit itself. For the other
 * errors where a transmit never started or was blocked, the count
 * would include the failing transmit, which may be one higher than
 * the configured \ref RAIL_TxRepeatConfig_t::iterations if it was
 * the original transmit that was blocked.
 *
 * If an infinite repeat was configured, this will return \ref
 * RAIL_TX_REPEAT_INFINITE_ITERATIONS.
 */
uint16_t RAIL_GetTxPacketsRemaining(RAIL_Handle_t railHandle);

/**
 * Configure RAIL automatic state transition timing.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in,out] timings The timings used to configure the RAIL state
 *   machine. This structure is overwritten with the actual times that were
 *   set, if an input timing is invalid.
 * @return Status code indicating a success of the function call.
 *
 * The timings given are close to the actual transition time. However,
 * a still uncharacterized software overhead occurs. Also, timings are not
 * always adhered to when using an automatic transition after an error, due to
 * the cleanup required to recover from the error.
 */
RAIL_Status_t RAIL_SetStateTiming(RAIL_Handle_t railHandle,
                                  RAIL_StateTiming_t *timings);

/**
 * Place the radio into an idle state.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] mode The method for shutting down the radio.
 * @param[in] wait Whether this function should wait for the radio to reach
 *   idle before returning.
 *
 * This function is used to remove the radio from TX and RX states. How these
 * states are left is defined by the mode parameter.
 *
 * In multiprotocol, this API will also cause the radio to be yielded so that
 * other tasks can be run. See \ref rail_radio_scheduler_yield for more details.
 */
void RAIL_Idle(RAIL_Handle_t railHandle,
               RAIL_IdleMode_t mode,
               bool wait);

/**
 * Get the current radio state.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return An enumeration for the current radio state.
 *
 * Returns the state of the radio as a bitmask containing:
 * \ref RAIL_RF_STATE_IDLE, \ref RAIL_RF_STATE_RX, \ref RAIL_RF_STATE_TX,
 * and \ref RAIL_RF_STATE_ACTIVE. \ref RAIL_RF_STATE_IDLE, \ref
 * RAIL_RF_STATE_RX, and \ref RAIL_RF_STATE_TX bits are mutually exclusive.
 * The radio can transition through intermediate states,
 * which are not reported but are instead considered part of the state
 * most closely associated. For example, when the radio is warming up
 * or shutting down the transmitter or receiver, this function returns
 * \ref RAIL_RF_STATE_TX or \ref RAIL_RF_STATE_RX, respectively.
 * When transitioning directly from RX to TX or vice-versa, this function
 * returns the earlier state.
 *
 * @note For a more detailed radio state, see \ref RAIL_GetRadioStateDetail
 */
RAIL_RadioState_t RAIL_GetRadioState(RAIL_Handle_t railHandle);

/**
 * Get the detailed current radio state.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return An enumeration for the current detailed radio state.
 *
 * Returns the state of the radio as a bitmask. The three core radio states
 * IDLE, RX, and TX are represented by mutually exclusive bits \ref
 * RAIL_RF_STATE_DETAIL_IDLE_STATE, \ref RAIL_RF_STATE_DETAIL_RX_STATE, and
 * \ref RAIL_RF_STATE_DETAIL_TX_STATE respectively. If the radio is
 * transitioning between these three states, the returned bitmask will have
 * \ref RAIL_RF_STATE_DETAIL_TRANSITION set along with a bit corresponding to
 * the destination core radio state. If, while in the receive state, the radio
 * is actively receiving a packet, \ref RAIL_RF_STATE_DETAIL_ACTIVE will be set;
 * otherwise, this bit will be clear. If frame detection is disabled, \ref
 * RAIL_RF_STATE_DETAIL_NO_FRAMES in the returned state bitmask will be set;
 * otherwise, this bit will be clear. If the radio is performing an LBT/CSMA
 * operation (e.g., a backoff period) \ref RAIL_RF_STATE_DETAIL_LBT in the
 * returned state bitmask will be set; otherwise, this bit will be clear.
 *
 * For the most part, the more detailed radio states returned by this API
 * correspond to radio states returned by \ref RAIL_GetRadioState as follows:
 *
 *   \ref RAIL_RadioStateDetail_t        \ref RAIL_RadioState_t
 *   RAIL_RF_STATE_DETAIL_INACTIVE       RAIL_RF_STATE_INACTIVE
 *   RAIL_RF_STATE_DETAIL_IDLE_STATE
 *   | RAIL_STATE_DETAIL_TRANSITION      If RX overflow or leaving RX unforced:
 *                                         RAIL_RF_STATE_RX
 *                                       Else if leaving TX unforced:
 *                                         RAIL_RF_STATE_TX
 *                                       Else:
 *                                         RAIL_RF_STATE_IDLE
 *   RAIL_RF_STATE_DETAIL_IDLE_STATE     RAIL_RF_STATE_IDLE
 *   RAIL_RF_STATE_DETAIL_IDLE_STATE
 *   | RAIL_STATE_DETAIL_LBT             RAIL_RF_STATE_TX
 *   RAIL_RF_STATE_DETAIL_RX_STATE
 *   | RAIL_STATE_DETAIL_TRANSITION      If leaving TX:
 *                                         RAIL_RF_STATE_TX
 *                                       Else:
 *                                         RAIL_RF_STATE_RX
 *   RAIL_RF_STATE_DETAIL_RX_STATE
 *   | RAIL_RF_STATE_DETAIL_TRANSITION
 *   | RAIL_RF_STATE_DETAIL_NO_FRAMES    If leaving TX:
 *                                         RAIL_RF_STATE_TX
 *                                       Else:
 *                                         RAIL_RF_STATE_RX
 *   RAIL_RF_STATE_DETAIL_RX_STATE       RAIL_RF_STATE_RX
 *   RAIL_RF_STATE_DETAIL_RX_STATE
 *   | RAIL_RF_STATE_DETAIL_NO_FRAMES    RAIL_RF_STATE_RX
 *   RAIL_RF_STATE_DETAIL_RX_STATE
 *   | RAIL_RF_STATE_DETAIL_LBT          RAIL_RF_STATE_RX
 *   RAIL_RF_STATE_DETAIL_RX_STATE
 *   | RAIL_RF_STATE_DETAIL_NO_FRAMES
 *   | RAIL_RF_STATE_DETAIL_LBT          RAIL_RF_STATE_RX
 *   RAIL_RF_STATE_DETAIL_RX_STATE
 *   | RAIL_RF_STATE_DETAIL_ACTIVE       RAIL_RF_STATE_RX_ACTIVE
 *   RAIL_RF_STATE_DETAIL_TX_STATE
 *   | RAIL_RF_STATE_TRANSITION          If leaving RX:
 *                                         RAIL_RF_STATE_RX
 *                                       Else:
 *                                         RAIL_RF_STATE_TX
 *   RAIL_RF_STATE_DETAIL_TX_STATE
 *   | RAIL_RF_STATE_ACTIVE              RAIL_RF_STATE_TX_ACTIVE
 */
RAIL_RadioStateDetail_t RAIL_GetRadioStateDetail(RAIL_Handle_t railHandle);

/** @} */ // end of group State_Transitions

/******************************************************************************
 * Transmit
 *****************************************************************************/
/**
 * @addtogroup Transmit
 * @brief APIs related to transmitting data packets
 * @{
 */

/// @addtogroup PA Power Amplifier (PA)
/// @brief APIs for interacting with one of the on chip PAs.
///
/// These APIs let you configure the on-chip PA to get the appropriate output
/// power.
///
/// These are the function types:
///   1) Configuration functions: These functions set and get configuration
///      for the PA. In this case, "configuration" refers to a) indicating
///      which PA to use, b) the voltage supplied by your board to the PA,
///      and c) the ramp time over which to ramp the PA up to its full
///      power.
///   2) Power-setting functions: These functions consume the actual
///      values written to the PA registers and write them appropriately.
///      These values are referred to as "(raw) power levels". The range of
///      acceptable values for these functions depends on which PA is
///      currently active. The higher the power level set, the higher
///      the dBm power output by the chip. However, the mapping
///      between dBm and these power levels can vary greatly between
///      modules/boards.
///   3) Conversion functions: These functions convert
///      between the "power levels" discussed previously and the
///      dBm values output by the chip. Continue reading for more information
///      about unit conversion.
///
/// The accuracy of the chip output power is application-specific.
/// For some protocols or channels, the protocol itself or
/// legal limitations require applications to know exactly what power
/// they're transmitting at, in dBm. Other applications do not have
/// these restrictions, and users determine power level(s)
/// that fit their criteria for the trade-off between radio range and
/// power savings, regardless of what dBm power that maps to.
///
/// \ref RAIL_ConvertRawToDbm and \ref RAIL_ConvertDbmToRaw,
/// which convert between the dBm power and the raw power levels,
/// provide a solution that fits all these applications.
/// The levels of customizability are outlined below:
///  1) No customizability needed: for a given dBm value, the result
///     of RAIL_ConvertDbmToRaw provides an appropriate
///     raw power level that, when written to the registers via
///     RAIL_SetPowerLevel, causes the chip to output at that
///     dBm power. In this case, no action is needed by the user,
///     the WEAK versions of the conversion functions can be used
///     and the default include paths in pa_conversions_efr32.h can
///     be used.
///  2) The mapping of power level to dBm is not ideal, but the
///     level of precision is sufficient: In pa_conversions_efr32.c,
///     the WEAK versions of the conversion functions work by using
///     8-segment piecewise linear curves to convert between dBm
///     and power levels for PA's with hundreds of power levels
///     and simple mapping tables for use with PA's with only a few
///     levels. If this method is sufficiently precise, but the mapping
///     between power levels and dBm is incorrect,
///     copy pa_curves_efr32.h into a new file, updating the segments
///     to form a better fit (_DCDC_CURVES or _VBAT_CURVES defines) and
///     then add the RAIL_PA_CURVES define to your build with the path
///     to the new file.
///  3) A different level of precision is needed and the fit is bad:
///     If the piecewise-linear line segment fit is not appropriate for
///     your solution, the functions in pa_conversions_efr32.c can be
///     totally rewritten, as long as RAIL_ConvertDbmToRaw and
///     RAIL_ConvertRawToDbm have the same signatures. It is completely
///     acceptable to re-write these in a way that makes the
///     pa_curves_efr32.h and pa_curve_types_efr32.h files referenced in
///     pa_conversions_efr32.h unnecessary. Those files are needed solely
///     for the provided conversion methods.
///  4) dBm values are not necessary: If the application does not require
///     dBm values at all, overwrite
///     RAIL_ConvertDbmToRaw and RAIL_ConvertRawToDbm with smaller functions
///     (i.e., return 0 or whatever was input). These functions are called
///     from within the RAIL library, so they can never be deadstripped,
///     but making them as small as possible is the best way to reduce code
///     size. From there, call RAIL_SetTxPower, without
///     converting from a dBm value. To stop the library from coercing the
///     power based on channels, overwrite RAIL_ConvertRawToDbm
///     to always return 0 and overwrite RAIL_ConvertDbmToRaw to
///     always return 255.
///
/// The following is example code that shows how to initialize your PA
/// @code{.c}
///
/// #include "pa_conversions_efr32.h"
///
/// // A helper macro to declare all the curve structures used by the provided
/// // conversion functions.
/// RAIL_DECLARE_TX_POWER_VBAT_CURVES(piecewiseSegments, curvesSg, curves24Hp, curves24Lp);
///
/// // Puts the variables declared above into the appropriate structure.
/// RAIL_TxPowerCurvesConfig_t txPowerCurvesConfig = { curves24Hp, curvesSg, curves24Lp, piecewiseSegments };
///
/// // Saves those curves
/// // to be referenced when the conversion functions are called.
/// RAIL_InitTxPowerCurves(&txPowerCurvesConfig);
///
/// // Declares the structure used to configure the PA.
/// RAIL_TxPowerConfig_t txPowerConfig = { RAIL_TX_POWER_MODE_2P4_HP, 3300, 10 };
///
/// // Initializes the PA. Here, it is assumed that 'railHandle' is a valid RAIL_Handle_t
/// // that has already been initialized.
/// RAIL_ConfigTxPower(railHandle, &txPowerConfig);
///
/// // Picks a dBm power to use: 100 deci-dBm = 10 dBm. See docs on RAIL_TxPower_t.
/// RAIL_TxPower_t power = 100;
///
/// // Gets the config written by RAIL_ConfigTxPower to confirm what was actually set.
/// RAIL_GetTxPowerConfig(railHandle, &txPowerConfig);
///
/// // RAIL_ConvertDbmToRaw is the default weak version,
/// // or the customer version, if overwritten.
/// RAIL_TxPowerLevel_t powerLevel = RAIL_ConvertDbmToRaw(railHandle,
///                                                       txPowerConfig.mode,
///                                                       power);
///
/// // Writes the result of the conversion to the PA power registers in terms
/// // of raw power levels.
/// RAIL_SetTxPower(railHandle, powerLevel);
/// @endcode
///
/// @note All lines following "RAIL_TxPower_t power = 100;" can be
/// replaced with the provided utility function, \ref RAIL_SetTxPowerDbm.
/// However, the full example here was provided for clarity. See the
/// documentation on \ref RAIL_SetTxPowerDbm for more details.
///
/// @{

/**
 * Initialize TX power settings.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] config An instance which contains desired initial settings
 *   for the TX amplifier.
 * @return RAIL_Status_t indicating success or an error.
 *
 * These settings include the selection between the multiple TX amplifiers,
 * voltage supplied to the TX power amplifier, and ramp times. This must
 * be called before any transmit occurs or \ref RAIL_SetTxPower is called.
 * While this function should always be called during initialization,
 * it can also be called any time if these settings need to change to adapt
 * to a different application/protocol. This API also resets TX power to
 * \ref RAIL_TX_POWER_LEVEL_INVALID, so \ref RAIL_SetTxPower must be called
 * afterwards.
 *
 * At times, certain combinations of configurations cannot be achieved.
 * This API attempts to get as close as possible to the requested settings. The
 * following "RAIL_Get..." API can be used to determine what values were set. A
 * change in \ref RAIL_TxPowerConfig_t::rampTime may affect the minimum timings
 * that can be achieved in \ref RAIL_StateTiming_t::idleToTx and
 * \ref RAIL_StateTiming_t::rxToTx. Call \ref RAIL_SetStateTiming() again to
 * check whether these times have changed.
 */
RAIL_Status_t RAIL_ConfigTxPower(RAIL_Handle_t railHandle,
                                 const RAIL_TxPowerConfig_t *config);

/**
 * Get the TX power settings currently used in the amplifier.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[out] config A pointer to memory allocated to hold the current TxPower
 *   configuration structure. A NULL configuration will produce undefined
 *   behavior.
 * @return RAIL status variable indicating whether or not the get was
 * successful.
 *
 * Note that this API does not return the current TX power, which is separately
 * managed by the \ref RAIL_GetTxPower / \ref RAIL_SetTxPower APIs. Use this API
 * to determine which values were set as a result of
 * \ref RAIL_ConfigTxPower.
 */
RAIL_Status_t RAIL_GetTxPowerConfig(RAIL_Handle_t railHandle,
                                    RAIL_TxPowerConfig_t *config);

/**
 * Set the TX power in units of raw units (see \ref rail_chip_specific.h for
 * value ranges).
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] powerLevel Power in chip-specific \ref RAIL_TxPowerLevel_t units.
 * @return RAIL_Status_t indicating success or an error.
 *
 * To convert between decibels and the integer values that the
 * registers take, call \ref RAIL_ConvertDbmToRaw.
 * A weak version of this function, which works well with our boards is provided. However,
 * customers using a custom board need to characterize
 * chip operation on that board and override the function to convert
 * appropriately from the desired dB values to raw integer values.
 *
 * Depending on the configuration used in \ref RAIL_ConfigTxPower, not all
 * power levels are achievable. This API will get as close as possible to
 * the desired power without exceeding it, and calling \ref RAIL_GetTxPower is
 * the only way to know the exact value written.
 *
 * Calling this function before configuring the PA (i.e., before a successful
 * call to \ref RAIL_ConfigTxPower) will return an error.
 */
RAIL_Status_t RAIL_SetTxPower(RAIL_Handle_t railHandle,
                              RAIL_TxPowerLevel_t powerLevel);

/**
 * Return the current power setting of the PA.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return The chip-specific \ref RAIL_TxPowerLevel_t value of the current
 * transmit power.
 *
 * This API returns the raw value that was set by \ref RAIL_SetTxPower.
 * A weak version of \ref RAIL_ConvertRawToDbm that works
 * with our boards to convert the raw values into actual output dBm values is provided.
 * However, customers using a custom board need to
 * re-characterize the relationship between raw and decibel values and rewrite
 * the provided function.
 *
 * Calling this function before configuring the PA (i.e., before a successful
 * call to \ref RAIL_ConfigTxPower) will return an error
 * (RAIL_TX_POWER_LEVEL_INVALID).
 */
RAIL_TxPowerLevel_t RAIL_GetTxPower(RAIL_Handle_t railHandle);

/**
 * Convert raw values written to registers to decibel value (in units of
 * deci-dBm).
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] mode PA mode for which to convert.
 * @param[in] powerLevel A raw amplifier register value to be converted to
 *   deci-dBm.
 * @return raw amplifier values converted to units of deci-dBm.
 *
 * A weak version of this function is provided that is tuned
 * to provide accurate values for our boards. For a
 * custom board, the relationship between what is written to the TX amplifier
 * and the actual output power should be re-characterized and implemented in an
 * overriding version of \ref RAIL_ConvertRawToDbm. For minimum code size and
 * best speed, use only raw values with the TxPower API and override this
 * function with a smaller function. In the weak version provided with the RAIL
 * library, railHandle is only used to indicate to the user from where the
 * function was called, so it is OK to use either a real protocol handle, or one
 * of the chip-specific ones, such as \ref RAIL_EFR32_HANDLE.
 *
 * Although the definitions of this function may change, the signature
 * must be as declared here.
 */
RAIL_TxPower_t RAIL_ConvertRawToDbm(RAIL_Handle_t railHandle,
                                    RAIL_TxPowerMode_t mode,
                                    RAIL_TxPowerLevel_t powerLevel);

/**
 * Convert the desired decibel value (in units of deci-dBm)
 * to raw integer values used by the TX amplifier registers.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] mode PA mode for which to do the conversion.
 * @param[in] power Desired dBm values in units of deci-dBm.
 * @return deci-dBm value converted to a raw
 *   integer value that can be used directly with \ref RAIL_SetTxPower.
 *
 * A weak version of this function is provided that is tuned
 * to provide accurate values for our boards. For a
 * custom board, the relationship between what is written to the TX amplifier
 * and the actual output power should be characterized and implemented in an
 * overriding version of \ref RAIL_ConvertDbmToRaw. For minimum code size and
 * best speed use only raw values with the TxPower API and override this
 * function with a smaller function. In the weak version provided with the RAIL
 * library, railHandle is only used to indicate to the user from where the
 * function was called, so it is OK to use either a real protocol handle, or one
 * of the chip-specific ones, such as \ref RAIL_EFR32_HANDLE.
 *
 * Although the definitions of this function may change, the signature
 * must be as declared here.
 *
 * @note This function is called from within the RAIL library for
 *   comparison between channel limitations and current power. It will
 *   throw an assert if you haven't called RAIL_InitTxPowerCurves
 *   which initializes the mappings between raw power levels and
 *   actual dBm powers. To avoid the assert, ensure that the
 *   maxPower of all channel configuration entries is \ref RAIL_TX_POWER_MAX
 *   or above, or override this function to always return 255.
 */
RAIL_TxPowerLevel_t RAIL_ConvertDbmToRaw(RAIL_Handle_t railHandle,
                                         RAIL_TxPowerMode_t mode,
                                         RAIL_TxPower_t power);

struct RAIL_TxPowerCurvesConfigAlt;
/// Verify the TX Power Curves on modules.
///
/// @param[in] config TX Power Curves to use on this module.
///
/// This function only needs to be called when using a module and has no
/// effect otherwise. Transmit will not work before this function is called.
void RAIL_VerifyTxPowerCurves(const struct RAIL_TxPowerCurvesConfigAlt *config);

/// Set the TX power in terms of deci-dBm instead of raw power level.
///
/// @param[in] railHandle A RAIL instance handle.
/// @param[in] power A desired deci-dBm power to be set.
/// @return RAIL Status variable indicate whether setting the
///   power was successful.
///
/// This is a utility function for user convenience. Normally, to set TX
/// power in dBm, do the following:
///
/// @code{.c}
/// RAIL_TxPower_t power = 100; // 100 deci-dBm, 10 dBm
/// RAIL_TxPowerConfig_t txPowerConfig;
/// RAIL_GetTxPowerConfig(railHandle, &txPowerConfig);
/// // RAIL_ConvertDbmToRaw will be the weak version provided by Silicon Labs
/// // by default, or the customer version, if overwritten.
/// RAIL_TxPowerLevel_t powerLevel = RAIL_ConvertDbmToRaw(railHandle,
///                                                       txPowerConfig.mode,
///                                                       power);
/// RAIL_SetTxPower(railHandle, powerLevel);
/// @endcode
///
/// This function wraps all those calls in a single function with power passed in
/// as a parameter.
///
RAIL_Status_t RAIL_SetTxPowerDbm(RAIL_Handle_t railHandle,
                                 RAIL_TxPower_t power);

/// Get the TX power in terms of deci-dBm instead of raw power level.
///
/// @param[in] railHandle A RAIL instance handle.
/// @return The current output power in deci-dBm.
///
/// This is a utility function for user convenience. Normally, to get TX
/// power in dBm, do the following:
///
/// @code{.c}
/// RAIL_TxPowerLevel_t powerLevel = RAIL_GetTxPower(railHandle);
/// RAIL_TxPowerConfig_t txPowerConfig;
/// RAIL_GetTxPowerConfig(railHandle, &txPowerConfig);
/// // RAIL_ConvertRawToDbm will be the weak version provided by Silicon Labs
/// // by default, or the customer version, if overwritten.
/// RAIL_TxPower_t power = RAIL_ConvertRawToDbm(railHandle,
///                                             txPowerConfig.mode,
///                                             powerLevel);
/// return power;
/// @endcode
///
/// This function wraps all those calls in a single function with power returned
/// as the result.
///
RAIL_TxPower_t RAIL_GetTxPowerDbm(RAIL_Handle_t railHandle);

/**
 * Enable automatic switching between PAs internally to the RAIL library.
 * While PA Automode is enabled, the PA will be chosen and set automatically whenever
 * \ref RAIL_SetTxPowerDbm is called or whenever powers are coerced automatically,
 * internally to the RAIL library during a channel change. While PA Auto Mode
 * is enabled, users cannot call \ref RAIL_ConfigTxPower or
 * \ref RAIL_SetTxPower. When entering auto mode, \ref RAIL_SetTxPowerDbm must
 * be called to specify the desired power. When leaving auto mode,
 * \ref RAIL_ConfigTxPower as well as one of \ref RAIL_SetTxPower or
 * \ref RAIL_SetTxPowerDbm must be called to re-specify the desired PA and power
 * level combination.
 *
 * @note: Power conversion curves must be initialized before calling this function.
 * That is, \ref RAIL_ConvertDbmToRaw and \ref RAIL_ConvertRawToDbm most both be
 * able to operate properly to ensure that PA Auto Mode functions correctly.
 * See the PA Conversions plugin or AN1127 for more details.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] enable Enable or disable PA Auto Mode.
 * @return Status parameter indicating success of function call.
 */
RAIL_Status_t RAIL_EnablePaAutoMode(RAIL_Handle_t railHandle, bool enable);

/**
 * Query status of PA Auto Mode.
 *
 * @param[in] railHandle A RAIL instance handle on which to query PA Auto Mode status.
 * @return Indicator of whether Auto Mode is enabled (true) or not (false).
 */
bool RAIL_IsPaAutoModeEnabled(RAIL_Handle_t railHandle);

/**
 * Callback that decides which PA and power level should be
 * used while in PA auto mode.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in,out] power Pointer to the dBm output power (in deci-dBm, 10*dBm)
 * being requested. The value this points to when the function returns
 * will be applied to the radio.
 * @param[out] mode Pointer to the \ref RAIL_TxPowerMode_t to be used to
 * achieve the requested power. The value this points to when the function
 * returns will be applied to the radio.
 * @param[in] chCfgEntry Pointer to a \ref RAIL_ChannelConfigEntry_t.
 * While switching channels, it will be the entry RAIL is switch *to*,
 * during a call to \ref RAIL_SetTxPowerDbm, it will be the entry
 * RAIL is *already on*. Can be NULL if a channel configuration
 * was not set or no valid channels are present.
 * @return Return status indicating result of function call. If this
 * is anything except \ref RAIL_STATUS_NO_ERROR, neither PA's nor their
 * powers will be configured automatically.
 *
 * Whatever values mode and powerLevel point to when this function return
 * will be applied to the PA hardware and used for transmits.
 * @note The mode and power level provided by this function depends on the
 * RAIL_PaAutoModeConfig provided for the chip. The RAIL_PaAutoModeConfig
 * definition for a chip should tend to all the bands supported by the chip and
 * cover the full range of power to find a valid entry for requested power
 * for a specific band.
 */
RAIL_Status_t RAILCb_PaAutoModeDecision(RAIL_Handle_t railHandle,
                                        RAIL_TxPower_t *power,
                                        RAIL_TxPowerMode_t *mode,
                                        const RAIL_ChannelConfigEntry_t *chCfgEntry);

/** @} */ // end of group PA

/// @addtogroup Packet_TX Packet Transmit
/// @brief APIs which initiate a packet transmission in RAIL
///
/// When using any of these functions, the data to be transmitted must have been
/// previously written to the Transmit FIFO via \ref RAIL_SetTxFifo() and/or
/// \ref RAIL_WriteTxFifo().
///
/// @{

/**
 * Start a transmit.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] channel Define the channel to transmit on.
 * @param[in] options TX options to be applied to this transmit only.
 * @param[in] schedulerInfo Information to allow the radio scheduler to place
 *   this transmit appropriately. This is only used in multiprotocol version of
 *   RAIL and may be set to NULL in all other versions.
 * @return Status code indicating success of the function call. If successfully
 *   initiated, transmit completion or failure will be reported by a later
 *   \ref RAIL_Config_t::eventsCallback with the appropriate \ref RAIL_Events_t.
 *
 * The transmit process will begin immediately or as soon as a packet being
 * received has finished. The data to be transmitted must have been previously
 * established via \ref RAIL_SetTxFifo() and/or \ref RAIL_WriteTxFifo().
 *
 * Returns an error if a previous transmit is still in progress.
 * If changing channels, any ongoing packet reception is aborted.
 *
 * In multiprotocol, ensure that the radio is properly yielded after this
 * operation completes. See \ref rail_radio_scheduler_yield for more details.
 */
RAIL_Status_t RAIL_StartTx(RAIL_Handle_t railHandle,
                           uint16_t channel,
                           RAIL_TxOptions_t options,
                           const RAIL_SchedulerInfo_t *schedulerInfo);

/**
 * Schedule sending a packet.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] channel Define the channel to transmit on.
 * @param[in] options TX options to be applied to this transmit only.
 * @param[in] config A pointer to the \ref RAIL_ScheduleTxConfig_t
 *   structure containing when the transmit should occur.
 * @param[in] schedulerInfo Information to allow the radio scheduler to place
 *   this transmit appropriately. This is only used in multiprotocol version of
 *   RAIL and may be set to NULL in all other versions.
 * @return Status code indicating success of the function call. If successfully
 *   initiated, a transmit completion or failure will be reported by a later
 *   \ref RAIL_Config_t::eventsCallback with the appropriate \ref RAIL_Events_t.
 *
 * The transmit process will begin at the scheduled time. The data to be
 * transmitted must have been previously established via \ref RAIL_SetTxFifo()
 * and/or \ref RAIL_WriteTxFifo().
 * The time (in microseconds) and whether that time is absolute or
 * relative is specified using the \ref RAIL_ScheduleTxConfig_t structure.
 * What to do if a scheduled transmit fires in
 * the middle of receiving a packet is also specified in this structure.
 *
 * Returns an error if a previous transmit is still in progress.
 * If changing channels, the channel is changed immediately and
 * will abort any ongoing packet reception.
 *
 * In multiprotocol, ensure that the radio is properly yielded after this
 * operation completes. See \ref rail_radio_scheduler_yield for more details.
 */
RAIL_Status_t RAIL_StartScheduledTx(RAIL_Handle_t railHandle,
                                    uint16_t channel,
                                    RAIL_TxOptions_t options,
                                    const RAIL_ScheduleTxConfig_t *config,
                                    const RAIL_SchedulerInfo_t *schedulerInfo);

/**
 * Start a transmit using CSMA.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] channel Define the channel to transmit on.
 * @param[in] options TX options to be applied to this transmit only.
 * @param[in] csmaConfig A pointer to the RAIL_CsmaConfig_t structure
 *   describing the CSMA parameters to use for this transmit.
 *   \n In multiprotocol this must point to global or heap storage that remains
 *   valid after the API returns until the transmit is actually started.
 * @param[in] schedulerInfo Information to allow the radio scheduler to place
 *   this transmit appropriately. This is only used in multiprotocol version of
 *   RAIL and may be set to NULL in all other versions.
 * @return Status code indicating success of the function call. If successfully
 *   initiated, a transmit completion or failure will be reported by a later
 *   \ref RAIL_Config_t::eventsCallback with the appropriate \ref RAIL_Events_t.
 *
 * Perform the Carrier Sense Multiple Access (CSMA) algorithm, and if
 * the channel is deemed clear (RSSI below the specified threshold), it will
 * commence transmission. The data to be transmitted must have been previously
 * established via \ref RAIL_SetTxFifo() and/or \ref RAIL_WriteTxFifo().
 * Packets can be received during CSMA backoff periods if receive is active
 * throughout the CSMA process. This will happen either by starting the CSMA
 * process while receive is already active, or if the csmaBackoff time in
 * the \ref RAIL_CsmaConfig_t is less than the idleToRx time (set by
 * RAIL_SetStateTiming()). If the csmaBackoff time is greater than the
 * idleToRx time, receive will only be active during CSMA's clear channel
 * assessments.
 *
 * If the CSMA algorithm deems the channel busy, the \ref RAIL_Config_t::eventsCallback
 * occurs with \ref RAIL_EVENT_TX_CHANNEL_BUSY, and the contents
 * of the transmit FIFO remain intact.
 *
 * Returns an error if a previous transmit is still in progress.
 * If changing channels, the channel is changed immediately and any ongoing
 * packet reception is aborted.
 *
 * Returns an error if a scheduled RX is still in progress.
 *
 * In multiprotocol, ensure that the radio is properly yielded after this
 * operation completes. See \ref rail_radio_scheduler_yield for more details.
 */
RAIL_Status_t RAIL_StartCcaCsmaTx(RAIL_Handle_t railHandle,
                                  uint16_t channel,
                                  RAIL_TxOptions_t options,
                                  const RAIL_CsmaConfig_t *csmaConfig,
                                  const RAIL_SchedulerInfo_t *schedulerInfo);

/**
 * Start a transmit using LBT.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] channel Define the channel to transmit on.
 * @param[in] options TX options to be applied to this transmit only.
 * @param[in] lbtConfig A pointer to the RAIL_LbtConfig_t structure
 *   describing the LBT parameters to use for this transmit.
 *   \n In multiprotocol this must point to global or heap storage that remains
 *   valid after the API returns until the transmit is actually started.
 * @param[in] schedulerInfo Information to allow the radio scheduler to place
 *   this transmit appropriately. This is only used in multiprotocol version of
 *   RAIL and may be set to NULL in all other versions.
 * @return Status code indicating success of the function call. If successfully
 *   initiated, a transmit completion or failure will be reported by a later
 *   \ref RAIL_Config_t::eventsCallback with the appropriate \ref RAIL_Events_t.
 *
 * Performs the Listen Before Talk (LBT) algorithm, and if the channel
 * is deemed clear (RSSI below the specified threshold), it will commence
 * transmission. The data to be transmitted must have been previously established
 * via \ref RAIL_SetTxFifo() and/or \ref RAIL_WriteTxFifo().
 * Packets can be received during LBT backoff periods if receive is active
 * throughout the LBT process. This will happen either by starting the LBT
 * process while receive is already active, or if the lbtBackoff time in
 * the \ref RAIL_LbtConfig_t is less than the idleToRx time (set by
 * RAIL_SetStateTiming()). If the lbtBackoff time is greater than the
 * idleToRx time, receive will only be active during LBT's clear channel
 * assessments.
 *
 * If the LBT algorithm deems the channel busy, the \ref RAIL_Config_t::eventsCallback occurs with
 * \ref RAIL_EVENT_TX_CHANNEL_BUSY, and the contents
 * of the transmit FIFO remain intact.
 *
 * Returns an error if a previous transmit is still in progress.
 * If changing channels, the channel is changed immediately and any ongoing
 * packet reception is aborted.
 *
 * Returns an error if a scheduled RX is still in progress.
 *
 * In multiprotocol, ensure that the radio is properly yielded after this
 * operation completes. See \ref rail_radio_scheduler_yield for more details.
 */
RAIL_Status_t RAIL_StartCcaLbtTx(RAIL_Handle_t railHandle,
                                 uint16_t channel,
                                 RAIL_TxOptions_t options,
                                 const RAIL_LbtConfig_t *lbtConfig,
                                 const RAIL_SchedulerInfo_t *schedulerInfo);

/**
 * Schedule a transmit using CSMA.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] channel Define the channel to transmit on.
 * @param[in] options TX options to be applied to this transmit only.
 * @param[in] scheduleTxConfig A pointer to the \ref RAIL_ScheduleTxConfig_t
 *   structure describing the CSMA parameters to use for this transmit.
 * @param[in] csmaConfig A pointer to the \ref RAIL_CsmaConfig_t structure
 *   describing the CSMA parameters to use for this transmit.
 *   \n In multiprotocol this must point to global or heap storage that remains
 *   valid after the API returns until the transmit is actually started.
 * @param[in] schedulerInfo Information to allow the radio scheduler to place
 *   this transmit appropriately. This is only used in multiprotocol version of
 *   RAIL and may be set to NULL in all other versions.
 * @return Status code indicating success of the function call. If successfully
 *   initiated, a transmit completion or failure will be reported by a later
 *   \ref RAIL_Config_t::eventsCallback with the appropriate \ref RAIL_Events_t.
 *
 * Internally, the RAIL library needs a PRS channel for this feature.
 * It will allocate an available PRS channel to use and hold onto that
 * channel for future use. If no PRS channel is available, the function
 * returns with \ref RAIL_STATUS_INVALID_CALL.
 *
 * Perform the Carrier Sense Multiple Access (CSMA) algorithm at the scheduled time,
 * and if the channel is deemed clear (RSSI below the specified threshold), it will
 * commence transmission. The data to be transmitted must have been previously
 * established via \ref RAIL_SetTxFifo() and/or \ref RAIL_WriteTxFifo().
 * Packets can be received during CSMA backoff periods if receive is active
 * throughout the CSMA process. This will happen either by starting the CSMA
 * process while receive is already active, or if the csmaBackoff time in
 * the \ref RAIL_CsmaConfig_t is less than the idleToRx time (set by
 * RAIL_SetStateTiming()). If the csmaBackoff time is greater than the
 * idleToRx time, receive will only be active during CSMA's clear channel
 * assessments.
 *
 * If the CSMA algorithm deems the channel busy, the \ref RAIL_Config_t::eventsCallback
 * occurs with \ref RAIL_EVENT_TX_CHANNEL_BUSY, and the contents
 * of the transmit FIFO remain intact.
 *
 * Returns an error if a previous transmit is still in progress.
 * If changing channels, the channel is changed immediately and any ongoing
 * packet reception is aborted.
 *
 * In multiprotocol, ensure that the radio is properly yielded after this
 * operation completes. See \ref rail_radio_scheduler_yield for more details.
 */
RAIL_Status_t RAIL_StartScheduledCcaCsmaTx(RAIL_Handle_t railHandle,
                                           uint16_t channel,
                                           RAIL_TxOptions_t options,
                                           const RAIL_ScheduleTxConfig_t *scheduleTxConfig,
                                           const RAIL_CsmaConfig_t *csmaConfig,
                                           const RAIL_SchedulerInfo_t *schedulerInfo);

/**
 * Schedule a transmit using LBT.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] channel Define the channel to transmit on.
 * @param[in] options TX options to be applied to this transmit only.
 * @param[in] scheduleTxConfig A pointer to the \ref RAIL_ScheduleTxConfig_t
 *   structure describing the CSMA parameters to use for this transmit.
 * @param[in] lbtConfig A pointer to the \ref RAIL_LbtConfig_t structure
 *   describing the LBT parameters to use for this transmit.
 *   \n In multiprotocol this must point to global or heap storage that remains
 *   valid after the API returns until the transmit is actually started.
 * @param[in] schedulerInfo Information to allow the radio scheduler to place
 *   this transmit appropriately. This is only used in multiprotocol version of
 *   RAIL and may be set to NULL in all other versions.
 * @return Status code indicating success of the function call. If successfully
 *   initiated, a transmit completion or failure will be reported by a later
 *   \ref RAIL_Config_t::eventsCallback with the appropriate \ref RAIL_Events_t.
 *
 * Internally, the RAIL library needs a PRS channel for this feature.
 * It will allocate an available PRS channel to use and hold onto that
 * channel for future use. If no PRS channel is available, the function
 * returns with \ref RAIL_STATUS_INVALID_CALL.
 *
 * Performs the Listen Before Talk (LBT) algorithm at the scheduled time,
 * and if the channel is deemed clear (RSSI below the specified threshold), it will
 * commence transmission. The data to be transmitted must have been previously
 * established via \ref RAIL_SetTxFifo() and/or \ref RAIL_WriteTxFifo().
 * Packets can be received during LBT backoff periods if receive is active
 * throughout the LBT process. This will happen either by starting the LBT
 * process while receive is already active, or if the lbtBackoff time in
 * the \ref RAIL_LbtConfig_t is less than the idleToRx time (set by
 * RAIL_SetStateTiming()). If the lbtBackoff time is greater than the
 * idleToRx time, receive will only be active during LBT's clear channel
 * assessments.
 *
 * If the LBT algorithm deems the channel busy, the \ref RAIL_Config_t::eventsCallback
 * occurs with \ref RAIL_EVENT_TX_CHANNEL_BUSY, and the contents of the transmit
 * FIFO remain intact.
 *
 * Returns an error if a previous transmit is still in progress.
 * If changing channels, the channel is changed immediately and any ongoing
 * packet reception is aborted.
 *
 * In multiprotocol, ensure that the radio is properly yielded after this
 * operation completes. See \ref rail_radio_scheduler_yield for more details.
 */
RAIL_Status_t RAIL_StartScheduledCcaLbtTx(RAIL_Handle_t railHandle,
                                          uint16_t channel,
                                          RAIL_TxOptions_t options,
                                          const RAIL_ScheduleTxConfig_t *scheduleTxConfig,
                                          const RAIL_LbtConfig_t *lbtConfig,
                                          const RAIL_SchedulerInfo_t *schedulerInfo);

/** @} */ // end of group Packet_TX

/**
 * Stop an active or pending transmit.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] mode Configure the types of transmits to stop.
 * @return \ref RAIL_STATUS_NO_ERROR if the transmit was successfully
 *   canceled.
 *   Returns \ref RAIL_STATUS_INVALID_STATE if the requested
 *                transmit mode cannot be stopped.
 *
 * @note This function will stop an auto-ACK in active transmit.
 */
RAIL_Status_t RAIL_StopTx(RAIL_Handle_t railHandle, RAIL_StopMode_t mode);

/**
 * Set the CCA threshold in dBm.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] ccaThresholdDbm The CCA threshold in dBm.
 * @return Status code indicating success of the function call.
 *
 * Unlike RAIL_StartCcaCsmaTx() or RAIL_StartCcaLbtTx(), which can cause a
 * transmit, this function only modifies the CCA threshold. A possible
 * use case for this function involves setting the CCA threshold to invalid RSSI
 * of -128 which blocks transmission by preventing clear channel assessments
 * from succeeding.
 */
RAIL_Status_t RAIL_SetCcaThreshold(RAIL_Handle_t railHandle,
                                   int8_t ccaThresholdDbm);

/**
 * Get detailed information about the last packet transmitted.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in,out] pPacketDetails An application-provided pointer to store
 *   RAIL_TxPacketDetails_t corresponding to the transmit event.
 *   The isAck and timeSent fields totalPacketBytes and timePosition
 *   must be initialized prior to each call:
 *   - isAck true to obtain details about the most recent ACK transmit,
 *     false to obtain details about the most recent app-initiated transmit.
 *   - totalPacketBytes with the total number of bytes of the transmitted
 *     packet for RAIL to use when calculating the specified timestamp.
 *     This should account for all bytes sent over the air after the
 *     Preamble and Sync word(s), including CRC bytes.
 *   - timePosition with a \ref RAIL_PacketTimePosition_t value specifying
 *     the packet position to put in the timeSent field on return.
 *     This field will also be updated with the actual position corresponding
 *     to the timeSent value filled in.
 * @return \ref RAIL_STATUS_NO_ERROR if pPacketDetails was filled in,
 *   or an appropriate error code otherwise.
 *
 * @note Consider using \ref RAIL_GetTxPacketDetailsAlt2 for smaller code size.
 *
 * This function can only be called from callback context for either
 * \ref RAIL_EVENT_TX_PACKET_SENT or \ref RAIL_EVENT_TXACK_PACKET_SENT
 * events.
 */
RAIL_Status_t RAIL_GetTxPacketDetails(RAIL_Handle_t railHandle,
                                      RAIL_TxPacketDetails_t *pPacketDetails);

/**
 * Get detailed information about the last packet transmitted.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] isAck True to obtain details about the most recent ACK transmit.
 *   False to obtain details about the most recent app-initiated transmit.
 * @param[out] pPacketTime An application-provided non-NULL pointer to store a
 *   RAIL_Time_t corresponding to the transmit event. This will be populated
 *   with a timestamp corresponding to an arbitrary location in the packet. Call
 *   \ref RAIL_GetTxTimePreambleStart, \ref RAIL_GetTxTimeSyncWordEnd, or
 *   \ref RAIL_GetTxTimeFrameEnd to adjust the timestamp for different locations
 *   in the packet.
 * @return \ref RAIL_STATUS_NO_ERROR if pPacketTime was filled in,
 *   or an appropriate error code otherwise.
 *
 * @note Consider using \ref RAIL_GetTxPacketDetailsAlt2 to pass in
 *   a \ref RAIL_PacketTimeStamp_t structure instead of a \ref RAIL_Time_t
 *   structure, particularly when \ref RAIL_PacketTimePosition_t information
 *   is needed or useful.
 *
 * This function can only be called from callback context for either
 * \ref RAIL_EVENT_TX_PACKET_SENT or \ref RAIL_EVENT_TXACK_PACKET_SENT
 * events.
 */
RAIL_Status_t RAIL_GetTxPacketDetailsAlt(RAIL_Handle_t railHandle,
                                         bool isAck,
                                         RAIL_Time_t *pPacketTime);

/**
 * Get detailed information about the last packet transmitted.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in, out] pPacketDetails An application-provided pointer to store
 *   RAIL_TxPacketDetails_t corresponding to the transmit event.
 *   The isAck must be initialized prior to each call:
 *   - isAck true to obtain details about the most recent ACK transmit,
 *     false to obtain details about the most recent app-initiated transmit.
 *   The timeSent field packetTime will be populated with a timestamp corresponding
 *   to a default location in the packet. The timeSent field timePosition will
 *   be populated with a \ref RAIL_PacketTimePosition_t value specifying that
 *   default packet location. Call \ref RAIL_GetTxTimePreambleStartAlt,
 *   \ref RAIL_GetTxTimeSyncWordEndAlt, or \ref RAIL_GetTxTimeFrameEndAlt to
 *   adjust the timestamp for different locations in the packet.
 * @return \ref RAIL_STATUS_NO_ERROR if pPacketDetails was filled in,
 *   or an appropriate error code otherwise.
 *
 * This function can only be called from callback context for either
 * \ref RAIL_EVENT_TX_PACKET_SENT or \ref RAIL_EVENT_TXACK_PACKET_SENT
 * events.
 */
RAIL_Status_t RAIL_GetTxPacketDetailsAlt2(RAIL_Handle_t railHandle,
                                          RAIL_TxPacketDetails_t *pPacketDetails);

/**
 * Adjust a RAIL TX completion timestamp to refer to the start of the
 * preamble. Also used to retrieve the \ref RAIL_EVENT_TX_STARTED
 * timestamp.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] totalPacketBytes The total number of bytes of the transmitted
 *   packet for RAIL to use when adjusting the provided timestamp. This
 *   should account for all bytes transmitted over the air after the Preamble
 *   and Sync word(s), including CRC bytes. Pass \ref RAIL_TX_STARTED_BYTES
 *   to retrieve the start-of-normal-TX timestamp (see below).
 * @param[in, out] pPacketTime This points to the \ref RAIL_Time_t returned
 *   from a previous call to \ref RAIL_GetTxPacketDetailsAlt for this same
 *   packet. That time will be updated with the time that the preamble for
 *   this packet started on air.
 *   Must be non-NULL.
 * @return \ref RAIL_STATUS_NO_ERROR if pPacketTime was successfully
 *   determined or an appropriate error code otherwise.
 *
 * When used for timestamp adjustment, call this function in the
 * same transmit-complete event-handling context as
 * \ref RAIL_GetTxPacketDetailsAlt() is called.
 *
 * This function may be called when handling the \ref RAIL_EVENT_TX_STARTED
 * event to retrieve that event's start-of-normal-TX timestamp. (ACK
 * transmits currently have no equivalent event or associated timestamp.)
 * In this case, totalPacketBytes must be \ref RAIL_TX_STARTED_BYTES, and
 * pPacketTime is an output-only parameter filled in with that time (so no
 * need to initialize it beforehand by calling \ref
 * RAIL_GetTxPacketDetailsAlt()).
 *
 */
RAIL_Status_t RAIL_GetTxTimePreambleStart(RAIL_Handle_t railHandle,
                                          uint16_t totalPacketBytes,
                                          RAIL_Time_t *pPacketTime);

/**
 * Adjust a RAIL TX completion timestamp to refer to the start of the
 * preamble. Also used to retrieve the \ref RAIL_EVENT_TX_STARTED
 * timestamp.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in, out] pPacketDetails The non-NULL details that were returned from
 *   a previous call to \ref RAIL_GetTxPacketDetailsAlt2 for this same packet.
 *   The application must update the timeSent field totalPacketBytes to be
 *   the total number of bytes of the sent packet for RAIL to use when
 *   calculating the specified timestamp. This should account for all bytes
 *   transmitted over the air after the Preamble and Sync word(s), including CRC
 *   bytes. Pass \ref RAIL_TX_STARTED_BYTES to retrieve the start-of-normal-TX
 *   timestamp (see below). After this function, the timeSent field packetTime
 *   will be updated with the time that the preamble for this packet started on air.
 * @return \ref RAIL_STATUS_NO_ERROR if the packet time was successfully
 *   calculated, or an appropriate error code otherwise.
 *
 * When used for timestamp adjustment, call this function in the
 * same transmit-complete event-handling context as
 * \ref RAIL_GetTxPacketDetailsAlt2() is called.
 *
 * This function may be called when handling the \ref RAIL_EVENT_TX_STARTED
 * event to retrieve that event's start-of-normal-TX timestamp. (ACK
 * transmits currently have no equivalent event or associated timestamp.)
 * In this case, the timeSent field totalPacketBytes must be
 * \ref RAIL_TX_STARTED_BYTES, and the timeSent field packetTime is an
 * output-only parameter filled in with that time (so no need to initialize
 * it beforehand by calling \ref RAIL_GetTxPacketDetailsAlt2()).
 *
 */
RAIL_Status_t RAIL_GetTxTimePreambleStartAlt(RAIL_Handle_t railHandle,
                                             RAIL_TxPacketDetails_t *pPacketDetails);

/**
 * Adjust a RAIL TX timestamp to refer to the end of the sync word.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] totalPacketBytes The total number of bytes of the transmitted
 *   packet for RAIL to use when calculating the specified timestamp. This
 *   should account for all bytes transmitted over the air after the Preamble
 *   and Sync word(s), including CRC bytes.
 * @param[in, out] pPacketTime The time that was returned in a
 *   \ref RAIL_Time_t from a previous call to \ref RAIL_GetTxPacketDetailsAlt
 *   for this same packet. After this function, the time at that location will
 *   be updated with the time that the sync word for this packet finished on
 *   air. Must be non-NULL.
 * @return \ref RAIL_STATUS_NO_ERROR if pPacketTime was successfully calculated,
 *   or an appropriate error code otherwise.
 *
 * Call the timestamp adjustment function in the same
 * transmit-complete event-handling context as
 * \ref RAIL_GetTxPacketDetailsAlt() is called.
 */
RAIL_Status_t RAIL_GetTxTimeSyncWordEnd(RAIL_Handle_t railHandle,
                                        uint16_t totalPacketBytes,
                                        RAIL_Time_t *pPacketTime);

/**
 * Adjust a RAIL TX timestamp to refer to the end of the sync word.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in, out] pPacketDetails The non-NULL details that were returned from
 *   a previous call to \ref RAIL_GetTxPacketDetailsAlt2 for this same packet.
 *   The application must update the timeSent field totalPacketBytes to be
 *   the total number of bytes of the sent packet for RAIL to use when
 *   calculating the specified timestamp. This should account for all bytes
 *   transmitted over the air after the Preamble and Sync word(s), including CRC
 *   bytes. Pass \ref RAIL_TX_STARTED_BYTES to retrieve the start-of-normal-TX
 *   timestamp (see below). After this function, the timeSent field packetTime
 *   will be updated with the time that the sync word for this packet finished on
 *   air. Must be non-NULL.
 * @return \ref RAIL_STATUS_NO_ERROR if the packet time was successfully
 *   calculated, or an appropriate error code otherwise.
 *
 * Call the timestamp adjustment function in the same
 * transmit-complete event-handling context as
 * \ref RAIL_GetTxPacketDetailsAlt2() is called.
 */
RAIL_Status_t RAIL_GetTxTimeSyncWordEndAlt(RAIL_Handle_t railHandle,
                                           RAIL_TxPacketDetails_t *pPacketDetails);

/**
 * Adjust a RAIL TX timestamp to refer to the end of frame.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] totalPacketBytes The total number of bytes of the transmitted
 *   packet for RAIL to use when calculating the specified timestamp. This
 *   should account for all bytes transmitted over the air after the Preamble
 *   and Sync word(s), including CRC bytes.
 * @param[in, out] pPacketTime The time that was returned in a
 *   \ref RAIL_Time_t from a previous call to \ref RAIL_GetTxPacketDetailsAlt
 *   for this same packet. After this function, the time at that location will
 *   be updated with the time that this packet finished on air. Must be
 *   non-NULL.
 * @return \ref RAIL_STATUS_NO_ERROR if pPacketTime was successfully calculated,
 *   or an appropriate error code otherwise.
 *
 * Call the timestamp adjustment function in the same
 * transmit-complete event-handling context as
 * \ref RAIL_GetTxPacketDetailsAlt() is called.
 */
RAIL_Status_t RAIL_GetTxTimeFrameEnd(RAIL_Handle_t railHandle,
                                     uint16_t totalPacketBytes,
                                     RAIL_Time_t *pPacketTime);

/**
 * Adjust a RAIL TX timestamp to refer to the end of frame.
 *
 * @param[in] railHandle A RAIL instance handle.

 * @param[in, out] pPacketDetails The non-NULL details that were returned from
 *   a previous call to \ref RAIL_GetTxPacketDetailsAlt2 for this same packet.
 *   The application must update the timeSent field totalPacketBytes to be
 *   the total number of bytes of the sent packet for RAIL to use when
 *   calculating the specified timestamp. This should account for all bytes
 *   transmitted over the air after the Preamble and Sync word(s), including CRC
 *   bytes. Pass \ref RAIL_TX_STARTED_BYTES to retrieve the start-of-normal-TX
 *   timestamp (see below). After this function, the timeSent field packetTime
 *   will be updated with the time that this packet finished on air. Must be
 *   non-NULL.
 * @return \ref RAIL_STATUS_NO_ERROR if the packet time was successfully
 *   calculated, or an appropriate error code otherwise.
 *
 * Call the timestamp adjustment function in the same
 * transmit-complete event-handling context as
 * \ref RAIL_GetTxPacketDetailsAlt2() is called.
 */
RAIL_Status_t RAIL_GetTxTimeFrameEndAlt(RAIL_Handle_t railHandle,
                                        RAIL_TxPacketDetails_t *pPacketDetails);

/**
 * Prevent the radio from starting a transmit.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] enable Enable/Disable TX hold off.
 *
 * Enable TX hold off to prevent the radio from starting any transmits.
 * Disable TX hold off to allow the radio to transmit again.
 * Attempting to transmit with the TX hold off enabled will result in
 * \ref RAIL_EVENT_TX_BLOCKED and/or \ref RAIL_EVENT_TXACK_BLOCKED
 * events.
 *
 * @note This function does not affect a transmit that has already started.
 *   To stop an already-started transmission, use RAIL_Idle() with
 *   \ref RAIL_IDLE_ABORT.
 */
void RAIL_EnableTxHoldOff(RAIL_Handle_t railHandle, bool enable);

/**
 * Check whether or not TX hold off is enabled.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return Returns true if TX hold off is enabled, false otherwise.
 *
 * TX hold off can be enabled/disabled using \ref RAIL_EnableTxHoldOff.
 * Attempting to transmit with the TX hold off enabled will block the
 * transmission and result in \ref RAIL_EVENT_TX_BLOCKED
 * and/or \ref RAIL_EVENT_TXACK_BLOCKED events.
 */
bool RAIL_IsTxHoldOffEnabled(RAIL_Handle_t railHandle);

/**
 * Set an alternate transmitter preamble length.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] length The desired preamble length, in bits.
 * @return Status code indicating success of the function call.
 *
 * @note Attempting to set a preamble length of 0xFFFF bits will result in
 * \ref RAIL_STATUS_INVALID_PARAMETER.
 **/
RAIL_Status_t RAIL_SetTxAltPreambleLength(RAIL_Handle_t railHandle, uint16_t length);

/** @} */ // end of group Transmit

/******************************************************************************
 * Receive
 *****************************************************************************/
/**
 * @addtogroup Receive
 * @brief APIs related to packet receive
 * @{
 */

/**
 * Configure receive options.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] mask A bitmask containing which options should be modified.
 * @param[in] options A bitmask containing desired configuration settings.
 *   Bit positions for each option are found in the \ref RAIL_RxOptions_t.
 * @return Status code indicating success of the function call.
 *
 * Configure the radio receive flow based on the list of available options.
 * Only the options indicated by the mask parameter will be affected. Pass
 * \ref RAIL_RX_OPTIONS_ALL to set all parameters.
 * The previous settings may affect the current frame if a packet is
 * received during this configuration.
 */
RAIL_Status_t RAIL_ConfigRxOptions(RAIL_Handle_t railHandle,
                                   RAIL_RxOptions_t mask,
                                   RAIL_RxOptions_t options);

/**
 * Include the code necessary for frame type based length decoding.
 *
 * @param[in] railHandle A RAIL instance handle.
 *
 * This function must be called before \ref RAIL_ConfigChannels to allow configurations
 * using a frame type based length setup. In RAIL 2.x, it is called by default
 * in the \ref RAILCb_ConfigFrameTypeLength API which can be overridden to save
 * code space. In future versions, the user may be required to call this API
 * explicitly.
 */
void RAIL_IncludeFrameTypeLength(RAIL_Handle_t railHandle);

/**
 * Handle frame type length.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] frameType A frame type configuration structure.
 *
 * This function is implemented in the radio configuration.
 * Currently, the frame type passed in only handles packet length decoding. If
 * NULL is passed into this function, it clears any currently configured
 * frame type settings. This will either be implemented as an empty function in
 * the radio configuration if it is not needed, to assist in dead code
 * elimination.
 */
void RAILCb_ConfigFrameTypeLength(RAIL_Handle_t railHandle,
                                  const RAIL_FrameType_t *frameType);

/**
 * Start the receiver on a specific channel.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] channel The channel to listen on.
 * @param[in] schedulerInfo Information to allow the radio scheduler to place
 *   this receive appropriately. This is only used in multiprotocol version of
 *   RAIL and may be set to NULL in all other versions.
 * @return Status code indicating success of the function call.
 *
 * This is a non-blocking function. Whenever a packet is received, \ref RAIL_Config_t::eventsCallback
 * will fire with \ref RAIL_EVENT_RX_PACKET_RECEIVED set. If you call
 * this while not idle but with a different channel, any ongoing
 * receive or transmit operation will be aborted.
 */
RAIL_Status_t RAIL_StartRx(RAIL_Handle_t railHandle,
                           uint16_t channel,
                           const RAIL_SchedulerInfo_t *schedulerInfo);

/**
 * Schedule a receive window for some future time.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] channel A channel to listen on.
 * @param[in] cfg The configuration structure to define the receive window.
 * @param[in] schedulerInfo Information to allow the radio scheduler to place
 *   this receive appropriately. This is only used in multiprotocol version of
 *   RAIL and may be set to NULL in all other versions.
 * @return Status code indicating success of the function call.
 *
 * This API immediately changes the channel and schedules receive to start
 * at the specified time and end at the given end time. If you do not specify
 * an end time, you may call this API later with an end time as long as you set
 * the start time to disabled. You can also terminate the receive
 * operation immediately using the RAIL_Idle() function. Note that relative
 * end times are always relative to the start unless no start time is
 * specified. If changing channels, the channel is changed immediately and
 * will abort any ongoing packet transmission or reception.
 *
 * Returns an error if a CSMA or LBT transmit is still in progress.
 *
 * In multiprotocol, ensure that you properly yield the radio after this
 * call. See \ref rail_radio_scheduler_yield for more details.
 */
RAIL_Status_t RAIL_ScheduleRx(RAIL_Handle_t railHandle,
                              uint16_t channel,
                              const RAIL_ScheduleRxConfig_t *cfg,
                              const RAIL_SchedulerInfo_t *schedulerInfo);

/******************************************************************************
 * Packet Information (RX)
 *****************************************************************************/
/// @addtogroup Packet_Information Packet Information
/// @brief APIs to get information about received packets.
///
/// After receiving a packet, RAIL will trigger a
/// \ref RAIL_EVENT_RX_PACKET_RECEIVED event. At that point, there is a variety
/// of information available to the application about the received packet. The
/// following example code assumes that the
/// \ref RAIL_RX_OPTION_REMOVE_APPENDED_INFO is not used, and the application
/// wants as much data about the packet as possible.
///
/// @code{.c}
/// // Get all information about a received packet.
/// RAIL_Status_t status;
/// RAIL_RxPacketInfo_t rxInfo;
/// RAIL_RxPacketDetails_t rxDetails;
/// RAIL_RxPacketHandle_t rxHandle
///   = RAIL_GetRxPacketInfo(railHandle, RAIL_RX_PACKET_HANDLE_NEWEST, &rxInfo);
/// assert(rxHandle != RAIL_RX_PACKET_HANDLE_INVALID);
/// status = RAIL_GetRxPacketDetailsAlt(railHandle, rxHandle, &rxDetails);
/// assert(status == RAIL_STATUS_NO_ERROR);
/// if (rxDetails.timeReceived.timePosition == RAIL_PACKET_TIME_INVALID) {
///   return; // No timestamp available for this packet
/// }
/// // CRC_BYTES only needs to be added when not using RAIL_RX_OPTION_STORE_CRC
/// rxDetails.timeReceived.totalPacketBytes = rxInfo.packetBytes + CRC_BYTES;
/// // Choose the function which gives the desired timestamp
/// status = RAIL_GetRxTimeFrameEndAlt(railHandle, &rxDetails);
/// assert(status == RAIL_STATUS_NO_ERROR);
/// // Now all fields of rxInfo and rxDetails have been populated correctly
/// @endcode
///
/// @{

/**
 * Get basic information about a pending or received packet.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] packetHandle A packet handle for the unreleased packet as
 *   returned from a previous call, or sentinel values
 *   \ref RAIL_RX_PACKET_HANDLE_OLDEST,
 *   \ref RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE or
 *   \ref RAIL_RX_PACKET_HANDLE_NEWEST.
 * @param[out] pPacketInfo An application-provided pointer to store
 *   \ref RAIL_RxPacketInfo_t for the requested packet. Must be non-NULL.
 * @return The packet handle for the requested packet:
 *   if packetHandle was one of the sentinel values, returns the actual
 *   packet handle for that packet, otherwise returns packetHandle.
 *   It may return \ref RAIL_RX_PACKET_HANDLE_INVALID to indicate an error.
 *
 * This function can be used in any RX mode. It does not free up any
 * internal resources. If used in RX \ref RAIL_DataMethod_t::FIFO_MODE, the
 * value in \ref RAIL_RxPacketInfo_t::packetBytes will only return the data
 * remaining in the FIFO. Any data read via earlier earlier calls to
 * \ref RAIL_ReadRxFifo() is not included.
 *
 * @note When getting information about an arriving packet that is not yet complete,
 *   (i.e., pPacketInfo->packetStatus == \ref RAIL_RX_PACKET_RECEIVING), keep
 *   in mind its data is highly suspect because it has not yet passed any CRC
 *   integrity checking. Also note that the packet could be aborted, canceled, or
 *   fail momentarily, invalidating its data in Packet mode. Furthermore, there
 *   is a small chance towards the end of packet reception that the filled-in
 *   RAIL_RxPacketInfo_t could include not only packet data received so far,
 *   but also some raw radio-appended info detail bytes that RAIL's
 *   packet-completion processing will subsequently deal with. It's up to the
 *   application to know its packet format well enough to avoid confusing such
 *   info as packet data.
 */
RAIL_RxPacketHandle_t RAIL_GetRxPacketInfo(RAIL_Handle_t railHandle,
                                           RAIL_RxPacketHandle_t packetHandle,
                                           RAIL_RxPacketInfo_t *pPacketInfo);

/**
 * Get information about the live incoming packet (if any).
 * Differs from \ref RAIL_GetRxPacketInfo() by only returning information
 * about a packet actively being received, something which even the
 * \ref RAIL_RX_PACKET_HANDLE_NEWEST may not represent if there are
 * completed but unprocessed packets in the receive FIFO.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[out] pPacketInfo Application provided pointer to store
 * RAIL_RxPacketInfo_t for the incoming packet.
 *
 * This function can only be called from callback context, e.g.,
 * when handling \ref RAIL_EVENT_RX_FILTER_PASSED or
 * \ref RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND.
 * It must not be used with receive \ref RAIL_DataMethod_t::FIFO_MODE
 * if any portion of an incoming packet has already been extracted from
 * the receive FIFO.
 *
 * @note The incomplete data of an arriving packet is highly suspect because
 *   it has not yet passed any CRC integrity checking. Also note that the
 *   packet could be aborted, canceled, or fail momentarily, invalidating
 *   its data in Packet mode. Furthermore, there is a small chance towards
 *   the end of packet reception that the filled-in RAIL_RxPacketInfo_t
 *   could include not only packet data received so far, but also some raw
 *   radio-appended info detail bytes that RAIL's packet-completion
 *   processing will subsequently deal with. It's up to the application to
 *   know its packet format well enough to avoid confusing such info as
 *   packet data.
 */
void RAIL_GetRxIncomingPacketInfo(RAIL_Handle_t railHandle,
                                  RAIL_RxPacketInfo_t *pPacketInfo);

/**
 * Copy a full packet to a user-specified contiguous buffer.
 *
 * @param[out] pDest An application-provided pointer to a buffer of at
 *   least pPacketInfo->packetBytes in size to store the packet data
 *   contiguously. This buffer must never overlay RAIL's receive FIFO buffer.
 *   Exactly pPacketInfo->packetBytes of packet data will be written into it.
 * @param[in] pPacketInfo
 *   \ref RAIL_RxPacketInfo_t for the requested packet.
 *
 * @note This is a convenience helper function, which
 * is intended to be expedient. As a result, it does not
 *   check the validity of its arguments,
 *   so don't pass either as NULL, and don't
 *   pass a pDest pointer to a buffer that's too small for the packet's data.
 * @note If only a portion of the packet is needed, use RAIL_PeekRxPacket()
 *   instead.
 */
static inline
void RAIL_CopyRxPacket(uint8_t *pDest,
                       const RAIL_RxPacketInfo_t *pPacketInfo)
{
  (void)memcpy(pDest, pPacketInfo->firstPortionData, pPacketInfo->firstPortionBytes);
  if (pPacketInfo->lastPortionData != NULL) {
    uint16_t size = pPacketInfo->packetBytes - pPacketInfo->firstPortionBytes;
    (void)memcpy(pDest + pPacketInfo->firstPortionBytes,
                 pPacketInfo->lastPortionData, size);
  }
}

/**
 * Get detailed information about a received packet.
 * This function can be used in any RX mode; it does not free up any
 * internal resources.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] packetHandle A packet handle for the unreleased packet as
 *   returned from a previous call to RAIL_GetRxPacketInfo() or
 *   RAIL_HoldRxPacket(), or sentinel values \ref RAIL_RX_PACKET_HANDLE_OLDEST,
 *   \ref RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE
 *   or \ref RAIL_RX_PACKET_HANDLE_NEWEST.
 * @param[in,out] pPacketDetails An application-provided non-NULL pointer to
 *   store \ref RAIL_RxPacketDetails_t for the requested packet.
 *   For \ref RAIL_RxPacketStatus_t RAIL_RX_PACKET_READY_ packets,
 *   the timeReceived fields totalPacketBytes and timePosition must be
 *   initialized prior to each call:
 *   - totalPacketBytes with the total number of bytes of the received
 *     packet for RAIL to use when calculating the specified timestamp.
 *     This should account for all bytes received over the air after the
 *     Preamble and Sync word(s), including CRC bytes.
 *   - timePosition with a \ref RAIL_PacketTimePosition_t value specifying
 *     the packet position to put in the timeReceived field on return.
 *     This field will also be updated with the actual position corresponding
 *     to the timeReceived value filled in.
 * @return \ref RAIL_STATUS_NO_ERROR if pPacketDetails was filled in,
 *   or an appropriate error code otherwise.
 *
 * @note Certain details are always available, while others are only available
 *   if the \ref RAIL_RxOptions_t \ref RAIL_RX_OPTION_REMOVE_APPENDED_INFO
 *   option is not in effect and the received packet's
 *   \ref RAIL_RxPacketStatus_t is among the RAIL_RX_PACKET_READY_ set.
 *   See \ref RAIL_RxPacketDetails_t for clarification.
 *
 * @note Consider using \ref RAIL_GetRxPacketDetailsAlt for smaller code size.
 */
RAIL_Status_t RAIL_GetRxPacketDetails(RAIL_Handle_t railHandle,
                                      RAIL_RxPacketHandle_t packetHandle,
                                      RAIL_RxPacketDetails_t *pPacketDetails);

/**
 * Get detailed information about a received packet.
 * This function can be used in any RX mode. It does not free up any
 * internal resources.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] packetHandle A packet handle for the unreleased packet as
 *   returned from a previous call to RAIL_GetRxPacketInfo() or
 *   RAIL_HoldRxPacket(), or sentinel values \ref RAIL_RX_PACKET_HANDLE_OLDEST
 *   \ref RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE or
 *   \ref RAIL_RX_PACKET_HANDLE_NEWEST.
 * @param[out] pPacketDetails An application-provided non-NULL pointer to
 *   store \ref RAIL_RxPacketDetails_t for the requested packet.
 *   For \ref RAIL_RxPacketStatus_t RAIL_RX_PACKET_READY_ packets,
 *   the timeReceived field packetTime will be populated with a timestamp
 *   corresponding to a default location in the packet. The timeReceived field
 *   timePosition will be populated with a \ref RAIL_PacketTimePosition_t value
 *   specifying that default packet location. Call
 *   \ref RAIL_GetRxTimePreambleStart, \ref RAIL_GetRxTimeSyncWordEnd, or
 *   \ref RAIL_GetRxTimeFrameEnd to adjust that timestamp for different
 *   locations in the packet.
 * @return \ref RAIL_STATUS_NO_ERROR if pPacketDetails was filled in,
 *   or an appropriate error code otherwise.
 *
 * This alternative API allows for smaller code size by deadstripping the
 * timestamp adjustment algorithms which are not in use.
 *
 * @note Certain details are always available, while others are only available
 *   if the \ref RAIL_RxOptions_t \ref RAIL_RX_OPTION_REMOVE_APPENDED_INFO
 *   option is not in effect and the received packet's
 *   \ref RAIL_RxPacketStatus_t is among the RAIL_RX_PACKET_READY_ set.
 *   See \ref RAIL_RxPacketDetails_t for clarification.
 */
RAIL_Status_t RAIL_GetRxPacketDetailsAlt(RAIL_Handle_t railHandle,
                                         RAIL_RxPacketHandle_t packetHandle,
                                         RAIL_RxPacketDetails_t *pPacketDetails);

/**
 * Adjust a RAIL RX timestamp to refer to the start of the preamble.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] totalPacketBytes The total number of bytes of the received packet
 *   for RAIL to use when calculating the specified timestamp. This should
 *   account for all bytes received over the air after the Preamble and Sync
 *   word(s), including CRC bytes.
 * @param[in, out] pPacketTime The time that was returned in the
 *   \ref RAIL_PacketTimeStamp_t::packetTime field of
 *   \ref RAIL_RxPacketDetails_t::timeReceived from a previous call to
 *   \ref RAIL_GetRxPacketDetailsAlt for this same packet. After this
 *   function, the time at that location will be updated with the time that the
 *   preamble for this packet started on air. Must be non-NULL.
 * @return \ref RAIL_STATUS_NO_ERROR if pPacketTime was successfully calculated,
 *   or an appropriate error code otherwise.
 *
 * Call this API while the given railHandle is active, or it will
 * return an error code of \ref RAIL_STATUS_INVALID_STATE. Note that this API
 * may return incorrect timestamps when sub-phys are in use. Prefer
 * \ref RAIL_GetRxTimePreambleStartAlt in those situations. See
 * \ref RAIL_RxPacketDetails_t::subPhyId for more details.
 */
RAIL_Status_t RAIL_GetRxTimePreambleStart(RAIL_Handle_t railHandle,
                                          uint16_t totalPacketBytes,
                                          RAIL_Time_t *pPacketTime);

/**
 * Adjust a RAIL RX timestamp to refer to the start of the preamble.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in, out] pPacketDetails The non-NULL details that were returned from
 *   a previous call to \ref RAIL_GetRxPacketDetailsAlt for this same packet.
 *   The application must update the timeReceived field totalPacketBytes to be
 *   the total number of bytes of the received packet for RAIL to use when
 *   calculating the specified timestamp. This should account for all bytes
 *   received over the air after the Preamble and Sync word(s), including CRC
 *   bytes. After this function, the timeReceived field packetTime will be
 *   updated with the time that the preamble for this packet started on air.
 * @return \ref RAIL_STATUS_NO_ERROR if the packet time was successfully
 *   calculated, or an appropriate error code otherwise.
 *
 * Call this API while the given railHandle is active, or it will
 * return an error code of \ref RAIL_STATUS_INVALID_STATE.
 */
RAIL_Status_t RAIL_GetRxTimePreambleStartAlt(RAIL_Handle_t railHandle,
                                             RAIL_RxPacketDetails_t *pPacketDetails);

/**
 * Adjust a RAIL RX timestamp to refer to the end of the sync word.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] totalPacketBytes The total number of bytes of the received packet
 *   for RAIL to use when calculating the specified timestamp. This should
 *   account for all bytes received over the air after the Preamble and Sync
 *   word(s), including CRC bytes.
 * @param[in, out] pPacketTime The time that was returned in the
 *   \ref RAIL_PacketTimeStamp_t::packetTime field of
 *   \ref RAIL_RxPacketDetails_t::timeReceived from a previous call to
 *   \ref RAIL_GetRxPacketDetailsAlt for this same packet. After this
 *   function, the time at that location will be updated with the time that the
 *   sync word for this packet finished on air. Must be non-NULL.
 * @return \ref RAIL_STATUS_NO_ERROR if pPacketTime was successfully calculated,
 *   or an appropriate error code otherwise.
 *
 * Call this API while the given railHandle is active, or it will
 * return an error code of \ref RAIL_STATUS_INVALID_STATE. Note that this API
 * may return incorrect timestamps when sub-phys are in use. Prefer
 * \ref RAIL_GetRxTimePreambleStartAlt in those situations. See
 * \ref RAIL_RxPacketDetails_t::subPhyId for more details.
 */
RAIL_Status_t RAIL_GetRxTimeSyncWordEnd(RAIL_Handle_t railHandle,
                                        uint16_t totalPacketBytes,
                                        RAIL_Time_t *pPacketTime);

/**
 * Adjust a RAIL RX timestamp to refer to the end of the sync word.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in, out] pPacketDetails The non-NULL details that were returned from
 *   a previous call to \ref RAIL_GetRxPacketDetailsAlt for this same packet.
 *   The application must update the timeReceived field totalPacketBytes to be
 *   the total number of bytes of the received packet for RAIL to use when
 *   calculating the specified timestamp. This should account for all bytes
 *   received over the air after the Preamble and Sync word(s), including CRC
 *   bytes. After this function, the timeReceived field packetTime will be
 *   updated with the time that the sync word for this packet finished on air.
 * @return \ref RAIL_STATUS_NO_ERROR if the packet time was successfully
 *   calculated, or an appropriate error code otherwise.
 *
 * Call this API while the given railHandle is active, or it will
 * return an error code of \ref RAIL_STATUS_INVALID_STATE.
 */
RAIL_Status_t RAIL_GetRxTimeSyncWordEndAlt(RAIL_Handle_t railHandle,
                                           RAIL_RxPacketDetails_t *pPacketDetails);

/**
 * Adjust a RAIL RX timestamp to refer to the end of frame.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] totalPacketBytes The total number of bytes of the received packet
 *   for RAIL to use when calculating the specified timestamp. This should
 *   account for all bytes received over the air after the Preamble and Sync
 *   word(s), including CRC bytes.
 * @param[in, out] pPacketTime The time that was returned in the
 *   \ref RAIL_PacketTimeStamp_t::packetTime field of
 *   \ref RAIL_RxPacketDetails_t::timeReceived from a previous call to
 *   \ref RAIL_GetRxPacketDetailsAlt for this same packet. After this
 *   function, the time at that location will be updated with the time that this
 *   packet finished on air. Must be non-NULL.
 * @return \ref RAIL_STATUS_NO_ERROR if pPacketTime was successfully calculated,
 *   or an appropriate error code otherwise.
 *
 * Call this API while the given railHandle is active, or it will
 * return an error code of \ref RAIL_STATUS_INVALID_STATE. Note that this API
 * may return incorrect timestamps when sub-phys are in use. Prefer
 * \ref RAIL_GetRxTimePreambleStartAlt in those situations. See
 * \ref RAIL_RxPacketDetails_t::subPhyId for more details.
 */
RAIL_Status_t RAIL_GetRxTimeFrameEnd(RAIL_Handle_t railHandle,
                                     uint16_t totalPacketBytes,
                                     RAIL_Time_t *pPacketTime);

/**
 * Adjust a RAIL RX timestamp to refer to the end of frame.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in, out] pPacketDetails The non-NULL details that were returned from
 *   a previous call to \ref RAIL_GetRxPacketDetailsAlt for this same packet.
 *   The application must update the timeReceived field totalPacketBytes to be
 *   the total number of bytes of the received packet for RAIL to use when
 *   calculating the specified timestamp. This should account for all bytes
 *   received over the air after the Preamble and Sync word(s), including CRC
 *   bytes. After this function, the timeReceived field packetTime will be
 *   updated with the time that the packet finished on air.
 * @return \ref RAIL_STATUS_NO_ERROR if the packet time was successfully
 *   calculated, or an appropriate error code otherwise.
 *
 * Call this API while the given railHandle is active, or it will
 * return an error code of \ref RAIL_STATUS_INVALID_STATE.
 */
RAIL_Status_t RAIL_GetRxTimeFrameEndAlt(RAIL_Handle_t railHandle,
                                        RAIL_RxPacketDetails_t *pPacketDetails);

/** @} */ // end of group Packet_Information

/**
 * Place a temporary hold on this packet's data and information resources
 * within RAIL.
 * This function can only be called from within RAIL callback context.
 * It can be used in any RX mode.
 *
 * Normally, when RAIL issues its callback indicating a packet is ready
 * or aborted, it expects the application's callback to retrieve and
 * copy (or discard) the packet's information and data, and will free up
 * its internal packet data after the callback returns. This function
 * tells RAIL to hold onto those resources after the callback returns in
 * case the application wants to defer processing the packet to a later
 * time, e.g., outside of callback context.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return The packet handle for the packet associated with the callback,
 *   \ref RAIL_RX_PACKET_HANDLE_INVALID if no such packet yet exists or
 *   railHandle is not active.
 *
 * @note When using multiprotocol the receive FIFO is reset during protocol
 * switches so any packets held with \ref RAIL_HoldRxPacket() will be lost. It
 * is best to avoid using this in DMP or to at least reset any internal held
 * packet information when the \ref RAIL_EVENT_CONFIG_UNSCHEDULED occurs.
 */
RAIL_RxPacketHandle_t RAIL_HoldRxPacket(RAIL_Handle_t railHandle);

/**
 * Copy 'len' bytes of packet data starting from 'offset' from the
 * receive FIFO. Those bytes remain valid for re-peeking.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] packetHandle A packet handle as returned from a previous
 *   RAIL_GetRxPacketInfo() or RAIL_HoldRxPacket() call, or
 *   sentinel values \ref RAIL_RX_PACKET_HANDLE_OLDEST,
 *   \ref RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE
 *   or \ref RAIL_RX_PACKET_HANDLE_NEWEST.
 * @param[out] pDst A pointer to the location where the received bytes will
 *   be copied. If NULL, no copying occurs.
 * @param[in] len A number of packet data bytes to copy.
 * @param[in] offset A byte offset within remaining packet data from which
 *   to copy.
 * @return Number of packet bytes copied.
 *
 * @note Peek does not permit peeking beyond the requested packet's
 *   available packet data (though there is a small chance it might
 *   for a \ref RAIL_RX_PACKET_HANDLE_NEWEST packet at the very end of
 *   still being received). Nor can one peek into already-consumed data read
 *   by RAIL_ReadRxFifo(). len and offset are relative to the remaining data
 *   available in the packet, if any was already consumed by RAIL_ReadRxFifo().
 */
uint16_t RAIL_PeekRxPacket(RAIL_Handle_t railHandle,
                           RAIL_RxPacketHandle_t packetHandle,
                           uint8_t *pDst,
                           uint16_t len,
                           uint16_t offset);

/**
 * Release RAIL's internal resources for the packet.
 * This function must be called for any packet previously held via
 * RAIL_HoldRxPacket(). It may optionally be called within a
 * callback context to release RAIL resources sooner than at
 * callback completion time when not holding the packet.
 * This function can be used in any RX mode.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] packetHandle A packet handle as returned from a previous
 *   RAIL_HoldRxPacket() call, or sentinel values
 *   \ref RAIL_RX_PACKET_HANDLE_OLDEST,
 *   \ref RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE
 *   or \ref RAIL_RX_PACKET_HANDLE_NEWEST.
 *   The latter might be used within RAIL callback context to explicitly
 *   release the packet associated with the callback early, before it's
 *   released automatically by RAIL on callback return (unless explicitly
 *   held).
 * @return \ref RAIL_STATUS_NO_ERROR if the held packet was released
 *   or an appropriate error code otherwise.
 */
RAIL_Status_t RAIL_ReleaseRxPacket(RAIL_Handle_t railHandle,
                                   RAIL_RxPacketHandle_t packetHandle);

/**
 * Return the current raw RSSI.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] wait if false returns instant RSSI with no checks.
 * @return \ref RAIL_RSSI_INVALID if the receiver is disabled and an RSSI
 *   value can't be obtained. Otherwise, return the RSSI in quarter dBm, dbm*4.
 *
 * Gets the current RSSI value. This value represents the current energy of the
 * channel. It can change rapidly and will be low if no RF energy is
 * in the current channel. The function from the value reported to dBm is an
 * offset dependent on the PHY and the PCB layout. Characterize the
 * RSSI received on your hardware and apply an offset in the application to
 * account for board and PHY parameters. When 'wait' is false, the radio needs
 * to be currently in RX and have been in there for a sufficient amount of time
 * for a fresh RSSI value to be read and returned. Otherwise, the RSSI is
 * considered stale and \ref RAIL_RSSI_INVALID is returned instead. When 'wait'
 * is true, if the radio is transitioning to or already in RX, this function
 * will wait for a valid RSSI to be read and return it. Otherwise, if the radio
 * is in or transitions to IDLE or TX, \ref RAIL_RSSI_INVALID will be returned.
 * On low datarate PHYs, this function can take a significantly longer time when
 * wait is true.
 *
 * In multiprotocol, this function returns \ref RAIL_RSSI_INVALID
 * immediately if railHandle is not the current active \ref RAIL_Handle_t.
 * Additionally, 'wait' should never be set 'true' in multiprotocol
 * as the wait time is not consistent, so scheduling a scheduler
 * slot cannot be done accurately. Rather if waiting for a valid RSSI is
 * desired, use \ref RAIL_GetRssiAlt instead to apply a bounded time period.
 *
 * @note If RX Antenna Diversity is enabled via \ref RAIL_ConfigRxOptions(),
 *   pass true for the wait parameter otherwise it's very likely
 *   \ref RAIL_RSSI_INVALID will be returned.
 *
 * @note If RX channel hopping is turned on, do not use this API.
 *   Instead, see RAIL_GetChannelHoppingRssi().
 *
 * @note When 'wait' is false, this API is equivalent to \ref RAIL_GetRssiAlt
 *   with 'waitTimeout' set to \ref RAIL_GET_RSSI_NO_WAIT. When 'wait' is
 *   true, this API is equivalent to \ref RAIL_GetRssiAlt with 'waitTimeout'
 *   set to \ref RAIL_GET_RSSI_WAIT_WITHOUT_TIMEOUT. Consider using
 *   \ref RAIL_GetRssiAlt if a bounded maximum wait timeout is desired.
 */
int16_t RAIL_GetRssi(RAIL_Handle_t railHandle, bool wait);

/**
 * Return the current raw RSSI within a definitive time period.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] waitTimeout Sets the maximum time to wait for a valid RSSI.
 *   If equal to \ref RAIL_GET_RSSI_NO_WAIT returns instant RSSI with no checks.
 *   If equal to \ref RAIL_GET_RSSI_WAIT_WITHOUT_TIMEOUT waits for a valid RSSI
 *   with no maximum timeout.
 * @return \ref RAIL_RSSI_INVALID if the receiver is disabled and an RSSI
 *   value can't be obtained. Otherwise, return the RSSI in quarter dBm, dbm*4.
 *
 * Gets the current RSSI value. This value represents the current energy of the
 * channel. It can change rapidly, and will be low if no RF energy is
 * in the current channel. The function from the value reported to dBm is an
 * offset dependent on the PHY and the PCB layout. Characterize the
 * RSSI received on your hardware and apply an offset in the application to
 * account for board and PHY parameters. If a value of \ref RAIL_GET_RSSI_NO_WAIT
 * is given for waitTimeout, the radio needs to be currently in RX and have been
 * in there for a sufficient amount of time for a fresh RSSI value to be read and
 * returned. Otherwise the RSSI is considered stale and \ref RAIL_RSSI_INVALID is
 * returned instead. For non-zero values of waitTimeout, if the radio is
 * transitioning to or already in RX, this function will wait a maximum time equal
 * to waitTimeout (or indefinitely if waitTimeout is set to
 * \ref RAIL_GET_RSSI_WAIT_WITHOUT_TIMEOUT) for a valid RSSI to be read and return
 * it. Otherwise, if the waitTimeout is reached, or the radio is in or transitions
 * to IDLE or TX, \ref RAIL_RSSI_INVALID will be returned. On low datarate PHYs,
 * this function can take a significantly longer time when waitTimeout is non-zero.
 *
 * In multiprotocol, this function returns \ref RAIL_RSSI_INVALID
 * immediately if railHandle is not the current active \ref RAIL_Handle_t.
 * Additionally, 'waitTimeout' should never be set to a value other than
 * \ref RAIL_GET_RSSI_NO_WAIT in multiprotocol as the integration between this
 * feature and the radio scheduler has not been implemented.
 *
 * @note If RX Antenna Diversity is enabled via \ref RAIL_ConfigRxOptions(),
 *   pass true for the wait parameter otherwise it's very likely
 *   \ref RAIL_RSSI_INVALID will be returned.
 *
 * @note If RX channel hopping is turned on, do not use this API.
 *   Instead, see RAIL_GetChannelHoppingRssi().
 */
int16_t RAIL_GetRssiAlt(RAIL_Handle_t railHandle, RAIL_Time_t waitTimeout);

/**
 * Start the RSSI averaging over a specified time in us.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] channel The physical channel to set.
 * @param[in] averagingTimeUs Averaging time in microseconds.
 * @param[in] schedulerInfo Information to allow the radio scheduler to place
 *   this operation appropriately. This is only used in multiprotocol version of
 *   RAIL and may be set to NULL in all other versions.
 * @return Status code indicating success of the function call.
 *
 * Starts a non-blocking hardware-based RSSI averaging mechanism. Only a single
 * instance of RSSI averaging can be run at any time and the radio must be idle
 * to start.
 *
 * In multiprotocol, this is a scheduled event. It will start when railHandle
 * becomes active. railHandle needs to stay active until the averaging
 * completes. If the averaging is interrupted, calls to
 * \ref RAIL_GetAverageRssi will return \ref RAIL_RSSI_INVALID.
 *
 * Also in multiprotocol, the user is required to call \ref RAIL_YieldRadio
 * after this event completes (i.e., when \ref RAIL_EVENT_RSSI_AVERAGE_DONE
 * occurs).
 *
 * @note If the radio is idled while RSSI averaging is still in effect, a
 *   \ref RAIL_EVENT_RSSI_AVERAGE_DONE event may not occur and
 *   \ref RAIL_IsAverageRssiReady may never return true.
 */
RAIL_Status_t RAIL_StartAverageRssi(RAIL_Handle_t railHandle,
                                    uint16_t channel,
                                    RAIL_Time_t averagingTimeUs,
                                    const RAIL_SchedulerInfo_t *schedulerInfo);

/**
 * Query whether the RSSI averaging is done.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return Returns true if done and false otherwise.
 *
 * This function can be used to poll for completion of the RSSI averaging
 * to avoid relying on an interrupt-based callback.
 *
 * @note If the radio is idled while RSSI averaging is still in effect,
 *   this function may never return true.
 */
bool RAIL_IsAverageRssiReady(RAIL_Handle_t railHandle);

/**
 * Get the RSSI averaged over a specified time in us.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return Return \ref RAIL_RSSI_INVALID if the receiver is disabled
 *   an RSSI value can't be obtained. Otherwise, return the RSSI in
 *   quarter dBm,dbm*4.
 *
 * Gets the hardware RSSI average after issuing RAIL_StartAverageRssi.
 * Use after \ref RAIL_StartAverageRssi.
 */
int16_t RAIL_GetAverageRssi(RAIL_Handle_t railHandle);

/**
 * Set the RSSI offset.
 *
 * @param[in] railHandle a RAIL instance handle.
 * @param[in] rssiOffset desired offset to be added to the RSSI measurements.
 * @return Status code indicating success of the function call.
 *   \ref RAIL_STATUS_INVALID_CALL if called with chip-specific handle, such
 *   as \ref RAIL_EFR32_HANDLE, after RAIL initialization.
 *   \ref RAIL_STATUS_INVALID_PARAMETER if the RSSI offset is deemed large
 *   enough to cause the RSSI readings to underflow or overflow.
 *
 * Adds an offset to the RSSI in dBm. This offset affects all functionality that
 * depends on RSSI values, such as CCA functions. Do not modify the offset
 * dynamically during packet reception. This function
 * can only be called while the radio is off, or in the case of multiprotocol,
 * on an inactive protocol.
 *
 * @note: If RAIL has not been initialized, a chip-specific handle,
 *  such as \ref RAIL_EFR32_HANDLE, can be used to set a chip level RSSI offset.
 *
 * @note: Setting a large rssiOffset may still cause the RSSI readings to
 *  underflow. If that happens, the RSSI value returned by
 *  \ref RAIL_GetRssi, \ref RAIL_GetAverageRssi,
 *  \ref RAIL_GetChannelHoppingRssi etc. will be \ref RAIL_RSSI_LOWEST
 */
RAIL_Status_t RAIL_SetRssiOffset(RAIL_Handle_t railHandle, int8_t rssiOffset);

/**
 * Get the RSSI offset.
 *
 * @param[in] railHandle a RAIL instance handle.
 * @return rssiOffset in dBm corresponding to the current handle.
 *
 * @note: A chip-specific handle, such as \ref RAIL_EFR32_HANDLE, can be used to
 * get the chip level RSSI offset otherwise this will return the RSSI offset
 * value associated with the RAIL instance handle, exclusive of any chip level
 * RSSI offset correction, if any.
 */
int8_t RAIL_GetRssiOffset(RAIL_Handle_t railHandle);

/**
 * Set the RSSI detection threshold(in dBm) to trigger
 * \ref RAIL_EVENT_DETECT_RSSI_THRESHOLD.
 *
 * @param[in] railHandle a RAIL instance handle.
 * @param[in] rssiThresholdDbm desired RSSI threshold(in dBm) over which the event
 *   \ref RAIL_EVENT_DETECT_RSSI_THRESHOLD is triggered.
 * @return Status code indicating success of the function call.
 *   Returns \ref RAIL_STATUS_INVALID_STATE in multiprotocol,
 *   if the requested \ref RAIL_Handle_t is not active.
 *   Returns \ref RAIL_STATUS_INVALID_CALL if called on parts on which this function
 *   is not supported.
 *
 * When in receive, RSSI is sampled and if it exceeds the threshold,
 * \ref RAIL_EVENT_DETECT_RSSI_THRESHOLD is triggered.
 *
 * @note:
 * If the radio is idled or this function is called with rssiThresholdDbm as
 * \ref RAIL_RSSI_INVALID_DBM while RSSI detect is still in effect, a
 * \ref RAIL_EVENT_DETECT_RSSI_THRESHOLD may not occur and the detection is disabled.
 * If the RSSI is already above threshold when this function is called then
 * \ref RAIL_EVENT_DETECT_RSSI_THRESHOLD will occur.
 * Once the RSSI goes over the configured threshold and
 * \ref RAIL_EVENT_DETECT_RSSI_THRESHOLD occurs, this function needs to be
 * called again to reactivate the RSSI threshold detection.
 * This function is only available on series-2 Sub-GHz parts EFR32XG23 and EFR32XG25.
 */
RAIL_Status_t RAIL_SetRssiDetectThreshold(RAIL_Handle_t railHandle,
                                          int8_t rssiThresholdDbm);

/**
 * Get the RSSI detection threshold(in dBm).
 *
 * @param[in] railHandle a RAIL instance handle.
 * @return rssiThreshold (in dBm) corresponding to the current handle.
 * @note:
 * The function returns \ref RAIL_RSSI_INVALID_DBM when
 * \ref RAIL_SetRssiDetectThreshold is not supported or disabled.
 * In multiprotocol, the function returns \ref RAIL_RSSI_INVALID_DBM if railHandle
 * is not active.
 * This function is only available on series-2 Sub-GHz parts EFR32XG23 and EFR32XG25.
 */
int8_t RAIL_GetRssiDetectThreshold(RAIL_Handle_t railHandle);

/**
 * Set up a callback function capable of converting a RX packet's LQI value
 * before being consumed by application code.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] cb A callback of type \ref RAIL_ConvertLqiCallback_t that is
 *   called before the RX packet LQI value is loaded into the \ref
 *   RAIL_RxPacketDetails_t structure for application consumption.
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_ConvertLqi(RAIL_Handle_t railHandle,
                              RAIL_ConvertLqiCallback_t cb);

/******************************************************************************
 * Address Filtering (RX)
 *****************************************************************************/
/**
 * @addtogroup Address_Filtering Address Filtering
 * @brief Configuration APIs for receive packet address filtering.
 *
 * The address filtering code examines the packet as follows.
 *
 * | `Bytes: 0 - 255` | `0 - 8`  | `0 - 255` | `0 - 8`  | `Variable` |
 * |:----------------:|---------:|----------:|---------:|:----------:|
 * | `Data0`          | `Field0` | `Data1`   | `Field1` | `Data2`    |
 *
 * In the above structure, anything listed as DataN is an optional section of
 * bytes that RAIL will not process for address filtering. The FieldN segments
 * reference specific sections in the packet that will each be interpreted
 * as an address during address filtering. The application may submit up to
 * four addresses to attempt to match each field segment and each address may
 * have a size of up to 8 bytes. To set up address filtering, first configure
 * the locations and length of the addresses in the packet. Next, configure
 * which combinations of matches in Field0 and Field1 should constitute an
 * address match. Last, enter addresses into tables for each field and
 * enable them. The first two of these are part of the \ref RAIL_AddrConfig_t
 * structure while the second part is configured at runtime using the
 * RAIL_SetAddressFilterAddress() API. A brief description of each
 * configuration is listed below.
 *
 * The offsets and sizes of the fields
 * are assumed fixed for the RAIL address filter. To set them, specify
 * arrays for these values in the sizes and offsets entries in the
 * \ref RAIL_AddrConfig_t structure. A size of zero indicates that a field is
 * disabled. The start offset for a field is relative to the previous start
 * offset and, if you're using FrameType decoding, the first start offset is
 * relative to the end of the byte containing the frame type.
 *
 * Configuring which combinations of Field0 and Field1 constitute a match is
 * the most complex portion of the address filter. The easiest way to think
 * about this is with a truth table. If you consider each of the four possible
 * address entries in a field, you can have a match on any one of those or a
 * match for none of them. This is shown in the 5x5 truth table below where
 * Field0 matches are the rows and Field1 matches are the columns.
 *
 * |                | No Match | Address 0 | Address 1 | Address 2 | Address 3 |
 * |----------------|----------|-----------|-----------|-----------|-----------|
 * | __No Match__   | bit0     | bit1      | bit2      | bit3      | bit4      |
 * | __Address 0__  | bit5     | bit6      | bit7      | bit8      | bit9      |
 * | __Address 1__  | bit10    | bit11     | bit12     | bit13     | bit14     |
 * | __Address 2__  | bit15    | bit16     | bit17     | bit18     | bit19     |
 * | __Address 3__  | bit20    | bit21     | bit22     | bit23     | bit24     |
 *
 * Because this is only 25 bits, it can be represented in one 32-bit integer
 * where 1 indicates a filter pass and 0 indicates a filter fail. This is the
 * matchTable parameter in the configuration structure and is used during
 * filtering. For common simple configurations, two defines are provided with
 * the truth tables as shown below. The first is \ref
 * ADDRCONFIG_MATCH_TABLE_SINGLE_FIELD, which can be used if only using
 * one address field (either field). If using two fields and want to
 * force in the same address entry in each field, use the second define: \ref
 * ADDRCONFIG_MATCH_TABLE_DOUBLE_FIELD. For more complex systems,
 * create a valid custom table.
 *
 * @note Address filtering does not function reliably with PHYs that use a data
 *   rate greater than 500 kbps. If this is a requirement, filtering must
 *   currently be done by the application.
 *
 * @{
 */

/**
 * Configure address filtering.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] addrConfig The configuration structure, which defines how
 *   addresses are set up in your packets.
 * @return Status code indicating success of the function call.
 *
 * You must call this function to set up address filtering. You may call it
 * multiple times but all previous information is wiped out each time you call
 * and any configured addresses must be reset.
 */
RAIL_Status_t RAIL_ConfigAddressFilter(RAIL_Handle_t railHandle,
                                       const RAIL_AddrConfig_t *addrConfig);

/**
 * Enable address filtering.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] enable An argument to indicate whether or not to enable address
 *   filtering.
 * @return True if address filtering was enabled to start with and false
 *   otherwise.
 *
 * Only allow packets through that pass the current address filtering
 * configuration. This does not reset or change the configuration so you can
 * set that up before turning on this feature.
 */
bool RAIL_EnableAddressFilter(RAIL_Handle_t railHandle, bool enable);

/**
 * Return whether address filtering is currently enabled.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return True if address filtering is enabled and false otherwise.
 */
bool RAIL_IsAddressFilterEnabled(RAIL_Handle_t railHandle);

/**
 * Reset the address filtering configuration.
 *
 * @param[in] railHandle A RAIL instance handle.
 *
 * Resets all structures related to address filtering. This does not disable
 * address filtering. It leaves the radio in a state where no packets
 * pass filtering.
 */
void RAIL_ResetAddressFilter(RAIL_Handle_t railHandle);

/**
 * Set an address for filtering in hardware.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] field Indicates an address field for this address.
 * @param[in] index Indicates a match entry for this address for a
 *   given field.
 * @param[in] value A pointer to the address data. This must be at least as
 *   long as the size specified in RAIL_ConfigAddressFilter(). The first byte,
 *   value[0], will be compared to the first byte received over the air for this
 *   address field.
 * @param[in] enable A boolean to indicate whether this address should be
 *   enabled immediately.
 * @return Status code indicating success of the function call.
 *
 * This function loads the given address into hardware for filtering and
 * starts filtering if you set the enable parameter to true. Otherwise,
 * call RAIL_EnableAddressFilterAddress() to turn it on later.
 */
RAIL_Status_t RAIL_SetAddressFilterAddress(RAIL_Handle_t railHandle,
                                           uint8_t field,
                                           uint8_t index,
                                           const uint8_t *value,
                                           bool enable);

/**
 * Set an address bit mask for filtering in hardware.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] field Indicates an address field for this address bit mask.
 * @param[in] bitMask A pointer to the address bitmask. This must be at least
 *   as long as the size specified in RAIL_ConfigAddressFilter(). The first
 *   byte, bitMask[0], will be applied to the first byte received over the air
 *   for this address field. Bits set to 1 in the bit mask indicate which bit
 *   positions in the incoming packet to compare against the stored addresses
 *   during address filtering. Bits set to 0 indicate which bit positions to
 *   ignore in the incoming packet during address filtering. This bit mask is
 *   applied to all address entries.
 * @return Status code indicating success of the function call.
 *
 * This function loads the given address bit mask into hardware for use when
 * address filtering is enabled. All bits in the stored address bit mask are
 * set to 1 during hardware initialization and when either \ref
 * RAIL_ConfigAddressFilter() or \ref RAIL_ResetAddressFilter() are called.
 *
 * @note This feature/API is not supported on the EFR32XG1 family of chips
 *       or the EFR32XG21. Use the compile time symbol \ref
 *       RAIL_SUPPORTS_ADDR_FILTER_ADDRESS_BIT_MASK or the runtime call \ref
 *       RAIL_SupportsAddrFilterAddressBitMask() to check whether the platform
 *       supports this feature.
 */
RAIL_Status_t RAIL_SetAddressFilterAddressMask(RAIL_Handle_t railHandle,
                                               uint8_t field,
                                               const uint8_t *bitMask);

/**
 * Enable address filtering for the specified address.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] enable An argument to indicate whether or not to enable address
 *   filtering.
 * @param[in] field Indicates an address for the address.
 * @param[in] index Indicates a match entry in the given field you want to enable.
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_EnableAddressFilterAddress(RAIL_Handle_t railHandle,
                                              bool enable,
                                              uint8_t field,
                                              uint8_t index);

/** @} */ // end of group Address_Filtering

/** @} */ // end of group Receive

/******************************************************************************
 * Auto-ACKing
 *****************************************************************************/
/// @addtogroup Auto_Ack Auto-ACK
/// @brief APIs for configuring auto-ACK functionality
///
/// These APIs configure the radio for automatic acknowledgment
/// features. Auto-ACK inherently changes how the underlying state machine
/// behaves so users should not modify RAIL_SetRxTransitions() and
/// RAIL_SetTxTransitions() while using auto-ACK features.
///
/// @code{.c}
/// // Go to RX after ACK operation.
/// RAIL_AutoAckConfig_t autoAckConfig = {
///   .enable = true,
///   .ackTimeout = 1000,
///   // "error" param ignored
///   .rxTransitions = { RAIL_RF_STATE_RX, RAIL_RF_STATE_RX},
///   // "error" param ignored
///   .txTransitions = { RAIL_RF_STATE_RX, RAIL_RF_STATE_RX}
/// };
///
/// RAIL_Status_t status = RAIL_ConfigAutoAck(railHandle, &autoAckConfig);
///
/// uint8_t ackData[] = {0x05, 0x02, 0x10, 0x00};
///
/// RAIL_Status_t status = RAIL_WriteAutoAckFifo(railHandle,
///                                              ackData,
///                                              sizeof(ackData));
/// @endcode
///
/// The acknowledgment transmits based on the frame format configured via
/// the Radio Configurator. For example, if the frame format is using a variable
/// length scheme, the ACK will be sent according to that scheme. If a 10-byte
/// packet is loaded into the ACK, but the variable length field of the ACK
/// payload specifies a length of 5, only 5 bytes will transmit for the ACK.
/// The converse is also true, if the frame length is configured to be a fixed
/// 10-byte packet but only 5 bytes are loaded into the ACK buffer, a TX
/// underflow occurs during the ACK transmit.
///
/// Unlike in non-auto-ACK mode, auto-ACK mode will always return to a single
/// state after all ACK sequences complete, regardless of whether
/// the ACK was successfully received/sent or not. See the documentation
/// of \ref RAIL_ConfigAutoAck() for configuration information. To
/// suspend automatic acknowledgment of a series of packets after transmit
/// or receive call RAIL_PauseTxAutoAck() or RAIL_PauseRxAutoAck() respectively
/// with the pause parameter set to true. When auto-ACKing is paused, after
/// receiving or transmitting a packet (regardless of success), the radio
/// transitions to the same state it would use while ACKing. To return to
/// normal state transition logic outside of ACKing, call \ref
/// RAIL_ConfigAutoAck() with the \ref RAIL_AutoAckConfig_t::enable field false
/// and specify the desired transitions in the \ref
/// RAIL_AutoAckConfig_t::rxTransitions and RAIL_AutoAckConfig_t::txTransitions
/// fields. To get out of a paused state and resume auto-ACKing, call
/// RAIL_PauseTxAutoAck() and/or RAIL_PauseRxAutoAck() with the pause parameter
/// set to false.
///
/// Applications can cancel the transmission of an ACK with
/// RAIL_CancelAutoAck(). Conversely, applications can control if a transmit
/// operation should wait for an ACK after transmitting by using
/// the \ref RAIL_TX_OPTION_WAIT_FOR_ACK option.
///
/// When \ref Antenna_Control is used for multiple antennas, ACKs are
/// transmitted on the antenna that was selected to receive the packet
/// being acknowledged. When receiving an ACK, the
/// \ref RAIL_RxOptions_t antenna options are used just like for any other
/// receive.
///
/// If the ACK payload is dynamic, the application must call
/// RAIL_WriteAutoAckFifo() with the appropriate ACK payload after the
/// application processes the receive. RAIL can auto-ACK from the normal
/// transmit buffer if RAIL_UseTxFifoForAutoAck() is called before the radio
/// transmits the ACK. Ensure the transmit buffer contains data loaded by
/// RAIL_WriteTxFifo().
///
/// Standard-based protocols that contain auto-ACK functionality are normally
/// configured in the protocol-specific configuration function. For example,
/// RAIL_IEEE802154_Init() provides auto-ACK configuration parameters in \ref
/// RAIL_IEEE802154_Config_t and should only be configured through that
/// function. It is not advisable to call both RAIL_IEEE802154_Init() and \ref
/// RAIL_ConfigAutoAck(). However, ACK modification functions are still valid to
/// use with protocol-specific ACKs. To cancel an IEEE 802.15.4 ACK transmit,
/// use RAIL_CancelAutoAck().
///
/// @{

/// Configure and enable automatic acknowledgment.
///
/// @param[in] railHandle A RAIL instance handle.
/// @param[in] config Auto-ACK configuration structure.
/// @return Status code indicating success of the function call.
///
/// Configures the RAIL state machine to for hardware-accelerated automatic
/// acknowledgment. ACK timing parameters are defined in the configuration
/// structure.
///
/// While auto-ACKing is enabled, do not call the following RAIL functions:
///   - RAIL_SetRxTransitions()
///   - RAIL_SetTxTransitions()
///
/// Note that if you are enabling auto-ACK (i.e., "enable" field is true)
/// the "error" fields of rxTransitions and txTransitions are ignored.
/// After all ACK sequences, (success or fail) the state machine will return
/// the radio to the "success" state, which can be either
/// \ref RAIL_RF_STATE_RX or \ref RAIL_RF_STATE_IDLE (returning to
/// \ref RAIL_RF_STATE_TX is not supported).
/// If you need information about the
/// actual success of the ACK sequence, use RAIL events such as
/// \ref RAIL_EVENT_TXACK_PACKET_SENT to make sure an ACK was sent, or
/// \ref RAIL_EVENT_RX_ACK_TIMEOUT to make sure that an ACK was received
/// within the specified timeout.
///
/// To set a certain turnaround time (i.e., txToRx and rxToTx
/// in \ref RAIL_StateTiming_t), make txToRx lower than
/// desired to ensure you get to RX in time to receive the ACK.
/// Silicon Labs recommends setting 10 us lower than desired:
///
/// @code{.c}
/// void setAutoAckStateTimings()
/// {
///   RAIL_StateTiming_t timings;
///
///   // User is already in auto-ACK and wants a turnaround of 192 us.
///   timings.rxToTx = 192;
///   timings.txToRx = 192 - 10;
///
///   // Set other fields of timings...
///   timings.idleToRx = 100;
///   timings.idleToTx = 100;
///   timings.rxSearchTimeout = 0;
///   timings.txToRxSearchTimeout = 0;
///
///   RAIL_SetStateTiming(railHandle, &timings);
/// }
/// @endcode
///
/// As opposed to an explicit "Disable" API, set the "enable"
/// field of the RAIL_AutoAckConfig_t to false. Then, auto-ACK will be
/// disabled and state transitions will be returned to the values set
/// in \ref RAIL_AutoAckConfig_t. When disabling, the "ackTimeout" field
/// isn't used.
///
/// @note Auto-ACKing may not be enabled while RX Channel Hopping is enabled,
///   or when BLE is enabled.
///
RAIL_Status_t RAIL_ConfigAutoAck(RAIL_Handle_t railHandle,
                                 const RAIL_AutoAckConfig_t *config);

/**
 * Return the enable status of the auto-ACK feature.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if auto-ACK is enabled, false if disabled.
 */
bool RAIL_IsAutoAckEnabled(RAIL_Handle_t railHandle);

/**
 * Load the auto-ACK buffer with ACK data.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] ackData A pointer to ACK data to transmit.
 * @param[in] ackDataLen The number of bytes in ACK data.
 * @return Status code indicating success of the function call.
 *
 * If the ACK buffer is available for updates, load the ACK buffer with data.
 */
RAIL_Status_t RAIL_WriteAutoAckFifo(RAIL_Handle_t railHandle,
                                    const uint8_t *ackData,
                                    uint8_t ackDataLen);

/**
 * Pause/resume RX auto-ACK functionality.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] pause Pause or resume RX auto-ACKing.
 *
 * When RX auto-ACKing is paused, the radio transitions to default
 * state after receiving a packet and does not transmit an ACK.
 * When RX auto-ACK is resumed, the radio resumes automatically ACKing
 * every successfully received packet.
 */
void RAIL_PauseRxAutoAck(RAIL_Handle_t railHandle,
                         bool pause);

/**
 * Return whether the RX auto-ACK is paused.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if RX auto-ACK is paused, false if not paused.
 */
bool RAIL_IsRxAutoAckPaused(RAIL_Handle_t railHandle);

/**
 * Pause/resume TX auto-ACK functionality.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] pause Pause or resume TX auto-ACKing.
 *
 * When TX auto-ACKing is paused, the radio transitions to a default
 * state after transmitting a packet and does not wait for an ACK. When TX
 * auto-ACK is resumed, the radio resumes automatically waiting for
 * an ACK after a successful transmit.
 */
void RAIL_PauseTxAutoAck(RAIL_Handle_t railHandle, bool pause);

/**
 * Return whether the TX auto-ACK is paused.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if TX auto-ACK is paused, false if not paused.
 */
bool RAIL_IsTxAutoAckPaused(RAIL_Handle_t railHandle);

/**
 * Modify the upcoming ACK to use the Transmit FIFO.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return Status code indicating success of the function call. The call will
 *   fail if it is too late to modify the outgoing ACK.
 *
 * This function allows the application to use the normal Transmit FIFO as
 * the data source for the upcoming ACK. The ACK modification to use the
 * Transmit FIFO only applies to one ACK transmission.
 *
 * This function only returns true if the following conditions are met:
 *   - Radio has not already decided to use the ACK buffer AND
 *   - Radio is either looking for sync, receiving the packet after sync, or in
 *     the Rx2Tx turnaround before the ACK is sent.
 *
 * @note The Transmit FIFO must not be used for AutoACK when IEEE 802.15.4,
 *   Z-Wave, or BLE protocols are active.
 */
RAIL_Status_t RAIL_UseTxFifoForAutoAck(RAIL_Handle_t railHandle);

/**
 * Cancel the upcoming ACK.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return Status code indicating success of the function call. This call will
 *   fail if it is too late to modify the outgoing ACK.
 *
 * This function allows the application to cancel the upcoming automatic
 * acknowledgment.
 *
 * This function only returns true if the following conditions are met:
 *   - Radio has not already decided to transmit the ACK AND
 *   - Radio is either looking for sync, receiving the packet after sync or in
 *     the Rx2Tx turnaround before the ACK is sent.
 */
RAIL_Status_t RAIL_CancelAutoAck(RAIL_Handle_t railHandle);

/**
 * Return whether the radio is currently waiting for an ACK.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return True if radio is waiting for ACK, false if radio is not waiting for
 *   an ACK.
 *
 * This function allows the application to query whether the radio is currently
 * waiting for an ACK after a transmit operation.
 */
bool RAIL_IsAutoAckWaitingForAck(RAIL_Handle_t railHandle);

/** @} */ // end of group Auto_Ack

/******************************************************************************
 * Calibration
 *****************************************************************************/
/// @addtogroup Calibration
/// @brief APIs for calibrating the radio
/// @{
///
/// These APIs calibrate the radio. The RAIL library
/// determines which calibrations are necessary. Calibrations can
/// be enabled/disabled with the RAIL_CalMask_t parameter.
///
/// Some calibrations produce values that can be saved and reapplied to
/// avoid repeating the calibration process.
///
/// Calibrations can either be run with \ref RAIL_Calibrate, or with the
/// individual chip-specific calibration routines. An example for running code
/// with \ref RAIL_Calibrate looks like the following:
///
/// @code{.c}
/// static RAIL_CalValues_t calValues = RAIL_CALVALUES_UNINIT;
///
/// void RAILCb_Event(RAIL_Handle_t railHandle, RAIL_Events_t events) {
///   // Omitting other event handlers
///   if (events & RAIL_EVENT_CAL_NEEDED) {
///     // Run all pending calibrations, and save the results
///     RAIL_Calibrate(railHandle, &calValues, RAIL_CAL_ALL_PENDING);
///   }
/// }
/// @endcode
///
/// Alternatively, if the image rejection calibration for your chip can be
/// determined ahead of time, such as by running the calibration on a separate
/// firmware image on each chip, the following calibration process will
/// result in smaller code.
///
/// @code{.c}
/// static uint32_t imageRejection = IRCAL_VALUE;
///
/// void RAILCb_Event(RAIL_Handle_t railHandle, RAIL_Events_t events) {
///   // Omitting other event handlers
///   if (events & RAIL_EVENT_CAL_NEEDED) {
///     RAIL_CalMask_t pendingCals = RAIL_GetPendingCal(railHandle);
///     // Disable the radio if we have to do an offline calibration
///     if (pendingCals & RAIL_CAL_TEMP_VC0) {
///       RAIL_CalibrateTemp(railHandle);
///     }
///     if (pendingCals & RAIL_CAL_ONETIME_IRCAL) {
///       RAIL_ApplyIrCalibration(railHandle, imageRejection);
///     }
///   }
/// }
/// @endcode

/**
 * Initialize RAIL calibration.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] calEnable A bitmask that indicates which calibrations
 *   to enable for a callback notification.
 *   The exact meaning of these bits is chip-specific.
 * @return Status code indicating success of the function call.
 *
 * Calibration initialization provides the calibration settings that
 * correspond to the current radio configuration.
 */
RAIL_Status_t RAIL_ConfigCal(RAIL_Handle_t railHandle,
                             RAIL_CalMask_t calEnable);

/**
 * Start the calibration process.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in,out] calValues A structure of calibration values to apply.
 *   If a valid calibration structure is provided and the structure
 *   contains valid calibration values, those values will be applied to the
 *   hardware and the RAIL library will cache those values for use again later.
 *   If a valid calibration structure is provided and the structure
 *   contains a calibration value of \ref RAIL_CAL_INVALID_VALUE for the
 *   desired calibration, the desired calibration will run, the calibration
 *   values structure will be updated with a valid calibration value, and the
 *   RAIL library will cache that value for use again later.
 *   If a NULL pointer is provided, the desired calibration will run
 *   and the RAIL library will cache that value for use again later. However,
 *   the valid calibration value will not be returned to the application.
 * @param[in] calForce A mask to force specific calibration(s) to execute.
 *   To run all pending calibrations, use the value \ref RAIL_CAL_ALL_PENDING.
 *   Only the calibrations specified will run, even if not enabled during
 *   initialization.
 * @return Status code indicating success of the function call.
 *
 * If calibrations were performed previously and the application saves the
 * calibration values (i.e., call this function with a calibration values
 * structure containing calibration values of \ref RAIL_CAL_INVALID_VALUE
 * before a reset), the application can later bypass the time it would normally
 * take to recalibrate hardware by reusing previous calibration values (i.e.,
 * call this function with a calibration values structure containing valid
 * calibration values after a reset).
 *
 * If multiple protocols are used, this function will make the given railHandle
 * active, if not already, and perform calibration. If called during a protocol
 * switch, to perform an IR calibration for the first time, it will
 * return \ref RAIL_STATUS_INVALID_STATE, in which case the application must
 * defer calibration until after the protocol switch is complete. Silicon Labs
 * recommends calling this function from the application main loop.
 *
 * @note Instead of this function, consider using the individual chip-specific
 *   functions. Using the individual functions will allow for better
 *   dead-stripping if not all calibrations are run.
 * @note Some calibrations should only be executed when the radio is IDLE. See
 *   chip-specific documentation for more details.
 */
RAIL_Status_t RAIL_Calibrate(RAIL_Handle_t railHandle,
                             RAIL_CalValues_t *calValues,
                             RAIL_CalMask_t calForce);

/**
 * Return the current set of pending calibrations.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return A mask of all pending calibrations that the user has been asked to
 *   perform.
 *
 * This function returns a full set of pending calibrations. The only way
 * to clear pending calibrations is to perform them using the \ref
 * RAIL_Calibrate() API with the appropriate list of calibrations.
 */
RAIL_CalMask_t RAIL_GetPendingCal(RAIL_Handle_t railHandle);

/**
 * Enable/disable the PA calibration.
 *
 * @param[in] enable Enables/disables the PA calibration.
 *
 * Enabling will ensure that the PA power remains constant chip-to-chip.
 * By default, this feature is disabled after reset.
 *
 * @note Call this function before \ref RAIL_ConfigTxPower() if this
 *   feature is desired.
 */
void RAIL_EnablePaCal(bool enable);

/** @} */ // end of group Calibration

/******************************************************************************
 * RF Sense Structures
 *****************************************************************************/
/**
 * @addtogroup Rf_Sense RF Sense
 * @{
 */

/**
 * Start/stop the RF Sense functionality in Energy Detection Mode for use
 * during low-energy sleep modes.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] band The frequency band(s) on which to sense the RF energy.
 *   To stop RF Sense, specify \ref RAIL_RFSENSE_OFF.
 * @param[in] senseTime The time (in microseconds) the RF energy must be
 *   continually detected to be considered "sensed".
 * @param[in] cb \ref RAIL_RfSense_CallbackPtr_t is called when the RF is
 *   sensed. Set null if polling via \ref RAIL_IsRfSensed().
 * @return The actual senseTime used, which may be different than
 *   requested due to limitations of the hardware. If 0, RF sense was
 *   disabled or could not be enabled (no callback will be issued).
 *
 * The EFR32 has the ability to sense the presence of RF Energy above -20 dBm
 * within either or both the 2.4 GHz and Sub-GHz bands and trigger an event
 * if that energy is continuously present for certain durations of time.
 *
 * @note After RF energy has been sensed, the RF Sense is automatically
 *   disabled and \ref RAIL_StartRfSense() must be called again to reactivate it.
 *   If RF energy has not been sensed and to manually disable RF Sense,
 *   \ref RAIL_StartRfSense() must be called with band specified as
 *   \ref RAIL_RFSENSE_OFF or with senseTime set to 0 microseconds.
 *
 * @note Packet reception is not guaranteed to work correctly once RF Sense is
 *   enabled, both in single protocol and multiprotocol RAIL.
 *   To be safe, an application should turn this on only after idling
 *   the radio to stop receive and turn it off before attempting to restart
 *   receive. Since EM4 sleep causes the chip to come up through the reset
 *   vector any wake from EM4 must also shut off RF Sense to ensure proper
 *   receive functionality.
 *
 * @warning For some chips, RF Sense functionality is only guaranteed within
 * a specified temperature range.
 * See chip-specific documentation for more details.
 */
RAIL_Time_t RAIL_StartRfSense(RAIL_Handle_t railHandle,
                              RAIL_RfSenseBand_t band,
                              RAIL_Time_t senseTime,
                              RAIL_RfSense_CallbackPtr_t cb);

/// Start/stop the RF Sense functionality in Selective(OOK Based) Mode for use
/// during low-energy sleep modes.
///
/// @param[in] railHandle A RAIL instance handle.
/// @param[in] config \ref RAIL_RfSenseSelectiveOokConfig_t holds the RFSENSE
/// configuration for Selective(OOK) mode.
/// @return Status code indicating success of the function call.
///
/// Some chips support Selective RF energy detection (OOK mode) where the
/// user can program the chip to look for a particular sync word pattern
/// (1byte - 4bytes) sent using OOK and wake only when that is detected.
/// See chip-specific documentation for more details.
///
/// The following code gives an example of how to use RF Sense functionality
/// in Selective(OOK Based) Mode.
/// @code{.c}
///
/// // Syncword Length in bytes, 1-4 bytes.
/// #define NUMSYNCWORDBYTES (2U)
/// // Syncword Value.
/// #define SYNCWORD         (0xB16FU)
///
/// // Configure the transmitting node for sending the wakeup packet.
/// RAIL_Idle(railHandle, RAIL_IDLE_ABORT, true);
/// RAIL_ConfigRfSenseSelectiveOokWakeupPhy(railHandle);
/// RAIL_SetRfSenseSelectiveOokWakeupPayload(railHandle, NUMSYNCWORDBYTES, SYNCWORD);
/// RAIL_StartTx(railHandle, channel, RAIL_TX_OPTIONS_DEFAULT, NULL);
///
/// // Configure the receiving node (EFR32XG22) for RF Sense.
/// RAIL_RfSenseSelectiveOokConfig_t config = {
///  .band = rfBand,
///  .syncWordNumBytes = NUMSYNCWORDBYTES,
///  .syncWord = SYNCWORD,
///  .cb = &RAILCb_SensedRf
/// };
/// RAIL_StartSelectiveOokRfSense(railHandle, &config);
///
/// @endcode
///
/// @note After RF energy has been sensed, the RF Sense is automatically
///   disabled and \ref RAIL_StartSelectiveOokRfSense() must be called again to
///   reactivate. If RF energy has not been sensed and to manually disable
///  RF Sense, \ref RAIL_StartSelectiveOokRfSense() must be called with band
///   specified as \ref RAIL_RFSENSE_OFF or with
///   \ref RAIL_RfSenseSelectiveOokConfig_t as NULL.
///
/// @note Packet reception is not guaranteed to work correctly once RF Sense is
///   enabled, both in single protocol and multiprotocol RAIL.
///   To be safe, an application should turn this on only after idling
///   the radio to stop receive and turn it off before attempting to restart
///   receive. Since EM4 sleep causes the chip to come up through the reset
///   vector any wake from EM4 must also shut off RF Sense to ensure proper
///   receive functionality.
///
RAIL_Status_t RAIL_StartSelectiveOokRfSense(RAIL_Handle_t railHandle,
                                            RAIL_RfSenseSelectiveOokConfig_t *config);

/**
 * Switch to RF Sense Selective(OOK) PHY.
 *
 * @param[in] railHandle A handle for RAIL instance.
 * @return A status code indicating success of the function call.
 *
 * This function switches to the RFSENSE Selective(OOK) PHY for transmitting a
 * packet to wake up a chip that supports Selective RF energy detection (OOK
 * mode). You may only call this function while the radio is idle. While the
 * radio is configured for this PHY, receive functionality should not be used.
 *
 * @note The user must also set up the transmit FIFO, via
 * \ref RAIL_SetRfSenseSelectiveOokWakeupPayload, post this function call to
 * include the first byte as the Preamble Byte, followed by the
 * Syncword (1byte - 4bytes).
 * See chip-specific documentation for more details.
 */
RAIL_Status_t RAIL_ConfigRfSenseSelectiveOokWakeupPhy(RAIL_Handle_t railHandle);

/**
 * Set the transmit payload for waking up a node configured for
 * RF Sense Selective(OOK).
 *
 * @param[in] railHandle A handle for RAIL instance.
 * @param[in] numSyncwordBytes Syncword Length in bytes, 1-4 bytes.
 * @param[in] syncword  Syncword Value.
 * @return A status code indicating success of the function call.
 *
 * @note You must call this function after the chip has been set up with the
 * RF Sense Selective(OOK) PHY, using \ref RAIL_ConfigRfSenseSelectiveOokWakeupPhy.
 *
 */
RAIL_Status_t RAIL_SetRfSenseSelectiveOokWakeupPayload(RAIL_Handle_t railHandle,
                                                       uint8_t numSyncwordBytes,
                                                       uint32_t syncword);

/**
 * Check whether the RF was sensed.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if RF was sensed since the last call to \ref RAIL_StartRfSense.
 *   False otherwise.
 *
 * This function is useful if \ref RAIL_StartRfSense is called with a null
 * callback. It is generally used after EM4 reboot but can be used any time.
 */
bool RAIL_IsRfSensed(RAIL_Handle_t railHandle);

/** @} */ // end of group Rf_Sense

/******************************************************************************
 * RX Channel Hopping
 *****************************************************************************/
/**
 * @addtogroup Rx_Channel_Hopping RX Channel Hopping
 * @brief Hardware accelerated hopping between channels while waiting for a
 * packet in receive.
 * @{
 *
 * Channel hopping provides a hardware accelerated method for
 * scanning across multiple channels quickly, as part of a receive protocol.
 * While it is possible to call \ref RAIL_StartRx on different channels,
 * back to back, and listen on many channels sequentially in that way, the
 * time it takes to switch channels with that method may be too long for some
 * protocols. This API pre-computes necessary channel change operations
 * for a given list of channels, so that the radio can move from channel
 * to channel much faster. Additionally, it leads to more succinct code
 * as channel changes will be done implicitly, without requiring numerous calls
 * to \ref RAIL_StartRx. Currently, while this feature is enabled, the radio
 * will hop channels in the given sequence each time it enters RX.
 *
 * The following code gives an example of how to use
 * the RX Channel Hopping API.
 * @code{.c}
 *
 * // The channel hopping buffer requires RAIL_CHANNEL_HOPPING_BUFFER_SIZE_PER_CHANNEL
 * // number of 32-bit words of overhead per channel, plus 3 words overall plus the
 * // twice the size of the radioConfigDeltaSubtract of the whole radio configuration,
 * // plus the twice the sum of the sizes of all the radioConfigDeltaAdds of
 * // all the channel hopping channels.
 * #define CHANNEL_HOPPING_NUMBER_OF_CHANNELS 4
 * #define CHANNEL_HOPPING_BUFFER_SIZE do {        \
 *   3 +                                           \
 *   (RAIL_CHANNEL_HOPPING_BUFFER_SIZE_PER_CHANNEL \
 *    * CHANNEL_HOPPING_NUMBER_OF_CHANNELS) +      \
 *   2 * (SIZEOF_UINT32_DELTA_SUBTRACT +           \
 *   SIZEOF_UINT32_DELTA_ADD_0 +                   \
 *   SIZEOF_UINT32_DELTA_ADD_1 +                   \
 *   SIZEOF_UINT32_DELTA_ADD_2 +                   \
 *   SIZEOF_UINT32_DELTA_ADD_3)                    \
 * } while (0)
 *
 * RAIL_RxChannelHoppingConfigEntry_t channelHoppingEntries[CHANNEL_HOPPING_NUMBER_OF_CHANNELS];
 * uint32_t channelHoppingBuffer[CHANNEL_HOPPING_BUFFER_SIZE];
 *
 * RAIL_RxChannelHoppingConfig_t channelHoppingConfig = {
 *   .buffer = channelHoppingBuffer,
 *   .bufferLength = CHANNEL_HOPPING_BUFFER_SIZE,
 *   .numberOfChannels = CHANNEL_HOPPING_NUMBER_OF_CHANNELS,
 *   .entries = channelHoppingEntries
 * };
 *
 * channelHoppingEntries[0].channel = 1;
 * channelHoppingEntries[1].channel = 2;
 * channelHoppingEntries[2].channel = 3;
 *
 * RAIL_ConfigRxChannelHopping(railHandle, &channelHoppingConfig);
 * RAIL_EnableRxChannelHopping(railHandle, true, true)
 * @endcode
 */

/**
 * Configure RX Channel Hopping.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] config Configuration parameters for RX Channel Hopping.
 * @return Status code indicating success of the function call.
 *
 * Configure Channel Hopping channels, conditions, and parameters. This
 * API must be called before \ref RAIL_EnableRxChannelHopping(). This API must
 * never be called while the radio is on with RX Duty Cycle or Channel
 * Hopping enabled.
 *
 * @note This feature/API is not supported on the EFR32XG1 family of chips.
 *       Use the compile time symbol \ref RAIL_SUPPORTS_CHANNEL_HOPPING or
 *       the runtime call \ref RAIL_SupportsChannelHopping() to check whether
 *       the platform supports this feature.
 *
 * @note Calling this function will overwrite any settings configured with
 *       \ref RAIL_ConfigRxDutyCycle.
 */
RAIL_Status_t RAIL_ConfigRxChannelHopping(RAIL_Handle_t railHandle,
                                          RAIL_RxChannelHoppingConfig_t *config);

/**
 * Enable RX channel hopping.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] enable Enable (true) or disable (false) RX Channel Hopping.
 * @param[in] reset Start from the first channel of the channel hopping
 * sequence (true) or from wherever hopping left off last time the code
 * left RX.
 * @return Status code indicating success of the function call.
 *
 * Enable or disable Channel Hopping. Additionally, specify whether hopping
 * should be reset to start from the channel at index zero, or continue
 * from the channel last hopped to. The radio should not be on when
 * this API is called. \ref RAIL_ConfigRxChannelHopping must be called
 * successfully before this API is called.
 *
 * @note This feature/API is not supported on the EFR32XG1 family of chips.
 *       Use the compile time symbol \ref RAIL_SUPPORTS_CHANNEL_HOPPING or
 *       the runtime call \ref RAIL_SupportsChannelHopping() to check whether
 *       the platform supports this feature.
 *
 * @note RX Channel Hopping may not be enabled while auto-ACKing is enabled.
 *
 * @note Calling this function will overwrite any settings configured with
 *       \ref RAIL_EnableRxDutyCycle.
 */
RAIL_Status_t RAIL_EnableRxChannelHopping(RAIL_Handle_t railHandle,
                                          bool enable,
                                          bool reset);
/**
 * Get RSSI of one channel in the channel hopping sequence, during
 * channel hopping.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] channelIndex Index in the channel hopping sequence of the
 *            channel of interest
 * @return Latest RSSI for the channel at the specified index.
 *
 * @note This feature/API is not supported on the EFR32XG1 family of chips.
 *       Use the compile time symbol \ref RAIL_SUPPORTS_CHANNEL_HOPPING or
 *       the runtime call \ref RAIL_SupportsChannelHopping() to check whether
 *       the platform supports this feature.
 *
 * @note In multiprotocol, this function returns \ref RAIL_RSSI_INVALID
 *       immediately if railHandle is not the current active \ref RAIL_Handle_t.
 *
 * @note \ref RAIL_ConfigRxChannelHopping must be called successfully
 *       before this API is called.
 *
 * @note When the Z-Wave protocol is active, it is expected that after
 *       \ref RAIL_ConfigRxChannelHopping is called successfully on EFR32XG1
 *       family of chips, running \ref RAIL_GetChannelHoppingRssi() on a 40kbps
 *       PHY will not work well. Plan to use the 9.6kbps PHY for estimating
 *       channel noise instead. On EFR32XG2 family of chips, running
 *       \ref RAIL_GetChannelHoppingRssi() on the 9.6kbps PHY returns the RSSI
 *       measurement of the 40kpbs PHY. This is because the 9.6kbps PHY has
 *       trouble with RSSI measurements on EFR32XG2 family of chips.
 */
int16_t RAIL_GetChannelHoppingRssi(RAIL_Handle_t railHandle,
                                   uint8_t channelIndex);

/// Configure RX duty cycle mode.
///
/// @param[in] railHandle A RAIL instance handle.
/// @param[in] config Configuration structure to specify duty cycle parameters.
/// @return Status code indicating success of the function call.
///
/// Configure RX duty cycle mode. With this mode enabled, every time the radio
/// enters RX, it will duty cycle on and off to save power. The duty cycle
/// ratio can be altered dynamically and intelligently by the hardware by
/// staying on longer if a preamble or other packet segments are detected in
/// the air. This API must never be called while the radio is on with RX Duty
/// Cycle or Channel Hopping enabled.
/// For short delays (in the order of microseconds),
/// \ref RAIL_RxDutyCycleConfig_t::delay, this can be used to save receive
/// current while having little impact on the radio performance, for protocols
/// with long preambles. For long delays (in the order of milliseconds or higher)
/// the chip can be put into EM2 energy mode before re-entering RX,
/// to save extra power, with some application hooks as shown below.
///
/// @code{.c}
/// #include <rail.h>
/// #include <rail_types.h>
///
/// extern RAIL_Handle_t railHandle;
/// RAIL_Time_t periodicWakeupUs;
///
/// void RAILCb_Event(RAIL_Handle_t railHandle, RAIL_Events_t events) {
///   // Omitting other event handlers
///   if (events & RAIL_EVENT_RX_DUTY_CYCLE_RX_END) {
///     // Schedule the next receive.
///     RAIL_ScheduleRxConfig_t rxCfg = {
///       .start = periodicWakeupUs,
///       .startMode = RAIL_TIME_DELAY,
///       .end = 0U,
///       .endMode = RAIL_TIME_DISABLED,
///       .rxTransitionEndSchedule = 0U,
///       .hardWindowEnd = 0U
///     };
///
///     RAIL_Idle(railHandle, RAIL_IDLE_ABORT, true);
///     RAIL_ScheduleRx(railHandle, channel, &rxCfg, NULL);
///   }
/// }
///
/// void main(void) {
///   RAIL_Status_t status;
///   bool shouldSleep = false;
///
///   // This function depends on your board/chip but it must enable the LFCLK
///   // you intend to use for RTCC sync before we configure sleep as that
///   // function will attempt to auto detect the clock.
///   BoardSetupLFCLK();
///   // Initialize Power Manager module
///   sl_power_manager_init();
///   // Initialize RAIL Power Manager
///   RAIL_InitPowerManager();
///
///   // Configure sleep for timer synchronization
///   status = RAIL_ConfigSleep(railHandle, RAIL_SLEEP_CONFIG_TIMERSYNC_ENABLED);
///   assert(status == RAIL_STATUS_NO_ERROR);
///
///   // Application main loop
///   while(1) {
///     // ... do normal app stuff and set shouldSleep when we want to sleep
///     if (shouldSleep) {
///       // Let the CPU go to sleep if the system allows it.
///       sl_power_manager_sleep();
///     }
///   }
/// }
/// @endcode
///
/// @note This feature/API is not supported on the EFR32XG1 family of chips.
///       Use the compile time symbol \ref RAIL_SUPPORTS_CHANNEL_HOPPING or
///       the runtime call \ref RAIL_SupportsChannelHopping() to check whether
///       the platform supports this feature.
///
/// @note Calling this function will overwrite any settings configured with
///       \ref RAIL_ConfigRxChannelHopping.
RAIL_Status_t RAIL_ConfigRxDutyCycle(RAIL_Handle_t railHandle,
                                     const RAIL_RxDutyCycleConfig_t *config);

/**
 * Enable RX duty cycle mode.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] enable Enable (true) or disable (false) RX Duty Cycling.
 * @return Status code indicating success of the function call.
 *
 * Enable or disable RX duty cycle mode. After this is called, the radio
 * will begin duty cycling each time it enters RX, based on the
 * configuration passed to \ref RAIL_ConfigRxDutyCycle. This API must not
 * be called while the radio is on.
 *
 * @note This feature/API is not supported on the EFR32XG1 family of chips.
 *       Use the compile time symbol \ref RAIL_SUPPORTS_CHANNEL_HOPPING or
 *       the runtime call \ref RAIL_SupportsChannelHopping() to check whether
 *       the platform supports this feature.
 *
 * @note Calling this function will overwrite any settings configured with
 *       \ref RAIL_EnableRxChannelHopping.
 */
RAIL_Status_t RAIL_EnableRxDutyCycle(RAIL_Handle_t railHandle,
                                     bool enable);

/**
 * Get the default RX duty cycle configuration.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[out] config An application-provided non-NULL pointer to store
 *                   the default RX duty cycle configuration.
 * @return Status code indicating success of the function call.
 * Note that RAIL_STATUS_INVALID_PARAMETER will be returned if the current
 * channel's radio configuration does not support the requested information.
 *
 * To save power during RX, an application may want to go to low power as long as
 * possible by periodically waking up and trying to
 * "sense" if there are any incoming packets. This API returns the recommended
 * RX duty cycle configuration, so the application can enter low power mode
 * periodically without missing packets. To wake up
 * earlier, the application can reduce the delay parameter.
 * Note that these value might be different if any configuration / channel has
 * changed.
 **/
RAIL_Status_t RAIL_GetDefaultRxDutyCycleConfig(RAIL_Handle_t railHandle,
                                               RAIL_RxDutyCycleConfig_t *config);

/** @} */ // end of group Rx_Channel_Hopping

/******************************************************************************
 * Multiprotocol-Specific Functions
 *****************************************************************************/
/**
 * @addtogroup Multiprotocol
 * @brief Multiprotocol scheduler APIs to support multiple time-sliced PHYs.
 * @{
 */

/**
 * Yield the radio to other configurations.
 *
 * @param[in] railHandle A RAIL instance handle.
 *
 * This function is used to indicate that the previous transmit or scheduled
 * receive operation has completed. It must be used in multiprotocol RAIL because
 * the scheduler assumes that any transmit or receive operation that is started
 * can go on indefinitely based on state transitions and your protocol.
 * RAIL will not allow a lower priority tasks to run until this is called so it
 * can negatively impact performance of those protocols if this is omitted or
 * delayed. It is also possible to call the \ref RAIL_Idle() API to
 * both terminate the operation and idle the radio. In single protocol RAIL
 * this API does nothing, however, if RAIL Power Manager is initialized,
 * calling \ref RAIL_YieldRadio after scheduled TX/RX and instantaneous TX
 * completion, is required, to indicate to the Power Manager that the the radio
 * is no longer busy and can be idled for sleeping.
 *
 * See \ref rail_radio_scheduler_yield for more details.
 */
void RAIL_YieldRadio(RAIL_Handle_t railHandle);

/**
 * Get the status of the RAIL scheduler.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return \ref RAIL_SchedulerStatus_t status.
 *
 * This function can only be called from a callback context after the
 * \ref RAIL_EVENT_SCHEDULER_STATUS event occurs.
 */
RAIL_SchedulerStatus_t RAIL_GetSchedulerStatus(RAIL_Handle_t railHandle);

/**
 * Get the status of the RAIL scheduler, specific to the radio operation,
 * along with \ref RAIL_Status_t returned by RAIL API invoked by the
 * RAIL scheduler.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[out] pSchedulerStatus An application-provided pointer to store
 *   \ref RAIL_SchedulerStatus_t status. Can be NULL as long as
 *   \ref RAIL_Status_t pointer is not NULL.
 * @param[out] pRailStatus An application-provided pointer to store
 *   \ref RAIL_Status_t of the RAIL API invoked by the RAIL scheduler.
 *   Can be NULL as long as \ref RAIL_SchedulerStatus_t pointer is not NULL.
 * @return \ref RAIL_Status_t indicating success of the function call.
 *
 * This function can only be called from a callback context after the
 * \ref RAIL_EVENT_SCHEDULER_STATUS event occurs.
 */
RAIL_Status_t RAIL_GetSchedulerStatusAlt(RAIL_Handle_t railHandle,
                                         RAIL_SchedulerStatus_t *pSchedulerStatus,
                                         RAIL_Status_t *pRailStatus);

/**
 * Change the priority of a specified task type in multiprotocol.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] priority Desired new priority for the railHandle's active task
 * @param[in] taskType Type of task whose priority should be updated
 * @return \ref RAIL_Status_t indicating success of the function call.
 *
 * While the application can use this function however it likes, a major use
 * case is being able to increase an infinite receive priority while receiving
 * a packet. In other words, a given RAIL_Handle_t can maintain a very low
 * priority background receive, but upon getting a
 * \ref RAIL_EVENT_RX_SYNC1_DETECT_SHIFT or
 * \ref RAIL_EVENT_RX_SYNC2_DETECT_SHIFT event, the app can call this function
 * to increase the background RX priority to lower the risk another protocol
 * might preempt during packet reception.
 */
RAIL_Status_t RAIL_SetTaskPriority(RAIL_Handle_t railHandle,
                                   uint8_t priority,
                                   RAIL_TaskType_t taskType);

/**
 * Get time needed to switch between protocols.
 *
 * @return \ref RAIL_Time_t Time needed to switch between protocols.
 */
RAIL_Time_t RAIL_GetTransitionTime(void);

/**
 * Set time needed to switch between protocols. Call this API
 * only once, before any protocol is initialized via
 * \ref RAIL_Init(). Changing this value during normal operation
 * can result in improper scheduling behavior.
 *
 * @param[in] transitionTime Time needed to switch between protocols.
 */
void RAIL_SetTransitionTime(RAIL_Time_t transitionTime);

/** @} */ // end of group Multiprotocol

/******************************************************************************
 * Diagnostic
 *****************************************************************************/
/**
 * @addtogroup Diagnostic
 * @brief APIs for diagnostic and test chip modes
 * @{
 */

/**
 * Configure direct mode for RAIL.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] directModeConfig Configuration structure to specify direct mode
 *   parameters. Default configuration will be used if NULL is passed.
 * @return \ref RAIL_STATUS_NO_ERROR on success and an error code on failure.
 *
 * This API configures direct mode and should be called before
 * calling \ref RAIL_EnableDirectMode().  If this function is not called, the
 * following default configuration will be used: \n
 * \b EFR32xG1x \n
 * Sync Rx : false \n
 * Sync Tx : false \n
 * TX data in (DIN) : EFR32_PC10 \n
 * RX data out (DOUT) : EFR32_PC11 \n
 * TX/RX clk out (DCLK) : EFR32_PC9 \n
 * \b EFR32xG2x: \n
 * Sync Rx : false \n
 * Sync Tx : false \n
 * TX data in (DIN) : EFR32_PA7 \n
 * RX data out (DOUT) : EFR32_PA5 \n
 * TX/RX clk out (DCLK) : EFR32_PA6
 *
 * @warning This API is not safe to use in a multiprotocol app.
 */
RAIL_Status_t RAIL_ConfigDirectMode(RAIL_Handle_t railHandle,
                                    const RAIL_DirectModeConfig_t *directModeConfig);

/**
 *  Enable or disable direct mode for RAIL.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] enable Whether or not to enable direct mode for TX and RX.
 * @return \ref RAIL_STATUS_NO_ERROR on success and an error code on failure.
 * *
 * See \ref RAIL_EnableDirectModeAlt() for more detailed function
 * description.
 *
 * @warning New applications should consider using RAIL_EnableDirectModeAlt() for
 *          this functionality.
 *
 * @note This feature is only available on certain devices.
 *   \ref RAIL_SupportsDirectMode() can be used to check if a particular
 *   device supports this feature or not.
 *
 * @warning This API is not safe to use in a true multiprotocol app.
 */
RAIL_Status_t RAIL_EnableDirectMode(RAIL_Handle_t railHandle,
                                    bool enable);

/**
 * Enable or disable direct mode for RAIL.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] enableDirectTx Enable direct mode for data being transmitted out
 *  of the radio.
 * @param[in] enableDirectRx Enable direct mode for data being received from the
 *  radio.
 * @return \ref RAIL_STATUS_NO_ERROR on success and an error code on failure.
 *
 * This API enables or disables the modem and GPIOs for direct mode operation.
 * see \ref RAIL_ConfigDirectMode for information on selecting the
 * correct hardware configuration.  If direct mode is enabled,
 * packets are output and input directly to the radio via GPIO
 * and RAIL packet handling is ignored.
 *
 * @note This feature is only available on certain chips.
 *   \ref RAIL_SupportsDirectMode() can be used to check if a particular
 *   chip supports this feature or not.
 *
 * @warning this API is not safe to use in a true multiprotocol app.
 */
RAIL_Status_t RAIL_EnableDirectModeAlt(RAIL_Handle_t railHandle,
                                       bool enableDirectTx,
                                       bool enableDirectRx);

/**
 * Get the radio subsystem clock frequency in Hz.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return Radio subsystem clock frequency in Hz.
 */
uint32_t RAIL_GetRadioClockFreqHz(RAIL_Handle_t railHandle);

/**
 * Set the crystal tuning.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] tune A chip-dependent crystal capacitor bank tuning parameter.
 * @return Status code indicating success of the function call.
 *
 * Tunes the crystal that the radio depends on to change the location of the
 * center frequency for transmitting and receiving. This function will only
 * succeed if the radio is idle at the time of the call.
 *
 * @note This function proportionally affects the entire chip's timing
 *   across all its peripherals, including radio tuning and channel spacing.
 *   A separate function, \ref RAIL_SetFreqOffset(), can be used to adjust
 *   just the radio tuner without disturbing channel spacing or other chip
 *   peripheral timing.
 * @note On EFR32xG2 series devices, this API sets CTUNEXIANA and internally
 *   CTUNEXOANA = CTUNEXIANA + delta where delta is set or changed by
 *   \ref RAIL_SetTuneDelta. The default delta may not be 0 on some devices.
 */
RAIL_Status_t RAIL_SetTune(RAIL_Handle_t railHandle, uint32_t tune);

/**
 * Get the crystal tuning.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return A chip-dependent crystal capacitor bank tuning parameter.
 *
 * Retrieves the current tuning value used by the crystal that the radio
 * depends on.
 * @note On EFR32xG2 series devices, this is the CTUNEXIANA value.
 */
uint32_t RAIL_GetTune(RAIL_Handle_t railHandle);

/**
 * Set the crystal tuning delta.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] delta A chip-dependent crystal capacitor bank tuning delta.
 * @return Status code indicating success of the function call.
 *
 * Set the CTUNEXOANA delta for \ref RAIL_SetTune to use on EFR32xG2 series devices:
 * CTUNEXOANA = CTUNEXIANA + delta. This function does not change CTUNE values;
 * call \ref RAIL_SetTune to put a new delta into effect.
 *
 */
RAIL_Status_t RAIL_SetTuneDelta(RAIL_Handle_t railHandle, int32_t delta);

/**
 * Get the crystal tuning delta on EFR32xG2 series devices.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return A chip-dependent crystal capacitor bank tuning delta.
 *
 * Retrieves the current tuning delta used by \ref RAIL_SetTune.
 * @note The default delta if \ref RAIL_SetTuneDelta has never been called
 *   is device-dependent and may not be 0.
 */
int32_t RAIL_GetTuneDelta(RAIL_Handle_t railHandle);

/**
 * Get the frequency offset.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return Returns the measured frequency offset on a received packet.
 *   The units are described in the \ref RAIL_FrequencyOffset_t
 *   documentation. If this returns \ref RAIL_FREQUENCY_OFFSET_INVALID,
 *   it was called while the radio wasn't active and there is no way
 *   to get the frequency offset.
 *
 * Retrieves the measured frequency offset used during the previous
 * received packet, which includes the current radio frequency offset
 * (see \ref RAIL_SetFreqOffset()). If the chip has not been in RX,
 * it returns the nominal radio frequency offset.
 *
 * @note Changing to any non-idle radio state after reception can cause this
 * value to be overwritten so it is safest to capture during packet reception.
 */
RAIL_FrequencyOffset_t RAIL_GetRxFreqOffset(RAIL_Handle_t railHandle);

/**
 * Set the nominal radio frequency offset.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] freqOffset \ref RAIL_FrequencyOffset_t parameter (signed, 2's
 *   complement).
 * @return Status code indicating success of the function call.
 *
 * This function is used to adjust the radio's tuning frequency slightly up or down.
 * It might be used in conjunction with \ref RAIL_GetRxFreqOffset() after
 * receiving a packet from a peer to adjust the tuner to better match the
 * peer's tuned frequency.
 *
 * @note Unlike \ref RAIL_SetTune(), which affects the entire chip's
 *   timing including radio tuning and channel spacing, this function
 *   only affects radio tuning without disturbing channel spacing or
 *   other chip peripheral timing.
 */
RAIL_Status_t RAIL_SetFreqOffset(RAIL_Handle_t railHandle,
                                 RAIL_FrequencyOffset_t freqOffset);

/**
 * Start transmitting a stream on a certain channel.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] channel A channel on which to emit a stream.
 * @param[in] mode Choose the stream mode (PN9, and so on).
 * @return Status code indicating success of the function call.
 *
 * Begins streaming onto the given channel. The sources can either be an
 * unmodulated carrier wave or an encoded stream of bits from a PN9 source.
 * All ongoing radio operations will be stopped before transmission begins.
 */
RAIL_Status_t RAIL_StartTxStream(RAIL_Handle_t railHandle,
                                 uint16_t channel,
                                 RAIL_StreamMode_t mode);

/**
 * Start transmitting a stream on a certain channel with the ability to select
 * an antenna.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] channel A channel on which to emit a stream.
 * @param[in] mode Choose the stream mode (PN9, and so on).
 * @param[in] options Choose the TX Antenna option.
 * Takes only \ref RAIL_TX_OPTION_ANTENNA0, \ref RAIL_TX_OPTION_ANTENNA1,
 * \ref RAIL_TX_OPTIONS_DEFAULT or \ref RAIL_TX_OPTIONS_NONE from the
 * \ref RAIL_TxOptions_t. If some other value is used then, transmission
 * is possible on any antenna.
 * @return Status code indicating success of the function call.
 *
 * Begins streaming onto the given channel. The sources can either be an
 * unmodulated carrier wave or an encoded stream of bits from a PN9 source.
 * All ongoing radio operations will be stopped before transmission begins.
 */
RAIL_Status_t RAIL_StartTxStreamAlt(RAIL_Handle_t railHandle,
                                    uint16_t channel,
                                    RAIL_StreamMode_t mode,
                                    RAIL_TxOptions_t options);

/**
 * Stop stream transmission.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return Status code indicating success of the function call.
 *
 * Halts the transmission started by RAIL_StartTxStream().
 */
RAIL_Status_t RAIL_StopTxStream(RAIL_Handle_t railHandle);

/**
 * Stop infinite preamble transmission started and start transmitting the rest
 * of the packet.
 *
 * This function is only useful for radio configurations that specify an
 * infinite preamble. Call this API only after \ref RAIL_EVENT_TX_STARTED
 * has occurred and the radio is transmitting.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return Status code indicating success of the function call:
 *    \ref RAIL_STATUS_NO_ERROR if infinite preamble was stopped;
 *    \ref RAIL_STATUS_INVALID_CALL if the radio isn't configured for infinite
 *         preamble;
 *    \ref RAIL_STATUS_INVALID_STATE if the radio isn't transmitting.
 */
RAIL_Status_t RAIL_StopInfinitePreambleTx(RAIL_Handle_t railHandle);

/**
 * Configure the verification of radio memory contents.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in,out] configVerify A configuration structure made available to
 *   RAIL to perform radio state verification. This structure must be
 *   allocated in application global read-write memory. RAIL may modify
 *   fields within or referenced by this structure during its operation.
 * @param[in] radioConfig A pointer to a radioConfig that is to be used as a
 *   white list for verifying memory contents.
 * @param[in] cb A callback that notifies the application of a mismatch in
 *   expected vs actual memory contents. A NULL parameter may be passed in
 *   if a callback is not provided by the application.
 * @return \ref RAIL_STATUS_NO_ERROR if setup of the verification feature
 *   successfully occurred.
 *   \ref RAIL_STATUS_INVALID_PARAMETER is returned if the provided railHandle
 *   or configVerify structures are invalid.
 */
RAIL_Status_t RAIL_ConfigVerification(RAIL_Handle_t railHandle,
                                      RAIL_VerifyConfig_t *configVerify,
                                      const uint32_t *radioConfig,
                                      RAIL_VerifyCallbackPtr_t cb);

/**
 * Verify radio memory contents.
 *
 * @param[in,out] configVerify A configuration structure made available to
 *   RAIL to perform radio state verification. This structure must be
 *   allocated in application global read-write memory. RAIL may modify
 *   fields within or referenced by this structure during its operation.
 * @param[in] durationUs The duration (in microseconds) for how long memory
 *   verification should occur before returning to the application. A value of
 *   RAIL_VERIFY_DURATION_MAX indicates that all memory contents should be
 *   verified before returning to the application.
 * @param[in] restart This flag only has meaning if a previous call of this
 *   function returned \ref RAIL_STATUS_SUSPENDED. By restarting (true), the
 *   verification process starts over from the beginning, or by resuming
 *   where verification left off after being suspended (false), verification
 *   can proceed towards completion.
 * @return \ref RAIL_STATUS_NO_ERROR if the contents of all applicable
 *   memory locations have been verified.
 *   \ref RAIL_STATUS_SUSPENDED is returned if the provided test duration
 *   expired but the time was not sufficient to verify all memory contents.
 *   By calling \ref RAIL_Verify again, further verification will commence.
 *   \ref RAIL_STATUS_INVALID_PARAMETER is returned if the provided
 *   verifyConfig structure pointer is not configured for use by the active
 *   RAIL handle.
 *   \ref RAIL_STATUS_INVALID_STATE is returned if any of the verified
 *   memory contents are different from their reference values.
 */
RAIL_Status_t RAIL_Verify(RAIL_VerifyConfig_t *configVerify,
                          uint32_t durationUs,
                          bool restart);

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/**
 * A global pointer to the memory address of the internal RAIL hardware timer
 * that drives the RAIL timebase.
 *
 * @note The corresponding timer tick value is not adjusted for overflow or the
 *   clock period, and will simply be a register read. The ticks wrap after
 *   about 17 minutes since it does not use the full 32-bit range.
 *   For more details, check the documentation for \ref RAIL_TimerTick_t.
 */
extern const volatile RAIL_TimerTick_t *RAIL_TimerTick;

/**
 * Get elapsed time, in microseconds, between two \ref RAIL_TimerTick_t ticks.
 *
 * @param[in] startTick Tick recorded at the start of the operation.
 * @param[in] endTick Tick recorded at the end of the operation.
 *
 * @return Returns the elapsed time, in microseconds, between two timer ticks.
 */
RAIL_Time_t RAIL_TimerTicksToUs(RAIL_TimerTick_t startTick,
                                RAIL_TimerTick_t endTick);

/**
 * Get \ref RAIL_TimerTick_t tick corresponding to the \ref RAIL_Time_t time.
 *
 * @param[in] microseconds Time in microseconds.
 *
 * @return Returns the \ref RAIL_TimerTick_t tick corresponding to the
 *   \ref RAIL_Time_t time.
 */
RAIL_TimerTick_t RAIL_UsToTimerTicks(RAIL_Time_t microseconds);

/**
 * Enable Radio state change interrupt.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] enable Enable/disable Radio state change interrupt.
 * @return Status code indicating success of the function call. Returns
 *   \ref RAIL_STATUS_NO_ERROR once the interrupt has been enabled or disabled.
 *
 * @note If enabled, state change events are reported through the separate
 *   RAILCb_RadioStateChanged() callback.
 */
RAIL_Status_t RAIL_EnableRadioStateChanged(RAIL_Handle_t railHandle,
                                           bool enable);

/**
 * Get the current radio state.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return An enumeration, \ref RAIL_RadioStateEfr32_t, for the current radio
 *   state.
 *
 */
RAIL_RadioStateEfr32_t RAIL_GetRadioStateAlt(RAIL_Handle_t railHandle);

#endif//DOXYGEN_SHOULD_SKIP_THIS

/** @} */ // end of group Diagnostic

/******************************************************************************
 * Energy Friendly Front End Module (EFF)
 *****************************************************************************/
/**
 * @addtogroup EFF Energy Friendly Front End Module (EFF)
 * @brief APIs for configuring and controlling an attached Energy Friendly Front
 * End Module (EFF).
 *
 * The EFF is a high-performance, transmit/receive (T/R) front end module (FEM)
 * for sub-GHz EFR32 devices. RAIL includes built-in functionality to transmit
 * and receive via an attached EFF. This functionality optimizes RF performance
 * while ensuring that the EFF stays within safe operating temperature limits.
 *
 * Configuration and control of the EFF is performed by the \ref rail_util_eff.
 *
 * @note The EFF is only supported with EFR32XG25 devices.
 * @{
 */

/**
 * Configure the attached EFF device.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] config  A pointer to a \ref RAIL_EffConfig_t struct that contains
 *                    configuration data for the EFF.
 * @return Status code indicating success of the function call.
 */

RAIL_Status_t RAIL_ConfigEff(RAIL_Handle_t railHandle,
                             const RAIL_EffConfig_t *config);

/** Number of temperature values provided by \ref RAIL_GetTemperature(). */
#define RAIL_TEMP_MEASURE_COUNT  (13U)

/**
 * Get the different temperature measurements in Kelvin done by sequencer or host.
 * Values that are not populated yet or incorrect are set to 0.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[out] tempBuffer The address of the array that will contain temperatures.
 *    tempBuffer array must be at least \ref RAIL_TEMP_MEASURE_COUNT int16_t.
 * @param[in] reset Reset the EFF temperature values.
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_GetTemperature(RAIL_Handle_t railHandle,
                                  int16_t tempBuffer[RAIL_TEMP_MEASURE_COUNT],
                                  bool reset);

/**
 * Set the FEM maximum power for continuous TX and the duty cycle for TX operations
 * from \ref RAIL_FemProtectionConfig_t.
 * Default continuous TX power is defined at 200 deci-dBm and can be set to a
 * maximum of 300 deci-dBm.
 * Default duty cycle is defined at 50 percent and can be set to a maximum of
 * 100 percent.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] newFemConfig Pointer to the new limits to use for the FEM protection configuration
 * @return Status code indicating success of the function call.
 *
 * @note Setting the power for continuous TX and the duty cycle from
 * \ref RAIL_FemProtectionConfig_t to 0 will apply their default value.
 * For an ambient temperature above 70 degrees Celsius, it is recommended to use the default configuration.
 */
RAIL_Status_t RAIL_SetFemProtectionConfig(RAIL_Handle_t railHandle, const RAIL_FemProtectionConfig_t *newFemConfig);

/**
 * Get the FEM protection configuration.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[out] femConfig Current FEM protection configuration written to this pointer.
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_GetFemProtectionConfig(RAIL_Handle_t railHandle, RAIL_FemProtectionConfig_t *femConfig);

/** Number of bytes provided by \ref RAIL_GetSetEffControl(). */
#define RAIL_EFF_CONTROL_SIZE (52U)

/**
 * Get the different EFF Control measurements.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[out] tempBuffer The address of the target array.
 *    tempBuffer array must be at least \ref RAIL_EFF_CONTROL_SIZE bytes.
 * @param[in] reset Reset the EFF Control measurements.
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_GetSetEffControl(RAIL_Handle_t railHandle,
                                    uint16_t tempBuffer[RAIL_EFF_CONTROL_SIZE / sizeof(uint16_t)],
                                    bool reset);

/**
 * Copy the current FEM_DATA pin values into newMode. If changeMode is true,
 * update FEM_DATA pin values with newMode first.
 *
 * @param[in] railHandle A RAIL instance handle
 * @param[in,out] newMode  A pointer to a uint8_t that will contain the FEM_DATA pin values
 * @param[in] changeMode If true, use newMode to update FEM_DATA pin values
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_GetSetEffMode(RAIL_Handle_t railHandle,
                                 uint8_t *newMode,
                                 bool changeMode);

/**
 * Copy the current Rural to Urban trip voltage into newTrip. If changeTrip is true,
 * update current Rural to Urban trip voltage with newTrip first.
 *
 * @param[in] railHandle A RAIL instance handle
 * @param[in,out] newTrip  A pointer to a uint16_t that will contain the Rural to Urban trip voltage, in millivolts
 * @param[in] changeTrip If true, use newTrip to update current Rural to Urban trip voltage
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_GetSetEffRuralUrbanMv(RAIL_Handle_t railHandle,
                                         uint16_t *newTrip,
                                         bool changeTrip);

/**
 * Copy the current Urban to Bypass trip voltage into newTrip. If changeTrip is true,
 * update current Urban to Bypass trip voltage with newTrip first.
 *
 * @param[in] railHandle A RAIL instance handle
 * @param[in,out] newTrip  A pointer to a uint16_t that will contain the Urban to Bypass trip voltage, in millivolts
 * @param[in] changeTrip If true, use newTrip to update current Urban to Bypass trip voltage
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_GetSetEffUrbanBypassMv(RAIL_Handle_t railHandle,
                                          uint16_t *newTrip,
                                          bool changeTrip);

/**
 * Copy the current Urban dwell time into newDwellTime. If changeDwellTime is true,
 * update current Urban dwell time with newDwellTime first.
 *
 * @param[in] railHandle A RAIL instance handle
 * @param[in,out] newDwellTime  A pointer to a uint16_t that will contain the Urban dwell
 *   dwell time, in milliseconds
 * @param[in] changeDwellTime If true, use newDwellTime to update current Urban dwell time
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_GetSetEffUrbanDwellTimeMs(RAIL_Handle_t railHandle,
                                             uint32_t *newDwellTime,
                                             bool changeDwellTime);

/**
 * Copy the current Bypass dwell time into newDwellTime. If changeDwellTime is true,
 * update current Bypass dwell time with newDwellTime first.
 *
 * @param[in] railHandle A RAIL instance handle
 * @param[in,out] newDwellTime  A pointer to a uint16_t that will contain the Bypass dwell
 *   dwell time, in milliseconds
 * @param[in] changeDwellTime If true, use newDwellTime to update current Bypass dwell time
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_GetSetEffBypassDwellTimeMs(RAIL_Handle_t railHandle,
                                              uint32_t *newDwellTime,
                                              bool changeDwellTime);

/**
 * Copy the current CLPC Slow Loop values into newTarget and newSlope. If changeTarget is true,
 * update current CLPC Slow Loop Target and Slope first.
 *
 * @param[in] railHandle A RAIL instance handle
 * @param[in,out] newTarget  A pointer to a uint16_t that will contain the CLPC Slow Loop Target
 * @param[in,out] newSlope  A pointer to a uint16_t that will contain the CLPC Slow Loop Slope
 * @param[in] changeValues If true, use newTarget and newGain to update the CLPC SLow Loop values
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_GetSetClpcSlowLoop(RAIL_Handle_t railHandle,
                                      uint16_t *newTarget,
                                      uint16_t *newSlope,
                                      bool changeValues);

/**
 * Copy the current CLPC Fast Loop values into newTarget and newSlope. If changeTarget is true,
 * update current CLPC Fast Loop Target and Slope first.
 *
 * @param[in] railHandle A RAIL instance handle
 * @param[in,out] newTarget  A pointer to a uint16_t that will contain the CLPC Fast Loop Target
 * @param[in,out] newSlope  A pointer to a uint16_t that will contain the CLPC Fast Loop Slope
 * @param[in] changeValues If true, use newTarget and newGain to update the CLPC Fast Loop values
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_GetSetClpcFastLoop(RAIL_Handle_t railHandle,
                                      uint16_t *newTarget,
                                      uint16_t *newSlope,
                                      bool changeValues);

/**
 * Copy the current CLPC Enable in to newClpcEnable. If changeClpcEnable is true,
 * update current CLPC Enable with newClpcEnable first.
 *
 * @param[in] railHandle A RAIL instance handle
 * @param[in,out] newClpcEnable  A pointer to a uint8_t that will contain the CLPC Enable
 * @param[in] changeClpcEnable If true, use newClpcEnable to update the current CLPC Enable
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_GetSetClpcEnable(RAIL_Handle_t railHandle,
                                    uint8_t *newClpcEnable,
                                    bool changeClpcEnable);

/**
 * Get and set the EFF temperature threshold.
 *
 * The EFR32 device periodically takes temperature measurements of the attached
 * EFF device. If the EFF temperature ever exceeds the EFF temperature
 * threshold, any active transmit operation is aborted and future transmit
 * operations are blocked until the EFF temperature falls below the threshold.
 *
 * @param[in] railHandle A RAIL instance handle
 * @param[in,out] newThreshold A pointer to a uint16_t that will contain the
 *   EFF temperature threshold, in Kelvin
 * @param[in] changeThreshold If true, use changeThreshold to update current
 *   EFF temperature threshold
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_GetSetEffTempThreshold(RAIL_Handle_t railHandle,
                                          uint16_t *newThreshold,
                                          bool changeThreshold);

/**
 * Get and set the internal temperature threshold.
 *
 * The EFR32 device periodically takes measurements of its internal temperature
 * when attached to an EFF device. If the internal temperature ever exceeds the
 * internal temperature threshold, any active transmit operation is aborted and
 * future transmit operations are blocked until the internal temperature falls
 * below the threshold.
 *
 * @param[in] railHandle A RAIL instance handle
 * @param[in,out] newThreshold A pointer to a uint16_t that will contain the
 *   internal temperature threshold, in Kelvin
 * @param[in] changeThreshold If true, use changeThreshold to update current
 *   internal temperature threshold
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_GetSetInternalTempThreshold(RAIL_Handle_t railHandle,
                                               uint16_t *newThreshold,
                                               bool changeThreshold);

/** @} */ // end of group EFF

#ifndef DOXYGEN_SHOULD_SKIP_THIS

/******************************************************************************
 * Debug
 *****************************************************************************/
/**
 * @addtogroup Debug
 * @brief APIs for debugging
 * @{
 */

/**
 * Configure the debug mode for the radio library. Do not use this function
 * unless instructed by Silicon Labs.
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] debugMode Debug mode to enter.
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_SetDebugMode(RAIL_Handle_t railHandle, uint32_t debugMode);

/**
 * Return the debug mode for the radio library. Do not use this function
 * unless instructed by Silicon Labs.
 * @param[in] railHandle A RAIL instance handle.
 * @return Debug mode for the radio library.
 */
uint32_t RAIL_GetDebugMode(RAIL_Handle_t railHandle);

/**
 * Override the radio base frequency.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] freq A desired frequency in Hz.
 * @return Status code indicating success of the function call.
 *
 * Sets the radio to transmit at the frequency given. This function can only
 * be used while in \ref RAIL_DEBUG_MODE_FREQ_OVERRIDE. The given frequency
 * needs to be close to the base frequency of the current PHY. After this
 * call, a full reset is needed to restore normal RAIL operation.
 */
RAIL_Status_t RAIL_OverrideDebugFrequency(RAIL_Handle_t railHandle,
                                          uint32_t freq);

/**
 * Get the size of the radio's multiprotocol scheduler state buffer.
 *
 * @param[in] genericRailHandle A generic RAIL instance handle.
 * @return Size, in bytes, of the radio's internal scheduler state buffer.
 *   Zero is returned if the handle is invalid or this is the singleprotocol
 *   library.
 */
uint32_t RAIL_GetSchedBufferSize(RAIL_Handle_t genericRailHandle);

/** @} */ // end of group Debug

#endif//DOXYGEN_SHOULD_SKIP_THIS

/******************************************************************************
 * Assertion Callback
 *****************************************************************************/
/**
 * @addtogroup Assertions
 * @brief Callbacks called by assertions
 *
 * The assertion framework was implemented to not only
 * assert that certain conditions are true in a block of code, but also
 * to handle them more appropriately. In previous implementations,
 * the behavior upon a failed assert was to hang in a while(1) loop.
 * However, with the callback, each assert is given a unique error code so that
 * they can be handled on a more case-by-case basis. For documentation on each
 * of the errors, see the rail_assert_error_codes.h file.
 * RAIL_ASSERT_ERROR_MESSAGES[errorCode] gives the explanation of the error.
 * With asserts built into the library, users can choose how to handle each
 * error inside the callback.
 *
 * @{
 */

/**
 * Callback called upon failed assertion.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] errorCode Value passed in by the calling assertion API indicating
 *   the RAIL error that is indicated by the failing assertion.
 */
void RAILCb_AssertFailed(RAIL_Handle_t railHandle,
                         RAIL_AssertErrorCodes_t errorCode);

/** @} */ // end of group Assertions

/******************************************************************************
 * External_Thermistor
 *****************************************************************************/
/**
 * @addtogroup External_Thermistor External Thermistor
 * @brief APIs to measure temperature using an external thermistor
 *
 * This feature allows reading the temperature via an external thermistor on
 * chips that support it. This will require connecting the necessary components
 * and configuring the pins as required.
 *
 * @{
 */

/**
 * Start a Thermistor measurement. To get the Thermistor impedance, call the
 * function \ref RAIL_GetThermistorImpedance. On the EFR32xG22, this function
 * reconfigures GPIO_THMSW_EN_PIN located in GPIO_THMSW_EN_PORT. To locate this
 * pin, refer to the data sheet or appropriate header files of the device. For
 * proper operation, \ref RAIL_Init must be called before using this function.
 *
 * @note This function is not designed for safe usage in multiprotocol
 * applications.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_StartThermistorMeasurement(RAIL_Handle_t railHandle);

/**
 * Get the thermistor impedance measurement and returns \ref
 * RAIL_INVALID_THERMISTOR_VALUE if the thermistor is not properly
 * configured or the thermistor measurement is not ready.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[out] thermistorImpedance Current thermistor impedance measurement in
 * Ohms.
 * @return Status code indicating success of the function call.
 */
RAIL_Status_t RAIL_GetThermistorImpedance(RAIL_Handle_t railHandle,
                                          uint32_t *thermistorImpedance);

/**
 * Convert the thermistor impedance into temperature, in Celsius.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] thermistorImpedance Current thermistor impedance measurement in
 *   Ohms.
 * @param[out] thermistorTemperatureC Pointer to current thermistor temperature
 *   in eighth of Celsius degree
 * @return Status code indicating success of the function call.
 *
 * This function is provided in the rail_util_thermistor plugin to get
 * accurate values from our boards thermistors. For a custom board, this
 * function could be modified and re-implemented for other needs.
 * The variable railHandle is only used to indicate to the user from where the
 * function was called, so it is okay to use either a real protocol handle,
 * or one of the chip-specific ones, such as \ref RAIL_EFR32_HANDLE.
 *
 * @note This plugin is mandatory on EFR32xG25 platform.
 */
RAIL_Status_t RAIL_ConvertThermistorImpedance(RAIL_Handle_t railHandle,
                                              uint32_t thermistorImpedance,
                                              int16_t *thermistorTemperatureC);

/**
 * Compute the crystal PPM deviation from the thermistor temperature.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] crystalTemperatureC Current crystal temperature.
 * @param[out] crystalPPMError Pointer to current ppm error.
 * @return Status code indicating success of the function call.
 *
 * This function is provided in the rail_util_thermistor plugin to get
 * accurate values from our boards thermistor. For a custom board, this
 * function could be modified and re-implemented for other needs.
 * The variable railHandle is only used to indicate to the user from where the
 * function was called, so it is okay to use either a real protocol handle,
 * or one of the chip-specific ones, such as \ref RAIL_EFR32_HANDLE.
 *
 * @note This plugin is mandatory on EFR32xG25 platform.
 */
RAIL_Status_t RAIL_ComputeHFXOPPMError(RAIL_Handle_t railHandle,
                                       int16_t crystalTemperatureC,
                                       int8_t *crystalPPMError);

/**
 * Configure the GPIO for thermistor usage.
 *
 * @param railHandle A RAIL instance handle.
 * @param[in] hfxoThermistorConfig The thermistor configuration pointing to
 *   the GPIO port and pin to access.
 * @return Status code indicating success of the function call.
 *
 * @note The port and pin that must be passed in \ref RAIL_HFXOThermistorConfig_t
 * are GPIO_THMSW_EN_PORT and GPIO_THMSW_EN_PIN respectively.
 */
RAIL_Status_t RAIL_ConfigHFXOThermistor(RAIL_Handle_t railHandle,
                                        const RAIL_HFXOThermistorConfig_t *hfxoThermistorConfig);
/** @} */ // end of group External_Thermistor

/**
 * @addtogroup Features
 * @{
 */

/**
 * Indicate whether RAIL supports 2.4 GHz band operation on this chip.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if the 2.4 GHz band is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_2P4GHZ_BAND.
 */
bool RAIL_Supports2p4GHzBand(RAIL_Handle_t railHandle);

/**
 * Indicate whether RAIL supports SubGHz band operation on this chip.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if the SubGHz band is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_SUBGHZ_BAND.
 */
bool RAIL_SupportsSubGHzBand(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports dual 2.4 GHz and SubGHz band operation.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if the dual band is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_DUAL_BAND.
 */
bool RAIL_SupportsDualBand(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports bit masked address filtering
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if bit masked address filtering is supported; false otherwise.
 *
 * Runtime refinement of compile-time
 * \ref RAIL_SUPPORTS_ADDR_FILTER_ADDRESS_BIT_MASK.
 */
bool RAIL_SupportsAddrFilterAddressBitMask(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports address filter mask information
 * for incoming packets in
 * \ref RAIL_RxPacketInfo_t::filterMask and
 * \ref RAIL_IEEE802154_Address_t::filterMask.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if address filter information is supported; false otherwise
 *   (in which case \ref RAIL_RxPacketInfo_t::filterMask value is undefined).
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_ADDR_FILTER_MASK.
 */
bool RAIL_SupportsAddrFilterMask(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports alternate TX power settings.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if alternate TX power settings are supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref
 * RAIL_SUPPORTS_ALTERNATE_TX_POWER.
 */
bool RAIL_SupportsAlternateTxPower(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports antenna diversity.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if antenna diversity is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_ANTENNA_DIVERSITY.
 *
 * @note Certain radio configurations may not support this feature even
 *   if the chip in general claims to support it.
 */
bool RAIL_SupportsAntennaDiversity(RAIL_Handle_t railHandle);

/**
 * Indicate whether RAIL supports AUXADC measurements on this chip.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if AUXADC measurements are supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_AUXADC.
 */
bool RAIL_SupportsAuxAdc(RAIL_Handle_t railHandle);

/**
 * Indicate whether RAIL supports channel hopping on this chip.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if channel hopping is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_CHANNEL_HOPPING.
 */
bool RAIL_SupportsChannelHopping(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports direct mode.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if direct mode is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref
 * RAIL_SUPPORTS_DIRECT_MODE.
 */
bool RAIL_SupportsDirectMode(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports dual sync words.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if dual sync words are supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_DUAL_SYNC_WORDS.
 *
 * @note Certain radio configurations may not support this feature even
 *   if the chip in general claims to support it.
 */
bool RAIL_SupportsDualSyncWords(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports EFF.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if EFF identifier is supported; false otherwise.
 */
bool RAIL_SupportsEff(RAIL_Handle_t railHandle);

/**
 * Indicate whether RAIL supports thermistor measurements on this chip.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if thermistor measurements are supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_EXTERNAL_THERMISTOR.
 */
bool RAIL_SupportsExternalThermistor(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports MFM protocol.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if MFM protocol is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_MFM.
 */
bool RAIL_SupportsMfm(RAIL_Handle_t railHandle);

/**
 * Indicate whether RAIL supports OFDM band operation on this chip.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if OFDM operation is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_OFDM_PA.
 */
bool RAIL_SupportsOFDMPA(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports a high-precision LFRCO.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if high-precision LFRCO is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_PRECISION_LFRCO.
 */
bool RAIL_SupportsPrecisionLFRCO(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports radio entropy.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if radio entropy is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_RADIO_ENTROPY.
 */
bool RAIL_SupportsRadioEntropy(RAIL_Handle_t railHandle);

/**
 * Indicate whether RAIL supports RFSENSE Energy Detection Mode on this chip.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if RFSENSE Energy Detection Mode is supported; false otherwise.
 *
 * Runtime refinement of compile-time
 * \ref RAIL_SUPPORTS_RFSENSE_ENERGY_DETECTION.
 */
bool RAIL_SupportsRfSenseEnergyDetection(RAIL_Handle_t railHandle);

/**
 * Indicate whether RAIL supports RFSENSE Selective(OOK) Mode on this chip.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if RFSENSE Selective(OOK) Mode is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_RFSENSE_SELECTIVE_OOK.
 */
bool RAIL_SupportsRfSenseSelectiveOok(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports configurable RSSI threshold
 * set by \ref RAIL_SetRssiDetectThreshold().
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if setting configurable RSSI is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_RSSI_DETECT_THRESHOLD.
 */
bool RAIL_SupportsRssiDetectThreshold(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports RX direct mode data to FIFO.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if direct mode data to FIFO is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref
 * RAIL_SUPPORTS_RX_DIRECT_MODE_DATA_TO_FIFO.
 */
bool RAIL_SupportsRxDirectModeDataToFifo(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports raw RX data
 * sources other than \ref RAIL_RxDataSource_t::RX_PACKET_DATA.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if direct mode is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_RX_RAW_DATA.
 */
bool RAIL_SupportsRxRawData(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports SQ-based PHY.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if the SQ-based PHY is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_SQ_PHY.
 */
bool RAIL_SupportsSQPhy(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports a particular power mode (PA).
 * @note Consider using \ref RAIL_SupportsTxPowerModeAlt to also get the power
 * mode's lowest allowed power level.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in] powerMode The power mode to check if supported.
 * @param[out] pMaxPowerLevel A pointer to a \ref RAIL_TxPowerLevel_t that
 *   if non-NULL will be filled in with the power mode's highest power level
 *   allowed if this function returns true.
 * @return true if the powerMode is supported; false otherwise.
 *
 * This function has no compile-time equivalent.
 */
bool RAIL_SupportsTxPowerMode(RAIL_Handle_t railHandle,
                              RAIL_TxPowerMode_t powerMode,
                              RAIL_TxPowerLevel_t *pMaxPowerLevel);

/**
 * Indicate whether this chip supports a particular power mode (PA) and
 * provides the maximum and minimum power level for that power mode
 * if supported by the chip.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @param[in,out] powerMode A pointer to PA power mode to check if supported.
 *   In case of  RAIL_TX_POWER_MODE_2P4_HIGHEST or
 *   RAIL_TX_POWER_MODE_SUBGIG_HIGHEST is used as input
 *   then the highest selected PA on the chip is updated here.
 * @param[out] maxPowerLevel A pointer to a \ref RAIL_TxPowerLevel_t that
 *   if non-NULL will be filled in with the power mode's highest power level
 *   allowed if this function returns \ref RAIL_STATUS_NO_ERROR.
 * @param[out] minPowerLevel A pointer to a \ref RAIL_TxPowerLevel_t that
 *   if non-NULL will be filled in with the power mode's lowest power level
 *   allowed if this function returns \ref RAIL_STATUS_NO_ERROR.
 * @return true if powerMode is supported; false otherwise.
 */
bool RAIL_SupportsTxPowerModeAlt(RAIL_Handle_t railHandle,
                                 RAIL_TxPowerMode_t *powerMode,
                                 RAIL_TxPowerLevel_t *maxPowerLevel,
                                 RAIL_TxPowerLevel_t *minPowerLevel);

/**
 * Indicate whether this chip supports automatic TX to TX transitions.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if TX to TX transitions are supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_TX_TO_TX.
 */
bool RAIL_SupportsTxToTx(RAIL_Handle_t railHandle);

/**
 * Indicate whether RAIL supports the BLE protocol on this chip.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if BLE is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_PROTOCOL_BLE.
 */
bool RAIL_SupportsProtocolBLE(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports BLE 1Mbps Non-Viterbi PHY.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if BLE 1Mbps Non-Viterbi is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_BLE_SUPPORTS_1MBPS_NON_VITERBI.
 */
bool RAIL_BLE_Supports1MbpsNonViterbi(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports BLE 1Mbps Viterbi PHY.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if BLE 1Mbps Viterbi is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_BLE_SUPPORTS_1MBPS_VITERBI.
 */
bool RAIL_BLE_Supports1MbpsViterbi(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports BLE 1Mbps operation.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if BLE 1Mbps operation is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_BLE_SUPPORTS_1MBPS.
 */
static inline
bool RAIL_BLE_Supports1Mbps(RAIL_Handle_t railHandle)
{
  return (RAIL_BLE_Supports1MbpsNonViterbi(railHandle)
          || RAIL_BLE_Supports1MbpsViterbi(railHandle));
}

/**
 * Indicate whether this chip supports BLE 2Mbps Non-Viterbi PHY.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if BLE 2Mbps Non-Viterbi is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_BLE_SUPPORTS_2MBPS_NON_VITERBI.
 */
bool RAIL_BLE_Supports2MbpsNonViterbi(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports BLE 2Mbps Viterbi PHY.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if BLE 2Mbps Viterbi is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_BLE_SUPPORTS_2MBPS_VITERBI.
 */
bool RAIL_BLE_Supports2MbpsViterbi(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports BLE 2Mbps operation.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if BLE 2Mbps operation is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_BLE_SUPPORTS_2MBPS.
 */
static inline
bool RAIL_BLE_Supports2Mbps(RAIL_Handle_t railHandle)
{
  return (RAIL_BLE_Supports2MbpsNonViterbi(railHandle)
          || RAIL_BLE_Supports2MbpsViterbi(railHandle));
}

/**
 * Indicate whether this chip supports BLE Antenna Switching needed for
 * Angle-of-Arrival receives or Angle-of-Departure transmits.
 * @param[in] railHandle A RAIL instance handle.
 * @return true if BLE Antenna Switching is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_BLE_SUPPORTS_ANTENNA_SWITCHING.
 */
bool RAIL_BLE_SupportsAntennaSwitching(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports BLE Coded PHY used for Long-Range.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if BLE Coded PHY is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_BLE_SUPPORTS_CODED_PHY.
 */
bool RAIL_BLE_SupportsCodedPhy(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports BLE CTE (Constant Tone Extension)
 * needed for Angle-of-Arrival/Departure transmits.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if BLE CTE is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_BLE_SUPPORTS_CTE.
 */
bool RAIL_BLE_SupportsCte(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports BLE IQ Sampling needed for
 * Angle-of-Arrival/Departure receives.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if BLE IQ Sampling is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_BLE_SUPPORTS_IQ_SAMPLING.
 */
bool RAIL_BLE_SupportsIQSampling(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports BLE PHY switch to RX
 * functionality, which is used to switch BLE PHYs at a specific time
 * to receive auxiliary packets.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if BLE PHY switch to RX is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_BLE_SUPPORTS_PHY_SWITCH_TO_RX.
 */
bool RAIL_BLE_SupportsPhySwitchToRx(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports the Quuppa PHY.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if the Quuppa is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_BLE_SUPPORTS_QUUPPA.
 */
bool RAIL_BLE_SupportsQuuppa(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports BLE signal identifier.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if signal identifier is supported; false otherwise.
 */
bool RAIL_BLE_SupportsSignalIdentifier(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports BLE Simulscan PHY used for simultaneous
 * BLE 1Mbps and Coded PHY reception.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if BLE Simulscan PHY is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_BLE_SUPPORTS_SIMULSCAN_PHY.
 */
bool RAIL_BLE_SupportsSimulscanPhy(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports the IEEE 802.15.4 protocol.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if the 802.15.4 protocol is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_PROTOCOL_IEEE802154.
 */
bool RAIL_SupportsProtocolIEEE802154(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports the IEEE 802.15.4 Wi-Fi Coexistence PHY.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if the 802.15.4 COEX PHY is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_IEEE802154_SUPPORTS_COEX_PHY.
 */
bool RAIL_IEEE802154_SupportsCoexPhy(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports the IEEE 802.15.4 PHY with custom settings.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if the 802.15.4 PHY with custom settings is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_IEEE802154_SUPPORTS_CUSTOM1_PHY.
 */
bool RAIL_IEEE802154_SupportsCustom1Phy(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports the IEEE 802.15.4
 * front end module optimized PHY.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if a front end module is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_IEEE802154_SUPPORTS_FEM_PHY.
 */
bool RAIL_IEEE802154_SupportsFemPhy(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports canceling the frame-pending lookup
 * event \ref RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND when the radio
 * transitions to a state that renders the the reporting of this event moot
 * (i.e., too late for the stack to influence the outgoing ACK).
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if canceling the lookup event is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref
 * RAIL_IEEE802154_SUPPORTS_CANCEL_FRAME_PENDING_LOOKUP.
 */
bool RAIL_IEEE802154_SupportsCancelFramePendingLookup(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports early triggering of the frame-pending
 * lookup event \ref RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND
 * just after MAC address fields have been received.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if early triggering is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref
 * RAIL_IEEE802154_SUPPORTS_EARLY_FRAME_PENDING_LOOKUP.
 */
bool RAIL_IEEE802154_SupportsEarlyFramePendingLookup(RAIL_Handle_t railHandle);

/**
 * Indicate whether RAIL supports dual PA mode on this chip.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if the dual PA mode is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_IEEE802154_SUPPORTS_DUAL_PA_CONFIG.
 */
bool RAIL_IEEE802154_SupportsDualPaConfig(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports IEEE 802.15.4E-2012 Enhanced ACKing.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if 802.15.4E Enhanced ACKing is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref
 * RAIL_IEEE802154_SUPPORTS_E_ENHANCED_ACK.
 */
bool RAIL_IEEE802154_SupportsEEnhancedAck(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports IEEE 802.15.4E-2012 Multipurpose frame
 * reception.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if Multipurpose frame reception is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref
 * RAIL_IEEE802154_SUPPORTS_E_MULTIPURPOSE_FRAMES.
 */
bool RAIL_IEEE802154_SupportsEMultipurposeFrames(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports the IEEE 802.15.4E-2012 feature
 * subset needed for Zigbee R22 GB868.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if 802.15.4E GB868 subset is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref
 * RAIL_IEEE802154_SUPPORTS_E_SUBSET_GB868.
 */
bool RAIL_IEEE802154_SupportsESubsetGB868(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports IEEE 802.15.4G-2012 reception and
 * transmission of frames with 4-byte CRC.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if 802.15.4G 4-byte CRC is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref
 * RAIL_IEEE802154_SUPPORTS_G_4BYTE_CRC.
 */
bool RAIL_IEEE802154_SupportsG4ByteCrc(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports IEEE 802.15.4G dynamic FEC
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if dynamic FEC is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref
 * RAIL_IEEE802154_SUPPORTS_G_DYNFEC.
 */
bool RAIL_IEEE802154_SupportsGDynFec(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports Wi-SUN mode switching
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if Wi-SUN mode switching is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref
 * RAIL_IEEE802154_SUPPORTS_G_MODESWITCH.
 */
bool RAIL_IEEE802154_SupportsGModeSwitch(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports IEEE 802.15.4G-2012 feature
 * subset needed for Zigbee R22 GB868.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if 802.15.4G GB868 subset is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref
 * RAIL_IEEE802154_SUPPORTS_G_SUBSET_GB868.
 */
bool RAIL_IEEE802154_SupportsGSubsetGB868(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports IEEE 802.15.4G-2012 reception
 * of unwhitened frames.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if 802.15.4G unwhitened frame reception is supported;
 *   false otherwise.
 *
 * Runtime refinement of compile-time \ref
 * RAIL_IEEE802154_SUPPORTS_G_UNWHITENED_RX.
 */
bool RAIL_IEEE802154_SupportsGUnwhitenedRx(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports IEEE 802.15.4G-2012 transmission
 * of unwhitened frames.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if 802.15.4G unwhitened frame transmit is supported;
 *   false otherwise.
 *
 * Runtime refinement of compile-time \ref
 * RAIL_IEEE802154_SUPPORTS_G_UNWHITENED_TX.
 */
bool RAIL_IEEE802154_SupportsGUnwhitenedTx(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports the Z-Wave protocol.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if the Z-Wave protocol is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_SUPPORTS_PROTOCOL_ZWAVE.
 */
bool RAIL_SupportsProtocolZWave(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports the Z-Wave concurrent PHY.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if the Z-Wave concurrent PHY is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_ZWAVE_SUPPORTS_CONC_PHY.
 */
bool RAIL_ZWAVE_SupportsConcPhy(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports the Z-Wave energy detect PHY.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if the Z-Wave energy detect PHY is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_ZWAVE_SUPPORTS_ED_PHY.
 */
bool RAIL_ZWAVE_SupportsEnergyDetectPhy(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports Z-Wave Region in PTI.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if ZWAVE Region in PTI is supported; false otherwise.
 *
 * Runtime refinement of compile-time \ref RAIL_ZWAVE_SUPPORTS_REGION_PTI.
 */
bool RAIL_ZWAVE_SupportsRegionPti(RAIL_Handle_t railHandle);

/**
 * Indicate whether this chip supports IEEE 802.15.4 signal identifier.
 *
 * @param[in] railHandle A RAIL instance handle.
 * @return true if signal identifier is supported; false otherwise.
 */
bool RAIL_IEEE802154_SupportsSignalIdentifier(RAIL_Handle_t railHandle);

/** @} */ // end of group Features

/** @} */ // end of group RAIL_API

#ifdef __cplusplus
}
#endif

#endif // __RAIL_H__
