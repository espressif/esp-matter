/***************************************************************************//**
 * @file
 * @brief CANopen Object Directory Management
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

#include  <cpu/include/cpu.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/rtos_err.h>

#include  <canopen_cfg.h>
#include  <canopen/include/canopen_core.h>
#include  <canopen/include/canopen_obj.h>
#include  <canopen/source/core/canopen_obj_priv.h>
#include  <canopen/source/core/canopen_core_priv.h>
#include  <canopen/source/objects/pdo/canopen_tpdo_priv.h>

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

static void CANopen_ObjTypeRd(CANOPEN_NODE *p_node,
                              CANOPEN_OBJ  *p_obj,
                              void         *p_dst,
                              CPU_INT32U   len,
                              CPU_INT32U   off,
                              RTOS_ERR     *p_err);

static void CANopen_ObjTypeWr(CANOPEN_NODE *p_node,
                              CANOPEN_OBJ  *p_obj,
                              void         *p_dst,
                              CPU_INT32U   len,
                              CPU_INT32U   off,
                              RTOS_ERR     *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                         INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         CANopen_ObjInit()
 *
 * @brief    This function (re-)initializes an object entry element.
 *
 * @param    p_obj   Pointer to the object entry.
 *******************************************************************************************************/
void CANopen_ObjInit(CANOPEN_OBJ *p_obj)
{
  RTOS_ASSERT_DBG((p_obj != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  p_obj->Key = 0;                                               // mark object to be unused
  p_obj->TypePtr = DEF_NULL;                                    // basic type (no linked type structure)
  p_obj->Data = 0;                                              // clear data
}

/****************************************************************************************************//**
 *                                         CANopen_ObjCmp()
 *
 * @brief    Compares the given value with the currently stored value within the object directory.
 *
 * @param    p_obj   Pointer to the CANopen object entry.
 *
 * @param    p_val   Pointer to new value to compare to.
 *
 * @param    p_err   Error pointer.
 *
 * @return   DEF_YES if current and new object values are the same.
 *           DEF_NO  otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN CANopen_ObjCmp(CANOPEN_OBJ *p_obj,
                           void        *p_val,
                           RTOS_ERR    *p_err)
{
  CPU_INT32U newval = 0u;                                       // Local: new value from parameter
  CPU_INT32U oldval = 0u;                                       // Local: current value in object
  CPU_INT16S result = 0;                                        // Local: result of comparison

  newval = *((CPU_INT32U *)p_val);                              // get value for comparison

  CANopen_ObjDirectRd(p_obj, &oldval, CANOPEN_OBJ_LONG, p_err); // get current value from object entry
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {             // see, if an error is detected
    return (result);                                            // indicate error
  }

  result = (newval == oldval) ? DEF_YES : DEF_NO;

  return (result);
}

/****************************************************************************************************//**
 *                                        CANopen_ObjSizeGet()
 *
 * @brief    Returns the size of the given object directory entry.
 *
 * @param    p_obj   Pointer to the CANopen directory entry.
 *
 * @param    width   Expected object size in byte (or 0 if unknown).
 *
 * @param    p_err   Error pointer.
 *
 * @return   Size of accessed object.
 *******************************************************************************************************/
CPU_INT32U CANopen_ObjSizeGet(CANOPEN_OBJ *p_obj,
                              CPU_INT32U  width,
                              RTOS_ERR    *p_err)
{
  CPU_INT32U       result = 0u;
  CANOPEN_OBJ_TYPE *p_obj_type;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, result);

  p_obj_type = p_obj->TypePtr;                                  // get pointer to object type structure
  if (p_obj_type != DEF_NULL) {                                 // see, if object type is configured
    if (p_obj_type->SizeCb != DEF_NULL) {                       // yes: see, if size function is configured
      result = p_obj_type->SizeCb(p_obj, width, p_err);         // yes: call object type specific size function
    } else {
      result = CANOPEN_OBJ_GET_SIZE(p_obj->Key);                // return size (encoded in key)
    }
  } else {                                                      // otherwise: size encoded in key
    result = CANOPEN_OBJ_GET_SIZE(p_obj->Key);                  // return size (encoded in key)
  }

  return (result);                                              // return function result
}

/****************************************************************************************************//**
 *                                         CANopen_ObjValRd()
 *
 * @brief    Reads a value from the given object directory entry.
 *
 * @param    p_node      Pointer to the node.
 *
 * @param    p_obj       Pointer to the CANopen entry.
 *
 * @param    p_val       Pointer to the result memory.
 *
 * @param    width       Width of read value (must be 1, 2 or 4 and reflecting the width of the
 *                       referenced variable with given parameter pointer).
 *
 * @param    node_id     Device node ID (only used in case of node ID dependent value)
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
void CANopen_ObjValRd(CANOPEN_NODE *p_node,
                      CANOPEN_OBJ  *p_obj,
                      void         *p_val,
                      CPU_INT08U   width,
                      CPU_INT08U   node_id,
                      RTOS_ERR     *p_err)
{
  CANOPEN_OBJ_TYPE *p_obj_type;
  CPU_INT32U       val = 0u;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_val != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET(((width == 1u) || (width == 2u) || (width == 4u)), *p_err, RTOS_ERR_INVALID_ARG,; );

  p_obj_type = p_obj->TypePtr;                                  // get type from object entry
  if (p_obj_type != DEF_NULL) {                                 // see, if type is configured
    CANopen_ObjTypeRd(p_node,
                      p_obj,                                    // read with referenced type function
                      (void *)&val,                             // the p_val with the
                      CANOPEN_OBJ_LONG,
                      0,
                      p_err);                                   // starting at offset 0
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  } else {                                                      // otherwise: no type object structure
    CANopen_ObjDirectRd(p_obj,                                  // read p_val from object data element
                        (void *)&val,
                        CANOPEN_OBJ_LONG,
                        p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }
  if (CANOPEN_OBJ_IS_NODEID(p_obj->Key) != 0) {                 // see, if node-id shall be considered
    val = (val + node_id);                                      // set p_val to object p_val + node id
  }

  switch (width) {
    case 1u:
      *((CPU_INT08U *)p_val) = (CPU_INT08U)val;
      break;

    case 2u:
      *((CPU_INT16U *)p_val) = (CPU_INT16U)val;
      break;

    case 4u:
      *((CPU_INT32U *)p_val) = (CPU_INT32U)val;
      break;

    default:
      RTOS_ASSERT_CRITICAL_FAILED_END_CALL(; );
      break;
  }
}

/****************************************************************************************************//**
 *                                        CANopen_ObjValWr()
 *
 * @brief    Writes a value to the given object directory entry.
 *
 * @param    p_node      Pointer to the node.
 *
 * @param    p_obj       Pointer to the CANopen object entry.
 *
 * @param    p_val       Pointer to the result memory.
 *
 * @param    width       Width of written value (must be 1, 2 or 4 and reflecting the width
 *                       of the referenced variable with given parameter pointer).
 *
 * @param    node_id     Device node ID (only used in case of node ID dependent value).
 *
 * @param    p_err       Error pointer.
 *******************************************************************************************************/
void CANopen_ObjValWr(CANOPEN_NODE *p_node,
                      CANOPEN_OBJ  *p_obj,
                      void         *p_val,
                      CPU_INT08U   width,
                      CPU_INT08U   node_id,
                      RTOS_ERR     *p_err)
{
  CANOPEN_OBJ_TYPE *p_obj_type;
  CPU_INT32U       val = 0u;
#if (CANOPEN_TPDO_MAX_QTY > 0)
  CPU_BOOLEAN status = DEF_NO;
#endif

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_val != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET(((width == 1u) || (width == 2u) || (width == 4u)), *p_err, RTOS_ERR_INVALID_ARG,; );

  switch (width) {
    case 1u:
      val = *((CPU_INT08U *)p_val) & 0xFF;                      // cast referenced value to a byte
      break;

    case 2u:
      val = *((CPU_INT16U *)p_val) & 0xFFFF;                    // cast referenced value to a word
      break;

    case 4u:
      val = *((CPU_INT32U *)p_val);                             // cast referenced value to a long
      break;

    default:
      RTOS_ASSERT_CRITICAL_FAILED_END_CALL(; );
      break;
  }

  if (CANOPEN_OBJ_IS_NODEID(p_obj->Key) != 0) {                 // see, if node-id shall be considered
    val = (val - node_id);                                      // set object value to value - node id
  }

#if (CANOPEN_TPDO_MAX_QTY > 0)
  if (CANOPEN_OBJ_IS_PDOMAP(p_obj->Key) != 0) {                 // see, if PDO mappable object entry
    status = CANopen_ObjCmp(p_obj,                              // compare existing value with new value
                            (void *)&val,
                            p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return;
    }
  }
#endif

  p_obj_type = p_obj->TypePtr;                                  // get object type from object entry
  if (p_obj_type != DEF_NULL) {                                 // see, if object type is configured
    CANopen_ObjTypeWr(p_node,
                      p_obj,                                    // write with referenced object type function
                      (void *)&val,                             // the value with the
                      CANOPEN_OBJ_LONG,
                      0,
                      p_err);                                   // starting at offset 0
  } else {                                                      // otherwise: no object type object structure
    CANopen_ObjDirectWr(p_obj,                                  // write value to object data element
                        (void *)&val,
                        CANOPEN_OBJ_LONG,
                        p_err);
  }

#if (CANOPEN_TPDO_MAX_QTY > 0)
  //                                                               see, if signal value has changed
  if ((status == DEF_NO)
      && (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE)) {
    CANOPEN_NODE_HANDLE handle;

    CANOPEN_NODE_HANDLE_SET(handle, p_node);

    p_obj_type = p_obj->TypePtr;                                // yes: get object entry object type
    if (p_obj_type == CANOPEN_OBJ_TYPE_TPDO_ASYNC) {            // see, if asynchronous trigger is defined
      p_obj->TypePtr->CtrlCb(handle,                            // call TPDO asynchronous trigger function
                             p_obj,
                             CANOPEN_TPDO_ASYNC,
                             0,
                             p_err);
      //                                                           potential errors already noted in node
    } else if (p_obj_type == CANOPEN_OBJ_TYPE_TPDO_SYNC) {
      p_obj->TypePtr->CtrlCb(handle,                            // call TPDO synchronous trigger function
                             p_obj,
                             CANOPEN_TPDO_SYNC,
                             0,
                             p_err);
    }
  }
#endif
}

/****************************************************************************************************//**
 *                                      CANopen_ObjBufStartRd()
 *
 * @brief    Starts the read operation at the beginning of the byte stream from the given object entry
 *           into the given destination buffer.
 *
 * @param    p_node  Pointer to the node.
 *
 * @param    p_obj   Pointer to the CANopen object entry.
 *
 * @param    p_buf   Pointer to the destination buffer.
 *
 * @param    len     Length of destination buffer.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void CANopen_ObjBufStartRd(CANOPEN_NODE *p_node,
                           CANOPEN_OBJ  *p_obj,
                           CPU_INT08U   *p_buf,
                           CPU_INT32U   len,
                           RTOS_ERR     *p_err)
{
  CANOPEN_OBJ_TYPE *p_obj_type;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_obj_type = p_obj->TypePtr;                                  // get object type from object entry
  if (p_obj_type != DEF_NULL) {                                 // see, if object type is configured
    CANopen_ObjTypeRd(p_node,
                      p_obj,                                    // read with referenced object type function
                      (void *)p_buf,                            // the value with the
                      len,
                      0u,
                      p_err);                                   // starting at offset 0
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
  }
}

/****************************************************************************************************//**
 *                                      CANopen_ObjBufContRd()
 *
 * @brief    Continues the read operation at the current offset of the byte stream from the given object
 *           entry into the given destination buffer.
 *
 * @param    p_node  Pointer to the node.
 *
 * @param    p_obj   Pointer to the CANopen object entry.
 *
 * @param    p_buf   Pointer to the destination buffer.
 *
 * @param    len     Length of destination buffer.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void CANopen_ObjBufContRd(CANOPEN_NODE *p_node,
                          CANOPEN_OBJ  *p_obj,
                          CPU_INT08U   *p_buf,
                          CPU_INT32U   len,
                          RTOS_ERR     *p_err)
{
  CANOPEN_OBJ_TYPE *p_obj_type;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_obj_type = p_obj->TypePtr;                                  // get type from object entry
  if (p_obj_type != DEF_NULL) {                                 // see, if type is configured
    if (p_obj_type->RdCb != DEF_NULL) {                         // see, if read function is referenced
      CANOPEN_NODE_HANDLE handle;

      CANOPEN_NODE_HANDLE_SET(handle, p_node);
      p_obj_type->RdCb(handle,                                  // call referenced type read function
                       p_obj,
                       p_buf,
                       len,
                       p_err);
    }
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
  }
}

/****************************************************************************************************//**
 *                                      CANopen_ObjBufStartWr()
 *
 * @brief    Starts the write operation at the beginning of the given object entry from the given source
 *           buffer.
 *
 * @param    p_node  Pointer to the node.
 *
 * @param    p_obj   Pointer to the CANopen object directory entry.
 *
 * @param    p_buf   Pointer to the source buffer.
 *
 * @param    len     Length of source buffer.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void CANopen_ObjBufStartWr(CANOPEN_NODE *p_node,
                           CANOPEN_OBJ  *p_obj,
                           CPU_INT08U   *p_buf,
                           CPU_INT32U   len,
                           RTOS_ERR     *p_err)
{
  CANOPEN_OBJ_TYPE *p_obj_type;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  p_obj_type = p_obj->TypePtr;                                  // get type from object entry
  if (p_obj_type != DEF_NULL) {                                 // see, if type is configured
    CANopen_ObjTypeWr(p_node,
                      p_obj,                                    // write with referenced type function
                      (void *)p_buf,                            // the value with the
                      len,
                      0,
                      p_err);                                   // starting at offset 0
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
  }
}

/****************************************************************************************************//**
 *                                      CANopen_ObjBufContWr()
 *
 * @brief    Continues the write operation at the current offset of the byte stream from the given source
 *           buffer to the object entry.
 *
 * @param    p_node  Pointer to the node.
 *
 * @param    p_obj   Pointer to the CANopen object entry.
 *
 * @param    p_buf   Pointer to the source buffer.
 *
 * @param    len     Length of source buffer.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
void CANopen_ObjBufContWr(CANOPEN_NODE *p_node,
                          CANOPEN_OBJ  *p_obj,
                          CPU_INT08U   *p_buf,
                          CPU_INT32U   len,
                          RTOS_ERR     *p_err)
{
  CANOPEN_OBJ_TYPE *p_obj_type;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_obj_type = p_obj->TypePtr;                                  // get p_obj_type from object entry
  if (p_obj_type != DEF_NULL) {                                 // see, if p_obj_type is configured
    if (p_obj_type->WrCb != DEF_NULL) {                         // see, if write function is referenced
      CANOPEN_NODE_HANDLE handle;

      CANOPEN_NODE_HANDLE_SET(handle, p_node);
      p_obj_type->WrCb(handle,                                  // call referenced p_obj_type write function
                       p_obj,
                       p_buf,
                       len,
                       p_err);
    }
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_TYPE);
  }
}

/****************************************************************************************************//**
 *                                       CANopen_ObjDirectRd()
 *
 * @brief    Reads the value of the entry directly from the data pointer.
 *
 * @param    p_obj   Pointer to the CANopen object entry.
 *
 * @param    p_val   Pointer to the result memory.
 *
 * @param    len     Length of value in bytes.
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) This function checks the internal flags and reacts as configured: reading the pointer
 *               address as value, or use the address and get the value from that address.
 *******************************************************************************************************/
void CANopen_ObjDirectRd(CANOPEN_OBJ *p_obj,
                         void        *p_val,
                         CPU_INT32U  len,
                         RTOS_ERR    *p_err)
{
  CPU_INT08U obj_size;

  RTOS_ASSERT_DBG_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_val != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  if (CANOPEN_OBJ_IS_DIRECT(p_obj->Key) != 0) {                 // see, if direct access is required
                                                                // get value directly from member Data
    if (len == CANOPEN_OBJ_BYTE) {                              // see, if target reference is a byte
      *((CPU_INT08U *)p_val) = (CPU_INT08U)((p_obj->Data) & 0xffu);
    } else if (len == CANOPEN_OBJ_WORD) {                       // see, if target reference is a word
      *((CPU_INT16U *)p_val) = (CPU_INT16U)((p_obj->Data) & 0xffffu);
    } else if (len == CANOPEN_OBJ_LONG) {                       // see, if target reference is a long
      *((CPU_INT32U *)p_val) = (CPU_INT32U)(p_obj->Data);
    } else {                                                    // otherwise: invalid data size
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    }
  } else {                                                      // otherwise: no direct access allowed
    obj_size = CANOPEN_OBJ_GET_SIZE(p_obj->Key);                // get object entry size
    if (p_obj->Data == 0) {                                     // see, if pointer is invalid
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
    } else if (len == CANOPEN_OBJ_BYTE) {                       // see, if target reference is a byte
                                                                // get value referenced by member Data
      if (obj_size == CANOPEN_OBJ_BYTE) {                       // see, if object reference is a byte
        *((CPU_INT08U *)p_val) = (CPU_INT08U)(*((CPU_INT08U *)(p_obj->Data)) & 0xffu);
      } else if (obj_size == CANOPEN_OBJ_WORD) {                // see, if object reference is a word
        *((CPU_INT08U *)p_val) = (CPU_INT08U)(*((CPU_INT16U *)(p_obj->Data)) & 0xffffu);
      } else if (obj_size == CANOPEN_OBJ_LONG) {                // see, if object reference is a long
        *((CPU_INT08U *)p_val) = (CPU_INT08U)(*((CPU_INT32U *)(p_obj->Data)) & 0xffffffffu);
      } else {                                                  // otherwise: invalid data size
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      }
    } else if (len == CANOPEN_OBJ_WORD) {                       // see, if target reference is a word
                                                                // get value referenced by member Data
      if (obj_size == CANOPEN_OBJ_BYTE) {                       // see, if object reference is a byte
        *((CPU_INT16U *)p_val) = (CPU_INT16U)(*((CPU_INT08U *)(p_obj->Data)) & 0xffu);
      } else if (obj_size == CANOPEN_OBJ_WORD) {                // see, if object reference is a word
        *((CPU_INT16U *)p_val) = (CPU_INT16U)(*((CPU_INT16U *)(p_obj->Data)) & 0xffffu);
      } else if (obj_size == CANOPEN_OBJ_LONG) {                // see, if object reference is a long
        *((CPU_INT16U *)p_val) = (CPU_INT16U)(*((CPU_INT32U *)(p_obj->Data)) & 0xffffffffu);
      } else {                                                  // otherwise: invalid data size
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      }
    } else if (len == CANOPEN_OBJ_LONG) {                       // see, if target reference is a long
                                                                // get value referenced by member Data
      if (obj_size == CANOPEN_OBJ_BYTE) {                       // see, if object reference is a byte
        *((CPU_INT32U *)p_val) = (CPU_INT32U)(*((CPU_INT08U *)(p_obj->Data)));
      } else if (obj_size == CANOPEN_OBJ_WORD) {                // see, if object reference is a word
        *((CPU_INT32U *)p_val) = (CPU_INT32U)(*((CPU_INT16U *)(p_obj->Data)));
      } else if (obj_size == CANOPEN_OBJ_LONG) {                // see, if object reference is a long
        *((CPU_INT32U *)p_val) = (CPU_INT32U)(*((CPU_INT32U *)(p_obj->Data)));
      } else {                                                  // otherwise: invalid data size
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      }
    }
  }
}

/****************************************************************************************************//**
 *                                       CANopen_ObjDirectWr()
 *
 * @brief    Writes the value to the entry directly to the data pointer.
 *
 * @param    p_obj   Pointer to the CANopen object directory entry.
 *
 * @param    p_val   Pointer to the source memory.
 *
 * @param    len     Length of value in bytes.
 *
 * @param    p_err   Error pointer.
 *
 * @note     (1) This function checks the internal flags and reacts as configured: writing the value
 *               to the pointer address, or use the address and write the value to that address.
 *******************************************************************************************************/
void CANopen_ObjDirectWr(CANOPEN_OBJ *p_obj,
                         void        *p_val,
                         CPU_INT32U  len,
                         RTOS_ERR    *p_err)
{
  CPU_INT08U obj_size;

  RTOS_ASSERT_DBG_ERR_SET((p_obj != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ASSERT_DBG_ERR_SET((p_val != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  if (CANOPEN_OBJ_IS_DIRECT(p_obj->Key) != 0) {                 // see, if direct access is required
                                                                // set referenced value directly to Data
    if (len == CANOPEN_OBJ_BYTE) {                              // see, if referenced value is byte
      p_obj->Data = (CPU_INT32U)(*((CPU_INT08U *)p_val) & 0xffu);
    } else if (len == CANOPEN_OBJ_WORD) {                       // see, if referenced value is word
      p_obj->Data = (CPU_INT32U)(*((CPU_INT16U *)p_val) & 0xffffu);
    } else if (len == CANOPEN_OBJ_LONG) {                       // see, if referenced value is long
      p_obj->Data = (CPU_INT32U)(*((CPU_INT32U *)p_val));
    } else {                                                    // otherwise: invalid data size
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    }
  } else {                                                      // otherwise: no direct access allowed
    obj_size = CANOPEN_OBJ_GET_SIZE(p_obj->Key);                // get object entry size
    if (p_obj->Data == 0) {                                     // see, if pointer is invalid
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
    } else if (obj_size == CANOPEN_OBJ_BYTE) {                  // see, if target reference is a byte
                                                                // set referenced value to data reference
      if (len == CANOPEN_OBJ_BYTE) {                            // see, if object reference is a byte
        *((CPU_INT08U *)(p_obj->Data)) = (CPU_INT08U)(*((CPU_INT08U *)(p_val)) & 0xffu);
      } else if (len == CANOPEN_OBJ_WORD) {                     // see, if object reference is a word
        *((CPU_INT08U *)(p_obj->Data)) = (CPU_INT08U)(*((CPU_INT16U *)(p_val)) & 0xffffu);
      } else if (len == CANOPEN_OBJ_LONG) {                     // see, if object reference is a long
        *((CPU_INT08U *)(p_obj->Data)) = (CPU_INT08U)(*((CPU_INT32U *)(p_val)) & 0xffffffffu);
      } else {                                                  // otherwise: invalid data size
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      }
    } else if (obj_size == CANOPEN_OBJ_WORD) {                  // see, if target reference is a word
                                                                // set referenced value to data reference
      if (len == CANOPEN_OBJ_BYTE) {                            // see, if object reference is a byte
        *((CPU_INT16U *)(p_obj->Data)) = (CPU_INT16U)(*((CPU_INT08U *)(p_val)) & 0xffu);
      } else if (len == CANOPEN_OBJ_WORD) {                     // see, if object reference is a word
        *((CPU_INT16U *)(p_obj->Data)) = (CPU_INT16U)(*((CPU_INT16U *)(p_val)) & 0xffffu);
      } else if (len == CANOPEN_OBJ_LONG) {                     // see, if object reference is a long
        *((CPU_INT16U *)(p_obj->Data)) = (CPU_INT16U)(*((CPU_INT32U *)(p_val)) & 0xffffffffu);
      } else {                                                  // otherwise: invalid data size
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      }
    } else if (obj_size == CANOPEN_OBJ_LONG) {                  // see, if target reference is a long
                                                                // set referenced value to data reference
      if (len == CANOPEN_OBJ_BYTE) {                            // see, if object reference is a byte
        *((CPU_INT32U *)(p_obj->Data)) = (CPU_INT32U)(*((CPU_INT08U *)(p_val)));
      } else if (len == CANOPEN_OBJ_WORD) {                     // see, if object reference is a word
        *((CPU_INT32U *)(p_obj->Data)) = (CPU_INT32U)(*((CPU_INT16U *)(p_val)));
      } else if (len == CANOPEN_OBJ_LONG) {                     // see, if object reference is a long
        *((CPU_INT32U *)(p_obj->Data)) = (CPU_INT32U)(*((CPU_INT32U *)(p_val)));
      } else {                                                  // otherwise: invalid data size
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      }
    }
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        CANopen_ObjTypeRd()
 *
 * @brief    Reads the value of the entry directly with the linked type-functions.
 *
 * @param    p_node  Pointer to the node.
 *
 * @param    p_obj   Pointer to the CANopen object entry.
 *
 * @param    p_dst   Pointer to the result memory.
 *
 * @param    len     Length of value in bytes.
 *
 * @param    off     Set this offset, before reading.
 *
 * @param    p_err   Error pointer.
 *******************************************************************************************************/
static void CANopen_ObjTypeRd(CANOPEN_NODE *p_node,
                              CANOPEN_OBJ  *p_obj,
                              void         *p_dst,
                              CPU_INT32U   len,
                              CPU_INT32U   off,
                              RTOS_ERR     *p_err)
{
  CANOPEN_OBJ_TYPE *p_obj_type;

  p_obj_type = p_obj->TypePtr;                                  // get type structure reference
  if (p_obj_type != DEF_NULL) {                                 // see, if type is valid
    if (p_obj_type->RdCb != DEF_NULL) {                         // see, if read function is refereced
      CANOPEN_NODE_HANDLE handle;

      CANOPEN_NODE_HANDLE_SET(handle, p_node);
      if (p_obj_type->CtrlCb != DEF_NULL) {                     // yes: see, if ctrl-function is refereced
        p_obj_type->CtrlCb(handle,                              // use ctrl-function to set read offset
                           p_obj,
                           CANOPEN_OBJ_CTRL_SET_OFFSET,
                           off,
                           p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return;
        }
      }

      if (len != 0u) {                                          // Read only if it is needed.
        p_obj_type->RdCb(handle,                                // call referenced type read function
                         p_obj,
                         p_dst,
                         len,
                         p_err);
      }
    } else {                                                    // otherwise: no read function referenced
      CANopen_ObjDirectRd(p_obj,                                // read value from object data element
                          (void *)p_dst,
                          len,
                          p_err);
    }
  }
}

/****************************************************************************************************//**
 *                                        CANopen_ObjTypeWr()
 *
 * @brief    Writes the value of the entry directly with the linked type-functions.
 *
 * @param    p_node  Pointer to the node.
 *
 * @param    p_obj   Pointer to the CANopen object entry.
 *
 * @param    p_src   Pointer to the source memory.
 *
 * @param    len     Length of value in bytes.
 *
 * @param    off     Set this offset, before writing
 *
 * @param    p_err   Error pointer.
 ***************************************************************************************************/
static void CANopen_ObjTypeWr(CANOPEN_NODE *p_node,
                              CANOPEN_OBJ  *p_obj,
                              void         *p_src,
                              CPU_INT32U   len,
                              CPU_INT32U   off,
                              RTOS_ERR     *p_err)
{
  CANOPEN_OBJ_TYPE *p_obj_type;

  p_obj_type = p_obj->TypePtr;                                  // get type structure reference
  if (p_obj_type != DEF_NULL) {                                 // see, if type is valid
    if (p_obj_type->WrCb != DEF_NULL) {                         // see, if write function is referenced
      CANOPEN_NODE_HANDLE handle;

      CANOPEN_NODE_HANDLE_SET(handle, p_node);
      if (p_obj_type->CtrlCb != DEF_NULL) {                     // yes: see, if ctrl-function is refereced
        p_obj_type->CtrlCb(handle,                              // use ctrl-function to reset write offset
                           p_obj,
                           CANOPEN_OBJ_CTRL_SET_OFFSET,
                           off,
                           p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          return;
        }
      }

      if (len != 0u) {                                          // Write only if it is needed.
        p_obj_type->WrCb(handle,                                // call referenced p_obj_type write function
                         p_obj,
                         p_src,
                         len,
                         p_err);
      }
    } else {                                                    // otherwise: no write function referenced
      CANopen_ObjDirectWr(p_obj,                                // write value to object data element
                          (void *)p_src,
                          len,
                          p_err);
    }
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_CANOPEN_AVAIL
