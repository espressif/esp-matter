/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/********************** Include Files **********************************/
#include "cc_common_error.h"
#include "cc_common_math.h"

/************************ Defines **************************************/

/************************ Enums ****************************************/

/************************ Typedefs *************************************/

/************************ Global Data **********************************/

/************* Private function prototype ******************************/

/************************ Public Functions *****************************/


/********************************************************************************
 * @brief This function adds a value to a large counter presented in a buffer.
 *        The MSB of the counter is stored in the first cell in the array.
 *
 *        for example:
 *
 *        a counter of 64 bit : the value is :
 *
 *        byte[0] << 56 | byte[1] << 48 ............ byte[6] << 8 | byte[7]
 *
 * @param[in] CounterBuff_ptr - The buffer containing the counter.
 * @param[in] Val             - this value to add.
 * @param[in] CounterSize      - the counter size in 32bit words.
 *
 * @return CCError_t - On success CC_OK is returned, on failure a
 *                        value MODULE_* as defined in ...
 */

void HASH_COMMON_IncMsbUnsignedCounter( uint32_t     *CounterBuff_ptr ,
                    uint32_t      Val,
                    uint32_t       CounterSize )
{
    /* FUNCTION LOCAL DECLERATIONS */

    /* a value for storing the current counter word value */
    uint32_t curretCounterWordVal;

    /* loop variable */
    uint32_t i;

    /* FUNCTION LOGIC */
    /* .. inversing the counters bytes to a word in little endian format. */
    /* ------------------------------------------------------------------ */
    for (i = 0 ; i < CounterSize ; i++)
        CounterBuff_ptr[i] = CC_COMMON_REVERSE32( CounterBuff_ptr[i] );

    /* .................... initialize local variables .................. */
    /* ------------------------------------------------------------------ */

    /* initialize the current local counter value to the first word */
    curretCounterWordVal = CounterBuff_ptr[CounterSize-1];

    /* .................... adding the value to the LSW ................. */
    /* ------------------------------------------------------------------ */

    /* adding the value to the word */
    CounterBuff_ptr[CounterSize-1] += Val;

    /* .................... adding the carry to the higher words ........ */
    /* ------------------------------------------------------------------ */

    /* if there is overflow on the word then handle the upper words */
    if (curretCounterWordVal > CounterBuff_ptr[CounterSize-1]) {
        /* adding the carry to the counter loop */
        i = CounterSize - 2;
        while (1) {
            /* set the current word value */
            curretCounterWordVal = CounterBuff_ptr[i];

            /* adding the carry to the current word */
            CounterBuff_ptr[i]++;

            /* if there is no overflow on the current word after adding the value
               exit the loop */
            if ((curretCounterWordVal < CounterBuff_ptr[i]) || (i == 0)) {
                break;
            }
            i--;

        }/* end of adding the carry loop */

    }/* end of setting the carrier on the upper words */

    /* .. restore the counters bytes order */
    /* ----------------------------------- */

    for (i = 0 ; i < CounterSize ; i++)
        CounterBuff_ptr[i] = CC_COMMON_REVERSE32( CounterBuff_ptr[i] );


    return;

}/* END OF HASH_COMMON_IncMsbUnsignedCounter */

/********************************************************************************
 * @brief This function adds a value to a large counter presented in a buffer.
 *        The LSB of the counter is stored in the first cell in the array.
 *
 *        for example:
 *
 *        a counter of 64 bit : the value is :
 *
 *        byte[7] << 56 | byte[6] << 48 ............ byte[1] << 8 | byte[0]
 *
 * @param[in] CounterBuff_ptr - The buffer containing the counter.
 * @param[in] Val             - this value to add.
 * @param[in] CounterSize      - the counter size in 32bit words.
 *
 * @return carry bit from MS word if carry occur
 *
 */

uint32_t HASH_COMMON_IncLsbUnsignedCounter(
                      uint32_t     *CounterBuff_ptr ,
                      uint32_t      Val,
                      uint32_t       CounterSize )
{
    /* FUNCTION LOCAL DECLERATIONS */

    /* a value for storing the current counter word value */
    uint32_t curretCounterWordVal;

    /* loop variable */
    uint32_t i;

    /* carry bit */
    uint32_t  carry = 0;

    /* FUNCTION LOGIC */

    /* .................... initialize local variables .................. */
    /* ------------------------------------------------------------------ */

    /* initialize the current local counter value to the first word */
    curretCounterWordVal = CounterBuff_ptr[0];

    /* .................... adding the value to the LSW ................. */
    /* ------------------------------------------------------------------ */

    /* adding the value to the word */
    CounterBuff_ptr[0] += Val;

    /* .................... adding the carry to the higher words ........ */
    /* ------------------------------------------------------------------ */

    /* if there is overflow on the word then handle the upper words */
    if (curretCounterWordVal > CounterBuff_ptr[0]) {
        /* adding the carry to the counter loop */
        for (i = 1 ; i < CounterSize ; i++) {
            /* set the current word value */
            curretCounterWordVal = CounterBuff_ptr[i];

            /* adding the carry to the current word */
            CounterBuff_ptr[i]++;

            /* if there is no overflow on the current word after adding the value
               exit the loop */
            if (curretCounterWordVal < CounterBuff_ptr[i]) {
                carry = 0;
                break;
            } else
                carry = 1;

        }/* end of adding the carry loop */

    }/* end of setting the carrier on the upper words */

    return carry;

}/* END OF HASH_COMMON_IncLsbUnsignedCounter */

