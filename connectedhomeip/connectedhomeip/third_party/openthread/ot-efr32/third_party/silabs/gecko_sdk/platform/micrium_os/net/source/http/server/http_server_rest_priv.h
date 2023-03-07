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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef _HTTP_SERVER_REST_PRIV_H_
#define _HTTP_SERVER_REST_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/http_server.h>
#include  <net/include/http_server_addon_rest.h>

#include  <cpu/include/cpu.h>
#include  <common/source/collections/slist_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define HTTPs_REST_MAX_PUBLISHED_RESOURCE     50

/********************************************************************************************************
 ********************************************************************************************************
 *                                              DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           REST RESOURCE ENTRY TYPE
 *******************************************************************************************************/

typedef  struct  https_rest_resource_entry {
  const HTTPs_REST_RESOURCE *ResourcePtr;
  SLIST_MEMBER              ListNode;
} HTTPs_REST_RESOURCE_ENTRY;

/********************************************************************************************************
 *                                         REST RESOURCE LIST TYPE
 *******************************************************************************************************/

typedef  struct  https_rest_resource_list {
  CPU_INT32U   Id;
  SLIST_MEMBER *ListHeadPtr;
  SLIST_MEMBER ListNode;
} HTTPs_REST_RESOURCE_LIST;

/********************************************************************************************************
 *                                           REST REQUEST TYPE
 *******************************************************************************************************/

typedef  struct  https_rest_request {
  const HTTPs_REST_RESOURCE *ResourcePtr;
  HTTPs_REST_MATCHED_URI    URI;
  HTTPs_REST_HOOK_FNCT      Hook;
  void                      *DataPtr;
} HTTPs_REST_REQUEST;

/********************************************************************************************************
 *                                    REST HTTP INSTANCE DATA TYPE
 *******************************************************************************************************/

typedef  struct  https_rest_inst_data {
  MEM_DYN_POOL Pool;
} HTTPs_REST_INST_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                            MEMORY MODULE
 *******************************************************************************************************/

HTTPs_REST_INST_DATA *HTTPsREST_Mem_Init_Pools(CPU_SIZE_T max_request);

HTTPs_REST_RESOURCE_LIST *HTTPsREST_Mem_GetResourceList(CPU_INT32U list_ID);

HTTPs_REST_REQUEST *HTTPsREST_Mem_AllocRequest(HTTPs_REST_INST_DATA *p_inst_data);

void HTTPsREST_Mem_FreeRequest(HTTPs_REST_INST_DATA *p_inst_data,
                               HTTPs_REST_REQUEST   *p_request);

CPU_BOOLEAN HTTPsREST_Mem_AllocResource(CPU_INT32U                listID,
                                        const HTTPs_REST_RESOURCE *resource);

/********************************************************************************************************
 *                                      HOOK FUNCTION PROTOTYPES
 *******************************************************************************************************/

CPU_BOOLEAN HTTPsREST_Init(const HTTPs_INSTANCE *p_instance,
                           const void           *p_cfg);

CPU_BOOLEAN HTTPsREST_Authenticate(const HTTPs_INSTANCE *p_instance,
                                   HTTPs_CONN           *p_conn,
                                   const void           *p_cfg);

CPU_BOOLEAN HTTPsREST_RxHeader(const HTTPs_INSTANCE *p_instance,
                               const HTTPs_CONN     *p_conn,
                               const void           *p_cfg,
                               HTTP_HDR_FIELD       hdr_field);

CPU_BOOLEAN HTTPsREST_RxBody(const HTTPs_INSTANCE *p_instance,
                             HTTPs_CONN           *p_conn,
                             const void           *p_cfg,
                             void                 *p_buf,
                             const CPU_SIZE_T     buf_size,
                             CPU_SIZE_T           *p_buf_size_used);

CPU_BOOLEAN HTTPsREST_ReqRdySignal(const HTTPs_INSTANCE *p_instance,
                                   HTTPs_CONN           *p_conn,
                                   const void           *p_cfg,
                                   const HTTPs_KEY_VAL  *p_data);

CPU_BOOLEAN HTTPsREST_GetChunk(const HTTPs_INSTANCE *p_instance,
                               HTTPs_CONN           *p_conn,
                               const void           *p_cfg,
                               void                 *p_buf,
                               CPU_SIZE_T           buf_len_max,
                               CPU_SIZE_T           *len_tx);

void HTTPsREST_OnTransComplete(const HTTPs_INSTANCE *p_instance,
                               HTTPs_CONN           *p_conn,
                               const void           *p_cfg);

void HTTPsREST_OnConnClosed(const HTTPs_INSTANCE *p_instance,
                            HTTPs_CONN           *p_conn,
                            const void           *p_cfg);

/********************************************************************************************************
 *                                     INTERNAL FUNCTION PROTOTYPES
 *******************************************************************************************************/

CPU_BOOLEAN HTTPsREST_ResourceListCompare(SLIST_MEMBER *p_item_l,
                                          SLIST_MEMBER *p_item_r);

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _HTTP_SERVER_REST_PRIV_H_
