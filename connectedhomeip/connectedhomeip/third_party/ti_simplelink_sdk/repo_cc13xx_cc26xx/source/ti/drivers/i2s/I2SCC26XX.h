/*
 * Copyright (c) 2019, Texas Instruments Incorporated
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
 *  @file       I2SCC26XX.h
 *
 *  @brief      I2S driver implementation for a CC26XX I2S controller
 *
 *  ============================================================================
 */
#ifndef ti_drivers_i2s_I2SCC26XX__include
#define ti_drivers_i2s_I2SCC26XX__include

#include <ti/drivers/I2S.h>
#include <ti/drivers/dpl/SemaphoreP.h>
#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/Power.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  @brief  I2S Hardware attributes
 *
 *  intPriority is the I2S peripheral's interrupt priority, as defined by the
 *  TI-RTOS kernel. This value is passed unmodified to Hwi_create().
 *
 *  pinSD1 and pinSD0 define the SD0 and SD1 data pin mapping, respectively.
 *  pinSCK, pinMCLK and pinWS define the SCK, MCLK and WS clock pin mapping, respectively.
 *  All these pins are typically defined with a macro in a header file, which maps to an IOID.
 *
 *  A sample structure is shown below:
 *  @code
 *  const I2SCC26XX_HWAttrs i2sHWAttrs[CC26X2R1_LAUNCHXL_I2SCOUNT] = {
 *      {
 *         .pinSD1      =  CONFIG_I2S_ADI,
 *         .pinSD0      =  CONFIG_I2S_ADO,
 *         .pinSCK      =  CONFIG_I2S_BCLK,
 *         .pinMCLK     =  CONFIG_I2S_MCLK,
 *         .pinWS       =  CONFIG_I2S_WCLK,
 *         .intPriority =  ~0,
 *      },
 *  };
 *  @endcode
 */
typedef struct {
    uint_least8_t       pinSD1;         /*!< Pin used for SD1 signal. */
    uint_least8_t       pinSD0;         /*!< Pin used for SD0 signal. */
    uint_least8_t       pinSCK;         /*!< Pin used for SCK signal. */
    uint_least8_t       pinMCLK;        /*!< Pin used for MCLK signal. Non used in most of the applications. */
    uint_least8_t       pinWS;          /*!< Pin used for WS signal. */
    uint8_t             intPriority;    /*!< I2S Peripheral's interrupt priority. */
} I2SCC26XX_HWAttrs;

/*!
 *  @cond NODOC
 *  I2S data-interface
 *
 *  This enum defines how the physical I2S interface (SD0/SD1) behaves.
 *  Do not modify.
 */
typedef struct {
    uint8_t                         numberOfChannelsUsed;     /*!< Number of channels used on SDx. */
    I2S_ChannelConfig               channelsUsed;             /*!< List of the used channels. */
    I2S_DataInterfaceUse            interfaceConfig;          /*!< IN / OUT / UNUSED */
}I2SCC26XX_DataInterface;
/*! @endcond */

/*!
 *  @cond NODOC
 *  I2S interface
 *
 *  This enum defines one of the interfaces (READ or WRITE) of the I2S module.
 *  Do not modify.
 */
typedef struct {
    uint16_t                        memoryStep;               /*!< Size of the memory step to access the following sample */
    uint16_t                        delay;                    /*!< Number of WS cycles to wait before starting the first transfer. This value is mostly used when performing constant latency transfers. */
    I2S_Callback                    callback;                 /*!< Pointer to callback */
    I2S_RegUpdate                   pointerSet;               /*!< Pointer on the function used to update PTR-NEXT */
    I2S_StopInterface               stopInterface;            /*!< Pointer on the function used to stop the interface */
    I2S_Transaction                *activeTransfer;           /*!< Pointer on the ongoing transfer */
}I2SCC26XX_Interface;
/*! @endcond */

/*!
 *  @brief      The definition of a function used by the I2S driver
 *              to refresh the pointer
 *
 *  @param      I2S_Handle  I2S_Handle
 *
 *  @param      I2SCC26XX_Interface *interface Pointer on the interface to update
 *
 */
typedef void (*I2SCC26XX_PtrUpdate)(I2S_Handle handle, I2SCC26XX_Interface *interface);

/*!
 *  @cond NODOC
 *  I2S Object.  The application must not access any member variables
 *  of this structure!
 */
typedef struct {

    bool                            isOpen;                  /*!< To avoid multiple openings of the I2S. */
    bool                            invertWS;                /*!< WS inversion.
                                                                    false: The WS signal is not internally inverted.
                                                                    true:  The WS signal is internally inverted. */
    uint8_t                         memorySlotLength;        /*!< Select the size of the memory used. The two options are 16 bits and 24 bits. Any value can be selected, whatever the value of ::i2sBitsPerWord.
                                                                    I2S_MEMORY_LENGTH_16BITS_CC26XX: Memory length is 16 bits.
                                                                    I2S_MEMORY_LENGTH_24BITS_CC26XX: Memory length is 24 bits.*/
    uint8_t                         dataShift;               /*!< When dataShift is set to 0, data are read/write on the data lines from the first SCK
                                                                  period of the half WS period to the last SCK edge of the WS half period.
                                                                  By setting dataShift to a value different from zero, you can postpone the moment when
                                                                  data are read/write during the WS half period.
                                                                  For example, by setting dataShift to 1, data are read/write on the data lines from the
                                                                  second SCK period of the half WS period to the first SCK edge of the next WS half period.
                                                                  If no padding is activated, this corresponds to the I2S standard. */
    uint8_t                         bitsPerWord;             /*!< Number of bits per word (must be between 8 and 24 bits). */
    uint8_t                         beforeWordPadding;       /*!< Number of SCK periods between the first WS edge and the MSB of the first audio channel data transferred during the phase.*/
    uint8_t                         afterWordPadding;        /*!< Number of SCK periods between the LSB of the last audio channel data transferred during the phase and the following WS edge.*/
    uint8_t                         dmaBuffSizeConfig;       /*!< Number of consecutive bytes of the samples buffers. This field must be set to a value x between 1 and 255. All the data buffers used must contain N*x bytes (with N an intger verifying N>0). */
    I2S_SamplingEdge                samplingEdge;            /*!< Select edge sampling type.
                                                                    I2S_SAMPLING_EDGE_FALLING: Sampling on falling edges.
                                                                    I2S_SAMPLING_EDGE_RISING:  Sampling on raising edges. */
    I2S_Role                        moduleRole;              /*!< Select if the current device is a Slave or a Master.
                                                                    I2S_SLAVE:  The device is a slave (clocks are generated externally).
                                                                    I2S_MASTER: The device is a master (clocks are generated internally). */
    I2S_PhaseType                   phaseType;               /*!< Select phase type.
                                                                    I2S_PHASE_TYPE_SINGLE: Single phase.
                                                                    I2S_PHASE_TYPE_DUAL:   Dual phase.*/
    uint16_t                        MCLKDivider;             /*!< Frequency divider for the MCLK signal. */
    uint16_t                        SCKDivider;              /*!< Frequency divider for the SCK signal. */
    uint16_t                        WSDivider;               /*!< Frequency divider for the WS signal. */
    uint16_t                        startUpDelay;            /*!< Time (in number of WS cycles) to wait before the first transfer. */
    I2SCC26XX_DataInterface         dataInterfaceSD0;        /*!< Structure to describe the SD0 interface */
    I2SCC26XX_DataInterface         dataInterfaceSD1;        /*!< Structure to describe the SD1 interface */

    /* PIN driver state object and handle */
    PIN_State                       pinState;                /*!< Pin state for the used pins */
    PIN_Handle                      hPin;                    /*!< Handle on the used pins */

    /* I2S SYS/BIOS objects */
    HwiP_Struct                     hwi;                     /*!< Hwi object for interrupts */
    I2SCC26XX_PtrUpdate             ptrUpdateFxn;            /*!< Pointer on the function used to update IN and OUT PTR-NEXT */
    I2SCC26XX_Interface             read;                    /*!< Structure to describe the read (in) interface */
    I2SCC26XX_Interface             write;                   /*!< Structure to describe the write (out) interface */
    I2S_Callback                    errorCallback;           /*!< Pointer to error callback */

    /* I2S pre and post notification functions */
    void                            *i2sPreFxn;              /*!< I2S pre-notification function pointer */
    void                            *i2sPostFxn;             /*!< I2S post-notification function pointer */
    Power_NotifyObj                 i2sPreObj;               /*!< I2S pre-notification object */
    Power_NotifyObj                 i2sPostObj;              /*!< I2S post-notification object */
    volatile bool                   i2sPowerConstraint;      /*!< I2S power constraint flag, guard to avoid power constraints getting out of sync */

} I2SCC26XX_Object;
/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_i2s_I2SCC26XX__include */
