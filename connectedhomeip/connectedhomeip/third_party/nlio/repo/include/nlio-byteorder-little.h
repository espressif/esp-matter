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
 *      for little endian target systems.
 */

#ifndef NLIO_BYTEORDER_LITTLE_H
#define NLIO_BYTEORDER_LITTLE_H

#include <nlio-base.h>
#include <nlbyteorder.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Perform a, potentially unaligned, memory read of the little endian
 * byte ordered 8-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to read
 *                    the 8-bit little endian byte ordered value from.
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  nlIOLittleEndianGet8(const void *p)
{
    return nlIOGet8(p);
}

/**
 * Perform a, potentially unaligned, memory read of the little endian
 * byte ordered 16-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to read
 *                    the 16-bit little endian byte ordered value from.
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t nlIOLittleEndianGet16(const void *p)
{
    return nlByteOrderSwap16LittleToHost(nlIOGet16(p));
}

/**
 * Perform a, potentially unaligned, memory read of the little endian
 * byte ordered 32-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to read
 *                    the 32-bit little endian byte ordered value from.
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t nlIOLittleEndianGet32(const void *p)
{
    return nlByteOrderSwap32LittleToHost(nlIOGet32(p));
}

/**
 * Perform a, potentially unaligned, memory read of the little endian
 * byte ordered 64-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to read
 *                    the 64-bit little endian byte ordered value from.
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t nlIOLittleEndianGet64(const void *p)
{
    return nlByteOrderSwap64LittleToHost(nlIOGet64(p));
}

/**
 * Perform a, potentially unaligned, memory write of the target system
 * byte ordered 8-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to write
 *                    the target system byte ordered 8-bit value to in little
 *                    endian byte ordering.
 *
 * @param[in]  v      The 8-bit value to write.
 *
 */
static inline void     nlIOLittleEndianPut8(void *p, uint8_t v)
{
    nlIOPut8(p, v);
}

/**
 * Perform a, potentially unaligned, memory write of the target system
 * byte ordered 16-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to write
 *                    the target system byte ordered 16-bit value to in little
 *                    endian byte ordering.
 *
 * @param[in]  v      The 16-bit value to write.
 *
 */
static inline void     nlIOLittleEndianPut16(void *p, uint16_t v)
{
    nlIOPut16(p, nlByteOrderSwap16HostToLittle(v));
}

/**
 * Perform a, potentially unaligned, memory write of the target system
 * byte ordered 32-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to write
 *                    the target system byte ordered 32-bit value to in little
 *                    endian byte ordering.
 *
 * @param[in]  v      The 32-bit value to write.
 *
 */
static inline void     nlIOLittleEndianPut32(void *p, uint32_t v)
{
    nlIOPut32(p, nlByteOrderSwap32HostToLittle(v));
}

/**
 * Perform a, potentially unaligned, memory write of the target system
 * byte ordered 64-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to write
 *                    the target system byte ordered 64-bit value to in little
 *                    endian byte ordering.
 *
 * @param[in]  v      The 64-bit value to write.
 *
 */
static inline void     nlIOLittleEndianPut64(void *p, uint64_t v)
{
    nlIOPut64(p, nlByteOrderSwap64HostToLittle(v));
}

/**
 * Perform a, potentially unaligned, memory read of the little endian
 * byte ordered 8-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 8-bits (1 byte).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 8-bit little endian byte
 *                    ordered value from and to then increment by 8-
 *                    bits (1 byte).
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  nlIOLittleEndianRead8(const void **p)
{
    return nlIORead8(p);
}

/**
 * Perform a, potentially unaligned, memory read of the little endian
 * byte ordered 16-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 16-bits (2 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 16-bit little endian byte
 *                    ordered value from and to then increment by 16-
 *                    bits (2 bytes).
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t nlIOLittleEndianRead16(const void **p)
{
    return nlByteOrderSwap16LittleToHost(nlIORead16(p));
}

/**
 * Perform a, potentially unaligned, memory read of the little endian
 * byte ordered 32-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 32-bits (4 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 32-bit little endian byte
 *                    ordered value from and to then increment by 32-
 *                    bits (4 bytes).
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t nlIOLittleEndianRead32(const void **p)
{
    return nlByteOrderSwap32LittleToHost(nlIORead32(p));
}

/**
 * Perform a, potentially unaligned, memory read of the little endian
 * byte ordered 64-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 64-bits (8 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 64-bit little endian byte
 *                    ordered value from and to then increment by 64-
 *                    bits (8 bytes).
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t nlIOLittleEndianRead64(const void **p)
{
    return nlByteOrderSwap64LittleToHost(nlIORead64(p));
}

/**
 * Perform a, potentially unaligned, memory write of the target system
 * byte ordered 8-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[in]  p      A pointer to a pointer address, potentially
 *                    unaligned, to write the target system byte
 *                    ordered 8-bit value to in little endian byte
 *                    ordering and to then increment by 8-bits (1
 *                    byte).
 *
 * @param[in]  v      The 8-bit value to write.
 *
 */
static inline void     nlIOLittleEndianWrite8(void **p, uint8_t v)
{
    nlIOWrite8(p, v);
}

/**
 * Perform a, potentially unaligned, memory write of the target system
 * byte ordered 16-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[in]  p      A pointer to a pointer address, potentially
 *                    unaligned, to write the target system byte
 *                    ordered 16-bit value to in little endian byte
 *                    ordering and to then increment by 16-bits (2
 *                    bytes).
 *
 * @param[in]  v      The 16-bit value to write.
 *
 */
static inline void     nlIOLittleEndianWrite16(void **p, uint16_t v)
{
    nlIOWrite16(p, nlByteOrderSwap16HostToLittle(v));
}

/**
 * Perform a, potentially unaligned, memory write of the target system
 * byte ordered 32-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[in]  p      A pointer to a pointer address, potentially
 *                    unaligned, to write the target system byte
 *                    ordered 32-bit value to in little endian byte
 *                    ordering and to then increment by 16-bits (4
 *                    bytes).
 *
 * @param[in]  v      The 32-bit value to write.
 *
 */
static inline void     nlIOLittleEndianWrite32(void **p, uint32_t v)
{
    nlIOWrite32(p, nlByteOrderSwap32HostToLittle(v));
}

/**
 * Perform a, potentially unaligned, memory write of the target system
 * byte ordered 64-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[in]  p      A pointer to a pointer address, potentially
 *                    unaligned, to write the target system byte
 *                    ordered 64-bit value to in little endian byte
 *                    ordering and to then increment by 64-bits (8
 *                    bytes).
 *
 * @param[in]  v      The 64-bit value to write.
 *
 */
static inline void     nlIOLittleEndianWrite64(void **p, uint64_t v)
{
    nlIOWrite64(p, nlByteOrderSwap64HostToLittle(v));
}

/**
 * Perform an aligned memory read of the little endian
 * byte ordered 8-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      An aligned pointer address to read
 *                    the 8-bit little endian byte ordered value from.
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  nlIOLittleEndianGetAligned8(const void *p)
{
    return nlIOGetAligned8(p);
}

/**
 * Perform an aligned memory read of the little endian
 * byte ordered 16-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      An aligned pointer address to read
 *                    the 16-bit little endian byte ordered value from.
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t nlIOLittleEndianGetAligned16(const void *p)
{
    return nlByteOrderSwap16LittleToHost(nlIOGetAligned16(p));
}

/**
 * Perform an aligned memory read of the little endian
 * byte ordered 32-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      An aligned pointer address to read
 *                    the 32-bit little endian byte ordered value from.
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t nlIOLittleEndianGetAligned32(const void *p)
{
    return nlByteOrderSwap32LittleToHost(nlIOGetAligned32(p));
}

/**
 * Perform an aligned memory read of the little endian
 * byte ordered 64-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      An aligned pointer address to read
 *                    the 64-bit little endian byte ordered value from.
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t nlIOLittleEndianGetAligned64(const void *p)
{
    return nlByteOrderSwap64LittleToHost(nlIOGetAligned64(p));
}

/**
 * Perform an aligned memory write of the target system
 * byte ordered 8-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[in]  p      An aligned pointer address to write
 *                    the target system byte ordered 8-bit value to in little
 *                    endian byte ordering.
 *
 * @param[in]  v      The 8-bit value to write.
 *
 */
static inline void     nlIOLittleEndianPutAligned8(void *p, uint8_t v)
{
    nlIOPutAligned8(p, v);
}

/**
 * Perform an aligned memory write of the target system
 * byte ordered 16-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[in]  p      An aligned pointer address to write
 *                    the target system byte ordered 16-bit value to in little
 *                    endian byte ordering.
 *
 * @param[in]  v      The 16-bit value to write.
 *
 */
static inline void     nlIOLittleEndianPutAligned16(void *p, uint16_t v)
{
    nlIOPutAligned16(p, nlByteOrderSwap16HostToLittle(v));
}

/**
 * Perform an aligned memory write of the target system
 * byte ordered 32-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[in]  p      An aligned pointer address to write
 *                    the target system byte ordered 32-bit value to in little
 *                    endian byte ordering.
 *
 * @param[in]  v      The 32-bit value to write.
 *
 */
static inline void     nlIOLittleEndianPutAligned32(void *p, uint32_t v)
{
    nlIOPutAligned32(p, nlByteOrderSwap32HostToLittle(v));
}

/**
 * Perform an aligned memory write of the target system
 * byte ordered 64-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[in]  p      An aligned pointer address to write
 *                    the target system byte ordered 64-bit value to in little
 *                    endian byte ordering.
 *
 * @param[in]  v      The 64-bit value to write.
 *
 */
static inline void     nlIOLittleEndianPutAligned64(void *p, uint64_t v)
{
    nlIOPutAligned64(p, nlByteOrderSwap64HostToLittle(v));
}

/**
 * Perform an aligned memory read of the little endian
 * byte ordered 8-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 8-bits (1 byte).
 *
 * @param[inout]  p   A pointer to an aligned pointer address, 
 *                    to read the 8-bit little endian byte
 *                    ordered value from and to then increment by 8-
 *                    bits (1 byte).
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  nlIOLittleEndianReadAligned8(const void **p)
{
    return nlIOReadAligned8(p);
}

/**
 * Perform an aligned memory read of the little endian
 * byte ordered 16-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 16-bits (2 bytes).
 *
 * @param[inout]  p   A pointer to an aligned pointer address, 
 *                    to read the 16-bit little endian byte
 *                    ordered value from and to then increment by 16-
 *                    bits (2 bytes).
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t nlIOLittleEndianReadAligned16(const void **p)
{
    return nlByteOrderSwap16LittleToHost(nlIOReadAligned16(p));
}

/**
 * Perform an aligned memory read of the little endian
 * byte ordered 32-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 32-bits (4 bytes).
 *
 * @param[inout]  p   A pointer to an aligned pointer address, 
 *                    to read the 32-bit little endian byte
 *                    ordered value from and to then increment by 32-
 *                    bits (4 bytes).
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t nlIOLittleEndianReadAligned32(const void **p)
{
    return nlByteOrderSwap32LittleToHost(nlIOReadAligned32(p));
}

/**
 * Perform an aligned memory read of the little endian
 * byte ordered 64-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 64-bits (8 bytes).
 *
 * @param[inout]  p   A pointer to an aligned pointer address, 
 *                    to read the 64-bit little endian byte
 *                    ordered value from and to then increment by 64-
 *                    bits (8 bytes).
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t nlIOLittleEndianReadAligned64(const void **p)
{
    return nlByteOrderSwap64LittleToHost(nlIOReadAligned64(p));
}

/**
 * Perform an aligned memory write of the target system
 * byte ordered 8-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[inout]  p   A pointer to an aligned pointer address, 
 *                    to write the target system byte
 *                    ordered 8-bit value to in little endian byte
 *                    ordering and to then increment by 8-bits (1
 *                    byte).
 *
 * @param[in]  v      The 8-bit value to write.
 *
 */
static inline void     nlIOLittleEndianWriteAligned8(void **p, uint8_t v)
{
    nlIOWriteAligned8(p, v);
}

/**
 * Perform an aligned memory write of the target system
 * byte ordered 16-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[inout]  p   A pointer to an aligned pointer address, 
 *                    to write the target system byte
 *                    ordered 16-bit value to in little endian byte
 *                    ordering and to then increment by 16-bits (2
 *                    bytes).
 *
 * @param[in]  v      The 16-bit value to write.
 *
 */
static inline void     nlIOLittleEndianWriteAligned16(void **p, uint16_t v)
{
    nlIOWriteAligned16(p, nlByteOrderSwap16HostToLittle(v));
}

/**
 * Perform an aligned memory write of the target system
 * byte ordered 32-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[inout]  p   A pointer to an aligned pointer address, 
 *                    to write the target system byte
 *                    ordered 32-bit value to in little endian byte
 *                    ordering and to then increment by 16-bits (4
 *                    bytes).
 *
 * @param[in]  v      The 32-bit value to write.
 *
 */
static inline void     nlIOLittleEndianWriteAligned32(void **p, uint32_t v)
{
    nlIOWriteAligned32(p, nlByteOrderSwap32HostToLittle(v));
}

/**
 * Perform an aligned memory write of the target system
 * byte ordered 64-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[inout]  p   A pointer to an aligned pointer address, 
 *                    to write the target system byte
 *                    ordered 64-bit value to in little endian byte
 *                    ordering and to then increment by 64-bits (8
 *                    bytes).
 *
 * @param[in]  v      The 64-bit value to write.
 *
 */
static inline void     nlIOLittleEndianWriteAligned64(void **p, uint64_t v)
{
    nlIOWriteAligned64(p, nlByteOrderSwap64HostToLittle(v));
}

/**
 * Perform an unaligned memory read of the little endian
 * byte ordered 8-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      An unaligned pointer address to read
 *                    the 8-bit little endian byte ordered value from.
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  nlIOLittleEndianGetUnaligned8(const void *p)
{
    return nlIOGetUnaligned8(p);
}

/**
 * Perform an unaligned memory read of the little endian
 * byte ordered 16-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      An unaligned pointer address to read
 *                    the 16-bit little endian byte ordered value from.
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t nlIOLittleEndianGetUnaligned16(const void *p)
{
    return nlByteOrderSwap16LittleToHost(nlIOGetUnaligned16(p));
}

/**
 * Perform an unaligned memory read of the little endian
 * byte ordered 32-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      An unaligned pointer address to read
 *                    the 32-bit little endian byte ordered value from.
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t nlIOLittleEndianGetUnaligned32(const void *p)
{
    return nlByteOrderSwap32LittleToHost(nlIOGetUnaligned32(p));
}

/**
 * Perform an unaligned memory read of the little endian
 * byte ordered 64-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in target system byte ordering.
 *
 * @param[in]  p      An unaligned pointer address to read
 *                    the 64-bit little endian byte ordered value from.
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t nlIOLittleEndianGetUnaligned64(const void *p)
{
    return nlByteOrderSwap64LittleToHost(nlIOGetUnaligned64(p));
}

/**
 * Perform an unaligned memory write of the target system
 * byte ordered 8-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[in]  p      An unaligned pointer address to write
 *                    the target system byte ordered 8-bit value to in little
 *                    endian byte ordering.
 *
 * @param[in]  v      The 8-bit value to write.
 *
 */
static inline void     nlIOLittleEndianPutUnaligned8(void *p, uint8_t v)
{
    nlIOPutUnaligned8(p, v);
}

/**
 * Perform an unaligned memory write of the target system
 * byte ordered 16-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[in]  p      An unaligned pointer address to write
 *                    the target system byte ordered 16-bit value to in little
 *                    endian byte ordering.
 *
 * @param[in]  v      The 16-bit value to write.
 *
 */
static inline void     nlIOLittleEndianPutUnaligned16(void *p, uint16_t v)
{
    nlIOPutUnaligned16(p, nlByteOrderSwap16HostToLittle(v));
}

/**
 * Perform an unaligned memory write of the target system
 * byte ordered 32-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[in]  p      An unaligned pointer address to write
 *                    the target system byte ordered 32-bit value to in little
 *                    endian byte ordering.
 *
 * @param[in]  v      The 32-bit value to write.
 *
 */
static inline void     nlIOLittleEndianPutUnaligned32(void *p, uint32_t v)
{
    nlIOPutUnaligned32(p, nlByteOrderSwap32HostToLittle(v));
}

/**
 * Perform an unaligned memory write of the target system
 * byte ordered 64-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[in]  p      An unaligned pointer address to write
 *                    the target system byte ordered 64-bit value to in little
 *                    endian byte ordering.
 *
 * @param[in]  v      The 64-bit value to write.
 *
 */
static inline void     nlIOLittleEndianPutUnaligned64(void *p, uint64_t v)
{
    nlIOPutUnaligned64(p, nlByteOrderSwap64HostToLittle(v));
}

/**
 * Perform an unaligned memory read of the little endian
 * byte ordered 8-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 8-bits (1 byte).
 *
 * @param[inout]  p   A pointer to an unaligined pointer address, 
 *                    to read the 8-bit little endian byte
 *                    ordered value from and to then increment by 8-
 *                    bits (1 byte).
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  nlIOLittleEndianReadUnaligned8(const void **p)
{
    return nlIOReadUnaligned8(p);
}

/**
 * Perform an unaligned memory read of the little endian
 * byte ordered 16-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 16-bits (2 bytes).
 *
 * @param[inout]  p   A pointer to an unaligined pointer address, 
 *                    to read the 16-bit little endian byte
 *                    ordered value from and to then increment by 16-
 *                    bits (2 bytes).
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t nlIOLittleEndianReadUnaligned16(const void **p)
{
    return nlByteOrderSwap16LittleToHost(nlIOReadUnaligned16(p));
}

/**
 * Perform an unaligned memory read of the little endian
 * byte ordered 32-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 32-bits (4 bytes).
 *
 * @param[inout]  p   A pointer to an unaligined pointer address, 
 *                    to read the 32-bit little endian byte
 *                    ordered value from and to then increment by 32-
 *                    bits (4 bytes).
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t nlIOLittleEndianReadUnaligned32(const void **p)
{
    return nlByteOrderSwap32LittleToHost(nlIOReadUnaligned32(p));
}

/**
 * Perform an unaligned memory read of the little endian
 * byte ordered 64-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 64-bits (8 bytes).
 *
 * @param[inout]  p   A pointer to an unaligined pointer address, 
 *                    to read the 64-bit little endian byte
 *                    ordered value from and to then increment by 64-
 *                    bits (8 bytes).
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t nlIOLittleEndianReadUnaligned64(const void **p)
{
    return nlByteOrderSwap64LittleToHost(nlIOReadUnaligned64(p));
}

/**
 * Perform an unaligned memory write of the target system
 * byte ordered 8-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[inout]  p   A pointer to an unaligined pointer address, 
 *                    to write the target system byte
 *                    ordered 8-bit value to in little endian byte
 *                    ordering and to then increment by 8-bits (1
 *                    byte).
 *
 * @param[in]  v      The 8-bit value to write.
 *
 */
static inline void     nlIOLittleEndianWriteUnaligned8(void **p, uint8_t v)
{
    nlIOWriteUnaligned8(p, v);
}

/**
 * Perform an unaligned memory write of the target system
 * byte ordered 16-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[inout]  p   A pointer to an unaligined pointer address, 
 *                    to write the target system byte
 *                    ordered 16-bit value to in little endian byte
 *                    ordering and to then increment by 16-bits (2
 *                    bytes).
 *
 * @param[in]  v      The 16-bit value to write.
 *
 */
static inline void     nlIOLittleEndianWriteUnaligned16(void **p, uint16_t v)
{
    nlIOWriteUnaligned16(p, nlByteOrderSwap16HostToLittle(v));
}

/**
 * Perform an unaligned memory write of the target system
 * byte ordered 32-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[inout]  p   A pointer to an unaligined pointer address, 
 *                    to write the target system byte
 *                    ordered 32-bit value to in little endian byte
 *                    ordering and to then increment by 32-bits (4
 *                    bytes).
 *
 * @param[in]  v      The 32-bit value to write.
 *
 */
static inline void     nlIOLittleEndianWriteUnaligned32(void **p, uint32_t v)
{
    nlIOWriteUnaligned32(p, nlByteOrderSwap32HostToLittle(v));
}

/**
 * Perform an unaligned memory write of the target system
 * byte ordered 64-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[inout]  p   A pointer to an unaligined pointer address, 
 *                    to write the target system byte
 *                    ordered 64-bit value to in little endian byte
 *                    ordering and to then increment by 64-bits (8
 *                    bytes).
 *
 * @param[in]  v      The 64-bit value to write.
 *
 */
static inline void     nlIOLittleEndianWriteUnaligned64(void **p, uint64_t v)
{
    nlIOWriteUnaligned64(p, nlByteOrderSwap64HostToLittle(v));
}

#ifdef __cplusplus
}
#endif

#endif /* NLIO_BYTEORDER_LITTLE_H */
