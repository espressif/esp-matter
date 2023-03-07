/***************************************************************************//**
 * @file
 * @brief CANopen Object Dictionary Management
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
 *                                             INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/rtos_err.h>

#include  <canopen/include/canopen_obj.h>
#include  <canopen/include/canopen_types.h>
#include  <canopen/source/core/canopen_core_priv.h>
#include  <canopen/source/core/canopen_dict_priv.h>
#include  <canopen/source/core/canopen_obj_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (CAN, CANOPEN)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_CANOPEN

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void CANopen_DictDataRd(CANOPEN_NODE_HANDLE node_handle,
                               CPU_INT32U          key,
                               CPU_INT32U          *p_val,
                               CPU_INT08U          size,
                               RTOS_ERR            *p_err);

static void CANopen_DictDataWr(CANOPEN_NODE_HANDLE node_handle,
                               CPU_INT32U          key,
                               CPU_INT32U          val,
                               CPU_INT08U          size,
                               RTOS_ERR            *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         CANopen_DictByteRd()
 *
 * @brief    Reads a 8-bit value from the given object dictionary.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    key             Object entry key; should be generated with the macro CANOPEN_DEV().
 *
 * @param    p_val           Pointer to the value destination.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_SIZE_INVALID
 *                               - RTOS_ERR_OBJ_RD
 *
 * @note     (1) The object entry is addressed with the given key and the value to read will be written
 *               to the given destination pointer.
 *******************************************************************************************************/
void CANopen_DictByteRd(CANOPEN_NODE_HANDLE node_handle,
                        CPU_INT32U          key,
                        CPU_INT08U          *p_val,
                        RTOS_ERR            *p_err)
{
  CANopen_DictDataRd(node_handle, key, (CPU_INT32U *)p_val, CANOPEN_OBJ_BYTE, p_err);
}

/****************************************************************************************************//**
 *                                        CANopen_DictWordRd()
 *
 * @brief    Reads a 16-bit value from the given object dictionary.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    key             Object entry key; should be generated with the macro CANOPEN_DEV().
 *
 * @param    p_val           Pointer to the value destination.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_SIZE_INVALID
 *                               - RTOS_ERR_OBJ_RD
 *
 * @note     (1) The object entry is addressed with the given key and the value to read will be written
 *               to the given destination pointer.
 *******************************************************************************************************/
void CANopen_DictWordRd(CANOPEN_NODE_HANDLE node_handle,
                        CPU_INT32U          key,
                        CPU_INT16U          *p_val,
                        RTOS_ERR            *p_err)
{
  CANopen_DictDataRd(node_handle, key, (CPU_INT32U *)p_val, CANOPEN_OBJ_WORD, p_err);
}

/****************************************************************************************************//**
 *                                        CANopen_DictLongRd()
 *
 * @brief    Reads a 32-bit value from the given object dictionary.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    key             Object entry key; should be generated with the macro CANOPEN_DEV().
 *
 * @param    p_val           Pointer to the value destination.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_SIZE_INVALID
 *                               - RTOS_ERR_OBJ_RD
 *
 * @note     (1) The object entry is addressed with the given key and the value to read will be written
 *               to the given destination pointer.
 *******************************************************************************************************/
void CANopen_DictLongRd(CANOPEN_NODE_HANDLE node_handle,
                        CPU_INT32U          key,
                        CPU_INT32U          *p_val,
                        RTOS_ERR            *p_err)
{
  CANopen_DictDataRd(node_handle, key, p_val, CANOPEN_OBJ_LONG, p_err);
}

/****************************************************************************************************//**
 *                                        CANopen_DictWrByte()
 *
 * @brief    Writes a 8-bit value to the given object dictionary.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    key             Object entry key; should be generated with the macro CANOPEN_DEV().
 *
 * @param    val             The source value.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_SIZE_INVALID
 *                               - RTOS_ERR_OBJ_WD
 *
 * @note     (1) The object entry is addressed with the given key and the value will be read from the
 *               given source value.
 *******************************************************************************************************/
void CANopen_DictByteWr(CANOPEN_NODE_HANDLE node_handle,
                        CPU_INT32U          key,
                        CPU_INT08U          val,
                        RTOS_ERR            *p_err)
{
  CANopen_DictDataWr(node_handle, key, val, CANOPEN_OBJ_BYTE, p_err);
}

/****************************************************************************************************//**
 *                                        CANopen_DictWrWord()
 *
 * @brief    Writes a 16-bit value to the given object dictionary.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    key             Object entry key; should be generated with the macro CANOPEN_DEV().
 *
 * @param    val             The source value.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_SIZE_INVALID
 *                               - RTOS_ERR_OBJ_WD
 *
 * @note     (1) The object entry is addressed with the given key and the value will be read from the
 *               given source value.
 *******************************************************************************************************/
void CANopen_DictWordWr(CANOPEN_NODE_HANDLE node_handle,
                        CPU_INT32U          key,
                        CPU_INT16U          val,
                        RTOS_ERR            *p_err)
{
  CANopen_DictDataWr(node_handle, key, val, CANOPEN_OBJ_WORD, p_err);
}

/****************************************************************************************************//**
 *                                        CANopen_DictWrLong()
 *
 * @brief    This function writes a 32-bit value to the given object dictionary.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    key             Object entry key; should be generated with the macro CANOPEN_DEV().
 *
 * @param    val             The source value.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_SIZE_INVALID
 *                               - RTOS_ERR_OBJ_WD
 *
 * @note     (1) The object entry is addressed with the given key and the value will be read from the
 *               given source value.
 *******************************************************************************************************/
void CANopen_DictLongWr(CANOPEN_NODE_HANDLE node_handle,
                        CPU_INT32U          key,
                        CPU_INT32U          val,
                        RTOS_ERR            *p_err)
{
  CANopen_DictDataWr(node_handle, key, val, CANOPEN_OBJ_LONG, p_err);
}

/****************************************************************************************************//**
 *                                        CANopen_DictRdBuf()
 *
 * @brief    Reads a buffer byte stream from the given object dictionary.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    key             Object entry key; should be generated with the macro CANOPEN_DEV().
 *
 * @param    p_buf           Pointer to the destination buffer.
 *
 * @param    len             Length of destination buffer.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @note     (1) The object entry is addressed with the given key and the bytes read will be written to
 *               the given destination buffer of the given length.
 *******************************************************************************************************/
void CANopen_DictBufRd(CANOPEN_NODE_HANDLE node_handle,
                       CPU_INT32U          key,
                       CPU_INT08U          *p_buf,
                       CPU_INT32U          len,
                       RTOS_ERR            *p_err)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CANOPEN_DICT *p_dict;
  CANOPEN_OBJ  *p_obj;
  CPU_INT32U   key_temp;
  RTOS_ERR     local_err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(CANOPEN_NODE_HANDLE_IS_VALID(node_handle), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_dict = &p_node->Dict;

  key_temp = CANOPEN_DEV(CANOPEN_OBJ_GET_IX(p_dict->LastObjUsedPtr->Key), CANOPEN_OBJ_GET_SUBIX(p_dict->LastObjUsedPtr->Key));

  if ((p_dict->LastObjUsedPtr != DEF_NULL)
      && (key == key_temp)) {
    p_obj = p_dict->LastObjUsedPtr;
  } else {
    p_obj = CANopen_DictFind(p_dict, key, p_err);               // try to find key within object dictionary
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    p_dict->LastObjUsedPtr = p_obj;
  }

  if (!CANOPEN_OBJ_IS_RD_ONLY(p_obj->Key)) {
    KAL_LockAcquire(p_node->ObjLock,
                    KAL_OPT_PEND_NONE,
                    p_node->LockTimeoutMs,
                    p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR_CONTEXT_REFRESH(*p_err);
      return;
    }
  }

  CANopen_ObjBufStartRd(p_node,
                        p_obj,                                  // read buffer from object dictionary
                        (void *)p_buf,
                        (CPU_INT08U)len,
                        p_err);

  if (!CANOPEN_OBJ_IS_RD_ONLY(p_obj->Key)) {
    KAL_LockRelease(p_node->ObjLock,
                    &local_err);
    RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
}

/****************************************************************************************************//**
 *                                        CANopen_DictWrBuf()
 *
 * @brief    Writes a buffer byte stream to the given object dictionary.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    key             Object entry key; should be generated with the macro CANOPEN_DEV().
 *
 * @param    p_buf           Pointer to the source buffer.
 *
 * @param    len             Length of source buffer.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s)
 *                           from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @note     (1) The object entry is addressed with the given key and the bytes to write will be read
 *               from to the given source buffer of the given length.
 *******************************************************************************************************/
void CANopen_DictBufWr(CANOPEN_NODE_HANDLE node_handle,
                       CPU_INT32U          key,
                       CPU_INT08U          *p_buf,
                       CPU_INT32U          len,
                       RTOS_ERR            *p_err)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CANOPEN_DICT *p_dict;
  CANOPEN_OBJ  *p_obj;
  CPU_INT32U   key_temp;
  RTOS_ERR     local_err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(CANOPEN_NODE_HANDLE_IS_VALID(node_handle), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_dict = &p_node->Dict;

  key_temp = CANOPEN_DEV(CANOPEN_OBJ_GET_IX(p_dict->LastObjUsedPtr->Key), CANOPEN_OBJ_GET_SUBIX(p_dict->LastObjUsedPtr->Key));

  if ((p_dict->LastObjUsedPtr != DEF_NULL)
      && (key == key_temp)) {
    p_obj = p_dict->LastObjUsedPtr;
  } else {
    p_obj = CANopen_DictFind(p_dict, key, p_err);               // try to find key within object dictionary
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    p_dict->LastObjUsedPtr = p_obj;
  }

  if (!CANOPEN_OBJ_IS_RD_ONLY(p_obj->Key)) {
    KAL_LockAcquire(p_node->ObjLock,
                    KAL_OPT_PEND_NONE,
                    p_node->LockTimeoutMs,
                    p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      RTOS_ERR_CONTEXT_REFRESH(*p_err);
      return;
    }
  }

  CANopen_ObjBufStartWr(p_node,
                        p_obj,                                  // write buffer into object dictionary
                        (void *)p_buf,
                        (CPU_INT08U)len,
                        p_err);

  if (!CANOPEN_OBJ_IS_RD_ONLY(p_obj->Key)) {
    KAL_LockRelease(p_node->ObjLock,
                    &local_err);
    RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           CANopen_DictInit()
 *
 * @brief    Identifies the number of already configured object dictionary entries
 *           within the given object entry array.
 *
 * @param    p_dict       Pointer to object dictionary which must be initialized.
 *
 * @param    p_node       Pointer to the CANopen device node information structure.
 *
 * @param    p_root_obj   Pointer to the first object entry of the dictionary.
 *
 * @param    p_err        Error pointer.
 *
 * @note     (1) The internal object dictionary information structure will be updated with the
 *               identified results and linked to the given node information structure.
 *******************************************************************************************************/
CPU_INT16S CANopen_DictInit(CANOPEN_DICT *p_dict,
                            CANOPEN_NODE *p_node,
                            CANOPEN_OBJ  *p_root_obj,
                            RTOS_ERR     *p_err)
{
  CANOPEN_OBJ *p_obj;
  CPU_INT16U  num = 0u;

  RTOS_ASSERT_DBG_ERR_SET((p_dict != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, -1);

  RTOS_ASSERT_DBG_ERR_SET((p_node != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, -1);

  RTOS_ASSERT_DBG_ERR_SET((p_root_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, -1);

  p_obj = p_root_obj;
  while ((p_obj->Key != 0u)) {                                  // Stop when dictionary's end marker is found
    num++;
    p_obj++;
  }

  p_dict->RootPtr = p_root_obj;                                 // set ptr to root object
  p_dict->ObjQty = num;                                         // set number of valid objects
  p_dict->LastObjUsedPtr = DEF_NULL;

  return ((CPU_INT16S)num);
}

/****************************************************************************************************//**
 *                                         CANopen_DictFind()
 *
 * @brief    Searches the given key within the given object dictionary.
 *
 * @param    p_dict    Pointer to object dictionary.
 *
 * @param    key       Object entry key; should be generated with the macro CANOPEN_DEV().
 *
 * @param    p_err     Error pointer.
 *
 * @return   Pointer to the identified object entry.
 *******************************************************************************************************/
CANOPEN_OBJ *CANopen_DictFind(CANOPEN_DICT *p_dict,
                              CPU_INT32U   key,
                              RTOS_ERR     *p_err)
{
  CANOPEN_OBJ *p_res = DEF_NULL;
  CANOPEN_OBJ *p_obj = DEF_NULL;
  CPU_INT32S  start = 0;
  CPU_INT32S  end;
  CPU_INT32S  center;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_dict != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, -1);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  key = CANOPEN_OBJ_GET_DEV(key);                               // ensure that key includes only idx/subidx
  end = p_dict->ObjQty;                                         // set end to number of entries
  while (start <= end) {                                        // binary search algorithm
    center = start + ((end - start) / 2);
    p_obj = &(p_dict->RootPtr[center]);                         // set to center object entry
    if (CANOPEN_OBJ_GET_DEV(p_obj->Key) == key) {               // see, if object device equal given device
      p_res = p_obj;
      break;
    }
    if (CANOPEN_OBJ_GET_DEV(p_obj->Key) > key) {                // see, if wanted device is in leading half
      end = center - 1;                                         // next search in the leading half
    } else {                                                    // searching entry is in the rear half
      start = center + 1;                                       // next search in the rear half
    }
  }

  if (p_res == DEF_NULL) {                                      // if object is not found
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
  }

  return (p_res);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         CANopen_DictDataRd()
 *
 * @brief    Reads an object value from the object dictionary.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    key             Object entry key; should be generated with the macro CANOPEN_DEV().
 *
 * @param    p_val           Pointer to a variable than will hold the read value.
 *
 * @param    size            Size of the object.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void CANopen_DictDataRd(CANOPEN_NODE_HANDLE node_handle,
                               CPU_INT32U          key,
                               CPU_INT32U          *p_val,
                               CPU_INT08U          size,
                               RTOS_ERR            *p_err)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CANOPEN_DICT *p_dict;
  CPU_INT32U   read_size;
  CANOPEN_OBJ  *p_obj;
  CPU_INT32U   key_temp;
  RTOS_ERR     local_err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(CANOPEN_NODE_HANDLE_IS_VALID(node_handle), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_val != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_dict = &p_node->Dict;

  key_temp = CANOPEN_DEV(CANOPEN_OBJ_GET_IX(p_dict->LastObjUsedPtr->Key), CANOPEN_OBJ_GET_SUBIX(p_dict->LastObjUsedPtr->Key));

  if ((p_dict->LastObjUsedPtr != DEF_NULL)
      && (key == key_temp)) {
    p_obj = p_dict->LastObjUsedPtr;
  } else {
    p_obj = CANopen_DictFind(p_dict, key, p_err);               // try to find key within object dictionary
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    p_dict->LastObjUsedPtr = p_obj;
  }

  read_size = CANopen_ObjSizeGet(p_obj, size, p_err);           // get size of object entry
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  } else if (read_size != (CPU_INT32U)size) {                   // see, if object size matches long
    RTOS_ERR_SET(*p_err, RTOS_ERR_SIZE_INVALID);
    return;
  } else {
    if (!CANOPEN_OBJ_IS_RD_ONLY(p_obj->Key)) {
      KAL_LockAcquire(p_node->ObjLock,
                      KAL_OPT_PEND_NONE,
                      p_node->LockTimeoutMs,
                      p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        RTOS_ERR_CONTEXT_REFRESH(*p_err);
        return;
      }
    }

    CANopen_ObjValRd(p_node,
                     p_obj,                                     // read value into given destination
                     (void *)p_val,
                     size,
                     p_node->NodeId,
                     p_err);

    if (!CANOPEN_OBJ_IS_RD_ONLY(p_obj->Key)) {
      KAL_LockRelease(p_node->ObjLock,
                      &local_err);
      RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }
  }
}

/****************************************************************************************************//**
 *                                         CANopen_DictDataWr()
 *
 * @brief    Writes an object value to the object dictionary.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    key             Object entry key; should be generated with the macro CANOPEN_DEV().
 *
 * @param    val             The source value.
 *
 * @param    size            Size of the object.
 *
 * @param    p_err           Error pointer.
 *******************************************************************************************************/
static void CANopen_DictDataWr(CANOPEN_NODE_HANDLE node_handle,
                               CPU_INT32U          key,
                               CPU_INT32U          val,
                               CPU_INT08U          size,
                               RTOS_ERR            *p_err)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CANOPEN_DICT *p_dict;
  CPU_INT32U   read_size;
  CANOPEN_OBJ  *p_obj;
  CPU_INT32U   key_temp;
  RTOS_ERR     local_err;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(CANOPEN_NODE_HANDLE_IS_VALID(node_handle), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_dict = &p_node->Dict;

  key_temp = CANOPEN_DEV(CANOPEN_OBJ_GET_IX(p_dict->LastObjUsedPtr->Key), CANOPEN_OBJ_GET_SUBIX(p_dict->LastObjUsedPtr->Key));

  if ((p_dict->LastObjUsedPtr != DEF_NULL)
      && (key == key_temp)) {
    p_obj = p_dict->LastObjUsedPtr;
  } else {
    p_obj = CANopen_DictFind(p_dict, key, p_err);               // try to find key within object dictionary
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
    p_dict->LastObjUsedPtr = p_obj;
  }

  read_size = CANopen_ObjSizeGet(p_obj, size, p_err);           // get size of object entry
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  } else if (read_size != (CPU_INT32U)size) {                   // see, if object size matches long
    RTOS_ERR_SET(*p_err, RTOS_ERR_SIZE_INVALID);
    return;
  } else {
    if (!CANOPEN_OBJ_IS_RD_ONLY(p_obj->Key)) {
      KAL_LockAcquire(p_node->ObjLock,
                      KAL_OPT_PEND_NONE,
                      p_node->LockTimeoutMs,
                      p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        RTOS_ERR_CONTEXT_REFRESH(*p_err);
        return;
      }
    }

    CANopen_ObjValWr(p_node,
                     p_obj,                                     // write value into object dictionary
                     (void *)&val,
                     size,
                     p_node->NodeId,
                     p_err);

    if (!CANOPEN_OBJ_IS_RD_ONLY(p_obj->Key)) {
      KAL_LockRelease(p_node->ObjLock,
                      &local_err);
      RTOS_ASSERT_CRITICAL((local_err.Code == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    }
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_CANOPEN_AVAIL
