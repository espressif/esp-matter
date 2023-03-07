/***************************************************************************//**
 * @file
 * @brief Network - HTTP Server Control Layer Add-On - Memory Module
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

#if (defined(RTOS_MODULE_NET_HTTP_SERVER_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error HTTP Server Module requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include "http_server_ctrl_layer_priv.h"

#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                   (NET, HTTP)
#define  RTOS_MODULE_CUR                               RTOS_CFG_MODULE_NET

#define  HTTPs_CTRL_LAYER_MEM_CACHE_LINE_LEN        32u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern MEM_SEG *HTTPs_MemSegPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       HTTPsCtrlLayerMem_InstDataAlloc()
 *
 * @brief    Dynamically allocate a control layer's instance data structure.
 *
 * @return   Pointer to Control Layer instance data object.
 *           DEF_NULL if the structure couldn't be allocated.
 *******************************************************************************************************/
HTTPs_CTRL_LAYER_INST_DATA *HTTPsCtrlLayerMem_InstDataAlloc(void)
{
  HTTPs_CTRL_LAYER_INST_DATA *p_seg;
  RTOS_ERR                   local_err;

  p_seg = (HTTPs_CTRL_LAYER_INST_DATA *)Mem_SegAlloc("CtrlLayer Instance Data",
                                                     HTTPs_MemSegPtr,
                                                     sizeof(HTTPs_CTRL_LAYER_INST_DATA),
                                                     &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  return (p_seg);
}

/****************************************************************************************************//**
 *                                   HTTPsCtrlLayerMem_InstDataPoolInit()
 *
 * @brief    Initialize the pool of instance data for substitution.
 *
 * @param    p_seg           Pointer to the Ctrl Layer instance data previously created.
 *
 * @param    pool_size_max   Size of the pool to allocate.
 *
 * @return   DEF_OK, if the pool was initialize successfully.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsCtrlLayerMem_InstDataPoolInit(HTTPs_CTRL_LAYER_INST_DATA *p_seg,
                                               CPU_SIZE_T                 pool_size_max)
{
  RTOS_ERR local_err;

  p_seg->InstDataHeadPtr = DEF_NULL;

  Mem_DynPoolCreate("CtrlL_InstData_DynMemPool",
                    &p_seg->InstDataEntryPool,
                    HTTPs_MemSegPtr,
                    sizeof(HTTPs_CTRL_LAYER_DATA_ENTRY),
                    HTTPs_CTRL_LAYER_MEM_CACHE_LINE_LEN,
                    pool_size_max,
                    pool_size_max,
                    &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                   HTTPsCtrlLayerMem_ConnDataPoolInit()
 *
 * @brief    Initialize the connection data pool for Ctrl Layer contextual informations.
 *
 * @param    p_seg           Pointer to the Ctrl Layer instance data previously allocated.
 *
 * @param    pool_size_max   Maximum size of the pool to allocate. (Number of connection)
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsCtrlLayerMem_ConnDataPoolInit(HTTPs_CTRL_LAYER_INST_DATA *p_seg,
                                               CPU_SIZE_T                 pool_size_max)
{
  RTOS_ERR local_err;

  Mem_DynPoolCreate("CtrlLayer_ConnData_DynMemPool",
                    &p_seg->ConnDataPool,
                    HTTPs_MemSegPtr,
                    sizeof(HTTPs_CTRL_LAYER_CONN_DATA),
                    HTTPs_CTRL_LAYER_MEM_CACHE_LINE_LEN,
                    pool_size_max,
                    pool_size_max,
                    &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                   HTTPsCtrlLayerMem_ConnDataEntryPoolInit()
 *
 * @brief    Initialize the pool of connection data for sub layer substitution.
 *
 * @param    p_seg           Pointer to Control Layer Instance data previously allocated.
 *
 * @param    pool_size_max   Maximum size of the pool to allocate. (Nb conn * (Max(nbAuth) + 1))
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsCtrlLayerMem_ConnDataEntryPoolInit(HTTPs_CTRL_LAYER_INST_DATA *p_seg,
                                                    CPU_SIZE_T                 pool_size_max)
{
  RTOS_ERR local_err;

  Mem_DynPoolCreate("CtrlLayer_ConnDataEntry_DynMemPool",
                    &p_seg->ConnDataEntryPool,
                    HTTPs_MemSegPtr,
                    sizeof(HTTPs_CTRL_LAYER_DATA_ENTRY),
                    HTTPs_CTRL_LAYER_MEM_CACHE_LINE_LEN,
                    pool_size_max,
                    pool_size_max,
                    &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    return (DEF_FAIL);
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                   HTTPsCtrlLayerMem_InstDataEntryAlloc()
 *
 * @brief    Allocates an instance data entry for substitution.
 *
 * @param    p_seg   Pointer to Control Layer instance data previously allocated.
 *
 * @return   Pointer to Control Layer data entry object allocated.
 *           DEF_NULL  if no data entry could be allocated.
 *******************************************************************************************************/
HTTPs_CTRL_LAYER_DATA_ENTRY *HTTPsCtrlLayerMem_InstDataEntryAlloc(HTTPs_CTRL_LAYER_INST_DATA *p_seg)
{
  HTTPs_CTRL_LAYER_DATA_ENTRY *p_inst_data_entry;
  RTOS_ERR                    local_err;

  p_inst_data_entry = (HTTPs_CTRL_LAYER_DATA_ENTRY *)Mem_DynPoolBlkGet(&p_seg->InstDataEntryPool,
                                                                       &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  p_inst_data_entry->NextPtr = p_seg->InstDataHeadPtr;
  p_seg->InstDataHeadPtr = p_inst_data_entry;

  return (p_inst_data_entry);
}

/****************************************************************************************************//**
 *                                           CtrlL_Alloc_ConnData()
 *
 * @brief    Allocates a connection data for the Ctrl Layer.
 *
 * @param    p_seg   Pointer to Ctrl Layer instance data previously allocated.
 *
 * @return   Pointer to Ctrl Layer Connection Data object allocated.
 *           DEF_NULL if no ConnData could be allocated.
 *******************************************************************************************************/
HTTPs_CTRL_LAYER_CONN_DATA *HTTPsCtrlLayerMem_ConnDataAlloc(HTTPs_CTRL_LAYER_INST_DATA *p_seg)
{
  HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data;
  RTOS_ERR                   local_err;

  p_conn_data = (HTTPs_CTRL_LAYER_CONN_DATA *)Mem_DynPoolBlkGet(&p_seg->ConnDataPool,
                                                                &local_err);

  if (p_conn_data != DEF_NULL) {
    p_conn_data->TargetCfgPtr = DEF_NULL;
    p_conn_data->TargetAppInstPtr = DEF_NULL;
    p_conn_data->ConnDataHeadPtr = DEF_NULL;
  }

  PP_UNUSED_PARAM(local_err);

  return (p_conn_data);
}

/****************************************************************************************************//**
 *                                       HTTPsCtrlLayer_ConnDataFree()
 *
 * @brief    Frees a given ConnData from the pool.
 *
 * @param    p_seg           Pointer to Ctrl Layer instance data previously allocated.
 *
 * @param    p_conn_data     Pointer to Ctrl Layer connection data previously allocated.
 *******************************************************************************************************/
void HTTPsCtrlLayer_ConnDataFree(HTTPs_CTRL_LAYER_INST_DATA *p_seg,
                                 HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data)
{
  RTOS_ERR local_err;

  HTTPsCtrlLayer_ConnDataEntriesFree(p_seg,
                                     p_conn_data);

  Mem_DynPoolBlkFree(&p_seg->ConnDataPool,
                     (void *) p_conn_data,
                     &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                   HTTPsCtrlLayer_ConnDataEntryAlloc()
 *
 * @brief    Allocates a ConnData for substitution.
 *
 * @param    p_seg           Pointer to Ctrl Layer instance data previously allocated.
 *
 * @param    p_conn_data     Pointer to Ctrl Layer connection data previously allocated.
 *******************************************************************************************************/
HTTPs_CTRL_LAYER_DATA_ENTRY *HTTPsCtrlLayer_ConnDataEntryAlloc(HTTPs_CTRL_LAYER_INST_DATA *p_seg,
                                                               HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data)
{
  HTTPs_CTRL_LAYER_DATA_ENTRY *p_conn_data_entry;
  RTOS_ERR                    local_err;

  p_conn_data_entry = (HTTPs_CTRL_LAYER_DATA_ENTRY *)Mem_DynPoolBlkGet(&p_seg->ConnDataEntryPool,
                                                                       &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  p_conn_data_entry->NextPtr = p_conn_data->ConnDataHeadPtr;
  p_conn_data->ConnDataHeadPtr = p_conn_data_entry;

  return (p_conn_data_entry);
}

/****************************************************************************************************//**
 *                                   HTTPsCtrlLayer_ConnDataEntriesFree()
 *
 * @brief    Frees all the ConnDataEntry used for substitution.
 *
 * @param    p_seg           Pointer to Ctrl Layer instance data previously allocated.
 *
 * @param    p_conn_data     Pointer to Ctrl Layer connection data previously allocated.
 *******************************************************************************************************/
void HTTPsCtrlLayer_ConnDataEntriesFree(HTTPs_CTRL_LAYER_INST_DATA *p_seg,
                                        HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data)
{
  HTTPs_CTRL_LAYER_DATA_ENTRY *p_current;
  HTTPs_CTRL_LAYER_DATA_ENTRY *p_entry_to_del;
  RTOS_ERR                    local_err;

  p_current = p_conn_data->ConnDataHeadPtr;

  while (p_current != DEF_NULL) {
    p_entry_to_del = p_current;
    p_current = p_current->NextPtr;

    Mem_DynPoolBlkFree(&p_seg->ConnDataEntryPool,
                       (void *)p_entry_to_del,
                       &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
  }

  p_conn_data->ConnDataHeadPtr = DEF_NULL;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_SERVER_AVAIL
