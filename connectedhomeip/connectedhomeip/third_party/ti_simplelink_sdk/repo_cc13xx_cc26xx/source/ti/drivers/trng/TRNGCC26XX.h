/*
 * Copyright (c) 2018-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!****************************************************************************
 *  @file       TRNGCC26XX.h
 *
 *  @brief      TRNG driver implementation for the CC26XX family
 *
 *  This file should only be included in the board file to fill the TRNG_config
 *  struct.
 *
 *  # Hardware
 *
 *  The CC26XX family has a dedicated hardware TRNG based on sampling multiple
 *  free running oscillators. With all FROs enabled, the TRNG hardware generates
 *  64 bits of entropy approximately every 5ms. The driver implementation
 *  chains multiple 64-bit entropy generation operations together to generate
 *  an arbitrary amount of entropy.
 *
 *  # Behaviour
 *
 *  The driver keeps a global pool of entropy available to service any requests
 *  immediately. If the pool is not sufficiently large or is too depleted to
 *  service a request, entropy will be generated until the request is serviced.
 *  If there is an ongoing TRNG request that the hardware is generating entropy
 *  for, the driver will queue any other requests and work them off in FIFO
 *  order.
 *
 *  Requests issued by driver instances with polling return behaviour will
 *  preempt any executing requests and be serviced immediately.
 *
 *  After the request queue is emptied, the driver will start an asynchronous
 *  operation in the background to refill the depleted entropy pool.
 *
 *  # Samples Per Cycle
 *
 *  The EIP-75t HW takes 240,000 clock cycles in the default setting to generate
 *  one round of output (i.e. 64 bits). This comes to 5ms on a 48MHz clock. The
 *  clock cycles per round can be configured to be as low as 2^8 (256) to as high
 *  as 2^24 (16,777,216).
 *  Entropy re-generation time can be tailored in a trade-off between speed of
 *  random number generation and amount of entropy in each of those random numbers.
 */

#ifndef ti_drivers_TRNG_TRNGCC26XX__include
#define ti_drivers_TRNG_TRNGCC26XX__include

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/TRNG.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! @brief Minimum random samples for each entropy generation call */
#define TRNGCC26XX_SAMPLES_PER_CYCLE_MIN        256
/*! @brief Default random samples for each entropy generation call
 *
 *  Set to generate 64 bits of randomness in 5ms with all FROs active. */
#define TRNGCC26XX_SAMPLES_PER_CYCLE_DEFAULT    240000
/*! @brief Maximum random samples for each entropy generation call */
#define TRNGCC26XX_SAMPLES_PER_CYCLE_MAX        16777216

/*! @brief Minimum number of bytes provided by the TRNG hardware in one go.
 *
 *  Smaller amounts can by requested in driver
 *  calls but the full number will always be generated.
 *  Part of the generated entropy will simply not be copied
 *  back to the target buffer if the requested length is not
 *  a multiple of TRNGCC26XX_MIN_BYTES_PER_ITERATION.
 */
#define TRNGCC26XX_MIN_BYTES_PER_ITERATION      (2 * sizeof(uint32_t))


/*! @brief Default TRNGCC26XX entropy pool size in 64-bit elements
 *
 *  By default, the entropy pool is sized to immediately fulfill a 256-bit
 *  entropy generation request. If we assume that the TRNG is used infrequently,
 *  this should greatly decrease the latency of <= 256-bit requests.
 *  This value may be overridden by defining it at the project level and
 *  recompiling the driver.
 */
#ifndef TRNGCC26XX_ENTROPY_POOL_SIZE
#define TRNGCC26XX_ENTROPY_POOL_SIZE (32 / sizeof(uint64_t))
#endif

/*!
 *  @brief      TRNGCC26XX Hardware Attributes
 *
 *  TRNG26X0 hardware attributes should be included in the board file
 *  and pointed to by the TRNG_config struct.
 */
typedef struct {
    /*! @brief Crypto Peripheral's interrupt priority.

        The CC26xx uses three of the priority bits, meaning ~0 has the same effect as (7 << 5).

        (7 << 5) will apply the lowest priority.

        (1 << 5) will apply the highest priority.

        Setting the priority to 0 is not supported by this driver.

        HWI's with priority 0 ignore the HWI dispatcher to support zero-latency interrupts, thus invalidating the critical sections in this driver.
    */
    uint8_t    intPriority;
    /*! @brief TRNG SWI priority.
        The higher the number, the higher the priority.
        The minimum is 0 and the maximum is 15 by default.
        The maximum can be reduced to save RAM by adding or modifying Swi.numPriorities in the kernel configuration file.
    */
    uint32_t   swiPriority;
    /*! @brief TRNG Maximum Samples per Cycle.
        Changes the maximum number of randomness samples in each entropy generation cycle before dump and interrupt.
        The minimum is 2^8 (256) and the maximum is 2^24 (16777216).
        The default is 240000 - enough to generate 64 bits of randomness at 5MHz.
    */
    uint32_t   samplesPerCycle;
} TRNGCC26XX_HWAttrs;

/*! \cond Internal APIs */

/*!
 *  The application must not access any member variables of this structure!
 */
typedef struct {
    List_Elem                       listElement;        /* Must start with a List_Elem
                                                         * to allow casting of List_Elem
                                                         * pointer returned by List APIs
                                                         * to TRNGCC26XX_Object pointers.
                                                         */
    TRNG_Handle                     handle;
    TRNG_CryptoKeyCallbackFxn       cryptoKeyCallbackFxn;
    TRNG_RandomBytesCallbackFxn     randomBytesCallbackFxn;
    uint32_t                        samplesPerCycle;
    CryptoKey                       *entropyKey;
    uint8_t                         *entropyBuffer;
    uint32_t                        semaphoreTimeout;
    size_t                          entropyGenerated;
    size_t                          entropyRequested;
    int_fast16_t                    returnStatus;
    TRNG_ReturnBehavior             returnBehavior;
    bool                            isOpen;
    bool                            isEnqueued;
    SemaphoreP_Struct               operationSemaphore;
} TRNGCC26XX_Object;

/*! \endcond */

/*!
 *  @brief  Sets the number of entropy generation cycles before
 *          the results are returned.
 *
 *  The default value is set to generate 64 bits of entropy.
 *
 *  @pre    TRNG_open() has to be called first successfully
 *
 *  @param  handle      A TRNGCC26XX handle returned from TRNGCC26XX_open
 *  @param  samplesPerCycle  Number of 48MHz clock cycles to sample. Must be between 2^8 and 2^24.
 *
 *  @sa     TRNG_open()
 */
extern int_fast16_t TRNGCC26XX_setSamplesPerCycle(TRNG_Handle handle, uint32_t samplesPerCycle);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_TRNG_TRNGCC26XX__include */
