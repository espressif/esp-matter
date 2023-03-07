/*
 * Copyright (c) 2018-2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IOTSDK_KVSTORE_H
#define IOTSDK_KVSTORE_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

namespace iotsdk {
namespace storage {

/** Enum of status codes
 *
 *  @enum kv_status
 */
enum class kv_status {
    OK = 0,
    NOT_INITIALIZED = -1,
    INIT_FAIL = -2,
    ITEM_NOT_FOUND = -3,
    WRITE_FAILED = -4,
    READ_FAILED = -5,
    ERASE_FAILED = -6,
    INVALID_SIZE = -7,
    INVALID_ARGUMENT = -8,
    INVALID_DATA_DETECTED = -9,
    OUT_OF_RESOURCES = -10,
    WRITE_PROTECTED = -11,
    MEDIA_FULL = -12,
    SYNC_FAILED = -13,
    NOT_FINALIZED = -14,
    MEMORY_ALLOC_FAILED = -15
};

/** KVStore class
 *
 *  Interface class for Key Value Storage
 */
class KVStore {
public:
    enum create_flags {
        WRITE_ONCE_FLAG = (1 << 0),
        REQUIRE_CONFIDENTIALITY_FLAG = (1 << 1),
        RESERVED_FLAG = (1 << 2),
        REQUIRE_REPLAY_PROTECTION_FLAG = (1 << 3),
    };

    static const uint32_t MAX_KEY_SIZE = 128;

    typedef struct _opaque_set_handle *set_handle_t;

    typedef struct _opaque_key_iterator *iterator_t;

    /**
     * Holds key information
     */
    typedef struct info {
        /**
         * The key size
         */
        size_t size;
        /*
         * The Key flags, possible flags combination:
         * WRITE_ONCE_FLAG,
         * REQUIRE_CONFIDENTIALITY_FLAG,
         * REQUIRE_REPLAY_PROTECTION_FLAG
         */
        uint32_t flags;
    } info_t;

    virtual ~KVStore(){};

    /**
     * @brief Initialize KVStore
     *
     * @returns kv_status::OK on success or an error code on failure
     */
    virtual kv_status init() = 0;

    /**
     * @brief Deinitialize KVStore
     *
     * @returns kv_status::OK on success or an error code on failure
     */
    virtual kv_status deinit() = 0;

    /**
     * @brief Reset KVStore contents (clear all keys)
     *
     * @returns kv_status::OK on success or an error code on failure
     */
    virtual kv_status reset() = 0;

    /**
     * @brief Set one KVStore item, given key and value.
     *
     * @param[in]  key                  Key - must not include '*' '/' '?' ':' ';' '\' '"' '|' ' ' '<' '>' '\'.
     * @param[in]  buffer               Value data buffer.
     * @param[in]  size                 Value data size.
     * @param[in]  create_flags         Flag mask.
     *
     * @returns kv_status::OK on success or an error code on failure
     */
    virtual kv_status set(const char *key, const void *buffer, size_t size, uint32_t create_flags) = 0;

    /**
     * @brief Get one KVStore item, given key.
     *
     * @param[in]  key                  Key - must not include '*' '/' '?' ':' ';' '\' '"' '|' ' ' '<' '>' '\'.
     * @param[in]  buffer               Value data buffer.
     * @param[in]  buffer_size          Value data buffer size.
     * @param[out] actual_size          Actual read size (NULL to pass nothing).
     * @param[in]  offset               Offset to read from in data.
     *
     * @returns kv_status::OK on success or an error code on failure
     */
    virtual kv_status
    get(const char *key, void *buffer, size_t buffer_size, size_t *actual_size = NULL, size_t offset = 0) = 0;

    /**
     * @brief Get information of a given key.
     *
     * @param[in]  key                  Key - must not include '*' '/' '?' ':' ';' '\' '"' '|' ' ' '<' '>' '\'.
     * @param[out] info                 Returned information structure (NULL to pass nothing).
     *
     * @returns kv_status::OK on success or an error code on failure
     */
    virtual kv_status get_info(const char *key, info_t *info = NULL) = 0;

    /**
     * @brief Remove a KVStore item, given key.
     *
     * @param[in]  key                  Key - must not include '*' '/' '?' ':' ';' '\' '"' '|' ' ' '<' '>' '\'.
     *
     * @returns kv_status::OK on success or an error code on failure
     */
    virtual kv_status remove(const char *key) = 0;

    /**
     * @brief Start an incremental KVStore set sequence.
     *
     * @param[out] handle               Returned incremental set handle.
     * @param[in]  key                  Key - must not include '*' '/' '?' ':' ';' '\' '"' '|' ' ' '<' '>' '\'.
     * @param[in]  final_data_size      Final value data size.
     * @param[in]  create_flags         Flag mask.
     *
     * @returns kv_status::OK on success or an error code on failure
     */
    virtual kv_status
    set_start(set_handle_t *handle, const char *key, size_t final_data_size, uint32_t create_flags) = 0;

    /**
     * @brief Add data to incremental KVStore set sequence.
     *
     * @param[in]  handle               Incremental set handle.
     * @param[in]  value_data           Value data to add.
     * @param[in]  data_size            Value data size.
     *
     * @returns kv_status::OK on success or an error code on failure
     */
    virtual kv_status set_add_data(set_handle_t handle, const void *value_data, size_t data_size) = 0;

    /**
     * @brief Finalize an incremental KVStore set sequence.
     *
     * @param[in]  handle               Incremental set handle.
     *
     * @returns kv_status::OK on success or an error code on failure
     */
    virtual kv_status set_finalize(set_handle_t handle) = 0;

    /**
     * @brief Start an iteration over KVStore keys.
     *
     * @param[out] it                   Returned iterator handle.
     * @param[in]  prefix               Key prefix (null for all keys).
     *
     * @returns kv_status::OK on success or an error code on failure
     */
    virtual kv_status iterator_open(iterator_t *it, const char *prefix = NULL) = 0;

    /**
     * @brief Get next key in iteration.
     *
     * @param[in]  it                   Iterator handle.
     * @param[in]  key                  Buffer for returned key.
     * @param[in]  key_size             Key buffer size.
     *
     * @returns kv_status::OK on success or an error code on failure
     */
    virtual kv_status iterator_next(iterator_t it, char *key, size_t key_size) = 0;

    /**
     * @brief Close iteration.
     *
     * @param[in]  it                   Iterator handle.
     *
     * @returns kv_status::OK on success or an error code on failure
     */
    virtual kv_status iterator_close(iterator_t it) = 0;

    /** Convenience function for checking key validity.
     *  Key must not include '*' '/' '?' ':' ';' '\' '"' '|' ' ' '<' '>' '\'.
     *
     * @param[in]  key                  Key buffer.
     *
     * @returns kv_status::OK on success or an error code on failure
     */
    static bool is_valid_key(const char *key)
    {
        if (!key || !strlen(key) || (strlen(key) > MAX_KEY_SIZE)) {
            return false;
        }

        if (strpbrk(key, " */?:;\"|<>\\")) {
            return false;
        }
        return true;
    }
};

} // namespace storage
} // namespace iotsdk

#endif // IOTSDK_KVSTORE_H
