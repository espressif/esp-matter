/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "iotsdk/BufferedBlockDevice.h"
#include "iotsdk/FlashIAPBlockDevice.h"

extern "C" {
#include "serial_printf.h"

#include "flash_cs300.h"
}

#include <inttypes.h>
#include <cstdint>
#include <memory>

int main()
{
    serial_printf("--- IoT SDK buffered block device example ---\r\n");
    // Define a FlashIAPBlockDevice, which might have a program_size equivalent to
    // its page size e.g. 256 bytes
    // This restricts us to programming by page size or a multiple of this
    iotsdk::storage::FlashIAPBlockDevice flash_bd{get_ram_drive_instance(), 0, 0};

    // BufferedBlockDevice is used to program or read a much smaller amount of data
    // than the minimum program or read size supported by the underlying block device
    iotsdk::storage::BufferedBlockDevice bd{&flash_bd};

    // The BlockDevice API uses dual stage initialization so we can handle any
    // errors returned by the flash device. Call the `init` method to initialize
    // both the buffered block device and the underlying device.
    iotsdk::storage::bd_status err = bd.init();
    if (err != iotsdk::storage::bd_status::OK) {
        serial_printf("bd.init failed. Error: %d\r\n", err);
        return static_cast<int>(err);
    }

    const char *type = bd.get_type();
    serial_printf("Underlying block device type: %s\r\n", type);

    iotsdk::storage::bd_size_t read_size = bd.get_read_size();
    iotsdk::storage::bd_size_t program_size = bd.get_program_size();
    iotsdk::storage::bd_size_t erase_size = bd.get_erase_size(0);
    iotsdk::storage::bd_size_t size = bd.size();

    serial_printf("--- Block device geometry ---\r\n");
    serial_printf("read_size:    %" PRIu64 " B\r\n", read_size);
    serial_printf("program_size: %" PRIu64 " B\r\n", program_size);
    serial_printf("erase_size:   %" PRIu64 " B\r\n", erase_size);
    serial_printf("size:         %" PRIu64 " B\r\n", size);
    serial_printf("---\r\n");

    // BufferedBlockDevice provides a read_size and program_size of 1, so there's
    // no need to worry about the underlying BlockDevice program/read size and
    // aligning our reads/writes with this.

    char buffer[] = "Hello Buffered Storage";
    size_t buffer_size = strlen(buffer);

    serial_printf("bd.program(0x%" PRIxPTR ", %d, %d)\r\n", buffer, 0, buffer_size);
    // As buffer_size is unaligned with the underlying FlashIAP program_size (256),
    // the below will be written to the BufferedBlockDevice cache, and not directly
    // onto the device
    err = bd.program(buffer, 0, buffer_size);
    if (err != iotsdk::storage::bd_status::OK) {
        serial_printf("bd.program failed. Error %d\r\n", err);
        return static_cast<int>(err);
    }

    // Clobber the buffer so we don't get old data.
    memset(buffer, 0xcc, buffer_size);

    // Read any amount of data
    serial_printf("bd.read(0x%" PRIxPTR ", %d, %d)\r\n", buffer, 0, buffer_size);
    err = bd.read(buffer, 0, buffer_size);
    if (err != iotsdk::storage::bd_status::OK) {
        serial_printf("bd.read failed. Error %d\r\n", err);
        return static_cast<int>(err);
    }

    serial_printf("--- Stored data ---\r\n");
    serial_printf("%s\r\n", buffer);
    serial_printf("--- Done! ---\r\n");

    // To write buffered/cached writes out to storage, we can call sync().
    // Calling program() on any other block or reaching the limit of the underlying
    // program_size will also write out the cache to storage.
    err = bd.sync();
    if (err != iotsdk::storage::bd_status::OK) {
        serial_printf("bd.sync failed. Error %d\r\n", err);
        return static_cast<int>(err);
    }

    // Always call deinit() when finished with the BufferedBlockDevice.
    // deinit() also calls sync() to flush the cache, so always check the
    // return codes to ensure any writes to underlying storage were successful
    // to avoid potential data loss.
    // This also deinits the underlying block device.
    err = bd.deinit();
    if (err != iotsdk::storage::bd_status::OK) {
        serial_printf("bd.deinit failed. Error: %d\r\n", err);
        return static_cast<int>(err);
    }

    return 0;
}
