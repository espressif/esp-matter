/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_PCA9420_H_
#define _FSL_PCA9420_H_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "fsl_common.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*! @brief PCA9420 Default I2C address. */
#define PCA9420_DEFAULT_I2C_ADDR 0x61U

/*! @brief Define the Register Memory Map of PCA9420. */
#define PCA9420_DEV_INFO (0x00U)
#define PCA9420_TOP_INT (0x01U)
#define PCA9420_SUB_INT0 (0x02U)
#define PCA9420_SUB_INT0_MASK (0x03U)
#define PCA9420_SUB_INT1 (0x04U)
#define PCA9420_SUB_INT1_MASK (0x05U)
#define PCA9420_SUB_INT2 (0x06U)
#define PCA9420_SUB_INT2_MASK (0x07U)
#define PCA9420_TOP_CNTL0 (0x09U)
#define PCA9420_TOP_CNTL1 (0x0AU)
#define PCA9420_TOP_CNTL2 (0x0BU)
#define PCA9420_TOP_CNTL3 (0x0CU)
#define PCA9420_TOP_CNTL4 (0x0DU)

#define PCA9420_CHG_CNTL0 (0x10U)
#define PCA9420_CHG_CNTL1 (0x11U)
#define PCA9420_CHG_CNTL2 (0x12U)
#define PCA9420_CHG_CNTL3 (0x13U)
#define PCA9420_CHG_CNTL4 (0x14U)
#define PCA9420_CHG_CNTL5 (0x15U)
#define PCA9420_CHG_CNTL6 (0x16U)
#define PCA9420_CHG_CNTL7 (0x17U)
#define PCA9420_CHG_STATUS_0 (0x18U)
#define PCA9420_CHG_STATUS_1 (0x19U)
#define PCA9420_CHG_STATUS_2 (0x1AU)
#define PCA9420_CHG_STATUS_3 (0x1BU)

#define PCA9420_REG_STATUS (0x20U)
#define PCA9420_ACT_DISCHARGE_CNTL_1 (0x21U)
#define PCA9420_MODECFG_0_0 (0x22U)
#define PCA9420_MODECFG_0_1 (0x23U)
#define PCA9420_MODECFG_0_2 (0x24U)
#define PCA9420_MODECFG_0_3 (0x25U)
#define PCA9420_MODECFG_1_0 (0x26U)
#define PCA9420_MODECFG_1_1 (0x27U)
#define PCA9420_MODECFG_1_2 (0x28U)
#define PCA9420_MODECFG_1_3 (0x29U)
#define PCA9420_MODECFG_2_0 (0x2AU)
#define PCA9420_MODECFG_2_1 (0x2BU)
#define PCA9420_MODECFG_2_2 (0x2CU)
#define PCA9420_MODECFG_2_3 (0x2DU)
#define PCA9420_MODECFG_3_0 (0x2EU)
#define PCA9420_MODECFG_3_1 (0x2FU)
#define PCA9420_MODECFG_3_2 (0x30U)
#define PCA9420_MODECFG_3_3 (0x31U)

/*! @brief Define the Register Masks of PCA9420. */
#define PCA9420_TOP_CNTL3_MODE_I2C_MASK (0x18U)
#define PCA9420_TOP_CNTL3_MODE_I2C_SHIFT (0x03U)

#define PCA9420_MODECFG_0_SHIP_EN_MASK (0x80U)
#define PCA9420_MODECFG_0_MODE_CTRL_SEL_MASK (0x40U)
#define PCA9420_MODECFG_0_SW1_OUT_MASK (0x3FU)
#define PCA9420_MODECFG_1_ON_CFG_MASK (0x40U)
#define PCA9420_MODECFG_1_SW2_OUT_MASK (0x3FU)
#define PCA9420_MODECFG_2_LDO1_OUT_MASK (0xF0U)
#define PCA9420_MODECFG_2_LDO1_OUT_SHIFT (0x04U)
#define PCA9420_MODECFG_3_WD_TIMER_MASK (0xC0U)
#define PCA9420_MODECFG_3_LDO2_OUT_MASK (0x3FU)

/*! @brief PCA9420 mode definition. */
typedef enum _pca9420_mode
{
    kPCA9420_Mode0 = 0x00U, /* Mode 0. */
    kPCA9420_Mode1 = 0x01U, /* Mode 1. */
    kPCA9420_Mode2 = 0x02U, /* Mode 2. */
    kPCA9420_Mode3 = 0x03U, /* Mode 3. */
} pca9420_mode_t;

/*! @brief PCA9420 Regulator definition. */
typedef enum _pca9420_regulator
{
    kPCA9420_RegulatorSwitch1 = 0x08U, /* Switch1 regulator. */
    kPCA9420_RegulatorSwitch2 = 0x04U, /* Switch2 regulator. */
    kPCA9420_RegulatorLdo1    = 0x02U, /* LDO1 regulator. */
    kPCA9420_RegulatorLdo2    = 0x01U, /* LDO2 regulator. */
} pca9420_regulator_t;

/*! @brief PCA9420 Interrupt Source definition. */
enum _pca9420_interrupt_source
{
    /* Sub_INT0 */
    kPCA9420_IntSrcSysVinOKChanged    = 0x1U,  /* VIN_OK changed interrupt source. */
    kPCA9420_IntSrcSysWdogTimeout     = 0x2U,  /* Watchdog timeout interrupt source. */
    kPCA9420_IntSrcSysAsysPreWarn     = 0x4U,  /* VSYS voltage falls below the VSYS_PRE_WRN interrupt source. */
    kPCA9420_IntSrcSysThermalShutdown = 0x8U,  /* Thermal shutdown (Die temp >= THM_STDN) interrupt source. */
    kPCA9420_IntSrcSysTempWarn        = 0x10U, /* Die temp pre-warning (Die temp >= T_WRN) interrupt source. */
    kPCA9420_IntSrcSysAll             = 0x1FU, /* All system interrupt sources. */

    /* Sub_INT1 */
    kPCA9420_IntSrcChgChgOk           = 0x100U,  /* CHG_OK changed interrupt source. */
    kPCA9420_IntSrcChgVbatOk          = 0x200U,  /* VBAT_OK changed interrupt source. */
    kPCA9420_IntSrcChgVbatDetOk       = 0x400U,  /* VBAT_DET_OK changed interrupt source. */
    kPCA9420_IntSrcChgPreChgTimeout   = 0x800U,  /* Pre-qualification charge timeout interrupt source. */
    kPCA9420_IntSrcChgFastChgTimeout  = 0x1000U, /* Fast charge timeout interrupt source. */
    kPCA9420_IntSrcChgInputCurrentLmt = 0x2000U, /* Input current limit interrupt source. */
    kPCA9420_IntSrcChgAll             = 0x3F00U, /* All charger interrupt sources. */

    /* Sub_INT2 */
    kPCA9420_IntSrcLdo2VoutOk   = 0x10000U, /* VOUTLDO2_OK changed interrupt source. */
    kPCA9420_IntSrcLdo1VoutOk   = 0x20000U, /* VOUTLDO1_OK changed interrupt source. */
    kPCA9420_IntSrcSw2VoutOk    = 0x40000U, /* VOUTSW2_OK changed interrupt source. */
    kPCA9420_IntSrcSw1VoutOk    = 0x80000U, /* VOUTSW1_OK changed interrupt source. */
    kPCA9420_IntSrcRegulatorAll = 0xF0000U, /* All regulator interrupt sources. */
};

/*! @brief PCA9420 VIN Current Limit Selection definition. */
typedef enum _pca9420_vin_ilim
{
    kPCA9420_VinIlim_74_85_98      = 0x00U, /* min: 74mA, typ: 85mA, max: 98mA */
    kPCA9420_VinIlim_222_225_293   = 0x20U, /* min: 222mA, typ: 225mA, max: 293mA */
    kPCA9420_VinIlim_370_425_489   = 0x40U, /* min: 370mA, typ: 425mA, max: 489mA */
    kPCA9420_VinIlim_517_595_684   = 0x60U, /* min: 517mA, typ: 595mA, max: 684mA */
    kPCA9420_VinIlim_665_765_880   = 0x80U, /* min: 665mA, typ: 765mA, max: 880mA */
    kPCA9420_VinIlim_813_935_1075  = 0xA0U, /* min: 813mA, typ: 935mA, max: 1075mA */
    kPCA9420_VinIlim_961_1105_1271 = 0xC0U, /* min: 961mA, typ: 1105mA, max: 1271mA */
    kPCA9420_VinIlim_Disabled      = 0xE0U, /* Input current limit disabled */
} pca9420_vin_ilim_t;

/*! @brief PCA9420 Ship Wakeup config definition. */
typedef enum _pca9420_ship_wkup_cfg
{
    kPCA9420_ShipWkup_PowerUp = 0x00U,   /* Upon VIN plug in, the chip will enable the battery charging process, and
                                              start the power-up sequence for LDO1/LDO2/SW1/SW2 per the setting */
    kPCA9420_ShipWkup_NoPowerUp = 0x10U, /* Upon VIN plug in, the chip will enable the charging process,
                                              LDO1/LDO2/SW1/SW2 remain in shutdown mode and the chip will only enable
                                              the power-up sequence upon ON pin falling edge */
} pca9420_ship_wkup_cfg_t;

/*! @brief PCA9420 Power Down Sequence Enable definition. */
typedef enum _pca9420_pwr_dn_en
{
    kPCA9420_PwrDnDisabled = 0x00U, /* Do not start power-down sequence */
    kPCA9420_PwrDnEnabled  = 0x08U, /* Start power-down sequence */
} pca9420_pwr_dn_en_t;

/*! @brief PCA9420 Ship Wakeup config definition. */
typedef enum _pca9420_chg_in_watchdog
{
    kPCA9420_ChgInWatchdogChargerContinue = 0x00U, /* When the watchdog timer expires, charger continue its operation */
    kPCA9420_ChgInWatchdogChargerDisabled = 0x04U, /* When the watchdog timer expires, charger is disabled */
} pca9420_chg_in_watchdog_t;

/*! @brief PCA9420 Power Good Comparator Enable definition. */
typedef enum _pca9420_pgood_en
{
    kPCA9420_PGoodDisabled = 0x00U, /* Output voltage power-good comparators are disabled. */
    kPCA9420_PGoodEnabled  = 0x01U, /* Output voltage power-good comparators are enabled. */
} pca9420_pgood_en_t;

/*! @brief PCA9420 VSYS Pre Warning Voltage Threshold definition. */
typedef enum _pca9420_asys_prewarning
{
    kPCA9420_AsysPreWarn3V3 = 0x00U, /* VSYS pre-warning voltage threshold 3.3V */
    kPCA9420_AsysPreWarn3V4 = 0x40U, /* VSYS pre-warning voltage threshold 3.4V */
    kPCA9420_AsysPreWarn3V5 = 0x80U, /* VSYS pre-warning voltage threshold 3.5V */
    kPCA9420_AsysPreWarn3V6 = 0xC0U, /* VSYS pre-warning voltage threshold 3.6V */
} pca9420_asys_prewarning_t;

/*! @brief PCA9420 VSYS Input Source Selection definition. */
typedef enum _pca9420_asys_input_sel
{
    kPCA9420_AsysInputSelVbatVin =
        0x00U, /* VSYS is powered by either VBAT or VIN, VIN has higher priority if both are presented. */
    kPCA9420_AsysInputSelVbat = 0x10U, /* VSYS is powered by VBAT only. */
    kPCA9420_AsysInputSelVin  = 0x20U, /* VSYS is powered by VIN only. */
    kPCA9420_AsysInputSelNone = 0x30U, /* VSYS is disconnected to either VBAT or VIN (test purpose only). */
} pca9420_asys_input_sel_t;

/*! @brief PCA9420 VIN Over-voltage Protection Threshold Selection definition. */
typedef enum _pca9420_vin_ovp_sel
{
    kPCA9420_VinOvpSel5V5 = 0x00U, /* VIN Over-Voltage Protection threshold 5.5V. */
    kPCA9420_VinOvpSel6V0 = 0x04U, /* VIN Over-Voltage Protection threshold 6.0V. */
} pca9420_vin_ovp_sel_t;

/*! @brief PCA9420 VIN Under-voltage Lockout Threshold Selection definition. */
typedef enum _pca9420_vin_uvlo_sel
{
    kPCA9420_VinUvloSel2V9 = 0x0U, /* VIN Under-Voltage Lockout threshold 2.9V. */
    kPCA9420_VinUvloSel3V1 = 0x1U, /* VIN Under-Voltage Lockout threshold 3.1V. */
    kPCA9420_VinUvloSel3V3 = 0x2U, /* VIN Under-Voltage Lockout threshold 3.3V. */
    kPCA9420_VinUvloSel3V5 = 0x3U, /* VIN Under-Voltage Lockout threshold 3.5V. */
} pca9420_vin_uvlo_sel_t;

/*! @brief PCA9420 VSYS Under-voltage Lockout Threshold Selection definition. */
typedef enum _pca9420_asys_uvlo_sel
{
    kPCA9420_AsysUvloSel2V4 = 0x00U, /* VSYS Under-Voltage Lockout threshold 2.4V. */
    kPCA9420_AsysUvloSel2V5 = 0x40U, /* VSYS Under-Voltage Lockout threshold 2.5V. */
    kPCA9420_AsysUvloSel2V6 = 0x80U, /* VSYS Under-Voltage Lockout threshold 2.6V. */
    kPCA9420_AsysUvloSel2V7 = 0xC0U, /* VSYS Under-Voltage Lockout threshold 2.7V. */
} pca9420_asys_uvlo_sel_t;

/*! @brief PCA9420 Disable Charge Termination definition. */
typedef enum _pca9420_charge_term_dis
{
    kPCA9420_ChargeTermEnabled  = 0x00U, /* Enable the charge termination control. */
    kPCA9420_ChargeTermDisabled = 0x20U, /* Disable the charge termination control, fast charge timer is reset. */
} pca9420_charge_term_dis_t;

/*! @brief PCA9420 Thermal Shutdown Temperature Threshold definition. */
typedef enum _pca9420_them_shdn
{
    kPCA9420_ThemShdn95C  = 0x00U, /* Thermal shutdown temperature threshold 95C. */
    kPCA9420_ThemShdn100C = 0x04U, /* Thermal shutdown temperature threshold 100C. */
    kPCA9420_ThemShdn105C = 0x08U, /* Thermal shutdown temperature threshold 105C. */
    kPCA9420_ThemShdn110C = 0x0CU, /* Thermal shutdown temperature threshold 110C. */
    kPCA9420_ThemShdn115C = 0x10U, /* Thermal shutdown temperature threshold 115C. */
    kPCA9420_ThemShdn120C = 0x14U, /* Thermal shutdown temperature threshold 120C. */
    kPCA9420_ThemShdn125C = 0x18U, /* Thermal shutdown temperature threshold 125C. */
} pca9420_them_shdn_t;

/*! @brief PCA9420 Die Temperature Warning Threshold definition. */
typedef enum _pca9420_die_temp_warning
{
    kPCA9420_DieTempWarn75C = 0x00U, /* Die temperature warning threshold 75C. */
    kPCA9420_DieTempWarn80C = 0x01U, /* Die temperature warning threshold 80C. */
    kPCA9420_DieTempWarn85C = 0x02U, /* Die temperature warning threshold 85C. */
    kPCA9420_DieTempWarn90C = 0x03U, /* Die temperature warning threshold 90C. */
} pca9420_die_temp_warning_t;

/*! @brief PCA9420 Mode I2C definition. */
typedef enum _pca9420_mode_i2c
{
    kPCA9420_ModeI2cMode0 = 0x00U, /* PMIC mode set to mode 0, in case MODE_CTRL_SEL == 0. */
    kPCA9420_ModeI2cMode1 = 0x08U, /* PMIC mode set to mode 1, in case MODE_CTRL_SEL == 0. */
    kPCA9420_ModeI2cMode2 = 0x10U, /* PMIC mode set to mode 2, in case MODE_CTRL_SEL == 0. */
    kPCA9420_ModeI2cMode3 = 0x18U, /* PMIC mode set to mode 3, in case MODE_CTRL_SEL == 0. */
} pca9420_mode_i2c_t;

/*! @brief PCA9420 ON Pin Long Glitch Timer definition. */
typedef enum _pca9420_on_glt_long
{
    kPCA9420_OnGltLong4s  = 0x00U, /* ON pin long glitch timer set to 4 sec. */
    kPCA9420_OnGltLong8s  = 0x01U, /* ON pin long glitch timer set to 8 sec. */
    kPCA9420_OnGltLong12s = 0x02U, /* ON pin long glitch timer set to 12 sec. */
    kPCA9420_OnGltLong16s = 0x03U, /* ON pin long glitch timer set to 16 sec. */
} pca9420_on_glt_long_t;

/* TODO: charger function */

/*! @brief PCA9420 Regulator Status definition. */
typedef enum _pca9420_reg_status
{
    kPCA9420_RegStatusVoutSw1OK  = 0x80U, /* SW1 VOUT Power good status */
    kPCA9420_RegStatusVoutSw2OK  = 0x40U, /* SW2 VOUT Power good status */
    kPCA9420_RegStatusVoutLdo1OK = 0x20U, /* LDO1 VOUT Power good status */
    kPCA9420_RegStatusVoutLdo2OK = 0x10U, /* LDO2 VOUT Power good status */
} pca9420_reg_status_t;

/*! @brief PCA9420 Regulator Control definition. */
typedef enum _pca9420_reg_ctl
{
    kPCA9420_RegCtlSw1Bleed  = 0x08U, /* SW1 output active discharge control (0: enable, 1: disable). */
    kPCA9420_RegCtlSw2Bleed  = 0x04U, /* SW2 output active discharge control (0: enable, 1: disable). */
    kPCA9420_RegCtlLdo1Bleed = 0x02U, /* LDO1 output active discharge control (0: enable, 1: disable). */
    kPCA9420_RegCtlLdo2Bleed = 0x01U, /* LDO2 output active discharge control (0: enable, 1: disable). */
} pca9420_reg_ctl_t;

/*! @brief PCA9420 Ship Mode Enablement definition. */
typedef enum _pca9420_ship_en
{
    kPCA9420_ShipModeDisabled = 0x00U, /* Ship mode disabled. */
    kPCA9420_ShipModeEnabled  = 0x80U, /* Ship mode enabled, chip enters state with lowest quiescent consumption. */
} pca9420_ship_en_t;

/*! @brief PCA9420 Mode Control Selection definition. */
typedef enum _pca9420_mode_sel
{
    kPCA9420_ModeSelI2C = 0x00U, /* Mode control select by I2C register bits. */
    kPCA9420_ModeSelPin = 0x40U, /* Mode control select by external pins. */
} pca9420_mode_sel_t;

/*! @brief PCA9420 SW1 Output Voltage definition. */
typedef enum _pca9420_sw1_out
{
    kPCA9420_Sw1OutVolt0V500 = 0x00U, /* SW1 output voltage 0.500V. */
    kPCA9420_Sw1OutVolt0V525 = 0x01U, /* SW1 output voltage 0.525V. */
    kPCA9420_Sw1OutVolt0V550 = 0x02U, /* SW1 output voltage 0.550V. */
    kPCA9420_Sw1OutVolt0V575 = 0x03U, /* SW1 output voltage 0.575V. */
    kPCA9420_Sw1OutVolt0V600 = 0x04U, /* SW1 output voltage 0.600V. */
    kPCA9420_Sw1OutVolt0V625 = 0x05U, /* SW1 output voltage 0.625V. */
    kPCA9420_Sw1OutVolt0V650 = 0x06U, /* SW1 output voltage 0.650V. */
    kPCA9420_Sw1OutVolt0V675 = 0x07U, /* SW1 output voltage 0.675V. */
    kPCA9420_Sw1OutVolt0V700 = 0x08U, /* SW1 output voltage 0.700V. */
    kPCA9420_Sw1OutVolt0V725 = 0x09U, /* SW1 output voltage 0.725V. */
    kPCA9420_Sw1OutVolt0V750 = 0x0AU, /* SW1 output voltage 0.750V. */
    kPCA9420_Sw1OutVolt0V775 = 0x0BU, /* SW1 output voltage 0.775V. */
    kPCA9420_Sw1OutVolt0V800 = 0x0CU, /* SW1 output voltage 0.800V. */
    kPCA9420_Sw1OutVolt0V825 = 0x0DU, /* SW1 output voltage 0.825V. */
    kPCA9420_Sw1OutVolt0V850 = 0x0EU, /* SW1 output voltage 0.850V. */
    kPCA9420_Sw1OutVolt0V875 = 0x0FU, /* SW1 output voltage 0.875V. */
    kPCA9420_Sw1OutVolt0V900 = 0x10U, /* SW1 output voltage 0.900V. */
    kPCA9420_Sw1OutVolt0V925 = 0x11U, /* SW1 output voltage 0.925V. */
    kPCA9420_Sw1OutVolt0V950 = 0x12U, /* SW1 output voltage 0.950V. */
    kPCA9420_Sw1OutVolt0V975 = 0x13U, /* SW1 output voltage 0.975V. */
    kPCA9420_Sw1OutVolt1V000 = 0x14U, /* SW1 output voltage 1.000V. */
    kPCA9420_Sw1OutVolt1V025 = 0x15U, /* SW1 output voltage 1.025V. */
    kPCA9420_Sw1OutVolt1V050 = 0x16U, /* SW1 output voltage 1.050V. */
    kPCA9420_Sw1OutVolt1V075 = 0x17U, /* SW1 output voltage 1.075V. */
    kPCA9420_Sw1OutVolt1V100 = 0x18U, /* SW1 output voltage 1.100V. */
    kPCA9420_Sw1OutVolt1V125 = 0x19U, /* SW1 output voltage 1.125V. */
    kPCA9420_Sw1OutVolt1V150 = 0x1AU, /* SW1 output voltage 1.150V. */
    kPCA9420_Sw1OutVolt1V175 = 0x1BU, /* SW1 output voltage 1.175V. */
    kPCA9420_Sw1OutVolt1V200 = 0x1CU, /* SW1 output voltage 1.200V. */
    kPCA9420_Sw1OutVolt1V225 = 0x1DU, /* SW1 output voltage 1.225V. */
    kPCA9420_Sw1OutVolt1V250 = 0x1EU, /* SW1 output voltage 1.250V. */
    kPCA9420_Sw1OutVolt1V275 = 0x1FU, /* SW1 output voltage 1.275V. */
    kPCA9420_Sw1OutVolt1V300 = 0x20U, /* SW1 output voltage 1.300V. */
    kPCA9420_Sw1OutVolt1V325 = 0x21U, /* SW1 output voltage 1.325V. */
    kPCA9420_Sw1OutVolt1V350 = 0x22U, /* SW1 output voltage 1.350V. */
    kPCA9420_Sw1OutVolt1V375 = 0x23U, /* SW1 output voltage 1.375V. */
    kPCA9420_Sw1OutVolt1V400 = 0x24U, /* SW1 output voltage 1.400V. */
    kPCA9420_Sw1OutVolt1V425 = 0x25U, /* SW1 output voltage 1.425V. */
    kPCA9420_Sw1OutVolt1V450 = 0x26U, /* SW1 output voltage 1.450V. */
    kPCA9420_Sw1OutVolt1V475 = 0x27U, /* SW1 output voltage 1.475V. */
    kPCA9420_Sw1OutVolt1V500 = 0x28U, /* SW1 output voltage 1.500V. */
    kPCA9420_Sw1OutVolt1V800 = 0x3FU, /* SW1 output voltage 1.800V. */
} pca9420_sw1_out_t;

/*! @brief PCA9420 ON Key Config definition for mode switch. */
typedef enum _pca9420_on_cfg
{
    kPCA9420_OnCfgEnableModeSwitch =
        0x00U, /* Upon valid falling edge applied on ON pin, the device will switch back to mode 0. */
    kPCA9420_OnCfgDisableModeSwitch = 0x40U, /* Upon valid falling edge applied on ON pin, no mode switch. */
} pca9420_on_cfg_t;

/*! @brief PCA9420 SW2 Output Voltage definition. */
typedef enum _pca9420_sw2_out
{
    kPCA9420_Sw2OutVolt1V500 = 0x00U, /* SW2 output voltage 1.500V. */
    kPCA9420_Sw2OutVolt1V525 = 0x01U, /* SW2 output voltage 1.525V. */
    kPCA9420_Sw2OutVolt1V550 = 0x02U, /* SW2 output voltage 1.550V. */
    kPCA9420_Sw2OutVolt1V575 = 0x03U, /* SW2 output voltage 1.575V. */
    kPCA9420_Sw2OutVolt1V600 = 0x04U, /* SW2 output voltage 1.600V. */
    kPCA9420_Sw2OutVolt1V625 = 0x05U, /* SW2 output voltage 1.625V. */
    kPCA9420_Sw2OutVolt1V650 = 0x06U, /* SW2 output voltage 1.650V. */
    kPCA9420_Sw2OutVolt1V675 = 0x07U, /* SW2 output voltage 1.675V. */
    kPCA9420_Sw2OutVolt1V700 = 0x08U, /* SW2 output voltage 1.700V. */
    kPCA9420_Sw2OutVolt1V725 = 0x09U, /* SW2 output voltage 1.725V. */
    kPCA9420_Sw2OutVolt1V750 = 0x0AU, /* SW2 output voltage 1.750V. */
    kPCA9420_Sw2OutVolt1V775 = 0x0BU, /* SW2 output voltage 1.775V. */
    kPCA9420_Sw2OutVolt1V800 = 0x0CU, /* SW2 output voltage 1.800V. */
    kPCA9420_Sw2OutVolt1V825 = 0x0DU, /* SW2 output voltage 1.825V. */
    kPCA9420_Sw2OutVolt1V850 = 0x0EU, /* SW2 output voltage 1.850V. */
    kPCA9420_Sw2OutVolt1V875 = 0x0FU, /* SW2 output voltage 1.875V. */
    kPCA9420_Sw2OutVolt1V900 = 0x10U, /* SW2 output voltage 1.900V. */
    kPCA9420_Sw2OutVolt1V925 = 0x11U, /* SW2 output voltage 1.925V. */
    kPCA9420_Sw2OutVolt1V950 = 0x12U, /* SW2 output voltage 1.950V. */
    kPCA9420_Sw2OutVolt1V975 = 0x13U, /* SW2 output voltage 1.975V. */
    kPCA9420_Sw2OutVolt2V000 = 0x14U, /* SW2 output voltage 2.000V. */
    kPCA9420_Sw2OutVolt2V025 = 0x15U, /* SW2 output voltage 2.025V. */
    kPCA9420_Sw2OutVolt2V050 = 0x16U, /* SW2 output voltage 2.050V. */
    kPCA9420_Sw2OutVolt2V075 = 0x17U, /* SW2 output voltage 2.075V. */
    kPCA9420_Sw2OutVolt2V100 = 0x18U, /* SW2 output voltage 2.100V. */

    kPCA9420_Sw2OutVolt2V700 = 0x20U, /* SW2 output voltage 2.700V. */
    kPCA9420_Sw2OutVolt2V725 = 0x21U, /* SW2 output voltage 2.725V. */
    kPCA9420_Sw2OutVolt2V750 = 0x22U, /* SW2 output voltage 2.750V. */
    kPCA9420_Sw2OutVolt2V775 = 0x23U, /* SW2 output voltage 2.775V. */
    kPCA9420_Sw2OutVolt2V800 = 0x24U, /* SW2 output voltage 2.800V. */
    kPCA9420_Sw2OutVolt2V825 = 0x25U, /* SW2 output voltage 2.825V. */
    kPCA9420_Sw2OutVolt2V850 = 0x26U, /* SW2 output voltage 2.850V. */
    kPCA9420_Sw2OutVolt2V875 = 0x27U, /* SW2 output voltage 2.875V. */
    kPCA9420_Sw2OutVolt2V900 = 0x28U, /* SW2 output voltage 2.900V. */
    kPCA9420_Sw2OutVolt2V925 = 0x29U, /* SW2 output voltage 2.925V. */
    kPCA9420_Sw2OutVolt2V950 = 0x2AU, /* SW2 output voltage 2.950V. */
    kPCA9420_Sw2OutVolt2V975 = 0x2BU, /* SW2 output voltage 2.975V. */
    kPCA9420_Sw2OutVolt3V000 = 0x2CU, /* SW2 output voltage 3.000V. */
    kPCA9420_Sw2OutVolt3V025 = 0x2DU, /* SW2 output voltage 3.025V. */
    kPCA9420_Sw2OutVolt3V050 = 0x2EU, /* SW2 output voltage 3.050V. */
    kPCA9420_Sw2OutVolt3V075 = 0x2FU, /* SW2 output voltage 3.075V. */
    kPCA9420_Sw2OutVolt3V100 = 0x30U, /* SW2 output voltage 3.100V. */
    kPCA9420_Sw2OutVolt3V125 = 0x31U, /* SW2 output voltage 3.125V. */
    kPCA9420_Sw2OutVolt3V150 = 0x32U, /* SW2 output voltage 3.150V. */
    kPCA9420_Sw2OutVolt3V175 = 0x33U, /* SW2 output voltage 3.175V. */
    kPCA9420_Sw2OutVolt3V200 = 0x34U, /* SW2 output voltage 3.200V. */
    kPCA9420_Sw2OutVolt3V225 = 0x35U, /* SW2 output voltage 3.225V. */
    kPCA9420_Sw2OutVolt3V250 = 0x36U, /* SW2 output voltage 3.250V. */
    kPCA9420_Sw2OutVolt3V275 = 0x37U, /* SW2 output voltage 3.275V. */
    kPCA9420_Sw2OutVolt3V300 = 0x38U, /* SW2 output voltage 3.300V. */
} pca9420_sw2_out_t;

/*! @brief PCA9420 LDO1 Output Voltage definition. */
typedef enum _pca9420_ldo1_out
{
    kPCA9420_Ldo1OutVolt1V700 = 0x00U, /* LDO1 output voltage 1.700V. */
    kPCA9420_Ldo1OutVolt1V725 = 0x10U, /* LDO1 output voltage 1.725V. */
    kPCA9420_Ldo1OutVolt1V750 = 0x20U, /* LDO1 output voltage 1.750V. */
    kPCA9420_Ldo1OutVolt1V775 = 0x30U, /* LDO1 output voltage 1.775V. */
    kPCA9420_Ldo1OutVolt1V800 = 0x40U, /* LDO1 output voltage 1.800V. */
    kPCA9420_Ldo1OutVolt1V825 = 0x50U, /* LDO1 output voltage 1.825V. */
    kPCA9420_Ldo1OutVolt1V850 = 0x60U, /* LDO1 output voltage 1.850V. */
    kPCA9420_Ldo1OutVolt1V875 = 0x70U, /* LDO1 output voltage 1.875V. */
    kPCA9420_Ldo1OutVolt1V900 = 0x80U, /* LDO1 output voltage 1.900V. */
} pca9420_ldo1_out_t;

/*! @brief PCA9420 Watchdog Timer Setting definition. */
typedef enum _pca9420_wd_timer
{
    kPCA9420_WdTimerDisabled = 0x00U, /* Watch dog timer disabled. */
    kPCA9420_WdTimer16s      = 0x40U, /* Watch dog timer timeout value is 16 sec. */
    kPCA9420_WdTimer32s      = 0x80U, /* Watch dog timer timeout value is 32 sec. */
    kPCA9420_WdTimer64s      = 0xC0U, /* Watch dog timer timeout value is 64 sec. */
} pca9420_wd_timer_t;

/*! @brief PCA9420 LDO2 Output Voltage definition. */
typedef enum _pca9420_ldo2_out
{
    kPCA9420_Ldo2OutVolt1V500 = 0x00U, /* LDO2 output voltage 1.500V. */
    kPCA9420_Ldo2OutVolt1V525 = 0x01U, /* LDO2 output voltage 1.525V. */
    kPCA9420_Ldo2OutVolt1V550 = 0x02U, /* LDO2 output voltage 1.550V. */
    kPCA9420_Ldo2OutVolt1V575 = 0x03U, /* LDO2 output voltage 1.575V. */
    kPCA9420_Ldo2OutVolt1V600 = 0x04U, /* LDO2 output voltage 1.600V. */
    kPCA9420_Ldo2OutVolt1V625 = 0x05U, /* LDO2 output voltage 1.625V. */
    kPCA9420_Ldo2OutVolt1V650 = 0x06U, /* LDO2 output voltage 1.650V. */
    kPCA9420_Ldo2OutVolt1V675 = 0x07U, /* LDO2 output voltage 1.675V. */
    kPCA9420_Ldo2OutVolt1V700 = 0x08U, /* LDO2 output voltage 1.700V. */
    kPCA9420_Ldo2OutVolt1V725 = 0x09U, /* LDO2 output voltage 1.725V. */
    kPCA9420_Ldo2OutVolt1V750 = 0x0AU, /* LDO2 output voltage 1.750V. */
    kPCA9420_Ldo2OutVolt1V775 = 0x0BU, /* LDO2 output voltage 1.775V. */
    kPCA9420_Ldo2OutVolt1V800 = 0x0CU, /* LDO2 output voltage 1.800V. */
    kPCA9420_Ldo2OutVolt1V825 = 0x0DU, /* LDO2 output voltage 1.825V. */
    kPCA9420_Ldo2OutVolt1V850 = 0x0EU, /* LDO2 output voltage 1.850V. */
    kPCA9420_Ldo2OutVolt1V875 = 0x0FU, /* LDO2 output voltage 1.875V. */
    kPCA9420_Ldo2OutVolt1V900 = 0x10U, /* LDO2 output voltage 1.900V. */
    kPCA9420_Ldo2OutVolt1V925 = 0x11U, /* LDO2 output voltage 1.925V. */
    kPCA9420_Ldo2OutVolt1V950 = 0x12U, /* LDO2 output voltage 1.950V. */
    kPCA9420_Ldo2OutVolt1V975 = 0x13U, /* LDO2 output voltage 1.975V. */
    kPCA9420_Ldo2OutVolt2V000 = 0x14U, /* LDO2 output voltage 2.000V. */
    kPCA9420_Ldo2OutVolt2V025 = 0x15U, /* LDO2 output voltage 2.025V. */
    kPCA9420_Ldo2OutVolt2V050 = 0x16U, /* LDO2 output voltage 2.050V. */
    kPCA9420_Ldo2OutVolt2V075 = 0x17U, /* LDO2 output voltage 2.075V. */
    kPCA9420_Ldo2OutVolt2V100 = 0x18U, /* LDO2 output voltage 2.100V. */

    kPCA9420_Ldo2OutVolt2V700 = 0x20U, /* LDO2 output voltage 2.700V. */
    kPCA9420_Ldo2OutVolt2V725 = 0x21U, /* LDO2 output voltage 2.725V. */
    kPCA9420_Ldo2OutVolt2V750 = 0x22U, /* LDO2 output voltage 2.750V. */
    kPCA9420_Ldo2OutVolt2V775 = 0x23U, /* LDO2 output voltage 2.775V. */
    kPCA9420_Ldo2OutVolt2V800 = 0x24U, /* LDO2 output voltage 2.800V. */
    kPCA9420_Ldo2OutVolt2V825 = 0x25U, /* LDO2 output voltage 2.825V. */
    kPCA9420_Ldo2OutVolt2V850 = 0x26U, /* LDO2 output voltage 2.850V. */
    kPCA9420_Ldo2OutVolt2V875 = 0x27U, /* LDO2 output voltage 2.875V. */
    kPCA9420_Ldo2OutVolt2V900 = 0x28U, /* LDO2 output voltage 2.900V. */
    kPCA9420_Ldo2OutVolt2V925 = 0x29U, /* LDO2 output voltage 2.925V. */
    kPCA9420_Ldo2OutVolt2V950 = 0x2AU, /* LDO2 output voltage 2.950V. */
    kPCA9420_Ldo2OutVolt2V975 = 0x2BU, /* LDO2 output voltage 2.975V. */
    kPCA9420_Ldo2OutVolt3V000 = 0x2CU, /* LDO2 output voltage 3.000V. */
    kPCA9420_Ldo2OutVolt3V025 = 0x2DU, /* LDO2 output voltage 3.025V. */
    kPCA9420_Ldo2OutVolt3V050 = 0x2EU, /* LDO2 output voltage 3.050V. */
    kPCA9420_Ldo2OutVolt3V075 = 0x2FU, /* LDO2 output voltage 3.075V. */
    kPCA9420_Ldo2OutVolt3V100 = 0x30U, /* LDO2 output voltage 3.100V. */
    kPCA9420_Ldo2OutVolt3V125 = 0x31U, /* LDO2 output voltage 3.125V. */
    kPCA9420_Ldo2OutVolt3V150 = 0x32U, /* LDO2 output voltage 3.150V. */
    kPCA9420_Ldo2OutVolt3V175 = 0x33U, /* LDO2 output voltage 3.175V. */
    kPCA9420_Ldo2OutVolt3V200 = 0x34U, /* LDO2 output voltage 3.200V. */
    kPCA9420_Ldo2OutVolt3V225 = 0x35U, /* LDO2 output voltage 3.225V. */
    kPCA9420_Ldo2OutVolt3V250 = 0x36U, /* LDO2 output voltage 3.250V. */
    kPCA9420_Ldo2OutVolt3V275 = 0x37U, /* LDO2 output voltage 3.275V. */
    kPCA9420_Ldo2OutVolt3V300 = 0x38U, /* LDO2 output voltage 3.300V. */
} pca9420_ldo2_out_t;

/*! @brief PCA9420 Mode Configuration definition. */
typedef struct _pca9420_modecfg
{
    pca9420_ship_en_t shipModeEnable;
    pca9420_mode_sel_t modeSel;
    pca9420_on_cfg_t onCfg;
    pca9420_wd_timer_t wdogTimerCfg;
    pca9420_sw1_out_t sw1OutVolt;
    pca9420_sw2_out_t sw2OutVolt;
    pca9420_ldo1_out_t ldo1OutVolt;
    pca9420_ldo2_out_t ldo2OutVolt;
    bool enableSw1Out;
    bool enableSw2Out;
    bool enableLdo1Out;
    bool enableLdo2Out;
} pca9420_modecfg_t;

/*! @brief PCA9420 regulators voltage definition. */
typedef struct _pca9420_regulator_mv
{
    uint32_t mVoltSw1;  /* SW1 milli volt. */
    uint32_t mVoltSw2;  /* SW2 milli volt. */
    uint32_t mVoltLdo1; /* LDO1 milli volt. */
    uint32_t mVoltLdo2; /* LDO2 milli volt. */
} pca9420_regulator_mv_t;

/*! @brief PCA9420 Handle definition. */
typedef struct _pca9420_handle
{
    /* Pointer to the user-defined I2C Send Data function. */
    status_t (*I2C_SendFunc)(
        uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, const uint8_t *txBuff, uint8_t txBuffSize);
    /* Pointer to the user-defined I2C Receive Data function. */
    status_t (*I2C_ReceiveFunc)(
        uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, uint8_t *rxBuff, uint8_t rxBuffSize);
    /* The I2C Slave Address Read From OTP. */
    uint8_t slaveAddress;
} pca9420_handle_t;

/*! @brief PCA9420 Configuration Structure definition. */
typedef struct _pca9420_config
{
    /* Pointer to the user-defined I2C Send Data function. */
    status_t (*I2C_SendFunc)(
        uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, const uint8_t *txBuff, uint8_t txBuffSize);
    /* Pointer to the user-defined I2C Receive Data function. */
    status_t (*I2C_ReceiveFunc)(
        uint8_t deviceAddress, uint32_t subAddress, uint8_t subAddressSize, uint8_t *rxBuff, uint8_t rxBuffSize);
    /* Upon VIN plugs in, power up by ON key or not. */
    pca9420_ship_wkup_cfg_t powerUpCfg;
    /* Power down sequence enable */
    pca9420_pwr_dn_en_t startPowerDown;
    /* When the watch dog timer expires, disable charger or not */
    pca9420_chg_in_watchdog_t wdogChargeCtrl;
    /* Power Good Comparators Enable */
    pca9420_pgood_en_t powerGoodEnable;
    /* VIN input current limit. */
    pca9420_vin_ilim_t vinCurrentLimit;
    /* VIN over voltage protection threshold */
    pca9420_vin_ovp_sel_t vinOvpThreshold;
    /* VIN under voltage lockout threshold */
    pca9420_vin_uvlo_sel_t vinUvloThreshold;
    /* VSYS pre-warning threshold */
    pca9420_asys_prewarning_t asysPreWarnThreshold;
    /* VSYS input selection */
    pca9420_asys_input_sel_t asysInputSource;
    /* VSYS under voltage lockout threshold */
    pca9420_asys_uvlo_sel_t asysUvloThreshold;
    /* Charge termination control */
    pca9420_charge_term_dis_t chargeTermDisable;
    /* Thermal shutdown threshold */
    pca9420_them_shdn_t thermalShutdownThreshold;
    /* Die temperature warning threshold */
    pca9420_die_temp_warning_t tempWarnThreshold;
    /* ON pin long glitch timer */
    pca9420_on_glt_long_t onPinTimer;
    /* Disable SW1 active discharge */
    bool disableSw1Bleed;
    /* Disable SW2 active discharge */
    bool disableSw2Bleed;
    /* Disable LDO1 active discharge */
    bool disableLdo1Bleed;
    /* Disable LDO2 active discharge */
    bool disableLdo2Bleed;
    /* The PCA9420 I2C Slave Address. */
    uint8_t slaveAddress;
} pca9420_config_t;

/*!
 * @addtogroup pca9420
 * @{
 */

/*******************************************************************************
 * API
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @name Initialization and Control function
 * @{
 */

/*!
 * @brief Gets the default configuration structure.
 *
 * This function initializes the PCA9420 configuration structure to default values. The default
 * values are as follows.
 *   pca9420Config->I2C_SendFunc              = NULL;
 *   pca9420Config->I2C_ReceiveFunc           = NULL;
 *   pca9420Config->powerUpCfg                = kPCA9420_ShipWkup_PowerUp;
 *   pca9420Config->startPowerDown            = kPCA9420_PwrDnDisabled;
 *   pca9420Config->wdogChargeCtrl            = kPCA9420_ChgInWatchdogChargerContinue;
 *   pca9420Config->powerGoodEnable           = kPCA9420_PGoodEnabled;
 *   pca9420Config->vinCurrentLimit           = kPCA9420_VinIlim_74_85_98;
 *   pca9420Config->vinOvpThreshold           = kPCA9420_VinOvpSel5V5;
 *   pca9420Config->vinUvloThreshold          = kPCA9420_VinUvloSel3V1;
 *   pca9420Config->asysPreWarnThreshold      = kPCA9420_AsysPreWarning3V5;
 *   pca9420Config->asysInputSource           = kPCA9420_AsysInputSelVbatVin;
 *   pca9420Config->asysUvloThreshold         = kPCA9420_AsysUvloSel2V7;
 *   pca9420Config->chargeTermDisable         = kPCA9420_ChargeTermEnabled;
 *   pca9420Config->thermalShutdownThreshold  = kPCA9420_ThemShdn110C;
 *   pca9420Config->tempWarnThreshold         = kPCA9420_DieTempWarn85C;
 *   pca9420Config->onPinTimer                = kPCA9420_OnGltLong8s;
 *   pca9420Config->disableSw1Bleed           = false;
 *   pca9420Config->disableSw2Bleed           = false;
 *   pca9420Config->disableLdo1Bleed          = false;
 *   pca9420Config->disableLdo2Bleed          = false;
 *   pca9420Config->slaveAddress              = PCA9420_DEFAULT_I2C_ADDR;
 *
 * @param config Pointer to the PCA9420 configuration structure.
 */
void PCA9420_GetDefaultConfig(pca9420_config_t *config);

/*!
 * @brief Initializes a PCA9420 instance.
 *
 * This function initializes the PCA9420 regulator with user-defined settings.
 * This example shows how to set up the pca9420_config_t parameters and how
 * to call the PCA9420_Init function by passing in these parameters.
 * @code
 *   pca9420_config_t pca9420Config;
 *   PCA9420_GetDefaultConfig(&pca9420Config);
 *   pca9420Config.I2C_SendFunc    = APP_I2C_SendFunc;
 *   pca9420Config.I2C_ReceiveFunc = APP_I2C_ReceiveFunc;
 *   PCA9420_Init(&pca9420Handle, &pca9420Config);
 * @endcode
 *
 * @param handle PCA9420 Handle.
 * @param config Pointer to the user-defined configuration structure.
 */
void PCA9420_Init(pca9420_handle_t *handle, const pca9420_config_t *config);

/*!
 * @brief Gets the default mode config structure.
 *
 * This function initializes the PCA9420 mode config structure to default values. The default
 * values are as follows.
 *   pca9420ModeCfg->shipModeEnable     = kPCA9420_ShipModeDisabled;
 *   pca9420ModeCfg->modeSel            = kPCA9420_ModeSelPin;
 *   pca9420ModeCfg->onCfg              = kPCA9420_OnCfgDisableModeSwitch;
 *   pca9420ModeCfg->wdogTimerCfg       = kPCA9420_WdTimerDisabled;
 *   pca9420ModeCfg->sw1OutVolt         = kPCA9420_Sw1OutVolt1V000;
 *   pca9420ModeCfg->sw2OutVolt         = kPCA9420_Sw2OutVolt1V800;
 *   pca9420ModeCfg->ldo1OutVolt        = kPCA9420_Ldo1OutVolt1V800;
 *   pca9420ModeCfg->ldo2OutVolt        = kPCA9420_Ldo2OutVolt3V300;
 *   pca9420ModeCfg->enableSw1Out       = true;
 *   pca9420ModeCfg->enableSw2Out       = true;
 *   pca9420ModeCfg->enableLdo1Out      = true;
 *   pca9420ModeCfg->enableLdo2Out      = true;
 *
 * @param config Pointer to the PCA9420 mode configuration structure.
 */
void PCA9420_GetDefaultModeConfig(pca9420_modecfg_t *config);

/*!
 * @brief Gets voltage value in mV from mode configuration.
 *
 * @param config Pointer to the PCA9420 mode configuration structure.
 * @param volt Pointer to the PCA9420 regulator voltage structure.
 */
void PCA9420_GetRegulatorVolt(pca9420_modecfg_t *config, pca9420_regulator_mv_t *volt);

/*!
 * @brief Configure PCA9420 modes for specific modes.
 *
 * This function configures the PCA9420 modes for specific modes with
 * user-defined settings.
 * This example shows how to set up the pca9420_modecfg_t parameters and how
 * to call the PCA9420_WriteModeConfigs function by passing in these parameters.
 * @code
 *   pca9420_modecfg_t pca9420ModeCfg[4];
 *   uint32_t i;
 *   for (i = 0; i < ARRAY_SIZE(pca9420ModeCfg); i++)
 *   {
 *       PCA9420_GetDefaultModeConfig(&pca9420ModeCfg[i]);
 *   }
 *   ...
 *   PCA9420_WriteModeConfigs(&pca9420Handle, kPCA9420_Mode0, &pca9420ModeCfg[0],
 * ARRAY_SIZE(pca9420ModeCfg));
 *   ...
 *   PCA9420_WriteModeConfigs(&pca9420Handle, kPCA9420_Mode2, &pca9420ModeCfg[2], 1);
 * @endcode
 *
 * @param handle PCA9420 Handle.
 * @param modeBase Target mode to start with.
 * @param configs Pointer to the user-defined configuration array.
 * @param num Number of modes to configure.
 */
void PCA9420_WriteModeConfigs(pca9420_handle_t *handle,
                              pca9420_mode_t modeBase,
                              const pca9420_modecfg_t *configs,
                              uint32_t num);

/*!
 * @brief Read PCA9420 mode configurations for specific modes.
 *
 * This function read the PCA9420 mode configurations for specific modes with
 * user-defined settings.
 *
 * @param handle PCA9420 Handle.
 * @param modeBase Target mode to start with.
 * @param configs Pointer to the configuration to save read information.
 * @param num Number of modes to read.
 */
void PCA9420_ReadModeConfigs(pca9420_handle_t *handle,
                             pca9420_mode_t modeBase,
                             pca9420_modecfg_t *configs,
                             uint32_t num);

/*!
 * @brief Switch PCA9420 mode.
 *
 * This function switch the PCA9420 regulators mode with I2C interface.
 *
 * @param handle PCA9420 Handle.
 * @param mode Target mode to go into.
 * @return true on success, false on failure.
 */
bool PCA9420_SwitchMode(pca9420_handle_t *handle, pca9420_mode_t mode);

/*!
 * @brief Get PCA9420 current mode.
 *
 * This function returns the PCA9420 current mode with I2C interface.
 *
 * @param handle PCA9420 Handle.
 * @param mode Current mode.
 * @return true on success, false on failure.
 */
bool PCA9420_GetCurrentMode(pca9420_handle_t *handle, pca9420_mode_t *mode);

/*!
 * @brief Get PCA9420 SW1, SW2, LDO1, LDO2 OK status.
 *
 * This function return the PCA9420 regulators status.
 *
 * @param handle PCA9420 Handle.
 * @return ORed regulator status. see @ref pca9420_reg_status_t for regulator status definition.
 */
uint8_t PCA9420_GetRegulatorStatus(pca9420_handle_t *handle);

/*!
 * @brief Feed PCA9420 watchdog.
 *
 * This function feed the PCA9420 watch dog.
 *
 * @param handle PCA9420 Handle.
 */
void PCA9420_FeedWatchDog(pca9420_handle_t *handle);

/* @} */

/*!
 * @name Basic register access functions
 * @{
 */

/*!
 * @brief Write the value to register array of PCA9420.
 *
 * @param handle Pointer to a valid PCA9420 instance structure.
 * @param regBase variable store base address of register array.
 * @param val variable array to be written to PCA9420.
 * @param size array length in bytes to write.
 * @return true if success or false if error.
 */
bool PCA9420_WriteRegs(pca9420_handle_t *handle, uint8_t regBase, uint8_t *val, uint32_t size);

/*!
 * @brief Read the value of register array in PCA9420.
 *
 * @param handle Pointer to a valid PCA9420 instance structure.
 * @param regBase variable store address of register array.
 * @param val variable array to store return value.
 * @param size array length in bytes to read.
 * @return true if success or false if error.
 */
bool PCA9420_ReadRegs(pca9420_handle_t *handle, uint8_t reg, uint8_t *val, uint32_t size);

/*!
 * @brief Modify some bits in the register in PCA9420.
 * @param handle Pointer to a valid PCA9420 instance structure.
 * @param reg variable store address of register.
 * @param mask The mask code for the bits want to write. The bit you want to write should be 1.
 * @param val Value needs to write into the register.
 * @return true if success or false if error.
 */
bool PCA9420_ModifyReg(pca9420_handle_t *handle, uint8_t reg, uint8_t mask, uint8_t val);

/* @} */

/*!
 * @name Interrupts
 * @{
 */

/*!
 * @brief Enables PCA9420 interrupts according to the provided interrupt source mask.
 *
 * This function enables the PCA9420 interrupts according to the provided interrupt source.
 * The interrupt source mask is a logical OR of enumeration members:
 * see @ref _pca9420_interrupt_source for interrupt sources definition;
 *
 * @param handle Pointer to a valid PCA9420 instance structure.
 * @param source Logic ORed interrupt sources of selected interrupt category to enable.
 */
void PCA9420_EnableInterrupts(pca9420_handle_t *handle, uint32_t source);

/*!
 * @brief Disable PCA9420 interrupts according to the provided interrupt source mask.
 *
 * This function disables the PCA9420 interrupts according to the provided interrupt source.
 * The interrupt source mask is a logical OR of enumeration members:
 * see @ref _pca9420_interrupt_source for interrupt sources definition;
 *
 * @param handle Pointer to a valid PCA9420 instance structure.
 * @param source Logic ORed interrupt sources of selected interrupt category to disable.
 */
void PCA9420_DisableInterrupts(pca9420_handle_t *handle, uint32_t source);

/*!
 * @brief Get interrupt flags.
 *
 * This function gets all interrupt flags.
 * The flags are returned as the logical OR value of the corresponding interrupt source:
 * see @ref _pca9420_interrupt_source for interrupt sources definition;
 *
 * @param handle Pointer to a valid PCA9420 instance structure.
 * @return status flags which are ORed by the enumerators in the corresponding interrupt source.
 */
uint32_t PCA9420_GetInterruptStatus(pca9420_handle_t *handle);

/*!
 * @brief Clear interrupt flags.
 *
 * This function clears interrupt flags of selected source.
 * The interrupt source mask is a logical OR of enumeration members:
 * see @ref _pca9420_interrupt_source for interrupt sources definition;
 *
 * @param handle Pointer to a valid PCA9420 instance structure.
 * @param source Logic ORed interrupt sources of selected interrupt source to clear.
 */
void PCA9420_ClearInterruptStatus(pca9420_handle_t *handle, uint32_t source);
/* @} */

/*!
 * @name mode control functions
 * @{
 */

/* @} */

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* _FSL_PCA9420_H_ */

/*******************************************************************************
 * EOF
 ******************************************************************************/
