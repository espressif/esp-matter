/*
 * Copyright (c) 2018-2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

/** \addtogroup storage */
/** @{*/

#ifndef IOTSDK_TDBSTORE_H
#define IOTSDK_TDBSTORE_H

#include "iotsdk/BlockDevice.h"
#include "iotsdk/KVStore.h"

#include <stdint.h>
#include <stdio.h>

namespace iotsdk {
namespace storage {

class BufferedBlockDevice;

/** TDBStore class
 *
 *  Lightweight Key Value storage over a block device
 */

class TDBStore final : public KVStore {
public:
    static const uint32_t RESERVED_AREA_SIZE = 64;

    /**
     * @brief Class constructor
     *
     * @param[in]  bd                   Underlying block device.
     *
     * @returns none
     */
    explicit TDBStore(BlockDevice *bd);

    /**
     * @brief Initialize TDBStore. If data exists, TDBStore will check the data integrity
     *        on initialize. If the integrity checks fails, the TDBStore will use GC to collect
     *        the available data and clean corrupted and erroneous records.
     *
     * @returns kv_status::OK                       Success.
     * @returns Negative error code on failure.
     */
    kv_status init() override;

    /**
     * @brief Deinitialize TDBStore, release and free resources.
     *        Since TDBStore uses a BufferedBlockDevice to cache/buffer writes to the underlying
     *        BlockDevice storage, it is imperative to check the return code of deinit() and retry
     *        should the sync with underlying storage fail. Failure may otherwise result in data
     *        loss.
     *
     * @returns kv_status::OK                       Success.
     *          kv_status::SYNC_FAILED              Unable to write cached writes to storage
     */
    kv_status deinit() override;

    /**
     * @brief Reset TDBStore contents (clear all keys) and reserved data
     *
     * @returns kv_status::OK                       Success.
     *          kv_status::NOT_READY                Not initialized.
     *          kv_status::READ_FAILED              Unable to read from media.
     *          kv_status::WRITE_FAILED             Unable to write to media.
     */
    kv_status reset() override;

    /**
     * @brief Set one TDBStore item, given key and value.
     *
     * @param[in]  key                  Key - must not include '*' '/' '?' ':' ';' '\' '"' '|' ' ' '<' '>' '\'.
     * @param[in]  buffer               Value data buffer.
     * @param[in]  size                 Value data size.
     * @param[in]  creation_flags       Flag mask.
     *
     * @returns kv_status::OK                       Success.
     *          kv_status::NOT_READY                Not initialized.
     *          kv_status::READ_FAILED              Unable to read from media.
     *          kv_status::WRITE_FAILED             Unable to write to media.
     *          kv_status::INVALID_ARGUMENT         Invalid argument given in function arguments.
     *          kv_status::INVALID_SIZE             Invalid size given in function arguments.
     *          kv_status::MEDIA_FULL               Not enough room on media.
     *          kv_status::WRITE_PROTECTED          Already stored with "write once" flag.
     */
    kv_status set(const char *key, const void *buffer, size_t size, uint32_t creation_flags) override;

    /**
     * @brief Get one TDBStore item by given key.
     *
     * @param[in]  key                  Key - must not include '*' '/' '?' ':' ';' '\' '"' '|' ' ' '<' '>' '\'.
     * @param[in]  buffer               Value data buffer.
     * @param[in]  buffer_size          Value data buffer size.
     * @param[out] actual_size          Actual read size.
     * @param[in]  offset               Offset to read from in data.
     *
     * @returns kv_status::OK                       Success.
     *          kv_status::NOT_READY                Not initialized.
     *          kv_status::READ_FAILED              Unable to read from media.
     *          kv_status::INVALID_ARGUMENT         Invalid argument given in function arguments.
     *          kv_status::INVALID_SIZE             Invalid size given in function arguments.
     *          kv_status::INVALID_DATA_DETECTED    Data is corrupt.
     *          kv_status::ITEM_NOT_FOUND           No such key.
     */
    kv_status
    get(const char *key, void *buffer, size_t buffer_size, size_t *actual_size = NULL, size_t offset = 0) override;

    /**
     * @brief Get information of a given key. The returned info contains size and flags
     *
     * @param[in]  key                  Key - must not include '*' '/' '?' ':' ';' '\' '"' '|' ' ' '<' '>' '\'.
     * @param[out] elem_info            Returned information structure.
     *
     * @returns kv_status::OK                       Success.
     *          kv_status::NOT_READY                Not initialized.
     *          kv_status::READ_FAILED              Unable to read from media.
     *          kv_status::INVALID_ARGUMENT         Invalid argument given in function arguments.
     *          kv_status::INVALID_DATA_DETECTED    Data is corrupt.
     *          kv_status::ITEM_NOT_FOUND           No such key.
     */
    kv_status get_info(const char *key, info_t *elem_info) override;

    /**
     * @brief Remove a TDBStore item by given key.
     *
     * @param[in]  key                  Key - must not include '*' '/' '?' ':' ';' '\' '"' '|' ' ' '<' '>' '\'.
     *
     * @returns kv_status::OK                       Success.
     *          kv_status::NOT_READY                Not initialized.
     *          kv_status::READ_FAILED              Unable to read from media.
     *          kv_status::WRITE_FAILED             Unable to write to media.
     *          kv_status::INVALID_ARGUMENT         Invalid argument given in function arguments.
     *          kv_status::MEDIA_FULL               Not enough room on media.
     *          kv_status::ITEM_NOT_FOUND           No such key.
     *          kv_status::WRITE_PROTECTED          Already stored with "write once" flag.
     */
    kv_status remove(const char *key) override;

    /**
     * @brief Start an incremental TDBStore set sequence. This operation is blocking other operations.
     *        Any get/set/remove/iterator operation will be blocked until set_finalize is called.
     *
     * @param[out] handle               Returned incremental set handle.
     * @param[in]  key                  Key - must not include '*' '/' '?' ':' ';' '\' '"' '|' ' ' '<' '>' '\'.
     * @param[in]  final_data_size      Final value data size.
     * @param[in]  creation_flags       Flag mask.
     *
     * @returns kv_status::OK                       Success.
     *          kv_status::NOT_READY                Not initialized.
     *          kv_status::READ_FAILED              Unable to read from media.
     *          kv_status::WRITE_FAILED             Unable to write to media.
     *          kv_status::INVALID_ARGUMENT         Invalid argument given in function arguments.
     *          kv_status::INVALID_SIZE             Invalid size given in function arguments.
     *          kv_status::MEDIA_FULL               Not enough room on media.
     *          kv_status::WRITE_PROTECTED          Already stored with "write once" flag.
     */
    kv_status
    set_start(set_handle_t *handle, const char *key, size_t final_data_size, uint32_t creation_flags) override;

    /**
     * @brief Add data to incremental TDBStore set sequence. This operation is blocking other operations.
     *        Any get/set/remove operation will be blocked until set_finalize will be called.
     *
     * @param[in]  handle               Incremental set handle.
     * @param[in]  value_data           Value data to add.
     * @param[in]  data_size            Value data size.
     *
     * @returns kv_status::OK                       Success.
     *          kv_status::NOT_READY                Not initialized.
     *          kv_status::WRITE_FAILED             Unable to write to media.
     *          kv_status::INVALID_ARGUMENT         Invalid argument given in function arguments.
     *          kv_status::INVALID_SIZE             Invalid size given in function arguments.
     */
    kv_status set_add_data(set_handle_t handle, const void *value_data, size_t data_size) override;

    /**
     * @brief Finalize an incremental KVStore set sequence.
     *
     * @param[in]  handle               Incremental set handle.
     *
     * @returns kv_status::OK                       Success.
     *          kv_status::NOT_READY                Not initialized.
     *          kv_status::WRITE_FAILED             Unable to write to media.
     *          kv_status::INVALID_ARGUMENT         Invalid argument given in function arguments.
     */
    kv_status set_finalize(set_handle_t handle) override;

    /**
     * @brief Start an iteration over KVStore keys.
     *        There are no issues with any other operations while iterator is open.
     *
     * @param[out] it                   Returned iterator handle.
     * @param[in]  prefix               Key prefix (null for all keys).
     *
     * @returns kv_status::OK                       Success.
     *          kv_status::NOT_READY                Not initialized.
     *          kv_status::INVALID_ARGUMENT         Invalid argument given in function arguments.
     */
    kv_status iterator_open(iterator_t *it, const char *prefix = NULL) override;

    /**
     * @brief Get next key in iteration.
     *        There are no issues with any other operations while iterator is open.
     *
     * @param[in]  it                   Iterator handle.
     * @param[in]  key                  Buffer for returned key.
     * @param[in]  key_size             Key buffer size.
     *
     * @returns kv_status::OK                       Success.
     *          kv_status::NOT_READY                Not initialized.
     *          kv_status::READ_FAILED              Unable to read from block device.
     *          kv_status::INVALID_ARGUMENT         Invalid argument given in function arguments.
     *          kv_status::INVALID_SIZE             Invalid size given in function arguments.
     *          kv_status::INVALID_DATA_DETECTED    Data is corrupt.
     *          kv_status::ITEM_NOT_FOUND           No more keys found.
     */
    kv_status iterator_next(iterator_t it, char *key, size_t key_size) override;

    /**
     * @brief Close iteration.
     *
     * @param[in]  it                   Iterator handle.
     *
     * @returns kv_status::OK                       Success.
     *          kv_status::NOT_READY                Not initialized.
     *          kv_status::INVALID_ARGUMENT         Invalid argument given in function arguments.
     */
    kv_status iterator_close(iterator_t it) override;

    /**
     * @brief Set data in reserved area, which is a special location for special data, such as ROT.
     *        The data written to reserved area can't be overwritten.
     *
     * @param[in]  reserved_data        Reserved data buffer.
     * @param[in]  reserved_data_buf_size
     *                                  Reserved data buffer size.
     *
     * @returns kv_status::OK                       Success.
     *          kv_status::NOT_READY                Not initialized.
     *          kv_status::READ_FAILED              Unable to read from media.
     *          kv_status::WRITE_FAILED             Unable to write to media.
     *          kv_status::INVALID_ARGUMENT         Invalid argument given in function arguments.
     *          kv_status::INVALID_SIZE             Invalid size given in function arguments.
     */
    kv_status reserved_data_set(const void *reserved_data, size_t reserved_data_buf_size);

    /**
     * @brief Get data from reserved area, which is a special location for special data, such as ROT.
     *
     * @param[in]  reserved_data        Reserved data buffer.
     * @param[in]  reserved_data_buf_size
     *                                  Reserved data buffer size.
     * @param[in]  actual_data_size     Return data size.
     *
     * @returns kv_status::OK                       Success.
     *          kv_status::NOT_READY                Not initialized.
     *          kv_status::READ_FAILED              Unable to read from media.
     *          kv_status::INVALID_ARGUMENT         Invalid argument given in function arguments.
     *          kv_status::INVALID_DATA_DETECTED    Data is corrupt.
     *          kv_status::ITEM_NOT_FOUND           No reserved data was written.
     */
    kv_status reserved_data_get(void *reserved_data, size_t reserved_data_buf_size, size_t *actual_data_size = 0);

private:
    typedef struct {
        bd_size_t address;
        bd_size_t size;
    } tdbstore_area_data_t;

    static const int _num_areas = 2;
    static const int _max_open_iterators = 16;

    void *_ram_table;
    size_t _max_keys;
    size_t _num_keys;
    BlockDevice *_bd;
    BufferedBlockDevice *_buff_bd;
    size_t _free_space_offset;
    size_t _master_record_offset;
    size_t _master_record_size;
    bool _is_initialized;
    uint8_t _active_area;
    uint16_t _active_area_version;
    bd_size_t _size;
    tdbstore_area_data_t _area_params[_num_areas];
    size_t _prog_size;
    uint8_t *_work_buf;
    size_t _work_buf_size;
    char *_key_buf;
    void *_inc_set_handle;
    void *_iterator_table[_max_open_iterators];
    bool _inc_set_in_progress = false;

    /**
     * @brief Read a block from an area.
     *
     * @param[in]  area                   Area.
     * @param[in]  offset                 Offset in area.
     * @param[in]  size                   Number of bytes to read.
     * @param[in]  buf                    Output buffer.
     *
     * @returns 0 for success, nonzero for failure.
     */
    kv_status read_area(uint8_t area, size_t offset, size_t size, void *buf);

    /**
     * @brief Write a block to an area.
     *
     * @param[in]  area                   Area.
     * @param[in]  offset                 Offset in area.
     * @param[in]  size                   Number of bytes to write.
     * @param[in]  buf                    Input buffer.
     *
     * @returns 0 for success, non-zero for failure.
     */
    kv_status write_area(uint8_t area, size_t offset, size_t size, const void *buf);

    /**
     * @brief Reset an area (erase its start).
     *        This erases master record, but preserves the
     *        reserved area data.
     *
     * @param[in]  area                   Area.
     *
     * @returns 0 for success, nonzero for failure.
     */
    kv_status reset_area(uint8_t area);

    /**
     * @brief Erase an area.
     *
     * @param[in]  area                   Area.
     * @param[in]  offset                 Offset in area.
     * @param[in]  size                   Number of bytes to erase.
     *
     * @returns 0 for success, nonzero for failure.
     */
    kv_status erase_area(uint8_t area, size_t offset, size_t size);

    /**
     * @brief Private implementation of public set() method
     *        Used by init() to set the master record before initialization has completed
     *
     * @param[in]  key                  Key - must not include '*' '/' '?' ':' ';' '\' '"' '|' ' ' '<' '>' '\'.
     * @param[in]  buffer               Value data buffer.
     * @param[in]  size                 Value data size.
     * @param[in]  creation_flags       Flag mask.
     *
     * @returns kv_status::OK                       Success.
     *          kv_status::NOT_READY                Not initialized.
     *          kv_status::READ_FAILED              Unable to read from media.
     *          kv_status::WRITE_FAILED             Unable to write to media.
     *          kv_status::INVALID_ARGUMENT         Invalid argument given in function arguments.
     *          kv_status::INVALID_SIZE             Invalid size given in function arguments.
     *          kv_status::MEDIA_FULL               Not enough room on media.
     *          kv_status::WRITE_PROTECTED          Already stored with "write once" flag.
     */
    kv_status set_impl(const char *key, const void *buffer, size_t size, uint32_t creation_flags);

    /**
     * @brief Private implementation of public set_start() method
     *        Used by set_impl to implement set without initialization check of public set_start method
     *
     * @param[out] handle               Returned incremental set handle.
     * @param[in]  key                  Key - must not include '*' '/' '?' ':' ';' '\' '"' '|' ' ' '<' '>' '\'.
     * @param[in]  final_data_size      Final value data size.
     * @param[in]  creation_flags       Flag mask.
     *
     * @returns kv_status::OK                       Success.
     *          kv_status::NOT_READY                Not initialized.
     *          kv_status::READ_FAILED              Unable to read from media.
     *          kv_status::WRITE_FAILED             Unable to write to media.
     *          kv_status::INVALID_ARGUMENT         Invalid argument given in function arguments.
     *          kv_status::INVALID_SIZE             Invalid size given in function arguments.
     *          kv_status::MEDIA_FULL               Not enough room on media.
     *          kv_status::WRITE_PROTECTED          Already stored with "write once" flag.
     */
    kv_status set_start_impl(set_handle_t *handle, const char *key, size_t final_data_size, uint32_t creation_flags);

    /**
     * @brief Calculate addresses and sizes of areas.
     */
    void calc_area_params();

    /**
     * @brief Read a TDBStore record from a given location.
     *
     * @param[in]  area                   Area.
     * @param[in]  offset                 Offset of record in area.
     * @param[out] key                    Key - must not include '*' '/' '?' ':' ';' '\' '"' '|' ' ' '<' '>' '\'.
     * @param[out] data_buf               Data buffer.
     * @param[in]  data_buf_size          Data buffer size.
     * @param[out] actual_data_size       Actual data size.
     * @param[in]  data_offset            Offset in data.
     * @param[in]  copy_key               Copy key to user buffer.
     * @param[in]  copy_data              Copy data to user buffer.
     * @param[in]  check_expected_key     Check whether key belongs to this record.
     * @param[in]  calc_hash              Calculate hash (on key).
     * @param[out] hash                   Calculated hash.
     * @param[out] flags                  Record flags.
     * @param[out] next_offset            Offset of next record.
     *
     * @returns 0 for success, nonzero for failure.
     */
    kv_status read_record(uint8_t area,
                          size_t offset,
                          char *key,
                          void *data_buf,
                          size_t data_buf_size,
                          size_t &actual_data_size,
                          size_t data_offset,
                          bool copy_key,
                          bool copy_data,
                          bool check_expected_key,
                          bool calc_hash,
                          uint32_t &hash,
                          uint32_t &flags,
                          size_t &next_offset);

    /**
     * @brief Write a master record of active area.
     *
     * @param[in]  version                Area version.
     * @param[out] next_offset            Offset of next record.
     *
     * @returns 0 for success, nonzero for failure.
     */
    kv_status write_master_record(uint16_t version, size_t &next_offset);

    /**
     * @brief Copy a record from one area to the opposite one.
     *
     * @param[in]  from_area              Area to copy record from.
     * @param[in]  from_offset            Offset in source area.
     * @param[in]  to_offset              Offset in destination area.
     * @param[out] to_next_offset         Offset of next record in destination area.
     *
     * @returns 0 for success, nonzero for failure.
     */
    kv_status copy_record(uint8_t from_area, size_t from_offset, size_t to_offset, size_t &to_next_offset);

    /**
     * @brief Garbage collection (compact all records from active area to the standby one).
     *
     * @returns 0 for success, nonzero for failure.
     */
    kv_status garbage_collection();

    /**
     * @brief Return record size given key and data size.
     *
     * @param[in]  key                    Key - must not include '*' '/' '?' ':' ';' '\' '"' '|' ' ' '<' '>' '\'.
     * @param[in]  data_size              Data size.
     *
     * @returns record size.
     */
    size_t record_size(const char *key, size_t data_size);

    /**
     * @brief Find a record given key
     *
     * @param[in]  key                    Key - must not include '*' '/' '?' ':' ';' '\' '"' '|' ' ' '<' '>' '\'.
     * @param[out] offset                 Offset of record.
     * @param[out] ram_table_ind          Index in RAM table (target one if not found).
     * @param[out] hash                   Calculated key hash.
     *
     * @returns 0 for success, nonzero for failure.
     */
    kv_status find_record(const char *key, bd_size_t &offset, size_t &ram_table_ind, uint32_t &hash);

    /**
     * @brief Build RAM table and update _free_space_offset (scanning all the records in the area).
     *
     * @returns 0 for success, nonzero for failure.
     */
    kv_status build_ram_table();

    /**
     * @brief Increment maximum number of keys and reallocate RAM table accordingly.
     *
     * @param[out] ram_table             Updated RAM table.
     *
     * @returns 0 for success, nonzero for failure.
     */
    kv_status increment_max_keys(void **ram_table = 0);

    /**
     * @brief Calculate offset from start of erase unit.
     *
     * @param[in]  area                  Area.
     * @param[in]  offset                Offset in area.
     * @param[out] offset_from_start     Offset from start of erase unit.
     * @param[out] dist_to_end           Distance to end of erase unit.
     *
     * @returns offset in erase unit.
     */
    void offset_in_erase_unit(uint8_t area, size_t offset, size_t &offset_from_start, size_t &dist_to_end) const;

    /**
     * @brief Before writing a record, check whether you are crossing an erase unit.
     *        If you do, check if it's erased, and erase it if not.
     *
     * @param[in]  area                  Area.
     * @param[in]  offset                Offset in area.
     * @param[in]  size                  Write size.
     * @param[in]  force_check           Force checking.
     *
     * @returns 0 for success, nonzero for failure.
     */
    kv_status check_erase_before_write(uint8_t area, size_t offset, size_t size, bool force_check = false);

    /**
     * @brief Get data from reserved area - worker function.
     *        This verifies that reserved data on both areas have
     *        correct checksums. If given pointer is not NULL, also
     *        write the reserved data to buffer. If checksums are not
     *        valid, return error code, and don't write anything to any
     *        pointers.
     *
     * @param[out] reserved_data        Reserved data buffer (NULL to return nothing).
     * @param[in]  reserved_data_buf_size
     *                                  Reserved data buffer size.
     * @param[out] actual_data_size     If not NULL, return actual data size.
     * @param[out] copy_trailer         If not NULL, copy the trailer content to given buffer.
     *
     * @returns 0 on success or a negative error code on failure
     */
    kv_status do_reserved_data_get(void *reserved_data,
                                   size_t reserved_data_buf_size,
                                   size_t *actual_data_size = 0,
                                   void *copy_trailer = 0);

    /**
     * @brief Update all iterators after adding or deleting of keys.
     *
     * @param[in]  added                True if added, false if deleted.
     * @param[in]  ram_table_ind        RAM table index.
     *
     * @returns none
     */
    void update_all_iterators(bool added, size_t ram_table_ind) const;
};

} // namespace storage
} // namespace iotsdk

/** @}*/
#endif // IOTSDK_TDBSTORE_H
