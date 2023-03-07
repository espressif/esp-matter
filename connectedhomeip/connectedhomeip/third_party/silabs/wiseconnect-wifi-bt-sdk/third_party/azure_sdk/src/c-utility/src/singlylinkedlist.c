// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/singlylinkedlist.h"
#include "azure_c_shared_utility/optimize_size.h"
#include "azure_c_shared_utility/xlogging.h"

typedef struct LIST_ITEM_INSTANCE_TAG
{
    const void* item;
    void* next;
} LIST_ITEM_INSTANCE;

typedef struct SINGLYLINKEDLIST_INSTANCE_TAG
{
    LIST_ITEM_INSTANCE* head;
    LIST_ITEM_INSTANCE* tail;
} LIST_INSTANCE;

SINGLYLINKEDLIST_HANDLE singlylinkedlist_create(void)
{
    LIST_INSTANCE* result;

    /* Codes_SRS_LIST_01_001: [singlylinkedlist_create shall create a new list and return a non-NULL handle on success.] */
    result = (LIST_INSTANCE*)malloc(sizeof(LIST_INSTANCE));
    if (result != NULL)
    {
        /* Codes_SRS_LIST_01_002: [If any error occurs during the list creation, singlylinkedlist_create shall return NULL.] */
        result->head = NULL;
        result->tail = NULL;
    }

    return result;
}

void singlylinkedlist_destroy(SINGLYLINKEDLIST_HANDLE list)
{
    /* Codes_SRS_LIST_01_004: [If the list argument is NULL, no freeing of resources shall occur.] */
    if (list != NULL)
    {
        LIST_INSTANCE* list_instance = (LIST_INSTANCE*)list;

        while (list_instance->head != NULL)
        {
            LIST_ITEM_INSTANCE* current_item = list_instance->head;
            list_instance->head = (LIST_ITEM_INSTANCE*)current_item->next;
            free(current_item);
        }

        /* Codes_SRS_LIST_01_003: [singlylinkedlist_destroy shall free all resources associated with the list identified by the handle argument.] */
        free(list_instance);
    }
}

LIST_ITEM_HANDLE singlylinkedlist_add(SINGLYLINKEDLIST_HANDLE list, const void* item)
{
    LIST_ITEM_INSTANCE* result;

    /* Codes_SRS_LIST_01_006: [If any of the arguments is NULL, singlylinkedlist_add shall not add the item to the list and return NULL.] */
    if ((list == NULL) ||
        (item == NULL))
    {
        LogError("Invalid argument (list=%p, item=%p)", list, item);
        result = NULL;
    }
    else
    {
        LIST_INSTANCE* list_instance = (LIST_INSTANCE*)list;
        result = (LIST_ITEM_INSTANCE*)malloc(sizeof(LIST_ITEM_INSTANCE));

        if (result == NULL)
        {
            /* Codes_SRS_LIST_01_007: [If allocating the new list node fails, singlylinkedlist_add shall return NULL.] */
            /*return as is*/
        }
        else
        {
            /* Codes_SRS_LIST_01_005: [singlylinkedlist_add shall add one item to the tail of the list and on success it shall return a handle to the added item.] */
            result->next = NULL;
            result->item = item;

            if (list_instance->head == NULL)
            {
                list_instance->head = result;
                list_instance->tail = result;
            }
            else
            {
                list_instance->tail->next = result;
                list_instance->tail = result;
            }
        }
    }

    return result;
}

int singlylinkedlist_remove(SINGLYLINKEDLIST_HANDLE list, LIST_ITEM_HANDLE item)
{
    int result;

    /* Codes_SRS_LIST_01_024: [If any of the arguments list or item_handle is NULL, singlylinkedlist_remove shall fail and return a non-zero value.] */
    if ((list == NULL) ||
        (item == NULL))
    {
        LogError("Invalid argument (list=%p, item=%p)", list, item);
        result = MU_FAILURE;
    }
    else
    {
        LIST_INSTANCE* list_instance = (LIST_INSTANCE*)list;
        LIST_ITEM_INSTANCE* current_item = list_instance->head;
        LIST_ITEM_INSTANCE* previous_item = NULL;

        while (current_item != NULL)
        {
            if (current_item == item)
            {
                if (previous_item != NULL)
                {
                    previous_item->next = current_item->next;
                }
                else
                {
                    list_instance->head = (LIST_ITEM_INSTANCE*)current_item->next;
                }

                if (current_item == list_instance->tail)
                {
                    list_instance->tail = previous_item;
                }

                free(current_item);

                break;
            }

            previous_item = current_item;
            current_item = (LIST_ITEM_INSTANCE*)current_item->next;
        }

        if (current_item == NULL)
        {
            /* Codes_SRS_LIST_01_025: [If the item item_handle is not found in the list, then singlylinkedlist_remove shall fail and return a non-zero value.] */
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_LIST_01_023: [singlylinkedlist_remove shall remove a list item from the list and on success it shall return 0.] */
            result = 0;
        }
    }

    return result;
}

LIST_ITEM_HANDLE singlylinkedlist_get_head_item(SINGLYLINKEDLIST_HANDLE list)
{
    LIST_ITEM_HANDLE result;

    if (list == NULL)
    {
        /* Codes_SRS_LIST_01_009: [If the list argument is NULL, singlylinkedlist_get_head_item shall return NULL.] */
        LogError("Invalid argument (list=NULL)");
        result = NULL;
    }
    else
    {
        LIST_INSTANCE* list_instance = (LIST_INSTANCE*)list;

        /* Codes_SRS_LIST_01_008: [singlylinkedlist_get_head_item shall return the head of the list.] */
        /* Codes_SRS_LIST_01_010: [If the list is empty, singlylinkedlist_get_head_item_shall_return NULL.] */
        result = list_instance->head;
    }

    return result;
}

LIST_ITEM_HANDLE singlylinkedlist_get_next_item(LIST_ITEM_HANDLE item_handle)
{
    LIST_ITEM_HANDLE result;

    if (item_handle == NULL)
    {
        LogError("Invalid argument (list is NULL)");
        /* Codes_SRS_LIST_01_019: [If item_handle is NULL then singlylinkedlist_get_next_item shall return NULL.] */
        result = NULL;
    }
    else
    {
        /* Codes_SRS_LIST_01_018: [singlylinkedlist_get_next_item shall return the next item in the list following the item item_handle.] */
        result = (LIST_ITEM_HANDLE)((LIST_ITEM_INSTANCE*)item_handle)->next;
    }

    return result;
}

const void* singlylinkedlist_item_get_value(LIST_ITEM_HANDLE item_handle)
{
    const void* result;

    if (item_handle == NULL)
    {
        LogError("Invalid argument (item_handle is NULL)");
        /* Codes_SRS_LIST_01_021: [If item_handle is NULL, singlylinkedlist_item_get_value shall return NULL.] */
        result = NULL;
    }
    else
    {
        /* Codes_SRS_LIST_01_020: [singlylinkedlist_item_get_value shall return the value associated with the list item identified by the item_handle argument.] */
        result = ((LIST_ITEM_INSTANCE*)item_handle)->item;
    }

    return result;
}

LIST_ITEM_HANDLE singlylinkedlist_find(SINGLYLINKEDLIST_HANDLE list, LIST_MATCH_FUNCTION match_function, const void* match_context)
{
    LIST_ITEM_HANDLE result;

    if ((list == NULL) ||
        (match_function == NULL))
    {
        LogError("Invalid argument (list=%p, match_function=%p)", list, match_function);
        /* Codes_SRS_LIST_01_012: [If the list or the match_function argument is NULL, singlylinkedlist_find shall return NULL.] */
        result = NULL;
    }
    else
    {
        LIST_INSTANCE* list_instance = (LIST_INSTANCE*)list;
        LIST_ITEM_INSTANCE* current = list_instance->head;

        /* Codes_SRS_LIST_01_011: [singlylinkedlist_find shall iterate through all items in a list and return the first one that satisfies a certain match function.] */
        while (current != NULL)
        {
            /* Codes_SRS_LIST_01_014: [list find shall determine whether an item satisfies the match criteria by invoking the match function for each item in the list until a matching item is found.] */
            /* Codes_SRS_LIST_01_013: [The match_function shall get as arguments the list item being attempted to be matched and the match_context as is.] */
            if (match_function((LIST_ITEM_HANDLE)current, match_context) == true)
            {
                /* Codes_SRS_LIST_01_017: [If the match function returns true, singlylinkedlist_find shall consider that item as matching.] */
                break;
            }

            /* Codes_SRS_LIST_01_016: [If the match function returns false, singlylinkedlist_find shall consider that item as not matching.] */
            current = (LIST_ITEM_INSTANCE*)current->next;
        }

        if (current == NULL)
        {
            /* Codes_SRS_LIST_01_015: [If the list is empty, singlylinkedlist_find shall return NULL.] */
            result = NULL;
        }
        else
        {
            result = current;
        }
    }

    return result;
}

int singlylinkedlist_remove_if(SINGLYLINKEDLIST_HANDLE list, LIST_CONDITION_FUNCTION condition_function, const void* match_context)
{
    int result;
    /* Codes_SRS_LIST_09_001: [ If the list or the condition_function argument is NULL, singlylinkedlist_remove_if shall return non-zero value. ] */
    if ((list == NULL) ||
        (condition_function == NULL))
    {
        LogError("Invalid argument (list=%p, condition_function=%p)", list, condition_function);
        result = MU_FAILURE;
    }
    else
    {
        LIST_INSTANCE* list_instance = (LIST_INSTANCE*)list;
        LIST_ITEM_INSTANCE* current_item = list_instance->head;
        LIST_ITEM_INSTANCE* next_item = NULL;
        LIST_ITEM_INSTANCE* previous_item = NULL;

        /* Codes_SRS_LIST_09_002: [ singlylinkedlist_remove_if shall iterate through all items in a list and remove all that satisfies a certain condition function. ] */
        while (current_item != NULL)
        {
            bool continue_processing = false;

            next_item = (LIST_ITEM_INSTANCE*)current_item->next;

            /* Codes_SRS_LIST_09_003: [ singlylinkedlist_remove_if shall determine whether an item satisfies the condition criteria by invoking the condition function for that item. ] */
            /* Codes_SRS_LIST_09_004: [ If the condition function returns true, singlylinkedlist_find shall consider that item as to be removed. ] */
            if (condition_function(current_item->item, match_context, &continue_processing) == true)
            {
                if (previous_item != NULL)
                {
                    previous_item->next = next_item;
                }
                else
                {
                    list_instance->head = next_item;
                }

                if (current_item == list_instance->tail)
                {
                    list_instance->tail = previous_item;
                }

                free(current_item);
            }
            /* Codes_SRS_LIST_09_005: [ If the condition function returns false, singlylinkedlist_find shall consider that item as not to be removed. ] */
            else
            {
                previous_item = current_item;
            }

            /* Codes_SRS_LIST_09_006: [ If the condition function returns continue_processing as false, singlylinkedlist_remove_if shall stop iterating through the list and return. ] */
            if (continue_processing == false)
            {
                break;
            }

            current_item = next_item;
        }

        /* Codes_SRS_LIST_09_007: [ If no errors occur, singlylinkedlist_remove_if shall return zero. ] */
        result = 0;
    }

    return result;
}

int singlylinkedlist_foreach(SINGLYLINKEDLIST_HANDLE list, LIST_ACTION_FUNCTION action_function, const void* action_context)
{
    int result;

    /* Codes_SRS_LIST_09_008: [ If the list or the action_function argument is NULL, singlylinkedlist_foreach shall return non-zero value. ] */
    if ((list == NULL) ||
        (action_function == NULL))
    {
        LogError("Invalid argument (list=%p, action_function=%p)", list, action_function);
        result = MU_FAILURE;
    }
    else
    {
        LIST_INSTANCE* list_instance = (LIST_INSTANCE*)list;
        LIST_ITEM_INSTANCE* list_item = list_instance->head;

        while (list_item != NULL)
        {
            bool continue_processing = false;

            /* Codes_SRS_LIST_09_009: [ singlylinkedlist_foreach shall iterate through all items in a list and invoke action_function for each one of them. ] */
            action_function(list_item->item, action_context, &continue_processing);

            /* Codes_SRS_LIST_09_010: [ If the condition function returns continue_processing as false, singlylinkedlist_foreach shall stop iterating through the list and return. ] */
            if (continue_processing == false)
            {
                break;
            }

            list_item = (LIST_ITEM_INSTANCE*)list_item->next;
        }

        /* Codes_SRS_LIST_09_011: [ If no errors occur, singlylinkedlist_foreach shall return zero. ] */
        result = 0;
    }

    return result;
}

LIST_ITEM_HANDLE singlylinkedlist_add_head(SINGLYLINKEDLIST_HANDLE list, const void* item)
{
    LIST_ITEM_HANDLE result;

    /* Codes_SRS_LIST_02_001: [ If list is NULL then singlylinkedlist_add_head shall fail and return NULL. ]*/
    if (list == NULL)
    {
        LogError("Invalid argument SINGLYLINKEDLIST_HANDLE list=%p", list);
        result = NULL;
    }
    else
    {
        result = malloc(sizeof(LIST_ITEM_INSTANCE));

        if (result == NULL)
        {
            /*Codes_SRS_LIST_02_003: [ If there are any failures then singlylinkedlist_add_head shall fail and return NULL. ]*/
            LogError("failure in malloc");
            /*return as is*/
        }
        else
        {
            /*Codes_SRS_LIST_02_002: [ singlylinkedlist_add_head shall insert item at head, succeed and return a non-NULL value. ]*/
            result->item = item;
            if (list->head == NULL)
            {
                result->next = NULL;
                list->head = result;
                list->tail = result;
                
            }
            else
            {
                result->next = list->head;
                list->head = result;
            }
        }
    }

    return result;
}
