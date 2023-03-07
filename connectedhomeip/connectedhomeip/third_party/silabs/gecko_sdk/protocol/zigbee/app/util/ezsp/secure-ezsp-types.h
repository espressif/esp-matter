/***************************************************************************//**
 * @file
 * @brief Secure EZSP types and defines.
 * See @ref secure-ezsp for documentation.
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
 * @addtogroup secure_ezsp
 *
 * See secure-ezsp-types.h for source code.
 * @{
 */

#ifndef SILABS_SECURE_EZSP_TYPES_H
#define SILABS_SECURE_EZSP_TYPES_H

#include "app/util/ezsp/ezsp-enum.h"

/**
 * @name Secure EZSP Types
 */
//@{

/** 32-bit security frame counter */
typedef uint32_t SecureEzspFrameCounter;

/** 32-bit security Message Integrity Code */
typedef uint32_t SecureEzspMic;

/**
 * @brief Size of Random number in bytes (16).
 */
#define SECURE_EZSP_RANDOM_NUMBER_SIZE  16

/**
 * @brief Size of Session ID in bytes (8).
 */
#define SECURE_EZSP_SESSION_ID_SIZE     8

/** @brief This data structure contains the random number data that is passed
 *   into various other functions. */
typedef struct {
  /** This is the random byte data. */
  uint8_t contents[SECURE_EZSP_RANDOM_NUMBER_SIZE];
} SecureEzspRandomNumber;

/** @brief This data structure contains the Session ID data that is passed
 *   into various other functions. */
typedef struct {
  /** This is the Session ID byte data. */
  uint8_t contents[SECURE_EZSP_SESSION_ID_SIZE];
} SecureEzspSessionId;

/**
 * @brief Security Type
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum SecureEzspSecurityType
#else
typedef uint32_t SecureEzspSecurityType;
enum
#endif
{
  SECURE_EZSP_SECURITY_TYPE_TEMPORARY = 0x00000000,
  SECURE_EZSP_SECURITY_TYPE_PERMANENT = 0x12345678,
};

/**
 * @brief Security Level
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum SecureEzspSecurityLevel
#else
typedef uint8_t SecureEzspSecurityLevel;
enum
#endif
{
  SECURE_EZSP_SECURITY_LEVEL_ENC_MIC_32 = 0x05,
};

/** @brief This function allows the programmer to gain access
 *  to the actual random number bytes of the SecureEzspRandomNumber struct.
 *
 * @param key  A Pointer to an SecureEzspRandomNumber structure.
 *
 * @return uint8_t* Returns a pointer to the first byte of the Random Number data.
 */
#if defined DOXYGEN_SHOULD_SKIP_THIS
uint8_t* SecureEzspRandomNumberContents(SecureEzspRandomNumber* randomNumber);
#else
#define SecureEzspRandomNumberContents(randomNumber) ((randomNumber)->contents)
#endif

/** @brief This function allows the programmer to gain access
 *  to the actual Session ID bytes of the SecureEzspSessionId struct.
 *
 * @param key  A Pointer to an SecureEzspSessionId structure.
 *
 * @return uint8_t* Returns a pointer to the first byte of the Random Number data.
 */
#if defined DOXYGEN_SHOULD_SKIP_THIS
uint8_t* SecureEzspSessionIdContents(SecureEzspSessionId* sessionId);
#else
#define SecureEzspSessionIdContents(sessionId) ((sessionId)->contents)
#endif

/** @} END addtogroup
 */

#endif // SILABS_SECURE_EZSP_TYPES_H
