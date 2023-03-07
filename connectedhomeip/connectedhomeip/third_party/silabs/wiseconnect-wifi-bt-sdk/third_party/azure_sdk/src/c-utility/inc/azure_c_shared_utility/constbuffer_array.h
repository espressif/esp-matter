// Copyright (C) Microsoft Corporation. All rights reserved.

#ifndef CONSTBUFFER_ARRAY_H
#define CONSTBUFFER_ARRAY_H

#ifdef __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_c_shared_utility/constbuffer.h"

#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CONSTBUFFER_ARRAY_HANDLE_DATA_TAG* CONSTBUFFER_ARRAY_HANDLE;

/*create*/
MOCKABLE_FUNCTION(, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_create, const CONSTBUFFER_HANDLE*, buffers, uint32_t, buffer_count);
MOCKABLE_FUNCTION(, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_create_with_move_buffers, CONSTBUFFER_HANDLE*, buffers, uint32_t, buffer_count);
MOCKABLE_FUNCTION(, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_create_empty);
MOCKABLE_FUNCTION(, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_create_from_array_array, const CONSTBUFFER_ARRAY_HANDLE*, buffer_arrays, uint32_t, buffer_array_count);

MOCKABLE_FUNCTION(, void, constbuffer_array_inc_ref, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_handle);
MOCKABLE_FUNCTION(, void, constbuffer_array_dec_ref, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_handle);

/*add in front*/
MOCKABLE_FUNCTION(, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_add_front, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_handle, CONSTBUFFER_HANDLE, constbuffer_handle);

/*remove front*/
MOCKABLE_FUNCTION(, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_remove_front, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_handle, CONSTBUFFER_HANDLE *, constbuffer_handle);

/* getters */
MOCKABLE_FUNCTION(, int, constbuffer_array_get_buffer_count, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_handle, uint32_t*, buffer_count);
MOCKABLE_FUNCTION(, CONSTBUFFER_HANDLE, constbuffer_array_get_buffer, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_handle, uint32_t, buffer_index);
MOCKABLE_FUNCTION(, const CONSTBUFFER*, constbuffer_array_get_buffer_content, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_handle, uint32_t, buffer_index);
MOCKABLE_FUNCTION(, int, constbuffer_array_get_all_buffers_size, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_handle, uint32_t*, all_buffers_size);
MOCKABLE_FUNCTION(, const CONSTBUFFER_HANDLE*, constbuffer_array_get_const_buffer_handle_array, CONSTBUFFER_ARRAY_HANDLE, constbuffer_array_handle);

/*compare*/
MOCKABLE_FUNCTION(, bool, CONSTBUFFER_ARRAY_HANDLE_contain_same, CONSTBUFFER_ARRAY_HANDLE, left, CONSTBUFFER_ARRAY_HANDLE, right);

#ifdef __cplusplus
}
#endif

#endif /*CONSTBUFFER_ARRAY_H*/
