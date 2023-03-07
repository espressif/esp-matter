/***************************************************************************//**
 * @file
 * @brief Network - IPerf Module
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

/****************************************************************************************************//**
 * @note     (1) IPerf is designed to work with NLANR IPerf 2.0.2 or higher.
 *               IPerf should be compatible with kPerf or jPerf using IPerf 2.0.2 or higher.
 *               - (a) Supports NLANR Iperf with the following restrictions/constraints :
 *                   - (1) TCP:
 *                       - (A) Multi-threaded                                NOT supported on both   mode
 *                       - (B) Measure bandwith                                  Supported on both   mode
 *                       - (C) Report MSS/MTU size & observed read sizes     NOT supported on both   mode
 *                       - (D) Support for TCP window size via socket buffer     Supported on server mode
 *                   - (2) UDP:
 *                       - (A) Multi-threaded                                NOT supported on both   mode
 *                       - (B) Create UDP streams of specified bandwidth     NOT supported on client mode
 *                       - (C) Measure packet loss                               Supported on server mode
 *                       - (D) Measure delay jitter                          NOT supported on both   mode
 *                       - (E) Multicast capable                             NOT supported on both   mode
 *               More information about NLANR IPerf can be obtained online at
 *               http://www.onl.wustl.edu/restricted/iperf.html.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_IPERF_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error IPerf Module requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  "iperf_priv.h"

#include  <rtos_err_cfg.h>

#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/kal/kal_priv.h>
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_types.h>
#include  <common/include/rtos_prio.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                        (NET, IPERF)
#define  RTOS_MODULE_CUR                     RTOS_CFG_MODULE_NET

/********************************************************************************************************
 *                                               IPERF VERSION
 *******************************************************************************************************/

#define  IPERF_VERSION                      50000u

/********************************************************************************************************
 *                                       OS TASK/OBJECT NAME DEFINES
 *******************************************************************************************************/

//                                                                 -------------------- TASK NAMES --------------------
#define  IPERF_OS_TASK_NAME                 "IPerf Task"
#define  IPERF_OS_TEST_ID_Q                 "IPerf Test ID Q"

#if (RTOS_ERR_CFG_STR_EN == DEF_ENABLED)
#define  IPERF_ERR_CODE_STR_GET(err_code)       RTOS_ERR_STR_GET((err_code))
#else
#define  IPERF_ERR_CODE_STR_GET(err_code)       (err_code)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const IPERF_CFG IPerf_CfgDflt = {
  .BufLen = IPERF_CFG_BUF_LEN_DFLT,
  .TestNbrMax = IPERF_CFG_TEST_NBR_MAX_DFLT,
  .Server = {
    .AcceptMaxRetry = IPERF_CFG_SERVER_ACCEPT_RETRY_NBR_MAX_DFLT,
    .AcceptMaxDlyMs = IPERF_CFG_SERVER_ACCEPT_DLY_MAX_MS_DFLT,
    .AcceptMaxTimeoutMs = IPERF_CFG_SERVER_ACCEPT_TIMEOUT_MAX_MS_DFLT,
    .TCP_RxMaxTimeoutMs = IPERF_CFG_SERVER_TCP_RX_TIMEOUT_MAX_MS_DFLT,
    .UDP_RxMaxTimeoutMs = IPERF_CFG_SERVER_UDP_RX_TIEMOUT_MAX_MS_DFLT
  },
  .Client = {
    .ConnMaxRetry = IPERF_CFG_CLIENT_CONN_RETRY_NBR_MAX_DFLT,
    .ConnMaxDlyMs = IPERF_CFG_CLIENT_CONN_DLY_MAX_MS_DFLT,
    .ConnMaxTimeoutMs = IPERF_CFG_CLIENT_CONN_TIMEOUT_MAX_MS_DFLT,
    .TCP_TxMaxTimeoutMs = IPERF_CFG_CLIENT_TCP_TX_TIMEOUT_MAX_MS_DFLT
  }
};

static RTOS_TASK_CFG IPerf_TaskCfg = {
  .Prio = IPERF_TASK_PRIO_DFLT,
  .StkSizeElements = IPERF_TASK_CFG_STK_SIZE_ELEMENTS_DFLT,
  .StkPtr = IPERF_TASK_CFG_STK_PTR_DFLT
};

#define  IPERF_INIT_CFG_DFLT            { \
    .MemSegPtr = DEF_NULL                 \
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                               VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const IPERF_INIT_CFG  IPerf_InitCfgDflt = IPERF_INIT_CFG_DFLT;
static IPERF_INIT_CFG IPerf_InitCfg = IPERF_INIT_CFG_DFLT;
#else
extern const IPERF_INIT_CFG IPerf_InitCfg;
#endif

//                                                                 --------------------- TASK TCB ---------------------
KAL_LOCK_HANDLE        IPerf_LockHandle;
static KAL_TASK_HANDLE IPerf_TaskHandle;
static KAL_Q_HANDLE    IPerf_QHandle;

static CPU_BOOLEAN IPerf_InitActive = DEF_ACTIVE;

//                                                                 Buf used to tx or rx.
CPU_CHAR *IPerf_BufPtr;

//                                                                 IPerf tests tbl (opt, conn, & stats data).
static MEM_DYN_POOL IPerf_TestObjPool;

static IPERF_TEST *IPerf_TestListHeadPtr;                    // Ptr to head of IPerf test Grp List.

static IPERF_TEST_ID IPerf_NextTestID;

const IPERF_CFG *IPerf_CfgPtr = DEF_NULL;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 --------------- IPERF PARSING FNCTS ----------------
static CPU_INT16U IPerf_ArgScan(CPU_CHAR   *p_in,
                                CPU_CHAR   *p_arg_tbl[],
                                CPU_INT16U arg_tbl_size,
                                RTOS_ERR   *p_err);

static void IPerf_ArgParse(CPU_INT16U      argc,
                           CPU_CHAR        *p_argv[],
                           IPERF_OPT       *p_opt,
                           IPERF_OUT_FNCT  p_out_fnct,
                           IPERF_OUT_PARAM *p_out_param,
                           RTOS_ERR        *p_err);

static void IPerf_ArgFmtGet(CPU_CHAR  *p_str_int_arg,
                            IPERF_FMT *p_fmt,
                            RTOS_ERR  *p_err);

//                                                                 ---------------- IPERF PRINT FNCTS -----------------
static void IPerf_PrintVer(IPERF_OUT_FNCT  p_out_fnct,
                           IPERF_OUT_PARAM *p_out_param);

static void IPerf_PrintMenu(IPERF_OUT_FNCT  p_out_fnct,
                            IPERF_OUT_PARAM *p_out_param);

static void IPerf_PrintErr(IPERF_OUT_FNCT  p_out_fnct,
                           IPERF_OUT_PARAM *p_out_param,
                           CPU_CHAR        *p_str,
                           RTOS_ERR        err);

//                                                                 ----------------- IPERF TEST FNCTS -----------------
static IPERF_TEST *IPerf_TestSrch(IPERF_TEST_ID test_id);

static IPERF_TEST *IPerf_TestAdd(IPERF_TEST_ID test_id,
                                 RTOS_ERR      *p_err);

static void IPerf_TestRemove(IPERF_TEST *p_test);

static void IPerf_TestInsert(IPERF_TEST *p_test);

static void IPerf_TestUnlink(IPERF_TEST *p_test);

static IPERF_TEST *IPerf_TestGet(RTOS_ERR *p_err);

static void IPerf_TestFree(IPERF_TEST *p_test);

static void IPerf_TestClr(IPERF_TEST *p_test);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           IPerf_ConfigureMemSeg()
 *
 * @brief    Configure the memory segment that will be used to allocate internal data needed by the
 *           IPerf module instead of the default memory segment.
 *
 * @param    p_mem_seg   Pointer to the memory segment from which the internal data will be allocated.
 *                       If DEF_NULL, the internal data will be allocated from the global Heap.
 *
 * @note     (1) This function is optional. If it is called, it must be called before IPerf_Init(). If
 *               it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/
#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void IPerf_ConfigureMemSeg(MEM_SEG *p_mem_seg)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_DBG((IPerf_InitActive != DEF_INACTIVE), RTOS_ERR_ALREADY_INIT,; );
  CORE_EXIT_ATOMIC();

  IPerf_InitCfg.MemSegPtr = p_mem_seg;
}
#endif

/****************************************************************************************************//**
 *                                           IPerf_ConfigureCfg()
 *
 * @brief    Configure the IPerf module parameters.
 *
 * @param    p_cfg   Pointer to the structure containing the IPerf module parameters.
 *
 * @note     (1) This function is optional. If it is called, it must be called before IPerf_Init(). If
 *               it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/
#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void IPerf_ConfigureCfg(const IPERF_CFG *p_cfg)
{
  RTOS_ERR err;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_DBG((IPerf_InitActive != DEF_INACTIVE), RTOS_ERR_ALREADY_INIT,; );
  RTOS_ASSERT_DBG((IPerf_CfgPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  RTOS_ASSERT_DBG_ERR_SET((p_cfg != DEF_NULL), err, RTOS_ERR_NULL_PTR,; );

  IPerf_CfgPtr = p_cfg;
  CORE_EXIT_ATOMIC();

  PP_UNUSED_PARAM(err);
}
#endif

/****************************************************************************************************//**
 *                                           IPerf_ConfigureTaskStk()
 *
 * @brief    Configure the IPerf module parameters.
 *
 * @param    stk_size_elements   Size of the stack, in CPU_STK elements.
 *
 * @param    p_stk_base          Pointer to base of the stack.
 *
 * @note     (1) This function is optional. If it is called, it must be called before IPerf_Init(). If
 *               it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/
#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void IPerf_ConfigureTaskStk(CPU_STK_SIZE stk_size_elements,
                            void         *p_stk_base)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_DBG((IPerf_InitActive != DEF_INACTIVE), RTOS_ERR_ALREADY_INIT,; );
  RTOS_ASSERT_DBG((IPerf_CfgPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  IPerf_TaskCfg.StkSizeElements = stk_size_elements;
  IPerf_TaskCfg.StkPtr = p_stk_base;
  CORE_EXIT_ATOMIC();
}
#endif

/****************************************************************************************************//**
 *                                           IPerf_TaskPrioSet()
 *
 * @brief    Sets priority of the IPerf task.
 *
 * @param    prio    New priority for the IPerf task.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s)
 *                   from this function.
 *******************************************************************************************************/
#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void IPerf_TaskPrioSet(RTOS_TASK_PRIO prio,
                       RTOS_ERR       *p_err)
{
  CPU_BOOLEAN init_active;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();
  init_active = IPerf_InitActive;
  CORE_EXIT_ATOMIC();

  if (init_active == DEF_ACTIVE) {
    CORE_ENTER_ATOMIC();
    IPerf_TaskCfg.Prio = prio;
    CORE_EXIT_ATOMIC();
  } else {
    KAL_TaskPrioSet(IPerf_TaskHandle,
                    prio,
                    p_err);
  }
}
#endif

/****************************************************************************************************//**
 *                                               IPerf_Init()
 *
 * @brief    Initializes and starts the IPerf application.
 *
 * @param    p_cfg       Pointer to the IPerf configuration.
 *                       DEF_NULL, to use the default configuration IPerf_CfgDflt.
 *
 * @param    p_task_cfg  Pointer to the IPerf task configuration.
 *                       DEF_NULL, to use the default configuration IPerf_TaskCfgDflt.
 *
 * @param    p_mem_seg   Memory segment from which internal data will be allocated.
 *                       If DEF_NULL, it will be allocated from the global heap.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                           - RTOS_ERR_SEG_OVF
 *                           - RTOS_ERR_INVALID_CFG
 *                           - RTOS_ERR_NOT_AVAIL
 *                           - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                           - RTOS_ERR_SEG_OVF
 *
 * @internal
 * @note         (1) [INTERNAL] The IPerf test pool MUST be initialized PRIOR to initializing the pool
 *               with pointers to IPerf test.
 * @endinternal
 *******************************************************************************************************/
void IPerf_Init(RTOS_ERR *p_err)
{
  const IPERF_CFG *p_cfg;
  RTOS_TASK_CFG   *p_task_cfg;
  CPU_INT32U      buf_len;
  CPU_BOOLEAN     is_en;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CORE_ENTER_ATOMIC();                                          // See Note #1.
  if (IPerf_InitActive == DEF_INACTIVE) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_ALREADY_INIT);
    goto exit;
  }
  CORE_EXIT_ATOMIC();

  is_en = KAL_FeatureQuery(KAL_FEATURE_CPU_USAGE_GET, KAL_OPT_NONE);
  RTOS_ASSERT_DBG_ERR_SET((is_en == DEF_YES), *p_err, RTOS_ERR_NOT_AVAIL,; );
  is_en = KAL_FeatureQuery(KAL_FEATURE_TASK_CREATE, KAL_OPT_NONE);
  RTOS_ASSERT_DBG_ERR_SET((is_en == DEF_YES), *p_err, RTOS_ERR_NOT_AVAIL,; );
  is_en = KAL_FeatureQuery(KAL_FEATURE_LOCK_CREATE, KAL_OPT_NONE);
  RTOS_ASSERT_DBG_ERR_SET((is_en == DEF_YES), *p_err, RTOS_ERR_NOT_AVAIL,; );

  if (IPerf_CfgPtr == DEF_NULL) {
    IPerf_CfgPtr = &IPerf_CfgDflt;
  }

  p_cfg = IPerf_CfgPtr;
  p_task_cfg = &IPerf_TaskCfg;

  //                                                               --------------- INIT IPERF TEST OBJ POOL ----------------
  Mem_DynPoolCreate("IPerf Test obj Pool",
                    &IPerf_TestObjPool,
                    IPerf_InitCfg.MemSegPtr,
                    sizeof(IPERF_TEST),
                    sizeof(CPU_ALIGN),
                    1u,
                    p_cfg->TestNbrMax,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  IPerf_TestListHeadPtr = DEF_NULL;                             // ------------- INIT IPERF TEST LIST PTR -------------

  IPerf_NextTestID = IPERF_TEST_ID_INIT;                        // ------------- INIT IPERF NEXT TEST ID --------------

  IPerf_LockHandle = KAL_LockCreate("IPerf lock", DEF_NULL, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  buf_len = p_cfg->BufLen + (CPU_CFG_DATA_SIZE - 1);

  IPerf_BufPtr = (CPU_CHAR *)Mem_SegAlloc("IPerf Buffer",
                                          IPerf_InitCfg.MemSegPtr,
                                          buf_len,
                                          p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  IPerf_TaskHandle = KAL_TaskAlloc(IPERF_OS_TASK_NAME,
                                   p_task_cfg->StkPtr,
                                   p_task_cfg->StkSizeElements,
                                   DEF_NULL,
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_free_lock;
  }

  IPerf_QHandle = KAL_QCreate(IPERF_OS_TEST_ID_Q,
                              p_cfg->TestNbrMax + 1u,
                              DEF_NULL,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_free_lock;
  }

  KAL_TaskCreate(IPerf_TaskHandle,
                 IPerf_TestTaskHandler,
                 DEF_NULL,
                 p_task_cfg->Prio,
                 DEF_NULL,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_free_lock;
  }

  CORE_ENTER_ATOMIC();                                          // See Note #1.
  IPerf_InitActive = DEF_INACTIVE;                              // Block iperf fncts/tasks until init complete.
  CORE_EXIT_ATOMIC();

#ifdef  RTOS_MODULE_COMMON_SHELL_AVAIL
  IPerfShell_Init(p_err);
#endif

  goto exit;

exit_free_lock:
  KAL_LockDel(IPerf_LockHandle);

exit:
  return;
}

/****************************************************************************************************//**
 *                                               IPerf_TestStart()
 *
 * @brief    Validates and schedules a new IPerf test.
 *
 * @param    argv            Pointer to the string arguments values.
 *
 * @param    p_out_fnct      Pointer to the string output function.
 *
 * @param    p_out_param     Pointer to the output function parameters.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_NO_MORE_RSRC
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_SEG_OVF
 *
 * @return   Test ID,            if no error(s).
 *           IPERF_TEST_ID_NONE, otherwise.
 *******************************************************************************************************/
IPERF_TEST_ID IPerf_TestStart(CPU_CHAR        *argv,
                              IPERF_OUT_FNCT  p_out_fnct,
                              IPERF_OUT_PARAM *p_out_param,
                              RTOS_ERR        *p_err)
{
  IPERF_TEST_ID test_id = IPERF_TEST_ID_NONE;
  IPERF_OPT     *p_opt;
  IPERF_TEST    *p_test;
  CPU_INT16U    argc;
  CPU_CHAR      *arg_tbl[IPERF_CMD_ARG_NBR_MAX + 1u];
  RTOS_ERR      local_err;
  RTOS_ERR      *p_err_lock;

  //                                                               ---------------- VALIDATE ARGV PTR -----------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, IPERF_TEST_ID_NONE);
  RTOS_ASSERT_DBG_ERR_SET((argv != DEF_NULL), *p_err, RTOS_ERR_INVALID_ARG, IPERF_TEST_ID_NONE);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_err_lock = p_err;

  KAL_LockAcquire(IPerf_LockHandle, KAL_OPT_PEND_BLOCKING, 0, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    IPerf_PrintErr(p_out_fnct, p_out_param, "Failed acquire the lock", *p_err);
    LOG_ERR(("IPerf: failed to acquire lock with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    goto exit;
  }

  //                                                               -------- ADD NEW TEST INTO IPERF TEST LIST ---------
  p_test = IPerf_TestAdd(IPerf_NextTestID, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    IPerf_PrintErr(p_out_fnct, p_out_param, "Failed to add test", *p_err);
    IPerf_TestRemove(p_test);
    goto exit_err;
  }

  //                                                               -------------- SCAN & PARSE CMD LINE ---------------
  argc = IPerf_ArgScan(argv,
                       &arg_tbl[0],
                       IPERF_CMD_ARG_NBR_MAX,
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    IPerf_PrintErr(p_out_fnct, p_out_param, "Failed to scan command line arguments", *p_err);
    IPerf_TestRemove(p_test);
    goto exit_err;
  }

  //                                                               -------------- VALIDATE & PARSE ARGS ---------------
  p_opt = &p_test->Opt;
  IPerf_ArgParse(argc,
                 &arg_tbl[0],
                 p_opt,
                 p_out_fnct,
                 p_out_param,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    IPerf_PrintErr(p_out_fnct, p_out_param, "Failed to parse command line arguments", *p_err);
    IPerf_TestRemove(p_test);
    goto exit_err;
  }

  //                                                               ---------------------- Q TEST ----------------------
  p_test->Status = IPERF_TEST_STATUS_QUEUED;
  KAL_QPost(IPerf_QHandle,
            (void *)p_test,
            DEF_NULL,
            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_test->Status = IPERF_TEST_STATUS_ERR;
    IPerf_PrintErr(p_out_fnct, p_out_param, "Failed to post test", *p_err);
    IPerf_TestRemove(p_test);
    goto exit_err;
  }
  //                                                               ----------- VALIDATE IPERF NEXT TEST ID ------------
  IPerf_NextTestID++;
  if (IPerf_NextTestID == DEF_INT_16U_MAX_VAL) {
    IPerf_NextTestID = IPERF_TEST_ID_INIT;
  }

  test_id = p_test->TestID;

  goto exit;

exit_err:
  p_err_lock = &local_err;

exit:
  KAL_LockRelease(IPerf_LockHandle, p_err_lock);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err_lock) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);
  return (test_id);
}

/****************************************************************************************************//**
 *                                           IPerf_TestShellStart()
 *
 * @brief    Validates and schedules a new IPerf test.
 *
 * @param    argc            Number of arguments.
 *
 * @param    p_argv          Pointer to the string arguments values.
 *
 * @param    p_out_fnct      Pointer to the string output function.
 *
 * @param    p_out_param     Pointer to the output function parameters.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_NO_MORE_RSRC
 *
 * @return   Test ID,            if no error(s).
 *           IPERF_TEST_ID_NONE, otherwise.
 *******************************************************************************************************/
IPERF_TEST_ID IPerf_TestShellStart(CPU_INT16U      argc,
                                   CPU_CHAR        *p_argv[],
                                   IPERF_OUT_FNCT  p_out_fnct,
                                   IPERF_OUT_PARAM *p_out_param,
                                   RTOS_ERR        *p_err)
{
  IPERF_TEST_ID test_id = IPERF_TEST_ID_NONE;
  IPERF_OPT     *p_opt;
  IPERF_TEST    *p_test;
  RTOS_ERR      local_err;
  RTOS_ERR      *p_err_lock;

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_err_lock = p_err;

  KAL_LockAcquire(IPerf_LockHandle, KAL_OPT_PEND_BLOCKING, 0, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("IPerf: failed to acquire lock with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    goto exit_err;
  }

  //                                                               -------- ADD NEW TEST INTO IPERF TEST LIST ---------
  p_test = IPerf_TestAdd(IPerf_NextTestID, p_err);
  if (p_test == DEF_NULL) {
    goto exit_err;                                              // Rtn err from IPerf_TestAdd().
  }

  //                                                               -------------- VALIDATE & PARSE ARGS ---------------
  p_opt = &p_test->Opt;
  IPerf_ArgParse(argc,
                 &p_argv[0],
                 p_opt,
                 p_out_fnct,
                 p_out_param,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    IPerf_TestRemove(p_test);
    goto exit_err;
  }
  //                                                               ---------------------- Q TEST ----------------------
  p_test->Status = IPERF_TEST_STATUS_QUEUED;
  KAL_QPost(IPerf_QHandle,
            (void *) p_test,
            DEF_NULL,
            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_test->Status = IPERF_TEST_STATUS_ERR;
    IPerf_TestRemove(p_test);
    goto exit_err;
  }

  //                                                               ----------- VALIDATE IPERF NEXT TEST ID ------------
  IPerf_NextTestID++;
  if (IPerf_NextTestID == DEF_INT_16U_MAX_VAL) {
    IPerf_NextTestID = IPERF_TEST_ID_INIT;
  }

  test_id = p_test->TestID;
  goto exit;

exit_err:
  p_err_lock = &local_err;

exit:
  KAL_LockRelease(IPerf_LockHandle, p_err_lock);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err_lock) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

  return (test_id);
}

/****************************************************************************************************//**
 *                                           IPerf_TestRelease()
 *
 * @brief    Removes the test in ring array holding.
 *
 * @param    test_id     Test ID of the test to release.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_FOUND
 *                           - RTOS_ERR_NET_OP_IN_PROGRESS
 *******************************************************************************************************/
void IPerf_TestRelease(IPERF_TEST_ID test_id,
                       RTOS_ERR      *p_err)
{
  IPERF_TEST *p_test;
  RTOS_ERR   local_err;
  RTOS_ERR   *p_err_lock;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  p_err_lock = p_err;

  KAL_LockAcquire(IPerf_LockHandle, KAL_OPT_PEND_BLOCKING, 0, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("IPerf: failed to acquire lock with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    goto exit;
  }

  //                                                               --------------- SRCH IPERF TEST LIST ---------------
  p_test = IPerf_TestSrch(test_id);
  if (p_test == DEF_NULL) {                                     // If test NOT found, ...
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);                       // ... rtn err.
    LOG_VRB(("IPerf Get Status Error: IPerf test ID not found.\n"));
    goto exit_err;
  }

  //                                                               ---------- REMOVE TEST OF IPERF TEST LIST ----------
  switch (p_test->Status) {
    case IPERF_TEST_STATUS_RUNNING:                             // If test currently running, ...
      RTOS_ERR_SET(*p_err, RTOS_ERR_NET_OP_IN_PROGRESS);            // ... rtn err.;
      goto exit;

    case IPERF_TEST_STATUS_FREE:
      break;

    case IPERF_TEST_STATUS_QUEUED:
    case IPERF_TEST_STATUS_DONE:
    case IPERF_TEST_STATUS_ERR:
    default:
      IPerf_TestRemove(p_test);
      break;
  }

  goto exit;

exit_err:
  p_err_lock = &local_err;

exit:
  KAL_LockRelease(IPerf_LockHandle, p_err_lock);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err_lock) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);
}

/****************************************************************************************************//**
 *                                           IPerf_GetTestStatus()
 *
 * @brief    Gets the test status.
 *
 * @param    test_id     Test ID of the test to get status.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error
 *                       code(s) from this function :
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_NOT_FOUND
 *
 * @return   Status of specified test.
 *******************************************************************************************************/
IPERF_TEST_STATUS IPerf_TestGetStatus(IPERF_TEST_ID test_id,
                                      RTOS_ERR      *p_err)
{
  IPERF_TEST        *p_test;
  IPERF_TEST_STATUS status;
  RTOS_ERR          local_err;
  RTOS_ERR          *p_err_lock;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, IPERF_TEST_STATUS_ERR);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_err_lock = p_err;

  KAL_LockAcquire(IPerf_LockHandle, KAL_OPT_PEND_BLOCKING, 0, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("IPerf: failed to acquire lock with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    goto exit_err;
  }

  //                                                               --------------- SRCH IPERF TEST LIST ---------------
  p_test = IPerf_TestSrch(test_id);
  if (p_test == DEF_NULL) {                                     // If test NOT found, ...
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);                       // ... rtn err.
    LOG_VRB(("IPerf Get Status Error: IPerf test ID not found.\n"));
    goto exit_err;
  }

  //                                                               ----------------- RTN TEST STATUS ------------------
  status = p_test->Status;

  goto exit;

exit_err:
  p_err_lock = &local_err;
  status = IPERF_TEST_STATUS_ERR;

exit:
  KAL_LockRelease(IPerf_LockHandle, p_err_lock);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err_lock) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);
  return (status);
}

/****************************************************************************************************//**
 *                                           IPerf_GetTestResults()
 *
 * @brief    (1) Gets the test result.
 *
 * @param    test_id         Test ID of the test to Get result.
 *
 * @param    p_test_result   Pointer to structure that will receive the result of specified test.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function :
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_INVALID_STATE
 *
 * @note     (1) Test results can be obtained before, after, or even during a test run.
 *******************************************************************************************************/
void IPerf_TestGetResults(IPERF_TEST_ID test_id,
                          IPERF_TEST    *p_test_result,
                          RTOS_ERR      *p_err)
{
  IPERF_TEST        *p_test;
  IPERF_TEST_STATUS status;
  RTOS_ERR          local_err;
  RTOS_ERR          *p_err_lock = DEF_NULL;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  KAL_LockAcquire(IPerf_LockHandle, KAL_OPT_PEND_BLOCKING, 0, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("IPerf: failed to acquire lock with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    goto exit;
  }

  p_err_lock = p_err;

  //                                                               --------------- SRCH IPERF TEST LIST ---------------
  p_test = IPerf_TestSrch(test_id);
  if (p_test == DEF_NULL) {                                     // If test NOT found, ...
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);                   // ... rtn err.
    LOG_VRB(("IPerf Get Status Error: IPerf test ID not found.\n"));
    goto exit_err;
  }

  //                                                               ---- COPY INTERNAL IPERF TEST TO APP IPERF TEST ----
  status = p_test->Status;
  switch (status) {
    case IPERF_TEST_STATUS_QUEUED:
    case IPERF_TEST_STATUS_RUNNING:
    case IPERF_TEST_STATUS_DONE:
    case IPERF_TEST_STATUS_ERR:
      *p_test_result = *p_test;
      break;

    case IPERF_TEST_STATUS_FREE:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      break;
  }

  goto exit;

exit_err:
  p_err_lock = &local_err;

exit:
  KAL_LockRelease(IPerf_LockHandle, p_err_lock);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err_lock) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);
}

/****************************************************************************************************//**
 *                                               IPerf_Get_TS_ms()
 *
 * @brief    Gets the current CPU timestamp in milliseconds.
 *
 * @return   Current CPU timestamp in milliseconds.
 *******************************************************************************************************/
IPERF_TS_MS IPerf_Get_TS_ms(void)
{
  uint32_t    tick_count;
  uint32_t    ts_ms;

  tick_count = sl_sleeptimer_get_tick_count();
  ts_ms = sl_sleeptimer_tick_to_ms(tick_count);
  return (ts_ms);
}

/****************************************************************************************************//**
 *                                           IPerf_Get_TS_Max_ms()
 *
 * @brief    Gets the maximum of milliseconds supported by the CPU timestamp functionality.
 *
 * @return   Maximum CPU timestamp in milliseconds.
 *******************************************************************************************************/
IPERF_TS_MS IPerf_Get_TS_Max_ms(void)
{
  CPU_TS_TMR_FREQ tmr_freq;
  CPU_INT32U      ts_max;

  tmr_freq = sl_sleeptimer_get_timer_frequency() / DEF_TIME_NBR_mS_PER_SEC;
  ts_max = IPERF_TS_MS_MAX_VAL / tmr_freq;

  return ((IPERF_TS_MS)ts_max);
}

/****************************************************************************************************//**
 *                                           IPerf_GetDataFmtd()
 *
 * @brief    Converts from bytes to the specified unit of measure.
 *
 * @param    fmt         Formats the character.
 *
 * @param    bytes_qty   Bytes quantity to convert.
 *
 * @return   Formatted bytes quantity.
 *******************************************************************************************************/
CPU_INT32U IPerf_GetDataFmtd(IPERF_FMT  fmt,
                             CPU_INT32U bytes_qty)
{
  CPU_INT32U data_fmt;

  //                                                               ------------ DECODE FMT & CONVERT CALC -------------
  switch (fmt) {
    case IPERF_ASCII_FMT_BITS_SEC:
      data_fmt = bytes_qty * DEF_OCTET_NBR_BITS;
      break;

    case IPERF_ASCII_FMT_KBITS_SEC:
      data_fmt = (bytes_qty * DEF_OCTET_NBR_BITS) / 1000u;
      break;

    case IPERF_ASCII_FMT_MBITS_SEC:
      data_fmt = (bytes_qty * DEF_OCTET_NBR_BITS) / (1000u * 1000u);
      break;

    case IPERF_ASCII_FMT_GBITS_SEC:
      data_fmt = (bytes_qty * DEF_OCTET_NBR_BITS) / (1000u * 1000u * 1000u);
      break;

    case IPERF_ASCII_FMT_BYTES_SEC:
      data_fmt = bytes_qty;
      break;

    case IPERF_ASCII_FMT_KBYTES_SEC:
      data_fmt = bytes_qty / 1024u;
      break;

    case IPERF_ASCII_FMT_MBYTES_SEC:
      data_fmt = bytes_qty / (1024u * 1024u);
      break;

    case IPERF_ASCII_FMT_GBYTES_SEC:
      data_fmt = bytes_qty / (1024u * 1024u * 1024u);
      break;

    default:
      data_fmt = 0u;
      break;
  }

  return (data_fmt);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           IPerf_TestTaskHandler()
 *
 * @brief    (1) Handle IPerf tests in the test queue:
 *               - (a) Wait for queued tests
 *               - (b) Search IPerf Test List for test with test id
 *               - (c) Start server or client test
 *
 * @param    p_arg   Pointer to task arguments.
 *
 * @note     (2) IPerf_Tbl[test_id] validated in IPerf_Parse().
 *******************************************************************************************************/
void IPerf_TestTaskHandler(void *p_arg)
{
  IPERF_TEST *p_test;
  IPERF_TEST *p_testQ;
  IPERF_OPT  *p_opt;
  RTOS_ERR   local_err;

  PP_UNUSED_PARAM(p_arg);

  while (DEF_ON) {
    //                                                             --------------- WAIT FOR IPERF TEST ----------------
    do {
      p_testQ = KAL_QPend(IPerf_QHandle,
                          KAL_OPT_PEND_NONE,
                          KAL_TIMEOUT_INFINITE,
                          &local_err);
    } while (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE);

    KAL_LockAcquire(IPerf_LockHandle, KAL_OPT_PEND_BLOCKING, 0, &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      LOG_ERR(("IPerf: failed to acquire lock with err: ", RTOS_ERR_LOG_ARG_GET(local_err)));
      continue;
    }

    //                                                             --------------- SRCH IPERF TEST LIST ---------------
    p_test = IPerf_TestSrch(p_testQ->TestID);
    if (p_test == DEF_NULL) {                                   // If test NOT found, ...
      LOG_VRB(("IPerf Error: IPerf test ID not found.\n"));
      continue;
    }

    p_opt = &p_test->Opt;
    p_test->Err = IPERF_REP_ERR_NONE;

    KAL_LockRelease(IPerf_LockHandle, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, handle);

    //                                                             -------------- START SERVER OR CLIENT --------------
    switch (p_opt->Mode) {
#ifdef  IPERF_SERVER_MODULE_PRESENT
      case IPERF_MODE_SERVER:
        p_test->Status = IPERF_TEST_STATUS_RUNNING;
        IPerf_ServerStart(p_test, &local_err);
        if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
          p_test->Status = IPERF_TEST_STATUS_DONE;
        } else {
          p_test->Status = IPERF_TEST_STATUS_ERR;
        }
        break;
#endif

#ifdef  IPERF_CLIENT_MODULE_PRESENT
      case IPERF_MODE_CLIENT:
        p_test->Status = IPERF_TEST_STATUS_RUNNING;
        p_test->Err = IPERF_REP_ERR_NONE;
        IPerf_ClientStart(p_test, &local_err);
        if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
          p_test->Status = IPERF_TEST_STATUS_DONE;
        } else {
          p_test->Status = IPERF_TEST_STATUS_ERR;
        }
        break;
#endif

      default:
        p_test->Status = IPERF_TEST_STATUS_FREE;
        break;
    }
  }
}

/****************************************************************************************************//**
 *                                           IPerf_TestClrStats()
 *
 * @brief    Clear test statistics.
 *
 * @param    p_stats     Pointer to an IPerf test statistics.
 *******************************************************************************************************/
void IPerf_TestClrStats(IPERF_STATS *p_stats)
{
  p_stats->TS_Start_ms = 0u;
  p_stats->TS_End_ms = 0u;

  p_stats->NbrCalls = 0u;

  p_stats->Errs = 0u;
  p_stats->Bytes = 0u;
  p_stats->TransitoryErrCnts = 0u;

  p_stats->UDP_RxLastPkt = -1;
  p_stats->UDP_LostPkt = 0u;
  p_stats->UDP_OutOfOrder = 0u;
  p_stats->UDP_DupPkt = 0u;
  p_stats->UDP_AsyncErr = DEF_NO;
  p_stats->UDP_EndErr = DEF_NO;

#if (IPERF_CFG_BANDWIDTH_CALC_EN == DEF_ENABLED)
  p_stats->Bandwidth = 0u;
#endif
#if (IPERF_CFG_CPU_USAGE_MAX_CALC_EN == DEF_ENABLED)
  p_stats->CPU_UsageMax = 0u;
  p_stats->CPU_UsageAvg = 0u;
  p_stats->CPU_CalcNbr = 0u;
#endif
}

/****************************************************************************************************//**
 *                                           IPerf_UpdateBandwidth()
 *
 * @brief    Calculation of the bandwidth and update of test statistics.
 *
 * @param    p_test              Pointer to a test.
 *
 * @param    p_ts_ms_prev        Pointer to variable that will receive the previous Time Stamp.
 *
 * @param    p_data_bytes_prev   Pointer to variable that will receive the previous data bytes number.
 *******************************************************************************************************/

#if (IPERF_CFG_BANDWIDTH_CALC_EN == DEF_ENABLED)
void IPerf_UpdateBandwidth(IPERF_TEST  *p_test,
                           IPERF_TS_MS *p_ts_ms_prev,
                           CPU_INT32U  *p_data_bytes_prev)
{
  IPERF_STATS *p_stats;
  IPERF_OPT   *p_opt;
  CPU_INT32U  data_bytes_cur = 0u;
  CPU_INT32U  data_bytes_delta;
  CPU_INT32U  data_fmtd_delta;
  IPERF_TS_MS ts_max;
  IPERF_TS_MS ts_ms_cur;
  IPERF_TS_MS ts_ms_delta;

  p_stats = &p_test->Stats;
  p_opt = &p_test->Opt;

  //                                                               elapsed time calc.
  if (p_stats->TS_End_ms != 0u) {
    *p_ts_ms_prev = p_stats->TS_Start_ms;
    ts_ms_cur = p_stats->TS_End_ms;
    data_bytes_delta = p_stats->Bytes;
  } else {
    data_bytes_cur = p_stats->Bytes;
    data_bytes_delta = data_bytes_cur - *p_data_bytes_prev;
    ts_ms_cur = IPerf_Get_TS_ms();
  }

  if (ts_ms_cur >= *p_ts_ms_prev) {
    ts_ms_delta = (ts_ms_cur - *p_ts_ms_prev);
  } else {
    ts_max = IPerf_Get_TS_Max_ms();
    ts_ms_delta = (ts_ms_cur + (ts_max - *p_ts_ms_prev));
  }

  if (ts_ms_delta >= p_opt->Interval_ms) {
    if (ts_ms_delta == 0) {                                     // Prevent division by 0.
      return;
    }
    //                                                             Get amount of data formated correclty.
    data_fmtd_delta = IPerf_GetDataFmtd(p_opt->Fmt, data_bytes_delta);

    //                                                             Bandwidth calc.
    p_stats->Bandwidth = (data_fmtd_delta * DEF_TIME_NBR_mS_PER_SEC) / ts_ms_delta;

    *p_ts_ms_prev = ts_ms_cur;                                  // Update ptrs values for the next call.
    *p_data_bytes_prev = data_bytes_cur;

    LOG_VRB(("Time Interval (ms)       = %u\n\r", (u)ts_ms_delta));
    LOG_VRB(("Delta bytes tx'd or rx'd = %u\n\r", (u)data_bytes_delta));
    LOG_VRB(("Bandwidth                = %u [format = %c]\n\r\n\r\n\r", (u)p_stats->Bandwidth, (u)p_opt->Fmt));
  }
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               IPerf_ArgScan()
 *
 * @brief    Scan & parse the command line.
 *
 * @param    p_in            Pointer to a NULL terminated string holding a complete command & its argument(s).
 *
 * @param    p_arg_tbl       Pointer to an array of pointer that will receive pointers to token.
 *
 * @param    arg_tbl_size    Size of arg_tbl array.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Number of token(s) (command name & argument(s)).
 *
 * @note     (1) The first token is always the command name itself.
 *
 * @note     (2) This function modify the 'in' arguments by replacing token's delimiter characters by
 *               termination character ('\0').
 *******************************************************************************************************/
static CPU_INT16U IPerf_ArgScan(CPU_CHAR   *p_in,
                                CPU_CHAR   *p_arg_tbl[],
                                CPU_INT16U arg_tbl_size,
                                RTOS_ERR   *p_err)
{
  CPU_CHAR    *p_in_rd;
  CPU_INT16U  tok_ix;
  CPU_BOOLEAN end_tok_found;
  CPU_BOOLEAN quote_opened;

  p_in_rd = p_in;
  tok_ix = 0u;
  end_tok_found = DEF_YES;
  quote_opened = DEF_NO;
  //                                                               ------------------ SCAN CMD LINE  ------------------
  while (*p_in_rd) {
    switch (*p_in_rd) {
      case IPERF_ASCII_QUOTE:                                   // Quote char found.
        if (quote_opened == DEF_YES) {
          quote_opened = DEF_NO;
          *p_in_rd = (CPU_CHAR)0;
          end_tok_found = DEF_YES;
        } else {
          quote_opened = DEF_YES;
        }
        break;

      case IPERF_ASCII_SPACE:                                   // Space char found.
        if ((end_tok_found == DEF_NO)                           // If first space between tok & quote NOT opened ...
            && (quote_opened == DEF_NO)) {
          *p_in_rd = IPERF_ASCII_ARG_END;                       // ... put termination char.
          end_tok_found = DEF_YES;
        }
        break;

      default:                                                  // Other char found.
        if (end_tok_found == DEF_YES) {
          if (tok_ix < arg_tbl_size) {
            p_arg_tbl[tok_ix] = p_in_rd;                        // Set p_arg_tbl ptr to tok location.
            tok_ix++;
            end_tok_found = DEF_NO;
          } else {
            RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
            goto exit_err;
          }
        }
        break;
    }
    p_in_rd++;
  }

  goto exit;

exit_err:
  tok_ix = 0u;

exit:
  return (tok_ix);
}

/****************************************************************************************************//**
 *                                               IPerf_ArgParse()
 *
 * @brief    (1) Valid argument & set IPerf command structure :
 *               - (a) Validate arguments counts
 *               - (b) Read from command line option & overwrite default test options
 *               - (c) Validate test mode set
 *               - (d) Validate buffer size option to not exceed maximum buffer size
 *
 * @param    argc            Count of the arguments supplied & argv.
 *
 * @param    p_argv          Pointer to an array of pointers to the strings which are arguments.
 *
 * @param    p_opt           Pointer to the current IPerf test cmd.
 *
 * @param    p_out_fnct      Pointer to string output function.
 *
 * @param    p_out_param     Pointer to output function parameters.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from
 *                           this function.
 *******************************************************************************************************/
static void IPerf_ArgParse(CPU_INT16U      argc,
                           CPU_CHAR        *p_argv[],
                           IPERF_OPT       *p_opt,
                           IPERF_OUT_FNCT  p_out_fnct,
                           IPERF_OUT_PARAM *p_out_param,
                           RTOS_ERR        *p_err)
{
  CPU_INT16U  arg_ctr;                                          // Switch between each param/val.
  CPU_INT16U  arg_int;
  CPU_BOOLEAN find_opt;                                         // Switch between param & val srch.
  CPU_BOOLEAN next_arg;                                         // Switch to the next arg.
  CPU_BOOLEAN server_client_found;
  CPU_BOOLEAN help_version_found;
  CPU_BOOLEAN len_found;                                        // Prevent erasing len with UDP protocol.
  CPU_BOOLEAN parse_done;                                       // Exit parsing while.
  CPU_CHAR    opt_found;                                        // Save the opt val wish will be found.
  CPU_CHAR    *p_opt_line;                                      // Ptr  to cur pos in the cmd line.
  CPU_BOOLEAN win_found;                                        // Prevent erasing win size.
#ifdef  IPERF_CLIENT_MODULE_PRESENT
  CPU_BOOLEAN valid_char;
#endif
  CPU_INT16U buf_len;
  CORE_DECLARE_IRQ_STATE;

  //                                                               ----------------- VALIDATE ARG CNT -----------------
  if (argc <= 1u) {
    p_out_fnct(IPERF_MSG_ERR_OPT, p_out_param);
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    LOG_VRB(("IPerf no arg error : %u\n", (u)RTOS_ERR_CODE_GET(*p_err)));
    return;
  }

  find_opt = DEF_YES;                                           // Start by trying to find an opt.
  next_arg = DEF_YES;                                           // First arg is the cmd name.
  len_found = DEF_NO;
  server_client_found = DEF_NO;
  help_version_found = DEF_NO;
  parse_done = DEF_NO;
  arg_ctr = 0;
  p_opt_line = p_argv[arg_ctr];
  win_found = DEF_NO;

  //                                                               ------------------ RD & SET OPTS -------------------
  while (parse_done == DEF_NO) {
    if (next_arg == DEF_YES) {
      arg_ctr++;
      if (arg_ctr < argc) {
        p_opt_line = p_argv[arg_ctr];                           // Set ptr to the next possible opt.
      } else {
        parse_done = DEF_YES;
      }
    }

    if ((find_opt == DEF_YES)
        && (*p_opt_line == IPERF_ASCII_BEGIN_OPT)) {
      p_opt_line++;                                             // Param found.
      if (p_opt_line[1] == IPERF_ASCII_ARG_END) {               // If param have only one char, it's valid.
        find_opt = DEF_NO;                                      // Try to find a val.
        next_arg = DEF_YES;                                     // Switch to the next argv.
        opt_found = *p_opt_line;                                // Save opt found.
      } else {
        p_out_fnct(IPERF_MSG_ERR_OPT, p_out_param);
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);                 // Param with more than 2 chars is invalid.
      }
    } else if (find_opt == DEF_NO) {                            // If an opt is found.
      next_arg = DEF_YES;
      switch (opt_found) {
        case IPERF_ASCII_OPT_HELP:                              // Help opt found.
          IPerf_PrintMenu(p_out_fnct, p_out_param);
          help_version_found = DEF_YES;
          next_arg = DEF_NO;
          break;

        case IPERF_ASCII_OPT_VER:                               // Ver opt found.
          IPerf_PrintVer(p_out_fnct, p_out_param);
          help_version_found = DEF_YES;
          next_arg = DEF_NO;
          break;

        case IPERF_ASCII_OPT_SERVER:                            // Server opt found.
#ifdef  IPERF_SERVER_MODULE_PRESENT
          if (server_client_found == DEF_NO) {                  // IF mode has not been set.
            server_client_found = DEF_YES;
            p_opt->Mode = IPERF_MODE_SERVER;
            if (win_found == DEF_NO) {
              p_opt->WinSize = IPERF_DFLT_RX_WIN;
            }
          } else {                                              // Mode can't be set more than one time.
            p_out_fnct(IPERF_MSG_ERR_OPT, p_out_param);
            RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
          }
          next_arg = DEF_NO;
#else
          RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);               // Rtn err if mode opt not en.
#endif
          break;

        case IPERF_ASCII_OPT_CLIENT:                            // Client opt found.
#ifdef  IPERF_CLIENT_MODULE_PRESENT
          if (server_client_found == DEF_NO) {
            server_client_found = DEF_YES;

            valid_char = ASCII_IsDigHex(*p_opt_line);
            if (valid_char == DEF_FALSE) {
              p_out_fnct(IPERF_MSG_ERR_OPT, p_out_param);
              RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
              return;
            }

            Str_Copy(p_opt->IP_AddrRemote, p_opt_line);
            p_opt->Mode = IPERF_MODE_CLIENT;

            if (win_found == DEF_NO) {
              p_opt->WinSize = IPERF_DFLT_TX_WIN;
            }
          } else {                                              // Mode can't be set more than one time.
            p_out_fnct(IPERF_MSG_ERR_OPT, p_out_param);
            RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
          }
#else
          p_out_fnct(IPERF_MSG_ERR_NOT_EN, p_out_param);
          RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);               // Rtn err if mode opt not en'd.
#endif
          break;

        case IPERF_ASCII_OPT_FMT:                               // Fmt opt found.
          IPerf_ArgFmtGet(p_opt_line, &p_opt->Fmt, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            p_out_fnct(IPERF_MSG_ERR_OPT, p_out_param);
          }
          break;

        case IPERF_ASCII_OPT_TIME:                              // Time dur opt found.
          arg_int = Str_ParseNbr_Int32U(p_opt_line,
                                        DEF_NULL,
                                        DEF_NBR_BASE_DEC);
          p_opt->Duration_ms = arg_int * DEF_TIME_NBR_mS_PER_SEC;
          p_opt->BytesNbr = 0u;
          break;

        case IPERF_ASCII_OPT_NUMBER:                            // Buf nbr opt found.
          p_opt->BytesNbr = Str_ParseNbr_Int32U(p_opt_line,
                                                DEF_NULL,
                                                DEF_NBR_BASE_DEC);
          p_opt->Duration_ms = 0u;
          break;

        case IPERF_ASCII_OPT_LENGTH:                            // Len opt found.
          p_opt->BufLen = Str_ParseNbr_Int32U(p_opt_line,
                                              DEF_NULL,
                                              DEF_NBR_BASE_DEC);
          len_found = DEF_YES;
          break;

        case IPERF_ASCII_OPT_PORT:                              // Port opt found.
          p_opt->Port = Str_ParseNbr_Int32U(p_opt_line,
                                            DEF_NULL,
                                            DEF_NBR_BASE_DEC);
          break;

        case IPERF_ASCII_OPT_UDP:                               // UDP mode opt found.
          p_opt->Protocol = IPERF_PROTOCOL_UDP;
          if (len_found == DEF_NO) {
            p_opt->BufLen = IPERF_DFLT_UDP_BUF_LEN;             // Use dflt UDP len.
          }                                                     // UDP frag not yet supported.
          if (p_opt->BufLen > IPERF_UDP_BUF_LEN_MAX) {
            p_out_fnct(IPERF_MSG_ERR_UDP_LEN, p_out_param);
            RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
          }
          next_arg = DEF_NO;
          break;

        case IPERF_ASCII_OPT_WINDOW:                            // Win opt found.
          p_opt->WinSize = Str_ParseNbr_Int32U(p_opt_line,
                                               DEF_NULL,
                                               DEF_NBR_BASE_DEC);
          win_found = DEF_YES;
          break;

        case IPERF_ASCII_OPT_PERSISTENT:                        // Persistent server opt found.
          p_opt->Persistent = DEF_ENABLED;
          next_arg = DEF_NO;
          break;

        case IPERF_ASCII_OPT_INTERVAL:                          // Interval opt found.
          p_opt->Interval_ms = Str_ParseNbr_Int32U(p_opt_line,
                                                   DEF_NULL,
                                                   DEF_NBR_BASE_DEC);
          break;

        case IPERF_ASCII_OPT_IPV6:                              // IPV6 opt found.
          p_opt->IPv4 = DEF_NO;
          break;

        default:                                                // Unknown opt found.
          p_out_fnct(IPERF_MSG_ERR_OPT, p_out_param);
          RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);               // Rtn invalid opt err.
          break;
      }
      find_opt = DEF_YES;
    } else {
      p_opt_line++;
    }

    if ((p_opt_line == ASCII_CHAR_NULL)
        || (arg_ctr >= argc)              ) {
      parse_done = DEF_YES;
    }

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_VRB(("IPerf option error : %u\n", (u)RTOS_ERR_CODE_GET(*p_err)));

      return;
    }
  }

  //                                                               ---------------- VALIDATE TEST MODE ----------------
  if (server_client_found == DEF_NO) {
    if (help_version_found == DEF_NO) {
      p_out_fnct(IPERF_MSG_ERR_OPT, p_out_param);
    }
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return;
  }
  //                                                               ---------------- VALIDATE BUF SIZE -----------------
  CORE_ENTER_ATOMIC();
  buf_len = IPerf_CfgPtr->BufLen;
  CORE_EXIT_ATOMIC();
  if (p_opt->IPv4 == DEF_YES) {
    if (((p_opt->Protocol == IPERF_PROTOCOL_TCP) && (p_opt->BufLen > buf_len))
        || ((p_opt->Protocol == IPERF_PROTOCOL_UDP) && (p_opt->BufLen > IPERF_UDP_BUF_LEN_MAX))) {
      p_out_fnct(IPERF_MSG_ERR_BUF_LEN, p_out_param);
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      return;
    }
  } else {
    if (((p_opt->Protocol == IPERF_PROTOCOL_TCP) && (p_opt->BufLen > buf_len))
        || ((p_opt->Protocol == IPERF_PROTOCOL_UDP) && (p_opt->BufLen > IPERF_UDP_BUF_LEN_MAX_IPv6))) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      return;
    }
  }
}

/****************************************************************************************************//**
 *                                               IPerf_ArgFmtGet()
 *
 * @brief    (1) Get format argument from command line string :
 *                   - (a) Validate string pointer
 *                   - (b) Validate format & set test option
 *
 * @param    p_str_int_arg   Pointer to first string charater of the integer argument value in the arguments array.
 *
 * @param    p_fmt           Pointer to variable that will receive the format result.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void IPerf_ArgFmtGet(CPU_CHAR  *p_str_arg,
                            IPERF_FMT *p_fmt,
                            RTOS_ERR  *p_err)
{
  if (p_str_arg[1] != IPERF_ASCII_ARG_END) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);                     // Can NOT have more than one char.
    goto exit;
  }

  //                                                               ---------------- VALIDATE & SET FMT ----------------
  *p_fmt = *p_str_arg;
  switch (*p_fmt) {
    case IPERF_ASCII_FMT_BITS_SEC:
    case IPERF_ASCII_FMT_KBITS_SEC:
    case IPERF_ASCII_FMT_MBITS_SEC:
    case IPERF_ASCII_FMT_GBITS_SEC:
    case IPERF_ASCII_FMT_BYTES_SEC:
    case IPERF_ASCII_FMT_KBYTES_SEC:
    case IPERF_ASCII_FMT_MBYTES_SEC:
    case IPERF_ASCII_FMT_GBYTES_SEC:
      break;

    case IPERF_ASCII_FMT_ADAPTIVE_BITS_SEC:
    case IPERF_ASCII_FMT_ADAPTIVE_BYTES_SEC:
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      break;
  }

  goto exit;

exit:
  return;
}

/****************************************************************************************************//**
 *                                           IPerf_PrintVersion()
 *
 * @brief    Output IPerf version via IPerf output display function.
 *           - (a) Validate output function pointer
 *           - (b) Print version
 *
 * @param    p_out_fnct      Pointer to string output function.
 *
 * @param    p_out_param     Pointer to output function parameters.
 *******************************************************************************************************/
static void IPerf_PrintVer(IPERF_OUT_FNCT  p_out_fnct,
                           IPERF_OUT_PARAM *p_out_param)
{
  CPU_INT32U ver;
  CPU_CHAR   buf[IPERF_MSG_VER_STR_MAX_LEN + 1];

  //                                                               ---------------- VALIDATE FNCT PTR -----------------
  if (p_out_fnct == DEF_NULL) {
    return;
  }

  //                                                               ------------------ PRINT VERSION -------------------
  ver = IPERF_VERSION / 10000u;                                 // Major.
  (void)Str_FmtNbr_Int32U(ver, 2, DEF_NBR_BASE_DEC, ' ', DEF_NO, DEF_NO, &buf[0]);
  buf[2] = '.';

  ver = IPERF_VERSION % 10000u;                                 // Minor.
  ver = ver           / 100u;
  (void)Str_FmtNbr_Int32U(ver, 2, DEF_NBR_BASE_DEC, '0', DEF_NO, DEF_NO, &buf[3]);
  buf[5] = '.';

  ver = IPERF_VERSION % 100u;                                   // Revision.
  (void)Str_FmtNbr_Int32U(ver, 2, DEF_NBR_BASE_DEC, '0', DEF_NO, DEF_YES, &buf[6]);

  p_out_fnct("iperf version", p_out_param);
  p_out_fnct(buf, p_out_param);
  p_out_fnct("\r\n", p_out_param);
}

/****************************************************************************************************//**
 *                                               IPerf_PrintMenu()
 *
 * @brief    Output IPerf menu via IPerf output display function.
 *               - (a) Validate output function pointer
 *               - (b) Print menu
 *
 * @param    p_out_fnct      Pointer to string output function.
 *
 * @param    p_out_param     Pointer to output function parameters.
 *******************************************************************************************************/
static void IPerf_PrintMenu(IPERF_OUT_FNCT  p_out_fnct,
                            IPERF_OUT_PARAM *p_out_param)
{
  //                                                               ---------------- VALIDATE FNCT PTR -----------------
  if (p_out_fnct == DEF_NULL) {
    return;
  }

  //                                                               -------------------- PRINT MENU --------------------
  p_out_fnct(IPERF_MSG_MENU, p_out_param);
}

/****************************************************************************************************//**
 *                                               IPerf_PrintErr()
 *
 * @brief    Output error code string via IPerf output display function.
 *           - (a) Validate output function pointer
 *           - (b) Print error string
 *
 * @param    p_out_fnct      Pointer to string output function.
 *
 * @param    p_out_param     Pointer to        output function parameters.
 *
 * @param    err             Desired error code to output corresponding error code string.
 *
 *           IPerf_TestStart().
 *******************************************************************************************************/
static void IPerf_PrintErr(IPERF_OUT_FNCT  p_out_fnct,
                           IPERF_OUT_PARAM *p_out_param,
                           CPU_CHAR        *p_str,
                           RTOS_ERR        err)
{
  CPU_CHAR *p_err_str;
#if (RTOS_ERR_CFG_STR_EN == DEF_DISABLED \
     || RTOS_ERR_CFG_EXT_EN == DEF_DISABLED)
  CPU_CHAR str[6];
#endif
  //                                                               ---------------- VALIDATE FNCT PTR -----------------
  if (p_out_fnct == (IPERF_OUT_FNCT)0) {
    return;
  }

  //                                                               ---------------- PRINT ERR CODE STR ----------------
  p_out_fnct("IPerf error occurred: ", p_out_param);
  p_out_fnct(p_str, p_out_param);
  p_out_fnct(STR_NEW_LINE, p_out_param);

#if (RTOS_ERR_CFG_STR_EN == DEF_ENABLED \
     && RTOS_ERR_CFG_EXT_EN == DEF_ENABLED)
  p_err_str = (CPU_CHAR *)err.DescText;
#else
  Str_FmtNbr_Int32U(RTOS_ERR_CODE_GET(err), 5, DEF_NBR_BASE_DEC, DEF_NO, DEF_NO, DEF_YES, str);
  p_err_str = str;
#endif

  p_out_fnct(p_err_str, p_out_param);

  LOG_ERR(("IPerf error occurred: ", (s)p_str, ". Error =", (s)p_err_str));
}

/****************************************************************************************************//**
 *                                               IPerf_TestSrch()
 *
 * @brief    Search IPerf test List for test with specific test ID.
 *       - (1) IPerf tests are linked to form an IPerf Test List.
 *           - (a) In the diagram below, ... :
 *               - (1) The horizontal row represents the list of IPerf tests.
 *               - (2) 'IPerf_TestListHead' points to the head of the IPerf Test List.
 *               - (3) IPerf tests' 'PrevPtr' & 'NextPtr' doubly-link each test to form the
 *                     IPerf Test List.
 *           - (b) (1) For any IPerf Test List lookup, all IPerf tests are searched in order
 *                       to find the test with the appropriate test ID.
 *               - (2) To expedite faster IPerf Test List lookup :
 *                   - (A) (1) (a) Iperf tests are added at;            ...
 *                           - (b) Iperf tests are searched starting at ...
 *                       - (2) ... the head of the IPerf Test List.
 *                   - (B) As IPerf tests are added into the list, older IPerf tests migrate to the tail
 *                           of the IPerf Test List.  Once an IPerf test is left, it is removed from the
 *                           IPerf Test List.
 *
 *                                   |                                               |
 *                                   |<------------ List of IPerf Tests ------------>|
 *                                   |                (see Note #1a1)                |
 *
 *                               New IPerf test                            Oldest Iperf test
 *                               inserted at head                           in IPerf Test List
 *                               (see Note #1b2A2)                           (see Note #1b2B)
 *
 *                                       |                 NextPtr                 |
 *                                       |             (see Note #1a3)             |
 *                                       v                    |                    v
 *                                                            |
 *                  Head of IPerf     -------       -------   v   -------       -------
 *                    Test List  ---->|     |------>|     |------>|     |------>|     |
 *                                    |     |       |     |       |     |       |     |        Tail of IPerf
 *                 (see Note #1a2)    |     |<------|     |<------|     |<------|     |<----     Test List
 *                                    -------       -------   ^   -------       -------
 *                                                            |
 *                                                            |
 *                                                         PrevPtr
 *                                                    (see Note #1a3)
 *
 * @param    test_id     Interface number to search for host group.
 *
 * @return   Pointer to IPerf test with specific test ID, if found.
 *           Pointer to NULL, otherwise.
 *******************************************************************************************************/
static IPERF_TEST *IPerf_TestSrch(IPERF_TEST_ID test_id)
{
  IPERF_TEST  *p_test;
  IPERF_TEST  *p_test_next;
  CPU_BOOLEAN found;

  p_test = IPerf_TestListHeadPtr;                               // Start @ IPerf Test List head.
  found = DEF_NO;

  while ((p_test != DEF_NULL)                                   // Srch Iperf Test List ...
         && (found == DEF_NO)) {                                // ... until test id found.
    p_test_next = (IPERF_TEST *) p_test->NextPtr;
    //                                                             Cmp TestID.
    found = (p_test->TestID == test_id) ? DEF_YES : DEF_NO;

    if (found != DEF_YES) {                                     // If NOT found, adv to next IPerf Test.
      p_test = p_test_next;
    }
  }

  return (p_test);
}

/****************************************************************************************************//**
 *                                               IPerf_TestAdd()
 *
 * @brief    (1) Add a test to the IPerf Test List :
 *               - (a) Get a     test from test pool
 *               - (b) Configure test
 *               - (c) Insert    test into IPerf Test List
 *
 * @param    test_id     Test ID to add test.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to test, if no error.
 *           Pointer to NULL, otherwise.
 *******************************************************************************************************/
static IPERF_TEST *IPerf_TestAdd(IPERF_TEST_ID test_id,
                                 RTOS_ERR      *p_err)
{
  IPERF_TEST *p_test = DEF_NULL;

  p_test = IPerf_TestGet(p_err);                                // ------------------ GET IPERF TEST ------------------
  if (p_test == DEF_NULL) {
    goto exit;                                                  // Rtn err from IPerf_TestGet().
  }

  //                                                               ------------------ CFG IPERF TEST ------------------
  p_test->TestID = test_id;

  //                                                               --------- INSERT TEST INTO IPERF TEST LIST ---------
  IPerf_TestInsert(p_test);

exit:
  return (p_test);
}

/****************************************************************************************************//**
 *                                           IPerf_TestRemove()
 *
 * @brief    Remove a test from the IPerf Test List :
 *               - (a) Remove test from IPerf Test List
 *               - (b) Free   test back to    test pool
 *
 * @param    p_test  Pointer to a test.
 *******************************************************************************************************/
static void IPerf_TestRemove(IPERF_TEST *p_test)
{
  IPerf_TestUnlink(p_test);                                     // --------- REMOVE TEST FROM IPERF TEST LIST ---------

  IPerf_TestFree(p_test);                                       // -------------------- FREE TEST ---------------------
}

/****************************************************************************************************//**
 *                                           IPerf_TestInsert()
 *
 * @brief    Insert a test into the IPerf Test List.
 *
 * @param    p_test  Pointer to a test.
 *******************************************************************************************************/
static void IPerf_TestInsert(IPERF_TEST *p_test)
{
  //                                                               --------------- CFG IPERF TEST PTRS ----------------
  p_test->PrevPtr = DEF_NULL;
  p_test->NextPtr = IPerf_TestListHeadPtr;

  //                                                               ----------- INSERT IPERF TEST INTO LIST ------------
  if (IPerf_TestListHeadPtr != DEF_NULL) {                      // If list NOT empty, insert before head.
    IPerf_TestListHeadPtr->PrevPtr = p_test;
  }

  IPerf_TestListHeadPtr = p_test;                               // Insert test @ list head.
}

/****************************************************************************************************//**
 *                                           IPerf_TestUnlink()
 *
 * @brief    Unlink a test from the IPerf Test List.
 *
 * @param    p_test  Pointer to a test.
 *******************************************************************************************************/
static void IPerf_TestUnlink(IPERF_TEST *p_test)
{
  IPERF_TEST *p_test_prev;
  IPERF_TEST *p_test_next;

  //                                                               ----------- UNLINK IPERF TEST FROM LIST ------------
  p_test_prev = p_test->PrevPtr;
  p_test_next = p_test->NextPtr;
  //                                                               Point prev test to next test.
  if (p_test_prev != DEF_NULL) {
    p_test_prev->NextPtr = p_test_next;
  } else {
    IPerf_TestListHeadPtr = p_test_next;
  }
  //                                                               Point next test to prev test.
  if (p_test_next != DEF_NULL) {
    p_test_next->PrevPtr = p_test_prev;
  }

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)                     // Clr test ptrs.
  p_test->PrevPtr = DEF_NULL;
  p_test->NextPtr = DEF_NULL;
#endif
}

/****************************************************************************************************//**
 *                                               IPerf_TestGet()
 *
 * @brief    (1) Allocate & initialize a test :
 *               - (a) Get a test
 *               - (b) Validate   test
 *               - (c) Initialize test
 *               - (e) Return pointer to test
 *                       OR
 *                       Null pointer & error code, on failure
 *
 *       - (2) The test pool is implemented as a stack :
 *           - (a) 'IPerf_TestPoolPtr' points to the head   of the test pool.
 *           - (b) Tests' 'NextPtr's link each test to form    the test pool stack.
 *           - (c) Tests are inserted & removed at the head of the test pool stack.
 *
 *                                       Test are
 *                               inserted & removed
 *                                   at the head
 *                                   (see Note #2c)
 *
 *                                       |                 NextPtr
 *                                       |             (see Note #2b)
 *                                       v                    |
 *                                                               |
 *                                       -------       -------   v   -------       -------
 *                           Test   ---->|     |------>|     |------>|     |------>|     |
 *                       Pointer       |     |       |     |       |     |       |     |
 *                                       |     |       |     |       |     |       |     |
 *                   (see Note #2a)   -------       -------       -------       -------
 *
 *                                       |                                               |
 *                                       |<------------ Pool of Free Tests ------------->|
 *                                       |                (see Note #2)                  |
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to test, if NO errors.
 *           Pointer to NULL, otherwise.
 *
 * @note         (3) (a) Test pool is accessed by 'IPerf_TestPoolPtr' during execution of
 *
 *               - (1) IPerf_Init()
 *               - (2) IPerf_TestGet()
 *               - (3) IPerf_TestFree()
 *******************************************************************************************************/
static IPERF_TEST *IPerf_TestGet(RTOS_ERR *p_err)
{
  IPERF_TEST *p_test;

  p_test = (IPERF_TEST *) Mem_DynPoolBlkGet(&IPerf_TestObjPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }
  //                                                               -------------------- INIT TEST ---------------------
  IPerf_TestClr(p_test);

  return (p_test);
}

/****************************************************************************************************//**
 *                                               IPerf_TestFree()
 *
 * @brief    (1) Free a test :
 *               - (b) Clear  test controls
 *               - (c) Free   test back to test pool
 *
 * @param    p_test  Pointer to a test.
 *******************************************************************************************************/
static void IPerf_TestFree(IPERF_TEST *p_test)
{
  RTOS_ERR local_err;

  Mem_DynPoolBlkFree(&IPerf_TestObjPool, (void *) p_test, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    return;
  }
}

/****************************************************************************************************//**
 *                                               IPerf_TestClr()
 *
 * @brief    Clear the given IPerf test.
 *
 * @param    p_test  Pointer to an IPerf test.
 *******************************************************************************************************/
static void IPerf_TestClr(IPERF_TEST *p_test)
{
  IPERF_OPT     *p_opt;
  IPERF_STATS   *p_stats;
  IPERF_CONN    *p_conn;
  NET_SOCK_ADDR *p_addr_sock;
  CORE_DECLARE_IRQ_STATE;

  p_opt = &p_test->Opt;
  p_conn = &p_test->Conn;
  p_stats = &p_test->Stats;

  p_test->PrevPtr = DEF_NULL;
  p_test->NextPtr = DEF_NULL;

  p_test->TestID = IPERF_TEST_ID_NONE;
  p_test->Status = IPERF_TEST_STATUS_FREE;
  p_test->Err = IPERF_REP_ERR_NONE;

  p_conn->SockID = NET_SOCK_ID_NONE;
  p_conn->SockID_TCP_Server = NET_SOCK_ID_NONE;
  p_conn->IF_Nbr = IPERF_DFLT_IF;
  p_conn->Run = DEF_NO;

  p_addr_sock = &p_conn->ServerAddrPort;
  Mem_Clr(p_addr_sock, NET_SOCK_ADDR_SIZE);

  p_addr_sock = &p_conn->ClientAddrPort;
  Mem_Clr(p_addr_sock, NET_SOCK_ADDR_SIZE);

  p_opt->Mode = IPERF_MODE_SERVER;
  p_opt->Protocol = IPERF_DFLT_PROTOCOL;
  p_opt->Port = IPERF_DFLT_PORT;
  p_opt->IPv4 = DEF_YES;

  Str_Copy(p_opt->IP_AddrRemote, IPERF_DFLT_IP_REMOTE);
  p_opt->BytesNbr = IPERF_DFLT_BYTES_NBR;
  CORE_ENTER_ATOMIC();
  p_opt->BufLen = IPerf_CfgPtr->BufLen;
  CORE_EXIT_ATOMIC();
  p_opt->Duration_ms = IPERF_DFLT_DURATION_MS;
  p_opt->WinSize = IPERF_DFLT_RX_WIN;
  p_opt->Persistent = IPERF_DFLT_PERSISTENT;
  p_opt->Fmt = IPERF_DFLT_FMT;
  p_opt->Interval_ms = IPERF_DFLT_INTERVAL_MS;

  IPerf_TestClrStats(p_stats);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_IPERF_AVAIL
