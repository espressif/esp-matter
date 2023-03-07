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

/** @addtogroup version
 * @brief Macros to determine the stack version.
 *
 * Note that the Connect Stack version might not match the version of Flex SDK.
 *
 * See config.h for source code.
 *
 * @{
 */

/**
 * @brief The major version of the release. First digit of A.B.C.D.
 */
#define EMBER_MAJOR_VERSION   4

/**
 * @brief The minor version of the release. Second digit of A.B.C.D
 */
#define EMBER_MINOR_VERSION   0

/**
 * @brief The patch version of the release. Third digit of A.B.C.D
 *
 * Patch versions are fully backwards compatible as long as the major and minor
 * version matches.
 */
#define EMBER_PATCH_VERSION   1

/**
 * @brief Special version of the release. Fourth digit of A.B.C.D
 */
#define EMBER_SPECIAL_VERSION 0

/**
 * @brief Build number of the release. Should be stored on 2 bytes.
 */
#define EMBER_BUILD_NUMBER   273

/**
 * @brief Full version number stored on 2 bytes, with each of the four digits
 * stored on 4 bits.
 */
#define EMBER_FULL_VERSION (  ((uint16_t)EMBER_MAJOR_VERSION << 12)   \
                              | ((uint16_t)EMBER_MINOR_VERSION <<  8) \
                              | ((uint16_t)EMBER_PATCH_VERSION <<  4) \
                              | ((uint16_t)EMBER_SPECIAL_VERSION))

/**
 * @brief Version type of the release. EMBER_VERSION_TYPE_GA means
 * generally available.
 */
#define EMBER_VERSION_TYPE EMBER_VERSION_TYPE_GA

/**
 * @copybrief EMBER_FULL_VERSION
 */
#define SOFTWARE_VERSION EMBER_FULL_VERSION

/** @} // End group
 */
