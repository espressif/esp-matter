/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

/** \addtogroup storage */
/** @{*/

#ifndef IOTSDK_BLOCKDEVICE_H
#define IOTSDK_BLOCKDEVICE_H

#include <stdint.h>

namespace iotsdk {
namespace storage {

/** Enum of status codes
 *
 *  @enum bd_status
 */
enum class bd_status {
    OK = 0,                         /*!< no error */
    DEVICE_ERROR = -4001,           /*!< device specific error */
    DEVICE_NOT_INITIALIZED = -4002, /*!< device not initialized */
    INCORRECT_SIZE = -4003,         /*!< size was incorrect */
    INCORRECT_ADDRESS = -4004,      /*!< address was incorrect */
    ADDRESS_NOT_ALIGNED = -4005,    /*!< address was not aligned with sector/page size */
    SIZE_NOT_ALIGNED = -4006        /*!< size was not aligned with sector/page size */
};

/** Type representing the address of a specific block
 */
typedef uint64_t bd_addr_t;

/** Type representing a quantity of 8-bit bytes
 */
typedef uint64_t bd_size_t;

/** A hardware device capable of writing and reading blocks
 */
class BlockDevice {
public:
    /** Lifetime of a block device
     */
    virtual ~BlockDevice(){};

    /** Initialize a block device
     *
     *  This method must be called before attempting any further block device operations.
     *
     *  @return         bd_status::OK on success or an error status on failure
     */
    virtual bd_status init() = 0;

    /** Deinitialize a block device
     *
     *  @return         bd_status::OK on success or an error status on failure
     */
    virtual bd_status deinit() = 0;

    /** Read blocks from a block device
     *
     *  If a failure occurs, it is not possible to determine how many bytes succeeded.
     *
     *  @param buffer   Buffer to write blocks to
     *  @param addr     Address of block to begin reading from
     *  @param size     Size to read in bytes, must be a multiple of the read block size
     *  @return         bd_status::OK on success or an error status on failure
     */
    virtual bd_status read(void *buffer, bd_addr_t addr, bd_size_t size) = 0;

    /** Program blocks to a block device
     *
     *  The blocks must have been erased prior to being programmed.
     *
     *  If a failure occurs, it is not possible to determine how many bytes succeeded.
     *
     *  @param buffer   Buffer of data to write to blocks
     *  @param addr     Address of block to begin writing to
     *  @param size     Size to write in bytes, must be a multiple of the program block size
     *  @return         bd_status::OK on success or an error status on failure
     */
    virtual bd_status program(const void *buffer, bd_addr_t addr, bd_size_t size) = 0;

    /** Erase blocks on a block device
     *
     *  The state of an erased block is undefined until it has been programmed,
     *  unless get_erase_value returns a non-negative byte value.
     *
     *  @param addr     Address of block to begin erasing
     *  @param size     Size to erase in bytes, must be a multiple of the erase block size
     *  @return         bd_status::OK on success or an error status on failure
     */
    virtual bd_status erase(bd_addr_t addr, bd_size_t size) = 0;

    /** Get the size of a readable block
     *
     *  @return         Size of a readable block in bytes
     */
    virtual bd_size_t get_read_size() const = 0;

    /** Get the size of a programmable block
     *
     *  @return         Size of a programmable block in bytes
     *  @note Must be a multiple of the read size
     */
    virtual bd_size_t get_program_size() const = 0;

    /** Get the size of an erasable block for the whole device
     *
     *  @return         Size of an erasable block in bytes for the whole device
     *  @note Must be a multiple of the program size, or 0 if no common erase size exists
     *        across all regions or the underlying implementation does not provide this
     *        information in which case you need to call get_erase_size(bd_addr_t) instead
     */
    virtual bd_size_t get_erase_size() const = 0;

    /** Get the size of an erasable block given address
     *
     *  @param addr     Address within the erasable block
     *  @return         Size of an erasable block in bytes
     *  @note Must be a multiple of the program size
     */
    virtual bd_size_t get_erase_size(bd_addr_t addr) const = 0;

    /** Get the value of storage when erased
     *
     *  If get_erase_value returns a non-negative byte value, the underlying
     *  storage is set to that value when erased, and storage containing
     *  that value can be programmed without another erase.
     *
     *  @return         The value of storage when erased, or -1 if you can't
     *                  rely on the value of the erased storage
     */
    virtual int get_erase_value() const = 0;

    /** Get the total size of the underlying device
     *
     *  @return         Size of the underlying device in bytes
     */
    virtual bd_size_t size() const = 0;

    /** Convenience function for checking block read validity
     *
     *  @param addr     Address of block to begin reading from
     *  @param size     Size to read in bytes
     *  @return         bd_status::OK if read is valid for underlying block device
     */
    virtual bd_status is_valid_read(bd_addr_t addr, bd_size_t size) const
    {
        if (addr + size > this->size()) {
            return bd_status::INCORRECT_SIZE;
        }

        if (addr % get_read_size() != 0) {
            return bd_status::ADDRESS_NOT_ALIGNED;
        }

        if (size % get_read_size() != 0) {
            return bd_status::SIZE_NOT_ALIGNED;
        }

        return bd_status::OK;
    }

    /** Convenience function for checking block program validity
     *
     *  @param addr     Address of block to begin writing to
     *  @param size     Size to write in bytes
     *  @return         bd_status::OK if program is valid for underlying block device
     */
    virtual bd_status is_valid_program(bd_addr_t addr, bd_size_t size) const
    {
        if (get_program_size() == 0) {
            return bd_status::DEVICE_ERROR;
        }

        if (addr + size > this->size()) {
            return bd_status::INCORRECT_SIZE;
        }

        if (size % get_program_size() != 0) {
            return bd_status::SIZE_NOT_ALIGNED;
        }

        if (addr % get_program_size() != 0) {
            return bd_status::ADDRESS_NOT_ALIGNED;
        }

        return bd_status::OK;
    }

    /** Convenience function for checking block erase validity
     *
     *  @param addr     Address of block to begin erasing
     *  @param size     Size to erase in bytes
     *  @return         bd_status::OK if erase is valid for underlying block device
     */
    virtual bd_status is_valid_erase(bd_addr_t addr, bd_size_t size) const
    {
        const auto erase_sz_addr = get_erase_size(addr);
        const auto erase_sz_blk = get_erase_size(addr + size - 1);

        if (erase_sz_addr == 0 || erase_sz_blk == 0) {
            return bd_status::DEVICE_ERROR;
        }

        if ((addr + size) > this->size()) {
            return bd_status::INCORRECT_SIZE;
        }

        if (addr % erase_sz_addr != 0) {
            return bd_status::ADDRESS_NOT_ALIGNED;
        }

        if ((addr + size) % erase_sz_blk != 0) {
            return bd_status::SIZE_NOT_ALIGNED;
        }

        return bd_status::OK;
    }

    /** Get the BlockDevice class type.
     *
     *  @return         A string represent the BlockDevice class type.
     */
    virtual const char *get_type() const = 0;
};

} // namespace storage
} // namespace iotsdk

#endif // IOTSDK_BLOCKDEVICE_H

/** @}*/
