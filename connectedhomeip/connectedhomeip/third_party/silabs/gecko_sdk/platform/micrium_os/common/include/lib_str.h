/***************************************************************************//**
 * @file
 * @brief Common - ASCII String Management
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

/****************************************************************************************************//**
 * @defgroup LIB_STR LIB String API
 * @ingroup  LIB
 * @brief      LIB String API
 *
 * @addtogroup LIB_STR
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _LIB_STR_H_
#define  _LIB_STR_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                   ASCII STRING CONFIGURATION DEFINES
 *
 * Note(s) : (1) Some ASCII string configuration #define's MUST be available PRIOR to including any
 *               application configuration (see 'INCLUDE FILES  Note #1a').
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       STRING FLOATING POINT DEFINES
 *
 * Note(s) : (1) (a) (1) The maximum accuracy for 32-bit floating-point numbers :
 *
 *                               Maximum Accuracy            log [Internal-Base ^ (Number-Internal-Base-Digits)]
 *                       32-bit Floating-point Number  =  -----------------------------------------------------
 *                                                                           log [External-Base]
 *
 *                                                           log [2 ^ 24]
 *                                                       =  --------------
 *                                                           log [10]
 *
 *                                                       <  7.225  Base-10 Digits
 *
 *                           where
 *                                   Internal-Base                   Internal number base of floating-
 *                                                                       point numbers (i.e.  2)
 *                                   External-Base                   External number base of floating-
 *                                                                       point numbers (i.e. 10)
 *                                   Number-Internal-Base-Digits     Number of internal number base
 *                                                                       significant digits (i.e. 24)
 *
 *                   (2) Also, since some 32-bit floating-point calculations are converted to 32-bit
 *                       unsigned numbers, the maximum accuracy is limited to the maximum accuracy
 *                       for 32-bit unsigned numbers of 9 digits.
 *
 *               (b) Some CPUs' &/or compilers' floating-point implementations MAY further reduce the
 *                   maximum accuracy.
 *******************************************************************************************************/

#define  LIB_STR_FP_MAX_NBR_DIG_SIG_MIN                    1u
#define  LIB_STR_FP_MAX_NBR_DIG_SIG_MAX                    9u   // See Note #1a2.
#define  LIB_STR_FP_MAX_NBR_DIG_SIG_DFLT                   7u   // See Note #1a1.

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 *
 * Note(s) : (1) NO compiler-supplied standard library functions SHOULD be used.
 *
 *               #### The reference to standard library header files SHOULD be removed once all custom
 *               library functions are implemented WITHOUT reference to ANY standard library function(s).
 *
 *               See also 'STANDARD LIBRARY MACRO'S  Note #1'.
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/lib_ascii.h>

#include  <common/include/rtos_path.h>
#include  <common_cfg.h>

#if 0                                                           // See Note #1.
#include  <stdio.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           DEFAULT CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   STRING FLOATING POINT CONFIGURATION
 *
 * Note(s) : (1) Configure LIB_STR_CFG_FP_EN to enable/disable floating point string function(s).
 *
 *           (2) Configure LIB_STR_CFG_FP_MAX_NBR_DIG_SIG to configure the maximum number of significant
 *               digits to calculate &/or display for floating point string function(s).
 *
 *               See also 'STRING FLOATING POINT DEFINES  Note #1'.
 *******************************************************************************************************/

//                                                                 Configure floating point feature(s) [see Note #1] :
#ifndef  LIB_STR_CFG_FP_EN
#define  LIB_STR_CFG_FP_EN                      DEF_DISABLED
//                                                                 DEF_DISABLED     Floating point functions DISABLED
//                                                                 DEF_ENABLED      Floating point functions ENABLED
#endif

//                                                                 Configure floating point feature(s)' number of ...
//                                                                 ... significant digits (see Note #2).
#ifndef  LIB_STR_CFG_FP_MAX_NBR_DIG_SIG
#define  LIB_STR_CFG_FP_MAX_NBR_DIG_SIG         LIB_STR_FP_MAX_NBR_DIG_SIG_DFLT
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  STR_CR_LF                     "\r\n"
#define  STR_LF_CR                     "\n\r"
#define  STR_NEW_LINE                   STR_CR_LF
#define  STR_PARENT_PATH               ".."

#define  STR_CR_LF_LEN                 (sizeof(STR_CR_LF)       - 1)
#define  STR_LF_CR_LEN                 (sizeof(STR_LF_CR)       - 1)
#define  STR_NEW_LINE_LEN              (sizeof(STR_NEW_LINE)    - 1)
#define  STR_PARENT_PATH_LEN           (sizeof(STR_PARENT_PATH) - 1)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       STANDARD LIBRARY MACRO'S
 *
 * Note(s) : (1) NO compiler-supplied standard library functions SHOULD be used.
 *
 *               #### The reference to standard memory functions SHOULD be removed once all custom library
 *               functions are implemented WITHOUT reference to ANY standard library function(s).
 *
 *               See also 'INCLUDE FILES  Note #1'.
 *******************************************************************************************************/

//                                                                 See Note #1.
#define  Str_FmtPrint                   snprintf
#define  Str_FmtScan                    sscanf

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//                                                                 ------------------ STR LEN  FNCTS ------------------
CPU_SIZE_T Str_Len(const CPU_CHAR *p_str);

CPU_SIZE_T Str_Len_N(const CPU_CHAR *p_str,
                     CPU_SIZE_T     len_max);

//                                                                 ------------------ STR COPY FNCTS ------------------
CPU_CHAR *Str_Copy(CPU_CHAR       *p_str_dest,
                   const CPU_CHAR *p_str_src);

CPU_CHAR *Str_Copy_N(CPU_CHAR       *p_str_dest,
                     const CPU_CHAR *p_str_src,
                     CPU_SIZE_T     len_max);

CPU_CHAR *Str_Cat(CPU_CHAR       *p_str_dest,
                  const CPU_CHAR *p_str_cat);

CPU_CHAR *Str_Cat_N(CPU_CHAR       *p_str_dest,
                    const CPU_CHAR *p_str_cat,
                    CPU_SIZE_T     len_max);

//                                                                 ------------------ STR CMP  FNCTS ------------------
CPU_INT16S Str_Cmp(const CPU_CHAR *p1_str,
                   const CPU_CHAR *p2_str);

CPU_INT16S Str_Cmp_N(const CPU_CHAR *p1_str,
                     const CPU_CHAR *p2_str,
                     CPU_SIZE_T     len_max);

CPU_INT16S Str_CmpIgnoreCase(const CPU_CHAR *p1_str,
                             const CPU_CHAR *p2_str);

CPU_INT16S Str_CmpIgnoreCase_N(const CPU_CHAR *p1_str,
                               const CPU_CHAR *p2_str,
                               CPU_SIZE_T     len_max);

//                                                                 ------------------ STR SRCH FNCTS ------------------
CPU_CHAR *Str_Char(const CPU_CHAR *p_str,
                   CPU_CHAR       srch_char);

CPU_CHAR *Str_Char_N(const CPU_CHAR *p_str,
                     CPU_SIZE_T     len_max,
                     CPU_CHAR       srch_char);

CPU_CHAR *Str_Char_Last(const CPU_CHAR *p_str,
                        CPU_CHAR       srch_char);

CPU_CHAR *Str_Char_Last_N(const CPU_CHAR *p_str,
                          CPU_SIZE_T     len_max,
                          CPU_CHAR       srch_char);

CPU_CHAR *Str_Char_Replace(CPU_CHAR *p_str,
                           CPU_CHAR char_srch,
                           CPU_CHAR char_replace);

CPU_CHAR *Str_Char_Replace_N(CPU_CHAR   *p_str,
                             CPU_CHAR   char_srch,
                             CPU_CHAR   char_replace,
                             CPU_SIZE_T len_max);

CPU_CHAR *Str_Str(const CPU_CHAR *p_str,
                  const CPU_CHAR *p_str_srch);

CPU_CHAR *Str_Str_N(const CPU_CHAR *p_str,
                    const CPU_CHAR *p_str_srch,
                    CPU_SIZE_T     len_max);

//                                                                 ------------------ STR FMT  FNCTS ------------------
CPU_CHAR *Str_FmtNbr_Int32U(CPU_INT32U  nbr,
                            CPU_INT08U  nbr_dig,
                            CPU_INT08U  nbr_base,
                            CPU_CHAR    lead_char,
                            CPU_BOOLEAN lower_case,
                            CPU_BOOLEAN nul,
                            CPU_CHAR    *p_str);

CPU_CHAR *Str_FmtNbr_Int32S(CPU_INT32S  nbr,
                            CPU_INT08U  nbr_dig,
                            CPU_INT08U  nbr_base,
                            CPU_CHAR    lead_char,
                            CPU_BOOLEAN lower_case,
                            CPU_BOOLEAN nul,
                            CPU_CHAR    *p_str);

#if (LIB_STR_CFG_FP_EN == DEF_ENABLED)
CPU_CHAR *Str_FmtNbr_32(CPU_FP32    nbr,
                        CPU_INT08U  nbr_dig,
                        CPU_INT08U  nbr_dp,
                        CPU_CHAR    lead_char,
                        CPU_BOOLEAN nul,
                        CPU_CHAR    *p_str);
#endif

//                                                                 ----------------- STR PARSE FNCTS ------------------
CPU_INT32U Str_ParseNbr_Int32U(const CPU_CHAR *p_str,
                               CPU_CHAR       **p_str_next,
                               CPU_INT08U     nbr_base);

CPU_INT32S Str_ParseNbr_Int32S(const CPU_CHAR *p_str,
                               CPU_CHAR       **p_str_next,
                               CPU_INT08U     nbr_base);

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  LIB_STR_CFG_FP_EN
#error  "LIB_STR_CFG_FP_EN not #define'd in 'common_cfg.h'. MUST be DEF_DISABLED or DEF_ENABLED."
#elif  ((LIB_STR_CFG_FP_EN != DEF_DISABLED) \
  && (LIB_STR_CFG_FP_EN != DEF_ENABLED))
#error  "LIB_STR_CFG_FP_EN illegally #define'd in 'common_cfg.h'. MUST be DEF_DISABLED or DEF_ENABLED."
#elif   (LIB_STR_CFG_FP_EN == DEF_ENABLED)

#ifndef  LIB_STR_CFG_FP_MAX_NBR_DIG_SIG
#error  "LIB_STR_CFG_FP_MAX_NBR_DIG_SIG not #define'd in 'common_cfg.h'. MUST be >= LIB_STR_FP_MAX_NBR_DIG_SIG_MIN and <= LIB_STR_FP_MAX_NBR_DIG_SIG_MAX."
#elif   (DEF_CHK_VAL(LIB_STR_CFG_FP_MAX_NBR_DIG_SIG, \
                     LIB_STR_FP_MAX_NBR_DIG_SIG_MIN, \
                     LIB_STR_FP_MAX_NBR_DIG_SIG_MAX) != DEF_OK)
#error  "LIB_STR_CFG_FP_MAX_NBR_DIG_SIG illegally #define'd in 'common_cfg.h'. MUST be >= LIB_STR_FP_MAX_NBR_DIG_SIG_MIN and <= LIB_STR_FP_MAX_NBR_DIG_SIG_MAX."
#endif

#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of lib str module include.
