/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "iotsdk/block_device.h"

#include "serial_printf.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Implement this function to provide a block device to the application. The
// application doesn't need to know or care what the underlying block device is
// so long as it works through the block device interface. The block device
// storage could hypothetically be backed by a region of internal flash, the
// second of four connected QSPI flash chips, or network-attached storage,
// among other things-- the application need not know nor care.
extern iotsdk_blockdevice_t *get_example_block_device(void);

static void display_device_information(iotsdk_blockdevice_t *bd);
static void exercise_device(iotsdk_blockdevice_t *bd);
static void populate_test_pattern(uint8_t *buf, size_t len);
static void hex_dump(const uint8_t *buf, size_t len);

int main(void)
{
    iotsdk_blockdevice_t *bd;
    iotsdk_blockdevice_status_t status;

    serial_printf("--- IoT SDK experimental block device example ---\r\n");

    /* Make a new internal-flash-backed block device. */
    bd = get_example_block_device();
    status = iotsdk_blockdevice_initialize(bd);
    if (status) {
        serial_printf("Initialize failed. Error: %d\r\n", status);
        return status;
    }

    display_device_information(bd);

    exercise_device(bd);

    /* Clean up */
    status = iotsdk_blockdevice_uninitialize(bd);
    if (status) {
        serial_printf("Uninitialize failed. Error: %d\r\n", status);
        return status;
    }
    serial_printf("--- Done! ---\r\n");

    return 0;
}

/* Display information about the device. */
static void display_device_information(iotsdk_blockdevice_t *bd)
{
    size_t program_size;
    size_t first_erase_size;
    size_t erase_size;
    uint8_t erase_val;
    size_t read_size;
    size_t size;

    read_size = iotsdk_blockdevice_get_read_size(bd);
    program_size = iotsdk_blockdevice_get_program_size(bd);
    first_erase_size = iotsdk_blockdevice_get_erase_size(bd, 0);
    erase_size = iotsdk_blockdevice_get_erase_size(bd, 0x1000);
    size = iotsdk_blockdevice_get_size(bd);
    erase_val = iotsdk_blockdevice_get_erase_value(bd);

    serial_printf("--- Block device information ---\r\n");
    serial_printf("size:                        %" PRIuPTR " B\r\n", size);
    serial_printf("read size:                   %" PRIuPTR " B\r\n", read_size);
    serial_printf("program size:                %" PRIuPTR " B\r\n", program_size);
    serial_printf("erase size at offset 0x0000: %" PRIuPTR " B\r\n", first_erase_size);
    serial_printf("erase size at offset 0x1000: %" PRIuPTR " B\r\n", erase_size);
    serial_printf("erase value:                 %" PRIX8 "  \r\n", erase_val);
}

/* Erase, Write, Read */
static void exercise_device(iotsdk_blockdevice_t *bd)
{
    size_t dest_addr;
    size_t erase_size;
    size_t program_size;
    uint8_t *buffer;
    iotsdk_blockdevice_status_t status;
    size_t read_size;

    dest_addr = 0;

    /*
     *  Erase the first sector.
     */
    serial_printf("--- Erase ---\r\n");
    erase_size = iotsdk_blockdevice_get_erase_size(bd, dest_addr);
    serial_printf("erase(0x%" PRIXPTR ", %" PRIuPTR ")\r\n", dest_addr, erase_size);
    status = iotsdk_blockdevice_erase(bd, dest_addr, erase_size);
    if (status) {
        serial_printf("Erase failed. Error %d\r\n", status);
        return;
    }
    serial_printf("Erased %" PRIuPTR " bytes\r\n", erase_size);

    /* Allocate a buffer large enough for a single program operation. */
    program_size = iotsdk_blockdevice_get_program_size(bd);
    buffer = calloc(program_size, sizeof(*buffer));
    if (!buffer) {
        return;
    }

    /* Write a test pattern in memory. */
    populate_test_pattern(buffer, program_size);

    /*
     * Write the test pattern to the block device.
     */
    serial_printf("--- Program ---\r\n");
    serial_printf("program(0x%" PRIXPTR ", 0x%" PRIXPTR ", %" PRIuPTR ")\r\n", dest_addr, buffer, program_size);
    status = iotsdk_blockdevice_program(bd, dest_addr, buffer, program_size);
    if (status) {
        serial_printf("Program failed. Error %d\r\n", status);
        goto done;
    }
    serial_printf("Programmed %" PRIuPTR " bytes\r\n", program_size);
    serial_printf("Up to 256 bytes of programmed data:\r\n");
    hex_dump(buffer, program_size > 256 ? 256 : program_size);

    /* Clobber the buffer so we know what data was recently read. */
    memset(buffer, 0xCC, program_size);

    /*
     * Read a small portion of data.
     */
    serial_printf("--- Read ---\r\n");
    read_size = iotsdk_blockdevice_get_read_size(bd);
    /* Read the larger of the minimum read size and 16 bytes. */
    read_size = read_size > 16 ? read_size : 16;
    serial_printf("read(0x%" PRIXPTR ", 0x%" PRIXPTR ", %" PRIuPTR ")\r\n", dest_addr, buffer, read_size);
    status = iotsdk_blockdevice_read(bd, dest_addr, buffer, read_size);
    if (status) {
        serial_printf("Read failed. Error %d\r\n", status);
        goto done;
    }
    serial_printf("Read %" PRIuPTR " bytes\r\n", read_size);
    serial_printf("Read data:\r\n");
    hex_dump(buffer, read_size);

done:
    free(buffer);
}

static void populate_test_pattern(uint8_t *buf, size_t len)
{
    const size_t pattern_modifier = 16U;
    for (size_t i = 0; i < len; ++i) {
        buf[i] = i * (i % pattern_modifier);
    }
}

static void hex_dump(const uint8_t *buf, size_t len)
{
    const size_t print_width = 16U;
    for (size_t i = 0; i < len; ++i) {
        if (i % print_width == 0) {
            if (i != 0) {
                serial_printf("\r\n");
            }
            serial_printf("\t");
        }
        serial_printf("%02X ", buf[i]);
    }
    serial_printf("\r\n");
}
