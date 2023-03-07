/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SST_ENCRYPTED_OBJECT_H__
#define __SST_ENCRYPTED_OBJECT_H__

#include <stdint.h>
#include "sst_object_defs.h"
#include "psa/protected_storage.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Reads object referenced by the object File ID.
 *
 * \param[in]  fid      File ID
 * \param[out] obj      Pointer to the object structure to fill in
 *
 * \return Returns error code specified in \ref psa_status_t
 */
psa_status_t sst_encrypted_object_read(uint32_t fid,
                                       struct sst_object_t *obj);

/**
 * \brief Creates and writes a new encrypted object based on the given
 *        sst_object_t structure data.
 *
 * \param[in]     fid      File ID
 * \param[in,out] obj      Pointer to the object structure to write.
 *
 * Note: The function will use obj to store the encrypted data before write it
 *       into the flash to reduce the memory requirements and the number of
 *       internal copies. So, this object will contain the encrypted object
 *       stored in the flash.
 *
 * \return Returns error code specified in \ref psa_status_t
 */
psa_status_t sst_encrypted_object_write(uint32_t fid,
                                        struct sst_object_t *obj);

#ifdef __cplusplus
}
#endif

#endif /* __SST_ENCRYPTED_OBJECT_H__ */
