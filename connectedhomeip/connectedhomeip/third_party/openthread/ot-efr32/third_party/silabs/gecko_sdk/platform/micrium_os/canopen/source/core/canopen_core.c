/***************************************************************************//**
 * @file
 * @brief CANopen Protocol Core Operations
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

#include  <em_core.h>

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_prio.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#include  <canopen_cfg.h>
#include  <canopen/include/canopen_core.h>
#include  <canopen/include/canopen_dict.h>
#include  <canopen/include/canopen_if.h>
#include  <canopen/source/core/canopen_core_priv.h>
#include  <canopen/source/core/canopen_if_priv.h>
#include  <canopen/source/nmt/canopen_nmt_priv.h>
#if (CANOPEN_EMCY_MAX_ERR_QTY > 0)
#include  <canopen/source/objects/emcy/canopen_emcy_priv.h>
#endif
#include  <canopen/source/objects/pdo/canopen_rpdo_priv.h>
#include  <canopen/source/objects/pdo/canopen_tpdo_priv.h>
#include  <canopen/source/objects/sdo/canopen_sdo_priv.h>
#include  <canopen/source/objects/sync/canopen_sync_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (CAN, CANOPEN)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_CANOPEN

#define  CANOPEN_INIT_CFG_DFLT      \
  {                                 \
    .MemSegPtr = DEF_NULL,          \
    .SvcTaskStkPtr = DEF_NULL,      \
    .SvcTaskStkSizeElements = 512u, \
    .EventQtyTot = 25u,             \
    .HwTmrPeriod = 1000u            \
  }

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const CANOPEN_INIT_CFG CANopen_InitCfgDflt = CANOPEN_INIT_CFG_DFLT;
CANOPEN_INIT_CFG       CANopen_InitCfg = CANOPEN_INIT_CFG_DFLT;
#endif

static CANOPEN_HANDLE CANopenHandle;

const CANOPEN_NODE_HANDLE CANopen_NodeHandleNull = { DEF_NULL };

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void CANopen_SvcTaskInit(RTOS_ERR *p_err);

static void CANopen_SvcTaskHandler(void *p_arg);

static void CANopen_NodeProcess(CANOPEN_NODE *p_node);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
/****************************************************************************************************//**
 *                                       CANopen_ConfigureMemSeg()
 *
 * @brief    Configures the memory segment where CANopen module data structures will be allocated.
 *
 * @param    p_seg       Pointer to memory segment to use when allocating control data.
 *                       DEF_NULL means general purpose heap segment.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called after the CANopen_Init() function.
 *******************************************************************************************************/
void CANopen_ConfigureMemSeg(MEM_SEG *p_seg)
{
  RTOS_ASSERT_CRITICAL((CANopenHandle.IsInit != DEF_TRUE), RTOS_ERR_ALREADY_INIT,; );

  CANopen_InitCfg.MemSegPtr = p_seg;
}
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
/****************************************************************************************************//**
 *                                     CANopen_ConfigureEventQty()
 *
 * @brief    Configures the maximum number of events for all the CANopen busses.
 *
 * @param    event_qty   Quantity of events.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the CANopen_Init() function.
 *******************************************************************************************************/
void CANopen_ConfigureEventQty(CPU_SIZE_T event_qty)
{
  RTOS_ASSERT_CRITICAL((CANopenHandle.IsInit != DEF_TRUE), RTOS_ERR_ALREADY_INIT,; );

  CANopen_InitCfg.EventQtyTot = event_qty;
}
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
/****************************************************************************************************//**
 *                                     CANopen_ConfigureSvcTaskStk()
 *
 * @brief    Configures the CANopen service task's stack.
 *
 * @param    stk_size_elements   Size, in stack elements, of the task's stack.
 *
 * @param    p_stk               Pointer to base of the task's stack. If DEF_NULL, stack will be allocated
 *                               from KAL's memory segment.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the CANopen module is initialized via the
 *               CANopen_Init() function.
 *
 * @note     (3) In order to change the priority of the CANopen core task, use the function
 *               CANopen_SvcTaskPrioSet().
 *******************************************************************************************************/
void CANopen_ConfigureSvcTaskStk(CPU_INT32U stk_size_elements,
                                 void       *p_stk)
{
  RTOS_ASSERT_CRITICAL((CANopenHandle.IsInit != DEF_TRUE), RTOS_ERR_ALREADY_INIT,; );

  CANopen_InitCfg.SvcTaskStkPtr = p_stk;
  CANopen_InitCfg.SvcTaskStkSizeElements = stk_size_elements;
}
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
/****************************************************************************************************//**
 *                                    CANopen_ConfigureTmrPeriod()
 *
 * @brief    Configures the hardware timer time base.
 *
 * @param    tmr_period    Timer period in microsecond.
 *
 * @note     (1) Calling this function is optional, if it is not called, the default value will be used.
 *
 * @note     (2) This function MUST be called before the CANopen_Init() function.
 *******************************************************************************************************/
void CANopen_ConfigureTmrPeriod(CPU_INT32U tmr_period)
{
  RTOS_ASSERT_CRITICAL((CANopenHandle.IsInit != DEF_TRUE), RTOS_ERR_ALREADY_INIT,; );

  CANopen_InitCfg.HwTmrPeriod = tmr_period;
}
#endif

/****************************************************************************************************//**
 *                                             CANopen_Init()
 *
 * @brief    Initializes CANopen resources and service task.
 *
 * @param    p_err    Pointer to the variable that will receive one of the following error code(s) from
 *                    this function:
 *                        - RTOS_ERR_NONE
 *                        - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
void CANopen_Init(RTOS_ERR *p_err)
{
  CPU_SIZE_T    i;
  CANOPEN_EVENT *p_event;

  p_event = (CANOPEN_EVENT *)Mem_SegAlloc("CANopen event list",
                                          CANopen_InitCfg.MemSegPtr,
                                          sizeof(CANOPEN_EVENT) * CANopen_InitCfg.EventQtyTot,
                                          p_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  SList_Init(&CANopenHandle.FreeEventListPtr);

  for (i = 0u; i < CANopen_InitCfg.EventQtyTot; i++) {
    SList_Push(&CANopenHandle.FreeEventListPtr,
               &p_event[i].EventList);
  }

  SList_Init(&CANopenHandle.NodeListPtr);

  CANopen_SvcTaskInit(p_err);
  RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  CANopenHandle.IsInit = DEF_TRUE;
}

/****************************************************************************************************//**
 *                                           CANopen_NodeAdd()
 *
 * @brief    Adds a node to the stack and configures it with the given specifications.
 *
 * @param    p_name           Pointer to a CAN Bus controller name.
 *
 * @param    p_spec           Pointer to the node's specifications.
 *
 * @param    p_event_fncts    Pointer to event functions callback structure.
 *
 * @param    p_err            Pointer to the variable that will receive one of the following error code(s) from
 *                            this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NULL_PTR
 *                               - RTOS_ERR_SEG_OVF
 *
 * @return   Handle to the added node, if successful. @n
 *           DEF_NULL,                 if there was a problem adding the node.
 *
 * @note     (1) The node is still in INIT state after this function call. To finalize the initialization
 *               phase (e.g. profile specific or application actions, etc.), see CANopen_NodeStart().
 *******************************************************************************************************/
CANOPEN_NODE_HANDLE CANopen_NodeAdd(const CPU_CHAR            *p_name,
                                    const CANOPEN_NODE_SPEC   *p_spec,
                                    const CANOPEN_EVENT_FNCTS *p_event_fncts,
                                    RTOS_ERR                  *p_err)
{
  CANOPEN_NODE        *p_node;
  CANOPEN_NODE_HANDLE handle;
  KAL_LOCK_EXT_CFG    lock_cfg;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, handle);

  RTOS_ASSERT_DBG_ERR_SET((p_spec != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, handle);

  RTOS_ASSERT_DBG_ERR_SET((p_name != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, handle);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_node = (CANOPEN_NODE *)Mem_SegAlloc("CANopen node",
                                        CANopen_InitCfg.MemSegPtr,
                                        sizeof(CANOPEN_NODE),
                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (CANopen_NodeHandleNull);
  }

  CANOPEN_NODE_HANDLE_SET(handle, p_node);

  Mem_Clr((void *)p_node,                                       // Initialize node structure.
          sizeof(CANOPEN_NODE));
  //                                                               ------------------ NODE VARIABLES ------------------
#if ((CANOPEN_SDO_SEG_EN == DEF_ENABLED) || (CANOPEN_SDO_BLK_EN == DEF_ENABLED))
  p_node->SdoBufPtr = p_spec->SdoBufPtr;                        // link specified SDO buffer memory
#endif
  p_node->Baudrate = p_spec->Baudrate;                          // set default CAN baudrate
  p_node->NodeId = p_spec->NodeId;                              // set default Node-Id
  p_node->Nmt.TmrId = CANOPEN_TMR_INVALID_ID;                   // invalidate timer entry
  p_node->NamePtr = (CPU_CHAR *)p_name;
  p_node->LockTimeoutMs = KAL_TIMEOUT_INFINITE;
  p_node->TmrPeriod = CANopen_InitCfg.HwTmrPeriod;
  p_node->EventFnctsPtr = p_event_fncts;

  lock_cfg.Opt = KAL_OPT_CREATE_REENTRANT;

  p_node->ObjLock = KAL_LockCreate("CANopen: node obj lock",
                                   &lock_cfg,
                                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (CANopen_NodeHandleNull);
  }

#if (CANOPEN_DBG_CTR_ERR_EN == DEF_ENABLED)
  //                                                               Alloc err cnt struct from dedicated mem seg.
  p_node->CntErrPtr = (CANOPEN_CTR_ERRS *)Mem_SegAlloc("CANopen - Dbg Error Counters",
                                                       CANopen_InitCfg.MemSegPtr,
                                                       sizeof(CANOPEN_CTR_ERRS),
                                                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (CANopen_NodeHandleNull);
  }
#endif

  //                                                               ------------------- TIMER MODULE -------------------
  CANopen_TmrInit(&p_node->Tmr,                                 // Initialize Highspeed-Timer
                  p_node,
                  p_spec->TmrMemPtr,
                  p_spec->TmrQty);

  //                                                               -------------------- CAN LAYER ---------------------
  CANopen_IfInit(&p_node->If, p_node, p_err);                   // Initialize CAN bus interface
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (CANopen_NodeHandleNull);
  }

  //                                                               ------------------- NODE OBJECTS -------------------
  CANopen_DictInit(&p_node->Dict,
                   p_node,                                      // initialize object directory
                   p_spec->DictPtr,
                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (CANopen_NodeHandleNull);
  }

#if (CANOPEN_OBJ_PARAM_EN == DEF_ENABLED)
  CANopen_NodeParamLoad(handle, CANOPEN_RESET_COMM, p_err);     // get all parameters within 1000h..1FFFh
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (CANopen_NodeHandleNull);
  }

  CANopen_NodeParamLoad(handle, CANOPEN_RESET_NODE, p_err);     // get all parameters within 2000h..9FFFh
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (CANopen_NodeHandleNull);
  }
#endif
  CANopen_SdoInit(p_node->Sdo, p_node, p_err);                  // Initialize SDO server
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (CANopen_NodeHandleNull);
  }
  //                                                               -------------- OPTIONAL NODE OBJECTS ---------------

#if (CANOPEN_TPDO_MAX_QTY > 0)
  CANopen_TPdoClr(p_node->TpdoTbl, p_node);                              // Clear TPDO tables
#endif
#if (CANOPEN_RPDO_MAX_QTY > 0)
  CANopen_RPdoClr(p_node->RpdoTbl, p_node);                              // Clear RPDO tables
#endif
#if (CANOPEN_EMCY_MAX_ERR_QTY > 0)
  if (p_spec->EmcyCodePtr != DEF_NULL) {
    CANopen_EmcyInit(&p_node->Emcy, p_node, p_spec->EmcyCodePtr, p_err); // Initialize EMCY tables
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (CANopen_NodeHandleNull);
    }
  }
#endif
#if (CANOPEN_SYNC_EN == DEF_ENABLED)
  CANopen_SyncInit(&p_node->Sync, p_node, p_err);               // Initialize SYNC tables
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (CANopen_NodeHandleNull);
  }
#endif

  SList_Push(&CANopenHandle.NodeListPtr,                        ///<Add node to the node list.
             &p_node->NodeList);

  return (handle);
}

/****************************************************************************************************//**
 *                                         CANopen_NodeStart()
 *
 * @brief    Starts a node by starting the CAN controller operations.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_err    Pointer to the variable that will receive one of the following error code(s) from
 *                    this function:
 *                        - RTOS_ERR_NONE
 *                        - RTOS_ERR_NULL_PTR
 *******************************************************************************************************/
void CANopen_NodeStart(CANOPEN_NODE_HANDLE node_handle,
                       RTOS_ERR            *p_err)
{
  CANOPEN_NODE       *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CANOPEN_NODE_STATE state;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(CANOPEN_NODE_HANDLE_IS_VALID(node_handle), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CANopen_IfTmrCbReg(p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CANopen_IfStart(p_node,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CANopen_NmtInit(&p_node->Nmt, p_node, p_err);                 // Initialize NMT slave
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;                                                     // abort further initializations
  }

  state = CANopen_NmtStateGet(node_handle);                     // get current state of CANopen node
  if (state == CANOPEN_INIT) {                                  // see, if state indicates INIT state
    CANopen_NmtBootup(&p_node->Nmt, p_err);                     // leave init state with final boot-up
  }
}

/****************************************************************************************************//**
 *                                         CANopen_NodeStop()
 *
 * @brief    Stops a node by stopping the CAN controller operations.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    p_err    Pointer to the variable that will receive one of the following error code(s) from
 *                    this function:
 *                        - RTOS_ERR_NONE
 *                        - RTOS_ERR_NULL_PTR
 *******************************************************************************************************/
void CANopen_NodeStop(CANOPEN_NODE_HANDLE node_handle,
                      RTOS_ERR            *p_err)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(CANOPEN_NODE_HANDLE_IS_VALID(node_handle), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  CANopen_TmrClr(&p_node->Tmr);                                 // clear all CANopen highspeed timers
  CANopen_NmtStateSet(node_handle, CANOPEN_INVALID, p_err);     // set CANopen node state to INVALID
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  CANopen_IfClose(&p_node->If, p_err);                          // close CAN bus connection
}

#if (CANOPEN_OBJ_PARAM_EN == DEF_ENABLED)
/****************************************************************************************************//**
 *                                       CANopen_NodeParamLoad()
 *
 * @brief    Loads all parameter groups with the given type.
 *
 * @param    node_handle     Handle to CANopen node object.
 *
 * @param    reset_type      Reset type:
 *                               - CANOPEN_RESET_COMM
 *                               - CANOPEN_RESET_NODE
 *
 * @param    p_err    Pointer to the variable that will receive one of the following error code(s) from
 *                    this function:
 *                        - RTOS_ERR_NONE
 *                        - RTOS_ERR_NULL_PTR
 *                        - RTOS_ERR_SIZE_INVALID
 *                        - RTOS_ERR_NOT_FOUND
 *                        - RTOS_ERR_FAIL
 *
 * @note     (1) The single parameter group(s) will be loaded from non-volatile memory by calling the user
 *               application callback function set through CANOPEN_EVENT_FNCTS / ParaOnLoad.
 *
 * @note     (2) This function considers all parameter groups, which are linked to the parameter
 *               store index (1010h) within the object directory. Every not linked parameter group
 *               is not in the scope of this function and must be handled within the application.
 *******************************************************************************************************/
void CANopen_NodeParamLoad(CANOPEN_NODE_HANDLE node_handle,
                           CANOPEN_NMT_RESET   reset_type,
                           RTOS_ERR            *p_err)
{
  CANOPEN_NODE  *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CANOPEN_DICT  *p_cod;                                         // Local: ptr to object dictionary
  CANOPEN_OBJ   *p_obj;                                         // Local: ptr to object entry
  CANOPEN_PARAM *p_pg;                                          // Local: ptr to parameter group info
  CPU_BOOLEAN   ok;
  CPU_INT08U    num = 0u;                                       // Local: highest subindex in index 1010h
  CPU_INT08U    sub;                                            // Local: current working subindex in loop

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET(CANOPEN_NODE_HANDLE_IS_VALID(node_handle), *p_err, RTOS_ERR_NULL_PTR,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  p_cod = &p_node->Dict;                                        // get ptr to object directory
  CANopen_DictByteRd(node_handle,                               // get highest subindex entry
                     CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_STORE_PARAM, 0),
                     &num,
                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);                        // abort with no error: entry is optional
    return;
  }

  for (sub = 1u; sub <= num; sub++) {                           // loop through storage subindex entries
    p_obj = CANopen_DictFind(p_cod,                             // get object entry
                             CANOPEN_DEV(CANOPEN_COMM_PROFILE_OBJ_IX_STORE_PARAM, sub),
                             p_err);
    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {           // see, if object entry is available
      p_pg = (CANOPEN_PARAM *)p_obj->Data;                      // get pointer to parameter group info
                                                                // see, if parameter reset type is matching
      if ((p_pg->ResetType == reset_type)
          && (p_node->EventFnctsPtr != DEF_NULL)
          && (p_node->EventFnctsPtr->ParamOnLoad != DEF_NULL)) {
        //                                                         yes: call application callback function
        ok = p_node->EventFnctsPtr->ParamOnLoad(node_handle, p_pg);
        if (ok != DEF_OK) {                                     // see, if user detects an error
          RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);                  // indicate an detected error in function
          break;
        }
      }
    }
  }
}
#endif

/****************************************************************************************************//**
 *                                    CANopen_NodeLockTimeoutSet()
 *
 * @brief    Sets the node object directory lock timeout.
 *
 * @param    node_handle  Handle to CANopen node object.
 *
 * @param    timeout      Timeout value for the object dictionary lock.
 *
 * @param    p_err        Pointer to the variable that will receive one of the following error code(s)
 *                         from this function:
 *                            - RTOS_ERR_NONE
 *                            - RTOS_ERR_NULL_PTR
 *******************************************************************************************************/
void CANopen_NodeLockTimeoutSet(CANOPEN_NODE_HANDLE node_handle,
                                CPU_INT32U          timeout,
                                RTOS_ERR            *p_err)
{
  CANOPEN_NODE *p_node = (CANOPEN_NODE *)node_handle.NodePtr;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_SET((p_node != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  CORE_ENTER_ATOMIC();
  p_node->LockTimeoutMs = timeout;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}

/****************************************************************************************************//**
 *                                      CANopen_SvcTaskPrioSet()
 *
 * @brief    Assigns a new priority to the CANopen service task.
 *
 * @param    prio    New priority of the service task.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from this
 *                   function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_ARG
 *
 * @note     (1) This function MUST be called before the CANopen_Init() function.
 *******************************************************************************************************/
void CANopen_SvcTaskPrioSet(CPU_INT08U prio,
                            RTOS_ERR   *p_err)
{
  KAL_TASK_HANDLE task_handle;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  CORE_ENTER_ATOMIC();
  RTOS_ASSERT_CRITICAL((CANopenHandle.IsInit == DEF_TRUE), RTOS_ERR_ALREADY_INIT,; );

  task_handle = CANopenHandle.SvcTaskHandle;
  CORE_EXIT_ATOMIC();

  KAL_TaskPrioSet(task_handle,
                  prio,
                  p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                         INTERNAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        CANopen_SvcTaskWakeUp()
 *
 * @brief    WakeUp service task.
 *
 * @param    p_event    Pointer to a CANopen event to be handled by the service task.
 *******************************************************************************************************/
void CANopen_SvcTaskWakeUp(CANOPEN_EVENT *p_event)
{
  RTOS_ERR      local_err;
  CANOPEN_EVENT *p_event_temp = DEF_NULL;

  p_event_temp = (CANOPEN_EVENT *)SList_Pop(&CANopenHandle.FreeEventListPtr);

  *p_event_temp = *p_event;
  p_event_temp->NextEventPtr = DEF_NULL;

  if (CANopenHandle.EventTailPtr != DEF_NULL) {
    CANopenHandle.EventTailPtr->NextEventPtr = p_event_temp;
  }

  CANopenHandle.EventTailPtr = p_event_temp;

  if (CANopenHandle.EventHeadPtr == DEF_NULL) {
    CANopenHandle.EventHeadPtr = CANopenHandle.EventTailPtr;
  }

  KAL_SemPost(CANopenHandle.EventSemHandle,
              KAL_OPT_POST_NONE,
              &local_err);
  RTOS_ASSERT_DBG((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), local_err.Code, RTOS_ERR_FAIL);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         CANopen_SvcTaskInit()
 *
 * @brief    Configures the resources used by the service task.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from this
 *                   function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *******************************************************************************************************/
static void CANopen_SvcTaskInit(RTOS_ERR *p_err)
{
  CPU_BOOLEAN kal_complete;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  kal_complete = KAL_FeatureQuery(KAL_FEATURE_TASK_CREATE, KAL_OPT_NONE);
  kal_complete = kal_complete && KAL_FeatureQuery(KAL_FEATURE_SEM_CREATE, KAL_OPT_NONE);
  kal_complete = kal_complete && KAL_FeatureQuery(KAL_FEATURE_SEM_PEND, KAL_OPT_NONE);
  kal_complete = kal_complete && KAL_FeatureQuery(KAL_FEATURE_SEM_POST, KAL_OPT_NONE);
  kal_complete = kal_complete && KAL_FeatureQuery(KAL_FEATURE_LOCK_CREATE, KAL_OPT_NONE);
  kal_complete = kal_complete && KAL_FeatureQuery(KAL_FEATURE_LOCK_ACQUIRE, KAL_OPT_NONE);
  kal_complete = kal_complete && KAL_FeatureQuery(KAL_FEATURE_LOCK_RELEASE, KAL_OPT_NONE);
  RTOS_ASSERT_CRITICAL(kal_complete, RTOS_ERR_NOT_SUPPORTED,; );

  CANopenHandle.EventSemHandle = KAL_SemCreate("CANopen Rx Sem",
                                               DEF_NULL,
                                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  KAL_SemSet(CANopenHandle.EventSemHandle,
             0u,
             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  CANopenHandle.SvcTaskHandle = KAL_TaskAlloc("CANopen - Core task",
                                              (CPU_STK *)CANopen_InitCfg.SvcTaskStkPtr,
                                              CANopen_InitCfg.SvcTaskStkSizeElements,
                                              DEF_NULL,
                                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }

  KAL_TaskCreate(CANopenHandle.SvcTaskHandle,
                 &CANopen_SvcTaskHandler,
                 DEF_NULL,
                 CANOPEN_CORE_TASK_PRIO_DFLT,
                 DEF_NULL,
                 p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_CONTEXT_REFRESH(*p_err);
    return;
  }
}

/****************************************************************************************************//**
 *                                        CANopen_SvcTaskHandler()
 *
 * @brief    Service task main loop. This task processes events coming from CANopen_SvcTaskWakeUp().
 *
 * @param    p_arg    Pointer to an argument for the service task.
 *******************************************************************************************************/
static void CANopen_SvcTaskHandler(void *p_arg)
{
  PP_UNUSED_PARAM(p_arg);

  while (DEF_ON) {
    RTOS_ERR      local_err;
    CANOPEN_EVENT *p_event = DEF_NULL;
    CANOPEN_NODE  *p_node;
    CORE_DECLARE_IRQ_STATE;

    KAL_SemPend(CANopenHandle.EventSemHandle,
                KAL_OPT_NONE,
                KAL_TIMEOUT_INFINITE,
                &local_err);
    RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

    CORE_ENTER_ATOMIC();
    p_event = CANopenHandle.EventHeadPtr;
    p_event->NodePtr->If.IntNbr = p_event->IntNbr;

    CANopenHandle.EventHeadPtr = (CANOPEN_EVENT *)p_event->NextEventPtr;

    if (CANopenHandle.EventHeadPtr == DEF_NULL) {
      CANopenHandle.EventTailPtr = DEF_NULL;
    }
    CORE_EXIT_ATOMIC();

    switch (p_event->EventType) {
      case CANOPEN_EVENT_RX:

        CANopen_NodeProcess(p_event->NodePtr);

        break;

      case CANOPEN_EVENT_TMR:

        CORE_ENTER_ATOMIC();
        SLIST_FOR_EACH_ENTRY(CANopenHandle.NodeListPtr, p_node, CANOPEN_NODE, NodeList) {
          CPU_BOOLEAN srv_result = DEF_NO;

          CORE_EXIT_ATOMIC();
          srv_result = CANopen_TmrServ(&p_node->Tmr,
                                       &local_err);
          RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

          if (srv_result == DEF_YES) {
            CANopen_TmrProcess(&p_node->Tmr);
          }
          CORE_ENTER_ATOMIC();
        }
        CORE_EXIT_ATOMIC();

        break;

#if (CANOPEN_EMCY_MAX_ERR_QTY > 0)
      case CANOPEN_EVENT_ERR_SET:

        CANopen_EmcySetInternal(&p_event->NodePtr->Emcy,
                                p_event->ErrCodeIx,
                                p_event->VendorErrCodePtr,
                                &local_err);
        RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

        break;

      case CANOPEN_EVENT_ERR_CLR:

        CANopen_EmcyClrInternal(&p_event->NodePtr->Emcy,
                                p_event->ErrCodeIx,
                                &local_err);
        RTOS_ASSERT_CRITICAL(RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE, RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

        break;
#endif
      default:
        break;
    }

    p_event->IntNbr = 0u;
    p_event->NodePtr = DEF_NULL;

    CORE_ENTER_ATOMIC();
    SList_Push(&CANopenHandle.FreeEventListPtr,
               &p_event->EventList);
    CORE_EXIT_ATOMIC();
  }
}

/****************************************************************************************************//**
 *                                        CANopenNodeProcess()
 *
 * @brief    Process CANopen data for a given node.
 *
 * @param    p_node     Pointer to the CANopen node object.
 *******************************************************************************************************/
static void CANopen_NodeProcess(CANOPEN_NODE *p_node)
{
  CANOPEN_IF_FRM          frm;                                  // Local: memory for one CAN frame
  CANOPEN_SDO             *p_srv;                               // Local: SDO server
  CPU_INT08U              allowed;                              // Local: allowed communication objects
  CANOPEN_RESPONSE_STATUS status;
  RTOS_ERR                local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  CANopen_IfRd(&p_node->If, &frm, &local_err);                  // check for a CAN frame on bus
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    return;                                                     // if error in CAN msg, no need to process it.
  }

  allowed = p_node->Nmt.CurStateObjAllowed;                     // set allowed objects with state encoding.

  if ((allowed & CANOPEN_SDO_ALLOWED) != 0) {                   // see, if SDO communication is allowed
    p_srv = CANopen_SdoCheck(p_node->Sdo, &frm);                // yes: see, if frame is a SDO request
    if (p_srv != DEF_NULL) {                                    // see, if a SDO server is addressed
      status = CANopen_SdoResp(p_srv, &local_err);              // yes: generate SDO response
      CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE, p_node->CntErrPtr->Sdo.RespPreparationFailCtr);

      if ((status == CANOPEN_RESPONSE_STATUS_NO_ERROR)
          || (status == CANOPEN_RESPONSE_STATUS_ABORTED)) {     // on success or abort message
        CANopen_IfWr(&p_node->If, &frm, &local_err);            // send SDO response at once
        CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE, p_node->CntErrPtr->Sdo.RespTxFailCtr);
      }
      allowed = 0;                                              // no further objects allowed (in 1 CAN id)
    }
  }

  if ((allowed & CANOPEN_NMT_ALLOWED) != 0) {                   // see, if NMT communication is allowed
    CPU_INT16S result;

    result = CANopen_NmtCheck(&p_node->Nmt, &frm, &local_err);
    CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE, p_node->CntErrPtr->Nmt.CmdCheckFailCtr);
    if (result >= 0) {                                          // yes: see, if frame is a NMT command
      allowed = 0;                                              // no further objects allowed (in 1 CAN id)
    } else {                                                    // see, if frame is a heartbeat
      result = CANopen_NmtHbConsCheck(&p_node->Nmt, &frm, &local_err);
      CANOPEN_CTR_ERR_INC_COND(RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE,
                               p_node->CntErrPtr->Nmt.HbConsCheckFailCtr);
      if (result >= 0) {
        allowed = 0;                                            // no further objects allowed (in 1 CAN id)
      }
    }
  }

#if (CANOPEN_RPDO_MAX_QTY > 0)
  if ((allowed & CANOPEN_PDO_ALLOWED) != 0) {                   // see, if PDO communication is allowed
    CPU_INT16S num;                                             // Local: number of RPDO

    num = CANopen_RPdoCheck(p_node->RpdoTbl, &frm);             // yes: see, if frame is a RPDO message
    if (num >= 0) {                                             // if frame is a PDO command
      CANopen_RPdoRd(p_node->RpdoTbl, num, &frm);               // distribute RPDO
      allowed = 0;                                              // no further objects allowed (in 1 CAN id)
    }
  }
#endif

#if (CANOPEN_SYNC_EN == DEF_ENABLED)
  if ((allowed & CANOPEN_SYNC_ALLOWED) != 0) {                  // see, if SYNC is allowed
#if (CANOPEN_TPDO_MAX_QTY > 0)
    CPU_BOOLEAN can_msg_is_sync;

    can_msg_is_sync = CANopen_SyncUpdate(&p_node->Sync, &frm);  // update sync management for TPDOs
    if (can_msg_is_sync == DEF_YES) {
#endif
    CANopen_SyncHandler(&p_node->Sync);                         // handling of all synchronized PDOs
    allowed = 0;                                                // no further objects allowed (in 1 CAN id)
#if (CANOPEN_TPDO_MAX_QTY > 0)
  }
#endif
  }
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_CANOPEN_AVAIL
