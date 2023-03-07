/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "sst_object_table.h"

#include <stddef.h>

#include "cmsis_compiler.h"
#include "crypto/sst_crypto_interface.h"
#include "flash_layout.h"
#include "nv_counters/sst_nv_counters.h"
#include "psa/internal_trusted_storage.h"
#include "tfm_memory_utils.h"
#include "sst_utils.h"
#include "tfm_sst_defs.h"
#ifdef TARGET_PLATFORM_MT793X
#include "mt7933_layout.h"
#endif

/* FIXME: Duplicated from flash info */
#define SST_FLASH_DEFAULT_VAL 0xFFU

/*!
 * \def SST_OBJECT_SYSTEM_VERSION
 *
 * \brief Current object system version.
 */
#define SST_OBJECT_SYSTEM_VERSION  0x01

/*!
 * \struct sst_obj_table_info_t
 *
 * \brief Object table information structure.
 */
struct sst_obj_table_entry_t {
#ifdef SST_ENCRYPTION
    uint8_t tag[SST_TAG_LEN_BYTES]; /*!< MAC value of AEAD object */
#else
    uint32_t version;               /*!< File version */
#endif
    psa_storage_uid_t uid;          /*!< Object UID */
    int32_t client_id;              /*!< Client ID */
};

/* Specifies number of entries in the table. The number of entries is the
 * number of assets, defined in asset_defs.h, plus one extra entry to store
 * a new object when the code processes a change in a file.
 */
#define SST_OBJ_TABLE_ENTRIES (SST_NUM_ASSETS + 1)

/*!
 * \struct sst_obj_table_t
 *
 * \brief Object table structure.
 */
struct sst_obj_table_t {
#ifdef SST_ENCRYPTION
  union sst_crypto_t crypto;     /*!< Crypto metadata. */
#endif

  uint8_t version;               /*!< SST object system version. */

#ifndef SST_ROLLBACK_PROTECTION
  uint8_t swap_count;            /*!< Swap counter to distinguish 2 different
                                  *   object tables.
                                  */
#endif /* SST_ROLLBACK_PROTECTION */

  struct sst_obj_table_entry_t obj_db[SST_OBJ_TABLE_ENTRIES]; /*!< Table's
                                                               *   entries
                                                               */
};

/* Object table indexes */
#define SST_OBJ_TABLE_IDX_0 0
#define SST_OBJ_TABLE_IDX_1 1

/* Number of object tables (active and scratch) */
#define SST_NUM_OBJ_TABLES  2

/*!
 * \def SST_TABLE_FS_ID
 *
 * \brief File ID to be used in order to store the object table in the
 *        file system.
 *
 * \param[in] idx  Table index to convert into a file ID.
 *
 * \return Returns file ID
 *
 */
#define SST_TABLE_FS_ID(idx) (idx + 1)

/*!
 * \def SST_OBJECT_FS_ID
 *
 * \brief File ID to be used in order to store an object in the
 *        file system.
 *
 * \param[in] idx  Object table index to convert into a file ID.
 *
 * \return Returns file ID
 */
#define SST_OBJECT_FS_ID(idx) ((idx + 1) + \
                                SST_TABLE_FS_ID(SST_OBJ_TABLE_IDX_1))

/*!
 * \def SST_OBJECT_FS_ID_TO_IDX
 *
 * \brief Gets object index in the table based on the file ID.
 *
 * \param[in] fid  File ID of an object in the object table
 *
 * \return Returns object table index
 */
#define SST_OBJECT_FS_ID_TO_IDX(fid) ((fid - 1) - \
                                      SST_TABLE_FS_ID(SST_OBJ_TABLE_IDX_1))

/*!
 * \struct sst_obj_table_ctx_t
 *
 * \brief Object table context structure.
 */
struct sst_obj_table_ctx_t {
    struct sst_obj_table_t obj_table; /*!< Object tables */
    uint8_t active_table;             /*!< Active object table */
    uint8_t scratch_table;            /*!< Scratch object table */
};

/* Object table context */
static struct sst_obj_table_ctx_t sst_obj_table_ctx;

/* Object table size */
#define SST_OBJ_TABLE_SIZE            sizeof(struct sst_obj_table_t)

/* Object table entry size */
#define SST_OBJECTS_TABLE_ENTRY_SIZE  sizeof(struct sst_obj_table_entry_t)

/* Size of the data that is not required to authenticate */
#define SST_NON_AUTH_OBJ_TABLE_SIZE   sizeof(union sst_crypto_t)

/* Start position to store the object table data in the FS object */
#define SST_OBJECT_TABLE_OBJECT_OFFSET 0

/* The associated data is the header minus the crypto data */
#define SST_CRYPTO_ASSOCIATED_DATA(crypto) ((uint8_t *)crypto + \
                                            SST_NON_AUTH_OBJ_TABLE_SIZE)

#ifdef SST_ROLLBACK_PROTECTION
#define SST_OBJ_TABLE_AUTH_DATA_SIZE (SST_OBJ_TABLE_SIZE - \
                                      SST_NON_AUTH_OBJ_TABLE_SIZE)

struct sst_crypto_assoc_data_t {
    uint8_t  obj_table_data[SST_OBJ_TABLE_AUTH_DATA_SIZE];
    uint32_t nv_counter;
};

#define SST_CRYPTO_ASSOCIATED_DATA_LEN  sizeof(struct sst_crypto_assoc_data_t)

#else

/* The associated data is the header, minus the the tag data */
#define SST_CRYPTO_ASSOCIATED_DATA_LEN (SST_OBJ_TABLE_SIZE - \
                                        SST_NON_AUTH_OBJ_TABLE_SIZE)
#endif /* SST_ROLLBACK_PROTECTION */

/* The sst_object_table_init function uses the static memory allocated for
 * the object data manipulation, in sst_object_table.c (g_sst_object), to load a
 * temporary object table to be validated at that stage.
 * To make sure the object table data fits in the static memory allocated for
 * object manipulation, the following macro checks if the memory allocated is
 * big enough, at compile time
 */

/* Check at compilation time if metadata fits in g_sst_object.data */
SST_UTILS_BOUND_CHECK(OBJ_TABLE_NOT_FIT_IN_STATIC_OBJ_DATA_BUF,
                      SST_OBJ_TABLE_SIZE, SST_MAX_ASSET_SIZE);

enum sst_obj_table_state {
    SST_OBJ_TABLE_VALID = 0,   /*!< Table content is valid */
    SST_OBJ_TABLE_INVALID,     /*!< Table content is invalid */
    SST_OBJ_TABLE_NVC_1_VALID, /*!< Table content valid with NVC 1 value */
    SST_OBJ_TABLE_NVC_3_VALID, /*!< Table content valid with NVC 3 value */
};

/* Specifies that SST NV counter value is invalid */
#define SST_INVALID_NVC_VALUE 0

/*!
 * \struct sst_obj_table_ctx_t
 *
 * \brief Object table init context structure.
 */
struct sst_obj_table_init_ctx_t {
    struct sst_obj_table_t *p_table[SST_NUM_OBJ_TABLES]; /*!< Pointers to
                                                          *   object tables
                                                          */
    enum sst_obj_table_state table_state[SST_NUM_OBJ_TABLES]; /*!< Array to
                                                               *   indicate if
                                                               *   the object
                                                               *   table X is
                                                               *   valid
                                                               */
#ifdef SST_ROLLBACK_PROTECTION
    uint32_t nvc_1;        /*!< Non-volatile counter value 1 */
    uint32_t nvc_3;        /*!< Non-volatile counter value 3 */
#endif /* SST_ROLLBACK_PROTECTION */
};

/**
 * \brief Reads object table from persistent memory.
 *
 * \param[out] init_ctx  Pointer to the init object table context
 *
 */
__attribute__ ((always_inline))
__STATIC_INLINE void sst_object_table_fs_read_table(
                                      struct sst_obj_table_init_ctx_t *init_ctx)
{
    psa_status_t err;
    size_t data_length;

    /* Read file with the table 0 data */

    err = psa_its_get(SST_TABLE_FS_ID(SST_OBJ_TABLE_IDX_0),
                      SST_OBJECT_TABLE_OBJECT_OFFSET,
                      SST_OBJ_TABLE_SIZE,
                      (void *)init_ctx->p_table[SST_OBJ_TABLE_IDX_0],
                      &data_length);
    if (err != PSA_SUCCESS) {
        init_ctx->table_state[SST_OBJ_TABLE_IDX_0] = SST_OBJ_TABLE_INVALID;
    }

    /* Read file with the table 1 data */
    err = psa_its_get(SST_TABLE_FS_ID(SST_OBJ_TABLE_IDX_1),
                      SST_OBJECT_TABLE_OBJECT_OFFSET,
                      SST_OBJ_TABLE_SIZE,
                      (void *)init_ctx->p_table[SST_OBJ_TABLE_IDX_1],
                      &data_length);
    if (err != PSA_SUCCESS) {
        init_ctx->table_state[SST_OBJ_TABLE_IDX_1] = SST_OBJ_TABLE_INVALID;
    }
}

/**
 * \brief Writes object table in persistent memory.
 *
 * \param[in,out] obj_table  Pointer to the object table to generate
 *                           authentication
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
__attribute__ ((always_inline))
__STATIC_INLINE psa_status_t sst_object_table_fs_write_table(
                                              struct sst_obj_table_t *obj_table)
{
    psa_status_t err;
    uint32_t obj_table_id = SST_TABLE_FS_ID(sst_obj_table_ctx.scratch_table);
    uint8_t swap_table_idxs = sst_obj_table_ctx.scratch_table;

    /* Create file to store object table in the FS */
    err = psa_its_set(obj_table_id,
                      SST_OBJ_TABLE_SIZE,
                      (const void *)obj_table,
                      PSA_STORAGE_FLAG_NONE);

    if (err != PSA_SUCCESS) {
        return err;
    }

    /* Swap active and scratch table values */
    sst_obj_table_ctx.scratch_table = sst_obj_table_ctx.active_table;
    sst_obj_table_ctx.active_table = swap_table_idxs;

    return PSA_SUCCESS;
}

#ifdef SST_ENCRYPTION
#ifdef SST_ROLLBACK_PROTECTION
/**
 * \brief Aligns all SST non-volatile counters.
 *
 * \param[in] nvc_1  Value of SST non-volatile counter 1
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
static psa_status_t sst_object_table_align_nv_counters(uint32_t nvc_1)
{
    psa_status_t err;
    uint32_t nvc_x_val = 0;

    /* Align SST NVC 2 with NVC 1 */
    err = sst_read_nv_counter(TFM_SST_NV_COUNTER_2, &nvc_x_val);
    if (err != PSA_SUCCESS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    for (; nvc_x_val < nvc_1; nvc_x_val++) {
        err = sst_increment_nv_counter(TFM_SST_NV_COUNTER_2);
        if (err != PSA_SUCCESS) {
            return err;
        }
    }

    /* Align SST NVC 3 with NVC 1 */
    err = sst_read_nv_counter(TFM_SST_NV_COUNTER_3, &nvc_x_val);
    if (err != PSA_SUCCESS) {
        return PSA_ERROR_GENERIC_ERROR;
    }

    for (; nvc_x_val < nvc_1; nvc_x_val++) {
        err = sst_increment_nv_counter(TFM_SST_NV_COUNTER_3);
        if (err != PSA_SUCCESS) {
            return err;
        }
    }

    return PSA_SUCCESS;
}

/**
 * \brief Generates table authentication tag.
 *
 * \param[in]     nvc_1      Value of SST non-volatile counter 1
 * \param[in,out] obj_table  Pointer to the object table to generate
 *                           authentication
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
__attribute__ ((always_inline))
__STATIC_INLINE psa_status_t sst_object_table_nvc_generate_auth_tag(
                                              uint32_t nvc_1,
                                              struct sst_obj_table_t *obj_table)
{
    struct sst_crypto_assoc_data_t assoc_data;
    union sst_crypto_t *crypto = &obj_table->crypto;

    /* Get new IV */
    sst_crypto_get_iv(crypto);

    assoc_data.nv_counter = nvc_1;
    (void)tfm_memcpy(assoc_data.obj_table_data,
                     SST_CRYPTO_ASSOCIATED_DATA(crypto),
                     SST_OBJ_TABLE_AUTH_DATA_SIZE);

    return sst_crypto_generate_auth_tag(crypto, (const uint8_t *)&assoc_data,
                                        SST_CRYPTO_ASSOCIATED_DATA_LEN);
}

/**
 * \brief Authenticates table of objects.
 *
 * \param[in]     table_idx  Table index in the init context
 * \param[in,out] init_ctx   Pointer to the object table to authenticate
 *
 */
static void sst_object_table_authenticate(uint8_t table_idx,
                                      struct sst_obj_table_init_ctx_t *init_ctx)
{
    struct sst_crypto_assoc_data_t assoc_data;
    union sst_crypto_t *crypto = &init_ctx->p_table[table_idx]->crypto;
    psa_status_t err;

    /* Init associated data with NVC 1 */
    assoc_data.nv_counter = init_ctx->nvc_1;
    (void)tfm_memcpy(assoc_data.obj_table_data,
                     SST_CRYPTO_ASSOCIATED_DATA(crypto),
                     SST_OBJ_TABLE_AUTH_DATA_SIZE);

    err = sst_crypto_authenticate(crypto, (const uint8_t *)&assoc_data,
                                  SST_CRYPTO_ASSOCIATED_DATA_LEN);
    if (err == PSA_SUCCESS) {
        init_ctx->table_state[table_idx] = SST_OBJ_TABLE_NVC_1_VALID;
        return;
    }

    if (init_ctx->nvc_3 == SST_INVALID_NVC_VALUE) {
        init_ctx->table_state[table_idx] = SST_OBJ_TABLE_INVALID;
        return;
    }

    /* Check with NVC 3 */
    assoc_data.nv_counter = init_ctx->nvc_3;

    err = sst_crypto_authenticate(crypto, (const uint8_t *)&assoc_data,
                                  SST_CRYPTO_ASSOCIATED_DATA_LEN);
    if (err != PSA_SUCCESS) {
        init_ctx->table_state[table_idx] = SST_OBJ_TABLE_INVALID;
    } else {
        init_ctx->table_state[table_idx] = SST_OBJ_TABLE_NVC_3_VALID;
    }
}

/**
 * \brief Authenticates tables of objects.
 *
 * \param[in,out] init_ctx  Pointer to the object table to authenticate
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
__attribute__ ((always_inline))
__STATIC_INLINE psa_status_t sst_object_table_nvc_authenticate(
                                      struct sst_obj_table_init_ctx_t *init_ctx)
{
    psa_status_t err;
    uint32_t nvc_2;

    err = sst_read_nv_counter(TFM_SST_NV_COUNTER_1, &init_ctx->nvc_1);
    if (err != PSA_SUCCESS) {
        return err;
    }

    err = sst_read_nv_counter(TFM_SST_NV_COUNTER_2, &nvc_2);
    if (err != PSA_SUCCESS) {
        return err;
    }

    err = sst_read_nv_counter(TFM_SST_NV_COUNTER_3, &init_ctx->nvc_3);
    if (err != PSA_SUCCESS) {
        return err;
    }

    /* Check if NVC 3 value can be used to validate an object table */
    if (init_ctx->nvc_3 != nvc_2) {
        /* If NVC 3 is different from NVC 2, it is possible to load an old SST
         * area image in the system by manipulating the FS to return a system
         * error from the file system layer and triggering power fault before
         * increasing the NVC 3. So, in that case, NVC 3 value cannot be used to
         * validate an old object table at the init process.
         */
        init_ctx->nvc_3 = SST_INVALID_NVC_VALUE;
    }

    /* Authenticate table 0 if data is valid */
    if (init_ctx->table_state[SST_OBJ_TABLE_IDX_0] != SST_OBJ_TABLE_INVALID) {
        sst_object_table_authenticate(SST_OBJ_TABLE_IDX_0, init_ctx);
    }

    /* Authenticate table 1 if data is valid */
    if (init_ctx->table_state[SST_OBJ_TABLE_IDX_1] != SST_OBJ_TABLE_INVALID) {
        sst_object_table_authenticate(SST_OBJ_TABLE_IDX_1, init_ctx);
    }

    return PSA_SUCCESS;
}
#else /* SST_ROLLBACK_PROTECTION */

/**
 * \brief Generates table authentication
 *
 * \param[in,out] obj_table  Pointer to the object table to generate
 *                           authentication
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
__attribute__ ((always_inline))
__STATIC_INLINE psa_status_t sst_object_table_generate_auth_tag(
                                              struct sst_obj_table_t *obj_table)
{
    union sst_crypto_t *crypto = &obj_table->crypto;

    /* Get new IV */
    sst_crypto_get_iv(crypto);

    return sst_crypto_generate_auth_tag(crypto,
                                        SST_CRYPTO_ASSOCIATED_DATA(crypto),
                                        SST_CRYPTO_ASSOCIATED_DATA_LEN);
}

/**
 * \brief Authenticates tables of objects.
 *
 * \param[in,out] init_ctx  Pointer to the object table to authenticate
 *
 */
__attribute__ ((always_inline))
__STATIC_INLINE void sst_object_table_authenticate_ctx_tables(
                                      struct sst_obj_table_init_ctx_t *init_ctx)
{
    psa_status_t err;
    union sst_crypto_t *crypto =
                                &init_ctx->p_table[SST_OBJ_TABLE_IDX_0]->crypto;

    /* Authenticate table 0 if data is valid */
    if (init_ctx->table_state[SST_OBJ_TABLE_IDX_0] != SST_OBJ_TABLE_INVALID) {
        err = sst_crypto_authenticate(crypto,
                                      SST_CRYPTO_ASSOCIATED_DATA(crypto),
                                      SST_CRYPTO_ASSOCIATED_DATA_LEN);
        if (err != PSA_SUCCESS) {
            init_ctx->table_state[SST_OBJ_TABLE_IDX_0] = SST_OBJ_TABLE_INVALID;
        }
    }

    /* Authenticate table 1 if data is valid */
    if (init_ctx->table_state[SST_OBJ_TABLE_IDX_1] != SST_OBJ_TABLE_INVALID) {
        crypto = &init_ctx->p_table[SST_OBJ_TABLE_IDX_1]->crypto;

        err = sst_crypto_authenticate(crypto,
                                      SST_CRYPTO_ASSOCIATED_DATA(crypto),
                                      SST_CRYPTO_ASSOCIATED_DATA_LEN);
        if (err != PSA_SUCCESS) {
            init_ctx->table_state[SST_OBJ_TABLE_IDX_1] = SST_OBJ_TABLE_INVALID;
        }
    }
}
#endif /* SST_ROLLBACK_PROTECTION */
#endif /* SST_ENCRYPTION */

/**
 * \brief Saves object table in the persistent memory.
 *
 * \param[in,out] obj_table  Pointer to the object table to save
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
static psa_status_t sst_object_table_save_table(
                                              struct sst_obj_table_t *obj_table)
{
    psa_status_t err;

#ifdef SST_ROLLBACK_PROTECTION
    uint32_t nvc_1 = 0;

    err = sst_increment_nv_counter(TFM_SST_NV_COUNTER_1);
    if (err != PSA_SUCCESS) {
        return err;
    }

    err = sst_read_nv_counter(TFM_SST_NV_COUNTER_1, &nvc_1);
    if (err != PSA_SUCCESS) {
        return err;
    }
#else
    obj_table->swap_count++;

    if (obj_table->swap_count == SST_FLASH_DEFAULT_VAL) {
        /* When a flash block is erased, the default value is usually 0xFF
         * (i.e. all 1s). Since the swap count is updated last (when encryption
         * is disabled), it is possible that due to a power failure, the swap
         * count value in metadata header is 0xFFFF..., which mean it will
         * appear to be most recent block.
         */
        obj_table->swap_count = 0;
    }
#endif /* SST_ROLLBACK_PROTECTION */

#ifdef SST_ENCRYPTION
    /* Set object table key */
    err = sst_crypto_setkey();
    if (err != PSA_SUCCESS) {
        return err;
    }

#ifdef SST_ROLLBACK_PROTECTION
    /* Generate authentication tag from the current table content and SST
     * NV counter 1.
     */
    err = sst_object_table_nvc_generate_auth_tag(nvc_1, obj_table);
#else
    /* Generate authentication tag from the current table content */
    err = sst_object_table_generate_auth_tag(obj_table);
#endif /* SST_ROLLBACK_PROTECTION */

    if (err != PSA_SUCCESS) {
        (void)sst_crypto_destroykey();
        return err;
    }

    err = sst_crypto_destroykey();
    if (err != PSA_SUCCESS) {
        return err;
    }
#endif /* SST_ENCRYPTION */

    err = sst_object_table_fs_write_table(obj_table);

#ifdef SST_ROLLBACK_PROTECTION
    if (err != PSA_SUCCESS) {
        return err;
    }

    /* Align SST NV counters to have the same value */
    err = sst_object_table_align_nv_counters(nvc_1);
#endif /* SST_ROLLBACK_PROTECTION */

    return err;
}

/**
 * \brief Checks the validity of the table version.
 *
 * \param[in,out] init_ctx  Pointer to the init object table context
 *
 */
__attribute__ ((always_inline))
__STATIC_INLINE void sst_object_table_validate_version(
                                      struct sst_obj_table_init_ctx_t *init_ctx)
{
    /* Looks for exact version number.
     * FIXME: backward compatibility could be considered in future revisions.
     */
    if (SST_OBJECT_SYSTEM_VERSION !=
        init_ctx->p_table[SST_OBJ_TABLE_IDX_0]->version) {
        init_ctx->table_state[SST_OBJ_TABLE_IDX_0] = SST_OBJ_TABLE_INVALID;
    }

    if (SST_OBJECT_SYSTEM_VERSION !=
        init_ctx->p_table[SST_OBJ_TABLE_IDX_1]->version) {
        init_ctx->table_state[SST_OBJ_TABLE_IDX_1] = SST_OBJ_TABLE_INVALID;
    }
}

/**
 * \brief Sets the active object table based on the swap count and validity of
 *        the object table data.
 *
 * \param[in] init_ctx  Pointer to the init object table context
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
static psa_status_t sst_set_active_object_table(
                                const struct sst_obj_table_init_ctx_t *init_ctx)
{
#ifndef SST_ROLLBACK_PROTECTION
    uint8_t table0_swap_count =
                             init_ctx->p_table[SST_OBJ_TABLE_IDX_0]->swap_count;
    uint8_t table1_swap_count =
                             init_ctx->p_table[SST_OBJ_TABLE_IDX_1]->swap_count;
#endif

    /* Check if there is an invalid object table */
    if ((init_ctx->table_state[SST_OBJ_TABLE_IDX_0] == SST_OBJ_TABLE_INVALID)
         && (init_ctx->table_state[SST_OBJ_TABLE_IDX_1] ==
                                                       SST_OBJ_TABLE_INVALID)) {
        /* Both tables are invalid */
        return PSA_ERROR_GENERIC_ERROR;
    } else if (init_ctx->table_state[SST_OBJ_TABLE_IDX_0] ==
                                                        SST_OBJ_TABLE_INVALID) {
          /* Table 0 is invalid, the active one is table 1 */
          sst_obj_table_ctx.active_table  = SST_OBJ_TABLE_IDX_1;
          sst_obj_table_ctx.scratch_table = SST_OBJ_TABLE_IDX_0;

          /* As table 1 is the active object, load the content into the
           * SST object table context.
           */
          (void)tfm_memcpy(&sst_obj_table_ctx.obj_table,
                           init_ctx->p_table[SST_OBJ_TABLE_IDX_1],
                           SST_OBJ_TABLE_SIZE);

          return PSA_SUCCESS;
    } else if (init_ctx->table_state[SST_OBJ_TABLE_IDX_1] ==
                                                        SST_OBJ_TABLE_INVALID) {
        /* Table 1 is invalid, the active one is table 0 */
        sst_obj_table_ctx.active_table  = SST_OBJ_TABLE_IDX_0;
        sst_obj_table_ctx.scratch_table = SST_OBJ_TABLE_IDX_1;

        /* As table 0 is already in the SST object table context, it is not
         * needed to copy the table in the context.
         */

        return PSA_SUCCESS;
    }

#ifdef SST_ROLLBACK_PROTECTION
    if (init_ctx->table_state[SST_OBJ_TABLE_IDX_1] ==
                                                    SST_OBJ_TABLE_NVC_1_VALID) {
        /* Table 0 is invalid, the active one is table 1 */
        sst_obj_table_ctx.active_table  = SST_OBJ_TABLE_IDX_1;
        sst_obj_table_ctx.scratch_table = SST_OBJ_TABLE_IDX_0;
    } else {
        /* In case both tables are valid or table 0 is valid, table 0 is the
         * valid on as it is already in the SST object table context.
         */
        sst_obj_table_ctx.active_table  = SST_OBJ_TABLE_IDX_0;
        sst_obj_table_ctx.scratch_table = SST_OBJ_TABLE_IDX_1;
    }
#else
    /* Logic: if the swap count is 0, then it has rolled over. The object table
     * with a swap count of 0 is the latest one, unless the other block has a
     * swap count of 1, in which case the roll over occurred in the previous
     * update. In all other cases, the table with the highest swap count is the
     * latest one.
     */
    if ((table1_swap_count == 0) && (table0_swap_count != 1)) {
        /* Table 1 swap count has rolled over and table 0 swap count has not,
         * so table 1 is the latest.
         */
        sst_obj_table_ctx.active_table  = SST_OBJ_TABLE_IDX_1;
        sst_obj_table_ctx.scratch_table = SST_OBJ_TABLE_IDX_0;

    } else if ((table0_swap_count == 0) && (table1_swap_count != 1)) {
        /* Table 0 swap count has rolled over and table 1 swap count has not,
         * so table 0 is the latest.
         */
        sst_obj_table_ctx.active_table  = SST_OBJ_TABLE_IDX_0;
        sst_obj_table_ctx.scratch_table = SST_OBJ_TABLE_IDX_1;

    } else if (table1_swap_count > table0_swap_count) {
        /* Neither swap count has just rolled over and table 1 has a
         * higher swap count, so table 1 is the latest.
         */
        sst_obj_table_ctx.active_table  = SST_OBJ_TABLE_IDX_1;
        sst_obj_table_ctx.scratch_table = SST_OBJ_TABLE_IDX_0;

    } else {
        /* Neither swap count has just rolled over and table 0 has a
         * higher or equal swap count, so table 0 is the latest.
         */
        sst_obj_table_ctx.active_table  = SST_OBJ_TABLE_IDX_0;
        sst_obj_table_ctx.scratch_table = SST_OBJ_TABLE_IDX_1;
    }
#endif /* SST_ROLLBACK_PROTECTION */

    /* If active object table is table 1, then copy the content into the
     * SST object table context.
     */
    if (sst_obj_table_ctx.active_table == SST_OBJ_TABLE_IDX_1) {
        (void)tfm_memcpy(&sst_obj_table_ctx.obj_table,
                         init_ctx->p_table[SST_OBJ_TABLE_IDX_1],
                         SST_OBJ_TABLE_SIZE);
    }

    return PSA_SUCCESS;
}

/**
 * \brief Gets table's entry index based on the given object UID and client ID.
 *
 * \param[in]  uid        Object UID
 * \param[in]  client_id  Client UID
 * \param[out] idx        Pointer to store the entry's index
 *
 * \return Returns PSA_SUCCESS and index of the table, if object exists
 *         in the table. Otherwise, it returns PSA_ERROR_DOES_NOT_EXIST.
 */
static psa_status_t sst_get_object_entry_idx(psa_storage_uid_t uid,
                                             int32_t client_id,
                                             uint32_t *idx)
{
    uint32_t i;
    struct sst_obj_table_t *p_table = &sst_obj_table_ctx.obj_table;

    for (i = 0; i < SST_OBJ_TABLE_ENTRIES; i++) {
        if (p_table->obj_db[i].uid == uid
            && p_table->obj_db[i].client_id == client_id) {
            *idx = i;
            return PSA_SUCCESS;
        }
    }

    return PSA_ERROR_DOES_NOT_EXIST;
}

/**
 * \brief Gets free index in the table
 *
 * \param[in] idx_num  The number of indices required to be free before one can
 *                     be allocated. Primarily used to prevent index
 *                     exhaustion.Note that this function will only ever return
 *                     1 index.
 * \param[out] idx     Pointer to store the free index
 *
 * \note The table is dimensioned to fit SST_NUM_ASSETS + 1
 *
 * \return Returns PSA_SUCCESS and a table index if idx_num free indices are
 *         available. Otherwise, it returns PSA_ERROR_INSUFFICIENT_STORAGE.
 */
__attribute__ ((always_inline))
__STATIC_INLINE psa_status_t sst_table_free_idx(uint32_t idx_num,
                                                uint32_t *idx)
{
    uint32_t i;
    uint32_t last_free = 0;
    struct sst_obj_table_t *p_table = &sst_obj_table_ctx.obj_table;

    if (idx_num == 0) {
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    for (i = 0; i < SST_OBJ_TABLE_ENTRIES && idx_num > 0; i++) {
        if (p_table->obj_db[i].uid == TFM_SST_INVALID_UID) {
            last_free = i;
            idx_num--;
        }
    }

    if (idx_num != 0) {
        return PSA_ERROR_INSUFFICIENT_STORAGE;
    } else {
        *idx = last_free;
        return PSA_SUCCESS;
    }
}

/**
 * \brief Deletes an entry from the table
 *
 * \param[in] idx  Entry index to delete
 *
 */
static void sst_table_delete_entry(uint32_t idx)
{
    /* Initialise object table entry structure */
    (void)tfm_memset(&sst_obj_table_ctx.obj_table.obj_db[idx],
                     SST_DEFAULT_EMPTY_BUFF_VAL, SST_OBJECTS_TABLE_ENTRY_SIZE);
}

psa_status_t sst_object_table_create(void)
{
    struct sst_obj_table_t *p_table = &sst_obj_table_ctx.obj_table;

    /* Initialize object structure */
    (void)tfm_memset(&sst_obj_table_ctx, SST_DEFAULT_EMPTY_BUFF_VAL,
                     sizeof(struct sst_obj_table_ctx_t));

    /* Invert the other in the context as sst_object_table_save_table will
     * use the scratch index to create and store the current table.
     */
    sst_obj_table_ctx.active_table  = SST_OBJ_TABLE_IDX_1;
    sst_obj_table_ctx.scratch_table = SST_OBJ_TABLE_IDX_0;

    p_table->version = SST_OBJECT_SYSTEM_VERSION;

    /* Save object table contents */
    return sst_object_table_save_table(p_table);
}

psa_status_t sst_object_table_init(uint8_t *obj_data)
{
    psa_status_t err;
    struct sst_obj_table_init_ctx_t init_ctx = {
        .p_table = {&sst_obj_table_ctx.obj_table, NULL},
        .table_state = {SST_OBJ_TABLE_VALID, SST_OBJ_TABLE_VALID},
#ifdef SST_ROLLBACK_PROTECTION
        .nvc_1 = 0U,
        .nvc_3 = 0U,
#endif /* SST_ROLLBACK_PROTECTION */
    };

    init_ctx.p_table[SST_OBJ_TABLE_IDX_1] = (struct sst_obj_table_t *)obj_data;

    /* Read table from the file system */
    sst_object_table_fs_read_table(&init_ctx);

#ifdef SST_ENCRYPTION
    /* Set object table key */
    err = sst_crypto_setkey();
    if (err != PSA_SUCCESS) {
        return err;
    }

#ifdef SST_ROLLBACK_PROTECTION
    /* Authenticate table */
    err = sst_object_table_nvc_authenticate(&init_ctx);
    if (err != PSA_SUCCESS) {
        (void)sst_crypto_destroykey();
        return err;
    }
#else
    sst_object_table_authenticate_ctx_tables(&init_ctx);
#endif /* SST_ROLLBACK_PROTECTION */

    err = sst_crypto_destroykey();
    if (err != PSA_SUCCESS) {
        return err;
    }
#endif /* SST_ENCRYPTION */

    /* Check tables version */
    sst_object_table_validate_version(&init_ctx);

    /* Set active tables */
    err = sst_set_active_object_table(&init_ctx);
    if (err != PSA_SUCCESS) {
        return err;
    }

    /* Remove the old object table file */
    err = psa_its_remove(SST_TABLE_FS_ID(sst_obj_table_ctx.scratch_table));
    if (err != PSA_SUCCESS && err != PSA_ERROR_DOES_NOT_EXIST) {
        return err;
    }

#ifdef SST_ROLLBACK_PROTECTION
    /* Align SST NV counters */
    err = sst_object_table_align_nv_counters(init_ctx.nvc_1);
    if (err != PSA_SUCCESS) {
        return err;
    }
#endif /* SST_ROLLBACK_PROTECTION */

#ifdef SST_ENCRYPTION
    sst_crypto_set_iv(&sst_obj_table_ctx.obj_table.crypto);
#endif

    return PSA_SUCCESS;
}

psa_status_t sst_object_table_obj_exist(psa_storage_uid_t uid,
                                        int32_t client_id)
{
    uint32_t idx = 0;

    return sst_get_object_entry_idx(uid, client_id, &idx);
}

psa_status_t sst_object_table_get_free_fid(uint32_t fid_num,
                                           uint32_t *p_fid)
{
    psa_status_t err;
    uint32_t fid;
    uint32_t idx;

    err = sst_table_free_idx(fid_num, &idx);
    if (err != PSA_SUCCESS) {
        return err;
    }

    /* There first two file IDs are reserved for the active table
     * and scratch table files.
     */
    fid = SST_OBJECT_FS_ID(idx);

    /* If there is a file in the persistent area with that ID then remove it.
     * That can happen when the system is rebooted (e.g. power cut, ...) in the
     * middle of a create, write or delete operation.
     */
    err = psa_its_remove(fid);
    if (err != PSA_SUCCESS && err != PSA_ERROR_DOES_NOT_EXIST) {
        return err;
    }

    *p_fid = fid;

    return PSA_SUCCESS;
}

psa_status_t sst_object_table_set_obj_tbl_info(psa_storage_uid_t uid,
                                               int32_t client_id,
                                const struct sst_obj_table_info_t *obj_tbl_info)
{
    psa_status_t err;
    uint32_t idx = 0;
    uint32_t backup_idx = 0;
    struct sst_obj_table_entry_t backup_entry = {
#ifdef SST_ENCRYPTION
        .tag = {0U},
#else
        .version = 0U,
#endif /* SST_ENCRYPTION */
        .uid = TFM_SST_INVALID_UID,
        .client_id = 0,
    };
    struct sst_obj_table_t *p_table = &sst_obj_table_ctx.obj_table;

    err = sst_get_object_entry_idx(uid, client_id, &backup_idx);
    if (err == PSA_SUCCESS) {
        /* If an entry exists for this UID, it creates a backup copy in case
         * an error happens while updating the new table in the filesystem.
         */
        (void)tfm_memcpy(&backup_entry, &p_table->obj_db[backup_idx],
                         SST_OBJECTS_TABLE_ENTRY_SIZE);

        /* Deletes old object information if it exist in the table */
        sst_table_delete_entry(backup_idx);
    }

    idx = SST_OBJECT_FS_ID_TO_IDX(obj_tbl_info->fid);
    p_table->obj_db[idx].uid = uid;
    p_table->obj_db[idx].client_id = client_id;

    /* Add new object information */
#ifdef SST_ENCRYPTION
    (void)tfm_memcpy(p_table->obj_db[idx].tag, obj_tbl_info->tag,
                     SST_TAG_LEN_BYTES);
#else
    p_table->obj_db[idx].version = obj_tbl_info->version;
#endif

    err = sst_object_table_save_table(p_table);
    if (err != PSA_SUCCESS) {
        if (backup_entry.uid != TFM_SST_INVALID_UID) {
            /* Rollback the change in the table */
            (void)tfm_memcpy(&p_table->obj_db[backup_idx], &backup_entry,
                             SST_OBJECTS_TABLE_ENTRY_SIZE);
        }

        sst_table_delete_entry(idx);
    }

    return err;
}

psa_status_t sst_object_table_get_obj_tbl_info(psa_storage_uid_t uid,
                                               int32_t client_id,
                                      struct sst_obj_table_info_t *obj_tbl_info)
{
    psa_status_t err;
    uint32_t idx;
    struct sst_obj_table_t *p_table = &sst_obj_table_ctx.obj_table;

    err = sst_get_object_entry_idx(uid, client_id, &idx);
    if (err != PSA_SUCCESS) {
        return err;
    }

    obj_tbl_info->fid = SST_OBJECT_FS_ID(idx);

#ifdef SST_ENCRYPTION
    (void)tfm_memcpy(obj_tbl_info->tag, p_table->obj_db[idx].tag,
                     SST_TAG_LEN_BYTES);
#else
    obj_tbl_info->version = p_table->obj_db[idx].version;
#endif

    return PSA_SUCCESS;
}

psa_status_t sst_object_table_delete_object(psa_storage_uid_t uid,
                                            int32_t client_id)
{
    uint32_t backup_idx = 0;
    struct sst_obj_table_entry_t backup_entry;
    psa_status_t err;
    struct sst_obj_table_t *p_table = &sst_obj_table_ctx.obj_table;

    /* Create a backup copy in case an error happens while updating the new
     * table in the filesystem.
     */
    err = sst_get_object_entry_idx(uid, client_id, &backup_idx);
    if (err != PSA_SUCCESS) {
        /* If the object is not present in the table, it returns an error
         * to not generate a new file where the table content is the same.
         * Otherwise, that could be used by an attacker to get the encryption
         * key.
         */
        return err;
    }

    (void)tfm_memcpy(&backup_entry, &p_table->obj_db[backup_idx],
                     SST_OBJECTS_TABLE_ENTRY_SIZE);

    sst_table_delete_entry(backup_idx);

    err = sst_object_table_save_table(p_table);
    if (err != PSA_SUCCESS) {
       /* Rollback the change in the table */
       (void)tfm_memcpy(&p_table->obj_db[backup_idx], &backup_entry,
                        SST_OBJECTS_TABLE_ENTRY_SIZE);
    }

    return err;
}

psa_status_t sst_object_table_delete_old_table(void)
{
    uint32_t table_id = SST_TABLE_FS_ID(sst_obj_table_ctx.scratch_table);

    return psa_its_remove(table_id);
}
