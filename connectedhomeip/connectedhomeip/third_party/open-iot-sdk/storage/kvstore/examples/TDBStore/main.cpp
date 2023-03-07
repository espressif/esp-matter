/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "iotsdk/FlashIAPBlockDevice.h"
#include "iotsdk/TDBStore.h"
#include "iotsdk/KVStore.h"

extern "C" {
#include "serial_printf.h"

#include "flash_cs300.h"
}

#include <inttypes.h>
#include <cstdint>
#include <memory>
#include <string>

int basic_set_get();
int incremental_set_partial_get();
int iterators();

int main()
{
    serial_printf("--- IoT SDK TDBStore example ---\r\n\r\n");

    // Basic set, get, remove
    basic_set_get();

    // Incremental set - add data in multiple stages
    // Partial get - get partial data using offsets
    incremental_set_partial_get();

    // Use iterators to iterate through all entries, or all entries with keys starting with "prefix"
    iterators();

    serial_printf("\r\n--- Done! ---\r\n");
    return 0;
}

int basic_set_get()
{
    // Underlying block device
    iotsdk::storage::FlashIAPBlockDevice flash_bd{get_ram_drive_instance(), 0, 0};

    // Create a TDBStore using the underlying storage
    iotsdk::storage::TDBStore tdb{&flash_bd};

    // KVStore uses dual stage initialization so we can handle any errors
    // Call the `init` method to setup the TDBStore
    iotsdk::storage::kv_status err = tdb.init();
    if (err != iotsdk::storage::kv_status::OK) {
        const auto ret = static_cast<int>(err);
        serial_printf("tdb.init failed. Error: %d\r\n", ret);
        return ret;
    }

    /*
        Basic set/get
    */
    serial_printf("--- Basic set/get ---\r\n");
    const char *val1 = "value1";
    err = tdb.set("key1", val1, strlen(val1), 0);
    if (err != iotsdk::storage::kv_status::OK) {
        serial_printf("tdb.set failed. Error: %d\r\n", static_cast<int>(err));
    } else {
        serial_printf("Set value of \'key1\'=\'%s\', size=%" PRIuPTR "\r\n", val1, strlen(val1));
    }

    char buffer[8];
    size_t read_size;
    err = tdb.get("key1", buffer, sizeof(buffer) / sizeof(char), &read_size);
    if (err != iotsdk::storage::kv_status::OK) {
        serial_printf("tdb.get failed. Error: %d\r\n", static_cast<int>(err));
    } else {
        serial_printf("Get value of \'key1\'=\'%s\', read_size=%" PRIuPTR "\r\n\r\n",
                      std::string(buffer, read_size).c_str(),
                      read_size);
    }

    err = tdb.remove("key1");
    if (err != iotsdk::storage::kv_status::OK) {
        serial_printf("tdb.remove failed. Error: %d\r\n", static_cast<int>(err));
    }

    // Always call deinit() when finished
    // Since TDBStore uses a BufferedBlockDevice, writes to underlying storage are cached
    // Calling deinit() and checking the error code ensures that any cached writes are
    // written out to storage
    err = tdb.deinit();
    if (err != iotsdk::storage::kv_status::OK) {
        serial_printf("tdb.deinit failed. Error: %d\r\n", static_cast<int>(err));
    }

    return 0;
}

int incremental_set_partial_get()
{
    // Underlying block device
    iotsdk::storage::FlashIAPBlockDevice flash_bd{get_ram_drive_instance(), 0, 0};

    // Create a TDBStore using the underlying storage
    iotsdk::storage::TDBStore tdb{&flash_bd};

    // KVStore uses dual stage initialization so we can handle any errors
    // Call the `init` method to setup the TDBStore
    iotsdk::storage::kv_status err = tdb.init();
    if (err != iotsdk::storage::kv_status::OK) {
        const auto ret = static_cast<int>(err);
        serial_printf("tdb.init failed. Error: %d\r\n", ret);
        return ret;
    }

    /*
        Incremental set can be used to set large data values with small buffers
        Note: only one incremental set can be in progress at any one time
        Get can be used with an offset to only get partial data
    */
    serial_printf("--- Incremental set/offset get ---\r\n");
    const size_t total_data_size = 256;
    iotsdk::storage::KVStore::set_handle_t handle;

    // Specify the key and total_data_size to be used
    err = tdb.set_start(&handle, "key2", total_data_size, 0);
    if (err != iotsdk::storage::kv_status::OK) {
        serial_printf("tdb.set_start failed. Error: %d\r\n", static_cast<int>(err));
    } else {
        serial_printf("Incremental set with total_data_size=%" PRIuPTR "\r\n", total_data_size);
    }

    // Call set_add_data() until we've added total_data_size bytes
    char buf[64];
    for (size_t i = 0; i < (total_data_size / sizeof(buf)); i++) {
        memset(buf, i + 'a', sizeof(buf) / sizeof(char));
        err = tdb.set_add_data(handle, buf, sizeof(buf) / sizeof(char));
        if (err != iotsdk::storage::kv_status::OK) {
            serial_printf("tdb.set_add_data failed. Error: %d\r\n", static_cast<int>(err));
        } else {
            serial_printf("Adding data of size=%" PRIuPTR " char=%c\r\n", sizeof(buf) / sizeof(char), buf[0]);
        }
    }

    // Finalize the write to storage
    err = tdb.set_finalize(handle);
    if (err != iotsdk::storage::kv_status::OK) {
        serial_printf("tdb.set_finalize failed. Error: %d\r\n", static_cast<int>(err));
    }

    // Partial get() by reading at offsets
    char return_buffer[64];
    size_t read_size;
    for (size_t i = 0; i < 4; i++) {
        err = tdb.get(
            "key2", return_buffer, 64, &read_size, i * 64); // get 64 bytes at a time at 0, 64, 128 and 196 offsets
        if (err != iotsdk::storage::kv_status::OK) {
            serial_printf("tdb.get failed. Error: %d\r\n", static_cast<int>(err));
        } else {
            serial_printf("Value of \'key2\' at offset:%" PRIuPTR "=\'%c\' read_size=%" PRIuPTR "\r\n",
                          i * 64,
                          return_buffer[0],
                          read_size);
        }
    }

    // Always call deinit() when finished
    // Since TDBStore uses a BufferedBlockDevice, writes to underlying storage are cached
    // Calling deinit() and checking the error code ensures that any cached writes are
    // written out to storage
    err = tdb.deinit();
    if (err != iotsdk::storage::kv_status::OK) {
        serial_printf("tdb.deinit failed. Error: %d\r\n", static_cast<int>(err));
    }

    return 0;
}

int iterators()
{
    // Underlying block device
    iotsdk::storage::FlashIAPBlockDevice flash_bd{get_ram_drive_instance(), 0, 0};

    // Create a TDBStore using the underlying storage
    iotsdk::storage::TDBStore tdb{&flash_bd};

    // KVStore uses dual stage initialization so we can handle any errors
    // Call the `init` method to setup the TDBStore
    iotsdk::storage::kv_status err = tdb.init();
    if (err != iotsdk::storage::kv_status::OK) {
        const auto ret = static_cast<int>(err);
        serial_printf("tdb.init failed. Error: %d\r\n", ret);
        return ret;
    }

    /*
        Using iterators
    */
    serial_printf("\r\n--- Iterators ---\r\n");
    iotsdk::storage::KVStore::iterator_t iterator;
    tdb.set("prefix_first_key", "one", 4, 0);
    tdb.set("second_key", "data", 5, 0);
    tdb.set("prefix_second_key", "two", 4, 0);
    tdb.set("third_key", "data", 5, 0);
    tdb.set("prefix_third_key", "three", 6, 0);

    err = tdb.iterator_open(&iterator, "prefix");
    if (err != iotsdk::storage::kv_status::OK) {
        serial_printf("tdb.iterator_next failed. Error: %d\r\n", static_cast<int>(err));
    }

    // Iterate through keys starting with "prefix"
    char key_buf[25];
    while (tdb.iterator_next(iterator, key_buf, 25) == iotsdk::storage::kv_status::OK) {
        serial_printf("Iteration: key=%s\r\n", std::string(key_buf, sizeof(key_buf)).c_str());
    }

    // Always close the iterator when finished to free memory
    err = tdb.iterator_close(iterator);
    if (err != iotsdk::storage::kv_status::OK) {
        serial_printf("tdb.iterator_close failed. Error: %d\r\n", static_cast<int>(err));
    }

    // Always call deinit() when finished
    // Since TDBStore uses a BufferedBlockDevice, writes to underlying storage are cached
    // Calling deinit() and checking the error code ensures that any cached writes are
    // written out to storage
    err = tdb.deinit();
    if (err != iotsdk::storage::kv_status::OK) {
        serial_printf("tdb.deinit failed. Error: %d\r\n", static_cast<int>(err));
    }

    return 0;
}
