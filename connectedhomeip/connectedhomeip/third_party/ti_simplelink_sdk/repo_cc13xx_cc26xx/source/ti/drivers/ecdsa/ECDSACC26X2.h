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
 *  @file       ECDSACC26X2.h
 *
 *  @brief      ECDSA driver implementation for the CC26X2 family
 *
 *  This file should only be included in the board file to fill the ECDSA_config
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
 *  The driver implementation supports the following curve types for ECDSA:
 *
 *  | Curve Type        | Supported |
 *  |-------------------|-----------|
 *  | Short Weierstrass | Yes       |
 *  | Montgomery        | No        |
 *  | Edwards           | No        |
 *
 *  # Public Key Validation #
 *
 *  When performing signature verification, the foreign public key will always be validated.
 *  However, the implementation assumes that the cofactor, h, of the curve is 1. This lets us
 *  skip the computationally expensive step of multiplying the foreign key by the order and
 *  checking if it yields the point at infinity. When the cofactor is 1, this property is
 *  implied by validating that the point is not already the point at infinity and that it
 *  validates against the curve equation.
 *  All curves supplied by default, the NIST and Brainpool curves, have cofactor = 1. While
 *  the implementation can use arbitrary curves, you should verify that any other curve used
 *  has a cofactor of 1.
 */

#ifndef ti_drivers_ecdsa_ECDSACC26X2__include
#define ti_drivers_ecdsa_ECDSACC26X2__include

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/ECDSA.h>
#include <ti/drivers/TRNG.h>
#include <ti/drivers/trng/TRNGCC26XX.h>
#include <ti/drivers/cryptoutils/ecc/ECCParams.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Exit the SWI and wait until an HWI call posts the SWI again */
#define ECDSACC26X2_STATUS_FSM_RUN_PKA_OP       ECDSA_STATUS_RESERVED - 0
/* Execute the next FSM state immediately without waiting for the next HWI */
#define ECDSACC26X2_STATUS_FSM_RUN_FSM          ECDSA_STATUS_RESERVED - 1

/*!
 *  @brief      ECDSACC26X2 Sign and Verify states
 *
 *  The sign and verify operations are implemented using multiple invidividual
 *  PKA operations. Since state transitions for these operations are almost
 *  always predictable, the state transitions are encoded linearly in this enum.
 *  The FSM controller will increment the state counter and iterate through
 *  states until it is told to stop or restart.
 */
typedef enum {
    ECDSACC26X2_FSM_ERROR = 0,

    ECDSACC26X2_FSM_SIGN_COMPUTE_R,
    ECDSACC26X2_FSM_SIGN_COMPUTE_R_RESULT,
    ECDSACC26X2_FSM_SIGN_R_MOD_N,
    ECDSACC26X2_FSM_SIGN_R_MOD_N_RESULT,
    ECDSACC26X2_FSM_SIGN_COMPUTE_PMSN_INVERSE,
    ECDSACC26X2_FSM_SIGN_COMPUTE_PMSN_INVERSE_RESULT,
    ECDSACC26X2_FSM_SIGN_COMPUTE_PRIVATE_KEY_X_R,
    ECDSACC26X2_FSM_SIGN_COMPUTE_PRIVATE_KEY_X_R_RESULT,
    ECDSACC26X2_FSM_SIGN_ADD_HASH,
    ECDSACC26X2_FSM_SIGN_ADD_HASH_RESULT,
    ECDSACC26X2_FSM_SIGN_MULT_BY_PMSN_INVERSE,
    ECDSACC26X2_FSM_SIGN_MULT_BY_PMSN_INVERSE_RESULT,
    ECDSACC26X2_FSM_SIGN_MOD_N,
    ECDSACC26X2_FSM_SIGN_MOD_N_RESULT,

    ECDSACC26X2_FSM_VERIFY_R_S_IN_RANGE,
    ECDSACC26X2_FSM_VERIFY_VALIDATE_PUBLIC_KEY,
    ECDSACC26X2_FSM_VERIFY_COMPUTE_S_INV,
    ECDSACC26X2_FSM_VERIFY_COMPUTE_S_INV_RESULT,
    ECDSACC26X2_FSM_VERIFY_MULT_S_INV_HASH,
    ECDSACC26X2_FSM_VERIFY_MULT_S_INV_HASH_RESULT,
    ECDSACC26X2_FSM_VERIFY_S_INV_MULT_HASH_MOD_N,
    ECDSACC26X2_FSM_VERIFY_S_INV_MULT_HASH_MOD_N_RESULT,
    ECDSACC26X2_FSM_VERIFY_MULT_G,
    ECDSACC26X2_FSM_VERIFY_MULT_G_RESULT,
    ECDSACC26X2_FSM_VERIFY_MULT_S_INV_R,
    ECDSACC26X2_FSM_VERIFY_MULT_S_INV_R_RESULT,
    ECDSACC26X2_FSM_VERIFY_MULT_S_INV_R_MOD_N,
    ECDSACC26X2_FSM_VERIFY_MULT_S_INV_R_MOD_N_RESULT,
    ECDSACC26X2_FSM_VERIFY_MULT_PUB_KEY,
    ECDSACC26X2_FSM_VERIFY_MULT_PUB_KEY_RESULT,
    ECDSACC26X2_FSM_VERIFY_ADD_MULT_RESULTS,
    ECDSACC26X2_FSM_VERIFY_ADD_MULT_RESULTS_RESULT,
    ECDSACC26X2_FSM_VERIFY_POINTX_MOD_N,
    ECDSACC26X2_FSM_VERIFY_POINTX_MOD_N_RESULT,
    ECDSACC26X2_FSM_VERIFY_COMPARE_RESULT_R,

} ECDSACC26X2_FsmState;

/*!
 *  @brief      ECDSACC26X2 state machine function prototype
 *
 *  The FSM controller in the ECDSACC26X2 SWI executes a state machine function
 *  containing a switch statement that governs state execution. This function
 *  pointer is stored in the object at the beginning of the transaction.
 *  This way, unused state machines are removed at link time.
 */
typedef int_fast16_t (*ECDSACC26X2_stateMachineFxn) (ECDSA_Handle handle);

/*!
 *  @brief      ECDSACC26X2 Hardware Attributes
 *
 *  ECDSACC26X2 hardware attributes should be included in the board file
 *  and pointed to by the ECDSA_config struct.
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
    uint8_t    trngIntPriority;
} ECDSACC26X2_HWAttrs;

/*!
 *  @brief      ECDSACC26X2 Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct {
    TRNG_Config                     trngConfig;
    TRNGCC26XX_Object               trngObject;
    TRNGCC26XX_HWAttrs              trngHwAttrs;
    TRNG_Handle                     trngHandle;
    bool                            isOpen;
    bool                            operationInProgress;
    bool                            operationCanceled;
    int_fast16_t                    operationStatus;
    ECDSA_Operation                 operation;
    ECDSA_OperationType             operationType;
    ECDSA_CallbackFxn               callbackFxn;
    ECDSACC26X2_stateMachineFxn     fsmFxn;
    ECDSA_ReturnBehavior            returnBehavior;
    ECDSACC26X2_FsmState            fsmState;
    uint32_t                        semaphoreTimeout;
    uint32_t                        resultAddress;
    uint32_t                       *scratchNumber1;
    uint32_t                       *scratchNumber2;
    CryptoKey                       pmsnKey;
} ECDSACC26X2_Object;

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_ecdsa_ECDSACC26X2__include */
