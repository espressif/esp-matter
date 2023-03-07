/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */
#ifndef _SLIST_H_
#define _SLIST_H_

/*******************************************************************************
*
* File Name: slist.h
*
* Abstract:  This file constains utilities of generic single linked list
*
* Functions:
*
*******************************************************************************/

/*
 * overview of the generic single linked list structure:

            +------+
list_head:  | next |--------------------------------------+
            +------+                                      |
                                                          |
                                                          |
                                                          |
                                                          v
        +-----------+       +-----------+           +-----------+
list:   | item1     |       | item2     |           | itemN     |
        |           |       |           |           |           |
        |           |       |           |           |           |
        +-----------+       +-----------+           +-----------+
 +----->|   next    |------>|   next    |   ...  -->|   next    |-------+
 |      +-----------+       +-----------+           +-----------+       |
 |      |           |       |           |           |           |       |
 |      |           |       |           |           |           |       |
 |      +-----------+       +-----------+           +-----------+       |
 |                                                                      |
 |                                                                      |
 |                                                                      |
 +----------------------------------------------------------------------+

 The reason that the list_head points to the last item of the list instead
 of first item is for better performance to insert an item at the tail.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

//#include "types.h"

#pragma pack(push, 4)

typedef struct slist_node_t {
	struct slist_node_t *next;
} slist_node_t;

#define SLIST_NODE_INIT(name) { 0 }

#define SLIST_NODE(name) \
	struct slist_node_t name = SLIST_NODE_INIT(name)

#define INIT_SLIST_NODE(ptr) do { \
	(ptr)->next = 0; \
} while (0)

/**
 * slist_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry at the beginning.
 * This is good for implementing stacks.
 */
void slist_add_front(struct slist_node_t *_new, struct slist_node_t *head);

/**
 * slist_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry at the tail.
 * This is useful for implementing queues.
 */
void slist_add_tail(struct slist_node_t *_new, struct slist_node_t *head);

/**
 * slist_add_before - add a new entry
 * @new: new entry to be added
 * @node: the node that will be pointed by new->next.
 * @head: list head to add it after
 *
 * Insert a new entry in front of existing node
 */
void slist_add_before(struct slist_node_t *_new, struct slist_node_t *node, struct slist_node_t *head);

/**
 * slist_add_after - add a new entry
 * @new: new entry to be added
 * @node: node->next will be new node
 * @head: list head to add it after
 *
 * Insert a new entry after an existing node
 */
void slist_add_after(struct slist_node_t *_new, struct slist_node_t *node, struct slist_node_t *head);

/**
 * slist_del_front - deletes the first entry from list.
 * @head: list head where the entry is
 */
void slist_del_front(struct slist_node_t* head);

/**
 * slist_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * @head: list head where the entry is
 */
void slist_del(struct slist_node_t *entry, struct slist_node_t* head);

/**
 * slist_empty - tests whether a list is empty
 * @head: the list to test.
 */
int slist_empty(struct slist_node_t *head);

/**
 * slist_front - return the front node pointer
 * @head: the list.
 */
struct slist_node_t* slist_front(struct slist_node_t *head);

/**
 * slist_tail - return the tail node pointer
 * @head: the list
 */
struct slist_node_t* slist_tail(struct slist_node_t *head);

/**
 * slist_get - remove the first entry from the list
 * @head: list head where the entry is
 */
struct slist_node_t* slist_get(struct slist_node_t* head);

/**
 * slist_entry - get the struct for this entry
 * @ptr:	the &struct dlist_node_t pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the dlist_node_t within the struct.
 */
#define slist_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))



/**
 * slist_count - get the number of items
 * @head: the list
 */
int slist_count(struct slist_node_t *head);

#pragma pack(pop)
#ifdef __cplusplus
}
#endif


#endif // end of #ifndef _SLIST_H_
