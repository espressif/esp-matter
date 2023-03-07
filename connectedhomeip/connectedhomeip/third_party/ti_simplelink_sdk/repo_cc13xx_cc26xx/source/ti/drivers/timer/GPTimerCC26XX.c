/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
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
/*!*****************************************************************************
 *  @file       GPTimerCC26XX.c
 *  @brief      CC26XX/CC13XX driver implementation for GPTimer peripheral
 *
 *
 *******************************************************************************
 */


#include <ti/drivers/dpl/DebugP.h>
#include <ti/drivers/dpl/HwiP.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

#include <ti/drivers/timer/GPTimerCC26XX.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_gpt.h)
#include DeviceFamily_constructPath(driverlib/timer.h)


/* GPTimer configuration array from application */
extern const GPTimerCC26XX_Config GPTimerCC26XX_config[];

/* Register masks used by GPTimerCC26XX_resetHw */
#define GPT_CTL_MASK    (GPT_CTL_TASTALL_M | GPT_CTL_TAEVENT_M | GPT_CTL_TAPWML_M)


/* GPTimerCC26XX internal functions */
static void GPTimerCC26XX_initHw(GPTimerCC26XX_Handle handle, const GPTimerCC26XX_Params *params);
static void GPTimerCC26XX_resetHw(GPTimerCC26XX_Handle handle);
static void GPTimerCC26XXThreadsafeConstraintClr(GPTimerCC26XX_Handle handle);
static void GPTimerCC26XXThreadsafeConstraintSet(GPTimerCC26XX_Handle handle);
static void GPTimerCC26XXHwiFxn(uintptr_t a0);
static void GPTimerCC26XXSetLoadMatch(GPTimerCC26XX_Handle handle, GPTimerCC26XX_Value loadMatchVal, uint32_t regPre, uint32_t regLoadMatch);
static GPTimerCC26XX_Value GPTimerCC26XX_getTimerValue(GPTimerCC26XX_Handle handle, uint32_t reg);

/* GPTimerCC26XX temporary internal functions.
   Will be removed once they are added to driverlib
 */
static inline void TimerSetConfig(uint32_t ui32Base, uint32_t ui32Config);
static inline void TimerSetMode(uint32_t ui32Base, uint32_t timer, uint32_t mode);


/* Lookup table definition for interfacing driverlib and register fields.
   Used to simplify code and to easily look up register fields as several fields
   are not symmetric across timer A and timer B registers (interrupts & dma)
 */
typedef struct GPTimerCC26XX_LUT
{
    uint16_t map;                      /* Timer argument in driverlib (TIMER_A / TIMER_B) */
    uint16_t shift;                    /* Bit shift for registers shared between GPT_A / GPT_B */
    uint16_t offset;                   /* Byte offset for registers sequentially in memory map for GPT_A/GPT_B */
    uint16_t interrupts[GPT_NUM_INTS]; /* Interrupt bitfields for GPTA/B. Order must match GPTimerCC26XX_Interrupt */
} const GPTimerCC26XX_LUT;

/* Lookup table definition for interfacing driverlib and register fields. */
static const GPTimerCC26XX_LUT GPT_LUT[GPT_PARTS_COUNT] =
{
    {
        .map        = TIMER_A,
        .shift      = 0,
        .offset     = 0,
        .interrupts ={ GPT_MIS_TATOMIS,                   GPT_MIS_CAMMIS, GPT_MIS_CAEMIS, GPT_MIS_TAMMIS },
    },
    {
        .map        = TIMER_B,
        .shift      = 8,
        .offset     = 4,
        .interrupts ={ GPT_MIS_TBTOMIS,                   GPT_MIS_CBMMIS, GPT_MIS_CBEMIS, GPT_MIS_TBMMIS },
    },
};

/* Default GPTimer parameters */
static const GPTimerCC26XX_Params GPT_DefaultParams =
{
    .width          = GPT_CONFIG_32BIT,
    .mode           = GPT_MODE_PERIODIC,
    .matchTiming    = GPTimerCC26XX_SET_MATCH_NEXT_CLOCK,
    .direction      = GPTimerCC26XX_DIRECTION_UP,
    .debugStallMode = GPTimerCC26XX_DEBUG_STALL_OFF,
};


/*!
 *  @brief Generic bit vector to lookup table vector implementation
 *
 *  Parses generic lookup table and checks whether inputs exists in lookup table.
 *
 *  @param pLookup      pointer to look-up table
 *  @param inputVector  Input vector
 *  @param length       Number of bits in lookup table
 *
 *  @return A bit vector containing set fields in lookup table
 */
static uint16_t GPTimerCC26XXLookupMask(const uint16_t *pLookup, uint16_t inputVector, uint8_t length)
{
    uint16_t maskLookup = 0;
    uint32_t i;
    /* Fetch data from lookup table */
    for (i = 0; i < length; i++)
    {
        /* Check if current index of lookup table is in input vector */
        if (inputVector & pLookup[i])
        {
            /* Add vector input to looked up mask. */
            maskLookup |= 1 << i;
        }
    }
    return maskLookup;
}

/*!
 *  @brief Generic lookup table to bit vector implementation
 *
 *  Parses input vector and checks which input fields are set
 *
 *  @param pLookup      pointer to look-up table
 *  @param inputVector  Input vector
 *  @param length       Number of bits in lookup table
 *
 *  @return A bit vector mapped from lookup table
 */
static uint16_t GPTimerCC26XXReverseLookupMask(const uint16_t *pLookup, uint16_t inputVector, uint8_t length)
{
    uint16_t revMaskLookup = 0;
    uint32_t i;
    for (i = 0; i < length; i++)
    {
        /* Check if current index is set in input vector */
        if (inputVector & (1 << i))
        {
            /* Add data from lookup table */
            revMaskLookup |= pLookup[i];
        }
    }
    return revMaskLookup;
}

/*!
 *  @brief  Function to initialize the GPTimerCC26XX_Params struct to default
 *          values
 *
 *  @param  params      An pointer to GPTimerCC26XX_Params structure for
 *                      initialization
 *
 *  Defaults values are:
 *  Timer width : 32 bits,
 *  Timer mode  : Periodic mode counting upwards
 *  Timer debug stall mode: Disabled
 */
void GPTimerCC26XX_Params_init(GPTimerCC26XX_Params *params)
{
    *params = GPT_DefaultParams;
}

/*!
 *  @brief  This function opens the given GPTimer peripheral. It will set a
 *          dependency on the timer, configure the timer mode and set timer
 *          as open. If params is NULL default values will be used.
 *
 *  @return A GPTimerCC26XX_Handle on success or NULL on error or if timer is
 *          already open. If NULL is returned further GPTimer API calls will
 *          result in undefined behaviour.
 *
 *  @sa     GPTimerCC26XX_close()
 */
GPTimerCC26XX_Handle GPTimerCC26XX_open(unsigned int index, const GPTimerCC26XX_Params *params)
{
    unsigned int         key;
    GPTimerCC26XX_Handle handle = (GPTimerCC26XX_Handle) & GPTimerCC26XX_config[index];

    /* Get the pointer to the object and hwAttrs and timer*/
    GPTimerCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;
    GPTimerCC26XX_Object        *object  = handle->object;


    /*
     *  Input argument checks
     */

    /* Fail if no params supplied */
    if (params == NULL)
    {
        DebugP_log1("Timer:(%p): No params supplied, using default.", hwAttrs->baseAddr);
        params = &GPT_DefaultParams;
    }

    if (params->width == GPT_CONFIG_32BIT)
    {
        /* Fail if invalid combination of mode and configuration
           32-bit config  are only valid for periodic / oneshot modes */
        if (params->mode != GPT_MODE_ONESHOT_UP &&
            params->mode != GPT_MODE_PERIODIC_UP)
        {
            DebugP_log1("Timer:(%p): Invalid combination of mode and configuration", hwAttrs->baseAddr);
            return(NULL);
        }
        /* Fail if invalid combination of timer unit and configuration.
           32-bit config is only valid in combination with GPT A */
        if (handle->timerPart != GPT_A)
        {
            DebugP_log1("Timer:(%p): Invalid combination of configuration and timer unit", hwAttrs->baseAddr);
            return(NULL);
        }
    }

    /*
     * Check if Timer is already opened
     */

    /* Disable preemption while checking */
    key = HwiP_disable();

    bool isOpen;
    /* If trying to open a 32-bit mode timer then both A and B must be available */
    if (params->width == GPT_CONFIG_32BIT)
    {
        /* Timer already opened if one unit opened */
        isOpen = object->isOpen[GPT_A] | object->isOpen[GPT_B];
    }
    else
    {
        isOpen = object->isOpen[handle->timerPart];
    }
    /* Fail if corresponding timer is already open or if one of the two
       units needed is taken for 32-bit mode */
    if (isOpen)
    {
        HwiP_restore(key);
        DebugP_log2("Timer:(%p), Unit: (%u): Already in use.", hwAttrs->baseAddr, handle->timerPart);
        return NULL;
    }

    /*
     * Open timer
     */
    if (params->width == GPT_CONFIG_32BIT)
    {
        object->isOpen[GPT_A] = true;
        object->isOpen[GPT_B] = true;
    }
    else
    {
        object->isOpen[handle->timerPart] = true;
    }
    /* Restore preemption again */
    HwiP_restore(key);

    /*  Configure timer object */
    object->width = params->width;

    /* Register power dependency - i.e. power up and enable clock for GPTimer.
       If both GPT_A and GPT_B is used then two dependencies will be set on GPTimer */
    Power_setDependency(hwAttrs->powerMngrId);

    /* Initialize HW */
    GPTimerCC26XX_initHw(handle, params);

    return handle;
}

/*!
 *  @brief  This function closes an opened GPTimer peripheral defined by the
 *          handle. It will remove the dependency on the timer and write all
 *          configured timer registers to its default values. Timer must be
 *          stopped beforing closing it.
 *
 *  @sa     GPTimerCC26XX_stop()
 *  @sa     GPTimerCC26XX_close()
 */
void GPTimerCC26XX_close(GPTimerCC26XX_Handle handle)
{
    /* Get the pointer to the object and hwAttrs */
    GPTimerCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;
    GPTimerCC26XX_Object        *object  = handle->object;

    /* Stop and reset timer */
    GPTimerCC26XX_resetHw(handle);

    /* Release dependency for timer */
    Power_releaseDependency(hwAttrs->powerMngrId);

    /* Mark the Timer unit as available */
    uint32_t key = HwiP_disable();

    /* Close Timer(s) */
    if (object->width == GPT_CONFIG_32BIT)
    {
        object->isOpen[GPT_A] = false;
        object->isOpen[GPT_B] = false;
    }
    else
    {
        object->isOpen[handle->timerPart] = false;
    }
    HwiP_restore(key);
}

/*!
 *  @brief  This function will start a GPTimer defined by the handle.
 *  @sa     GPTimerCC26XX_stop()
 */
void GPTimerCC26XX_start(GPTimerCC26XX_Handle handle)
{
    /* Get the pointer to the object and hwAttrs */
    GPTimerCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;

    /* Enable timer */
    uint32_t timer = GPT_LUT[handle->timerPart].map;
    TimerEnable(hwAttrs->baseAddr, timer);

    /* Set constraint to disallow standby while running */
    GPTimerCC26XXThreadsafeConstraintSet(handle);
}

/*!
 *  @brief  This function will stop a running GPTimer defined by the handle
 *  @sa     GPTimerCC26XX_start()
 */
void GPTimerCC26XX_stop(GPTimerCC26XX_Handle handle)
{
    /* Get the pointer to the object and hwAttrs */
    GPTimerCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;

    /* Disable timer */
    uint32_t timer = GPT_LUT[handle->timerPart].map;
    TimerDisable(hwAttrs->baseAddr, timer);

    /* Clear constraint to allow standby again */
    GPTimerCC26XXThreadsafeConstraintClr(handle);
}

/*!
 *  @brief  Shared code to be used by GPTimerCC26XX_setLoadValue / GPTimerCC26XX_setMatchValue
 *          Sets load/match values using input value and register offset for
 *          prescaler and load/match register
 *          Functions calling this should specifiy which the register offset
 *          within the module base to the corresponding timer A register.
 */
static void GPTimerCC26XXSetLoadMatch(GPTimerCC26XX_Handle handle, GPTimerCC26XX_Value loadMatchVal, uint32_t regPre, uint32_t regLoadMatch)
{
    /* Get the pointer to the object and hwAttrs */
    GPTimerCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;
    GPTimerCC26XX_Object        *object  = handle->object;
    uint32_t offset = GPT_LUT[handle->timerPart].offset;

    /* Split value into correct timer and prescaler register for 16 bit modes. */
    if (object->width == GPT_CONFIG_16BIT)
    {
        /* Upper byte is used by prescaler */
        uint8_t prescaleValue = 0xFF & (loadMatchVal >> 16);
        /* Discard upper byte (24 bits max) */
        loadMatchVal &= 0xFFFF;

        /* Set prescale value */
        HWREG(hwAttrs->baseAddr + offset + regPre) = prescaleValue;
    }

    /* Set load / match value */
    HWREG(hwAttrs->baseAddr + offset + regLoadMatch) = loadMatchVal;
}

/*!
 *  @brief  Set GPTimer load value. For 32-bit configuration all 32 bits can
 *          be used. For split mode / 16-bit mode maximum value is 24 bits.
 *          Function concatenates prescaler functionality automatically
 */
void GPTimerCC26XX_setLoadValue(GPTimerCC26XX_Handle handle, GPTimerCC26XX_Value loadValue)
{
    GPTimerCC26XXSetLoadMatch(handle, loadValue, GPT_O_TAPR, GPT_O_TAILR);
}

/*!
 *  @brief  Set GPTimer match value. For 32-bit configuration all 32 bits can
 *          be used. For split mode / 16-bit mode maximum value is 24 bits.
 *          Function concatenates prescaler functionality automatically
 */
void GPTimerCC26XX_setMatchValue(GPTimerCC26XX_Handle handle, GPTimerCC26XX_Value matchValue)
{
    GPTimerCC26XXSetLoadMatch(handle, matchValue, GPT_O_TAPMR, GPT_O_TAMATCHR);
}


/*!
 *  @brief  Function to set which input event edge the GPTimer capture should
 *          use. Applies to edge-count and edge-time modes
 *          be called while GPTimer is running.
 */
void GPTimerCC26XX_setCaptureEdge(GPTimerCC26XX_Handle handle, GPTimerCC26XX_Edge event)
{
    GPTimerCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;

    uint32_t shift = GPT_LUT[handle->timerPart].shift;

    /* Disable interrupts during RMW operation */
    uint32_t key = HwiP_disable();

    uint32_t ctl = HWREG(hwAttrs->baseAddr + GPT_O_CTL);
    /* Clear old setting */
    ctl &= ~(GPT_CTL_TAEVENT_M << shift);
    /* Apply new setting */
    HWREG(hwAttrs->baseAddr + GPT_O_CTL) = ctl | (event << shift);
    /* Restore HW interrupts */
    HwiP_restore(key);
}

/*!
 *  @brief Shared code used to retrieve timer values by
 *         GPTimerCC26XX_getFreeRunValue and GPTimerCC26XX_getValue
 *         Functions calling this should specifiy which the register offset
 *         within the module base to the corresponding timer A register.
 */
static GPTimerCC26XX_Value GPTimerCC26XX_getTimerValue(GPTimerCC26XX_Handle handle, uint32_t reg)
{
    /* Get the pointer to the object and hwAttrs */
    GPTimerCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;
    GPTimerCC26XX_Object        *object  = handle->object;
    uint32_t offset = GPT_LUT[handle->timerPart].offset;

    uint32_t value = HWREG(hwAttrs->baseAddr + offset + reg);

    /* Correct for 16-bit mode (remove bits 31:24)
       Supported 16-bit modes uses prescaler as timer extension only.
       In 16-bit mode, current prescaler value is found in bits 23:16 of the timer value register
     */

    if (object->width == GPT_CONFIG_16BIT)
    {
        /* Discard any upper byte */
        value = value & 0xFFFFFF;
    }
    return (GPTimerCC26XX_Value)value;
}


/*!
 *  @brief  Retrieve current free-running value from GPTimer
 *          In 16-bit modes the function will return a 24-bit word where the
 *          8-bit prescaler value is included.
 */
GPTimerCC26XX_Value GPTimerCC26XX_getFreeRunValue(GPTimerCC26XX_Handle handle)
{
    return GPTimerCC26XX_getTimerValue(handle, GPT_O_TAV);
}

/*!
 *  @brief  Retrieve the current value of timer
 *          This returns the value of the timer in all modes except for
 *          input edge count and input edge time mode.
 *          In edge count mode, this register contains the number of edges that
 *          have occurred. In input edge time, this register contains the
 *          timer value at which the last edge event took place.
 *          In 16-bit modes the function will return a 24-bit word where the
 *          8-bit prescaler value is included.
 */
GPTimerCC26XX_Value GPTimerCC26XX_getValue(GPTimerCC26XX_Handle handle)
{
    return GPTimerCC26XX_getTimerValue(handle, GPT_O_TAR);
}

/*!
 *  @brief  Register interrupts for timer handle.
 *          This function must only be called once after opening a timer.
 *          Interrupts should be unregistered again before closing
 *          the timer resource.
 */
void GPTimerCC26XX_registerInterrupt(GPTimerCC26XX_Handle handle, GPTimerCC26XX_HwiFxn callback, GPTimerCC26XX_IntMask intMask)
{
    GPTimerCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;
    GPTimerCC26XX_Object        *object  = handle->object;

    /* Store callback function */
    object->hwiCallbackFxn[handle->timerPart] = callback;
    /* Construct RTOS HWI */
    HwiP_Struct *pHwi = &object->hwi[handle->timerPart];
    HwiP_Params  hp;
    HwiP_Params_init(&hp);
    hp.arg       = (uintptr_t)handle;
    hp.enableInt = true;
    hp.priority  = hwAttrs->intPriority;
    HwiP_construct(pHwi, hwAttrs->intNum, GPTimerCC26XXHwiFxn, &hp);

    GPTimerCC26XX_enableInterrupt(handle, intMask);
}

/*!
 *  @brief  Destruct interrupt for timer handle.
 *          This function must only be called once after opening a timer and
 *          should not be called before calling GPTimerCC26XX_registerInterrupt
 */
void GPTimerCC26XX_unregisterInterrupt(GPTimerCC26XX_Handle handle)
{
    GPTimerCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;
    GPTimerCC26XX_Object        *object  = handle->object;

    uint32_t ui32Base = hwAttrs->baseAddr;
    uint32_t timer    = GPT_LUT[handle->timerPart].map;

    /* Disable all timer unit interrupts, use "timer" variable as mask */
    TimerIntDisable(ui32Base, timer);

    /* Destroy callback function */
    object->hwiCallbackFxn[handle->timerPart] = NULL;
    /* Destruct HWI */
    HwiP_Struct *pHwi = &object->hwi[handle->timerPart];
    HwiP_destruct(pHwi);
}

/*!
 *  @brief  Enable interrupt source for current GPTimer unit. CPU interrupt
 *          for timer will not be enabled before calling
 *          GPTimerCC26XX_registerInterrupt
 */
void GPTimerCC26XX_enableInterrupt(GPTimerCC26XX_Handle handle, GPTimerCC26XX_IntMask intMask)
{
    GPTimerCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;
    uint32_t ui32Base = hwAttrs->baseAddr;

    /* Interrupt registers are shared by TA and TB but bit fields are not symmetric
       Fetch mask from lookup table.*/
    uint32_t intMaskLookup = GPTimerCC26XXReverseLookupMask(GPT_LUT[handle->timerPart].interrupts, intMask, GPT_NUM_INTS);

    /* Enable interrupts in timer unit */
    TimerIntEnable(ui32Base, intMaskLookup);
}

/*!
 *  @brief  Disable interrupt source for current GPTimer unit. CPU interrupt
 *          for timer will not be disabled before calling GPTimerCC26XX_unregisterInterrupt
 */
void GPTimerCC26XX_disableInterrupt(GPTimerCC26XX_Handle handle, GPTimerCC26XX_IntMask intMask)
{
    GPTimerCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;
    uint32_t ui32Base = hwAttrs->baseAddr;

    /* Interrupt registers are shared by TA and TB but bit fields are not symmetric
       Fetch mask from lookup table. */
    uint32_t intMaskLookup = GPTimerCC26XXReverseLookupMask(GPT_LUT[handle->timerPart].interrupts, intMask, GPT_NUM_INTS);

    /* Enable interrupts in timer unit */
    TimerIntDisable(ui32Base, intMaskLookup);
}

static void GPTimerCC26XX_initHw(GPTimerCC26XX_Handle handle, const GPTimerCC26XX_Params *params)
{
    /* Get the pointer to the object and hwAttrs */
    GPTimerCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;
    GPTimerCC26XX_Object const  *object  = handle->object;

    TimerSetConfig(hwAttrs->baseAddr, object->width);
    uint32_t timer = GPT_LUT[handle->timerPart].map;

    uint32_t mode = (uint32_t) params->mode;

    if (params->matchTiming == GPTimerCC26XX_SET_MATCH_ON_TIMEOUT)
    {
        /* Same bit position is also valid for timer B in the TBMR register. */
        mode |= GPT_TAMR_TAMRSU_TOUPDATE;
    }
    else
    {
        /* Same bit position is also valid for timer B in the TBMR register. */
        mode |= GPT_TAMR_TAMRSU_CYCLEUPDATE;
    }

    if (params->direction == GPTimerCC26XX_DIRECTION_UP)
    {
        /* Same bit position also valid for timer B in the TBMR register. */
        mode |= GPT_TAMR_TACDIR_UP;
    }
    else
    {
        /* Same bit position also valid for timer B in the TBMR register. */
        mode |= GPT_TAMR_TACDIR_DOWN;
    }

    TimerSetMode(hwAttrs->baseAddr, timer, mode);

    GPTimerCC26XX_configureDebugStall(handle, params->debugStallMode);
}

/*!
 *  @brief  Restore GPTimer unit registers back to reset values.
 *          Needed since module does not have reset functionality.
 */
static void GPTimerCC26XX_resetHw(GPTimerCC26XX_Handle handle)
{
    GPTimerCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;
    uint32_t ui32Base = hwAttrs->baseAddr;

    uint32_t timer = GPT_LUT[handle->timerPart].map;
    uint32_t regMask;
    /* Some registers are shared by TA and TB.
       Shift bit fields by 1 byte for timer B. */
    uint32_t shift = GPT_LUT[handle->timerPart].shift;
    /* Some registers are offset by one word (4 bytes) for Timer B. */
    uint32_t offset = GPT_LUT[handle->timerPart].offset;

    /* Disable timer before configuring */
    TimerDisable(ui32Base, timer);

    /* Reset control regs for timer N (CTL) */
    regMask = ~(GPT_CTL_MASK << shift);
    HWREG(ui32Base + GPT_O_CTL) &= regMask;

    /* Reset interrupt mask for Timer N (IMR).
       Equivalent to TimerIntDisable(ui32Base, regMask) */
    regMask = 0;
    uint8_t i;
    for (i = 0; i < GPT_NUM_INTS; i++)
    {
        regMask |= (uint32_t)(GPT_LUT[handle->timerPart].interrupts[i]);
    }
    HWREG(ui32Base + GPT_O_IMR) &= ~regMask;


    /* Clear interrupts for Timer N ( ICLR).  Same regMask as GPT_O_IMR.
       Equivalent to TimerIntClear(ui32Base, regMask) */
    HWREG(ui32Base + GPT_O_ICLR) = regMask;

    /* Reset load register for timer N.
       Equivalent to TimerLoadSet(ui32Base, timer, 0xFFFFFFF) */
    HWREG(ui32Base + offset + GPT_O_TAILR) = 0xFFFFFFFF;

    /* Reset match register for timer N.
       Equivalent to TimerMatchSet(ui32Base, timer, 0xFFFFFFFF) */
    HWREG(ui32Base + offset + GPT_O_TAMATCHR) = 0xFFFFFFFF;

    /* Reset pre-scale register for timer N.
       Equivalent to TimerPrescaleSet(ui32Base, timer, 0) */
    HWREG(ui32Base + offset + GPT_O_TAPR) = 0;

    /* Reset pre-scale match register for timer N.
       Equivalent to TimerPrescaleMatchSet(ui32Base, timer, 0) */
    HWREG(ui32Base + offset + GPT_O_TAPMR) = 0;
}


/*!
 *  @brief  GPTimer interrupt handler - Clears corresponding interrupt(s)
 *          and calls callback function with handle and bitmask argument
 *          as given in implementation header file.
 */
static void GPTimerCC26XXHwiFxn(uintptr_t a0)
{
    GPTimerCC26XX_Handle         handle  = (GPTimerCC26XX_Handle)a0;
    GPTimerCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;
    GPTimerCC26XX_Object        *object  = handle->object;

    GPTimerCC26XX_HwiFxn         callbackFxn = object->hwiCallbackFxn[handle->timerPart];
    uint32_t timer = GPT_LUT[handle->timerPart].map;

    /* Full width raw interrupt status */
    uint32_t interrupts = HWREG(hwAttrs->baseAddr + GPT_O_MIS);
    /* Interrupt mask to clear (byte 0 or 1) */
    uint32_t interruptClr = timer & interrupts;
    /* Clear interrupts */
    HWREG(hwAttrs->baseAddr + GPT_O_ICLR) = interruptClr;

    /* Interrupt registers are shared by TA and TB but bit fields are not
       symmetric. Need to go through LUT and fetch bit vector based on interrupts
     */
    uint16_t intMaskLookup = GPTimerCC26XXLookupMask(GPT_LUT[handle->timerPart].interrupts, interrupts, GPT_NUM_INTS);

    if (callbackFxn != NULL)
    {
        callbackFxn(handle, intMaskLookup);
    }
}

/*!
 *  @brief  Set Standby constraint while using timer to avoid TI RTOS
 *          going into standby when timer is running.  As constraints are
 *          counting, store constraint status and access atomically and only
 *          once per timer resource.
 */
static inline void GPTimerCC26XXThreadsafeConstraintSet(GPTimerCC26XX_Handle handle)
{
    GPTimerCC26XX_Object *object = handle->object;

    uint32_t key = HwiP_disable();
    /* Only set if not already set */
    if (object->powerConstraint[handle->timerPart])
    {
        HwiP_restore(key);
        return;
    }
    object->powerConstraint[handle->timerPart] = true;
    HwiP_restore(key);
    Power_setConstraint(PowerCC26XX_DISALLOW_STANDBY);
}

/*!
 *  @brief  Clear Standby constraint while using timer to avoid TI RTOS
 *          going into standby when timer is running.  As constraints are
 *          counting, store constraint status and access atomically and only
 *          once per timer resource.
 */
static inline void GPTimerCC26XXThreadsafeConstraintClr(GPTimerCC26XX_Handle handle)
{
    GPTimerCC26XX_Object *object = handle->object;

    uint32_t key = HwiP_disable();
    /* Only release if constraint set */
    if (!object->powerConstraint[handle->timerPart])
    {
        HwiP_restore(key);
        return;
    }
    object->powerConstraint[handle->timerPart] = false;
    HwiP_restore(key);
    Power_releaseConstraint(PowerCC26XX_DISALLOW_STANDBY);
}

/*!
 *  @brief  Configure debug stall mode in timer. When enabled the timer will
 *          stop when the CPU is halted by the debugger.
 */
void GPTimerCC26XX_configureDebugStall(GPTimerCC26XX_Handle handle, GPTimerCC26XX_DebugMode mode)
{
    GPTimerCC26XX_HWAttrs const *hwAttrs = handle->hwAttrs;
    uint32_t timer = GPT_LUT[handle->timerPart].map;
    TimerStallControl(hwAttrs->baseAddr, timer, mode);
}

/*!
 *  @brief  Set custom argument in GPTimerCC26XX_Object
 */
void GPTimerCC26XX_setArg(GPTimerCC26XX_Handle handle, void *arg)
{
    GPTimerCC26XX_Object *object = handle->object;
    object->arg = (uint32_t)arg;
}

/*!
 *  @brief  Get custom argument in GPTimerCC26XX_Object
 */
uint32_t GPTimerCC26XX_getArg(GPTimerCC26XX_Handle handle)
{
    GPTimerCC26XX_Object *object = handle->object;

    return ((uint32_t) object->arg);

}

/*!
 *  @brief  Set timer configuration. Will be moved to driverlib.
 */
static inline void TimerSetConfig(uint32_t ui32Base, uint32_t ui32Config)
{
    HWREG(ui32Base + GPT_O_CFG) = ui32Config;
}
/*!
 *  @brief  Set timer mode. Will be moved to driverlib.
 */
static inline void TimerSetMode(uint32_t ui32Base, uint32_t timer, uint32_t mode)
{
    uint32_t addr = ui32Base;

    if (timer == TIMER_B)
    {
        addr += GPT_O_TBMR;
    }
    else
    {
        addr += GPT_O_TAMR;
    }
    HWREG(addr) = mode;
}
