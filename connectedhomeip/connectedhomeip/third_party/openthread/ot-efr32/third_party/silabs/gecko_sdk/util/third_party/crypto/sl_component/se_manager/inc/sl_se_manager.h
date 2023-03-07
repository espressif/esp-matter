/***************************************************************************//**
 * @file
 * @brief Silicon Labs Secure Engine Manager API.
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
#ifndef SL_SE_MANAGER_H
#define SL_SE_MANAGER_H

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT) || defined(DOXYGEN)

/***************************************************************************//**
 * @addtogroup sl_se_manager Secure Engine Manager
 *
 * @note The APIs are thread-safe.
 *
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup sl_se_manager_core Core
 *
 * @brief
 *   Secure Engine Manager Core API
 *
 * @details
 *   API for initialization of SE Manager and SE command context with yield
 *   attribute.
 *
 * @{
 ******************************************************************************/

#if !defined(SL_CATALOG_TZ_SECURE_KEY_LIBRARY_NS_PRESENT)
  #include "sl_se_manager_key_handling.h"
  #include "sl_se_manager_cipher.h"
#endif // SL_CATALOG_TZ_SECURE_KEY_LIBRARY_NS_PRESENT
#include "sl_se_manager_types.h"

#include "em_se.h"
#include "sl_status.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Prototypes

/***************************************************************************//**
 * @brief
 *   Initialize the SE Manager.
 *
 * @details
 *   Initialize the SE Manager by checking hardware availability and setting up
 *   internal module specific resources like mutexes etc.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_init(void);

/***************************************************************************//**
 * @brief
 *   Denitialize the SE Manager.
 *
 * @details
 *   Free resources held by the SE Manager.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_deinit(void);

#if !defined(SL_CATALOG_TZ_SECURE_KEY_LIBRARY_NS_PRESENT) || defined(DOXYGEN)
/***************************************************************************//**
 * @brief
 *   Set the yield attribute of the SE command context object.
 *
 * @param[in,out] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @param[in] yield
 *   The user may set this parameter to true in order to tell the SE Manager
 *   to yield the cpu core while waiting for the SE mailbox command to complete.
 *   If false, the SE Manager will busy-wait, by polling the SE mailbox response
 *   register until the SE mailbox command completes.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_set_yield(sl_se_command_context_t *cmd_ctx,
                            bool yield);
#endif // !SL_CATALOG_TZ_SECURE_KEY_LIBRARY_NS_PRESENT || DOXYGEN

#if defined(CRYPTOACC_PRESENT) || defined(DOXYGEN)
/***************************************************************************//**
 * @brief
 *   From VSE mailbox read which command, if any, was executed.
 *
 * @param[in,out] cmd_ctx
 *   Pointer to an SE command context object. If this function returns
 *   SL_STATUS_OK the command word of the SE command context object will be set.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_read_executed_command(sl_se_command_context_t *cmd_ctx);

/***************************************************************************//**
 * @brief
 *   Acknowledge and get status and output data of a completed command.
 *
 * @details
 *   This function acknowledges and gets the status and output data of a
 *   completed mailbox command. The acknowledge operation invalidates the
 *   contents of the output mailbox. The output data is copied into the linked
 *   list of output buffers pointed to in the given command data structure.
 *
 * @param[in,out] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_ack_command(sl_se_command_context_t *cmd_ctx);
#endif //defined(CRYPTOACC_PRESENT)

/***************************************************************************//**
 * @brief
 *   Initialize an SE command context object
 *
 * @details
 *   Initialize an SE command context which can be used in subsequent calls to
 *   the SE Manager API in order to execute SE mailbox commands.
 *
 * @param[in,out] cmd_ctx
 *   Pointer to an SE command context object to be initialized. This context
 *   object should be used in subsequent calls to the SE Manager API in order
 *   to execute SE mailbox commands. The same command context object cannot be
 *   used concurrently, e.g. by two different threads. However a command context
 *   object may be reused for the next and any subsequent mailbox operatons,
 *   except when streaming commands are in progress in which case only streaming
 *   commands of the same operation type is allowed until the streaming operation
 *   is finished (i.e. the corresponding sl_se_xxx_finish is called).
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_init_command_context(sl_se_command_context_t *cmd_ctx);

/***************************************************************************//**
 * @brief
 *   De-initialize an SE command context
 *
 * @details
 *   De-initialize an SE command context object.
 *
 * @param[in,out] cmd_ctx
 *   Pointer to an SE command context object to deinitialize.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sl_se_deinit_command_context(sl_se_command_context_t *cmd_ctx);

#ifdef __cplusplus
}
#endif

/// @} (end addtogroup sl_se_manager_core)
/// @} (end addtogroup sl_se_manager)

#endif // defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)

#endif // SL_SE_MANAGER_H

// THE REST OF THE FILE IS DOCUMENTATION ONLY
/// @addtogroup sl_se_manager Secure Engine Manager API
/// @brief Silicon Labs Secure Engine Manager
/// @{
///
/// @details
/// # Introduction
///
/// The Secure Engine (SE) Manager provides thread-safe APIs for the Secure Engine's mailbox interface. Note that PSA Crypto is the main device independant crypto API and should be used
/// whenever possible, see @ref ls_psa_usage. However, the SE manager APIs can be used directly for performance or space constrained applications.

/// Available functionality will vary between devices: device management, such as secure firmware upgrade, secure boot and secure debug implementation, is available on all series 2 devices.
/// Devices with the SE subsystem includes a low level crypto API where the SE Manager will use the SE hardware peripherals to accelerate cryptographic operations. Finally, Vault High
/// devices also include secure key storage functionality, anti-tamper protection, advanced crypto API and attestation.
///
/// @note Below are some of the useful application notes linked with Secure Engine Manager:\n
/// <a href="https://www.silabs.com/documents/public/application-notes/an1190-efr32-secure-debug.pdf">AN1190: Series 2 Secure Debug</a>\n
/// <a href="https://www.silabs.com/documents/public/application-notes/an1247-efr32-secure-vault-tamper.pdf">AN1247: Anti-Tamper Protection Configuration and Use</a>\n
/// <a href="https://www.silabs.com/documents/public/application-notes/an1268-efr32-secure-identity.pdf">AN1268: Authenticating Silicon Labs Devices Using Device Certificates</a>\n
/// <a href="https://www.silabs.com/documents/public/application-notes/an1271-efr32-secure-key-storage.pdf">AN1271: Secure Key Storage</a>\n
/// <a href="https://www.silabs.com/documents/public/application-notes/an1218-secure-boot-with-rtsl.pdf">AN1218: Series 2 Secure Boot with RTSL</a>\n
///
/// # Functionality
///
/// The functionality of the SE Manager includes
///
/// - Core API, inititalizing of SE Manager and SE command context (@ref sl_se_manager_core)
/// - Secure key storage (@ref sl_se_manager_key_handling)
///     - Key wrapping
///     - Storing keys in the SE volatile storage
///     - Using key by reference
/// - Configuration of tamper responses (@ref sl_se_manager_util)
///     - The available signals include core hard-fault, glitch detectors, PRS, and failed authenticated commands,
///       while the responses vary from triggering an interrupt to the hardware autonomously erasing the one-time-programmable (OTP) memory
/// - Block ciphers (@ref sl_se_manager_cipher)
///     - Supports AES-ECB, AES-CBC, AES-CFB128, AES-CFB8, AES-CTR, AES-CCM, AES-GCM, CMAC, HMAC and ChaCha20/Poly1305
///     - The cipher operations can be performed using plaintext keys, wrapped keys or referencing a key in the SE
///     - Streaming operations are supported for AES-GCM and CMAC
/// - Block and streaming hash operations (@ref sl_se_manager_hash)
///     - Supports SHA-1, SHA-224, SHA-256, SHA-384 and SHA-512
/// - True Random Number Generator (@ref sl_se_manager_entropy)
///     - Hardware block inside the SE used for generating random numbers. Can be used as a source of entropy, or to securely generate keys inside the SE
/// - Elliptic Curve Signature operation (@ref sl_se_manager_signature)
///     - ECDSA and EDDSA
/// - Key agreement (@ref sl_se_manager_key_derivation)
///     - Perform Elliptic Curve Diffie-Hellman and J-PAKE key agreement operations inside the SE
/// - Key derivation functions (@ref sl_se_manager_key_derivation)
///     - Perform HKDF and PBKDF2 key derivation functions inside the SE
/// - Device configuration and utilities (@ref sl_se_manager_util)
///     - Write to user data stored inside the SE
///     - Configuration of debug locks
///     - Configuration of secure boot
///     - Configuration of flash lock
///     - Read SE OTP contents
///     - Read SE firmware version
///     - Read provisioned certificates
/// - Multi-thread safe APIs for MicriumOS and FreeRTOS
/// - Retrieveing attestation tokens (@ref sl_se_manager_attestation)
///
/// ## Key Storage and Use of SE Wrapped Keys
///
/// The way keys are stored and operated on depends on the options set in the key descriptor used (@ref sl_se_key_descriptor_t).
/// Each key descriptor is initialized with a storage location, a key type, and length of the key (some key types have a known length, and it is not required to be set).
/// The storage location can either be application memory or inside the SE, for more details, see @ref sl_se_storage_method_t.
/// Depending on the use-case, the key descriptors will also store the pointer to a key and an SE key slot, see @ref sl_se_key_slot_t for the list of available internal SE key slots.
///
/// For more information on the key handling APIs see @ref sl_se_manager_key_handling.
///
/// ### Supported Key Types
/// Symmetric keys
/// - AES-128 (16 bytes)
/// - AES-192 (24 bytes)
/// - AES-256 (32 bytes)
/// - ChaCha20 (32 bytes)
///
/// Asymmetric keys for ECC
/// - NIST P-192
/// - NIST P-256
/// - NIST P-384
/// - NIST P-521
/// - Curve25519
/// - Curve448
///
/// Custom Weierstrass Prime curves are also supported (@ref sl_se_custom_weierstrass_prime_domain_t).
///
/// ### Example Usage of Keys
///
/// @code{.c}
///     #define WRAPPED_KEY_OVERHEAD (12UL + 16UL)
///     #define AES_256_KEY_SIZE 32UL
///
///     uint8_t key_buffer[AES_256_KEY_SIZE];
///     uint8_t wrapped_key_buffer[AES_256_KEY_SIZE + WRAPPED_KEY_OVERHEAD];
///
///     void demo_se_create_key_in_slot(void) {
///         sl_se_key_descriptor_t new_key = {
///             .type = SL_SE_KEY_TYPE_AES_256,
///             .flags = SL_SE_KEY_FLAG_NON_EXPORTABLE,
///             .storage.method = SL_SE_KEY_STORAGE_INTERNAL_VOLATILE,
///             .storage.location.slot = SL_SE_KEY_SLOT_VOLATILE_0,
///         };
///         sl_se_generate_key(&new_key);
///     }
///
///     void demo_se_create_plaintext_key(void) {
///         sl_se_key_descriptor_t new_key = {
///             .type = SL_SE_KEY_TYPE_AES_256,
///             .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT,
///         };
///         new_key.storage.location.buffer.pointer = key_buffer;
///         new_key.storage.location.buffer.size = sizeof(key_buffer);
///         sl_se_generate_key(&new_key);
///     }
///
///     void demo_se_create_wrapped_key(void) {
///         sl_se_key_descriptor_t new_wrapped_key = {
///             .type = SL_SE_KEY_TYPE_AES_256,
///             .flags = SL_SE_KEY_FLAG_NON_EXPORTABLE,
///             .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED,
///         };
///         new_wrapped_key.storage.location.buffer.pointer = wrapped_key_buffer;
///         new_wrapped_key.storage.location.buffer.size = sizeof(wrapped_key_buffer);
///         sl_se_generate_key(&new_wrapped_key);
///     }
///
///     void demo_se_wrapped_key_to_volatile_slot(void) {
///         sl_se_key_descriptor_t existing_wrapped_key = {
///             .type = SL_SE_KEY_TYPE_AES_256,
///             .flags = SL_SE_KEY_FLAG_NON_EXPORTABLE,
///             .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED,
///         };
///         existing_wrapped_key.storage.location.buffer.pointer = wrapped_key_buffer;
///         existing_wrapped_key.storage.location.buffer.size = sizeof(wrapped_key_buffer);
///         sl_se_key_descriptor_t key_in_slot = {
///             .type = SL_SE_KEY_TYPE_AES_256,
///             .flags = SL_SE_KEY_FLAG_NON_EXPORTABLE,
///             .storage.method = SL_SE_KEY_STORAGE_INTERNAL_VOLATILE,
///             .storage.location.slot = SL_SE_KEY_SLOT_VOLATILE_0,
///         };
///         sl_se_import_key(&existing_wrapped_key, &key_in_slot);
///     }
///
///     void demo_se_volatile_slot_to_wrapped_key(void) {
///         sl_se_key_descriptor_t existing_volatile_key = {
///             .type = SL_SE_KEY_TYPE_AES_256,
///             .flags = SL_SE_KEY_FLAG_NON_EXPORTABLE,
///             .storage.method = SL_SE_KEY_STORAGE_INTERNAL_VOLATILE,
///             .storage.location.slot = SL_SE_KEY_SLOT_VOLATILE_0,
///         };
///         sl_se_key_descriptor_t wrapped_key_out = {
///             .type = SL_SE_KEY_TYPE_AES_256,
///             .flags = SL_SE_KEY_FLAG_NON_EXPORTABLE,
///             .storage.method = SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED,
///         };
///         wrapped_key_out.storage.location.buffer.pointer = wrapped_key_buffer;
///         wrapped_key_out.storage.location.buffer.size = sizeof(wrapped_key_buffer);
///         sl_se_export_key(&existing_volatile_key, &wrapped_key_out);
///     }
///
///     void demo_se_delete_from_volatile_slot(void) {
///         sl_se_key_descriptor_t existing_volatile_key = {
///             .type = SL_SE_KEY_TYPE_AES_256,
///             .flags = SL_SE_KEY_FLAG_NON_EXPORTABLE,
///             .storage.method = SL_SE_KEY_STORAGE_INTERNAL_VOLATILE,
///             .storage.location.slot = SL_SE_KEY_SLOT_VOLATILE_0,
///         };
///         sl_se_delete_key(&existing_volatile_key);
///     }
/// @endcode
///
/// ## Tamper
///
/// The Secure Engine (SE) tamper module connects a number of hardware and software-driven tamper signals to a set of configurable hardware and software responses.
/// This can be used to program the device to automatically respond to external events that could signal that someone is trying to tamper with the device,
/// and very rapidly remove secrets stored in the SE. The available tamper signals range from signals based on failed authentication and secure boot to specialized glitch detectors.
/// When any of these signals fire, the tamper block can be configured to trigger several different responses,
/// ranging from triggering an interrupt to erasing the one-time-programmable (OTP) memory, removing all SE secrets and resulting in a permanently destroyed device.
///
/// A tamper signal can lead to a series of different autonomous responses from the tamper module. These responses are listed in the table below.
/// |       | Response  | Description  |
/// | ----: | :----:    | :---- |
/// | 0     | Ignore    | No action is taken.    |
/// | 1     | Interrupt | The SETAMPERHOST interrupt on the host is triggered.    |
/// | 2     | Filter    | A counter in the tamper filter is increased.    |
/// | 4     | Reset     | The device is reset.    |
/// | 7     | Erase OTP | Erases the OTP configuration of the device.    |
///
/// These responses are cumulative, meaning that if a filter response is triggered, an interrupt will also be triggered. For a full overview of the tamper signals, see @ref sl_se_manager_defines.h.
///
/// The tamper configuration is one-time-programmable, and is done using the initialise OTP command to the SE (see @ref sl_se_init_otp).
/// This means that tamper settings must be written together with secure boot settings, and are immutable after they are written.
/// After tamper has been initialized, it is possible to temporarily disable one or several tamper signals using an authenticated command,
/// similar to secure debug unlock. This is only possible if the debug public key has been installed on the device.
/// It is only possible to disable the customer enabled response. The default response to a signal cannot be disabled.
///
/// Tamper is configured by providing the following:
/// <table>
/// <caption id="multi_row">Tamper configuration table</caption>
/// <tr><th>Setting                       <th>Description
/// <tr><td>Tamper response levels        <td>A response level for each tamper signal.\n\n It is not possible to degrade the default response level of a tamper signal, so if a response is set to a lower level than the default response level listed in the table in the Signals section, this won't have any effect.
/// <tr><td>Filter settings               <td>The tamper filter counter has two settings:
///                                                                       <ul>
///                                                                         <li>Reset period
///                                                                         <li>Trigger threshold
///                                                                       </ul>
///                                                                   These options can be set to the values given in the tables in the Response Filter section. Please see the examples section for a suggested use of the tamper filter signal.
/// <tr><td>Flags                                         <td>On EFR32MG21 the tamper flags is only used to configure one option:
///                                                                       <ul>
///                                                                         <li>Digital Glitch Detector Always On – This option will keep the digital glitch detector running even while the SE is not performing any operations. This leads to increased energy consumption.
///                                                                       </ul>
/// <tr><td>Reset threshold                       <td>The number of consecutive tamper resets before the the part enters debug mode.\n\n
///                                                                           If the threshold is set to 0, the part will never enter the debug mode due to tamper reset.
/// </table>
///
/// ### Example Usage
///
/// The glitch detectors can see spurious activations, and should typically not be used to directly drive a serious tamper response.
/// Instead they should feed their signals into a tamper interrupt (to handle the response logic on the M33), or into the tamper filter counter,
/// which can be used to activate a high level response if a number of incidents occur in a short time window.
/// The time period and counter threshold must be tuned to the use case. In the following example the device will erase OTP and become inoperable if 4 glitch signals is seen in a 1 minute time period.
///
/// Since you can only configure tamper once for each device, please make sure that this is the configuration you actually want before you execute this example on actual device.
///
/// @code{.c}
///     sl_se_otp_init_t otp_settings_init = SL_SE_OTP_INIT_DEFAULT;
///
///     // Configure tamper levels
///     otp_settings_init.tamper_levels[SL_SE_TAMPER_SIGNAL_FILTER] = SL_SE_TAMPER_LEVEL_PERMANENTLY_ERASE_OTP;
///     otp_settings_init.tamper_levels[SL_SE_TAMPER_SIGNAL_VGLITCHFALLING] = SL_SE_TAMPER_LEVEL_FILTER;
///     otp_settings_init.tamper_levels[SL_SE_TAMPER_SIGNAL_VGLITCHRISING] = SL_SE_TAMPER_LEVEL_FILTER;
///     otp_settings_init.tamper_levels[SL_SE_TAMPER_SIGNAL_DGLITCH] = SL_SE_TAMPER_LEVEL_FILTER;
///
///
///     // Configure tamper filter options
///     otp_settings_init.tamper_filter_period = SL_SE_TAMPER_FILTER_PERIOD_1MIN;
///     otp_settings_init.tamper_filter_threshold = SL_SE_TAMPER_FILTER_THRESHOLD_4;
///
///
///     // Commit OTP settings. This command is only available once!
///     sl_se_init_otp(&otp_settings_init);
/// @endcode
///
/// ## RTOS Mode and Multi-Thread Safety
///
/// @note The SE Manager API is multi-thread safe, but does not support preemption.
///       This means the API cannot be called from ISR or critical/atomic sections when running in an RTOS thread.
///       When using the SE Manager API in a bare-metal application, it is the application developer's responsibility
///       to not call the SE Manager APIs when another operation is in progress.
///
/// The SE Manager supports multi-thread safe APIs for MicriumOS and FreeRTOS interfacing with CMSIS RTOS2 APIs.
///
/// For MicriumOS support the user application must define the compile time option SL_CATALOG_MICRIUMOS_KERNEL_PRESENT.
/// For FreeRTOS support the user application must define the compile time option SL_CATALOG_FREERTOS_KERNEL_PRESENT.
/// For bare metal mode (non-RTOS) the user must not define SL_CATALOG_MICRIUMOS_KERNEL_PRESENT or SL_CATALOG_FREERTOS_KERNEL_PRESENT.
///
/// Applications created using Simplicity Studio 5 need to include the header file called _sl_component_catalog.h_ which will include a macro define for one of the abovementioned RTOSes if present.
///
/// In the cases with SL_CATALOG_MICRIUMOS_KERNEL_PRESENT or SL_CATALOG_FREERTOS_KERNEL_PRESENT defined (RTOS-mode), the SE Manager will be configured with threading and yield support.
/// Configure ::sl_se_command_context_t with ::sl_se_set_yield to yield the CPU core when the SE Manager is waiting for the Secure Engine to complete a mailbox command.
///
/// For threading support the SE Manager applies an SE lock mechanism to protect the Secure Engine Mailbox interface from being accessed by more than one thread,
/// ensuring multi-thread safety. For yielding the CPU core while waiting for the SE, the SE Manager APIs that invoke
/// SE mailbox commands will wait on a semaphore which is signaled in the ISR that handles the SE mailbox completion interrupt.
/// Hence other threads may run on the CPU core while the SE is processing the mailbox command.
///
/// @} (end addtogroup sl_se_manager)
