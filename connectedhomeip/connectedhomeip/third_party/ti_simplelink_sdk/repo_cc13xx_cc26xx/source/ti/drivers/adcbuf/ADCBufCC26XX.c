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
#include <ti/drivers/dpl/ClockP.h>
#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/SwiP.h>

/* TI-RTOS drivers */
#include <ti/drivers/ADCBuf.h>
#include <ti/drivers/adcbuf/ADCBufCC26XX.h>
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/dma/UDMACC26XX.h>
#include <ti/drivers/timer/GPTimerCC26XX.h>
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
    #define AUX_EVCTL_DMACTL_SEL_FIFO_NOT_EMPTY     AUX_EVCTL_DMACTL_SEL_AUX_ADC_FIFO_NOT_EMPTY
    #define AUX_EVCTL_EVTOMCUFLAGS_ADC_DONE         AUX_EVCTL_EVTOMCUFLAGS_AUX_ADC_DONE
    #define AUX_EVCTL_EVTOMCUFLAGS_ADC_IRQ          AUX_EVCTL_EVTOMCUFLAGS_AUX_ADC_IRQ
#endif

/*
 * =============================================================================
 * Public Function Declarations
 * =============================================================================
 */
void ADCBufCC26XX_init(ADCBuf_Handle handle);
ADCBuf_Handle ADCBufCC26XX_open(ADCBuf_Handle handle, const ADCBuf_Params *params);
int_fast16_t ADCBufCC26XX_convert(ADCBuf_Handle handle,
                                  ADCBuf_Conversion conversions[],
                                  uint_fast8_t channelCount);
int_fast16_t ADCBufCC26XX_convertCancel(ADCBuf_Handle handle);
void ADCBufCC26XX_close(ADCBuf_Handle handle);
uint_fast8_t ADCBufCC26XX_getResolution(ADCBuf_Handle handle);
int_fast16_t ADCBufCC26XX_adjustRawValues(ADCBuf_Handle handle,
                                          void *sampleBuffer,
                                          uint_fast16_t sampleCount,
                                          uint32_t adcChannel);
int_fast16_t ADCBufCC26XX_convertAdjustedToMicroVolts(
        ADCBuf_Handle handle, uint32_t adcChannel, void *adjustedSampleBuffer,
        uint32_t outputMicroVoltBuffer[], uint_fast16_t sampleCount);
int_fast16_t ADCBufCC26XX_control(ADCBuf_Handle handle, uint_fast16_t cmd,
                                  void *arg);

/*
 * =============================================================================
 * Private Function Declarations
 * =============================================================================
 */
static bool ADCBufCC26XX_acquireADCSemaphore(ADCBuf_Handle handle);
static bool ADCBufCC26XX_releaseADCSemaphore(ADCBuf_Handle handle);
static void ADCBufCC26XX_configDMA(ADCBuf_Handle handle,
                                   ADCBuf_Conversion *conversion);
static void ADCBufCC26XX_hwiFxn (uintptr_t arg);
static void ADCBufCC26XX_swiFxn (uintptr_t arg0, uintptr_t arg1);
static void ADCBufCC26XX_conversionCallback(ADCBuf_Handle handle,
                                            ADCBuf_Conversion *conversion,
                                            void *completedADCBuffer,
                                            uint32_t completedChannel,
                                            int_fast16_t status);
static uint32_t ADCBufCC26XX_freqToCounts(uint32_t frequency);
static void ADCBufCC26XX_configGPTDMA(ADCBuf_Handle handle,
                                      ADCBuf_Conversion *conversion);
static void ADCBufCC26XX_cleanADC(ADCBuf_Handle handle);
static void ADCBufCC26XX_loadDMAControlTableEntry(ADCBuf_Handle handle,
                                                  ADCBuf_Conversion *conversion,
                                                  bool primaryEntry);
static void ADCBufCC26XX_loadGPTDMAControlTableEntry(
        ADCBuf_Handle handle, ADCBuf_Conversion *conversion, bool primaryEntry);

/*
 * =============================================================================
 * Constants
 * =============================================================================
 */

const ADCBuf_FxnTable ADCBufCC26XX_fxnTable = {
    /*! Function to close the specified peripheral */
    ADCBufCC26XX_close,
    /*! Function to driver implementation specific control function */
    ADCBufCC26XX_control,
    /*! Function to initialize the given data object */
    ADCBufCC26XX_init,
    /*! Function to open the specified peripheral */
    ADCBufCC26XX_open,
    /*! Function to start an ADC conversion with the specified peripheral */
    ADCBufCC26XX_convert,
    /*! Function to abort a conversion being carried out by the specified
     *  peripheral */
    ADCBufCC26XX_convertCancel,
    /*! Function to get the resolution in bits of the ADC */
    ADCBufCC26XX_getResolution,
    /*! Function to adjust raw ADC output values to values comparable between
     *  devices of the same type */
    ADCBufCC26XX_adjustRawValues,
    /*! Function to convert adjusted ADC values to microvolts */
    ADCBufCC26XX_convertAdjustedToMicroVolts
};


/*
 * =============================================================================
 * Private Global Variables
 * =============================================================================
 */

/* Allocate space for DMA control table entry */
ALLOCATE_CONTROL_TABLE_ENTRY(dmaADCPriControlTableEntry,
                             (UDMA_CHAN_AUX_ADC + UDMA_PRI_SELECT));
ALLOCATE_CONTROL_TABLE_ENTRY(dmaADCAltControlTableEntry,
                             (UDMA_CHAN_AUX_ADC + UDMA_ALT_SELECT));
ALLOCATE_CONTROL_TABLE_ENTRY(dmaGPT0APriControlTableEntry,
                             (UDMA_CHAN_TIMER0_A + UDMA_PRI_SELECT));
ALLOCATE_CONTROL_TABLE_ENTRY(dmaGPT0AAltControlTableEntry,
                             (UDMA_CHAN_TIMER0_A + UDMA_ALT_SELECT));

/*!
 *  Timeout interrupt bitmask of the GPT that the DMA copies into GPT_O_ICLR
 *  to bring the DMA trigger event low.
 *  Needs to go into RAM not flash to prevent system from hanging in edge cases.
 */
static uint8_t gptClear = GPT_ICLR_TATOCINT;

/*
 * =============================================================================
 * Function Definitions
 * =============================================================================
 */

/*
 * ======== ADCBufCC26XX_init ========
 */
void ADCBufCC26XX_init(ADCBuf_Handle handle) {
    ADCBufCC26XX_Object        *object;

    /* Get the pointer to the object */
    object = handle->object;
    /* Mark the object as available */
    object->isOpen = false;
}


/*
 * ======== ADCBufCC26XX_open ========
 */
ADCBuf_Handle ADCBufCC26XX_open(ADCBuf_Handle handle,
                                const ADCBuf_Params *params) {
    /* Use union to save on stack allocation */
    union {
        HwiP_Params              hwiParams;
        SwiP_Params              swiParams;
        GPTimerCC26XX_Params    timerParams;
    } paramsUnion;
    ADCBufCC26XX_Object         *object;
    ADCBufCC26XX_HWAttrs  const *hwAttrs;
    uint32_t                    key;
    uint32_t                    adcPeriodCounts;

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* Disable preemption while checking if the ADC is open. */
    key = HwiP_disable();

    /* Check if the ADC is open already with the base addr. */
    if (object->isOpen == true) {
        HwiP_restore(key);

        DebugP_log0("ADCBuf: already in use.");

        return (NULL);
    }

    /* Mark the handle as being used */
    object->isOpen = true;

    /* On Chameleon, ANAIF must be clocked to use it. On Agama, the register
     * inferface is always available. */
#if (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X0_CC26X0)
    /* Turn on the ANAIF clock. ANIAF contains the AUX ADC. */
    AUXWUCClockEnable(AUX_WUC_ANAIF_CLOCK);
    AUXWUCClockEnable(AUX_WUC_ADI_CLOCK);
#endif

    HwiP_restore(key);

    /* Initialise the ADC object */
    /* Initialise params section of object */
    object->conversionInProgress        = false;
    object->semaphoreTimeout            = params->blockingTimeout;
    object->samplingFrequency           = params->samplingFrequency;
    object->returnMode                  = params->returnMode;
    object->recurrenceMode              = params->recurrenceMode;
    object->keepADCSemaphore            = false;
    object->adcSemaphoreInPossession    = false;

    if (params->custom) {
        /* If CC26XX specific params were specified, use them */
        object->samplingDuration =
                ((ADCBufCC26XX_ParamsExtension *) (params->custom))->samplingDuration;
        object->refSource =
                ((ADCBufCC26XX_ParamsExtension *) (params->custom))->refSource;
        object->samplingMode =
                ((ADCBufCC26XX_ParamsExtension *) (params->custom))->samplingMode;
        object->inputScalingEnabled =
                ((ADCBufCC26XX_ParamsExtension *) (params->custom))->inputScalingEnabled;
    }
    else {
        /* Initialise CC26XX specific settings to defaults */
        object->inputScalingEnabled     = true;
        object->refSource               = ADCBufCC26XX_FIXED_REFERENCE;
        object->samplingMode            = ADCBufCC26XX_SAMPING_MODE_SYNCHRONOUS;
        object->samplingDuration        = ADCBufCC26XX_SAMPLING_DURATION_2P7_US;
    }

    /* Open timer resource */
    GPTimerCC26XX_Params_init(&paramsUnion.timerParams);
    paramsUnion.timerParams.width           = GPT_CONFIG_16BIT;
    paramsUnion.timerParams.mode            = GPT_MODE_PERIODIC_UP;
    paramsUnion.timerParams.debugStallMode  = GPTimerCC26XX_DEBUG_STALL_OFF;
    /* Open position 0 of the GPT config table - by convention this is timer 0A. */
    object->timerHandle = GPTimerCC26XX_open(0, &paramsUnion.timerParams);

    if (object->timerHandle == NULL) {
        /* We did not manage to open the GPTimer we wanted */
        return NULL;
    }

    adcPeriodCounts = ADCBufCC26XX_freqToCounts(params->samplingFrequency);
    GPTimerCC26XX_setLoadValue(object->timerHandle, adcPeriodCounts);

    GPTimerCC26XX_enableInterrupt(object->timerHandle, GPT_INT_TIMEOUT);

    if (params->returnMode == ADCBuf_RETURN_MODE_BLOCKING) {
        /* Continuous trigger mode and blocking return mode is an illegal combination */
        DebugP_assert(!(params->recurrenceMode == ADCBuf_RECURRENCE_MODE_CONTINUOUS));

        /* Create a semaphore to block task execution for the duration of the
         * ADC conversions */
        SemaphoreP_constructBinary(&(object->conversionComplete), 0);

        /* Store internal callback function */
        object->callbackFxn = ADCBufCC26XX_conversionCallback;
    }
    else {
        /* Callback mode without a callback function defined */
        DebugP_assert(params->callbackFxn);

        /* Save the callback function pointer */
        object->callbackFxn = params->callbackFxn;
    }

    /* Clear the event flags to prevent an immediate interrupt from a previous
     *  configuration */
    HWREG(AUX_EVCTL_BASE + AUX_EVCTL_O_EVTOMCUFLAGSCLR) =
            (AUX_EVCTL_EVTOMCUFLAGS_ADC_IRQ | AUX_EVCTL_EVTOMCUFLAGS_ADC_DONE);
    HwiP_clearInterrupt(INT_AUX_ADC_IRQ);

    /* Create the Hwi for this ADC peripheral. */
    HwiP_Params_init(&paramsUnion.hwiParams);
    paramsUnion.hwiParams.arg = (uintptr_t) handle;
    paramsUnion.hwiParams.priority = hwAttrs->intPriority;
    HwiP_construct(&(object->hwi), INT_AUX_ADC_IRQ, ADCBufCC26XX_hwiFxn,
                   &paramsUnion.hwiParams);

    /* Create the Swi object for this ADC peripheral */
    SwiP_Params_init(&paramsUnion.swiParams);
    paramsUnion.swiParams.arg0 = (uintptr_t)handle;
    paramsUnion.swiParams.priority = hwAttrs->swiPriority;
    SwiP_construct(&(object->swi), ADCBufCC26XX_swiFxn, &(paramsUnion.swiParams));



    /* Declare the dependency on the UDMA driver */
    object->udmaHandle = UDMACC26XX_open();

    /* Return the handle after finishing initialisation of the driver */
    DebugP_log0("ADC: opened");
    return handle;

}


/*!
 *  @brief  HWI ISR of the ADC triggered when the DMA transaction is complete
 *
 *  @param  arg         An ADCBufCC26XX_Handle
 *
 */
static void ADCBufCC26XX_hwiFxn (uintptr_t arg) {
    ADCBufCC26XX_Object            *object;
    ADCBuf_Conversion              *conversion;
    uint32_t                        intStatus;

    /* Get the pointer to the object and current conversion*/
    object     = ((ADCBuf_Handle)arg)->object;
    conversion = object->currentConversion;

    /* Set activeSampleBuffer to primary as default */
    object->activeSampleBuffer = conversion->sampleBuffer;

    if (object->recurrenceMode == ADCBuf_RECURRENCE_MODE_ONE_SHOT) {
        /* Disable the ADC */
        AUXADCDisable();
        /* Disable ADC DMA if we are only doing one conversion and clear DMA
         * done interrupt. */
        HWREG(AUX_EVCTL_BASE + AUX_EVCTL_O_DMACTL) =
                AUX_EVCTL_DMACTL_REQ_MODE_SINGLE
                        | AUX_EVCTL_DMACTL_SEL_FIFO_NOT_EMPTY;
    }
    else if (object->recurrenceMode == ADCBuf_RECURRENCE_MODE_CONTINUOUS) {
        /* Reload the finished DMA control table entry */
        if (HWREG(UDMA0_BASE + UDMA_O_SETCHNLPRIALT) & (1 << UDMA_CHAN_AUX_ADC)) {
            /* We are currently using the alternate entry -> we just finished
             * the primary entry -> reload primary entry */
            ADCBufCC26XX_loadDMAControlTableEntry((ADCBuf_Handle) arg,
                                                  conversion, true);
            ADCBufCC26XX_loadGPTDMAControlTableEntry((ADCBuf_Handle) arg,
                                                     conversion, true);
        }
        else {
            /* We are currently using the primary entry -> we just finished the
             * alternate entry -> reload the alternate entry */
            ADCBufCC26XX_loadDMAControlTableEntry((ADCBuf_Handle) arg,
                                                  conversion, false);
            ADCBufCC26XX_loadGPTDMAControlTableEntry((ADCBuf_Handle) arg,
                                                     conversion, false);
            object->activeSampleBuffer = conversion->sampleBufferTwo;
        }
    }
    /* Clear DMA interrupts */
    UDMACC26XX_clearInterrupt(
            object->udmaHandle,
            (1 << UDMA_CHAN_AUX_ADC) | (1 << UDMA_CHAN_TIMER0_A));

    /* Get the status of the ADC_IRQ line and ADC_DONE */
    intStatus =
            HWREG(AUX_EVCTL_BASE + AUX_EVCTL_O_EVTOMCUFLAGS)
                    & (AUX_EVCTL_EVTOMCUFLAGS_ADC_IRQ
                            | AUX_EVCTL_EVTOMCUFLAGS_ADC_DONE);
    /* Clear the ADC_IRQ flag if it triggered the ISR */
    HWREG(AUX_EVCTL_BASE + AUX_EVCTL_O_EVTOMCUFLAGSCLR) = intStatus;

    /* Post SWI to handle remaining clean up and invocation of callback */
    SwiP_post(&(object->swi));
}

/*!
 *  @brief  SWI ISR of the ADC triggered when the DMA transaction is complete
 *
 *  @param  arg0        An ADCBufCC26XX_Handle
 *
 */
static void ADCBufCC26XX_swiFxn (uintptr_t arg0, uintptr_t arg1) {
    uint32_t                        key;
    ADCBuf_Conversion               *conversion;
    ADCBufCC26XX_Object             *object;
    uint16_t                        *sampleBuffer;
    uint8_t                          channel;

    /* Get the pointer to the object */
    object = ((ADCBuf_Handle)arg0)->object;

    DebugP_log0("ADC: swi interrupt context start");

    /* Disable interrupts */
    key = HwiP_disable();

    /* Use a temporary pointers in case the callback function
     * attempts to perform another ADCBuf_transfer call
     */
    conversion   = object->currentConversion;
    sampleBuffer = object->activeSampleBuffer;
    channel      = object->currentChannel;

    if (object->recurrenceMode == ADCBuf_RECURRENCE_MODE_ONE_SHOT) {
        /* Clean up ADC and DMA */
        ADCBufCC26XX_cleanADC(((ADCBuf_Handle)arg0));
        /* Indicate we are done with this transfer */
        object->currentConversion = NULL;
    }

    /* Restore interrupts */
    HwiP_restore(key);

    /* Perform callback */
    object->callbackFxn((ADCBuf_Handle)arg0, conversion, sampleBuffer, channel, ADCBuf_STATUS_SUCCESS);

    DebugP_log0("ADC: swi interrupt context end");
}

/*!
 *  @brief  CC26XX internal callback function that posts the semaphore in blocking mode
 *
 *  @param  handle      An ADCBufCC26XX_Handle
 *
 *  @param  conversion  A pointer to the current ADCBuf_Conversion
 *
 */
static void ADCBufCC26XX_conversionCallback(ADCBuf_Handle handle,
                                            ADCBuf_Conversion *conversion,
                                            void *completedADCBuffer,
                                            uint32_t completedChannel,
                                            int_fast16_t status)
{
    ADCBufCC26XX_Object        *object;

    DebugP_log0("ADC DMA: posting conversionComplete semaphore");

    /* Get the pointer to the object */
    object = handle->object;

    /* Post the semaphore */
    SemaphoreP_post(&(object->conversionComplete));
}

/*
 * ======== ADCBufCC26XX_convert ========
 */
int_fast16_t ADCBufCC26XX_convert(ADCBuf_Handle handle,
                                  ADCBuf_Conversion conversions[],
                                  uint_fast8_t channelCount)
{
    uint32_t                        key;
    ADCBufCC26XX_Object             *object;
    ADCBufCC26XX_HWAttrs const      *hwAttrs;
    PIN_Config                      adcPinTable[2];
    uint8_t i = 0;

    DebugP_assert(handle);

    /* Get the pointer to the object */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    DebugP_assert(channelCount == 1);
    DebugP_assert((conversions->samplesRequestedCount <= UDMA_XFER_SIZE_MAX));
    DebugP_assert(conversions->sampleBuffer);
    DebugP_assert(!(object->recurrenceMode == (ADCBuf_RECURRENCE_MODE_CONTINUOUS
            && !(conversions->sampleBufferTwo))));

    /* Disable interrupts */
    key = HwiP_disable();

    /* Check if ADC is open and that no other transfer is in progress */
    if (!(object->isOpen) || object->conversionInProgress) {
        /* Restore interrupts */
        HwiP_restore(key);
        DebugP_log0("ADCBuf: conversion failed");
        return ADCBuf_STATUS_ERROR;
    }
    object->conversionInProgress = true;

    /* Restore interrupts*/
    HwiP_restore(key);

    /* Specify input in ADC module */
    AUXADCSelectInput(hwAttrs->adcChannelLut[conversions->adcChannel].compBInput);

    /* Add pin to measure on */
    adcPinTable[i++] = (hwAttrs->adcChannelLut[conversions->adcChannel].dio) |
                        PIN_NOPULL |
                        PIN_INPUT_DIS |
                        PIN_GPIO_OUTPUT_DIS |
                        PIN_IRQ_DIS |
                        PIN_DRVSTR_MIN;

    /* Terminate pin list */
    adcPinTable[i] = PIN_TERMINATE;
    object->pinHandle = PIN_open(&object->pinState, adcPinTable);
    if (!object->pinHandle) {
        object->conversionInProgress = false;
        return ADCBuf_STATUS_ERROR;
    }

    /* Save which channel we are converting on for the callbackFxn */
    object->currentChannel = conversions->adcChannel;

    /* Try to acquire the ADC semaphore if we do not already have it. */
    if (object->adcSemaphoreInPossession == false) {
         if (!AUXSMPHTryAcquire(AUX_SMPH_2)) {
            PIN_close(object->pinHandle);
            object->conversionInProgress = false;
            DebugP_log0("ADCBuf: failed to acquire semaphore");
            return ADCBuf_STATUS_ERROR;
        }
        object->adcSemaphoreInPossession = true;
    }

    /* Store location of the current conversion */
    object->currentConversion = conversions;

    /* Configure and arm the DMA and AUX DMA control */
    ADCBufCC26XX_configDMA(handle, conversions);

    /* Configure and arm the GPT DMA channel to clear the level-based GPT IRQ signal */
    ADCBufCC26XX_configGPTDMA(handle, conversions);

    /* Flush the ADC FIFO in case we have triggered prior to this call */
    AUXADCFlushFifo();

    /* If input scaling is set to disabled in the params, disable it */
    if (!object->inputScalingEnabled) {
        AUXADCDisableInputScaling();
    }

    /* Arm the ADC in preparation for incoming conversion triggers */
    if (object->samplingMode == ADCBufCC26XX_SAMPING_MODE_SYNCHRONOUS) {
        /* ADCBufCC26XX_SYNCHRONOUS sampling mode */
        AUXADCEnableSync(object->refSource, object->samplingDuration,
                         AUXADC_TRIGGER_GPT0A);
    }
    else {
        /* ADCBufCC26XX_ASYNCHRONOUS sampling mode */
        AUXADCEnableAsync(object->refSource, AUXADC_TRIGGER_GPT0A);
    }

    /* Start the GPTimer to create ADC triggers */
    GPTimerCC26XX_start(object->timerHandle);

    /* Set constraints to guarantee operation */
    Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);

    if (object->returnMode == ADCBuf_RETURN_MODE_BLOCKING) {
        DebugP_log0("ADCBuf: transfer pending on conversionComplete "
                                "semaphore");

        if (SemaphoreP_OK != SemaphoreP_pend(&(object->conversionComplete),
                    object->semaphoreTimeout)) {
            /* Cancel the transfer if we experience a timeout */
            ADCBufCC26XX_convertCancel(handle);
            /*
             * ADCBufCC26XX_convertCancel performs a callback which posts a
             * conversionComplete semaphore. This call consumes this extra post.
             */
            SemaphoreP_pend(&(object->conversionComplete), SemaphoreP_NO_WAIT);
            return ADCBuf_STATUS_ERROR;
        }
    }


    return ADCBuf_STATUS_SUCCESS;
}


/*
 * ======== ADCBufCC26XX_convertCancel ========
 */
int_fast16_t ADCBufCC26XX_convertCancel(ADCBuf_Handle handle) {
    ADCBufCC26XX_Object            *object;
    ADCBuf_Conversion              *conversion;

    DebugP_assert(handle);

    /* Get the pointer to the object and hwAttrs*/
    object = handle->object;

    /* Check if ADC is open and that no other transfer is in progress */
    if (!(object->conversionInProgress)) {
        DebugP_log0("ADCBuf: a conversion must be in progress to cancel one");
        return ADCBuf_STATUS_ERROR;
    }

    /* Stop triggering a conversion on trigger events */
    AUXADCDisable();

    /* Set hardware and software configuration to default and turn off driver */
    ADCBufCC26XX_cleanADC(handle);

    /* Use a temporary transaction pointer in case the callback function
     * attempts to perform another ADCBuf_convert call
     */
    conversion = object->currentConversion;

    /* Perform callback if we are in one-shot mode. In continuous mode,
     * ADCBuf_convertCancel will probably be called from the callback function
     * itself. No need to call it again. */
    if (object->recurrenceMode == ADCBuf_RECURRENCE_MODE_ONE_SHOT) {
        object->callbackFxn(handle, conversion, conversion->sampleBuffer,
                            object->currentChannel, ADCBuf_STATUS_SUCCESS);
    }

    return ADCBuf_STATUS_SUCCESS;
}


/*
 * ======== ADCBufCC26XX_close ========
 */
void ADCBufCC26XX_close(ADCBuf_Handle handle) {
    ADCBufCC26XX_Object         *object;

    DebugP_assert(handle);

    /* Get the pointer to the object */
    object = handle->object;

    /* Check if the ADC is running and abort conversion if necessary. */
    if (object->conversionInProgress) {
       ADCBuf_convertCancel(handle);
    }

    /* Get the pointer to the object */
    object = handle->object;

    /* Release the uDMA dependency and potentially power down uDMA. */
    UDMACC26XX_close(object->udmaHandle);

    /* Destroy the Hwi */
    HwiP_destruct(&(object->hwi));

    /* Destroy the Swi */
    SwiP_destruct(&(object->swi));

    /* Close the timer */
    GPTimerCC26XX_close(object->timerHandle);

    if (object->returnMode == ADCBuf_RETURN_MODE_BLOCKING) {
        SemaphoreP_destruct(&(object->conversionComplete));
    }

    if (object->adcSemaphoreInPossession) {
        ADCBufCC26XX_releaseADCSemaphore(handle);
    }

    /* Mark the module as available */
    object->isOpen = false;

    DebugP_log0("ADCBuf: closed");
}

/*
 * ======== ADCBufCC26XX_getResolution ========
 */
uint_fast8_t ADCBufCC26XX_getResolution(ADCBuf_Handle handle) {
    return (ADCBufCC26XX_RESOLUTION);
}


/*
 * ======== ADCBufCC26XX_adjustRawValues ========
 */
int_fast16_t ADCBufCC26XX_adjustRawValues(ADCBuf_Handle handle,
                                          void *sampleBuffer,
                                          uint_fast16_t sampleCount,
                                          uint32_t adcChannel)
{
    ADCBufCC26XX_Object *object;
    uint32_t            gain;
    uint32_t            offset;
    uint16_t            i;

    object = handle->object;

    gain = AUXADCGetAdjustmentGain(object->refSource);
    offset = AUXADCGetAdjustmentOffset(object->refSource);

    for (i = 0; i < sampleCount; i++) {
        uint16_t tmpRawADCVal = ((uint16_t *)sampleBuffer)[i];
        ((uint16_t *) sampleBuffer)[i] = AUXADCAdjustValueForGainAndOffset(
                tmpRawADCVal, gain, offset);
    }

    return ADCBuf_STATUS_SUCCESS;
}

/*
 * ======== ADCBufCC26XX_convertAdjustedToMicroVolts ========
 */
int_fast16_t ADCBufCC26XX_convertAdjustedToMicroVolts(
        ADCBuf_Handle handle, uint32_t adcChannel, void *adjustedSampleBuffer,
        uint32_t outputMicroVoltBuffer[], uint_fast16_t sampleCount)
{
    ADCBufCC26XX_Object *object;
    uint16_t            i;
    uint32_t            voltageRef;

    object = handle->object;

    voltageRef =
            (object->inputScalingEnabled) ?
                    AUXADC_FIXED_REF_VOLTAGE_NORMAL :
                    AUXADC_FIXED_REF_VOLTAGE_UNSCALED;

    for (i = 0; i < sampleCount; i++) {
        outputMicroVoltBuffer[i] = AUXADCValueToMicrovolts(
                voltageRef, ((uint16_t *) adjustedSampleBuffer)[i]);
    }

    return ADCBuf_STATUS_SUCCESS;
}

/*!
 *  @brief  Function to configure the DMA to automatically transfer ADC output
 *          data into a provided array
 *
 *  @pre    ADCBufCC26XX_open() has to be called first.
 *
 *  @pre    There must not currently be a conversion in progress
 *
 *  @param  handle An ADCBufCC26XX handle returned from ADCBufCC26XX_open()
 *
 *  @param  conversion A pointer to an ADCBuf_Conversion
 *
 */
static void ADCBufCC26XX_configDMA(ADCBuf_Handle handle,
                                   ADCBuf_Conversion *conversion)
{
    ADCBufCC26XX_Object             *object;

    /* Get the pointer to the object */
    object = handle->object;

    /* Set configure control table entry */
    ADCBufCC26XX_loadDMAControlTableEntry(handle, conversion, true);

    /* If we are operating in continous mode, load the alternate DMA control
     * table data structure */
    if (object->recurrenceMode == ADCBuf_RECURRENCE_MODE_CONTINUOUS) {
        ADCBufCC26XX_loadDMAControlTableEntry(handle, conversion, false);
    }

    /* Enable the channels */
    UDMACC26XX_channelEnable(object->udmaHandle, 1 << UDMA_CHAN_AUX_ADC);

    /* Configure DMA settings in AUX_EVCTL */
    HWREG(AUX_EVCTL_BASE + AUX_EVCTL_O_DMACTL) =
            AUX_EVCTL_DMACTL_REQ_MODE_SINGLE | AUX_EVCTL_DMACTL_EN
                    | AUX_EVCTL_DMACTL_SEL_FIFO_NOT_EMPTY;

    DebugP_log0("ADCBuf: DMA transfer enabled");
}

/*!
 *  @brief  Function to configure the adc DMA control table entry for basic or
 *          ping pong mode
 *
 *  @pre    ADCBufCC26XX_open() has to be called first.
 *
 *  @pre    There must not currently be a conversion in progress
 *
 *  @param  handle An ADCBufCC26XX handle returned from ADCBufCC26XX_open()
 *
 *  @param  conversion A pointer to an ADCBuf_Conversion
 *
 *  @param  primaryEntry Is this supposed to modify the primary or the alternate
 *          control table entry
 *
 */
static void ADCBufCC26XX_loadDMAControlTableEntry(ADCBuf_Handle handle,
                                                  ADCBuf_Conversion *conversion,
                                                  bool primaryEntry)
{
    ADCBufCC26XX_Object             *object;
    volatile tDMAControlTable       *dmaControlTableEntry;
    uint32_t                        numberOfBytes;

    /* Get the pointer to the object*/
    object = handle->object;

    /* Calculate the number of bytes for the transfer */
    numberOfBytes = (uint16_t) (conversion->samplesRequestedCount)
            * ADCBufCC26XX_BYTES_PER_SAMPLE;

    /* Set configure control table entry */
    dmaControlTableEntry =
            primaryEntry ?
                    &dmaADCPriControlTableEntry : &dmaADCAltControlTableEntry;
    dmaControlTableEntry->ui32Control = (
            (object->recurrenceMode == ADCBuf_RECURRENCE_MODE_ONE_SHOT) ?
                    UDMA_MODE_BASIC : UDMA_MODE_PINGPONG) |
    UDMA_SIZE_16 |
    UDMA_SRC_INC_NONE |
    UDMA_DST_INC_16 |
    UDMA_ARB_1
            | UDMACC26XX_SET_TRANSFER_SIZE(
                    (uint16_t )conversion->samplesRequestedCount);
    dmaControlTableEntry->pvDstEndAddr = (void *) ((uint32_t) (
            primaryEntry ?
                    conversion->sampleBuffer : conversion->sampleBufferTwo)
            + numberOfBytes - 1);
    dmaControlTableEntry->pvSrcEndAddr = (void *) (AUX_ANAIF_BASE
            + AUX_ANAIF_O_ADCFIFO);
}

/*!
 *  @brief  Function to configure the DMA to automatically clear the GPT_IRQ
 *          line that the ADC triggers off of without using an interrupt handler
 *
 *  @pre    ADCBufCC26XX_open() has to be called first.
 *
 *  @pre    There must not currently be a conversion in progress
 *
 *  @param  handle An ADCBufCC26XX handle returned from ADCBufCC26XX_open()
 *
 *  @param  conversion A pointer to an ADCBuf_Conversion
 *
 */
static void ADCBufCC26XX_configGPTDMA(ADCBuf_Handle handle,
                                      ADCBuf_Conversion *conversion)
{
    ADCBufCC26XX_Object             *object;

    /* Get the pointer to the object */
    object = handle->object;

    /* Set configure control table entry */
    ADCBufCC26XX_loadGPTDMAControlTableEntry(handle, conversion, true);

    /* If we are operating in continous mode, load the alternate DMA control
     * table data structure */
    if (object->recurrenceMode == ADCBuf_RECURRENCE_MODE_CONTINUOUS) {
        ADCBufCC26XX_loadGPTDMAControlTableEntry(handle, conversion, false);
    }

    /* Enable the channels */
    UDMACC26XX_channelEnable(object->udmaHandle, 1 << UDMA_CHAN_TIMER0_A);

    /* Enable event signal */
    HWREG(object->timerHandle->hwAttrs->baseAddr + GPT_O_DMAEV) = GPT_DMAEV_TATODMAEN;

}

/*!
 *  @brief  Function to configure the GPT DMA control table entry for basic or
 *          ping pong mode
 *
 *  @pre    ADCBufCC26XX_open() has to be called first.
 *
 *  @pre    There must not currently be a conversion in progress
 *
 *  @param  handle An ADCBufCC26XX handle returned from ADCBufCC26XX_open()
 *
 *  @param  conversion A pointer to an ADCBuf_Conversion
 *
 *  @param  primaryEntry Is this supposed to modify the primary or the alternate
 *          control table entry
 *
 */
static void ADCBufCC26XX_loadGPTDMAControlTableEntry(
        ADCBuf_Handle handle, ADCBuf_Conversion *conversion, bool primaryEntry)
{
    ADCBufCC26XX_Object             *object;
    volatile tDMAControlTable       *dmaControlTableEntry;

    /* Get the pointer to the object */
    object = handle->object;

    /* Set configure control table entry */
    dmaControlTableEntry =
            primaryEntry ?
                    &dmaGPT0APriControlTableEntry :
                    &dmaGPT0AAltControlTableEntry;
    dmaControlTableEntry->ui32Control = (
            (object->recurrenceMode == ADCBuf_RECURRENCE_MODE_ONE_SHOT) ?
                    UDMA_MODE_BASIC : UDMA_MODE_PINGPONG) |
    UDMA_SIZE_8 |
    UDMA_SRC_INC_NONE |
    UDMA_DST_INC_NONE |
    UDMA_ARB_1
            | UDMACC26XX_SET_TRANSFER_SIZE(
                    (uint16_t )conversion->samplesRequestedCount);
    dmaControlTableEntry->pvDstEndAddr =
            (void *) ((uint32_t) (object->timerHandle->hwAttrs->baseAddr
                    + GPT_O_ICLR));
    dmaControlTableEntry->pvSrcEndAddr = (void *)(&gptClear);
}

/*!
 *  @brief  Function to undo all configurations done by the ADC driver
 *
 *  @pre    ADCBuf_open() has to be called first.
 *
 *  @pre    ADCBuf_convert() has to be called first.
 *
 *  @param  handle An ADCBufCC26XX handle returned from ADCBufCC26XX_open()
 *
 */
static void ADCBufCC26XX_cleanADC(ADCBuf_Handle handle) {
    ADCBufCC26XX_Object            *object;

    /* Get the pointer to the object */
    object = handle->object;

    /* Stop the timer to stop generating triggers */
    GPTimerCC26XX_stop(object->timerHandle);

    /* Set constraints to guarantee operation */
    Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);

    if (object->adcSemaphoreInPossession && !object->keepADCSemaphore) {
        /* Release the ADC semaphore */
        AUXSMPHRelease(AUX_SMPH_2);
        object->adcSemaphoreInPossession = false;
    }

    /* Disable the UDMA channels */
    UDMACC26XX_channelDisable(
            object->udmaHandle,
            (1 << UDMA_CHAN_AUX_ADC) | (1 << UDMA_CHAN_TIMER0_A));

    /* Deallocate pins */
    PIN_close(object->pinHandle);

    /* Disable UDMA mode for ADC */
    HWREG(AUX_EVCTL_BASE + AUX_EVCTL_O_DMACTL) =
            AUX_EVCTL_DMACTL_REQ_MODE_SINGLE
                    | AUX_EVCTL_DMACTL_SEL_FIFO_NOT_EMPTY;

    /* Clear any remaining GPT_IRQ flags */
    HWREG(object->timerHandle->hwAttrs->baseAddr + GPT_O_ICLR) =
            GPT_ICLR_DMAAINT + GPT_ICLR_TATOCINT;

    /* Note that the driver is no longer converting */
    object->conversionInProgress = false;
}

/* Return period in timer counts */
static uint32_t ADCBufCC26XX_freqToCounts(uint32_t frequency)
{
    ClockP_FreqHz freq;
    ClockP_getCpuFreq(&freq);

    uint32_t periodCounts = (freq.lo / frequency) - 1;

    return periodCounts;
}

/*!
 *  @brief  Function to acquire the semaphore that arbitrates access to the ADC
 *          between the CM3 and the sensor controller
 *
 *  @pre    ADCBufCC26XX_open() has to be called first.
 *
 *  @pre    There must not currently be a conversion in progress
 *
 *  @param  handle An ADCBufCC26XX handle returned from ADCBufCC26XX_open()
 *
 */
static bool ADCBufCC26XX_acquireADCSemaphore(ADCBuf_Handle handle) {
    uint32_t                    key;
    bool                        semaphoreAvailable;
    ADCBufCC26XX_Object         *object;

    object = handle->object;

    /* Set semaphoreAvailable false at default */
    semaphoreAvailable = false;

    /* Disable interrupts */
    key = HwiP_disable();

    /* Check if ADC is closed or a conversion is in progress */
    if (!(object->isOpen) || object->conversionInProgress) {
        DebugP_log0("ADC: driver must be open and no conversion must be in "
                "progress to disable input scaling");
    }
    /* This is a non-blocking call to acquire the ADC semaphore. */
    else if (AUXSMPHTryAcquire(AUX_SMPH_2)) {
        object->adcSemaphoreInPossession = true;
        semaphoreAvailable = true;
    }

    /* Restore interrupts */
    HwiP_restore(key);

    return semaphoreAvailable;
}

/*!
 *  @brief  This function releases the ADC semaphore
 *
 *  @pre    ADCBufCC26XX_open() has to be called first.
 *
 *  @pre    There must not currently be a conversion in progress
 *
 *  @param  handle An ADCBufCC26XX handle returned from ADCBufCC26XX_open()
 *
 */
static bool ADCBufCC26XX_releaseADCSemaphore(ADCBuf_Handle handle) {
    uint32_t                key;
    bool                    semaphoreReleased;
    ADCBufCC26XX_Object     *object;

    object= handle->object;

    /* Set semaphoreReleased true at default */
    semaphoreReleased = true;

    /* Disable interrupts */
    key = HwiP_disable();

    /* Check if ADC is closed or a conversion is in progress */
    if (!(object->isOpen) || object->conversionInProgress) {
        DebugP_log0("ADC: driver must be open and no conversion must be in "
                "progress to disable input scaling");
        semaphoreReleased = false;
    }
    else {
        /* Release the ADC semaphore */
        AUXSMPHRelease(AUX_SMPH_2);
        object->adcSemaphoreInPossession = false;
    }

    /* Restore interrupts */
    HwiP_restore(key);

    return semaphoreReleased;
}

/*
 * ======== ADCBufCC26XX_control ========
 */
int_fast16_t ADCBufCC26XX_control(ADCBuf_Handle handle, uint_fast16_t cmd,
                                  void * arg)
{
    ADCBufCC26XX_Object *object = handle->object;
    int status = ADCBuf_STATUS_ERROR;

    DebugP_assert(handle);

    switch (cmd) {
        case ADCBufCC26XX_CMD_ACQUIRE_ADC_SEMAPHORE:
            if (ADCBufCC26XX_acquireADCSemaphore(handle)) {
                status = ADCBuf_STATUS_SUCCESS;
            }
            break;
        case ADCBufCC26XX_CMD_KEEP_ADC_SEMAPHORE:
            object->keepADCSemaphore = true;
            status = ADCBuf_STATUS_SUCCESS;
            break;
        case ADCBufCC26XX_CMD_KEEP_ADC_SEMAPHORE_DISABLE:
            object->keepADCSemaphore = false;
            status = ADCBuf_STATUS_SUCCESS;
            break;
        case ADCBufCC26XX_CMD_RELEASE_ADC_SEMAPHORE:
            if (ADCBufCC26XX_releaseADCSemaphore(handle)) {
                status = ADCBuf_STATUS_SUCCESS;
            }
            break;
        default:
            status = ADCBuf_STATUS_UNDEFINEDCMD;
            break;
    }
    return status;
}
