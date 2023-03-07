/*******************************************************************************
* The confidential and proprietary information contained in this file may      *
* only be used by a person authorised under and to the extent permitted        *
* by a subsisting licensing agreement from ARM Limited or its affiliates.      *
*   (C) COPYRIGHT [2001-2017] ARM Limited or its affiliates.                   *
*       ALL RIGHTS RESERVED                                                    *
* This entire notice must be reproduced on all copies of this file             *
* and copies of this file may only be made by a person if such person is       *
* permitted to do so under the terms of a subsisting license agreement         *
* from ARM Limited or its affiliates.                                          *
*******************************************************************************/

#ifndef TEST_PAL_THREAD_H_
#define TEST_PAL_THREAD_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *ThreadHandle;

/******************************************************************************/
/*
 * @brief This function returns the minimal stack size in bytes.
 *
 * @param[in]

 * @param[out]
 *
 * @return - Minimal stack size in bytes.
 */
size_t Test_PalGetMinimalStackSize(void);

/******************************************************************************/
/*
 * @brief This function returns the highest thread priority.
 *
 * @param[in]

 * @param[out]
 *
 * @return - Highest thread priority.
 */
uint32_t Test_PalGetHighestPriority(void);

/******************************************************************************/
/*
 * @brief This function returns the lowest thread priority.
 *
 * @param[in]

 * @param[out]
 *
 * @return - Lowest thread priority.
 */
uint32_t Test_PalGetLowestPriority(void);

/******************************************************************************/
/*
 * @brief This function returns the default thread priority.
 *
 * @param[in]

 * @param[out]
 *
 * @return - Default thread priority.
 */
uint32_t Test_PalGetDefaultPriority(void);

/******************************************************************************/
/*
 * @brief This function creates a thread. The user should call
 * Test_PalThreadJoin() in order to wait until the thread ends and then to
 * Test_PalThreadDestroy() in order to free resources.
 * In case of a thread without an end, the user shouldn't call
 * Test_PalThreadJoin() which will not return. Instead, the user should call
 * Test_PalThreadDestroy() which will cancel the thread and free
 * its resources.
 *
 * @param[in]
 * stackSize - Thread stack size in bytes. The allocated stack size will be the
 * biggest between stackSize and the minimal stack size.
 * threadFunc - Thread function. The function shall return a pointer to the
 * returned value or NULL. In case TZM is supported, this function must have the
 * same security attribute as TestAL's (either secure or non-secure).
 * priority - Thread priority. Highest and lowest priorities can be received
 * by calling Test_PalGetLowestPriority() and Test_PalGetHighestPriority()
 * accordingly.
 * args - Function input arguments.
 * threadName - Thread name. Not in use for Linux.
 * nameLen - Thread name length. Not in use for Linux.
 * dmaAble - Determines whether the stack should be DMA-able (true)
 * or not (false).
 *
 * @param[out]
 *
 * @return - threadHandle address for success, NULL for failure.
 */
ThreadHandle Test_PalThreadCreate(size_t stackSize,
                  void *(*threadFunc)(void *),
                  int priority, void *args,
                  const char *threadName,
                  uint8_t nameLen, uint8_t dmaAble);

/******************************************************************************/
/*
 * @brief This function waits for a thread to terminate (BLOCKING).
 * If that thread has already terminated it returns immediately.
 *
 * @param[in]
 * threadHandle - Thread structure. Not in use for FreeRTOS.
 * threadRet - A pointer to pointer to the returned value of the target thread.
 * Note that threadRet is not changed, yet *threadRet is changed and
 * can be NULL. Hence, the user shall not try accessing **threadRet without
 * checking that *threadRet in not NULL.
 *
 * @param[out]
 *
 * @return rc - 0 for success, 1 for failure.
 */
uint32_t Test_PalThreadJoin(ThreadHandle threadHandle, void **threadRet);

/******************************************************************************/
/*
 * @brief This function destroys a thread (if it's still running) and frees
 * its resources.
 * In order to free thread resources only after thread's end this function
 * should be called after Test_PalThreadJoin.
 * In order to cancel the thread immediately and free its resources, this
 * function should be called alone (i.e. without Test_PalThreadJoin).
 * It must eventually be called in any case.
 * Note that this function doesn't deallocate the memory that the
 * thread itself allocates. This needs to be done by the thread itself.
 *
 * @param[in]
 * threadHandle - Thread structure.
 *
 * @param[out]
 *
 * @return rc - 0 for success, 1 for failure.
 */
uint32_t Test_PalThreadDestroy(ThreadHandle threadHandle);

#ifdef __cplusplus
}
#endif

#endif /* TEST_PAL_THREAD_H_ */
