/***************************************************************************//**
 * @file
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

/**
 * @addtogroup memory_buffer
 * @brief Ember Connect API dynamically allocates and frees memory.
 *
 * Generally, dynamic memory allocation in embedded code is not recommended.
 * However, in some cases, the drawbacks of avoiding them is even bigger. Using
 * C standard library dynamic memory is still not recommended due because it
 * could cause fragmented memory.
 *
 * For these reasons, Connect allocates some (configurable in the stack common
 * plugin) memory as HEAP at compile-time. Memory allocation from this heap is
 * possible through the API below.
 *
 * See memory-buffer.h for source code.
 * @{
 */

/** @brief A special ::EmberBuffer ID indicating that no memory is currently
 * allocated.
 */
#define EMBER_NULL_BUFFER    0x0000u

/**
 * @name Handlers
 * @anchor Handlers-memory-buffer
 * The Application Framework implements all handlers, directly calling their
 * associated callbacks. By default, Connect projects declare such callbacks as
 * stubs in flex-callbacks-stubs.c. Hence, to use an enabled Connect feature,
 * applications should replace the stub with their own implementation of the
 * associated callback (typically in flex-callbacks.c).  See UG235.04 for more
 * info.
 * @{
 */

/**
 * @brief This handler is invoked by the memory buffers system garbage
 *   collector and allows the application to properly mark the application-defined
 *   ::EmberBuffer variables with ::emberMarkBuffer().
 * @warning Implement associated callback
 *   @ref emberAfMarkApplicationBuffersCallback() to use. See
 *  @ref Handlers-memory-buffer "Handlers" for additional information.
 */
//void emberMarkApplicationBuffersHandler(void);

/** @} END Handlers */

/** @name APIs
 * @{
 */

/** @brief Dynamically allocates memory.
 *
 * @param[in] dataSizeInBytes   The size in bytes of the memory to be allocated.
 *
 * @return An ::EmberBuffer value of ::EMBER_NULL_BUFFER if the memory
 * management system could not allocate the requested memory, or any other
 * ::EmberBuffer value indicating that the requested memory was successfully
 * allocated.
 * The allocated memory can easily be freed by assigning an ::EmberBuffer
 * variable to EMBER_NULL_BUFFER. The memory will be freed by the garbage
 * collector during the next ::emberTick() call.
 */
EmberBuffer emberAllocateBuffer(uint16_t dataSizeInBytes);

/** @brief Prevent the garbage collector from reclaiming the memory
 * associated with the passed ::EmberBuffer. The application should call
 * this API within the ::emberMarkApplicationBuffersHandler() stack handler
 * for each ::EmberBuffer object.
 *
 * @param[in] buffer   A pointer to the ::EmberBuffer buffer to be marked.
 */
void emberMarkBuffer(EmberBuffer *buffer);

/** @brief Return a pointer to the memory segment corresponding to
 * the passed ::EmberBuffer buffer. Notice that the garbage collector can move
 * memory segments to defragment the available memory. As result, the
 * application should always use this API to obtain an updated pointer prior to
 * accessing the memory.
 *
 * @param[in] buffer   A pointer to the ::EmberBuffer buffer for which the
 * corresponding memory pointer should be returned.
 *
 * @return  A NULL pointer if the passed ::EmberBuffer value is
 * ::EMBER_NULL_BUFFER. Otherwise, a pointer to the corresponding memory
 * segment.
 */
uint8_t *emberGetBufferPointer(EmberBuffer buffer);

/** @brief Return the length in bytes of the passed ::EmberBuffer
 * buffer.
 *
 * @param[in] buffer   A pointer to the ::EmberBuffer buffer for which the
 * corresponding length in bytes should be returned.
 *
 * @return  The length in bytes of a memory segment corresponding to the passed
 * ::EmberBuffer buffer.
 */
uint16_t emberGetBufferLength(EmberBuffer buffer);

/** @brief Return the available memory at the buffer manager in
 * bytes.
 *
 * @return  The number of available bytes.
 */
uint16_t emberGetAvailableBufferMemory(void);

/** @} END APIs */

/** @} END addtogroup */
