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

pw_add_library(pw_blob_store INTERFACE
  PUBLIC_DEPS
    pw_bytes
    pw_containers
    pw_kvs
    pw_span
    pw_status
    pw_stream
  PRIVATE_DEPS
    pw_assert
    pw_checksum
    pw_log
    pw_random
    pw_string
)

pw_add_library(pw_blob_store.flat_file_system_entry INTERFACE
  PUBLIC_DEPS
    pw_blob_store
    pw_bytes
    pw_containers
    pw_file.flat_file_system
    pw_kvs
    pw_span
    pw_status
    pw_stream
    pw_sync.mutex
  PRIVATE_DEPS
    pw_assert
    pw_checksum
    pw_log
    pw_random
    pw_string
)

pw_add_test(pw_blob_store.blob_store_chunk_write_test
  SOURCES
    blob_store_chunk_write_test.cc
  PRIVATE_DEPS
    pw_blob_store
  GROUPS
    pw_blob_store
)

pw_add_test(pw_blob_store.blob_store_deferred_write_test
  SOURCES
    blob_store_deferred_write_test.cc
  PRIVATE_DEPS
    pw_blob_store
  GROUPS
    pw_blob_store
)

pw_add_test(pw_blob_store.flat_file_system_entry_test
  SOURCES
    flat_file_system_entry_test.cc
  PRIVATE_DEPS
    pw_blob_store
    pw_blob_store.flat_file_system_entry
  GROUPS
    pw_blob_store
)

