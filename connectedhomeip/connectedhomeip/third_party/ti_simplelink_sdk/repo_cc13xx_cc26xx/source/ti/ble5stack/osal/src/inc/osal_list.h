/******************************************************************************

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2004-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/**
 *  @file  osal_list.h
 *  @brief      Linked List interface for use in the stack
 *
 *  This module provides simple doubly-link list implementation. There are two
 *  main structures:
 *     - ::osal_list_list: The structure that holds the start of a linked list. There
 *  is no API to create one. It is up to the user to provide the structure
 *  itself.
 *     - ::osal_list_elem: The structure that must be in the structure that is placed
 *  onto a linked list. Generally it is the first field in the structure. For
 *  example:
 *  @code
 *  typedef struct MyStruct {
 *      osal_list_elem elem;
 *      void *buffer;
 *  } MyStruct;
 *  @endcode
 *
 *  The following shows how to create a linked list with three elements.
 *
 *  @code
 *  + denotes null-terminated
 *          _______        _______        _______      _______
 *         |_______|----->|_______|----->|_______|--->|_______|--//---,
 *    ,----|_______|    ,-|_______|<-----|_______|<---|_______|<-//-, +
 *    |      List       +   elem           elem          elem       |
 *    |_____________________________________________________________|
 *  @endcode
 *
 *  Initializing and adding an element to the tail and removing it
 *  @code
 *  typedef struct MyStruct {
 *      osal_list_elem elem;
 *      void *buffer;
 *  } MyStruct;
 *
 *  osal_list_list list;
 *  MyStruct foo;
 *  MyStruct *bar;
 *
 *  osal_list_clearList(&list);
 *  osal_list_put(&list, (osal_list_elem *)&foo);
 *  bar = (MyStruct *)osal_list_get(&list);
 *  @endcode
 *
 *  The ::osal_list_put and ::osal_list_get APIs are used to maintain a first-in first-out
 *  (FIFO) linked list.
 *
 *  The ::osal_list_putHead and ::osal_list_get APIs are used to maintain a last-in first-out
 *  (LIFO) linked list.
 *
 *  Traversing a list from head to tail. Note: thread-safety calls are
 *  not shown here.
 *  @code
 *  osal_list_list list;
 *  osal_list_elem *temp;
 *
 *  for (temp = osal_list_head(&list); temp != NULL; temp = osal_list_next(temp)) {
 *       printf("address = 0x%x\n", temp);
 *  }
 *  @endcode
 *
 *  Traversing a list from tail to head. Note: thread-safety calls are
 *  not shown here.
 *  @code
 *  osal_list_list list;
 *  osal_list_elem *temp;
 *
 *  for (temp = osal_list_tail(&list); temp != NULL; temp = osal_list_prev(temp)) {
 *       printf("address = 0x%x\n", temp);
 *  }
 *  @endcode
 *
 *  ============================================================================
 */

#ifndef OSAL_LIST_H
#define OSAL_LIST_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

#include "comdef.h"

/*********************************************************************
 * TYPEDEFS
 */
typedef struct osal_list_elem {
    struct osal_list_elem *next;
    struct osal_list_elem *prev;
} osal_list_elem;

typedef struct osal_list_list {
    osal_list_elem *head;
    osal_list_elem *tail;
} osal_list_list;

/*********************************************************************
 * FUNCTIONS
 */


/**
 *  @brief  Function to initialize the contents of an osal_list_list
 *
 *  @param  list Pointer to an osal_list_list structure that will be used to
 *               maintain a linked list
 */
extern void osal_list_clearList(osal_list_list *list);

/**
 *  @brief  Function to test whether a linked list is empty
 *
 *  @param  list A pointer to a linked list
 *
 *  @return true if empty, false if not empty
 */
extern bool osal_list_empty(osal_list_list *list);

/**
 *  @brief  Function to atomically get the first elem in a linked list
 *
 *  @param  list A pointer to a linked list
 *
 *  @return Pointer the first elem in the linked list or NULL if empty
 */
extern osal_list_elem *osal_list_get(osal_list_list *list);

/**
 *  @brief  Function to return the head of a linked list
 *
 *  This function does not remove the head, it simply returns a pointer to
 *  it. This function is typically used when traversing a linked list.
 *
 *  @param  list A pointer to the linked list
 *
 *  @return Pointer to the first elem in the linked list or NULL if empty
 */
extern osal_list_elem *osal_list_head(osal_list_list *list);

/**
 *  @brief  Function to insert an elem into a linked list
 *
 *  @param  list A pointer to the linked list
 *
 *  @param  newElem New elem to insert
 *
 *  @param  curElem Elem to insert the newElem in front of.
 *          This value cannot be NULL.
 */
extern void osal_list_insert(osal_list_list *list, osal_list_elem *newElem,
                            osal_list_elem *curElem);

/**
 *  @brief  Function to return the next elem in a linked list
 *
 *  This function does not remove the elem, it simply returns a pointer to
 *  next one. This function is typically used when traversing a linked list.
 *
 *  @param  elem Elem in the list
 *
 *  @return Pointer to the next elem in linked list or NULL if at the end
 */
extern osal_list_elem *osal_list_next(osal_list_elem *elem);

/**
 *  @brief  Function to return the previous elem in a linked list
 *
 *  This function does not remove the elem, it simply returns a pointer to
 *  previous one. This function is typically used when traversing a linked list.
 *
 *  @param  elem Elem in the list
 *
 *  @return Pointer to the previous elem in linked list or NULL if at the
 *  beginning
 */
extern osal_list_elem *osal_list_prev(osal_list_elem *elem);

/**
 *  @brief  Function to put an elem onto the end of a linked list
 *
 *  @param  list A pointer to the linked list
 *
 *  @param  elem Element to place onto the end of the linked list
 */
extern void osal_list_put(osal_list_list *list, osal_list_elem *elem);

/**
 *  @brief  Function to put an elem onto the head of a linked list
 *
 *  @param  list A pointer to the linked list
 *
 *  @param  elem Element to place onto the beginning of the linked list
 */
extern void osal_list_putHead(osal_list_list *list, osal_list_elem *elem);

/**
 *  @brief  Function to remove an elem from a linked list
 *
 *  @param  list A pointer to the linked list
 *
 *  @param  elem Element to be removed from a linked list
 */
extern void osal_list_remove(osal_list_list *list, osal_list_elem *elem);

/**
 *  @brief  Function to return the tail of a linked list
 *
 *  This function does not remove the tail, it simply returns a pointer to
 *  it. This function is typically used when traversing a linked list.
 *
 *  @param  list A pointer to the linked list
 *
 *  @return Pointer to the last elem in the linked list or NULL if empty
 */
extern osal_list_elem *osal_list_tail(osal_list_list *list);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* OSAL_LIST_H */
