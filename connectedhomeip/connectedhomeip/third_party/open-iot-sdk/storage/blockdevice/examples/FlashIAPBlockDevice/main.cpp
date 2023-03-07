/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
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
    serial_printf("--- IoT SDK block device example ---\r\n");

    iotsdk::storage::FlashIAPBlockDevice bd{get_ram_drive_instance(), 0, 0};

    // The BlockDevice API uses dual stage initialization so we can handle any
    // errors returned by the flash device. We must call the `init` method to
    // initialize the underlying device.
    iotsdk::storage::bd_status err = bd.init();
    if (err != iotsdk::storage::bd_status::OK) {
        const auto ret = static_cast<int>(err);
        serial_printf("bd.init failed. Error: %d\r\n", ret);
        return ret;
    }

    const char *type = bd.get_type();
    serial_printf("Block device type: %s\r\n", type);

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

    // Allocate a buffer with enough space for our data, aligned to the
    // nearest program_size. This is the minimum size necessary to write
    // data to a block.
    size_t buffer_size = sizeof("Hello Storage!") + program_size - 1;
    buffer_size = buffer_size - (buffer_size % program_size);
    std::unique_ptr<char> buffer(new (std::nothrow) char[buffer_size]);

    // Update the buffer with the string we want to store.
    snprintf(buffer.get(), buffer_size, "%s", "Hello Storage!");

    // Write data to the first block. The write occurs in two parts: an erase
    // followed by a program.
    serial_printf("bd.erase(%d, %" PRIu64 ")\r\n", 0, erase_size);
    err = bd.erase(0, erase_size);
    if (err != iotsdk::storage::bd_status::OK) {
        const auto ret = static_cast<int>(err);
        serial_printf("bd.erase failed. Error %d\r\n", ret);
        return ret;
    }

    serial_printf("bd.program(0x%" PRIxPTR ", %d, %d)\r\n", buffer.get(), 0, buffer_size);
    err = bd.program(buffer.get(), 0, buffer_size);
    if (err != iotsdk::storage::bd_status::OK) {
        const auto ret = static_cast<int>(err);
        serial_printf("bd.program failed. Error %d\r\n", ret);
        return ret;
    }

    // Clobber the buffer so we don't get old data.
    memset(buffer.get(), 0xcc, buffer_size);

    // Read the data from the first block. Note that the program_size must be a
    // multiple of the read_size. This is so we don't have to check for
    // alignment.
    serial_printf("bd.read(0x%" PRIxPTR ", %d, %d)\r\n", buffer.get(), 0, buffer_size);
    err = bd.read(buffer.get(), 0, buffer_size);
    if (err != iotsdk::storage::bd_status::OK) {
        const auto ret = static_cast<int>(err);
        serial_printf("bd.read failed. Error %d\r\n", ret);
        return ret;
    }

    serial_printf("--- Stored data ---\r\n");
    serial_printf("%s\r\n", buffer.get());
    serial_printf("--- Done! ---\r\n");
    return 0;
}
