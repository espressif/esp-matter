/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SST_OBJECT_DEFS_H__
#define __SST_OBJECT_DEFS_H__

#include <stdint.h>

#include "flash_layout.h"
#include "psa/protected_storage.h"
#ifdef TARGET_PLATFORM_MT793X
#include "mt7933_layout.h"
#endif

#ifdef SST_ENCRYPTION
#include "crypto/sst_crypto_interface.h"
#endif

/*!
 * \struct sst_object_info_t
 *
 * \brief Object information.
 */
struct sst_object_info_t {
    uint32_t current_size; /*!< Current size of the object content in bytes */
    uint32_t max_size;     /*!< Maximum size of the object content in bytes */
    psa_storage_create_flags_t create_flags; /*!< Object creation flags */
};

/*!
 * \struct sst_obj_header_t
 *
 * \brief Metadata attached as a header to object data before storage.
 */
struct sst_obj_header_t {
#ifdef SST_ENCRYPTION
    union sst_crypto_t crypto;     /*!< Crypto metadata */
#else
    uint32_t version;              /*!< Object version */
    uint32_t fid;                  /*!< File ID */
#endif
    struct sst_object_info_t info; /*!< Object information */
};


#define SST_MAX_OBJECT_DATA_SIZE  SST_MAX_ASSET_SIZE

/*!
 * \struct sst_object_t
 *
 * \brief The object to be written to the file system below. Made up of the
 *        object header and the object data.
 */
struct sst_object_t {
    struct sst_obj_header_t header;         /*!< Object header */
    uint8_t data[SST_MAX_OBJECT_DATA_SIZE]; /*!< Object data */
};


#define SST_OBJECT_HEADER_SIZE    sizeof(struct sst_obj_header_t)
#define SST_MAX_OBJECT_SIZE       sizeof(struct sst_object_t)

/*!
 * \def SST_MAX_NUM_OBJECTS
 *
 * \brief Specifies the maximum number of objects in the system, which is the
 *        number of defined assets, the object table and 2 temporary objects to
 *        store the temporary object table and temporary updated object.
 */
#define SST_MAX_NUM_OBJECTS (SST_NUM_ASSETS + 3)

#endif /* __SST_OBJECT_DEFS_H__ */
