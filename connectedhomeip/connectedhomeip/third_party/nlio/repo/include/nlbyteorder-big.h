/**
 *    Copyright (c) 2020 nlio Authors. All Rights Reserved.
 *    Copyright 2012-2016 Nest Labs Inc. All Rights Reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/**
 *    @file
 *      This file defines defines functions for performing by value
 *      byte reordering for 16-, 32-, and 64-bit quantities both
 *      to-and-from the target system (i.e. host) byte ordering
 *      to-and-from both little and big endian byte ordering,
 *      specifically for big endian target systems.
 */

#ifndef NLBYTEORDER_BIG_H
#define NLBYTEORDER_BIG_H

/**
 * This conditionally performs, as necessary for the target system, a
 * byte order swap by value of the specified 16-bit value, presumed to
 * be in little endian byte ordering to the target system (i.e. host)
 * byte ordering.
 *
 * Consequently, on little endian target systems, this is a no-op and
 * on big endian target systems, this performs a reordering.
 *
 * @param[in]  inValue  The 16-bit value to be byte order swapped.
 *
 * @return The input value, byte order swapped.
 */
static inline uint16_t nlByteOrderSwap16LittleToHost(uint16_t inValue)
{
    return nlByteOrderValueSwap16(inValue);
}

/**
 * This conditionally performs, as necessary for the target system, a
 * byte order swap by value of the specified 32-bit value, presumed to
 * be in little endian byte ordering to the target system (i.e. host)
 * byte ordering.
 *
 * Consequently, on little endian target systems, this is a no-op and
 * on big endian target systems, this performs a reordering.
 *
 * @param[in]  inValue  The 32-bit value to be byte order swapped.
 *
 * @return The input value, byte order swapped.
 */
static inline uint32_t nlByteOrderSwap32LittleToHost(uint32_t inValue)
{
    return nlByteOrderValueSwap32(inValue);
}

/**
 * This conditionally performs, as necessary for the target system, a
 * byte order swap by value of the specified 64-bit value, presumed to
 * be in little endian byte ordering to the target system (i.e. host)
 * byte ordering.
 *
 * Consequently, on little endian target systems, this is a no-op and
 * on big endian target systems, this performs a reordering.
 *
 * @param[in]  inValue  The 64-bit value to be byte order swapped.
 *
 * @return The input value, byte order swapped.
 */
static inline uint64_t nlByteOrderSwap64LittleToHost(uint64_t inValue)
{
    return nlByteOrderValueSwap64(inValue);
}

/**
 * This conditionally performs, as necessary for the target system, a
 * byte order swap by value of the specified 16-bit value, presumed to
 * be in target system (i.e. host) byte ordering to little endian byte
 * ordering.
 *
 * Consequently, on little endian target systems, this is a no-op and
 * on big endian target systems, this performs a reordering.
 *
 * @param[in]  inValue  The 16-bit value to be byte order swapped.
 *
 * @return The input value, byte order swapped.
 */
static inline uint16_t nlByteOrderSwap16HostToLittle(uint16_t inValue)
{
    return nlByteOrderValueSwap16(inValue);
}

/**
 * This conditionally performs, as necessary for the target system, a
 * byte order swap by value of the specified 32-bit value, presumed to
 * be in target system (i.e. host) byte ordering to little endian byte
 * ordering.
 *
 * Consequently, on little endian target systems, this is a no-op and
 * on big endian target systems, this performs a reordering.
 *
 * @param[in]  inValue  The 32-bit value to be byte order swapped.
 *
 * @return The input value, byte order swapped.
 */
static inline uint32_t nlByteOrderSwap32HostToLittle(uint32_t inValue)
{
    return nlByteOrderValueSwap32(inValue);
}

/**
 * This conditionally performs, as necessary for the target system, a
 * byte order swap by value of the specified 64-bit value, presumed to
 * be in target system (i.e. host) byte ordering to little endian byte
 * ordering.
 *
 * Consequently, on little endian target systems, this is a no-op and
 * on big endian target systems, this performs a reordering.
 *
 * @param[in]  inValue  The 64-bit value to be byte order swapped.
 *
 * @return The input value, byte order swapped.
 */
static inline uint64_t nlByteOrderSwap64HostToLittle(uint64_t inValue)
{
    return nlByteOrderValueSwap64(inValue);
}

/**
 * This conditionally performs, as necessary for the target system, a
 * byte order swap by value of the specified 16-bit value, presumed to
 * be in big endian byte ordering to the target system (i.e. host)
 * byte ordering.
 *
 * Consequently, on little endian target systems, this performs a
 * reordering and on big endian target systems, this is a no-op.
 *
 * @param[in]  inValue  The 16-bit value to be byte order swapped.
 *
 * @return The input value, byte order swapped.
 */
static inline uint16_t nlByteOrderSwap16BigToHost(uint16_t inValue)
{
    return inValue;
}

/**
 * This conditionally performs, as necessary for the target system, a
 * byte order swap by value of the specified 32-bit value, presumed to
 * be in big endian byte ordering to the target system (i.e. host)
 * byte ordering.
 *
 * Consequently, on little endian target systems, this performs a
 * reordering and on big endian target systems, this is a no-op.
 *
 * @param[in]  inValue  The 32-bit value to be byte order swapped.
 *
 * @return The input value, byte order swapped.
 */
static inline uint32_t nlByteOrderSwap32BigToHost(uint32_t inValue)
{
    return inValue;
}

/**
 * This conditionally performs, as necessary for the target system, a
 * byte order swap by value of the specified 64-bit value, presumed to
 * be in big endian byte ordering to the target system (i.e. host)
 * byte ordering.
 *
 * Consequently, on little endian target systems, this performs a
 * reordering and on big endian target systems, this is a no-op.
 *
 * @param[in]  inValue  The 64-bit value to be byte order swapped.
 *
 * @return The input value, byte order swapped.
 */
static inline uint64_t nlByteOrderSwap64BigToHost(uint64_t inValue)
{
    return inValue;
}

/**
 * This conditionally performs, as necessary for the target system, a
 * byte order swap by value of the specified 16-bit value, presumed to
 * be in target system (i.e. host) byte ordering to big endian byte
 * ordering.
 *
 * Consequently, on little endian target systems, this performs a
 * reordering and on big endian target systems, this is a no-op.
 *
 * @param[in]  inValue  The 16-bit value to be byte order swapped.
 *
 * @return The input value, byte order swapped.
 */
static inline uint16_t nlByteOrderSwap16HostToBig(uint16_t inValue)
{
    return inValue;
}

/**
 * This conditionally performs, as necessary for the target system, a
 * byte order swap by value of the specified 32-bit value, presumed to
 * be in target system (i.e. host) byte ordering to big endian byte
 * ordering.
 *
 * Consequently, on little endian target systems, this performs a
 * reordering and on big endian target systems, this is a no-op.
 *
 * @param[in]  inValue  The 32-bit value to be byte order swapped.
 *
 * @return The input value, byte order swapped.
 */
static inline uint32_t nlByteOrderSwap32HostToBig(uint32_t inValue)
{
    return inValue;
}

/**
 * This conditionally performs, as necessary for the target system, a
 * byte order swap by value of the specified 64-bit value, presumed to
 * be in target system (i.e. host) byte ordering to big endian byte
 * ordering.
 *
 * Consequently, on little endian target systems, this performs a
 * reordering and on big endian target systems, this is a no-op.
 *
 * @param[in]  inValue  The 64-bit value to be byte order swapped.
 *
 * @return The input value, byte order swapped.
 */
static inline uint64_t nlByteOrderSwap64HostToBig(uint64_t inValue)
{
    return inValue;
}

#endif /* NLBYTEORDER_BIG_H */

