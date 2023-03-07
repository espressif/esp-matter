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

#ifndef TEST_PAL_SEMPHR_H_
#define TEST_PAL_SEMPHR_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INFINITE 0xFFFFFFFF

typedef void *Test_PalMutex;
typedef void *Test_PalBinarySemaphore;

/******************************************************************************/
/**
 * @brief This function creates a mutex.
 *
 * @param[in]
 *
 * @param[out]
 * ppMutexId - pointer to the created Test_PalMutex.
 *
 * @return - 0 on success, 1 for failure.
 */
uint8_t Test_PalMutexCreate(Test_PalMutex *ppMutexId);

/******************************************************************************/
/**
 * @brief This function destroys a mutex.
 *
 * @param[in]
 * ppMutexId - pointer to Test_PalMutex.
 *
 * @param[out]
 *
 * @return - 0.
 */
uint8_t Test_PalMutexDestroy(Test_PalMutex *ppMutexId);

/******************************************************************************/
/**
 * @brief This function waits for a mutex with timeout. The timeout is
 * specified in milliseconds. INFINITE is blocking.
 *
 * @param[in]
 * ppMutexId - pointer to Test_PalMutex.
 * timeout - Timeout in msec, or INFINITE.
 *
 * @return - 0 on success, 1 for failure.
 */
uint8_t Test_PalMutexLock(Test_PalMutex *ppMutexId, uint32_t timeout);

/******************************************************************************/
/**
 * @brief This function releases a mutex.
 * Must not be used from an ISR.
 *
 * @param[in]
 * ppMutexId - pointer to Test_PalMutex.
 *
 * @param[out]
 *
 * @return - 0 on success, 1 for failure.
 */
uint8_t Test_PalMutexUnlock(Test_PalMutex *ppMutexId);

/******************************************************************************/
/**
 * @brief This function creates a binary semaphore.
 *
 * @param[in]
 *
 * @param[out]
 * ppBinSemphrId - Pointer to the created Test_PalBinarySemaphore.
 *
 * @return - 0 on success, 1 for failure.
 */
uint8_t Test_PalBinarySemaphoreCreate(Test_PalBinarySemaphore *ppBinSemphrId);

/******************************************************************************/
/**
 * @brief This function destroys a binary semaphore.
 *
 * @param[in]
 * ppBinSemphrId - Pointer to Test_PalBinarySemaphore.
 *
 * @param[out]
 *
 * @return - 0.
 */
uint8_t Test_PalBinarySemaphoreDestroy(Test_PalBinarySemaphore *ppBinSemphrId);

/******************************************************************************/
/**
 * @brief This function waits for a binary semaphore with timeout. The timeout
 * is specified in milliseconds. INFINITE is blocking.
 *
 * @param[in]
 * ppBinSemphrId - Pointer to Test_PalBinarySemaphore.
 * timeout - Timeout in msec, or INFINITE.
 *
 * @param[out]
 *
 * @return - 0 on success, 1 for failure.
 */
uint8_t Test_PalBinarySemaphoreTake(Test_PalBinarySemaphore *ppBinSemphrId,
                            uint32_t timeout);

/******************************************************************************/
/**
 * @brief This function releases a binary semaphore.
 *
 * @param[in]
 * ppBinSemphrId - Pointer to Test_PalBinarySemaphore.
 *
 * @param[out]
 *
 * @return - 0 on success, 1 for failure.
 */
uint8_t Test_PalBinarySemaphoreGive(Test_PalBinarySemaphore *ppBinSemphrId);

#ifdef __cplusplus
}
#endif

#endif /* TEST_PAL_SEMPHR_H_ */
