/*
 * Copyright (c) 2016-2021, Texas Instruments Incorporated
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
#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/HwiP.h>

/* TI-RTOS drivers */
#include <ti/drivers/ADC.h>
#include <ti/drivers/adc/ADCCC26XX.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOCC26XX.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(inc/hw_aux_evctl.h)
#include DeviceFamily_constructPath(driverlib/aux_adc.h)
#include DeviceFamily_constructPath(driverlib/aux_smph.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/ioc.h)
#include DeviceFamily_constructPath(driverlib/aon_ioc.h)

#if (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X0_CC26X0)
    #include DeviceFamily_constructPath(driverlib/aux_wuc.h)
#elif (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X1_CC26X1 || \
       DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X2_CC26X2 || \
       DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X4_CC26X3_CC26X4)
    #define AUX_EVCTL_EVTOMCUFLAGS_ADC_DONE         AUX_EVCTL_EVTOMCUFLAGS_AUX_ADC_DONE
    #define AUX_EVCTL_EVTOMCUFLAGS_ADC_IRQ          AUX_EVCTL_EVTOMCUFLAGS_AUX_ADC_IRQ
#endif


/*
 * =============================================================================
 * Public Function Declarations
 * =============================================================================
 */
void ADCCC26XX_close(ADC_Handle handle);
void ADCCC26XX_init(ADC_Handle handle);
ADC_Handle ADCCC26XX_open(ADC_Handle handle, ADC_Params *params);
int_fast16_t ADCCC26XX_convert(ADC_Handle handle, uint16_t *value);
int_fast16_t ADCCC26XX_convertChain(ADC_Handle *handleList,
                                    uint16_t *dataBuffer,
                                    uint8_t channelCount);
int_fast16_t ADCCC26XX_control(ADC_Handle handle, uint_fast16_t cmd, void *arg);
uint32_t ADCCC26XX_convertToMicroVolts(ADC_Handle handle, uint16_t adcValue);

/*
 * =============================================================================
 * Private Function Declarations
 * =============================================================================
 */

/*
 * =============================================================================
 * Constants
 * =============================================================================
 */

/* ADC function table for ADCCC26XX implementation */
const ADC_FxnTable ADCCC26XX_fxnTable = {
    ADCCC26XX_close,
    ADCCC26XX_control,
    ADCCC26XX_convert,
    ADCCC26XX_convertChain,
    ADCCC26XX_convertToMicroVolts,
    ADCCC26XX_init,
    ADCCC26XX_open
};

/*
 * =============================================================================
 * Private Global Variables
 * =============================================================================
 */

/* Keep track of the adc handle instance to create and delete adcSemaphore */
static uint16_t adcInstance = 0;

/* Semaphore to arbitrate access to the single ADC peripheral between multiple handles */
static SemaphoreP_Struct adcSemaphore;

/*
 * =============================================================================
 * Function Definitions
 * =============================================================================
 */

/*
 *  ======== ADCCC26XX_close ========
 */
void ADCCC26XX_close(ADC_Handle handle) {
    ADCCC26XX_HWAttrs const *hwAttrs;
    ADCCC26XX_Object  *object;

    DebugP_assert(handle);

    object = handle->object;
    hwAttrs = handle->hwAttrs;

    uint32_t key = HwiP_disable();

    if (object->isOpen) {
        adcInstance--;
        if (adcInstance == 0) {
            SemaphoreP_destruct(&adcSemaphore);
        }

        /* Deallocate pins */
        GPIO_resetConfig(hwAttrs->adcDIO);
        DebugP_log0("ADC: Object closed");
    }
    else {
        return;
    }
    object->isOpen = false;
    HwiP_restore(key);
}


/*
 *  ======== ADCCC26XX_control ========
 */
int_fast16_t ADCCC26XX_control(ADC_Handle handle, uint_fast16_t cmd, void *arg){
    /* No implementation yet */
    return ADC_STATUS_UNDEFINEDCMD;
}

/*
 *  ======== ADCCC26XX_convert ========
 */
int_fast16_t ADCCC26XX_convert(ADC_Handle handle, uint16_t *value){
    ADCCC26XX_HWAttrs   const *hwAttrs;
    ADCCC26XX_Object    *object;
    int_fast16_t        conversionResult = ADC_STATUS_ERROR;
    uint16_t            conversionValue = 0;
    uint32_t            interruptStatus = 0;

    DebugP_assert(handle);

    /* Get handle */
    hwAttrs = handle->hwAttrs;

    /* Get the object */
    object = handle->object;

    if (object->isProtected) {
        /* Acquire the lock for this particular ADC handle */
        SemaphoreP_pend(&adcSemaphore, SemaphoreP_WAIT_FOREVER);
    }

    /* Set constraints to guarantee operation */
    Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);

    /* Acquire the ADC hw semaphore. Return an error if the hw semaphore is not
     * available. There is only one interrupt available for the hw semaphores
     * and it is used by the TDC already. Busy-wait polling might lock up the
     * device and starting timeout clocks would add overhead and be clunky. It
     * is better if such functionality is implemented at application level
     * if desired.
     */
    if(!AUXSMPHTryAcquire(AUX_SMPH_2)){
        Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);
        if (object->isProtected) {
            SemaphoreP_post(&adcSemaphore);
        }
        return conversionResult;
    }

    /* Specify input in ADC module */
    AUXADCSelectInput(hwAttrs->adcCompBInput);

    /* Flush the ADC FIFO in case we have triggered prior to this call */
    AUXADCFlushFifo();

    /* If input scaling is set to disabled in the params, disable it */
    if (!hwAttrs->inputScalingEnabled){
        AUXADCDisableInputScaling();
    }

    /* Use synchronous sampling mode and prepare for trigger */
    AUXADCEnableSync(hwAttrs->refSource, hwAttrs->samplingDuration,
                     hwAttrs->triggerSource);

    /* Manually trigger the ADC once */
    AUXADCGenManualTrigger();

    /* Poll until the sample is ready */
    conversionValue = AUXADCReadFifo();

    /* Get the status of the ADC_IRQ line and ADC_DONE.
     * Despite not using the interrupt line, we need to clear it so that the
     * ADCBuf driver does not call Hwi_construct and have the interrupt fire
     * immediately.
     */
    interruptStatus = HWREG(AUX_EVCTL_BASE + AUX_EVCTL_O_EVTOMCUFLAGS) &
                        (AUX_EVCTL_EVTOMCUFLAGS_ADC_IRQ |
                         AUX_EVCTL_EVTOMCUFLAGS_ADC_DONE);

    /* Clear the ADC_IRQ flag in AUX_EVTCTL */
    HWREG(AUX_EVCTL_BASE + AUX_EVCTL_O_EVTOMCUFLAGSCLR) = interruptStatus;

    /* Clear the ADC_IRQ within the NVIC as AUX_EVTCTL will only set the
     * relevant flag in the NVIC it will not clear it.
     */
    HwiP_clearInterrupt(INT_AUX_ADC_IRQ);

    conversionResult = ADC_STATUS_SUCCESS;

    /* Shut down the ADC peripheral */
    AUXADCDisable();

    /* Release the ADC hw semaphore */
    AUXSMPHRelease(AUX_SMPH_2);

    /* Allow entering standby again after ADC conversion complete */
    Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);

    if (object->isProtected) {
        /* Release the lock for this particular ADC handle */
        SemaphoreP_post(&adcSemaphore);
    }

    /* If we want to return the trimmed value, calculate it here. */
    if (hwAttrs->returnAdjustedVal) {
        uint32_t gain = AUXADCGetAdjustmentGain(hwAttrs->refSource);
        uint32_t offset = AUXADCGetAdjustmentOffset(hwAttrs->refSource);
        conversionValue = AUXADCAdjustValueForGainAndOffset(conversionValue,
                                                            gain, offset);
    }

    *value = conversionValue;

    /* Return the number of bytes transfered by the ADC */
    return conversionResult;
}

/*
 *  ======== ADCCC26XX_convertChain ========
 */
int_fast16_t ADCCC26XX_convertChain(ADC_Handle *handleList,
                                    uint16_t *dataBuffer,
                                    uint8_t channelCount)
{
    ADCCC26XX_HWAttrs   const *hwAttrs;
    int_fast16_t        conversionResult = ADC_STATUS_ERROR;
    uint16_t            conversionValue = 0;
    uint8_t             i;
    uint32_t            gain = 1;
    uint32_t            offset = 0;

    /* Use the first handle in the list to set the sampling mode and
     * prepare the trigger for the entire conversion chain. This means that
     * we are effectively ignoring the attributes of the other handles since
     * we expect them to be the same.
     */
    hwAttrs = handleList[0]->hwAttrs;

    /* Acquire the lock used arbitrate access to the ADC peripheral
     * between multiple handles.
     */
    SemaphoreP_pend(&adcSemaphore, SemaphoreP_WAIT_FOREVER);

    /* Try to acquire the ADC hw semaphore. */
    if(!AUXSMPHTryAcquire(AUX_SMPH_2)) {
        SemaphoreP_post(&adcSemaphore);
        return conversionResult;
    }

    /* Flush the ADC FIFO since we have triggered the ADC prior to this call */
        AUXADCFlushFifo();

    /* If input scaling is set to disabled in the params, disable it */
    if (!hwAttrs->inputScalingEnabled) {
        AUXADCDisableInputScaling();
    }

    /* Use synchronous sampling mode and prepare for trigger */
    AUXADCEnableSync(hwAttrs->refSource,
                     hwAttrs->samplingDuration,
                     hwAttrs->triggerSource);

    /* Calculate gain and offset in case we want to return a trimmed value */
    if (hwAttrs->returnAdjustedVal) {
        gain = AUXADCGetAdjustmentGain(hwAttrs->refSource);
        offset = AUXADCGetAdjustmentOffset(hwAttrs->refSource);
    }

    for (i = 0; i < channelCount; i++) {

        /* Get the particular handle */
        hwAttrs = handleList[i]->hwAttrs;

        /* Specify input in ADC module */
        AUXADCSelectInput(hwAttrs->adcCompBInput);

        /* Manually trigger the ADC once */
        AUXADCGenManualTrigger();

        /* Poll until the sample is ready */
        conversionValue = AUXADCReadFifo();

        /* If necessary, adjust value for gain and offset */
        if (hwAttrs->returnAdjustedVal) {
            conversionValue = AUXADCAdjustValueForGainAndOffset(conversionValue,
                                                                gain,
                                                                offset);
        }
        dataBuffer[i] = conversionValue;
    }

    /* Clear the ADC_IRQ and ADC_DONE flags in AUX_EVTCTL.
     * We need to clear it so that the ADCBuf driver does not call
     * Hwi_construct and have the interrupt fire immediately.*/
    HWREG(AUX_EVCTL_BASE + AUX_EVCTL_O_EVTOMCUFLAGSCLR) = (AUX_EVCTL_EVTOMCUFLAGS_ADC_IRQ |
                                                           AUX_EVCTL_EVTOMCUFLAGS_ADC_DONE);

    /* Clear the ADC_IRQ within the NVIC as AUX_EVTCTL will only set the
     * relevant flag in the NVIC it will not clear it.
     */
    HwiP_clearInterrupt(INT_AUX_ADC_IRQ);

    /* Shut down the ADC peripheral.*/
    AUXADCDisable();

    /* Release the ADC hw semaphore */
    AUXSMPHRelease(AUX_SMPH_2);

    conversionResult = ADC_STATUS_SUCCESS;

    /* Release the lock used arbitrate access to the single ADC peripheral
     * between multiple handles.
     */
    SemaphoreP_post(&adcSemaphore);

    return conversionResult;
}

/*
 *  ======== ADCCC26XX_convertToMicroVolts ========
 */
uint32_t ADCCC26XX_convertToMicroVolts(ADC_Handle handle, uint16_t adcValue){
    ADCCC26XX_HWAttrs           const *hwAttrs;
    uint32_t                    adjustedValue;

    DebugP_assert(handle);

    /* Get the pointer to the hwAttrs */
    hwAttrs = handle->hwAttrs;

    /* Only apply trim if specified */
    if (hwAttrs->returnAdjustedVal) {
        adjustedValue = adcValue;
    }
    else {
        uint32_t gain = AUXADCGetAdjustmentGain(hwAttrs->refSource);
        uint32_t offset = AUXADCGetAdjustmentOffset(hwAttrs->refSource);
        adjustedValue = AUXADCAdjustValueForGainAndOffset(adcValue, gain,
                                                          offset);
    }

    if(hwAttrs->refSource == ADCCC26XX_FIXED_REFERENCE)
    {
        return AUXADCValueToMicrovolts(
                (hwAttrs->inputScalingEnabled ?
                        AUXADC_FIXED_REF_VOLTAGE_NORMAL :
                        AUXADC_FIXED_REF_VOLTAGE_UNSCALED),
                        adjustedValue);
    }
    else
    {
        return AUXADCValueToMicrovolts(hwAttrs->refVoltage, adjustedValue);
    }
}

/*
 *  ======== ADCCC26XX_init ========
 */
void ADCCC26XX_init(ADC_Handle handle){
    ADCCC26XX_Object *object;

    /* Get the object */
    object = handle->object;

    /* Mark the object as available */
    object->isOpen = false;
}

/*
 *  ======== ADCCC26XX_open ========
 */
ADC_Handle ADCCC26XX_open(ADC_Handle handle, ADC_Params *params){
    ADCCC26XX_Object            *object;
    ADCCC26XX_HWAttrs           const *hwAttrs;

    DebugP_assert(handle);

    /* Get object and hwAttrs */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* Determine if the driver was already opened */
    uint32_t key = HwiP_disable();

    if (object->isOpen){
        DebugP_log0("ADC: Error! Already in use.");
        HwiP_restore(key);
        return NULL;
    }
    object->isOpen = true;

    /* Remember thread safety protection setting */
    object->isProtected = params->isProtected;

    /* If this is the first handle requested, set up the semaphore as well */
    if (adcInstance == 0) {
        /* Setup semaphore */
        SemaphoreP_constructBinary(&adcSemaphore, 1);
    }
    adcInstance++;

    /* On Chameleon, ANAIF must be clocked to use it. On Agama, the register
     * interface is always available. */
#if (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X0_CC26X0)
    /* Turn on the ANAIF clock. ANIAF contains the aux ADC. */
    AUXWUCClockEnable(AUX_WUC_ANAIF_CLOCK);
    AUXWUCClockEnable(AUX_WUC_ADI_CLOCK);
#endif

    HwiP_restore(key);

    /* Reserve the DIO defined in the hwAttrs */
    GPIO_setConfig(hwAttrs->adcDIO, GPIO_CFG_NO_DIR);

    DebugP_log0("ADC: Object opened");

    return handle;
}
