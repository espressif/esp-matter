/***************************************************************************//**
 * @file
 * @brief CANopen Parameters Object Management
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_CANOPEN_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <canopen_cfg.h>

#if (CANOPEN_OBJ_PARAM_EN == DEF_ENABLED)
#include  <cpu/include/cpu.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/rtos_err.h>

#include  <canopen/include/canopen_dict.h>
#include  <canopen/source/core/canopen_core_priv.h>
#include  <canopen/source/core/canopen_dict_priv.h>
#include  <canopen/source/core/canopen_param_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (CAN, CANOPEN)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_CANOPEN

#define CANOPEN_PARAM_STORE_SIGNATURE    0x65766173             // Signature for storing parameter
#define CANOPEN_PARAM_RESTORE_SIGNATURE  0x64616F6c             // Signature for restoring parameter

#define CANOPEN_PARAM_SUBIX0_HIGHEST_SUB_IX     0x7F

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void CANopen_ParamCheck(CANOPEN_NODE_HANDLE node_handle,
                               CANOPEN_OBJ         *p_obj,
                               void                *p_buf,
                               CPU_INT32U          size,
                               RTOS_ERR            *p_err);

static void CANopen_ParamRd(CANOPEN_NODE_HANDLE node_handle,
                            CANOPEN_OBJ         *p_obj,
                            void                *p_buf,
                            CPU_INT32U          size,
                            RTOS_ERR            *p_err);

static void CANopen_ParamWr(CANOPEN_NODE_HANDLE node_handle,
                            CANOPEN_OBJ         *p_obj,
                            void                *p_buf,
                            CPU_INT32U          size,
                            RTOS_ERR            *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       Object type parameter
 *
 * @note     (1) This object type specializes the general handling of objects for the object dictionary
 *               entry 0x1010 and 0x1011. These entries are designed to provide the read and write feature
 *               of a configurable parameter group.
 *******************************************************************************************************/

const CANOPEN_OBJ_TYPE CANopen_ObjTypeParam = {
  .SizeCb = DEF_NULL,
  .CtrlCb = DEF_NULL,
  .RdCb = CANopen_ParamRd,
  .WrCb = CANopen_ParamWr
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************
 *                                       CANopen_ParamStore()
 *
 * @brief    Stores activities of the given parameter group.
 *
 * @param    p_pg    Pointer to parameter group information.
 *
 * @param    p_node  Pointer to node information.
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) The whole parameter group will be stored in non-volatile memory by calling the
 *               user callback function ParamOnSave().
 ***************************************************************************************************/
void CANopen_ParamStore(CANOPEN_PARAM *p_pg,
                        CANOPEN_NODE  *p_node,
                        RTOS_ERR      *p_err)
{
  CPU_BOOLEAN result;

  RTOS_ASSERT_DBG_ERR_SET((p_pg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_node != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  if (((p_pg->Val & CANOPEN_PARAM___E) != 0)                    // see, if parameter group is enabled
      && (p_node->EventFnctsPtr != DEF_NULL)
      && (p_node->EventFnctsPtr->ParamOnSave != DEF_NULL)) {
    CANOPEN_NODE_HANDLE handle;

    CANOPEN_NODE_HANDLE_SET(handle, p_node);
    //                                                             call application callback function
    result = p_node->EventFnctsPtr->ParamOnSave(handle, p_pg);

    if (result != DEF_OK) {                                     // see, if user detects an error
      RTOS_ERR_SET(*p_err, RTOS_ERR_CANOPEN_PARAM_STORE);
    }
  }
}

/****************************************************************************************************
 *                                      CANopen_ParamRestore()
 *
 * @brief    Removes the changes on the parameter values of the given parameter group.
 *
 * @param    p_pg    Pointer to parameter group information.
 *
 * @param    p_node  Pointer to node information.
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) The changes in non-volatile memory of the given parameter group will be replaced
 *               with the default values by calling the user callback function ParamOnDflt().
 ***************************************************************************************************/
void CANopen_ParamRestore(CANOPEN_PARAM *p_pg,
                          CANOPEN_NODE  *p_node,
                          RTOS_ERR      *p_err)
{
  CPU_BOOLEAN result;

  RTOS_ASSERT_DBG_ERR_SET((p_pg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_node != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  if (((p_pg->Val & CANOPEN_PARAM___E) != 0)
      && (p_node->EventFnctsPtr->ParamOnDflt != DEF_NULL)
      && (p_node->EventFnctsPtr != DEF_NULL)) {                 // see, if parameter group is enabled
    CANOPEN_NODE_HANDLE handle;

    CANOPEN_NODE_HANDLE_SET(handle, p_node);
    //                                                             call application callback function
    result = p_node->EventFnctsPtr->ParamOnDflt(handle, p_pg);

    if (result != DEF_OK) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_CANOPEN_PARAM_RESTORE);
    }
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        CANopen_ParamCheck()
 *
 * @brief    Checks the access to parameter storage (0x1010) and parameter restore default (0x1011)
 *           object entries. There are some plausibility checks of given parameter and configuration.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_obj           Pointer to addressed object entry.
 *
 * @param    p_buf           Pointer to data buffer.
 *
 * @param    size            Size of given data in buffer.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void CANopen_ParamCheck(CANOPEN_NODE_HANDLE node_handle,
                               CANOPEN_OBJ         *p_obj,
                               void                *p_buf,
                               CPU_INT32U          size,
                               RTOS_ERR            *p_err)
{
  CPU_INT32U  signature;                                        // Local: written signature to the entry
  CPU_INT08U  highest_sub_ix;
  CPU_INT16U  obj_ix;
  CPU_INT08U  obj_sub_ix;
  CPU_BOOLEAN signature_ok = DEF_NO;

  RTOS_ASSERT_DBG_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((size == CANOPEN_OBJ_LONG), *p_err, RTOS_ERR_INVALID_ARG,; );
  RTOS_ASSERT_DBG_ERR_SET(((CANOPEN_OBJ_GET_IX(p_obj->Key) == CANOPEN_COMM_PROFILE_OBJ_IX_STORE_PARAM)
                           || (CANOPEN_OBJ_GET_IX(p_obj->Key) == CANOPEN_COMM_PROFILE_OBJ_IX_RESTORE_DFLT_PARAM)), *p_err, RTOS_ERR_INVALID_IX,; );

  obj_ix = CANOPEN_OBJ_GET_IX(p_obj->Key);

  CANopen_DictByteRd(node_handle,                               // get highest number of parameter group
                     CANOPEN_DEV(obj_ix, 0),
                     &highest_sub_ix,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  if (highest_sub_ix > CANOPEN_PARAM_SUBIX0_HIGHEST_SUB_IX) {   // see, if number is out of range
    RTOS_ERR_SET(*p_err, RTOS_ERR_CANOPEN_INVALID_ENTRY);
    return;
  }

  obj_sub_ix = CANOPEN_OBJ_GET_SUBIX(p_obj->Key);               // get addressed subindex
  if ((obj_sub_ix < 1) || (obj_sub_ix > highest_sub_ix)) {      // see, if addressed subidx is invalid
    RTOS_ERR_SET(*p_err, RTOS_ERR_CANOPEN_INVALID_ENTRY);
    return;
  }

  signature = *((CPU_INT32U *)p_buf);                           // get signature value
  signature_ok = (signature == CANOPEN_PARAM_STORE_SIGNATURE) || (signature == CANOPEN_PARAM_RESTORE_SIGNATURE);
  if (!signature_ok) {                                          // see, if signature is wrong
    RTOS_ERR_SET(*p_err, RTOS_ERR_CANOPEN_INVALID_ENTRY);
    return;
  }
}

/****************************************************************************************************//**
 *                                       CANopen_ParamRd()
 *
 * @brief    Performs all necessary activities during a read access of the parameter object entry.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_obj           Pointer to addressed object entry.
 *
 * @param    p_buf           Pointer to data buffer.
 *
 * @param    size            Size of given data in buffer.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void CANopen_ParamRd(CANOPEN_NODE_HANDLE node_handle,
                            CANOPEN_OBJ         *p_obj,
                            void                *p_buf,
                            CPU_INT32U          size,
                            RTOS_ERR            *p_err)
{
  CANOPEN_PARAM *pg;                                            // Local: ptr to parameter group information

  PP_UNUSED_PARAM(node_handle);

  RTOS_ASSERT_DBG_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((size == CANOPEN_OBJ_LONG), *p_err, RTOS_ERR_INVALID_ARG,; );

  pg = (CANOPEN_PARAM *)p_obj->Data;                            // get configured parameter group
  if (pg == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_OBJ_RD);
  } else {
    *(CPU_INT32U *)p_buf = pg->Val;                             // set result value
  }
}

/****************************************************************************************************//**
 *                                       CANopen_ParamWr()
 *
 * @brief    Performs all necessary activities during a write access of the parameter object entry.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_obj           Pointer to addressed object entry.
 *
 * @param    p_buf           Pointer to data buffer.
 *
 * @param    size            Size of given data in buffer.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void CANopen_ParamWr(CANOPEN_NODE_HANDLE node_handle,
                            CANOPEN_OBJ         *p_obj,
                            void                *p_buf,
                            CPU_INT32U          size,
                            RTOS_ERR            *p_err)
{
  CANOPEN_NODE  *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CANOPEN_OBJ   *pwo;                                           // Local: working ptr to object
  CANOPEN_PARAM *pg;                                            // Local: ptr to parameter group information
  CPU_INT16U    obj_ix;                                         // Local: current working index
  CPU_INT08U    highest_sub_ix;                                 // Local: number of entries in storage idx
  CPU_INT08U    obj_sub_ix;                                     // Local: current working subindex
  RTOS_ERR      local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  CANopen_ParamCheck(node_handle, p_obj, p_buf, size, p_err);   // check parameter and object config
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;                                                     // yes: abort with error indication
  }

  obj_ix = CANOPEN_OBJ_GET_IX(p_obj->Key);                      // get given index
  obj_sub_ix = CANOPEN_OBJ_GET_SUBIX(p_obj->Key);               // get given subindex
                                                                // get number of parameter groups
  CANopen_DictByteRd(node_handle, CANOPEN_DEV(obj_ix, 0), &highest_sub_ix, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    return;
  }

  if ((obj_sub_ix == 1)                                         // see, if special case: access _ALL_ param
      && (highest_sub_ix > 1)) {                                // with more than 1 param group
    for (obj_sub_ix = 2; obj_sub_ix < highest_sub_ix; obj_sub_ix++) {
      pwo = CANopen_DictFind(&(p_node->Dict), CANOPEN_DEV(obj_ix, obj_sub_ix), &local_err);
      if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {      // see, if parameter group is found
        pg = (CANOPEN_PARAM *)pwo->Data;                        // get configured parameter group information
        if (obj_ix == CANOPEN_COMM_PROFILE_OBJ_IX_RESTORE_DFLT_PARAM) {
          CANopen_ParamRestore(pg, p_node, p_err);              // restore parameter
        } else {                                                // otherwise: storing is selected
          CANopen_ParamStore(pg, p_node, p_err);                // store parameter
        }
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return;                                               // If a failure, abort all ongoing stores or restores.
        }
      }
    }
  } else {                                                      // otherwise: save addressed para group
    pg = (CANOPEN_PARAM *)p_obj->Data;                          // get configured parameter group information
    if (obj_ix == CANOPEN_COMM_PROFILE_OBJ_IX_RESTORE_DFLT_PARAM) {
      CANopen_ParamRestore(pg, p_node, p_err);                  // restore parameter
    } else {                                                    // otherwise: storing is selected
      CANopen_ParamStore(pg, p_node, p_err);                    // store parameter
    }
  }
}

#endif // CANOPEN_OBJ_PARAM_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_CANOPEN_AVAIL
