/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2019, NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_INPUTMUX_CONNECTIONS_
#define _FSL_INPUTMUX_CONNECTIONS_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.inputmux_connections"
#endif

/*!
 * @addtogroup inputmux_driver
 * @{
 */

/*! @brief Periphinmux IDs */
#define SCT0_PMUX_ID 0x00U
#define PINTSEL_PMUX_ID 0x100U
#define DSP_INT_PMUX_ID 0x140U
#define DMA0_ITRIG_PMUX_ID 0x200U
#define DMA0_OTRIG_PMUX_ID 0x300U
#define DMA0_CHMUX_SEL0_ID 0x320U
#define DMA1_ITRIG_PMUX_ID 0x400U
#define DMA1_OTRIG_PMUX_ID 0x500U
#define DMA1_CHMUX_SEL0_ID 0x520U
#define CT32BIT_CAP_PMUX_ID 0x600U
#define FREQMEAS_PMUX_ID 0x700U
#define SMART_DMA_PMUX_ID 0x720U
#define DMA0_REQ_ENA0_ID 0x740U
#define DMA1_REQ_ENA0_ID 0x760U
#define DMA0_ITRIG_EN0_ID 0x780U
#define DMA1_ITRIG_EN0_ID 0x7A0U

#define ENA_SHIFT 5U
#define PMUX_SHIFT 20U
#define CHMUX_AVL_SHIFT 31U
#define CHMUX_OFF_SHIFT 19U
#define CHMUX_VAL_SHIFT 17U

/*! @brief INPUTMUX connections type */
typedef enum _inputmux_connection_t
{
    /*!< SCT INMUX. */
    kINPUTMUX_Sct0PinInp0ToSct0     = 0U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Sct0PinInp1ToSct0     = 1U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Sct0PinInp2ToSct0     = 2U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Sct0PinInp3ToSct0     = 3U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Sct0PinInp4ToSct0     = 4U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Sct0PinInp5ToSct0     = 5U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Sct0PinInp6ToSct0     = 6U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Sct0PinInp7ToSct0     = 7U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer0Mat0ToSct0     = 8U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer1Mat0ToSct0     = 9U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer2Mat0ToSct0     = 10U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer3Mat0ToSct0     = 11U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer4Mat0ToSct0     = 12U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_AdcIrqToSct0          = 13U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioIntBmatchToSct0   = 14U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Usb0FrameToggleToSct0 = 15U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Cmp0OutToSct0         = 16U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_SharedI2s0SclkToSct0  = 17U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_SharedI2s1SclkToSct0  = 18U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_SharedI2s0WsToSct0    = 19U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_SharedI2s1WsToSct0    = 20U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_MclkToSct0            = 21U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_ArmTxevToSct0         = 22U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DebugHaltedToSct0     = 23U + (SCT0_PMUX_ID << PMUX_SHIFT),

    /*!< Pin Interrupt. */
    kINPUTMUX_GpioPort0Pin0ToPintsel  = 0U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin1ToPintsel  = 1U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin2ToPintsel  = 2U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin3ToPintsel  = 3U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin4ToPintsel  = 4U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin5ToPintsel  = 5U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin6ToPintsel  = 6U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin7ToPintsel  = 7U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin8ToPintsel  = 8U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin9ToPintsel  = 9U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin10ToPintsel = 10U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin11ToPintsel = 11U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin12ToPintsel = 12U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin13ToPintsel = 13U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin14ToPintsel = 14U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin15ToPintsel = 15U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin16ToPintsel = 16U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin17ToPintsel = 17U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin18ToPintsel = 18U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin19ToPintsel = 19U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin20ToPintsel = 20U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin21ToPintsel = 21U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin22ToPintsel = 22U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin23ToPintsel = 23U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin24ToPintsel = 24U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin25ToPintsel = 25U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin26ToPintsel = 26U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin27ToPintsel = 27U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin28ToPintsel = 28U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin29ToPintsel = 29U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin30ToPintsel = 30U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin31ToPintsel = 31U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin0ToPintsel  = 32U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin1ToPintsel  = 33U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin2ToPintsel  = 34U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin3ToPintsel  = 35U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin4ToPintsel  = 36U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin5ToPintsel  = 37U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin6ToPintsel  = 38U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin7ToPintsel  = 39U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin8ToPintsel  = 40U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin9ToPintsel  = 41U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin10ToPintsel = 42U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin11ToPintsel = 43U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin12ToPintsel = 44U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin13ToPintsel = 45U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin14ToPintsel = 46U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin15ToPintsel = 47U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin16ToPintsel = 48U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin17ToPintsel = 49U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin18ToPintsel = 50U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin19ToPintsel = 51U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin20ToPintsel = 52U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin21ToPintsel = 53U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin22ToPintsel = 54U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin23ToPintsel = 55U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin24ToPintsel = 56U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin25ToPintsel = 57U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin26ToPintsel = 58U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin27ToPintsel = 59U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin28ToPintsel = 60U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin29ToPintsel = 61U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin30ToPintsel = 62U + (PINTSEL_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin31ToPintsel = 63U + (PINTSEL_PMUX_ID << PMUX_SHIFT),

    /*!< DSP Interrupt. */
    kINPUTMUX_Flexcomm0ToDspInterrupt    = 0U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexcomm1ToDspInterrupt    = 1U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexcomm2ToDspInterrupt    = 2U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexcomm3ToDspInterrupt    = 3U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexcomm4ToDspInterrupt    = 4U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexcomm5ToDspInterrupt    = 5U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexcomm6ToDspInterrupt    = 6U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexcomm7ToDspInterrupt    = 7U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexcomm14ToDspInterrupt   = 8U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexcomm16ToDspInterrupt   = 9U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt0ToDspInterrupt     = 10U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt1ToDspInterrupt     = 11U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt2ToDspInterrupt     = 12U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt3ToDspInterrupt     = 13U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt4ToDspInterrupt     = 14U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt5ToDspInterrupt     = 15U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt6ToDspInterrupt     = 16U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt7ToDspInterrupt     = 17U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_NsHsGpioInt0ToDspInterrupt = 18U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_NsHsGpioInt1ToDspInterrupt = 19U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Wdt1ToDspInterrupt         = 20U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dmac0ToDspInterrupt        = 21U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dmac1ToDspInterrupt        = 22U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_MuBToDspInterrupt          = 23U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Utick0ToDspInterrupt       = 24U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Mrt0ToDspInterrupt         = 25U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_OsEventTimerToDspInterrupt = 26U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer0ToDspInterrupt      = 27U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer1ToDspInterrupt      = 28U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer2ToDspInterrupt      = 29U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer3ToDspInterrupt      = 30U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer4ToDspInterrupt      = 31U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_RtcToDspInterrupt          = 32U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_I3c0ToDspInterrupt         = 33U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_I3c1ToDspInterrupt         = 34U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dmic0ToDspInterrupt        = 35U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_HwvadToDspInterrupt        = 36U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_LcdifToDspInterrupt        = 37U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpuToDspInterrupt          = 38U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_SmartDmaToDspInterrupt     = 39U + (DSP_INT_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_FlexioToDspInterrupt       = 40U + (DSP_INT_PMUX_ID << PMUX_SHIFT),

    /*!< Frequency measure. */
    kINPUTMUX_XtalinToFreqmeas        = 0U + (FREQMEAS_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Fro12mToFreqmeas        = 1U + (FREQMEAS_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Fro192mToFreqmeas       = 2U + (FREQMEAS_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_LposcToFreqmeas         = 3U + (FREQMEAS_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_32KhzOscToFreqmeas      = 4U + (FREQMEAS_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_MainSysClkToFreqmeas    = 5U + (FREQMEAS_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_FreqmeGpioClkToFreqmeas = 6U + (FREQMEAS_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_ClockOutToFreqmeas      = 11U + (FREQMEAS_PMUX_ID << PMUX_SHIFT),

    /*!< SMARTDMA input mux. */
    kINPUTMUX_GpioPort0Pin0ToSmartDmaInput   = 0U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin1ToSmartDmaInput   = 1U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin2ToSmartDmaInput   = 2U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin3ToSmartDmaInput   = 3U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin4ToSmartDmaInput   = 4U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin5ToSmartDmaInput   = 5U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin6ToSmartDmaInput   = 6U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort0Pin7ToSmartDmaInput   = 7U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin0ToSmartDmaInput   = 8U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin1ToSmartDmaInput   = 9U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin2ToSmartDmaInput   = 10U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin3ToSmartDmaInput   = 11U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin4ToSmartDmaInput   = 12U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin5ToSmartDmaInput   = 13U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin6ToSmartDmaInput   = 14U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioPort1Pin7ToSmartDmaInput   = 15U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexcomm0IrqToSmartDmaInput    = 16U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexcomm1IrqToSmartDmaInput    = 17U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexcomm2IrqToSmartDmaInput    = 18U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexcomm3IrqToSmartDmaInput    = 19U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexcomm4IrqToSmartDmaInput    = 20U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexcomm5IrqToSmartDmaInput    = 21U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexcomm6IrqToSmartDmaInput    = 22U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexcomm7IrqToSmartDmaInput    = 23U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexcomm14IrqToSmartDmaInput   = 24U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexcomm16IrqToSmartDmaInput   = 25U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_I3c0IrqToSmartDmaInput         = 26U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_I3c1IrqToSmartDmaInput         = 27U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_FlexioIrqToSmartDmaInput       = 28U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt0Irq0ToSmartDmaInput    = 29U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt0Irq1ToSmartDmaInput    = 30U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt0Irq2ToSmartDmaInput    = 31U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt0Irq3ToSmartDmaInput    = 32U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt0Irq4ToSmartDmaInput    = 33U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt0Irq5ToSmartDmaInput    = 34U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt0Irq6ToSmartDmaInput    = 35U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt0Irq7ToSmartDmaInput    = 36U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_NsGpioHsIrq0ToSmartDmaInput    = 37U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_NsGpioHsIrq1ToSmartDmaInput    = 38U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Sct0IrqToSmartDmaInput         = 39U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer0IrqToSmartDmaInput      = 40U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer1IrqToSmartDmaInput      = 41U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer2IrqToSmartDmaInput      = 42U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer3IrqToSmartDmaInput      = 43U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer4IrqToSmartDmaInput      = 44U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Utick0IrqToSmartDmaInput       = 45U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Mrt0IrqToSmartDmaInput         = 46U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_RtcLite0IrqToSmartDmaInput     = 47U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_OsEventIrqToSmartDmaInput      = 48U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Wdt0IrqToSmartDmaInput         = 49U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Wdt1IrqToSmartDmaInput         = 50U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Adc0IrqToSmartDmaInput         = 51U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_AcmpIrqToSmartDmaInput         = 52U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dmic0ToSmartDmaInput           = 53U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_HwvadToSmartDmaInput           = 54U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Sdio0IrqToSmartDmaInput        = 55U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Sdio1IrqToSmartDmaInput        = 56U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Usb0IrqToSmartDmaInput         = 57U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Usb0NeedClkToSmartDmaInput     = 58U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_LcdifIrqToSmartDmaInput        = 59U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpuIrqToSmartDmaInput          = 60U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0IrqToSmartDmaInput         = 61U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1IrqToSmartDmaInput         = 62U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_PowerquadIrqToSmartDmaInput    = 63U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_FlexspiIrqToSmartDmaInput      = 64U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DspTieExpstate1ToSmartDmaInput = 65U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_SctOut8ToSmartDmaInput         = 66U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_SctOut9ToSmartDmaInput         = 67U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_T4Mat3ToSmartDmaInput          = 68U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_T4Mat2ToSmartDmaInput          = 69U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_T3Mat3ToSmartDmaInput          = 70U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_T3Mat2ToSmartDmaInput          = 71U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_ArmTxevToSmartDmaInput         = 72U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpiointBmatchToSmartDmaInput   = 73U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_MipiIrqToSmartDmaInput         = 74U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_UsbFsToggleToSmartDmaInput     = 75U + (SMART_DMA_PMUX_ID << PMUX_SHIFT),

    /*!< CTmier capture input mux. */
    kINPUTMUX_CtInp0ToCaptureChannels          = 0U + (CT32BIT_CAP_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_CtInp1ToCaptureChannels          = 1U + (CT32BIT_CAP_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_CtInp2ToCaptureChannels          = 2U + (CT32BIT_CAP_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_CtInp3ToCaptureChannels          = 3U + (CT32BIT_CAP_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_CtInp4ToCaptureChannels          = 4U + (CT32BIT_CAP_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_CtInp5ToCaptureChannels          = 5U + (CT32BIT_CAP_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_CtInp6ToCaptureChannels          = 6U + (CT32BIT_CAP_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_CtInp7ToCaptureChannels          = 7U + (CT32BIT_CAP_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_CtInp8ToCaptureChannels          = 8U + (CT32BIT_CAP_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_CtInp9ToCaptureChannels          = 9U + (CT32BIT_CAP_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_CtInp10ToCaptureChannels         = 10U + (CT32BIT_CAP_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_CtInp11ToCaptureChannels         = 11U + (CT32BIT_CAP_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_CtInp12ToCaptureChannels         = 12U + (CT32BIT_CAP_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_CtInp13ToCaptureChannels         = 13U + (CT32BIT_CAP_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_CtInp14ToCaptureChannels         = 14U + (CT32BIT_CAP_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_CtInp15ToCaptureChannels         = 15U + (CT32BIT_CAP_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_SharedI2s0WsToCaptureChannels    = 16U + (CT32BIT_CAP_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_SharedI2s1WsToCaptureChannels    = 17U + (CT32BIT_CAP_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Usb0FrameToggleToCaptureChannels = 18U + (CT32BIT_CAP_PMUX_ID << PMUX_SHIFT),

    /*!< DMA0 ITRIG. */
    kINPUTMUX_GpioInt0ToDma0     = 0U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt1ToDma0     = 1U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt2ToDma0     = 2U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt3ToDma0     = 3U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer0M0ToDma0    = 4U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer0M1ToDma0    = 5U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer1M0ToDma0    = 6U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer1M1ToDma0    = 7U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer2M0ToDma0    = 8U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer2M1ToDma0    = 9U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer3M0ToDma0    = 10U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer3M1ToDma0    = 11U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer4M0ToDma0    = 12U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer4M1ToDma0    = 13U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0TrigOutAToDma0 = 14U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0TrigOutBToDma0 = 15U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0TrigOutCToDma0 = 16U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0TrigOutDToDma0 = 17U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_SctDma0ToDma0      = 18U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_SctDma1ToDma0      = 19U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_HashCryptOutToDma0 = 20U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_AcmpToDma0         = 21U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexspi0RxToDma0   = 22U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexspi0TxToDma0   = 23U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_AdcToDma0          = 24U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexspi1RxToDma0   = 25U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexspi1TxToDma0   = 26U + (DMA0_ITRIG_PMUX_ID << PMUX_SHIFT),

    /*!< DMA1 ITRIG. */
    kINPUTMUX_GpioInt0ToDma1     = 0U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt1ToDma1     = 1U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt2ToDma1     = 2U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpioInt3ToDma1     = 3U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer0M0ToDma1    = 4U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer0M1ToDma1    = 5U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer1M0ToDma1    = 6U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer1M1ToDma1    = 7U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer2M0ToDma1    = 8U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer2M1ToDma1    = 9U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer3M0ToDma1    = 10U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer3M1ToDma1    = 11U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer4M0ToDma1    = 12U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Ctimer4M1ToDma1    = 13U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0TrigOutAToDma1 = 14U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0TrigOutBToDma1 = 15U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0TrigOutCToDma1 = 16U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0TrigOutDToDma1 = 17U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_SctDma0ToDma1      = 18U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_SctDma1ToDma1      = 19U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_HashCryptOutToDma1 = 20U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_AcmpToDma1         = 21U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexspi0RxToDma1   = 22U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexspi0TxToDma1   = 23U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_AdcToDma1          = 24U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexspi1RxToDma1   = 25U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Flexspi1TxToDma1   = 26U + (DMA1_ITRIG_PMUX_ID << PMUX_SHIFT),

    /*!< DMA0 OTRIG. */
    kINPUTMUX_Dma0OtrigChannel0ToTriginChannels  = 0U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel1ToTriginChannels  = 1U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel2ToTriginChannels  = 2U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel3ToTriginChannels  = 3U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel4ToTriginChannels  = 4U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel5ToTriginChannels  = 5U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel6ToTriginChannels  = 6U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel7ToTriginChannels  = 7U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel8ToTriginChannels  = 8U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel9ToTriginChannels  = 9U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel10ToTriginChannels = 10U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel11ToTriginChannels = 11U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel12ToTriginChannels = 12U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel13ToTriginChannels = 13U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel14ToTriginChannels = 14U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel15ToTriginChannels = 15U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel16ToTriginChannels = 16U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel17ToTriginChannels = 17U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel18ToTriginChannels = 18U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel19ToTriginChannels = 19U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel20ToTriginChannels = 20U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel21ToTriginChannels = 21U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel22ToTriginChannels = 22U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel23ToTriginChannels = 23U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel24ToTriginChannels = 24U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel25ToTriginChannels = 25U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel26ToTriginChannels = 26U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel27ToTriginChannels = 27U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel28ToTriginChannels = 28U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel29ToTriginChannels = 29U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel30ToTriginChannels = 30U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel31ToTriginChannels = 31U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel32ToTriginChannels = 32U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel33ToTriginChannels = 33U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel34ToTriginChannels = 34U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel35ToTriginChannels = 35U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma0OtrigChannel36ToTriginChannels = 36U + (DMA0_OTRIG_PMUX_ID << PMUX_SHIFT),

    /*!< DMA1 OTRIG. */
    kINPUTMUX_Dma1OtrigChannel0ToTriginChannels  = 0U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel1ToTriginChannels  = 1U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel2ToTriginChannels  = 2U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel3ToTriginChannels  = 3U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel4ToTriginChannels  = 4U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel5ToTriginChannels  = 5U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel6ToTriginChannels  = 6U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel7ToTriginChannels  = 7U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel8ToTriginChannels  = 8U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel9ToTriginChannels  = 9U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel10ToTriginChannels = 10U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel11ToTriginChannels = 11U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel12ToTriginChannels = 12U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel13ToTriginChannels = 13U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel14ToTriginChannels = 14U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel15ToTriginChannels = 15U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel16ToTriginChannels = 16U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel17ToTriginChannels = 17U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel18ToTriginChannels = 18U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel19ToTriginChannels = 19U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel20ToTriginChannels = 20U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel21ToTriginChannels = 21U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel22ToTriginChannels = 22U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel23ToTriginChannels = 23U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel24ToTriginChannels = 24U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel25ToTriginChannels = 25U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel26ToTriginChannels = 26U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel27ToTriginChannels = 27U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel28ToTriginChannels = 28U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel29ToTriginChannels = 29U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel30ToTriginChannels = 30U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel31ToTriginChannels = 31U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel32ToTriginChannels = 32U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel33ToTriginChannels = 33U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel34ToTriginChannels = 34U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel35ToTriginChannels = 35U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_Dma1OtrigChannel36ToTriginChannels = 36U + (DMA1_OTRIG_PMUX_ID << PMUX_SHIFT),
} inputmux_connection_t;

/*! @brief INPUTMUX signal enable/disable type */
/* Encode: [31:31]: ChannelMux available flag
 *         [30:19]: ChannelMux register offset
 *         [18:17]: ChannelMux register value
 *         [16: 5]: Signal enable register offset
 *         [ 4: 0]: Signal enable bit location
 */
typedef enum _inputmux_signal_t
{
    /*!< DMA0 input trigger source enable. */
    kINPUTMUX_Dmac0InputTriggerPint0Ena      = 0U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerPint1Ena      = 1U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerPint2Ena      = 2U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerPint3Ena      = 3U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerCtimer0M0Ena  = 4U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerCtimer0M1Ena  = 5U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerCtimer1M0Ena  = 6U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerCtimer1M1Ena  = 7U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerCtimer2M0Ena  = 8U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerCtimer2M1Ena  = 9U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerCtimer3M0Ena  = 10U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerCtimer3M1Ena  = 11U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerCtimer4M0Ena  = 12U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerCtimer4M1Ena  = 13U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerDma0OutAEna   = 14U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerDma0OutBEna   = 15U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerDma0OutCEna   = 16U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerDma0OutDEna   = 17U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerSctDmac0Ena   = 18U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerSctDmac1Ena   = 19U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerHashOutEna    = 20U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerAcmpEna       = 21U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerFlexspi0RxEna = 22U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerFlexspi0TxEna = 23U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerAdcEna        = 24U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerFlexspi1RxEna = 25U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac0InputTriggerFlexspi1TxEna = 26U + (DMA0_ITRIG_EN0_ID << ENA_SHIFT),

    /*!< DMA1 input trigger source enable. */
    kINPUTMUX_Dmac1InputTriggerPint0Ena      = 0U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerPint1Ena      = 1U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerPint2Ena      = 2U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerPint3Ena      = 3U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerCtimer0M0Ena  = 4U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerCtimer0M1Ena  = 5U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerCtimer1M0Ena  = 6U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerCtimer1M1Ena  = 7U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerCtimer2M0Ena  = 8U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerCtimer2M1Ena  = 9U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerCtimer3M0Ena  = 10U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerCtimer3M1Ena  = 11U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerCtimer4M0Ena  = 12U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerCtimer4M1Ena  = 13U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerDma1OutAEna   = 14U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerDma1OutBEna   = 15U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerDma1OutCEna   = 16U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerDma1OutDEna   = 17U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerSctDmac0Ena   = 18U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerSctDmac1Ena   = 19U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerHashOutEna    = 20U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerAcmpEna       = 21U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerFlexspi0RxEna = 22U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerFlexspi0TxEna = 23U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerAdcEna        = 24U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerFlexspi1RxEna = 25U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),
    kINPUTMUX_Dmac1InputTriggerFlexspi1TxEna = 26U + (DMA1_ITRIG_EN0_ID << ENA_SHIFT),

    /*!< DMA0 REQ signal. */
    kINPUTMUX_Flexcomm0RxToDmac0Ch0RequestEna  = 0U + (DMA0_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm0TxToDmac0Ch1RequestEna  = 1U + (DMA0_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm1RxToDmac0Ch2RequestEna  = 2U + (DMA0_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm1TxToDmac0Ch3RequestEna  = 3U + (DMA0_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm2RxToDmac0Ch4RequestEna  = 4U + (DMA0_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm2TxToDmac0Ch5RequestEna  = 5U + (DMA0_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm3RxToDmac0Ch6RequestEna  = 6U + (DMA0_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm3TxToDmac0Ch7RequestEna  = 7U + (DMA0_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm4RxToDmac0Ch8RequestEna  = 8U + (DMA0_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm4TxToDmac0Ch9RequestEna  = 9U + (DMA0_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm5RxToDmac0Ch10RequestEna = 10U + (DMA0_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm5TxToDmac0Ch11RequestEna = 11U + (DMA0_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm6RxToDmac0Ch12RequestEna = 12U + (DMA0_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm6TxToDmac0Ch13RequestEna = 13U + (DMA0_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm7RxToDmac0Ch14RequestEna = 14U + (DMA0_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm7TxToDmac0Ch15RequestEna = 15U + (DMA0_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Dmic0Ch0ToDmac0Ch16RequestEna    = 16U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                              (DMA0_CHMUX_SEL0_ID << CHMUX_OFF_SHIFT) + (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm8RxToDmac0Ch16RequestEna = 16U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 (DMA0_CHMUX_SEL0_ID << CHMUX_OFF_SHIFT) + (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Dmic0Ch1ToDmac0Ch17RequestEna = 17U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                              ((DMA0_CHMUX_SEL0_ID + 4) << CHMUX_OFF_SHIFT) + (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm8TxToDmac0Ch17RequestEna = 17U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA0_CHMUX_SEL0_ID + 4) << CHMUX_OFF_SHIFT) +
                                                 (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Dmic0Ch2ToDmac0Ch18RequestEna = 18U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                              ((DMA0_CHMUX_SEL0_ID + 8) << CHMUX_OFF_SHIFT) + (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm9RxToDmac0Ch18RequestEna = 18U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA0_CHMUX_SEL0_ID + 8) << CHMUX_OFF_SHIFT) +
                                                 (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Dmic0Ch3ToDmac0Ch19RequestEna = 19U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                              ((DMA0_CHMUX_SEL0_ID + 12) << CHMUX_OFF_SHIFT) + (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm9TxToDmac0Ch19RequestEna = 19U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA0_CHMUX_SEL0_ID + 12) << CHMUX_OFF_SHIFT) +
                                                 (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Dmic0Ch4ToDmac0Ch20RequestEna = 20U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                              ((DMA0_CHMUX_SEL0_ID + 16) << CHMUX_OFF_SHIFT) + (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm10RxToDmac0Ch20RequestEna = 20U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                  ((DMA0_CHMUX_SEL0_ID + 16) << CHMUX_OFF_SHIFT) +
                                                  (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Dmic0Ch5ToDmac0Ch21RequestEna = 21U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                              ((DMA0_CHMUX_SEL0_ID + 20) << CHMUX_OFF_SHIFT) + (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm10TxToDmac0Ch21RequestEna = 21U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                  ((DMA0_CHMUX_SEL0_ID + 20) << CHMUX_OFF_SHIFT) +
                                                  (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Dmic0Ch6ToDmac0Ch22RequestEna = 22U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                              ((DMA0_CHMUX_SEL0_ID + 24) << CHMUX_OFF_SHIFT) + (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm13RxToDmac0Ch22RequestEna = 22U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                  ((DMA0_CHMUX_SEL0_ID + 24) << CHMUX_OFF_SHIFT) +
                                                  (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Dmic0Ch7ToDmac0Ch23RequestEna = 23U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                              ((DMA0_CHMUX_SEL0_ID + 28) << CHMUX_OFF_SHIFT) + (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm13TxToDmac0Ch23RequestEna = 23U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                  ((DMA0_CHMUX_SEL0_ID + 28) << CHMUX_OFF_SHIFT) +
                                                  (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_I3c0RxToDmac0Ch24RequestEna       = 24U + (DMA0_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_I3c0TxToDmac0Ch25RequestEna       = 25U + (DMA0_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm14RxToDmac0Ch26RequestEna = 26U + (DMA0_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm14TxToDmac0Ch27RequestEna = 27U + (DMA0_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm16RxToDmac0Ch28RequestEna = 28U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                  ((DMA0_CHMUX_SEL0_ID + 32) << CHMUX_OFF_SHIFT) +
                                                  (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_FlexioShft4ToDmac0Ch28RequestEna = 28U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA0_CHMUX_SEL0_ID + 32) << CHMUX_OFF_SHIFT) +
                                                 (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm16TxToDmac0Ch29RequestEna = 29U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                  ((DMA0_CHMUX_SEL0_ID + 36) << CHMUX_OFF_SHIFT) +
                                                  (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_FlexioShft5ToDmac0Ch29RequestEna = 29U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA0_CHMUX_SEL0_ID + 36) << CHMUX_OFF_SHIFT) +
                                                 (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_I3c1RxToDmac0Ch30RequestEna = 30U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                            ((DMA0_CHMUX_SEL0_ID + 40) << CHMUX_OFF_SHIFT) + (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_FlexioShft6ToDmac0Ch30RequestEna = 30U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA0_CHMUX_SEL0_ID + 40) << CHMUX_OFF_SHIFT) +
                                                 (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_I3c1TxToDmac0Ch31RequestEna = 31U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                            ((DMA0_CHMUX_SEL0_ID + 44) << CHMUX_OFF_SHIFT) + (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_FlexioShft7ToDmac0Ch31RequestEna = 31U + (DMA0_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA0_CHMUX_SEL0_ID + 44) << CHMUX_OFF_SHIFT) +
                                                 (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_FlexioShft0ToDmac0Ch32RequestEna = 0U + ((DMA0_REQ_ENA0_ID + 4) << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA0_CHMUX_SEL0_ID + 48) << CHMUX_OFF_SHIFT) +
                                                 (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm11RxToDmac0Ch32RequestEna = 0U + ((DMA0_REQ_ENA0_ID + 4) << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                  ((DMA0_CHMUX_SEL0_ID + 48) << CHMUX_OFF_SHIFT) +
                                                  (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_FlexioShft1ToDmac0Ch33RequestEna = 1U + ((DMA0_REQ_ENA0_ID + 4) << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA0_CHMUX_SEL0_ID + 52) << CHMUX_OFF_SHIFT) +
                                                 (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm11TxToDmac0Ch33RequestEna = 1U + ((DMA0_REQ_ENA0_ID + 4) << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                  ((DMA0_CHMUX_SEL0_ID + 52) << CHMUX_OFF_SHIFT) +
                                                  (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_FlexioShft2ToDmac0Ch34RequestEna = 2U + ((DMA0_REQ_ENA0_ID + 4) << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA0_CHMUX_SEL0_ID + 56) << CHMUX_OFF_SHIFT) +
                                                 (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm12RxToDmac0Ch34RequestEna = 2U + ((DMA0_REQ_ENA0_ID + 4) << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                  ((DMA0_CHMUX_SEL0_ID + 56) << CHMUX_OFF_SHIFT) +
                                                  (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_FlexioShft3ToDmac0Ch35RequestEna = 3U + ((DMA0_REQ_ENA0_ID + 4) << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA0_CHMUX_SEL0_ID + 60) << CHMUX_OFF_SHIFT) +
                                                 (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm12TxToDmac0Ch35RequestEna = 3U + ((DMA0_REQ_ENA0_ID + 4) << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                  ((DMA0_CHMUX_SEL0_ID + 60) << CHMUX_OFF_SHIFT) +
                                                  (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_HashCryptToDmac0Ch36RequestEna = 4U + ((DMA0_REQ_ENA0_ID + 4) << ENA_SHIFT),

    /*!< DMA1 REQ signal. */
    kINPUTMUX_Flexcomm0RxToDmac1Ch0RequestEna  = 0U + (DMA1_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm0TxToDmac1Ch1RequestEna  = 1U + (DMA1_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm1RxToDmac1Ch2RequestEna  = 2U + (DMA1_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm1TxToDmac1Ch3RequestEna  = 3U + (DMA1_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm2RxToDmac1Ch4RequestEna  = 4U + (DMA1_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm2TxToDmac1Ch5RequestEna  = 5U + (DMA1_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm3RxToDmac1Ch6RequestEna  = 6U + (DMA1_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm3TxToDmac1Ch7RequestEna  = 7U + (DMA1_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm4RxToDmac1Ch8RequestEna  = 8U + (DMA1_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm4TxToDmac1Ch9RequestEna  = 9U + (DMA1_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm5RxToDmac1Ch10RequestEna = 10U + (DMA1_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm5TxToDmac1Ch11RequestEna = 11U + (DMA1_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm6RxToDmac1Ch12RequestEna = 12U + (DMA1_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm6TxToDmac1Ch13RequestEna = 13U + (DMA1_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm7RxToDmac1Ch14RequestEna = 14U + (DMA1_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm7TxToDmac1Ch15RequestEna = 15U + (DMA1_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Dmic0Ch0ToDmac1Ch16RequestEna    = 16U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                              (DMA1_CHMUX_SEL0_ID << CHMUX_OFF_SHIFT) + (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm8RxToDmac1Ch16RequestEna = 16U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 (DMA1_CHMUX_SEL0_ID << CHMUX_OFF_SHIFT) + (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Dmic0Ch1ToDmac1Ch17RequestEna = 17U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                              ((DMA1_CHMUX_SEL0_ID + 4) << CHMUX_OFF_SHIFT) + (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm8TxToDmac1Ch17RequestEna = 17U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA1_CHMUX_SEL0_ID + 4) << CHMUX_OFF_SHIFT) +
                                                 (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Dmic0Ch2ToDmac1Ch18RequestEna = 18U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                              ((DMA1_CHMUX_SEL0_ID + 8) << CHMUX_OFF_SHIFT) + (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm9RxToDmac1Ch18RequestEna = 18U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA1_CHMUX_SEL0_ID + 8) << CHMUX_OFF_SHIFT) +
                                                 (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Dmic0Ch3ToDmac1Ch19RequestEna = 19U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                              ((DMA1_CHMUX_SEL0_ID + 12) << CHMUX_OFF_SHIFT) + (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm9TxToDmac1Ch19RequestEna = 19U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA1_CHMUX_SEL0_ID + 12) << CHMUX_OFF_SHIFT) +
                                                 (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Dmic0Ch4ToDmac1Ch20RequestEna = 20U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                              ((DMA1_CHMUX_SEL0_ID + 16) << CHMUX_OFF_SHIFT) + (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm10RxToDmac1Ch20RequestEna = 20U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                  ((DMA1_CHMUX_SEL0_ID + 16) << CHMUX_OFF_SHIFT) +
                                                  (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Dmic0Ch5ToDmac1Ch21RequestEna = 21U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                              ((DMA1_CHMUX_SEL0_ID + 20) << CHMUX_OFF_SHIFT) + (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm10TxToDmac1Ch21RequestEna = 21U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                  ((DMA1_CHMUX_SEL0_ID + 20) << CHMUX_OFF_SHIFT) +
                                                  (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Dmic0Ch6ToDmac1Ch22RequestEna = 22U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                              ((DMA1_CHMUX_SEL0_ID + 24) << CHMUX_OFF_SHIFT) + (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm13RxToDmac1Ch22RequestEna = 22U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                  ((DMA1_CHMUX_SEL0_ID + 24) << CHMUX_OFF_SHIFT) +
                                                  (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Dmic0Ch7ToDmac1Ch23RequestEna = 23U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                              ((DMA1_CHMUX_SEL0_ID + 28) << CHMUX_OFF_SHIFT) + (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm13TxToDmac1Ch23RequestEna = 23U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                  ((DMA1_CHMUX_SEL0_ID + 28) << CHMUX_OFF_SHIFT) +
                                                  (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_I3c0RxToDmac1Ch24RequestEna       = 24U + (DMA1_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_I3c0TxToDmac1Ch25RequestEna       = 25U + (DMA1_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm14RxToDmac1Ch26RequestEna = 26U + (DMA1_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm14TxToDmac1Ch27RequestEna = 27U + (DMA1_REQ_ENA0_ID << ENA_SHIFT),
    kINPUTMUX_Flexcomm16RxToDmac1Ch28RequestEna = 28U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                  ((DMA1_CHMUX_SEL0_ID + 32) << CHMUX_OFF_SHIFT) +
                                                  (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_FlexioShft4ToDmac1Ch28RequestEna = 28U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA1_CHMUX_SEL0_ID + 32) << CHMUX_OFF_SHIFT) +
                                                 (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm16TxToDmac1Ch29RequestEna = 29U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                  ((DMA1_CHMUX_SEL0_ID + 36) << CHMUX_OFF_SHIFT) +
                                                  (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_FlexioShft5ToDmac1Ch29RequestEna = 29U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA1_CHMUX_SEL0_ID + 36) << CHMUX_OFF_SHIFT) +
                                                 (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_I3c1RxToDmac1Ch30RequestEna = 30U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                            ((DMA1_CHMUX_SEL0_ID + 40) << CHMUX_OFF_SHIFT) + (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_FlexioShft6ToDmac1Ch30RequestEna = 30U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA1_CHMUX_SEL0_ID + 40) << CHMUX_OFF_SHIFT) +
                                                 (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_I3c1TxToDmac1Ch31RequestEna = 31U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                            ((DMA1_CHMUX_SEL0_ID + 44) << CHMUX_OFF_SHIFT) + (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_FlexioShft7ToDmac1Ch31RequestEna = 31U + (DMA1_REQ_ENA0_ID << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA1_CHMUX_SEL0_ID + 44) << CHMUX_OFF_SHIFT) +
                                                 (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_FlexioShft0ToDmac1Ch32RequestEna = 0U + ((DMA1_REQ_ENA0_ID + 4) << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA1_CHMUX_SEL0_ID + 48) << CHMUX_OFF_SHIFT) +
                                                 (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm11RxToDmac1Ch32RequestEna = 0U + ((DMA1_REQ_ENA0_ID + 4) << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                  ((DMA1_CHMUX_SEL0_ID + 48) << CHMUX_OFF_SHIFT) +
                                                  (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_FlexioShft1ToDmac1Ch33RequestEna = 1U + ((DMA1_REQ_ENA0_ID + 4) << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA1_CHMUX_SEL0_ID + 52) << CHMUX_OFF_SHIFT) +
                                                 (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm11TxToDmac1Ch33RequestEna = 1U + ((DMA1_REQ_ENA0_ID + 4) << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                  ((DMA1_CHMUX_SEL0_ID + 52) << CHMUX_OFF_SHIFT) +
                                                  (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_FlexioShft2ToDmac1Ch34RequestEna = 2U + ((DMA1_REQ_ENA0_ID + 4) << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA1_CHMUX_SEL0_ID + 56) << CHMUX_OFF_SHIFT) +
                                                 (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm12RxToDmac1Ch34RequestEna = 2U + ((DMA1_REQ_ENA0_ID + 4) << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                  ((DMA1_CHMUX_SEL0_ID + 56) << CHMUX_OFF_SHIFT) +
                                                  (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_FlexioShft3ToDmac1Ch35RequestEna = 3U + ((DMA1_REQ_ENA0_ID + 4) << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                 ((DMA1_CHMUX_SEL0_ID + 60) << CHMUX_OFF_SHIFT) +
                                                 (0U << CHMUX_VAL_SHIFT),
    kINPUTMUX_Flexcomm12TxToDmac1Ch35RequestEna = 3U + ((DMA1_REQ_ENA0_ID + 4) << ENA_SHIFT) + (1U << CHMUX_AVL_SHIFT) +
                                                  ((DMA1_CHMUX_SEL0_ID + 60) << CHMUX_OFF_SHIFT) +
                                                  (1U << CHMUX_VAL_SHIFT),
    kINPUTMUX_HashCryptToDmac1Ch36RequestEna = 4U + ((DMA1_REQ_ENA0_ID + 4) << ENA_SHIFT),
} inputmux_signal_t;

/*@}*/

#endif /* _FSL_INPUTMUX_CONNECTIONS_ */
