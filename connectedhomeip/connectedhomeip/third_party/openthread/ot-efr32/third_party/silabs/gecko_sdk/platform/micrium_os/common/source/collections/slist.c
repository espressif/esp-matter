/***************************************************************************//**
 * @file
 * @brief Common - Singly-Linked Lists (Slist)
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/source/collections/slist_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (COMMON, SLIST)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_COMMON

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               SList_Init()
 *
 * @brief    Initializes a singly-linked list.
 *
 * @param    p_head_ptr  Pointer to pointer of head element of list.
 *******************************************************************************************************/
void SList_Init(SLIST_MEMBER **p_head_ptr)
{
  *p_head_ptr = DEF_NULL;
}

/****************************************************************************************************//**
 *                                               SList_Push()
 *
 * @brief    Add given item at beginning of list.
 *
 * @param    p_head_ptr  Pointer to pointer of head element of list.
 *
 * @param    p_item      Pointer to item to add.
 *******************************************************************************************************/
void SList_Push(SLIST_MEMBER **p_head_ptr,
                SLIST_MEMBER *p_item)
{
  p_item->p_next = *p_head_ptr;
  *p_head_ptr = p_item;
}

/****************************************************************************************************//**
 *                                               SList_PushBack()
 *
 * @brief    Add item at end of list.
 *
 * @param    p_head_ptr  Pointer to pointer of head element of list.
 *
 * @param    p_item      Pointer to item to add.
 *******************************************************************************************************/
void SList_PushBack(SLIST_MEMBER **p_head_ptr,
                    SLIST_MEMBER *p_item)
{
  SLIST_MEMBER **p_next_ptr = p_head_ptr;

  while (*p_next_ptr != DEF_NULL) {
    p_next_ptr = &((*p_next_ptr)->p_next);
  }

  p_item->p_next = DEF_NULL;
  *p_next_ptr = p_item;
}

/****************************************************************************************************//**
 *                                               SList_Pop()
 *
 * @brief    Removes and returns first element of list.
 *
 * @param    p_head_ptr  Pointer to pointer of head element of list.
 *
 * @return   Pointer to item that was at top of the list.
 *******************************************************************************************************/
SLIST_MEMBER *SList_Pop(SLIST_MEMBER **p_head_ptr)
{
  SLIST_MEMBER *p_item;

  p_item = *p_head_ptr;
  if (p_item == DEF_NULL) {
    return (DEF_NULL);
  }

  *p_head_ptr = p_item->p_next;

  p_item->p_next = DEF_NULL;

  return (p_item);
}

/****************************************************************************************************//**
 *                                               SList_Add()
 *
 * @brief    Add item after given item.
 *
 * @param    p_item  Pointer to item to add.
 *
 * @param    p_pos   Pointer to item after which the item to add will be inserted.
 *******************************************************************************************************/
void SList_Add(SLIST_MEMBER *p_item,
               SLIST_MEMBER *p_pos)
{
  p_item->p_next = p_pos->p_next;
  p_pos->p_next = p_item;
}

/****************************************************************************************************//**
 *                                               SList_Rem()
 *
 * @brief    Remove item from list.
 *
 * @param    p_head_ptr  Pointer to pointer of head element of list.
 *
 * @param    p_item      Pointer to item to remove.
 *
 * @note     (1) An ASSERT_FAIL_END_CALL is thrown if the item is not found within the list.
 *******************************************************************************************************/
void SList_Rem(SLIST_MEMBER **p_head_ptr,
               SLIST_MEMBER *p_item)
{
  SLIST_MEMBER **p_next_ptr;

  for (p_next_ptr = p_head_ptr; *p_next_ptr != DEF_NULL; p_next_ptr = &((*p_next_ptr)->p_next)) {
    if (*p_next_ptr == p_item) {
      *p_next_ptr = p_item->p_next;
      return;
    }
  }

  RTOS_DBG_FAIL_EXEC(RTOS_ERR_NOT_FOUND,; );                    // See Note #1.
}

/****************************************************************************************************//**
 *                                               SList_Sort()
 *
 * @brief    Sorts list items.
 *
 * @param    p_head_ptr  Pointer to pointer of head element of list.
 *
 * @param    cmp_fnct    Pointer to function to use for sorting the list.
 *                       p_item_l    Pointer to left  item.
 *                       p_item_r    Pointer to right item.
 *                       Returns whether the two items are ordered (DEF_YES) or not (DEF_NO).
 *******************************************************************************************************/
void SList_Sort(SLIST_MEMBER **p_head_ptr,
                CPU_BOOLEAN (*cmp_fnct)(SLIST_MEMBER *p_item_l,
                                        SLIST_MEMBER *p_item_r))
{
  CPU_BOOLEAN  swapped;
  SLIST_MEMBER **pp_item_l;

  do {
    swapped = DEF_NO;

    pp_item_l = p_head_ptr;
    //                                                             Loop until end of list is found.
    while ((*pp_item_l != DEF_NULL) && ((*pp_item_l)->p_next != DEF_NULL)) {
      SLIST_MEMBER *p_item_r = (*pp_item_l)->p_next;
      CPU_BOOLEAN  ordered;

      ordered = cmp_fnct(*pp_item_l, p_item_r);                 // Call provided compare fnct.
      if (ordered == DEF_NO) {                                  // If order is not correct, swap items.
        SLIST_MEMBER *p_tmp = p_item_r->p_next;

        p_item_r->p_next = *pp_item_l;                          // Swap the two items.
        (*pp_item_l)->p_next = p_tmp;
        *pp_item_l = p_item_r;
        pp_item_l = &(p_item_r->p_next);
        swapped = DEF_YES;                                      // Indicate a swap has been done.
      } else {
        pp_item_l = &((*pp_item_l)->p_next);
      }
    }
  } while (swapped == DEF_YES);                                 // Re-loop until no items have been swapped.
}
