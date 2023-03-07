/***************************************************************************/
/**
 * @file
 * @brief Data store and fetch routines.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 *
 * https://www.silabs.com/about-us/legal/master-software-license-agreement
 *
 * This software is distributed to you in Source Code format and is governed by
 * the sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/


#include "byte_util.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

uint16_t sl_fetch_low_high_int16u(const uint8_t *contents)
{
    return HIGH_LOW_TO_INT(contents[1], contents[0]);
}

uint16_t sl_fetch_high_low_int16u(const uint8_t *contents)
{
    return HIGH_LOW_TO_INT(contents[0], contents[1]);
}

void sl_store_low_high_int16u(uint8_t *contents, uint16_t value)
{
    contents[0] = BYTE_0(value);
    contents[1] = BYTE_1(value);
}

void sl_store_high_low_int16u(uint8_t *contents, uint16_t value)
{
    contents[0] = BYTE_1(value);
    contents[1] = BYTE_0(value);
}

void sl_store_int32u(bool lowHigh, uint8_t *contents, uint32_t value)
{
    uint8_t ii;
    for (ii = 0; ii < 4; ii++)
    {
        uint8_t index   = (lowHigh ? ii : 3 - ii);
        contents[index] = (uint8_t)(value & 0xFF);
        value           = (value >> 8);
    }
}

void sl_store_int48u(bool lowHigh, uint8_t *contents, uint64_t value)
{
    uint8_t ii;
    for (ii = 0; ii < 6; ii++)
    {
        uint8_t index   = (lowHigh ? ii : 5 - ii);
        contents[index] = (uint8_t)(value & 0xFF);
        value           = (value >> 8);
    }
}

uint32_t sl_fetch_int32u(bool lowHigh, const uint8_t *contents)
{
    uint8_t b0;
    uint8_t b1;
    uint8_t b2;
    uint8_t b3;

    if (lowHigh)
    {
        b0 = contents[3];
        b1 = contents[2];
        b2 = contents[1];
        b3 = contents[0];
    }
    else
    {
        b0 = contents[0];
        b1 = contents[1];
        b2 = contents[2];
        b3 = contents[3];
    }
    return ((((uint32_t)((((uint16_t)b0) << 8) | ((uint16_t)b1))) << 16) |
            (uint32_t)((((uint16_t)b2) << 8) | ((uint16_t)b3)));
}

uint64_t sl_fetch_int48u(bool lowHigh, const uint8_t *contents)
{
    uint8_t bytes[6];

    if (lowHigh)
    {
        sl_reverse_memcpy(bytes, contents, 6);
    }
    else
    {
        memcpy(bytes, contents, 6);
    }

    return (uint64_t)(((uint64_t)bytes[0] << 40) | ((uint64_t)bytes[1] << 32) | ((uint64_t)bytes[2] << 24) |
                      ((uint64_t)bytes[3] << 16) | ((uint64_t)bytes[4] << 8) | ((uint64_t)bytes[5]));
}

bool sl_memory_byte_compare(const uint8_t *bytes, uint8_t count, uint8_t target)
{
    uint8_t i;
    for (i = 0; i < count; i++, bytes++)
    {
        if (*bytes != target)
        {
            return false;
        }
    }
    return true;
}

void sl_reverse_memcpy(uint8_t *dest, const uint8_t *src, uint8_t length)
{
    uint8_t i;
    for (i = 0; i < length; i++)
    {
        dest[i] = src[length - 1 - i];
    }
}

uint16_t sl_strlen(const uint8_t *const string)
{
    uint16_t i = 0;

    if (string != NULL)
    {
        while (string[i] != '\0' && i != 0xFFFF)
        {
            i++;
        }
        assert(i != 0xFFFF);
    }

    return i;
}

int8_t sl_strcmp(const uint8_t *s1, const uint8_t *s2)
{
    uint8_t c1, c2;

    do
    {
        c1 = (uint8_t)*s1++;
        c2 = (uint8_t)*s2++;
        if (c1 == '\0')
        {
            return c1 - c2;
        }
    } while (c1 == c2);
    return c1 - c2;
}

//----------------------------------------------------------------
// Returns length of the matching prefix of x and y in bits.

uint16_t sl_matching_prefix_bit_length(const uint8_t *x, uint16_t xLength, const uint8_t *y, uint16_t yLength)
{
    uint16_t length = (xLength < yLength ? xLength : yLength);
    uint16_t count  = 0;
    for (; count + 8 <= length && *x == *y; x++, y++, count += 8)
    {
        ;
    }
    if (count < length)
    {
        uint8_t mask = 0x80;
        uint8_t same = ~(*x ^ *y);
        for (; count < length && (mask & same); mask >>= 1, count += 1)
        {
            ;
        }
    }
    return count;
}

// Copy the first 'length' bits of the prefix.

void sl_bit_copy(uint8_t *to, const uint8_t *from, uint16_t count)
{
    uint8_t fullBytes = count >> 3;
    uint8_t extraBits = count & 0x07;
    memmove(to, from, fullBytes);
    if (extraBits != 0)
    {
        uint8_t mask  = (0xFF >> extraBits);
        to[fullBytes] = ((to[fullBytes] & mask) | (from[fullBytes] & ~mask));
    }
}

static const uint8_t nibbleBitCounts[] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};

uint8_t sl_bit_count_int32u(uint32_t num)
{
    uint8_t result;

    for (result = 0; num != 0; num >>= 4)
    {
        result += nibbleBitCounts[num & 0x0F];
    }

    return result;
}

// If you change the above function, run this to test it.
void sl_test_bit_count(void)
{
    assert(sl_bit_count_int32u(0) == 0);
    uint32_t i;
    for (i = 1; i; i++)
    {
        uint32_t num   = i;
        uint8_t  count = 0;
        for (; num; num >>= 1)
        {
            count += num & 1;
        }
        assert(sl_bit_count_int32u(i) == count);
    }
}

// This is from http://graphics.stanford.edu/~seander/bithacks.html.
// I've checked that it always gets the right answer.  On my PC it
// runs about four times faster than the above version when run on
// all possible 32-bit values.  But (a) it depends on having a fast
// multiply and (b) it's not the most obvious code in the world.
//
// uint8_t sl_bit_count_int32u(uint32_t num)
// {
//   num -= ((num >> 1) & 0x55555555);
//   num = (num & 0x33333333) + ((num >> 2) & 0x33333333);
//   return (((num + (num >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24;
// }

uint8_t sl_hex_to_int(uint8_t ch)
{
    return ch - (ch >= 'a' ? 'a' - 10 : (ch >= 'A' ? 'A' - 10 : (ch <= '9' ? '0' : 0)));
}

uint32_t sl_reverse_bits_in_each_byte(uint32_t v)
{
    // swap odd and even bits
    v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
    // swap consecutive pairs
    v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
    // swap nibbles ...
    v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
    return v;
}
