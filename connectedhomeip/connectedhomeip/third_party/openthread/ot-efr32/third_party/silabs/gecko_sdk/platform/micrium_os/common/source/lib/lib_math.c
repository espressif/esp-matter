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
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  <common/include/lib_math.h>
#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static RAND_NBR Math_RandSeedCur;                               // Cur rand nbr seed.

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           Math_RandSetSeed()
 *
 * @brief    Sets the current pseudo-random number generator seed.
 *
 * @param    seed    Initial (or current) value to set for the pseudo-random number sequence.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'rand() : DESCRIPTION' states that "srand()
 *               ... uses the argument as a seed for a new sequence of pseudo-random numbers to be
 *               returned by subsequent calls to rand()".
 *
 * @note     (2) 'Math_RandSeedCur' MUST always be accessed exclusively in critical sections.
 *               See also 'Math_Rand()  Note #1b'.
 *******************************************************************************************************/
void Math_RandSetSeed(RAND_NBR seed)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  Math_RandSeedCur = seed;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                               Math_Rand()
 *
 * @brief    Calculates the next pseudo-random number.
 *
 * @return   Next pseudo-random number in the sequence after 'Math_RandSeedCur'.
 *
 * @note     (1) The pseudo-random number generator is implemented as a Linear Congruential
 *               Generator (LCG).
 *               The pseudo-random number generated is in the range [0, RAND_LCG_PARAM_M].
 *               See 'Math_RandSeed()  Note #1'.
 *
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'rand() : DESCRIPTION' states that "rand()
 *               ... need not be reentrant ... [and] is not required to be thread-safe".
 *               However, to implement Math_Rand() as re-entrant; 'Math_RandSeedCur' MUST
 *               always be accessed & updated exclusively in critical sections.
 *               See 'Math_RandSeed()  Note #2'.
 *******************************************************************************************************/
RAND_NBR Math_Rand(void)
{
  RAND_NBR seed;
  RAND_NBR rand_nbr;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  seed = Math_RandSeedCur;
  rand_nbr = Math_RandSeed(seed);
  Math_RandSeedCur = rand_nbr;
  CORE_EXIT_ATOMIC();

  return (rand_nbr);
}

/****************************************************************************************************//**
 *                                               Math_RandSeed()
 *
 * @brief          Calculates the next pseudo-random number.
 *
 * @param          seed    Initial (or current) value for the pseudo-random number sequence.
 *
 * @return        Next pseudo-random number in the sequence after 'seed'.
 *
 * @note     (1) BSD/ANSI-C implements rand() as a Linear Congruential Generator (LCG) :
 *                   @verbatim
 *                       random_number       =  [(a * random_number ) + b]  modulo m
 *                                           n + 1                        n
 *
 *                               where
 *                                       (1) random_number           Next     random number to generate
 *                                                           n+1
 *                                           random_number           Previous random number    generated
 *                                                               n
 *
 *                                       (2) a = RAND_LCG_PARAM_A        LCG multiplier
 *                                       (3) b = RAND_LCG_PARAM_B        LCG incrementor
 *                                       (4) m = RAND_LCG_PARAM_M + 1    LCG modulus
 *                   @endverbatim
 *                   - (a) The pseudo-random number generated is in the range [0, RAND_LCG_PARAM_M].
 *                   See 'lib_math.h  RANDOM NUMBER DEFINES  Note #1'.
 * @note     (2) IEEE Std 1003.1, 2004 Edition, Section 'rand() : DESCRIPTION' states that "rand()
 *                   ... need not be reentrant ... [and] is not required to be thread-safe".
 *                   However, Math_RandSeed() is re-entrant since it calculates the next random number
 *                   using ONLY local variables.
 *******************************************************************************************************/
RAND_NBR Math_RandSeed(RAND_NBR seed)
{
  RAND_NBR rand_nbr;

  rand_nbr = (((RAND_NBR)RAND_LCG_PARAM_A * seed) + (RAND_NBR)RAND_LCG_PARAM_B)  %  ((RAND_NBR)RAND_LCG_PARAM_M + 1u);

  return (rand_nbr);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                       DEPRECATED GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               Math_Init()
 *
 * @brief    Initializes the Mathematic Module.
 *
 * @note     (1) IEEE Std 1003.1, 2004 Edition, Section 'rand() : DESCRIPTION' states that "if rand()
 *               is called before any calls to srand() are made, the same sequence shall be generated
 *               as when srand() is first called with a seed value of 1".
 *
 * @note     (2) The following function is deprecated. Unless already used, it should no longer be
 *               used, since they will be removed in a future version.
 * @deprecated
 *******************************************************************************************************/
void Math_Init(void)
{
  Math_RandSetSeed((RAND_NBR)RAND_SEED_INIT_VAL);               // See Note #1.
}
