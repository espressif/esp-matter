/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SST_OBJECT_SYSTEM_H__
#define __SST_OBJECT_SYSTEM_H__

#include <stdint.h>

#include "psa/protected_storage.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Prepares the secure storage system for usage, populating internal
 *        structures.
 *        It identifies and validates the system metadata.
 *
 * \return Returns error code specified in \ref psa_status_t
 */
psa_status_t sst_system_prepare(void);

/**
 * \brief Creates a new object with the provided UID and client ID.
 *
 * \param[in] uid           Unique identifier for the data
 * \param[in] client_id     Identifier of the asset's owner (client)
 * \param[in] create_flags  Flags indicating the properties of the data
 * \param[in] size          Size of the contents of `data` in bytes
 *
 * \return Returns error code specified in \ref psa_status_t
 */
psa_status_t sst_object_create(psa_storage_uid_t uid, int32_t client_id,
                               psa_storage_create_flags_t create_flags,
                               uint32_t size);

/**
 * \brief Gets the data of the object with the provided UID and client ID.
 *
 * \param[in]  uid            Unique identifier for the data
 * \param[in]  client_id      Identifier of the asset's owner (client)
 * \param[in]  offset         Offset in the object at which to begin the read
 * \param[in]  size           Size of the contents of `data` in bytes
 * \param[out] p_data_length  On success, this will contain size of the data
 *                            written to asset
 *
 * \return Returns error code specified in \ref psa_status_t
 */
psa_status_t sst_object_read(psa_storage_uid_t uid, int32_t client_id,
                             uint32_t offset, uint32_t size,
                             size_t *p_data_length);

/**
 * \brief Writes data into the object with the provided UID and client ID.
 *
 * \param[in] uid        Unique identifier for the data
 * \param[in] client_id  Identifier of the asset's owner (client)
 * \param[in] offset     Offset in the object at which to begin the write
 * \param[in] size       Size of the contents of `data` in bytes
 *
 * \return Returns error code specified in \ref psa_status_t
 */
psa_status_t sst_object_write(psa_storage_uid_t uid, int32_t client_id,
                              uint32_t offset, uint32_t size);

/**
 * \brief Deletes the object with the provided UID and client ID.
 *
 * \param[in] uid        Unique identifier for the data
 * \param[in] client_id  Identifier of the asset's owner (client)
 *
 * \return Returns error code specified in \ref psa_status_t
 */
psa_status_t sst_object_delete(psa_storage_uid_t uid, int32_t client_id);

/**
 * \brief Gets the asset information for the object with the provided UID and
 *        client ID.
 *
 * \param[in]  uid        Unique identifier for the data
 * \param[in]  client_id  Identifier of the asset's owner (client)
 * \param[out] info       Pointer to the `psa_storage_info_t` struct that will
 *                        be populated with the metadata
 *
 * \return Returns error code specified in \ref psa_status_t
 */
psa_status_t sst_object_get_info(psa_storage_uid_t uid, int32_t client_id,
                                 struct psa_storage_info_t *info);

/**
 * \brief Wipes the secure storage system and all object data.
 *
 * \return Returns error code specified in \ref psa_status_t
 */
psa_status_t sst_system_wipe_all(void);

#ifdef __cplusplus
}
#endif

#endif /* __SST_OBJECT_SYSTEM_H__ */
