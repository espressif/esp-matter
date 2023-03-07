/***************************************************************************//**
 * @file
 * @brief See @ref random for detailed documentation.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup legacyhal
 * @{
 ******************************************************************************/

/** @addtogroup random Random Numbers
 * @brief Functions that provide access to random numbers
 *
 * These functions may be hardware accelerated, though often are not.
 *
 * See random.h for source code.
 *@{
 */

#ifndef __RANDOM_H__
#define __RANDOM_H__

/** @brief Seeds the ::halCommonGetRandom() pseudorandom number
 * generator.
 *
 * Called by the stack during initialization with a seed from the radio.
 *
 * @param seed  A seed for the pseudorandom number generator.
 */
void halStackSeedRandom(uint32_t seed);

/** @brief Runs a standard LFSR to generate pseudorandom numbers.
 *
 * Called by the MAC in the stack to choose random backoff slots.
 *
 * Complicated implementations may improve the MAC's
 * ability to avoid collisions in large networks, but it is \b critical to
 * implement this function to return quickly.
 */
uint16_t halCommonGetRandom(void);

/** @} (end addtogroup random) */
/** @} (end addtogroup legacyhal) */

#endif //__RANDOM_H__
