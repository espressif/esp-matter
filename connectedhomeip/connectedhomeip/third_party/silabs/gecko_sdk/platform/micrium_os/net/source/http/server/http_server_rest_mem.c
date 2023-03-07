/***************************************************************************//**
 * @file
 * @brief Network - HTTP Server Rest Module
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

#include  <em_core.h>

#include "http_server_rest_priv.h"

#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                   (NET, HTTP)
#define  RTOS_MODULE_CUR                               RTOS_CFG_MODULE_NET

#define  HTTPs_REST_MEM_CACHE_LINE_LEN          32u
#define  HTTPs_REST_MEM_RESOURCE_LIST_MAX       10u
#define  HTTPs_REST_MEM_RESOURCE_MAX            50u

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern MEM_SEG *HTTPs_MemSegPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static SLIST_MEMBER *HTTPsREST_Mem_ResourceList;

static MEM_DYN_POOL HTTPsREST_Mem_ResourceListPool;
static MEM_DYN_POOL HTTPsREST_Mem_ResourcePool;

static volatile CPU_BOOLEAN HTTPsREST_Mem_PoolsInitialized = DEF_NO;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       HTTPsREST_Mem_Init_Pools()
 *
 * @brief    Initialize the rest memory pools.
 *           This function is called during the initialization of the HTTP-s
 *
 * @param    max_request     Maximum number of simultaneous request the server can handle
 *
 * @return   DEF_NULL    if out of memory
 *           OBJ         otherwise
 *******************************************************************************************************/
HTTPs_REST_INST_DATA *HTTPsREST_Mem_Init_Pools(CPU_SIZE_T max_request)
{
  HTTPs_REST_INST_DATA *p_inst_data;
  RTOS_ERR             local_err;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (HTTPsREST_Mem_PoolsInitialized == DEF_NO) {
    Mem_DynPoolCreate("REST_ResourceList_MemPool",
                      &HTTPsREST_Mem_ResourceListPool,
                      HTTPs_MemSegPtr,
                      sizeof(HTTPs_REST_RESOURCE_LIST),
                      HTTPs_REST_MEM_CACHE_LINE_LEN,
                      0,                                        // Min block
                      HTTPs_REST_MEM_RESOURCE_LIST_MAX,         // Max block
                      &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      CORE_EXIT_ATOMIC();
      return (DEF_NULL);
    }

    Mem_DynPoolCreate("REST_Resource_MemPool",
                      &HTTPsREST_Mem_ResourcePool,
                      HTTPs_MemSegPtr,
                      sizeof(HTTPs_REST_RESOURCE_ENTRY),
                      HTTPs_REST_MEM_CACHE_LINE_LEN,
                      0,                                        // Min block
                      HTTPs_REST_MEM_RESOURCE_MAX,              // Max block
                      &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      CORE_EXIT_ATOMIC();
      return (DEF_NULL);
    }

    SList_Init(&HTTPsREST_Mem_ResourceList);
  }
  CORE_EXIT_ATOMIC();

  p_inst_data = (HTTPs_REST_INST_DATA *)Mem_SegAlloc("REST_InstanceSeg",
                                                     HTTPs_MemSegPtr,
                                                     sizeof(HTTPs_REST_INST_DATA),
                                                     &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  Mem_DynPoolCreate("REST_Conn_MemPool",
                    &p_inst_data->Pool,
                    HTTPs_MemSegPtr,
                    sizeof(HTTPs_REST_REQUEST),
                    HTTPs_REST_MEM_CACHE_LINE_LEN,
                    0,                                  // Min block
                    max_request,                        // Max block
                    &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  return (p_inst_data);
}

/****************************************************************************************************//**
 *                                       HTTPsREST_Mem_GetResourceList()
 *
 * @brief    Get the resource list associated with an ID.
 *           If the list doesn't exist, the list is created.
 *
 * @param    list_ID     Differentiate two or more lists
 *
 * @return   Pointer to resource list object.
 *           DEF_NULL if out of memory.
 *
 * @note         (1)  Once a list is created it is never freed.
 *******************************************************************************************************/
HTTPs_REST_RESOURCE_LIST *HTTPsREST_Mem_GetResourceList(CPU_INT32U list_ID)
{
  CPU_BOOLEAN              found;
  HTTPs_REST_RESOURCE_LIST *p_list;
  RTOS_ERR                 local_err;

  found = DEF_NO;

  SLIST_FOR_EACH_ENTRY(HTTPsREST_Mem_ResourceList, p_list, HTTPs_REST_RESOURCE_LIST, ListNode)
  {
    if (p_list->Id == list_ID) {
      found = DEF_TRUE;
      break;
    }
  }

  if (found != DEF_TRUE) {
    p_list = (HTTPs_REST_RESOURCE_LIST *)Mem_DynPoolBlkGet(&HTTPsREST_Mem_ResourceListPool,
                                                           &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      return (DEF_NULL);
    }

    p_list->Id = list_ID;
    p_list->ListHeadPtr = DEF_NULL;

    SList_Push(&HTTPsREST_Mem_ResourceList, &(p_list->ListNode));
  }

  return (p_list);
}

/****************************************************************************************************//**
 *                                       HTTPsREST_Mem_AllocResource()
 *
 * @brief    Allocate a resource token to be chained.
 *
 * @param    list_ID     ID of the list to chain the resource token.
 *
 * @param    p_resource  Pointer to the resource to add to the list.
 *
 * @note     (1) This function must be called after the HTTP-s init but before the HTTP-s start.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsREST_Mem_AllocResource(CPU_INT32U                list_ID,
                                        const HTTPs_REST_RESOURCE *p_resource)
{
  HTTPs_REST_RESOURCE_LIST  *p_list;
  HTTPs_REST_RESOURCE_ENTRY *p_entry;
  RTOS_ERR                  local_err;

  p_list = HTTPsREST_Mem_GetResourceList(list_ID);
  if (p_list == DEF_NULL) {
    return (DEF_FAIL);
  }

  p_entry = (HTTPs_REST_RESOURCE_ENTRY *)Mem_DynPoolBlkGet(&HTTPsREST_Mem_ResourcePool,
                                                           &local_err);
  if (p_entry == DEF_NULL) {
    return (DEF_FAIL);
  }

  p_entry->ResourcePtr = p_resource;

  SList_Push(&p_list->ListHeadPtr, &p_entry->ListNode);
  SList_Sort(&p_list->ListHeadPtr, HTTPsREST_ResourceListCompare);

  PP_UNUSED_PARAM(local_err);

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       HTTPsREST_Mem_AllocRequest()
 *
 * @brief    Allocate a REST request data structure.
 *
 * @param    p_inst_data     Pointer to REST instance with the pool to allocate on.
 *
 * @return   DEF_NULL    if out of memory.
 *           OBJ         otherwise
 *******************************************************************************************************/
HTTPs_REST_REQUEST *HTTPsREST_Mem_AllocRequest(HTTPs_REST_INST_DATA *p_inst_data)
{
  HTTPs_REST_REQUEST *p_req;
  RTOS_ERR           local_err;

  p_req = (HTTPs_REST_REQUEST *)Mem_DynPoolBlkGet(&p_inst_data->Pool, &local_err);

  PP_UNUSED_PARAM(local_err);

  return (p_req);
}

/****************************************************************************************************//**
 *                                       HTTPsREST_Mem_FreeRequest()
 *
 * @brief    Frees a previously allocated request.
 *
 * @param    p_inst_data     Pointer to REST instance data containing the pool to free the request.
 *
 * @param    p_request       Pointer to REST request to free.
 *******************************************************************************************************/
void HTTPsREST_Mem_FreeRequest(HTTPs_REST_INST_DATA *p_inst_data,
                               HTTPs_REST_REQUEST   *p_request)
{
  RTOS_ERR local_err;

  Mem_DynPoolBlkFree(&p_inst_data->Pool,
                     p_request,
                     &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_SERVER_AVAIL
