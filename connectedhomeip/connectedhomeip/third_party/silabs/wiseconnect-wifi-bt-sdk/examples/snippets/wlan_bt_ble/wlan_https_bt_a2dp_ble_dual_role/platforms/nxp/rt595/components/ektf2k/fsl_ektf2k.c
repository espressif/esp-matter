/*
 * Copyright  2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_ektf2k.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief EKTF2K I2C address. */
#define EKTF2K_I2C_ADDRESS (0x15)

/*! @brief EKTF2K hello packet data length. */
#define EKTF2K_TOUCH_HELLO_DATA_LEN (0x04)

/*! @brief Retry times to get the hello packet. */
#define EKTF2K_TOUCH_HELLO_RETRY_TIME (20)

__PACKED_STRUCT _ektf2k_touch_point
{
    uint8_t XHYH;
    uint8_t XL;
    uint8_t YL;
};

typedef struct _ektf2k_touch_point ektf2k_touch_point_t;

__PACKED_STRUCT _ektf2k_touch_data
{
    uint8_t PKT_TYPE;
    ektf2k_touch_point_t TOUCH[EKTF2K_MAX_TOUCHES];
    uint8_t BTN_FID;
};

typedef struct _ektf2k_touch_data ektf2k_touch_data_t;

enum _ektf2k_pkt_type
{
    kEKTF2K_Pkt2Finger = 0x5A,
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

status_t EKTF2K_Init(ektf2k_handle_t *handle, const ektf2k_config_t *config)
{
    status_t status;
    uint32_t helloPkt;
    int retryTime         = EKTF2K_TOUCH_HELLO_RETRY_TIME;
    const uint8_t cmd_x[] = {0x53, 0x60, 0x00, 0x00};
    const uint8_t cmd_y[] = {0x53, 0x63, 0x00, 0x00};
    uint8_t buf_recv[4];

    assert(handle);

    memset(handle, 0, sizeof(*handle));

    handle->I2C_SendFunc    = config->I2C_SendFunc;
    handle->I2C_ReceiveFunc = config->I2C_ReceiveFunc;
    handle->timeDelayMsFunc = config->timeDelayMsFunc;
    handle->pullResetPin    = config->pullResetPin;

    /* Reset the panel. */
    handle->pullResetPin(false);
    handle->timeDelayMsFunc(5);
    handle->pullResetPin(true);

    handle->timeDelayMsFunc(400);

    while (--retryTime > 0)
    {
        status = handle->I2C_ReceiveFunc(EKTF2K_I2C_ADDRESS, 0, 0, (uint8_t *)&helloPkt, 4);

        if ((kStatus_Success == status) && (helloPkt == 0x55555555))
        {
            break;
        }
        else
        {
            handle->timeDelayMsFunc(5);
        }
    }

    if (retryTime <= 0)
    {
        return kStatus_Fail;
    }

    status = handle->I2C_SendFunc(EKTF2K_I2C_ADDRESS, 0, 0, cmd_x, 4);
    if (kStatus_Success != status)
    {
        return status;
    }

    status = handle->I2C_ReceiveFunc(EKTF2K_I2C_ADDRESS, 0, 0, buf_recv, 4);
    if (kStatus_Success != status)
    {
        return status;
    }

    handle->resolutionX = ((buf_recv[2])) | ((buf_recv[3] & 0xf0U) << 4U);

    status = handle->I2C_SendFunc(EKTF2K_I2C_ADDRESS, 0, 0, cmd_y, 4);
    if (kStatus_Success != status)
    {
        return status;
    }

    status = handle->I2C_ReceiveFunc(EKTF2K_I2C_ADDRESS, 0, 0, buf_recv, 4);
    if (kStatus_Success != status)
    {
        return status;
    }

    handle->resolutionY = ((buf_recv[2])) | ((buf_recv[3] & 0xf0U) << 4U);

    return kStatus_Success;
}

status_t EKTF2K_Deinit(ektf2k_handle_t *handle)
{
    handle->pullResetPin(false);
    return kStatus_Success;
}

static void EKTF2K_ParseTouchData(const ektf2k_touch_point_t *data, int *touch_x, int *touch_y)
{
    *touch_x = data->XL + ((data->XHYH & 0xF0U) << 4U);
    *touch_y = data->YL + ((data->XHYH & 0x0FU) << 8U);
}

status_t EKTF2K_GetSingleTouch(ektf2k_handle_t *handle, int *touch_x, int *touch_y)
{
    status_t status;
    uint32_t i;

    touch_point_t touch_array[EKTF2K_MAX_TOUCHES];

    status = EKTF2K_GetMultiTouch(handle, touch_array);

    if (kStatus_Success == status)
    {
        for (i = 0; i < EKTF2K_MAX_TOUCHES; i++)
        {
            if (touch_array[i].valid)
            {
                *touch_x = touch_array[i].x;
                *touch_y = touch_array[i].y;
                break;
            }
        }

        if (i >= EKTF2K_MAX_TOUCHES)
        {
            status = kStatus_Fail;
        }
    }

    return status;
}

status_t EKTF2K_GetMultiTouch(ektf2k_handle_t *handle, touch_point_t touch_array[EKTF2K_MAX_TOUCHES])
{
    status_t status;
    uint32_t i;
    int x, y;

    for (i = 0; i < EKTF2K_MAX_TOUCHES; i++)
    {
        touch_array[i].valid = false;
    }

    status = handle->I2C_ReceiveFunc(EKTF2K_I2C_ADDRESS, 0, 0, handle->receiveBuf, EKTF2K_TOUCH_DATA_LEN);

    if (status == kStatus_Success)
    {
        ektf2k_touch_data_t *touch_data = (ektf2k_touch_data_t *)(void *)(handle->receiveBuf);

        if (kEKTF2K_Pkt2Finger == touch_data->PKT_TYPE)
        {
            /* Decode valid touch points */
            for (i = 0; i < EKTF2K_MAX_TOUCHES; i++)
            {
                touch_array[i].touchID = i;

                if (touch_data->BTN_FID & (1U << i))
                {
                    EKTF2K_ParseTouchData(&touch_data->TOUCH[i], &x, &y);
                    touch_array[i].x     = x;
                    touch_array[i].y     = y;
                    touch_array[i].valid = true;
                }
            }
        }
        else
        {
            status = kStatus_Fail;
        }
    }

    return status;
}

status_t EKTF2K_GetResolution(ektf2k_handle_t *handle, int *resolutionX, int *resolutionY)
{
    *resolutionX = handle->resolutionX;
    *resolutionY = handle->resolutionY;

    return kStatus_Success;
}
