/*
 * Copyright (c) 2018-2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "iotsdk/TDBStore.h"

#include <algorithm>
#include <new>
#include <stdio.h>
#include <string.h>

#include "iotsdk/BufferedBlockDevice.h"

#include "common/align.h"
#include "common/crc.h"
#include "common/debug.h"

using namespace iotsdk::storage;

static const uint32_t delete_flag = (1UL << 31);
static const uint32_t internal_flags = delete_flag;
// Only write once flag is supported, other two are kept in storage but ignored
static const uint32_t supported_flags = KVStore::WRITE_ONCE_FLAG;

namespace {

typedef struct {
    uint32_t magic;
    uint16_t header_size;
    uint16_t revision;
    uint32_t flags;
    uint16_t key_size;
    uint16_t reserved;
    uint32_t data_size;
    uint32_t crc;
} record_header_t;

typedef struct {
    uint32_t hash;
    bd_size_t bd_offset;
} ram_table_entry_t;

static const char *master_rec_key = "TDBS";
static const uint32_t tdbstore_magic = 0x54686683;
static const uint32_t tdbstore_revision = 1;

typedef struct {
    uint16_t version;
    uint16_t tdbstore_revision;
    uint32_t reserved;
} master_record_data_t;

typedef enum {
    TDBSTORE_AREA_STATE_NONE = 0,
    TDBSTORE_AREA_STATE_ERASED,
    TDBSTORE_AREA_STATE_INVALID,
    TDBSTORE_AREA_STATE_VALID,
} area_state_e;

typedef struct {
    uint16_t trailer_size;
    uint16_t data_size;
    uint32_t crc;
} reserved_trailer_t;

static const size_t min_work_buf_size = 64;
static const uint32_t initial_crc = 0xFFFFFFFF;
static const uint32_t initial_max_keys = 16;

// incremental set handle
typedef struct {
    record_header_t header;
    bd_size_t bd_base_offset;
    bd_size_t bd_curr_offset;
    size_t offset_in_data;
    size_t ram_table_ind;
    uint32_t hash;
    bool new_key;
} inc_set_handle_t;

// iterator handle
typedef struct {
    int iterator_num;
    size_t ram_table_ind;
    char *prefix;
} key_iterator_handle_t;

} // anonymous namespace

static uint32_t calc_crc(uint32_t init_crc, uint32_t data_size, const void *data_buf)
{
    uint32_t crc = init_crc;
    crc32(data_buf, data_size, &crc);
    return crc;
}

TDBStore::TDBStore(BlockDevice *bd)
    : _ram_table(0), _max_keys(0), _num_keys(0), _bd(bd), _buff_bd(0), _free_space_offset(0), _master_record_offset(0),
      _master_record_size(0), _is_initialized(false), _active_area(0), _active_area_version(0),
      _size(0), _area_params{}, _prog_size(0), _work_buf(0), _work_buf_size(0), _key_buf(0),
      _inc_set_handle(0), _iterator_table{}
{
}

kv_status TDBStore::read_area(uint8_t area, size_t offset, size_t size, void *buf)
{
    bd_size_t total_size = offset + size;
    // Check that we are not crossing area boundary
    COMMON_ASSERT(total_size <= _size, "read_area: attempt to read beyond area boundary");
    const bd_status os_ret = _buff_bd->read(buf, _area_params[area].address + offset, size);

    if (os_ret != bd_status::OK) {
        return kv_status::READ_FAILED;
    }

    return kv_status::OK;
}

kv_status TDBStore::write_area(uint8_t area, size_t offset, size_t size, const void *buf)
{
    const bd_status os_ret = _buff_bd->program(buf, _area_params[area].address + offset, size);
    if (os_ret != bd_status::OK) {
        return kv_status::WRITE_FAILED;
    }

    return kv_status::OK;
}

kv_status TDBStore::erase_area(uint8_t area, size_t offset, size_t size)
{
    bd_size_t bd_offset = _area_params[area].address + offset;

    bd_status ret = _buff_bd->erase(bd_offset, size);
    if (ret != bd_status::OK) {
        return kv_status::ERASE_FAILED;
    }

    if (_buff_bd->get_erase_value() == -1) {
        // We need to simulate erase to wipe records, as our block device
        // may not do it. Program in chunks of _work_buf_size if the minimum
        // program size is too small (e.g. one-byte) to avoid performance
        // issues.
        COMMON_ASSERT(_work_buf != nullptr && _work_buf_size != 0, "init() failed to allocate _work_buf");

        memset(_work_buf, 0xFF, _work_buf_size);
        while (size) {
            size_t chunk = std::min(_work_buf_size, size);
            ret = _buff_bd->program(_work_buf, bd_offset, chunk);
            if (ret != bd_status::OK) {
                return kv_status::WRITE_FAILED;
            }
            size -= chunk;
            bd_offset += chunk;
        }
    }
    return kv_status::OK;
}

void TDBStore::calc_area_params()
{
    // TDBStore can't exceed 32 bits
    bd_size_t bd_size = std::min<bd_size_t>(_bd->size(), 0x80000000L);

    memset(_area_params, 0, sizeof(_area_params));
    size_t area_0_size = 0;
    size_t area_1_size = 0;

    // The size calculations are a bit complex because we need to make sure we're
    // always aligned to an erase block boundary (whose size may not be uniform
    // across the address space), and we also need to make sure that the first
    // area never goes over half of the total size even if bd_size is an odd
    // number of sectors.
    while (true) {
        bd_size_t erase_unit_size = _bd->get_erase_size(area_0_size);
        if (area_0_size + erase_unit_size <= (bd_size / 2)) {
            area_0_size += erase_unit_size;
        } else {
            break;
        }
    }

    while (true) {
        bd_size_t erase_unit_size = _bd->get_erase_size(area_0_size + area_1_size);
        if (area_1_size + erase_unit_size <= (bd_size / 2)) {
            area_1_size += erase_unit_size;
        } else {
            break;
        }
    }

    _area_params[0].address = 0;
    _area_params[0].size = area_0_size;
    _area_params[1].address = area_0_size;
    _area_params[1].size = area_1_size;

    // The areas must be of same size
    COMMON_ASSERT(_area_params[0].size == _area_params[1].size, "_area_params[0].size != _area_params[1].size");
}

// This function, reading a record from the BD, is used for multiple purposes:
// - Init (scan all records, no need to return file name and data)
// - Get (return file data)
// - Get first/next file (check whether name matches, return name if so)
kv_status TDBStore::read_record(uint8_t area,
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
                                size_t &next_offset)
{
    kv_status ret;
    record_header_t header;
    size_t total_size, key_size, data_size;
    char *user_key_ptr;
    uint32_t crc = initial_crc;

    // next offset should only be updated to the end of record if successful
    next_offset = offset;

    ret = read_area(area, offset, sizeof(header), &header);
    if (ret != kv_status::OK) {
        return ret;
    }

    if (header.magic != tdbstore_magic) {
        return kv_status::INVALID_DATA_DETECTED;
    }

    offset += align_up(sizeof(header), _prog_size);

    key_size = header.key_size;
    data_size = header.data_size;
    flags = header.flags;

    if ((!key_size) || (key_size >= MAX_KEY_SIZE)) {
        return kv_status::INVALID_DATA_DETECTED;
    }

    total_size = key_size + data_size;

    // Make sure our read sizes didn't cause any wraparounds
    if ((total_size < key_size) || (total_size < data_size)) {
        return kv_status::INVALID_DATA_DETECTED;
    }

    if (offset + total_size >= _size) {
        return kv_status::INVALID_DATA_DETECTED;
    }

    if (data_offset > data_size) {
        return kv_status::INVALID_SIZE;
    }

    actual_data_size = std::min(data_buf_size, data_size - data_offset);

    COMMON_ASSERT(!(copy_data && actual_data_size && !data_buf),
                  "read_record invalid params - copy_data passed with NULL data_buf");

    // Upper layers typically use non-zero offsets for reading the records chunk by chunk.
    // In order to avoid a serious performance penalty, only validate where:
    //  a) we're reading the entire record from offset 0
    //  b) we're not reading any data, we're just calling read_record to validate the record
    bool validate = (data_offset == 0) && (data_buf_size >= total_size || data_buf_size == 0);

    if (validate) {
        // Calculate CRC on header (excluding CRC itself)
        crc = calc_crc(crc, sizeof(record_header_t) - sizeof(crc), &header);
    } else {
        // Non validation case: No need to read the key, nor the parts before data_offset
        // or after the actual part requested by the user.
        total_size = actual_data_size;
        offset += data_offset + key_size;
        // Mark code that key handling is finished
        key_size = 0;
    }

    user_key_ptr = key;
    hash = initial_crc;

    while (total_size) {
        uint8_t *dest_buf;
        size_t chunk_size;
        if (key_size) {
            // This means that we're on the key part
            if (copy_key) {
                dest_buf = reinterpret_cast<uint8_t *>(user_key_ptr);
                chunk_size = key_size;
                user_key_ptr[key_size] = '\0';
            } else {
                dest_buf = _work_buf;
                chunk_size = std::min<size_t>(key_size, _work_buf_size);
            }
        } else {
            // This means that we're on the data part
            if (copy_data && actual_data_size) {
                chunk_size = actual_data_size;
                dest_buf = static_cast<uint8_t *>(data_buf);
            } else {
                chunk_size = std::min<size_t>(_work_buf_size, total_size);
                dest_buf = _work_buf;
            }
        }
        ret = read_area(area, offset, chunk_size, dest_buf);
        if (ret != kv_status::OK) {
            goto end;
        }

        if (validate) {
            // calculate CRC on current read chunk
            crc = calc_crc(crc, chunk_size, dest_buf);
        }

        if (key_size) {
            // We're on key part. May need to calculate hash or check whether key is the expected one
            if (check_expected_key) {
                if (memcmp(user_key_ptr, dest_buf, chunk_size)) {
                    ret = kv_status::ITEM_NOT_FOUND;
                }
            }

            if (calc_hash) {
                hash = calc_crc(hash, chunk_size, dest_buf);
            }

            user_key_ptr += chunk_size;
            key_size -= chunk_size;
            if (!key_size) {
                offset += data_offset;
            }
        }

        total_size -= chunk_size;
        offset += chunk_size;
    }

    if (validate && (crc != header.crc)) {
        ret = kv_status::INVALID_DATA_DETECTED;
        goto end;
    }

    next_offset = align_up(offset, _prog_size);

end:
    return ret;
}

kv_status TDBStore::find_record(const char *key, bd_size_t &offset, size_t &ram_table_ind, uint32_t &hash)
{
    ram_table_entry_t *ram_table = static_cast<ram_table_entry_t *>(_ram_table);
    ram_table_entry_t *entry;
    kv_status ret = kv_status::ITEM_NOT_FOUND;
    uint32_t flags, dummy_hash;
    size_t next_offset, actual_data_size;

    hash = calc_crc(initial_crc, strlen(key), key);

    for (ram_table_ind = 0; ram_table_ind < _num_keys; ram_table_ind++) {
        entry = &ram_table[ram_table_ind];
        offset = entry->bd_offset;
        if (hash < entry->hash) {
            continue;
        }
        if (hash > entry->hash) {
            return kv_status::ITEM_NOT_FOUND;
        }
        ret = read_record(_active_area,
                          offset,
                          const_cast<char *>(key),
                          0,
                          0,
                          actual_data_size,
                          0,
                          false,
                          false,
                          true,
                          false,
                          dummy_hash,
                          flags,
                          next_offset);
        // not found return code here means that hash doesn't belong to name. Continue searching.
        if (ret != kv_status::ITEM_NOT_FOUND) {
            break;
        }
    }

    return ret;
}

size_t TDBStore::record_size(const char *key, size_t data_size)
{
    return align_up(sizeof(record_header_t), _prog_size) + align_up(strlen(key) + data_size, _prog_size);
}

kv_status TDBStore::set_start(set_handle_t *handle, const char *key, size_t final_data_size, uint32_t creation_flags)
{
    if (!_is_initialized) {
        return kv_status::NOT_INITIALIZED;
    }

    return set_start_impl(handle, key, final_data_size, creation_flags);
}

kv_status
TDBStore::set_start_impl(set_handle_t *handle, const char *key, size_t final_data_size, uint32_t creation_flags)
{
    kv_status ret;
    bd_size_t offset = 0;
    uint32_t hash = 0;
    size_t ram_table_ind = 0;
    inc_set_handle_t *ih;
    bool need_gc = false;

    if (!is_valid_key(key)) {
        return kv_status::INVALID_ARGUMENT;
    }

    if (final_data_size > UINT32_MAX) {
        return kv_status::INVALID_SIZE;
    }

    if (creation_flags & ~(supported_flags | internal_flags)) {
        return kv_status::INVALID_ARGUMENT;
    }

    if (_inc_set_in_progress) {
        return kv_status::NOT_FINALIZED;
    }

    *handle = reinterpret_cast<set_handle_t>(_inc_set_handle);
    ih = reinterpret_cast<inc_set_handle_t *>(*handle);

    if (!strcmp(key, master_rec_key)) { // master record
        ih->bd_base_offset = _master_record_offset;
        ih->new_key = false;
        ram_table_ind = 0;
        hash = 0;
        _inc_set_in_progress = true;
    } else {
        // If we have no room for the record, perform garbage collection
        size_t rec_size = record_size(key, final_data_size);
        if (_free_space_offset + rec_size > _size) {
            ret = garbage_collection();
            if (ret != kv_status::OK) {
                goto fail;
            }
            // If even after GC we have no room for the record, return error
            if (_free_space_offset + rec_size > _size) {
                ret = kv_status::MEDIA_FULL;
                goto fail;
            }
        }
        // set _inc_set_in_progress after garbage_collection(), since garbage_collection may call set_start() to write
        // the master record
        _inc_set_in_progress = true;

        ret = find_record(key, offset, ram_table_ind, hash);

        if (ret == kv_status::OK) {
            ret = read_area(_active_area, offset, sizeof(ih->header), &ih->header);
            if (ret != kv_status::OK) {
                goto fail;
            }
            if (ih->header.flags & WRITE_ONCE_FLAG) {
                ret = kv_status::WRITE_PROTECTED;
                goto fail;
            }
            ih->new_key = false;
        } else if (ret == kv_status::ITEM_NOT_FOUND) {
            if (creation_flags & delete_flag) {
                goto fail;
            }
            if (_num_keys >= _max_keys) {
                increment_max_keys();
            }
            ih->new_key = true;
        } else {
            goto fail;
        }
        ih->bd_base_offset = _free_space_offset;

        check_erase_before_write(_active_area, ih->bd_base_offset, rec_size);
    }

    // Fill handle and header fields
    // Jump to offset after header (header will be written at finalize phase)
    ih->bd_curr_offset = ih->bd_base_offset + align_up(sizeof(record_header_t), _prog_size);
    ih->offset_in_data = 0;
    ih->hash = hash;
    ih->ram_table_ind = ram_table_ind;
    ih->header.magic = tdbstore_magic;
    ih->header.header_size = sizeof(record_header_t);
    ih->header.revision = tdbstore_revision;
    ih->header.flags = creation_flags;
    ih->header.key_size = static_cast<uint16_t>(
        strlen(key)); // key is guaranteed to be <= MAX_KEY_SIZE as we've called KVStore::is_valid_key
    ih->header.reserved = 0;
    ih->header.data_size =
        static_cast<uint32_t>(final_data_size); // final_data_size is guaranteed to be <= UINT32_MAX as checked at start
    // Calculate CRC on header and key
    ih->header.crc = calc_crc(initial_crc, sizeof(record_header_t) - sizeof(ih->header.crc), &ih->header);
    ih->header.crc = calc_crc(ih->header.crc, ih->header.key_size, key);

    // Write key now
    ret = write_area(_active_area, ih->bd_curr_offset, ih->header.key_size, key);
    if (ret != kv_status::OK) {
        need_gc = true;
        goto fail;
    }
    ih->bd_curr_offset += ih->header.key_size;
    goto end;

fail:
    if ((need_gc) && (ih->bd_base_offset != _master_record_offset)) {
        garbage_collection();
    }
    // mark handle as invalid by clearing magic field in header
    ih->header.magic = 0;
    _inc_set_in_progress = false;

end:
    return ret;
}

kv_status TDBStore::set_add_data(set_handle_t handle, const void *value_data, size_t data_size)
{
    kv_status ret;
    inc_set_handle_t *ih;
    bool need_gc = false;

    if (handle != _inc_set_handle) {
        return kv_status::INVALID_ARGUMENT;
    }

    if (!value_data && data_size) {
        return kv_status::INVALID_ARGUMENT;
    }

    ih = reinterpret_cast<inc_set_handle_t *>(handle);

    if (!ih->header.magic) {
        ret = kv_status::INVALID_ARGUMENT;
        goto end;
    }

    if (ih->offset_in_data + data_size > ih->header.data_size) {
        ret = kv_status::INVALID_SIZE;
        goto end;
    }

    // Update CRC with data chunk
    ih->header.crc = calc_crc(ih->header.crc, data_size, value_data);

    // Write the data chunk
    ret = write_area(_active_area, ih->bd_curr_offset, data_size, value_data);
    if (ret != kv_status::OK) {
        need_gc = true;
        goto end;
    }
    ih->bd_curr_offset += data_size;
    ih->offset_in_data += data_size;

end:
    if ((need_gc) && (ih->bd_base_offset != _master_record_offset)) {
        garbage_collection();
    }

    return ret;
}

kv_status TDBStore::set_finalize(set_handle_t handle)
{
    bd_status os_ret;
    kv_status ret;
    inc_set_handle_t *ih;
    ram_table_entry_t *ram_table = static_cast<ram_table_entry_t *>(_ram_table);
    ram_table_entry_t *entry;
    bool need_gc = false;
    uint32_t hash, flags;
    size_t actual_data_size, next_offset;

    if (handle != _inc_set_handle) {
        return kv_status::INVALID_ARGUMENT;
    }

    ih = reinterpret_cast<inc_set_handle_t *>(handle);

    if (!ih->header.magic) {
        return kv_status::INVALID_ARGUMENT;
    }

    if (ih->offset_in_data != ih->header.data_size) {
        ret = kv_status::INVALID_SIZE;
        need_gc = true;
        goto end;
    }

    // Write header
    ret = write_area(_active_area, ih->bd_base_offset, sizeof(record_header_t), &ih->header);
    if (ret != kv_status::OK) {
        need_gc = true;
        goto end;
    }

    // Need to flush buffered BD as our record is totally written now
    os_ret = _buff_bd->sync();
    if (os_ret != bd_status::OK) {
        ret = kv_status::WRITE_FAILED;
        need_gc = true;
        goto end;
    }

    // In master record case we don't update RAM table
    if (ih->bd_base_offset == _master_record_offset) {
        goto end;
    }

    // Writes may fail without returning a failure (especially in flash components). Reread the record
    // to ensure write success (this won't read the data anywhere - just use the CRC calculation).
    ret = read_record(_active_area,
                      ih->bd_base_offset,
                      0,
                      0,
                      static_cast<size_t>(-1),
                      actual_data_size,
                      0,
                      false,
                      false,
                      false,
                      false,
                      hash,
                      flags,
                      next_offset);
    if (ret != kv_status::OK) {
        need_gc = true;
        goto end;
    }

    // Update RAM table
    if (ih->header.flags & delete_flag) {
        _num_keys--;
        if (ih->ram_table_ind < _num_keys) {
            memmove(&ram_table[ih->ram_table_ind],
                    &ram_table[ih->ram_table_ind + 1],
                    sizeof(ram_table_entry_t) * (_num_keys - ih->ram_table_ind));
        }
        update_all_iterators(false, ih->ram_table_ind);
    } else {
        if (ih->new_key) {
            if (ih->ram_table_ind < _num_keys) {
                memmove(&ram_table[ih->ram_table_ind + 1],
                        &ram_table[ih->ram_table_ind],
                        sizeof(ram_table_entry_t) * (_num_keys - ih->ram_table_ind));
            }
            _num_keys++;
            update_all_iterators(true, ih->ram_table_ind);
        }
        entry = &ram_table[ih->ram_table_ind];
        entry->hash = ih->hash;
        entry->bd_offset = ih->bd_base_offset;
    }

    _free_space_offset = align_up(ih->bd_curr_offset, _prog_size);

    // Safety check: If there seems to be valid keys on the free space
    // we should erase one sector more, just to ensure that in case of power failure
    // next init() would not extend the scan phase to that section as well.
    if (read_record(_active_area,
                    _free_space_offset,
                    0,
                    0,
                    0,
                    actual_data_size,
                    0,
                    false,
                    false,
                    false,
                    false,
                    hash,
                    flags,
                    next_offset)
        == kv_status::OK) {
        check_erase_before_write(_active_area, _free_space_offset, sizeof(record_header_t));
    }

end:
    // mark handle as invalid by clearing magic field in header
    ih->header.magic = 0;

    if (ih->bd_base_offset != _master_record_offset) {
        if (need_gc) {
            garbage_collection();
        }
    }
    _inc_set_in_progress = false;
    return ret;
}

kv_status TDBStore::set(const char *key, const void *buffer, size_t size, uint32_t creation_flags)
{
    if (!_is_initialized) {
        return kv_status::NOT_INITIALIZED;
    }

    return set_impl(key, buffer, size, creation_flags);
}

kv_status TDBStore::set_impl(const char *key, const void *buffer, size_t size, uint32_t creation_flags)
{
    kv_status ret;
    set_handle_t handle;

    // Don't wait till we get to set_add_data to catch this
    if (!buffer && size) {
        return kv_status::INVALID_ARGUMENT;
    }

    ret = set_start_impl(&handle, key, size, creation_flags);
    if (ret != kv_status::OK) {
        return ret;
    }

    ret = set_add_data(handle, buffer, size);
    if (ret != kv_status::OK) {
        return ret;
    }

    ret = set_finalize(handle);
    return ret;
}

kv_status TDBStore::remove(const char *key)
{
    return set(key, 0, 0, delete_flag);
}

kv_status TDBStore::get(const char *key, void *buffer, size_t buffer_size, size_t *actual_size, size_t offset)
{
    kv_status ret;
    size_t actual_data_size, next_bd_offset, ram_table_ind;
    bd_size_t bd_offset;
    uint32_t flags, hash;

    if (!_is_initialized) {
        return kv_status::NOT_INITIALIZED;
    }

    if (!is_valid_key(key)) {
        return kv_status::INVALID_ARGUMENT;
    }

    ret = find_record(key, bd_offset, ram_table_ind, hash);

    if (ret != kv_status::OK) {
        goto end;
    }

    ret = read_record(_active_area,
                      bd_offset,
                      const_cast<char *>(key),
                      buffer,
                      buffer_size,
                      actual_data_size,
                      offset,
                      false,
                      true,
                      false,
                      false,
                      hash,
                      flags,
                      next_bd_offset);

    if (actual_size) {
        *actual_size = actual_data_size;
    }

end:
    return ret;
}

kv_status TDBStore::get_info(const char *key, info_t *elem_info)
{
    kv_status ret;
    size_t actual_data_size, next_bd_offset, ram_table_ind;
    bd_size_t bd_offset;
    uint32_t flags, hash;

    if (!_is_initialized) {
        return kv_status::NOT_INITIALIZED;
    }

    if (!is_valid_key(key)) {
        return kv_status::INVALID_ARGUMENT;
    }

    ret = find_record(key, bd_offset, ram_table_ind, hash);

    if (ret != kv_status::OK) {
        goto end;
    }

    // Give a large dummy buffer size in order to achieve actual data size
    // (as copy_data flag is not set, data won't be copied anywhere)
    ret = read_record(_active_area,
                      bd_offset,
                      const_cast<char *>(key),
                      0,
                      static_cast<size_t>(-1),
                      actual_data_size,
                      0,
                      false,
                      false,
                      false,
                      false,
                      hash,
                      flags,
                      next_bd_offset);

    if (ret != kv_status::OK) {
        goto end;
    }

    if (elem_info) {
        elem_info->flags = flags;
        elem_info->size = actual_data_size;
    }

end:
    return ret;
}

kv_status TDBStore::write_master_record(uint16_t version, size_t &next_offset)
{
    master_record_data_t master_rec;

    master_rec.version = version;
    master_rec.tdbstore_revision = tdbstore_revision;
    master_rec.reserved = 0;
    next_offset = _master_record_offset + _master_record_size;
    return set_impl(master_rec_key, &master_rec, sizeof(master_rec), 0);
}

static inline uint8_t get_other_area(uint8_t area)
{
    return static_cast<uint8_t>(1 - area);
}

kv_status TDBStore::copy_record(uint8_t from_area, size_t from_offset, size_t to_offset, size_t &to_next_offset)
{
    kv_status ret;
    record_header_t header;
    size_t total_size;
    size_t chunk_size;

    ret = read_area(from_area, from_offset, sizeof(header), &header);
    if (ret != kv_status::OK) {
        return ret;
    }

    total_size =
        align_up(sizeof(record_header_t), _prog_size) + align_up(header.key_size + header.data_size, _prog_size);
    ;

    if (to_offset + total_size > _size) {
        // We are trying to copy more that the are can hold
        return kv_status::MEDIA_FULL;
    }
    ret = check_erase_before_write(get_other_area(from_area), to_offset, total_size);
    if (ret != kv_status::OK) {
        return ret;
    }

    chunk_size = align_up(sizeof(record_header_t), _prog_size);
    // The record header takes up whole program units
    memset(_work_buf, 0, chunk_size);
    memcpy(_work_buf, &header, sizeof(record_header_t));
    ret = write_area(get_other_area(from_area), to_offset, chunk_size, _work_buf);
    if (ret != kv_status::OK) {
        return ret;
    }

    from_offset += chunk_size;
    to_offset += chunk_size;
    total_size -= chunk_size;

    while (total_size) {
        chunk_size = std::min(total_size, _work_buf_size);
        ret = read_area(from_area, from_offset, chunk_size, _work_buf);
        if (ret != kv_status::OK) {
            return ret;
        }

        ret = write_area(get_other_area(from_area), to_offset, chunk_size, _work_buf);
        if (ret != kv_status::OK) {
            return ret;
        }

        from_offset += chunk_size;
        to_offset += chunk_size;
        total_size -= chunk_size;
    }

    to_next_offset = align_up(to_offset, _prog_size);
    return kv_status::OK;
}

kv_status TDBStore::garbage_collection()
{
    ram_table_entry_t *ram_table = static_cast<ram_table_entry_t *>(_ram_table);
    size_t to_offset, to_next_offset;
    kv_status ret;
    size_t ind;

    // Reset the standby area
    ret = reset_area(get_other_area(_active_area));
    if (ret != kv_status::OK) {
        return ret;
    }

    to_offset = _master_record_offset + _master_record_size;

    // Initialize in case table is empty
    to_next_offset = to_offset;

    // Go over ram table and copy all entries to opposite area
    for (ind = 0; ind < _num_keys; ind++) {
        bd_size_t from_offset = ram_table[ind].bd_offset;
        ret = copy_record(_active_area, from_offset, to_offset, to_next_offset);
        if (ret != kv_status::OK) {
            return ret;
        }
        // Update RAM table
        ram_table[ind].bd_offset = to_offset;
        to_offset = to_next_offset;
    }

    to_offset = to_next_offset;
    _free_space_offset = to_next_offset;

    // Now we can switch to the new active area
    _active_area = get_other_area(_active_area);

    // Now write master record, with version incremented by 1.
    _active_area_version++;
    ret = write_master_record(_active_area_version, to_offset);
    if (ret != kv_status::OK) {
        return ret;
    }

    return kv_status::OK;
}

kv_status TDBStore::build_ram_table()
{
    ram_table_entry_t *ram_table = static_cast<ram_table_entry_t *>(_ram_table);
    size_t offset, actual_data_size, next_offset = 0, ram_table_ind;
    bd_size_t dummy;
    kv_status ret = kv_status::OK;
    uint32_t hash, flags;

    _num_keys = 0;
    offset = _master_record_offset;

    while (offset + sizeof(record_header_t) < _free_space_offset) {
        ret = read_record(_active_area,
                          offset,
                          _key_buf,
                          0,
                          0,
                          actual_data_size,
                          0,
                          true,
                          false,
                          false,
                          true,
                          hash,
                          flags,
                          next_offset);

        if (ret != kv_status::OK) {
            goto end;
        }

        ret = find_record(_key_buf, dummy, ram_table_ind, hash);

        if ((ret != kv_status::OK) && (ret != kv_status::ITEM_NOT_FOUND)) {
            goto end;
        }

        size_t save_offset = offset;
        offset = next_offset;

        if (ret == kv_status::ITEM_NOT_FOUND) {
            // Key doesn't exist, need to add it to RAM table
            ret = kv_status::OK;

            if (flags & delete_flag) {
                continue;
            }
            if (_num_keys >= _max_keys) {
                // In order to avoid numerous reallocations of ram table,
                // Add a chunk of entries now
                increment_max_keys(reinterpret_cast<void **>(&ram_table));
            }
            memmove(&ram_table[ram_table_ind + 1],
                    &ram_table[ram_table_ind],
                    sizeof(ram_table_entry_t) * (_num_keys - ram_table_ind));

            _num_keys++;
        } else if (flags & delete_flag) {
            _num_keys--;
            memmove(&ram_table[ram_table_ind],
                    &ram_table[ram_table_ind + 1],
                    sizeof(ram_table_entry_t) * (_num_keys - ram_table_ind));

            continue;
        }

        // update record parameters
        ram_table[ram_table_ind].hash = hash;
        ram_table[ram_table_ind].bd_offset = save_offset;
    }

end:
    _free_space_offset = next_offset;
    return ret;
}

kv_status TDBStore::increment_max_keys(void **ram_table)
{
    // Reallocate ram table with new size
    ram_table_entry_t *old_ram_table = static_cast<ram_table_entry_t *>(_ram_table);
    ram_table_entry_t *new_ram_table = new (std::nothrow) ram_table_entry_t[_max_keys + 1];
    if (new_ram_table == nullptr) {
        return kv_status::MEMORY_ALLOC_FAILED;
    }
    memset(new_ram_table, 0, sizeof(ram_table_entry_t) * (_max_keys + 1));

    // Copy old content to new table
    memcpy(new_ram_table, old_ram_table, sizeof(ram_table_entry_t) * _max_keys);
    _max_keys++;

    _ram_table = new_ram_table;
    delete[] old_ram_table;

    if (ram_table) {
        *ram_table = _ram_table;
    }
    return kv_status::OK;
}

kv_status TDBStore::init()
{
    ram_table_entry_t *ram_table;
    area_state_e area_state[_num_areas];
    size_t next_offset;
    uint32_t flags, hash;
    size_t actual_data_size;
    bd_status os_ret;
    kv_status ret = kv_status::OK;
    uint16_t versions[_num_areas];

    if (_is_initialized) {
        goto end;
    }

    _max_keys = initial_max_keys;

    ram_table = new (std::nothrow) ram_table_entry_t[_max_keys];
    if (ram_table == nullptr) {
        ret = kv_status::MEMORY_ALLOC_FAILED;
        goto fail;
    }
    memset(ram_table, 0, sizeof(ram_table_entry_t) * _max_keys);
    _ram_table = ram_table;
    _num_keys = 0;

    _size = static_cast<size_t>(-1);

    _buff_bd = new (std::nothrow) BufferedBlockDevice(_bd);
    if (_buff_bd == nullptr) {
        ret = kv_status::MEMORY_ALLOC_FAILED;
        goto fail;
    }
    os_ret = _buff_bd->init();
    if (os_ret != bd_status::OK) {
        ret = kv_status::INIT_FAIL;
        goto fail;
    }

    _prog_size = _bd->get_program_size();
    _work_buf_size = std::max<size_t>(_prog_size, min_work_buf_size);
    _work_buf = new (std::nothrow) uint8_t[_work_buf_size];
    if (_work_buf == nullptr) {
        ret = kv_status::MEMORY_ALLOC_FAILED;
        goto fail;
    }
    _key_buf = new (std::nothrow) char[MAX_KEY_SIZE];
    if (_key_buf == nullptr) {
        ret = kv_status::MEMORY_ALLOC_FAILED;
        goto fail;
    }
    _inc_set_handle = new (std::nothrow) inc_set_handle_t;
    if (_inc_set_handle == nullptr) {
        ret = kv_status::MEMORY_ALLOC_FAILED;
        goto fail;
    }
    memset(_inc_set_handle, 0, sizeof(inc_set_handle_t));
    memset(_iterator_table, 0, sizeof(_iterator_table));

    _master_record_offset = align_up(RESERVED_AREA_SIZE + sizeof(reserved_trailer_t), _prog_size);
    _master_record_size = record_size(master_rec_key, sizeof(master_record_data_t));

    calc_area_params();

    /* Minimum space required by Reserved area and master record */
    if (_bd->size() < (align_up(RESERVED_AREA_SIZE + sizeof(reserved_trailer_t), _prog_size)
                       + record_size(master_rec_key, sizeof(master_record_data_t)))) {
        ret = kv_status::MEDIA_FULL;
        goto fail;
    }

    for (uint8_t area = 0; area < _num_areas; area++) {
        area_state[area] = TDBSTORE_AREA_STATE_NONE;
        versions[area] = 0;

        _size = std::min(_size, _area_params[area].size);

        // Check validity of master record
        master_record_data_t master_rec;
        ret = read_record(area,
                          _master_record_offset,
                          const_cast<char *>(master_rec_key),
                          &master_rec,
                          sizeof(master_rec),
                          actual_data_size,
                          0,
                          false,
                          true,
                          true,
                          false,
                          hash,
                          flags,
                          next_offset);
        if ((ret != kv_status::OK) && (ret != kv_status::INVALID_DATA_DETECTED)) {
            goto fail;
        }

        // Master record may be either corrupt or erased
        if (ret == kv_status::INVALID_DATA_DETECTED) {
            area_state[area] = TDBSTORE_AREA_STATE_INVALID;
            continue;
        }

        versions[area] = master_rec.version;

        area_state[area] = TDBSTORE_AREA_STATE_VALID;

        // Unless both areas are valid (a case handled later), getting here means
        // that we found our active area.
        _active_area = area;
        _active_area_version = versions[area];
    }

    // In case we have two empty areas, arbitrarily use area 0 as the active one.
    if ((area_state[0] == TDBSTORE_AREA_STATE_INVALID) && (area_state[1] == TDBSTORE_AREA_STATE_INVALID)) {
        reset_area(0);
        _active_area = 0;
        _active_area_version = 1;
        area_state[0] = TDBSTORE_AREA_STATE_ERASED;
        ret = write_master_record(_active_area_version, _free_space_offset);
        if (ret != kv_status::OK) {
            goto fail;
        }
        // Nothing more to do here if active area is empty
        goto end;
    }

    // In case we have two valid areas, choose the one having the higher version (or 0
    // in case of wrap around).
    if ((area_state[0] == TDBSTORE_AREA_STATE_VALID) && (area_state[1] == TDBSTORE_AREA_STATE_VALID)) {
        if ((versions[0] > versions[1]) || (!versions[0])) {
            _active_area = 0;
        } else {
            _active_area = 1;
        }
        _active_area_version = versions[_active_area];
    }

    // Currently set free space offset pointer to the end of free space.
    // Ram table build process needs it, but will update it.
    _free_space_offset = _size;
    ret = build_ram_table();

    // build_ram_table() scans all keys, until invalid data found.
    // Therefore INVALID_DATA is not considered error.
    if ((ret != kv_status::OK) && (ret != kv_status::INVALID_DATA_DETECTED)) {
        goto fail;
    }

end:
    _is_initialized = true;
    return kv_status::OK;
fail:
    delete[] ram_table;
    delete _buff_bd;
    delete[] _work_buf;
    delete[] _key_buf;
    delete reinterpret_cast<inc_set_handle_t *>(_inc_set_handle);
    _ram_table = nullptr;
    _buff_bd = nullptr;
    _work_buf = nullptr;
    _key_buf = nullptr;
    _inc_set_handle = nullptr;
    return ret;
}

kv_status TDBStore::deinit()
{
    if (!_is_initialized) {
        return kv_status::NOT_INITIALIZED;
    } else {
        const bd_status ret = _buff_bd->deinit();
        if (ret != bd_status::OK) {
            return kv_status::SYNC_FAILED;
        }
        delete _buff_bd;

        ram_table_entry_t *ram_table = static_cast<ram_table_entry_t *>(_ram_table);
        delete[] ram_table;
        delete[] _work_buf;
        delete[] _key_buf;
        delete reinterpret_cast<inc_set_handle_t *>(_inc_set_handle);
    }

    _is_initialized = false;

    return kv_status::OK;
}

kv_status TDBStore::reset_area(uint8_t area)
{
    uint8_t buf[RESERVED_AREA_SIZE + sizeof(reserved_trailer_t)];
    kv_status ret;
    bool copy_reserved_data = do_reserved_data_get(buf, sizeof(buf), 0, buf + RESERVED_AREA_SIZE) == kv_status::OK;

    // Erase reserved area and master record
    ret = check_erase_before_write(area, 0, _master_record_offset + _master_record_size + _prog_size, true);
    if (ret != kv_status::OK) {
        return ret;
    }
    if (copy_reserved_data) {
        ret = write_area(area, 0, sizeof(buf), buf);
    }
    return ret;
}

kv_status TDBStore::reset()
{
    uint8_t area;
    kv_status ret;

    if (!_is_initialized) {
        return kv_status::NOT_INITIALIZED;
    }

    // Reset both areas
    for (area = 0; area < _num_areas; area++) {
        ret = check_erase_before_write(area, 0, _master_record_offset + _master_record_size + _prog_size, true);
        if (ret != kv_status::OK) {
            goto end;
        }
    }

    _active_area = 0;
    _num_keys = 0;
    _free_space_offset = _master_record_offset;
    _active_area_version = 1;
    memset(_ram_table, 0, sizeof(ram_table_entry_t) * _max_keys);
    // Write an initial master record on active area
    ret = write_master_record(_active_area_version, _free_space_offset);

end:
    return ret;
}

kv_status TDBStore::iterator_open(iterator_t *it, const char *prefix)
{
    key_iterator_handle_t *handle;
    kv_status ret = kv_status::OK;

    if (!_is_initialized) {
        return kv_status::NOT_INITIALIZED;
    }

    if (!it) {
        return kv_status::INVALID_ARGUMENT;
    }

    int it_num;
    for (it_num = 0; it_num < _max_open_iterators; it_num++) {
        if (!_iterator_table[it_num]) {
            break;
        }
    }

    if (it_num == _max_open_iterators) {
        return kv_status::OUT_OF_RESOURCES;
    }

    handle = new (std::nothrow) key_iterator_handle_t;
    if (handle == nullptr) {
        return kv_status::MEMORY_ALLOC_FAILED;
    }
    *it = reinterpret_cast<iterator_t>(handle);

    if (prefix && strcmp(prefix, "")) {
        handle->prefix = new (std::nothrow) char[strlen(prefix) + 1];
        if (handle->prefix == nullptr) {
            return kv_status::MEMORY_ALLOC_FAILED;
        }
        memcpy(handle->prefix, prefix, strlen(prefix) + 1);
    } else {
        handle->prefix = 0;
    }
    handle->ram_table_ind = 0;
    handle->iterator_num = it_num;
    _iterator_table[it_num] = handle;

    return ret;
}

kv_status TDBStore::iterator_next(iterator_t it, char *key, size_t key_size)
{
    ram_table_entry_t *ram_table = static_cast<ram_table_entry_t *>(_ram_table);
    key_iterator_handle_t *handle;
    kv_status ret;
    size_t actual_data_size, next_offset;
    uint32_t hash, flags;

    if (!_is_initialized) {
        return kv_status::NOT_INITIALIZED;
    }

    handle = reinterpret_cast<key_iterator_handle_t *>(it);

    ret = kv_status::ITEM_NOT_FOUND;

    while ((ret != kv_status::OK) && (handle->ram_table_ind < _num_keys)) {
        ret = read_record(_active_area,
                          ram_table[handle->ram_table_ind].bd_offset,
                          _key_buf,
                          0,
                          0,
                          actual_data_size,
                          0,
                          true,
                          false,
                          false,
                          false,
                          hash,
                          flags,
                          next_offset);
        if (ret != kv_status::OK) {
            goto end;
        }
        if (!handle->prefix || (strstr(_key_buf, handle->prefix) == _key_buf)) {
            if (strlen(_key_buf) >= key_size) {
                ret = kv_status::INVALID_SIZE;
                goto end;
            }
            snprintf(key, key_size, "%s", _key_buf);
        } else {
            ret = kv_status::ITEM_NOT_FOUND;
        }
        handle->ram_table_ind++;
    }

end:
    return ret;
}

kv_status TDBStore::iterator_close(iterator_t it)
{
    key_iterator_handle_t *handle;

    if (!_is_initialized) {
        return kv_status::NOT_INITIALIZED;
    }

    handle = reinterpret_cast<key_iterator_handle_t *>(it);
    delete[] handle->prefix;
    _iterator_table[handle->iterator_num] = 0;
    delete handle;

    return kv_status::OK;
}

void TDBStore::update_all_iterators(bool added, size_t ram_table_ind) const
{
    for (int it_num = 0; it_num < _max_open_iterators; it_num++) {
        key_iterator_handle_t *handle = static_cast<key_iterator_handle_t *>(_iterator_table[it_num]);
        if (!handle) {
            continue;
        }

        if (ram_table_ind >= handle->ram_table_ind) {
            continue;
        }

        if (added) {
            handle->ram_table_ind++;
        } else {
            handle->ram_table_ind--;
        }
    }
}

kv_status TDBStore::reserved_data_set(const void *reserved_data, size_t reserved_data_buf_size)
{
    reserved_trailer_t trailer;
    bd_status os_ret;
    kv_status ret;

    if (!_is_initialized) {
        return kv_status::NOT_INITIALIZED;
    }

    if (reserved_data_buf_size > RESERVED_AREA_SIZE || reserved_data_buf_size > UINT16_MAX) {
        return kv_status::INVALID_SIZE;
    }

    ret = do_reserved_data_get(0, 0);
    if (ret == kv_status::OK) {
        ret = kv_status::WRITE_FAILED;
        goto end;
    }

    trailer.trailer_size = sizeof(trailer);
    // we check size of reserved_data_buf_size above so ok to truncate here with narrowing cast: size_t -> uint16_t
    trailer.data_size = static_cast<uint16_t>(reserved_data_buf_size);
    trailer.crc = calc_crc(initial_crc, reserved_data_buf_size, reserved_data);

    // Erase the header of non-active area, just to make sure that we can write to it
    // In case garbage collection has not yet been run, the area can be un-erased
    ret = reset_area(get_other_area(_active_area));
    if (ret != kv_status::OK) {
        goto end;
    }

    /*
     * Write to both areas
     * Both must kv_status::OK, as they are required to be erased when TDBStore initializes
     * its area
     */
    for (uint8_t i = 0; i < _num_areas; ++i) {
        ret = write_area(i, 0, reserved_data_buf_size, reserved_data);
        if (ret != kv_status::OK) {
            goto end;
        }
        ret = write_area(i, RESERVED_AREA_SIZE, sizeof(trailer), &trailer);
        if (ret != kv_status::OK) {
            goto end;
        }
        os_ret = _buff_bd->sync();
        if (os_ret != bd_status::OK) {
            ret = kv_status::WRITE_FAILED;
            goto end;
        }
    }
    ret = kv_status::OK;
end:
    return ret;
}

kv_status TDBStore::do_reserved_data_get(void *reserved_data,
                                         size_t reserved_data_buf_size,
                                         size_t *actual_data_size,
                                         void *copy_trailer)
{
    reserved_trailer_t trailer;
    uint8_t buf[RESERVED_AREA_SIZE];
    kv_status ret;
    uint32_t crc;

    /*
     * Try to keep reserved data identical on both areas, therefore
     * we can return any of these data, if the checmsum is correct.
     */
    for (uint8_t i = 0; i < _num_areas; ++i) {
        ret = read_area(i, RESERVED_AREA_SIZE, sizeof(trailer), &trailer);
        if (ret != kv_status::OK) {
            return ret;
        }

        // First validy check: is the trailer header size correct
        if (trailer.trailer_size != sizeof(trailer)) {
            continue;
        }
        // Second validy check: Is the data too big (corrupt header)
        if (trailer.data_size > RESERVED_AREA_SIZE) {
            continue;
        }

        // Next, verify the checksum
        ret = read_area(i, 0, trailer.data_size, buf);
        if (ret != kv_status::OK) {
            return ret;
        }
        crc = calc_crc(initial_crc, trailer.data_size, buf);
        if (crc == trailer.crc) {
            // Correct data, copy it and return to caller
            if (reserved_data) {
                if (reserved_data_buf_size < trailer.data_size) {
                    return kv_status::INVALID_SIZE;
                }
                memcpy(reserved_data, buf, trailer.data_size);
            }
            if (actual_data_size) {
                *actual_data_size = trailer.data_size;
            }
            if (copy_trailer) {
                memcpy(copy_trailer, &trailer, sizeof(trailer));
            }
            return kv_status::OK;
        }
    }

    return kv_status::ITEM_NOT_FOUND;
}

kv_status TDBStore::reserved_data_get(void *reserved_data, size_t reserved_data_buf_size, size_t *actual_data_size)
{
    if (!_is_initialized) {
        return kv_status::NOT_INITIALIZED;
    }

    kv_status ret = do_reserved_data_get(reserved_data, reserved_data_buf_size, actual_data_size);
    return ret;
}

void TDBStore::offset_in_erase_unit(uint8_t area, size_t offset, size_t &offset_from_start, size_t &dist_to_end) const
{
    bd_size_t bd_offset = _area_params[area].address + offset;

    // The parameter of `BlockDevice::get_erase_size(bd_addr_t addr)`
    // does not need to be aligned.
    bd_size_t erase_unit = _buff_bd->get_erase_size(bd_offset);

    // Even on a flash device with multiple regions, the start address of
    // an erase unit is aligned to the current region's unit size.
    offset_from_start = bd_offset % erase_unit;
    dist_to_end = erase_unit - offset_from_start;
}

kv_status TDBStore::check_erase_before_write(uint8_t area, size_t offset, size_t size, bool force_check)
{
    // In order to save init time, we don't check that the entire area is erased.
    // Instead, whenever reaching an erase unit start erase it.
    bool erase = false;
    size_t start_offset;
    bd_size_t end_offset;
    while (size) {
        size_t offset_from_start;
        size_t dist;
        offset_in_erase_unit(area, offset, offset_from_start, dist);
        size_t chunk = std::min(size, dist);

        if (offset_from_start == 0 || force_check) {
            if (!erase) {
                erase = true;
                start_offset = offset - offset_from_start;
            }
            end_offset = offset + dist;
        }
        offset += chunk;
        size -= chunk;
    }

    if (erase) {
        kv_status ret = erase_area(area, start_offset, end_offset - start_offset);
        if (ret != kv_status::OK) {
            return kv_status::WRITE_FAILED;
        }
    }

    return kv_status::OK;
}
