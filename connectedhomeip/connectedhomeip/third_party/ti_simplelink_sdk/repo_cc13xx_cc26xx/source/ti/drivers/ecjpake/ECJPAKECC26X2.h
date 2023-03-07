/*
 * Copyright (c) 2017-2019, Texas Instruments Incorporated
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
/** ============================================================================
 *  @file       ECJPAKECC26X2.h
 *
 *  @brief      ECJPAKE driver implementation for the CC26X2 family
 *
 *  This file should only be included in the board file to fill the ECJPAKE_config
 *  struct.
 *
 *  # Hardware and Implementation Details #
 *
 *  The CC26X2 family has a dedicated public key accelerator.
 *  It is capable of multiple mathematical operations including dedicated ECC point addition, doubling,
 *  and scalar multiplication. Only one operation can be carried out on the accelerator
 *  at a time. Mutual exclusion is implemented at the driver level and coordinated
 *  between all drivers relying on the accelerator. It is transparent to the application
 *  and only noted ensure sensible access timeouts are set.
 *
 *  The large number maths engine (LNME) uses a dedicated 2kB block of RAM (PKA RAM) for its operations.
 *  The operands of the maths operations must be copied into and results out of the PKA ram.
 *  This necessitates a significant number of reads and writes for each operation.
 *  The bus interface to the RAM only allows for word-aligned reads and writes. The CPU splits
 *  the reads and writes from and to general SRAM from unaligned addresses into multiple
 *  bus operations while accumulating the data in a register until it is full.
 *  The result of this hardware process is that providing buffers such as plaintext CryptoKey
 *  keying material to ECC APIs that are word-aligned will significantly speed up the operation and
 *  reduce power consumption.
 *
 *  The driver implementation does not perform runtime checks for most input parameters.
 *  Only values that are likely to have a stochastic element to them are checked (such
 *  as whether a driver is already open). Higher input paramter validation coverage is
 *  achieved by turning on assertions when compiling the driver.
 *
 *  # Supported Curve Types #
 *
 *  The driver implementation supports the following curve types for ECJPAKE:
 *
 *  | Curve Type        | Supported |
 *  |-------------------|-----------|
 *  | Short Weierstrass | Yes       |
 *  | Montgomery        | No        |
 *  | Edwards           | No        |
 *
 *  # Public Key Validation #
 *
 *  When performing Schnorr-ZKP verification, the foreign public key will always be validated.
 *  The public V is validated implicitly during the operation.
 *  The implementation assumes that the cofactor, h, of the curve is 1. This lets us
 *  skip the computationally expensive step of multiplying the foreign key by the order and
 *  checking if it yields the point at infinity. When the cofactor is 1, this property is
 *  implied by validating that the point is not already the point at infinity and that it
 *  validates against the curve equation.
 *  All curves supplied by default, the NIST and Brainpool curves, have cofactor = 1. While
 *  the implementation can use arbitrary curves, you should verify that any other curve used
 *  has a cofactor of 1.
 */

#ifndef ti_drivers_ecjpake_ECJPAKECC26X2__include
#define ti_drivers_ecjpake_ECJPAKECC26X2__include

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/ECJPAKE.h>
#include <ti/drivers/cryptoutils/ecc/ECCParams.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Exit the SWI and wait until an HWI call posts the SWI again */
#define ECJPAKECC26X2_STATUS_FSM_RUN_PKA_OP       ECJPAKE_STATUS_RESERVED - 0
/* Execute the next FSM state immediately without waiting for the next HWI */
#define ECJPAKECC26X2_STATUS_FSM_RUN_FSM          ECJPAKE_STATUS_RESERVED - 1

/*!
 *  @brief      ECJPAKECC26X2 states
 *
 *  The EC-JPAKE operations are implemented using multiple invidividual
 *  PKA operations. Since state transitions for these operations are almost
 *  always predictable, the state transitions are encoded linearly in this enum.
 *  The FSM controller will increment the state counter and iterate through
 *  states until it is told to stop or restart.
 */
typedef enum {
    ECJPAKECC26X2_FSM_ERROR = 0,
    ECJPAKECC26X2_FSM_ROUND_ONE_GENERATE_MYPUBLICKEY1,
    ECJPAKECC26X2_FSM_ROUND_ONE_GENERATE_MYPUBLICKEY1_RESULT,
    ECJPAKECC26X2_FSM_ROUND_ONE_GENERATE_MYPUBLICKEY2,
    ECJPAKECC26X2_FSM_ROUND_ONE_GENERATE_MYPUBLICKEY2_RESULT,
    ECJPAKECC26X2_FSM_ROUND_ONE_GENERATE_MYPUBLICV1,
    ECJPAKECC26X2_FSM_ROUND_ONE_GENERATE_MYPUBLICV1_RESULT,
    ECJPAKECC26X2_FSM_ROUND_ONE_GENERATE_MYPUBLICV2,
    ECJPAKECC26X2_FSM_ROUND_ONE_GENERATE_MYPUBLICV2_RESULT,
    ECJPAKECC26X2_FSM_ROUND_ONE_GENERATE_RETURN,

    ECJPAKECC26X2_FSM_GENERATE_ZKP_PRIVATEKEY_X_HASH,
    ECJPAKECC26X2_FSM_GENERATE_ZKP_PRIVATEKEY_X_HASH_RESULT,
    ECJPAKECC26X2_FSM_GENERATE_ZKP_PRIVATEKEY_X_HASH_MOD_N,
    ECJPAKECC26X2_FSM_GENERATE_ZKP_PRIVATEKEY_X_HASH_MOD_N_RESULT,
    ECJPAKECC26X2_FSM_GENERATE_ZKP_ADD_V_TO_N,
    ECJPAKECC26X2_FSM_GENERATE_ZKP_ADD_V_TO_N_RESULT,
    ECJPAKECC26X2_FSM_GENERATE_ZKP_SUBTRACT_RESULTS,
    ECJPAKECC26X2_FSM_GENERATE_ZKP_SUBTRACT_RESULTS_RESULT,
    ECJPAKECC26X2_FSM_GENERATE_ZKP_SUBTRACT_RESULT_MOD_N,
    ECJPAKECC26X2_FSM_GENERATE_ZKP_SUBTRACT_RESULT_MOD_N_RESULT,
    ECJPAKECC26X2_FSM_ZKP_GENERATE_RETURN,

    ECJPAKECC26X2_FSM_VERIFY_ZKP_VALIDATE_PUBLIC_KEY,
    ECJPAKECC26X2_FSM_VERIFY_ZKP_HASH_MOD_N,
    ECJPAKECC26X2_FSM_VERIFY_ZKP_HASH_MOD_N_RESULT,
    ECJPAKECC26X2_FSM_VERIFY_ZKP_MULT_G_BY_R,
    ECJPAKECC26X2_FSM_VERIFY_ZKP_MULT_G_BY_R_RESULT,
    ECJPAKECC26X2_FSM_VERIFY_ZKP_MULT_X_BY_HASH,
    ECJPAKECC26X2_FSM_VERIFY_ZKP_MULT_X_BY_HASH_RESULT,
    ECJPAKECC26X2_FSM_VERIFY_ZKP_ADD_RESULTS,
    ECJPAKECC26X2_FSM_VERIFY_ZKP_ADD_RESULTS_RESULT,
    ECJPAKECC26X2_FSM_VERIFY_ZKP_COMPARE_AGAINST_V,

    ECJPAKECC26X2_FSM_ROUND_TWO_MULT_MYPRIVATEKEY2_BY_PRESHAREDSECRET,
    ECJPAKECC26X2_FSM_ROUND_TWO_MULT_MYPRIVATEKEY2_BY_PRESHAREDSECRET_RESULT,
    ECJPAKECC26X2_FSM_ROUND_TWO_MYCOMBINEDPRIVATEKEY_MOD_N,
    ECJPAKECC26X2_FSM_ROUND_TWO_MYCOMBINEDPRIVATEKEY_MOD_N_RESULT,
    ECJPAKECC26X2_FSM_ROUND_TWO_ADD_MYPUBLICKEY1_TO_THEIRPUBLICKEY1,
    ECJPAKECC26X2_FSM_ROUND_TWO_ADD_MYPUBLICKEY1_TO_THEIRPUBLICKEY1_RESULT,
    ECJPAKECC26X2_FSM_ROUND_TWO_ADD_THEIRPUBLICKEY2,
    ECJPAKECC26X2_FSM_ROUND_TWO_ADD_THEIRPUBLICKEY2_RESULT,
    ECJPAKECC26X2_FSM_ROUND_TWO_ADD_MYPUBLICKEY2,
    ECJPAKECC26X2_FSM_ROUND_TWO_ADD_MYPUBLICKEY2_RESULT,
    ECJPAKECC26X2_FSM_ROUND_TWO_MULT_MYCOMBINEDPRIVATEKEY_BY_MYNEWGENERATOR,
    ECJPAKECC26X2_FSM_ROUND_TWO_MULT_MYCOMBINEDPRIVATEKEY_BY_MYNEWGENERATOR_RESULT,
    ECJPAKECC26X2_FSM_ROUND_TWO_GENERATE_MYPUBLICV,
    ECJPAKECC26X2_FSM_ROUND_TWO_GENERATE_MYPUBLICV_RESULT,
    ECJPAKECC26X2_FSM_ROUND_TWO_GENERATE_RETURN,

    ECJPAKECC26X2_FSM_GENERATE_SHARED_SECRET_MULT_THEIRPUBLICKEY2_BY_MYCOMBINEDPRIVATEKEY,
    ECJPAKECC26X2_FSM_GENERATE_SHARED_SECRET_MULT_THEIRPUBLICKEY2_BY_MYCOMBINEDPRIVATEKEY_RESULT,
    ECJPAKECC26X2_FSM_GENERATE_SHARED_SECRET_SUB_YCOORDINATE_FROM_PRIME,
    ECJPAKECC26X2_FSM_GENERATE_SHARED_SECRET_SUB_YCOORDINATE_FROM_PRIME_RESULT,
    ECJPAKECC26X2_FSM_GENERATE_SHARED_SECRET_ADD_THEIRCOMBINEDPUBLICKEY,
    ECJPAKECC26X2_FSM_GENERATE_SHARED_SECRET_ADD_THEIRCOMBINEDPUBLICKEY_RESULT,
    ECJPAKECC26X2_FSM_GENERATE_SHARED_SECRET_MULT_BY_MYPRIVATEKEY2,
    ECJPAKECC26X2_FSM_GENERATE_SHARED_SECRET_MULT_BY_MYPRIVATEKEY2_RESULT,
    ECJPAKECC26X2_FSM_GENERATE_SHARED_SECRET_RETURN,

} ECJPAKECC26X2_FsmState;

/*!
 *  @brief      ECJPAKECC26X2 Hardware Attributes
 *
 *  ECJPAKECC26X2 hardware attributes should be included in the board file
 *  and pointed to by the ECJPAKE_config struct.
 */
typedef struct {
    /*! @brief PKA Peripheral's interrupt priority.

        The CC26xx uses three of the priority bits, meaning ~0 has the same effect as (7 << 5).

        (7 << 5) will apply the lowest priority.

        (1 << 5) will apply the highest priority.

        Setting the priority to 0 is not supported by this driver.

        HWI's with priority 0 ignore the HWI dispatcher to support zero-latency interrupts, thus invalidating the critical sections in this driver.
    */
    uint8_t    intPriority;
} ECJPAKECC26X2_HWAttrs;

/*!
 *  @brief      ECJPAKECC26X2 Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct {
    bool                                isOpen;
    bool                                operationInProgress;
    bool                                operationCanceled;
    int_fast16_t                        operationStatus;
    ECJPAKE_CallbackFxn                 callbackFxn;
    ECJPAKE_ReturnBehavior              returnBehavior;
    ECJPAKECC26X2_FsmState              fsmState;
    ECJPAKE_Operation                   operation;
    ECJPAKE_OperationType               operationType;
    uint32_t                            semaphoreTimeout;
    uint32_t                            resultAddress;
} ECJPAKECC26X2_Object;

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_ecjpake_ECJPAKECC26X2__include */
