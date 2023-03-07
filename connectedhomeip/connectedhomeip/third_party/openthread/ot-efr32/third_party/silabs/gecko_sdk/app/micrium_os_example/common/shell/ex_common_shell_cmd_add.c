/***************************************************************************//**
 * @file
 * @brief Common Shell Example
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
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#ifdef  RTOS_MODULE_COMMON_SHELL_AVAIL

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/lib_def.h>
#include  <common/include/lib_math.h>
#include  <common/include/lib_str.h>

#include  <common/include/shell.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  EX_COMMON_SHELL_CMD_TBL_NAME              ("rng")

#define  EX_COMMON_SHELL_CMD_NAME_HELP             ("rng_help")
#define  EX_COMMON_SHELL_CMD_NAME_SET              ("rng_seed")
#define  EX_COMMON_SHELL_CMD_NAME_GET              ("rng_get")

#define  EX_COMMON_SHELL_RNG_SEED_INVALID_ARG       "Invalid argument."
#define  EX_COMMON_SHELL_RNG_SEED_HELP              "Usage is as follows:\r\n    rng_seed x\r\n  where x is an integer."

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_INT16S Ex_CommonShellRNG_Help(CPU_INT16U      argc,
                                         CPU_CHAR        *p_argv[],
                                         SHELL_OUT_FNCT  out_fnct,
                                         SHELL_CMD_PARAM *p_cmd_param);

static CPU_INT16S Ex_CommonShellRNG_Seed(CPU_INT16U      argc,
                                         CPU_CHAR        *p_argv[],
                                         SHELL_OUT_FNCT  out_fnct,
                                         SHELL_CMD_PARAM *p_cmd_param);

static CPU_INT16S Ex_CommonShellRNG_Get(CPU_INT16U      argc,
                                        CPU_CHAR        *p_argv[],
                                        SHELL_OUT_FNCT  out_fnct,
                                        SHELL_CMD_PARAM *p_cmd_param);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

static SHELL_CMD Ex_CommonShellCmdAddCmdTbl[] =
{
  { EX_COMMON_SHELL_CMD_NAME_HELP, Ex_CommonShellRNG_Help },    // Associate cmd str with cmd fnct.
  { EX_COMMON_SHELL_CMD_NAME_SET, Ex_CommonShellRNG_Seed },
  { EX_COMMON_SHELL_CMD_NAME_GET, Ex_CommonShellRNG_Get },
  { 0, 0 }                                                      // Tbl is NULL-terminated to indicate end.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         Ex_CommonShellCmdAdd()
 *
 * @brief  Provides example on how to add new commands in the Shell sub-module.
 *******************************************************************************************************/
void Ex_CommonShellCmdAdd(void)
{
  RTOS_ERR err;

  //                                                               Add cmds in Cmd Tbl to Shell's available cmds.
  Shell_CmdTblAdd((CPU_CHAR *)EX_COMMON_SHELL_CMD_TBL_NAME,
                  Ex_CommonShellCmdAddCmdTbl,
                  &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                        Ex_CommonShellRNG_Help()
 *
 * @brief  Outputs the help message for the RNG commands.
 *
 * @param  argc         Argument count.
 *
 * @param  p_argv       Array of arguments. Index 0 contains the command name.
 *
 * @param  out_fnct     Function to use to output information.
 *
 * @param  p_cmd_param  Pointer to SHELL_CMD_PARAM containing additional optional parameters,
 *                      unused in this case.
 *
 * @return  SHELL_EXEC_ERR_NONE, if NO error(s),
 *          SHELL_EXEC_ERR, otherwise.
 *******************************************************************************************************/
static CPU_INT16S Ex_CommonShellRNG_Help(CPU_INT16U      argc,
                                         CPU_CHAR        *p_argv[],
                                         SHELL_OUT_FNCT  out_fnct,
                                         SHELL_CMD_PARAM *p_cmd_param)
{
  SHELL_CMD  *p_shell_cmd;
  CPU_INT16S ret_val;

  PP_UNUSED_PARAM(argc);
  PP_UNUSED_PARAM(p_argv);

  //                                                               Iterate over all commands in cmd tbl.
  p_shell_cmd = Ex_CommonShellCmdAddCmdTbl;
  while (p_shell_cmd->Fnct != 0) {
    //                                                             Output each cmd's name.
    ret_val = out_fnct((CPU_CHAR *)p_shell_cmd->Name, Str_Len(p_shell_cmd->Name), p_cmd_param->OutputOptPtr);
    if ((ret_val == SHELL_OUT_RTN_CODE_CONN_CLOSED)
        || (ret_val == SHELL_OUT_ERR)) {
      return (SHELL_EXEC_ERR);
    }

    //                                                             Output new line.
    ret_val = out_fnct((CPU_CHAR *)STR_NEW_LINE, STR_NEW_LINE_LEN, p_cmd_param->OutputOptPtr);
    if ((ret_val == SHELL_OUT_RTN_CODE_CONN_CLOSED)
        || (ret_val == SHELL_OUT_ERR)) {
      return (SHELL_EXEC_ERR);
    }

    p_shell_cmd++;
  }

  return (SHELL_EXEC_ERR_NONE);
}

/****************************************************************************************************//**
 *                                        Ex_CommonShellRNG_Seed()
 *
 * @brief  Seeds the RNG with a new value.
 *
 * @param  argc         Argument count. In this case, should be 2: the command's name and the
 *                      seed value.
 *
 * @param  p_argv       Array of arguments. Index 0 contains the command name. In this case,
 *                      index 1 should contain the seed value for this function to execute
 *                      correctly.
 *
 * @param  out_fnct     Function to use to output information.
 *
 * @param  p_cmd_param  Pointer to SHELL_CMD_PARAM containing additional optional parameters,
 *                      unused in this case.
 *
 * @return  SHELL_EXEC_ERR_NONE.
 *******************************************************************************************************/
static CPU_INT16S Ex_CommonShellRNG_Seed(CPU_INT16U      argc,
                                         CPU_CHAR        *p_argv[],
                                         SHELL_OUT_FNCT  out_fnct,
                                         SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S result_h;
  CPU_INT16S result_help;
  RAND_NBR   seed;

  if (argc != 2) {                                              // If not enough or too much args, display help.
    (void)out_fnct((CPU_CHAR *)EX_COMMON_SHELL_RNG_SEED_INVALID_ARG, Str_Len(EX_COMMON_SHELL_RNG_SEED_INVALID_ARG), p_cmd_param->OutputOptPtr);
    (void)out_fnct((CPU_CHAR *)STR_NEW_LINE, STR_NEW_LINE_LEN, p_cmd_param->OutputOptPtr);
    (void)out_fnct((CPU_CHAR *)EX_COMMON_SHELL_RNG_SEED_HELP, Str_Len(EX_COMMON_SHELL_RNG_SEED_HELP), p_cmd_param->OutputOptPtr);
    (void)out_fnct((CPU_CHAR *)STR_NEW_LINE, STR_NEW_LINE_LEN, p_cmd_param->OutputOptPtr);
    return (SHELL_EXEC_ERR_NONE);
  }

  result_h = Str_Cmp(p_argv[1u], "-h");
  result_help = Str_Cmp(p_argv[1u], "--help");
  if ((result_h == 0)
      || (result_help == 0)) {                                  // Display help.
    (void)out_fnct((CPU_CHAR *)EX_COMMON_SHELL_RNG_SEED_HELP, Str_Len(EX_COMMON_SHELL_RNG_SEED_HELP), p_cmd_param->OutputOptPtr);
    (void)out_fnct((CPU_CHAR *)STR_NEW_LINE, STR_NEW_LINE_LEN, p_cmd_param->OutputOptPtr);
    return (SHELL_EXEC_ERR_NONE);
  }

  //                                                               Convert string number to int.
  seed = (RAND_NBR)Str_ParseNbr_Int32U((const  CPU_CHAR *)p_argv[1u],
                                       DEF_NULL,
                                       10u);

  Math_RandSetSeed(seed);

  return (SHELL_EXEC_ERR_NONE);
}

/****************************************************************************************************//**
 *                                         Ex_CommonShellRNG_Get()
 *
 * @brief  Outputs a new random number generated by LIB's RNG.
 *
 * @param  argc         Argument count. In this case, should be 1: the command's name and no
 *                      other argument.
 *
 * @param  p_argv       Array of arguments. Index 0 contains the command name.
 *
 * @param  out_fnct     Function to use to output information.
 *
 * @param  p_cmd_param  Pointer to SHELL_CMD_PARAM containing additional optional parameters,
 *                      unused in this case.
 *
 * @return  SHELL_EXEC_ERR_NONE, if NO error(s),
 *          SHELL_EXEC_ERR, otherwise.
 *******************************************************************************************************/
static CPU_INT16S Ex_CommonShellRNG_Get(CPU_INT16U      argc,
                                        CPU_CHAR        *p_argv[],
                                        SHELL_OUT_FNCT  out_fnct,
                                        SHELL_CMD_PARAM *p_cmd_param)
{
  RAND_NBR   rand;
  CPU_INT16S ret_val;
  CPU_CHAR   rand_str_buf[DEF_INT_32U_NBR_DIG_MAX + 1u];

  PP_UNUSED_PARAM(argc);
  PP_UNUSED_PARAM(p_argv);

  rand = Math_Rand();                                           // Obtain random nbr from LIB Math module.

  //                                                               Convert int decimal number to str.
  (void)Str_FmtNbr_Int32U(rand,
                          DEF_INT_32U_NBR_DIG_MAX,
                          DEF_NBR_BASE_DEC,
                          '\0',
                          DEF_NO,
                          DEF_YES,
                          &rand_str_buf[0u]);

  //                                                               Output random number obtained.
  ret_val = out_fnct(&rand_str_buf[0u], Str_Len(&rand_str_buf[0u]), p_cmd_param->OutputOptPtr);
  if ((ret_val == SHELL_OUT_RTN_CODE_CONN_CLOSED)
      || (ret_val == SHELL_OUT_ERR)) {
    return (SHELL_EXEC_ERR);
  }

  //                                                               Output new line.
  ret_val = out_fnct((CPU_CHAR *)STR_NEW_LINE, STR_NEW_LINE_LEN, p_cmd_param->OutputOptPtr);
  if ((ret_val == SHELL_OUT_RTN_CODE_CONN_CLOSED)
      || (ret_val == SHELL_OUT_ERR)) {
    return (SHELL_EXEC_ERR);
  }

  return (SHELL_EXEC_ERR_NONE);
}

#endif // RTOS_MODULE_COMMON_SHELL_AVAIL
