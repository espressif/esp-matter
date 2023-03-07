/**
 *    Copyright 2013-2017 Nest Labs Inc. All Rights Reserved.
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
 *      memory-mapped accesses, potentially to unaligned memory
 *      locations.
 */

#ifndef NLIO_BASE_H
#define NLIO_BASE_H

#include <nlio-private.h>

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/*
 * If we are compiling under clang, GCC, or any such compatible
 * compiler, in which -fno-builtins or -ffreestanding might be
 * asserted, thereby eliminating built-in function optimization, we
 * STILL want built-in memcpy. We want this because it allows the
 * compiler to use architecture-specific machine instructions or
 * inline code generation to optimize an otherwise-expensive memcpy
 * for unaligned reads and writes, which is exactly the kind of
 * efficiency that would be expected of nlIO.
 */
#if __nlIOHasBuiltin(__builtin_memcpy)
#define __nlIO_MEMCPY __builtin_memcpy
#else
#define __nlIO_MEMCPY memcpy
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This determines whether the specified pointer is aligned on the
 * indicated size boundary.
 *
 * @note The size should be a power of 2 (e.g. 1, 2, 4, 8, 16, etc.).
 *
 * @param[in]  p     The pointer to check the alignment of.
 * @param[in]  size  The size, in bytes, boundary to check
 *                   the alignment against.
 *
 * @return True if the pointer is aligned to the specified size;
 *         otherwise, false.
 */
static inline bool nlIOIsAligned(const void *p, size_t size)
{
    const uintptr_t value = (uintptr_t)(p);
    const uintptr_t mask  = size - 1;

    return ((value & mask) == 0);
}

/**
 * Perform an aligned memory read of the 8-bit value at the specified
 * pointer address.
 *
 * @param[in]  p      A pointer address to read the 8-bit value from.
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  nlIOGetAligned8(const void *p)
{
    return *(const uint8_t *)(p);
}

/**
 * Perform an aligned memory read of the 16-bit value at the specified
 * pointer address.
 *
 * @param[in]  p      A pointer address to read the 16-bit value from.
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t nlIOGetAligned16(const void *p)
{
    return *(const uint16_t *)(p);
}

/**
 * Perform an aligned memory read of the 32-bit value at the specified
 * pointer address.
 *
 * @param[in]  p      A pointer address to read the 32-bit value from.
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t nlIOGetAligned32(const void *p)
{
    return *(const uint32_t *)(p);
}

/**
 * Perform an aligned memory read of the 64-bit value at the specified
 * pointer address.
 *
 * @param[in]  p      A pointer address to read the 64-bit value from.
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t nlIOGetAligned64(const void *p)
{
    return *(const uint64_t *)(p);
}

/**
 * Perform an unaligned memory read of the 8-bit value at the specified
 * pointer address.
 *
 * @param[in]  p      A pointer address to read the 8-bit value from.
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  nlIOGetUnaligned8(const void *p)
{
    return nlIOGetAligned8(p);
}

/**
 * Perform an unaligned memory read of the 16-bit value at the specified
 * pointer address.
 *
 * @param[in]  p      A pointer address to read the 16-bit value from.
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t nlIOGetUnaligned16(const void *p)
{
    uint16_t temp;

    __nlIO_MEMCPY(&temp, p, sizeof(uint16_t));

    return temp;
}

/**
 * Perform an unaligned memory read of the 32-bit value at the specified
 * pointer address.
 *
 * @param[in]  p      A pointer address to read the 32-bit value from.
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t nlIOGetUnaligned32(const void *p)
{
    uint32_t temp;

    __nlIO_MEMCPY(&temp, p, sizeof(uint32_t));

    return temp;
}

/**
 * Perform an unaligned memory read of the 64-bit value at the specified
 * pointer address.
 *
 * @param[in]  p      A pointer address to read the 64-bit value from.
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t nlIOGetUnaligned64(const void *p)
{
    uint64_t temp;

    __nlIO_MEMCPY(&temp, p, sizeof(uint64_t));

    return temp;
}

/**
 * Perform a, potentially unaligned, memory read of the 8-bit value
 * from the specified pointer address.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to read
 *                    the 8-bit value from.
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  nlIOGetMaybeAligned8(const void *p)
{
    return nlIOGetAligned8(p);
}

/**
 * Perform a, potentially unaligned, memory read of the 16-bit value
 * from the specified pointer address.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to read
 *                    the 16-bit value from.
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t nlIOGetMaybeAligned16(const void *p)
{
    if (nlIOIsAligned(p, sizeof (uint16_t)))
        return nlIOGetAligned16(p);
    else
        return nlIOGetUnaligned16(p);
}

/**
 * Perform a, potentially unaligned, memory read of the 32-bit value
 * from the specified pointer address.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to read
 *                    the 32-bit value from.
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t nlIOGetMaybeAligned32(const void *p)
{
    if (nlIOIsAligned(p, sizeof (uint32_t)))
        return nlIOGetAligned32(p);
    else
        return nlIOGetUnaligned32(p);
}

/**
 * Perform a, potentially unaligned, memory read of the 64-bit value
 * from the specified pointer address.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to read
 *                    the 64-bit value from.
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t nlIOGetMaybeAligned64(const void *p)
{
    if (nlIOIsAligned(p, sizeof (uint64_t)))
        return nlIOGetAligned64(p);
    else
        return nlIOGetUnaligned64(p);
}

/**
 * Perform an aligned memory write of the 8-bit value to the specified
 * pointer address.
 *
 * @param[in]  p      A pointer address to write the 8-bit value to.
 *
 * @param[in]  v      The 8-bit value to write.
 *
 */
static inline void     nlIOPutAligned8(void *p, const uint8_t v)
{
    *(uint8_t *)(p) = v;
}

/**
 * Perform an aligned memory write of the 16-bit value to the specified
 * pointer address.
 *
 * @param[in]  p      A pointer address to write the 16-bit value to.
 *
 * @param[in]  v      The 16-bit value to write.
 *
 */
static inline void     nlIOPutAligned16(void *p, const uint16_t v)
{
    *(uint16_t *)(p) = v;
}

/**
 * Perform an aligned memory write of the 32-bit value to the specified
 * pointer address.
 *
 * @param[in]  p      A pointer address to write the 32-bit value to.
 *
 * @param[in]  v      The 32-bit value to write.
 *
 */
static inline void     nlIOPutAligned32(void *p, const uint32_t v)
{
    *(uint32_t *)(p) = v;
}

/**
 * Perform an aligned memory write of the 64-bit value to the specified
 * pointer address.
 *
 * @param[in]  p      A pointer address to write the 64-bit value to.
 *
 * @param[in]  v      The 64-bit value to write.
 *
 */
static inline void     nlIOPutAligned64(void *p, const uint64_t v)
{
    *(uint64_t *)(p) = v;
}

/**
 * Perform an unaligned memory write of the 8-bit value to the specified
 * pointer address.
 *
 * @param[in]  p      A pointer address to write the 8-bit value to.
 *
 * @param[in]  v      The 8-bit value to write.
 *
 */
static inline void     nlIOPutUnaligned8(void *p, const uint8_t v)
{
    nlIOPutAligned8(p, v);
}

/**
 * Perform an unaligned memory write of the 16-bit value to the specified
 * pointer address.
 *
 * @param[in]  p      A pointer address to write the 16-bit value to.
 *
 * @param[in]  v      The 16-bit value to write.
 *
 */
static inline void     nlIOPutUnaligned16(void *p, const uint16_t v)
{
    __nlIO_MEMCPY(p, &v, sizeof(uint16_t));
}

/**
 * Perform an unaligned memory write of the 32-bit value to the specified
 * pointer address.
 *
 * @param[in]  p      A pointer address to write the 32-bit value to.
 *
 * @param[in]  v      The 32-bit value to write.
 *
 */
static inline void     nlIOPutUnaligned32(void *p, const uint32_t v)
{
    __nlIO_MEMCPY(p, &v, sizeof(uint32_t));
}

/**
 * Perform an unaligned memory write of the 64-bit value to the specified
 * pointer address.
 *
 * @param[in]  p      A pointer address to write the 64-bit value to.
 *
 * @param[in]  v      The 64-bit value to write.
 *
 */
static inline void     nlIOPutUnaligned64(void *p, const uint64_t v)
{
    __nlIO_MEMCPY(p, &v, sizeof(uint64_t));
}

/**
 * Perform a, potentially unaligned, memory write of the 8-bit value
 * to the specified pointer address.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to write
 *                    the 8-bit value to.
 *
 * @param[in]  v      The 8-bit value to write.
 *
 */
static inline void     nlIOPutMaybeAligned8(void *p, const uint8_t v)
{
    nlIOPutAligned8(p, v);
}

/**
 * Perform a, potentially unaligned, memory write of the 16-bit value
 * to the specified pointer address.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to write
 *                    the 16-bit value to.
 *
 * @param[in]  v      The 16-bit value to write.
 *
 */
static inline void     nlIOPutMaybeAligned16(void *p, const uint16_t v)
{
    if (nlIOIsAligned(p, sizeof (uint16_t)))
        nlIOPutAligned16(p, v);
    else
        nlIOPutUnaligned16(p, v);
}

/**
 * Perform a, potentially unaligned, memory write of the 32-bit value
 * to the specified pointer address.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to write
 *                    the 32-bit value to.
 *
 * @param[in]  v      The 32-bit value to write.
 *
 */
static inline void     nlIOPutMaybeAligned32(void *p, const uint32_t v)
{
    if (nlIOIsAligned(p, sizeof (uint32_t)))
        nlIOPutAligned32(p, v);
    else
        nlIOPutUnaligned32(p, v);
}

/**
 * Perform a, potentially unaligned, memory write of the 64-bit value
 * to the specified pointer address.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to write
 *                    the 64-bit value to.
 *
 * @param[in]  v      The 64-bit value to write.
 *
 */
static inline void     nlIOPutMaybeAligned64(void *p, const uint64_t v)
{
    if (nlIOIsAligned(p, sizeof (uint64_t)))
        nlIOPutAligned64(p, v);
    else
        nlIOPutUnaligned64(p, v);
}

/**
 * Perform an aligned memory read of the 8-bit value at the specified
 * pointer address and increment the pointer by 8-bits (1 byte).
 *
 * @param[inout]  p   A pointer to a pointer address to read the
 *                    8-bit value from and to then increment by
 *                    8-bits (1 byte).
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  nlIOReadAligned8(const void **p)
{
    const uint8_t temp = nlIOGetAligned8(*p);

    *(const uint8_t **)(p) += sizeof (uint8_t);

    return temp;
}

/**
 * Perform an aligned memory read of the 16-bit value at the specified
 * pointer address and increment the pointer by 16-bits (2 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address to read the
 *                    16-bit value from and to then increment by
 *                    16-bits (2 bytes).
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t nlIOReadAligned16(const void **p)
{
    const uint16_t temp = nlIOGetAligned16(*p);

    *(const uint8_t **)(p) += sizeof (uint16_t);

    return temp;
}

/**
 * Perform an aligned memory read of the 32-bit value at the specified
 * pointer address and increment the pointer by 32-bits (4 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address to read the
 *                    32-bit value from and to then increment by
 *                    32-bits (4 bytes).
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t nlIOReadAligned32(const void **p)
{
    const uint32_t temp = nlIOGetAligned32(*p);

    *(const uint8_t **)(p) += sizeof (uint32_t);

    return temp;
}

/**
 * Perform an aligned memory read of the 64-bit value at the specified
 * pointer address and increment the pointer by 64-bits (8 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address to read the
 *                    64-bit value from and to then increment by
 *                    64-bits (8 bytes).
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t nlIOReadAligned64(const void **p)
{
    const uint64_t temp = nlIOGetAligned64(*p);

    *(const uint8_t **)(p) += sizeof (uint64_t);

    return temp;
}

/**
 * Perform an unaligned memory read of the 8-bit value at the specified
 * pointer address and increment the pointer by 8-bits (1 byte).
 *
 * @param[inout]  p   A pointer to a pointer address to read the
 *                    8-bit value from and to then increment by
 *                    8-bits (1 byte).
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t nlIOReadUnaligned8(const void **p)
{
    const uint8_t temp = nlIOGetUnaligned8(*p);

    *(const uint8_t **)(p) += sizeof (uint8_t);

    return temp;
}

/**
 * Perform an unaligned memory read of the 16-bit value at the specified
 * pointer address and increment the pointer by 16-bits (2 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address to read the
 *                    16-bit value from and to then increment by
 *                    16-bits (2 bytes).
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t nlIOReadUnaligned16(const void **p)
{
    const uint16_t temp = nlIOGetUnaligned16(*p);

    *(const uint8_t **)(p) += sizeof (uint16_t);

    return temp;
}

/**
 * Perform an aligned memory read of the 32-bit value at the specified
 * pointer address and increment the pointer by 32-bits (4 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address to read the
 *                    32-bit value from and to then increment by
 *                    32-bits (4 bytes).
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t nlIOReadUnaligned32(const void **p)
{
    const uint32_t temp = nlIOGetUnaligned32(*p);

    *(const uint8_t **)(p) += sizeof (uint32_t);

    return temp;
}

/**
 * Perform an unaligned memory read of the 64-bit value at the specified
 * pointer address and increment the pointer by 64-bits (8 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address to read the
 *                    64-bit value from and to then increment by
 *                    64-bits (8 bytes).
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t nlIOReadUnaligned64(const void **p)
{
    const uint64_t temp = nlIOGetUnaligned64(*p);

    *(const uint8_t **)(p) += sizeof (uint64_t);

    return temp;
}

/**
 * Perform a, potentially unaligned, memory read of the 8-bit value
 * from the specified pointer address and increment the pointer by
 * 8-bits (1 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 8-bit value from and to
 *                    then increment by 8-bits (1 byte).
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  nlIOReadMaybeAligned8(const void **p)
{
    const uint8_t temp = nlIOGetMaybeAligned8(*p);

    *(const uint8_t **)(p) += sizeof (uint8_t);

    return temp;
}

/**
 * Perform a, potentially unaligned, memory read of the 16-bit value
 * from the specified pointer address and increment the pointer by
 * 16-bits (2 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 16-bit value from and to
 *                    then increment by 16-bits (2 bytes).
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t nlIOReadMaybeAligned16(const void **p)
{
    const uint16_t temp = nlIOGetMaybeAligned16(*p);

    *(const uint8_t **)(p) += sizeof (uint16_t);

    return temp;
}

/**
 * Perform a, potentially unaligned, memory read of the 32-bit value
 * from the specified pointer address and increment the pointer by
 * 32-bits (4 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 32-bit value from and to
 *                    then increment by 32-bits (4 bytes).
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t nlIOReadMaybeAligned32(const void **p)
{
    const uint32_t temp = nlIOGetMaybeAligned32(*p);

    *(const uint8_t **)(p) += sizeof (uint32_t);

    return temp;
}

/**
 * Perform a, potentially unaligned, memory read of the 64-bit value
 * from the specified pointer address and increment the pointer by
 * 64-bits (8 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 64-bit value from and to
 *                    then increment by 64-bits (8 bytes).
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t nlIOReadMaybeAligned64(const void **p)
{
    const uint64_t temp = nlIOGetMaybeAligned64(*p);

    *(const uint8_t **)(p) += sizeof (uint64_t);

    return temp;
}

/**
 * Perform an aligned memory write of the 8-bit value to the specified
 * pointer address and increment the pointer by 8-bits (1 byte).
 *
 * @param[inout]  p   A pointer to a pointer address to read the 8-bit
 *                    value from and to then increment by 8-bits (1 byte).
 *
 * @param[in]     v   The 8-bit value to write.
 *
 */
static inline void     nlIOWriteAligned8(void **p, const uint8_t v)
{
    nlIOPutAligned8(*p, v);       *(const uint8_t **)(p) += sizeof (uint8_t);
}

/**
 * Perform an aligned memory write of the 16-bit value to the specified
 * pointer address and increment the pointer by 16-bits (2 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address to read the 16-bit
 *                    value from and to then increment by 16-bits (2 bytes).
 *
 * @param[in]     v   The 16-bit value to write.
 *
 */
static inline void     nlIOWriteAligned16(void **p, const uint16_t v)
{
    nlIOPutAligned16(*p, v);      *(const uint8_t **)(p) += sizeof (uint16_t);
}

/**
 * Perform an aligned memory write of the 32-bit value to the specified
 * pointer address and increment the pointer by 32-bits (4 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address to read the 32-bit
 *                    value from and to then increment by 32-bits (4 bytes).
 *
 * @param[in]     v   The 32-bit value to write.
 *
 */
static inline void     nlIOWriteAligned32(void **p, const uint32_t v)
{
    nlIOPutAligned32(*p, v);      *(const uint8_t **)(p) += sizeof (uint32_t);
}

/**
 * Perform an aligned memory write of the 64-bit value to the specified
 * pointer address and increment the pointer by 64-bits (8 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address to read the 64-bit
 *                    value from and to then increment by 64-bits (8 bytes).
 *
 * @param[in]     v   The 64-bit value to write.
 *
 */
static inline void     nlIOWriteAligned64(void **p, const uint64_t v)
{
    nlIOPutAligned64(*p, v);      *(const uint8_t **)(p) += sizeof (uint64_t);
}

/**
 * Perform an unaligned memory write of the 8-bit value to the specified
 * pointer address and increment the pointer by 8-bits (1 byte).
 *
 * @param[inout]  p   A pointer to a pointer address to read the 8-bit
 *                    value from and to then increment by 8-bits (1 byte).
 *
 * @param[in]     v   The 8-bit value to write.
 *
 */
static inline void     nlIOWriteUnaligned8(void **p, const uint8_t v)
{
    nlIOPutUnaligned8(*p, v);     *(const uint8_t **)(p) += sizeof (uint8_t);
}

/**
 * Perform an unaligned memory write of the 16-bit value to the specified
 * pointer address and increment the pointer by 16-bits (2 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address to read the 16-bit
 *                    value from and to then increment by 16-bits (2 bytes).
 *
 * @param[in]     v   The 16-bit value to write.
 *
 */
static inline void     nlIOWriteUnaligned16(void **p, const uint16_t v)
{
    nlIOPutUnaligned16(*p, v);    *(const uint8_t **)(p) += sizeof (uint16_t);
}

/**
 * Perform an unaligned memory write of the 32-bit value to the specified
 * pointer address and increment the pointer by 32-bits (4 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address to read the 32-bit
 *                    value from and to then increment by 32-bits (4 bytes).
 *
 * @param[in]     v   The 32-bit value to write.
 *
 */
static inline void     nlIOWriteUnaligned32(void **p, const uint32_t v)
{
    nlIOPutUnaligned32(*p, v);    *(const uint8_t **)(p) += sizeof (uint32_t);
}

/**
 * Perform an unaligned memory write of the 64-bit value to the specified
 * pointer address and increment the pointer by 64-bits (8 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address to read the 64-bit
 *                    value from and to then increment by 64-bits (8 bytes).
 *
 * @param[in]     v   The 64-bit value to write.
 *
 */
static inline void     nlIOWriteUnaligned64(void **p, const uint64_t v)
{
    nlIOPutUnaligned64(*p, v);    *(const uint8_t **)(p) += sizeof (uint64_t);
}

/**
 * Perform a, potentially unaligned, memory write of the 8-bit value
 * to the specified pointer address and increment the pointer by
 * 8-bits (2 byte).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 8-bit value from and to
 *                    then increment by 8-bits (1 byte).
 *
 * @param[in]     v   The 8-bit value to write.
 *
 */
static inline void     nlIOWriteMaybeAligned8(void **p, const uint8_t v)
{
    nlIOPutMaybeAligned8(*p, v);  *(const uint8_t **)(p) += sizeof (uint8_t);
}

/**
 * Perform a, potentially unaligned, memory write of the 16-bit value
 * to the specified pointer address and increment the pointer by
 * 16-bits (2 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 16-bit value from and to
 *                    then increment by 16-bits (2 bytes).
 *
 * @param[in]     v   The 16-bit value to write.
 *
 */
static inline void     nlIOWriteMaybeAligned16(void **p, const uint16_t v)
{
    nlIOPutMaybeAligned16(*p, v); *(const uint8_t **)(p) += sizeof (uint16_t);
}

/**
 * Perform a, potentially unaligned, memory write of the 32-bit value
 * to the specified pointer address and increment the pointer by
 * 32-bits (4 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 32-bit value from and to
 *                    then increment by 32-bits (4 bytes).
 *
 * @param[in]     v   The 32-bit value to write.
 *
 */
static inline void     nlIOWriteMaybeAligned32(void **p, const uint32_t v)
{
    nlIOPutMaybeAligned32(*p, v); *(const uint8_t **)(p) += sizeof (uint32_t);
}

/**
 * Perform a, potentially unaligned, memory write of the 64-bit value
 * to the specified pointer address and increment the pointer by
 * 64-bits (8 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 64-bit value from and to
 *                    then increment by 64-bits (8 bytes).
 *
 * @param[in]     v   The 64-bit value to write.
 *
 */
static inline void     nlIOWriteMaybeAligned64(void **p, const uint64_t v)
{
    nlIOPutMaybeAligned64(*p, v); *(const uint8_t **)(p) += sizeof (uint64_t);
}

/**
 * Perform a memory read of the 8-bit value at the specified pointer
 * address.
 *
 * @param[in]  p      A pointer address to read the 8-bit value from.
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t nlIOGet8(const void *p)
{
    return nlIOGetAligned8(p);
}

/**
 * Perform a, potentially unaligned, memory read of the 16-bit value
 * from the specified pointer address.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to read
 *                    the 16-bit value from.
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t nlIOGet16(const void *p)
{
    return nlIOGetMaybeAligned16(p);
}

/**
 * Perform a, potentially unaligned, memory read of the 32-bit value
 * from the specified pointer address.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to read
 *                    the 32-bit value from.
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t nlIOGet32(const void *p)
{
    return nlIOGetMaybeAligned32(p);
}

/**
 * Perform a, potentially unaligned, memory read of the 64-bit value
 * from the specified pointer address.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to read
 *                    the 64-bit value from.
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t nlIOGet64(const void *p)
{
    return nlIOGetMaybeAligned64(p);
}

/**
 * Perform a memory write of the 8-bit value to the specified pointer
 * address.
 *
 * @param[in]  p      A pointer address to write the 8-bit value to.
 *
 * @param[in]  v      The 8-bit value to write.
 *
 */
static inline void nlIOPut8(void *p, uint8_t v)
{
    nlIOPutAligned8(p, v);
}

/**
 * Perform a, potentially unaligned, memory write of the 16-bit value
 * to the specified pointer address.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to write
 *                    the 16-bit value to.
 *
 * @param[in]  v      The 16-bit value to write.
 *
 */
static inline void nlIOPut16(void *p, uint16_t v)
{
    nlIOPutMaybeAligned16(p, v);
}

/**
 * Perform a, potentially unaligned, memory write of the 32-bit value
 * to the specified pointer address.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to write
 *                    the 32-bit value to.
 *
 * @param[in]  v      The 32-bit value to write.
 *
 */
static inline void nlIOPut32(void *p, uint32_t v)
{
    nlIOPutMaybeAligned32(p, v);
}

/**
 * Perform a, potentially unaligned, memory write of the 64-bit value
 * to the specified pointer address.
 *
 * @param[in]  p      A pointer address, potentially unaligned, to write
 *                    the 64-bit value to.
 *
 * @param[in]  v      The 64-bit value to write.
 *
 */
static inline void nlIOPut64(void *p, uint64_t v)
{
    nlIOPutMaybeAligned64(p, v);
}

/**
 * Perform a, potentially unaligned, memory read of the 16-bit value
 * from the specified pointer address and increment the pointer by
 * 8-bits (1 byte).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 8-bit value from and to
 *                    then increment by 8-bits (1 byte).
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  nlIORead8(const void **p)
{
    return nlIOReadAligned8(p);
}

/**
 * Perform a, potentially unaligned, memory read of the 16-bit value
 * from the specified pointer address and increment the pointer by
 * 16-bits (2 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 16-bit value from and to
 *                    then increment by 16-bits (2 bytes).
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t nlIORead16(const void **p)
{
    return nlIOReadMaybeAligned16(p);
}

/**
 * Perform a, potentially unaligned, memory read of the 32-bit value
 * from the specified pointer address and increment the pointer by
 * 32-bits (4 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 32-bit value from and to
 *                    then increment by 32-bits (4 bytes).
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t nlIORead32(const void **p)
{
    return nlIOReadMaybeAligned32(p);
}

/**
 * Perform a, potentially unaligned, memory read of the 64-bit value
 * from the specified pointer address and increment the pointer by
 * 64-bits (8 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 64-bit value from and to
 *                    then increment by 64-bits (8 bytes).
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t nlIORead64(const void **p)
{
    return nlIOReadMaybeAligned64(p);
}

/**
 * Perform a, potentially unaligned, memory write of the 8-bit value
 * to the specified pointer address and increment the pointer by
 * 8-bits (1 byte).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 8-bit value from and to
 *                    then increment by 8-bits (1 byte).
 *
 * @param[in]     v   The 8-bit value to write.
 *
 */
static inline void nlIOWrite8(void **p, uint8_t v)
{
    nlIOWriteAligned8(p, v);
}

/**
 * Perform a, potentially unaligned, memory write of the 16-bit value
 * to the specified pointer address and increment the pointer by
 * 16-bits (2 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 16-bit value from and to
 *                    then increment by 16-bits (2 bytes).
 *
 * @param[in]     v   The 16-bit value to write.
 *
 */
static inline void nlIOWrite16(void **p, uint16_t v)
{
    nlIOWriteMaybeAligned16(p, v);
}

/**
 * Perform a, potentially unaligned, memory write of the 32-bit value
 * to the specified pointer address and increment the pointer by
 * 32-bits (4 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 32-bit value from and to
 *                    then increment by 32-bits (4 bytes).
 *
 * @param[in]     v   The 32-bit value to write.
 *
 */
static inline void nlIOWrite32(void **p, uint32_t v)
{
    nlIOWriteMaybeAligned32(p, v);
}

/**
 * Perform a, potentially unaligned, memory write of the 64-bit value
 * to the specified pointer address and increment the pointer by
 * 64-bits (8 bytes).
 *
 * @param[inout]  p   A pointer to a pointer address, potentially
 *                    unaligned, to read the 64-bit value from and to
 *                    then increment by 64-bits (8 bytes).
 *
 * @param[in]     v   The 64-bit value to write.
 *
 */
static inline void nlIOWrite64(void **p, uint64_t v)
{
    nlIOWriteMaybeAligned64(p, v);
}

#ifdef __cplusplus
}
#endif

#undef __nlIO_MEMCPY

#endif /* NLIO_BASE_H */
