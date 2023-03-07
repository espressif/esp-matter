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
/** ============================================================================
 *  @file   PDMCC26XX_util.h
 *
 *  @brief  PDM utility includes helper functions for configuring the CC26XX I2S
 *          controller.
 *
 *  The PDMCC26XX utility header file should be included in an application as
 *  follows:
 *  @code
 *  #include <ti/drivers/pdm/PDMCC26XX_util.h>
 *  @endcode
 *
 * # Overview #
 * This utility file is written specifically for the I2S module on CC26XX. The
 * user should be aware that although this HW module is called I2S it is a
 * highly configurable audio interface module. I2S is only one of many
 * configurations.
 *
 *
 *  ============================================================================
 */

#ifndef ti_drivers_i2s_PDMCC26XX_I2S__include
#define ti_drivers_i2s_PDMCC26XX_I2S__include

#include <ti/drivers/pin/PINCC26XX.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/dpl/SemaphoreP.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 *  @brief      Define to control debug mode
 *
 *  Production code should set this to xI2S_DEBUG. To enable debug mode
 *  rename the define to \b I2S_DEBUG.
 */
#define xI2S_DEBUG

/*!
 *  At least three elements must exist for good flow in driver
 */
#define PDMCC26XX_I2S_MIN_ALLOWED_QUEUE_SIZE        3

/*!
 *  PDM block overhead size in number of bytes --> sizeof(PDMCC26XX_I2S_QueueNode)
 */
#ifdef I2S_DEBUG
#define I2S_BLOCK_OVERHEAD_IN_BYTES             16
#else //I2S_DEBUG
#define I2S_BLOCK_OVERHEAD_IN_BYTES             12
#endif //I2S_DEBUG

/*! Return code when PDMCC26XX_I2S_control() was successful. */
#define PDMCC26XX_I2S_CMD_SUCCESS              0
/*! Return code when a I2S command or function is undefined/not-implemented. */
#define PDMCC26XX_I2S_CMD_UNDEFINED            -1
/*! Return code when PDMCC26XX_I2S_control() was unsuccessful. */
#define PDMCC26XX_I2S_CMD_NO_SUCCESS           -2

/*! Generic macro for disabled */
#define PDMCC26XX_I2S_GENERIC_DISABLED              0
/*! Generic macro for enabled */
#define PDMCC26XX_I2S_GENERIC_ENABLED               1

/*!
 *  @brief
 *  PDMCC26XX_I2S_MallocFxn is a function pointer for the malloc function to
 *  be used by the driver.
 */
typedef void *(*PDMCC26XX_I2S_MallocFxn)(size_t memSize);

/*!
 *  @brief
 *  PDMCC26XX_I2S_FreeFxn is a function pointer for the free function to
 *  be used by the driver. This is needed for memory clean up, if something goes
 *  wrong.
 */
typedef void (*PDMCC26XX_I2S_FreeFxn)(void *ptr, size_t memSize);

/*!
 *  @brief
 *  The PDMCC26XX_I2S_Config structure contains a set of pointers used to characterize
 *  the PDMCC26XX_I2S driver implementation.
 */
typedef struct {
    /*! Pointer to a driver specific data object */
    void                        *object;

    /*! Pointer to a driver specific hardware attributes structure */
    void                const   *hwAttrs;
} PDMCC26XX_I2S_Config;

/*!
 *  @brief      A handle that is returned from a PDMCC26XX_I2S_open() call.
 */
typedef PDMCC26XX_I2S_Config *PDMCC26XX_I2S_Handle;

/*!
 *  @brief      Status codes that are set by the I2S driver.
 */
typedef enum {
    PDMCC26XX_I2S_STREAM_IDLE = 0,          /*!< Idle mode. Stream not started */
    PDMCC26XX_I2S_STREAM_STARTED,           /*!< Stream started, no buffer yet available */
    PDMCC26XX_I2S_STREAM_CANCELED,          /*!< Unused state. */
    PDMCC26XX_I2S_STREAM_FAILED,            /*!< PDMCC26XX_I2S_startStream() called while stream
                                             * is already running
                                             */
    PDMCC26XX_I2S_STREAM_ERROR,             /*!< No pointer available when one was expected,
                                             * meaning the driver failed to provide new
                                             * pointer and PDMCC26XX_I2S_stopStream() was not
                                             * called
                                             */
    PDMCC26XX_I2S_STREAM_BUFFER_READY,      /*!< Buffer ready, either IN or OUT or
                                             * both, whichever are expected
                                             */
    PDMCC26XX_I2S_STREAM_BUFFER_DROPPED,    /*!< A new buffer is ready but the old ones were not
                                             * processed in time. The oldest buffer was dropped
                                             * to provide space for newer data.
                                             */
    PDMCC26XX_I2S_STREAM_STOPPING,          /*!< PDMCC26XX_I2S_stopStream() is called, a
                                             * graceful shutdown procedure is started
                                             */
    PDMCC26XX_I2S_STREAM_STOPPED,           /*!< Driver transitioned from Stopping to
                                             * Stopped state during graceful shutdown. Now
                                             * a pointer error is expected, and upon it a
                                             * semaphore is set allowing
                                             * PDMCC26XX_I2S_stopStream() to return
                                             */
    PDMCC26XX_I2S_STREAM_FAILED_TO_STOP     /*!< PDMCC26XX_I2S_stopStream() was called,
                                             * but driver timed out trying to gracefully
                                             * shutdown
                                             */
} PDMCC26XX_I2S_Status;

/*!
 *  @brief
 *  Definitions for various PDMCC26XX_I2S modes of operation.
 */
typedef enum {
    PDMCC26XX_I2S_PDM       = 0,    /*!< PDMCC26XX_I2S in PDM microphone mode */
    PDMCC26XX_I2S_I2S       = 1     /*!< PDMCC26XX_I2S in I2S mode */
} PDMCC26XX_I2S_Mode;

/*!
 *  We don't use sample stamp generator, but we can't start without
 *  enabling it and configuring the trigger. This is because the
 *  trigger also starts the audio stream
 *  Since we don't use it we keep the word period at its max 2^16
 *  For the driverlib which runs a modulo on the word period we can
 *  set the modulo to 0xFFFF to avoid issues with division by zero.
 */
#define PDMCC26XX_I2S_DEFAULT_SAMPLE_STAMP_MOD      0x0000FFFF

/*!
 *  Definitions for different I2S Word Clock phase settings.
 *
 *  Defines how WCLK division ratio is calculated to generate different
 *  duty cycles.
 *  \sa I2SWCLKDIV.WDIV
 *
 *  Macro                                       | Value |
 *  ------------------------------------------- | ------|
 *  PDMCC26XX_I2S_WordClockPhase_Single             | 0     |
 *  PDMCC26XX_I2S_WordClockPhase_Dual               | 1     |
 *  PDMCC26XX_I2S_WordClockPhase_UserDefined        | 2     |
 */
#define PDMCC26XX_I2S_WordClockPhase_Single         0
/*!
 *  \sa PDMCC26XX_I2S_WordClockPhase_Single
 */
#define PDMCC26XX_I2S_WordClockPhase_Dual           1
/*!
 *  \sa PDMCC26XX_I2S_WordClockPhase_Single
 */
#define PDMCC26XX_I2S_WordClockPhase_UserDefined    2

/*!
 *  Definitions to set sample edge
 *
 *  Macro                               | Value |
 *  ----------------------------------- | ------|
 *  PDMCC26XX_I2S_SampleEdge_Negative       | 0     |
 *  PDMCC26XX_I2S_SampleEdge_Postive        | 1     |
 */
#define PDMCC26XX_I2S_SampleEdge_Negative   0
/*!
 *  \sa PDMCC26XX_I2S_SampleEdge_Negative
 */
#define PDMCC26XX_I2S_SampleEdge_Postive    1

/*!
 *  Definitions different I2S Word Clock source settings.
 *
 *  Macro                               | Value |
 *  ----------------------------------- | ------|
 *  PDMCC26XX_I2S_WordClockSource_Ext       | 0     |
 *  PDMCC26XX_I2S_WordClockSource_Int       | 1     |
 */
#define PDMCC26XX_I2S_WordClockSource_Ext       1
/*!
 *  \sa PDMCC26XX_I2S_WordClockSource_Ext
 */
#define PDMCC26XX_I2S_WordClockSource_Int       2

/*!
 *  Definitions different I2S Bit Clock source settings.
 *
 *  Macro                               | Value |
 *  ----------------------------------- | ------|
 *  PDMCC26XX_I2S_BitClockSource_Ext        | 0     |
 *  PDMCC26XX_I2S_BitClockSource_Int        | 1     |
 */
#define PDMCC26XX_I2S_BitClockSource_Ext       0
/*!
 *  \sa PDMCC26XX_I2S_BitClockSource_Ext
 */
#define PDMCC26XX_I2S_BitClockSource_Int       1

/*!
 *  Definitions to either invert I2S word or bit clock or not
 *
 *  Macro                               | Value |
 *  ----------------------------------- | ------|
 *  PDMCC26XX_I2S_ClockSource_Normal        | 0     |
 *  PDMCC26XX_I2S_ClockSource_Inverted      | 1     |
 */
#define PDMCC26XX_I2S_ClockSource_Normal    0
/*!
 *  \sa PDMCC26XX_I2S_ClockSource_Normal
 */
#define PDMCC26XX_I2S_ClockSource_Inverted  1

/*!
 *  PDMCC26XX_I2S Audio Data Pin Usage.
 *
 *  Macro                       | Details       |
 *  --------------------------- | --------------|
 *  PDMCC26XX_I2S_ADUsageDisabled   | Disabled      |
 *  PDMCC26XX_I2S_ADUsageInput      | Input         |
 *  PDMCC26XX_I2S_ADUsageOutput     | Output        |
 */
#define PDMCC26XX_I2S_ADUsageDisabled       0
/*!
 *  \sa PDMCC26XX_I2S_ADUsageDisabled
 */
#define PDMCC26XX_I2S_ADUsageInput          1
/*!
 *  \sa PDMCC26XX_I2S_ADUsageDisabled
 */
#define PDMCC26XX_I2S_ADUsageOutput         2

/*!
 *  PDMCC26XX_I2S Audio Channel Masks.
 *
 *  Macro                       | Value         | Usage                         |
 *  --------------------------- | --------------|-------------------------------|
 *  PDMCC26XX_I2S_CHAN0_ACT         | 0x00000001    | OR in to enable channel 0     |
 *  PDMCC26XX_I2S_CHAN1_ACT         | 0x00000002    | OR in to enable channel 1     |
 *  PDMCC26XX_I2S_CHAN2_ACT         | 0x00000004    | OR in to enable channel 2     |
 *  PDMCC26XX_I2S_CHAN3_ACT         | 0x00000008    | OR in to enable channel 3     |
 *  PDMCC26XX_I2S_CHAN4_ACT         | 0x00000010    | OR in to enable channel 4     |
 *  PDMCC26XX_I2S_CHAN5_ACT         | 0x00000020    | OR in to enable channel 5     |
 *  PDMCC26XX_I2S_CHAN6_ACT         | 0x00000040    | OR in to enable channel 6     |
 *  PDMCC26XX_I2S_CHAN7_ACT         | 0x00000080    | OR in to enable channel 7     |
 *  PDMCC26XX_I2S_MONO_MODE         | 0x00000001    | Use to set Mono mode          |
 *  PDMCC26XX_I2S_STEREO_MODE       | 0x00000003    | Use to set Stereo mode        |
 *  PDMCC26XX_I2S_DISABLED_MODE     | 0x00000000    | Use to disable                |
 *  PDMCC26XX_I2S_CHAN_CFG_MASK     | 0x000000FF    | Use to mask out invalid       |
 */
#define PDMCC26XX_I2S_CHAN0_ACT           0x00000001
/*! \sa PDMCC26XX_I2S_CHAN0_ACT */
#define PDMCC26XX_I2S_CHAN1_ACT           0x00000002
/*! \sa PDMCC26XX_I2S_CHAN0_ACT */
#define PDMCC26XX_I2S_CHAN2_ACT           0x00000004
/*! \sa PDMCC26XX_I2S_CHAN0_ACT */
#define PDMCC26XX_I2S_CHAN3_ACT           0x00000008
/*! \sa PDMCC26XX_I2S_CHAN0_ACT */
#define PDMCC26XX_I2S_CHAN4_ACT           0x00000010
/*! \sa PDMCC26XX_I2S_CHAN0_ACT */
#define PDMCC26XX_I2S_CHAN5_ACT           0x00000020
/*! \sa PDMCC26XX_I2S_CHAN0_ACT */
#define PDMCC26XX_I2S_CHAN6_ACT           0x00000040
/*! \sa PDMCC26XX_I2S_CHAN0_ACT */
#define PDMCC26XX_I2S_CHAN7_ACT           0x00000080
/*! \sa PDMCC26XX_I2S_CHAN0_ACT */
#define PDMCC26XX_I2S_MONO_MODE           0x00000001
/*! \sa PDMCC26XX_I2S_CHAN0_ACT */
#define PDMCC26XX_I2S_STEREO_MODE         0x00000003
/*! \sa PDMCC26XX_I2S_CHAN0_ACT */
#define PDMCC26XX_I2S_DISABLED_MODE       0x00000000
/*! \sa PDMCC26XX_I2S_CHAN0_ACT */
#define PDMCC26XX_I2S_CHAN_CFG_MASK       0x000000FF

/*!
 *  PDMCC26XX_I2S data word length is used to determine how bits to transfer per word.
 *
 *  Macro                       | Value | Usage                                 |
 *  --------------------------- | ------|---------------------------------------|
 *  PDMCC26XX_I2S_WordLengthMin     | 8     | Minimum transfer length is 8 bits     |
 *  PDMCC26XX_I2S_WordLength16      | 16    | A typical transfer length is 16 bits  |
 *  PDMCC26XX_I2S_WordLengthMax     | 24    | Maximum transfer length is 24 bits    |
 */
#define PDMCC26XX_I2S_WordLengthMin         8
/*! \sa PDMCC26XX_I2S_WordLengthMin */
#define PDMCC26XX_I2S_WordLength16          16
/*! \sa PDMCC26XX_I2S_WordLengthMin */
#define PDMCC26XX_I2S_WordLengthMax         24

/*!
 *  PDMCC26XX_I2S Phase is set to select Dual or Single phase format
 *
 *  Macro                       | Value |
 *  --------------------------- | ------|
 *  PDMCC26XX_I2S_SinglePhase       | 0     |
 *  PDMCC26XX_I2S_DualPhase         | 1     |
 */
#define PDMCC26XX_I2S_SinglePhase           0
/*! \sa PDMCC26XX_I2S_SinglePhase */
#define PDMCC26XX_I2S_DualPhase             1

/*!
 *  PDMCC26XX_I2S Sample Edge is set to control what edge to sample and clock out
 *  data on.
 *
 *  Macro                       | Value |
 *  --------------------------- | ------|
 *  PDMCC26XX_I2S_NegativeEdge      | 0     |
 *  PDMCC26XX_I2S_PositiveEdge      | 1     |
 */
#define PDMCC26XX_I2S_NegativeEdge          0
/*! \sa PDMCC26XX_I2S_NegativeEdge */
#define PDMCC26XX_I2S_PositiveEdge          1

/*!
 *  PDMCC26XX_I2S data word size is used to determine how to configure the
 *  I2S data transfers to/from memory.
 *
 *  Macro                       | Value | Usage                         |
 *  --------------------------- | ------|-------------------------------|
 *  PDMCC26XX_I2S_MemLen16bit       | 0     | sample 16 bits per word       |
 *  PDMCC26XX_I2S_MemLen24bit       | 1     | sample 24 bits per word       |
 *
 *  PDMCC26XX_I2S_MemLen16bit: sample 16 bits per word
 *  PDMCC26XX_I2S_MemLen24bit: sample 24 bits per word
 */
#define PDMCC26XX_I2S_MemLen16bit  0
/*! \sa PDMCC26XX_I2S_MemLen16bit */
#define PDMCC26XX_I2S_MemLen24bit  1

/*!
 *  PDMCC26XX_I2S Data Delay, which translates into format (LJF, I2S/DSP, RJF).
 *
 *  This field can be set to any 8 bit value. The macros are just defined
 *  for convenience. Left justified mode means that sampling should start
 *  immediately. For right justified mode the data delay depends on how
 *  many samples should be taken per word. It is an alignment.
 *
 *  I2S is a special mode that defines that no sample occur on first edge,
 *  hence there is one period data delay.
 *
 *  Macro                       | Value | Usage                         |
 *  --------------------------- | ------|-------------------------------|
 *  PDMCC26XX_I2S_FormatLJF         | 0     | no data delay                 |
 *  PDMCC26XX_I2S_FormatI2SandDSP   | 1     | one period data delay         |
 *  PDMCC26XX_I2S_FormatRJFmin      | 2     | two periods data delay        |
 *  PDMCC26XX_I2S_FormatRJFmax      | 255   | 255 periods data delay        |
 */
#define PDMCC26XX_I2S_FormatLJF                     0
/*! \sa PDMCC26XX_I2S_FormatLJF */
#define PDMCC26XX_I2S_FormatI2SandDSP               1
/*! \sa PDMCC26XX_I2S_FormatLJF */
#define PDMCC26XX_I2S_FormatRJFmin                  2
/*! \sa PDMCC26XX_I2S_FormatLJF */
#define PDMCC26XX_I2S_FormatRJFmax                  255

/*! Number of samples (16 or 24 bits) per queue element buffer */
typedef uint32_t PDMCC26XX_I2S_TransferSize;

/*!
 *  @brief  PDMCC26XX_I2S Hardware attributes
 *
 *  These fields are used by driverlib APIs and therefore must be populated by
 *  driverlib macro definitions. For CC26xxWare these definitions are found in:
 *      - inc/hw_memmap.h
 *      - inc/hw_ints.h
 *
 *  A sample structure is shown below:
 *  @code
 *  const PDMCC26XX_I2S_HWAttrs PDMCC26XX_I2SHWAttrs = {
 *      {
 *          I2S0_BASE,
 *          INT_I2S,
 *          PERIPH_I2S,
 *          CONFIG_I2S_MCLK,
 *          CONFIG_I2S_BCLK,
 *          CONFIG_I2S_WCLK,
 *          CONFIG_I2S_ADI,
 *          CONFIG_I2S_ADO
 *      },
 *  };
 *  @endcode
 */
typedef struct {
    /*! I2S Peripheral's interrupt vector */
    uint8_t          intNum;
    /*! I2S Peripheral's interrupt priority */
    uint8_t          intPriority;
    /*! I2S Peripheral's power manager ID */
    PowerCC26XX_Resource   powerMngrId;
    /*! I2S MCLK pin */
    PIN_Id           mclkPin;
    /*! I2S BCLK pin */
    PIN_Id           bclkPin;
    /*! I2S WCLK pin */
    PIN_Id           wclkPin;
    /*! I2S AD0 pin */
    PIN_Id           ad0Pin;
    /*! I2S Peripheral's base address */
    uint32_t         baseAddr;
} PDMCC26XX_I2S_HWAttrs;

/*!
 *  @brief  PDMCC26XX_I2S Audio Clock configuration
 *
 *  These fields are used by the driver to set up the I2S module
 *
 *  A sample structure is shown below (single PDM microphone):
 *  @code
 *  const PDMCC26XX_I2S_AudioClockConfig PDMCC26XX_I2Sobjects[] = {
 *          16, // Word clock division
 *          PDMCC26XX_I2S_SampleEdge_Negative,
 *          PDMCC26XX_I2S_WordClockPhase_Dual,
 *          PDMCC26XX_I2S_ClockSource_Inverted,
 *          PDMCC26XX_I2S_WordClockSource_Int,
 *          47, // Bit clock division
 *          0, // Reserved
 *          PDMCC26XX_I2S_BitClockSource_Int
 *          6, // Master clock division
 *  };
 *  @endcode
 */
typedef struct {
    /*! I2S Word Clock divider override */
    uint16_t    wclkDiv;
    /*! I2S Sample Edge.
     *   0 - data and WCLK are sampled on the negative edge and clocked out on the positive edge.
     *   1 - data and WCLK are sampled on the positive edge and clocked out on the negative edge */
    uint16_t    sampleOnPositiveEdge:1;
    /*! I2S Word Clock Phase(PDMCC26XX_I2S_WordClockPhase_Dual, PDMCC26XX_I2S_WordClockPhase_Single or PDMCC26XX_I2S_WordClockPhase_UserDefined) */
    uint16_t    wclkPhase:2;
    /*! I2S Invert Word Clock (PDMCC26XX_I2S_ClockSource_Inverted or PDMCC26XX_I2S_ClockSource_Normal) */
    uint16_t    wclkInverted:1;
    /*! I2S Word Clock source (PDMCC26XX_I2S_WordClockSource_Ext or PDMCC26XX_I2S_WordClockSource_Int) */
    uint16_t    wclkSource:2;
    /*! I2S Bit Clock divider override */
    uint16_t    bclkDiv:10;
    /*! Reserved bit field */
    uint16_t    reserved:5;
    /*! I2S Bit Clock source (PDMCC26XX_I2S_BitClockSource_Ext or PDMCC26XX_I2S_BitClockSource_Int) */
    uint16_t    bclkSource:1;
    /*! I2S Master Clock divider override */
    uint16_t    mclkDiv:10;
} PDMCC26XX_I2S_AudioClockConfig;

/*!
 *  @brief  PDMCC26XX_I2S Audio Pin configuration
 *
 *  These fields are used by the driver to set up the I2S module
 *
 *  A sample structure is shown below (single PDM microphone):
 *  @code
 *  const PDMCC26XX_I2S_AudioPinConfig PDMCC26XX_I2Sobjects[] = {
 *          PDMCC26XX_I2S_ADUsageDisabled,
 *          0,
 *          0,
 *          0,
 *          0,
 *          PDMCC26XX_I2S_ADUsageInput,
 *          0,
 *          1,
 *          2,
 *          I2S_MONO_MODE
 *  };
 *  @endcode
 */
typedef union PDMCC26XX_I2S_AudioPinConfig {
  /*! Can be used to set pin configurations in DriverLib*/
  struct {
    /*! Field that can be used to set pin configuration in DriverLib */
    uint16_t    ad1;
    /*! Field that can be used to set pin configuration in DriverLib */
    uint16_t    ad0;
  } driverLibParams;
  /*! Used to configure various aspects of the I2S hardware during initialisation */
  struct {
    /*! I2S AD1 usage (0: Disabled, 1: Input, 2: Output) */
    uint8_t     ad1Usage:2;
    /*! I2S Enable Master clock output on pin (0: Disabled, 1: Enabled) */
    uint8_t     enableMclkPin:1;
    /*! Reserved bit field */
    uint8_t     reserved:1;
    /*! I2S AD1 number of channels (1 - 8). !Must match channel mask */
    uint8_t     ad1NumOfChannels:4;
    /*! I2S AD1 Channel Mask bitwise 0:Disabled, 1:Enabled) E.g. Mono: 0x01, Stereo: 0x03 */
    uint8_t     ad1ChannelMask;
    /*! I2S AD0 usage (0: Disabled, 1: Input, 2: Output) */
    uint8_t     ad0Usage:2;
    /*! I2S Enable Word clock output on pin (0: Disabled, 1: Enabled) */
    uint8_t     enableWclkPin:1;
    /*! I2S Enable Bit clock output on pin (0: Disabled, 1: Enabled) */
    uint8_t     enableBclkPin:1;
    /*! I2S AD0 number of channels (1 - 8). !Must match channel mask*/
    uint8_t     ad0NumOfChannels:4;
    /*! I2S AD0 Channel Mask bitwise(0:Disabled, 1:Enabled) E.g. Mono: 0x01, Stereo: 0x03 */
    uint8_t     ad0ChannelMask;
  } bitFields;
} PDMCC26XX_I2S_AudioPinConfig;
/*! Mask to use with PDMCC26XX_I2S_AudioPinConfig.driverLibParams when calling
 * DriverLib.
 */
#define PDMCC26XX_I2S_DIR_CHA_M     (I2S_LINE_MASK | I2S_CHAN_CFG_MASK)

/*!
 *  @brief  PDMCC26XX_I2S Hardware configuration
 *
 *  These fields are used by the driver to set up the I2S module
 *
 *  A sample structure is shown below (single PDM microphone):
 *  @code
 *  const PDMCC26XX_I2S_AudioFormatConfig PDMCC26XX_I2Sobjects[] = {
 *          PDMCC26XX_I2S_WordLength16,
 *          PDMCC26XX_I2S_PositiveEdge,
 *          PDMCC26XX_I2S_DualPhase,
 *          PDMCC26XX_I2S_MemLen16bit,
 *          PDMCC26XX_I2S_FormatI2SandDSP
 *  };
 *  @endcode
 */
typedef struct {
    /*! Number of bits per word (8-24). Exact for single phase, max for dual phase */
    uint8_t          wordLength:5;
    /*! Sample edge. Data and Word clock is samples, and clocked out, on opposite edges of BCLK.
     *   0: NEG (Data is sample on the negative edge and clocked out on the positive edge)
     *   1: POS (Data is sample on the positive edge and clocked out on the negative edge)*/
    uint8_t          sampleEdge:1;
    /*! Selects dual- or single phase format (0: Single, 1: Dual) */
    uint8_t          dualPhase:1;
    /*! Size of each word stored to or loaded from memory (0: 16, 1: 24) */
    uint8_t          memLen:1;
    /*! Number of BCLK perids between a WCLK edge and MSB of the first word in a phase */
    uint8_t          dataDelay;
} PDMCC26XX_I2S_AudioFormatConfig;


/*!
 *  @brief  PDMCC26XX_I2S Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef enum {
    /*!
     * PDMCC26XX_I2S_requestBuffer() blocks execution. This mode can only be used when called
     * within a Task context.
     */
    PDMCC26XX_I2S_MODE_BLOCKING,
    /*!
     * PDMCC26XX_I2S_requestBuffer returns immediately
     * if no buffer is available. The caller is notified through events each time
     * a buffer is available. This mode can be used in a Task, Swi, or Hwi context.
     */
    PDMCC26XX_I2S_CALLBACK_MODE
} PDMCC26XX_I2S_RequestMode;

/*!
 *  @brief
 *  A ::PDMCC26XX_I2S_StreamNotification data structure is used with PDMCC26XX_I2S_CallbackFxn().
 *  Provides notification about available buffers and potential errors
 */
typedef struct {
    void      *arg;             /*!< Argument to be passed to the callback function */
    PDMCC26XX_I2S_Status status;    /*!< Status code set by PDMCC26XX_I2S driver */
} PDMCC26XX_I2S_StreamNotification;

/*!
 *  @brief
 *  A ::PDMCC26XX_I2S_BufferRequest data structure is used with PDMCC26XX_I2S_requestBuffer().
 *
 *  bufferIn is a pointer to the requested buffer. It is NULL if no buffer is
 *  available\n
 *
 *  Input Mode          | Interpretation of bufferIn being NULL after returning |
 *  --------------------|-------------------------------------------------------|
 *  Blocking mode       | Request timed out and still no buffer available       |
 *  Non-Blocking mode   | No buffer available                                   |
 *
 *  PDMCC26XX_I2S_requestBuffer will also return \b false if there are no buffers
 *  available.
 *
 *  \sa PDMCC26XX_I2S_requestBuffer
 */
typedef struct {
    void      *bufferIn;        /*!< Pointer to requested In buffer */
    void      *bufferHandleIn;  /*!< Pointer to requested In buffers handle */
    PDMCC26XX_I2S_Status status;    /*!< Status code set by PDMCC26XX_I2S_requestBuffer */
} PDMCC26XX_I2S_BufferRequest;

/*!
 *  @brief
 *  A ::PDMCC26XX_I2S_BufferRelease data structure is used with PDMCC26XX_I2S_releaseBuffer().
 *
 *  bufferHandleIn and bufferHandleOut allows the driver to take back and
 *  reuse memory.
 */
typedef struct {
    void      *bufferHandleIn;    /*!< Pointer to requested In buffers handle that we now release */
} PDMCC26XX_I2S_BufferRelease;

/*!
 *  @brief      The definition of a callback function used when wakeup on
 *              chip select is enabled
 *
 *  @param      PDMCC26XX_I2S_Handle          PDMCC26XX_I2S_Handle
 */
typedef void        (*PDMCC26XX_I2S_CallbackFxn) (PDMCC26XX_I2S_Handle handle, PDMCC26XX_I2S_StreamNotification *notification);

/*!
 *  @brief
 *  PDMCC26XX I2S Parameters are used when calling ::PDMCC26XX_I2S_open().
 */
typedef struct {
    uint8_t                             blockCount;             /*!< Number of PDM buffers the I2S driver can fill without the PDM driver processing them. Must be larger than 3. */
    uint32_t                            requestTimeout;         /*!< Timeout for the request when in blocking mode */
    PDMCC26XX_I2S_RequestMode           requestMode;            /*!< Blocking or Callback mode */
    PDMCC26XX_I2S_TransferSize          blockSizeInSamples;     /*!< I2S DMA transfer size in number of samples. Each
                                                                 * sample consumes either 16 or 24 bits per channel,
                                                                 * set by ::PDMCC26XX_I2S_AudioFormatConfig.memLen. Number
                                                                 * of channels are set in
                                                                 * ::PDMCC26XX_I2S_AudioPinConfig.ad0NumOfChannels and
                                                                 * ::PDMCC26XX_I2S_AudioPinConfig.ad1NumOfChannels
                                                                 */
    PDMCC26XX_I2S_CallbackFxn           callbackFxn;            /*!< Callback function pointer */

    PDMCC26XX_I2S_MallocFxn             mallocFxn;              /*!< Malloc function pointer */
    PDMCC26XX_I2S_FreeFxn               freeFxn;                /*!< Free function pointer */
    PDMCC26XX_I2S_StreamNotification    *currentStream;         /*!< Pointer to information about the current state of the stream */
} PDMCC26XX_I2S_Params;

/*!
 *  @brief  PDMCC26XX_I2S Object
 *
 *  The application must not access any member variables of this structure!
 */
typedef struct {
    bool                                isOpen;                 /*!< Has the object been opened */
    uint8_t                             blockCount;             /*!< Number of PDM buffers the I2S driver can fill without the PDM driver processing them. Must be larger than 3. */
    uint16_t                            blockSizeInBytes;       /*!< Size of an individual PDM block buffer in bytes */
    PDMCC26XX_I2S_RequestMode           requestMode;            /*!< Blocking or return mode */
    uint32_t                            requestTimeout;         /*!< Timeout for the request when in blocking mode */
    int32_t                             sampleRate;          /*!< I2S bit clock frequency in Hz. If negative, or not one of I2S_SAMPLE_RATE_16K/_24K/_32K/_48K then use user configured clock division.*/
    PIN_Handle                          pinHandle;              /*!< PIN driver handle */
    PDMCC26XX_I2S_TransferSize          blockSizeInSamples;     /*!< I2S DMA transfer size, determines the block size in number of samples. Each sample consumes either 16 or 24 bits,
                                                                 *  set by ::PDMCC26XX_I2S_AudioFormatConfig.memLen
                                                                 */
    PDMCC26XX_I2S_CallbackFxn           callbackFxn;            /*!< Callback function pointer */
    PDMCC26XX_I2S_MallocFxn             mallocFxn;              /*!< Malloc function pointer */
    PDMCC26XX_I2S_FreeFxn               freeFxn;                /*!< Free function pointer */
    PDMCC26XX_I2S_StreamNotification    *currentStream;         /*!< Ptr to information about the current transaction*/
    PDMCC26XX_I2S_AudioFormatConfig     audioFmtCfg;            /*!< I2S audio format configuration */
    PDMCC26XX_I2S_AudioPinConfig        audioPinCfg;            /*!< I2S pin configuration*/
    HwiP_Struct hwi;                    /*!< Hwi object handle */
    SemaphoreP_Struct                    blockComplete;          /*!< Notify complete PDMCC26XX_I2S block transfer */
    SemaphoreP_Struct                    semStopping;            /*!< PDMCC26XX_I2S stopping sequence semaphore */
    PIN_State                           pinState;               /*!< PIN driver state object */
    PDMCC26XX_I2S_AudioClockConfig      audioClkCfg;            /*!< I2S clock division override and clock config */
} PDMCC26XX_I2S_Object;

/*!
 *  @brief I2S CC26XX initialization
 *
 *  @param handle  A PDMCC26XX_I2S_Handle
 *
 */
extern void PDMCC26XX_I2S_init(PDMCC26XX_I2S_Handle handle);

/*!
 *  @brief  Function to open a given CC26XX I2S peripheral specified by the
 *          I2S handle.
 *
 *  The function will set a dependency on its power domain, i.e. power up the
 *  module and enable the clock. The IOs are allocated. The I2S will not be
 *  enabled.
 *
 *  \b params must point a correctly initialized PDMCC26XX_I2S_Params struct.
 *
 *  @pre    I2S controller has been initialized
 *
 *  @param  handle        A PDMCC26XX_I2S_Handle
 *
 *  @param  params        Pointer to a parameter block, if NULL it will use
 *                        default values
 *
 *  @return A PDMCC26XX_I2S_Handle on success or a NULL on an error or if it has been
 *          already opened
 *
 *  @sa     PDMCC26XX_I2S_close()
 */
extern PDMCC26XX_I2S_Handle  PDMCC26XX_I2S_open(PDMCC26XX_I2S_Handle handle, PDMCC26XX_I2S_Params *params);

/*!
 *  @brief  Function to close a given CC26XX I2S peripheral specified by the
 *          I2S handle.
 *
 *  Will disable the I2S, disable all I2S interrupts and release the
 *  dependency on the corresponding power domain. It will also destroy all
 *  semaphores and queues that have been used.
 *
 *  @pre    PDMCC26XX_I2S_open() has to be called first.
 *
 *  @param  handle  A PDMCC26XX_I2S_Handle returned from PDMCC26XX_I2S_open()
 *
 *  @sa     PDMCC26XX_I2S_open
 */
extern void PDMCC26XX_I2S_close(PDMCC26XX_I2S_Handle handle);

/*!
 *  @brief  Function for starting an I2S interface.
 *
 *  Calling this function will prevent the device from sleeping, as the
 *  stream is continuous and thus require power to the audio interface module
 *  from start to end.
 *  This function will configure all hardware registers that does not have
 *  retention. Hence, one may call PDMCC26XX_I2S_open then go to sleep before this
 *  function is called.
 *
 *  If non-blocking mode is selected then the PDMCC26XX_I2S module will begin
 *  calling the provided callback function every time a new buffer is ready.
 *
 *  @pre    PDMCC26XX_I2S_open() has to be called first.
 *
 *  @param  handle An I2S handle returned from PDMCC26XX_I2S_open()
 *
 *  @return True if transfer is successful and false if not
 *
 *  @sa     PDMCC26XX_I2S_open(), PDMCC26XX_I2S_stopStream()
 */
extern bool PDMCC26XX_I2S_startStream(PDMCC26XX_I2S_Handle handle);

/*!
 *  @brief  Function for stopping an I2S interface.
 *
 *  This function will initiate the shut down sequence. The audio interface
 *  module is designed with a graceful shutdown. What this means is that the
 *  current buffer is filled before stopping. This function will block while
 *  the last buffer is completing. The maximum blocking delay is a function
 *  of the configured DMA transfer size, and the word clock rate.
 *
 *  When this function returns it is recommended to complete processing of
 *  all pending ready buffers. If the caller is not interested in the last
 *  audio data it may simply call PDMCC26XX_I2S_requestBuffer() and
 *  PDMCC26XX_I2S_releaseBuffer() in a loop until PDMCC26XX_I2S_requestBuffer() returns
 *  false.
 *
 *  Will disable the I2S, disable all I2S interrupts and release the
 *  dependency on the corresponding power domain.
 *
 *  @pre    PDMCC26XX_I2S_startStream() has to be called first.
 *
 *  @param  handle An I2S handle returned from PDMCC26XX_I2S_open()
 *
 *  @return True if stream stopped successfully and false if not
 *
 *  @sa     PDMCC26XX_I2S_open(), PDMCC26XX_I2S_startStream()
 */
extern bool PDMCC26XX_I2S_stopStream(PDMCC26XX_I2S_Handle handle);

/*!
 *  @brief  Function for requesting buffer.
 *
 *  In ::PDMCC26XX_I2S_MODE_BLOCKING, PDMCC26XX_I2S_requestBuffer will block task
 *  execution until at least one buffer is ready.
 *
 *  In ::PDMCC26XX_I2S_CALLBACK_MODE, PDMCC26XX_I2S_requestBuffer returns immediately
 *  if no buffer is available. The caller is notified through events each time
 *  a buffer is available.
 *
 *  This function takes as an argument a pointer to a struct which contains
 *  4 pointers. There are two pairs; one for Input and one for Output.
 *  If input is defined then the input buffer pointer will point to the
 *  new input buffer. Same goes for output and the output buffer pointer.
 *  The caller is not expected to allocate memory for the buffer as no copy
 *  is performed. The other two pointers must be maintained by the caller until
 *  it is ready to call PDMCC26XX_I2S_releaseBuffer().
 *
 *  @pre    PDMCC26XX_I2S_open() and PDMCC26XX_I2S_startStream() has to be called first.
 *
 *  @param  handle A I2S handle returned from PDMCC26XX_I2S_open()
 *
 *  @param  *bufferRequest Pointer to PDMCC26XX_I2S_BufferRequest struct
 *
 *  @return True if a buffer is available, false if not.
 *
 *  @sa     PDMCC26XX_I2S_open(), PDMCC26XX_I2S_startStream(), PDMCC26XX_I2S_releaseBuffer()
 */
extern bool PDMCC26XX_I2S_requestBuffer(PDMCC26XX_I2S_Handle handle, PDMCC26XX_I2S_BufferRequest *bufferRequest);

/*!
 *  @brief  Function for releasing buffer.
 *
 *  The caller of PDMCC26XX_I2S_requestBuffer() must call this function when it is
 *  finished working on the buffer. This function takes as an argument a pointer
 *  to a struct which contains two other pointers. These pointers must be set
 *  the pointers received in PDMCC26XX_I2S_requestBuffer().
 *
 *  @pre    PDMCC26XX_I2S_requestBuffer() has to be called first.
 *
 *  @param  handle A I2S handle returned from PDMCC26XX_I2S_open()
 *
 *  @param  *bufferRelease Pointer to PDMCC26XX_I2S_BufferRelease struct
 *
 *  @return True if release is successful and false if not
 *
 *  @sa     PDMCC26XX_I2S_open(), PDMCC26XX_I2S_startStream(), PDMCC26XX_I2S_requestBuffer()
 */
extern void PDMCC26XX_I2S_releaseBuffer(PDMCC26XX_I2S_Handle handle, PDMCC26XX_I2S_BufferRelease *bufferRelease);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_i2s_PDMCC26XX_I2S__include */
