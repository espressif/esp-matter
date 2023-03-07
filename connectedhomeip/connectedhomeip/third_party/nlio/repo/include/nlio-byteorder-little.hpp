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
 *      This file defines C++ functions for safely performing simple,
 *      memory-mapped accesses, potentially to unaligned, aligned, and
 *      unaligned memory locations with byte reordering, specifically
 *      for little endian target systems.
 */

#ifndef NLIO_BYTEORDER_LITTLE_HPP
#define NLIO_BYTEORDER_LITTLE_HPP

#include <nlio-base.hpp>
#include <nlbyteorder.hpp>

namespace nl
{

namespace IO
{

namespace LittleEndian
{

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
static inline uint8_t  Get8(const void *p)
{
    return IO::Get8(p);
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
static inline uint16_t Get16(const void *p)
{
    return ByteOrder::Swap16LittleToHost(IO::Get16(p));
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
static inline uint32_t Get32(const void *p)
{
    return ByteOrder::Swap32LittleToHost(IO::Get32(p));
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
static inline uint64_t Get64(const void *p)
{
    return ByteOrder::Swap64LittleToHost(IO::Get64(p));
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
static inline void     Put8(void *p, uint8_t v)
{
    IO::Put8(p, v);
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
static inline void     Put16(void *p, uint16_t v)
{
    IO::Put16(p, ByteOrder::Swap16HostToLittle(v));
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
static inline void     Put32(void *p, uint32_t v)
{
    IO::Put32(p, ByteOrder::Swap32HostToLittle(v));
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
static inline void     Put64(void *p, uint64_t v)
{
    IO::Put64(p, ByteOrder::Swap64HostToLittle(v));
}

/**
 * Perform a, potentially unaligned, memory read of the little endian
 * byte ordered 8-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 8-bits (1 byte).
 *
 * @param[inout]  p   A reference to a pointer address, potentially
 *                    unaligned, to read the 8-bit little endian byte
 *                    ordered value from and to then increment by 8-
 *                    bits (1 byte).
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  Read8(const void *&p)
{
    return IO::Read8(p);
}

/**
 * Perform a, potentially unaligned, memory read of the little endian
 * byte ordered 16-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 16-bits (2 bytes).
 *
 * @param[inout]  p   A reference to a pointer address, potentially
 *                    unaligned, to read the 16-bit little endian byte
 *                    ordered value from and to then increment by 16-
 *                    bits (2 bytes).
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t Read16(const void *&p)
{
    return ByteOrder::Swap16LittleToHost(IO::Read16(p));
}

/**
 * Perform a, potentially unaligned, memory read of the little endian
 * byte ordered 32-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 32-bits (4 bytes).
 *
 * @param[inout]  p   A reference to a pointer address, potentially
 *                    unaligned, to read the 32-bit little endian byte
 *                    ordered value from and to then increment by 32-
 *                    bits (4 bytes).
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t Read32(const void *&p)
{
    return ByteOrder::Swap32LittleToHost(IO::Read32(p));
}

/**
 * Perform a, potentially unaligned, memory read of the little endian
 * byte ordered 64-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 64-bits (8 bytes).
 *
 * @param[inout]  p   A reference to a pointer address, potentially
 *                    unaligned, to read the 64-bit little endian byte
 *                    ordered value from and to then increment by 64-
 *                    bits (8 bytes).
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t Read64(const void *&p)
{
    return ByteOrder::Swap64LittleToHost(IO::Read64(p));
}

/**
 * Perform a, potentially unaligned, memory write of the target system
 * byte ordered 8-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[in]  p      A reference to a pointer address, potentially
 *                    unaligned, to write the target system byte
 *                    ordered 8-bit value to in little endian byte
 *                    ordering and to then increment by 8-bits (1
 *                    byte).
 *
 * @param[in]  v      The 8-bit value to write.
 *
 */
static inline void     Write8(void *&p, uint8_t v)
{
    IO::Write8(p, v);
}

/**
 * Perform a, potentially unaligned, memory write of the target system
 * byte ordered 16-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[in]  p      A reference to a pointer address, potentially
 *                    unaligned, to write the target system byte
 *                    ordered 16-bit value to in little endian byte
 *                    ordering and to then increment by 16-bits (2
 *                    bytes).
 *
 * @param[in]  v      The 16-bit value to write.
 *
 */
static inline void     Write16(void *&p, uint16_t v)
{
    IO::Write16(p, ByteOrder::Swap16HostToLittle(v));
}

/**
 * Perform a, potentially unaligned, memory write of the target system
 * byte ordered 32-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[in]  p      A reference to a pointer address, potentially
 *                    unaligned, to write the target system byte
 *                    ordered 32-bit value to in little endian byte
 *                    ordering and to then increment by 16-bits (4
 *                    bytes).
 *
 * @param[in]  v      The 32-bit value to write.
 *
 */
static inline void     Write32(void *&p, uint32_t v)
{
    IO::Write32(p, ByteOrder::Swap32HostToLittle(v));
}

/**
 * Perform a, potentially unaligned, memory write of the target system
 * byte ordered 64-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[in]  p      A reference to a pointer address, potentially
 *                    unaligned, to write the target system byte
 *                    ordered 64-bit value to in little endian byte
 *                    ordering and to then increment by 64-bits (8
 *                    bytes).
 *
 * @param[in]  v      The 64-bit value to write.
 *
 */
static inline void     Write64(void *&p, uint64_t v)
{
    IO::Write64(p, ByteOrder::Swap64HostToLittle(v));
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
static inline uint8_t  GetAligned8(const void *p)
{
    return IO::GetAligned8(p);
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
static inline uint16_t GetAligned16(const void *p)
{
    return ByteOrder::Swap16LittleToHost(IO::GetAligned16(p));
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
static inline uint32_t GetAligned32(const void *p)
{
    return ByteOrder::Swap32LittleToHost(IO::GetAligned32(p));
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
static inline uint64_t GetAligned64(const void *p)
{
    return ByteOrder::Swap64LittleToHost(IO::GetAligned64(p));
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
static inline void     PutAligned8(void *p, uint8_t v)
{
    IO::PutAligned8(p, v);
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
static inline void     PutAligned16(void *p, uint16_t v)
{
    IO::PutAligned16(p, ByteOrder::Swap16HostToLittle(v));
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
static inline void     PutAligned32(void *p, uint32_t v)
{
    IO::PutAligned32(p, ByteOrder::Swap32HostToLittle(v));
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
static inline void     PutAligned64(void *p, uint64_t v)
{
    IO::PutAligned64(p, ByteOrder::Swap64HostToLittle(v));
}

/**
 * Perform an aligned memory read of the little endian
 * byte ordered 8-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 8-bits (1 byte).
 *
 * @param[inout]  p   A reference to an aligned pointer address
 *                    to read the 8-bit little endian byte
 *                    ordered value from and to then increment by 8-
 *                    bits (1 byte).
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  ReadAligned8(const void *&p)
{
    return IO::ReadAligned8(p);
}

/**
 * Perform an aligned memory read of the little endian
 * byte ordered 16-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 16-bits (2 bytes).
 *
 * @param[inout]  p   A reference to an aligned pointer address
 *                    to read the 16-bit little endian byte
 *                    ordered value from and to then increment by 16-
 *                    bits (2 bytes).
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t ReadAligned16(const void *&p)
{
    return ByteOrder::Swap16LittleToHost(IO::ReadAligned16(p));
}

/**
 * Perform an aligned memory read of the little endian
 * byte ordered 32-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 32-bits (4 bytes).
 *
 * @param[inout]  p   A reference to an aligned pointer address
 *                    to read the 32-bit little endian byte
 *                    ordered value from and to then increment by 32-
 *                    bits (4 bytes).
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t ReadAligned32(const void *&p)
{
    return ByteOrder::Swap32LittleToHost(IO::ReadAligned32(p));
}

/**
 * Perform an aligned memory read of the little endian
 * byte ordered 64-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 64-bits (8 bytes).
 *
 * @param[inout]  p   A reference to an aligned pointer address
 *                    to read the 64-bit little endian byte
 *                    ordered value from and to then increment by 64-
 *                    bits (8 bytes).
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t ReadAligned64(const void *&p)
{
    return ByteOrder::Swap64LittleToHost(IO::ReadAligned64(p));
}

/**
 * Perform an aligned memory write of the target system
 * byte ordered 8-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[inout]  p   A reference to an aligned pointer address
 *                    to write the target system byte
 *                    ordered 8-bit value to in little endian byte
 *                    ordering and to then increment by 8-bits (1
 *                    byte).
 *
 * @param[in]  v      The 8-bit value to write.
 *
 */
static inline void     WriteAligned8(void *&p, uint8_t v)
{
    IO::WriteAligned8(p, v);
}

/**
 * Perform an aligned memory write of the target system
 * byte ordered 16-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[inout]  p   A reference to an aligned pointer address
 *                    to write the target system byte
 *                    ordered 16-bit value to in little endian byte
 *                    ordering and to then increment by 16-bits (2
 *                    bytes).
 *
 * @param[in]  v      The 16-bit value to write.
 *
 */
static inline void     WriteAligned16(void *&p, uint16_t v)
{
    IO::WriteAligned16(p, ByteOrder::Swap16HostToLittle(v));
}

/**
 * Perform an aligned memory write of the target system
 * byte ordered 32-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[inout]  p   A reference to an aligned pointer address
 *                    to write the target system byte
 *                    ordered 32-bit value to in little endian byte
 *                    ordering and to then increment by 16-bits (4
 *                    bytes).
 *
 * @param[in]  v      The 32-bit value to write.
 *
 */
static inline void     WriteAligned32(void *&p, uint32_t v)
{
    IO::WriteAligned32(p, ByteOrder::Swap32HostToLittle(v));
}

/**
 * Perform an aligned memory write of the target system
 * byte ordered 64-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[inout]  p   A reference to an aligned pointer address
 *                    to write the target system byte
 *                    ordered 64-bit value to in little endian byte
 *                    ordering and to then increment by 64-bits (8
 *                    bytes).
 *
 * @param[in]  v      The 64-bit value to write.
 *
 */
static inline void     WriteAligned64(void *&p, uint64_t v)
{
    IO::WriteAligned64(p, ByteOrder::Swap64HostToLittle(v));
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
static inline uint8_t  GetUnaligned8(const void *p)
{
    return IO::GetUnaligned8(p);
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
static inline uint16_t GetUnaligned16(const void *p)
{
    return ByteOrder::Swap16LittleToHost(IO::GetUnaligned16(p));
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
static inline uint32_t GetUnaligned32(const void *p)
{
    return ByteOrder::Swap32LittleToHost(IO::GetUnaligned32(p));
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
static inline uint64_t GetUnaligned64(const void *p)
{
    return ByteOrder::Swap64LittleToHost(IO::GetUnaligned64(p));
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
static inline void     PutUnaligned8(void *p, uint8_t v)
{
    IO::PutUnaligned8(p, v);
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
static inline void     PutUnaligned16(void *p, uint16_t v)
{
    IO::PutUnaligned16(p, ByteOrder::Swap16HostToLittle(v));
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
static inline void     PutUnaligned32(void *p, uint32_t v)
{
    IO::PutUnaligned32(p, ByteOrder::Swap32HostToLittle(v));
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
static inline void     PutUnaligned64(void *p, uint64_t v)
{
    IO::PutUnaligned64(p, ByteOrder::Swap64HostToLittle(v));
}

/**
 * Perform an unaligned memory read of the little endian
 * byte ordered 8-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 8-bits (1 byte).
 *
 * @param[inout]  p   A reference to an unaligned pointer address
 *                    to read the 8-bit little endian byte
 *                    ordered value from and to then increment by 8-
 *                    bits (1 byte).
 *
 * @return The 8-bit value at the specified pointer address.
 */
static inline uint8_t  ReadUnaligned8(const void *&p)
{
    return IO::ReadUnaligned8(p);
}

/**
 * Perform an unaligned memory read of the little endian
 * byte ordered 16-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 16-bits (2 bytes).
 *
 * @param[inout]  p   A reference to an unaligned pointer address
 *                    to read the 16-bit little endian byte
 *                    ordered value from and to then increment by 16-
 *                    bits (2 bytes).
 *
 * @return The 16-bit value at the specified pointer address.
 */
static inline uint16_t ReadUnaligned16(const void *&p)
{
    return ByteOrder::Swap16LittleToHost(IO::ReadUnaligned16(p));
}

/**
 * Perform an unaligned memory read of the little endian
 * byte ordered 32-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 32-bits (4 bytes).
 *
 * @param[inout]  p   A reference to an unaligned pointer address
 *                    to read the 32-bit little endian byte
 *                    ordered value from and to then increment by 32-
 *                    bits (4 bytes).
 *
 * @return The 32-bit value at the specified pointer address.
 */
static inline uint32_t ReadUnaligned32(const void *&p)
{
    return ByteOrder::Swap32LittleToHost(IO::ReadUnaligned32(p));
}

/**
 * Perform an unaligned memory read of the little endian
 * byte ordered 64-bit value from the specified pointer address,
 * perform byte reordering, as necessary, for the target system to put
 * the value in target system byte ordering, and increment the pointer
 * by 64-bits (8 bytes).
 *
 * @param[inout]  p   A reference to an unaligned pointer address
 *                    to read the 64-bit little endian byte
 *                    ordered value from and to then increment by 64-
 *                    bits (8 bytes).
 *
 * @return The 64-bit value at the specified pointer address.
 */
static inline uint64_t ReadUnaligned64(const void *&p)
{
    return ByteOrder::Swap64LittleToHost(IO::ReadUnaligned64(p));
}

/**
 * Perform an unaligned memory write of the target system
 * byte ordered 8-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[inout]  p   A reference to an unaligned pointer address
 *                    to write the target system byte
 *                    ordered 8-bit value to in little endian byte
 *                    ordering and to then increment by 8-bits (1
 *                    byte).
 *
 * @param[in]  v      The 8-bit value to write.
 *
 */
static inline void     WriteUnaligned8(void *&p, uint8_t v)
{
    IO::WriteUnaligned8(p, v);
}

/**
 * Perform an unaligned memory write of the target system
 * byte ordered 16-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[inout]  p   A reference to an unaligned pointer address
 *                    to write the target system byte
 *                    ordered 16-bit value to in little endian byte
 *                    ordering and to then increment by 16-bits (2
 *                    bytes).
 *
 * @param[in]  v      The 16-bit value to write.
 *
 */
static inline void     WriteUnaligned16(void *&p, uint16_t v)
{
    IO::WriteUnaligned16(p, ByteOrder::Swap16HostToLittle(v));
}

/**
 * Perform an unaligned memory write of the target system
 * byte ordered 32-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[inout]  p   A reference to an unaligned pointer address
 *                    to write the target system byte
 *                    ordered 32-bit value to in little endian byte
 *                    ordering and to then increment by 16-bits (4
 *                    bytes).
 *
 * @param[in]  v      The 32-bit value to write.
 *
 */
static inline void     WriteUnaligned32(void *&p, uint32_t v)
{
    IO::WriteUnaligned32(p, ByteOrder::Swap32HostToLittle(v));
}

/**
 * Perform an unaligned memory write of the target system
 * byte ordered 64-bit value to the specified pointer address,
 * perform byte reordering, as necessary, for the target system to
 * put the value in little endian byte ordering.
 *
 * @param[inout]  p   A reference to an unaligned pointer address
 *                    to write the target system byte
 *                    ordered 64-bit value to in little endian byte
 *                    ordering and to then increment by 64-bits (8
 *                    bytes).
 *
 * @param[in]  v      The 64-bit value to write.
 *
 */
static inline void     WriteUnaligned64(void *&p, uint64_t v)
{
    IO::WriteUnaligned64(p, ByteOrder::Swap64HostToLittle(v));
}

} // namespace LittleEndian

} // namespace IO

} // namespace nl

#endif // NLIO_BYTEORDER_LITTLE_HPP
