/*
 * Copyright (c) 2021, Texas Instruments Incorporated
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


#include <stdint.h>

/* Kernel services */
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/HwiP.h>

/* TI-RTOS drivers */
#include <ti/drivers/ADC.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/DAC.h>
#include <ti/drivers/dac/DACCC26X2.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/aux_dac.h)
#include DeviceFamily_constructPath(driverlib/aux_smph.h)

#define DAC_CLOCK_FREQ_24MHZ        0U
#define MICROVOLTS_PER_MILLIVOLT    1000
#define CALIBRATION_VDDS            3000

/* Forward declarations */
static int_fast16_t DAC_setRange(DAC_Handle handle);

/* Extern globals */
extern const DAC_Config DAC_config[];
extern const uint_least8_t DAC_count;
extern const ADC_Config ADC_config[];
extern const uint_least8_t ADC_count;
extern const uint_least8_t CONFIG_ADC_AUX_CONST;

/* Static Globals */
static bool isInitialized = (bool)false;
static uint16_t dacInstance = 0;
static SemaphoreP_Struct dacSemaphore;


/*
 *  ======== DAC_init ========
 */
void DAC_init(void) {

    isInitialized = (bool)true;
}

/*
 *  ======== DAC_open ========
 */
DAC_Handle DAC_open(uint_least8_t index, DAC_Params *params) {

    DAC_Handle              handle;
    DACCC26XX_Object        *object;
    DACCC26XX_HWAttrs       const *hwAttrs;

    /* Get handle and the object */
    handle = (DAC_Handle)&(DAC_config[index]);
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* Determine if the driver was already opened */
    uint32_t key = HwiP_disable();

    if (!isInitialized || object->isOpen) {
        HwiP_restore(key);
        return NULL;
    }

    /* If this is the first handle requested, set up the semaphore as well */
    if (dacInstance == 0) {
        /* Setup semaphore */
        SemaphoreP_constructBinary(&dacSemaphore, 1);
    }
    dacInstance++;

    /* Initialize DAC Object*/
    object->isOpen = (bool)true;
    HwiP_restore(key);

    if (params != NULL) {
        object->currCode = params->initCode;
    }
    else {
        object->currCode = 0;
    }

    GPIO_setConfig(hwAttrs->outputPin, GPIO_CFG_NO_DIR);

    return handle;
}

/*
 *  ======== DAC_close ========
 */
void DAC_close(DAC_Handle handle) {
    DACCC26XX_Object *object = handle->object;
    DACCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;

    uint32_t key = HwiP_disable();

    if (object->isOpen) {
        /* Check that the DAC is no longer in use. If it is, disable DAC's output. */
        if (object->isEnabled) {
            DAC_disable(handle);
        }
        dacInstance--;
        if (dacInstance == 0) {
            SemaphoreP_destruct(&dacSemaphore);
        }
    }
    else {
        HwiP_restore(key);
        return;
    }

    /* Mark the module as available */
    object->isOpen = (bool)false;
    HwiP_restore(key);

    /* Deallocate pins */
    GPIO_resetConfig(hwAttrs->outputPin);
}

/*
 *  ======== DAC_setVoltage ========
 */
int_fast16_t DAC_setVoltage(DAC_Handle handle, uint32_t uVoltOutput) {

    DACCC26XX_Object     *object = handle->object;
    int_fast16_t          status = DAC_STATUS_ERROR;
    uint32_t              code;

    /* Get the object */
    object = handle->object;

    /* Verify that the DAC's output is enabled for the handle. This is necessary given that
     * the DAC's output range depends on the voltage reference source.
     */
    if (object->isEnabled) {
        /* Check that the value is within the DAC's voltage range. */
        if ((uVoltOutput >= object->dacOutputMin) && (uVoltOutput <= object->dacOutputMax)) {
            code =  AUXDACCalcCode(uVoltOutput, object->dacOutputMin, object->dacOutputMax);
            AUXDACSetCode((uint8_t)code);
            object->currCode = code;
            status = DAC_STATUS_SUCCESS;
        }
        else {
            status =  DAC_STATUS_INVALID;
        }
    }

    return status;
}

/*
 *  ======== DAC_setCode ========
 */
int_fast16_t DAC_setCode(DAC_Handle handle, uint32_t code) {

    DACCC26XX_Object        *object;
    int_fast16_t             status = DAC_STATUS_ERROR;

    /* Get the object */
    object = handle->object;

    /* Verify that the DAC is currently enabled. */
    if (object->isEnabled) {
        AUXDACSetCode((uint8_t)code);
        object->currCode = code;
        status = DAC_STATUS_SUCCESS;
    }
    else {
        /* Even if the DAC is not enabled for this handle, update the DAC code in the object
         * so that once the DAC is enabled for the handle, this value will be set and the output
         * voltage will depend on the selected voltage reference source.
         */
        object->currCode = code;
        status = DAC_STATUS_SUCCESS;
    }
    return status;
}

/*
 *  ======== DAC_enable ========
 */
int_fast16_t DAC_enable(DAC_Handle handle) {

    DACCC26XX_Object        *object;
    DACCC26XX_HWAttrs       const *hwAttrs;
    int_fast16_t            status = DAC_STATUS_ERROR;

    /* Get handle */
    hwAttrs = handle->hwAttrs;

    /* Get the object */
    object = handle->object;

    /* Give control of the peripheral to this DAC handle. Return error if the peripheral has been
     * enabled by another handle.
     */
    if (SemaphoreP_pend(&dacSemaphore, SemaphoreP_NO_WAIT) != SemaphoreP_OK) {
        object->isEnabled = (bool)false;
        status = DAC_STATUS_INUSE;
        return status;
    }

    /* Acquire the DAC HW semaphore. Return an error if the HW semaphore is not available. */
    if (!AUXSMPHTryAcquire(AUX_SMPH_4)) {
        object->isEnabled = (bool)false;
        SemaphoreP_post(&dacSemaphore);
        status = DAC_STATUS_INUSE;
        return status;
    }

    /* Set voltage reference source */
    AUXDACSetVref((uint8_t)hwAttrs->dacVrefSource);

    /* Check the precharge status if DCOUPL has been selected as the voltage reference source. */
    if ((hwAttrs->dacVrefSource == AUXDAC_VREF_SEL_DCOUPL) && (hwAttrs->dacPrecharge)) {
        AUXDACEnablePreCharge();
    }
    else {
        AUXDACDisablePreCharge();
    }

    /* Determine DAC's output range */
    if(DAC_setRange(handle) != DAC_STATUS_SUCCESS) {
        object->isEnabled = (bool)false;
        SemaphoreP_post(&dacSemaphore);
        return status;
    }

    /* Configure the frequency of the DAC's sample clock. */
    AUXDACSetSampleClock(DAC_CLOCK_FREQ_24MHZ);

    /* Set the DAC's output code. */
    AUXDACSetCode((uint8_t)object->currCode);

    /* Enable the DAC's Sample Clock, the DAC's output buffer and the DAC, and connect output. */
    AUXDACEnable(hwAttrs->dacCompAInput);

    /* Set power constraints to guarantee operation */
    Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);

    /* Keep track of the DAC's current state as enabled. */
    object->isEnabled = (bool)true;

    status = DAC_STATUS_SUCCESS;

    return status;
}

/*
 *  ======== DAC_disable ========
 */
int_fast16_t DAC_disable(DAC_Handle handle) {

    DACCC26XX_Object        *object;
    int_fast16_t             status = DAC_STATUS_ERROR;

    /* Get the object */
    object = handle->object;

    if (object->isEnabled) {
        /* Disable the DAC's sample clock, the DAC's output buffer, the DAC, and disconnect output. */
        AUXDACDisable();

        /* Release the DAC HW semaphore */
        AUXSMPHRelease(AUX_SMPH_4);

        /* Allow entering standby again after DAC has been disabled */
        Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);

        /* Release the lock for this particular DAC handle */
        SemaphoreP_post(&dacSemaphore);

        /* Keep track of the DAC's current state as disabled. */
        object->isEnabled = (bool)false;
        status = DAC_STATUS_SUCCESS;
    }

    return status;
}

/*
 *  ======== DAC_setRange ========
 */
int_fast16_t DAC_setRange(DAC_Handle handle) {

    DACCC26XX_Object        *object;
    DACCC26XX_HWAttrs       const *hwAttrs;
    ADC_Handle              auxadcHandle;
    ADC_Params              auxadcParams;
    int_fast16_t            status = DAC_STATUS_ERROR;
    uint16_t                vdds;
    uint32_t                vddsMicroVolt;

    /* Get handle and the object */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    if (hwAttrs->dacVrefSource == AUXDAC_VREF_SEL_VDDS) {
        /* Create and initialize auxiliary ADC handle.
         * To account for VDDS variations, it's necessary to measure VDDS with the ADC peripheral.
         * This is done in such a way that access to the single ADC peripheral between the
         * auxiliary handle and other existing ADC handles is controlled by the semaphore
         * defined in the ADC driver.
         */
        ADC_init();

        /* Open auxiliary ADC handle */
        ADC_Params_init(&auxadcParams);
        auxadcHandle = ADC_open(CONFIG_ADC_AUX_CONST, &auxadcParams);
        if (auxadcHandle == NULL) {
            return status;
        }

        /* Measure VDDS using 12-bit ADC */
        if (ADC_convert(auxadcHandle, &vdds) != ADC_STATUS_SUCCESS) {
            return status;
        }
        vddsMicroVolt = ADC_convertToMicroVolts(auxadcHandle, vdds);

        /* Close auxiliary ADC handle */
        ADC_close(auxadcHandle);

        /* Obtain DAC's output range for the particular handle and scale it to microvolts.
         * Calibration values for VDDS are measured at 3.0 V, so it's necessary to scale to the actual VDDS.
         */
        object->dacOutputMin = AUXDACCalcMin() * MICROVOLTS_PER_MILLIVOLT;
        object->dacOutputMax = ((AUXDACCalcMax() * (vddsMicroVolt / MICROVOLTS_PER_MILLIVOLT)) / CALIBRATION_VDDS) * MICROVOLTS_PER_MILLIVOLT;

        status = DAC_STATUS_SUCCESS;
    }
    else {
        /* Calculate DAC's output range for the particular handle and scale it to microvolts.
         * No need to measure VDDS since it was not selected as the voltage reference source.
         */
        object->dacOutputMin  =  AUXDACCalcMin() * MICROVOLTS_PER_MILLIVOLT;
        object->dacOutputMax  =  AUXDACCalcMax() * MICROVOLTS_PER_MILLIVOLT;

        status = DAC_STATUS_SUCCESS;
    }

    return status;
}
