/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

/** \addtogroup storage */
/** @{*/

#ifndef IOTSDK_BLOCK_DEVICE_H
#define IOTSDK_BLOCK_DEVICE_H

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

typedef struct iotsdk_blockdevice_s iotsdk_blockdevice_t;

/** Enum of status codes
 *
 *  @enum iotsdk_blockdevice_status_t
 */
typedef enum {
    IOTSDK_BD_STATUS_OK = 0,                         /*!< no error */
    IOTSDK_BD_STATUS_DEVICE_ERROR = -4001,           /*!< device specific error */
    IOTSDK_BD_STATUS_DEVICE_NOT_INITIALIZED = -4002, /*!< device not initialized */
    IOTSDK_BD_STATUS_INCORRECT_SIZE = -4003,         /*!< size was incorrect */
    IOTSDK_BD_STATUS_INCORRECT_ADDRESS = -4004,      /*!< address was incorrect */
    IOTSDK_BD_STATUS_ADDRESS_NOT_ALIGNED = -4005,    /*!< address was not aligned with sector/page size */
    IOTSDK_BD_STATUS_SIZE_NOT_ALIGNED = -4006        /*!< size was not aligned with sector/page size */
} iotsdk_blockdevice_status_t;

typedef struct iotsdk_blockdevice_vtable_s {
    iotsdk_blockdevice_status_t (*initialize)(iotsdk_blockdevice_t *self);
    iotsdk_blockdevice_status_t (*uninitialize)(iotsdk_blockdevice_t *self);
    iotsdk_blockdevice_status_t (*read)(iotsdk_blockdevice_t *self, size_t addr, void *data, size_t size);
    iotsdk_blockdevice_status_t (*program)(iotsdk_blockdevice_t *self, size_t addr, const void *data, size_t size);
    iotsdk_blockdevice_status_t (*erase)(iotsdk_blockdevice_t *self, size_t addr, size_t size);
    iotsdk_blockdevice_status_t (*is_valid_read)(iotsdk_blockdevice_t *self, size_t addr, size_t size);
    iotsdk_blockdevice_status_t (*is_valid_program)(iotsdk_blockdevice_t *self, size_t addr, size_t size);
    iotsdk_blockdevice_status_t (*is_valid_erase)(iotsdk_blockdevice_t *self, size_t addr, size_t size);
    size_t (*get_size)(iotsdk_blockdevice_t *self);
    size_t (*get_read_size)(iotsdk_blockdevice_t *self);
    size_t (*get_program_size)(iotsdk_blockdevice_t *self);
    size_t (*get_erase_size)(iotsdk_blockdevice_t *self, size_t addr);
    int32_t (*get_erase_value)(iotsdk_blockdevice_t *self);
} iotsdk_blockdevice_vtable_t;

struct iotsdk_blockdevice_s {
    const iotsdk_blockdevice_vtable_t *vtbl;
};

/** Initialize a block device
 *
 *  This method must be called before attempting any further block device operations.
 *
 *  @return         IOTSDK_BD_STATUS_OK on success or an error status on failure
 */
static inline iotsdk_blockdevice_status_t iotsdk_blockdevice_initialize(iotsdk_blockdevice_t *self)
{
    assert(self != NULL && self->vtbl != NULL && self->vtbl->initialize != NULL);
    return self->vtbl->initialize(self);
}

/** Uninitialize a block device
 *
 *  @return         IOTSDK_BD_STATUS_OK on success or an error status on failure
 */
static inline iotsdk_blockdevice_status_t iotsdk_blockdevice_uninitialize(iotsdk_blockdevice_t *self)
{
    assert(self != NULL && self->vtbl != NULL && self->vtbl->uninitialize != NULL);
    return self->vtbl->uninitialize(self);
}

/** Read blocks from a block device
 *
 *  If a failure occurs, it is not possible to determine how many bytes succeeded.
 *
 *  @param buffer   Buffer to write blocks to
 *  @param addr     Address of block to begin reading from
 *  @param size     Size to read in bytes, must be a multiple of the read block size
 *  @return         IOTSDK_BD_STATUS_OK on success or an error status on failure
 */
static inline iotsdk_blockdevice_status_t
iotsdk_blockdevice_read(iotsdk_blockdevice_t *self, size_t addr, void *data, size_t size)
{
    assert(self != NULL && self->vtbl != NULL && self->vtbl->read != NULL);
    return self->vtbl->read(self, addr, data, size);
}

/** Program blocks to a block device
 *
 *  The blocks must have been erased prior to being programmed.
 *
 *  If a failure occurs, it is not possible to determine how many bytes succeeded.
 *
 *  @param addr     Address of block to begin writing to
 *  @param data     Buffer of data to write to blocks
 *  @param size     Size to write in bytes, must be a multiple of the program block size
 *  @return         IOTSDK_BD_STATUS_OK on success or an error status on failure
 */
static inline iotsdk_blockdevice_status_t
iotsdk_blockdevice_program(iotsdk_blockdevice_t *self, size_t addr, const void *data, size_t size)
{
    assert(self != NULL && self->vtbl != NULL && self->vtbl->program != NULL);
    return self->vtbl->program(self, addr, data, size);
}

/** Erase blocks on a block device
 *
 *  The state of an erased block is undefined until it has been programmed,
 *  unless get_erase_value returns a non-negative byte value.
 *
 *  @param addr     Address of block to begin erasing
 *  @param size     Size to erase in bytes, must be a multiple of the erase block size
 *  @return         IOTSDK_BD_STATUS_OK on success or an error status on failure
 */
static inline iotsdk_blockdevice_status_t iotsdk_blockdevice_erase(iotsdk_blockdevice_t *self, size_t addr, size_t size)
{
    assert(self != NULL && self->vtbl != NULL && self->vtbl->erase != NULL);
    return self->vtbl->erase(self, addr, size);
}

/** Check block read validity
 *
 *  @param addr     Address of block to begin reading from
 *  @param size     Size to read in bytes
 *  @return         IOTSDK_BD_STATUS_OK if read is valid for underlying block device
 */
static inline iotsdk_blockdevice_status_t
iotsdk_blockdevice_is_valid_read(iotsdk_blockdevice_t *self, size_t addr, size_t size)
{
    assert(self != NULL && self->vtbl != NULL && self->vtbl->is_valid_read != NULL);
    return self->vtbl->is_valid_read(self, addr, size);
}

/** Check block program validity
 *
 *  @param addr     Address of block to begin writing to
 *  @param size     Size to write in bytes
 *  @return         IOTSDK_BD_STATUS_OK if program is valid for underlying block device
 */
static inline iotsdk_blockdevice_status_t
iotsdk_blockdevice_is_valid_program(iotsdk_blockdevice_t *self, size_t addr, size_t size)
{
    assert(self != NULL && self->vtbl != NULL && self->vtbl->is_valid_program != NULL);
    return self->vtbl->is_valid_program(self, addr, size);
}

/** Check block erase validity
 *
 *  @param addr     Address of block to begin erasing
 *  @param size     Size to erase in bytes
 *  @return         IOTSDK_BD_STATUS_OK if erase is valid for underlying block device
 */
static inline iotsdk_blockdevice_status_t
iotsdk_blockdevice_is_valid_erase(iotsdk_blockdevice_t *self, size_t addr, size_t size)
{
    assert(self != NULL && self->vtbl != NULL && self->vtbl->is_valid_erase != NULL);
    return self->vtbl->is_valid_erase(self, addr, size);
}

/** Get the total size of the underlying device
 *
 *  @return         Size of the underlying device in bytes
 */
static inline size_t iotsdk_blockdevice_get_size(iotsdk_blockdevice_t *self)
{
    assert(self != NULL && self->vtbl != NULL && self->vtbl->get_size != NULL);
    return self->vtbl->get_size(self);
}

/** Get the size of a readable block
 *
 *  @return         Size of a readable block in bytes
 */
static inline size_t iotsdk_blockdevice_get_read_size(iotsdk_blockdevice_t *self)
{
    assert(self != NULL && self->vtbl != NULL && self->vtbl->get_read_size != NULL);
    return self->vtbl->get_read_size(self);
}

/** Get the size of a programmable block
 *
 *  @return         Size of a programmable block in bytes
 *  @note Must be a multiple of the read size
 */
static inline size_t iotsdk_blockdevice_get_program_size(iotsdk_blockdevice_t *self)
{
    assert(self != NULL && self->vtbl != NULL && self->vtbl->get_program_size != NULL);
    return self->vtbl->get_program_size(self);
}

/** Get the size of an erasable block given address
 *
 *  @param addr     Address within the erasable block
 *  @return         Size of an erasable block in bytes
 *  @note Must be a multiple of the program size
 */
static inline size_t iotsdk_blockdevice_get_erase_size(iotsdk_blockdevice_t *self, size_t addr)
{
    assert(self != NULL && self->vtbl != NULL && self->vtbl->get_erase_size != NULL);
    return self->vtbl->get_erase_size(self, addr);
}

/** Get the value of storage when erased
 *
 *  If get_erase_value returns a non-negative byte value, the underlying
 *  storage is set to that value when erased, and storage containing
 *  that value can be programmed without another erase.
 *
 *  @return         The value of storage when erased, or -1 if you can't
 *                  rely on the value of the erased storage
 */
static inline int32_t iotsdk_blockdevice_get_erase_value(iotsdk_blockdevice_t *self)
{
    assert(self != NULL && self->vtbl != NULL && self->vtbl->get_erase_value != NULL);
    return self->vtbl->get_erase_value(self);
}

#endif // IOTSDK_BLOCK_DEVICE_H

/** @}*/
