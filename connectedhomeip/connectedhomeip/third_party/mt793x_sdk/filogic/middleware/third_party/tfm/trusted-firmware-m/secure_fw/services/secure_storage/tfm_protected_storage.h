/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_PROTECTED_STORAGE_H__
#define __TFM_PROTECTED_STORAGE_H__

#include <stdint.h>

#include "psa/protected_storage.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Initializes the secure storage system.
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref psa_status_t
 *
 * \retval PSA_SUCCESS                     The operation completed successfully
 * \retval PSA_ERROR_STORAGE_FAILURE       The operation failed because the
 *                                         storage system initialization has
 *                                         failed (fatal error)
 * \retval PSA_ERROR_GENERIC_ERROR         The operation failed because of an
 *                                         unspecified internal failure
 */
psa_status_t tfm_sst_init(void);

/**
 * \brief Creates a new or modifies an existing asset.
 *
 * \param[in] client_id     Identifier of the asset's owner (client)
 * \param[in] uid           Unique identifier for the data
 * \param[in] data_length   The size in bytes of the data in `p_data`
 * \param[in] create_flags  The flags indicating the properties of the data
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref psa_status_t
 *
 * \retval PSA_SUCCESS                      The operation completed successfully
 * \retval PSA_ERROR_NOT_PERMITTED          The operation failed because the
 *                                          provided uid value was already
 *                                          created with
 *                                          PSA_STORAGE_FLAG_WRITE_ONCE
 * \retval PSA_ERROR_INVALID_ARGUMENT       The operation failed because one or
 *                                          more of the given arguments were
 *                                          invalid (null pointer, etc.)
 * \retval PSA_ERROR_NOT_SUPPORTED          The operation failed because one or
 *                                          more of the flags provided in
 *                                          `create_flags` is not supported or
 *                                          is not valid
 * \retval PSA_ERROR_INSUFFICIENT_STORAGE   The operation failed because there
 *                                          was insufficient space on the
 *                                          storage medium
 * \retval PSA_ERROR_STORAGE_FAILURE        The operation failed because the
 *                                          physical storage has failed (fatal
 *                                          error)
 * \retval PSA_ERROR_GENERIC_ERROR          The operation failed because of an
 *                                          unspecified internal failure.
 */
psa_status_t tfm_sst_set(int32_t client_id,
                         psa_storage_uid_t uid,
                         uint32_t data_length,
                         psa_storage_create_flags_t create_flags);
/**
 * \brief Gets the asset data for the provided uid.
 *
 * \param[in]  client_id    Identifier of the asset's owner (client)
 * \param[in]  uid          Unique identifier for the data
 * \param[in]  data_offset  The offset within the data associated with the `uid`
 *                          to start retrieving data
 * \param[in]  data_length  The amount of data to read (and the minimum
 *                          allocated size of the `p_data` buffer)
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref psa_status_t
 *
 * \retval PSA_SUCCESS                    The operation completed successfully
 * \retval PSA_ERROR_INVALID_ARGUMENT     The operation failed because one or
 *                                        more of the given arguments were
 *                                        invalid (null pointer, etc.)
 * \retval PSA_ERROR_DOES_NOT_EXIST       The operation failed because the
 *                                        provided uid value was not found in
 *                                        the storage
 * \retval PSA_ERROR_STORAGE_FAILURE      The operation failed because the
 *                                        physical storage has failed (fatal
 *                                        error)
 * \retval PSA_ERROR_GENERIC_ERROR        The operation failed because of an
 *                                        unspecified internal failure
 * \retval PSA_ERROR_DATA_CORRUPT         The operation failed because the data
 *                                        associated with the UID was corrupt
 * \retval PSA_ERROR_INVALID_SIGNATURE    The operation failed because the data
 *                                        associated with the UID failed
 *                                        authentication
 */
psa_status_t tfm_sst_get(int32_t client_id,
                         psa_storage_uid_t uid,
                         uint32_t data_offset,
                         uint32_t data_size,
                         size_t *p_data_length);
/**
 * \brief Gets the metadata for the provided uid.
 *
 * \param[in]  client_id  Identifier of the asset's owner (client)
 * \param[in]  uid        Unique identifier for the data
 * \param[out] p_info     A pointer to the `psa_storage_info_t` struct that will
 *                        be populated with the metadata
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref psa_status_t
 *
 * \retval PSA_SUCCESS                    The operation completed successfully
 * \retval PSA_ERROR_INVALID_ARGUMENT     The operation failed because one or
 *                                        more of the given arguments were
 *                                        invalid (null pointer, etc.)
 * \retval PSA_ERROR_DOES_NOT_EXIST       The operation failed because the
 *                                        provided uid value was not found in
 *                                        the storage
 * \retval PSA_ERROR_STORAGE_FAILURE      The operation failed because the
 *                                        physical storage has failed (fatal
 *                                        error)
 * \retval PSA_ERROR_GENERIC_ERROR        The operation failed because of an
 *                                        unspecified internal failure
 * \retval PSA_ERROR_DATA_CORRUPT         The operation failed because the data
 *                                        associated with the UID was corrupt
 * \retval PSA_ERROR_INVALID_SIGNATURE    The operation failed because the data
 *                                        associated with the UID failed
 *                                        authentication
 */
psa_status_t tfm_sst_get_info(int32_t client_id, psa_storage_uid_t uid,
                              struct psa_storage_info_t *p_info);

/**
 * \brief Removes the provided uid and its associated data from storage.
 *
 * \param[in] client_id  Identifier of the asset's owner (client)
 * \param[in] uid        Unique identifier for the data to be removed
 *
 * \return A status indicating the success/failure of the operation as specified
 *         in \ref psa_status_t
 *
 * \retval PSA_SUCCESS                    The operation completed successfully
 * \retval PSA_ERROR_INVALID_ARGUMENT     The operation failed because one or
 *                                        more of the given arguments were
 *                                        invalid (null pointer, etc.)
 * \retval PSA_ERROR_DOES_NOT_EXIST       The operation failed because the
 *                                        provided uid value was not found in
 *                                        the storage
 * \retval PSA_ERROR_NOT_PERMITTED        The operation failed because the
 *                                        provided uid value was created with
 *                                        PSA_STORAGE_FLAG_WRITE_ONCE
 * \retval PSA_ERROR_STORAGE_FAILURE      The operation failed because the
 *                                        physical storage has failed (fatal
 *                                        error)
 * \retval PSA_ERROR_GENERIC_ERROR        The operation failed because of an
 *                                        unspecified internal failure
 */
psa_status_t tfm_sst_remove(int32_t client_id, psa_storage_uid_t uid);

/**
 * \brief Gets a bitmask with flags set for all of the optional features
 *        supported by the implementation.
 *
 * \return Bitmask value which contains all the bits set for all the optional
 *         features supported by the implementation
 */
uint32_t tfm_sst_get_support(void);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_PROTECTED_STORAGE_H__ */
