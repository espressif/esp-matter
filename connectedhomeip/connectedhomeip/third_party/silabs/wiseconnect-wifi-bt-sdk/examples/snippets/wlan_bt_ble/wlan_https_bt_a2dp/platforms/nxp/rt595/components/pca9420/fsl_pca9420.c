/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_common.h"
#include "fsl_pca9420.h"
#include "fsl_power.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/
void PCA9420_GetDefaultConfig(pca9420_config_t *config)
{
    assert(config);

    /* Set callback function to NULL Pointer. */
    config->I2C_SendFunc    = NULL;
    config->I2C_ReceiveFunc = NULL;

    /* Enable power up sequence when VIN on */
    config->powerUpCfg = kPCA9420_ShipWkup_PowerUp;
    /* Disable power down sequence. */
    config->startPowerDown = kPCA9420_PwrDnDisabled;
    /* Continue charging when wdog timeout. */
    config->wdogChargeCtrl = kPCA9420_ChgInWatchdogChargerContinue;
    /* Enable power good detection. */
    config->powerGoodEnable = kPCA9420_PGoodEnabled;
    /* VIN current limit. */
    config->vinCurrentLimit = kPCA9420_VinIlim_370_425_489;
    /* VIN over voltage protection set to 5.5V. */
    config->vinOvpThreshold = kPCA9420_VinOvpSel5V5;
    /* VIN under voltage lockout set to 3.1V. */
    config->vinUvloThreshold = kPCA9420_VinUvloSel3V1;
    /* VSYS pre-warning voltage set to 3.5V. */
    config->asysPreWarnThreshold = kPCA9420_AsysPreWarn3V5;
    /* VSYS input source set to either VBAT or VIN. */
    config->asysInputSource = kPCA9420_AsysInputSelVbatVin;
    /* VSYS under voltage lockout set to 2.7V. */
    config->asysUvloThreshold = kPCA9420_AsysUvloSel2V7;
    /* Fast charge timer enabled. */
    config->chargeTermDisable = kPCA9420_ChargeTermEnabled;
    /* Thermal shutdown temperature set to 110 degree Celsius. */
    config->thermalShutdownThreshold = kPCA9420_ThemShdn110C;
    /* Die warning temperature set to 85 degree Celsius. */
    config->tempWarnThreshold = kPCA9420_DieTempWarn85C;
    /* ON pin long glitch timer set to 8 seconds. */
    config->onPinTimer = kPCA9420_OnGltLong8s;
    /* Enable active discharge control for all regulators. */
    config->disableSw1Bleed  = false;
    config->disableSw2Bleed  = false;
    config->disableLdo1Bleed = false;
    config->disableLdo2Bleed = false;
    /* I2C slave address. */
    config->slaveAddress = PCA9420_DEFAULT_I2C_ADDR;
}

void PCA9420_Init(pca9420_handle_t *handle, const pca9420_config_t *config)
{
    uint8_t topCtl[4], regCtl;
    bool result;

    assert(handle);
    assert(config);

    /* Initialize Callback functions. */
    handle->I2C_SendFunc    = config->I2C_SendFunc;
    handle->I2C_ReceiveFunc = config->I2C_ReceiveFunc;
    /* Set Slave Address. */
    handle->slaveAddress = config->slaveAddress;

    topCtl[0] = ((uint8_t)config->vinCurrentLimit) | ((uint8_t)config->powerUpCfg) | ((uint8_t)config->startPowerDown) |
                ((uint8_t)config->wdogChargeCtrl) | ((uint8_t)config->powerGoodEnable);
    topCtl[1] = ((uint8_t)config->asysPreWarnThreshold) | ((uint8_t)config->asysInputSource) |
                ((uint8_t)config->vinOvpThreshold) | ((uint8_t)config->vinUvloThreshold);
    topCtl[2] = ((uint8_t)config->asysUvloThreshold) | ((uint8_t)config->chargeTermDisable) |
                ((uint8_t)config->thermalShutdownThreshold) | ((uint8_t)config->tempWarnThreshold);
    topCtl[3] = ((uint8_t)config->onPinTimer);
    regCtl    = (config->disableSw1Bleed ? (uint8_t)kPCA9420_RegCtlSw1Bleed : 0) |
             (config->disableSw2Bleed ? (uint8_t)kPCA9420_RegCtlSw2Bleed : 0) |
             (config->disableLdo1Bleed ? (uint8_t)kPCA9420_RegCtlLdo1Bleed : 0) |
             (config->disableLdo2Bleed ? (uint8_t)kPCA9420_RegCtlLdo2Bleed : 0);

    result = PCA9420_WriteRegs(handle, PCA9420_TOP_CNTL0, topCtl, sizeof(topCtl));
    result = result ? PCA9420_WriteRegs(handle, PCA9420_ACT_DISCHARGE_CNTL_1, &regCtl, 1) : result;
    if (!result)
    {
        assert(false);
    }
}

void PCA9420_GetDefaultModeConfig(pca9420_modecfg_t *config)
{
    /* Don't enter ship mode in this PMIC mode. */
    config->shipModeEnable = kPCA9420_ShipModeDisabled;
    /* Use Pin to select mode. */
    config->modeSel = kPCA9420_ModeSelPin;
    /* No mode switch on ON pin falling edge. */
    config->onCfg = kPCA9420_OnCfgDisableModeSwitch;
    /* Watch dog disabled. */
    config->wdogTimerCfg = kPCA9420_WdTimerDisabled;
    /* SW1 output set to 1.0V. */
    config->sw1OutVolt = kPCA9420_Sw1OutVolt1V000;
    /* SW2 output set to 1.8V. */
    config->sw2OutVolt = kPCA9420_Sw2OutVolt1V800;
    /* LDO1 output set to 1.8V. */
    config->ldo1OutVolt = kPCA9420_Ldo1OutVolt1V800;
    /* LDO2 output set to 3.3V. */
    config->ldo2OutVolt = kPCA9420_Ldo2OutVolt3V300;
    /* All regulators output enabled. */
    config->enableSw1Out  = true;
    config->enableSw2Out  = true;
    config->enableLdo1Out = true;
    config->enableLdo2Out = true;
}

void PCA9420_GetRegulatorVolt(pca9420_modecfg_t *config, pca9420_regulator_mv_t *volt)
{
    assert(config);
    assert(volt);

    /* SW1 voltage */
    if (config->sw1OutVolt <= kPCA9420_Sw1OutVolt1V500)
    {
        volt->mVoltSw1 = 500 + config->sw1OutVolt * 25;
    }
    else if (config->sw1OutVolt < kPCA9420_Sw1OutVolt1V800)
    {
        volt->mVoltSw1 = 1500;
    }
    else
    {
        volt->mVoltSw1 = 1800;
    }
    /* SW2 voltage */
    if (config->sw2OutVolt <= kPCA9420_Sw2OutVolt2V100)
    {
        volt->mVoltSw2 = 1500 + config->sw2OutVolt * 25;
    }
    else if (config->sw2OutVolt < kPCA9420_Sw2OutVolt2V700)
    {
        volt->mVoltSw2 = 2100;
    }
    else if (config->sw2OutVolt <= kPCA9420_Sw2OutVolt3V300)
    {
        volt->mVoltSw2 = 2700 + (config->sw2OutVolt - kPCA9420_Sw2OutVolt2V700) * 25;
    }
    else
    {
        volt->mVoltSw2 = 3300;
    }
    /* LDO1 voltage */
    if (config->ldo1OutVolt <= kPCA9420_Ldo1OutVolt1V900)
    {
        volt->mVoltLdo1 = 1700 + (((uint32_t)config->ldo1OutVolt) >> PCA9420_MODECFG_2_LDO1_OUT_SHIFT) * 25;
    }
    else
    {
        volt->mVoltLdo1 = 1900;
    }
    /* LDO2 voltage */
    if (config->ldo2OutVolt <= kPCA9420_Ldo2OutVolt2V100)
    {
        volt->mVoltLdo2 = 1500 + config->ldo2OutVolt * 25;
    }
    else if (config->ldo2OutVolt < kPCA9420_Ldo2OutVolt2V700)
    {
        volt->mVoltLdo2 = 2100;
    }
    else if (config->ldo2OutVolt <= kPCA9420_Ldo2OutVolt3V300)
    {
        volt->mVoltLdo2 = 2700 + (config->ldo2OutVolt - kPCA9420_Ldo2OutVolt2V700) * 25;
    }
    else
    {
        volt->mVoltLdo2 = 3300;
    }
}

void PCA9420_WriteModeConfigs(pca9420_handle_t *handle,
                              pca9420_mode_t modeBase,
                              const pca9420_modecfg_t *configs,
                              uint32_t num)
{
    uint8_t modeCfgRegBase;
    uint8_t modeCfg[16];
    uint32_t i;
    bool result;

    assert(num >= 1 && num <= 4);

    switch (modeBase)
    {
        case kPCA9420_Mode0:
            modeCfgRegBase = PCA9420_MODECFG_0_0;
            break;
        case kPCA9420_Mode1:
            modeCfgRegBase = PCA9420_MODECFG_1_0;
            break;
        case kPCA9420_Mode2:
            modeCfgRegBase = PCA9420_MODECFG_2_0;
            break;
        case kPCA9420_Mode3:
            modeCfgRegBase = PCA9420_MODECFG_3_0;
            break;
        default:
            assert(false);
            return;
    }

    for (i = 0; i < num; i++)
    {
        modeCfg[i * 4] = ((uint8_t)(configs[i].shipModeEnable)) | ((uint8_t)(configs[i].modeSel)) |
                         ((uint8_t)(configs[i].sw1OutVolt));
        modeCfg[i * 4 + 1] = ((uint8_t)(configs[i].onCfg)) | ((uint8_t)(configs[i].sw2OutVolt));
        modeCfg[i * 4 + 2] = ((uint8_t)(configs[i].ldo1OutVolt)) |
                             (configs[i].enableSw1Out ? (uint8_t)kPCA9420_RegulatorSwitch1 : 0) |
                             (configs[i].enableSw2Out ? (uint8_t)kPCA9420_RegulatorSwitch2 : 0) |
                             (configs[i].enableLdo1Out ? (uint8_t)kPCA9420_RegulatorLdo1 : 0) |
                             (configs[i].enableLdo2Out ? (uint8_t)kPCA9420_RegulatorLdo2 : 0);
        modeCfg[i * 4 + 3] = ((uint8_t)(configs[i].wdogTimerCfg)) | ((uint8_t)(configs[i].ldo2OutVolt));
    }

    result = PCA9420_WriteRegs(handle, modeCfgRegBase, modeCfg, 4 * num);
    if (!result)
    {
        assert(false);
    }
}

void PCA9420_ReadModeConfigs(pca9420_handle_t *handle,
                             pca9420_mode_t modeBase,
                             pca9420_modecfg_t *configs,
                             uint32_t num)
{
    uint8_t modeCfgRegBase;
    uint8_t modeCfg[16];
    uint32_t i;
    bool result;

    assert(num >= 1 && num <= 4);

    switch (modeBase)
    {
        case kPCA9420_Mode0:
            modeCfgRegBase = PCA9420_MODECFG_0_0;
            break;
        case kPCA9420_Mode1:
            modeCfgRegBase = PCA9420_MODECFG_1_0;
            break;
        case kPCA9420_Mode2:
            modeCfgRegBase = PCA9420_MODECFG_2_0;
            break;
        case kPCA9420_Mode3:
            modeCfgRegBase = PCA9420_MODECFG_3_0;
            break;
        default:
            assert(false);
            return;
    }

    result = PCA9420_ReadRegs(handle, modeCfgRegBase, modeCfg, 4 * num);
    if (!result)
    {
        assert(false);
    }

    for (i = 0; i < num; i++)
    {
        configs[i].shipModeEnable = (pca9420_ship_en_t)(modeCfg[i * 4] & PCA9420_MODECFG_0_SHIP_EN_MASK);
        configs[i].modeSel        = (pca9420_mode_sel_t)(modeCfg[i * 4] & PCA9420_MODECFG_0_MODE_CTRL_SEL_MASK);
        configs[i].sw1OutVolt     = (pca9420_sw1_out_t)(modeCfg[i * 4] & PCA9420_MODECFG_0_SW1_OUT_MASK);

        configs[i].onCfg      = (pca9420_on_cfg_t)(modeCfg[i * 4 + 1] & PCA9420_MODECFG_1_ON_CFG_MASK);
        configs[i].sw2OutVolt = (pca9420_sw2_out_t)(modeCfg[i * 4 + 1] & PCA9420_MODECFG_1_SW2_OUT_MASK);

        configs[i].ldo1OutVolt   = (pca9420_ldo1_out_t)(modeCfg[i * 4 + 2] & PCA9420_MODECFG_2_LDO1_OUT_MASK);
        configs[i].enableSw1Out  = (modeCfg[i * 4 + 2] & ((uint8_t)kPCA9420_RegulatorSwitch1)) ? true : false;
        configs[i].enableSw2Out  = (modeCfg[i * 4 + 2] & ((uint8_t)kPCA9420_RegulatorSwitch2)) ? true : false;
        configs[i].enableLdo1Out = (modeCfg[i * 4 + 2] & ((uint8_t)kPCA9420_RegulatorLdo1)) ? true : false;
        configs[i].enableLdo2Out = (modeCfg[i * 4 + 2] & ((uint8_t)kPCA9420_RegulatorLdo2)) ? true : false;

        configs[i].wdogTimerCfg = (pca9420_wd_timer_t)(modeCfg[i * 4 + 3] & PCA9420_MODECFG_3_WD_TIMER_MASK);
        configs[i].ldo2OutVolt  = (pca9420_ldo2_out_t)(modeCfg[i * 4 + 3] & PCA9420_MODECFG_3_LDO2_OUT_MASK);
    }
}

static bool PCA9420_ModeControlledByI2C(pca9420_handle_t *handle, pca9420_mode_t mode)
{
    uint8_t modeCfgReg;
    uint8_t modeCfg0;
    bool result;

    switch (mode)
    {
        case kPCA9420_Mode0:
            modeCfgReg = PCA9420_MODECFG_0_0;
            break;
        case kPCA9420_Mode1:
            modeCfgReg = PCA9420_MODECFG_1_0;
            break;
        case kPCA9420_Mode2:
            modeCfgReg = PCA9420_MODECFG_2_0;
            break;
        case kPCA9420_Mode3:
            modeCfgReg = PCA9420_MODECFG_3_0;
            break;
        default:
            assert(false);
            return false;
    }
    result = PCA9420_ReadRegs(handle, modeCfgReg, &modeCfg0, 1);
    assert(result);
    if ((modeCfg0 & PCA9420_MODECFG_0_MODE_CTRL_SEL_MASK) == kPCA9420_ModeSelI2C)
    {
        result = true;
    }
    else
    {
        result = false;
    }

    return result;
}

bool PCA9420_SwitchMode(pca9420_handle_t *handle, pca9420_mode_t mode)
{
    bool result;

    /* Switch by Pins first */
    POWER_SetPmicMode((uint32_t)mode, kCfg_Run);

    /* Switch by I2C next to make sure switch succeeds no matter modes are controlled by Pins or I2C. */
    result = PCA9420_ModifyReg(handle, PCA9420_TOP_CNTL3, PCA9420_TOP_CNTL3_MODE_I2C_MASK,
                               ((uint8_t)mode) << PCA9420_TOP_CNTL3_MODE_I2C_SHIFT);

    return result;
}

bool PCA9420_GetCurrentMode(pca9420_handle_t *handle, pca9420_mode_t *mode)
{
    bool result = true;
    uint8_t regValue;
    pca9420_mode_t pinMode, i2cMode;

    assert(mode);

    pinMode = (pca9420_mode_t)(POWER_GetPmicMode(kCfg_Run));

    if (!PCA9420_ModeControlledByI2C(handle, pinMode))
    {
        *mode = pinMode;
    }
    else
    {
        result = PCA9420_ReadRegs(handle, PCA9420_TOP_CNTL3, &regValue, 1);
        if (result)
        {
            i2cMode =
                (pca9420_mode_t)((regValue & PCA9420_TOP_CNTL3_MODE_I2C_MASK) >> PCA9420_TOP_CNTL3_MODE_I2C_SHIFT);
            *mode = i2cMode;
        }
    }

    return result;
}

uint8_t PCA9420_GetRegulatorStatus(pca9420_handle_t *handle)
{
    uint8_t status = 0;

    /* powerGoodEnable must be configured to true, otherwise the status is always 0. */
    PCA9420_ReadRegs(handle, PCA9420_REG_STATUS, &status, 1);

    return status;
}

void PCA9420_FeedWatchDog(pca9420_handle_t *handle)
{
    uint8_t regValue = 1;

    PCA9420_WriteRegs(handle, PCA9420_TOP_CNTL4, &regValue, 1);
}

bool PCA9420_WriteRegs(pca9420_handle_t *handle, uint8_t reg, uint8_t *val, uint32_t size)
{
    assert(handle);
    assert(handle->I2C_SendFunc);
    assert(val);

    return (kStatus_Success == handle->I2C_SendFunc(handle->slaveAddress, reg, 1U, val, size)) ? true : false;
}

bool PCA9420_ReadRegs(pca9420_handle_t *handle, uint8_t reg, uint8_t *val, uint32_t size)
{
    assert(handle);
    assert(handle->I2C_ReceiveFunc);
    assert(val);

    return (kStatus_Success == handle->I2C_ReceiveFunc(handle->slaveAddress, reg, 1U, val, size)) ? true : false;
}

bool PCA9420_ModifyReg(pca9420_handle_t *handle, uint8_t reg, uint8_t mask, uint8_t val)
{
    bool result;
    uint8_t regValue;

    assert(handle);

    /* Read back the register content. */
    result = PCA9420_ReadRegs(handle, reg, &regValue, 1);
    if (result)
    {
        /* Modify the bit-fields you want to change. */
        regValue &= (uint8_t)~mask;
        regValue |= val;

        /* Write back the content to the registers. */
        result = PCA9420_WriteRegs(handle, reg, &regValue, 1);
    }

    return result;
}

void PCA9420_EnableInterrupts(pca9420_handle_t *handle, uint32_t source)
{
    bool result;
    uint8_t regValues[6];

    assert(handle);

    /* Read back the register content. */
    result = PCA9420_ReadRegs(handle, PCA9420_SUB_INT0, regValues, sizeof(regValues));
    if (!result)
    {
        assert(false);
    }

    regValues[0] = regValues[2] = regValues[4] = 0; /* Don't clear int status */

    regValues[1] &= ~(source & 0xFFU);         /* SUB_INT0_MASK */
    regValues[3] &= ~((source >> 8) & 0xFFU);  /* SUB_INT1_MASK */
    regValues[5] &= ~((source >> 16) & 0xFFU); /* SUB_INT2_MASK */

    result = PCA9420_WriteRegs(handle, PCA9420_SUB_INT0, regValues, sizeof(regValues));
    if (!result)
    {
        assert(false);
    }
}

void PCA9420_DisableInterrupts(pca9420_handle_t *handle, uint32_t source)
{
    bool result;
    uint8_t regValues[6];

    assert(handle);

    /* Read back the register content. */
    result = PCA9420_ReadRegs(handle, PCA9420_SUB_INT0, regValues, sizeof(regValues));
    if (!result)
    {
        assert(false);
    }

    regValues[0] = regValues[2] = regValues[4] = 0; /* Don't clear int status */

    regValues[1] |= (source & 0xFFU);         /* SUB_INT0_MASK */
    regValues[3] |= ((source >> 8) & 0xFFU);  /* SUB_INT1_MASK */
    regValues[5] |= ((source >> 16) & 0xFFU); /* SUB_INT2_MASK */

    result = PCA9420_WriteRegs(handle, PCA9420_SUB_INT0, regValues, sizeof(regValues));
    if (!result)
    {
        assert(false);
    }
}

uint32_t PCA9420_GetInterruptStatus(pca9420_handle_t *handle)
{
    bool result;
    uint8_t regValues[6];

    assert(handle);

    /* Read back the register content. */
    result = PCA9420_ReadRegs(handle, PCA9420_SUB_INT0, regValues, sizeof(regValues));
    if (!result)
    {
        assert(false);
    }

    return (((uint32_t)regValues[4]) << 16) | (((uint32_t)regValues[2]) << 8) | (uint32_t)regValues[0];
}

void PCA9420_ClearInterruptStatus(pca9420_handle_t *handle, uint32_t source)
{
    bool result;
    uint8_t regValues[6];

    assert(handle);

    /* Read back the register content. */
    result = PCA9420_ReadRegs(handle, PCA9420_SUB_INT0, regValues, sizeof(regValues));
    if (!result)
    {
        assert(false);
    }

    regValues[0] = source & 0xFFU;         /* SUB_INT1 */
    regValues[2] = (source >> 8) & 0xFFU;  /* SUB_INT2 */
    regValues[4] = (source >> 16) & 0xFFU; /* SUB_INT3 */

    result = PCA9420_WriteRegs(handle, PCA9420_SUB_INT0, regValues, sizeof(regValues));
    if (!result)
    {
        assert(false);
    }
}
/*******************************************************************************
 * EOF
 ******************************************************************************/
