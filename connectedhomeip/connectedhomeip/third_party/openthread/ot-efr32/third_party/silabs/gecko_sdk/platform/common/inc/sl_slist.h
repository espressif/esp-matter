/*******************************************************************************
 * @file
 * @brief Single Link List.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#ifndef SL_SLIST_H
#define SL_SLIST_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * @addtogroup slist Singly-Linked List
 * @brief Singly-linked List module provides APIs to handle singly-linked list
 *        operations such as insert, push, pop, push back, sort and remove.
 *
 * @note The pop operation follows FIFO method.
 * @n @section slist_usage Singly-Linked List module Usage
 * @{
 ******************************************************************************/

/// List node type
typedef struct sl_slist_node sl_slist_node_t;

/// List node
struct sl_slist_node {
  sl_slist_node_t *node; ///< List node
};

#ifndef DOXYGEN
#define  container_of(ptr, type, member)  (type *)((uintptr_t)(ptr) - ((uintptr_t)(&((type *)0)->member)))

#define  SL_SLIST_ENTRY                               container_of

#define  SL_SLIST_FOR_EACH(list_head, iterator)       for ((iterator) = (list_head); (iterator) != NULL; (iterator) = (iterator)->node)

#define  SL_SLIST_FOR_EACH_ENTRY(list_head, entry, type, member) for (  (entry) = SL_SLIST_ENTRY(list_head, type, member);     \
                                                                        (type *)(entry) != SL_SLIST_ENTRY(NULL, type, member); \
                                                                        (entry) = SL_SLIST_ENTRY((entry)->member.node, type, member))
#endif

// -----------------------------------------------------------------------------
// Prototypes

/*******************************************************************************
 * Initialize a singly-linked list.
 *
 * @param    head  Pointer to pointer of head element of list.
 ******************************************************************************/
void sl_slist_init(sl_slist_node_t **head);

/*******************************************************************************
 * Add given item at beginning of the list.
 *
 * @param    head  Pointer to pointer of head element of the list.
 *
 * @param    item  Pointer to an item to add.
 ******************************************************************************/
void sl_slist_push(sl_slist_node_t **head,
                   sl_slist_node_t *item);

/*******************************************************************************
 * Add item at the end of the list.
 *
 * @param    head  Pointer to the pointer of a head element of the list.
 *
 * @param    item  Pointer to the item to add.
 ******************************************************************************/
void sl_slist_push_back(sl_slist_node_t **head,
                        sl_slist_node_t *item);

/*******************************************************************************
 * Remove and return the first element of the list.
 *
 * @param    head  Pointer to he pointer of the head element of the list.
 *
 * @return   Pointer to item that was at top of the list.
 ******************************************************************************/
sl_slist_node_t *sl_slist_pop(sl_slist_node_t **head);

/*******************************************************************************
 * Insert an item after the given item.
 *
 * @param    item  Pointer to an item to add.
 *
 * @param    pos   Pointer to an item after which the item to add will be inserted.
 ******************************************************************************/
void sl_slist_insert(sl_slist_node_t *item,
                     sl_slist_node_t *pos);

/*******************************************************************************
 * Remove an item from the list.
 *
 * @param    head  Pointer to pointer of the head element of list.
 *
 * @param    item  Pointer to the item to remove.
 *
 * @note     (1) An EFM_ASSERT is thrown if the item is not found within the list.
 ******************************************************************************/
void sl_slist_remove(sl_slist_node_t **head,
                     sl_slist_node_t *item);

/*******************************************************************************
 * Sort list items.
 *
 * @param    head      Pointer to the pointer of the head element of the list.
 *
 * @param    cmp_fnct  Pointer to function to use for sorting the list.
 *                     item_l    Pointer to left  item.
 *                     item_r    Pointer to right item.
 *                     Returns whether the two items are ordered (true) or not (false).
 ******************************************************************************/
void sl_slist_sort(sl_slist_node_t **head,
                   bool (*cmp_fnct)(sl_slist_node_t *item_l,
                                    sl_slist_node_t *item_r));

/** @} (end addtogroup slist) */

#ifdef __cplusplus
}
#endif

#endif /* SL_SLIST_H */
