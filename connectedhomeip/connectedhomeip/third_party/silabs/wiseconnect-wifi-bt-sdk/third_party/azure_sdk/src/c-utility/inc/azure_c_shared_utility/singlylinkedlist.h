// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef SINGLYLINKEDLIST_H
#define SINGLYLINKEDLIST_H

#ifdef __cplusplus
#else
#include "stdbool.h"
#endif /* __cplusplus */

#include "umock_c/umock_c_prod.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct SINGLYLINKEDLIST_INSTANCE_TAG* SINGLYLINKEDLIST_HANDLE;
typedef struct LIST_ITEM_INSTANCE_TAG* LIST_ITEM_HANDLE;

/**
* @brief                        Function passed to singlylinkedlist_find, which returns whichever first list item that matches it.
* @param list_item                Current list node being evaluated.
* @param match_context            Context passed to singlylinkedlist_find.
* @returns                        True to indicate that the current list node is the one to be returned, or false to continue traversing the list.
*/
typedef bool (*LIST_MATCH_FUNCTION)(LIST_ITEM_HANDLE list_item, const void* match_context);
/**
* @brief                        Function passed to singlylinkedlist_remove_if, which is used to define if an item of the list should be removed or not.
* @param item                    Value of the current list node being evaluated for removal.
* @param match_context            Context passed to singlylinkedlist_remove_if.
* @param continue_processing    Indicates if singlylinkedlist_remove_if shall continue iterating through the next nodes of the list or stop.
* @returns                        True to indicate that the current list node shall be removed, or false to not to.
*/
typedef bool (*LIST_CONDITION_FUNCTION)(const void* item, const void* match_context, bool* continue_processing);
/**
* @brief                        Function passed to singlylinkedlist_foreach, which is called for the value of each node of the list.
* @param item                    Value of the current list node being processed.
* @param action_context            Context passed to singlylinkedlist_foreach.
* @param continue_processing    Indicates if singlylinkedlist_foreach shall continue iterating through the next nodes of the list or stop.
*/
typedef void (*LIST_ACTION_FUNCTION)(const void* item, const void* action_context, bool* continue_processing);

MOCKABLE_FUNCTION(, SINGLYLINKEDLIST_HANDLE, singlylinkedlist_create);
MOCKABLE_FUNCTION(, void, singlylinkedlist_destroy, SINGLYLINKEDLIST_HANDLE, list);
MOCKABLE_FUNCTION(, LIST_ITEM_HANDLE, singlylinkedlist_add, SINGLYLINKEDLIST_HANDLE, list, const void*, item);
MOCKABLE_FUNCTION(, LIST_ITEM_HANDLE, singlylinkedlist_add_head, SINGLYLINKEDLIST_HANDLE, list, const void*, item);
MOCKABLE_FUNCTION(, int, singlylinkedlist_remove, SINGLYLINKEDLIST_HANDLE, list, LIST_ITEM_HANDLE, item_handle);
MOCKABLE_FUNCTION(, LIST_ITEM_HANDLE, singlylinkedlist_get_head_item, SINGLYLINKEDLIST_HANDLE, list);
MOCKABLE_FUNCTION(, LIST_ITEM_HANDLE, singlylinkedlist_get_next_item, LIST_ITEM_HANDLE, item_handle);
MOCKABLE_FUNCTION(, LIST_ITEM_HANDLE, singlylinkedlist_find, SINGLYLINKEDLIST_HANDLE, list, LIST_MATCH_FUNCTION, match_function, const void*, match_context);
MOCKABLE_FUNCTION(, const void*, singlylinkedlist_item_get_value, LIST_ITEM_HANDLE, item_handle);
MOCKABLE_FUNCTION(, int, singlylinkedlist_remove_if, SINGLYLINKEDLIST_HANDLE, list, LIST_CONDITION_FUNCTION, condition_function, const void*, match_context);
MOCKABLE_FUNCTION(, int, singlylinkedlist_foreach, SINGLYLINKEDLIST_HANDLE, list, LIST_ACTION_FUNCTION, action_function, const void*, action_context);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SINGLYLINKEDLIST_H */
