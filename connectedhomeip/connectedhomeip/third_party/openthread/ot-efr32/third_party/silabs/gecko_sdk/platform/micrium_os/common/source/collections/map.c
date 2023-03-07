/***************************************************************************//**
 * @file
 * @brief Common - Map Interface
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

#include  <common/source/collections/map_priv.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>

#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (COMMON, COLLECTIONS, MAP)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_COMMON

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_BOOLEAN MapItemExists(MAP_INSTANCE *p_map_instance,
                                 MAP_ITEM     *p_map_item);

static CPU_BOOLEAN MapKeyExists(MAP_INSTANCE *p_map_instance,
                                CPU_CHAR     *key);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                                   MapInit()
 *
 * @brief    Initializes map instance object.
 *
 * @param    p_map_instance  Pointer to map instance object.
 *******************************************************************************************************/
void MapInit(MAP_INSTANCE *p_map_instance)
{
  RTOS_ASSERT_DBG((p_map_instance != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  SList_Init((SLIST_MEMBER **)p_map_instance);
}

/****************************************************************************************************//**
 *                                               MapItemAdd()
 *
 * @brief    Add item to map instance.
 *
 * @param    p_map_instance  Pointer to map instance object.
 *
 * @param    p_map_item      Pointer to map item to add to map instance.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_ALREADY_EXISTS
 *******************************************************************************************************/
void MapItemAdd(MAP_INSTANCE *p_map_instance,
                MAP_ITEM     *p_map_item,
                RTOS_ERR     *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_map_instance != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_map_item != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  if (MapItemExists(p_map_instance, p_map_item)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_EXISTS);
    return;
  }

  if (MapKeyExists(p_map_instance, p_map_item->Key)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_EXISTS);
    return;
  }

  SList_Push((SLIST_MEMBER **)p_map_instance,
             &p_map_item->ListNode);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return;
}

/****************************************************************************************************//**
 *                                               MapItemRemove()
 *
 * @brief    Remove map item from map instance.
 *
 * @param    p_map_instance  Pointer to map instance object.
 *
 * @param    p_map_item      Pointer to map item to remove from map instance.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_FOUND
 *******************************************************************************************************/
void MapItemRemove(MAP_INSTANCE *p_map_instance,
                   MAP_ITEM     *p_map_item,
                   RTOS_ERR     *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_map_instance != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_map_item != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  if (MapItemExists(p_map_instance, p_map_item)) {
    SList_Rem((SLIST_MEMBER **)p_map_instance, &(p_map_item->ListNode));
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
  }

  return;
}

/****************************************************************************************************//**
 *                                               MapKeyRemove()
 *
 * @brief    Remove map item associated to 'key'.
 *
 * @param    p_map_instance  Pointer to map instance object.
 *
 * @param    key             String containing key of map item to remove.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_FOUND
 *******************************************************************************************************/
void MapKeyRemove(MAP_INSTANCE *p_map_instance,
                  CPU_CHAR     *key,
                  RTOS_ERR     *p_err)
{
  MAP_ITEM *p_remove_item;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_map_instance != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((key != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_remove_item = MapKeyItemGet(p_map_instance, key, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    SList_Rem((SLIST_MEMBER **)p_map_instance, &(p_remove_item->ListNode));
  }

  return;
}

/****************************************************************************************************//**
 *                                               MapKeyItemGet()
 *
 * @brief    Obtain pointer to map item with corresponding 'key'.
 *
 * @param    p_map_instance  Pointer to map instance object.
 *
 * @param    key             String containing key of map item to obtain.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_FOUND
 *
 * @return   Pointer to map item, if item exists,
 *           DEF_NULL, otherwise.
 *
 * @note     (1) Item is not removed from list at this point. This function merely returns a pointer
 *               to the requested map item.
 *******************************************************************************************************/
MAP_ITEM *MapKeyItemGet(MAP_INSTANCE *p_map_instance,
                        CPU_CHAR     *key,
                        RTOS_ERR     *p_err)
{
  MAP_ITEM *p_map_item_ret = DEF_NULL;
  MAP_ITEM *p_map_item_iter;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  RTOS_ASSERT_DBG_ERR_SET((p_map_instance != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);
  RTOS_ASSERT_DBG_ERR_SET((key != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
  if (*p_map_instance != DEF_NULL) {
    SLIST_FOR_EACH_ENTRY(*p_map_instance, p_map_item_iter, MAP_ITEM, ListNode) {
      if (Str_Cmp_N(p_map_item_iter->Key, key, DEF_INT_08U_MAX_VAL) == 0) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        p_map_item_ret = p_map_item_iter;
        break;
      }
    }
  }

  return (p_map_item_ret);
}

/****************************************************************************************************//**
 *                                               MapKeyValueGet()
 *
 * @brief    Obtain value associated to the map item corresponding to the 'key' passed.
 *
 * @param    p_map_instance  Pointer to map instance object.
 *
 * @param    key             String containing key of map item to obtain value from.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_FOUND
 *
 * @return   Value to map item, if item exists,
 *           DEF_NULL, otherwise.
 *******************************************************************************************************/
void *MapKeyValueGet(MAP_INSTANCE *p_map_instance,
                     CPU_CHAR     *key,
                     RTOS_ERR     *p_err)
{
  MAP_ITEM *p_ret_item;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);

  RTOS_ASSERT_DBG_ERR_SET((p_map_instance != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);
  RTOS_ASSERT_DBG_ERR_SET((key != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);

  p_ret_item = MapKeyItemGet(p_map_instance, key, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  return (p_ret_item->Value);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               MapItemExists()
 *
 * @brief    Checks if a map item exists in map instance.
 *
 * @param    p_map_instance  Pointer to map instance object.
 *
 * @param    p_map_item      Pointer to map item to search for.
 *
 * @return   DEF_YES, if 'p_map_item' points to an existing item in map instance,
 *           DEF_NO, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN MapItemExists(MAP_INSTANCE *p_map_instance,
                                 MAP_ITEM     *p_map_item)
{
  MAP_ITEM    *p_map_item_iter;
  CPU_BOOLEAN found = DEF_NO;

  if (*p_map_instance != DEF_NULL) {
    SLIST_FOR_EACH_ENTRY(*p_map_instance, p_map_item_iter, MAP_ITEM, ListNode) {
      if (p_map_item_iter == p_map_item) {
        found = DEF_YES;
      }
    }
  }

  return (found);
}

/****************************************************************************************************//**
 *                                               MapKeyExists()
 *
 * @brief    Checks if a map item's key exists in map instance.
 *
 * @param    p_map_instance  Pointer to map instance object.
 *
 * @param    key             String containing key to search for in map instance.
 *
 * @return   DEF_YES, if 'key' exists in map instance,
 *           DEF_NO, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN MapKeyExists(MAP_INSTANCE *p_map_instance,
                                CPU_CHAR     *key)
{
  MAP_ITEM    *p_map_item_iter;
  CPU_BOOLEAN found = DEF_NO;

  if (*p_map_instance != DEF_NULL) {
    SLIST_FOR_EACH_ENTRY(*p_map_instance, p_map_item_iter, MAP_ITEM, ListNode) {
      if (Str_Cmp_N(p_map_item_iter->Key, key, DEF_INT_08U_MAX_VAL) == 0) {
        found = DEF_YES;
      }
    }
  }

  return (found);
}
