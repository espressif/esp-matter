/*
 * Copyright (c) 2015-2021, Texas Instruments Incorporated
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
 *  @file       UDMACC26XX.h
 *
 *  @brief      UDMACC26XX driver implementation.
 *
 * # Driver include #
 *  The UDMACC26XX header file should be included in an application as follows:
 *  @code
 *  #include <ti/drivers/dma/UDMACC26XX.h>
 *  @endcode
 *
 * # Overview #
 * The UDMACC26XX driver currently only supports internal use by the drivers
 * that use the uDMA peripheral (e.g., SPICC26XXDMA).
 * In other words, the application should never call any of the functions in this file.
 *
 * # General Behavior #
 * This driver is used implicitly by other drivers (e.g., the SPICC26XXDMA
 * driver) so users should not have to interface to this driver from the
 * application.
 * The uDMA HW makes use of a control table in RAM which must be 1024 bytes aligned.
 * The default base address of this control table is 0x20000400, however this
 * can be changed by simply changing UDMACC26XX_CONFIG_BASE.
 * The SPICC26XXDMA.h supports SPI0 and SPI1, and uses both TX and RX DMA channels.
 * Each control table entry is 16 bytes, so if an application uses both SSI0 and SSI1
 * the total RAM usage will be 4*16=64 bytes. If only one SSI module is used
 * only 2*16=32 bytes of RAM is used. Please see [Use cases] (@ref USE_CASE) for example.
 *
 * # Error handling #
 * Error handling is handled by the overlying driver which uses the DMA.
 *
 * # Power management #
 * Power management is handled by the overlying driver which uses the DMA.
 *
 * # Supported functions #
 * Note that these functions should never be called from the application, they
 * are only called from other drivers. They are however included here for completeness:
 *
 * | API function             | Description                                                    |
 * |------------------------- |----------------------------------------------------------------|
 * | UDMACC26XX_open()        | Initialize and enable the uDMA HW and set system dependencies. |
 * | UDMACC26XX_close()       | Disable uUDMA HW and release system dependencies               |
 *
 * @note These functions should not be called by code. These functions are called
 *       by drivers who're using the DMA.
 *
 * # Unsupported Functionality #
 * No known limitations
 *
 * # Use Cases @anchor USE_CASE #
 * The DMA is only used internally by other drivers, so the application
 * should never call any of the functions in this driver directly.
 * The only thing that the application is allowed to modify is the base address
 * of the DMA control table in RAM. (Default value is 0x2000_0400)
 * Remember it must be 1024 bytes aligned.
 * @code
 * #define UDMACC26XX_CONFIG_BASE 0x2000_0400
 * @endcode
 *
 * - If only SSI0 is used, this will allocate 2*16=32 RAM bytes at address:\n
 * [0x2000_0430-0x2000_044F] = SSI0 RX/TX DMA channels
 * - If only SSI1 is used, this will allocate 2*16=32 RAM bytes at address:\n
 * [0x2000_0500-0x2000_051F] = SSI1 RX/TX DMA channels
 * - If both SSI0 and SSI1 are used, this will allocate 4*16=64 RAM bytes at addresses:\n
 * [0x2000_0430-0x2000_044F] = SSI0 RX/TX DMA channels\n
 * [0x2000_0500-0x2000_051F] = SSI1 RX/TX DMA channels
 *
 * ============================================================================
 */

#ifndef ti_drivers_UDMACC26XX__include
#define ti_drivers_UDMACC26XX__include

#include <stdint.h>
#include <stdbool.h>

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26XX.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_types.h)
#include DeviceFamily_constructPath(driverlib/udma.h)

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @addtogroup DMA_STATUS
 *  UDMACC26XX_STATUS_* macros are command codes only defined in the
 *  UDMACC26XX.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/dma/UDMACC26XX.h>
 *  @endcode
 *  @{
 */

/* Add DMACC26XX_STATUS_* macros here */

/** @}*/

/**
 *  @addtogroup DMA_CMD
 *  UDMACC26XX_CMD_* macros are command codes only defined in the
 *  UDMACC26XX.h driver implementation and need to:
 *  @code
 *  #include <ti/drivers/dma/UDMACC26XX.h>
 *  @endcode
 *  @{
 */

/* Add DMACC26XX_CMD_* macros here */

/** @}*/

/*! Base address for the DMA control table, must be 1024 bytes aligned */
#if !defined(UDMACC26XX_CONFIG_BASE) && \
    (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X2_CC26X2)
    /* On CC13X2, CC13X2X7, CC26X2, and CC26X2X7 devices, the uDMA table needs
     * to be offset a few kB since the ROM area of SRAM is placed at the start
     * of SRAM on those devices.
     */
    #define UDMACC26XX_CONFIG_BASE 0x20001800
#elif !defined(UDMACC26XX_CONFIG_BASE) && \
      (DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X1_CC26X1 || \
       DeviceFamily_PARENT == DeviceFamily_PARENT_CC13X4_CC26X3_CC26X4)
    /* Since there is no ROM area of SRAM on the CC13X1, CC26X1, CC13X4, and
     * CC26X4 devices, we can move the uDMA table closer to the start of SRAM.
     * This improves the linker efficiency when using dynamically sized heaps.
     */
    #define UDMACC26XX_CONFIG_BASE 0x20000400
#elif !defined(UDMACC26XX_CONFIG_BASE)
    #define UDMACC26XX_CONFIG_BASE 0x20000400
#endif

/*! Make sure DMA control table base address is 1024 bytes aligned */
#if(UDMACC26XX_CONFIG_BASE & 0x3FF)
    #error "Base address for DMA control table 'UDMACC26XX_CONFIG_BASE' must be 1024 bytes aligned."
#endif

/*! Compiler specific macros to allocate DMA control table entries */
#if defined(__IAR_SYSTEMS_ICC__)
#define ALLOCATE_CONTROL_TABLE_ENTRY(ENTRY_NAME, CHANNEL_INDEX) \
__no_init static volatile tDMAControlTable ENTRY_NAME @ UDMACC26XX_CONFIG_BASE + CHANNEL_INDEX * sizeof(tDMAControlTable)
#elif defined(__TI_COMPILER_VERSION__) || defined(__clang__)
#define ALLOCATE_CONTROL_TABLE_ENTRY(ENTRY_NAME, CHANNEL_INDEX) \
static volatile tDMAControlTable ENTRY_NAME __attribute__((retain, location(\
    UDMACC26XX_CONFIG_BASE + CHANNEL_INDEX * sizeof(tDMAControlTable))))
#elif defined(__GNUC__)
#define ALLOCATE_CONTROL_TABLE_ENTRY(ENTRY_NAME, CHANNEL_INDEX) \
    extern int UDMACC26XX_ ## ENTRY_NAME ## _is_placed; __attribute__ ((section("."#ENTRY_NAME))) static volatile tDMAControlTable ENTRY_NAME = {&UDMACC26XX_ ## ENTRY_NAME ## _is_placed}
#else
#error "don't know how to define ALLOCATE_CONTROL_TABLE_ENTRY for this toolchain"
#endif

/*! Sets the DMA transfer size in number of items */
#define UDMACC26XX_SET_TRANSFER_SIZE(SIZE)    (((SIZE - 1) << UDMA_XFER_SIZE_S) & UDMA_XFER_SIZE_M)
/*! Gets the DMA transfer size in number of items*/
#define UDMACC26XX_GET_TRANSFER_SIZE(CONTROL) (((CONTROL & UDMA_XFER_SIZE_M) >> UDMA_XFER_SIZE_S) + 1)

/*!
 *  @brief  UDMACC26XX object
 */
typedef struct {
    bool             isOpen;           /*!< Flag for open/close status */
    HwiP_Struct hwi;  /*!< Embedded Hwi Object */
} UDMACC26XX_Object;

/*!
 *  @brief  UDMACC26XX hardware attributes
 */
typedef struct {
    uint32_t        baseAddr;    /*!< Base adddress for UDMACC26XX */
    PowerCC26XX_Resource  powerMngrId; /*!< UDMACC26XX Peripheral's power manager ID */
    uint8_t         intNum;      /*!< UDMACC26XX error interrupt number */
    /*! @brief UDMACC26XX error interrupt priority.
     *  intPriority is the DMA peripheral's interrupt priority, as
     *  defined by the underlying OS.  It is passed unmodified to the
     *  underlying OS's interrupt handler creation code, so you need to
     *  refer to the OS documentation for usage.  If the
     *  driver uses the ti.dpl interface instead of making OS
     *  calls directly, then the HwiP port handles the interrupt priority
     *  in an OS specific way.  In the case of the SYS/BIOS port,
     *  intPriority is passed unmodified to Hwi_create().
     *
     *  The CC26xx uses three of the priority bits,
     *  meaning ~0 has the same effect as (7 << 5).
     *
     *  (7 << 5) will apply the lowest priority.
     *
     *  (1 << 5) will apply the highest priority.
     *
     *  Setting the priority to 0 is not supported by this driver.
     *
     *  HWI's with priority 0 ignore the HWI dispatcher to support zero-latency interrupts, thus invalidating the critical sections in this driver.
     */
    uint8_t         intPriority;
} UDMACC26XX_HWAttrs;

/*!
 *  @brief      UDMACC26XX Global configuration
 */
typedef struct {
    void              *object;            /*!< Pointer to UDMACC26XX object */
    void const        *hwAttrs;           /*!< Pointer to hardware attribute */
} UDMACC26XX_Config;

/*!
 *  @brief      A handle that is returned from a UDMACC26XX_open() call.
 */
typedef UDMACC26XX_Config *UDMACC26XX_Handle;

/* Extern'd hwiIntFxn */
extern void UDMACC26XX_hwiIntFxn(uintptr_t callbacks);

/*!
 *  @brief  Function to initialize the CC26XX DMA driver
 *
 *  The function will set the isOpen flag to false, and should be called prior
 *  to opening the DMA driver.
 *
 *  @pre    Calling context: Hwi, Swi, Task
 *
 *  @return none
 *
 *  @sa     UDMACC26XX_open()
 */
__STATIC_INLINE void UDMACC26XX_init(UDMACC26XX_Handle handle)
{
    UDMACC26XX_Object           *object;

    /* Get the pointer to the object */
    object = (UDMACC26XX_Object *)(handle->object);

    /* mark the module as available */
    object->isOpen = false;
}

/*!
 *  @brief  Function to initialize the CC26XX DMA peripheral
 *
 *  The function will set a dependency on the peripheral power domain, i.e. power up the
 *  module and enable the clock.
 *  Note this function always uses the first DMA entry in the global UDMACC26XX_config list.
 *
 *  @pre    UDMACC26XX_init() has to be called first.
 *          Calling context: Task
 *
 *  @return UDMACC26XX_Handle on success or NULL if error or if it has been
 *          already opened
 *
 *  @sa     UDMACC26XX_close()
 */
extern UDMACC26XX_Handle UDMACC26XX_open();

/*!
 *  @internal
 *  @brief  Function to enable a given DMA channel
 *
 *  @pre    UDMACC26XX_open() has to be called first.
 *          Calling context: Hwi, Swi, Task
 *
 *  @param  handle  A UDMACC26XX_Handle returned from UDMACC26XX_open()
 *
 *  @param  params  A 32-bit bitmask of the channels to enable.
 *
 *  @sa     UDMACC26XX_channelDisable
 */
__STATIC_INLINE void UDMACC26XX_channelEnable(UDMACC26XX_Handle handle, uint32_t channelBitMask)
{
    UDMACC26XX_HWAttrs const *hwAttrs;

    /* Get the pointer to the hwAttrs */
    hwAttrs = (UDMACC26XX_HWAttrs *)(handle->hwAttrs);

    /* Enable DMA channel */
    HWREG(hwAttrs->baseAddr + UDMA_O_SETCHANNELEN) = channelBitMask;
}

/*!
 *  @internal
 *  @brief  Function to see if a given DMA channel is done.
 *
 *  Will read the request done signal for the give channels
 *  and return true if all channels are done, otherwise false.
 *
 *  @pre    UDMACC26XX_open() has to be called first.
 *          Calling context: Hwi, Swi, Task
 *
 *  @param  handle  A UDMACC26XX_Handle returned from UDMACC26XX_open()
 *
 *  @param  params  A 32-bit bitmask of the channels to check for if are done.
 *
 *  @return True if the channels are done, false otherwise.
 *
 *  @sa     UDMACC26XX_open, UDMACC26XX_channelDisable
 */
__STATIC_INLINE bool UDMACC26XX_channelDone(UDMACC26XX_Handle handle, uint32_t channelBitMask)
{
    UDMACC26XX_HWAttrs const *hwAttrs;

    /* Get the pointer to the hwAttrs */
    hwAttrs = (UDMACC26XX_HWAttrs *)(handle->hwAttrs);

    /* Check if REQDONE is set for a specific channel */
    return (uDMAIntStatus(hwAttrs->baseAddr) & channelBitMask) ? true : false;
}

/*!
 *  @internal
 *  @brief  Function to clear a given DMA channel interrupt.
 *
 *  Will clear the DMA interrupt(s) for the given bitmask provided.
 *
 *  @pre    UDMACC26XX_open() has to be called first.
 *          Calling context: Hwi, Swi, Task
 *
 *  @param  handle  A UDMACC26XX_Handle returned from UDMACC26XX_open()
 *
 *  @param  params  A 32-bit bitmask of the channels to check for if are done.
 *
 *  @param  channelBitMask  A 32-bit bitmask of the channels to clear interrupts for.
 *
 *  @return none
 */
__STATIC_INLINE void UDMACC26XX_clearInterrupt(UDMACC26XX_Handle handle, uint32_t channelBitMask)
{
    UDMACC26XX_HWAttrs const *hwAttrs;

    /* Get the pointer to the hwAttrs and object */
    hwAttrs = (UDMACC26XX_HWAttrs *)(handle->hwAttrs);

    /* Clear UDMA done interrupt */
    uDMAIntClear(hwAttrs->baseAddr, channelBitMask);
}

/*!
 *  @internal
 *  @brief  Function to disable one or more DMA channels.
 *
 *  Will disable the channel(s) for the given bitmask provided.
 *
 *  @pre    UDMACC26XX_open() has to be called first.
 *          Calling context: Hwi, Swi, Task
 *
 *  @param  handle  A UDMACC26XX_Handle returned from UDMACC26XX_open()
 *
 *  @param  params  A 32-bit bitmask of the channels to disable.
 *
 *  @return none
 *
 *  @sa     UDMACC26XX_channelEnable
 */
__STATIC_INLINE void UDMACC26XX_channelDisable(UDMACC26XX_Handle handle, uint32_t channelBitMask)
{
    UDMACC26XX_HWAttrs const *hwAttrs;

    /* Get the pointer to the hwAttrs */
    hwAttrs = (UDMACC26XX_HWAttrs *)(handle->hwAttrs);

    HWREG(hwAttrs->baseAddr + UDMA_O_CLEARCHANNELEN) = channelBitMask;
}

/*!
 *  @internal
 *  @brief  Function to disable a DMA channel's attributes.
 *
 *  Will disable a channel's attributes.
 *
 *  @pre    UDMACC26XX_open() has to be called first.
 *          Calling context: Hwi, Swi, Task
 *
 *  @param  handle  A UDMACC26XX_Handle returned from UDMACC26XX_open()
 *
 *  @param  channelNum  the channel to configure.
 *
 *  @param  attr  Channel attribute to disable.
 *
 *
 *  @return none
 *
 *  @sa     UDMACC26XX_channelEnable
 */
__STATIC_INLINE void UDMACC26XX_disableAttribute(UDMACC26XX_Handle handle,
    uint32_t channelNum, uint32_t attr)
{
    UDMACC26XX_HWAttrs const *hwAttrs;

    /* Get the pointer to the hwAttrs */
    hwAttrs = (UDMACC26XX_HWAttrs *)(handle->hwAttrs);

    uDMAChannelAttributeDisable(hwAttrs->baseAddr, channelNum, attr);
}

/*!
 *  @brief  Function to close the DMA driver.
 *
 *  Will disable the DMA hardware, release the power dependency and destruct
 *  the HWI interrupt.
 *
 *  @pre    UDMACC26XX_open() has to be called first.
 *          Calling context: Task
 *
 *  @param  handle  A UDMACC26XX_Handle returned from UDMACC26XX_open()
 *
 *  @return none
 *
 *  @sa     UDMACC26XX_open
 */
extern void UDMACC26XX_close(UDMACC26XX_Handle handle);

#ifdef __cplusplus
}
#endif

#endif /* ti_drivers_UDMACC26XX__include */
