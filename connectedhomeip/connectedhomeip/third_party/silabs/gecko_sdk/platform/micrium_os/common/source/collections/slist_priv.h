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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _SLIST_PRIV_H_
#define  _SLIST_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/lib_utils.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef struct slist_member SLIST_MEMBER;

struct slist_member {
  SLIST_MEMBER *p_next;
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  OFFSET_OF(type, member)                      ((CPU_SIZE_T)&(((type *)0)->member))

#define  SLIST_FOR_EACH(list_head, iterator)          for ((iterator) = (list_head); (iterator) != DEF_NULL; (iterator) = (iterator)->p_next)

#define  SLIST_FOR_EACH_ENTRY(list_head, entry, type, member) for (  (entry) = SLIST_ENTRY(list_head, type, member);         \
                                                                     (type *)(entry) != SLIST_ENTRY(DEF_NULL, type, member); \
                                                                     (entry) = SLIST_ENTRY((entry)->member.p_next, type, member))

#if (RTOS_TOOLCHAIN_NAME == RTOS_TOOLCHAIN_WIN32)
#undef  SLIST_ENTRY
#endif
#define  SLIST_ENTRY                                  CONTAINER_OF

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void SList_Init(SLIST_MEMBER **p_head_ptr);

void SList_Push(SLIST_MEMBER **p_head_ptr,
                SLIST_MEMBER *p_item);

void SList_PushBack(SLIST_MEMBER **p_head_ptr,
                    SLIST_MEMBER *p_item);

SLIST_MEMBER *SList_Pop(SLIST_MEMBER **p_head_ptr);

void SList_Add(SLIST_MEMBER *p_item,
               SLIST_MEMBER *p_pos);

void SList_Rem(SLIST_MEMBER **p_head_ptr,
               SLIST_MEMBER *p_item);

void SList_Sort(SLIST_MEMBER **p_head_ptr,
                CPU_BOOLEAN (*cmp_fnct)(SLIST_MEMBER *p_item_l, SLIST_MEMBER *p_item_r));

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of private slist module include.
