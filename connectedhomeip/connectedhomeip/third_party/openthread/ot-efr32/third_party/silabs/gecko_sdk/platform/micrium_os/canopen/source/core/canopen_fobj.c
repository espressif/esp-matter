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
#include  <common/include/rtos_err.h>
#include  <common/include/lib_mem.h>
#include  <common/include/lib_str.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <canopen_cfg.h>
#include  <canopen/include/canopen_core.h>
#include  <canopen/include/canopen_obj.h>

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

#if (CANOPEN_OBJ_STRING_EN == DEF_ENABLED)
static CPU_INT32U CANopen_StrSize(CANOPEN_OBJ *p_obj,
                                  CPU_INT32U  width,
                                  RTOS_ERR    *p_err);

static void CANopen_StrCtrl(CANOPEN_NODE_HANDLE node_handle,
                            CANOPEN_OBJ         *p_obj,
                            CPU_INT16U          fnct,
                            CPU_INT32U          param,
                            RTOS_ERR            *p_err);

static void CANopen_StrRd(CANOPEN_NODE_HANDLE node_handle,
                          CANOPEN_OBJ         *p_obj,
                          void                *p_buf,
                          CPU_INT32U          len,
                          RTOS_ERR            *p_err);
#endif

#if (CANOPEN_OBJ_DOMAIN_EN == DEF_ENABLED)
static CPU_INT32U CANopen_DomainSize(CANOPEN_OBJ *p_obj,
                                     CPU_INT32U  width,
                                     RTOS_ERR    *p_err);

static void CANopen_DomainCtrl(CANOPEN_NODE_HANDLE node_handle,
                               CANOPEN_OBJ         *p_obj,
                               CPU_INT16U          fnct,
                               CPU_INT32U          param,
                               RTOS_ERR            *p_err);

static void CANopen_DomainRd(CANOPEN_NODE_HANDLE node_handle,
                             CANOPEN_OBJ         *p_obj,
                             void                *p_buf,
                             CPU_INT32U          len,
                             RTOS_ERR            *p_err);

static void CANopen_DomainWr(CANOPEN_NODE_HANDLE node_handle,
                             CANOPEN_OBJ         *p_obj,
                             void                *p_buf,
                             CPU_INT32U          len,
                             RTOS_ERR            *p_err);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         Object type String
 *
 * @note     (1) This type is responsible for the access to unlimited string constants. It is assumed,
 *               that the strings are declared in non-volatile memory (e.g. FLASH).
 *
 * @note     (2) If the object entry is marked to be read-/write-able, the write access to the object
 *               entry will change the start address of the string - not the string itself. If writing
 *               the string itself is needed, another (user-)type must be defined. The write function
 *               must be implemented with the needed memory handling. The other functions may be reused.
 *******************************************************************************************************/

#if (CANOPEN_OBJ_STRING_EN == DEF_ENABLED)
const CANOPEN_OBJ_TYPE CANopen_ObjTypeStr = {
  .SizeCb = CANopen_StrSize,
  .CtrlCb = CANopen_StrCtrl,
  .RdCb = CANopen_StrRd,
  .WrCb = DEF_NULL
};
#endif

/****************************************************************************************************//**
 *                                         Object type Domain
 *
 * @note     (1) This type is responsible for the access to domain memory areas. It is assumed, that the
 *               memory is declared in random accessible memory (e.g. RAM).
 *
 * @note     (2) This implementation is usable for reading and writing in direct accessible memory (e.g.
 *               RAM). The reading from FLASH memory is no problem, but for the write access to FLASH
 *               memory, there should be a special DOMAIN implementation for this media.
 *******************************************************************************************************/

#if (CANOPEN_OBJ_DOMAIN_EN == DEF_ENABLED)
const CANOPEN_OBJ_TYPE CANopen_ObjTypeDomain = {
  .SizeCb = CANopen_DomainSize,
  .CtrlCb = CANopen_DomainCtrl,
  .RdCb = CANopen_DomainRd,
  .WrCb = CANopen_DomainWr
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

#if (CANOPEN_OBJ_STRING_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                          CANopen_StrSize()
 *
 * @brief    Gets number of characters contained in the given String object.
 *
 * @param    p_obj   Pointer to String object entry.
 *
 * @param    width   Requested string size (or 0 if unknown).
 *
 * @param    p_err   Error pointer.
 *
 * @return   Number of characters.
 *
 * @note     (1) The NULL character is not counted in the number of characters.
 *******************************************************************************************************/
static CPU_INT32U CANopen_StrSize(CANOPEN_OBJ *p_obj,
                                  CPU_INT32U  width,
                                  RTOS_ERR    *p_err)
{
  CANOPEN_DOMAIN_STR *p_str_info = (CANOPEN_DOMAIN_STR *)(p_obj->Data);
  CPU_CHAR           *p_str = (CPU_CHAR *) p_str_info->DataMemStartPtr;
  CPU_INT32U         str_len = 0;

  PP_UNUSED_PARAM(width);
  PP_UNUSED_PARAM(p_err);

  RTOS_ASSERT_DBG((p_obj->Data != 0), RTOS_ERR_ASSERT_DBG_FAIL,; );

  str_len = Str_Len(p_str);

  return (str_len);
}
#endif

#if (CANOPEN_OBJ_STRING_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                          CANopen_StrCtrl()
 *
 * @brief    Performs special control operations on the String object entry.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_obj           Pointer to String object entry.
 *
 * @param    fnct            Control function code.
 *
 * @param    param           Control function parameter.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) This control function supports only the function code CANOPEN_OBJ_CTRL_SET_OFFSET.
 *******************************************************************************************************/
static void CANopen_StrCtrl(CANOPEN_NODE_HANDLE node_handle,
                            CANOPEN_OBJ         *p_obj,
                            CPU_INT16U          fnct,
                            CPU_INT32U          param,
                            RTOS_ERR            *p_err)
{
  PP_UNUSED_PARAM(node_handle);

  RTOS_ASSERT_DBG((p_obj->Data != 0), RTOS_ERR_ASSERT_DBG_FAIL,; );

  if (fnct == CANOPEN_OBJ_CTRL_SET_OFFSET) {                    // see, if function code is correct
    CANOPEN_DOMAIN_STR *p_str_info;

    p_str_info = (CANOPEN_DOMAIN_STR *)(p_obj->Data);
    p_str_info->DataMemOffset = param;                          // yes: set offset for further operations
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
  }
}
#endif

#if (CANOPEN_OBJ_STRING_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                           CANopen_StrRd()
 *
 * @brief   Copies the string characters from the object entry into the given buffer memory.
 *
 * @param   node_handle  Handle to CANopen node object.
 *
 * @param   p_obj        Pointer to String object entry.
 *
 * @param   p_buf        Pointer to buffer memory.
 *
 * @param   len          Length of buffer memory.
 *
 * @param   p_err        Error pointer.
 *******************************************************************************************************/
static void CANopen_StrRd(CANOPEN_NODE_HANDLE node_handle,
                          CANOPEN_OBJ         *p_obj,
                          void                *p_buf,
                          CPU_INT32U          len,
                          RTOS_ERR            *p_err)
{
  CANOPEN_DOMAIN_STR *p_str_info = (CANOPEN_DOMAIN_STR *)(p_obj->Data);
  CPU_CHAR           *p_str = (CPU_CHAR *)p_str_info->DataMemStartPtr;
  CPU_CHAR           *p_dest = (CPU_CHAR *)p_buf;
  CPU_INT32U         offset = p_str_info->DataMemOffset;
  CPU_SIZE_T         nbr_bytes_to_copy;

  PP_UNUSED_PARAM(node_handle);
  PP_UNUSED_PARAM(p_err);

  p_str += offset;                                              // Set string pointer to current location.

  nbr_bytes_to_copy = Str_Len_N(p_str, len);                    // Determine how many bytes to copy.
  Str_Copy_N(p_dest, p_str, nbr_bytes_to_copy);                 // Copy string characters into buffer.

  p_str_info->DataMemOffset += nbr_bytes_to_copy;               // Update string offset for next read.
}
#endif

#if (CANOPEN_OBJ_DOMAIN_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                         CANopen_DomainSize()
 *
 * @brief    Gets size of the given Domain object.
 *
 * @param    p_obj   Pointer to Domain object entry.
 *
 * @param    width   Requested domain size (or 0 if unknown).
 *
 * @param    p_err   Error pointer.
 *
 * @return   Size in bytes of the domain.
 *******************************************************************************************************/
static CPU_INT32U CANopen_DomainSize(CANOPEN_OBJ *p_obj,
                                     CPU_INT32U  width,
                                     RTOS_ERR    *p_err)
{
  CANOPEN_DOMAIN_STR *p_dom_info;
  CPU_INT32U         result = 0;

  PP_UNUSED_PARAM(p_err);

  RTOS_ASSERT_DBG((p_obj->Data != 0), RTOS_ERR_ASSERT_DBG_FAIL,; );

  p_dom_info = (CANOPEN_DOMAIN_STR *)(p_obj->Data);             // get domain info structure
  if ((width > 0)
      && (width < p_dom_info->DataMemSize)) {                   // see, if part of a domain is requested?
    result = width;                                             // allow operations on a part of a domain
  } else {
    result = p_dom_info->DataMemSize;                           // get domain size information
  }

  return (result);
}
#endif

#if (CANOPEN_OBJ_DOMAIN_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                        CANopen_DomainCtrl()
 *
 * @brief    Performs special control operations on the Domain object entry.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_obj           Pointer to Domain object entry.
 *
 * @param    fnct            Control function code.
 *
 * @param    param           Control function parameter.
 *
 * @param    p_err           Error pointer.
 *
 * @note     (1) This control function supports only the function code CANOPEN_OBJ_CTRL_SET_OFFSET.
 *******************************************************************************************************/
static void CANopen_DomainCtrl(CANOPEN_NODE_HANDLE node_handle,
                               CANOPEN_OBJ         *p_obj,
                               CPU_INT16U          fnct,
                               CPU_INT32U          param,
                               RTOS_ERR            *p_err)
{
  PP_UNUSED_PARAM(node_handle);

  RTOS_ASSERT_DBG((p_obj->Data != 0), RTOS_ERR_ASSERT_DBG_FAIL,; );

  if (fnct == CANOPEN_OBJ_CTRL_SET_OFFSET) {                    // see, if function code is correct
    CANOPEN_DOMAIN_STR *p_dom_info;

    //                                                             get domain info structure
    p_dom_info = (CANOPEN_DOMAIN_STR *)(p_obj->Data);
    p_dom_info->DataMemOffset = param;                          // yes: set offset for further operations
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
  }
}
#endif

#if (CANOPEN_OBJ_DOMAIN_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                         CANopen_DomainRd()
 *
 * @brief   Copies the domain memory from the object entry into the given buffer memory.
 *
 * @param   node_handle  Handle to CANopen node object.
 *
 * @param   p_obj        Pointer to Domain object entry.
 *
 * @param   p_buf        Pointer to buffer memory.
 *
 * @param   len          Length of buffer memory.
 *
 * @param   p_err        Error pointer.
 *******************************************************************************************************/
static void CANopen_DomainRd(CANOPEN_NODE_HANDLE node_handle,
                             CANOPEN_OBJ         *p_obj,
                             void                *p_buf,
                             CPU_INT32U          len,
                             RTOS_ERR            *p_err)
{
  CANOPEN_DOMAIN_STR *p_dom_info;
  CPU_INT08U         *p_src_dom;
  CPU_INT08U         *p_dst_buf;
  CPU_INT32U         remaining_bytes_dom;
  CPU_INT32U         remaining_bytes;

  PP_UNUSED_PARAM(node_handle);
  PP_UNUSED_PARAM(p_err);

  RTOS_ASSERT_DBG((p_obj->Data != 0), RTOS_ERR_ASSERT_DBG_FAIL,; );

  p_dom_info = (CANOPEN_DOMAIN_STR *)(p_obj->Data);             // get domain info structure
                                                                // set pointer to working domain address
  p_src_dom = (CPU_INT08U *)(p_dom_info->DataMemStartPtr + p_dom_info->DataMemOffset);
  p_dst_buf = (CPU_INT08U *)p_buf;                              // get buffer pointer

  //                                                               calculate remaining bytes in domain
  remaining_bytes_dom = p_dom_info->DataMemSize - p_dom_info->DataMemOffset;
  remaining_bytes = DEF_MIN(len, remaining_bytes_dom);

  Mem_Copy(p_dst_buf, p_src_dom, remaining_bytes);

  p_dom_info->DataMemOffset += remaining_bytes;                 // Update domain area offset for nxt rd.
}
#endif

#if (CANOPEN_OBJ_DOMAIN_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                         CANopen_DomainWr()
 *
 * @brief   Copies the given buffer memory into the object entry domain memory.
 *
 * @param   node_handle  Handle to CANopen node object.
 *
 * @param   p_obj        Pointer to Domain object entry.
 *
 * @param   p_buf        Pointer to buffer memory.
 *
 * @param   len          Length of buffer memory.
 *
 * @param   p_err        Error pointer.
 *******************************************************************************************************/
static void CANopen_DomainWr(CANOPEN_NODE_HANDLE node_handle,
                             CANOPEN_OBJ         *p_obj,
                             void                *p_buf,
                             CPU_INT32U          len,
                             RTOS_ERR            *p_err)
{
  CANOPEN_DOMAIN_STR *p_dom_info;
  CPU_INT08U         *p_src_buf;
  CPU_INT08U         *p_dest_dom;
  CPU_INT32U         remaining_bytes_dom;
  CPU_INT32U         remaining_bytes;

  PP_UNUSED_PARAM(node_handle);
  PP_UNUSED_PARAM(p_err);

  RTOS_ASSERT_DBG((p_obj->Data != 0), RTOS_ERR_ASSERT_DBG_FAIL,; );

  p_dom_info = (CANOPEN_DOMAIN_STR *)(p_obj->Data);             // get domain info structure
                                                                // set pointer to working domain address
  p_dest_dom = (CPU_INT08U *)(p_dom_info->DataMemStartPtr +  p_dom_info->DataMemOffset);
  p_src_buf = (CPU_INT08U *)p_buf;                              // get buffer pointer

  //                                                               calculate remaining bytes in domain
  remaining_bytes_dom = p_dom_info->DataMemSize - p_dom_info->DataMemOffset;
  remaining_bytes = DEF_MIN(len, remaining_bytes_dom);

  Mem_Copy(p_dest_dom, p_src_buf, remaining_bytes);

  p_dom_info->DataMemOffset += remaining_bytes;                 // Update domain area offset for nxt wr.
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_CANOPEN_AVAIL
