/***************************************************************************//**
 * @file
 * @brief Common - Shell Utility
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <common/include/lib_str.h>
#include  <common/include/shell.h>

#include  <cpu/include/cpu.h>

#include  <common/source/shell/shell_priv.h>
#include  <common/source/shell/shell_cmd_priv.h>

#include  <common/source/kal/kal_priv.h>

#include  <common/source/lib/lib_mem_cmd_priv.h>

#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <rtos_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                       (COMMON, SHELL)
#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_COMMON

#define  SHELL_INIT_CFG_DFLT                { \
    .CfgCmdUsage =                            \
    {                                         \
      .CmdTblItemNbrInit = 10u,               \
      .CmdTblItemNbrMax = 10u,                \
      .CmdArgNbrMax = 19u,                    \
      .CmdNameLenMax = 10u                    \
    },                                        \
    .MemSegPtr = DEF_NULL                     \
}

#define SHELL_ERR_STR_CMD_EMPTY             "Shell_Exec: shell command is empty."
#define SHELL_ERR_STR_CMD_EMPTY_LEN          sizeof(SHELL_ERR_STR_CMD_EMPTY)

#define SHELL_ERR_STR_CMD_NOT_FOUND         "Shell_Exec: command not found."
#define SHELL_ERR_STR_CMD_NOT_FOUND_LEN      sizeof(SHELL_ERR_STR_CMD_NOT_FOUND)

#define SHELL_ERR_STR_CMD_EXEC              "Shell_Exec: shell command failed to execute"
#define SHELL_ERR_STR_CMD_EXEC_LEN           sizeof(SHELL_ERR_STR_CMD_EXEC)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       SHELL MODULE COMMAND DATA TYPE
 *
 * Note(s) : 'Name' is a NULL terminated character string representing the name of the module command.
 *           See Shell_CmdTblAdd(), Note #2, for more details.
 *******************************************************************************************************/

typedef struct shell_module_cmd SHELL_MODULE_CMD;
struct shell_module_cmd {
  CPU_CHAR         *Name;                                       ///< Name (prefix) of module cmd (see Note #1).
  SHELL_CMD        *CmdTblPtr;                                  ///< Ptr to cmd tbl.
  SHELL_MODULE_CMD *NextModuleCmdPtr;                           ///< Ptr to next module cmd.
};

typedef struct shell_data {
  MEM_DYN_POOL     ModuleCmdPool;                               ///< Pool containing SHELL_MODULE_CMD elements.
  KAL_LOCK_HANDLE  LockHandle;                                  ///< Handle to lock for shell data fields.

  //                                                               These fields need to be protected by the lock.
  SHELL_MODULE_CMD *ModuleCmdListHeadPtr;                       ///< Ptr to head of module cmd list.
  CPU_CHAR         **ExecArgvTbl;                               ///< Tbl of ptrs to argvs, used when executing.

  //                                                               This field need to be protected by the Get/Free fncts
  //                                                               Ptr to start of shared buf.
  CPU_CHAR    *SharedBufPtr;
  //                                                               This field need to be protected by a crit section.
  //                                                               Flag indicating if shared buf is in use.
  CPU_BOOLEAN SharedBufIsUsed;
} SHELL_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static SHELL_DATA *Shell_DataPtr;

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
const SHELL_INIT_CFG  Shell_InitCfgDflt = SHELL_INIT_CFG_DFLT;
static SHELL_INIT_CFG Shell_InitCfg = SHELL_INIT_CFG_DFLT;
#else
extern const SHELL_INIT_CFG Shell_InitCfg;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static SHELL_CMD_FNCT Shell_CmdSearch(CPU_CHAR *cmd_name);

static CPU_BOOLEAN Shell_ModuleCmdNameGet(const CPU_CHAR *cmd_name,
                                          CPU_CHAR       module_cmd_name[],
                                          CPU_INT16U     len);

static CPU_BOOLEAN Shell_ModuleCmdAllocCallback(MEM_DYN_POOL *p_pool,
                                                MEM_SEG      *p_seg,
                                                void         *p_blk,
                                                void         *p_arg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           Shell_ConfigureCmdUsage()
 *
 * @brief    Configure the properties of the commands used by the Shell sub-module.
 *
 * @param    p_cfg   Pointer to the structure containing the command usage parameters.
 *
 * @note     (1) This function is optional. If it is called, it must be called before Shell_Init(). If
 *               it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void Shell_ConfigureCmdUsage(SHELL_CFG_CMD_USAGE *p_cfg)
{
  RTOS_ASSERT_DBG((Shell_DataPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  RTOS_ASSERT_DBG((p_cfg != DEF_NULL), RTOS_ERR_NULL_PTR,; );

  Shell_InitCfg.CfgCmdUsage = *p_cfg;
}
#endif

/****************************************************************************************************//**
 *                                           Shell_ConfigureMemSeg()
 *
 * @brief    Configure the memory segment that will be used to allocate internal data needed by Shell
 *           instead of the default memory segment.
 *
 * @param    p_mem_seg   Pointer to the memory segment from which the internal data will be allocated.
 *                       If DEF_NULL, the internal data will be allocated from the global Heap.
 *
 * @note     (1) This function is optional. If it is called, it must be called before Shell_Init(). If
 *               it is not called, default values will be used to initialize the module.
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void Shell_ConfigureMemSeg(MEM_SEG *p_mem_seg)
{
  RTOS_ASSERT_DBG((Shell_DataPtr == DEF_NULL), RTOS_ERR_ALREADY_INIT,; );

  Shell_InitCfg.MemSegPtr = p_mem_seg;
}
#endif

/****************************************************************************************************//**
 *                                               Shell_Init()
 *
 * @brief    Initializes the shell module.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_OWNERSHIP
 *                       - RTOS_ERR_ALREADY_EXISTS
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_OS_OBJ_DEL
 *                       - RTOS_ERR_WOULD_BLOCK
 *                       - RTOS_ERR_INVALID_ARG
 *                       - RTOS_ERR_NO_MORE_RSRC
 *                       - RTOS_ERR_IS_OWNER
 *                       - RTOS_ERR_ABORT
 *                       - RTOS_ERR_TIMEOUT
 *
 * @note     (1) Shell_Init() MUST be called before the other Shell functions are invoked, except for
 *               the 'Configure' functions.
 *
 * @note     (2) Shell_Init() MUST ONLY be called ONCE from the product's application.
 *
 * @note     (3) Module command pools MUST be initialized before initializing the pool with pointers
 *               to module commands.
 *
 * @note     (4) The functions Shell_Configure...() can be used to configure more specific properties
 *               of the Shell sub-module, when RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN is set to
 *               DEF_DISABLED. If set to DEF_ENABLED, the structure Shell_InitCfg needs to be declared
 *               and filled by the application to configure these specific properties for the module.
 *******************************************************************************************************/
void Shell_Init(RTOS_ERR *p_err)
{
  CPU_BOOLEAN is_en;

  is_en = KAL_FeatureQuery(KAL_FEATURE_LOCK_CREATE, KAL_OPT_CREATE_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_LOCK_ACQUIRE, KAL_OPT_PEND_NONE);
  is_en &= KAL_FeatureQuery(KAL_FEATURE_LOCK_RELEASE, KAL_OPT_POST_NONE);

  RTOS_ASSERT_DBG_ERR_SET((is_en == DEF_YES), *p_err, RTOS_ERR_NOT_AVAIL,; );

  Shell_DataPtr = (SHELL_DATA *)Mem_SegAlloc("Shell Data struct",
                                             Shell_InitCfg.MemSegPtr,
                                             sizeof(SHELL_DATA),
                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Shell_Init: failed to allocate data with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    return;
  }

  Shell_DataPtr->ExecArgvTbl = (CPU_CHAR **)Mem_SegAlloc("Shell_Exec argv tbl",
                                                         Shell_InitCfg.MemSegPtr,
                                                         sizeof(CPU_CHAR *) * Shell_InitCfg.CfgCmdUsage.CmdArgNbrMax,
                                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Shell_Init: failed to allocate data with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    return;
  }

  Shell_DataPtr->SharedBufPtr = (CPU_CHAR *)Mem_SegAlloc("Shell_Exec argv tbl",
                                                         Shell_InitCfg.MemSegPtr,
                                                         sizeof(CPU_CHAR) * (Shell_InitCfg.CfgCmdUsage.CmdNameLenMax + 4u),
                                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Shell_Init: failed to allocate data with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    return;
  }
  Shell_DataPtr->SharedBufIsUsed = DEF_NO;

  //                                                               ---------------- INIT SHELL CMD TBL ----------------
  Shell_DataPtr->ModuleCmdListHeadPtr = DEF_NULL;               // Init-clr module cmd pools    (see Note #3).

  Mem_DynPoolCreatePersistent("Shell module cmd pool",
                              &Shell_DataPtr->ModuleCmdPool,
                              Shell_InitCfg.MemSegPtr,
                              sizeof(SHELL_MODULE_CMD),
                              sizeof(CPU_ALIGN),
                              Shell_InitCfg.CfgCmdUsage.CmdTblItemNbrInit,
                              Shell_InitCfg.CfgCmdUsage.CmdTblItemNbrMax,
                              Shell_ModuleCmdAllocCallback,
                              DEF_NULL,
                              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Shell_Init: failed to allocate data with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    return;
  }

  Shell_DataPtr->LockHandle = KAL_LockCreate("Shell exec lock",
                                             DEF_NULL,
                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_ERR(("Shell_Init: failed to create lock with err: ", RTOS_ERR_LOG_ARG_GET(*p_err)));
    return;
  }

  LOG_VRB(("Shell_Init: completed successfully."));

  ShellCmd_Init(p_err);                                         // Add Shell commands table.

#if (LIB_MEM_CFG_DBG_INFO_EN == DEF_ENABLED)
  MemCmd_Init(p_err);                                           // Add Memory commands table.
#endif

  return;
}

/****************************************************************************************************//**
 *                                               Shell_Exec()
 *
 * @brief    Parses and executes a command passed in parameter. :
 *
 * @param    in_str          Pointer to a CPU_CHAR string holding a complete command and its
 *                           argument(s). This string MUST be editable, it cannot be 'const'.
 *
 * @param    out_fnct        Pointer to the 'output' function used by command (see Note #1).
 *
 * @param    p_cmd_param     Pointer to the command additional parameters.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_SHELL_CMD_EXEC
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_CMD_EMPTY
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_NO_MORE_RSRC
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @return   Command specific return value.
 *
 * @note     (1) The command may generate some output that should be transmitted to some devices
 *               (socket, RS-232 link, ...). The caller of this function is responsible for the
 *               implementation of such functions, if output is desired.
 *******************************************************************************************************/
CPU_INT16S Shell_Exec(CPU_CHAR        *in_str,
                      SHELL_OUT_FNCT  out_fnct,
                      SHELL_CMD_PARAM *p_cmd_param,
                      RTOS_ERR        *p_err)
{
  CPU_INT16U     argc;
  SHELL_CMD_FNCT cmd_fnct;
  CPU_INT16S     ret_val = 0;

  //                                                               ------------------- VALIDATE PTR -------------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0);

  RTOS_ASSERT_DBG_ERR_SET((in_str != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, 0);

  KAL_LockAcquire(Shell_DataPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  KAL_TIMEOUT_INFINITE,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (ret_val);
  }

  //                                                               ------------------ PARSE INPUT STR -----------------
  argc = Shell_Scanner(in_str,
                       Shell_DataPtr->ExecArgvTbl,
                       Shell_InitCfg.CfgCmdUsage.CmdArgNbrMax,
                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_rel;
  } else if (argc == 0) {
    RTOS_ERR_SET_AND_LOG_DBG(*p_err, RTOS_ERR_CMD_EMPTY, ("Shell_Exec: shell command is empty."));
    (void)out_fnct(SHELL_ERR_STR_CMD_EMPTY,
                   SHELL_ERR_STR_CMD_EMPTY_LEN,
                   p_cmd_param->OutputOptPtr);
    goto end_rel;
  }

  //                                                               -------------------- SEARCH CMD --------------------
  cmd_fnct = Shell_CmdSearch(Shell_DataPtr->ExecArgvTbl[0]);
  if (cmd_fnct == DEF_NULL) {
    RTOS_ERR_SET_AND_LOG_DBG(*p_err, RTOS_ERR_NOT_FOUND, ("Shell_Exec: command not found."));
    (void)out_fnct(SHELL_ERR_STR_CMD_NOT_FOUND,
                   SHELL_ERR_STR_CMD_NOT_FOUND_LEN,
                   p_cmd_param->OutputOptPtr);
    goto end_rel;
  }

  //                                                               -------------------- EXECUTE CMD -------------------
  ret_val = cmd_fnct(argc, Shell_DataPtr->ExecArgvTbl, out_fnct, p_cmd_param);
  if (ret_val == SHELL_EXEC_ERR) {
    RTOS_ERR_SET_AND_LOG_DBG(*p_err, RTOS_ERR_SHELL_CMD_EXEC, ("Shell_Exec: shell command failed to execute with return value", (d)ret_val));
    (void)out_fnct(SHELL_ERR_STR_CMD_EXEC,
                   SHELL_ERR_STR_CMD_EXEC_LEN,
                   p_cmd_param->OutputOptPtr);
  }

end_rel:
  {
    RTOS_ERR local_err;

    KAL_LockRelease(Shell_DataPtr->LockHandle, &local_err);
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL, ret_val);
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                               Shell_CmdTblAdd()
 *
 * @brief    (1) Allocates and initializes a module command as follows :
 *           - (a) Validate module command
 *           - (b) Get a free module command
 *           - (c) Initialize module command
 *           - (d) Add to module command used pool.
 *
 *       - (2) The module command pools are implemented as doubly-linked lists :
 *           - (a) 'Shell_ModuleCmdUsedPoolPtr' and 'Shell_ModuleCmdFreePoolPtr' points to the head
 *                   of the module command pool.
 *           - (b) Module command NextModuleCmdPtr's and PrevModuleCmdPtr's links each command to
 *                   form the module command pool doubly-linked list.
 *           - (c) Module command are inserted and removed at the head of the module command pool
 *                   lists.
 *                   @verbatim
 *                               Module commands are
 *                               inserted & removed
 *                                   at the head
 *                                   (see Note #2c)
 *
 *                                       |            NextModuleCmdPtr
 *                                       |             (see Note #2b)
 *                                       v                    |
 *                                                            |
 *                                    -------       -------   v   -------       -------
 *           Module command Pool ---->|     |------>|     |------>|     |------>|     |
 *                Pointer             |     |       |     |       |     |       |     |
 *                                    |     |<------|     |<------|     |<------|     |
 *             (see Note #2a)         -------       -------       -------   ^   -------
 *                                                                          |
 *                                                                          |
 *                                                                  PrevModuleCmdPtr
 *                                                                    (see Note #2b)
 *
 *                                    |                                               |
 *                                    |<-----Pool of Free/Used Module Commands ------>|
 *                                    |                (see Note #2)                  |
 *                   @endverbatim
 * @param    cmd_tbl_name    Pointer to character string representing the name of the command table.
 *
 * @param    cmd_tbl         _name    Pointer to character string representing the name of the command table.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_OWNERSHIP
 *                               - RTOS_ERR_ALREADY_EXISTS
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_INVALID_ARG
 *                               - RTOS_ERR_NO_MORE_RSRC
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *
 * @note     (4) The 'cmd_tbl_name' parameter is not mandatory in the current implementation.
 *               Although you could pass a 'NULL' value for this parameter, it is recommended to provide
 *               it to allow the removal of 'cmd_tbl' from the  Shell_CmdTblRem().
 *               @n
 *               However, passing NULL for this parameter will result in the first command prefix to
 *               be extracted and used as the command table name.
 *
 * @note     (5) If an empty character array is passed in the cmd_tbl_name parameter, the function
 *               will extract the first command prefix to use as the command table name.
 *******************************************************************************************************/
void Shell_CmdTblAdd(CPU_CHAR  *cmd_tbl_name,
                     SHELL_CMD cmd_tbl[],
                     RTOS_ERR  *p_err)
{
  SHELL_CMD        *p_cmd;
  CPU_CHAR         *p_tbl_name = DEF_NULL;
  SHELL_MODULE_CMD *p_module_cmd_list;
  SHELL_MODULE_CMD *p_module_cmd_alloc;
  CPU_SIZE_T       name_len;
  CPU_INT16S       name_compare;
  RTOS_ERR         local_err;

  //                                                               ------------------- VALIDATE PTR -------------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((cmd_tbl != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  //                                                               If cmd tbl empty rtn err.
  RTOS_ASSERT_DBG_ERR_SET((cmd_tbl[0].Fnct != DEF_NULL), *p_err, RTOS_ERR_CMD_EMPTY,; );

  if (cmd_tbl_name != DEF_NULL) {                               // If cmd_tbl_name not null ...
    name_len = Str_Len(cmd_tbl_name);
    //                                                             ... If name too long ...
    if (name_len >= Shell_InitCfg.CfgCmdUsage.CmdNameLenMax) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);               // ... rtn err.
      return;
    } else if (name_len > 0) {                                  // ... else if name greater of 0 ...
      p_tbl_name = cmd_tbl_name;                                // ... use as tbl name.
    }
  }

  //                                                               ------------------ GET MODULE CMD ------------------
  p_module_cmd_alloc = (SHELL_MODULE_CMD *)Mem_DynPoolBlkGet(&Shell_DataPtr->ModuleCmdPool,
                                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET_AND_LOG_ERR(*p_err, RTOS_ERR_NO_MORE_RSRC, ("Shell_CmdTblAdd: failed to alloc command module, limit reached."));
    return;
  }

  //                                                               If cmd tbl name not gotten from param get name from first cmd.
  if (p_tbl_name == DEF_NULL) {
    CPU_BOOLEAN name_get_ok;

    p_cmd = &cmd_tbl[0];
    name_get_ok = Shell_ModuleCmdNameGet(p_cmd->Name,
                                         p_module_cmd_alloc->Name,
                                         Shell_InitCfg.CfgCmdUsage.CmdNameLenMax);
    if (name_get_ok != DEF_OK) {
      RTOS_ERR_SET_AND_LOG_ERR(*p_err, RTOS_ERR_INVALID_ARG, ("Shell_CmdTblAdd: failed to obtain command table name from first command."));
      goto end_free;
    }
    p_tbl_name = p_module_cmd_alloc->Name;
  }

  KAL_LockAcquire(Shell_DataPtr->LockHandle,                    // Prevent multiple add at the same time.
                  KAL_OPT_PEND_NONE,
                  KAL_TIMEOUT_INFINITE,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto end_free;
  }

  //                                                               -------------- CHK FOR DUPLICATE ENTRY -------------
  p_module_cmd_list = Shell_DataPtr->ModuleCmdListHeadPtr;
  while (p_module_cmd_list != DEF_NULL) {
    name_compare = Str_Cmp(p_module_cmd_list->Name, p_tbl_name);

    if ((p_module_cmd_list->CmdTblPtr == cmd_tbl)               // If module name already used ...
        || (name_compare == 0)) {
      //                                                           ... rtn err.
      RTOS_ERR_SET_AND_LOG_ERR(*p_err, RTOS_ERR_ALREADY_EXISTS, ("Shell_CmdTblAdd: module name already used."));
      goto end_rel_free;
    }

    p_cmd = &cmd_tbl[0];
    while (p_cmd->Fnct != DEF_NULL) {
      SHELL_CMD_FNCT cmd_fnct;

      cmd_fnct = Shell_CmdSearch((CPU_CHAR *)p_cmd->Name);
      if (cmd_fnct != DEF_NULL) {
        RTOS_ERR_SET_AND_LOG_ERR(*p_err, RTOS_ERR_ALREADY_EXISTS, ("Shell_CmdTblAdd: command name already used."));
        goto end_rel_free;
      }

      p_cmd++;
    }
    p_module_cmd_list = p_module_cmd_list->NextModuleCmdPtr;
  }

  //                                                               ----------------- INIT MODULE CMD ------------------
  Str_Copy(p_module_cmd_alloc->Name, p_tbl_name);
  p_module_cmd_alloc->CmdTblPtr = cmd_tbl;

  //                                                               ---------- ADD TO MODULE CMD TBL USED POOL ---------
  p_module_cmd_alloc->NextModuleCmdPtr = Shell_DataPtr->ModuleCmdListHeadPtr;
  Shell_DataPtr->ModuleCmdListHeadPtr = p_module_cmd_alloc;

  KAL_LockRelease(Shell_DataPtr->LockHandle, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  LOG_VRB(("Shell_CmdTblAdd: call completed."));

  return;

end_rel_free:
  KAL_LockRelease(Shell_DataPtr->LockHandle, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

end_free:
  Mem_DynPoolBlkFree(&Shell_DataPtr->ModuleCmdPool,
                     (void *)p_module_cmd_alloc,
                     &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
}

/****************************************************************************************************//**
 *                                               Shell_CmdTblRem()
 *
 * @brief    Removes a module command.
 *
 * @param    cmd_tbl_name    Pointer to character string representing the name of the command table.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_POOL_FULL
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_OWNERSHIP
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_IS_OWNER
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void Shell_CmdTblRem(CPU_CHAR *cmd_tbl_name,
                     RTOS_ERR *p_err)
{
  SHELL_MODULE_CMD *p_module_cmd_prev = DEF_NULL;
  SHELL_MODULE_CMD *p_module_cmd;

  //                                                               ------------------- VALIDATE PTR -------------------
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((cmd_tbl_name != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  KAL_LockAcquire(Shell_DataPtr->LockHandle,
                  KAL_OPT_PEND_NONE,
                  KAL_TIMEOUT_INFINITE,
                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ----------------- SEARCH MODULE CMD ----------------
  p_module_cmd = Shell_DataPtr->ModuleCmdListHeadPtr;
  while (p_module_cmd != DEF_NULL) {
    CPU_INT16S name_compare;

    name_compare = Str_Cmp(cmd_tbl_name, p_module_cmd->Name);
    if (name_compare == 0) {
      break;
    }
    p_module_cmd_prev = p_module_cmd;
    p_module_cmd = p_module_cmd->NextModuleCmdPtr;
  }

  if (p_module_cmd == DEF_NULL) {
    RTOS_ERR_SET_AND_LOG_ERR(*p_err, RTOS_ERR_NOT_FOUND, ("Shell_CmdTblRem: failed to find command to remove."));
    goto end_rel;
  }

  if (p_module_cmd_prev != DEF_NULL) {
    p_module_cmd_prev->NextModuleCmdPtr = p_module_cmd->NextModuleCmdPtr;
  } else {
    Shell_DataPtr->ModuleCmdListHeadPtr = p_module_cmd->NextModuleCmdPtr;
  }

  Mem_DynPoolBlkFree(&Shell_DataPtr->ModuleCmdPool,
                     (void *)p_module_cmd,
                     p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

end_rel:
  KAL_LockRelease(Shell_DataPtr->LockHandle, p_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

  LOG_VRB(("Shell_CmdTblAdd: call completed."));

  return;
}

/****************************************************************************************************//**
 *                                               Shell_Scanner()
 *
 * @brief    Scans and parses the command line.
 *
 * @param    in_str          Pointer to a NULL terminated string holding a complete command and its
 *                           argument(s).
 *
 * @param    arg_tbl         Array of pointer that will receive pointers to token.
 *
 * @param    arg_tbl_size    Size of arg_tbl array.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error
 *                           code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_NO_MORE_RSRC
 *
 * @return   Number of token(s) (command name and argument(s)).
 *
 * @note     (1) The first token is always the command name itself.
 *
 * @note     (2) This function modify the 'in' arguments by replacing token's delimiter characters by
 *               termination character ('\0').
 *******************************************************************************************************/
CPU_INT16U Shell_Scanner(CPU_CHAR   *in_str,
                         CPU_CHAR   *arg_tbl[],
                         CPU_INT16U arg_tbl_size,
                         RTOS_ERR   *p_err)
{
  CPU_CHAR    *in_rd = in_str;
  CPU_INT16U  tok_ix = 0;
  CPU_BOOLEAN end_tok_found = DEF_YES;
  CPU_BOOLEAN quote_opened = DEF_NO;

  //                                                               ------------------ SCAN CMD LINE  ------------------
  while (*in_rd) {
    switch (*in_rd) {
      case ASCII_CHAR_QUOTATION_MARK:                           // Quote char found.
        if (quote_opened == DEF_YES) {
          quote_opened = DEF_NO;
          *in_rd = (CPU_CHAR)0;
          end_tok_found = DEF_YES;
        } else {
          quote_opened = DEF_YES;
        }
        break;

      case ASCII_CHAR_SPACE:                                    // Space char found.
        if ((end_tok_found == DEF_NO)                           // If first space between tok && quote NOT opened ...
            && (quote_opened == DEF_NO)) {
          *in_rd = SHELL_ASCII_ARG_END;                         // ... put termination char.
          end_tok_found = DEF_YES;
        }
        break;

      default:                                                  // Other char found ...
        if (end_tok_found == DEF_YES) {
          if (tok_ix < arg_tbl_size) {
            arg_tbl[tok_ix] = in_rd;                            // Set arg_tbl ptr to tok location.
            tok_ix++;
            end_tok_found = DEF_NO;
          } else {
            RTOS_ERR_SET_AND_LOG_ERR(*p_err, RTOS_ERR_NO_MORE_RSRC, ("Shell_Scanner: arguments scan failed, overflows argument table size."));
            return (0);
          }
        }
        break;
    }
    in_rd++;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (tok_ix);
}

/****************************************************************************************************//**
 *                                               Shell_OptParse()
 *
 * @brief    Parses optional command line arguments.
 *
 * @param    argc        Argument count.
 *
 * @param    argv        Pointer to argument table.
 *
 * @param    state       Argument parsing state structure.
 *
 * @param    opt_str     Argument string to parse.
 *
 * @return   SHELL_OPTPARSE_STATE.
 *******************************************************************************************************/
SHELL_OPTPARSE_STATE Shell_OptParse(CPU_INT16U           argc,
                                    CPU_CHAR             *argv[],
                                    SHELL_OPTPARSE_STATE state,
                                    CPU_CHAR             *opt_str)
{
  CPU_CHAR arg_char;

  state.OptVal = '?';
  state.OptArg = DEF_NULL;

  if (state.CurArgIx == 0u) {                                   // Init state if first call.
    if (argc < 2u) {
      state.OptVal = '\0';
      return (state);
    }
    state.CurArgIx = 1u;
    state.CurArgPosPtr = argv[1];
    state.CurArgIsOpt = DEF_NO;
  }

  while (DEF_YES) {
    arg_char = *(state.CurArgPosPtr++);

    if (arg_char == '\0') {                                     // End of arg.
      state.CurArgIx++;
      if (state.CurArgIx >= argc) {
        state.OptVal = '\0';
        break;
      }
      state.CurArgPosPtr = argv[state.CurArgIx];
      state.CurArgIsOpt = DEF_NO;
    } else if (arg_char == '-') {
      if (!state.CurArgIsOpt) {                                 // Begin opts.
        state.CurArgIsOpt = DEF_YES;
      } else {                                                  // Double dash: end opt parsing.
        state.OptVal = '\0';
        break;
      }
    } else if (!state.CurArgIsOpt) {                            // Char outside of opt: end opt parsing.
      state.OptVal = '\0';
      break;
    } else {                                                    // Got an opt char.
      CPU_CHAR *p_opt_str;

      if (opt_str == DEF_NULL) {
        state.OptVal = '\0';
        break;
      }

      for (p_opt_str = opt_str; *p_opt_str != '\0'; p_opt_str++) {
        if (*p_opt_str == arg_char) {
          break;
        }
      }

      if (*p_opt_str == '\0') {                                 // Opt val not in optstr.
        state.OptVal = '?';
        break;
      }

      state.OptVal = arg_char;
      if (*(p_opt_str + 1) == ':') {                            // Opt arg reqd.
        if (*(state.CurArgPosPtr) == '\0') {                    // Opt arg in next arg.
          if (state.CurArgIx + 1 < argc) {
            state.CurArgIx++;
            state.CurArgIsOpt = DEF_NO;
            state.CurArgPosPtr = argv[state.CurArgIx];
            state.OptArg = state.CurArgPosPtr;
          }
        } else {
          state.OptArg = state.CurArgPosPtr;
        }
        //                                                         Advance to end of opt arg.
        if (state.OptArg != DEF_NULL) {
          while ((*(state.CurArgPosPtr) != '\0') || (*(state.CurArgPosPtr) == '-')) {
            state.CurArgPosPtr++;
          }

          if (state.CurArgPosPtr == state.OptArg) {             // Null-len opt arg.
            state.OptArg = DEF_NULL;
          }
        }

        if (state.OptArg == DEF_NULL) {
          if (*(p_opt_str + 2) != ':') {                        // Opt arg is mandatory.
            state.OptVal = ':';
          }
        }
      }

      break;
    }
  }

  return (state);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PRIVATE FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           Shell_ListCmdOutput()
 *
 * @brief    Outputs a list of all commands.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @note     (1) This function must only be called with the Shell lock acquired.
 *******************************************************************************************************/
void Shell_ListCmdOutput(SHELL_OUT_FNCT  out_fnct,
                         SHELL_CMD_PARAM *p_cmd_param)
{
  SHELL_MODULE_CMD *p_module_cmd;
  SHELL_CMD        *p_cmd;

  p_module_cmd = Shell_DataPtr->ModuleCmdListHeadPtr;

  while (p_module_cmd != DEF_NULL) {
    p_cmd = p_module_cmd->CmdTblPtr;
    while (p_cmd->Fnct != DEF_NULL) {
      (void)out_fnct((CPU_CHAR *)p_cmd->Name,
                     (CPU_INT16U)Str_Len(p_cmd->Name),
                     p_cmd_param->OutputOptPtr);
      (void)out_fnct(STR_NEW_LINE, (CPU_INT16U)Str_Len(STR_NEW_LINE), p_cmd_param->OutputOptPtr);
      p_cmd++;
    }
    p_module_cmd = p_module_cmd->NextModuleCmdPtr;
  }
}

/****************************************************************************************************//**
 *                                           Shell_SharedBufGet()
 *
 * @brief    Obtain temporary access to shared buffer offered by shell. The maximum size is set to
 *           CmdNameLenMax in the shell cfg passed at initialization.
 *
 * @param    req_len     Requested length for the buffer. Must be smaller or equal to CmdNameLenMax.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s)
 *                       from this function:
 *                           - RTOS_ERR_NONE
 *                           - RTOS_ERR_OWNERSHIP
 *
 * @return   Pointer to shared buffer start, if NO error(s),
 *           DEF_NULL, otherwise.
 *******************************************************************************************************/
CPU_CHAR *Shell_SharedBufGet(CPU_INT16U req_len,
                             RTOS_ERR   *p_err)
{
  CPU_CHAR *ret_val = DEF_NULL;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, ret_val);

  RTOS_ASSERT_DBG_ERR_SET((req_len <= Shell_InitCfg.CfgCmdUsage.CmdNameLenMax), *p_err, RTOS_ERR_WOULD_OVF, ret_val);

  CORE_ENTER_ATOMIC();
  if (Shell_DataPtr->SharedBufIsUsed == DEF_NO) {
    Shell_DataPtr->SharedBufIsUsed = DEF_YES;
    ret_val = Shell_DataPtr->SharedBufPtr;
  }
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, (ret_val != DEF_NULL) ? RTOS_ERR_NONE : RTOS_ERR_OWNERSHIP);

  return (ret_val);
}

/****************************************************************************************************//**
 *                                           Shell_SharedBufFree()
 *
 * @brief    Release ownership on temporary buffer.
 *******************************************************************************************************/
void Shell_SharedBufFree(void)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  Shell_DataPtr->SharedBufIsUsed = DEF_NO;
  CORE_EXIT_ATOMIC();
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               Shell_CmdSearch()
 *
 * @brief    Search for specified command in command tables.
 *
 * @param    cmd_name    Pointer to command name.
 *
 * @return   Pointer to command function if found,
 *           DEF_NULL, otherwise.
 *
 * @note     (1) Lock must be acquired when calling this function.
 *******************************************************************************************************/
static SHELL_CMD_FNCT Shell_CmdSearch(CPU_CHAR *cmd_name)
{
  SHELL_MODULE_CMD *p_module_cmd_list = Shell_DataPtr->ModuleCmdListHeadPtr;
  SHELL_CMD        *p_cmd = DEF_NULL;
  SHELL_CMD        *p_cmd_list;
  CPU_INT16S       name_compare;
  SHELL_CMD_FNCT   fnct = DEF_NULL;

  //                                                               ----------------- SEARCH MODULE CMD ----------------
  while (p_module_cmd_list != DEF_NULL) {
    p_cmd_list = p_module_cmd_list->CmdTblPtr;
    while (p_cmd_list->Fnct != DEF_NULL) {
      name_compare = Str_Cmp(cmd_name, p_cmd_list->Name);
      if (name_compare == 0) {
        p_cmd = p_cmd_list;
        break;
      }
      p_cmd_list++;
    }
    if (p_cmd == DEF_NULL) {
      p_module_cmd_list = p_module_cmd_list->NextModuleCmdPtr;
    } else {
      break;
    }
  }

  if (p_cmd != DEF_NULL) {
    fnct = (SHELL_CMD_FNCT)p_cmd->Fnct;
  }

  return (fnct);
}

/****************************************************************************************************//**
 *                                           Shell_ModuleCmdNameGet()
 *
 * @brief    (1) Get the command module name (prefix) from a command string :
 *               - (a) Search for module command name delimiter
 *               - (b) Copy       module command name
 *
 * @param    cmd_str             Pointer to command string holding the command module name.
 *
 * @param    module_cmd_name     Pointer to a preallocated variable that will receive the module
 *                               command name.
 *
 * @param    len                 Length of the array pointed by 'module_cmd_name'.
 *
 * @return   DEF_OK    if name has been correctly obtained.
 *           DEF_FAIL, otherwise.
 *
 * @note     (1) The command module name consists in the first part of a command part, that is the
 *               part preceding the underscore ('_') character. If there is no underscored character,
 *               the command name is interpreted as the commande module name.
 *******************************************************************************************************/
static CPU_BOOLEAN Shell_ModuleCmdNameGet(const CPU_CHAR *cmd_str,
                                          CPU_CHAR       module_cmd_name[],
                                          CPU_INT16U     len)
{
  CPU_CHAR    *p_cmd;
  CPU_BOOLEAN found;
  CPU_INT16U  name_len;
  CPU_CHAR    *copy_ret_val;

  RTOS_ASSERT_DBG((module_cmd_name != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG((cmd_str != DEF_NULL), RTOS_ERR_NULL_PTR, DEF_FAIL);

  p_cmd = (CPU_CHAR *)cmd_str;
  found = DEF_NO;

  //                                                               --------- SEARCH MODULE CMD NAME DELIMITER ---------
  while ((p_cmd != DEF_NULL)
         && (found == DEF_NO)) {
    if (*p_cmd == SHELL_ASCII_CDM_NAME_DELIMITER) {
      found = DEF_YES;
      break;
    } else if (*p_cmd == SHELL_ASCII_ARG_END) {
      found = DEF_YES;
      break;
    }
    p_cmd++;
  }

  if (found == DEF_NO) {
    return (DEF_FAIL);
  }

  //                                                               --------------- COPY MODULE CMD NAME ---------------
  name_len = (p_cmd - cmd_str);
  if (name_len >= len) {                                        // If module cmd name too long ...
    return (DEF_FAIL);                                          // ... rtn with error.
  }

  copy_ret_val = Str_Copy_N(module_cmd_name, cmd_str, name_len);
  RTOS_ASSERT_DBG((copy_ret_val != DEF_NULL), RTOS_ERR_ASSERT_DBG_FAIL, DEF_FAIL);

  module_cmd_name[name_len] = '\0';

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       Shell_ModuleCmdAllocCallback()
 *
 * @brief    Callback function executed when a SHELL_MODULE_CMD block is obtained from pool for the
 *           first time.
 *
 * @param    p_pool  Pointer to pool from which block was allocated.
 *
 * @param    p_seg   Pointer to segment with which pool was created.
 *
 * @param    p_blk   Pointer to allocated block.
 *
 * @param    p_arg   Pointer to arg, nothing in this case.
 *
 * @return   DEF_OK,   if NO error(s),
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN Shell_ModuleCmdAllocCallback(MEM_DYN_POOL *p_pool,
                                                MEM_SEG      *p_seg,
                                                void         *p_blk,
                                                void         *p_arg)
{
  SHELL_MODULE_CMD *p_module_cmd = (SHELL_MODULE_CMD *)p_blk;
  RTOS_ERR         err;

  PP_UNUSED_PARAM(p_pool);
  PP_UNUSED_PARAM(p_arg);

  p_module_cmd->Name = (CPU_CHAR *)Mem_SegAlloc(DEF_NULL,
                                                p_seg,
                                                sizeof(CPU_CHAR) * Shell_InitCfg.CfgCmdUsage.CmdNameLenMax,
                                                &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    LOG_ERR(("Shell_ModuleCmdAllocCallback: failed to allocate data with err: ", RTOS_ERR_LOG_ARG_GET(err)));
    return (DEF_FAIL);
  }

  Str_Copy(p_module_cmd->Name, "");

  p_module_cmd->NextModuleCmdPtr = DEF_NULL;
  p_module_cmd->CmdTblPtr = DEF_NULL;

  return (DEF_OK);
}
