/*
 * Copyright (c) 2020-2021, Texas Instruments Incorporated
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
/** ===========================================================================
 *  @file       EDDSACC26X2.h
 *
 *  @brief      EdDSA driver implementation for the CC26X2 family
 *
 *  This file should only be included in the board file to fill the
 *  EDDSA_config struct.
 *
 *  # Hardware and Implementation Details #
 *
 *  The CC26X2 family has a dedicated public key accelerator.
 *  It is capable of multiple mathematical operations including dedicated ECC
 *  point addition, doubling, and scalar multiplication. Only one operation can
 *  be carried out on the accelerator at a time. Mutual exclusion is
 *  implemented at the driver level and coordinated between all drivers relying
 *  on the accelerator. It is transparent to the application
 *  and only noted ensure sensible access timeouts are set.
 *
 *  The large number maths engine (LNME) uses a dedicated 2kB block of RAM
 *  (PKA RAM) for its operations. The operands of the maths operations must be
 *  copied into and results out of the PKA RAM. This necessitates a significant
 *  number of reads and writes for each operation. The bus interface to the RAM
 *  only allows for word-aligned reads and writes. The CPU splits the reads and
 *  writes from and to general SRAM from unaligned addresses into multiple
 *  bus operations while accumulating the data in a register until it is full.
 *  The result of this hardware process is that providing buffers such as
 *  plaintext CryptoKey keying material to ECC APIs that are word-aligned will
 *  significantly speed up the operation and
 *  reduce power consumption.
 *
 *  The driver implementation does not perform runtime checks for most input
 *  parameters. Only values that are likely to have a stochastic element to
 *  them are checked (such as whether a driver is already open). Higher input
 *  parameter validation coverage is achieved by turning on assertions when
 *  compiling the driver.
 *
 *  # Supported Curve Types #
 *
 *  The driver implementation supports the following curves for EdDSA:
 *
 *  | Curve        | Supported |
 *  |--------------|-----------|
 *  | Ed25519      | Yes       |
 *  | Ed448        | No        |
 *
 *  # Public Key Validation #
 *
 *  When performing signature verification, the foreign public key will always
 *  be validated by performing a public key decompression followed by verifying
 *  that the point is on the Edwards curve.
 */

#ifndef ti_drivers_eddsa_EDDSACC26X2__include
#define ti_drivers_eddsa_EDDSACC26X2__include

#include <ti/drivers/EDDSA.h>
#include <ti/drivers/SHA2.h>
#include <ti/drivers/sha2/SHA2CC26X2.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Exit the SWI and wait until an HWI call posts the SWI again */
#define EDDSACC26X2_STATUS_FSM_RUN_PKA_OP       EDDSA_STATUS_RESERVED - 0
/* Execute the next FSM state immediately without waiting for the next HWI */
#define EDDSACC26X2_STATUS_FSM_RUN_FSM          EDDSA_STATUS_RESERVED - 1
/* PKA operation started correctly (needed for subFSM functionality) */
#define EDDSACC26X2_PKA_OPERATION_STARTED       (0xFFFFFFFFUL)

/*!
 *  @brief      EDDSACC26X2 KeyGen, Sign, and Verify states
 *
 *  The public key generation, sign, and verify operations are implemented
 *  using multiple individual PKA operations. Since state transitions for these
 *  operations are almost always predictable, the state transitions are encoded
 *  linearly in this enum. The FSM controller will increment the state counter
 *  and iterate through states until it is told to stop or restart.
 */
typedef enum {
    EDDSACC26X2_FSM_ERROR,

    EDDSACC26X2_FSM_GEN_PUB_KEY_HASH_PRIVATE_KEY,
    EDDSACC26X2_FSM_GEN_PUB_KEY_MULT_PRIV_KEY_BY_GENER_WEIER,
    EDDSACC26X2_FSM_GEN_PUB_KEY_MULT_PRIV_KEY_BY_GENER_WEIER_RESULT,
    EDDSACC26X2_FSM_GEN_PUB_KEY_CONVERT_WEI_TO_ED,
    EDDSACC26X2_FSM_GEN_PUB_KEY_ENCODE_PUBLIC_KEY,

    EDDSACC26X2_FSM_SIGN1_HASH_PRIVATE_KEY,
    EDDSACC26X2_FSM_SIGN1_HASH_UPPER_SECRET_KEY,
    EDDSACC26X2_FSM_SIGN1_HASH_MESSAGE1,
    EDDSACC26X2_FSM_SIGN1_HASH_MESSAGE2,
    EDDSACC26X2_FSM_SIGN1_HASH_FINALIZE,

    EDDSACC26X2_FSM_SIGN2_MOD_SECRET_HASH,
    EDDSACC26X2_FSM_SIGN2_MOD_SECRET_HASH_RESULT,
    EDDSACC26X2_FSM_SIGN2_MULT_SECRET_HASH_BY_GENERATOR_WEIER,
    EDDSACC26X2_FSM_SIGN2_MULT_SECRET_HASH_BY_GENERATOR_WEIER_RESULT,
    EDDSACC26X2_FSM_SIGN2_CONVERT_WEI_TO_ED,
    EDDSACC26X2_FSM_SIGN2_ENCODE_PUBLIC_KEY,

    EDDSACC26X2_FSM_SIGN3_HASH_SIG_R,
    EDDSACC26X2_FSM_SIGN3_HASH_PUBLIC_KEY,
    EDDSACC26X2_FSM_SIGN3_HASH_MESSAGE1,
    EDDSACC26X2_FSM_SIGN3_HASH_MESSAGE2,
    EDDSACC26X2_FSM_SIGN3_HASH_FINALIZE,

    EDDSACC26X2_FSM_SIGN4_MULT_SIG_HASH_BY_SECRET_KEY,
    EDDSACC26X2_FSM_SIGN4_MULT_SIG_HASH_BY_SECRET_KEY_RESULT,
    EDDSACC26X2_FSM_SIGN4_ADD_SECRET_HASH_TO_MULT,
    EDDSACC26X2_FSM_SIGN4_ADD_SECRET_HASH_TO_MULT_RESULT,
    EDDSACC26X2_FSM_SIGN4_MOD_S,
    EDDSACC26X2_FSM_SIGN4_MOD_S_RESULT,

    EDDSACC26X2_FSM_VERIFY1_HASH_SIG_R,
    EDDSACC26X2_FSM_VERIFY1_HASH_PUBLIC_KEY,
    EDDSACC26X2_FSM_VERIFY1_HASH_MESSAGE1,
    EDDSACC26X2_FSM_VERIFY1_HASH_MESSAGE2,
    EDDSACC26X2_FSM_VERIFY1_HASH_FINALIZE,

    EDDSACC26X2_FSM_VERIFY1_S_IN_RANGE,
    EDDSACC26X2_FSM_VERIFY1_A_IS_NOT_POINT_AT_INFINITY,
    EDDSACC26X2_FSM_VERIFY1_MULT_Y_BY_Y,
    EDDSACC26X2_FSM_VERIFY1_MULT_Y_BY_Y_RESULT,
    EDDSACC26X2_FSM_VERIFY1_MOD_Y2,
    EDDSACC26X2_FSM_VERIFY1_MOD_Y2_RESULT,
    EDDSACC26X2_FSM_VERIFY1_SUBTRACT_ONE_FROM_Y2,
    EDDSACC26X2_FSM_VERIFY1_SUBTRACT_ONE_FROM_Y2_RESULT,
    EDDSACC26X2_FSM_VERIFY1_MULT_D_BY_Y2,
    EDDSACC26X2_FSM_VERIFY1_MULT_D_BY_Y2_RESULT,
    EDDSACC26X2_FSM_VERIFY1_MOD_D_Y2,
    EDDSACC26X2_FSM_VERIFY1_MOD_D_Y2_RESULT,
    EDDSACC26X2_FSM_VERIFY1_ADD_ONE_TO_D_Y2,
    EDDSACC26X2_FSM_VERIFY1_ADD_ONE_TO_D_Y2_RESULT,
    EDDSACC26X2_FSM_VERIFY1_MULT_V_BY_V,
    EDDSACC26X2_FSM_VERIFY1_MULT_V_BY_V_RESULT,
    EDDSACC26X2_FSM_VERIFY1_MOD_V2,
    EDDSACC26X2_FSM_VERIFY1_MOD_V2_RESULT,
    EDDSACC26X2_FSM_VERIFY1_MULT_V2_BY_V,
    EDDSACC26X2_FSM_VERIFY1_MULT_V2_BY_V_RESULT,
    EDDSACC26X2_FSM_VERIFY1_MOD_V3,
    EDDSACC26X2_FSM_VERIFY1_MOD_V3_RESULT,
    EDDSACC26X2_FSM_VERIFY1_MULT_V3_BY_U,
    EDDSACC26X2_FSM_VERIFY1_MULT_V3_BY_U_RESULT,
    EDDSACC26X2_FSM_VERIFY1_MOD_U_V3,
    EDDSACC26X2_FSM_VERIFY1_MOD_U_V3_RESULT,
    EDDSACC26X2_FSM_VERIFY1_MULT_U_V3_BY_V3,
    EDDSACC26X2_FSM_VERIFY1_MULT_U_V3_BY_V3_RESULT,
    EDDSACC26X2_FSM_VERIFY1_MOD_U_V6,
    EDDSACC26X2_FSM_VERIFY1_MOD_U_V6_RESULT,
    EDDSACC26X2_FSM_VERIFY1_MULT_U_V6_BY_V,
    EDDSACC26X2_FSM_VERIFY1_MULT_U_V6_BY_V_RESULT,
    EDDSACC26X2_FSM_VERIFY1_MOD_U_V7,
    EDDSACC26X2_FSM_VERIFY1_MOD_U_V7_RESULT,
    EDDSACC26X2_FSM_VERIFY1_EXP_U_V7,
    EDDSACC26X2_FSM_VERIFY1_EXP_U_V7_RESULT,
    EDDSACC26X2_FSM_VERIFY1_MULT_U_V3_BY_EXP,
    EDDSACC26X2_FSM_VERIFY1_MULT_U_V3_BY_EXP_RESULT,
    EDDSACC26X2_FSM_VERIFY1_MOD_CANDIDATE_X,
    EDDSACC26X2_FSM_VERIFY1_MOD_CANDIDATE_X_RESULT,
    EDDSACC26X2_FSM_VERIFY1_MULT_X_BY_X,
    EDDSACC26X2_FSM_VERIFY1_MULT_X_BY_X_RESULT,
    EDDSACC26X2_FSM_VERIFY1_MOD_X2,
    EDDSACC26X2_FSM_VERIFY1_MOD_X2_RESULT,
    EDDSACC26X2_FSM_VERIFY1_MULT_X2_BY_V,
    EDDSACC26X2_FSM_VERIFY1_MULT_X2_BY_V_RESULT,
    EDDSACC26X2_FSM_VERIFY1_MOD_V_X2,
    EDDSACC26X2_FSM_VERIFY1_MOD_V_X2_RESULT,
    EDDSACC26X2_FSM_VERIFY1_MOD_U,
    EDDSACC26X2_FSM_VERIFY1_MOD_U_RESULT,
    EDDSACC26X2_FSM_VERIFY1_NEGATE_U,
    EDDSACC26X2_FSM_VERIFY1_NEGATE_U_RESULT,
    EDDSACC26X2_FSM_VERIFY1_CHECK_SQUARE_ROOT,

    EDDSACC26X2_FSM_VERIFY2_ADD_ONE_TO_P,
    EDDSACC26X2_FSM_VERIFY2_ADD_ONE_TO_P_RESULT,
    EDDSACC26X2_FSM_VERIFY2_SUBTRACT_Y_FROM_P_PLUS_ONE,
    EDDSACC26X2_FSM_VERIFY2_SUBTRACT_Y_FROM_P_PLUS_ONE_RESULT,
    EDDSACC26X2_FSM_VERIFY2_MULT_ONE_MINUS_Y_BY_X,
    EDDSACC26X2_FSM_VERIFY2_MULT_ONE_MINUS_Y_BY_X_RESULT,
    EDDSACC26X2_FSM_VERIFY2_MOD_ONE_MINUS_Y_X,
    EDDSACC26X2_FSM_VERIFY2_MOD_ONE_MINUS_Y_X_RESULT,
    EDDSACC26X2_FSM_VERIFY2_INVERSION,
    EDDSACC26X2_FSM_VERIFY2_INVERSION_RESULT,
    EDDSACC26X2_FSM_VERIFY2_MULT_INVERSE_BY_X,
    EDDSACC26X2_FSM_VERIFY2_MULT_INVERSE_BY_X_RESULT,
    EDDSACC26X2_FSM_VERIFY2_MOD_ONE_MINUS_Y_INV,
    EDDSACC26X2_FSM_VERIFY2_MOD_ONE_MINUS_Y_INV_RESULT,
    EDDSACC26X2_FSM_VERIFY2_MULT_INVERSE_BY_ONE_MINUS_Y,
    EDDSACC26X2_FSM_VERIFY2_MULT_INVERSE_BY_ONE_MINUS_Y_RESULT,
    EDDSACC26X2_FSM_VERIFY2_MOD_X_INV,
    EDDSACC26X2_FSM_VERIFY2_MOD_X_INV_RESULT,
    EDDSACC26X2_FSM_VERIFY2_ADD_ONE_TO_Y,
    EDDSACC26X2_FSM_VERIFY2_ADD_ONE_TO_Y_RESULT,
    EDDSACC26X2_FSM_VERIFY2_MULT_ONE_MINUS_Y_INV_BY_ONE_PLUS_Y,
    EDDSACC26X2_FSM_VERIFY2_MULT_ONE_MINUS_Y_INV_BY_ONE_PLUS_Y_RESULT,
    EDDSACC26X2_FSM_VERIFY2_MOD_UM,
    EDDSACC26X2_FSM_VERIFY2_MOD_UM_RESULT,
    EDDSACC26X2_FSM_VERIFY2_MULT_UM_BY_X_INV,
    EDDSACC26X2_FSM_VERIFY2_MULT_UM_BY_X_INV_RESULT,
    EDDSACC26X2_FSM_VERIFY2_MOD_UM_X_INV,
    EDDSACC26X2_FSM_VERIFY2_MOD_UM_X_INV_RESULT,
    EDDSACC26X2_FSM_VERIFY2_ADD_CONST_TO_UM,
    EDDSACC26X2_FSM_VERIFY2_ADD_CONST_TO_UM_RESULT,
    EDDSACC26X2_FSM_VERIFY2_MOD_WEIERX,
    EDDSACC26X2_FSM_VERIFY2_MOD_WEIERX_RESULT,
    EDDSACC26X2_FSM_VERIFY2_MULT_UM_X_INV_BY_CONST,
    EDDSACC26X2_FSM_VERIFY2_MULT_UM_X_INV_BY_CONST_RESULT,
    EDDSACC26X2_FSM_VERIFY2_MOD_WEIERY,
    EDDSACC26X2_FSM_VERIFY2_MOD_WEIERY_RESULT,
    EDDSACC26X2_FSM_VERIFY2_MOD_SIG_DIGEST,
    EDDSACC26X2_FSM_VERIFY2_MOD_SIG_DIGEST_RESULT,
    EDDSACC26X2_FSM_VERIFY2_NEGATE_SIG_DIGEST,
    EDDSACC26X2_FSM_VERIFY2_NEGATE_SIG_DIGEST_RESULT,
    EDDSACC26X2_FSM_VERIFY2_MULT_PUBLIC_KEY_BY_NEG_SIG_DIGEST,
    EDDSACC26X2_FSM_VERIFY2_MULT_PUBLIC_KEY_BY_NEG_SIG_DIGEST_RESULT,
    EDDSACC26X2_FSM_VERIFY2_MULT_BASE_POINT_BY_S,
    EDDSACC26X2_FSM_VERIFY2_MULT_BASE_POINT_BY_S_RESULT,
    EDDSACC26X2_FSM_VERIFY2_ADD_POINTS,
    EDDSACC26X2_FSM_VERIFY2_ADD_POINTS_RESULT,
    EDDSACC26X2_FSM_VERIFY2_CONVERT_WEI_TO_ED,
    EDDSACC26X2_FSM_VERIFY2_COMPARE_RESULT_R,

} EDDSACC26X2_FsmState;

/*!
 *  @brief      EDDSACC26X2 Weierstrass to Edwards states
 *
 *  The public key generation, sign, and verify operations all use a common
 *  subroutine that converts an input point on the short Weierstrass curve
 *  Wei25519 to an output point on the twisted Edwards curve Ed25519.
 *  Since state transitions for these operations are almost always
 *  predictable, the state transitions are encoded linearly in this enum.
 *  The FSM controller will increment the substate counter and iterate through
 *  states until it is told to stop or restart.
 */
typedef enum {
    EDDSACC26X2_SUBFSM_WEI_TO_MONT_ADDITION,
    EDDSACC26X2_SUBFSM_WEI_TO_MONT_ADDITION_RESULT,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_ADD_ONE,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_ADD_ONE_RESULT,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_MULT_V,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_MULT_V_RESULT,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_MOD_MULT_V,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_MOD_MULT_V_RESULT,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_INVERSION,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_INVERSION_RESULT,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_MULT_INVERSE1,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_MULT_INVERSE1_RESULT,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_MULT_INVERSE2,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_MULT_INVERSE2_RESULT,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_SUB_ONE,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_SUB_ONE_RESULT,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_MOD_SUB,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_MOD_SUB_RESULT,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_MULT_RETRIEVE_Y,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_MULT_RETRIEVE_Y_RESULT,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_MOD_Y,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_MOD_Y_RESULT,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_U_MULT_ISO_CONST,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_U_MULT_ISO_CONST_RESULT,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_MULT_V_INVERSE,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_MULT_V_INVERSE_RESULT,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_MOD_X,
    EDDSACC26X2_SUBFSM_MONT_TO_ED_MOD_X_RESULT,
} EDDSACC26X2_FsmSubState;

/*!
 *  @brief      EDDSACC26X2 state machine function prototype
 *
 *  The FSM controller in the EDDSACC26X2 SWI executes a state machine function
 *  containing a switch statement that governs state execution. This function
 *  pointer is stored in the object at the beginning of the transaction.
 *  This way, unused state machines are removed at link time.
 */
typedef int_fast16_t (*EDDSACC26X2_stateMachineFxn) (EDDSA_Handle handle);

/*!
 *  @brief      EDDSACC26X2 Hardware Attributes
 *
 *  EDDSACC26X2 hardware attributes should be included in the board file
 *  and pointed to by the EDDSA_config struct.
 */
typedef struct {
    /*!
     *  @brief PKA Peripheral's interrupt priority.
     *
     *  The CC26xx uses three of the priority bits, meaning ~0 has the same
     *  effect as (7 << 5).
     *
     *  (7 << 5) will apply the lowest priority.
     *
     *  (1 << 5) will apply the highest priority.
     *
     *  Setting the priority to 0 is not supported by this driver.
     *
     *  HWI's with priority 0 ignore the HWI dispatcher to support zero-latency
     *  interrupts, thus invalidating the critical sections in this driver.
     */
    uint8_t    intPriority;
    uint8_t    sha2IntPriority;
} EDDSACC26X2_HWAttrs;

#define ED25519_LENGTH 32

/*!
 *  @brief      EDDSACC26X2 Workspace
 *
 *  Holds intermediate memory to perform Ed25519 operations. Unlike PKA SRAM,
 *  these values can be easily accessed or modified.
 */
typedef struct {
    uint8_t digestResult[SHA2_DIGEST_LENGTH_BYTES_512];
    uint8_t digestResult2[SHA2_DIGEST_LENGTH_BYTES_512];
    uint8_t publicKey[ED25519_LENGTH];
    uint8_t secretScalar[ED25519_LENGTH];
    uint8_t xResult[ED25519_LENGTH];
    uint8_t yResult[ED25519_LENGTH];
    uint8_t x_0;
} EDDSACC26X2_Workspace;

/*!
 *  @brief      EDDSACC26X2 Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct {
    /*
     * Note that sha2Config must be the first member so that we can use the
     * SHA2 callback in the EdDSA driver.
     */
    SHA2_Config                     sha2Config;
    SHA2CC26X2_Object               sha2Object;
    SHA2CC26X2_HWAttrs              sha2HwAttrs;
    SHA2_Handle                     sha2Handle;
    bool                            isOpen;
    bool                            operationInProgress;
    bool                            operationCanceled;
    int_fast16_t                    operationStatus;
    EDDSA_Operation                 operation;
    EDDSA_OperationType             operationType;
    EDDSA_CallbackFxn               callbackFxn;
    EDDSACC26X2_stateMachineFxn     fsmFxn;
    EDDSA_ReturnBehavior            returnBehavior;
    EDDSACC26X2_FsmState            fsmState;
    EDDSACC26X2_FsmSubState         fsmSubState;
    uint32_t                        semaphoreTimeout;
    uint32_t                        resultAddress;
    uint32_t                       *scratchNumber1;
    uint32_t                       *scratchNumber2;
    EDDSACC26X2_Workspace          EDDSACC26X2_GlobalWorkspace;
} EDDSACC26X2_Object;

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_eddsa_EDDSACC26X2__include */
