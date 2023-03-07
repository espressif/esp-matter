/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* includes */
#include <ti/display/lcd/LCDDogm1286_util.h>

/*
 * ======== LCD_getStringLength ========
 */
unsigned char LCD_getStringLength(const char *pcStr)
{
    unsigned char ucI = 0;
    while (pcStr[ucI] != '\0')
    {
        ucI++;
    }
    return (ucI);
}

/*
 * ======== LCD_getIntLength ========
 */
unsigned char LCD_getIntLength(int i32Number, unsigned char uiRadix)
{
    unsigned char ucNumOfDigits = 0;

    if (i32Number == 0)
    {
        /* The character zero also takes up one place. */
        return (1);
    }

    if (i32Number < 0)
    {
        /* Add one character to length (for minus sign) */
        i32Number *= (-1);
        ucNumOfDigits++;
    }

    /* Count the number of digits used and return number.*/
    while (i32Number >= 1)
    {
        i32Number /= uiRadix;
        ucNumOfDigits++;
    }
    return (ucNumOfDigits);
}

/*
 * ======== LCD_getFloatLength ========
 */
unsigned char LCD_getFloatLength(float fNumber, unsigned char ucDecimals)
{
    unsigned char ucI;
    unsigned char ucNumOfDigits = 0;

    /* fThreshold defines how small a Float must be to be considered negative.
     * For example, if a Float is -0.001 and the number of decimals is 2,
     * then the number will be considered as 0.00 and not -0.00
     */
    float fThreshold = -0.5;
    for (ucI = 0; ucI < ucDecimals; ucI++)
    {
        fThreshold *= 0.1;
    }

    if (fNumber <= fThreshold)
    {
        /* Add one character for minus sign if number is negative */
        ucNumOfDigits++;

        /* Work only with positive part afterwards */
        fNumber *= (-1);
    }

    /* Get character count of integer part, comma and decimal part */
    ucNumOfDigits += LCD_getIntLength((int) fNumber, 10);
    if (ucDecimals)
    {
        ucNumOfDigits++;
    }
    ucNumOfDigits += ucDecimals;

    /* Return character count */
    return (ucNumOfDigits);
}

/*!
 *  ======== _itoa ========
 */
void _itoa(unsigned int uiNum, unsigned char *buf, unsigned char uiRadix)
{
    unsigned char c, i;
    unsigned char *p, rst[32];

    p = rst;
    for (i = 0; i < 32; i++, p++)
    {
        /* Isolate a digit */
        c = uiNum % uiRadix;
        /* Convert to Ascii */
        *p = c + ((c < 10) ? '0' : '7');
        uiNum /= uiRadix;
        if (!uiNum)
            break;
    }

    for (c = 0; c <= i; c++)
    {
        /* Reverse character order */
        *buf++ = *p--;
    }
    *buf = '\0';
}
