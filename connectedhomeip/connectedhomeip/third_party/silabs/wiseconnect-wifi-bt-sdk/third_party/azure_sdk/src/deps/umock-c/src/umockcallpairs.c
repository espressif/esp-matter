// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <string.h>
#include <stddef.h>
#include "umock_c/umockcallpairs.h"
#include "umock_c/umockalloc.h"
#include "umock_c/umock_log.h"
#include "umock_c/umocktypes.h"

int umockcallpairs_track_create_paired_call(PAIRED_HANDLES* paired_handles, const void* handle, const char* handle_type, size_t handle_type_size)
{
    PAIRED_HANDLE* new_paired_handles;
    int result;

    if ((paired_handles == NULL) ||
        (handle == NULL) ||
        (handle_type == NULL))
    {
        /* Codes_SRS_UMOCKCALLPAIRS_01_004: [ If any of the arguments paired_handles, handle or handle_type is NULL, umockcallpairs_track_create_paired_call shallfail and return a non-zero value. ]*/
        result = __LINE__;
        UMOCK_LOG("umock_track_create_destroy_paired_calls_malloc: NULL paired_handles");
    }
    else
    {
        /* Codes_SRS_UMOCKCALLPAIRS_01_001: [ umockcallpairs_track_create_paired_call shall add a new entry to the PAIRED_HANDLES array and on success it shall return 0. ]*/
        new_paired_handles = (PAIRED_HANDLE*)umockalloc_realloc(paired_handles->paired_handles, sizeof(PAIRED_HANDLE) * (paired_handles->paired_handle_count + 1));
        if (new_paired_handles == NULL)
        {
            result = __LINE__;
            UMOCK_LOG("umock_track_create_destroy_paired_calls_malloc: Allocation failed");
        }
        else
        {
            paired_handles->paired_handle_count++;
            paired_handles->paired_handles = new_paired_handles;

            /* Codes_SRS_UMOCKCALLPAIRS_01_003: [ umockcallpairs_track_create_paired_call shall allocate a memory block and store a pointer to it in the memory field of the new entry. ]*/
            paired_handles->paired_handles[paired_handles->paired_handle_count - 1].handle_value = umockalloc_malloc(handle_type_size);
            if (paired_handles->paired_handles[paired_handles->paired_handle_count - 1].handle_value == NULL)
            {
                /* Codes_SRS_UMOCKCALLPAIRS_01_005: [ If allocating memory fails, umockcallpairs_track_create_paired_call shall fail and return a non-zero value. ]*/
                paired_handles->paired_handle_count--;
                if (paired_handles->paired_handle_count == 0)
                {
                    umockalloc_free(paired_handles->paired_handles);
                    paired_handles->paired_handles = NULL;
                }

                result = __LINE__;
                UMOCK_LOG("umock_track_create_destroy_paired_calls_malloc: Failed allocating memory for handle value for create");
            }
            else
            {
                size_t handle_type_length = strlen(handle_type);

                paired_handles->paired_handles[paired_handles->paired_handle_count - 1].handle_type = (char*)umockalloc_malloc(handle_type_length + 1);
                if (paired_handles->paired_handles[paired_handles->paired_handle_count - 1].handle_type == NULL)
                {
                    /* Codes_SRS_UMOCKCALLPAIRS_01_005: [ If allocating memory fails, umockcallpairs_track_create_paired_call shall fail and return a non-zero value. ]*/
                    umockalloc_free(paired_handles->paired_handles[paired_handles->paired_handle_count - 1].handle_value);
                    paired_handles->paired_handle_count--;
                    if (paired_handles->paired_handle_count == 0)
                    {
                        umockalloc_free(paired_handles->paired_handles);
                        paired_handles->paired_handles = NULL;
                    }

                    result = __LINE__;
                    UMOCK_LOG("umock_track_create_destroy_paired_calls_malloc: Failed allocating memory for handle type for create");
                }
                else
                {
                    (void)memcpy(paired_handles->paired_handles[paired_handles->paired_handle_count - 1].handle_type, handle_type, handle_type_length + 1);

                    /* Codes_SRS_UMOCKCALLPAIRS_01_002: [ umockcallpairs_track_create_paired_call shall copy the handle_value to the handle_value member of the new entry. ] */
                    /* Codes_SRS_UMOCKCALLPAIRS_01_006: [ The handle value shall be copied by using umocktypes_copy. ]*/
                    if (umocktypes_copy(handle_type, paired_handles->paired_handles[paired_handles->paired_handle_count - 1].handle_value, handle) != 0)
                    {
                        /* Codes_SRS_UMOCKCALLPAIRS_01_007: [ If umocktypes_copy fails, umockcallpairs_track_create_paired_call shall fail and return a non-zero value. ]*/
                        umockalloc_free(paired_handles->paired_handles[paired_handles->paired_handle_count - 1].handle_type);
                        umockalloc_free(paired_handles->paired_handles[paired_handles->paired_handle_count - 1].handle_value);
                        paired_handles->paired_handle_count--;
                        if (paired_handles->paired_handle_count == 0)
                        {
                            umockalloc_free(paired_handles->paired_handles);
                            paired_handles->paired_handles = NULL;
                        }

                        result = __LINE__;
                        UMOCK_LOG("umock_track_create_destroy_paired_calls_malloc: Failed copying handle");
                    }
                    else
                    {
                        result = 0;
                    }
                }
            }
        }
    }

    return result;
}

int umockcallpairs_track_destroy_paired_call(PAIRED_HANDLES* paired_handles, const void* handle)
{
    size_t i;
    int result;

    if ((paired_handles == NULL) ||
        (handle == NULL))
    {
        /* Codes_SRS_UMOCKCALLPAIRS_01_010: [ If any of the arguments is NULL, umockcallpairs_track_destroy_paired_call shall fail and return a non-zero value. ]*/
        result = __LINE__;
        UMOCK_LOG("umock_track_create_destroy_paired_calls_free: are_equal failed");
    }
    else
    {
        unsigned char is_error = 0;

        /* Codes_SRS_UMOCKCALLPAIRS_01_008: [ umockcallpairs_track_destroy_paired_call shall remove from the paired handles array pointed by the paired_handles field the entry that is associated with the handle passed in the handle argument. ]*/
        for (i = 0; i < paired_handles->paired_handle_count; i++)
        {
            /* Codes_SRS_UMOCKCALLPAIRS_01_013: [ When looking up which entry to remove, the comparison of the handle values shall be done by calling umocktypes_are_equal. ]*/
            int are_equal_result = umocktypes_are_equal(paired_handles->paired_handles[i].handle_type, paired_handles->paired_handles[i].handle_value, handle);
            if (are_equal_result < 0)
            {
                /* Codes_SRS_UMOCKCALLPAIRS_01_014: [ If umocktypes_are_equal fails, umockcallpairs_track_destroy_paired_call shall fail and return a non-zero value. ]*/
                result = __LINE__;
                is_error = 1;
                UMOCK_LOG("umock_track_create_destroy_paired_calls_free: are_equal failed");
                break;
            }
            else
            {
                if (are_equal_result == 1)
                {
                    break;
                }
            }
        }

        if (i == paired_handles->paired_handle_count)
        {
            /* Codes_SRS_UMOCKCALLPAIRS_01_015: [ If the handle is not found in the array then umockcallpairs_track_destroy_paired_call shall fail and return a non-zero value. ]*/
            result = __LINE__;
            UMOCK_LOG("umock_track_create_destroy_paired_calls_free: could not find handle");
        }
        else
        {
            if (!is_error)
            {
                /* Codes_SRS_UMOCKCALLPAIRS_01_011: [ umockcallpairs_track_destroy_paired_call shall free the memory pointed by the memory field in the PAIRED_HANDLES array entry associated with handle. ]*/
                umocktypes_free(paired_handles->paired_handles[i].handle_type, paired_handles->paired_handles[i].handle_value);
                umockalloc_free(paired_handles->paired_handles[i].handle_type);
                umockalloc_free(paired_handles->paired_handles[i].handle_value);
                (void)memmove(&paired_handles->paired_handles[i], &paired_handles->paired_handles[i + 1], sizeof(PAIRED_HANDLE) * (paired_handles->paired_handle_count - 1));
                paired_handles->paired_handle_count--;
                if (paired_handles->paired_handle_count == 0)
                {
                    /* Codes_SRS_UMOCKCALLPAIRS_01_012: [ If the array paired handles array is empty after removing the entry, the paired_handles field shall be freed and set to NULL. ]*/
                    umockalloc_free(paired_handles->paired_handles);
                    paired_handles->paired_handles = NULL;
                }

                /* Codes_SRS_UMOCKCALLPAIRS_01_009: [ On success umockcallpairs_track_destroy_paired_call shall return 0. ]*/
                result = 0;
            }
            else
            {
                result = __LINE__;
            }
        }
    }

    return result;
}
