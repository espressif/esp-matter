/***************************************************************************//**
 * @brief Macros and APIs defined for packet-buffer.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

// None of these should endup in doxygen
#ifndef DOXYGEN_SHOULD_SKIP_THIS

extern EmberMessageBuffer *emPacketBufferLinks;

#define LOG_PACKET_BUFFER_SIZE 5
#define PACKET_BUFFER_SIZE (1 << LOG_PACKET_BUFFER_SIZE)

#define emberAllocateStackBuffer() (emberAllocateLinkedBuffers(1))
#define emberStackBufferLink(buffer) \
  (emPacketBufferLinks[(buffer)])
#define emberLinkedBufferContents(buffer) emberMessageBufferContents(buffer)

EmberMessageBuffer emberAllocateLinkedBuffers(uint8_t count);
sl_status_t emberAppendToLinkedBuffers(EmberMessageBuffer buffer,
                                       uint8_t *contents,
                                       uint8_t length);
uint16_t emberMessageBufferLength(EmberMessageBuffer buffer);
void emberHoldMessageBuffer(EmberMessageBuffer buffer);
void emberReleaseMessageBuffer(EmberMessageBuffer buffer);
uint8_t *emberMessageBufferContents(EmberMessageBuffer buffer);
EmberStatus emberAppendPgmStringToLinkedBuffers(EmberMessageBuffer buffer, PGM_P suffix);
EmberMessageBuffer emberFillLinkedBuffers(uint8_t *contents, uint8_t length);

#endif // !DOXYGEN_SHOULD_SKIP_THIS
