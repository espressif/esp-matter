/*
 * Copyright  2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_RM68200_H_
#define _FSL_RM68200_H_

#include "fsl_display.h"
#include "fsl_mipi_dsi_cmd.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*!
 * @brief RM68200 resource.
 */
typedef struct _rm68200_resource
{
    mipi_dsi_device_t dsiDevice;       /*!< MIPI DSI device. */
    void (*pullResetPin)(bool pullUp); /*!< Function to pull reset pin high or low. */
    void (*pullPowerPin)(bool pullUp); /*!< Function to pull power pin high or low. */
} rm68200_resource_t;

extern const display_operations_t rm68200_ops;

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

status_t RM68200_Init(display_handle_t *handle, const display_config_t *config);

status_t RM68200_Deinit(display_handle_t *handle);

status_t RM68200_Start(display_handle_t *handle);

status_t RM68200_Stop(display_handle_t *handle);

#if defined(__cplusplus)
}
#endif

#endif /* _FSL_RM68200_H_ */
