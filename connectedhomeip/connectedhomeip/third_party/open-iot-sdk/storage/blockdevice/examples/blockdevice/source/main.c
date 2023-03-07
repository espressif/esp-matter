/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "serial_printf.h"

#include "iotsdk/Driver_Flash.h"

static bool display_device_information(ARM_DRIVER_FLASH *block_device);
// Erase, write and read block device
static void exercise_device(ARM_DRIVER_FLASH *block_device);
static void populate_test_pattern(uint8_t *buffer, size_t length);
static void hex_dump(const uint8_t *buffer, size_t length);
static size_t get_read_size(const ARM_DRIVER_FLASH *block_device);

// Implement the functions below to provide the application with block device
// information.

/** Gets an uninitialised instance of a block device
 *
 * @note
 * Provides an unitialised instance of a block device to the application
 *
 *  @return An uninitialised instance of a block device
 */
extern ARM_DRIVER_FLASH *get_example_block_device(void);

/** Gets the total size of a block device
 *
 * @note
 * Provides the total flash size depending on whether the sectors are uniform or not
 *
 *  @return The number of bytes in the block device
 */
extern size_t get_flash_size(void);

/** Gets the size of an erasable block given address
 *
 * @note
 * Provides the erase size of a sector the address pertains to depending on whether
 * the sectors are uniform or not.
 *
 *  @param addr Address within the erasable block
 *  @param erase_size Pointer to location to store the size of an erasable block in bytes
 *  @return
 * - ARM_DRIVER_OK: If the erase size is successfully retrieved
 * - ARM_DRIVER_ERROR_PARAMETER: If the address is out of bound
 */
extern int32_t get_erase_size(uint32_t addr, size_t *erase_size);

int main(void)
{
    serial_printf("--- IoT SDK block device example ---\r\n");

    ARM_DRIVER_FLASH *block_device = get_example_block_device();

    int32_t status = block_device->Initialize(NULL);

    if (ARM_DRIVER_OK != status) {
        serial_printf("Initialize failed. Error: %d\r\n", status);
        return status;
    }

    if (!display_device_information(block_device)) {
        return -1;
    };

    exercise_device(block_device);

    status = block_device->Uninitialize();

    if (ARM_DRIVER_OK != status) {
        serial_printf("Uninitialize failed. Error: %d\r\n", status);
        return status;
    }

    serial_printf("--- Done! ---\r\n");

    return 0;
}

static bool display_device_information(ARM_DRIVER_FLASH *block_device)
{
    size_t first_erase_size = 0U;
    int32_t status = get_erase_size(0x0000, &first_erase_size);
    if (ARM_DRIVER_OK != status) {
        serial_printf("Erase size. Error %d\r\n", status);
        return false;
    }

    size_t erase_size = 0U;
    status = get_erase_size(0x1000, &erase_size);
    if (ARM_DRIVER_OK != status) {
        serial_printf("Erase size. Error %d\r\n", status);
        return false;
    }

    serial_printf("--- Block device information ---\r\n");
    serial_printf("size:                        %" PRIuPTR " B\r\n", get_flash_size());
    serial_printf("read size:                   %" PRIuPTR " B\r\n", get_read_size(block_device));
    serial_printf("program size:                %" PRIuPTR " B\r\n", block_device->GetInfo()->page_size);
    serial_printf("erase size at offset 0x0000: %" PRIuPTR " B\r\n", first_erase_size);
    serial_printf("erase size at offset 0x1000: %" PRIuPTR " B\r\n", erase_size);
    serial_printf("erase value:                 %" PRIX8 "  \r\n", block_device->GetInfo()->erased_value);

    return true;
}

static void exercise_device(ARM_DRIVER_FLASH *block_device)
{
    /*
     *  Erase the first sector.
     */
    serial_printf("--- Erase ---\r\n");
    uint32_t dest_addr = 0x00000000;
    serial_printf("EraseSector(0x%" PRIXPTR ")\r\n", dest_addr);
    int32_t status = block_device->EraseSector(dest_addr);
    if (ARM_DRIVER_OK != status) {
        serial_printf("Erase failed. Error %d\r\n", status);
        return;
    }
    serial_printf("Erased sector at 0x%" PRIuPTR "\r\n", dest_addr);

    /* Allocate a buffer large enough for a single program operation. */
    size_t program_size = block_device->GetInfo()->page_size;
    uint8_t *buffer = calloc(program_size, sizeof(*buffer));
    if (!buffer) {
        return;
    }

    /* Write a test pattern in memory. */
    populate_test_pattern(buffer, program_size);

    /*
     * Write the test pattern to the block device.
     */
    serial_printf("--- Program ---\r\n");
    uint32_t cnt = program_size / block_device->GetInfo()->program_unit;
    serial_printf("ProgramData(0x%" PRIXPTR ", 0x%" PRIXPTR ", %" PRIuPTR ")\r\n", dest_addr, buffer, cnt);
    status = block_device->ProgramData(dest_addr, buffer, cnt);
    if (ARM_DRIVER_OK != status) {
        serial_printf("Program failed. Error %d\r\n", status);
        goto done;
    }
    serial_printf("Programmed %" PRIuPTR " bytes\r\n", cnt * block_device->GetInfo()->program_unit);
    serial_printf("Up to 256 bytes of programmed data:\r\n");
    hex_dump(buffer, program_size > 256 ? 256 : program_size);

    /* Clobber the buffer so we know what data was recently read. */
    memset(buffer, 0xCC, program_size);

    /*
     * Read a small portion of data.
     */
    serial_printf("--- Read ---\r\n");
    size_t read_data_width = get_read_size(block_device);
    cnt = 16U / read_data_width;
    serial_printf("ReadData(0x%" PRIXPTR ", 0x%" PRIXPTR ", %" PRIuPTR ")\r\n", dest_addr, buffer, cnt);
    status = block_device->ReadData(dest_addr, buffer, cnt);
    if (ARM_DRIVER_OK != status) {
        serial_printf("Read failed. Error %d\r\n", status);
        goto done;
    }
    size_t read_size = cnt * read_data_width;
    serial_printf("Read %" PRIuPTR " bytes\r\n", read_size);
    serial_printf("Read data:\r\n");
    hex_dump(buffer, read_size);

done:
    free(buffer);
}

static size_t get_read_size(const ARM_DRIVER_FLASH *block_device)
{
    assert(block_device->GetCapabilities().data_width <= 2);

    return 1U << block_device->GetCapabilities().data_width;
}

static void populate_test_pattern(uint8_t *buffer, size_t length)
{
    const size_t pattern_modifier = 16U;
    for (size_t i = 0; i < length; ++i) {
        buffer[i] = i * (i % pattern_modifier);
    }
}

static void hex_dump(const uint8_t *buffer, size_t length)
{
    const size_t print_width = 16U;
    for (size_t i = 0; i < length; ++i) {
        if (i % print_width == 0) {
            if (i != 0) {
                serial_printf("\r\n");
            }

            serial_printf("\t");
        }

        serial_printf("%02X ", buffer[i]);
    }

    serial_printf("\r\n");
}
