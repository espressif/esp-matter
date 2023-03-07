/***************************************************************************//**
 * @file
 * @brief Common - Mathematic Operations
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
 * @defgroup LIB_MATH LIB Math API
 * @ingroup  LIB
 * @brief      LIB Math API
 *
 * @addtogroup LIB_MATH
 * @{
 ********************************************************************************************************
 * @note     (1) The Institute of Electrical and Electronics Engineers and The Open Group, have given us
 *               permission to reprint portions of their documentation. Portions of this text are
 *               reprinted and reproduced in electronic form from the IEEE Std 1003.1, 2004 Edition,
 *               Standard for Information Technology -- Portable Operating System Interface (POSIX), The
 *               Open Group Base Specifications Issue 6, Copyright (C) 2001-2004 by the Institute of
 *               Electrical and Electronics Engineers, Inc and The Open Group. In the event of any
 *               discrepancy between these versions and the original IEEE and The Open Group Standard, the
 *               original IEEE and The Open Group Standard is the referee document. The original Standard
 *               can be obtained online at http://www.opengroup.org/unix/online.html.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _LIB_MATH_H_
#define  _LIB_MATH_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           RANDOM NUMBER DEFINES
 *
 * Note(s) : (1) (a) IEEE Std 1003.1, 2004 Edition, Section 'rand() : DESCRIPTION' states that "if rand()
 *                   is called before any calls to srand() are made, the same sequence shall be generated
 *                   as when srand() is first called with a seed value of 1".
 *
 *               (b) (1) BSD/ANSI-C implements rand() as a Linear Congruential Generator (LCG) :
 *
 *                       (A) random_number       =  [(a * random_number ) + b]  modulo m
 *                                           n + 1                        n
 *
 *                               where
 *                                       (1) (a) random_number       Next     random number to generate
 *                                                               n+1
 *                                           (b) random_number       Previous random number    generated
 *                                                               n
 *                                           (c) random_number       Initial  random number seed
 *                                                               0                      See also Note #1a
 *
 *                                       (2) a =   1103515245        LCG multiplier
 *                                       (3) b =        12345        LCG incrementor
 *                                       (4) m = RAND_MAX + 1        LCG modulus     See also Note #1b2
 *
 *                   (2) (A) IEEE Std 1003.1, 2004 Edition, Section 'rand() : DESCRIPTION' states that
 *                           "rand() ... shall compute a sequence of pseudo-random integers in the range
 *                           [0, {RAND_MAX}] with a period of at least 2^32".
 *
 *                       (B) However, BSD/ANSI-C 'stdlib.h' defines "RAND_MAX" as "0x7fffffff", or 2^31;
 *                           which therefore limits the range AND period to no more than 2^31.
 *******************************************************************************************************/

#define  RAND_SEED_INIT_VAL                                1u   // See Note #1a.

#define  RAND_LCG_PARAM_M                         0x7FFFFFFFu   // See Note #1b2B.
#define  RAND_LCG_PARAM_A                         1103515245u   // See Note #1b1A2.
#define  RAND_LCG_PARAM_B                              12345u   // See Note #1b1A3.

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       RANDOM NUMBER DATA TYPE
 *******************************************************************************************************/

typedef CPU_INT32U RAND_NBR;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               MATH_IS_PWR2()
 *
 * @brief    Determine if a value is a power of 2.
 *
 * @param    nbr     Value.
 *
 * @return   DEF_YES, 'nbr' is a power of 2.
 *           DEF_NO,  'nbr' is not a power of 2.
 *******************************************************************************************************/

#define  MATH_IS_PWR2(nbr)                                 ((((nbr) != 0u) && (((nbr) & ((nbr) - 1u)) == 0u)) ? DEF_YES : DEF_NO)

/****************************************************************************************************//**
 *                                           MATH_ROUND_INC_UP_PWR2()
 *
 * @brief    Round value up to the next (power of 2) increment.
 *
 * @param    nbr     Value to round.
 *
 * @param    inc     Increment to use. MUST be a power of 2.
 *
 * @return   Rounded up value.
 *******************************************************************************************************/

#define  MATH_ROUND_INC_UP_PWR2(nbr, inc)                  (((nbr) & ~((inc) - 1)) + (((nbr) & ((inc) - 1)) == 0 ? 0 : (inc)))

/****************************************************************************************************//**
 *                                           MATH_ROUND_INC_UP()
 *
 * @brief    Round value up to the next increment.
 *
 * @param    nbr     Value to round.
 *
 * @param    inc     Increment to use.
 *
 * @return   Rounded up value.
 *******************************************************************************************************/

#define  MATH_ROUND_INC_UP(nbr, inc)                       (((nbr) + ((inc) - 1)) / (inc) * (inc))

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//                                                                 ------------------ RAND NBR FNCTS ------------------
void Math_RandSetSeed(RAND_NBR seed);

RAND_NBR Math_Rand(void);

RAND_NBR Math_RandSeed(RAND_NBR seed);

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPRECATED FUNCTION PROTOTYPES
 *
 *  Note(s) : (1) The following functions are deprecated. Unless already used, these functions should not
 *                   be used, since they will be removed in a future version.
 ********************************************************************************************************
 *******************************************************************************************************/

void Math_Init(void);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of lib math module include.
