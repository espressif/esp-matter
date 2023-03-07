/***************************************************************************//**
 * @file
 * @brief USB Host Shell Operations
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

#if (defined(RTOS_MODULE_USB_HOST_AVAIL) && defined(RTOS_MODULE_COMMON_SHELL_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>
#include  <common/include/shell.h>
#include  <common/include/rtos_err.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/kal/kal_priv.h>

#include  <usb/include/host/usbh_core_dev.h>
#include  <usb/include/host/usbh_core_config.h>

#include  <usb/source/host/cmd/usbh_cmd_priv.h>
#include  <usb/source/host/core/usbh_core_types_priv.h>
#include  <usb/source/host/core/usbh_core_priv.h>
#include  <usb/source/host/core/usbh_core_hub_priv.h>
#include  <usb/source/host/core/usbh_core_dev_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  USBH_SHELL_CMD_BIT_NONE            DEF_BIT_NONE
#define  USBH_SHELL_CMD_BIT_INVALID         USBH_SHELL_CMD_BIT_NONE
#define  USBH_SHELL_CMD_BIT_UNKNOWN         DEF_BIT_00
#define  USBH_SHELL_CMD_BIT_NO_DEV          DEF_BIT_01

#define  USBH_SHELL_CMD_SUPPORT_HOST_QTY    2u

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
#define  USBH_SHELL_CMD_DEV_MAX             128u
#else
#define  USBH_SHELL_CMD_DEV_MAX             32u
#endif

/********************************************************************************************************
 *                                               LSUSB DEFINES
 *******************************************************************************************************/

#define  USBH_SHELL_CMD_LSUSB              "lsusb"

#define  USBH_SHELL_CMD_ENTER               ASCII_CHAR_HYPHEN_MINUS
#define  USBH_SHELL_CMD_QUESTION_MARK       ASCII_CHAR_QUESTION_MARK

#define  USBH_SHELL_CMD_DEV                 ASCII_CHAR_LATIN_LOWER_D
#define  USBH_SHELL_CMD_HOST                ASCII_CHAR_LATIN_LOWER_H
#if (USBH_CFG_STR_EN == DEF_ENABLED)
#define  USBH_SHELL_CMD_STR                 ASCII_CHAR_LATIN_LOWER_S
#endif
#define  USBH_SHELL_CMD_BASIC               ASCII_CHAR_LATIN_LOWER_B
#define  USBH_SHELL_CMD_HELP               "-help"

#define  USBH_SHELL_CMD_STRING_HELP        "Description:\r\n"                                             \
                                           "\tTree display with basic info of all connected devices.\r\n" \
                                           "Options:\r\n"                                                 \
                                           "\t-s\r\n"                                                     \
                                           "\t\tAdd Strings when available.\r\n"                          \
                                           "\t\ti.e. lsusb -s\r\n"                                        \
                                           "\t-d [-s] #a [#b]\r\n"                                        \
                                           "\t\tDisplay all info on device addr #a on host #b.\r\n"       \
                                           "\t\tHost 0 is called by default if no #b is not present.\r\n" \
                                           "\t\ti.e. lsusb -d 1, lsusb -d -s 4 1\r\n"                     \
                                           "\t-h [-b]\r\n"                                                \
                                           "\t\tDisplay all info on host. Requires an option\r\n"         \
                                           "\t\ti.e. lsusb -h -b, lsusb -h -d, lsusb -h -s\r\n"           \
                                           "\t?, --help\r\n"                                              \
                                           "\t\tDisplay this menu.\r\n\r\n"

#define  USBH_SHELL_CMD_HOST_STATUS       (USBH_CMD_STATUS_OPT_HOST \
                                           | USBH_CMD_STATUS_OPT_HC \
                                           | USBH_CMD_STATUS_OPT_CLASS_DRV)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef enum USBH_ShellCmdOptEnum {
  USBH_SHELL_CMD_OPT_ENTER = ASCII_CHAR_HYPHEN_MINUS,
  USBH_SHELL_CMD_OPT_DEV_ONLY = ASCII_CHAR_LATIN_LOWER_D,
#if (USBH_CFG_STR_EN == DEF_ENABLED)
  USBH_SHELL_CMD_OPT_DEV_STR = ASCII_CHAR_LATIN_LOWER_T,
#endif
  USBH_SHELL_CMD_OPT_WAIT_HOST = ASCII_CHAR_LATIN_LOWER_Z,
  USBH_SHELL_CMD_OPT_HOST_ONLY = ASCII_CHAR_LATIN_LOWER_H,
  USBH_SHELL_CMD_OPT_HELP = ASCII_CHAR_QUESTION_MARK
} USBH_SHELL_CMD_OPT_ENUM;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_INT16S USBH_ShellCmdListUSB(CPU_INT16U      argc,
                                       CPU_CHAR        *argv[],
                                       SHELL_OUT_FNCT  out_fnct,
                                       SHELL_CMD_PARAM *pcmd_param);

static void USBH_ShellCmdAcquire(RTOS_ERR *p_err);

static void USBH_ShellCmdRel(RTOS_ERR *p_err);

static void USBH_ShellCmdInvalidCmdHandler(CPU_CHAR                *p_str_buf,
                                           USBH_SHELL_CMD_OPT_ENUM *p_cmd_buffer,
                                           CPU_INT08U              opt,
                                           USBH_CMD_TRACE_FNCT     trace_fnct);

static void USBH_ShellCmdHubDevListHandler(USBH_HUB_FNCT   *p_hub_fnct,
                                           USBH_DEV_HANDLE *dev_handle_list,
                                           CPU_INT08U      tier_nbr);

static CPU_BOOLEAN USBH_ShellCmdCharToInt(CPU_CHAR   *p_str,
                                          CPU_INT32U *p_val);

static void USBH_ShellCmdTraceFnct(CPU_CHAR *p_str);

static void USBH_ShellCmdReportWrite(CPU_INT32U          opt,
                                     USBH_CMD_TRACE_FNCT trace_fnct);

static void USBH_ShellCmdDevReportWrite(USBH_DEV_HANDLE     dev_handle,
                                        CPU_INT32U          opt,
                                        USBH_CMD_TRACE_FNCT trace_fnct);

static void USBH_ShellCmdUSB_TreeWrite(void                *p_arg,
                                       CPU_INT08U          tier_nbr,
                                       CPU_INT32U          opt,
                                       USBH_CMD_TRACE_FNCT trace_fnct);

static CPU_CHAR *USBH_ShellCmdSpdMsgGet(USBH_DEV_SPD dev_spd);

#if (USBH_CFG_STR_EN == DEF_ENABLED)
static void USBH_ShellCmdStrWrite(USBH_DEV            *p_dev,
                                  CPU_INT08U          str_ix,
                                  USBH_CMD_TRACE_FNCT trace_fnct);
#endif

static void USBH_ShellCmdNbrWrite(CPU_INT32U          nbr,
                                  CPU_INT08U          nbr_dig,
                                  CPU_INT08U          nbr_base,
                                  USBH_CMD_TRACE_FNCT trace_fnct);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static SHELL_CMD USBH_ShellCmdTbl[] =
{
  { USBH_SHELL_CMD_LSUSB, USBH_ShellCmdListUSB },
  { 0, 0                   }
};

static SHELL_OUT_FNCT  USBH_ShellCmdOutFnct = DEF_NULL;
static SHELL_CMD_PARAM *USBH_ShellCmdParam = DEF_NULL;

static CPU_INT08U *USBH_ShellCmdDataPtr = DEF_NULL;
static CPU_INT08U USBH_ShellCmdDataLen = 0u;

static KAL_LOCK_HANDLE USBH_ShellCmdLock;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_ShellCmdInit()
 *
 * @brief    Initializes the USBH's Shell command list.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function :
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_ABORT
 *                       - RTOS_ERR_OWNERSHIP
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_NO_MORE_RSRC
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_WOULD_BLOCK
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *                       - RTOS_ERR_IS_OWNER
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_OS_ILLEGAL_RUN_TIME
 *                       - RTOS_ERR_INVALID_ARG
 *                       - RTOS_ERR_TIMEOUT
 *                       - RTOS_ERR_OS_OBJ_DEL
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_ALREADY_EXISTS
 *******************************************************************************************************/
void USBH_ShellCmdInit(RTOS_ERR *p_err)
{
  USBH_ShellCmdDataLen = DEF_MIN(USBH_InitCfg.MaxDescLen, 255u);
  USBH_ShellCmdDataPtr = (CPU_INT08U *)Mem_SegAllocHW("USBH - Shell cmd buffer",
                                                      USBH_InitCfg.MemSegBufPtr,
                                                      USBH_ShellCmdDataLen,
                                                      USBH_InitCfg.BufAlignOctets,
                                                      DEF_NULL,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  USBH_ShellCmdLock = KAL_LockCreate("USBH - Shell Cmd lock",
                                     DEF_NULL,
                                     p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  Shell_CmdTblAdd("USBH", USBH_ShellCmdTbl, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
}

/****************************************************************************************************//**
 *                                           USBH_ShellCmdListUSB()
 *
 * @brief    Handle the lsusb command, which list all connected devices on the Core.
 *
 * @param    argc        Argument count.
 *
 * @param    argv        Array of pointers containing arguments' string.
 *
 * @param    out_fnct    Unused.
 *
 * @param    pcmd_param  Unused.
 *
 * @return   SHELL_EXEC_ERR_NONE, if no error(s),
 *           SHELL_EXEC_ERR,      otherwise.
 *
 * @note     (1) This function will require to acquire the lock which protects the global variables.
 *
 * @note     (2) Device Command requires with an address. Due to constrains, in optimized footprint, Core
 *               is limited to up to the address 31, whereas in optimized speed, Core is limited to an
 *               array size, set in CfgExt->OptimizedSpd->NbrDevPerHost.
 *******************************************************************************************************/
static CPU_INT16S USBH_ShellCmdListUSB(CPU_INT16U      argc,
                                       CPU_CHAR        *argv[],
                                       SHELL_OUT_FNCT  out_fnct,
                                       SHELL_CMD_PARAM *pcmd_param)
{
  USBH_DEV_HANDLE     dev_handle_list[USBH_SHELL_CMD_SUPPORT_HOST_QTY][USBH_SHELL_CMD_DEV_MAX];
  CPU_INT08U          ix;
  USBH_HOST           *p_host;
  USBH_HC             *p_hc;
  CPU_INT08U          hc_ix;
  USBH_HUB_FNCT       *p_hub_fnct;
  USBH_CMD_TRACE_FNCT trace_fnct = &USBH_ShellCmdTraceFnct;
  RTOS_ERR            err;

  USBH_ShellCmdAcquire(&err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return (SHELL_EXEC_ERR);
  }

  USBH_ShellCmdOutFnct = out_fnct;
  USBH_ShellCmdParam = pcmd_param;

  if (argc == 1u) {                                             // Default command without options.
    for (ix = 0u; ix < USBH_Ptr->HostQty; ix++) {
      p_host = &(USBH_Ptr->HostTblPtr[ix]);
      for (hc_ix = 0u; hc_ix < p_host->HC_NbrNext; hc_ix++) {
        p_hc = USBH_HC_PtrGet(p_host, hc_ix, &err);
        if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
          continue;
        }

        p_hub_fnct = &(p_hc->RH_Fnct);                          // The Root Hub is always on the tier one.
        USBH_ShellCmdUSB_TreeWrite(p_hub_fnct,
                                   1u,
                                   USBH_CMD_USB_TREE_OPT_NONE,
                                   trace_fnct);
      }
    }
    trace_fnct("\r\n");
  } else {
    CPU_INT08U              dev_addr = 0u;
    USBH_SHELL_CMD_OPT_ENUM cmd_buffer = USBH_SHELL_CMD_OPT_ENTER;
    CPU_INT16U              cmd_ix;
    CPU_CHAR                str_buf[20u];
    CPU_INT32U              buf_int;
    CPU_INT32U              opt = 0u;
    CPU_INT16S              val;
    CPU_CHAR                *p_char;

    //                                                             Get the list of all connected devices.
    for (ix = 0u; ix < USBH_SHELL_CMD_SUPPORT_HOST_QTY; ix++) {
      for (hc_ix = 0u; hc_ix < USBH_SHELL_CMD_DEV_MAX; hc_ix++) {
        dev_handle_list[ix][hc_ix] = USBH_DEV_HANDLE_INVALID;
      }
      if (ix < USBH_Ptr->HostQty) {
        p_host = &(USBH_Ptr->HostTblPtr[ix]);
        for (hc_ix = 0u; hc_ix < p_host->HC_NbrNext; hc_ix++) {
          p_hc = USBH_HC_PtrGet(p_host, hc_ix, &err);
          if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
            continue;
          }

          p_hub_fnct = &(p_hc->RH_Fnct);
          //                                                       The Root Hub is always on the tier one.
          USBH_ShellCmdHubDevListHandler(p_hub_fnct,
                                         dev_handle_list[ix],
                                         1u);
        }
      }
    }

    Mem_Clr((void *)str_buf, Str_Len(str_buf));
    for (cmd_ix = 1u; cmd_ix < argc; cmd_ix++) {
      p_char = argv[cmd_ix];
      //                                                           Look for a command prefix.
      switch (p_char[0u]) {
        case USBH_SHELL_CMD_ENTER:
          if (cmd_buffer == USBH_SHELL_CMD_OPT_WAIT_HOST) {
            USBH_ShellCmdDevReportWrite(dev_handle_list[0u][dev_addr], opt, trace_fnct);
            cmd_buffer = USBH_SHELL_CMD_OPT_ENTER;
          }
          val = Str_Cmp((const  CPU_CHAR *)  USBH_SHELL_CMD_HELP,
                        (const  CPU_CHAR *)&(p_char[1u]));
          if (val == 0u) {
            cmd_buffer = USBH_SHELL_CMD_OPT_ENTER;
            trace_fnct(USBH_SHELL_CMD_STRING_HELP);
            continue;
          }
          if (Str_Len(p_char) == 2u) {
            switch (p_char[1]) {
              case USBH_SHELL_CMD_DEV:
                if (cmd_buffer == USBH_SHELL_CMD_OPT_HOST_ONLY) {
                  //                                               Host command with all device display.
                  opt = (USBH_SHELL_CMD_HOST_STATUS
                         | USBH_CMD_STATUS_OPT_DEV);
                  USBH_ShellCmdReportWrite(opt, trace_fnct);
                  cmd_buffer = USBH_SHELL_CMD_OPT_ENTER;
                  continue;
                } else if (cmd_buffer == USBH_SHELL_CMD_OPT_ENTER) {
                  //                                               Device command detected.
                  cmd_buffer = USBH_SHELL_CMD_OPT_DEV_ONLY;
                  continue;
                }
                USBH_ShellCmdInvalidCmdHandler(str_buf,
                                               &cmd_buffer,
                                               USBH_SHELL_CMD_BIT_INVALID,
                                               trace_fnct);
                break;

              case USBH_SHELL_CMD_HOST:
                if (cmd_buffer == USBH_SHELL_CMD_OPT_ENTER) {
                  //                                               Host command detected.
                  cmd_buffer = USBH_SHELL_CMD_OPT_HOST_ONLY;
                  continue;
                }
                USBH_ShellCmdInvalidCmdHandler(str_buf,
                                               &cmd_buffer,
                                               USBH_SHELL_CMD_BIT_INVALID,
                                               trace_fnct);
                break;

#if (USBH_CFG_STR_EN == DEF_ENABLED)
              case USBH_SHELL_CMD_STR:
                //                                                 String command detected: looking for case.
                if (cmd_buffer == USBH_SHELL_CMD_OPT_DEV_ONLY) {
                  //                                               Device command with string display.
                  cmd_buffer = USBH_SHELL_CMD_OPT_DEV_STR;
                  continue;
                } else if (cmd_buffer == USBH_SHELL_CMD_OPT_HOST_ONLY) {
                  //                                               Host command with all device with string display.
                  opt = (USBH_SHELL_CMD_HOST_STATUS
                         | USBH_CMD_STATUS_OPT_DEV
                         | USBH_CMD_STATUS_OPT_STR);
                  USBH_ShellCmdReportWrite(opt, trace_fnct);
                  cmd_buffer = USBH_SHELL_CMD_OPT_ENTER;
                  continue;
                } else if (cmd_buffer == USBH_SHELL_CMD_OPT_ENTER) {
                  //                                               Summary USB Tree with string display.
                  buf_int = USBH_CMD_USB_TREE_OPT_STR;
                  for (ix = 0u; ix < USBH_Ptr->HostQty; ix++) {
                    p_host = &(USBH_Ptr->HostTblPtr[ix]);
                    for (hc_ix = 0u; hc_ix < p_host->HC_NbrNext; hc_ix++) {
                      p_hc = USBH_HC_PtrGet(p_host, hc_ix, &err);
                      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
                        continue;
                      }
                      //                                           The Root Hub is always on the tier one.
                      p_hub_fnct = &(p_hc->RH_Fnct);
                      USBH_ShellCmdUSB_TreeWrite(p_hub_fnct,
                                                 1u,
                                                 USBH_CMD_USB_TREE_OPT_STR,
                                                 trace_fnct);
                    }
                  }
                  trace_fnct("\r\n");
                  continue;
                }
                USBH_ShellCmdInvalidCmdHandler(str_buf,
                                               &cmd_buffer,
                                               USBH_SHELL_CMD_BIT_INVALID,
                                               trace_fnct);
                break;
#endif

              case USBH_SHELL_CMD_BASIC:
                if (cmd_buffer == USBH_SHELL_CMD_OPT_HOST_ONLY) {
                  //                                               Host command status display.
                  opt = USBH_SHELL_CMD_HOST_STATUS;
                  USBH_ShellCmdReportWrite(opt, trace_fnct);
                  cmd_buffer = USBH_SHELL_CMD_OPT_ENTER;
                  continue;
                }
                USBH_ShellCmdInvalidCmdHandler(str_buf,
                                               &cmd_buffer,
                                               USBH_SHELL_CMD_BIT_INVALID,
                                               trace_fnct);
                break;

              default:
                USBH_ShellCmdInvalidCmdHandler(str_buf,
                                               &cmd_buffer,
                                               USBH_SHELL_CMD_BIT_UNKNOWN,
                                               trace_fnct);
                break;
            }
          }
          break;

        case USBH_SHELL_CMD_QUESTION_MARK:
          if (cmd_buffer == USBH_SHELL_CMD_OPT_WAIT_HOST) {
            USBH_ShellCmdDevReportWrite(dev_handle_list[0u][dev_addr], opt, trace_fnct);
          }
          cmd_buffer = USBH_SHELL_CMD_OPT_ENTER;
          trace_fnct(USBH_SHELL_CMD_STRING_HELP);
          break;

        default:
          switch (cmd_buffer) {
            case USBH_SHELL_CMD_OPT_DEV_ONLY:
#if (USBH_CFG_STR_EN == DEF_ENABLED)
            case USBH_SHELL_CMD_OPT_DEV_STR:
#endif
              if ((!USBH_ShellCmdCharToInt(p_char, &buf_int))
                  || (buf_int > USBH_SHELL_CMD_DEV_MAX)) {
                USBH_ShellCmdInvalidCmdHandler(str_buf,
                                               &cmd_buffer,
                                               USBH_SHELL_CMD_BIT_NO_DEV,
                                               trace_fnct);
                continue;
              }
              dev_addr = (CPU_INT08U)buf_int;
#if (USBH_CFG_STR_EN == DEF_ENABLED)
              opt = (cmd_buffer == USBH_SHELL_CMD_OPT_DEV_ONLY)
                    ? (USBH_CMD_DEV_INFO_OPT_NONE)
                    : (USBH_CMD_DEV_INFO_OPT_STR);
#else
              opt = USBH_CMD_DEV_INFO_OPT_NONE;
#endif
              if (cmd_ix + 1u == argc) {
                USBH_ShellCmdDevReportWrite(dev_handle_list[0u][dev_addr], opt, trace_fnct);
                cmd_buffer = USBH_SHELL_CMD_OPT_ENTER;
              } else {
                cmd_buffer = USBH_SHELL_CMD_OPT_WAIT_HOST;
              }
              break;

            case USBH_SHELL_CMD_OPT_WAIT_HOST:
              if ((!USBH_ShellCmdCharToInt(p_char, &buf_int))
                  || (buf_int > USBH_SHELL_CMD_SUPPORT_HOST_QTY)) {
                USBH_ShellCmdInvalidCmdHandler(str_buf,
                                               &cmd_buffer,
                                               USBH_SHELL_CMD_BIT_NO_DEV,
                                               trace_fnct);
                continue;
              }
              USBH_ShellCmdDevReportWrite(dev_handle_list[buf_int][dev_addr], opt, trace_fnct);
              cmd_buffer = USBH_SHELL_CMD_OPT_ENTER;
              break;

            default:
              USBH_ShellCmdInvalidCmdHandler(str_buf,
                                             &cmd_buffer,
                                             USBH_SHELL_CMD_BIT_UNKNOWN,
                                             trace_fnct);
              continue;
          }
          break;
      }
    }
  }

  USBH_ShellCmdOutFnct = DEF_NULL;
  USBH_ShellCmdParam = DEF_NULL;
  USBH_ShellCmdRel(&err);

  return (SHELL_EXEC_ERR_NONE);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           USBH_ShellCmdAcquire()
 *
 * @brief    Acquire lock for USBH's Shell command to allow use of out_fnct and params global variables.
 *
 * @param    p_err   Pointer to the variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void USBH_ShellCmdAcquire(RTOS_ERR *p_err)
{
  KAL_LockAcquire(USBH_ShellCmdLock,
                  KAL_OPT_PEND_BLOCKING,
                  KAL_TIMEOUT_INFINITE,
                  p_err);
}

/****************************************************************************************************//**
 *                                           USBH_ShellCmdRel()
 *
 * @brief    Release lock to allow other USBH's Shell command to use out_fnct and params global variables.
 *
 * @param    p_err   Pointer to the variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void USBH_ShellCmdRel(RTOS_ERR *p_err)
{
  KAL_LockRelease(USBH_ShellCmdLock,
                  p_err);
}

/****************************************************************************************************//**
 *                                       USBH_ShellCmdInvalidCmdHandler()
 *
 * @brief    List all devices connected on the current hub.
 *
 * @param    p_str_buf       Pointer to the string.
 *
 * @param    p_cmd_buffer    Pointer to the command.
 *
 * @param    opt             Options for call.
 *                               - APP_USBH_SHELL_CMD_BIT_NONE       Invalid Command Option
 *                               - APP_USBH_SHELL_CMD_BIT_INVALID    Invalid Command Option
 *                               - APP_USBH_SHELL_CMD_BIT_UNKNOWN    Unknown Command
 *                               - APP_USBH_SHELL_CMD_BIT_NO_DEV     Invalid Device
 *
 * @param    trace_fnct      Pointer to the function where to trace the string.
 *
 * @note     (1) When this is called, the command returns to the default value and the algorithm will
 *               continue to read the arguments.
 *******************************************************************************************************/
static void USBH_ShellCmdInvalidCmdHandler(CPU_CHAR                *p_str_buf,
                                           USBH_SHELL_CMD_OPT_ENUM *p_cmd_buffer,
                                           CPU_INT08U              opt,
                                           USBH_CMD_TRACE_FNCT     trace_fnct)
{
  CPU_CHAR *p_str;

  *p_cmd_buffer = USBH_SHELL_CMD_OPT_ENTER;
  if (DEF_BIT_IS_SET(opt, USBH_SHELL_CMD_BIT_NO_DEV)) {
    p_str = Str_Cat((CPU_CHAR *) p_str_buf,
                    (const  CPU_CHAR *)"Invalid device!\r\n");
  } else if (DEF_BIT_IS_SET(opt, USBH_SHELL_CMD_BIT_UNKNOWN)) {
    p_str = Str_Cat((CPU_CHAR *) p_str_buf,
                    (const  CPU_CHAR *)"Unknown cmd!\r\n");
  } else {
    p_str = Str_Cat((CPU_CHAR *) p_str_buf,
                    (const  CPU_CHAR *)"Invalid cmd opt!\r\n");
  }
  p_str = &(p_str[Str_Len(p_str)]);
  p_str[0u] = ASCII_CHAR_NULL;
  trace_fnct(p_str_buf);
  Mem_Clr((void *)p_str_buf, Str_Len(p_str_buf));
}

/****************************************************************************************************//**
 *                                       USBH_ShellCmdHubDevListHandler()
 *
 * @brief    List all devices connected on the current hub and add its handle in the list.
 *
 * @param    p_hub_fnct          Pointer to the hub.
 *
 * @param    dev_handle_list     Table containing all connected devices.
 *
 * @param    tier_nbr            Tier number which hub currently is in the bus topology.
 *
 * @note     (1) Universal Serial Bus Specification Revision 2.0, Chapter 4.1.1, on Bus Topology states
 *               that "five non-root [or external] hubs can be supported in a communication path between
 *               the host and any device [...] only functions can be enabled in tier seven.
 *******************************************************************************************************/
static void USBH_ShellCmdHubDevListHandler(USBH_HUB_FNCT   *p_hub_fnct,
                                           USBH_DEV_HANDLE *dev_handle_list,
                                           CPU_INT08U      tier_nbr)
{
  USBH_DEV_HANDLE dev_handle;
  CPU_INT08U      port_nbr;

  for (port_nbr = 1u; port_nbr <= p_hub_fnct->NbrPort; port_nbr++) {
    USBH_DEV *p_dev = USBH_HUB_DevAtPortGet(p_hub_fnct,
                                            port_nbr);
    if (p_dev == DEF_NULL) {
      continue;
    }
    dev_handle = p_dev->Handle;
    if ((CPU_INT08U)dev_handle < USBH_SHELL_CMD_DEV_MAX) {
      dev_handle_list[(CPU_INT08U)(dev_handle)] = dev_handle;
    }

    if ((p_dev->ClassCode == USBH_CLASS_CODE_HUB)
        && (tier_nbr <= 6u)) {                                  // See Note #1.
      USBH_ShellCmdHubDevListHandler((USBH_HUB_FNCT *)p_dev->ClassFnctPtr,
                                     dev_handle_list,
                                     tier_nbr + 1u);
    }
  }
}

/****************************************************************************************************//**
 *                                           USBH_ShellCmdCharToInt()
 *
 * @brief    Convert an ascii string into an integer value.
 *
 * @param    p_str   Pointer to the string.
 *
 * @param    p_val   Pointer to the add value.
 *
 * @return   DEF_OK          Success.
 *           DEF_FAIL        Failed.
 *******************************************************************************************************/
static CPU_BOOLEAN USBH_ShellCmdCharToInt(CPU_CHAR   *p_str,
                                          CPU_INT32U *p_val)
{
  CPU_INT08U str_len;
  CPU_INT08U ix;

  *p_val = 0u;
  str_len = (CPU_INT08U)Str_Len((const  CPU_CHAR *)p_str);
  for (ix = 0u; ix < str_len; ix++) {
    if ((p_str[ix] < ASCII_CHAR_DIGIT_ZERO) || (p_str[ix] > ASCII_CHAR_DIGIT_NINE)) {
      *p_val = 0u;
      return (DEF_FAIL);
    }
    *p_val *= 10u;
    *p_val += p_str[ix] - ASCII_CHAR_DIGIT_ZERO;
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           USBH_ShellCmdTraceFnct()
 *
 * @brief    Send to the output fnct the string from the trace.
 *
 * @param    p_str   Pointer to the string.
 *
 * @note     (1) This function must always be called in a context where the lock has been acquired, as
 *               it will use the values within the global variables.
 *******************************************************************************************************/
static void USBH_ShellCmdTraceFnct(CPU_CHAR *p_str)
{
  CPU_INT16U str_len;

  str_len = (CPU_INT16U)Str_Len((const  CPU_CHAR *)p_str);

  USBH_ShellCmdOutFnct(p_str, str_len, USBH_ShellCmdParam->OutputOptPtr);
}

/****************************************************************************************************//**
 *                                           USBH_DbgReportWrite()
 *
 * @brief    Writes stack status.
 *
 * @param    opt         Status write option(s).
 *
 * @param    trace_fnct  Pointer to the debug display in which info will be displayed.
 *******************************************************************************************************/
static void USBH_ShellCmdReportWrite(CPU_INT32U          opt,
                                     USBH_CMD_TRACE_FNCT trace_fnct)
{
  CPU_INT08U          host_ix;
  CPU_INT08U          nbr_class_driver = 0u;
  CPU_INT32U          opt_dev;
  CPU_SIZE_T          seg_rem_size;
  USBH_CLASS_DRV_ITEM *p_class_drv_item;
  USBH                *p_local_usbh_ptr;
  RTOS_ERR            err;

  opt_dev = USBH_CMD_DEV_INFO_OPT_NONE;
  if (DEF_BIT_IS_SET(opt, USBH_CMD_STATUS_OPT_DESC) == DEF_YES) {
    DEF_BIT_SET(opt_dev, USBH_CMD_DEV_INFO_OPT_DESC);
  }

  if (DEF_BIT_IS_SET(opt, USBH_CMD_STATUS_OPT_STR) == DEF_YES) {
    DEF_BIT_SET(opt_dev, USBH_CMD_DEV_INFO_OPT_STR);
  }

  p_local_usbh_ptr = USBH_Ptr;
  if (p_local_usbh_ptr == DEF_NULL) {                           // Cannot output any report if USBH not initialized.
    return;
  }

  trace_fnct("----------------------- USB Host status ------------------------\r\n");
  trace_fnct("| # class drv | Free space seg    |                            |\r\n");
  trace_fnct("| ");

  //                                                               Output nbr class drivers.
  p_class_drv_item = p_local_usbh_ptr->ClassDrvItemHeadPtr;
  while (p_class_drv_item != DEF_NULL) {
    p_class_drv_item = p_class_drv_item->NextPtr;
    nbr_class_driver++;
  }

  USBH_ShellCmdNbrWrite(nbr_class_driver, 3u, DEF_NBR_BASE_DEC, trace_fnct);
  trace_fnct("         | ");

  //                                                               Output free space on seg.
  seg_rem_size = Mem_SegRemSizeGet(USBH_InitCfg.MemSegPtr,
                                   1u,
                                   DEF_NULL,
                                   &err);
  USBH_ShellCmdNbrWrite(seg_rem_size, 7u, DEF_NBR_BASE_DEC, trace_fnct);
  trace_fnct(" / ");

  seg_rem_size = Mem_SegRemSizeGet(USBH_InitCfg.MemSegBufPtr,
                                   1u,
                                   DEF_NULL,
                                   &err);
  USBH_ShellCmdNbrWrite(seg_rem_size, 7u, DEF_NBR_BASE_DEC, trace_fnct);
  trace_fnct(" |                            |\r\n");

  //                                                               Output class drivers.
  if (DEF_BIT_IS_SET(opt, USBH_CMD_STATUS_OPT_CLASS_DRV) == DEF_YES) {
    trace_fnct("------------------- USB Host class drivers ---------------------\r\n");
    trace_fnct("| ");

    p_class_drv_item = p_local_usbh_ptr->ClassDrvItemHeadPtr;
    while (p_class_drv_item != DEF_NULL) {
      if (p_class_drv_item->ClassDrvPtr->NamePtr != DEF_NULL) {
        trace_fnct(p_class_drv_item->ClassDrvPtr->NamePtr);
      } else {
        trace_fnct(" No name ");
      }

      p_class_drv_item = p_class_drv_item->NextPtr;

      trace_fnct(" - ");
    }

    trace_fnct("\r\n");
  }

  for (host_ix = 0u; host_ix < p_local_usbh_ptr->HostQty; host_ix++) {
    CPU_INT08U hc_ix;
    USBH_HOST  *p_host = &p_local_usbh_ptr->HostTblPtr[host_ix];

    if (DEF_BIT_IS_SET(opt, USBH_CMD_STATUS_OPT_HOST) == DEF_YES) {
      trace_fnct("---------------- USB Host (host information) -------------------\r\n");
      trace_fnct("| Ix  | Nbr HC |                                               |\r\n");
      trace_fnct("| ");

      //                                                           Output host ix.
      USBH_ShellCmdNbrWrite(p_host->Ix, 3u, DEF_NBR_BASE_DEC, trace_fnct);
      trace_fnct(" | ");

      //                                                           Output nbr of HC.
      USBH_ShellCmdNbrWrite(p_host->HC_NbrNext, 3u, DEF_NBR_BASE_DEC, trace_fnct);
      trace_fnct("    | ");

      trace_fnct("                                              |");

      trace_fnct("\r\n");
    }

    for (hc_ix = 0u; hc_ix < p_host->HC_NbrNext; hc_ix++) {
      USBH_HC *p_hc;

      p_hc = USBH_HC_PtrGet(p_host, hc_ix, &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        continue;
      }

      if (DEF_BIT_IS_SET(opt, USBH_CMD_STATUS_OPT_HC) == DEF_YES) {
        CPU_CHAR *p_str;

        trace_fnct("------------ USB Host (host controller information) ------------\r\n");
        trace_fnct("| Ix  | RH Spd |                                               |\r\n");
        trace_fnct("| ");

        //                                                         Output HC ix.
        USBH_ShellCmdNbrWrite(p_hc->Ix, 3u, DEF_NBR_BASE_DEC, trace_fnct);
        trace_fnct(" | ");

        p_str = USBH_ShellCmdSpdMsgGet(p_hc->Drv.HW_InfoPtr->RH_Spd);
        trace_fnct(p_str);
        trace_fnct("     | ");

        trace_fnct("                                              |");

        trace_fnct("\r\n");
      }

      if (DEF_BIT_IS_SET(opt, USBH_CMD_STATUS_OPT_DEV) == DEF_YES) {
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
        CPU_INT08U dev_ix;

        for (dev_ix = 0u; dev_ix < USBH_InitCfg.OptimizeSpd.DevPerHostQty; dev_ix++) {
          USBH_DEV *p_dev = p_host->DevTbl[dev_ix];

          if (p_dev != DEF_NULL) {
            USBH_ShellCmdDevReportWrite(p_dev->Handle, opt_dev, trace_fnct);
          }
        }
#else
        USBH_DEV *p_dev = p_host->DevHeadPtr;

        while (p_dev != DEF_NULL) {
          USBH_ShellCmdDevReportWrite(p_dev->Handle, opt_dev, trace_fnct);

          p_dev = p_dev->HostDevNextPtr;
        }
#endif
      }
    }
  }
}

/****************************************************************************************************//**
 *                                           USBH_DbgDevReportWrite()
 *
 * @brief    Writes information and status on specified device.
 *
 * @param    dev_handle  Handle to device.
 *
 * @param    opt         Device status and information write option(s).
 *
 * @param    trace_fnct  Pointer to the debug display in which info will be displayed.
 *******************************************************************************************************/
static void USBH_ShellCmdDevReportWrite(USBH_DEV_HANDLE     dev_handle,
                                        CPU_INT32U          opt,
                                        USBH_CMD_TRACE_FNCT trace_fnct)
{
  CPU_CHAR    *p_str;
  CPU_INT08U  fnct_ix;
  CPU_INT08U  *p_buf;
  CPU_INT16U  desc_len;
  CPU_INT16U  ix;
  USBH_DEV    *p_dev;
  USBH_CONFIG *p_config;
  USBH_FNCT   *p_fnct;
  USBH_IF     *p_if;
  USBH_EP     *p_ep;
  RTOS_ERR    err;
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  CPU_INT08U ep_ix;
#endif

  p_dev = USBH_DevAcquireShared(dev_handle, &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               ------------------ PRINT DEV INFO ------------------
  trace_fnct("------------------------- Device info --------------------------\r\n");
  trace_fnct("| Addr | Vend ID | Prod ID | Spd | Class | Subclass | Protocol |\r\n");
  trace_fnct("| ");
  //                                                               Output dev addr.
  USBH_ShellCmdNbrWrite(p_dev->AddrCur, 3u, DEF_NBR_BASE_DEC, trace_fnct);
  trace_fnct("  | ");

  //                                                               Output dev vendor ID.
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_VENDOR_ID) == DEF_YES)
  USBH_ShellCmdNbrWrite(p_dev->VendorID, 4u, DEF_NBR_BASE_HEX, trace_fnct);
  trace_fnct("  | ");
#else
  trace_fnct("N/A     | ");
#endif

  //                                                               Output dev product ID.
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_PRODUCT_ID) == DEF_YES)
  USBH_ShellCmdNbrWrite(p_dev->ProductID, 4u, DEF_NBR_BASE_HEX, trace_fnct);
  trace_fnct("  | ");
#else
  trace_fnct("N/A     | ");
#endif

  p_str = USBH_ShellCmdSpdMsgGet(p_dev->Spd);                   // Output dev spd.
  trace_fnct(p_str);
  trace_fnct("  | ");
  //                                                               Output dev class code.
  USBH_ShellCmdNbrWrite(p_dev->ClassCode, 3u, DEF_NBR_BASE_DEC, trace_fnct);
  trace_fnct("   | ");

  //                                                               Output dev subclass code.
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_SUBCLASS) == DEF_YES)
  USBH_ShellCmdNbrWrite(p_dev->Subclass, 3u, DEF_NBR_BASE_DEC, trace_fnct);
  trace_fnct("      | ");
#else
  trace_fnct("N/A      | ");
#endif

  //                                                               Output dev protocol code.
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_PROTOCOL) == DEF_YES)
  USBH_ShellCmdNbrWrite(p_dev->Protocol, 3u, DEF_NBR_BASE_DEC, trace_fnct);
  trace_fnct("      |\r\n");
#else
  trace_fnct("N/A      |\r\n");
#endif

  //                                                               Print second part.
  trace_fnct("|--------------------------------------------------------------|\r\n");
  trace_fnct("| bcdUSB | bcdDevice | Nbr cfg | iManu | iProduct | iSerial #  |\r\n");

  trace_fnct("| ");

  //                                                               Output dev bcdUSB.
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_SPEC_NBR) == DEF_YES)
  USBH_ShellCmdNbrWrite(p_dev->SpecNbr, 4u, DEF_NBR_BASE_HEX, trace_fnct);
  trace_fnct(" | ");
#else
  trace_fnct("N/A    | ");
#endif

  //                                                               Output dev bcdDevice.
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_REL_NBR) == DEF_YES)
  USBH_ShellCmdNbrWrite(p_dev->RelNbr, 4u, DEF_NBR_BASE_HEX, trace_fnct);
  trace_fnct("    | ");
#else
  trace_fnct("N/A       | ");
#endif
  //                                                               Output dev nbr of config.
  USBH_ShellCmdNbrWrite(p_dev->NbrConfig, 3u, DEF_NBR_BASE_DEC, trace_fnct);
  trace_fnct("     | ");

  //                                                               Output dev iManufacturer.
#if (USBH_CFG_STR_EN == DEF_ENABLED)
  USBH_ShellCmdNbrWrite(p_dev->ManufacturerStrIx, 3u, DEF_NBR_BASE_DEC, trace_fnct);
  trace_fnct("   | ");
  //                                                               Output dev iProduct.
  USBH_ShellCmdNbrWrite(p_dev->ProdStrIx, 3u, DEF_NBR_BASE_DEC, trace_fnct);
  trace_fnct("      | ");
  //                                                               Output dev iSerial number.
  USBH_ShellCmdNbrWrite(p_dev->SerNbrStrIx, 3u, DEF_NBR_BASE_DEC, trace_fnct);
  trace_fnct("        |\r\n");
#else
  trace_fnct("N/A   | N/A      | N/A        |\r\n");
#endif

  //                                                               Print third part.
  trace_fnct("|--------------------------------------------------------------|\r\n");
  trace_fnct("| CtrlMPS | State  | IsSupended | Hub addr | Port |            |\r\n");

  trace_fnct("| ");
  //                                                               Output dev ctrl EP max pkt size.
  USBH_ShellCmdNbrWrite(p_dev->CtrlMaxPacketSize, 2u, DEF_NBR_BASE_DEC, trace_fnct);

  switch (p_dev->State) {                                       // Output dev state.
    case USBH_DEV_STATE_ADDR:
      trace_fnct("      | ADDR   | ");
      break;

    case USBH_DEV_STATE_CONFIG:
      trace_fnct("      | CONFIG | ");
      break;

    default:
      trace_fnct("      | Other  | ");
      break;
  }

  if (p_dev->IsSuspended == DEF_NO) {                           // Output dev suspend status.
    trace_fnct("No         | ");
  } else {
    trace_fnct("YES        | ");
  }

  if (p_dev->HubFnctPtr->DevPtr != DEF_NULL) {                  // Output dev hub addr.
    USBH_ShellCmdNbrWrite(p_dev->HubFnctPtr->DevPtr->AddrCur, 3u, DEF_NBR_BASE_DEC, trace_fnct);
    trace_fnct("      | ");
  } else {
    trace_fnct("ROOT     | ");
  }
  //                                                               Output dev port nbr.
  USBH_ShellCmdNbrWrite(p_dev->PortNbr, 1u, DEF_NBR_BASE_DEC, trace_fnct);
  trace_fnct("    |            |\r\n");

#if (USBH_CFG_STR_EN == DEF_ENABLED)
  if (DEF_BIT_IS_SET(opt, USBH_CMD_DEV_INFO_OPT_STR) == DEF_YES) {
    trace_fnct("----------------------------------------------------------------\r\n");
    trace_fnct("| - STRINGS -\r\n");

    if (p_dev->ManufacturerStrIx != 0u) {
      trace_fnct("| Manufacturer -> ");
      USBH_ShellCmdStrWrite(p_dev, p_dev->ManufacturerStrIx, trace_fnct);
      trace_fnct("\r\n");
    }

    if (p_dev->ProdStrIx != 0u) {
      trace_fnct("| Product      -> ");
      USBH_ShellCmdStrWrite(p_dev, p_dev->ProdStrIx, trace_fnct);
      trace_fnct("\r\n");
    }

    if (p_dev->SerNbrStrIx != 0u) {
      trace_fnct("| Serial nbr   -> ");
      USBH_ShellCmdStrWrite(p_dev, p_dev->SerNbrStrIx, trace_fnct);
      trace_fnct("\r\n");
    }
  }
#endif

  //                                                               ------------------ PRINT CFG INFO ------------------
  p_config = &p_dev->ConfigCur;
  trace_fnct("---------------------- Configuration info ----------------------\r\n");
  trace_fnct("| Nbr | Attributes               | Max pwr | Nbr fnct | Str ix |\r\n");
  trace_fnct("| ");

  //                                                               Output config nbr.
  USBH_ShellCmdNbrWrite(p_config->Nbr, 3u, DEF_NBR_BASE_DEC, trace_fnct);

  //                                                               Output config's attributes.
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_CONFIG_ATTR) == DEF_YES)
  if (DEF_BIT_IS_SET(p_config->Attr, USBH_CONFIG_ATTR_SELF_POWERED) == DEF_YES) {
    trace_fnct(" | Pwr: SELF -  ");
  } else {
    trace_fnct(" | Pwr: BUS  -  ");
  }

  if (DEF_BIT_IS_SET(p_config->Attr, USBH_CONFIG_ATTR_REMOTE_WAKEUP) == DEF_YES) {
    trace_fnct("R. Wkp: YES | ");
  } else {
    trace_fnct("R. Wkp: NO  | ");
  }
#else
  trace_fnct(" | N/A                      | ");
#endif

  //                                                               Output config max power.
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_CONFIG_MAX_PWR) == DEF_YES)
  USBH_ShellCmdNbrWrite(p_config->MaxPower * 2u, 3u, DEF_NBR_BASE_DEC, trace_fnct);
  trace_fnct(" mA  | ");
#else
  trace_fnct(" N/A    | ");
#endif

  //                                                               Output config's nbr of fnct.
  USBH_ShellCmdNbrWrite(p_config->NbrFnct, 3u, DEF_NBR_BASE_DEC, trace_fnct);
  trace_fnct("      | ");

#if (USBH_CFG_STR_EN == DEF_ENABLED)
  //                                                               Output config's str ix.
  USBH_ShellCmdNbrWrite(p_config->StrIx, 3u, DEF_NBR_BASE_DEC, trace_fnct);
  trace_fnct("    |");

  if ((DEF_BIT_IS_SET(opt, USBH_CMD_DEV_INFO_OPT_STR) == DEF_YES)
      && (p_config->StrIx != 0u)) {
    trace_fnct("----------------------------------------------------------------\r\n");
    trace_fnct("| String -> ");

    USBH_ShellCmdStrWrite(p_dev, p_config->StrIx, trace_fnct);
  }
#else
  trace_fnct("N/A    |");
#endif

  trace_fnct("\r\n");

  //                                                               ----------------- PRINT FNCTS INFO -----------------
  fnct_ix = 0;
  USBH_OBJ_ITERATOR_INIT(p_fnct, p_config, Fnct) {
    trace_fnct("    ------------------------- Function info ------------------------\r\n");
    trace_fnct("    | Nbr | Class | Subclass | Protocol | Str | # IF | Class drv   |\r\n");
    trace_fnct("    | ");

    //                                                             Output fnct nbr.
    USBH_ShellCmdNbrWrite(fnct_ix, 3u, DEF_NBR_BASE_DEC, trace_fnct);
    trace_fnct(" | ");

    //                                                             Output fnct class.
    USBH_ShellCmdNbrWrite(p_fnct->ClassCode, 3u, DEF_NBR_BASE_DEC, trace_fnct);
    trace_fnct("   | ");

    //                                                             Output fnct subclass.
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_FNCT_SUBCLASS) == DEF_YES)
    USBH_ShellCmdNbrWrite(p_fnct->Subclass, 3u, DEF_NBR_BASE_DEC, trace_fnct);
    trace_fnct("      | ");
#else
    trace_fnct("N/A      | ");
#endif

    //                                                             Output fnct protocol.
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_FNCT_PROTOCOL) == DEF_YES)
    USBH_ShellCmdNbrWrite(p_fnct->Protocol, 3u, DEF_NBR_BASE_DEC, trace_fnct);
    trace_fnct("      | ");
#else
    trace_fnct("N/A      | ");
#endif

    //                                                             Output fnct str ix.
#if (USBH_CFG_STR_EN == DEF_ENABLED)
    USBH_ShellCmdNbrWrite(p_fnct->StrIx, 3u, DEF_NBR_BASE_DEC, trace_fnct);
    trace_fnct(" | ");
#else
    trace_fnct("N/A | ");
#endif

    //                                                             Output fnct nbr of IF.
    USBH_ShellCmdNbrWrite(p_fnct->NbrIF, 3u, DEF_NBR_BASE_DEC, trace_fnct);
    trace_fnct("  | ");

    if (p_fnct->ClassDrvPtr != DEF_NULL) {
      trace_fnct(p_fnct->ClassDrvPtr->NamePtr);

      //                                                           Output class specific info.
      if (p_fnct->ClassDrvPtr->TraceDump != DEF_NULL) {
        trace_fnct("\r\n");

        p_fnct->ClassDrvPtr->TraceDump(p_fnct->ClassFnctPtr,
                                       opt,
                                       trace_fnct);
      }
    } else {
      trace_fnct("None\r\n");
    }

#if (USBH_CFG_STR_EN == DEF_ENABLED)
    if ((DEF_BIT_IS_SET(opt, USBH_CMD_DEV_INFO_OPT_STR) == DEF_YES)
        && (p_fnct->StrIx != 0u)) {
      trace_fnct("    ----------------------------------------------------------------\r\n");
      trace_fnct("    | String -> ");

      USBH_ShellCmdStrWrite(p_dev, p_fnct->StrIx, trace_fnct);
    }
#endif

    trace_fnct("\r\n");

    //                                                             ------------------ PRINT IF INFO -------------------
    USBH_OBJ_ITERATOR_INIT(p_if, p_fnct, IF) {
      trace_fnct("        ------------------------- Interface info -----------------------\r\n");
      trace_fnct("        | Nbr | Class | Subclass | Protocol | Str | # EP | Alt # | Cur |\r\n");
      trace_fnct("        | ");

      //                                                           Output IF nbr.
      USBH_ShellCmdNbrWrite(p_if->Nbr, 3u, DEF_NBR_BASE_DEC, trace_fnct);
      trace_fnct(" | ");

      //                                                           Output IF class.
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_IF_CLASS) == DEF_YES)
      USBH_ShellCmdNbrWrite(p_if->ClassCode, 3u, DEF_NBR_BASE_DEC, trace_fnct);
      trace_fnct("   | ");
#else
      trace_fnct("N/A   | ");
#endif

      //                                                           Output IF subclass.
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_IF_SUBCLASS) == DEF_YES)
      USBH_ShellCmdNbrWrite(p_if->Subclass, 3u, DEF_NBR_BASE_DEC, trace_fnct);
      trace_fnct("      | ");
#else
      trace_fnct("N/A      | ");
#endif

      //                                                           Output IF protocol.
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_IF_PROTOCOL) == DEF_YES)
      USBH_ShellCmdNbrWrite(p_if->Protocol, 3u, DEF_NBR_BASE_DEC, trace_fnct);
      trace_fnct("      | ");
#else
      trace_fnct("N/A      | ");
#endif

      //                                                           Output IF str ix.
#if (USBH_CFG_STR_EN == DEF_ENABLED)
      USBH_ShellCmdNbrWrite(p_if->StrIx, 3u, DEF_NBR_BASE_DEC, trace_fnct);
      trace_fnct(" | ");
#else
      trace_fnct("N/A | ");
#endif

      //                                                           Output IF nbr of EP.
      USBH_ShellCmdNbrWrite(p_if->NbrEP, 3u, DEF_NBR_BASE_DEC, trace_fnct);
      trace_fnct("  | ");

#if (USBH_CFG_ALT_IF_EN == DEF_ENABLED)
      //                                                           Output IF alternate nbr.
      USBH_ShellCmdNbrWrite(p_if->AltNbr, 3u, DEF_NBR_BASE_DEC, trace_fnct);
      trace_fnct("   | ");

      //                                                           Output IF current alt nbr.
      USBH_ShellCmdNbrWrite(p_if->AltCur, 3u, DEF_NBR_BASE_DEC, trace_fnct);
      trace_fnct(" |\r\n");
#else
      trace_fnct("N/A   | N/A |\r\n");
#endif

#if (USBH_CFG_STR_EN == DEF_ENABLED)
      if ((DEF_BIT_IS_SET(opt, USBH_CMD_DEV_INFO_OPT_STR) == DEF_YES)
          && (p_if->StrIx != 0u)) {
        trace_fnct("        ----------------------------------------------------------------\r\n");
        trace_fnct("        | String -> ");

        USBH_ShellCmdStrWrite(p_dev, p_if->StrIx, trace_fnct);
      }
#endif

      trace_fnct("\r\n");

      //                                                           ------------------ PRINT EP INFO -------------------
#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
      for (ep_ix = 0u; ep_ix < USBH_DEV_NBR_EP - 2u; ep_ix++) {
        p_ep = p_if->EPTblPtr[ep_ix];

        if (p_ep == DEF_NULL) {
          continue;
        }
#else
      p_ep = p_if->EPHeadPtr;
      while (p_ep != DEF_NULL) {
#endif
        trace_fnct("            -------------------------- Endpoint info -----------------------\r\n");
        trace_fnct("            | Addr | Phy nbr | Max pkt size | Interval | Type  Sync  Usage |\r\n");
        trace_fnct("            | ");

        //                                                         Output EP addr.
        USBH_ShellCmdNbrWrite(p_ep->Addr, 3u, DEF_NBR_BASE_DEC, trace_fnct);
        trace_fnct("  | ");

        //                                                         Output EP phy nbr.
        USBH_ShellCmdNbrWrite(USBH_EP_ADDR_TO_PHY(p_ep->Addr),
                              2u,
                              DEF_NBR_BASE_DEC,
                              trace_fnct);
        trace_fnct("      | ");

        //                                                         Output EP max pkt size.
        USBH_ShellCmdNbrWrite(USBH_EP_MAX_PKT_SIZE_GET(p_ep->MaxPktSize),
                              4u,
                              DEF_NBR_BASE_DEC,
                              trace_fnct);
        trace_fnct("         | ");

        //                                                         Output EP interval.
#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
        USBH_ShellCmdNbrWrite(p_ep->Interval, 3u, DEF_NBR_BASE_DEC, trace_fnct);
        trace_fnct("      | ");
#else
        trace_fnct("N/A      | ");
#endif

        //                                                         Output EP attributes.
        if (USBH_EP_TYPE_GET(p_ep->Attrib) == USBH_EP_TYPE_BULK) {
          trace_fnct("BULK  ");
        } else if (USBH_EP_TYPE_GET(p_ep->Attrib) == USBH_EP_TYPE_INTR) {
          trace_fnct("INTR  ");
        } else {
          trace_fnct("ISOC  ");
        }

        if (USBH_EP_SYNC_GET(p_ep->Attrib) == USBH_EP_TYPE_SYNC_NONE) {
          trace_fnct("NO    ");
        } else if (USBH_EP_SYNC_GET(p_ep->Attrib) == USBH_EP_TYPE_SYNC_SYNC) {
          trace_fnct("SYNC  ");
        } else {
          trace_fnct("ASYNC ");
        }

        if (USBH_EP_USAGE_GET(p_ep->Attrib) == USBH_EP_TYPE_USAGE_DATA) {
          trace_fnct("DATA  |");
        } else if (USBH_EP_USAGE_GET(p_ep->Attrib) == USBH_EP_TYPE_USAGE_FEEDBACK) {
          trace_fnct("FEEDB |");
        } else {
          trace_fnct("IMP F |");
        }

        trace_fnct("\r\n");

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_DISABLED)
        p_ep = p_ep->NextPtr;
#endif
      }

      USBH_OBJ_ITERATOR_NEXT_GET(p_if);
    }

    fnct_ix++;
    USBH_OBJ_ITERATOR_NEXT_GET(p_fnct);
  }

  //                                                               Output raw config desc content.
  if (DEF_BIT_IS_SET(opt, USBH_CMD_DEV_INFO_OPT_DESC) == DEF_YES) {
    p_buf = (CPU_INT08U *)USBH_ShellCmdDataPtr;
    desc_len = USBH_DevDescRdLocked(p_dev,
                                    USBH_DEV_REQ_RECIPIENT_DEV,
                                    USBH_DEV_REQ_TYPE_STD,
                                    USBH_DESC_TYPE_CONFIG,
                                    (p_dev->ConfigCur.Nbr - 1u),
                                    USBH_ShellCmdDataLen,
                                    p_buf,
                                    USBH_ShellCmdDataLen,
                                    &err);
    if (RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE) {
      trace_fnct("---------------- Configuration descriptor content --------------\r\n");

      for (ix = 1u; ix <= desc_len; ix++) {
        USBH_ShellCmdNbrWrite(p_buf[ix - 1u], 2u, DEF_NBR_BASE_HEX, trace_fnct);

        if ((ix % 12u) == 0u) {
          trace_fnct("\r\n");
        } else {
          trace_fnct(" ");
        }
      }
    }
  }

  USBH_DevRelShared(p_dev);

  trace_fnct("\r\n");
}

/****************************************************************************************************//**
 *                                       USBH_ShellCmdUSB_TreeWrite()
 *
 * @brief    Writes information and status on specified device.
 *
 * @param    p_arg       Pointer to the arg, in this case, a hub function structure.
 *
 * @param    tier_nbr    Tier number of the hub, in the bus topology.
 *
 * @param    opt         Device status and information write option(s).
 *
 * @param    trace_fnct  Pointer to the debug display in which info will be displayed.
 *
 * @note     (1) Universal Serial Bus Specification Revision 2.0, Chapter 4.1.1, on Bus Topology states
 *               that "five non-root [or external] hubs can be supported in a communication path between
 *               the host and any device [...] only functions can be enabled in tier seven.
 *******************************************************************************************************/
static void USBH_ShellCmdUSB_TreeWrite(void                *p_arg,
                                       CPU_INT08U          tier_nbr,
                                       CPU_INT32U          opt,
                                       USBH_CMD_TRACE_FNCT trace_fnct)
{
  USBH_HUB_FNCT *p_hub_fnct;
  CPU_INT08U    port_nbr;
  CPU_INT08U    ix;
  USBH_DEV      *p_dev;
  CPU_CHAR      *p_str;
  RTOS_ERR      err;

  if (p_arg == DEF_NULL) {
    return;
  }
  p_hub_fnct = (USBH_HUB_FNCT *)p_arg;

  for (port_nbr = 1u; port_nbr <= p_hub_fnct->NbrPort; port_nbr++) {
    p_dev = USBH_HUB_DevAtPortGet(p_hub_fnct,
                                  port_nbr);
    if (p_dev == DEF_NULL) {
      continue;
    }

    p_dev = USBH_DevAcquireShared(p_dev->Handle, &err);
    if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
      continue;
    }

    //                                                             Display device info in tree fashion
    if (tier_nbr == 1u) {
      trace_fnct("Root Hub Port #");
    } else {
      ix = tier_nbr - 1u;
      while (ix) {
        if (ix == 1u) {
          trace_fnct("|__ ");
        } else {
          trace_fnct("    ");
        }
        ix--;
      }
      trace_fnct("Port #");
    }
    //                                                             Output port nbr.
    USBH_ShellCmdNbrWrite(port_nbr, 1u, DEF_NBR_BASE_DEC, trace_fnct);

    trace_fnct(":  Dev #");                                     // Output dev addr.
    USBH_ShellCmdNbrWrite(p_dev->AddrCur, 3u, DEF_NBR_BASE_DEC, trace_fnct);

#if ((USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_VENDOR_ID) == DEF_YES) \
    || (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_PRODUCT_ID) == DEF_YES))
    trace_fnct(":  IDs #");
    //                                                             Output dev vendor ID.
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_VENDOR_ID) == DEF_YES)
    USBH_ShellCmdNbrWrite(p_dev->VendorID, 4u, DEF_NBR_BASE_HEX, trace_fnct);
    trace_fnct("|");
#else
    trace_fnct("    |");
#endif
    //                                                             Output dev product ID.
#if (USBH_CFG_FIELD_IS_EN(USBH_CFG_FIELD_EN_DEV_PRODUCT_ID) == DEF_YES)
    USBH_ShellCmdNbrWrite(p_dev->ProductID, 4u, DEF_NBR_BASE_HEX, trace_fnct);
#else
    trace_fnct("    ");
#endif
#endif

    trace_fnct(":  Spd: ");
    p_str = USBH_ShellCmdSpdMsgGet(p_dev->Spd);                 // Output dev spd.
    trace_fnct(p_str);

#if (USBH_CFG_STR_EN == DEF_ENABLED)
    if (DEF_BIT_IS_SET(opt, USBH_CMD_USB_TREE_OPT_STR) == DEF_YES) {
      //                                                           Output dev iManufacturer.
      if (p_dev->ManufacturerStrIx != 0u) {
        trace_fnct(":  ManuStr: ");
        USBH_ShellCmdStrWrite(p_dev, p_dev->ManufacturerStrIx, trace_fnct);
      }
      //                                                           Output dev iProduct.
      if (p_dev->ProdStrIx != 0u) {
        trace_fnct(":  ProdStr: ");
        USBH_ShellCmdStrWrite(p_dev, p_dev->ProdStrIx, trace_fnct);
      }
    }
#endif

    trace_fnct("\r\n");

    if ((p_dev->ClassCode == USBH_CLASS_CODE_HUB)
        && (tier_nbr <= 6u)) {                                  // See Note #1.
      USBH_ShellCmdUSB_TreeWrite(p_dev->ClassFnctPtr,
                                 tier_nbr + 1u,
                                 opt,
                                 trace_fnct);
    }

    USBH_DevRelShared(p_dev);
  }
}

/****************************************************************************************************//**
 *                                           USBH_DbgNbrWrite()
 *
 * @brief    Writes number.
 *
 * @param    nbr         Number to write.
 *
 * @param    nbr_dig     Number of digits to write.
 *
 * @param    nbr_base    Base of number.
 *                           - DEF_NBR_BASE_DEC
 *                           - DEF_NBR_BASE_HEX
 *
 * @param    trace_fnct  Pointer to the debug display in which info will be displayed.
 *******************************************************************************************************/
static void USBH_ShellCmdNbrWrite(CPU_INT32U          nbr,
                                  CPU_INT08U          nbr_dig,
                                  CPU_INT08U          nbr_base,
                                  USBH_CMD_TRACE_FNCT trace_fnct)
{
  CPU_CHAR str[10u];
  CPU_CHAR lead_char;

  if (nbr_base == DEF_NBR_BASE_HEX) {
    lead_char = '0';
    trace_fnct("0x");
  } else {
    lead_char = ' ';
  }

  (void)Str_FmtNbr_Int32U(nbr,
                          nbr_dig,
                          nbr_base,
                          lead_char,
                          DEF_NO,
                          DEF_YES,
                          &str[0u]);

  trace_fnct(str);
}

/****************************************************************************************************//**
 *                                           USBH_DbgDevSpdMsgGet()
 *
 * @brief    Gets description string corresponding to device speed.
 *
 * @param    dev_spd     Device speed.
 *
 * @return   Pointer to the device speed description string.
 *******************************************************************************************************/
static CPU_CHAR *USBH_ShellCmdSpdMsgGet(USBH_DEV_SPD dev_spd)
{
  CPU_CHAR *p_str;

  switch (dev_spd) {
    case USBH_DEV_SPD_LOW:
      p_str = "LS";
      break;

    case USBH_DEV_SPD_FULL:
      p_str = "FS";
      break;

    case USBH_DEV_SPD_HIGH:
      p_str = "HS";
      break;

    default:
      p_str = "??";
      break;
  }

  return (p_str);
}

/****************************************************************************************************//**
 *                                           USBH_DbgDevStrWrite()
 *
 * @brief    Writes device's string.
 *
 * @param    p_Dev       Pointer to the USBH_DEV structure.
 *
 * @param    str_ix      String index.
 *
 * @param    trace_fnct  Pointer to the debug display in which info will be displayed.
 *******************************************************************************************************/

#if (USBH_CFG_STR_EN == DEF_ENABLED)
static void USBH_ShellCmdStrWrite(USBH_DEV            *p_dev,
                                  CPU_INT08U          str_ix,
                                  USBH_CMD_TRACE_FNCT trace_fnct)
{
  CPU_CHAR   *p_str;
  CPU_INT08U char_cnt;
  CPU_INT08U str_len;
  RTOS_ERR   err;

  p_str = (CPU_CHAR *)USBH_ShellCmdDataPtr;

  str_len = USBH_DevStrRd(p_dev,
                          str_ix,
                          p_str,
                          USBH_ShellCmdDataLen,
                          &err);
  if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
    trace_fnct("ERROR retrieving string\r\n");
    return;
  }

  for (char_cnt = 2u; char_cnt < str_len; char_cnt++) {
    trace_fnct(&p_str[char_cnt]);
  }
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // (defined(RTOS_MODULE_USB_HOST_AVAIL) && defined(RTOS_MODULE_COMMON_SHELL_AVAIL))
