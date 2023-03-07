/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_POWER_H_
#define _FSL_POWER_H_

#include "fsl_common.h"

/*!
 * @addtogroup power
 * @{
 */

/*! @file */

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @name Driver version */
/*@{*/
/*! @brief POWER driver version 2.1.0. */
#define FSL_POWER_DRIVER_VERSION (MAKE_VERSION(2, 1, 0))
/*@}*/

/*!
 * @brief Pin edge for wakeup.
 */
typedef enum _power_wakeup_edge
{
    kPOWER_WakeupEdgeLow  = 0U, /*!< Wakeup on pin low level. */
    kPOWER_WakeupEdgeHigh = 1U, /*!< Wakeup on pin high level. */
} power_wakeup_edge_t;

/*!
 * @brief Wakeup pin.
 */
typedef enum _power_wakeup_pin
{
    kPOWER_WakeupPin0 = 0U, /*!< Wakeup0 pin - GPIO22. */
    kPOWER_WakeupPin1 = 1U, /*!< Wakeup1 pin - GPIO23. */
} power_wakeup_pin_t;

/*!
 * @brief Reset cause.
 */
typedef enum _power_reset_cause
{
    kPOWER_ResetCauseVbatBrownout = 1U << 0U, /*!< VBAT power brown-out detected. */
    kPOWER_ResetCauseAv12Brownout = 1U << 1U, /*!< AV12 power brown-out detected. */
    kPOWER_ResetCauseAv18Brownout = 1U << 2U, /*!< AV18 power brown-out detected. */
    kPOWER_ResetCauseSysResetReq  = 1U << 3U, /*!< CM4 system soft reset request. */
    kPOWER_ResetCauseLockup       = 1U << 4U, /*!< CM4 locked up. */
    kPOWER_ResetCauseWdt          = 1U << 5U, /*!< watchdog timer. */
    kPOWER_ResetCauseAll          = 0x3FU,    /*!< All reset causes. Used in POWER_ClearResetCause(). */
} power_reset_cause_t;

/*!
 * @brief Wakeup source.
 */
typedef enum _power_wakeup_src
{
    kPOWER_WakeupSrcPin0 = 1U << 0U, /*!< Wakeup0 pin. */
    kPOWER_WakeupSrcPin1 = 1U << 1U, /*!< Wakeup1 pin. */
    kPOWER_WakeupSrcWlan = 1U << 2U, /*!< WLAN interrupt. */
    kPOWER_WakeupSrcRtc  = 1U << 3U, /*!< RTC interrupt. */
    kPOWER_WakeupSrcComp = 1U << 4U, /*!< PMIP Comp interrupt. */
    kPOWER_WakeupSrcAll  = 0x1FU,    /*!< All wakeup sources. Used in POWER_ClearWakeupSource(). */
} power_wakeup_src_t;

/*!
 * @brief Wakeup interrupt mask.
 */
typedef enum _power_wakeup_mask
{
    kPOWER_WakeupMaskPin0 = 1U << 3U, /*!< Wakeup0 pin. */
    kPOWER_WakeupMaskPin1 = 1U << 4U, /*!< Wakeup1 pin. */
    kPOWER_WakeupMaskRtc  = 1U << 5U, /*!< RTC interrupt. */
    kPOWER_WakeupMaskComp = 1U << 6U, /*!< PMIP Comp interrupt. */
    kPOWER_WakeupMaskWlan = 1U << 7U, /*!< WLAN interrupt. */
    kPOWER_WakeupMaskAll  = 0xF8U,    /*!< All wakeup interrupt mask. */
} power_wakeup_mask_t;

/*!
 * @brief IO domain.
 */
typedef enum _power_vddio
{
    kPOWER_VddIo0   = 0U,  /*!< VDDIO0 pad group. */
    kPOWER_VddIo1   = 1U,  /*!< VDDIO1 pad group. */
    kPOWER_VddIo2   = 2U,  /*!< VDDIO2 pad group. */
    kPOWER_VddIo3   = 3U,  /*!< VDDIO3 pad group. */
    kPOWER_VddIoAon = 19U, /*!< Always on pad group. */
} power_vddio_t;

/*!
 * @brief IO domain voltage level.
 */
typedef enum _power_vddio_volt
{
    kPOWER_VddioVolt3V3 = 0x00U, /*!< VDDIO 3.3V */
    kPOWER_VddioVolt2V5 = 0x01U, /*!< VDDIO 2.5V */
    kPOWER_VddioVolt1V8 = 0x10U, /*!< VDDIO 1.8V */
} power_vddio_volt_t;

/**
 *  @brief AON Comparator Hysteresis list
 */
typedef enum _power_aon_comp_hyst
{
    kPOWER_AonCompHyst_0 = 0x0, /*!< Ulpcomp hysteresis level 0 */
    kPOWER_AonCompHyst_1 = 0x1, /*!< Ulpcomp hysteresis level 1 */
    kPOWER_AonCompHyst_2 = 0x2, /*!< Ulpcomp hysteresis level 2 */
    kPOWER_AonCompHyst_3 = 0x3, /*!< Ulpcomp hysteresis level 3 */
} power_aon_comp_hyst_t;

/*!
 * @brief AON Comparator mode.
 */
typedef enum _power_aon_comp_mode
{
    kPOWER_AonCompMode_SingleEnded  = 0U,
    kPOWER_AonCompMode_Differential = 1U,
} power_aon_comp_mode_t;

/*!
 * @brief AON Comparator Reference for Single-Ended mode.
 */
typedef enum _power_aon_comp_ref
{
    kPOWER_AonCompRef_0V2 = 0U, /*!< 0.2V */
    kPOWER_AonCompRef_0V4 = 1U, /*!< 0.4 */
    kPOWER_AonCompRef_0V6 = 2U, /*!< 0.6V */
    kPOWER_AonCompRef_0V8 = 3U, /*!< 0.8V */
    kPOWER_AonCompRef_1V0 = 4U, /*!< 1.0V */
    kPOWER_AonCompRef_1V2 = 5U, /*!< 1.2V */
    kPOWER_AonCompRef_1V4 = 6U, /*!< 1.4V */
    kPOWER_AonCompRef_1V6 = 7U, /*!< 1.6V */
} power_aon_comp_ref_t;

/*!
 * @brief Internal bandgap reference voltage trim value.
 */
typedef enum _power_bandgap_ref_voltage
{
    kPOWER_BGRefVoltage1P159V = 0U, /*!< Trim reference voltage to 1.159V. */
    kPOWER_BGRefVoltage1P163V,      /*!< Trim reference voltage to 1.163V. */
    kPOWER_BGRefVoltage1P168V,      /*!< Trim reference voltage to 1.168V. */
    kPOWER_BGRefVoltage1P172V,      /*!< Trim reference voltage to 1.172V. */
    kPOWER_BGRefVoltage1P177V,      /*!< Trim reference voltage to 1.177V. */
    kPOWER_BGRefVoltage1P181V,      /*!< Trim reference voltage to 1.181V. */
    kPOWER_BGRefVoltage1P186V,      /*!< Trim reference voltage to 1.186V. */
    kPOWER_BGRefVoltage1P190V,      /*!< Trim reference voltage to 1.190V. */
    kPOWER_BGRefVoltage1P194V,      /*!< Trim reference voltage to 1.194V. */
    kPOWER_BGRefVoltage1P199V,      /*!< Trim reference voltage to 1.199V. */
    kPOWER_BGRefVoltage1P204V,      /*!< Trim reference voltage to 1.204V. */
    kPOWER_BGRefVoltage1P208,       /*!< Trim reference voltage to 1.208V. */
    kPOWER_BGRefVoltage1P213V,      /*!< Trim reference voltage to 1.213V. */
    kPOWER_BGRefVoltage1P217V,      /*!< Trim reference voltage to 1.217V. */
    kPOWER_BGRefVoltage1P222V,      /*!< Trim reference voltage to 1.222V. */
    kPOWER_BGRefVoltage1P226V,      /*!< Trim reference voltage to 1.226V. */
} power_bandgap_ref_voltage_t;

/*!
 * @brief AON Comparator config structure.
 */
typedef struct _power_aon_comp_config
{
    power_aon_comp_hyst_t hyst;
    power_aon_comp_mode_t mode;
    power_aon_comp_ref_t ref;
} power_aon_comp_config_t;

#if defined(__cplusplus)
extern "C" {
#endif

/**
 * @brief   Configure pin edge for wakeup
 * @param   pin     : Wakeup pin
 * @param   edge    : Pin level for wakeup
 */
__STATIC_INLINE void POWER_ConfigWakeupPin(power_wakeup_pin_t pin, power_wakeup_edge_t edge)
{
    PMU->WAKEUP_EDGE_DETECT = (PMU->WAKEUP_EDGE_DETECT & ~(1U << (uint8_t)pin)) | ((uint8_t)edge << (uint8_t)pin);
}

/**
 * @brief   Get last reset cause
 * @return  Or'ed cause of \ref power_reset_cause_t
 */
__STATIC_INLINE uint32_t POWER_GetResetCause(void)
{
    return PMU->LAST_RST_CAUSE & (uint32_t)kPOWER_ResetCauseAll;
}

/**
 * @brief   Clear last reset cause
 * @param   cause   : A bitmask of of \ref power_reset_cause_t
 */
__STATIC_INLINE void POWER_ClearResetCause(uint32_t cause)
{
    assert((cause & ~(uint32_t)kPOWER_ResetCauseAll) == 0U);

    PMU->LAST_RST_CLR = cause;
}

/**
 * @brief   Get last wakeup source
 * @return  Bitmask of \ref power_wakeup_src_t
 */
__STATIC_INLINE uint32_t POWER_GetWakeupSource(void)
{
    return PMU->WAKEUP_STATUS & (uint32_t)kPOWER_WakeupSrcAll;
}

/**
 * @brief   Clear wakeup source
 * @param   src   : A bitmask of \ref power_wakeup_src_t
 */
__STATIC_INLINE void POWER_ClearWakeupSource(uint32_t src)
{
    assert((src & ~(uint32_t)kPOWER_WakeupSrcAll) == 0U);

    PMU->WAKE_SRC_CLR = src;
}

/**
 * @brief   Enable the Wakeup interrupts.
 * @param   interruptMask : A bitmask of interrupts to enable. See \ref power_wakeup_mask_t.
 */
__STATIC_INLINE void POWER_EnableWakeup(uint32_t interruptMask)
{
    assert((interruptMask & ~(uint32_t)kPOWER_WakeupMaskAll) == 0U);

    PMU->WAKEUP_MASK |= interruptMask;
}

/**
 * @brief   Disable the Wakeup interrupts.
 * @param   interruptMask : A bitmask of interrupts to disable. See \ref power_wakeup_mask_t.
 */
__STATIC_INLINE void POWER_DisableWakeup(uint32_t interruptMask)
{
    assert((interruptMask & ~(uint32_t)kPOWER_WakeupMaskAll) == 0U);

    PMU->WAKEUP_MASK &= ~interruptMask;
}

/**
 * @brief   Set sleep mode on idle.
 * @param   mode : 0 ~ 4 stands for PM0 ~ PM4.
 */
void POWER_SetSleepMode(uint32_t mode);

/**
 * @brief   Switch system into certain power mode.
 * @param   mode : 0 ~ 4 stands for PM0 ~ PM4.
 * @param   excludeIo: IO domains bitmask to keep unchanged during PM2.
 */
void POWER_EnterPowerMode(uint32_t mode, uint32_t excludeIo);

/**
 * @brief   Power on IO domain.
 * @param   domain : IO domain to power on.
 */
__STATIC_INLINE void POWER_PowerOnVddio(power_vddio_t domain)
{
    assert(domain <= kPOWER_VddIo3);

    PMU->IO_PAD_PWR_CFG |= (1UL << (PMU_IO_PAD_PWR_CFG_GPIO0_LOW_VDDB_SHIFT + (uint32_t)domain));
}

/**
 * @brief   Power off IO domain.
 * @param   domain : IO domain to power off.
 */
__STATIC_INLINE void POWER_PowerOffVddio(power_vddio_t domain)
{
    assert(domain <= kPOWER_VddIo3);

    /* Both pad regulator and IO domain powered off */
    PMU->IO_PAD_PWR_CFG &= ~((1UL << (PMU_IO_PAD_PWR_CFG_GPIO0_LOW_VDDB_SHIFT + (uint32_t)domain)) |
                             (1UL << (PMU_IO_PAD_PWR_CFG_GPIO0_PDB_SHIFT + (uint32_t)domain)));
}

/**
 * @brief   Power on IO domain pad regulator.
 * @param   domain : IO domain to power on.
 */
void POWER_PowerOnVddioPad(power_vddio_t domain);

/**
 * @brief   Power off IO domain pad regulator.
 * @param   domain : IO domain to power off.
 */
void POWER_PowerOffVddioPad(power_vddio_t domain);

/**
 * @brief   Set IO domain pad voltage.
 * @param   domain : IO domain.
 * @param   volt   : Voltage level to be set.
 */
void POWER_SetVddioPadVolt(power_vddio_t domain, power_vddio_volt_t volt);

/**
 * @brief   Power on WLAN.
 */
__STATIC_INLINE void POWER_PowerOnWlan(void)
{
    PMU->WLAN_CTRL |= PMU_WLAN_CTRL_PD_MASK;
}

/**
 * @brief   Power off WLAN.
 */
__STATIC_INLINE void POWER_PowerOffWlan(void)
{
    PMU->WLAN_CTRL &= ~PMU_WLAN_CTRL_PD_MASK;
}

/**
 * @brief   Enable MCI wakeup WLAN
 */
__STATIC_INLINE void PMU_EnableWlanWakeup(void)
{
    PMU->WLAN_CTRL1 |= PMU_WLAN_CTRL1_MCI_WL_WAKEUP_MASK;
}

/**
 * @brief   Disable MCI wakeup WLAN
 */
__STATIC_INLINE void PMU_DisableWlanWakeup(void)
{
    PMU->WLAN_CTRL1 &= ~PMU_WLAN_CTRL1_MCI_WL_WAKEUP_MASK;
}

/**
 * @brief   Attach GPIO interrupt to NVIC vector table.
 *          Since 2 GPIO pins shares 1 IRQ number. Attaching one GPIO IRQ implicits
 *          detaching the sibling GPIO IRQ.
 * @param   pin    : GPIO index.
 */
void POWER_AttachGpioIrq(uint32_t pin);

/**
 * @brief   Enable AON Domain Comparator.
 * @param   config : AON Comparator config data.
 */
void POWER_EnableAonComp(const power_aon_comp_config_t *config);

/**
 * @brief   Disable AON Domain Comparator.
 */
void POWER_DisableAonComp(void);

/**
 * @brief   Get AON Domain Comparator result.
 * @return  AON Comparator output value
 */
uint32_t POWER_GetAonCompValue(void);

/**
 * @brief Power up internal bandgap.
 */
__STATIC_INLINE void POWER_PowerUpBandgap(void)
{
    BG->CTRL &= ~BG_CTRL_PD_MASK;
}

/**
 * @brief Power down internal bandgap.
 */
__STATIC_INLINE void POWER_PowerDownBandgap(void)
{
    BG->CTRL |= BG_CTRL_PD_MASK;
}

/**
 * @brief Trim internal bandgap reference voltage.
 *
 * @param refVoltage The reference voltage value to trim, please refer to @ref power_bandgap_ref_voltage_t for details.
 */
void POWER_TrimBandgapRefVoltage(power_bandgap_ref_voltage_t refVoltage);

/**
 * @brief Check internal bandgap ready status
 *
 * @retval true Internal bandgap is ready.
 * @retval false Internal bandgap is not ready.
 */
__STATIC_INLINE bool POWER_CheckBandgapReady(void)
{
    return ((BG->STATUS & BG_STATUS_RDY_MASK) == BG_STATUS_RDY_MASK);
}

#if defined(__cplusplus)
}
#endif /*_cplusplus */

/*! @}*/

#endif /* _FSL_POWER_H_ */
