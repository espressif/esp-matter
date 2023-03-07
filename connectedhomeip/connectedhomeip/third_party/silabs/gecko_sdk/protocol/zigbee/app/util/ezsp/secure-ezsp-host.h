/***************************************************************************//**
 * @file
 * @brief Secure EZSP API exposed to the Host
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

#ifndef SILABS_SECURE_EZSP_HOST_H
#define SILABS_SECURE_EZSP_HOST_H

//------------------------------------------------------------------------------
// Public APIs

/** @brief Sets the security key for the Secure EZSP protocol.
 *
 * @param securityKey   The security key for the protocol.
 *
 * @param securityType  The security type for the protocol,
 * where ::SECURE_EZSP_SECURITY_TYPE_TEMPORARY sets up temporary security,
 * which can be reset through ::emberSecureEzspResetToFactoryDefaults().
 * ::SECURE_EZSP_SECURITY_TYPE_PERMANENT sets up permanent security, which
 * cannot be reset. Currently, we only have temporary security implemented.
 *
 * @return An ::EzspStatus value:\n\n
 * - ::EZSP_SUCCESS - The security key is successfully set.
 * - ::EZSP_ERROR_INVALID_CALL - Permanent security is not currently implemented.
 * - ::EZSP_ERROR_SECURITY_TYPE_INVALID - Wrong security type is requested.
 * - ::EZSP_ERROR_SECURITY_KEY_ALREADY_SET - Security key is already set.
 */
EzspStatus emberSecureEzspSetSecurityKey(EmberKeyData *securityKey,
                                         SecureEzspSecurityType securityType);

/** @brief Sets the security parameters for the Secure EZSP protocol.
 *
 * @param securityLevel   The security level for the protocol. Currently, we
 * only allow ::SECURE_EZSP_SECURITY_LEVEL_ENC_MIC_32.
 *
 * @param SecureEzspRandomNumber  The random number to be used for Session ID
 * generation.
 *
 * @return An ::EzspStatus value:\n\n
 * - ::EZSP_SUCCESS - The security parameters are successfully set.
 * - ::EZSP_ERROR_SECURITY_KEY_NOT_SET - Security key is not set, so we cannot
 * establish security parameters.
 * - ::EZSP_ERROR_SECURITY_PARAMETERS_ALREADY_SET - Security parameters are
 * already set.
 * - ::EZSP_ASH_NCP_FATAL_ERROR - NCP failed to generate a random number,
 * needed for the Session ID generation.
 */
EzspStatus emberSecureEzspSetSecurityParameters(SecureEzspSecurityLevel securityLevel,
                                                SecureEzspRandomNumber *randomNumber);

/** @brief Resets security key and security parameters. Node will leave the
 * network before doing so for security reasons.
 *
 * @return An ::EzspStatus value:\n\n
 * - ::EZSP_SUCCESS - The security settings are successfully reset.
 * - ::EZSP_ERROR_INVALID_CALL - Cannot reset permanent security.
 * - ::EZSP_ERROR_SECURITY_KEY_NOT_SET - Security key is not set, so we cannot
 * reset security.
 */
EzspStatus emberSecureEzspResetToFactoryDefaults(void);

//------------------------------------------------------------------------------
// Public Callbacks

/** @brief A callback invoked on initialization to inform the security key
 * status (set or not set) to the application for both Host and NCP. This allows
 * the application to make decisions on what to do with security establishment.
 *
 * @param ncpSecurityKeyStatus  The security key status of NCP.
 * @param hostSecurityKeyStatus  The security key status of Host.
 * @param ncpSecurityType  The security key type used on NCP, if security is set.
 */
void emberSecureEzspInitCallback(EzspStatus ncpSecurityKeyStatus,
                                 EzspStatus hostSecurityKeyStatus,
                                 SecureEzspSecurityType ncpSecurityType);

#endif // SILABS_SECURE_EZSP_HOST_H
