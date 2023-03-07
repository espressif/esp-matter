/***************************************************************************//**
 * @file
 * @brief EmberZNet AES-MMO Hash API.
 * See @ref security for documentation.
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

#ifndef SILABS_AES_MMO_H
#define SILABS_AES_MMO_H

/**
 * @addtogroup security
 *
 * This file describes the functions for performing an AES-MMO hash on a
 * block of data.  The block of data may be small and contiguous, in which
 * case the emberAesHashSimple() routine can be used.  Or large and
 * discontiguous (such as a file stored in EEPROM), in which case the
 * application must pass chunks to emberAesMmoHashUpdate() routine
 * and use emberAesMmoHashFinal() on the last chunk.
 *
 * @{
 */

/** @brief
 *  This routine clears the passed context so that a new hash calculation
 *  can be performed.
 *
 *  @param context A pointer to the location of hash context to clear.
 */
void emberAesMmoHashInit(EmberAesMmoHashContext* context);

/** @brief
 *  This routine processes the passed chunk of data and updates
 *  the hash calculation based on it.  The data passed in MUST
 *  have a length that is a multiple of 16.
 *
 * @param context A pointer to the location of the hash context to update.
 * @param length The length of the passed data.
 * @param data A pointer to the location of the data to hash.
 *
 * @return An ::EmberStatus value indicating EMBER_SUCCESS if the hash was
 *   calculated successfully.  EMBER_INVALID_CALL if the block size is not a
 *   multiple of 16 bytes, and EMBER_INDEX_OUT_OF_RANGE is returned when the
 *   data exceeds the maximum limits of the hash function.
 */
EmberStatus emberAesMmoHashUpdate(EmberAesMmoHashContext* context,
                                  uint32_t length,
                                  const uint8_t* data);

/** @brief
 *  This routine processes the passed chunk of data (if non-NULL)
 *  and update the hash context that is passed in.  In then performs
 *  the final calculations on the hash and returns the final answer
 *  in the result parameter of the ::EmberAesMmoHashContext structure.
 *  The length of the data passed in may be any value, it does not have
 *  to be a multiple of 16.
 *
 * @param context A pointer to the location of the hash context to finalize.
 * @param length The length of the finalData parameter.
 * @param finalData A pointer to the location of data to hash.  May be NULL.
 *
 * @return An ::EmberStatus value indicating EMBER_SUCCESS if the hash was
 *   calculated successfully.  EMBER_INVALID_CALL if the block size is not a
 *   multiple of 16 bytes, and EMBER_INDEX_OUT_OF_RANGE is returned when the
 *   data exceeds the maximum limits of the hash function.
 */
EmberStatus emberAesMmoHashFinal(EmberAesMmoHashContext* context,
                                 uint32_t length,
                                 const uint8_t* finalData);

/** @brief
 *  This is a convenience method when the hash data is less than 255
 *  bytes.  It inits, updates, and finalizes the hash in one function call.
 *
 * @param totalLength The length of the data.
 * @param data The data to hash.
 * @param result The location where the result of the hash will be written.
 *
 * @return An ::EmberStatus value indicating EMBER_SUCCESS if the hash was
 *   calculated successfully.  EMBER_INVALID_CALL if the block size is not a
 *   multiple of 16 bytes, and EMBER_INDEX_OUT_OF_RANGE is returned when the
 *   data exceeds the maximum limits of the hash function.
 */
EmberStatus emberAesHashSimple(uint8_t totalLength,
                               const uint8_t* data,
                               uint8_t* result);

// @} END addtogroup

#endif // SILABS_AES_MMO_H
