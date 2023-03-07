/***************************************************************************//**
 * @file
 * @brief Core CPU Module
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
 * @defgroup CPU CPU API
 * @brief      CPU API
 *
 * @defgroup CPU_CORE CPU Core API
 * @ingroup CPU
 * @brief      CPU Core API
 *
 * @addtogroup CPU_CORE
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _CPU_H_
#define  _CPU_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   EXTERNS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef   CPU_CORE_MODULE
#define  CPU_CORE_EXT
#else
#define  CPU_CORE_EXT  extern
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "cpu_def.h"

#include  <common/include/rtos_path.h>
#include  <cpu_cfg.h>

#include  "cpu_port_sel.h"

#include  <common/include/rtos_err.h>

#include  <common/include/lib_def.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           CPU CONFIGURATION
 *
 * Note(s) : (1) The following pre-processor directives correctly configure CPU parameters. DO NOT MODIFY.
 *
 *           (2) CPU timestamp timer feature is required for :
 *
 *               (a) CPU timestamps
 *               (b) CPU interrupts disabled time measurement
 *
 *               See also 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
 *                       & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1'.
 *******************************************************************************************************/

#ifdef   CPU_CFG_TS_EN
#undef   CPU_CFG_TS_EN
#endif

#if    ((CPU_CFG_TS_32_EN == DEF_ENABLED) \
  || (CPU_CFG_TS_64_EN == DEF_ENABLED))
#define  CPU_CFG_TS_EN                          DEF_ENABLED
#else
#define  CPU_CFG_TS_EN                          DEF_DISABLED
#endif

#if    (CPU_CFG_TS_EN == DEF_ENABLED)
#define  CPU_CFG_TS_TMR_EN                      DEF_ENABLED
#else
#define  CPU_CFG_TS_TMR_EN                      DEF_DISABLED
#endif

/********************************************************************************************************
 *                                           CACHE CONFIGURATION
 *
 * Note(s) : (1) The following pre-processor directives correctly configure CACHE parameters. DO NOT MODIFY.
 *******************************************************************************************************/

#ifndef CPU_CFG_CACHE_MGMT_EN
#define CPU_CFG_CACHE_MGMT_EN       DEF_DISABLED
#endif

/********************************************************************************************************
 *                                               GENERAL DEFINES
 *******************************************************************************************************/

#define  CPU_TIME_MEAS_NBR_MIN                             1u
#define  CPU_TIME_MEAS_NBR_MAX                           128u

//                                                                 --------------- CPU INTEGER DEFINES ----------------
#define  DEF_INT_CPU_NBR_BITS                           (CPU_CFG_DATA_SIZE     * DEF_OCTET_NBR_BITS)
#define  DEF_INT_CPU_NBR_BITS_MAX                       (CPU_CFG_DATA_SIZE_MAX * DEF_OCTET_NBR_BITS)

#if     (DEF_INT_CPU_NBR_BITS == DEF_INT_08_NBR_BITS)

#define  DEF_INT_CPU_MASK                                DEF_INT_08_MASK

#define  DEF_INT_CPU_U_MIN_VAL                           DEF_INT_08U_MIN_VAL
#define  DEF_INT_CPU_U_MAX_VAL                           DEF_INT_08U_MAX_VAL

#define  DEF_INT_CPU_S_MIN_VAL                           DEF_INT_08S_MIN_VAL
#define  DEF_INT_CPU_S_MAX_VAL                           DEF_INT_08S_MAX_VAL

#define  DEF_INT_CPU_S_MIN_VAL_ONES_CPL                  DEF_INT_08S_MIN_VAL_ONES_CPL
#define  DEF_INT_CPU_S_MAX_VAL_ONES_CPL                  DEF_INT_08S_MAX_VAL_ONES_CPL

#elif   (DEF_INT_CPU_NBR_BITS == DEF_INT_16_NBR_BITS)

#define  DEF_INT_CPU_MASK                                DEF_INT_16_MASK

#define  DEF_INT_CPU_U_MIN_VAL                           DEF_INT_16U_MIN_VAL
#define  DEF_INT_CPU_U_MAX_VAL                           DEF_INT_16U_MAX_VAL

#define  DEF_INT_CPU_S_MIN_VAL                           DEF_INT_16S_MIN_VAL
#define  DEF_INT_CPU_S_MAX_VAL                           DEF_INT_16S_MAX_VAL

#define  DEF_INT_CPU_S_MIN_VAL_ONES_CPL                  DEF_INT_16S_MIN_VAL_ONES_CPL
#define  DEF_INT_CPU_S_MAX_VAL_ONES_CPL                  DEF_INT_16S_MAX_VAL_ONES_CPL

#elif   (DEF_INT_CPU_NBR_BITS == DEF_INT_32_NBR_BITS)

#define  DEF_INT_CPU_MASK                                DEF_INT_32_MASK

#define  DEF_INT_CPU_U_MIN_VAL                           DEF_INT_32U_MIN_VAL
#define  DEF_INT_CPU_U_MAX_VAL                           DEF_INT_32U_MAX_VAL

#define  DEF_INT_CPU_S_MIN_VAL                           DEF_INT_32S_MIN_VAL
#define  DEF_INT_CPU_S_MAX_VAL                           DEF_INT_32S_MAX_VAL

#define  DEF_INT_CPU_S_MIN_VAL_ONES_CPL                  DEF_INT_32S_MIN_VAL_ONES_CPL
#define  DEF_INT_CPU_S_MAX_VAL_ONES_CPL                  DEF_INT_32S_MAX_VAL_ONES_CPL

#elif   (DEF_INT_CPU_NBR_BITS == DEF_INT_64_NBR_BITS)

#define  DEF_INT_CPU_MASK                                DEF_INT_64_MASK

#define  DEF_INT_CPU_U_MIN_VAL                           DEF_INT_64U_MIN_VAL
#define  DEF_INT_CPU_U_MAX_VAL                           DEF_INT_64U_MAX_VAL

#define  DEF_INT_CPU_S_MIN_VAL                           DEF_INT_64S_MIN_VAL
#define  DEF_INT_CPU_S_MAX_VAL                           DEF_INT_64S_MAX_VAL

#define  DEF_INT_CPU_S_MIN_VAL_ONES_CPL                  DEF_INT_64S_MIN_VAL_ONES_CPL
#define  DEF_INT_CPU_S_MAX_VAL_ONES_CPL                  DEF_INT_64S_MAX_VAL_ONES_CPL

#else

#error  "CPU_CFG_DATA_SIZE illegally #defined in '[arch]_cpu_port.h'. [See '[arch]_cpu_port.h  CONFIGURATION ERRORS']."

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       CPU TIMESTAMP DATA TYPES
 *
 * Note(s) : (1) CPU timestamp timer data type defined to the binary-multiple of 8-bit octets as configured
 *               by 'CPU_CFG_TS_TMR_SIZE' (see 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #2').
 *******************************************************************************************************/

typedef CPU_INT32U CPU_TS32;
typedef CPU_INT64U CPU_TS64;

typedef CPU_TS32 CPU_TS;                                        // Req'd for backwards-compatibility.

#if     (CPU_CFG_TS_TMR_EN == DEF_ENABLED)                      // CPU ts tmr defined to cfg'd word size (see Note #1).
#if     (CPU_CFG_TS_TMR_SIZE == CPU_WORD_SIZE_08)
typedef CPU_INT08U CPU_TS_TMR;
#elif   (CPU_CFG_TS_TMR_SIZE == CPU_WORD_SIZE_16)
typedef CPU_INT16U CPU_TS_TMR;
#elif   (CPU_CFG_TS_TMR_SIZE == CPU_WORD_SIZE_64)
typedef CPU_INT64U CPU_TS_TMR;
#else //                                                           CPU ts tmr dflt size = 32-bits.
typedef CPU_INT32U CPU_TS_TMR;
#endif
#endif

/********************************************************************************************************
 *                                   CPU TIMESTAMP TIMER FREQUENCY DATA TYPE
 *******************************************************************************************************/

typedef CPU_INT32U CPU_TS_TMR_FREQ;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if    (CPU_CFG_NAME_EN == DEF_ENABLED)
CPU_CORE_EXT CPU_CHAR CPU_Name[CPU_CFG_NAME_SIZE];              ///< CPU host name.
#endif

#if ((CPU_CFG_TS_32_EN == DEF_ENABLED) \
  && (CPU_CFG_TS_TMR_SIZE < CPU_WORD_SIZE_32))
CPU_CORE_EXT CPU_TS32   CPU_TS_32_Accum;                        ///< 32-bit accum'd ts  (in ts tmr cnts).
CPU_CORE_EXT CPU_TS_TMR CPU_TS_32_TmrPrev;                      ///< 32-bit ts prev tmr (in ts tmr cnts).
#endif

#if ((CPU_CFG_TS_64_EN == DEF_ENABLED) \
  && (CPU_CFG_TS_TMR_SIZE < CPU_WORD_SIZE_64))
CPU_CORE_EXT CPU_TS64   CPU_TS_64_Accum;                        ///< 64-bit accum'd ts  (in ts tmr cnts).
CPU_CORE_EXT CPU_TS_TMR CPU_TS_64_TmrPrev;                      ///< 64-bit ts prev tmr (in ts tmr cnts).
#endif

#if  (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
CPU_CORE_EXT CPU_TS_TMR_FREQ CPU_TS_TmrFreq_Hz;                 ///< CPU ts tmr freq (in Hz).
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               CPU_BREAK()
 *
 * @brief    Create software generated break.
 *
 * @note     (1) CPU_BREAK() is used to create a CPU break.
 *******************************************************************************************************/

#ifndef  CPU_BREAK                                              // See Note #1.
#define  CPU_BREAK() \
  do {               \
    CPU_Break();     \
  } while (0)
#endif

/****************************************************************************************************//**
 *                                       CPU_MIN_DATA_ALIGN_BYTES()
 *
 * @brief    Obtain the minimum data alignment, in bytes.
 *
 * @return   Minimum data alignment, in bytes, required by the CPU.
 *
 * @note     (1) CPU_MIN_DATA_ALIGN_BYTES() can be implemented as a function in the port if the
 *               minimum data alignment is port-specific.
 *******************************************************************************************************/

#ifndef  CPU_MIN_DATA_ALIGN_BYTES                               // See Note #1.
#define  CPU_MIN_DATA_ALIGN_BYTES()             sizeof(CPU_ALIGN)
#endif

/****************************************************************************************************//**
 *                                           CPU_SW_EXCEPTION()
 *
 * @brief    Trap unrecoverable software exception.
 *
 * @param    err_rtn_val     Error type &/or value of the calling function to return.
 *
 * @note     (1) CPU_SW_EXCEPTION() deadlocks the current code execution -- whether multi-tasked/
 *               -processed/-threaded or single-threaded -- when the current code execution cannot
 *               gracefully recover or report a fault or exception condition.
 *                      @n
 *               Example CPU_SW_EXCEPTION() call :
 *                      @verbatim
 *                   void  Fnct (RTOS_ERR  *p_err)
 *                   {
 *                       :
 *
 *                       if (p_err == (RTOS_ERR *)0) {       If 'p_err' NULL, cannot return error ...
 *                           CPU_SW_EXCEPTION(;);            ... so trap invalid argument exception.
 *                       }
 *
 *                       :
 *                   }
 *                      @endverbatim
 *               See also 'cpu_core.c  CPU_SW_Exception()  Note #1'.
 *
 * @note     (2) CPU_SW_EXCEPTION()  MAY be developer-implemented to output &/or handle any error or
 *               exception conditions; but since CPU_SW_EXCEPTION() is intended to trap unrecoverable
 *               software  conditions, it is recommended that developer-implemented versions prevent
 *               execution of any code following calls to CPU_SW_EXCEPTION() by deadlocking the code
 *               (see Note #1).
 *                              @n
 *                   Example CPU_SW_EXCEPTION() :
 *                              @verbatim
 *                       #define  CPU_SW_EXCEPTION(err_rtn_val)      do {                         \
 *                                                                           Log(__FILE__, __LINE__); \
 *                                                                           CPU_SW_Exception();      \
 *                                                                       } while (0)
 *                              @endverbatim
 *
 *           - (a) However, if execution of code following calls to CPU_SW_EXCEPTION() is required
 *                   (e.g. for automated testing); it is recommended that the last statement in
 *                   developer-implemented versions be to return from the current function to prevent
 *                   possible software exception(s) in the current function from triggering CPU &/or
 *                   hardware exception(s).
 *                                      @n
 *                       Example CPU_SW_EXCEPTION() :
 *                                      @verbatim
 *                           #define  CPU_SW_EXCEPTION(err_rtn_val)      do {                         \
 *                                                                           Log(__FILE__, __LINE__); \
 *                                                                           return  err_rtn_val;     \
 *                                                                       } while (0)
 *                                      @endverbatim
 *               - (1) Note that 'err_rtn_val' in the return statement MUST NOT be enclosed in
 *                       parentheses. This allows CPU_SW_EXCEPTION() to return from functions that
 *                       return 'void', i.e. NO return type or value (see also Note #2b2A).
 *           - (b) In order for CPU_SW_EXCEPTION() to return from functions with various return
 *                   types/values, each caller function MUST pass an appropriate error return type
 *                   & value to CPU_SW_EXCEPTION().
 *               - (1) Note that CPU_SW_EXCEPTION()  MUST NOT be passed any return type or value
 *                       for functions that return 'void', i.e. NO return type or value; but SHOULD
 *                       instead be passed a single semicolon. This prevents possible compiler
 *                       warnings that CPU_SW_EXCEPTION() is passed too few arguments. However,
 *                       the compiler may warn that CPU_SW_EXCEPTION() does NOT prevent creating
 *                       null statements on lines with NO other code statements.
 *                                      @n
 *                       Example CPU_SW_EXCEPTION() calls :
 *                                      @verbatim
 *                           void  Fnct (RTOS_ERR  *p_err)
 *                           {
 *                               :
 *
 *                               if (p_err == (RTOS_ERR *)0) {
 *                                   CPU_SW_EXCEPTION(;);            Exception macro returns NO value
 *                               }                                       (see Note #2b2A)
 *
 *                               :
 *                           }
 *
 *                           CPU_BOOLEAN  Fnct (RTOS_ERR  *p_err)
 *                           {
 *                               :
 *
 *                               if (p_err == (RTOS_ERR *)0) {
 *                                   CPU_SW_EXCEPTION(DEF_FAIL);     Exception macro returns 'DEF_FAIL'
 *                               }
 *
 *                               :
 *                           }
 *
 *                           OBJ  *Fnct (RTOS_ERR  *p_err)
 *                           {
 *                               :
 *
 *                               if (p_err == (RTOS_ERR *)0) {
 *                                   CPU_SW_EXCEPTION((OBJ *)0);     Exception macro returns NULL 'OBJ *'
 *                               }
 *
 *                               :
 *                           }
 *                                              @endverbatim
 *******************************************************************************************************/

#ifndef  CPU_SW_EXCEPTION                                       // See Note #2.
#define  CPU_SW_EXCEPTION(err_rtn_val) \
  do {                                 \
    CPU_SW_Exception();                \
  } while (0)
#endif

/****************************************************************************************************//**
 *                                               CPU_TYPE_CREATE()
 *
 * @brief    Creates a generic type value.
 *
 * @param    char_1  1st ASCII character to create generic type value.
 *
 * @param    char_2  2nd ASCII character to create generic type value.
 *
 * @param    char_3  3rd ASCII character to create generic type value.
 *
 * @param    char_4  4th ASCII character to create generic type value.
 *
 * @return   32-bit generic type value.
 *
 * @note     (1) Generic type values should be #define'd with large, non-trivial values to trap
 *               & discard invalid/corrupted objects based on type value.
 *                      @n
 *               In other words, by assigning large, non-trivial values to valid objects' type
 *               fields; the likelihood that an object with an unassigned &/or corrupted type
 *               field will contain a value is highly improbable & therefore the object itself
 *               will be trapped as invalid.
 *                      @n
 *               CPU_TYPE_CREATE()  creates a 32-bit type value from four values.
 *           - (a) Ideally, generic type values SHOULD be created from 'CPU_CHAR' characters to
 *                   represent ASCII string abbreviations of the specific object types. Memory
 *                   displays of object type values will display the specific object types with
 *                   their chosen ASCII names.
 *                   Examples :
 *                              @verbatim
 *                       #define  FILE_TYPE  CPU_TYPE_CREATE('F', 'I', 'L', 'E')
 *                       #define  BUF_TYPE   CPU_TYPE_CREATE('B', 'U', 'F', ' ')
 *                                      @endverbatim
 * @{
 *******************************************************************************************************/

#if     (CPU_CFG_ENDIAN_TYPE == CPU_ENDIAN_TYPE_BIG)
#define  CPU_TYPE_CREATE(char_1, char_2, char_3, char_4)        (((CPU_INT32U)((CPU_INT08U)(char_1)) << (3u * DEF_OCTET_NBR_BITS))   \
                                                                 | ((CPU_INT32U)((CPU_INT08U)(char_2)) << (2u * DEF_OCTET_NBR_BITS)) \
                                                                 | ((CPU_INT32U)((CPU_INT08U)(char_3)) << (1u * DEF_OCTET_NBR_BITS)) \
                                                                 | ((CPU_INT32U)((CPU_INT08U)(char_4))))

#else

#if    ((CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_64) \
  || (CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_32))
#define  CPU_TYPE_CREATE(char_1, char_2, char_3, char_4)        (((CPU_INT32U)((CPU_INT08U)(char_1)))                                \
                                                                 | ((CPU_INT32U)((CPU_INT08U)(char_2)) << (1u * DEF_OCTET_NBR_BITS)) \
                                                                 | ((CPU_INT32U)((CPU_INT08U)(char_3)) << (2u * DEF_OCTET_NBR_BITS)) \
                                                                 | ((CPU_INT32U)((CPU_INT08U)(char_4)) << (3u * DEF_OCTET_NBR_BITS)))

#elif   (CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_16)
#define  CPU_TYPE_CREATE(char_1, char_2, char_3, char_4)        (((CPU_INT32U)((CPU_INT08U)(char_1)) << (2u * DEF_OCTET_NBR_BITS))   \
                                                                 | ((CPU_INT32U)((CPU_INT08U)(char_2)) << (3u * DEF_OCTET_NBR_BITS)) \
                                                                 | ((CPU_INT32U)((CPU_INT08U)(char_3)))                              \
                                                                 | ((CPU_INT32U)((CPU_INT08U)(char_4)) << (1u * DEF_OCTET_NBR_BITS)))

#else //                                                           Dflt CPU_WORD_SIZE_08.
#define  CPU_TYPE_CREATE(char_1, char_2, char_3, char_4)        (((CPU_INT32U)((CPU_INT08U)(char_1)) << (3u * DEF_OCTET_NBR_BITS))   \
                                                                 | ((CPU_INT32U)((CPU_INT08U)(char_2)) << (2u * DEF_OCTET_NBR_BITS)) \
                                                                 | ((CPU_INT32U)((CPU_INT08U)(char_3)) << (1u * DEF_OCTET_NBR_BITS)) \
                                                                 | ((CPU_INT32U)((CPU_INT08U)(char_4))))
#endif
#endif

///< @}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 *
 * Note(s) : (2) (a) CPU_CntLeadZeros()  defined in :
 *
 *                   (1) 'cpu_a.asm',  if CPU_CFG_LEAD_ZEROS_ASM_PRESENT       #define'd in '[arch]_cpu_port.h'
 *                                           to enable assembly-optimized function(s)
 *
 *                   (2) 'cpu_core.c', if CPU_CFG_LEAD_ZEROS_ASM_PRESENT   NOT #define'd in '[arch]_cpu_port.h'
 *                                           to enable C-source-optimized function(s)
 *
 *               (b) CPU_CntTrailZeros() defined in :
 *
 *                   (1) 'cpu_a.asm',  if CPU_CFG_TRAIL_ZEROS_ASM_PRESENT      #define'd in '[arch]_cpu_port.h'
 *                                           to enable assembly-optimized function(s)
 *
 *                   (2) 'cpu_core.c', if CPU_CFG_TRAIL_ZEROS_ASM_PRESENT  NOT #define'd in '[arch]_cpu_port.h'
 *                                           to enable C-source-optimized function(s)
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void CPU_Init(void);

void CPU_Break(void);

void CPU_SW_Exception(void);

#if (CPU_CFG_NAME_EN == DEF_ENABLED)                            // ------------------ CPU NAME FNCTS ------------------
void CPU_NameClr(void);

void CPU_NameGet(CPU_CHAR *p_name,
                 RTOS_ERR *p_err);

void CPU_NameSet(const CPU_CHAR *p_name,
                 RTOS_ERR       *p_err);
#endif

//                                                                 ------------------- CPU TS FNCTS -------------------
#if (CPU_CFG_TS_32_EN == DEF_ENABLED)
CPU_TS32 CPU_TS_Get32(void);
#endif

#if (CPU_CFG_TS_64_EN == DEF_ENABLED)
CPU_TS64 CPU_TS_Get64(void);
#endif

#if (CPU_CFG_TS_EN == DEF_ENABLED)
void CPU_TS_Update(void);
#endif

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)                          // ----------------- CPU TS TMR FNCTS -----------------
CPU_TS_TMR_FREQ CPU_TS_TmrFreqGet(RTOS_ERR *p_err);

void CPU_TS_TmrFreqSet(CPU_TS_TMR_FREQ freq_hz);
#endif

//                                                                 --------------- CPU CNT ZEROS FNCTS ----------------
CPU_DATA CPU_CntLeadZeros(CPU_DATA val);

#if     (CPU_CFG_DATA_SIZE_MAX >= CPU_WORD_SIZE_08)
CPU_DATA CPU_CntLeadZeros08(CPU_INT08U val);
#endif
#if     (CPU_CFG_DATA_SIZE_MAX >= CPU_WORD_SIZE_16)
CPU_DATA CPU_CntLeadZeros16(CPU_INT16U val);
#endif
#if     (CPU_CFG_DATA_SIZE_MAX >= CPU_WORD_SIZE_32)
CPU_DATA CPU_CntLeadZeros32(CPU_INT32U val);
#endif
#if     (CPU_CFG_DATA_SIZE_MAX >= CPU_WORD_SIZE_64)
CPU_DATA CPU_CntLeadZeros64(CPU_INT64U val);
#endif

CPU_DATA CPU_CntTrailZeros(CPU_DATA val);

#if     (CPU_CFG_DATA_SIZE_MAX >= CPU_WORD_SIZE_08)
CPU_DATA CPU_CntTrailZeros08(CPU_INT08U val);
#endif
#if     (CPU_CFG_DATA_SIZE_MAX >= CPU_WORD_SIZE_16)
CPU_DATA CPU_CntTrailZeros16(CPU_INT16U val);
#endif
#if     (CPU_CFG_DATA_SIZE_MAX >= CPU_WORD_SIZE_32)
CPU_DATA CPU_CntTrailZeros32(CPU_INT32U val);
#endif
#if     (CPU_CFG_DATA_SIZE_MAX >= CPU_WORD_SIZE_64)
CPU_DATA CPU_CntTrailZeros64(CPU_INT64U val);
#endif

CPU_INT08U CPU_PopCnt32(CPU_INT32U value);

CPU_DATA CPU_RevBits(CPU_DATA val);                             // C-code replacement for asm function

/********************************************************************************************************
 ********************************************************************************************************
 *                                   BSP SPECIFIC FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               CPU_TS_TmrInit()
 *
 * @brief    Initialize & start CPU timestamp timer.
 *
 * @note     (1) CPU_TS_TmrInit() is an application/BSP function that MUST be defined by the developer
 *               if either of the following CPU features is enabled :
 *               See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
 *               & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
 *           - (a) CPU timestamps
 *           - (b) CPU interrupts disabled time measurements
 *           - (c) Timer count values MUST be returned via word-size-configurable 'CPU_TS_TMR'
 *                   data type.
 *                   See also 'cpu_cfg.h   CPU TIMESTAMP CONFIGURATION  Note #2'
 *                           & 'cpu_core.h  CPU TIMESTAMP DATA TYPES     Note #1'.
 *               - (1) If timer has more bits, truncate timer values' higher-order bits greater
 *                       than the configured 'CPU_TS_TMR' timestamp timer data type word size.
 *               - (2) Since the timer MUST NOT have less bits than the configured 'CPU_TS_TMR'
 *                       timestamp timer data type word size; 'CPU_CFG_TS_TMR_SIZE' MUST be
 *                       configured so that ALL bits in 'CPU_TS_TMR' data type are significant.
 *                                      @n
 *                       In other words, if timer size is not a binary-multiple of 8-bit octets
 *                       (e.g. 20-bits or even 24-bits), then the next lower, binary-multiple
 *                       octet word size SHOULD be configured (e.g. to 16-bits). However, the
 *                       minimum supported word size for CPU timestamp timers is 8-bits.
 *           - (d) Timer SHOULD be an 'up'  counter whose values increase with each time count.
 *           - (e) When applicable, timer period SHOULD be less than the typical measured time
 *                   but MUST be less than the maximum measured time; otherwise, timer resolution
 *                   inadequate to measure desired times.
 *               See also 'CPU_TS_TmrRd()  Note #2'.
 *
 * @note     (2) This function is an INTERNAL CPU module function and your application MUST NOT call it.
 *******************************************************************************************************/

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
void CPU_TS_TmrInit(void);
#endif

/****************************************************************************************************//**
 *                                               CPU_TS_TmrRd()
 *
 * @brief    Get current CPU timestamp timer count value.
 *
 * @return   Timestamp timer count (see Notes #1c & #1d).
 *
 * @note     (1) CPU_TS_TmrRd() is an application/BSP function that MUST be defined by the developer
 *               if either of the following CPU features is enabled :
 *               See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
 *               & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
 *           - (a) CPU timestamps
 *           - (b) CPU interrupts disabled time measurements
 *           - (c) Timer count values MUST be returned via word-size-configurable 'CPU_TS_TMR'
 *                   data type.
 *                   See also 'cpu_cfg.h   CPU TIMESTAMP CONFIGURATION  Note #2'
 *                           & 'cpu_core.h  CPU TIMESTAMP DATA TYPES     Note #1'.
 *               - (1) If timer has more bits, truncate timer values' higher-order bits greater
 *                       than the configured 'CPU_TS_TMR' timestamp timer data type word size.
 *               - (2) Since the timer MUST NOT have less bits than the configured 'CPU_TS_TMR'
 *                       timestamp timer data type word size; 'CPU_CFG_TS_TMR_SIZE' MUST be
 *                       configured so that ALL bits in 'CPU_TS_TMR' data type are significant.
 *                       In other words, if timer size is not a binary-multiple of 8-bit octets
 *                       (e.g. 20-bits or even 24-bits), then the next lower, binary-multiple
 *                       octet word size SHOULD be configured (e.g. to 16-bits). However, the
 *                       minimum supported word size for CPU timestamp timers is 8-bits.
 *           - (d) Timer SHOULD be an 'up'  counter whose values increase with each time count.
 *               - (1) If timer is a 'down' counter whose values decrease with each time count,
 *                       then the returned timer value MUST be ones-complemented.
 *           - (e) When applicable, the amount of time measured by CPU timestamps is
 *                   calculated by either of the following equations :
 *                              @verbatim
 *               - (1) Time measured  =  Number timer counts  *  Timer period
 *
 *                           where
 *
 *                               Number timer counts     Number of timer counts measured
 *                               Timer period            Timer's period in some units of
 *                                                           (fractional) seconds
 *                               Time measured           Amount of time measured, in same
 *                                                           units of (fractional) seconds
 *                                                           as the Timer period
 *
 *                                           Number timer counts
 *               - (2) Time measured  =  ---------------------
 *                                           Timer frequency
 *
 *                           where
 *
 *                               Number timer counts     Number of timer counts measured
 *                               Timer frequency         Timer's frequency in some units
 *                                                           of counts per second
 *                               Time measured           Amount of time measured, in seconds
 *                      @endverbatim
 *               Timer period SHOULD be less than the typical measured time but MUST be less
 *               than the maximum measured time; otherwise, timer resolution inadequate to
 *               measure desired times.
 *
 * @note     (2) This function is an INTERNAL CPU module function and your application MAY call it.
 *******************************************************************************************************/

#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
CPU_TS_TMR CPU_TS_TmrRd(void);
#endif

/****************************************************************************************************//**
 *                                           CPU_TSxx_to_uSec()
 *
 * @brief    Convert a 32-/64-bit CPU timestamp from timer counts to microseconds.
 *
 * @param    ts_cnts     CPU timestamp (in timestamp timer counts [see Note #2aA]).
 *
 * @return   Converted CPU timestamp (in microseconds           [see Note #2aD]).
 *
 * @note     (1) CPU_TS32_to_uSec()/CPU_TS64_to_uSec() are application/BSP functions that MAY be
 *               optionally defined by the developer when either of the following CPU features is
 *               enabled :
 *           - (a) CPU timestamps
 *           - (b) CPU interrupts disabled time measurements
 *               See 'cpu_cfg.h  CPU TIMESTAMP CONFIGURATION  Note #1'
 *               & 'cpu_cfg.h  CPU INTERRUPTS DISABLED TIME MEASUREMENT CONFIGURATION  Note #1a'.
 *
 * @note     (2) (a) The amount of time measured by CPU timestamps is calculated by either of
 *                   the following equations :
 *                      @verbatim
 *                                                                   10^6 microseconds
 *               - (1) Time measured  =   Number timer counts   *  -------------------  *  Timer period
 *                                                                       1 second
 *
 *                                           Number timer counts       10^6 microseconds
 *               - (2) Time measured  =  ---------------------  *  -------------------
 *                                           Timer frequency             1 second
 *
 *                           where
 *
 *                           - (A) Number timer counts     Number of timer counts measured
 *                           - (B) Timer frequency         Timer's frequency in some units
 *                                                               of counts per second
 *                           - (C) Timer period            Timer's period in some units of
 *                                                               (fractional)  seconds
 *                           - (D) Time measured           Amount of time measured,
 *                                                               in microseconds
 *                      @endverbatim
 *           - (b) Timer period SHOULD be less than the typical measured time but MUST be less
 *                   than the maximum measured time; otherwise, timer resolution inadequate to
 *                   measure desired times.
 *           - (c) Specific implementations may convert any number of CPU_TS32 or CPU_TS64 bits
 *                   -- up to 32 or 64, respectively -- into microseconds.
 *******************************************************************************************************/

#if (CPU_CFG_TS_32_EN == DEF_ENABLED)
CPU_INT64U CPU_TS32_to_uSec(CPU_TS32 ts_cnts);
#endif

#if (CPU_CFG_TS_64_EN == DEF_ENABLED)
CPU_INT64U CPU_TS64_to_uSec(CPU_TS64 ts_cnts);
#endif

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  CPU_CFG_NAME_EN
#error  "CPU_CFG_NAME_EN not #define'd in 'cpu_cfg.h'. MUST be DEF_ENABLED || DEF_DISABLED."

#elif  ((CPU_CFG_NAME_EN != DEF_ENABLED) \
  && (CPU_CFG_NAME_EN != DEF_DISABLED))
#error  "CPU_CFG_NAME_EN illegally #define'd in 'cpu_cfg.h'. MUST be DEF_ENABLED || DEF_DISABLED."

#elif   (CPU_CFG_NAME_EN == DEF_ENABLED)

#ifndef  CPU_CFG_NAME_SIZE
#error  "CPU_CFG_NAME_SIZE not #define'd in 'cpu_cfg.h'. MUST be >= 1 && <= 255."

#elif   (DEF_CHK_VAL(CPU_CFG_NAME_SIZE, \
                     1,                 \
                     DEF_INT_08U_MAX_VAL) != DEF_OK)
#error  "CPU_CFG_NAME_SIZE illegally #define'd in 'cpu_cfg.h'. MUST be >= 1 && <= 255."
#endif

#endif

#ifndef  CPU_CFG_TS_32_EN
#error  "CPU_CFG_TS_32_EN not #define'd in 'cpu_cfg.h'. MUST be DEF_DISABLED || DEF_ENABLED."

#elif  ((CPU_CFG_TS_32_EN != DEF_DISABLED) \
  && (CPU_CFG_TS_32_EN != DEF_ENABLED))
#error  "CPU_CFG_TS_32_EN illegally #define'd in 'cpu_cfg.h'. MUST be DEF_DISABLED || DEF_ENABLED."

#endif

#ifndef  CPU_CFG_TS_64_EN
#error  "CPU_CFG_TS_64_EN not #define'd in 'cpu_cfg.h'. MUST be DEF_DISABLED || DEF_ENABLED."

#elif  ((CPU_CFG_TS_64_EN != DEF_DISABLED) \
  && (CPU_CFG_TS_64_EN != DEF_ENABLED))
#error  "CPU_CFG_TS_64_EN illegally #define'd in 'cpu_cfg.h'. MUST be DEF_DISABLED || DEF_ENABLED."

#endif

//                                                                 Correctly configured in 'cpu_core.h'; DO NOT MODIFY.
#ifndef  CPU_CFG_TS_EN
#error  "CPU_CFG_TS_EN not #define'd in 'cpu_core.h'. MUST be DEF_DISABLED || DEF_ENABLED."

#elif  ((CPU_CFG_TS_EN != DEF_DISABLED) \
  && (CPU_CFG_TS_EN != DEF_ENABLED))
#error  "CPU_CFG_TS_EN illegally #define'd in 'cpu_core.h'. MUST be DEF_DISABLED || DEF_ENABLED."

#endif

//                                                                 Correctly configured in 'cpu_core.h'; DO NOT MODIFY.
#ifndef  CPU_CFG_TS_TMR_EN
#error  "CPU_CFG_TS_TMR_EN not #define'd in 'cpu_core.h'. MUST be DEF_DISABLED || DEF_ENABLED."

#elif  ((CPU_CFG_TS_TMR_EN != DEF_DISABLED) \
  && (CPU_CFG_TS_TMR_EN != DEF_ENABLED))
#error  "CPU_CFG_TS_TMR_EN illegally #define'd in 'cpu_core.h'. MUST be DEF_DISABLED || DEF_ENABLED."

#elif   (CPU_CFG_TS_TMR_EN == DEF_ENABLED)

#ifndef  CPU_CFG_TS_TMR_SIZE
#error  "CPU_CFG_TS_TMR_SIZE not #define'd in 'cpu_cfg.h'. MUST be CPU_WORD_SIZE_08 for 8-bit timer || CPU_WORD_SIZE_16 for 16-bit timer || CPU_WORD_SIZE_32 for 32-bit timer || CPU_WORD_SIZE_64 for 64-bit timer."

#elif  ((CPU_CFG_TS_TMR_SIZE != CPU_WORD_SIZE_08) \
  && (CPU_CFG_TS_TMR_SIZE != CPU_WORD_SIZE_16)    \
  && (CPU_CFG_TS_TMR_SIZE != CPU_WORD_SIZE_32)    \
  && (CPU_CFG_TS_TMR_SIZE != CPU_WORD_SIZE_64))
#error  "CPU_CFG_TS_TMR_SIZE illegally #define'd in 'cpu_cfg.h'. MUST be CPU_WORD_SIZE_08 for 8-bit timer || CPU_WORD_SIZE_16 for 16-bit timer || CPU_WORD_SIZE_32 for 32-bit timer || CPU_WORD_SIZE_64 for 64-bit timer."
#endif

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       CPU PORT CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  CPU_CFG_ADDR_SIZE
#error  "CPU_CFG_ADDR_SIZE not #define'd in '[arch]_cpu_port.h'"
#endif

#ifndef  CPU_CFG_DATA_SIZE
#error  "CPU_CFG_DATA_SIZE not #define'd in '[arch]_cpu_port.h'"
#endif

#ifndef  CPU_CFG_DATA_SIZE_MAX
#error  "CPU_CFG_DATA_SIZE_MAX not #define'd in '[arch]_cpu_port.h'"
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                           MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of CPU core module include.
