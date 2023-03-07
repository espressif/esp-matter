# Copyright 2020 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

include($ENV{PW_ROOT}/pw_build/pigweed.cmake)

pw_add_module_config(pw_kvs_CONFIG)

pw_add_library(pw_kvs.config INTERFACE
  HEADERS
    pw_kvs_private/config.h
  PUBLIC_DEPS
    ${pw_kvs_CONFIG}
)

pw_add_library(pw_kvs STATIC
  HEADERS
    public/pw_kvs/alignment.h
    public/pw_kvs/checksum.h
    public/pw_kvs/flash_memory.h
    public/pw_kvs/flash_test_partition.h
    public/pw_kvs/format.h
    public/pw_kvs/io.h
    public/pw_kvs/key.h
    public/pw_kvs/key_value_store.h
    public/pw_kvs/internal/entry.h
    public/pw_kvs/internal/entry_cache.h
    public/pw_kvs/internal/hash.h
    public/pw_kvs/internal/key_descriptor.h
    public/pw_kvs/internal/sectors.h
    public/pw_kvs/internal/span_traits.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_assert
    pw_bytes
    pw_containers
    pw_span
    pw_status
    pw_stream
  SOURCES
    alignment.cc
    checksum.cc
    entry.cc
    entry_cache.cc
    flash_memory.cc
    format.cc
    key_value_store.cc
    sectors.cc
  PRIVATE_DEPS
    pw_checksum
    pw_kvs.config
    pw_log
)

pw_add_library(pw_kvs.crc16 INTERFACE
  HEADERS
    public/pw_kvs/crc16_checksum.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_checksum
    pw_kvs
    pw_span
)

pw_add_library(pw_kvs.flash_test_partition INTERFACE
  HEADERS
    public/pw_kvs/flash_test_partition.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_kvs
)

pw_add_library(pw_kvs.test_key_value_store INTERFACE
  HEADERS
    public/pw_kvs/test_key_value_store.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_kvs
    pw_sync.borrow
)

pw_add_library(pw_kvs.fake_flash STATIC
  HEADERS
    public/pw_kvs/fake_flash_memory.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_containers
    pw_kvs
    pw_status
  SOURCES
    fake_flash_memory.cc
  PRIVATE_DEPS
    pw_kvs.config
    pw_log
)

pw_add_library(pw_kvs.fake_flash_12_byte_partition STATIC
  HEADERS
    public/pw_kvs/flash_test_partition.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_kvs.flash_test_partition
  SOURCES
    fake_flash_test_partition.cc
  PRIVATE_DEPS
    pw_kvs.fake_flash
    pw_kvs
  PRIVATE_DEFINES
    PW_FLASH_TEST_SECTORS=3
    PW_FLASH_TEST_SECTOR_SIZE=4
    PW_FLASH_TEST_ALIGNMENT=4
)

pw_add_library(pw_kvs.fake_flash_1_aligned_partition STATIC
  HEADERS
    public/pw_kvs/flash_test_partition.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_kvs.flash_test_partition
  SOURCES
    fake_flash_test_partition.cc
  PRIVATE_DEPS
    pw_kvs.fake_flash
    pw_kvs
  PRIVATE_DEFINES
    PW_FLASH_TEST_SECTORS=6U
    PW_FLASH_TEST_SECTOR_SIZE=4096U
    PW_FLASH_TEST_ALIGNMENT=1U
)

pw_add_library(pw_kvs.fake_flash_16_aligned_partition STATIC
  HEADERS
    public/pw_kvs/flash_test_partition.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_kvs.flash_test_partition
  SOURCES
    fake_flash_test_partition.cc
  PRIVATE_DEPS
    pw_kvs.fake_flash
    pw_kvs
  PRIVATE_DEFINES
    PW_FLASH_TEST_SECTORS=6U
    PW_FLASH_TEST_SECTOR_SIZE=4096U
    PW_FLASH_TEST_ALIGNMENT=16U
)

pw_add_library(pw_kvs.fake_flash_64_aligned_partition STATIC
  HEADERS
    public/pw_kvs/flash_test_partition.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_kvs.flash_test_partition
  SOURCES
    fake_flash_test_partition.cc
  PRIVATE_DEPS
    pw_kvs.fake_flash
    pw_kvs
  PRIVATE_DEFINES
    PW_FLASH_TEST_SECTORS=6U
    PW_FLASH_TEST_SECTOR_SIZE=4096U
    PW_FLASH_TEST_ALIGNMENT=64U
)

pw_add_library(pw_kvs.fake_flash_256_aligned_partition STATIC
  HEADERS
    public/pw_kvs/flash_test_partition.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_kvs.flash_test_partition
  SOURCES
    fake_flash_test_partition.cc
  PRIVATE_DEPS
    pw_kvs.fake_flash
    pw_kvs
  PRIVATE_DEFINES
    PW_FLASH_TEST_SECTORS=6U
    PW_FLASH_TEST_SECTOR_SIZE=4096U
    PW_FLASH_TEST_ALIGNMENT=256U
)

pw_add_library(pw_kvs.fake_flash_test_key_value_store STATIC
  SOURCES
    fake_flash_test_key_value_store.cc
  PRIVATE_DEPS
    pw_kvs
    pw_kvs.crc16
    pw_kvs.fake_flash
    pw_kvs.test_key_value_store
)

pw_add_library(pw_kvs.flash_partition_stream_test STATIC
  HEADERS
    public/pw_kvs/flash_memory.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_sync.borrow
    pw_bytes
    pw_kvs
    pw_polyfill
    pw_preprocessor
    pw_status
    pw_stream
  SOURCES
    flash_partition_stream_test.cc
  PRIVATE_DEPS
    pw_kvs.config
    pw_kvs.fake_flash
    pw_kvs.flash_test_partition
    pw_kvs
    pw_log
    pw_random
    pw_unit_test
)

pw_add_library(pw_kvs.flash_partition_test_100_iterations STATIC
  SOURCES
    flash_partition_test.cc
  PRIVATE_DEPS
    pw_kvs.config
    pw_kvs.flash_test_partition
    pw_kvs
    pw_log
    pw_unit_test
  PRIVATE_DEFINES
    PW_FLASH_TEST_ITERATIONS=100
    PW_FLASH_TEST_WRITE_SIZE=1
)

pw_add_library(pw_kvs.flash_partition_test_2_iterations STATIC
  SOURCES
    flash_partition_test.cc
  PRIVATE_DEPS
    pw_kvs.config
    pw_kvs.flash_test_partition
    pw_kvs
    pw_log
    pw_unit_test
  PRIVATE_DEFINES
    PW_FLASH_TEST_ITERATIONS=2
    PW_FLASH_TEST_WRITE_SIZE=1
)

pw_add_library(pw_kvs.flash_partition_test_100_iterations_256_write STATIC
  SOURCES
    flash_partition_test.cc
  PRIVATE_DEPS
    pw_kvs.config
    pw_kvs.flash_test_partition
    pw_kvs
    pw_log
    pw_unit_test
  PRIVATE_DEFINES
    PW_FLASH_TEST_ITERATIONS=100
    PW_FLASH_TEST_WRITE_SIZE=256
)

pw_add_library(pw_kvs.flash_partition_test_2_iterations_256_write STATIC
  SOURCES
    flash_partition_test.cc
  PRIVATE_DEPS
    pw_kvs.config
    pw_kvs.flash_test_partition
    pw_kvs
    pw_log
    pw_unit_test
  PRIVATE_DEFINES
    PW_FLASH_TEST_ITERATIONS=2
    PW_FLASH_TEST_WRITE_SIZE=256
)

pw_add_library(pw_kvs.key_value_store_initialized_test STATIC
  SOURCES
    key_value_store_initialized_test.cc
  PRIVATE_DEPS
    pw_kvs.crc16
    pw_kvs.flash_test_partition
    pw_kvs
    pw_string.builder
    pw_bytes
    pw_checksum
    pw_log
    pw_unit_test
)

pw_add_library(pw_kvs.key_value_store_fuzz_test STATIC
  SOURCES
    key_value_store_fuzz_test.cc
  PRIVATE_DEPS
    pw_kvs.config
    pw_kvs.crc16
    pw_kvs.flash_test_partition
    pw_kvs
    pw_string.builder
    pw_bytes
    pw_checksum
    pw_log
    pw_unit_test
)

pw_add_library(pw_kvs.test_key_value_store_test STATIC
  SOURCES
    test_key_value_store_test.cc
  PRIVATE_DEPS
    pw_kvs
    pw_kvs.test_key_value_store
    pw_string.builder
    pw_sync.borrow
    pw_unit_test
)

pw_add_library(pw_kvs.test_partition STATIC
  HEADERS
    public/pw_kvs/flash_partition_with_stats.h
  PUBLIC_INCLUDES
    public
  PUBLIC_DEPS
    pw_kvs
    pw_log
    pw_status
  SOURCES
    flash_partition_with_stats.cc
  PRIVATE_DEPS
    pw_kvs.config
)

pw_add_test(pw_kvs.alignment_test
  SOURCES
    alignment_test.cc
  PRIVATE_DEPS
    pw_kvs
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.checksum_test
  SOURCES
    checksum_test.cc
  PRIVATE_DEPS
    pw_kvs.crc16
    pw_kvs
    pw_log
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.converts_to_span_test
  SOURCES
    converts_to_span_test.cc
  PRIVATE_DEPS
    pw_kvs
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.entry_test
  SOURCES
    entry_test.cc
  PRIVATE_DEPS
    pw_kvs.crc16
    pw_kvs.fake_flash
    pw_kvs
    pw_bytes
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.entry_cache_test
  SOURCES
    entry_cache_test.cc
  PRIVATE_DEPS
    pw_kvs.fake_flash
    pw_kvs
    pw_bytes
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.flash_partition_1_stream_test
  PRIVATE_DEPS
    pw_kvs.fake_flash
    pw_kvs.fake_flash_1_aligned_partition
    pw_kvs.flash_partition_stream_test
    pw_log
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.flash_partition_1_alignment_test
  PRIVATE_DEPS
    pw_kvs.fake_flash
    pw_kvs.fake_flash_1_aligned_partition
    pw_kvs.flash_partition_test_100_iterations
    pw_log
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.flash_partition_16_alignment_test
  PRIVATE_DEPS
    pw_kvs.fake_flash
    pw_kvs.fake_flash_16_aligned_partition
    pw_kvs.flash_partition_test_100_iterations
    pw_log
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.flash_partition_64_alignment_test
  PRIVATE_DEPS
    pw_kvs.fake_flash
    pw_kvs.fake_flash_64_aligned_partition
    pw_kvs.flash_partition_test_100_iterations
    pw_log
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.flash_partition_256_alignment_test
  PRIVATE_DEPS
    pw_kvs.fake_flash
    pw_kvs.fake_flash_256_aligned_partition
    pw_kvs.flash_partition_test_100_iterations
    pw_log
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.flash_partition_256_write_size_test
  PRIVATE_DEPS
    pw_kvs.fake_flash
    pw_kvs.fake_flash_1_aligned_partition
    pw_kvs.flash_partition_test_100_iterations_256_write
    pw_log
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.key_value_store_test
  SOURCES
    key_value_store_test.cc
  PRIVATE_DEPS
    pw_kvs.config
    pw_kvs.crc16
    pw_kvs.fake_flash
    pw_kvs
    pw_string.builder
    pw_bytes
    pw_checksum
    pw_log
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.key_value_store_1_alignment_flash_test
  PRIVATE_DEPS
    pw_kvs.fake_flash_1_aligned_partition
    pw_kvs.key_value_store_initialized_test
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.key_value_store_16_alignment_flash_test
  PRIVATE_DEPS
    pw_kvs.fake_flash_16_aligned_partition
    pw_kvs.key_value_store_initialized_test
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.key_value_store_64_alignment_flash_test
  PRIVATE_DEPS
    pw_kvs.fake_flash_64_aligned_partition
    pw_kvs.key_value_store_initialized_test
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.key_value_store_256_alignment_flash_test
  PRIVATE_DEPS
    pw_kvs.fake_flash_256_aligned_partition
    pw_kvs.key_value_store_initialized_test
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.key_value_store_fuzz_1_alignment_flash_test
  PRIVATE_DEPS
    pw_kvs.fake_flash_1_aligned_partition
    pw_kvs.key_value_store_fuzz_test
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.key_value_store_fuzz_64_alignment_flash_test
  PRIVATE_DEPS
    pw_kvs.fake_flash_64_aligned_partition
    pw_kvs.key_value_store_fuzz_test
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.key_value_store_binary_format_test
  SOURCES
    key_value_store_binary_format_test.cc
  PRIVATE_DEPS
    pw_kvs.crc16
    pw_kvs.fake_flash
    pw_kvs
    pw_bytes
    pw_log
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.key_value_store_put_test
  SOURCES
    key_value_store_put_test.cc
  PRIVATE_DEPS
    pw_kvs.crc16
    pw_kvs.fake_flash
    pw_kvs
    pw_kvs.test_partition
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.fake_flash_test_key_value_store_test
  PRIVATE_DEPS
    pw_kvs.fake_flash_test_key_value_store
    pw_kvs.test_key_value_store_test
    pw_sync.borrow
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.key_value_store_map_test
  SOURCES
    key_value_store_map_test.cc
  PRIVATE_DEPS
    pw_kvs.crc16
    pw_kvs.fake_flash
    pw_kvs
    pw_kvs.test_partition
    pw_string.builder
    pw_checksum
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.sectors_test
  SOURCES
    sectors_test.cc
  PRIVATE_DEPS
    pw_kvs.fake_flash
    pw_kvs
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.key_test
  SOURCES
    key_test.cc
  PRIVATE_DEPS
    pw_kvs
  GROUPS
    modules
    pw_kvs
)

pw_add_test(pw_kvs.key_value_store_wear_test
  SOURCES
    key_value_store_wear_test.cc
  PRIVATE_DEPS
    pw_kvs.fake_flash
    pw_kvs
    pw_kvs.test_partition
    pw_log
  GROUPS
    modules
    pw_kvs
)
