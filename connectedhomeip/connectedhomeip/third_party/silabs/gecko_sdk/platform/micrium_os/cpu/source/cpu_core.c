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

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#define    MICRIUM_SOURCE
#define    CPU_CORE_MODULE

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <cpu/include/cpu.h>

#include  <common/source/rtos/rtos_utils_priv.h>

#if (CPU_CFG_CACHE_MGMT_EN == DEF_ENABLED)
#include  <cpu/include/cpu_cache.h>
#endif

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
#include  <common/include/lib_mem.h>
#include  <common/include/lib_str.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR                 RTOS_CFG_MODULE_CPU

//                                                                 Pop cnt algorithm csts.
#define CRC_UTIL_POPCNT_MASK01010101_32  0x55555555u
#define CRC_UTIL_POPCNT_MASK00110011_32  0x33333333u
#define CRC_UTIL_POPCNT_MASK00001111_32  0x0F0F0F0Fu
#define CRC_UTIL_POPCNT_POWERSOF256_32   0x01010101u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL TABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   CPU COUNT LEAD ZEROs LOOKUP TABLE
 *
 * Note(s) : (1) Index into bit pattern table determines the number of leading zeros in an 8-bit value :
 *
 *                           b07  b06  b05  b04  b03  b02  b01  b00    # Leading Zeros
 *                           ---  ---  ---  ---  ---  ---  ---  ---    ---------------
 *                           1    x    x    x    x    x    x    x            0
 *                           0    1    x    x    x    x    x    x            1
 *                           0    0    1    x    x    x    x    x            2
 *                           0    0    0    1    x    x    x    x            3
 *                           0    0    0    0    1    x    x    x            4
 *                           0    0    0    0    0    1    x    x            5
 *                           0    0    0    0    0    0    1    x            6
 *                           0    0    0    0    0    0    0    1            7
 *                           0    0    0    0    0    0    0    0            8
 *******************************************************************************************************/

#if (!(defined(CPU_CFG_LEAD_ZEROS_ASM_PRESENT)) \
  || (CPU_CFG_DATA_SIZE_MAX > CPU_CFG_DATA_SIZE))
static const CPU_INT08U CPU_CntLeadZerosTbl[256] = {              // Data vals :
// 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
  8u, 7u, 6u, 6u, 5u, 5u, 5u, 5u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, 4u, // 0x00 to 0x0F
  3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, 3u, // 0x10 to 0x1F
  2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, // 0x20 to 0x2F
  2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, // 0x30 to 0x3F
  1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, // 0x40 to 0x4F
  1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, // 0x50 to 0x5F
  1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, // 0x60 to 0x6F
  1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, // 0x70 to 0x7F
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, // 0x80 to 0x8F
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, // 0x90 to 0x9F
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, // 0xA0 to 0xAF
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, // 0xB0 to 0xBF
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, // 0xC0 to 0xCF
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, // 0xD0 to 0xDF
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, // 0xE0 to 0xEF
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u  // 0xF0 to 0xFF
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

CPU_INT32U const CPU_EndiannessTest = 0x12345678LU;             // Variable to test CPU endianness.

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if    (CPU_CFG_NAME_EN == DEF_ENABLED)                         // ------------------ CPU NAME FNCTS ------------------
static void CPU_NameInit(void);
#endif

//                                                                 ------------------- CPU TS FNCTS -------------------
#if   ((CPU_CFG_TS_EN == DEF_ENABLED) \
  || (CPU_CFG_TS_TMR_EN == DEF_ENABLED))
static void CPU_TS_Init(void);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               CPU_Init()
 *
 * @brief    Initialize CPU module :
 *               - (a) Initialize CPU timestamps;
 *               - (b) Initialize CPU interrupts disabled time measurements;
 *               - (c) Initialize CPU host name;
 *               - (d) Initialize CPU cache management;
 *               - (e) Initialize CPU interrupt management.
 *
 * @note     (1) CPU_Init() MUST be called ... :
 *               - (a) ONLY ONCE from a product's application; ...
 *               - (b) BEFORE product's application calls any core CPU module function(s)
 *
 * @note     (2) The following initialization functions MUST be sequenced as follows :
 *               - (a) CPU_TS_Init()           SHOULD precede ALL calls to other CPU timestamp functions.
 *               - (b) CPU_IntDisMeasInit()    SHOULD precede ALL calls to CPU_CRITICAL_ENTER()/CPU_CRITICAL_EXIT()
 *                                               & other CPU interrupts disabled time measurement
 *                                               functions.
 *******************************************************************************************************/
void CPU_Init(void)
{
  //                                                               --------------------- INIT TS ----------------------
#if ((CPU_CFG_TS_EN == DEF_ENABLED) \
  || (CPU_CFG_TS_TMR_EN == DEF_ENABLED))
  CPU_TS_Init();                                                // See Note #2a.
#endif

  //                                                               ------------------ INIT CPU NAME -------------------
#if (CPU_CFG_NAME_EN == DEF_ENABLED)
  CPU_NameInit();
#endif
  //                                                               -------------- INIT CACHE MANAGEMENT ---------------
#if (CPU_CFG_CACHE_MGMT_EN == DEF_ENABLED)
  CPU_Cache_Init();
#endif

#ifdef  CPU_PORT_INIT
  CPU_PortInit();
#endif
}

/****************************************************************************************************//**
 *                                               CPU_Break()
 *
 * @brief    If the port does not define a CPU_Break() function, emulate the break with a software
 *           exception equivalent.
 *
 * @note     (1) See 'cpu.h  CPU_BREAK()  Note #1'.
 *******************************************************************************************************/
void CPU_Break(void)
{
  while (DEF_ON) {
    ;
  }
}

/****************************************************************************************************//**
 *                                           CPU_SW_Exception()
 *
 * @brief    Trap unrecoverable software exception.
 *
 * @note     (1) CPU_SW_Exception() deadlocks the current code execution -- whether multi-tasked/
 *               -processed/-threaded or single-threaded -- when the current code execution cannot
 *               gracefully recover or report a fault or exception condition.
 *               See also 'cpu.h  CPU_SW_EXCEPTION()  Note #1'.
 *******************************************************************************************************/
void CPU_SW_Exception(void)
{
  while (DEF_ON) {
    ;
  }
}

/****************************************************************************************************//**
 *                                               CPU_NameClr()
 *
 * @brief    Clear CPU Name.
 *******************************************************************************************************/

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
void CPU_NameClr(void)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  Mem_Clr((void *)&CPU_Name[0],
          (CPU_SIZE_T) CPU_CFG_NAME_SIZE);
  CORE_EXIT_ATOMIC();
}
#endif

/****************************************************************************************************//**
 *                                               CPU_NameGet()
 *
 * @brief    Get CPU host name.
 *
 * @param    p_name  Pointer to an ASCII character array that will receive the return CPU host
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from this function:
 *                       - RTOS_ERR_NONE
 *
 * @note     (1) The size of the ASCII character array that will receive the return CPU host name
 *               ASCII string :
 *               - (a) MUST   be greater than or equal to the current CPU host name's ASCII string
 *                               size including the terminating NULL character;
 *               - (b) SHOULD be greater than or equal to CPU_CFG_NAME_SIZE
 *******************************************************************************************************/

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
void CPU_NameGet(CPU_CHAR *p_name,
                 RTOS_ERR *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_name != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  CORE_ENTER_ATOMIC();
  (void)Str_Copy_N(p_name,
                   &CPU_Name[0],
                   CPU_CFG_NAME_SIZE);
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
}
#endif

/****************************************************************************************************//**
 *                                               CPU_NameSet()
 *
 * @brief    Set CPU host name.
 *
 * @param    p_name  Pointer to CPU host name to set.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_ARG
 *
 * @note     (1) 'p_name' ASCII string size, including the terminating NULL character, MUST be less
 *               than or equal to CPU_CFG_NAME_SIZE.
 *******************************************************************************************************/

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
void CPU_NameSet(const CPU_CHAR *p_name,
                 RTOS_ERR       *p_err)
{
  CPU_SIZE_T len;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  RTOS_ASSERT_DBG_ERR_SET((p_name != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );

  len = Str_Len_N(p_name,
                  CPU_CFG_NAME_SIZE);
  if (len < CPU_CFG_NAME_SIZE) {                                // If       cfg name len < max name size, ...
    CORE_ENTER_ATOMIC();
    (void)Str_Copy_N(&CPU_Name[0],                              // ... copy cfg name to CPU host name.
                     p_name,
                     CPU_CFG_NAME_SIZE);
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
  }
}
#endif

/****************************************************************************************************//**
 *                                               CPU_TS_Get32()
 *
 * @brief    Get current 32-bit CPU timestamp.
 *
 * @return   Current 32-bit CPU timestamp (in timestamp timer counts).
 *
 * @note     (1) When applicable, the amount of time measured by CPU timestamps is calculated by
 *               either of the following equations :
 *                   @verbatim
 *               - (a) Time measured  =  Number timer counts  *  Timer period
 *
 *                           where
 *
 *                               Number timer counts     Number of timer counts measured
 *                               Timer period            Timer's period in some units of
 *                                                       (fractional) seconds
 *                               Time measured           Amount of time measured, in same
 *                                                       units of (fractional) seconds
 *                                                       as the Timer period
 *
 *                                           Number timer counts
 *               - (b) Time measured  =  ---------------------
 *                                           Timer frequency
 *
 *                           where
 *
 *                               Number timer counts     Number of timer counts measured
 *                               Timer frequency         Timer's frequency in some units
 *                                                       of counts per second
 *                               Time measured           Amount of time measured, in seconds
 *                   @endverbatim
 *               See also 'cpu.h  FUNCTION PROTOTYPES  CPU_TS_TmrRd()  Note #2c1'.
 *
 * @note     (2) In case the CPU timestamp timer has lower precision than the 32-bit CPU timestamp;
 *               its precision is extended via periodic updates by accumulating the deltas of the
 *               timestamp timer count values into the higher-precision 32-bit CPU timestamp.
 *
 * @note     (3) After initialization, 'CPU_TS_32_Accum' & 'CPU_TS_32_TmrPrev' MUST ALWAYS
 *               be accessed AND updated exclusively with interrupts disabled -- but NOT
 *               with critical sections.
 *******************************************************************************************************/

#if (CPU_CFG_TS_32_EN == DEF_ENABLED)
CPU_TS32 CPU_TS_Get32(void)
{
  CPU_TS32 ts;
#if (CPU_CFG_TS_TMR_SIZE < CPU_WORD_SIZE_32)
  CPU_TS_TMR tmr_cur;
  CPU_TS_TMR tmr_delta;
  CORE_DECLARE_IRQ_STATE;

#endif

#if (CPU_CFG_TS_TMR_SIZE >= CPU_WORD_SIZE_32)
  ts = (CPU_TS32)CPU_TS_TmrRd();                                // Get cur ts tmr val (in 32-bit ts cnts).

#else
  CORE_ENTER_ATOMIC();
  tmr_cur = (CPU_TS_TMR) CPU_TS_TmrRd();                        // Get cur ts tmr val (in ts tmr cnts).
  tmr_delta = (CPU_TS_TMR)(tmr_cur - CPU_TS_32_TmrPrev);        // Calc      delta ts tmr cnts.
  CPU_TS_32_Accum += (CPU_TS32) tmr_delta;                      // Inc ts by delta ts tmr cnts (see Note #2).
  CPU_TS_32_TmrPrev = (CPU_TS_TMR) tmr_cur;                     // Save cur ts tmr cnts for next update.
  ts = (CPU_TS32) CPU_TS_32_Accum;
  CORE_EXIT_ATOMIC();
#endif

  return (ts);
}
#endif

/****************************************************************************************************//**
 *                                               CPU_TS_Get64()
 *
 * @brief    Get current 64-bit CPU timestamp.
 *
 * @return   Current 64-bit CPU timestamp (in timestamp timer counts).
 *
 * @note     (1) When applicable, the amount of time measured by CPU timestamps is calculated by
 *               either of the following equations :
 *                   @verbatim
 *               - (a) Time measured  =  Number timer counts  *  Timer period
 *
 *                           where
 *
 *                               Number timer counts     Number of timer counts measured
 *                               Timer period            Timer's period in some units of
 *                                                       (fractional) seconds
 *                               Time measured           Amount of time measured, in same
 *                                                       units of (fractional) seconds
 *                                                       as the Timer period
 *
 *                                           Number timer counts
 *               - (b) Time measured  =  ---------------------
 *                                           Timer frequency
 *
 *                           where
 *
 *                               Number timer counts     Number of timer counts measured
 *                               Timer frequency         Timer's frequency in some units
 *                                                       of counts per second
 *                               Time measured           Amount of time measured, in seconds
 *                   @endverbatim
 *               See also 'cpu.h  FUNCTION PROTOTYPES  CPU_TS_TmrRd()  Note #2c1'.
 *
 * @note     (2) In case the CPU timestamp timer has lower precision than the 64-bit CPU timestamp;
 *               its precision is extended via periodic updates by accumulating the deltas of the
 *               timestamp timer count values into the higher-precision 64-bit CPU timestamp.
 *
 * @note     (3) After initialization, 'CPU_TS_64_Accum' & 'CPU_TS_64_TmrPrev' MUST ALWAYS
 *               be accessed AND updated exclusively with interrupts disabled -- but NOT
 *               with critical sections.
 *******************************************************************************************************/

#if (CPU_CFG_TS_64_EN == DEF_ENABLED)
CPU_TS64 CPU_TS_Get64(void)
{
  CPU_TS64 ts;
#if (CPU_CFG_TS_TMR_SIZE < CPU_WORD_SIZE_64)
  CPU_TS_TMR tmr_cur;
  CPU_TS_TMR tmr_delta;
  CORE_DECLARE_IRQ_STATE;
#endif

#if (CPU_CFG_TS_TMR_SIZE >= CPU_WORD_SIZE_64)
  ts = (CPU_TS64)CPU_TS_TmrRd();                                // Get cur ts tmr val (in 64-bit ts cnts).

#else
  CORE_ENTER_ATOMIC();
  tmr_cur = (CPU_TS_TMR) CPU_TS_TmrRd();                        // Get cur ts tmr val (in ts tmr cnts).
  tmr_delta = (CPU_TS_TMR)(tmr_cur - CPU_TS_64_TmrPrev);        // Calc      delta ts tmr cnts.
  CPU_TS_64_Accum += (CPU_TS64) tmr_delta;                      // Inc ts by delta ts tmr cnts (see Note #2).
  CPU_TS_64_TmrPrev = (CPU_TS_TMR) tmr_cur;                     // Save cur ts tmr cnts for next update.
  ts = (CPU_TS64) CPU_TS_64_Accum;
  CORE_EXIT_ATOMIC();
#endif

  return (ts);
}
#endif

/****************************************************************************************************//**
 *                                               CPU_TS_Update()
 *
 * @brief    Update current CPU timestamp(s).
 *
 * @note     (1) CPU timestamp(s) MUST be updated periodically by some application (or BSP) time
 *               handler in order to (adequately) maintain CPU timestamp(s)' time.
 *               CPU timestamp(s) MUST be updated more frequently than the CPU timestamp timer
 *               overflows; otherwise, CPU timestamp(s) will lose time.
 *               See also 'cpu.h  FUNCTION PROTOTYPES  CPU_TS_TmrRd()  Note #2e2'.
 *******************************************************************************************************/

#if (CPU_CFG_TS_EN == DEF_ENABLED)
void CPU_TS_Update(void)
{
#if ((CPU_CFG_TS_32_EN == DEF_ENABLED) \
  && (CPU_CFG_TS_TMR_SIZE < CPU_WORD_SIZE_32))
  (void)CPU_TS_Get32();
#endif

#if ((CPU_CFG_TS_64_EN == DEF_ENABLED) \
  && (CPU_CFG_TS_TMR_SIZE < CPU_WORD_SIZE_64))
  (void)CPU_TS_Get64();
#endif
}
#endif

/****************************************************************************************************//**
 *                                           CPU_TS_TmrFreqGet()
 *
 * @brief    Get CPU timestamp's timer frequency.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_CFG
 *                       - RTOS_ERR_NOT_AVAIL
 *
 * @return   CPU timestamp's timer frequency (in Hertz), if NO error(s).
 *           0                                         , otherwise.
 *******************************************************************************************************/
CPU_TS_TMR_FREQ CPU_TS_TmrFreqGet(RTOS_ERR *p_err)
{
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
  CPU_TS_TMR_FREQ freq_hz;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0);

  freq_hz = CPU_TS_TmrFreq_Hz;
  RTOS_ERR_SET(*p_err, (freq_hz != 0u) ? RTOS_ERR_NONE : RTOS_ERR_INVALID_CFG);

  return (freq_hz);
#else
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err, 0);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);

  return (0u);
#endif
}

/****************************************************************************************************//**
 *                                           CPU_TS_TmrFreqSet()
 *
 * @brief    Set CPU timestamp's timer frequency.
 *
 * @param    freq_hz     Frequency (in Hertz) to set for CPU timestamp's timer.
 *
 * @note     (1) CPU timestamp timer frequency is NOT required for internal CPU timestamp
 *               operations but may OPTIONALLY be configured by CPU_TS_TmrInit() or other
 *               application/BSP initialization functions.
 *               CPU timestamp timer frequency MAY be used with optional CPU_TSxx_to_uSec()
 *               to convert CPU timestamps from timer counts into microseconds.
 *               See also 'cpu.h  FUNCTION PROTOTYPES  CPU_TSxx_to_uSec().
 *               @n
 *               CPU timestamp timer period SHOULD be less than the typical measured time but MUST
 *               be less than the maximum measured time; otherwise, timer resolution inadequate to
 *               measure desired times.
 *               See also 'cpu.h  FUNCTION PROTOTYPES  CPU_TSxx_to_uSec().
 *******************************************************************************************************/
void CPU_TS_TmrFreqSet(CPU_TS_TMR_FREQ freq_hz)
{
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
  CPU_TS_TmrFreq_Hz = freq_hz;
#else
  PP_UNUSED_PARAM(freq_hz);
#endif
}

/****************************************************************************************************//**
 *                                           CPU_CntLeadZeros()
 *
 * @brief    Count the number of contiguous, most-significant, leading zero bits in a data value.
 *
 * @param    val     Data value to count leading zero bits.
 *
 * @return   Number of contiguous, most-significant, leading zero bits in 'val', if NO error(s).
 *           DEF_INT_CPU_U_MAX_VAL,                                              otherwise.
 *
 * @note     (1) Supports the following data value sizes :
 *               - 8-bits,
 *               - 16-bits,
 *               - 32-bits,
 *               - 64-bits
 *               @n
 *               See also 'cpu_def.h  CPU WORD CONFIGURATION  Note #1'.
 *
 *           - (a) For  8-bit values :
 *               @verbatim
 *                   b07  b06  b05  b04  b03  b02  b01  b00    # Leading Zeros
 *                   ---  ---  ---  ---  ---  ---  ---  ---    ---------------
 *                   1    x    x    x    x    x    x    x            0
 *                   0    1    x    x    x    x    x    x            1
 *                   0    0    1    x    x    x    x    x            2
 *                   0    0    0    1    x    x    x    x            3
 *                   0    0    0    0    1    x    x    x            4
 *                   0    0    0    0    0    1    x    x            5
 *                   0    0    0    0    0    0    1    x            6
 *                   0    0    0    0    0    0    0    1            7
 *                   0    0    0    0    0    0    0    0            8
 *               @endverbatim
 *           - (b) For 16-bit values :
 *               @verbatim
 *                   b15  b14  b13  ...  b04  b03  b02  b01  b00    # Leading Zeros
 *                   ---  ---  ---       ---  ---  ---  ---  ---    ---------------
 *                   1    x    x         x    x    x    x    x            0
 *                   0    1    x         x    x    x    x    x            1
 *                   0    0    1         x    x    x    x    x            2
 *                   :    :    :         :    :    :    :    :            :
 *                   :    :    :         :    :    :    :    :            :
 *                   0    0    0         1    x    x    x    x           11
 *                   0    0    0         0    1    x    x    x           12
 *                   0    0    0         0    0    1    x    x           13
 *                   0    0    0         0    0    0    1    x           14
 *                   0    0    0         0    0    0    0    1           15
 *                   0    0    0         0    0    0    0    0           16
 *               @endverbatim
 *           - (c) For 32-bit values :
 *               @verbatim
 *                   b31  b30  b29  ...  b04  b03  b02  b01  b00    # Leading Zeros
 *                   ---  ---  ---       ---  ---  ---  ---  ---    ---------------
 *                   1    x    x         x    x    x    x    x            0
 *                   0    1    x         x    x    x    x    x            1
 *                   0    0    1         x    x    x    x    x            2
 *                   :    :    :         :    :    :    :    :            :
 *                   :    :    :         :    :    :    :    :            :
 *                   0    0    0         1    x    x    x    x           27
 *                   0    0    0         0    1    x    x    x           28
 *                   0    0    0         0    0    1    x    x           29
 *                   0    0    0         0    0    0    1    x           30
 *                   0    0    0         0    0    0    0    1           31
 *                   0    0    0         0    0    0    0    0           32
 *               @endverbatim
 *
 *           - (d) For 64-bit values :
 *               @verbatim
 *                   b63  b62  b61  ...  b04  b03  b02  b01  b00    # Leading Zeros
 *                   ---  ---  ---       ---  ---  ---  ---  ---    ---------------
 *                   1    x    x         x    x    x    x    x            0
 *                   0    1    x         x    x    x    x    x            1
 *                   0    0    1         x    x    x    x    x            2
 *                   :    :    :         :    :    :    :    :            :
 *                   :    :    :         :    :    :    :    :            :
 *                   0    0    0         1    x    x    x    x           59
 *                   0    0    0         0    1    x    x    x           60
 *                   0    0    0         0    0    1    x    x           61
 *                   0    0    0         0    0    0    1    x           62
 *                   0    0    0         0    0    0    0    1           63
 *                   0    0    0         0    0    0    0    0           64
 *               @endverbatim
 *                   See also 'CPU COUNT LEAD ZEROs LOOKUP TABLE  Note #1'.
 *******************************************************************************************************/

#ifndef  CPU_CFG_LEAD_ZEROS_ASM_PRESENT
CPU_DATA CPU_CntLeadZeros(CPU_DATA val)
{
  CPU_DATA nbr_lead_zeros;

#if   (CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_08)
  nbr_lead_zeros = CPU_CntLeadZeros08((CPU_INT08U)val);

#elif (CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_16)
  nbr_lead_zeros = CPU_CntLeadZeros16((CPU_INT16U)val);

#elif (CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_32)
  nbr_lead_zeros = CPU_CntLeadZeros32((CPU_INT32U)val);

#elif (CPU_CFG_DATA_SIZE == CPU_WORD_SIZE_64)
  nbr_lead_zeros = CPU_CntLeadZeros64((CPU_INT64U)val);

#else //                                                           See Note #1a.
  nbr_lead_zeros = DEF_INT_CPU_U_MAX_VAL;
#endif

  return (nbr_lead_zeros);
}
#endif

/****************************************************************************************************//**
 *                                           CPU_CntLeadZeros08()
 *
 * @brief    Count the number of contiguous, most-significant, leading zero bits in an 8-bit data value.
 *
 * @param    val     Data value to count leading zero bits.
 *
 * @return   Number of contiguous, most-significant, leading zero bits in 'val'.
 *
 * @note     (1) Supports  8-bit values :
 *               @verbatim
 *                           b07  b06  b05  b04  b03  b02  b01  b00    # Leading Zeros
 *                           ---  ---  ---  ---  ---  ---  ---  ---    ---------------
 *                           1    x    x    x    x    x    x    x            0
 *                           0    1    x    x    x    x    x    x            1
 *                           0    0    1    x    x    x    x    x            2
 *                           0    0    0    1    x    x    x    x            3
 *                           0    0    0    0    1    x    x    x            4
 *                           0    0    0    0    0    1    x    x            5
 *                           0    0    0    0    0    0    1    x            6
 *                           0    0    0    0    0    0    0    1            7
 *                           0    0    0    0    0    0    0    0            8
 *               @endverbatim
 *               See also 'CPU COUNT LEAD ZEROs LOOKUP TABLE  Note #1'.
 *******************************************************************************************************/

#if (CPU_CFG_DATA_SIZE_MAX >= CPU_WORD_SIZE_08)
CPU_DATA CPU_CntLeadZeros08(CPU_INT08U val)
{
#if  (!((defined(CPU_CFG_LEAD_ZEROS_ASM_PRESENT)) \
  && (CPU_CFG_DATA_SIZE >= CPU_WORD_SIZE_08)))
  CPU_DATA ix;
#endif
  CPU_DATA nbr_lead_zeros;

#if ((defined(CPU_CFG_LEAD_ZEROS_ASM_PRESENT)) \
  && (CPU_CFG_DATA_SIZE >= CPU_WORD_SIZE_08))
  //                                                               ---------- ASM-OPTIMIZED -----------
  nbr_lead_zeros = CPU_CntLeadZeros((CPU_DATA)val);
  nbr_lead_zeros -= (CPU_CFG_DATA_SIZE - CPU_WORD_SIZE_08) * DEF_OCTET_NBR_BITS;

#else
  //                                                               ----------- C-OPTIMIZED ------------
  //                                                               Chk bits [07:00] :
  //                                                               .. Nbr lead zeros =               ..
  //                                                               .. lookup tbl ix  = 'val' >>  0 bits
  //                                                               .. plus nbr msb lead zeros =  0 bits.
  ix = (CPU_DATA)(val);
  nbr_lead_zeros = (CPU_DATA)(CPU_CntLeadZerosTbl[ix]);
#endif

  return (nbr_lead_zeros);
}
#endif

/****************************************************************************************************//**
 *                                           CPU_CntLeadZeros16()
 *
 * @brief    Count the number of contiguous, most-significant, leading zero bits in a 16-bit data value.
 *
 * @param    val     Data value to count leading zero bits.
 *
 * @return   Number of contiguous, most-significant, leading zero bits in 'val'.
 *
 * @note     (1) Supports 16-bit values :
 *               @verbatim
 *                       b15  b14  b13  ...  b04  b03  b02  b01  b00    # Leading Zeros
 *                       ---  ---  ---       ---  ---  ---  ---  ---    ---------------
 *                       1    x    x         x    x    x    x    x            0
 *                       0    1    x         x    x    x    x    x            1
 *                       0    0    1         x    x    x    x    x            2
 *                       :    :    :         :    :    :    :    :            :
 *                       :    :    :         :    :    :    :    :            :
 *                       0    0    0         1    x    x    x    x           11
 *                       0    0    0         0    1    x    x    x           12
 *                       0    0    0         0    0    1    x    x           13
 *                       0    0    0         0    0    0    1    x           14
 *                       0    0    0         0    0    0    0    1           15
 *                       0    0    0         0    0    0    0    0           16
 *               @endverbatim
 *
 *               See also 'CPU COUNT LEAD ZEROs LOOKUP TABLE  Note #1'.
 *******************************************************************************************************/

#if (CPU_CFG_DATA_SIZE_MAX >= CPU_WORD_SIZE_16)
CPU_DATA CPU_CntLeadZeros16(CPU_INT16U val)
{
#if  (!((defined(CPU_CFG_LEAD_ZEROS_ASM_PRESENT)) \
  && (CPU_CFG_DATA_SIZE >= CPU_WORD_SIZE_16)))
  CPU_DATA ix;
#endif
  CPU_DATA nbr_lead_zeros;

  //                                                               ---------- ASM-OPTIMIZED -----------
#if ((defined(CPU_CFG_LEAD_ZEROS_ASM_PRESENT)) \
  && (CPU_CFG_DATA_SIZE >= CPU_WORD_SIZE_16))
  nbr_lead_zeros = CPU_CntLeadZeros((CPU_DATA)val);
  nbr_lead_zeros -= (CPU_CFG_DATA_SIZE - CPU_WORD_SIZE_16) * DEF_OCTET_NBR_BITS;

#else
  //                                                               ----------- C-OPTIMIZED ------------
  if (val > 0x00FFu) {                                          // Chk bits [15:08] :
                                                                // .. Nbr lead zeros =               ..
    ix = (CPU_DATA)((CPU_DATA)val >> 8u);                       // .. lookup tbl ix  = 'val' >>  8 bits
    nbr_lead_zeros = (CPU_DATA)(CPU_CntLeadZerosTbl[ix]);       // .. plus nbr msb lead zeros =  0 bits.
  } else {                                                      // Chk bits [07:00] :
                                                                // .. Nbr lead zeros =               ..
    ix = (CPU_DATA)(val);                                       // .. lookup tbl ix  = 'val' >>  0 bits
                                                                // .. plus nbr msb lead zeros =  8 bits.
    nbr_lead_zeros = (CPU_DATA)((CPU_DATA)CPU_CntLeadZerosTbl[ix] +  8u);
  }
#endif

  return (nbr_lead_zeros);
}
#endif

/****************************************************************************************************//**
 *                                           CPU_CntLeadZeros32()
 *
 * @brief    Count the number of contiguous, most-significant, leading zero bits in a 32-bit data value.
 *
 * @param    val     Data value to count leading zero bits.
 *
 * @return   Number of contiguous, most-significant, leading zero bits in 'val'.
 *
 * @note     (1) Supports 32-bit values :
 *               @verbatim
 *                       b31  b30  b29  ...  b04  b03  b02  b01  b00    # Leading Zeros
 *                       ---  ---  ---       ---  ---  ---  ---  ---    ---------------
 *                       1    x    x         x    x    x    x    x            0
 *                       0    1    x         x    x    x    x    x            1
 *                       0    0    1         x    x    x    x    x            2
 *                       :    :    :         :    :    :    :    :            :
 *                       :    :    :         :    :    :    :    :            :
 *                       0    0    0         1    x    x    x    x           27
 *                       0    0    0         0    1    x    x    x           28
 *                       0    0    0         0    0    1    x    x           29
 *                       0    0    0         0    0    0    1    x           30
 *                       0    0    0         0    0    0    0    1           31
 *                       0    0    0         0    0    0    0    0           32
 *               @endverbatim
 *
 *               See also 'CPU COUNT LEAD ZEROs LOOKUP TABLE  Note #1'.
 *******************************************************************************************************/

#if (CPU_CFG_DATA_SIZE_MAX >= CPU_WORD_SIZE_32)
CPU_DATA CPU_CntLeadZeros32(CPU_INT32U val)
{
#if  (!((defined(CPU_CFG_LEAD_ZEROS_ASM_PRESENT)) \
  && (CPU_CFG_DATA_SIZE >= CPU_WORD_SIZE_32)))
  CPU_DATA ix;
#endif
  CPU_DATA nbr_lead_zeros;

  //                                                               ---------- ASM-OPTIMIZED -----------
#if ((defined(CPU_CFG_LEAD_ZEROS_ASM_PRESENT)) \
  && (CPU_CFG_DATA_SIZE >= CPU_WORD_SIZE_32))
  nbr_lead_zeros = CPU_CntLeadZeros((CPU_DATA)val);
  nbr_lead_zeros -= (CPU_CFG_DATA_SIZE - CPU_WORD_SIZE_32) * DEF_OCTET_NBR_BITS;

#else //                                                           ----------- C-OPTIMIZED ------------
  if (val > 0x0000FFFFu) {
    if (val > 0x00FFFFFFu) {                                    // Chk bits [31:24] :
                                                                // .. Nbr lead zeros =               ..
      ix = (CPU_DATA)((CPU_DATA)(val >> 24u));                  // .. lookup tbl ix  = 'val' >> 24 bits
      nbr_lead_zeros = (CPU_DATA)(CPU_CntLeadZerosTbl[ix]);     // .. plus nbr msb lead zeros =  0 bits.
    } else {                                                    // Chk bits [23:16] :
                                                                // .. Nbr lead zeros =               ..
      ix = (CPU_DATA)((CPU_DATA)(val >> 16u));                  // .. lookup tbl ix  = 'val' >> 16 bits
                                                                // .. plus nbr msb lead zeros =  8 bits.
      nbr_lead_zeros = (CPU_DATA)((CPU_DATA)CPU_CntLeadZerosTbl[ix] +  8u);
    }
  } else {
    if (val > 0x000000FFu) {                                    // Chk bits [15:08] :
                                                                // .. Nbr lead zeros =               ..
      ix = (CPU_DATA)((CPU_DATA)(val >>  8u));                  // .. lookup tbl ix  = 'val' >>  8 bits
                                                                // .. plus nbr msb lead zeros = 16 bits.
      nbr_lead_zeros = (CPU_DATA)((CPU_DATA)CPU_CntLeadZerosTbl[ix] + 16u);
    } else {                                                    // Chk bits [07:00] :
                                                                // .. Nbr lead zeros =               ..
      ix = (CPU_DATA)((CPU_DATA)(val >>  0u));                  // .. lookup tbl ix  = 'val' >>  0 bits
                                                                // .. plus nbr msb lead zeros = 24 bits.
      nbr_lead_zeros = (CPU_DATA)((CPU_DATA)CPU_CntLeadZerosTbl[ix] + 24u);
    }
  }
#endif

  return (nbr_lead_zeros);
}
#endif

/****************************************************************************************************//**
 *                                           CPU_CntLeadZeros64()
 *
 * @brief    Count the number of contiguous, most-significant, leading zero bits in a 64-bit data value.
 *
 * @param    val     Data value to count leading zero bits.
 *
 * @return   Number of contiguous, most-significant, leading zero bits in 'val'.
 *
 * @note     (1) Supports 64-bit values :
 *               @verbatim
 *                       b63  b62  b61  ...  b04  b03  b02  b01  b00    # Leading Zeros
 *                       ---  ---  ---       ---  ---  ---  ---  ---    ---------------
 *                       1    x    x         x    x    x    x    x            0
 *                       0    1    x         x    x    x    x    x            1
 *                       0    0    1         x    x    x    x    x            2
 *                       :    :    :         :    :    :    :    :            :
 *                       :    :    :         :    :    :    :    :            :
 *                       0    0    0         1    x    x    x    x           59
 *                       0    0    0         0    1    x    x    x           60
 *                       0    0    0         0    0    1    x    x           61
 *                       0    0    0         0    0    0    1    x           62
 *                       0    0    0         0    0    0    0    1           63
 *                       0    0    0         0    0    0    0    0           64
 *               @endverbatim
 *
 *               See also 'CPU COUNT LEAD ZEROs LOOKUP TABLE  Note #1'.
 *******************************************************************************************************/

#if (CPU_CFG_DATA_SIZE_MAX >= CPU_WORD_SIZE_64)
CPU_DATA CPU_CntLeadZeros64(CPU_INT64U val)
{
#if  (!((defined(CPU_CFG_LEAD_ZEROS_ASM_PRESENT)) \
  && (CPU_CFG_DATA_SIZE >= CPU_WORD_SIZE_64)))
  CPU_DATA ix;
#endif
  CPU_DATA nbr_lead_zeros;

  //                                                               ---------- ASM-OPTIMIZED -----------
#if ((defined(CPU_CFG_LEAD_ZEROS_ASM_PRESENT)) \
  && (CPU_CFG_DATA_SIZE >= CPU_WORD_SIZE_64))
  nbr_lead_zeros = CPU_CntLeadZeros((CPU_DATA)val);
  nbr_lead_zeros -= (CPU_CFG_DATA_SIZE - CPU_WORD_SIZE_64) * DEF_OCTET_NBR_BITS;

#else //                                                           ----------- C-OPTIMIZED ------------
  if (val > 0x00000000FFFFFFFFu) {
    if (val > 0x0000FFFFFFFFFFFFu) {
      if (val > 0x00FFFFFFFFFFFFFFu) {                          // Chk bits [63:56] :
                                                                // .. Nbr lead zeros =               ..
        ix = (CPU_DATA)((CPU_INT64U)val >> 56u);                // .. lookup tbl ix  = 'val' >> 56 bits
        nbr_lead_zeros = (CPU_DATA)(CPU_CntLeadZerosTbl[ix]);   // .. plus nbr msb lead zeros =  0 bits.
      } else {                                                  // Chk bits [55:48] :
                                                                // .. Nbr lead zeros =               ..
        ix = (CPU_DATA)((CPU_INT64U)val >> 48u);                // .. lookup tbl ix  = 'val' >> 48 bits
                                                                // .. plus nbr msb lead zeros =  8 bits.
        nbr_lead_zeros = (CPU_DATA)((CPU_INT64U)CPU_CntLeadZerosTbl[ix] +  8u);
      }
    } else {
      if (val > 0x000000FFFFFFFFFFu) {                          // Chk bits [47:40] :
                                                                // .. Nbr lead zeros =               ..
        ix = (CPU_DATA)((CPU_INT64U)val >> 40u);                // .. lookup tbl ix  = 'val' >> 40 bits
                                                                // .. plus nbr msb lead zeros = 16 bits.
        nbr_lead_zeros = (CPU_DATA)((CPU_INT64U)CPU_CntLeadZerosTbl[ix] + 16u);
      } else {                                                  // Chk bits [39:32] :
                                                                // .. Nbr lead zeros =               ..
        ix = (CPU_DATA)((CPU_INT64U)val >> 32u);                // .. lookup tbl ix  = 'val' >> 32 bits
                                                                // .. plus nbr msb lead zeros = 24 bits.
        nbr_lead_zeros = (CPU_DATA)((CPU_INT64U)CPU_CntLeadZerosTbl[ix] + 24u);
      }
    }
  } else {
    if (val > 0x000000000000FFFFu) {
      if (val > 0x0000000000FFFFFFu) {                          // Chk bits [31:24] :
                                                                // .. Nbr lead zeros =               ..
        ix = (CPU_DATA)((CPU_INT64U)val >> 24u);                // .. lookup tbl ix  = 'val' >> 24 bits
                                                                // .. plus nbr msb lead zeros = 32 bits.
        nbr_lead_zeros = (CPU_DATA)((CPU_INT64U)CPU_CntLeadZerosTbl[ix] + 32u);
      } else {                                                  // Chk bits [23:16] :
                                                                // .. Nbr lead zeros =               ..
        ix = (CPU_DATA)((CPU_INT64U)val >> 16u);                // .. lookup tbl ix  = 'val' >> 16 bits
                                                                // .. plus nbr msb lead zeros = 40 bits.
        nbr_lead_zeros = (CPU_DATA)((CPU_INT64U)CPU_CntLeadZerosTbl[ix] + 40u);
      }
    } else {
      if (val > 0x00000000000000FFu) {                          // Chk bits [15:08] :
                                                                // .. Nbr lead zeros =               ..
        ix = (CPU_DATA)((CPU_INT64U)val >>  8u);                // .. lookup tbl ix  = 'val' >>  8 bits
                                                                // .. plus nbr msb lead zeros = 48 bits.
        nbr_lead_zeros = (CPU_DATA)((CPU_INT64U)CPU_CntLeadZerosTbl[ix] + 48u);
      } else {                                                  // Chk bits [07:00] :
                                                                // .. Nbr lead zeros =               ..
        ix = (CPU_DATA)(val);                                   // .. lookup tbl ix  = 'val' >>  0 bits
                                                                // .. plus nbr msb lead zeros = 56 bits.
        nbr_lead_zeros = (CPU_DATA)((CPU_INT64U)CPU_CntLeadZerosTbl[ix] + 56u);
      }
    }
  }
#endif

  return (nbr_lead_zeros);
}
#endif

/****************************************************************************************************//**
 *                                           CPU_CntTrailZeros()
 *
 * @brief    Count the number of contiguous, least-significant, trailing zero bits in a data value.
 *
 * @param    val     Data value to count trailing zero bits.
 *
 * @return   Number of contiguous, least-significant, trailing zero bits in 'val'.
 *
 * @note     (1) Supports the following data value sizes :
 *               -  8-bits,
 *               - 16-bits,
 *               - 32-bits,
 *               - 64-bits
 *                   See also 'cpu_def.h  CPU WORD CONFIGURATION  Note #1'.
 *               @verbatim
 *           - (a) For  8-bit values :
 *
 *                   b07  b06  b05  b04  b03  b02  b01  b00    # Trailing Zeros
 *                   ---  ---  ---  ---  ---  ---  ---  ---    ----------------
 *                   x    x    x    x    x    x    x    1            0
 *                   x    x    x    x    x    x    1    0            1
 *                   x    x    x    x    x    1    0    0            2
 *                   x    x    x    x    1    0    0    0            3
 *                   x    x    x    1    0    0    0    0            4
 *                   x    x    1    0    0    0    0    0            5
 *                   x    1    0    0    0    0    0    0            6
 *                   1    0    0    0    0    0    0    0            7
 *                   0    0    0    0    0    0    0    0            8
 *               @endverbatim
 *
 *           - (b) For 16-bit values :
 *               @verbatim
 *                   b15  b14  b13  b12  b11  ...  b02  b01  b00    # Trailing Zeros
 *                   ---  ---  ---  ---  ---       ---  ---  ---    ----------------
 *                   x    x    x    x    x         x    x    1            0
 *                   x    x    x    x    x         x    1    0            1
 *                   x    x    x    x    x         1    0    0            2
 *                   :    :    :    :    :         :    :    :            :
 *                   :    :    :    :    :         :    :    :            :
 *                   x    x    x    x    1         0    0    0           11
 *                   x    x    x    1    0         0    0    0           12
 *                   x    x    1    0    0         0    0    0           13
 *                   x    1    0    0    0         0    0    0           14
 *                   1    0    0    0    0         0    0    0           15
 *                   0    0    0    0    0         0    0    0           16
 *               @endverbatim
 *
 *           - (c) For 32-bit values :
 *               @verbatim
 *                   b31  b30  b29  b28  b27  ...  b02  b01  b00    # Trailing Zeros
 *                   ---  ---  ---  ---  ---       ---  ---  ---    ----------------
 *                   x    x    x    x    x         x    x    1            0
 *                   x    x    x    x    x         x    1    0            1
 *                   x    x    x    x    x         1    0    0            2
 *                   :    :    :    :    :         :    :    :            :
 *                   :    :    :    :    :         :    :    :            :
 *                   x    x    x    x    1         0    0    0           27
 *                   x    x    x    1    0         0    0    0           28
 *                   x    x    1    0    0         0    0    0           29
 *                   x    1    0    0    0         0    0    0           30
 *                   1    0    0    0    0         0    0    0           31
 *                   0    0    0    0    0         0    0    0           32
 *               @endverbatim
 *
 *           - (d) For 64-bit values :
 *               @verbatim
 *                   b63  b62  b61  b60  b59  ...  b02  b01  b00    # Trailing Zeros
 *                   ---  ---  ---  ---  ---       ---  ---  ---    ----------------
 *                   x    x    x    x    x         x    x    1            0
 *                   x    x    x    x    x         x    1    0            1
 *                   x    x    x    x    x         1    0    0            2
 *                   :    :    :    :    :         :    :    :            :
 *                   :    :    :    :    :         :    :    :            :
 *                   x    x    x    x    1         0    0    0           59
 *                   x    x    x    1    0         0    0    0           60
 *                   x    x    1    0    0         0    0    0           61
 *                   x    1    0    0    0         0    0    0           62
 *                   1    0    0    0    0         0    0    0           63
 *                   0    0    0    0    0         0    0    0           64
 *               @endverbatim
 *
 * @note     (2) For non-zero values, the returned number of contiguous, least-significant, trailing
 *               zero bits is also equivalent to the bit position of the least-significant set bit.
 *
 * @note     (3) 'val' SHOULD be validated for non-'0' PRIOR to all other counting zero calculations :
 *           - (a) CPU_CntTrailZeros()'s final conditional statement calculates 'val's number of
 *                   trailing zeros based on its return data size, 'CPU_CFG_DATA_SIZE', & 'val's
 *                   calculated number of lead zeros ONLY if the initial 'val' is non-'0' :
 *                   @verbatim
 *                       if (val != 0u) {
 *                           nbr_trail_zeros = ((CPU_CFG_DATA_SIZE * DEF_OCTET_NBR_BITS) - 1u) - nbr_lead_zeros;
 *                       } else {
 *                           nbr_trail_zeros = nbr_lead_zeros;
 *                       }
 *                   @endverbatim
 *                   Therefore, initially validating all non-'0' values avoids having to conditionally
 *                   execute the final 'if' statement.
 *******************************************************************************************************/

#ifndef   CPU_CFG_TRAIL_ZEROS_ASM_PRESENT
CPU_DATA CPU_CntTrailZeros(CPU_DATA val)
{
  CPU_DATA val_bit_mask;
  CPU_DATA nbr_lead_zeros;
  CPU_DATA nbr_trail_zeros;

  if (val == 0u) {                                              // Rtn ALL val bits as zero'd (see Note #3).
    return (CPU_CFG_DATA_SIZE * DEF_OCTET_NBR_BITS);
  }

  val_bit_mask = val & ((CPU_DATA) ~val + 1u);                  // Zero/clr all bits EXCEPT least-sig set bit.
  nbr_lead_zeros = CPU_CntLeadZeros(val_bit_mask);              // Cnt  nbr lead  0s.
                                                                // Calc nbr trail 0s = (nbr val bits - 1) - nbr lead 0s.
  nbr_trail_zeros = ((CPU_CFG_DATA_SIZE * DEF_OCTET_NBR_BITS) - 1u) - nbr_lead_zeros;

  return (nbr_trail_zeros);
}
#endif

/****************************************************************************************************//**
 *                                           CPU_CntTrailZeros08()
 *
 * @brief    Count the number of contiguous, least-significant, trailing zero bits in an 8-bit data value.
 *
 * @param    val     Data value to count trailing zero bits.
 *
 * @return   Number of contiguous, least-significant, trailing zero bits in 'val'.
 *
 * @note     (1) Supports  8-bit values :
 *               @verbatim
 *                           b07  b06  b05  b04  b03  b02  b01  b00    # Trailing Zeros
 *                           ---  ---  ---  ---  ---  ---  ---  ---    ----------------
 *                           x    x    x    x    x    x    x    1            0
 *                           x    x    x    x    x    x    1    0            1
 *                           x    x    x    x    x    1    0    0            2
 *                           x    x    x    x    1    0    0    0            3
 *                           x    x    x    1    0    0    0    0            4
 *                           x    x    1    0    0    0    0    0            5
 *                           x    1    0    0    0    0    0    0            6
 *                           1    0    0    0    0    0    0    0            7
 *                           0    0    0    0    0    0    0    0            8
 *               @endverbatim
 *
 * @note     (2) For non-zero values, the returned number of contiguous, least-significant, trailing
 *               zero bits is also equivalent to the bit position of the least-significant set bit.
 *
 * @note     (3) 'val' SHOULD be validated for non-'0' PRIOR to all other counting zero calculations :
 *           - (a) For assembly-optimized implementations, CPU_CntTrailZeros() returns 'val's
 *                   number of trailing zeros via CPU's native data size, 'CPU_CFG_DATA_SIZE'.
 *                   If the returned number of zeros exceeds CPU_CntTrailZeros08()'s  8-bit return
 *                   data size, then the returned number of zeros must be offset by the difference
 *                   between CPU_CntTrailZeros()'s  & CPU_CntTrailZeros08()'s return data size :
 *                   @verbatim
 *                       nbr_trail_zeros = CPU_CntTrailZeros((CPU_DATA)val);
 *                       if (nbr_trail_zeros >  (CPU_WORD_SIZE_08  * DEF_OCTET_NBR_BITS)) {
 *                           nbr_trail_zeros -= (CPU_CFG_DATA_SIZE - CPU_WORD_SIZE_08) * DEF_OCTET_NBR_BITS;
 *                       }
 *                   @endverbatim
 *                   However, this ONLY occurs for an initial 'val' of '0' since all non-'0'  8-bit
 *                   values would return a number of trailing zeros less than or equal to  8 bits.
 *                   @n
 *                   Therefore, initially validating all non-'0' values prior to calling assembly-
 *                   optimized CPU_CntTrailZeros() avoids having to offset the number of returned
 *                   trailing zeros by the difference in CPU data size and  8-bit data value bits.
 *           - (b) For CPU_CntTrailZeros08()'s C implementation, the final conditional statement
 *                   calculates 'val's number of trailing zeros based on CPU_CntTrailZeros08()'s
 *                   8-bit return data size & 'val's calculated number of lead zeros ONLY if the
 *                   initial 'val' is non-'0' :
 *                   @verbatim
 *                       if (val != 0u) {
 *                           nbr_trail_zeros = ((CPU_WORD_SIZE_08 * DEF_OCTET_NBR_BITS) - 1u) - nbr_lead_zeros;
 *                       } else {
 *                           nbr_trail_zeros = nbr_lead_zeros;
 *                       }
 *                   @endverbatim
 *                   Therefore, initially validating all non-'0' values avoids having to conditionally
 *                   execute the final 'if' statement.
 *******************************************************************************************************/

#if (CPU_CFG_DATA_SIZE_MAX >= CPU_WORD_SIZE_08)
CPU_DATA CPU_CntTrailZeros08(CPU_INT08U val)
{
#if  (!((defined(CPU_CFG_TRAIL_ZEROS_ASM_PRESENT)) \
  && (CPU_CFG_DATA_SIZE >= CPU_WORD_SIZE_08)))
  CPU_INT08U val_bit_mask;
  CPU_DATA   nbr_lead_zeros;
#endif
  CPU_DATA nbr_trail_zeros;

  if (val == 0u) {                                              // Rtn ALL val bits as zero'd (see Note #3).
    return (CPU_WORD_SIZE_08 * DEF_OCTET_NBR_BITS);
  }

  //                                                               ------------------ ASM-OPTIMIZED -------------------
#if ((defined(CPU_CFG_TRAIL_ZEROS_ASM_PRESENT)) \
  && (CPU_CFG_DATA_SIZE >= CPU_WORD_SIZE_08))
  nbr_trail_zeros = CPU_CntTrailZeros((CPU_DATA)val);

#else //                                                           ------------------- C-OPTIMIZED --------------------
  val_bit_mask = val & ((CPU_INT08U) ~val + 1u);                // Zero/clr all bits EXCEPT least-sig set bit.
  nbr_lead_zeros = CPU_CntLeadZeros08(val_bit_mask);            // Cnt  nbr lead  0s.
                                                                // Calc nbr trail 0s = (nbr val bits - 1) - nbr lead 0s.
  nbr_trail_zeros = ((CPU_WORD_SIZE_08 * DEF_OCTET_NBR_BITS) - 1u) - nbr_lead_zeros;
#endif

  return (nbr_trail_zeros);
}
#endif

/****************************************************************************************************//**
 *                                           CPU_CntTrailZeros16()
 *
 * @brief    Count the number of contiguous, least-significant, trailing zero bits in a 16-bit data value.
 *
 * @param    val     Data value to count trailing zero bits.
 *
 * @return   Number of contiguous, least-significant, trailing zero bits in 'val'.
 *
 * @note     (1) Supports 16-bit values :
 *               @verbatim
 *                       b15  b14  b13  b12  b11  ...  b02  b01  b00    # Trailing Zeros
 *                       ---  ---  ---  ---  ---       ---  ---  ---    ----------------
 *                       x    x    x    x    x         x    x    1            0
 *                       x    x    x    x    x         x    1    0            1
 *                       x    x    x    x    x         1    0    0            2
 *                       :    :    :    :    :         :    :    :            :
 *                       :    :    :    :    :         :    :    :            :
 *                       x    x    x    x    1         0    0    0           11
 *                       x    x    x    1    0         0    0    0           12
 *                       x    x    1    0    0         0    0    0           13
 *                       x    1    0    0    0         0    0    0           14
 *                       1    0    0    0    0         0    0    0           15
 *                       0    0    0    0    0         0    0    0           16
 *               @endverbatim
 *
 * @note     (2) For non-zero values, the returned number of contiguous, least-significant, trailing
 *               zero bits is also equivalent to the bit position of the least-significant set bit.
 *
 * @note     (3) 'val' SHOULD be validated for non-'0' PRIOR to all other counting zero calculations :
 *           - (a) For assembly-optimized implementations, CPU_CntTrailZeros() returns 'val's
 *                   number of trailing zeros via CPU's native data size, 'CPU_CFG_DATA_SIZE'.
 *                   If the returned number of zeros exceeds CPU_CntTrailZeros16()'s 16-bit return
 *                   data size, then the returned number of zeros must be offset by the difference
 *                   between CPU_CntTrailZeros()'s  & CPU_CntTrailZeros16()'s return data size :
 *               @verbatim
 *                       nbr_trail_zeros = CPU_CntTrailZeros((CPU_DATA)val);
 *                       if (nbr_trail_zeros >  (CPU_WORD_SIZE_16  * DEF_OCTET_NBR_BITS)) {
 *                           nbr_trail_zeros -= (CPU_CFG_DATA_SIZE - CPU_WORD_SIZE_16) * DEF_OCTET_NBR_BITS;
 *                       }
 *               @endverbatim
 *                   However, this ONLY occurs for an initial 'val' of '0' since all non-'0' 16-bit
 *                   values would return a number of trailing zeros less than or equal to 16 bits.
 *                   @n
 *                   Therefore, initially validating all non-'0' values prior to calling assembly-
 *                   optimized CPU_CntTrailZeros() avoids having to offset the number of returned
 *                   trailing zeros by the difference in CPU data size and 16-bit data value bits.
 *           - (b) For CPU_CntTrailZeros16()'s C implementation, the final conditional statement
 *                   calculates 'val's number of trailing zeros based on CPU_CntTrailZeros16()'s
 *                   16-bit return data size & 'val's calculated number of lead zeros ONLY if the
 *                   initial 'val' is non-'0' :
 *                   @verbatim
 *                       if (val != 0u) {
 *                           nbr_trail_zeros = ((CPU_WORD_SIZE_16 * DEF_OCTET_NBR_BITS) - 1u) - nbr_lead_zeros;
 *                       } else {
 *                           nbr_trail_zeros = nbr_lead_zeros;
 *                       }
 *                   @endverbatim
 *                   Therefore, initially validating all non-'0' values avoids having to conditionally
 *                   execute the final 'if' statement.
 *******************************************************************************************************/

#if (CPU_CFG_DATA_SIZE_MAX >= CPU_WORD_SIZE_16)
CPU_DATA CPU_CntTrailZeros16(CPU_INT16U val)
{
#if  (!((defined(CPU_CFG_TRAIL_ZEROS_ASM_PRESENT)) \
  && (CPU_CFG_DATA_SIZE >= CPU_WORD_SIZE_16)))
  CPU_INT16U val_bit_mask;
  CPU_DATA   nbr_lead_zeros;
#endif
  CPU_DATA nbr_trail_zeros;

  if (val == 0u) {                                              // Rtn ALL val bits as zero'd (see Note #3).
    return (CPU_WORD_SIZE_16 * DEF_OCTET_NBR_BITS);
  }

  //                                                               ------------------ ASM-OPTIMIZED -------------------
#if ((defined(CPU_CFG_TRAIL_ZEROS_ASM_PRESENT)) \
  && (CPU_CFG_DATA_SIZE >= CPU_WORD_SIZE_16))
  nbr_trail_zeros = CPU_CntTrailZeros((CPU_DATA)val);

#else //                                                           ------------------- C-OPTIMIZED --------------------
  val_bit_mask = val & ((CPU_INT16U) ~val + 1u);                // Zero/clr all bits EXCEPT least-sig set bit.
  nbr_lead_zeros = CPU_CntLeadZeros16(val_bit_mask);            // Cnt  nbr lead  0s.
                                                                // Calc nbr trail 0s = (nbr val bits - 1) - nbr lead 0s.
  nbr_trail_zeros = ((CPU_WORD_SIZE_16 * DEF_OCTET_NBR_BITS) - 1u) - nbr_lead_zeros;
#endif

  return (nbr_trail_zeros);
}
#endif

/****************************************************************************************************//**
 *                                           CPU_CntTrailZeros32()
 *
 * @brief    Count the number of contiguous, least-significant, trailing zero bits in a 32-bit data value.
 *
 * @param    val     Data value to count trailing zero bits.
 *
 * @return   Number of contiguous, least-significant, trailing zero bits in 'val'.
 *
 * @note     (1) Supports 32-bit values :
 *               @verbatim
 *                       b31  b30  b29  b28  b27  ...  b02  b01  b00    # Trailing Zeros
 *                       ---  ---  ---  ---  ---       ---  ---  ---    ----------------
 *                       x    x    x    x    x         x    x    1            0
 *                       x    x    x    x    x         x    1    0            1
 *                       x    x    x    x    x         1    0    0            2
 *                       :    :    :    :    :         :    :    :            :
 *                       :    :    :    :    :         :    :    :            :
 *                       x    x    x    x    1         0    0    0           27
 *                       x    x    x    1    0         0    0    0           28
 *                       x    x    1    0    0         0    0    0           29
 *                       x    1    0    0    0         0    0    0           30
 *                       1    0    0    0    0         0    0    0           31
 *                       0    0    0    0    0         0    0    0           32
 *               @endverbatim
 *
 * @note     (2) For non-zero values, the returned number of contiguous, least-significant, trailing
 *               zero bits is also equivalent to the bit position of the least-significant set bit.
 *
 * @note     (3) 'val' SHOULD be validated for non-'0' PRIOR to all other counting zero calculations :
 *           - (a) For assembly-optimized implementations, CPU_CntTrailZeros() returns 'val's
 *                   number of trailing zeros via CPU's native data size, 'CPU_CFG_DATA_SIZE'.
 *                   If the returned number of zeros exceeds CPU_CntTrailZeros32()'s 32-bit return
 *                   data size, then the returned number of zeros must be offset by the difference
 *                   between CPU_CntTrailZeros()'s  & CPU_CntTrailZeros32()'s return data size :
 *                   @verbatim
 *                       nbr_trail_zeros = CPU_CntTrailZeros((CPU_DATA)val);
 *                       if (nbr_trail_zeros >  (CPU_WORD_SIZE_32  * DEF_OCTET_NBR_BITS)) {
 *                           nbr_trail_zeros -= (CPU_CFG_DATA_SIZE - CPU_WORD_SIZE_32) * DEF_OCTET_NBR_BITS;
 *                       }
 *                   @endverbatim
 *                   However, this ONLY occurs for an initial 'val' of '0' since all non-'0' 32-bit
 *                   values would return a number of trailing zeros less than or equal to 32 bits.
 *                   @n
 *                   Therefore, initially validating all non-'0' values prior to calling assembly-
 *                   optimized CPU_CntTrailZeros() avoids having to offset the number of returned
 *                   trailing zeros by the difference in CPU data size and 32-bit data value bits.
 *
 *           - (b) For CPU_CntTrailZeros32()'s C implementation, the final conditional statement
 *                   calculates 'val's number of trailing zeros based on CPU_CntTrailZeros32()'s
 *                   32-bit return data size & 'val's calculated number of lead zeros ONLY if the
 *                   initial 'val' is non-'0' :
 *                   @verbatim
 *                       if (val != 0u) {
 *                           nbr_trail_zeros = ((CPU_WORD_SIZE_32 * DEF_OCTET_NBR_BITS) - 1u) - nbr_lead_zeros;
 *                       } else {
 *                           nbr_trail_zeros = nbr_lead_zeros;
 *                       }
 *                   @endverbatim
 *                   Therefore, initially validating all non-'0' values avoids having to conditionally
 *                   execute the final 'if' statement.
 *******************************************************************************************************/

#if (CPU_CFG_DATA_SIZE_MAX >= CPU_WORD_SIZE_32)
CPU_DATA CPU_CntTrailZeros32(CPU_INT32U val)
{
#if  (!((defined(CPU_CFG_TRAIL_ZEROS_ASM_PRESENT)) \
  && (CPU_CFG_DATA_SIZE >= CPU_WORD_SIZE_32)))
  CPU_INT32U val_bit_mask;
  CPU_DATA   nbr_lead_zeros;
#endif
  CPU_DATA nbr_trail_zeros;

  if (val == 0u) {                                              // Rtn ALL val bits as zero'd (see Note #3).
    return (CPU_WORD_SIZE_32 * DEF_OCTET_NBR_BITS);
  }

  //                                                               ------------------ ASM-OPTIMIZED -------------------
#if ((defined(CPU_CFG_TRAIL_ZEROS_ASM_PRESENT)) \
  && (CPU_CFG_DATA_SIZE >= CPU_WORD_SIZE_32))
  nbr_trail_zeros = CPU_CntTrailZeros((CPU_DATA)val);

#else //                                                           ------------------- C-OPTIMIZED --------------------
  val_bit_mask = val & ((CPU_INT32U) ~val + 1u);                // Zero/clr all bits EXCEPT least-sig set bit.
  nbr_lead_zeros = CPU_CntLeadZeros32(val_bit_mask);            // Cnt  nbr lead  0s.
                                                                // Calc nbr trail 0s = (nbr val bits - 1) - nbr lead 0s.
  nbr_trail_zeros = ((CPU_WORD_SIZE_32 * DEF_OCTET_NBR_BITS) - 1u) - nbr_lead_zeros;
#endif

  return (nbr_trail_zeros);
}
#endif

/****************************************************************************************************//**
 *                                           CPU_CntTrailZeros64()
 *
 * @brief    Count the number of contiguous, least-significant, trailing zero bits in a 64-bit data value.
 *
 * @param    val     Data value to count trailing zero bits.
 *
 * @return   Number of contiguous, least-significant, trailing zero bits in 'val'.
 *
 * @note     (1) Supports 64-bit values :
 *               @verbatim
 *                       b63  b62  b61  b60  b59  ...  b02  b01  b00    # Trailing Zeros
 *                       ---  ---  ---  ---  ---       ---  ---  ---    ----------------
 *                       x    x    x    x    x         x    x    1            0
 *                       x    x    x    x    x         x    1    0            1
 *                       x    x    x    x    x         1    0    0            2
 *                       :    :    :    :    :         :    :    :            :
 *                       :    :    :    :    :         :    :    :            :
 *                       x    x    x    x    1         0    0    0           59
 *                       x    x    x    1    0         0    0    0           60
 *                       x    x    1    0    0         0    0    0           61
 *                       x    1    0    0    0         0    0    0           62
 *                       1    0    0    0    0         0    0    0           63
 *                       0    0    0    0    0         0    0    0           64
 *               @endverbatim
 *
 * @note     (2) For non-zero values, the returned number of contiguous, least-significant, trailing
 *               zero bits is also equivalent to the bit position of the least-significant set bit.
 *
 * @note     (3) 'val' SHOULD be validated for non-'0' PRIOR to all other counting zero calculations :
 *           - (a) For assembly-optimized implementations, CPU_CntTrailZeros() returns 'val's
 *                   number of trailing zeros via CPU's native data size, 'CPU_CFG_DATA_SIZE'.
 *                   If the returned number of zeros exceeds CPU_CntTrailZeros64()'s 64-bit return
 *                   data size, then the returned number of zeros must be offset by the difference
 *                   between CPU_CntTrailZeros()'s  & CPU_CntTrailZeros64()'s return data size :
 *                   @verbatim
 *                       nbr_trail_zeros = CPU_CntTrailZeros((CPU_DATA)val);
 *                       if (nbr_trail_zeros >  (CPU_WORD_SIZE_64  * DEF_OCTET_NBR_BITS)) {
 *                           nbr_trail_zeros -= (CPU_CFG_DATA_SIZE - CPU_WORD_SIZE_64) * DEF_OCTET_NBR_BITS;
 *                       }
 *                   @endverbatim
 *                   However, this ONLY occurs for an initial 'val' of '0' since all non-'0' 64-bit
 *                   values would return a number of trailing zeros less than or equal to 64 bits.
 *                   @n
 *                   Therefore, initially validating all non-'0' values prior to calling assembly-
 *                   optimized CPU_CntTrailZeros() avoids having to offset the number of returned
 *                   trailing zeros by the difference in CPU data size and 64-bit data value bits.
 *
 *           - (b) For CPU_CntTrailZeros64()'s C implementation, the final conditional statement
 *                   calculates 'val's number of trailing zeros based on CPU_CntTrailZeros64()'s
 *                   64-bit return data size & 'val's calculated number of lead zeros ONLY if the
 *                   initial 'val' is non-'0' :
 *                   @verbatim
 *                       if (val != 0u) {
 *                           nbr_trail_zeros = ((CPU_WORD_SIZE_64 * DEF_OCTET_NBR_BITS) - 1u) - nbr_lead_zeros;
 *                       } else {
 *                           nbr_trail_zeros = nbr_lead_zeros;
 *                       }
 *                   @endverbatim
 *                   Therefore, initially validating all non-'0' values avoids having to conditionally
 *                   execute the final 'if' statement.
 *******************************************************************************************************/

#if (CPU_CFG_DATA_SIZE_MAX >= CPU_WORD_SIZE_64)
CPU_DATA CPU_CntTrailZeros64(CPU_INT64U val)
{
#if  (!((defined(CPU_CFG_TRAIL_ZEROS_ASM_PRESENT)) \
  && (CPU_CFG_DATA_SIZE >= CPU_WORD_SIZE_64)))
  CPU_INT64U val_bit_mask;
  CPU_DATA   nbr_lead_zeros;
#endif
  CPU_DATA nbr_trail_zeros;

  if (val == 0u) {                                              // Rtn ALL val bits as zero'd (see Note #3).
    return (CPU_WORD_SIZE_64 * DEF_OCTET_NBR_BITS);
  }

  //                                                               ------------------ ASM-OPTIMIZED -------------------
#if ((defined(CPU_CFG_TRAIL_ZEROS_ASM_PRESENT)) \
  && (CPU_CFG_DATA_SIZE >= CPU_WORD_SIZE_64))
  nbr_trail_zeros = CPU_CntTrailZeros((CPU_DATA)val);

#else //                                                           ------------------- C-OPTIMIZED --------------------
  val_bit_mask = val & ((CPU_INT64U) ~val + 1u);                // Zero/clr all bits EXCEPT least-sig set bit.
  nbr_lead_zeros = CPU_CntLeadZeros64(val_bit_mask);            // Cnt  nbr lead  0s.
                                                                // Calc nbr trail 0s = (nbr val bits - 1) - nbr lead 0s.
  nbr_trail_zeros = ((CPU_WORD_SIZE_64 * DEF_OCTET_NBR_BITS) - 1u) - nbr_lead_zeros;
#endif

  return (nbr_trail_zeros);
}
#endif

/****************************************************************************************************//**
 *                                               CPU_PopCnt32()
 *
 * @brief    Compute population count (hamming weight) for value (number of bits set).
 *
 * @param    value   Value to compute population count on.
 *
 * @return   Value's population count.
 *
 * @note     (1) Algorithm taken from http://en.wikipedia.org/wiki/Hamming_weight
 *******************************************************************************************************/
CPU_INT08U CPU_PopCnt32(CPU_INT32U value)
{
  CPU_INT32U even_cnt;
  CPU_INT32U odd_cnt;
  CPU_INT32U result;

  odd_cnt = (value >> 1u) & CRC_UTIL_POPCNT_MASK01010101_32;    // 2-bits pieces.
  result = value - odd_cnt;                                     // Same result as result=odd_cnt+(value & 0x55555555).

  even_cnt = result & CRC_UTIL_POPCNT_MASK00110011_32;          // 4-bits pieces.
  odd_cnt = (result >> 2u) & CRC_UTIL_POPCNT_MASK00110011_32;
  result = even_cnt + odd_cnt;

  even_cnt = result & CRC_UTIL_POPCNT_MASK00001111_32;          // 8-bits pieces.
  odd_cnt = (result >> 4u) & CRC_UTIL_POPCNT_MASK00001111_32;
  result = even_cnt + odd_cnt;

  result = (result * CRC_UTIL_POPCNT_POWERSOF256_32) >> 24u;

  return ((CPU_INT08U)result);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               CPU_NameInit()
 *
 * @brief    Initialize CPU Name.
 *******************************************************************************************************/

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
static void CPU_NameInit(void)
{
  CPU_NameClr();
}
#endif

/****************************************************************************************************//**
 *                                               CPU_TS_Init()
 *
 * @brief    (1) Initialize CPU timestamp :
 *               - (a) Initialize/start CPU timestamp timer                            See Note #1
 *               - (b) Initialize       CPU timestamp controls
 *
 * @note     (1) The following initialization MUST be sequenced as follows :
 *               - (a) CPU_TS_TmrFreq_Hz     MUST be initialized prior to CPU_TS_TmrInit()
 *               - (b) CPU_TS_TmrInit()      SHOULD precede calls to all other CPU timestamp functions;
 *                                               otherwise, invalid time measurements may be calculated/
 *                                               returned.
 *               See also 'CPU_Init()  Note #3a'.
 *******************************************************************************************************/

#if ((CPU_CFG_TS_EN == DEF_ENABLED) \
  || (CPU_CFG_TS_TMR_EN == DEF_ENABLED))
static void CPU_TS_Init(void)
{
#if (((CPU_CFG_TS_32_EN == DEF_ENABLED)        \
  && (CPU_CFG_TS_TMR_SIZE < CPU_WORD_SIZE_32)) \
  || ((CPU_CFG_TS_64_EN == DEF_ENABLED)        \
  && (CPU_CFG_TS_TMR_SIZE < CPU_WORD_SIZE_64)))
  CPU_TS_TMR ts_tmr_cnts;
#endif

  //                                                               ----------------- INIT CPU TS TMR ------------------
#if (CPU_CFG_TS_TMR_EN == DEF_ENABLED)
  CPU_TS_TmrFreq_Hz = 0u;                                       // Init/clr     ts tmr freq (see Note #1a).
  CPU_TS_TmrInit();                                             // Init & start ts tmr      (see Note #1b).
#endif

  //                                                               ------------------- INIT CPU TS --------------------
#if (((CPU_CFG_TS_32_EN == DEF_ENABLED)        \
  && (CPU_CFG_TS_TMR_SIZE < CPU_WORD_SIZE_32)) \
  || ((CPU_CFG_TS_64_EN == DEF_ENABLED)        \
  && (CPU_CFG_TS_TMR_SIZE < CPU_WORD_SIZE_64)))
  ts_tmr_cnts = CPU_TS_TmrRd();                                 // Get init ts tmr val (in ts tmr cnts).
#endif

#if  ((CPU_CFG_TS_32_EN == DEF_ENABLED) \
  && (CPU_CFG_TS_TMR_SIZE < CPU_WORD_SIZE_32))
  CPU_TS_32_Accum = 0u;                                         // Init 32-bit accum'd ts.
  CPU_TS_32_TmrPrev = ts_tmr_cnts;                              // Init 32-bit ts prev tmr val.
#endif

#if  ((CPU_CFG_TS_64_EN == DEF_ENABLED) \
  && (CPU_CFG_TS_TMR_SIZE < CPU_WORD_SIZE_64))
  CPU_TS_64_Accum = 0u;                                         // Init 64-bit accum'd ts.
  CPU_TS_64_TmrPrev = ts_tmr_cnts;                              // Init 64-bit ts prev tmr val.
#endif
}
#endif

/****************************************************************************************************//**
 *                                         CPU_RevBits()
 * @brief    Reverses the bits in a data value.
 *
 * @param    val     Data value to reverse bits.
 *
 * @return   Value with all bits in 'val' reversed (see Note #1).
 *
 *               This function is an INTERNAL CPU module function but MAY be called by application function(s).
 *
 * @note     (1) val is a 32-bit number
 *
 * @note     (2) Goes through a number and checks for sets bits which are then set
 *               in the reverse locations:
 *
 *               reverse_val                 =>     0b00000....00
 *               val                         =>     0101100....10
 *               val's 2nd bit is set        =>     reverse_val's bit (num_bits - 1 - count)
 *               val's 5th bit is set        =>     reverse_val's bit (num_bits - 1 - count)
 *               ...                                ...
 *               ...                                ...
 *
 *******************************************************************************************************/

#ifndef  CPU_CFG_REVERSE_BIT_ASM_PRESENT
CPU_DATA CPU_RevBits(CPU_DATA val)
{
  CPU_DATA   reverse_val;
  CPU_INT08U nbr_bits;                                          // establish how many bits are in val
  CPU_INT32U cnt;                                               // for stepping through each bit in val
  CPU_INT32U tmp;                                               // gets shifted off bit to check if set or not

  nbr_bits = sizeof(CPU_DATA) * 8;
  reverse_val = 0;                                              // make sure reverse_val is cleared out to zeros

  for (cnt = 0; cnt < nbr_bits; cnt++) {
    tmp = (val & (1 << cnt));                                   // shift the next bit into tmp

    if (tmp) {
      reverse_val |= (1 << ((nbr_bits - 1) - cnt));             // shift in a 1 bit to reverse equivalent bit
    }
  }

  return (reverse_val);
}
#endif
