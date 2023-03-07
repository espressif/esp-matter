/*
 * Copyright  2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_gt911.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief GT911 I2C address. */
#define GT911_I2C_ADDRESS0 (0x5D)
#define GT911_I2C_ADDRESS1 (0x14)

#define GT911_REG_ADDR_SIZE 2

/*! @brief GT911 registers. */
#define GT911_REG_ID 0x8140U
#define GT911_CONFIG_ADDR 0x8047U
#define GT911_REG_XL 0x8048U
#define GT911_REG_XH 0x8049U
#define GT911_REG_YL 0x804AU
#define GT911_REG_YH 0x804BU
#define GT911_REG_TOUCH_NUM 0x804CU
#define GT911_REG_MODULE_SWITCH1 0x804DU
#define GT911_REG_STAT 0x814EU
#define GT911_REG_FIRST_POINT 0x814FU

#define GT911_STAT_BUF_MASK (1U << 7U)
#define GT911_STAT_POINT_NUMBER_MASK (0xFU << 0U)
#define GT911_MODULE_SWITCH_X2Y_MASK (1U << 3U)
#define GT911_MODULE_SWITCH_INT_MASK (3U << 0U)

#define GT911_CONFIG_SIZE (186)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

status_t GT911_Init(gt911_handle_t *handle, const gt911_config_t *config)
{
    status_t status;
    uint32_t deviceID;
    uint8_t gt911Config[GT911_CONFIG_SIZE];
    uint8_t i;
    uint8_t sum;

    assert(handle);

    memset(handle, 0, sizeof(*handle));

    handle->I2C_SendFunc     = config->I2C_SendFunc;
    handle->I2C_ReceiveFunc  = config->I2C_ReceiveFunc;
    handle->timeDelayMsFunc  = config->timeDelayMsFunc;
    handle->pullResetPinFunc = config->pullResetPinFunc;

    /* Reset the panel and set the I2C address mode. */
    config->intPinFunc(kGT911_IntPinPullDown);
    config->pullResetPinFunc(false);

    /* >= 10ms. */
    handle->timeDelayMsFunc(20);

    if (kGT911_I2cAddrMode1 == config->i2cAddrMode)
    {
        config->intPinFunc(kGT911_IntPinPullUp);
        handle->i2cAddr = GT911_I2C_ADDRESS1;
    }
    else
    {
        handle->i2cAddr = GT911_I2C_ADDRESS0;
    }

    /* >= 100us */
    handle->timeDelayMsFunc(1);

    config->pullResetPinFunc(true);

    /* >= 5ms */
    handle->timeDelayMsFunc(5);

    config->intPinFunc(kGT911_IntPinInput);

    /* Verify the device. */
    status = handle->I2C_ReceiveFunc(handle->i2cAddr, GT911_REG_ID, GT911_REG_ADDR_SIZE, (uint8_t *)&deviceID, 4);
    if (kStatus_Success != status)
    {
        return status;
    }
    if (deviceID != 0x00313139U)
    {
        return kStatus_Fail;
    }

    /* Initialize the IC. */
    status = handle->I2C_ReceiveFunc(handle->i2cAddr, GT911_CONFIG_ADDR, GT911_REG_ADDR_SIZE, gt911Config,
                                     GT911_CONFIG_SIZE);
    if (kStatus_Success != status)
    {
        return status;
    }

    handle->resolutionX = gt911Config[GT911_REG_XH - GT911_CONFIG_ADDR] << 8U;
    handle->resolutionX += gt911Config[GT911_REG_XL - GT911_CONFIG_ADDR];
    handle->resolutionY = gt911Config[GT911_REG_YH - GT911_CONFIG_ADDR] << 8U;
    handle->resolutionY += gt911Config[GT911_REG_YL - GT911_CONFIG_ADDR];

    gt911Config[GT911_REG_TOUCH_NUM - GT911_CONFIG_ADDR] = (config->touchPointNum) & 0x0FU;

    gt911Config[GT911_REG_MODULE_SWITCH1 - GT911_CONFIG_ADDR] &= ~GT911_MODULE_SWITCH_INT_MASK;
    gt911Config[GT911_REG_MODULE_SWITCH1 - GT911_CONFIG_ADDR] |= (uint8_t)(config->intTrigMode);

    sum = 0;
    for (i = 0; i < GT911_CONFIG_SIZE - 2; i++)
    {
        sum += gt911Config[i];
    }
    gt911Config[i++] = ~sum + 1;
    gt911Config[i]   = 1;

    handle->timeDelayMsFunc(50);

    return handle->I2C_SendFunc(handle->i2cAddr, GT911_CONFIG_ADDR, GT911_REG_ADDR_SIZE, gt911Config,
                                GT911_CONFIG_SIZE);
}

status_t GT911_Deinit(gt911_handle_t *handle)
{
    handle->pullResetPinFunc(false);
    return kStatus_Success;
}

static status_t GT911_ReadRawTouchData(gt911_handle_t *handle, uint8_t *touchPointNum)
{
    status_t status;
    uint8_t gt911Stat;

    status = handle->I2C_ReceiveFunc(handle->i2cAddr, GT911_REG_STAT, GT911_REG_ADDR_SIZE, &gt911Stat, 1);
    if (kStatus_Success != status)
    {
        *touchPointNum = 0;
        return status;
    }

    *touchPointNum = gt911Stat & GT911_STAT_POINT_NUMBER_MASK;

    if (gt911Stat & GT911_STAT_BUF_MASK)
    {
        if (*touchPointNum > 0)
        {
            status = handle->I2C_ReceiveFunc(handle->i2cAddr, GT911_REG_FIRST_POINT, GT911_REG_ADDR_SIZE,
                                             (void *)handle->pointReg, (*touchPointNum) * sizeof(gt911_point_reg_t));
        }

        /* Must set the status register to 0 after read. */
        gt911Stat = 0;
        handle->I2C_SendFunc(handle->i2cAddr, GT911_REG_STAT, GT911_REG_ADDR_SIZE, &gt911Stat, 1);
    }

    return status;
}

status_t GT911_GetSingleTouch(gt911_handle_t *handle, int *touch_x, int *touch_y)
{
    status_t status;
    uint8_t touchPointNum;

    status = GT911_ReadRawTouchData(handle, &touchPointNum);

    if ((kStatus_Success == status) && (touchPointNum > 0))
    {
        *touch_x = handle->pointReg[0].lowX + (handle->pointReg[0].highX << 8U);
        *touch_y = handle->pointReg[0].lowY + (handle->pointReg[0].highY << 8U);
    }
    else
    {
        status = kStatus_Fail;
    }

    return status;
}

status_t GT911_GetMultiTouch(gt911_handle_t *handle, uint8_t *touch_count, touch_point_t touch_array[])
{
    status_t status;
    uint32_t i;
    uint8_t desiredTouchPointNum;
    uint8_t actualTouchPointNum;

    status = GT911_ReadRawTouchData(handle, &actualTouchPointNum);

    if ((kStatus_Success == status) && (actualTouchPointNum > 0))
    {
        desiredTouchPointNum = *touch_count;

        if (actualTouchPointNum > desiredTouchPointNum)
        {
            actualTouchPointNum = desiredTouchPointNum;
        }

        for (i = 0; i < actualTouchPointNum; i++)
        {
            touch_array[i].valid   = true;
            touch_array[i].touchID = handle->pointReg[i].id;
            touch_array[i].x       = handle->pointReg[i].lowX + (handle->pointReg[i].highX << 8U);
            touch_array[i].y       = handle->pointReg[i].lowY + (handle->pointReg[i].highY << 8U);
        }

        for (; i < desiredTouchPointNum; i++)
        {
            touch_array[i].valid = false;
        }
    }
    else
    {
        status = kStatus_Fail;
    }

    *touch_count = actualTouchPointNum;

    return status;
}

status_t GT911_GetResolution(gt911_handle_t *handle, int *resolutionX, int *resolutionY)
{
    *resolutionX = handle->resolutionX;
    *resolutionY = handle->resolutionY;

    return kStatus_Success;
}
