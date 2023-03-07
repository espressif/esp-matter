/**
 *    Copyright 2013-2016 Nest Labs Inc. All Rights Reserved.
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
 *      This file defines C functions for safely performing simple,
 *      memory-mapped accesses, potentially to unaligned, aligned, and
 *      unaligned memory locations with byte reordering, specifically
 *      for big endian target systems.
 */

#ifndef NLIO_BYTEORDER_BIG_H
#define NLIO_BYTEORDER_BIG_H

#include <nlio-base.h>
#include <nlbyteorder.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Perform a, potentially unaligned, memory read of the big endian
 * byte ordered 8-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to read
 *                    the 8-bit big endian byte ordered value from.
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  nlIOBigEndianGet8(const void *p)
{
    return nlIOGet8(p);
}

/**
 * Perform a, potentially unaligned, memory read of the big endian
 * byte ordered 16-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to read
 *                    the 16-bit big endian byte ordered value from.
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t nlIOBigEndianGet16(const void *p)
{
    return nlByteOrderSwap16BigToHost(nlIOGet16(p));
}

/**
 * Perform a, potentially unaligned, memory read of the big endian
 * byte ordered 32-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to read
 *                    the 32-bit big endian byte ordered value from.
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t nlIOBigEndianGet32(const void *p)
{
    return nlByteOrderSwap32BigToHost(nlIOGet32(p));
}

/**
 * Perform a, potentially unaligned, memory read of the big endian
 * byte ordered 64-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to read
 *                    the 64-bit big endian byte ordered value from.
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t nlIOBigEndianGet64(const void *p)
{
    return nlByteOrderSwap64BigToHost(nlIOGet64(p));
}

/**
 * Perform a, potentially unaligned, memory write of the target system
 * byte ordered 8-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to write
 *                    the target system byte ordered 8-bit value to in big
 *                    endian byte ordering.
 *
 * @param[in]  v      The 8-bit value to write.
 *
 */
static inline void     nlIOBigEndianPut8(void *p, uint8_t v)
{
    nlIOPut8(p, v);
}

/**
 * Perform a, potentially unaligned, memory write of the target system
 * byte ordered 16-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to write
 *                    the target system byte ordered 16-bit value to in big
 *                    endian byte ordering.
 *
 * @param[in]  v      The 16-bit value to write.
 *
 */
static inline void     nlIOBigEndianPut16(void *p, uint16_t v)
{
    nlIOPut16(p, nlByteOrderSwap16HostToBig(v));
}

/**
 * Perform a, potentially unaligned, memory write of the target system
 * byte ordered 32-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to write
 *                    the target system byte ordered 32-bit value to in big
 *                    endian byte ordering.
 *
 * @param[in]  v      The 32-bit value to write.
 *
 */
static inline void     nlIOBigEndianPut32(void *p, uint32_t v)
{
    nlIOPut32(p, nlByteOrderSwap32HostToBig(v));
}

/**
 * Perform a, potentially unaligned, memory write of the target system
 * byte ordered 64-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to write
 *                    the target system byte ordered 64-bit value to in big
 *                    endian byte ordering.
 *
 * @param[in]  v      The 64-bit value to write.
 *
 */
static inline void     nlIOBigEndianPut64(void *p, uint64_t v)
{
    nlIOPut64(p, nlByteOrderSwap64HostToBig(v));
}

/**
 * Perform a, potentially unaligned, memory read of the big endian
 * byte ordered 8-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 8-bits (1 byte).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 8-bit big endian byte
 *                    ordered value from and to then increment by 8-
 *                    bits (1 byte).
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  nlIOBigEndianRead8(const void **p)
{
    return nlIORead8(p);
}

/**
 * Perform a, potentially unaligned, memory read of the big endian
 * byte ordered 16-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 16-bits (2 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 16-bit big endian byte
 *                    ordered value from and to then increment by 16-
 *                    bits (2 bytes).
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t nlIOBigEndianRead16(const void **p)
{
    return nlByteOrderSwap16BigToHost(nlIORead16(p));
}

/**
 * Perform a, potentially unaligned, memory read of the big endian
 * byte ordered 32-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 32-bits (4 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 32-bit big endian byte
 *                    ordered value from and to then increment by 32-
 *                    bits (4 bytes).
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t nlIOBigEndianRead32(const void **p)
{
    return nlByteOrderSwap32BigToHost(nlIORead32(p));
}

/**
 * Perform a, potentially unaligned, memory read of the big endian
 * byte ordered 64-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 64-bits (8 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 64-bit big endian byte
 *                    ordered value from and to then increment by 64-
 *                    bits (8 bytes).
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t nlIOBigEndianRead64(const void **p)
{
    return nlByteOrderSwap64BigToHost(nlIORead64(p));
}

/**
 * Perform a, potentially unaligned, memory write of the target system
 * byte ordered 8-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[in]  p      A pointer to a pointer address, potentially
 *                    unaligned, to write the target system byte
 *                    ordered 8-bit value to in big endian byte
 *                    ordering and to then increment by 8-bits (1
 *                    byte).
 *
 * @param[in]  v      The 8-bit value to write.
 *
 */
static inline void     nlIOBigEndianWrite8(void **p, uint8_t v)
{
    nlIOWrite8(p, v);
}

/**
 * Perform a, potentially unaligned, memory write of the target system
 * byte ordered 16-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[in]  p      A pointer to a pointer address, potentially
 *                    unaligned, to write the target system byte
 *                    ordered 16-bit value to in big endian byte
 *                    ordering and to then increment by 16-bits (2
 *                    bytes).
 *
 * @param[in]  v      The 16-bit value to write.
 *
 */
static inline void     nlIOBigEndianWrite16(void **p, uint16_t v)
{
    nlIOWrite16(p, nlByteOrderSwap16HostToBig(v));
}

/**
 * Perform a, potentially unaligned, memory write of the target system
 * byte ordered 32-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[in]  p      A pointer to a pointer address, potentially
 *                    unaligned, to write the target system byte
 *                    ordered 32-bit value to in big endian byte
 *                    ordering and to then increment by 16-bits (4
 *                    bytes).
 *
 * @param[in]  v      The 32-bit value to write.
 *
 */
static inline void     nlIOBigEndianWrite32(void **p, uint32_t v)
{
    nlIOWrite32(p, nlByteOrderSwap32HostToBig(v));
}

/**
 * Perform a, potentially unaligned, memory write of the target system
 * byte ordered 64-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[in]  p      A pointer to a pointer address, potentially
 *                    unaligned, to write the target system byte
 *                    ordered 64-bit value to in big endian byte
 *                    ordering and to then increment by 64-bits (8
 *                    bytes).
 *
 * @param[in]  v      The 64-bit value to write.
 *
 */
static inline void     nlIOBigEndianWrite64(void **p, uint64_t v)
{
    nlIOWrite64(p, nlByteOrderSwap64HostToBig(v));
}

/**
 * Perform an aligned memory read of the big endian
 * byte ordered 8-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      An aligned pointer address to read
 *                    the 8-bit big endian byte ordered value from.
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  nlIOBigEndianGetAligned8(const void *p)
{
    return nlIOGetAligned8(p);
}

/**
 * Perform an aligned memory read of the big endian
 * byte ordered 16-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      An aligned pointer address to read
 *                    the 16-bit big endian byte ordered value from.
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t nlIOBigEndianGetAligned16(const void *p)
{
    return nlByteOrderSwap16BigToHost(nlIOGetAligned16(p));
}

/**
 * Perform an aligned memory read of the big endian
 * byte ordered 32-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      An aligned pointer address to read
 *                    the 32-bit big endian byte ordered value from.
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t nlIOBigEndianGetAligned32(const void *p)
{
    return nlByteOrderSwap32BigToHost(nlIOGetAligned32(p));
}

/**
 * Perform an aligned memory read of the big endian
 * byte ordered 64-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      An aligned pointer address to read
 *                    the 64-bit big endian byte ordered value from.
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t nlIOBigEndianGetAligned64(const void *p)
{
    return nlByteOrderSwap64BigToHost(nlIOGetAligned64(p));
}

/**
 * Perform an aligned memory write of the target system
 * byte ordered 8-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[in]  p      An aligned pointer address to write
 *                    the target system byte ordered 8-bit value to in big
 *                    endian byte ordering.
 *
 * @param[in]  v      The 8-bit value to write.
 *
 */
static inline void     nlIOBigEndianPutAligned8(void *p, uint8_t v)
{
    nlIOPutAligned8(p, v);
}

/**
 * Perform an aligned memory write of the target system
 * byte ordered 16-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[in]  p      An aligned pointer address to write
 *                    the target system byte ordered 16-bit value to in big
 *                    endian byte ordering.
 *
 * @param[in]  v      The 16-bit value to write.
 *
 */
static inline void     nlIOBigEndianPutAligned16(void *p, uint16_t v)
{
    nlIOPutAligned16(p, nlByteOrderSwap16HostToBig(v));
}

/**
 * Perform an aligned memory write of the target system
 * byte ordered 32-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[in]  p      An aligned pointer address to write
 *                    the target system byte ordered 32-bit value to in big
 *                    endian byte ordering.
 *
 * @param[in]  v      The 32-bit value to write.
 *
 */
static inline void     nlIOBigEndianPutAligned32(void *p, uint32_t v)
{
    nlIOPutAligned32(p, nlByteOrderSwap32HostToBig(v));
}

/**
 * Perform an aligned memory write of the target system
 * byte ordered 64-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[in]  p      An aligned pointer address to write
 *                    the target system byte ordered 64-bit value to in big
 *                    endian byte ordering.
 *
 * @param[in]  v      The 64-bit value to write.
 *
 */
static inline void     nlIOBigEndianPutAligned64(void *p, uint64_t v)
{
    nlIOPutAligned64(p, nlByteOrderSwap64HostToBig(v));
}

/**
 * Perform an aligned memory read of the big endian
 * byte ordered 8-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 8-bits (1 byte).
 *
 * @param[inout]  p   A pointer to an aligned pointer address, 
 *                    to read the 8-bit big endian byte
 *                    ordered value from and to then increment by 8-
 *                    bits (1 byte).
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  nlIOBigEndianReadAligned8(const void **p)
{
    return nlIOReadAligned8(p);
}

/**
 * Perform an aligned memory read of the big endian
 * byte ordered 16-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 16-bits (2 bytes).
 *
 * @param[inout]  p   A pointer to an aligned pointer address, 
 *                    to read the 16-bit big endian byte
 *                    ordered value from and to then increment by 16-
 *                    bits (2 bytes).
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t nlIOBigEndianReadAligned16(const void **p)
{
    return nlByteOrderSwap16BigToHost(nlIOReadAligned16(p));
}

/**
 * Perform an aligned memory read of the big endian
 * byte ordered 32-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 32-bits (4 bytes).
 *
 * @param[inout]  p   A pointer to an aligned pointer address, 
 *                    to read the 32-bit big endian byte
 *                    ordered value from and to then increment by 32-
 *                    bits (4 bytes).
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t nlIOBigEndianReadAligned32(const void **p)
{
    return nlByteOrderSwap32BigToHost(nlIOReadAligned32(p));
}

/**
 * Perform an aligned memory read of the big endian
 * byte ordered 64-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 64-bits (8 bytes).
 *
 * @param[inout]  p   A pointer to an aligned pointer address, 
 *                    to read the 64-bit big endian byte
 *                    ordered value from and to then increment by 64-
 *                    bits (8 bytes).
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t nlIOBigEndianReadAligned64(const void **p)
{
    return nlByteOrderSwap64BigToHost(nlIOReadAligned64(p));
}

/**
 * Perform an aligned memory write of the target system
 * byte ordered 8-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[inout]  p   A pointer to an aligned pointer address, 
 *                    to write the target system byte
 *                    ordered 8-bit value to in big endian byte
 *                    ordering and to then increment by 8-bits (1
 *                    byte).
 *
 * @param[in]  v      The 8-bit value to write.
 *
 */
static inline void     nlIOBigEndianWriteAligned8(void **p, uint8_t v)
{
    nlIOWriteAligned8(p, v);
}

/**
 * Perform an aligned memory write of the target system
 * byte ordered 16-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[inout]  p   A pointer to an aligned pointer address, 
 *                    to write the target system byte
 *                    ordered 16-bit value to in big endian byte
 *                    ordering and to then increment by 16-bits (2
 *                    bytes).
 *
 * @param[in]  v      The 16-bit value to write.
 *
 */
static inline void     nlIOBigEndianWriteAligned16(void **p, uint16_t v)
{
    nlIOWriteAligned16(p, nlByteOrderSwap16HostToBig(v));
}

/**
 * Perform an aligned memory write of the target system
 * byte ordered 32-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[inout]  p   A pointer to an aligned pointer address, 
 *                    to write the target system byte
 *                    ordered 32-bit value to in big endian byte
 *                    ordering and to then increment by 16-bits (4
 *                    bytes).
 *
 * @param[in]  v      The 32-bit value to write.
 *
 */
static inline void     nlIOBigEndianWriteAligned32(void **p, uint32_t v)
{
    nlIOWriteAligned32(p, nlByteOrderSwap32HostToBig(v));
}

/**
 * Perform an aligned memory write of the target system
 * byte ordered 64-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[inout]  p   A pointer to an aligned pointer address, 
 *                    to write the target system byte
 *                    ordered 64-bit value to in big endian byte
 *                    ordering and to then increment by 64-bits (8
 *                    bytes).
 *
 * @param[in]  v      The 64-bit value to write.
 *
 */
static inline void     nlIOBigEndianWriteAligned64(void **p, uint64_t v)
{
    nlIOWriteAligned64(p, nlByteOrderSwap64HostToBig(v));
}

/**
 * Perform an unaligned memory read of the big endian
 * byte ordered 8-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      An unaligned pointer address to read
 *                    the 8-bit big endian byte ordered value from.
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  nlIOBigEndianGetUnaligned8(const void *p)
{
    return nlIOGetUnaligned8(p);
}

/**
 * Perform an unaligned memory read of the big endian
 * byte ordered 16-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      An unaligned pointer address to read
 *                    the 16-bit big endian byte ordered value from.
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t nlIOBigEndianGetUnaligned16(const void *p)
{
    return nlByteOrderSwap16BigToHost(nlIOGetUnaligned16(p));
}

/**
 * Perform an unaligned memory read of the big endian
 * byte ordered 32-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      An unaligned pointer address to read
 *                    the 32-bit big endian byte ordered value from.
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t nlIOBigEndianGetUnaligned32(const void *p)
{
    return nlByteOrderSwap32BigToHost(nlIOGetUnaligned32(p));
}

/**
 * Perform an unaligned memory read of the big endian
 * byte ordered 64-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      An unaligned pointer address to read
 *                    the 64-bit big endian byte ordered value from.
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t nlIOBigEndianGetUnaligned64(const void *p)
{
    return nlByteOrderSwap64BigToHost(nlIOGetUnaligned64(p));
}

/**
 * Perform an unaligned memory write of the target system
 * byte ordered 8-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[in]  p      An unaligned pointer address to write
 *                    the target system byte ordered 8-bit value to in big
 *                    endian byte ordering.
 *
 * @param[in]  v      The 8-bit value to write.
 *
 */
static inline void     nlIOBigEndianPutUnaligned8(void *p, uint8_t v)
{
    nlIOPutUnaligned8(p, v);
}

/**
 * Perform an unaligned memory write of the target system
 * byte ordered 16-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[in]  p      An unaligned pointer address to write
 *                    the target system byte ordered 16-bit value to in big
 *                    endian byte ordering.
 *
 * @param[in]  v      The 16-bit value to write.
 *
 */
static inline void     nlIOBigEndianPutUnaligned16(void *p, uint16_t v)
{
    nlIOPutUnaligned16(p, nlByteOrderSwap16HostToBig(v));
}

/**
 * Perform an unaligned memory write of the target system
 * byte ordered 32-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[in]  p      An unaligned pointer address to write
 *                    the target system byte ordered 32-bit value to in big
 *                    endian byte ordering.
 *
 * @param[in]  v      The 32-bit value to write.
 *
 */
static inline void     nlIOBigEndianPutUnaligned32(void *p, uint32_t v)
{
    nlIOPutUnaligned32(p, nlByteOrderSwap32HostToBig(v));
}

/**
 * Perform an unaligned memory write of the target system
 * byte ordered 64-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[in]  p      An unaligned pointer address to write
 *                    the target system byte ordered 64-bit value to in big
 *                    endian byte ordering.
 *
 * @param[in]  v      The 64-bit value to write.
 *
 */
static inline void     nlIOBigEndianPutUnaligned64(void *p, uint64_t v)
{
    nlIOPutUnaligned64(p, nlByteOrderSwap64HostToBig(v));
}

/**
 * Perform an unaligned memory read of the big endian
 * byte ordered 8-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 8-bits (1 byte).
 *
 * @param[inout]  p   A pointer to an unaligined pointer address, 
 *                    to read the 8-bit big endian byte
 *                    ordered value from and to then increment by 8-
 *                    bits (1 byte).
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  nlIOBigEndianReadUnaligned8(const void **p)
{
    return nlIOReadUnaligned8(p);
}

/**
 * Perform an unaligned memory read of the big endian
 * byte ordered 16-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 16-bits (2 bytes).
 *
 * @param[inout]  p   A pointer to an unaligined pointer address, 
 *                    to read the 16-bit big endian byte
 *                    ordered value from and to then increment by 16-
 *                    bits (2 bytes).
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t nlIOBigEndianReadUnaligned16(const void **p)
{
    return nlByteOrderSwap16BigToHost(nlIOReadUnaligned16(p));
}

/**
 * Perform an unaligned memory read of the big endian
 * byte ordered 32-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 32-bits (4 bytes).
 *
 * @param[inout]  p   A pointer to an unaligined pointer address, 
 *                    to read the 32-bit big endian byte
 *                    ordered value from and to then increment by 32-
 *                    bits (4 bytes).
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t nlIOBigEndianReadUnaligned32(const void **p)
{
    return nlByteOrderSwap32BigToHost(nlIOReadUnaligned32(p));
}

/**
 * Perform an unaligned memory read of the big endian
 * byte ordered 64-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 64-bits (8 bytes).
 *
 * @param[inout]  p   A pointer to an unaligined pointer address, 
 *                    to read the 64-bit big endian byte
 *                    ordered value from and to then increment by 64-
 *                    bits (8 bytes).
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t nlIOBigEndianReadUnaligned64(const void **p)
{
    return nlByteOrderSwap64BigToHost(nlIOReadUnaligned64(p));
}

/**
 * Perform an unaligned memory write of the target system
 * byte ordered 8-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[inout]  p   A pointer to an unaligined pointer address, 
 *                    to write the target system byte
 *                    ordered 8-bit value to in big endian byte
 *                    ordering and to then increment by 8-bits (1
 *                    byte).
 *
 * @param[in]  v      The 8-bit value to write.
 *
 */
static inline void     nlIOBigEndianWriteUnaligned8(void **p, uint8_t v)
{
    nlIOWriteUnaligned8(p, v);
}

/**
 * Perform an unaligned memory write of the target system
 * byte ordered 16-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[inout]  p   A pointer to an unaligined pointer address, 
 *                    to write the target system byte
 *                    ordered 16-bit value to in big endian byte
 *                    ordering and to then increment by 16-bits (2
 *                    bytes).
 *
 * @param[in]  v      The 16-bit value to write.
 *
 */
static inline void     nlIOBigEndianWriteUnaligned16(void **p, uint16_t v)
{
    nlIOWriteUnaligned16(p, nlByteOrderSwap16HostToBig(v));
}

/**
 * Perform an unaligned memory write of the target system
 * byte ordered 32-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[inout]  p   A pointer to an unaligined pointer address, 
 *                    to write the target system byte
 *                    ordered 32-bit value to in big endian byte
 *                    ordering and to then increment by 32-bits (4
 *                    bytes).
 *
 * @param[in]  v      The 32-bit value to write.
 *
 */
static inline void     nlIOBigEndianWriteUnaligned32(void **p, uint32_t v)
{
    nlIOWriteUnaligned32(p, nlByteOrderSwap32HostToBig(v));
}

/**
 * Perform an unaligned memory write of the target system
 * byte ordered 64-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in big endian byte ordering.
 *
 * @param[inout]  p   A pointer to an unaligined pointer address, 
 *                    to write the target system byte
 *                    ordered 64-bit value to in big endian byte
 *                    ordering and to then increment by 64-bits (8
 *                    bytes).
 *
 * @param[in]  v      The 64-bit value to write.
 *
 */
static inline void     nlIOBigEndianWriteUnaligned64(void **p, uint64_t v)
{
    nlIOWriteUnaligned64(p, nlByteOrderSwap64HostToBig(v));
}

#ifdef __cplusplus
}
#endif

#endif /* NLIO_BYTEORDER_BIG_H */
