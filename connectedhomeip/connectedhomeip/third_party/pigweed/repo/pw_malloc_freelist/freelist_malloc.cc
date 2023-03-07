// Copyright 2020 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#include "pw_allocator/freelist_heap.h"
#include "pw_malloc/malloc.h"
#include "pw_preprocessor/compiler.h"
#include "pw_preprocessor/util.h"
#include "pw_span/span.h"

namespace {
std::aligned_storage_t<sizeof(pw::allocator::FreeListHeapBuffer<>),
                       alignof(pw::allocator::FreeListHeapBuffer<>)>
    buf;
}  // namespace
pw::allocator::FreeListHeapBuffer<>* pw_freelist_heap;

#if __cplusplus
extern "C" {
#endif  // __cplusplus
// Define the global heap variables.
void pw_MallocInit(uint8_t* heap_low_addr, uint8_t* heap_high_addr) {
  pw::span<std::byte> pw_allocator_freelist_raw_heap =
      pw::span(reinterpret_cast<std::byte*>(heap_low_addr),
               heap_high_addr - heap_low_addr);
  pw_freelist_heap = new (&buf)
      pw::allocator::FreeListHeapBuffer(pw_allocator_freelist_raw_heap);
}

// Wrapper functions for malloc, free, realloc and calloc.
// With linker options "-Wl --wrap=<function name>", linker will link
// "__wrap_<function name>" with "<function_name>", and calling
// "<function name>" will call "__wrap_<function name>" instead
// Linker options are set in a config in "pw_malloc:pw_malloc_config".
void* __wrap_malloc(size_t size) { return pw_freelist_heap->Allocate(size); }

void __wrap_free(void* ptr) { pw_freelist_heap->Free(ptr); }

void* __wrap_realloc(void* ptr, size_t size) {
  return pw_freelist_heap->Realloc(ptr, size);
}

void* __wrap_calloc(size_t num, size_t size) {
  return pw_freelist_heap->Calloc(num, size);
}

void* __wrap__malloc_r(struct _reent*, size_t size) {
  return pw_freelist_heap->Allocate(size);
}

void __wrap__free_r(struct _reent*, void* ptr) { pw_freelist_heap->Free(ptr); }

void* __wrap__realloc_r(struct _reent*, void* ptr, size_t size) {
  return pw_freelist_heap->Realloc(ptr, size);
}

void* __wrap__calloc_r(struct _reent*, size_t num, size_t size) {
  return pw_freelist_heap->Calloc(num, size);
}
#if __cplusplus
}
#endif  // __cplusplus
