/******************************************************************************

 @file  osal_list.c

 @brief This implements a doubly linked list module for the stack

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

/*********************************************************************
 * INCLUDES
 */

#include "osal_list.h"

/*
 *  ======== osal_list_clearList ========
 */
void osal_list_clearList(osal_list_list *list)
{
  list->head = list->tail = NULL;
}

/*
 *  ======== osal_list_empty ========
 */
bool osal_list_empty(osal_list_list *list)
{
  return (list->head == NULL);
}

/*
 *  ======== osal_list_get ========
 */
osal_list_elem *osal_list_get(osal_list_list *list)
{
  osal_list_elem *elem;

  elem = list->head;

  /* See if the List was empty */
  if(elem != NULL)
  {
    list->head = elem->next;

    if(elem->next != NULL)
    {
      elem->next->prev = NULL;
    }
    else
    {
      list->tail = NULL;
    }
  }

  return (elem);
}

/*
 *  ======== osal_list_head ========
 */
osal_list_elem *osal_list_head(osal_list_list *list)
{
  return (list->head);
}

/*
 *  ======== osal_list_insert ========
 */
void osal_list_insert(osal_list_list *list, osal_list_elem *newElem,
                     osal_list_elem *curElem)
{
  newElem->next = curElem;
  newElem->prev = curElem->prev;

  if(curElem->prev != NULL)
  {
    curElem->prev->next = newElem;
  }
  else
  {
    list->head = newElem;
  }

  curElem->prev = newElem;
}

/*
 *  ======== osal_list_next ========
 */
osal_list_elem *osal_list_next(osal_list_elem *elem)
{
  return (elem->next);
}

/*
 *  ======== osal_list_prev ========
 */
osal_list_elem *osal_list_prev(osal_list_elem *elem)
{
  return (elem->prev);
}

/*
 *  ======== osal_list_put ========
 */
void osal_list_put(osal_list_list *list, osal_list_elem *elem)
{
  elem->next = NULL;
  elem->prev = list->tail;

  if(list->tail != NULL)
  {
    list->tail->next = elem;
  }
  else
  {
    list->head = elem;
  }

  list->tail = elem;
}

/*
 *  ======== osal_list_putHead ========
 */
void osal_list_putHead(osal_list_list *list, osal_list_elem *elem)
{
  elem->next = list->head;
  elem->prev = NULL;

  if(list->head != NULL)
  {
    list->head->prev = elem;
  }
  else
  {
    list->tail = elem;
  }

  list->head = elem;
}

/*
 *  ======== osal_list_remove ========
 */
void osal_list_remove(osal_list_list *list, osal_list_elem *elem)
{
  /* Handle the case where the elem to remove is the last one */
  if(elem->next == NULL)
  {
    list->tail = elem->prev;
  }
  else
  {
    elem->next->prev = elem->prev;
  }

  /* Handle the case where the elem to remove is the first one */
  if(elem->prev == NULL)
  {
    list->head = elem->next;
  }
  else
  {
    elem->prev->next = elem->next;
  }
}

/*
 *  ======== osal_list_tail ========
 */
osal_list_elem *osal_list_tail(osal_list_list *list)
{
  return (list->tail);
}

/*********************************************************************
 */
