/*
 * Copyright  2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_EKTF2K_H_
#define _FSL_EKTF2K_H_

#include "fsl_common.h"

/*!
 * @addtogroup ektf2k
 * @{
 */

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief EKTF2K maximum number of simultaneously detected touches. */
#define EKTF2K_MAX_TOUCHES (2U)

/*! @brief EKTF2K raw touch data length. */
#define EKTF2K_TOUCH_DATA_LEN (0x08)

typedef status_t (*ektf2k_i2c_send_func_t)(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, const uint8_t *txBuff, uint8_t txBuffSize);
typedef status_t (*ektf2k_i2c_receive_func_t)(
    uint8_t deviceAddress, uint32_t subAddress, uint8_t subaddressSize, uint8_t *rxBuff, uint8_t rxBuffSize);

typedef struct _touch_point
{
    bool valid;      /*!< Whether the touch point coordinate value is valid. */
    uint8_t touchID; /*!< Id of the touch point. This numeric value stays constant between down and up event. */
    uint16_t x;      /*!< X coordinate of the touch point */
    uint16_t y;      /*!< Y coordinate of the touch point */
} touch_point_t;

/*! @brief ektf2k configure structure.*/
typedef struct _ektf2k_config
{
    ektf2k_i2c_send_func_t I2C_SendFunc;       /*!< Function to send I2C data. */
    ektf2k_i2c_receive_func_t I2C_ReceiveFunc; /*!< Function to receive I2C data. */
    void (*timeDelayMsFunc)(uint32_t delayMs); /*!< Function to delay some MS. */
    void (*pullResetPin)(bool pullUp);         /*!< Function to pull reset pin high or low. */
} ektf2k_config_t;

/*! @brief ektf2k driver structure.*/
typedef struct _ektf2k_handle
{
    ektf2k_i2c_send_func_t I2C_SendFunc;       /*!< Function to send I2C data. */
    ektf2k_i2c_receive_func_t I2C_ReceiveFunc; /*!< Function to receive I2C data. */
    void (*timeDelayMsFunc)(uint32_t delayMs); /*!< Function to delay some MS. */
    void (*pullResetPin)(bool pullUp);         /*!< Function to pull reset pin high or low. */
    uint8_t receiveBuf[EKTF2K_TOUCH_DATA_LEN]; /*!< The I2C receive buffer. */
    uint16_t resolutionX; /*!< Resolution of the touch IC, it might be different with the display resolution. */
    uint16_t resolutionY; /*!< Resolution of the touch IC, it might be different with the display resolution. */
} ektf2k_handle_t;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Initialize the driver.
 *
 * @param handle Pointer to the EKTF2K driver.
 * @param config Pointer to the configuration.
 * @return Returns @ref kStatus_Success if erase success, otherwise return error code.
 */
status_t EKTF2K_Init(ektf2k_handle_t *handle, const ektf2k_config_t *config);

/*!
 * @brief De-initialize the driver.
 *
 * @param handle Pointer to the EKTF2K driver.
 * @return Returns @ref kStatus_Success if erase success, otherwise return error code.
 */
status_t EKTF2K_Deinit(ektf2k_handle_t *handle);

/*!
 * @brief Get touch IC resolution.
 *
 * Note the touch resolution might be different with display resolution.
 *
 * @param handle Pointer to the EKTF2K driver.
 * @param touch_x X resolution.
 * @param touch_y Y resolution.
 * @return Returns @ref kStatus_Success if erase success, otherwise return error code.
 */
status_t EKTF2K_GetResolution(ektf2k_handle_t *handle, int *resolutionX, int *resolutionY);

/*!
 * @brief Get single touch point coordinate.
 *
 * @param handle Pointer to the EKTF2K driver.
 * @param touch_x X coordinate of the touch point.
 * @param touch_y Y coordinate of the touch point.
 * @return Returns @ref kStatus_Success if erase success, otherwise return error code.
 */
status_t EKTF2K_GetSingleTouch(ektf2k_handle_t *handle, int *touch_x, int *touch_y);

/*!
 * @brief Get multiple touch points coordinate.
 *
 * @param handle Pointer to the EKTF2K driver.
 * @param touch_array Array of touch points coordinate.
 * @return Returns @ref kStatus_Success if erase success, otherwise return error code.
 */
status_t EKTF2K_GetMultiTouch(ektf2k_handle_t *handle, touch_point_t touch_array[EKTF2K_MAX_TOUCHES]);

#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* _FSL_EKTF2K_H_ */
