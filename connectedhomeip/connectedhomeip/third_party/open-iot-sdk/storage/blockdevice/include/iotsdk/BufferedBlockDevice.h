/*
 * Copyright (c) 2018-2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

/** \addtogroup storage */
/** @{*/

#ifndef IOTSDK_BUFFEREDBLOCKDEVICE_H
#define IOTSDK_BUFFEREDBLOCKDEVICE_H

#include <memory>
#include <stdint.h>

#include "iotsdk/BlockDevice.h"

namespace iotsdk {
namespace storage {

/** Block device for allowing minimal read and program sizes (of 1) for the underlying BD,
 *  using a buffer on the heap.
 */
class BufferedBlockDevice final : public BlockDevice {
public:
    /** Lifetime of a memory-buffered block device wrapping an underlying block device
     *
     *  @param bd        Block device to back the BufferedBlockDevice
     */
    explicit BufferedBlockDevice(BlockDevice *bd);

    /** Initialize a buffered-memory block device and its underlying block device
     *
     *  @return         0 on success or a negative error code on failure
     */
    bd_status init() override;

    /** Deinitialize the buffered-memory block device and its underlying block device
     *
     *  @return         0 on success or a negative error code on failure
     */
    bd_status deinit() override;

    /** Ensure that data on the underlying storage block device is in sync with the
     *  memory-buffered block device
     *
     *  @return         0 on success or a negative error code on failure
     */
    bd_status sync();

    /** Read blocks from the memory-buffered block device
     *
     *  @param buffer   Buffer to read blocks into
     *  @param addr     Address of block to begin reading from
     *  @param size     Size to read in bytes, must be a multiple of read block size
     *  @return         0 on success, negative error code on failure
     */
    bd_status read(void *buffer, bd_addr_t addr, bd_size_t size) override;

    /** Program data to the memory-buffered block device
     *
     *  The write address blocks must be erased prior to being programmed.
     *
     *  @param buffer   Buffer of data to write to blocks
     *  @param addr     Address of block to begin writing to
     *  @param size     Size to write in bytes, must be a multiple of program block size
     *  @return         0 on success, negative error code on failure
     */
    bd_status program(const void *buffer, bd_addr_t addr, bd_size_t size) override;

    /** Erase blocks from the memory-buffered block device
     *
     *  The state of an erased block is undefined until it has been programmed,
     *  unless get_erase_value returns a non-negative byte value.
     *
     *  @param addr     Address of block to begin erasing
     *  @param size     Size to erase in bytes, must be a multiple of erase block size
     *  @return         0 on success, negative error code on failure
     */
    bd_status erase(bd_addr_t addr, bd_size_t size) override;

    /** Get the size of a readable block
     *
     *  @return         Size of a readable block in bytes
     */
    bd_size_t get_read_size() const override;

    /** Get the size of a programmable block
     *
     *  @return         Size of a programmable block in bytes
     *  @note Must be a multiple of the read size
     */
    bd_size_t get_program_size() const override;

    /** Get the size of an erasable block
     *
     *  @return         Size of an erasable block in bytes
     *  @note Must be a multiple of the program size
     */
    bd_size_t get_erase_size() const override;

    /** Get the size of an erasable block of a given address
     *
     *  @param addr     Address within the erasable block
     *  @return         Size of an erasable block in bytes
     *  @note Must be a multiple of the program size
     */
    bd_size_t get_erase_size(bd_addr_t addr) const override;

    /** Get the value of storage data after being erased
     *
     *  If get_erase_value returns a non-negative byte value, the underlying
     *  storage is set to that value when erased, and storage containing
     *  that value can be programmed without another erase.
     *
     *  @return         The value of storage when erased, or negative error code
     *                  if you can't rely on the value of erased storage
     */
    int get_erase_value() const override;

    /** Get the total size of the underlying device
     *
     *  @return         Size of the underlying device in bytes
     */
    bd_size_t size() const override;

    /** Get the underlying BlockDevice class type
     *
     *  @return         A string representing the underlying BlockDevice class type
     */
    const char *get_type() const override;

protected:
    BlockDevice *_bd;
    bd_size_t _bd_program_size;
    bd_size_t _bd_read_size;
    bd_size_t _bd_size;
    bd_size_t _write_cache_addr;
    bool _write_cache_valid;
    std::unique_ptr<uint8_t[]> _write_cache;
    std::unique_ptr<uint8_t[]> _read_buf;
    uint32_t _init_ref_count;
    bool _is_initialized;

    /** Invalidate write cache
     *
     *  @return         none
     */
    void invalidate_write_cache();
};
} // namespace storage
} // namespace iotsdk

#endif // IOTSDK_BUFFEREDBLOCKDEVICE_H

/** @}*/
