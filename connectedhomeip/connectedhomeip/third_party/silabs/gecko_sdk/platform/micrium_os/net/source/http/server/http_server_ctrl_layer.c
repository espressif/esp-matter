/***************************************************************************//**
 * @file
 * @brief Network - HTTP Server Control Layer Add-On
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

#include  "http_server_ctrl_layer_priv.h"

#include  <net/include/http_server.h>

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                   (NET, HTTP)
#define  RTOS_MODULE_CUR                               RTOS_CFG_MODULE_NET

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const HTTPs_HOOK_CFG HTTPsCtrlLayer_HookCfg = {
  HTTPsCtrlLayer_OnInstanceInit,
  HTTPsCtrlLayer_OnReqRxHdr,
  HTTPsCtrlLayer_OnReq,
  HTTPsCtrlLayer_OnReqRxBody,
  HTTPsCtrlLayer_OnReqRdySignal,
  HTTPsCtrlLayer_OnReqRdyPoll,
  HTTPsCtrlLayer_OnRespTxHdr,
  HTTPsCtrlLayer_OnRespToken,
  HTTPsCtrlLayer_OnRespChunk,
  HTTPsCtrlLayer_OnTransComplete,
  HTTPsCtrlLayer_OnErr,
  HTTPsCtrlLayer_OnErrFileGet,
  HTTPsCtrlLayer_OnConnClose
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL GLOBAL MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  INST_SCOPE_ALLOC()             HTTPs_CTRL_LAYER_INST_DATA *__M_InstData

#define  INST_SCOPE_ENTER(id, p_instance)                                                  \
  {                                                                                        \
    __M_InstData = (HTTPs_CTRL_LAYER_INST_DATA *)p_instance->DataPtr;                      \
    ((HTTPs_INSTANCE *)p_instance)->DataPtr = HTTPsCtrlLayer_InstDataGet((CPU_INT32U)  id, \
                                                                         __M_InstData);
// *INDENT-OFF*
#define  INST_SCOPE_EXIT(id, p_instance)                  \
  HTTPsCtrlLayer_InstDataSet((CPU_INT32U)id,              \
                             __M_InstData,                \
                             p_instance->DataPtr);        \
  ((HTTPs_INSTANCE *)p_instance)->DataPtr = __M_InstData; \
  }
// *INDENT-ON*

#define  CONN_SCOPE_ALLOC()             INST_SCOPE_ALLOC(); \
  HTTPs_CTRL_LAYER_CONN_DATA *__M_ConnData

#define  CONN_SCOPE_ENTER(id, p_instance, p_conn)                         \
  INST_SCOPE_ENTER(id, p_instance)                                        \
  {                                                                       \
    __M_ConnData = (HTTPs_CTRL_LAYER_CONN_DATA *)p_conn->ConnDataPtr;     \
    p_conn->ConnDataPtr = HTTPsCtrlLayer_ConnDataEntryGet((CPU_INT32U)id, \
                                                          __M_InstData,   \
                                                          __M_ConnData);  \

// *INDENT-OFF*
#define  CONN_SCOPE_EXIT(id, p_instance, p_conn)        \
  HTTPsCtrlLayer_ConnDataEntrySet((CPU_INT32U)   id,    \
                                  __M_InstData,         \
                                  __M_ConnData,         \
                                  p_conn->ConnDataPtr); \
  p_conn->ConnDataPtr = __M_ConnData;                   \
  }                                                       \
  INST_SCOPE_EXIT(id, p_instance)
// *INDENT-ON*

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static HTTPs_CTRL_LAYER_CONN_DATA *HTTPsCtrlLayer_CreateConnDataIfNull(HTTPs_CTRL_LAYER_INST_DATA *p_inst_data,
                                                                       HTTPs_CONN                 *p_conn);

static void HTTPsCtrlLayer_InstDataSet(CPU_INT32U                 id,
                                       HTTPs_CTRL_LAYER_INST_DATA *p_inst_data,
                                       void                       *p_val);

static void *HTTPsCtrlLayer_InstDataGet(CPU_INT32U                 id,
                                        HTTPs_CTRL_LAYER_INST_DATA *p_inst_data);

static void HTTPsCtrlLayer_ConnDataEntrySet(CPU_INT32U                 id,
                                            HTTPs_CTRL_LAYER_INST_DATA *p_inst_data,
                                            HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data,
                                            void                       *p_val);

static void *HTTPsCtrlLayer_ConnDataEntryGet(CPU_INT32U                 id,
                                             HTTPs_CTRL_LAYER_INST_DATA *p_inst_data,
                                             HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data);

static CPU_BOOLEAN HTTPsCtrlLayer_InstanceInit(CPU_INT32U                     id,
                                               const HTTPs_INSTANCE_INIT_HOOK instance_init_fnct,
                                               HTTPs_INSTANCE                 *p_instance,
                                               const void                     *p_hook_cfg);

static CPU_BOOLEAN HTTPsCtrlLayer_OnReqRx(CPU_INT32U     id,
                                          HTTPs_REQ_HOOK req_fnct,
                                          HTTPs_INSTANCE *p_instance,
                                          HTTPs_CONN     *p_conn,
                                          const void     *p_hook_cfg);

static CPU_BOOLEAN HTTPsCtrlLayer_RxHdr(CPU_INT32U            id,
                                        HTTPs_REQ_HDR_RX_HOOK rx_hdr_fnct,
                                        HTTPs_INSTANCE        *p_instance,
                                        HTTPs_CONN            *p_conn,
                                        const void            *p_hook_cfg,
                                        HTTP_HDR_FIELD        hdr_field);

static CPU_BOOLEAN HTTPsCtrlLayer_TxHdr(CPU_INT32U             id,
                                        HTTPs_RESP_HDR_TX_HOOK tx_hdr_fnct,
                                        HTTPs_INSTANCE         *p_instance,
                                        HTTPs_CONN             *p_conn,
                                        const void             *p_hook_cfg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           HOOK FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       HTTPsCtrlLayer_OnInstanceInit()
 *
 * @brief    This function is bound to the instance initialization of HTTP server.
 *           It calls the instance initialization function of each sub-modules(auth and app).
 *
 * @param    p_instance  Pointer to HTTPs instance.
 *
 * @param    p_cfg       Pointer to Control Layer configuration.
 *
 * @note         (1) This function is HTTP server hook function.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsCtrlLayer_OnInstanceInit(const HTTPs_INSTANCE *p_instance,
                                          const void           *p_cfg)
{
  HTTPs_CTRL_LAYER_CFG_LIST  *p_cfg_list;
  HTTPs_CTRL_LAYER_AUTH_INST *p_auth_inst;
  HTTPs_CTRL_LAYER_APP_INST  *p_app_inst;
  HTTPs_CTRL_LAYER_CFG       *p_ctrl_layer_cfg;
  HTTPs_CTRL_LAYER_INST_DATA *p_inst_data;
  CPU_INT16U                 ctrl_layer_cfg_ix;
  CPU_INT16U                 cfg_ix;
  CPU_INT16U                 cfg_nbr;
  CPU_INT16U                 auth_insts_nbr;
  CPU_INT16U                 app_insts_nbr;
  CPU_SIZE_T                 ctrl_insts_def;
  CPU_BOOLEAN                result;

  p_cfg_list = (HTTPs_CTRL_LAYER_CFG_LIST *)p_cfg;
  ctrl_insts_def = 0;

  if (p_cfg_list == DEF_NULL) {                                 // If no config has been added to the control layer ...
    return (DEF_OK);                                            // There is nothing to do.
  }

  cfg_nbr = p_cfg_list->Size;

  //                                                               Count the number of control instances.
  for (ctrl_layer_cfg_ix = 0; ctrl_layer_cfg_ix < cfg_nbr; ++ctrl_layer_cfg_ix) {
    p_ctrl_layer_cfg = p_cfg_list->CfgsPtr[ctrl_layer_cfg_ix];
    ctrl_insts_def += p_ctrl_layer_cfg->AuthInstsNbr + p_ctrl_layer_cfg->AppInstsNbr;
  }

  p_inst_data = HTTPsCtrlLayerMem_InstDataAlloc();
  if (p_inst_data == DEF_NULL) {
    return (DEF_FAIL);
  }

  p_inst_data->InstDataHeadPtr = DEF_NULL;

  //                                                               Cast p_instance out of const can't be done otherwise.
  ((HTTPs_INSTANCE *)p_instance)->DataPtr = p_inst_data;

  //                                                               Initialize the substitution pool for instance data.
  result = HTTPsCtrlLayerMem_InstDataPoolInit(p_inst_data,
                                              ctrl_insts_def);
  if (result != DEF_OK) {
    return (DEF_FAIL);
  }

  //                                                               Initialize the pool for Ctrl Layer conn data.
  result = HTTPsCtrlLayerMem_ConnDataPoolInit(p_inst_data,
                                              p_instance->CfgPtr->ConnNbrMax);
  if (result != DEF_OK) {
    return (DEF_FAIL);
  }

  //                                                               Initialize the conn data substitution pool.
  result = HTTPsCtrlLayerMem_ConnDataEntryPoolInit(p_inst_data,
                                                   ctrl_insts_def);
  if (result != DEF_OK) {
    return (DEF_FAIL);
  }

  //                                                               For all the configuration of the control layer.
  for (ctrl_layer_cfg_ix = 0; ctrl_layer_cfg_ix < cfg_nbr; ++ctrl_layer_cfg_ix) {
    p_ctrl_layer_cfg = p_cfg_list->CfgsPtr[ctrl_layer_cfg_ix];

    auth_insts_nbr = p_ctrl_layer_cfg->AuthInstsNbr;
    app_insts_nbr = p_ctrl_layer_cfg->AppInstsNbr;

    //                                                             Initialize the authentication services.
    for (cfg_ix = 0; cfg_ix < auth_insts_nbr; ++cfg_ix) {
      p_auth_inst = p_ctrl_layer_cfg->AuthInstsPtr[cfg_ix];

      result = HTTPsCtrlLayer_InstanceInit((CPU_INT32U)p_auth_inst,
                                           p_auth_inst->HooksPtr->OnInstanceInit,
                                           (HTTPs_INSTANCE *)p_instance,
                                           p_auth_inst->HooksCfgPtr);
      if (result != DEF_OK) {
        return (DEF_FAIL);
      }
    }

    //                                                             Initialize the application services.
    for (cfg_ix = 0; cfg_ix < app_insts_nbr; ++cfg_ix) {
      p_app_inst = p_ctrl_layer_cfg->AppInstsPtr[cfg_ix];

      result = HTTPsCtrlLayer_InstanceInit((CPU_INT32U)      p_app_inst,
                                           p_app_inst->HooksPtr->OnInstanceInit,
                                           (HTTPs_INSTANCE *)p_instance,
                                           p_app_inst->HooksCfgPtr);
      if (result != DEF_OK) {
        return (DEF_FAIL);
      }
    }
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       HTTPsCtrlLayer_OnReqRxHdr()
 *
 * @brief    Bound to the RxHeader hook of HTTPs.
 *           Tries to determine if a given header is useful for further process.
 *
 * @param    p_instance  Pointer to HTTPs instance object.
 *
 * @param    p_conn      Pointer to HTTPs connection object.
 *
 * @param    p_cfg       Pointer to Ctrl Layer configuration.
 *
 * @param    hdr_field   HTTP header to check.
 *
 * @return   DEF_YES    if the header must be kept.
 *           DEF_NO     otherwise.
 *
 * @note     (1) This function is HTTP server hook function.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsCtrlLayer_OnReqRxHdr(const HTTPs_INSTANCE *p_instance,
                                      const HTTPs_CONN     *p_conn,
                                      const void           *p_cfg,
                                      HTTP_HDR_FIELD       hdr_field)
{
  HTTPs_CTRL_LAYER_CFG_LIST  *p_cfg_list;
  HTTPs_CTRL_LAYER_CFG       *p_ctrl_layer_cfg;
  HTTPs_CTRL_LAYER_AUTH_INST *p_auth_inst;
  HTTPs_CTRL_LAYER_APP_INST  *p_app_inst;
  CPU_INT16U                 ctrl_layer_cfg_ix;
  CPU_INT16U                 cfg_ix;
  CPU_INT16U                 cfg_nbr;
  CPU_INT16U                 auth_insts_nbr;
  CPU_INT16U                 app_insts_nbr;
  CPU_BOOLEAN                keep_hdr;

  p_cfg_list = (HTTPs_CTRL_LAYER_CFG_LIST *)p_cfg;

  if (p_cfg_list == DEF_NULL) {
    return (DEF_NO);
  }

  cfg_nbr = p_cfg_list->Size;
  keep_hdr = DEF_NO;

  HTTPsCtrlLayer_CreateConnDataIfNull((HTTPs_CTRL_LAYER_INST_DATA *)p_instance->DataPtr,
                                      (HTTPs_CONN *)p_conn);

  //                                                               For all the configurations of the control layer ...
  //                                                               .. find at least one auth or app service whom ...
  //                                                               ... require this header.
  for (ctrl_layer_cfg_ix = 0; ctrl_layer_cfg_ix < cfg_nbr && keep_hdr == DEF_NO; ++ctrl_layer_cfg_ix) {
    p_ctrl_layer_cfg = p_cfg_list->CfgsPtr[ctrl_layer_cfg_ix];

    auth_insts_nbr = p_ctrl_layer_cfg->AuthInstsNbr;
    app_insts_nbr = p_ctrl_layer_cfg->AppInstsNbr;

    for (cfg_ix = 0; cfg_ix < auth_insts_nbr && keep_hdr == DEF_NO; ++cfg_ix) {
      p_auth_inst = p_ctrl_layer_cfg->AuthInstsPtr[cfg_ix];

      keep_hdr = HTTPsCtrlLayer_RxHdr((CPU_INT32U)p_auth_inst,
                                      p_auth_inst->HooksPtr->OnReqHdrRx,
                                      (HTTPs_INSTANCE *)p_instance,
                                      (HTTPs_CONN *)p_conn,
                                      p_cfg,
                                      hdr_field);
    }

    for (cfg_ix = 0; cfg_ix < app_insts_nbr && keep_hdr == DEF_NO; ++cfg_ix) {
      p_app_inst = p_ctrl_layer_cfg->AppInstsPtr[cfg_ix];

      keep_hdr = HTTPsCtrlLayer_RxHdr((CPU_INT32U)p_app_inst,
                                      p_app_inst->HooksPtr->OnReqHdrRx,
                                      (HTTPs_INSTANCE *)p_instance,
                                      (HTTPs_CONN *)p_conn,
                                      p_cfg,
                                      hdr_field);
    }
  }

  return (keep_hdr);
}

/****************************************************************************************************//**
 *                                           HTTPsCtrlLayer_OnReq()
 *
 * @brief    Bound to the ConnRequest hook of HTTPs.
 *           Tries to find the matching authentications and application for the request.
 *
 * @param    p_instance  Pointer to HTTPs instance object.
 *
 * @param    p_conn      Pointer to HTTPs connection object.
 *
 * @param    p_cfg       Pointer to Control Layer configuration object.
 *
 * @return   DEF_OK    if an appropriate match has been found.
 *           DEF_FAIL  otherwise
 *
 * @note     (1) This function is HTTP server hook function.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsCtrlLayer_OnReq(const HTTPs_INSTANCE *p_instance,
                                 HTTPs_CONN           *p_conn,
                                 const void           *p_cfg)
{
  HTTPs_CTRL_LAYER_INST_DATA *p_inst_data;
  HTTPs_CTRL_LAYER_CFG_LIST  *p_cfg_list;
  HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data;
  HTTPs_CTRL_LAYER_AUTH_INST *p_auth_inst;
  HTTPs_CTRL_LAYER_APP_INST  *p_app_inst;
  HTTPs_CTRL_LAYER_CFG       *p_ctrl_layer_cfg;
  CPU_INT16U                 ctrl_layer_cfg_ix;
  CPU_INT16U                 cfg_ix;
  CPU_INT16U                 cfg_nbr;
  CPU_INT16U                 auth_insts_nbr;
  CPU_INT16U                 app_insts_nbr;
  CPU_BOOLEAN                auth_succeed;
  CPU_BOOLEAN                app_found;

  p_inst_data = (HTTPs_CTRL_LAYER_INST_DATA *)p_instance->DataPtr;
  p_cfg_list = (HTTPs_CTRL_LAYER_CFG_LIST *)p_cfg;

  if (p_cfg_list == DEF_NULL) {
    return (DEF_FAIL);
  }

  cfg_nbr = p_cfg_list->Size;

  p_conn_data = HTTPsCtrlLayer_CreateConnDataIfNull(p_inst_data, p_conn);
  if (p_conn_data == DEF_NULL) {
    return (DEF_FAIL);
  }

  app_found = DEF_NO;

  //                                                               For all the control layer configurations.
  for (ctrl_layer_cfg_ix = 0; (ctrl_layer_cfg_ix < cfg_nbr) && (app_found == DEF_FALSE); ++ctrl_layer_cfg_ix) {
    auth_succeed = DEF_YES;

    p_ctrl_layer_cfg = p_cfg_list->CfgsPtr[ctrl_layer_cfg_ix];
    auth_insts_nbr = p_ctrl_layer_cfg->AuthInstsNbr;
    app_insts_nbr = p_ctrl_layer_cfg->AppInstsNbr;

    //                                                             Try to pass all the authentication. Exit on a fail.
    for (cfg_ix = 0; cfg_ix < auth_insts_nbr && auth_succeed == DEF_TRUE; ++cfg_ix) {
      p_auth_inst = p_ctrl_layer_cfg->AuthInstsPtr[cfg_ix];

      auth_succeed = HTTPsCtrlLayer_OnReqRx((CPU_INT32U)      p_auth_inst,
                                            p_auth_inst->HooksPtr->OnReqAuth,
                                            (HTTPs_INSTANCE *)p_instance,
                                            p_conn,
                                            p_auth_inst->HooksCfgPtr);
    }

    //                                                             If all the authentications services succeeded.
    if (auth_succeed == DEF_TRUE) {
      //                                                           Try to find an app service matching with this req.
      for (cfg_ix = 0; (cfg_ix < app_insts_nbr) && (app_found == DEF_NO); ++cfg_ix) {
        p_app_inst = p_ctrl_layer_cfg->AppInstsPtr[cfg_ix];

        app_found = HTTPsCtrlLayer_OnReqRx((CPU_INT32U)      p_app_inst,
                                           p_app_inst->HooksPtr->OnReq,
                                           (HTTPs_INSTANCE *)p_instance,
                                           p_conn,
                                           p_app_inst->HooksCfgPtr);
        if (app_found == DEF_YES) {
          p_conn_data->TargetCfgPtr = p_ctrl_layer_cfg;
          p_conn_data->TargetAppInstPtr = p_app_inst;
        }
      }
    }

    if (app_found == DEF_NO) {
      //                                                           Free the memory used by successful Authentication.
      if (auth_succeed == DEF_NO) {
        for (; cfg_ix > 0; --cfg_ix) {
          p_auth_inst = p_ctrl_layer_cfg->AuthInstsPtr[cfg_ix - 1];

          if (p_auth_inst->HooksPtr->OnConnClose != DEF_NULL) {
            CONN_SCOPE_ALLOC();
            CONN_SCOPE_ENTER(p_auth_inst, p_instance, p_conn);
            p_auth_inst->HooksPtr->OnConnClose(p_instance,
                                               p_conn,
                                               p_auth_inst->HooksCfgPtr);
            CONN_SCOPE_EXIT(p_auth_inst, p_instance, p_conn);
          }
        }
      }

      HTTPsCtrlLayer_ConnDataEntriesFree(p_inst_data, p_conn_data);
    }
  }

  if (app_found == DEF_NO) {
    HTTPsCtrlLayer_ConnDataFree(p_inst_data, p_conn_data);
    p_conn->ConnDataPtr = DEF_NULL;
  }

  return (app_found);
}

/****************************************************************************************************//**
 *                                       HTTPsCtrlLayer_OnReqRxBody()
 *
 * @brief    Bound to RxBody hook of HTTPs.
 *           Provide the body data to the application associated to the request.
 *
 * @param    p_instance          Pointer to HTTPs instance object.
 *
 * @param    p_conn              Pointer to HTTPs connection object.
 *
 * @param    p_cfg               Pointer to Ctrl Layer configuration.
 *
 * @param    p_buf               Pointer to the data buffer.
 *
 * @param    buf_size            Size of the data rx available inside the buffer.
 *
 * @param    p_buf_size_used     Pointer to the variable that will received the length of the data consumed by the app.
 *
 * @return   DEF_YES    To continue with the data reception.
 *           DEF_NO     If the application doesn't want to rx data anymore.
 *
 * @note     (1) This function is HTTP server hook function.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsCtrlLayer_OnReqRxBody(const HTTPs_INSTANCE *p_instance,
                                       HTTPs_CONN           *p_conn,
                                       const void           *p_cfg,
                                       void                 *p_buf,
                                       const CPU_SIZE_T     buf_size,
                                       CPU_SIZE_T           *p_buf_size_used)
{
  HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data;
  HTTPs_CTRL_LAYER_APP_INST  *p_app_inst;
  CPU_BOOLEAN                hook_continue;

  PP_UNUSED_PARAM(p_cfg);
  PP_UNUSED_PARAM(p_buf);
  PP_UNUSED_PARAM(buf_size);

  p_conn_data = (HTTPs_CTRL_LAYER_CONN_DATA *)p_conn->ConnDataPtr;
  if (p_conn_data == DEF_NULL) {
    return (DEF_YES);
  }

  p_app_inst = p_conn_data->TargetAppInstPtr;

  if (p_app_inst->HooksPtr->OnReqBodyRx == DEF_NULL) {
    return (DEF_YES);
  }

  CONN_SCOPE_ALLOC();

  CONN_SCOPE_ENTER(p_app_inst, p_instance, p_conn);

  hook_continue = p_app_inst->HooksPtr->OnReqBodyRx(p_instance,
                                                    p_conn,
                                                    p_app_inst->HooksCfgPtr,
                                                    p_conn->RxBufPtr,
                                                    p_conn->RxBufLenRem,
                                                    p_buf_size_used);

  CONN_SCOPE_EXIT(p_app_inst, p_instance, p_conn);

  return (hook_continue);
}

/****************************************************************************************************//**
 *                                       HTTPsCtrlLayer_OnReqRdySignal()
 *
 * @brief    Bound to Request Ready Signal hook from HTTPs.
 *           Signals the application that the HTTP request have been fully parsed, it is now the time
 *           to provide a response.
 *
 * @param    p_instance  Pointer to HTTPs instance object.
 *
 * @param    p_conn      Pointer to HTTPs connection object.
 *
 * @param    p_cfg       Pointer to Ctrl Layer configuration.
 *
 * @param    p_data      UNUSED
 *
 * @return   DEF_YES  if the p_conn contains everything necessary to build a response.
 *           DEF_NO   otherwise.
 *
 * @note     (1) This function is HTTP server hook function.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsCtrlLayer_OnReqRdySignal(const HTTPs_INSTANCE *p_instance,
                                          HTTPs_CONN           *p_conn,
                                          const void           *p_cfg,
                                          const HTTPs_KEY_VAL  *p_data)
{
  HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data;
  HTTPs_CTRL_LAYER_APP_INST  *p_app_inst;
  CPU_BOOLEAN                done;

  PP_UNUSED_PARAM(p_cfg);

  p_conn_data = (HTTPs_CTRL_LAYER_CONN_DATA *)p_conn->ConnDataPtr;

  if ((p_conn_data == DEF_NULL)
      || (p_conn_data->TargetAppInstPtr->HooksPtr->OnReqSignal == DEF_NULL)) {
    return (DEF_YES);
  }

  p_app_inst = p_conn_data->TargetAppInstPtr;

  CONN_SCOPE_ALLOC();

  CONN_SCOPE_ENTER(p_app_inst, p_instance, p_conn);

  done = p_app_inst->HooksPtr->OnReqSignal(p_instance,
                                           p_conn,
                                           p_app_inst->HooksCfgPtr,
                                           p_data);

  CONN_SCOPE_EXIT(p_app_inst, p_instance, p_conn);

  return (done);
}

/****************************************************************************************************//**
 *                                       HTTPsCtrlLayer_OnReqRdyPoll()
 *
 * @brief    Bound to Request Ready Poll hook from HTTPs.
 *           Polls the application for a response.
 *
 * @param    p_instance  Pointer to HTTPs instance object.
 *
 * @param    p_conn      Pointer to HTTPs connection object.
 *
 * @param    p_cfg       Pointer to Control Layer configuration.
 *
 * @return   DEF_YES    if the p_conn contains everything necessary to build a response.
 *           DEF_NO     otherwise.
 *
 * @note     (1) This function is HTTP server hook function.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsCtrlLayer_OnReqRdyPoll(const HTTPs_INSTANCE *p_instance,
                                        HTTPs_CONN           *p_conn,
                                        const void           *p_cfg)
{
  HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data;
  HTTPs_CTRL_LAYER_APP_INST  *p_app_inst;
  CPU_BOOLEAN                done;

  PP_UNUSED_PARAM(p_cfg);

  p_conn_data = (HTTPs_CTRL_LAYER_CONN_DATA *)p_conn->ConnDataPtr;

  if ((p_conn_data == DEF_NULL)
      || (p_conn_data->TargetAppInstPtr->HooksPtr->OnReqPoll == DEF_NULL)) {
    return (DEF_YES);
  }

  p_app_inst = p_conn_data->TargetAppInstPtr;

  CONN_SCOPE_ALLOC();

  CONN_SCOPE_ENTER(p_app_inst, p_instance, p_conn);

  done = p_app_inst->HooksPtr->OnReqPoll(p_instance,
                                         p_conn,
                                         p_app_inst->HooksCfgPtr);

  CONN_SCOPE_EXIT(p_app_inst, p_instance, p_conn);

  return (done);
}

/****************************************************************************************************//**
 *                                           HTTPsCtrlLayer_OnTxHdr()
 *
 * @brief    Bound to Tx header of HTTPs hook.
 *
 * @param    p_instance  Pointer to HTTPs instance object.
 *
 * @param    p_conn      Pointer to HTTPs connection object.
 *
 * @param    p_cfg       Pointer to Ctrl Layer configuration
 *
 * @return   DEF_YES    if all header have been sent.
 *           DEF_NO     otherwise.
 *
 * @note     (1) This function is HTTP server hook function.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsCtrlLayer_OnRespTxHdr(const HTTPs_INSTANCE *p_instance,
                                       HTTPs_CONN           *p_conn,
                                       const void           *p_cfg)
{
  HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data;
  HTTPs_CTRL_LAYER_CFG       *p_ctrl_layer_cfg;
  HTTPs_CTRL_LAYER_AUTH_INST *p_auth_inst;
  HTTPs_CTRL_LAYER_APP_INST  *p_app_inst;
  CPU_INT16U                 cfg_ix;
  CPU_INT16U                 auth_insts_nbr;
  CPU_BOOLEAN                send_hdr = DEF_NO;

  p_conn_data = (HTTPs_CTRL_LAYER_CONN_DATA *)p_conn->ConnDataPtr;

  if (p_conn_data == DEF_NULL) {
    return (send_hdr);
  }

  p_ctrl_layer_cfg = p_conn_data->TargetCfgPtr;
  auth_insts_nbr = p_ctrl_layer_cfg->AuthInstsNbr;

  p_app_inst = p_conn_data->TargetAppInstPtr;

  send_hdr = HTTPsCtrlLayer_TxHdr((CPU_INT32U)p_app_inst,
                                  p_app_inst->HooksPtr->OnRespHdrTx,
                                  (HTTPs_INSTANCE *)p_instance,
                                  p_conn,
                                  p_cfg);

  for (cfg_ix = 0; cfg_ix < auth_insts_nbr; ++cfg_ix) {
    p_auth_inst = p_ctrl_layer_cfg->AuthInstsPtr[cfg_ix];

    send_hdr |= HTTPsCtrlLayer_TxHdr((CPU_INT32U)p_auth_inst,
                                     p_auth_inst->HooksPtr->OnRespHdrTx,
                                     (HTTPs_INSTANCE *)p_instance,
                                     p_conn,
                                     p_cfg);
  }

  return (send_hdr);
}

/****************************************************************************************************//**
 *                                       HTTPsCtrlLayer_OnRespToken()
 *
 * @brief    Bound to Token Value Get hook from HTTPs.
 *
 * @param    p_instance      Pointer to HTTPs instance object.
 *
 * @param    p_conn          Pointer HTTPs connection object.
 *
 * @param    p_cfg           Pointer to Ctrl Layer configuration object.
 *
 * @param    p_token         Pointer to the token name.
 *
 * @param    token_len       Length of the token name.
 *
 * @param    p_val           Pointer to the value string.
 *
 * @param    val_len_max     Max len of the value string.
 *
 * @return   DEF_YES   if the token has been replaced.
 *           DEF_NO    otherwise
 *
 * @note     (1) This function is HTTP server hook function.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsCtrlLayer_OnRespToken(const HTTPs_INSTANCE *p_instance,
                                       HTTPs_CONN           *p_conn,
                                       const void           *p_cfg,
                                       const CPU_CHAR       *p_token,
                                       CPU_INT16U           token_len,
                                       CPU_CHAR             *p_val,
                                       CPU_INT16U           val_len_max)
{
  HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data;
  HTTPs_CTRL_LAYER_APP_INST  *p_app_inst;
  CPU_BOOLEAN                token_replaced;

  PP_UNUSED_PARAM(p_cfg);

  p_conn_data = (HTTPs_CTRL_LAYER_CONN_DATA *)p_conn->ConnDataPtr;

  if ((p_conn_data == DEF_NULL)
      || (p_conn_data->TargetAppInstPtr->HooksPtr->OnRespToken == DEF_NULL)) {
    return (DEF_YES);
  }

  p_app_inst = p_conn_data->TargetAppInstPtr;

  CONN_SCOPE_ALLOC();

  CONN_SCOPE_ENTER(p_app_inst, p_instance, p_conn);

  token_replaced = p_app_inst->HooksPtr->OnRespToken(p_instance,
                                                     p_conn,
                                                     p_app_inst->HooksCfgPtr,
                                                     p_token,
                                                     token_len,
                                                     p_val,
                                                     val_len_max);

  CONN_SCOPE_EXIT(p_app_inst, p_instance, p_conn);

  return (token_replaced);
}

/****************************************************************************************************//**
 *                                       HTTPsCtrlLayer_OnRespChunk()
 *
 * @brief    Bound to Chunked Data Get hook from HTTP-s.
 *
 * @param    p_instance      Pointer to HTTPs instance object.
 *
 * @param    p_conn          Pointer HTTPs connection object.
 *
 * @param    p_cfg           Pointer to Ctrl Layer configuration object.
 *
 * @param    p_buf           Pointer to the buffer to fill.
 *
 * @param    buf_len_max     Maximum length the buffer can contain.
 *
 * @param    p_len_tx        Variable that will received the length written in the buffer.
 *
 * @return   DEF_YES      if there is no more data to send.
 *           DEF_NO       otherwise.
 *
 * @note     (1) This function is HTTP server hook function.
 *******************************************************************************************************/
CPU_BOOLEAN HTTPsCtrlLayer_OnRespChunk(const HTTPs_INSTANCE *p_instance,
                                       HTTPs_CONN           *p_conn,
                                       const void           *p_cfg,
                                       void                 *p_buf,
                                       CPU_SIZE_T           buf_len_max,
                                       CPU_SIZE_T           *p_len_tx)
{
  HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data;
  HTTPs_CTRL_LAYER_APP_INST  *p_app_inst;
  CPU_BOOLEAN                last_chunk = DEF_YES;

  PP_UNUSED_PARAM(p_cfg);

  p_conn_data = (HTTPs_CTRL_LAYER_CONN_DATA *)p_conn->ConnDataPtr;

  if ((p_conn_data == DEF_NULL)
      || (p_conn_data->TargetAppInstPtr->HooksPtr->OnRespChunk == DEF_NULL)) {
    return (last_chunk);
  }

  p_app_inst = p_conn_data->TargetAppInstPtr;

  CONN_SCOPE_ALLOC();

  CONN_SCOPE_ENTER(p_app_inst, p_instance, p_conn);

  last_chunk = p_app_inst->HooksPtr->OnRespChunk(p_instance,
                                                 p_conn,
                                                 p_app_inst->HooksCfgPtr,
                                                 p_buf,
                                                 buf_len_max,
                                                 p_len_tx);

  CONN_SCOPE_EXIT(p_app_inst, p_instance, p_conn);

  return (last_chunk);
}

/****************************************************************************************************//**
 *                                       HTTPsCtrlLayer_OnTransComplete()
 *
 * @brief    Bound to TransComplete hook from HTTPs.
 *           Notifies all sub module of an HTTP Transaction completion.
 *
 * @param    p_instance  Pointer to HTTPs instance object.
 *
 * @param    p_conn      Pointer HTTPs connection object.
 *
 * @param    p_cfg       Pointer to Ctrl Layer configuration object.
 *
 * @note     (1) This function is HTTP server hook function.
 *******************************************************************************************************/
void HTTPsCtrlLayer_OnTransComplete(const HTTPs_INSTANCE *p_instance,
                                    HTTPs_CONN           *p_conn,
                                    const void           *p_cfg)
{
  HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data;
  HTTPs_CTRL_LAYER_CFG       *p_ctrl_layer_cfg;
  HTTPs_CTRL_LAYER_AUTH_INST *p_auth_inst;
  HTTPs_CTRL_LAYER_APP_INST  *p_app_inst;
  HTTPs_TRANS_COMPLETE_HOOK  trans_compl_fnct;
  CPU_INT16U                 cfg_ix;
  CPU_INT16U                 auth_insts_nbr;

  PP_UNUSED_PARAM(p_cfg);

  p_conn_data = (HTTPs_CTRL_LAYER_CONN_DATA *)p_conn->ConnDataPtr;

  if (p_conn_data == DEF_NULL) {
    return;
  }

  p_ctrl_layer_cfg = p_conn_data->TargetCfgPtr;

  auth_insts_nbr = p_ctrl_layer_cfg->AuthInstsNbr;

  //                                                               Call conn close only on the control layer cfg ...
  //                                                               ..  that actually worked on the request.
  for (cfg_ix = 0; cfg_ix < auth_insts_nbr; ++cfg_ix) {
    p_auth_inst = p_ctrl_layer_cfg->AuthInstsPtr[cfg_ix];

    trans_compl_fnct = p_auth_inst->HooksPtr->OnTransComplete;
    if (trans_compl_fnct != DEF_NULL) {
      CONN_SCOPE_ALLOC();

      CONN_SCOPE_ENTER(p_auth_inst, p_instance, p_conn);

      trans_compl_fnct(p_instance,
                       p_conn,
                       p_auth_inst->HooksCfgPtr);

      CONN_SCOPE_EXIT(p_auth_inst, p_instance, p_conn);
    }
  }

  p_app_inst = p_conn_data->TargetAppInstPtr;

  trans_compl_fnct = p_app_inst->HooksPtr->OnTransComplete;
  if (trans_compl_fnct != DEF_NULL) {
    CONN_SCOPE_ALLOC();

    CONN_SCOPE_ENTER(p_app_inst, p_instance, p_conn);

    trans_compl_fnct(p_instance,
                     p_conn,
                     p_app_inst->HooksCfgPtr);

    CONN_SCOPE_EXIT(p_app_inst, p_instance, p_conn);
  }

  HTTPsCtrlLayer_ConnDataFree((HTTPs_CTRL_LAYER_INST_DATA *)p_instance->DataPtr, p_conn_data);

  p_conn->ConnDataPtr = DEF_NULL;
}

/****************************************************************************************************//**
 *                                           HTTPsCtrlLayer_OnErr()
 *
 * @brief    Bound to ConnErr hook from HTTPs.
 *           Forwards the error to the application concerned if any.
 *
 * @param    p_instance  Pointer to HTTPs instance object.
 *
 * @param    p_conn      Pointer HTTPs connection object.
 *
 * @param    p_cfg       Pointer to Ctrl Layer configuration object.
 *
 * @param    err         HTTPs error code.
 *
 * @note     (1) This function is HTTP server hook function.
 *******************************************************************************************************/
void HTTPsCtrlLayer_OnErr(const HTTPs_INSTANCE *p_instance,
                          HTTPs_CONN           *p_conn,
                          const void           *p_cfg,
                          HTTPs_ERR            err)
{
  HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data;
  HTTPs_CTRL_LAYER_APP_INST  *p_app_inst;

  PP_UNUSED_PARAM(p_cfg);

  p_conn_data = (HTTPs_CTRL_LAYER_CONN_DATA *)p_conn->ConnDataPtr;

  if ((p_conn_data == DEF_NULL)
      || (p_conn_data->TargetAppInstPtr->HooksPtr->OnError == DEF_NULL)) {
    return;
  }

  p_app_inst = p_conn_data->TargetAppInstPtr;

  CONN_SCOPE_ALLOC();

  CONN_SCOPE_ENTER(p_app_inst, p_instance, p_conn);

  p_app_inst->HooksPtr->OnError(p_instance,
                                p_conn,
                                p_app_inst->HooksCfgPtr,
                                err);

  CONN_SCOPE_EXIT(p_app_inst, p_instance, p_conn);
}

/****************************************************************************************************//**
 *                                       HTTPsCtrlLayer_OnErrFileGet()
 *
 * @brief    Bound to ErrFileGet hook from HTTPs.
 *           Does nothing.
 *
 * @param    p_cfg           UNUSED
 *
 * @param    status_code     UNUSED
 *
 * @param    p_file_str      UNUSED
 *
 * @param    file_len_max    UNUSED
 *
 * @param    p_file_type     UNUSED
 *
 * @param    p_content_type  UNUSED
 *
 * @param    p_data          UNUSED
 *
 * @param    p_date_len      UNUSED
 *
 * @note     (1) This function is HTTP server hook function.
 *******************************************************************************************************/
void HTTPsCtrlLayer_OnErrFileGet(const void           *p_cfg,
                                 HTTP_STATUS_CODE     status_code,
                                 CPU_CHAR             *p_file_str,
                                 CPU_INT32U           file_len_max,
                                 HTTPs_BODY_DATA_TYPE *p_file_type,
                                 HTTP_CONTENT_TYPE    *p_content_type,
                                 void                 **p_data,
                                 CPU_INT32U           *p_date_len)
{
  PP_UNUSED_PARAM(p_cfg);
  PP_UNUSED_PARAM(status_code);
  PP_UNUSED_PARAM(p_file_str);
  PP_UNUSED_PARAM(file_len_max);
  PP_UNUSED_PARAM(p_file_type);
  PP_UNUSED_PARAM(p_content_type);
  PP_UNUSED_PARAM(p_data);
  PP_UNUSED_PARAM(p_date_len);
}

/****************************************************************************************************//**
 *                                       HTTPsCtrlLayer_OnConnClose()
 *
 * @brief    Bound to ConnClose hook from HTTPs.
 *           Notifies all sub module of a connection close.
 *
 * @param    p_instance  Pointer to HTTPs instance object.
 *
 * @param    p_conn      Pointer HTTPs connection object.
 *
 * @param    p_cfg       Pointer to Ctrl Layer configuration object.
 *
 * @note     (1) This function is HTTP server hook function.
 *******************************************************************************************************/
void HTTPsCtrlLayer_OnConnClose(const HTTPs_INSTANCE *p_instance,
                                HTTPs_CONN           *p_conn,
                                const void           *p_cfg)
{
  HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data;
  HTTPs_CTRL_LAYER_CFG       *p_ctrl_layer_cfg;
  HTTPs_CTRL_LAYER_AUTH_INST *p_auth_inst;
  HTTPs_CTRL_LAYER_APP_INST  *p_app_inst;
  HTTPs_CONN_CLOSE_HOOK      conn_close_fnct;
  CPU_INT16U                 cfg_ix;
  CPU_INT16U                 auth_insts_nbr;

  PP_UNUSED_PARAM(p_cfg);

  p_conn_data = (HTTPs_CTRL_LAYER_CONN_DATA *)p_conn->ConnDataPtr;

  if (p_conn_data == DEF_NULL) {
    return;
  }

  p_ctrl_layer_cfg = p_conn_data->TargetCfgPtr;

  auth_insts_nbr = p_ctrl_layer_cfg->AuthInstsNbr;

  //                                                               Call conn close only on the control layer cfg ...
  //                                                               ..  that actually worked on the request.
  for (cfg_ix = 0; cfg_ix < auth_insts_nbr; ++cfg_ix) {
    p_auth_inst = p_ctrl_layer_cfg->AuthInstsPtr[cfg_ix];

    conn_close_fnct = p_auth_inst->HooksPtr->OnConnClose;
    if (conn_close_fnct != DEF_NULL) {
      CONN_SCOPE_ALLOC();

      CONN_SCOPE_ENTER(p_auth_inst, p_instance, p_conn);

      conn_close_fnct(p_instance,
                      p_conn,
                      p_auth_inst->HooksCfgPtr);

      CONN_SCOPE_EXIT(p_auth_inst, p_instance, p_conn);
    }
  }

  p_app_inst = p_conn_data->TargetAppInstPtr;

  conn_close_fnct = p_app_inst->HooksPtr->OnConnClose;
  if (conn_close_fnct != DEF_NULL) {
    CONN_SCOPE_ALLOC();

    CONN_SCOPE_ENTER(p_app_inst, p_instance, p_conn);

    conn_close_fnct(p_instance,
                    p_conn,
                    p_app_inst->HooksCfgPtr);

    CONN_SCOPE_EXIT(p_app_inst, p_instance, p_conn);
  }

  HTTPsCtrlLayer_ConnDataFree((HTTPs_CTRL_LAYER_INST_DATA *)p_instance->DataPtr,
                              p_conn_data);

  p_conn->ConnDataPtr = DEF_NULL;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                   HTTPsCtrlLayer_CreateConnDataIfNull()
 *
 * @brief    Creates a CtrlLayer ConnData if it was null.
 *
 * @param    p_inst_data     Pointer to Ctrl Layer Instance Data.
 *
 * @param    p_conn          Pointer to HTTPs Connection object.
 *
 * @return   Pointer to Ctrl Layer Connection Data object.
 *           DEF_NULL if a ConnData couldn't be created.
 *
 * @note     (1) If not null, it will return the ConnDataPtr from p_conn.
 *******************************************************************************************************/
static HTTPs_CTRL_LAYER_CONN_DATA *HTTPsCtrlLayer_CreateConnDataIfNull(HTTPs_CTRL_LAYER_INST_DATA *p_inst_data,
                                                                       HTTPs_CONN                 *p_conn)
{
  if (p_conn->ConnDataPtr == DEF_NULL) {
    p_conn->ConnDataPtr = HTTPsCtrlLayerMem_ConnDataAlloc(p_inst_data);
    if (p_conn->ConnDataPtr == DEF_NULL) {
      CPU_SW_EXCEPTION(DEF_NULL);                               // Not suppose to happen.
    }
  }

  return ((HTTPs_CTRL_LAYER_CONN_DATA *)p_conn->ConnDataPtr);
}

/****************************************************************************************************//**
 *                                       HTTPsCtrlLayer_InstDataSet()
 *
 * @brief    Set the value of a substitution.
 *
 * @param    id              Instance data ID to be set
 *
 * @param    p_inst_data     Pointer to Ctrl Layer instance data.
 *
 * @param    p_val           Pointer to value of the instance data pointer.
 *******************************************************************************************************/
static void HTTPsCtrlLayer_InstDataSet(CPU_INT32U                 id,
                                       HTTPs_CTRL_LAYER_INST_DATA *p_inst_data,
                                       void                       *p_val)
{
  HTTPs_CTRL_LAYER_DATA_ENTRY *p_current;

  p_current = p_inst_data->InstDataHeadPtr;

  while (p_current != DEF_NULL && p_current->OwnerId != id) {
    p_current = p_current->NextPtr;
  }

  if (p_current == DEF_NULL) {
    if (p_val != DEF_NULL) {
      p_current = HTTPsCtrlLayerMem_InstDataEntryAlloc(p_inst_data);
      if (p_current == DEF_NULL) {
        CPU_SW_EXCEPTION(; );                                   // Not suppose to happen.
      }
      p_current->DataPtr = p_val;
      p_current->OwnerId = id;
    }
  } else {
    p_current->DataPtr = p_val;
  }
}

/****************************************************************************************************//**
 *                                       HTTPsCtrlLayer_InstDataGet()
 *
 * @brief    Get the Control Layer instance data related to the ID.
 *
 * @param    id              Id of the instance data to get.
 *
 * @param    p_inst_data     Pointer to Ctrl Layer instance data.
 *
 * @return   Pointer to instance data object.
 *           DEF_NULL    if the id doesn't exist.
 *******************************************************************************************************/
static void *HTTPsCtrlLayer_InstDataGet(CPU_INT32U                 id,
                                        HTTPs_CTRL_LAYER_INST_DATA *p_inst_data)
{
  HTTPs_CTRL_LAYER_DATA_ENTRY *p_current;

  p_current = p_inst_data->InstDataHeadPtr;

  while (p_current != DEF_NULL) {
    if (p_current->OwnerId == id) {
      return (p_current->DataPtr);
    } else {
      p_current = p_current->NextPtr;
    }
  }

  return (DEF_NULL);
}

/****************************************************************************************************//**
 *                                       HTTPsCtrlLayer_ConnDataEntrySet()
 *
 * @brief    Sets the connection data entry value for substitution.
 *
 * @param    id              id of the connection data entry
 *
 * @param    p_inst_data     Pointer to Ctrl Layer instance data.
 *
 * @param    p_conn_data     Pointer to Ctrl Layer connection data.
 *
 * @param    p_val           Pointer to value to set.
 *******************************************************************************************************/
static void HTTPsCtrlLayer_ConnDataEntrySet(CPU_INT32U                 id,
                                            HTTPs_CTRL_LAYER_INST_DATA *p_inst_data,
                                            HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data,
                                            void                       *p_val)
{
  HTTPs_CTRL_LAYER_DATA_ENTRY *p_current;

  p_current = p_conn_data->ConnDataHeadPtr;

  while ((p_current != DEF_NULL)
         && (p_current->OwnerId != id)      ) {
    p_current = p_current->NextPtr;
  }

  if (p_current == DEF_NULL) {
    if (p_val != DEF_NULL) {
      p_current = HTTPsCtrlLayer_ConnDataEntryAlloc(p_inst_data, p_conn_data);
      if (p_current == DEF_NULL) {
        CPU_SW_EXCEPTION(; );                                   // Not suppose to happen.
      }
      p_current->DataPtr = p_val;
      p_current->OwnerId = id;
    }
  } else {
    p_current->DataPtr = p_val;
  }
}

/****************************************************************************************************//**
 *                                       HTTPsCtrlLayer_ConnDataEntryGet()
 *
 * @brief    Get a saved connection data entry.
 *
 * @param    id              ID of the saved data.
 *
 * @param    p_inst_dData    Pointer to Ctrl Layer instance data.
 *
 * @param    p_conn_data     Pointer to Ctrl Layer connection data.
 *
 * @return   Pointer to the saved connection data entry.
 *           DEF_NULL if the id doesn't exist.
 *******************************************************************************************************/
static void *HTTPsCtrlLayer_ConnDataEntryGet(CPU_INT32U                 id,
                                             HTTPs_CTRL_LAYER_INST_DATA *p_inst_data,
                                             HTTPs_CTRL_LAYER_CONN_DATA *p_conn_data)
{
  HTTPs_CTRL_LAYER_DATA_ENTRY *p_current;

  PP_UNUSED_PARAM(p_inst_data);

  p_current = p_conn_data->ConnDataHeadPtr;

  while (p_current != DEF_NULL) {
    if (p_current->OwnerId == id) {
      return (p_current->DataPtr);
    } else {
      p_current = p_current->NextPtr;
    }
  }

  return (DEF_NULL);
}

/****************************************************************************************************//**
 *                                       HTTPsCtrlLayer_InstanceInit()
 *
 * @brief    Call the given init function with the appropriate parameters in the appropriate context.
 *
 * @param    id                  Context ID.
 *
 * @param    instance_init_fnct  Function pointer for the initialization.
 *
 * @param    p_instance          Pointer to the HTTPs instance object.
 *
 * @param    p_hook_cfg          Sub-module hook configuration.
 *******************************************************************************************************/
static CPU_BOOLEAN HTTPsCtrlLayer_InstanceInit(CPU_INT32U                     id,
                                               const HTTPs_INSTANCE_INIT_HOOK instance_init_fnct,
                                               HTTPs_INSTANCE                 *p_instance,
                                               const void                     *p_hook_cfg)
{
  CPU_BOOLEAN result;

  if (instance_init_fnct == DEF_NULL) {
    return (DEF_OK);
  }

  INST_SCOPE_ALLOC();

  INST_SCOPE_ENTER(id, p_instance);

  result = instance_init_fnct(p_instance, p_hook_cfg);

  INST_SCOPE_EXIT(id, p_instance);

  return (result);
}

/****************************************************************************************************//**
 *                                           HTTPsCtrlLayer_OnReqRx()
 *
 * @brief    Call the given ConnRequest fnct with the appropriate parameters in the appropriate context.
 *
 * @param    id          Context ID.
 *
 * @param    req_fnct    handler function fo the request (can be an auth or an app)
 *
 * @param    p_instance  HTTP-s instance
 *
 * @param    p_conn      HTTP-s connection
 *
 * @param    p_hook_cfg  pointer to the resource config
 *
 * @return   DEF_OK    if the request follows the resource requirements.
 *           DEF_FAIL  otherwise
 *******************************************************************************************************/
static CPU_BOOLEAN HTTPsCtrlLayer_OnReqRx(CPU_INT32U     id,
                                          HTTPs_REQ_HOOK req_fnct,
                                          HTTPs_INSTANCE *p_instance,
                                          HTTPs_CONN     *p_conn,
                                          const void     *p_hook_cfg)
{
  if (req_fnct == DEF_NULL) {
    return (DEF_FAIL);
  }

  CPU_BOOLEAN success;

  CONN_SCOPE_ALLOC();

  CONN_SCOPE_ENTER(id, p_instance, p_conn);

  success = req_fnct(p_instance, p_conn, p_hook_cfg);

  CONN_SCOPE_EXIT(id, p_instance, p_conn);

  return (success);
}

/****************************************************************************************************//**
 *                                           HTTPsCtrlLayer_RxHdr()
 *
 * @brief    Calls the given RxHeader function with the appropriate parameters in the appropriate
 *           context.
 *
 * @param    id              ID of the context.
 *
 * @param    rx_hdr_fnct     Received header function to call.
 *
 * @param    p_instance      Pointer to HTTPs instance object.
 *
 * @param    p_conn          Pointer to HTTPs connection object.
 *
 * @param    p_hook_cfg      Pointer to called module's configuration.
 *
 * @param    hdr_field       Header field to check.
 *
 * @return   DEF_YES    if the header must be kept
 *           DEF_NO     otherwise
 *******************************************************************************************************/
static CPU_BOOLEAN HTTPsCtrlLayer_RxHdr(CPU_INT32U            id,
                                        HTTPs_REQ_HDR_RX_HOOK rx_hdr_fnct,
                                        HTTPs_INSTANCE        *p_instance,
                                        HTTPs_CONN            *p_conn,
                                        const void            *p_hook_cfg,
                                        HTTP_HDR_FIELD        hdr_field)
{
  CPU_BOOLEAN keep_hdr;

  if (rx_hdr_fnct == DEF_NULL) {
    return (DEF_NO);
  }

  CONN_SCOPE_ALLOC();

  CONN_SCOPE_ENTER(id, p_instance, p_conn);

  keep_hdr = rx_hdr_fnct(p_instance, p_conn, p_hook_cfg, hdr_field);

  CONN_SCOPE_EXIT(id, p_instance, p_conn);

  return (keep_hdr);
}

/****************************************************************************************************//**
 *                                           HTTPsCtrlLayer_TxHdr()
 *
 * @brief    Calls the given TxHeader function in the appropriate context.
 *
 * @param    id              ID of the context.
 *
 * @param    tx_hdr_fnct     Transmit header function to call.
 *
 * @param    p_instance      Pointer to HTTPs instance object.
 *
 * @param    p_conn          Pointer to HTTPs connection object.
 *
 * @param    p_hook_cfg      Pointer to called module's configuration.
 *
 * @return   DEF_OK    if all headers have been tx'ed.
 *           DEF_FAIL  otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN HTTPsCtrlLayer_TxHdr(CPU_INT32U             id,
                                        HTTPs_RESP_HDR_TX_HOOK tx_hdr_fnct,
                                        HTTPs_INSTANCE         *p_instance,
                                        HTTPs_CONN             *p_conn,
                                        const void             *p_hook_cfg)
{
  CPU_BOOLEAN send_hdr;

  if (tx_hdr_fnct == DEF_NULL) {
    return (DEF_FAIL);
  }

  CONN_SCOPE_ALLOC();

  CONN_SCOPE_ENTER(id, p_instance, p_conn);

  send_hdr = tx_hdr_fnct(p_instance, p_conn, p_hook_cfg);

  CONN_SCOPE_EXIT(id, p_instance, p_conn);

  return (send_hdr);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_SERVER_AVAIL
