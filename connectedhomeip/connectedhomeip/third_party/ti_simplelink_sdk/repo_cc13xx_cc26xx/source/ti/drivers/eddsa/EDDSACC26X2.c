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

/*
 *  ======== EDDSACC26X2.c ========
 *
 *  This file contains the CC26X2 implementation of EDDSA
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26X2.h>
#include <ti/drivers/EDDSA.h>
#include <ti/drivers/eddsa/EDDSACC26X2.h>
#include <ti/drivers/cryptoutils/ecc/ECCParams.h>
#include <ti/drivers/cryptoutils/sharedresources/PKAResourceCC26XX.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKeyPlaintext.h>
#include <ti/drivers/cryptoutils/utils/CryptoUtils.h>
#include <ti/drivers/cryptoutils/cryptokey/CryptoKey.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(inc/hw_pka.h)
#include DeviceFamily_constructPath(inc/hw_pka_ram.h)
#include DeviceFamily_constructPath(driverlib/pka.h)
#include DeviceFamily_constructPath(driverlib/cpu.h)
#include DeviceFamily_constructPath(driverlib/interrupt.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

/* Octet string format requires an extra byte at the start of the public key */
#define OCTET_STRING_OFFSET 1

#define SCRATCH_KEY_OFFSET 512
#define SCRATCH_KEY_SIZE 96
#define SCRATCH_PRIVATE_KEY ((uint32_t *)(PKA_RAM_BASE                         \
                                         + SCRATCH_KEY_OFFSET))
#define SCRATCH_PUBLIC_X ((uint8_t *)(PKA_RAM_BASE                            \
                                      + SCRATCH_KEY_OFFSET                    \
                                      + 1 * SCRATCH_KEY_SIZE))
#define SCRATCH_PUBLIC_Y ((uint8_t *)(PKA_RAM_BASE                            \
                                      + SCRATCH_KEY_OFFSET                    \
                                      + 2 * SCRATCH_KEY_SIZE))

#define SCRATCH_BUFFER_OFFSET 1024
#define SCRATCH_BUFFER_SIZE 256
#define SCRATCH_BUFFER_0 ((uint8_t *)(PKA_RAM_BASE                            \
                                      + SCRATCH_BUFFER_OFFSET                 \
                                      + 0 * SCRATCH_BUFFER_SIZE))
#define SCRATCH_BUFFER_1 ((uint8_t *)(PKA_RAM_BASE                            \
                                      + SCRATCH_BUFFER_OFFSET                 \
                                      + 1 * SCRATCH_BUFFER_SIZE))
#define SCRATCH_BUFFER_2 ((uint8_t *)(PKA_RAM_BASE                            \
                                      + SCRATCH_BUFFER_OFFSET                 \
                                      + 2 * SCRATCH_BUFFER_SIZE))
#define SCRATCH_BUFFER_3 ((uint8_t *)(PKA_RAM_BASE                            \
                                      + SCRATCH_BUFFER_OFFSET                 \
                                      + 3 * SCRATCH_BUFFER_SIZE))

/* Forward declarations */
static void EDDSACC26X2_hwiFxn(uintptr_t arg0);

static void EDDSACC26X2_internalCallbackFxn(EDDSA_Handle handle,
                                            int_fast16_t returnStatus,
                                            EDDSA_Operation operation,
                                            EDDSA_OperationType
                                            operationType);
static int_fast16_t EDDSACC26X2_waitForAccess(EDDSA_Handle handle);
static int_fast16_t EDDSACC26X2_waitForResult(EDDSA_Handle handle);
static int_fast16_t EDDSACC26X2_encodePublicKey(EDDSA_Handle handle,
                                                uint8_t *outputKey,
                                                uint8_t *xCoordinate,
                                                uint8_t *yCoordinate);
static uint32_t EDDSACC26X2_runWeiToEdFSM(EDDSA_Handle handle);
static int_fast16_t EDDSACC26X2_runGeneratePublicKeyFSM(EDDSA_Handle handle);
static int_fast16_t EDDSACC26X2_runSignFSM(EDDSA_Handle handle);
static int_fast16_t EDDSACC26X2_runVerifyFSM(EDDSA_Handle handle);
static int_fast16_t EDDSACC26X2_convertReturnValue(uint32_t pkaResult);

/* Extern globals */
extern const EDDSA_Params EDDSA_defaultParams;

/* Static globals */
static bool EDDSACC26X2_isInitialized = false;
static uint32_t EDDSACC26X2_resultAddress;

static uint32_t EDDSACC26X2_scratchPrivKeySize = SCRATCH_KEY_SIZE;
static uint32_t EDDSACC26X2_scratchPublicXSize = SCRATCH_KEY_SIZE;
static uint32_t EDDSACC26X2_scratchBuffer0Size = SCRATCH_BUFFER_SIZE;
static uint32_t EDDSACC26X2_scratchBuffer1Size = SCRATCH_BUFFER_SIZE;
static uint32_t EDDSACC26X2_scratchBuffer2Size = SCRATCH_BUFFER_SIZE;
static uint32_t EDDSACC26X2_scratchBuffer3Size = SCRATCH_BUFFER_SIZE;

/* Temporary constant for 1 */
const uint8_t EDDSACC26X2_one[32] =
    {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00 ,0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/* Temporary constant for -1 */
const uint8_t EDDSACC26X2_negOne[32] =
    {0xec, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f};

/* Sqrt(-486664) */
const uint8_t EDDSACC26X2_isoConst1[32] =
    {0xe7, 0x81, 0xba, 0x00, 0x55, 0xfb, 0x91, 0x33,
     0x7d, 0xe5, 0x82, 0xb4, 0x2e, 0x2c, 0x5e, 0x3a,
     0x81, 0xb0, 0x03, 0xfc, 0x23, 0xf7, 0x84, 0x2d,
     0x44, 0xf9, 0x5f, 0x9f, 0x0b, 0x12, 0xd9, 0x70};

/* X25519.A / 3 */
const uint8_t EDDSACC26X2_isoConst2[32] =
    {0x51, 0x24, 0xad, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
     0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
     0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa,
     0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0x2a};

/* p - X25519.A / 3 */
const uint8_t EDDSACC26X2_isoConst3[32] =
    {0x9c, 0xdb, 0x52, 0x55, 0x55, 0x55, 0x55, 0x55,
     0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
     0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
     0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55};

/* (p - 5) / 8 */
const uint8_t EDDSACC26X2_decodeConst1[32] =
    {0xfd, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x0f};

/* 2 ^ ((p - 1) / 4) */
const uint8_t EDDSACC26X2_decodeConst2[32] =
    {0xb0, 0xa0, 0x0e, 0x4a, 0x27, 0x1b, 0xee, 0xc4,
     0x78, 0xe4, 0x2f, 0xad, 0x06, 0x18, 0x43, 0x2f,
     0xa7, 0xd7, 0xfb, 0x3d, 0x99, 0x00, 0x4d, 0x2b,
     0x0b, 0xdf, 0xc1, 0x4f, 0x80, 0x24, 0x83, 0x2b};

/*
 *  ======== EDDSACC26X2_internalCallbackFxn ========
 */
static void EDDSACC26X2_internalCallbackFxn(EDDSA_Handle handle,
                                            int_fast16_t returnStatus,
                                            EDDSA_Operation operation,
                                            EDDSA_OperationType operationType)
{
    EDDSACC26X2_Object *object = handle->object;

    /*
     * This function is only ever registered when in
     * EDDSA_RETURN_BEHAVIOR_BLOCKING or EDDSA_RETURN_BEHAVIOR_POLLING.
     */
    if(object->returnBehavior == EDDSA_RETURN_BEHAVIOR_BLOCKING) {
        SemaphoreP_post(&PKAResourceCC26XX_operationSemaphore);
    }
    else {
        PKAResourceCC26XX_pollingFlag = 1;
    }
}

/*
 *  ======== EDDSACC26X2_hwiFxn ========
 */
static void EDDSACC26X2_hwiFxn(uintptr_t arg0)
{
    EDDSACC26X2_Object *object = ((EDDSA_Handle)arg0)->object;
    uint32_t key;

    /* Disable interrupt again */
    IntDisable(INT_PKA_IRQ);

    /* Execute next states */
    do
    {
        object->operationStatus = object->fsmFxn((EDDSA_Handle)arg0);
        /* Do not increment state FSM if in a substate */
        if((object->fsmState == EDDSACC26X2_FSM_GEN_PUB_KEY_CONVERT_WEI_TO_ED)
            || (object->fsmState == EDDSACC26X2_FSM_SIGN2_CONVERT_WEI_TO_ED)
            || (object->fsmState == EDDSACC26X2_FSM_VERIFY2_CONVERT_WEI_TO_ED))
        {
            if (object->fsmSubState == EDDSACC26X2_SUBFSM_MONT_TO_ED_MOD_X_RESULT)
            {
                /*
                 * Increment the state counter if the last substate operation
                 * was performed
                 */
                object->fsmState++;
            }
            else /* Otherwise continue with the substate FSM */
            {
                object->fsmSubState++;
            }
        }
        else /* All other states are post-increment */
        {
            object->fsmState++;
        }
    } while(object->operationStatus == EDDSACC26X2_STATUS_FSM_RUN_FSM);

    /*
     * We need a critical section here in case the operation is canceled
     * asynchronously.
     */
    key = HwiP_disable();

    if(object->operationCanceled)
    {
        /*
         * Set function register to 0. This should stop the current
         * operation
         */
        HWREG(PKA_BASE + PKA_O_FUNCTION) = 0;

        object->operationStatus = EDDSA_STATUS_CANCELED;
    }

    switch(object->operationStatus)
    {
        case EDDSACC26X2_STATUS_FSM_RUN_PKA_OP:

            HwiP_restore(key);

            /*
             * Do nothing. The PKA hardware
             * will execute in the background and post
             * this SWI when it is done.
             */
            break;
        case EDDSA_STATUS_SUCCESS:
            /* Intentional fall through */
        case EDDSA_STATUS_ERROR:
            /* Intentional fall through */
        case EDDSA_STATUS_CANCELED:
            /* Intentional fall through */
        default:

            /* Mark this operation as complete */
            object->operationInProgress = false;

            /*
             * Clear any pending interrupt in case a transaction kicked off
             * above already finished
             */
            IntDisable(INT_PKA_IRQ);
            IntPendClear(INT_PKA_IRQ);

            /*
             * We can end the critical section since the operation may no
             * longer be canceled
             */
            HwiP_restore(key);

            /* Make sure there is no keying material remaining in PKA RAM */
            PKAClearPkaRam();

            /* Make sure there are no secrets left in the workspace memory */
            memset(object->EDDSACC26X2_GlobalWorkspace.digestResult,
                   0x0,
                   SHA2_DIGEST_LENGTH_BYTES_512);
            memset(object->EDDSACC26X2_GlobalWorkspace.digestResult2,
                   0x0,
                   SHA2_DIGEST_LENGTH_BYTES_512);
            memset(object->EDDSACC26X2_GlobalWorkspace.publicKey,
                   0x0,
                   ED25519_LENGTH);
            memset(object->EDDSACC26X2_GlobalWorkspace.secretScalar,
                   0x0,
                   ED25519_LENGTH);
            memset(object->EDDSACC26X2_GlobalWorkspace.xResult,
                   0x0,
                   ED25519_LENGTH);
            memset(object->EDDSACC26X2_GlobalWorkspace.yResult,
                   0x0,
                   ED25519_LENGTH);
            object->EDDSACC26X2_GlobalWorkspace.x_0 = 0;

            /*
             *   Grant access for other threads to use the crypto module.
             *  The semaphore must be posted before the callbackFxn to allow
             *  the chaining of operations.
             */
            SemaphoreP_post(&PKAResourceCC26XX_accessSemaphore);

            Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);

            object->callbackFxn((EDDSA_Handle)arg0,
                                object->operationStatus,
                                object->operation,
                                object->operationType);
    }
}

/*
 *  ======== EDDSACC26X2_sha2Callback ========
 */
void EDDSACC26X2_sha2Callback(SHA2_Handle handle,
                              int_fast16_t returnValue)
{

    EDDSACC26X2_Object *object = ((EDDSACC26X2_Object *)(handle));

    /* Handle the SHA2 return value */
    if(returnValue != SHA2_STATUS_SUCCESS)
    {
        /* If SHA2 failed, then return a SHA2 error for EdDSA. */
        object->operationStatus = EDDSA_STATUS_SHA2_HASH_FAILURE;

        SHA2_close(object->sha2Handle);

        /* Post hwi as if operation finished for cleanup */
        IntEnable(INT_PKA_IRQ);
    }
    else
    {
        /*
         * Run the PKA FSM by triggering the PKA interrupt. It is level
         * triggered and the complement of the RUN bit.
         */
        IntEnable(INT_PKA_IRQ);
    }
}

static int_fast16_t EDDSACC26X2_encodePublicKey(EDDSA_Handle handle,
                                                uint8_t *outputKey,
                                                uint8_t *xCoordinate,
                                                uint8_t *yCoordinate)
{
    /*
     * Copy the y-coordinate to the buffer. The input and output
     * both are little endian.
     */
    memcpy(outputKey, yCoordinate, ECCParams_Ed25519.length);

    /*
     * If the least significant bit of the x-coordinate is 0x1, then we set
     * the most significant bit of the encoded public key to indicate so.
     * This is to compress the public key to {[0x80,0x00] | y }
     */
    outputKey[31] |= (xCoordinate[0] & 0x1) ? 0x80 : 0x00;

    return(EDDSA_STATUS_SUCCESS);
}

static uint32_t EDDSACC26X2_runWeiToEdFSM(EDDSA_Handle handle)
{
    EDDSACC26X2_Object *object = handle->object;

    switch(object->fsmSubState)
    {
        case EDDSACC26X2_SUBFSM_WEI_TO_MONT_ADDITION:
            /*
             * Perform conversion from Ed25519 to Wei25519
             * (u,v) on Wei25519 to (x,y) on Ed25519
             * x = sqrt(-486664) * (u - X25519.A/3) * (v)^-1
             * y = (u - X25519.A/3 - 1) * (u - X25519.A/3 + 1)^-1
             * To only compute one inversion, we compute
             * (v * (u - X25519.A/3 + 1))^-1
             */
            /*
             * Compute u - X25519.A/3 by performing u + (p - X25519.A/3) to
             * stay positive
             */
            PKABigNumAddStart(SCRATCH_PUBLIC_X,
                              ECCParams_Ed25519.length,
                              EDDSACC26X2_isoConst3,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_SUBFSM_WEI_TO_MONT_ADDITION_RESULT:
            EDDSACC26X2_scratchPublicXSize = SCRATCH_KEY_SIZE;
            return(PKABigNumAddGetResult(SCRATCH_PUBLIC_X,
                                         &EDDSACC26X2_scratchPublicXSize,
                                         EDDSACC26X2_resultAddress));

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_ADD_ONE:
            /* Compute u - X25519.A/3 + 1 */
            PKABigNumAddStart(SCRATCH_PUBLIC_X,
                              ECCParams_Ed25519.length,
                              EDDSACC26X2_one,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_ADD_ONE_RESULT:
            EDDSACC26X2_scratchBuffer0Size = SCRATCH_BUFFER_SIZE;
            return(PKABigNumAddGetResult(SCRATCH_BUFFER_0,
                                         &EDDSACC26X2_scratchBuffer0Size,
                                         EDDSACC26X2_resultAddress));

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_MULT_V:
            /* Compute (v * (u - X25519.A/3 + 1)) */
            PKABigNumMultiplyStart(SCRATCH_PUBLIC_Y,
                                   ECCParams_Ed25519.length,
                                   SCRATCH_BUFFER_0,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_MULT_V_RESULT:
            EDDSACC26X2_scratchBuffer1Size = SCRATCH_BUFFER_SIZE;
            return(PKABigNumMultGetResult(SCRATCH_BUFFER_1,
                                          &EDDSACC26X2_scratchBuffer1Size,
                                          EDDSACC26X2_resultAddress));

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_MOD_MULT_V:
            /* Compute (v * (u - X25519.A/3 + 1)) mod p */
            PKABigNumModStart(SCRATCH_BUFFER_1,
                              EDDSACC26X2_scratchBuffer1Size,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_MOD_MULT_V_RESULT:
            EDDSACC26X2_scratchBuffer1Size = SCRATCH_BUFFER_SIZE;
            return(PKABigNumModGetResult(SCRATCH_BUFFER_1,
                                         EDDSACC26X2_scratchBuffer1Size,
                                         EDDSACC26X2_resultAddress));

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_INVERSION:
            /* Compute (v * (u - X25519.A/3 + 1))^-1 */
            PKABigNumInvModStart(SCRATCH_BUFFER_1,
                                 ECCParams_Ed25519.length,
                                 ECCParams_Ed25519.prime,
                                 ECCParams_Ed25519.length,
                                 &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_INVERSION_RESULT:
            EDDSACC26X2_scratchBuffer1Size = SCRATCH_BUFFER_SIZE;
            return(PKABigNumInvModGetResult(SCRATCH_BUFFER_1,
                                            EDDSACC26X2_scratchBuffer1Size,
                                            EDDSACC26X2_resultAddress));

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_MULT_INVERSE1:
            /* Compute (v)^-1 */
            PKABigNumMultiplyStart(SCRATCH_BUFFER_1,
                                   ECCParams_Ed25519.length,
                                   SCRATCH_BUFFER_0,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_MULT_INVERSE1_RESULT:
            EDDSACC26X2_scratchBuffer2Size = SCRATCH_BUFFER_SIZE;
            return(PKABigNumMultGetResult(SCRATCH_BUFFER_2,
                                          &EDDSACC26X2_scratchBuffer2Size,
                                          EDDSACC26X2_resultAddress));

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_MULT_INVERSE2:
            /* Compute (u - X25519.A/3 + 1)^-1 */
            PKABigNumMultiplyStart(SCRATCH_BUFFER_1,
                                   ECCParams_Ed25519.length,
                                   SCRATCH_PUBLIC_Y,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_MULT_INVERSE2_RESULT:
            EDDSACC26X2_scratchBuffer1Size = SCRATCH_BUFFER_SIZE;
            return(PKABigNumMultGetResult(SCRATCH_BUFFER_1,
                                          &EDDSACC26X2_scratchBuffer1Size,
                                          EDDSACC26X2_resultAddress));

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_SUB_ONE:
            /*
             * Compute u - X25519.A/3 - 1 by performing
             * u - X25519.A/3 + (p - 1)
             */
            PKABigNumAddStart(SCRATCH_PUBLIC_X,
                              ECCParams_Ed25519.length,
                              EDDSACC26X2_negOne,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_SUB_ONE_RESULT:
            EDDSACC26X2_scratchBuffer0Size = SCRATCH_BUFFER_SIZE;
            return(PKABigNumAddGetResult(SCRATCH_BUFFER_0,
                                         &EDDSACC26X2_scratchBuffer0Size,
                                         EDDSACC26X2_resultAddress));

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_MOD_SUB:
            /* Compute x mod p */
            PKABigNumModStart(SCRATCH_BUFFER_0,
                              EDDSACC26X2_scratchBuffer0Size,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_MOD_SUB_RESULT:
            EDDSACC26X2_scratchPublicXSize = SCRATCH_KEY_SIZE;
            return(PKABigNumModGetResult(SCRATCH_BUFFER_0,
                                         ECCParams_Ed25519.length,
                                         EDDSACC26X2_resultAddress));

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_MULT_RETRIEVE_Y:
            /* Compute y = (u - X25519.A/3 - 1) * (u - X25519.A/3 + 1)^-1 */
            PKABigNumMultiplyStart(SCRATCH_BUFFER_0,
                                   ECCParams_Ed25519.length,
                                   SCRATCH_BUFFER_1,
                                   2 * ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_MULT_RETRIEVE_Y_RESULT:
            EDDSACC26X2_scratchBuffer0Size = SCRATCH_BUFFER_SIZE;
            return(PKABigNumMultGetResult(SCRATCH_BUFFER_0,
                                          &EDDSACC26X2_scratchBuffer0Size,
                                          EDDSACC26X2_resultAddress));

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_MOD_Y:
            /* Compute y mod p */
            PKABigNumModStart(SCRATCH_BUFFER_0,
                              EDDSACC26X2_scratchBuffer0Size,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_MOD_Y_RESULT:
            return(PKABigNumModGetResult(object->EDDSACC26X2_GlobalWorkspace.
                                             yResult,
                                         ECCParams_Ed25519.length,
                                         EDDSACC26X2_resultAddress));

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_U_MULT_ISO_CONST:
            /* Compute sqrt(-486664) * (u - X25519.A/3) */
            PKABigNumMultiplyStart(EDDSACC26X2_isoConst1,
                                   ECCParams_Ed25519.length,
                                   SCRATCH_PUBLIC_X,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_U_MULT_ISO_CONST_RESULT:
            EDDSACC26X2_scratchBuffer0Size = SCRATCH_BUFFER_SIZE;
            return(PKABigNumMultGetResult(SCRATCH_BUFFER_0,
                                          &EDDSACC26X2_scratchBuffer0Size,
                                          EDDSACC26X2_resultAddress));

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_MULT_V_INVERSE:
            /* Compute x = sqrt(-486664) * (u - X25519.A/3) * (v)^-1 */
            PKABigNumMultiplyStart(SCRATCH_BUFFER_0,
                                   2 * ECCParams_Ed25519.length,
                                   SCRATCH_BUFFER_2,
                                   2 * ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_MULT_V_INVERSE_RESULT:
            EDDSACC26X2_scratchBuffer0Size = SCRATCH_BUFFER_SIZE;
            return(PKABigNumMultGetResult(SCRATCH_BUFFER_0,
                                          &EDDSACC26X2_scratchBuffer0Size,
                                          EDDSACC26X2_resultAddress));

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_MOD_X:
            /* Compute x mod p */
            PKABigNumModStart(SCRATCH_BUFFER_0,
                              EDDSACC26X2_scratchBuffer0Size,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_SUBFSM_MONT_TO_ED_MOD_X_RESULT:
            EDDSACC26X2_scratchPublicXSize = SCRATCH_KEY_SIZE;
            return(PKABigNumModGetResult(
                    object->EDDSACC26X2_GlobalWorkspace.xResult,
                    ECCParams_Ed25519.length,
                    EDDSACC26X2_resultAddress));

        default:
            return(PKA_STATUS_FAILURE);
    }
    /* If we break from the FSM, that means we just started a PKA operation,
     * so we want to indicate this to the upstream FSM.
     */
    return(EDDSACC26X2_PKA_OPERATION_STARTED);
}

/*
 *  ======== EDDSACC26X2_runGeneratePublicKeyFSM ========
 */
static int_fast16_t EDDSACC26X2_runGeneratePublicKeyFSM(EDDSA_Handle handle)
{
    EDDSACC26X2_Object *object = handle->object;
    uint32_t pkaResult;

    switch(object->fsmState)
    {
        case EDDSACC26X2_FSM_GEN_PUB_KEY_HASH_PRIVATE_KEY:
            /* Hash and prune the private key to generate the public key */
            SHA2_hashData(object->sha2Handle,
                          object->operation.generatePublicKey->myPrivateKey->
                              u.plaintext.keyMaterial,
                          ECCParams_Ed25519.length,
                          object->EDDSACC26X2_GlobalWorkspace.digestResult);

            return(EDDSACC26X2_STATUS_FSM_RUN_PKA_OP);

        case EDDSACC26X2_FSM_GEN_PUB_KEY_MULT_PRIV_KEY_BY_GENER_WEIER:

            /* Close the SHA2 driver as we no longer need it */
            SHA2_close(object->sha2Handle);

            /* Prune the least significant half of the hash digest */
            /* Clear bits 0,1,2 */
            object->EDDSACC26X2_GlobalWorkspace.digestResult[0]   &= 0xF8;
            /* Clear Bit 255 */
            object->EDDSACC26X2_GlobalWorkspace.digestResult[31]  &= 0x7F;
            /* Set bit 254 */
            object->EDDSACC26X2_GlobalWorkspace.digestResult[31]  |= 0x40;

            /*
             * Copy the hashed and pruned private key to generate the public
             * key Edwards point
             */
            CryptoUtils_copyPad(
                    object->EDDSACC26X2_GlobalWorkspace.digestResult,
                    SCRATCH_PRIVATE_KEY,
                    ECCParams_Ed25519.length);

            /*
             * Perform the point multiplication A = s*B to generate the public
             * key point in short Weierstrass form
             */
            PKAEccMultiplyStart((uint8_t*)SCRATCH_PRIVATE_KEY,
                                ECCParams_Wei25519.generatorX,
                                ECCParams_Wei25519.generatorY,
                                ECCParams_Wei25519.prime,
                                ECCParams_Wei25519.a,
                                ECCParams_Wei25519.b,
                                ECCParams_Wei25519.length,
                                &EDDSACC26X2_resultAddress);
            break;

        case EDDSACC26X2_FSM_GEN_PUB_KEY_MULT_PRIV_KEY_BY_GENER_WEIER_RESULT:

            pkaResult = PKAEccMultiplyGetResult(SCRATCH_PUBLIC_X,
                                                SCRATCH_PUBLIC_Y,
                                                EDDSACC26X2_resultAddress,
                                                ECCParams_Ed25519.length);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_GEN_PUB_KEY_CONVERT_WEI_TO_ED:
            /*
             * Run through the substate FSM to convert the short Weierstrass
             * point to an Edwards point.
             */
            pkaResult = EDDSACC26X2_runWeiToEdFSM(handle);

            /* We break if we just started a PKA operation */
            if(pkaResult == EDDSACC26X2_PKA_OPERATION_STARTED)
            {
                break;
            }
            else /* Otherwise, convert the return value */
            {
                return(EDDSACC26X2_convertReturnValue(pkaResult));
            }

        case EDDSACC26X2_FSM_GEN_PUB_KEY_ENCODE_PUBLIC_KEY:
            /* Encode the public key to the output */
            EDDSACC26X2_encodePublicKey(handle,
                                        object->operation.generatePublicKey->
                                            myPublicKey->u.plaintext.
                                            keyMaterial,
                                        object->EDDSACC26X2_GlobalWorkspace
                                            .xResult,
                                        object->EDDSACC26X2_GlobalWorkspace
                                            .yResult);

            /* Mark the public key CryptoKey as non-empty */
            object->operation.generatePublicKey->myPublicKey->encoding =
                    CryptoKey_PLAINTEXT;

            return(EDDSA_STATUS_SUCCESS);

        default:
            return(EDDSA_STATUS_ERROR);
    }

    /* If we get to this point, we want to perform another PKA operation */
    IntPendClear(INT_PKA_IRQ);
    IntEnable(INT_PKA_IRQ);

    return(EDDSACC26X2_STATUS_FSM_RUN_PKA_OP);
}

/*
 *  ======== EDDSACC26X2_runSignFSM ========
 */
static int_fast16_t EDDSACC26X2_runSignFSM(EDDSA_Handle handle) {
    EDDSACC26X2_Object *object = handle->object;
    uint32_t pkaResult;

    switch(object->fsmState)
    {

        case EDDSACC26X2_FSM_SIGN1_HASH_PRIVATE_KEY:
            /* Hash and prune the private key to sign the message */
            SHA2_hashData(object->sha2Handle,
                          object->operation.sign->myPrivateKey->
                              u.plaintext.keyMaterial,
                          ECCParams_Ed25519.length,
                          object->EDDSACC26X2_GlobalWorkspace.digestResult);

            return(EDDSACC26X2_STATUS_FSM_RUN_PKA_OP);

        case EDDSACC26X2_FSM_SIGN1_HASH_UPPER_SECRET_KEY:

            /*
             * Generate the secret scalar r by hashing the upper half of the
             * previous hash digest as well as the message.
             * r = H ( upperHalf(H(sk)) || PH(M))
             */
            SHA2_addData(object->sha2Handle,
                         &object->EDDSACC26X2_GlobalWorkspace.digestResult[32],
                         ECCParams_Ed25519.length);

            return(EDDSACC26X2_STATUS_FSM_RUN_PKA_OP);

        case EDDSACC26X2_FSM_SIGN1_HASH_MESSAGE1:

            /*
             * Perform a first hash so that the following hashes are only in
             * preHashedMessage
             */
            if((ECCParams_Ed25519.length +
                    object->operation.sign->preHashedMessageLength)
                    > SHA2_BLOCK_SIZE_BYTES_512)
            {
                SHA2_addData(object->sha2Handle,
                             object->operation.sign->preHashedMessage,
                             SHA2_BLOCK_SIZE_BYTES_512 -
                             ECCParams_Ed25519.length);
            }
            else
            {
                SHA2_addData(object->sha2Handle,
                             object->operation.sign->preHashedMessage,
                             object->operation.sign->preHashedMessageLength);
            }

            return(EDDSACC26X2_STATUS_FSM_RUN_PKA_OP);

        case EDDSACC26X2_FSM_SIGN1_HASH_MESSAGE2:
            /*
             * Perform an additional hash of the remaining preHashedMessage, if
             * needed.
             */
            if((ECCParams_Ed25519.length +
                    object->operation.sign->preHashedMessageLength)
                    > SHA2_BLOCK_SIZE_BYTES_512)
            {
                SHA2_addData(object->sha2Handle,
                             &object->operation.sign->
                                 preHashedMessage[SHA2_BLOCK_SIZE_BYTES_512 -
                                                  ECCParams_Ed25519.length],
                             object->operation.sign->
                                 preHashedMessageLength -
                             (SHA2_BLOCK_SIZE_BYTES_512 -
                                 ECCParams_Ed25519.length));

                return(EDDSACC26X2_STATUS_FSM_RUN_PKA_OP);
            }
            else
            {
                /* Otherwise just continue to the next FSM state */
                return(EDDSACC26X2_STATUS_FSM_RUN_FSM);
            }

        case EDDSACC26X2_FSM_SIGN1_HASH_FINALIZE:

            SHA2_finalize(object->sha2Handle,
                          object->EDDSACC26X2_GlobalWorkspace.digestResult2);

            return(EDDSACC26X2_STATUS_FSM_RUN_PKA_OP);

        case EDDSACC26X2_FSM_SIGN2_MOD_SECRET_HASH:
            /*
             * Prune the least significant half of the hash digest as this is
             * used to generate signature component S.
             */
            /* Clear bits 0,1,2 */
            object->EDDSACC26X2_GlobalWorkspace.digestResult[0]   &= 0xF8;
            /* Clear Bit 255 */
            object->EDDSACC26X2_GlobalWorkspace.digestResult[31]  &= 0x7F;
            /* Set bit 254 */
            object->EDDSACC26X2_GlobalWorkspace.digestResult[31]  |= 0x40;

            /*
             * Copy the private key to scratch memory so that
             * we can perform operations with it.
             */
            CryptoUtils_copyPad(object->EDDSACC26X2_GlobalWorkspace.
                                    digestResult2,
                                SCRATCH_PRIVATE_KEY,
                                2 * ECCParams_Ed25519.length);

            /*
             * Compute r mod n. This is necessary to run the ECC point
             * multiplication and also roughly doubles the speed of the point
             * multiplication
             */
            PKABigNumModStart((uint8_t*)SCRATCH_PRIVATE_KEY,
                              2 * ECCParams_Ed25519.length,
                              ECCParams_Ed25519.order,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_SIGN2_MOD_SECRET_HASH_RESULT:

            pkaResult = PKABigNumModGetResult(object->
                                              EDDSACC26X2_GlobalWorkspace.
                                                  secretScalar,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_SIGN2_MULT_SECRET_HASH_BY_GENERATOR_WEIER:
            /*
             * Perform the point multiplication R = r*B to generate the public
             * key point in short Weierstrass form
             */
            PKAEccMultiplyStart(object->EDDSACC26X2_GlobalWorkspace.
                                    secretScalar,
                                ECCParams_Wei25519.generatorX,
                                ECCParams_Wei25519.generatorY,
                                ECCParams_Wei25519.prime,
                                ECCParams_Wei25519.a,
                                ECCParams_Wei25519.b,
                                ECCParams_Wei25519.length,
                                &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_SIGN2_MULT_SECRET_HASH_BY_GENERATOR_WEIER_RESULT:

            pkaResult = PKAEccMultiplyGetResult(SCRATCH_PUBLIC_X,
                                                SCRATCH_PUBLIC_Y,
                                                EDDSACC26X2_resultAddress,
                                                ECCParams_Wei25519.length);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_SIGN2_CONVERT_WEI_TO_ED:
            /*
             * Run through the substate FSM to convert the short Weierstrass
             * point to an Edwards point.
             */
            pkaResult = EDDSACC26X2_runWeiToEdFSM(handle);

            /* We break if we just started a PKA operation */
            if(pkaResult == EDDSACC26X2_PKA_OPERATION_STARTED)
            {
                break;
            }
            else /* Otherwise, convert the return value */
            {
                return(EDDSACC26X2_convertReturnValue(pkaResult));
            }

        case EDDSACC26X2_FSM_SIGN2_ENCODE_PUBLIC_KEY:
            /* Encode the public key to the output */
            EDDSACC26X2_encodePublicKey(handle,
                                        object->operation.sign->R,
                                        object->EDDSACC26X2_GlobalWorkspace
                                            .xResult,
                                        object->EDDSACC26X2_GlobalWorkspace
                                            .yResult);

            return(EDDSACC26X2_STATUS_FSM_RUN_FSM);

        case EDDSACC26X2_FSM_SIGN3_HASH_SIG_R:
            /*
             * Generate the signature digest by hashing the signature component
             * R,
             * public key A, and prehashed message PH(M)
             * H ( R || A || PH(M))
             */
            SHA2_addData(object->sha2Handle,
                         object->operation.sign->R,
                         ECCParams_Ed25519.length);

            return(EDDSACC26X2_STATUS_FSM_RUN_PKA_OP);

        case EDDSACC26X2_FSM_SIGN3_HASH_PUBLIC_KEY:

            SHA2_addData(object->sha2Handle,
                         object->operation.sign->myPublicKey->
                             u.plaintext.keyMaterial,
                         ECCParams_Ed25519.length);

            return(EDDSACC26X2_STATUS_FSM_RUN_PKA_OP);

        case EDDSACC26X2_FSM_SIGN3_HASH_MESSAGE1:

            /*
             * Perform a first hash so that the following hashes are only in
             * preHashedMessage
             */
            if(((2 * ECCParams_Ed25519.length) + object->operation.sign->
                    preHashedMessageLength) > SHA2_BLOCK_SIZE_BYTES_512)
            {
                SHA2_addData(object->sha2Handle,
                             object->operation.sign->preHashedMessage,
                             SHA2_BLOCK_SIZE_BYTES_512 -
                             (2 * ECCParams_Ed25519.length));
            }
            else
            {
                SHA2_addData(object->sha2Handle,
                             object->operation.sign->preHashedMessage,
                             object->operation.sign->preHashedMessageLength);
            }

            return(EDDSACC26X2_STATUS_FSM_RUN_PKA_OP);

        case EDDSACC26X2_FSM_SIGN3_HASH_MESSAGE2:

            /*
             * Perform an additional hash of the remaining preHashedMessage,
             * if needed.
             */
            if(((2 * ECCParams_Ed25519.length) + object->operation.sign->
                    preHashedMessageLength) > SHA2_BLOCK_SIZE_BYTES_512)
            {
                SHA2_addData(object->sha2Handle,
                             &object->operation.sign->
                             preHashedMessage[SHA2_BLOCK_SIZE_BYTES_512 -
                                              (2 * ECCParams_Ed25519.length)],
                             object->operation.sign->preHashedMessageLength -
                             (SHA2_BLOCK_SIZE_BYTES_512 -
                                     (2 * ECCParams_Ed25519.length)));

                return(EDDSACC26X2_STATUS_FSM_RUN_PKA_OP);
            }
            else
            {
                /* Otherwise just continue to the next FSM state */
                return(EDDSACC26X2_STATUS_FSM_RUN_FSM);
            }

        case EDDSACC26X2_FSM_SIGN3_HASH_FINALIZE:

            SHA2_finalize(object->sha2Handle,
                          object->EDDSACC26X2_GlobalWorkspace.digestResult2);

            return(EDDSACC26X2_STATUS_FSM_RUN_PKA_OP);

        case EDDSACC26X2_FSM_SIGN4_MULT_SIG_HASH_BY_SECRET_KEY:
            /* Close the SHA2 driver as we no longer need it */
            SHA2_close(object->sha2Handle);
            /*
             * Compute the signature component S
             * S = r + H(R || A || M) * s mod n
             */
            /* Compute H(R || A || M) * s */
            PKABigNumMultiplyStart(object->EDDSACC26X2_GlobalWorkspace.
                                       digestResult2,
                                   2 * ECCParams_Ed25519.length,
                                   object->EDDSACC26X2_GlobalWorkspace.
                                       digestResult,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_SIGN4_MULT_SIG_HASH_BY_SECRET_KEY_RESULT:

            EDDSACC26X2_scratchBuffer0Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumMultGetResult(SCRATCH_BUFFER_0,
                                               &EDDSACC26X2_scratchBuffer0Size,
                                               EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_SIGN4_ADD_SECRET_HASH_TO_MULT:
            /* Compute S = r + H(R || A || M) * s */
            PKABigNumAddStart(SCRATCH_BUFFER_0,
                              3 * ECCParams_Ed25519.length,
                              object->EDDSACC26X2_GlobalWorkspace.secretScalar,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_SIGN4_ADD_SECRET_HASH_TO_MULT_RESULT:

            EDDSACC26X2_scratchBuffer0Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumAddGetResult(SCRATCH_BUFFER_0,
                                              &EDDSACC26X2_scratchBuffer0Size,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_SIGN4_MOD_S:
            /* Compute S mod n */
            PKABigNumModStart(SCRATCH_BUFFER_0,
                              3 * ECCParams_Ed25519.length,
                              ECCParams_Ed25519.order,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_SIGN4_MOD_S_RESULT:

            pkaResult = PKABigNumModGetResult(object->operation.sign->S,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            if(pkaResult == PKA_STATUS_SUCCESS)
            {
                return(EDDSA_STATUS_SUCCESS);
            }
            else
            {
                return(EDDSA_STATUS_ERROR);
            }
        default:
            return(EDDSA_STATUS_ERROR);
    }

    /* If we get to this point, we want to perform another PKA operation */
    IntPendClear(INT_PKA_IRQ);
    IntEnable(INT_PKA_IRQ);

    return(EDDSACC26X2_STATUS_FSM_RUN_PKA_OP);
}

/*
 *  ======== EDDSACC26X2_runVerifyFSM ========
 */
static int_fast16_t EDDSACC26X2_runVerifyFSM(EDDSA_Handle handle) {
    EDDSACC26X2_Object *object = handle->object;
    uint32_t pkaResult;

    switch(object->fsmState)
    {

        case EDDSACC26X2_FSM_VERIFY1_HASH_SIG_R:
            /*
             * Generate the signature digest by hashing the signature component
             * R, public key A, and prehashed message PH(M)
             * H ( R || A || PH(M))
             */
            SHA2_addData(object->sha2Handle,
                         object->operation.verify->R,
                         ECCParams_Ed25519.length);

            return(EDDSACC26X2_STATUS_FSM_RUN_PKA_OP);

        case EDDSACC26X2_FSM_VERIFY1_HASH_PUBLIC_KEY:

            SHA2_addData(object->sha2Handle,
                         object->operation.verify->theirPublicKey->
                             u.plaintext.keyMaterial,
                         ECCParams_Ed25519.length);

            return(EDDSACC26X2_STATUS_FSM_RUN_PKA_OP);

        case EDDSACC26X2_FSM_VERIFY1_HASH_MESSAGE1:

            /*
             * Perform a first hash so that the following hashes are only in
             * preHashedMessage
             */
            if(((2 * ECCParams_Ed25519.length) + object->operation.verify->
                    preHashedMessageLength) > SHA2_BLOCK_SIZE_BYTES_512)
            {
                SHA2_addData(object->sha2Handle,
                             object->operation.verify->preHashedMessage,
                             SHA2_BLOCK_SIZE_BYTES_512 -
                             (2 * ECCParams_Ed25519.length));
            }
            else
            {
                SHA2_addData(object->sha2Handle,
                             object->operation.verify->preHashedMessage,
                             object->operation.verify->preHashedMessageLength);
            }

            return(EDDSACC26X2_STATUS_FSM_RUN_PKA_OP);

        case EDDSACC26X2_FSM_VERIFY1_HASH_MESSAGE2:

            /*
             * Perform an additional hash of the remaining preHashedMessage,
             * if needed.
             */
            if(((2 * ECCParams_Ed25519.length) + object->operation.verify->
                    preHashedMessageLength) > SHA2_BLOCK_SIZE_BYTES_512)
            {
                SHA2_addData(object->sha2Handle,
                             &object->operation.verify->
                                 preHashedMessage[SHA2_BLOCK_SIZE_BYTES_512 -
                                                  (2 *
                                                   ECCParams_Ed25519.length)],
                             object->operation.verify->preHashedMessageLength -
                             (SHA2_BLOCK_SIZE_BYTES_512 -
                                     (2 * ECCParams_Ed25519.length)));

                return(EDDSACC26X2_STATUS_FSM_RUN_PKA_OP);
            }
            else
            {
                /* Otherwise just continue to the next FSM state */
                return(EDDSACC26X2_STATUS_FSM_RUN_FSM);
            }

        case EDDSACC26X2_FSM_VERIFY1_HASH_FINALIZE:

            SHA2_finalize(object->sha2Handle,
                          object->EDDSACC26X2_GlobalWorkspace.digestResult2);

            return(EDDSACC26X2_STATUS_FSM_RUN_PKA_OP);

        case EDDSACC26X2_FSM_VERIFY1_S_IN_RANGE:

            /* Close the SHA2 driver as we no longer need it */
            SHA2_close(object->sha2Handle);

            /* Check if S < n ? */
            PKABigNumCmpStart(object->operation.verify->S,
                              ECCParams_Ed25519.order,
                              ECCParams_Ed25519.length);

            while(PKAGetOpsStatus() == PKA_STATUS_OPERATION_BUSY)
                {
                continue;
                }

            pkaResult = PKABigNumCmpGetResult();

            /* If S is not less than n, then it is invalid. */
            if(pkaResult != PKA_STATUS_A_LESS_THAN_B)
            {
                return(EDDSA_STATUS_S_LARGER_THAN_ORDER);
            }

            break;

        case EDDSACC26X2_FSM_VERIFY1_A_IS_NOT_POINT_AT_INFINITY:

            /* Retrieve the public key */
            memcpy(object->EDDSACC26X2_GlobalWorkspace.publicKey,
                   object->operation.verify->theirPublicKey->u.plaintext.keyMaterial,
                   ECCParams_Ed25519.length);

            /* The MSB of the public key indicates if sqrt(x) is even or odd */
            object->EDDSACC26X2_GlobalWorkspace.x_0 = object->
                    EDDSACC26X2_GlobalWorkspace.publicKey[31] >> 7;

            /*
             * Remove the MSB of the public key so that it is only the
             * y-coordinate
             */
            object->EDDSACC26X2_GlobalWorkspace.publicKey[31] &= 0x7F;

            /* Check if y <= 1 ? */
            PKABigNumCmpStart(object->EDDSACC26X2_GlobalWorkspace.publicKey,
                              EDDSACC26X2_one,
                              ECCParams_Ed25519.length);

            while(PKAGetOpsStatus() == PKA_STATUS_OPERATION_BUSY)
                {
                continue;
                }

            pkaResult = PKABigNumCmpGetResult();

            /*
             * If y is 1 or 0, then this is a point of low order which cannot
             * be a public key. This also catches an invalid encoded
             * representation of the point at infinity.
             */
            if(pkaResult != PKA_STATUS_A_GREATER_THAN_B)
            {
                return(EDDSA_STATUS_PUBLIC_KEY_NOT_ON_CURVE);
            }

            break;

        case EDDSACC26X2_FSM_VERIFY1_MULT_Y_BY_Y:
            /*
             * Public Key decoding: Retrieve the x-coordinate
             * from the compressed public key encoding
             * Edwards curve form: a * x^2 + y^2 = 1 + d * x^2 * y^2
             * Solving for x^2:
             * x^2 = (y^2 - 1) / (d * y^2 - a)
             * To simplify the square root computation,
             * we let u = y^2 - 1 and v = d * y^2 + 1 and solve
             * (found in RFC 8032 Section 5.1.3)
             * x = u * v^3 * (u * v^7) ^ ((p-5)/8)
             */

            /* Compute y^2 */
            PKABigNumMultiplyStart(object->
                                       EDDSACC26X2_GlobalWorkspace.publicKey,
                                   ECCParams_Ed25519.length,
                                   object->
                                       EDDSACC26X2_GlobalWorkspace.publicKey,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MULT_Y_BY_Y_RESULT:

            EDDSACC26X2_scratchBuffer1Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumMultGetResult(SCRATCH_BUFFER_1,
                                               &EDDSACC26X2_scratchBuffer1Size,
                                               EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_MOD_Y2:
            /* Compute y^2 mod p */
            PKABigNumModStart(SCRATCH_BUFFER_1,
                              2 * ECCParams_Ed25519.length,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MOD_Y2_RESULT:

            EDDSACC26X2_scratchBuffer1Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumModGetResult(SCRATCH_BUFFER_1,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_SUBTRACT_ONE_FROM_Y2:
            /* Compute u = y^2 - 1 */
            PKABigNumAddStart(SCRATCH_BUFFER_1,
                              ECCParams_Ed25519.length,
                              EDDSACC26X2_negOne,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_SUBTRACT_ONE_FROM_Y2_RESULT:

            EDDSACC26X2_scratchBuffer0Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumAddGetResult(SCRATCH_BUFFER_0,
                                              &EDDSACC26X2_scratchBuffer0Size,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_MULT_D_BY_Y2:
            /* Compute d * y^2 */
            PKABigNumMultiplyStart(SCRATCH_BUFFER_1,
                                   ECCParams_Ed25519.length,
                                   ECCParams_Ed25519.b,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MULT_D_BY_Y2_RESULT:

            EDDSACC26X2_scratchBuffer1Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumMultGetResult(SCRATCH_BUFFER_1,
                                               &EDDSACC26X2_scratchBuffer1Size,
                                               EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_MOD_D_Y2:
            /* Compute d * y^2 mod p */
            PKABigNumModStart(SCRATCH_BUFFER_1,
                              2 * ECCParams_Ed25519.length,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MOD_D_Y2_RESULT:

            EDDSACC26X2_scratchBuffer1Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumModGetResult(SCRATCH_BUFFER_1,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_ADD_ONE_TO_D_Y2:
            /* Compute v = d * y^2 + 1 */
            PKABigNumAddStart(SCRATCH_BUFFER_1,
                              ECCParams_Ed25519.length,
                              EDDSACC26X2_one,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_ADD_ONE_TO_D_Y2_RESULT:

            EDDSACC26X2_scratchBuffer1Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumAddGetResult(SCRATCH_BUFFER_1,
                                              &EDDSACC26X2_scratchBuffer1Size,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_MULT_V_BY_V:
            /* We need to compute u * v^3 * (u * v^7)^((p - 5) / 8)
             * Start by computing u*v^3 and u*v^7
             */
            /* Compute v^2 */
            PKABigNumMultiplyStart(SCRATCH_BUFFER_1,
                                   ECCParams_Ed25519.length,
                                   SCRATCH_BUFFER_1,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MULT_V_BY_V_RESULT:

            EDDSACC26X2_scratchBuffer2Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumMultGetResult(SCRATCH_BUFFER_2,
                                               &EDDSACC26X2_scratchBuffer2Size,
                                               EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_MOD_V2:
            /* Compute v^2 mod p */
            PKABigNumModStart(SCRATCH_BUFFER_2,
                              2 * ECCParams_Ed25519.length,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MOD_V2_RESULT:

            EDDSACC26X2_scratchBuffer1Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumModGetResult(SCRATCH_BUFFER_2,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_MULT_V2_BY_V:
            /* Compute v^3 */
            PKABigNumMultiplyStart(SCRATCH_BUFFER_1,
                                   ECCParams_Ed25519.length,
                                   SCRATCH_BUFFER_2,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MULT_V2_BY_V_RESULT:

            EDDSACC26X2_scratchBuffer2Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumMultGetResult(SCRATCH_BUFFER_2,
                                               &EDDSACC26X2_scratchBuffer2Size,
                                               EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_MOD_V3:
            /* Compute v^3 mod p */
            PKABigNumModStart(SCRATCH_BUFFER_2,
                              2 * ECCParams_Ed25519.length,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MOD_V3_RESULT:

            pkaResult = PKABigNumModGetResult(SCRATCH_BUFFER_3,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_MULT_V3_BY_U:
            /* Compute u * v^3 */
            PKABigNumMultiplyStart(SCRATCH_BUFFER_0,
                                   ECCParams_Ed25519.length,
                                   SCRATCH_BUFFER_3,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MULT_V3_BY_U_RESULT:

            EDDSACC26X2_scratchBuffer2Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumMultGetResult(SCRATCH_BUFFER_2,
                                               &EDDSACC26X2_scratchBuffer2Size,
                                               EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_MOD_U_V3:
            /* Compute leftside = u * v^3 mod p */
            PKABigNumModStart(SCRATCH_BUFFER_2,
                              2 * ECCParams_Ed25519.length,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MOD_U_V3_RESULT:

            EDDSACC26X2_scratchBuffer1Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumModGetResult(SCRATCH_BUFFER_2,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_MULT_U_V3_BY_V3:
            /* Compute u * v^6 */
            PKABigNumMultiplyStart(SCRATCH_BUFFER_3,
                                   ECCParams_Ed25519.length,
                                   SCRATCH_BUFFER_2,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MULT_U_V3_BY_V3_RESULT:

            EDDSACC26X2_scratchBuffer3Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumMultGetResult(SCRATCH_BUFFER_3,
                                               &EDDSACC26X2_scratchBuffer2Size,
                                               EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_MOD_U_V6:
            /* Compute u * v^6 mod p */
            PKABigNumModStart(SCRATCH_BUFFER_3,
                              2 * ECCParams_Ed25519.length,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MOD_U_V6_RESULT:

            pkaResult = PKABigNumModGetResult(SCRATCH_BUFFER_3,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_MULT_U_V6_BY_V:
            /* Compute rightside = u * v^7 */
            PKABigNumMultiplyStart(SCRATCH_BUFFER_3,
                                   ECCParams_Ed25519.length,
                                   SCRATCH_BUFFER_1,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MULT_U_V6_BY_V_RESULT:

            EDDSACC26X2_scratchBuffer3Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumMultGetResult(SCRATCH_BUFFER_3,
                                               &EDDSACC26X2_scratchBuffer2Size,
                                               EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_MOD_U_V7:
            /* Compute u * v^7 mod p */
            PKABigNumModStart(SCRATCH_BUFFER_3,
                              2 * ECCParams_Ed25519.length,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MOD_U_V7_RESULT:

            pkaResult = PKABigNumModGetResult(SCRATCH_BUFFER_3,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_EXP_U_V7:
            /* Compute (u * v^7) ^ ((p - 5) / 8) */
            PKABigNumExpModStart(SCRATCH_BUFFER_3,
                                 ECCParams_Ed25519.length,
                                 EDDSACC26X2_decodeConst1,
                                 ECCParams_Ed25519.length,
                                 ECCParams_Ed25519.prime,
                                 ECCParams_Ed25519.length,
                                 &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_EXP_U_V7_RESULT:

            EDDSACC26X2_scratchBuffer3Size = SCRATCH_BUFFER_SIZE;
            /* Exponentiation and inversion have the same kind of result */
            pkaResult = PKABigNumInvModGetResult(SCRATCH_BUFFER_3,
                                             EDDSACC26X2_scratchBuffer3Size,
                                             EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_MULT_U_V3_BY_EXP:
            /* Compute u * v^3 * (u * v^7) ^ ((p - 5) / 8 ) */
            PKABigNumMultiplyStart(SCRATCH_BUFFER_2,
                                   ECCParams_Ed25519.length,
                                   SCRATCH_BUFFER_3,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MULT_U_V3_BY_EXP_RESULT:

            EDDSACC26X2_scratchBuffer2Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumMultGetResult(SCRATCH_BUFFER_2,
                                               &EDDSACC26X2_scratchBuffer2Size,
                                               EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_MOD_CANDIDATE_X:
            /*
             * Compute candidate x = u * v^3 * (u * v^7) ^ ((p - 5) / 8 ) mod p
             */
            PKABigNumModStart(SCRATCH_BUFFER_2,
                              2 * ECCParams_Ed25519.length,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MOD_CANDIDATE_X_RESULT:

            pkaResult = PKABigNumModGetResult(object->
                                                  EDDSACC26X2_GlobalWorkspace.
                                                  xResult,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_MULT_X_BY_X:
            /* For p = 5 mod 8 (as is the case for Ed25519), we need to check
             * for the three following conditions:
             * 1. v * x^2 = u mod p, then x is the square root
             * 2. v * x^2 = -u mod p, then x * 2^((p - 1) / 4) is the square
             * root
             * 3. Otherwise no square root exists
             */

            /* Compute candidate x^2 */
            PKABigNumMultiplyStart(object->EDDSACC26X2_GlobalWorkspace.xResult,
                                   ECCParams_Ed25519.length,
                                   object->EDDSACC26X2_GlobalWorkspace.xResult,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MULT_X_BY_X_RESULT:

            EDDSACC26X2_scratchBuffer2Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumMultGetResult(SCRATCH_BUFFER_3,
                                               &EDDSACC26X2_scratchBuffer2Size,
                                               EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_MOD_X2:
            /* Compute candidate x^2 mod p */
            PKABigNumModStart(SCRATCH_BUFFER_3,
                              2 * ECCParams_Ed25519.length,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MOD_X2_RESULT:

            pkaResult = PKABigNumModGetResult(SCRATCH_BUFFER_3,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_MULT_X2_BY_V:
            /* Compute candidate v * x^2 */
            PKABigNumMultiplyStart(SCRATCH_BUFFER_1,
                                   ECCParams_Ed25519.length,
                                   SCRATCH_BUFFER_3,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MULT_X2_BY_V_RESULT:

            EDDSACC26X2_scratchBuffer2Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumMultGetResult(SCRATCH_BUFFER_3,
                                               &EDDSACC26X2_scratchBuffer2Size,
                                               EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_MOD_V_X2:
            /* Compute candidate v * x^2 mod p */
            PKABigNumModStart(SCRATCH_BUFFER_3,
                              2 * ECCParams_Ed25519.length,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MOD_V_X2_RESULT:

            pkaResult = PKABigNumModGetResult(SCRATCH_BUFFER_3,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_MOD_U:
            /* Compute u mod p */
            PKABigNumModStart(SCRATCH_BUFFER_0,
                              ECCParams_Ed25519.length,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_MOD_U_RESULT:

            EDDSACC26X2_scratchBuffer0Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumModGetResult(SCRATCH_BUFFER_0,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_NEGATE_U:
            /*
             * Compute -u mod p by performing p - u (note that if u = 0,
             * then -u = p, but the comparison will either pass or fail if
             * v * x^2 = u mod p */
            PKABigNumSubStart(ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              SCRATCH_BUFFER_0,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY1_NEGATE_U_RESULT:

            EDDSACC26X2_scratchBuffer1Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumSubGetResult(SCRATCH_BUFFER_1,
                                              &EDDSACC26X2_scratchBuffer1Size,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY1_CHECK_SQUARE_ROOT:
            /* For p = 5 mod 8 (as is the case for Ed25519), we need to check
             * for the three following conditions:
             * 1. v * x^2 = u mod p, then x is the square root
             * 2. v * x^2 = -u mod p, then x * 2^((p - 1) / 4) is the square
             * root
             * 3. Otherwise no square root exists
             *
             * Since there is conditional logic and these are all quick
             * operations, we do these in one FSM state with polling. The
             * resulting x-coordinate (if it exists) is moved to the
             * object->EDDSACC26X2_GlobalWorkspace.xResult buffer.
             */
            /* Check if v * x^2 == u ? */
            PKABigNumCmpStart(SCRATCH_BUFFER_3,
                              SCRATCH_BUFFER_0,
                              ECCParams_Ed25519.length);

            while(PKAGetOpsStatus() == PKA_STATUS_OPERATION_BUSY)
                {
                continue;
                }

            pkaResult = PKABigNumCmpGetResult();

            /* If v * x^2 != u, then we have to check further */
            if(pkaResult != PKA_STATUS_EQUAL)
            {
                /* Check if condition 2 is true that v * x^2 == -u mod p */
                PKABigNumCmpStart(SCRATCH_BUFFER_3,
                                  SCRATCH_BUFFER_1,
                                  ECCParams_Ed25519.length);

                while(PKAGetOpsStatus() == PKA_STATUS_OPERATION_BUSY)
                    {
                    continue;
                    }

                pkaResult = PKABigNumCmpGetResult();
                if(pkaResult == PKA_STATUS_EQUAL)
                {
                    /* Compute x * 2^((p - 1) / 4) */
                    PKABigNumMultiplyStart(object->
                                           EDDSACC26X2_GlobalWorkspace.xResult,
                                           ECCParams_Ed25519.length,
                                           EDDSACC26X2_decodeConst2,
                                           ECCParams_Ed25519.length,
                                           &EDDSACC26X2_resultAddress);

                    while(PKAGetOpsStatus() == PKA_STATUS_OPERATION_BUSY)
                        {
                        continue;
                        }

                    EDDSACC26X2_scratchBuffer2Size = SCRATCH_BUFFER_SIZE;
                    pkaResult = PKABigNumMultGetResult(SCRATCH_BUFFER_2,
                                               &EDDSACC26X2_scratchBuffer2Size,
                                               EDDSACC26X2_resultAddress);

                    if(pkaResult != PKA_STATUS_SUCCESS)
                    {
                        return(EDDSACC26X2_convertReturnValue(pkaResult));
                    }

                    /* Compute x * 2^((p - 1) / 4) mod p */
                    PKABigNumModStart(SCRATCH_BUFFER_2,
                                      2 * ECCParams_Ed25519.length,
                                      ECCParams_Ed25519.prime,
                                      ECCParams_Ed25519.length,
                                      &EDDSACC26X2_resultAddress);

                    while(PKAGetOpsStatus() == PKA_STATUS_OPERATION_BUSY)
                        {
                        continue;
                        }

                    pkaResult = PKABigNumModGetResult(object->
                                           EDDSACC26X2_GlobalWorkspace.xResult,
                                           ECCParams_Ed25519.length,
                                           EDDSACC26X2_resultAddress);

                    if(pkaResult != PKA_STATUS_SUCCESS)
                    {
                        return(EDDSACC26X2_convertReturnValue(pkaResult));
                    }
                }
                else
                {
                    /*
                     * x-coordinate square root does not exist so this is not a
                     * valid point
                     */
                    return(EDDSA_STATUS_PUBLIC_KEY_NOT_ON_CURVE);
                }
            }

            /*
             * Check if the correct square root was chosen by bit x_0.
             * The MSB of the public key specifies if the x-coordinate
             * should be even or odd.
             */
            if(object->EDDSACC26X2_GlobalWorkspace.x_0 != (object->
                    EDDSACC26X2_GlobalWorkspace.xResult[0] & 0x1))
            {
                /* Wrong square root result selected, so perform p - x */
                PKABigNumSubStart(ECCParams_Ed25519.prime,
                                  ECCParams_Ed25519.length,
                                  object->EDDSACC26X2_GlobalWorkspace.xResult,
                                  ECCParams_Ed25519.length,
                                  &EDDSACC26X2_resultAddress);

                while(PKAGetOpsStatus() == PKA_STATUS_OPERATION_BUSY)
                    {
                    continue;
                    }

                EDDSACC26X2_scratchPublicXSize = SCRATCH_KEY_SIZE;
                pkaResult = PKABigNumSubGetResult(object->
                                              EDDSACC26X2_GlobalWorkspace.
                                                  xResult,
                                              &EDDSACC26X2_scratchPublicXSize,
                                              EDDSACC26X2_resultAddress);

                if(pkaResult != PKA_STATUS_SUCCESS)
                {
                    return(EDDSACC26X2_convertReturnValue(pkaResult));
                }
            }

            return(EDDSACC26X2_STATUS_FSM_RUN_FSM);

        case EDDSACC26X2_FSM_VERIFY2_ADD_ONE_TO_P:
            /*
             * Perform conversion from Ed25519 to Wei25519
             * (x,y) on Ed25519 to (u,v) on Wei25519
             * um is the x-coordinate on X25519
             * um = (1 + y) / (1 - y)
             * u = um + X25519.A/3
             * v = sqrt(-486664) * um * (x)^-1
             * To only compute one inversion, we compute ((1 - y) * (x))^-1
             */

            /* Compute p + 1 so that we can perform 1 - y */
            PKABigNumAddStart(ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              EDDSACC26X2_one,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_ADD_ONE_TO_P_RESULT:

            EDDSACC26X2_scratchBuffer0Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumAddGetResult(SCRATCH_BUFFER_0,
                                              &EDDSACC26X2_scratchBuffer0Size,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_SUBTRACT_Y_FROM_P_PLUS_ONE:
            /* Compute 1 - y + p (positive number) */
            PKABigNumSubStart(SCRATCH_BUFFER_0,
                              ECCParams_Ed25519.length,
                              object->EDDSACC26X2_GlobalWorkspace.publicKey,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_SUBTRACT_Y_FROM_P_PLUS_ONE_RESULT:

            EDDSACC26X2_scratchBuffer0Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumSubGetResult(SCRATCH_BUFFER_0,
                                              &EDDSACC26X2_scratchBuffer0Size,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_MULT_ONE_MINUS_Y_BY_X:
            /* Compute (1 - y) * x */
            PKABigNumMultiplyStart(SCRATCH_BUFFER_0,
                                   ECCParams_Ed25519.length,
                                   object->EDDSACC26X2_GlobalWorkspace.xResult,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_MULT_ONE_MINUS_Y_BY_X_RESULT:

            EDDSACC26X2_scratchBuffer1Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumMultGetResult(SCRATCH_BUFFER_1,
                                               &EDDSACC26X2_scratchBuffer1Size,
                                               EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_MOD_ONE_MINUS_Y_X:
            /* Compute (1 - y) * x mod p */
            PKABigNumModStart(SCRATCH_BUFFER_1,
                              2 * ECCParams_Ed25519.length,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_MOD_ONE_MINUS_Y_X_RESULT:

            pkaResult = PKABigNumModGetResult(SCRATCH_BUFFER_1,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_INVERSION:
            /* Compute ((1 - y) * x) ^ (-1) mod p */
            PKABigNumInvModStart(SCRATCH_BUFFER_1,
                                 ECCParams_Ed25519.length,
                                 ECCParams_Ed25519.prime,
                                 ECCParams_Ed25519.length,
                                 &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_INVERSION_RESULT:

            EDDSACC26X2_scratchBuffer1Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumInvModGetResult(SCRATCH_BUFFER_1,
                                                 EDDSACC26X2_scratchBuffer1Size,
                                                 EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_MULT_INVERSE_BY_X:
            /* Compute (1 - y)^-1 */
            PKABigNumMultiplyStart(SCRATCH_BUFFER_1,
                                   ECCParams_Ed25519.length,
                                   object->EDDSACC26X2_GlobalWorkspace.xResult,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_MULT_INVERSE_BY_X_RESULT:

            EDDSACC26X2_scratchBuffer2Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumMultGetResult(SCRATCH_BUFFER_2,
                                               &EDDSACC26X2_scratchBuffer2Size,
                                               EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_MOD_ONE_MINUS_Y_INV:
            /* Compute (1 - y)^-1 mod p */
            PKABigNumModStart(SCRATCH_BUFFER_2,
                              2 * ECCParams_Ed25519.length,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_MOD_ONE_MINUS_Y_INV_RESULT:

            pkaResult = PKABigNumModGetResult(SCRATCH_BUFFER_2,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_MULT_INVERSE_BY_ONE_MINUS_Y:
            /* Compute (x)^-1 */
            PKABigNumMultiplyStart(SCRATCH_BUFFER_1,
                                   ECCParams_Ed25519.length,
                                   SCRATCH_BUFFER_0,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_MULT_INVERSE_BY_ONE_MINUS_Y_RESULT:

            EDDSACC26X2_scratchBuffer3Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumMultGetResult(SCRATCH_BUFFER_3,
                                               &EDDSACC26X2_scratchBuffer3Size,
                                               EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_MOD_X_INV:
            /* Compute (x)^-1 mod p */
            PKABigNumModStart(SCRATCH_BUFFER_3, 2 * ECCParams_Ed25519.length,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_MOD_X_INV_RESULT:

            pkaResult = PKABigNumModGetResult(SCRATCH_BUFFER_3,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_ADD_ONE_TO_Y:
            /* Compute y + 1 */
            PKABigNumAddStart(object->EDDSACC26X2_GlobalWorkspace.publicKey,
                              ECCParams_Ed25519.length,
                              EDDSACC26X2_one, ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_ADD_ONE_TO_Y_RESULT:

            EDDSACC26X2_scratchBuffer0Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumAddGetResult(SCRATCH_BUFFER_0,
                                              &EDDSACC26X2_scratchBuffer0Size,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_MULT_ONE_MINUS_Y_INV_BY_ONE_PLUS_Y:
            /* Compute um = (1 + y) * (1 - y)^-1 */
            PKABigNumMultiplyStart(SCRATCH_BUFFER_0,
                                   ECCParams_Ed25519.length,
                                   SCRATCH_BUFFER_2,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_MULT_ONE_MINUS_Y_INV_BY_ONE_PLUS_Y_RESULT:

            EDDSACC26X2_scratchBuffer0Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumMultGetResult(SCRATCH_BUFFER_0,
                                               &EDDSACC26X2_scratchBuffer0Size,
                                               EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_MOD_UM:
            /* Compute um = (1 + y) * (1 - y)^-1 mod p */
            PKABigNumModStart(SCRATCH_BUFFER_0,
                              2 * ECCParams_Ed25519.length,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_MOD_UM_RESULT:

            pkaResult = PKABigNumModGetResult(SCRATCH_BUFFER_0,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_MULT_UM_BY_X_INV:

            /* Compute um * (x)^-1 */
            PKABigNumMultiplyStart(SCRATCH_BUFFER_0,
                                   ECCParams_Ed25519.length,
                                   SCRATCH_BUFFER_3,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_MULT_UM_BY_X_INV_RESULT:

            EDDSACC26X2_scratchBuffer3Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumMultGetResult(SCRATCH_BUFFER_3,
                                               &EDDSACC26X2_scratchBuffer3Size,
                                               EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_MOD_UM_X_INV:
            /* Compute um * (x)^-1 mod p */
            PKABigNumModStart(SCRATCH_BUFFER_3,
                              2 * ECCParams_Ed25519.length,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_MOD_UM_X_INV_RESULT:

            pkaResult = PKABigNumModGetResult(SCRATCH_BUFFER_3,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_ADD_CONST_TO_UM:
            /* Compute u = um + Ed25519.A / 3 */
            PKABigNumAddStart(SCRATCH_BUFFER_0,
                              ECCParams_Ed25519.length,
                              EDDSACC26X2_isoConst2,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_ADD_CONST_TO_UM_RESULT:

            EDDSACC26X2_scratchBuffer0Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumAddGetResult(SCRATCH_BUFFER_0,
                                              &EDDSACC26X2_scratchBuffer0Size,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_MOD_WEIERX:
            /* Compute u = um + Ed25519.A / 3 mod p */
            PKABigNumModStart(SCRATCH_BUFFER_0,
                              ECCParams_Ed25519.length,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_MOD_WEIERX_RESULT:

            pkaResult = PKABigNumModGetResult(SCRATCH_PUBLIC_X,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_MULT_UM_X_INV_BY_CONST:
            /* Compute v = sqrt(-486664) * um * (x)^-1 */
            PKABigNumMultiplyStart(EDDSACC26X2_isoConst1,
                                   ECCParams_Ed25519.length,
                                   SCRATCH_BUFFER_3,
                                   ECCParams_Ed25519.length,
                                   &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_MULT_UM_X_INV_BY_CONST_RESULT:

            EDDSACC26X2_scratchBuffer3Size = SCRATCH_BUFFER_SIZE;
            pkaResult = PKABigNumMultGetResult(SCRATCH_BUFFER_3,
                                               &EDDSACC26X2_scratchBuffer3Size,
                                               EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_MOD_WEIERY:
            /* Compute v = sqrt(-486664) * um * (x)^-1 mod p */
            PKABigNumModStart(SCRATCH_BUFFER_3,
                              2 * ECCParams_Ed25519.length,
                              ECCParams_Ed25519.prime,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_MOD_WEIERY_RESULT:

            pkaResult = PKABigNumModGetResult(SCRATCH_PUBLIC_Y,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_MOD_SIG_DIGEST:
            /*
             * Perform -H(R || A || M) * A by first finding H(R || A || M) mod
             * order
             */
            PKABigNumModStart(object->EDDSACC26X2_GlobalWorkspace.
                                  digestResult2,
                              2 * ECCParams_Ed25519.length,
                              ECCParams_Ed25519.order,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_MOD_SIG_DIGEST_RESULT:

            pkaResult = PKABigNumModGetResult((uint8_t*)SCRATCH_PRIVATE_KEY,
                                              ECCParams_Ed25519.length,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_NEGATE_SIG_DIGEST:
            /*
             * Perform order - H(R || A || M) to get
             * -H(R || A || M) mod order
             */
            PKABigNumSubStart(ECCParams_Ed25519.order,
                              ECCParams_Ed25519.length,
                              (uint8_t*)SCRATCH_PRIVATE_KEY,
                              ECCParams_Ed25519.length,
                              &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_NEGATE_SIG_DIGEST_RESULT:

            EDDSACC26X2_scratchPrivKeySize = SCRATCH_KEY_SIZE;
            pkaResult = PKABigNumSubGetResult((uint8_t*)SCRATCH_PRIVATE_KEY,
                                              &EDDSACC26X2_scratchPrivKeySize,
                                              EDDSACC26X2_resultAddress);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_MULT_PUBLIC_KEY_BY_NEG_SIG_DIGEST:
            /* Perform - H(R || A || M) * A using short Weierstrass curves */
            PKAEccMultiplyStart((uint8_t*)SCRATCH_PRIVATE_KEY,
                                SCRATCH_PUBLIC_X,
                                SCRATCH_PUBLIC_Y,
                                ECCParams_Wei25519.prime,
                                ECCParams_Wei25519.a,
                                ECCParams_Wei25519.b,
                                ECCParams_Wei25519.length,
                                &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_MULT_PUBLIC_KEY_BY_NEG_SIG_DIGEST_RESULT:

            pkaResult = PKAEccMultiplyGetResult(object->
                                                EDDSACC26X2_GlobalWorkspace.
                                                    xResult,
                                                object->
                                                EDDSACC26X2_GlobalWorkspace.
                                                    yResult,
                                                EDDSACC26X2_resultAddress,
                                                ECCParams_Ed25519.length);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_MULT_BASE_POINT_BY_S:

            CryptoUtils_copyPad(object->operation.verify->S,
                                SCRATCH_PRIVATE_KEY,
                                ECCParams_Wei25519.length);

            /* Perform S * B using short Weierstrass curves */
            PKAEccMultiplyStart((uint8_t*)SCRATCH_PRIVATE_KEY,
                                ECCParams_Wei25519.generatorX,
                                ECCParams_Wei25519.generatorY,
                                ECCParams_Wei25519.prime,
                                ECCParams_Wei25519.a,
                                ECCParams_Wei25519.b,
                                ECCParams_Wei25519.length,
                                &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_MULT_BASE_POINT_BY_S_RESULT:

            pkaResult = PKAEccMultiplyGetResult(SCRATCH_BUFFER_0,
                                                SCRATCH_BUFFER_1,
                                                EDDSACC26X2_resultAddress,
                                                ECCParams_Ed25519.length);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_ADD_POINTS:
            /*
             * Perform S * B - H(R || A || M) * A using short Weierstrass
             * curves
             */
            PKAEccAddStart(SCRATCH_BUFFER_0,
                           SCRATCH_BUFFER_1,
                           object->EDDSACC26X2_GlobalWorkspace.xResult,
                           object->EDDSACC26X2_GlobalWorkspace.yResult,
                           ECCParams_Wei25519.prime,
                           ECCParams_Wei25519.a,
                           ECCParams_Wei25519.length,
                           &EDDSACC26X2_resultAddress);

            break;

        case EDDSACC26X2_FSM_VERIFY2_ADD_POINTS_RESULT:

            pkaResult = PKAEccAddGetResult(SCRATCH_PUBLIC_X,
                                           SCRATCH_PUBLIC_Y,
                                           EDDSACC26X2_resultAddress,
                                           ECCParams_Wei25519.length);

            return(EDDSACC26X2_convertReturnValue(pkaResult));

        case EDDSACC26X2_FSM_VERIFY2_CONVERT_WEI_TO_ED:
            /*
             * Run through the substate FSM to convert the short Weierstrass
             * point to an Edwards point.
             */
            pkaResult = EDDSACC26X2_runWeiToEdFSM(handle);

            /* We break if we just started a PKA operation */
            if(pkaResult == EDDSACC26X2_PKA_OPERATION_STARTED)
            {
                break;
            }
            else /* Otherwise, convert the return value */
            {
                return(EDDSACC26X2_convertReturnValue(pkaResult));
            }

        case EDDSACC26X2_FSM_VERIFY2_COMPARE_RESULT_R:
            /* Encode the public key and check if it matches R */
            EDDSACC26X2_encodePublicKey(handle,
                                        object->EDDSACC26X2_GlobalWorkspace.
                                            publicKey,
                                        object->EDDSACC26X2_GlobalWorkspace.
                                            xResult,
                                        object->EDDSACC26X2_GlobalWorkspace.
                                            yResult);

            /* Check R' == R, which indicates a valid signature */
            PKABigNumCmpStart(object->EDDSACC26X2_GlobalWorkspace.publicKey,
                              object->operation.verify->R,
                              ECCParams_Ed25519.length);

            while(PKAGetOpsStatus() == PKA_STATUS_OPERATION_BUSY)
                {
                continue;
                }

            pkaResult = PKABigNumCmpGetResult();
            if(pkaResult == PKA_STATUS_EQUAL)
            {
                return(EDDSA_STATUS_SUCCESS);
            }
            else
            {
                return(EDDSACC26X2_convertReturnValue(pkaResult));
            }
        default:
            return(EDDSA_STATUS_ERROR);
    }

    /* If we get to this point, we want to perform another PKA operation */
    IntPendClear(INT_PKA_IRQ);
    IntEnable(INT_PKA_IRQ);

    return(EDDSACC26X2_STATUS_FSM_RUN_PKA_OP);
}

/*
 *  ======== EDDSACC26X2_convertReturnValue ========
 */
static int_fast16_t EDDSACC26X2_convertReturnValue(uint32_t pkaResult)
{
    switch(pkaResult)
    {
        case PKA_STATUS_SUCCESS:

            return(EDDSACC26X2_STATUS_FSM_RUN_FSM);

        case PKA_STATUS_X_ZERO:
        case PKA_STATUS_Y_ZERO:
        case PKA_STATUS_RESULT_0:
            /*
             * Theoretically, PKA_STATUS_RESULT_0 might be caused by other
             * operations failing but the only one that really should yield
             * 0 is ECC multiplication with invalid inputs that yield the
             * point at infinity.
             */
            return(EDDSA_STATUS_POINT_AT_INFINITY);

        case PKA_STATUS_X_LARGER_THAN_PRIME:
        case PKA_STATUS_Y_LARGER_THAN_PRIME:

            return(EDDSA_STATUS_PUBLIC_KEY_NOT_ON_CURVE);

        case PKA_STATUS_POINT_NOT_ON_CURVE:

            return(EDDSA_STATUS_PUBLIC_KEY_NOT_ON_CURVE);

        default:
            return(EDDSA_STATUS_ERROR);
    }
}

/*
 *  ======== EDDSACC26X2_waitForAccess ========
 */
static int_fast16_t EDDSACC26X2_waitForAccess(EDDSA_Handle handle)
{
    EDDSACC26X2_Object *object = handle->object;
    uint32_t timeout;

    /* Set to SemaphoreP_NO_WAIT to start operations from SWI or HWI context */
    timeout = object->returnBehavior == EDDSA_RETURN_BEHAVIOR_BLOCKING ?
            object->semaphoreTimeout : SemaphoreP_NO_WAIT;

    return(SemaphoreP_pend(&PKAResourceCC26XX_accessSemaphore, timeout));
}

/*
 *  ======== EDDSACC26X2_waitForResult ========
 */
static int_fast16_t EDDSACC26X2_waitForResult(EDDSA_Handle handle)
{
    EDDSACC26X2_Object *object = handle->object;

    object->operationInProgress = true;

    switch(object->returnBehavior)
    {
        case EDDSA_RETURN_BEHAVIOR_POLLING:
            while(!PKAResourceCC26XX_pollingFlag)
                {
                continue;
                }
            return(object->operationStatus);
        case EDDSA_RETURN_BEHAVIOR_BLOCKING:
            SemaphoreP_pend(&PKAResourceCC26XX_operationSemaphore,
                            SemaphoreP_WAIT_FOREVER);
            return(object->operationStatus);
        case EDDSA_RETURN_BEHAVIOR_CALLBACK:
            return(EDDSA_STATUS_SUCCESS);
        default:
            return(EDDSA_STATUS_ERROR);
    }
}

/*
 *  ======== EDDSA_init ========
 */
void EDDSA_init(void)
{
    PKAResourceCC26XX_constructRTOSObjects();

    EDDSACC26X2_isInitialized = true;
}

/*
 *  ======== EDDSA_close ========
 */
void EDDSA_close(EDDSA_Handle handle)
{
    EDDSACC26X2_Object         *object;

    DebugP_assert(handle);

    /* Get the pointer to the object */
    object = handle->object;

    /* Mark the module as available */
    object->isOpen = false;

    /* Release power dependency on PKA Module. */
    Power_releaseDependency(PowerCC26X2_PERIPH_PKA);
}

/*
 *  ======== EDDSA_construct ========
 */
EDDSA_Handle EDDSA_construct(EDDSA_Config *config, const EDDSA_Params *params)
{
    EDDSA_Handle                  handle;
    EDDSACC26X2_Object           *object;
    uint_fast8_t                key;

    handle = (EDDSA_Handle)config;
    object = handle->object;

    /* If params are NULL, use defaults */
    if(params == NULL)
    {
        params = (EDDSA_Params *)&EDDSA_defaultParams;
    }

    key = HwiP_disable();

    if(!EDDSACC26X2_isInitialized || object->isOpen)
    {
        HwiP_restore(key);
        return(NULL);
    }

    object->isOpen = true;

    HwiP_restore(key);

    DebugP_assert((params->returnBehavior == EDDSA_RETURN_BEHAVIOR_CALLBACK) ?
            params->callbackFxn : true);

    object->returnBehavior = params->returnBehavior;
    object->callbackFxn = params->returnBehavior ==
            EDDSA_RETURN_BEHAVIOR_CALLBACK ? params->callbackFxn :
                    EDDSACC26X2_internalCallbackFxn;
    object->semaphoreTimeout = params->timeout;

    /*
     * Set power dependency --> power up and enable clock for PKA
     * (PKAResourceCC26XX) module.
     */
    Power_setDependency(PowerCC26X2_PERIPH_PKA);

    return(handle);
}

/*
 *  ======== EDDSA_generatePublicKey ========
 */
int_fast16_t EDDSA_generatePublicKey(EDDSA_Handle handle,
                                     EDDSA_OperationGeneratePublicKey
                                     *operation)
{
    EDDSACC26X2_Object *object              = handle->object;
    EDDSACC26X2_HWAttrs const *hwAttrs      = handle->hwAttrs;
    SHA2_Params sha2Params;

    /* Validate key sizes */
    if(operation->myPrivateKey->u.plaintext.keyLength !=
            operation->curve->length)
    {
        return(EDDSA_STATUS_INVALID_PRIVATE_KEY_SIZE);
    }

    if(operation->myPublicKey->u.plaintext.keyLength !=
            operation->curve->length)
    {
        return(EDDSA_STATUS_INVALID_PUBLIC_KEY_SIZE);
    }

    if(EDDSACC26X2_waitForAccess(handle) != SemaphoreP_OK)
    {
        return(EDDSA_STATUS_PKA_UNAVAILABLE);
    }

    /*
     * Copy over all parameters we will need access to in the FSM.
     * The FSM runs in SWI context and thus needs to keep track of
     * all of them somehow.
     */
    object->operation.generatePublicKey      = operation;
    object->operationType                    =
            EDDSA_OPERATION_TYPE_GENERATE_PUBLIC_KEY;
    object->fsmState                         =
            EDDSACC26X2_FSM_GEN_PUB_KEY_HASH_PRIVATE_KEY;
    object->fsmSubState                      =
            EDDSACC26X2_SUBFSM_WEI_TO_MONT_ADDITION;
    object->fsmFxn                           =
            EDDSACC26X2_runGeneratePublicKeyFSM;
    object->operationStatus                  = EDDSACC26X2_STATUS_FSM_RUN_FSM;
    object->operationCanceled                = false;

    /* Hash and prune the private key */
    SHA2_init();

    object->sha2HwAttrs.intPriority = hwAttrs->sha2IntPriority;
    object->sha2Config.object       = &object->sha2Object;
    object->sha2Config.hwAttrs      = &object->sha2HwAttrs;

    SHA2_Params_init(&sha2Params);
    sha2Params.hashType       = SHA2_HASH_TYPE_512;
    sha2Params.returnBehavior = SHA2_RETURN_BEHAVIOR_CALLBACK;
    sha2Params.callbackFxn    = EDDSACC26X2_sha2Callback;
    object->sha2Handle = SHA2_construct(&object->sha2Config, &sha2Params);

    if(object->sha2Handle == NULL)
    {
        SemaphoreP_post(&PKAResourceCC26XX_accessSemaphore);

        return(EDDSA_STATUS_HASH_UNAVAILABLE);
    }

    /*
     * We need to set the HWI function and priority since the same physical
     * interrupt is shared by multiple drivers and they all need to coexist.
     * Whenever a driver starts an operation, it registers its HWI callback
     * with the OS.
     */
    HwiP_setFunc(&PKAResourceCC26XX_hwi, EDDSACC26X2_hwiFxn,
                 (uintptr_t)handle);
    HwiP_setPriority(INT_PKA_IRQ, hwAttrs->intPriority);

    PKAResourceCC26XX_pollingFlag = 0;

    Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);

    /*
     * Run the FSM by triggering the interrupt. It is level triggered
     * and the complement of the RUN bit.
     */
    IntEnable(INT_PKA_IRQ);

    return(EDDSACC26X2_waitForResult(handle));
}

/*
 *  ======== EDDSA_sign ========
 */
int_fast16_t EDDSA_sign(EDDSA_Handle handle, EDDSA_OperationSign *operation)
{
    EDDSACC26X2_Object *object              = handle->object;
    EDDSACC26X2_HWAttrs const *hwAttrs      = handle->hwAttrs;
    SHA2_Params sha2Params;

    /* Validate key sizes */
    if(operation->myPrivateKey->u.plaintext.keyLength !=
            operation->curve->length)
    {
        return(EDDSA_STATUS_INVALID_PRIVATE_KEY_SIZE);
    }

    if(operation->myPublicKey->u.plaintext.keyLength !=
            operation->curve->length)
    {
        return(EDDSA_STATUS_INVALID_PUBLIC_KEY_SIZE);
    }

    if(EDDSACC26X2_waitForAccess(handle) != SemaphoreP_OK)
    {
        return(EDDSA_STATUS_PKA_UNAVAILABLE);
    }

    /*
     * Copy over all parameters we will need access to in the FSM.
     * The FSM runs in SWI context and thus needs to keep track of
     * all of them somehow.
     */
    object->operation.sign                   = operation;
    object->operationType                    = EDDSA_OPERATION_TYPE_SIGN;
    object->fsmState                         =
            EDDSACC26X2_FSM_SIGN1_HASH_PRIVATE_KEY;
    object->fsmSubState                      =
            EDDSACC26X2_SUBFSM_WEI_TO_MONT_ADDITION;
    object->fsmFxn                           =
            EDDSACC26X2_runSignFSM;
    object->operationStatus                  =
            EDDSACC26X2_STATUS_FSM_RUN_FSM;
    object->operationCanceled                = false;

    /* Hash and prune the private key */
    SHA2_init();

    object->sha2HwAttrs.intPriority = hwAttrs->sha2IntPriority;
    object->sha2Config.object       = &object->sha2Object;
    object->sha2Config.hwAttrs      = &object->sha2HwAttrs;

    SHA2_Params_init(&sha2Params);
    sha2Params.hashType       = SHA2_HASH_TYPE_512;
    sha2Params.returnBehavior = SHA2_RETURN_BEHAVIOR_CALLBACK;
    sha2Params.callbackFxn    = EDDSACC26X2_sha2Callback;
    object->sha2Handle = SHA2_construct(&object->sha2Config, &sha2Params);

    if(object->sha2Handle == NULL)
    {
        SemaphoreP_post(&PKAResourceCC26XX_accessSemaphore);

        return(EDDSA_STATUS_HASH_UNAVAILABLE);
    }

    /*
     * We need to set the HWI function and priority since the same physical
     * interrupt is shared by multiple drivers and they all need to coexist.
     * Whenever a driver starts an operation, it registers its HWI callback
     * with the OS.
     */
    HwiP_setFunc(&PKAResourceCC26XX_hwi, EDDSACC26X2_hwiFxn,
                 (uintptr_t)handle);
    HwiP_setPriority(INT_PKA_IRQ, hwAttrs->intPriority);

    PKAResourceCC26XX_pollingFlag = 0;

    Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);

    /*
     * Run the FSM by triggering the interrupt. It is level triggered
     * and the complement of the RUN bit.
     */
    IntEnable(INT_PKA_IRQ);

    return(EDDSACC26X2_waitForResult(handle));
}

/*
 *  ======== EDDSA_verify ========
 */
int_fast16_t EDDSA_verify(EDDSA_Handle handle,
                          EDDSA_OperationVerify *operation)
{
    EDDSACC26X2_Object *object              = handle->object;
    EDDSACC26X2_HWAttrs const *hwAttrs      = handle->hwAttrs;
    SHA2_Params sha2Params;

    /* Validate key sizes */
    if(operation->theirPublicKey->u.plaintext.keyLength !=
            operation->curve->length)
    {
        return(EDDSA_STATUS_INVALID_PUBLIC_KEY_SIZE);
    }

    if(EDDSACC26X2_waitForAccess(handle) != SemaphoreP_OK)
    {
        return(EDDSA_STATUS_PKA_UNAVAILABLE);
    }

    /*
     * Copy over all parameters we will need access to in the FSM.
     * The FSM runs in SWI context and thus needs to keep track of
     * all of them somehow.
     */
    object->operation.verify                 = operation;
    object->operationType                    = EDDSA_OPERATION_TYPE_VERIFY;
    object->fsmState                         =
            EDDSACC26X2_FSM_VERIFY1_HASH_SIG_R;
    object->fsmSubState                      =
            EDDSACC26X2_SUBFSM_WEI_TO_MONT_ADDITION;
    object->fsmFxn                           =
            EDDSACC26X2_runVerifyFSM;
    object->operationStatus                  = EDDSACC26X2_STATUS_FSM_RUN_FSM;
    object->operationCanceled                = false;

    /* Hash and prune the private key */
    SHA2_init();

    object->sha2HwAttrs.intPriority = hwAttrs->sha2IntPriority;
    object->sha2Config.object       = &object->sha2Object;
    object->sha2Config.hwAttrs      = &object->sha2HwAttrs;

    SHA2_Params_init(&sha2Params);
    sha2Params.hashType       = SHA2_HASH_TYPE_512;
    sha2Params.returnBehavior = SHA2_RETURN_BEHAVIOR_CALLBACK;
    sha2Params.callbackFxn    = EDDSACC26X2_sha2Callback;
    object->sha2Handle = SHA2_construct(&object->sha2Config, &sha2Params);

    if(object->sha2Handle == NULL)
    {
        SemaphoreP_post(&PKAResourceCC26XX_accessSemaphore);

        return(EDDSA_STATUS_HASH_UNAVAILABLE);
    }

    /*
     * We need to set the HWI function and priority since the same physical
     * interrupt is shared by multiple drivers and they all need to coexist.
     * Whenever a driver starts an operation, it registers its HWI callback
     * with the OS.
     */
    HwiP_setFunc(&PKAResourceCC26XX_hwi, EDDSACC26X2_hwiFxn,
                 (uintptr_t)handle);
    HwiP_setPriority(INT_PKA_IRQ, hwAttrs->intPriority);

    PKAResourceCC26XX_pollingFlag = 0;

    Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);

    /*
     * Run the FSM by triggering the interrupt. It is level triggered
     * and the complement of the RUN bit.
     */
    IntEnable(INT_PKA_IRQ);

    return(EDDSACC26X2_waitForResult(handle));
}

/*
 *  ======== EDDSA_cancelOperation ========
 */
int_fast16_t EDDSA_cancelOperation(EDDSA_Handle handle)
{
    EDDSACC26X2_Object *object = handle->object;

    if(!object->operationInProgress)
    {
        return(EDDSA_STATUS_ERROR);
    }

    object->operationCanceled = true;

    /* Post hwi as if operation finished for cleanup */
    IntEnable(INT_PKA_IRQ);
    HwiP_post(INT_PKA_IRQ);

    return(EDDSA_STATUS_SUCCESS);
}
