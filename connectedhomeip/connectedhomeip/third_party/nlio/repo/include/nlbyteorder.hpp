/**
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
 *      This file defines C++ functions for performing byte-swapping
 *      by value and in place by pointer for 16-, 32-, and 64-bit
 *      types.
 *
 */

#ifndef NLBYTEORDER_HPP
#define NLBYTEORDER_HPP

#include <nlbyteorder.h>

namespace nl
{

    namespace ByteOrder
    {
        enum {
            Unknown	 = nlByteOrderUnknown,
            LittleEndian = nlByteOrderLittleEndian,
            BigEndian    = nlByteOrderBigEndian    
        };

        /**
         * This represents a type for a byte ordering.
         */
        typedef nlByteOrder ByteOrder;

        /**
         * This returns the byte order of the current system.
         *
         * @return The byte order of the current system.
         */
        inline ByteOrder GetCurrent(void)
        {
            return nlByteOrderGetCurrent();
        }

        /**
         * This unconditionally performs a byte order swap by value of the
         * specified 16-bit value.
         *
         * @param[in]  inValue  The 16-bit value to be byte order swapped.
         *
         * @return The input value, byte order swapped.
         */
        inline uint16_t Swap16(uint16_t inValue)
        {
            return nlByteOrderValueSwap16(inValue);
        }

        /**
         * This unconditionally performs a byte order swap by value of the
         * specified 32-bit value.
         *
         * @param[in]  inValue  The 32-bit value to be byte order swapped.
         *
         * @return The input value, byte order swapped.
         */
        inline uint32_t Swap32(uint32_t inValue)
        {
            return nlByteOrderValueSwap32(inValue);
        }

        /**
         * This unconditionally performs a byte order swap by value of the
         * specified 64-bit value.
         *
         * @param[in]  inValue  The 64-bit value to be byte order swapped.
         *
         * @return The input value, byte order swapped.
         */
        inline uint64_t Swap64(uint64_t inValue)
        {
            return nlByteOrderValueSwap64(inValue);
        }

        /**
         * This unconditionally performs a byte order swap by pointer in place
         * of the specified 16-bit value.
         *
         * @warning  The input value is assumed to be on a natural alignment
         * boundary for the target system. It is the responsibility of the
         * caller to perform any necessary alignment to avoid system faults
         * for systems that do not support unaligned accesses.
         *
         * @param[inout]  inValue  A pointer to the 16-bit value to be byte
         *                         order swapped.
         */
        inline void Swap16(uint16_t *inValue)
        {
            nlByteOrderPointerSwap16(inValue);
        }

        /**
         * This unconditionally performs a byte order swap by pointer in place
         * of the specified 32-bit value.
         *
         * @warning  The input value is assumed to be on a natural alignment
         * boundary for the target system. It is the responsibility of the
         * caller to perform any necessary alignment to avoid system faults
         * for systems that do not support unaligned accesses.
         *
         * @param[inout]  inValue  A pointer to the 32-bit value to be byte
         *                         order swapped.
         */
        inline void Swap32(uint32_t *inValue)
        {
            nlByteOrderPointerSwap32(inValue);
        }

        /**
         * This unconditionally performs a byte order swap by pointer in place
         * of the specified 64-bit value.
         *
         * @warning  The input value is assumed to be on a natural alignment
         * boundary for the target system. It is the responsibility of the
         * caller to perform any necessary alignment to avoid system faults
         * for systems that do not support unaligned accesses.
         *
         * @param[inout]  inValue  A pointer to the 64-bit value to be byte
         *                         order swapped.
         */
        inline void Swap64(uint64_t *inValue)
        {
            nlByteOrderPointerSwap64(inValue);
        }

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
        inline uint16_t Swap16LittleToHost(uint16_t inValue)
        {
            return nlByteOrderSwap16LittleToHost(inValue);
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
        inline uint32_t Swap32LittleToHost(uint32_t inValue)
        {
            return nlByteOrderSwap32LittleToHost(inValue);
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
        inline uint64_t Swap64LittleToHost(uint64_t inValue)
        {
            return nlByteOrderSwap64LittleToHost(inValue);
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
        inline uint16_t Swap16HostToLittle(uint16_t inValue)
        {
            return nlByteOrderSwap16HostToLittle(inValue);
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
        inline uint32_t Swap32HostToLittle(uint32_t inValue)
        {
            return nlByteOrderSwap32HostToLittle(inValue);
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
        inline uint64_t Swap64HostToLittle(uint64_t inValue)
        {
            return nlByteOrderSwap64HostToLittle(inValue);
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
        inline uint16_t Swap16BigToHost(uint16_t inValue)
        {
            return nlByteOrderSwap16BigToHost(inValue);
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
        inline uint32_t Swap32BigToHost(uint32_t inValue)
        {
            return nlByteOrderSwap32BigToHost(inValue);
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
        inline uint64_t Swap64BigToHost(uint64_t inValue)
        {
            return nlByteOrderSwap64BigToHost(inValue);
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
        inline uint16_t Swap16HostToBig(uint16_t inValue)
        {
            return nlByteOrderSwap16HostToBig(inValue);
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
        inline uint32_t Swap32HostToBig(uint32_t inValue)
        {
            return nlByteOrderSwap32HostToBig(inValue);
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
        inline uint64_t Swap64HostToBig(uint64_t inValue)
        {
            return nlByteOrderSwap64HostToBig(inValue);
        }

    } // namespace ByteOrder

} // namespace nl

#endif // NLBYTEORDER_HPP












