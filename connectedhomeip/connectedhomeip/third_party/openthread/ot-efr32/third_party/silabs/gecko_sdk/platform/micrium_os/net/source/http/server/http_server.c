/***************************************************************************//**
 * @file
 * @brief Network - HTTP Server
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

#include  <em_core.h>

#include  "http_server_priv.h"

#include  "../../../include/net_sock.h"
#include  "../../../include/net_cfg_net.h"

#include  <common/include/lib_ascii.h>
#include  <common/include/rtos_err.h>

#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                     (NET, HTTP)
#define  RTOS_MODULE_CUR                  RTOS_CFG_MODULE_NET

#define  HTTPs_INIT_CFG_DFLT            { \
    .MemSegPtr = DEF_NULL                 \
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

CPU_INT16U HTTPs_InstanceRunningNbr;
CPU_INT16U HTTPs_InstanceInitializedNbr;

const HTTP_DICT HTTPs_DictionaryTokenInternal[] = {
  { HTTPs_TOKEN_INTERNAL_STATUS_CODE, HTTPs_STR_TOKEN_INTERNAL_STATUS_CODE, (sizeof(HTTPs_STR_TOKEN_INTERNAL_STATUS_CODE)   - 1) },
  { HTTPs_TOKEN_INTERNAL_REASON_PHRASE, HTTPs_STR_TOKEN_INTERNAL_REASON_PHRASE, (sizeof(HTTPs_STR_TOKEN_INTERNAL_REASON_PHRASE) - 1) },
};

CPU_SIZE_T HTTPs_DictionarySizeTokenInternal = sizeof(HTTPs_DictionaryTokenInternal);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const HTTPs_INIT_CFG  HTTPs_InitCfgDflt = HTTPs_INIT_CFG_DFLT;
static HTTPs_INIT_CFG HTTPs_InitCfg = HTTPs_INIT_CFG_DFLT;
#else
extern const HTTPs_INIT_CFG HTTPs_InitCfg;
#endif

static CPU_BOOLEAN HTTPs_InitActive = DEF_ACTIVE;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         HTTPs_ConfigureMemSeg()
 *
 * @brief    Configure the memory segment that will be used to allocate internal data needed by the
 *           HTTP server module instead of the default memory segment.
 *
 * @param    p_mem_seg   Pointer to the memory segment from which the internal data will be allocated.
 *                       If DEF_NULL, the internal data will be allocated from the global Heap.
 *
 *                       @note         (1) This function is optional. If it is called, it must be called before HTTPs_Init(). If
 *                       it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void HTTPs_ConfigureMemSeg(MEM_SEG *p_mem_seg)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_DBG((HTTPs_InitActive != DEF_INACTIVE), RTOS_ERR_ALREADY_INIT,; );
  CORE_EXIT_ATOMIC();

  HTTPs_InitCfg.MemSegPtr = p_mem_seg;
}
#endif

/****************************************************************************************************//**
 *                                             HTTPs_Init()
 *
 * @brief    Initializes the HTTP server suite.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_ALREADY_INIT
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *
 * @internal
 * @note         (1) [INTERNAL] 'HTTPs_InitActive' MUST be accessed exclusively in critical sections during
 *               initialization.
 * @endinternal
 *******************************************************************************************************/
void HTTPs_Init(RTOS_ERR *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((HTTPs_HTML_DLFT_ERR_LEN != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();                                          // See Note #1.
  if (HTTPs_InitActive == DEF_INACTIVE) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_INIT);
    return;
  }
  CORE_EXIT_ATOMIC();

  //                                                               ------------- INIT HTTPs INSTANCE POOL -------------
  HTTPsMem_InstanceInit(HTTPs_InitCfg.MemSegPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CORE_ENTER_ATOMIC();                                          // See Note #1.
  HTTPs_InitActive = DEF_INACTIVE;                              // Block http-s fncts/tasks until init complete.
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                         HTTPs_InstanceInit()
 *
 * @brief    Initializes an HTTP server instance.
 *
 * @param    p_cfg       Pointer to the instance configuration object.
 *
 * @param    p_task_cfg  Pointer to the instance task configuration object.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_READY
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_SEG_OVF
 *                           - RTOS_ERR_ALLOC
 *                           - RTOS_ERR_SEG_OVF
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                           - RTOS_ERR_INIT
 *
 * @return   Pointer to the instance handler, if NO error(s).
 *           NULL pointer, otherwise.
 *******************************************************************************************************/
HTTPs_INSTANCE *HTTPs_InstanceInit(const HTTPs_CFG     *p_cfg,
                                   const RTOS_TASK_CFG *p_task_cfg,
                                   RTOS_ERR            *p_err)
{
#if (HTTPs_CFG_FS_PRESENT_EN == DEF_ENABLED)
  const NET_FS_API *p_fs_api;
#endif
  HTTPs_INSTANCE *p_instance;
  CPU_BOOLEAN    init_active;
  CPU_BOOLEAN    hook_def;
  CPU_BOOLEAN    result;
#if ((RTOS_ARG_CHK_EXT_EN == DEF_ENABLED) \
  && (HTTPs_CFG_FS_PRESENT_EN == DEF_ENABLED))
  CPU_CHAR path_sep_char;
#endif
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);
  RTOS_ASSERT_DBG_ERR_SET((p_cfg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);
  RTOS_ASSERT_DBG_ERR_SET((p_task_cfg != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  init_active = HTTPs_InitActive;
  CORE_EXIT_ATOMIC();

  if (init_active == DEF_ACTIVE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_READY);
    return (DEF_NULL);
  }

#if (RTOS_ARG_CHK_EXT_EN == DEF_ENABLED)

  switch (p_cfg->SockSel) {
#ifdef   NET_IPv4_MODULE_EN
    case HTTPs_SOCK_SEL_IPv4:
      break;
#endif // NET_IPv4_MODULE_EN

#ifdef   NET_IPv6_MODULE_EN
    case HTTPs_SOCK_SEL_IPv6:
      break;
#endif // NET_IPv6_MODULE_EN

#if  (defined(NET_IPv4_MODULE_EN) && defined(NET_IPv6_MODULE_EN))
    case HTTPs_SOCK_SEL_IPv4_IPv6:
      break;
#endif // NET_IPv6_MODULE_EN && NET_IPv6_MODULE_EN

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
  }

  //                                                               ------------------- VALIDATE CFG -------------------

  if (p_cfg->SecurePtr != DEF_NULL) {                           // Validate secure cfg.
#ifndef  NET_SECURE_MODULE_EN
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, DEF_NULL);
#else
    if (p_cfg->SecurePtr->CertPtr == DEF_NULL) {
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    }

    if (p_cfg->SecurePtr->CertLen == 0u) {
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    }

    if (p_cfg->SecurePtr->KeyPtr == DEF_NULL) {
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    }

    if (p_cfg->SecurePtr->KeyLen == 0u) {
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    }

    switch (p_cfg->SecurePtr->Fmt) {
      case NET_SOCK_SECURE_CERT_KEY_FMT_PEM:
      case NET_SOCK_SECURE_CERT_KEY_FMT_DER:
        break;

      case NET_SOCK_SECURE_CERT_KEY_FMT_NONE:
      default:
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    }
#endif // NET_SECURE_MODULE_EN
  }

  if (p_cfg->ConnNbrMax <= 0u) {                                // Validate nbr conn.
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
  }

  if (p_cfg->BufLen < HTTPs_BUF_LEN_MIN) {                      // Validate buf len.
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
  }

  if ((p_cfg->FS_CfgPtr == DEF_NULL)                            // Validate FS configuration pointer.
      && (p_cfg->FS_Type != HTTPs_FS_TYPE_NONE)) {
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
  }

  if (p_cfg->DfltResourceNamePtr == DEF_NULL) {
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
  }

  if (p_cfg->PathLenMax <= 0) {                                 // Validate path len max.
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
  }

  switch (p_cfg->FS_Type) {
#if (HTTPs_CFG_FS_PRESENT_EN == DEF_ENABLED)
    case HTTPs_FS_TYPE_STATIC:
      p_fs_api = ((HTTPs_CFG_FS_STATIC *)p_cfg->FS_CfgPtr)->FS_API_Ptr;
      if (p_fs_api == DEF_NULL) {
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
      }

      path_sep_char = p_fs_api->CfgPathGetSepChar();
      if (path_sep_char == ASCII_CHAR_NULL) {                   // Validate path sep char.
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
      }
      break;
#endif

#if (HTTPs_CFG_FS_PRESENT_EN == DEF_ENABLED)
    case HTTPs_FS_TYPE_DYN:
      p_fs_api = ((HTTPs_CFG_FS_DYN *)p_cfg->FS_CfgPtr)->FS_API_Ptr;
      if (p_fs_api == DEF_NULL) {
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
      }

      path_sep_char = p_fs_api->CfgPathGetSepChar();
      if (path_sep_char == ASCII_CHAR_NULL) {                   // Validate path sep char.
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
      }
      break;
#endif

    case HTTPs_FS_TYPE_NONE:
      break;

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
  }

#if (HTTPs_CFG_ABSOLUTE_URI_EN == DEF_ENABLED)
  if (p_cfg->HostNameLenMax <= 0) {                             // Validate host len max.
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
  }
#endif // HTTPs_CFG_ABSOLUTE_URI_EN

  //                                                               Validate hdr field param.
  if (p_cfg->HdrRxCfgPtr != DEF_NULL) {
#if (HTTPs_CFG_HDR_RX_EN == DEF_ENABLED)

    if (p_cfg->HdrRxCfgPtr->DataLenMax <= 0) {                  // Validate Req hdr field val len.
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    }

    if (p_cfg->HooksPtr == DEF_NULL) {                          // The Header Rx hook must be defined.
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    } else {
      if (p_cfg->HooksPtr->OnReqHdrRxHook == DEF_NULL) {
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
      }
    }

#else
    //                                                             Validate hdr field en param.
    RTOS_DBG_FAIL_EXEC(RTOS_ERR_NOT_AVAIL, DEF_NULL);
#endif // HTTPs_CFG_HDR_RX_EN
  }

  if (p_cfg->HdrTxCfgPtr != DEF_NULL) {
#if (HTTPs_CFG_HDR_TX_EN == DEF_ENABLED)

    if (p_cfg->HdrTxCfgPtr->DataLenMax <= 0) {                  // Validate Resp hdr field val len.
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    }

    if (p_cfg->HooksPtr == DEF_NULL) {                          // The Header Tx hook must be defined.
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    } else {
      if (p_cfg->HooksPtr->OnRespHdrTxHook == DEF_NULL) {
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
      }
    }

#else
    //                                                             Validate hdr field en param.
    RTOS_DBG_FAIL_EXEC(RTOS_ERR_NOT_AVAIL, DEF_NULL);
#endif // HTTPs_CFG_HDR_TX_EN
  }

  if (p_cfg->TokenCfgPtr != DEF_NULL) {                         // Validate token param.
#if (HTTPs_CFG_TOKEN_PARSE_EN == DEF_ENABLED)

    if (p_cfg->TokenCfgPtr->ValLenMax <= 0u) {                  // Validate token val len max.
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    }

    if (p_cfg->HooksPtr == DEF_NULL) {                          // Validate token hook function.
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    } else {
      if (p_cfg->HooksPtr->OnRespTokenHook == DEF_NULL) {
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
      }
    }
#else
    //                                                             Validate token en param.
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, DEF_NULL);
#endif // HTTPs_CFG_TOKEN_PARSE_EN
  }

  if (p_cfg->QueryStrCfgPtr != DEF_NULL) {
#if (HTTPs_CFG_QUERY_STR_EN == DEF_ENABLED)

    if (p_cfg->QueryStrCfgPtr->KeyLenMax <= 0u) {               // Validate Key length max.
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    }

    if (p_cfg->QueryStrCfgPtr->ValLenMax <= 0u) {               // Validate Value length max.
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    }
#else
    //                                                             Validate Query String en param.
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, DEF_NULL);
#endif
  }

  if (p_cfg->FormCfgPtr != DEF_NULL) {                          // Validate Form param.
#if (HTTPs_CFG_FORM_EN == DEF_ENABLED)

    if (p_cfg->FormCfgPtr->KeyLenMax <= 0u) {                   // Validate Key length max.
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    }

    if (p_cfg->FormCfgPtr->ValLenMax <= 0u) {                   // Validate Value length max.
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    }

    //                                                             Validate multipart param.
    if ((p_cfg->FormCfgPtr->MultipartEn == DEF_ENABLED)
        && (p_cfg->FormCfgPtr->MultipartFileUploadEn == DEF_ENABLED)
        && (p_cfg->FormCfgPtr->MultipartFileUploadFolderPtr == DEF_NULL)  ) {
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    }

    if ((p_cfg->FormCfgPtr->MultipartFileUploadEn == DEF_ENABLED)
        && (p_cfg->FS_Type != HTTPs_FS_TYPE_DYN)) {
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    }

    if (p_cfg->HooksPtr == DEF_NULL) {                          // Validate Form hook function.
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    } else {
      if (p_cfg->HooksPtr->OnReqRdySignalHook == DEF_NULL) {
        RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
      }
    }
    //                                                             Validate buf len for Form.
    if (p_cfg->BufLen < (p_cfg->FormCfgPtr->KeyLenMax + p_cfg->FormCfgPtr->ValLenMax)) {
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
    }
#else
    //                                                             Validate Form en param.
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL, DEF_NULL);
#endif // HTTPs_CFG_FORM_EN
  }
#endif // RTOS_ARG_CHK_EXT_EN

  //                                                               ------------------- GET INSTANCE -------------------
  p_instance = HTTPsMem_InstanceGet(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  //                                                               ---------- INITIALIZE INSTANCE PARAMETERS ----------
  p_instance->CfgPtr = p_cfg;
  p_instance->TaskCfgPtr = p_task_cfg;
  p_instance->Started = DEF_NO;

  switch (p_cfg->FS_Type) {
    case HTTPs_FS_TYPE_NONE:
      break;

#if (HTTPs_CFG_FS_PRESENT_EN == DEF_ENABLED)
    case HTTPs_FS_TYPE_STATIC:
      p_fs_api = ((HTTPs_CFG_FS_STATIC *)p_cfg->FS_CfgPtr)->FS_API_Ptr;
      p_instance->FS_PathSepChar = p_fs_api->CfgPathGetSepChar();
      break;
#endif

#if (HTTPs_CFG_FS_PRESENT_EN == DEF_ENABLED)
    case HTTPs_FS_TYPE_DYN:
      p_fs_api = ((HTTPs_CFG_FS_DYN *)p_cfg->FS_CfgPtr)->FS_API_Ptr;
      p_instance->FS_PathSepChar = p_fs_api->CfgPathGetSepChar();
      break;
#endif

    default:
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG, DEF_NULL);
  }

  //                                                               ------------------- INIT OS OBJ --------------------
  HTTPsTask_InstanceObjInit(p_instance, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    HTTPsMem_InstanceRelease(p_instance);
    return (DEF_NULL);
  }

  //                                                               ------------------ INIT CONN POOL ------------------
  HTTPsMem_ConnPoolInit(p_instance,
                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    HTTPsMem_InstanceRelease(p_instance);
    return (DEF_NULL);
  }

  //                                                               --- SET THE BUF STR FOR THE INSTANCE WORKING DIR ---
#if (HTTPs_CFG_FS_PRESENT_EN == DEF_ENABLED)
  if (p_cfg->FS_Type == HTTPs_FS_TYPE_DYN) {
    HTTPsMem_InstanceWorkingDirInit(p_instance, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      HTTPsMem_InstanceRelease(p_instance);
      return (DEF_NULL);
    }
  }
#endif

  //                                                               ------------- CALL INSTANCE INIT HOOK --------------
  hook_def = HTTPs_HOOK_DEFINED(p_cfg->HooksPtr, OnInstanceInitHook);
  if (hook_def == DEF_YES) {
    //                                                             If Instance conn objs init handler is not null ...
    //                                                             ... call Instance conn objs init handler.
    result = p_cfg->HooksPtr->OnInstanceInitHook(p_instance,
                                                 p_cfg->Hooks_CfgPtr);
    if (result != DEF_OK) {
      HTTPsMem_InstanceRelease(p_instance);
      RTOS_ERR_SET(*p_err, RTOS_ERR_INIT);
      return (DEF_NULL);
    }
  }

  CORE_ENTER_ATOMIC();
  ++HTTPs_InstanceInitializedNbr;
  CORE_EXIT_ATOMIC();

  return (p_instance);
}

/****************************************************************************************************//**
 *                                     HTTPs_InstanceTaskPrioSet()
 *
 * @brief    Sets the priority of the given HTTP server instance's task.
 *
 * @param    p_instance  Pointer to specific HTTP server instance handler.
 *
 * @param    prio        Priority of the HTTP instance's task.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_INVALID_ARG
 *******************************************************************************************************/
void HTTPs_InstanceTaskPrioSet(HTTPs_INSTANCE *p_instance,
                               RTOS_TASK_PRIO prio,
                               RTOS_ERR       *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_instance != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  HTTPsTask_InstanceTaskPrioSet(p_instance, prio, p_err);
}

/****************************************************************************************************//**
 *                                        HTTPs_InstanceStart()
 *
 * @brief    Starts a specific HTTPs server instance which had been previously initialized.
 *
 * @param    p_instance  Pointer to specific HTTP server instance handler.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                           - RTOS_ERR_INVALID_TYPE
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_NOT_FOUND
 *                           - RTOS_ERR_ALREADY_EXISTS
 *                           - RTOS_ERR_INVALID_HANDLE
 *                           - RTOS_ERR_NET_INVALID_CONN
 *                           - RTOS_ERR_INVALID_STATE
 *                           - RTOS_ERR_NET_CONN_CLOSED_FAULT
 *******************************************************************************************************/
void HTTPs_InstanceStart(HTTPs_INSTANCE *p_instance,
                         RTOS_ERR       *p_err)
{
  const HTTPs_CFG *p_cfg;
#ifdef   NET_IPv4_MODULE_EN
  NET_SOCK_ID sock_listen_ipv4;
#endif
#ifdef   NET_IPv6_MODULE_EN
  NET_SOCK_ID sock_listen_ipv6;
#endif
  RTOS_ERR local_err;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_instance != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               Acquire Instance lock.
  HTTPsTask_LockAcquire(p_instance->OS_TaskObjPtr->LockObj);

  if (p_instance->Started == DEF_YES) {
    goto exit;
  }

  p_cfg = p_instance->CfgPtr;

  //                                                               ----------------- INIT LISTEN SOCK -----------------
  switch (p_cfg->SockSel) {
#ifdef   NET_IPv4_MODULE_EN
    case HTTPs_SOCK_SEL_IPv4:
      sock_listen_ipv4 = HTTPsSock_ListenInit(p_cfg, NET_SOCK_PROTOCOL_FAMILY_IP_V4, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      p_instance->SockListenID_IPv4 = sock_listen_ipv4;
      break;
#endif

#ifdef   NET_IPv6_MODULE_EN
    case HTTPs_SOCK_SEL_IPv6:
      sock_listen_ipv6 = HTTPsSock_ListenInit(p_cfg, NET_SOCK_PROTOCOL_FAMILY_IP_V6, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }

      p_instance->SockListenID_IPv6 = sock_listen_ipv6;
      break;
#endif

#if  (defined(NET_IPv4_MODULE_EN) && defined(NET_IPv6_MODULE_EN))
    case HTTPs_SOCK_SEL_IPv4_IPv6:
      sock_listen_ipv4 = HTTPsSock_ListenInit(p_cfg, NET_SOCK_PROTOCOL_FAMILY_IP_V4, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }

      p_instance->SockListenID_IPv4 = sock_listen_ipv4;

      sock_listen_ipv6 = HTTPsSock_ListenInit(p_cfg, NET_SOCK_PROTOCOL_FAMILY_IP_V6, p_err);

      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        (void)NetSock_Close(sock_listen_ipv4, &local_err);           // Close IPv4 sock.
        goto exit;
      }

      p_instance->SockListenID_IPv6 = sock_listen_ipv6;
      break;
#endif

    default:
      HTTPsTask_LockRelease(p_instance->OS_TaskObjPtr->LockObj);
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG,; );
  }

  PP_UNUSED_PARAM(local_err);

  //                                                               -------------- CREATE & START OS TASK --------------
  HTTPsTask_InstanceTaskCreate(p_instance, p_err);              // return err of sub-fcnts.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  CORE_ENTER_ATOMIC();
  ++HTTPs_InstanceRunningNbr;
  CORE_EXIT_ATOMIC();

exit:
  HTTPsTask_LockRelease(p_instance->OS_TaskObjPtr->LockObj);                // Release instance lock.
}

/****************************************************************************************************//**
 *                                         HTTPs_InstanceStop()
 *
 * @brief    Stops a specific HTTPs server instance.
 *
 * @param    p_instance  Pointer to Instance handler.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_OS_OBJ_DEL
 *                           - RTOS_ERR_WOULD_BLOCK
 *                           - RTOS_ERR_OS_SCHED_LOCKED
 *                           - RTOS_ERR_ABORT
 *                           - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void HTTPs_InstanceStop(HTTPs_INSTANCE *p_instance,
                        RTOS_ERR       *p_err)
{
  const HTTPs_CFG *p_cfg;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_instance != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  //                                                               Acquire Instance lock.
  HTTPsTask_LockAcquire(p_instance->OS_TaskObjPtr->LockObj);

  if (p_instance->Started != DEF_YES) {
    goto exit_release;
  }

  p_cfg = p_instance->CfgPtr;

  //                                                               ------------- SIGNAL INSTANCE TO STOP --------------
  HTTPsTask_InstanceStopReqSignal(p_instance);

  HTTPsTask_LockRelease(p_instance->OS_TaskObjPtr->LockObj);    // Release Instance lock before pending.

  //                                                               Stop listening for incoming connections.
#ifdef NET_IPv4_MODULE_EN
  if (p_cfg->SockSel == HTTPs_SOCK_SEL_IPv4
      || p_cfg->SockSel == HTTPs_SOCK_SEL_IPv4_IPv6) {
    NetSock_SelAbort(p_instance->SockListenID_IPv4, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }
#endif

#ifdef NET_IPv6_MODULE_EN
  if (p_cfg->SockSel == HTTPs_SOCK_SEL_IPv6
      || p_cfg->SockSel == HTTPs_SOCK_SEL_IPv4_IPv6) {
    NetSock_SelAbort(p_instance->SockListenID_IPv6, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }
#endif

  //                                                               --------------- WAIT STOP COMPLETED ----------------
  HTTPsTask_InstanceStopCompletedPending(p_instance, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               Re-acquire Instance lock.
  HTTPsTask_LockAcquire(p_instance->OS_TaskObjPtr->LockObj);

  //                                                               ---------------- DEL INSTANCE TASK -----------------
  HTTPsTask_InstanceTaskDel(p_instance);

  CORE_ENTER_ATOMIC();
  --HTTPs_InstanceRunningNbr;
  CORE_EXIT_ATOMIC();

exit_release:
  HTTPsTask_LockRelease(p_instance->OS_TaskObjPtr->LockObj);    // Release Instance lock.

exit:
  return;
}

/****************************************************************************************************//**
 *                                          HTTPs_RespHdrGet()
 *
 * @brief    Acquires a new response header block.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    hdr_field   Type of the response header value :
 *                       See enumeration HTTPs_HDR_FIELD.
 *
 * @param    val_type    Data type of the response header field value :
 *                           - HTTP_HDR_VAL_TYPE_NONE      Header field does not require a value.
 *                           - HTTP_HDR_VAL_TYPE_STR_CONST Header value type is a constant string.
 *                           - HTTP_HDR_VAL_TYPE_STR       Header value type is a variable string.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_POOL_EMPTY
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_SEG_OVF
 *
 * @return   Pointer to the response header block that can be filled with data, if no error(s).
 *           Null pointer, otherwise
 *
 * @note     (1) Must be called from a callback function only. Should be called by the function
 *               pointed by RespHdrTxFnctPtr in the instance configuration.
 *               - (a) The instance lock must be acquired before calling this function. It is why this
 *                     function must be called from a callback function.
 *
 * @note     (2) The header block is automatically added to the header blocks list. Thus the caller
 *               has not to add the block to the list. Only filling the value and value length should
 *               be required by the caller.
 *******************************************************************************************************/
#if (HTTPs_CFG_HDR_TX_EN == DEF_ENABLED)
HTTPs_HDR_BLK *HTTPs_RespHdrGet(const HTTPs_INSTANCE *p_instance,
                                HTTPs_CONN           *p_conn,
                                HTTP_HDR_FIELD       hdr_field,
                                HTTPs_HDR_VAL_TYPE   val_type,
                                RTOS_ERR             *p_err)
{
  HTTPs_HDR_BLK *p_blk = DEF_NULL;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, DEF_NULL);
  RTOS_ASSERT_DBG_ERR_SET((p_instance != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);
  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_NULL);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_blk = HTTPsMem_RespHdrGet((HTTPs_INSTANCE *)p_instance,
                              p_conn,
                              hdr_field,
                              val_type,
                              p_err);

  return (p_blk);
}
#endif

/****************************************************************************************************//**
 *                                     HTTPs_RespBodySetParamFile()
 *
 * @brief    Sets the parameters for the response body when the body's data is a file inside a
 *           File System infrastructure.
 *
 * @param    p_instance      Pointer to the instance.
 *
 * @param    p_conn          Pointer to the connection.
 *
 * @param    p_path          Pointer to the string file path.
 *
 * @param    content_type    Content type of the file.
 *                           If unknown, this can be set to HTTP_CONTENT_TYPE_UNKNOWN. The server
 *                           core will find it with the file extension.
 *                           See HTTP_CONTENT_TYPE enum in http.h for possible content types.
 *
 * @param    token_en        DEF_YES, if the file contents tokens the server needs to replace.
 *                           DEF_NO, otherwise.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function :
 *                               - RTOS_ERR_NONE
 *
 * @note     (1) Must be called from a callback function only.
 *               - (a) The instance lock must be acquired before calling this function. It is why this
 *                     function must be called from a callback function.
 *******************************************************************************************************/
void HTTPs_RespBodySetParamFile(const HTTPs_INSTANCE *p_instance,
                                HTTPs_CONN           *p_conn,
                                CPU_CHAR             *p_path,
                                HTTP_CONTENT_TYPE    content_type,
                                CPU_BOOLEAN          token_en,
                                RTOS_ERR             *p_err)
{
#if (HTTPs_CFG_TOKEN_PARSE_EN == DEF_ENABLED)
  const HTTPs_CFG *p_cfg = p_instance->CfgPtr;
#endif

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_instance != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_path != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_conn->RespBodyDataType = HTTPs_BODY_DATA_TYPE_FILE;

  Mem_Copy(p_conn->PathPtr, p_path, p_conn->PathLenMax);

  if (content_type != HTTP_CONTENT_TYPE_UNKNOWN) {
    p_conn->RespContentType = content_type;
  }

  if (token_en == DEF_YES) {
#if (HTTPs_CFG_TOKEN_PARSE_EN == DEF_ENABLED)
    if (p_cfg->TokenCfgPtr != DEF_NULL) {
      DEF_BIT_SET(p_conn->Flags, HTTPs_FLAG_RESP_CHUNKED);
    } else {
      DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_CHUNKED);
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG,; );
    }
#else
    DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_CHUNKED);
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
  } else {
    DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_CHUNKED);
  }
}

/****************************************************************************************************//**
 *                                  HTTPs_RespBodySetParamStaticData()
 *
 * @brief    Sets the parameters for the response body when the body's data is a static data contained
 *           in a memory space.
 *
 * @param    p_instance      Pointer to the instance.
 *
 * @param    p_conn          Pointer to the connection.
 *
 * @param    content_type    Content type of the file.
 *                           See HTTP_CONTENT_TYPE enum in http.h for possible content types.
 *
 * @param    p_data          Pointer to memory section containing data.
 *                           DEF_NULL, if data will added to the response with the 'OnRespChunkHook'
 *                           Hook.
 *
 * @param    data_len        Data length.
 *
 * @param    token_en        DEF_YES, if the data contents tokens the server needs to replace.
 *                           DEF_NO, otherwise.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function :
 *                               - RTOS_ERR_NONE
 *
 * @note     (1) Must be called from a callback function only.
 *               - (a) The instance lock must be acquired before calling this function. This is why this
 *                     function must be called from a callback function.
 *
 * @note     (2) This function can be used when the data to put in the response body is not in a file
 *               in a File System.
 *
 * @note     (3) If all the data to send is inside a memory space, the 'p_data' parameter can be set
 *               to point to the memory space and the 'data_len' must be set, since the data length is
 *               known.
 *
 * @note     (4) When the data to send is a stream of unknown size, the Chunked Transfer Encoding
 *               must be used. In this case, the function can work with the parameter 'p_data' when set
 *               to DEF_NULL. This tells the server to use the hook function
 *               'p_cfg->p_hooks->OnRespChunkHook' to retrieve the data to put in the HTTP response.
 *******************************************************************************************************/
void HTTPs_RespBodySetParamStaticData(const HTTPs_INSTANCE *p_instance,
                                      HTTPs_CONN           *p_conn,
                                      HTTP_CONTENT_TYPE    content_type,
                                      void                 *p_data,
                                      CPU_INT32U           data_len,
                                      CPU_BOOLEAN          token_en,
                                      RTOS_ERR             *p_err)
{
#if (HTTPs_CFG_TOKEN_PARSE_EN == DEF_ENABLED)
  const HTTPs_CFG *p_cfg = p_instance->CfgPtr;
#endif

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_instance != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_conn->RespBodyDataType = HTTPs_BODY_DATA_TYPE_STATIC_DATA;

  p_conn->RespContentType = content_type;

  if (p_data != DEF_NULL) {
    RTOS_ASSERT_DBG_ERR_SET((data_len > 0), *p_err, RTOS_ERR_INVALID_ARG,; );

    p_conn->DataPtr = p_data;
    p_conn->DataLen = data_len;
    DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_CHUNKED_HOOK);
  } else {
    DEF_BIT_SET(p_conn->Flags, HTTPs_FLAG_RESP_CHUNKED_HOOK);
  }

  if (token_en == DEF_YES) {
#if (HTTPs_CFG_TOKEN_PARSE_EN == DEF_ENABLED)
    if (p_cfg->TokenCfgPtr != DEF_NULL) {
      DEF_BIT_SET(p_conn->Flags, HTTPs_FLAG_RESP_CHUNKED);
    } else {
      DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_CHUNKED);
      RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_INVALID_CFG,; );
    }
#else
    DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_CHUNKED);
    RTOS_DBG_FAIL_EXEC_ERR(*p_err, RTOS_ERR_NOT_AVAIL,; );
#endif
  } else {
    DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_CHUNKED);
  }
}

/****************************************************************************************************//**
 *                                    HTTPs_RespBodySetParamNoBody()
 *
 * @brief    Sets the parameters to let the server know that no body is necessary in the response.
 *
 * @param    p_instance  Pointer to the instance.
 *
 * @param    p_conn      Pointer to the connection.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function :
 *                           - RTOS_ERR_NONE
 *
 * @note     (1) Must be called from a callback function only.
 *               - (a) The instance lock must be acquired before calling this function. This is why this
 *                     function must be called from a callback function
 *******************************************************************************************************/
void HTTPs_RespBodySetParamNoBody(const HTTPs_INSTANCE *p_instance,
                                  HTTPs_CONN           *p_conn,
                                  RTOS_ERR             *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_instance != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_conn->RespBodyDataType = HTTPs_BODY_DATA_TYPE_NONE;
  p_conn->RespContentType = HTTP_CONTENT_TYPE_UNKNOWN;
  p_conn->DataPtr = DEF_NULL;
  p_conn->DataLen = 0;

  DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_CHUNKED);
  DEF_BIT_CLR(p_conn->Flags, (HTTPs_FLAGS)HTTPs_FLAG_RESP_CHUNKED_HOOK);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_HTTP_SERVER_AVAIL
