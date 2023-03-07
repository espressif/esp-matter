/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdbool.h>
#include "platform/include/tfm_platform_system.h"
#include "platform_description.h"
#include "target_cfg.h"
#include "device_definition.h"
#include "psa/client.h"
#include "services/include/tfm_ioctl_api.h"

void tfm_platform_hal_system_reset(void)
{
    /* Reset the system */
    NVIC_SystemReset();
}

enum tfm_platform_err_t
tfm_platform_hal_gpio_service(const psa_invec  *in_vec,
                              const psa_outvec *out_vec)
{
    struct tfm_gpio_service_args_t *args;
    struct tfm_gpio_service_out_t *out;
    enum gpio_cmsdk_direction_t dir;
    /* Alternate function is configured through the SCC, this is not used
     * on Musca-A, the default value is passed to the driver
     */
    enum gpio_cmsdk_altfunc_t altfunc = GPIO_CMSDK_MAIN_FUNC;

    if (in_vec->len != sizeof(struct tfm_gpio_service_args_t) ||
        out_vec->len != sizeof(struct tfm_gpio_service_out_t)) {
        return TFM_PLATFORM_ERR_INVALID_PARAM;
    }

    args = (struct tfm_gpio_service_args_t *)in_vec->base;
    out = (struct tfm_gpio_service_out_t *)out_vec->base;
    switch (args->type) {
    case TFM_GPIO_SERVICE_TYPE_INIT:
        gpio_cmsdk_init(&GPIO0_CMSDK_DEV_S);
        out->u.result = GPIO_CMSDK_ERR_NONE;
        break;
    case TFM_GPIO_SERVICE_TYPE_PIN_CONFIG:
        dir = (enum gpio_cmsdk_direction_t)args->u.gpio_config.direction;
        out->u.result = gpio_cmsdk_pin_config(
                                            &GPIO0_CMSDK_DEV_S,
                                            args->u.gpio_config.pin_num_or_mask,
                                            dir, altfunc);
        break;
    case TFM_GPIO_SERVICE_TYPE_PIN_WRITE:
        out->u.result = gpio_cmsdk_pin_write(&GPIO0_CMSDK_DEV_S,
                                             args->u.gpio_write.pin_num_or_mask,
                                             args->u.gpio_write.value);
        break;
    case TFM_GPIO_SERVICE_TYPE_PIN_READ:
        out->u.gpio_read_result.result =
                          gpio_cmsdk_pin_read(&GPIO0_CMSDK_DEV_S,
                                              args->u.gpio_read.pin_num_or_mask,
                                              &out->u.gpio_read_result.data);
        break;
    case TFM_GPIO_SERVICE_TYPE_PORT_CONFIG:
        dir = (enum gpio_cmsdk_direction_t)args->u.gpio_config.direction;
        out->u.result = gpio_cmsdk_port_config(
                                            &GPIO0_CMSDK_DEV_S,
                                            args->u.gpio_config.pin_num_or_mask,
                                            dir, altfunc);
        break;
    case TFM_GPIO_SERVICE_TYPE_PORT_WRITE:
        out->u.result = gpio_cmsdk_port_write(
                                             &GPIO0_CMSDK_DEV_S,
                                             args->u.gpio_write.pin_num_or_mask,
                                             args->u.gpio_write.value);
        break;
    case TFM_GPIO_SERVICE_TYPE_PORT_READ:
        out->u.gpio_read_result.result =
                         gpio_cmsdk_port_read(&GPIO0_CMSDK_DEV_S,
                                              args->u.gpio_read.pin_num_or_mask,
                                              &out->u.gpio_read_result.data);
        break;
    default:
        out->u.result = GPIO_CMSDK_ERR_INVALID_ARG;
        break;
    }

    in_vec++;
    out_vec++;

    return TFM_PLATFORM_ERR_SUCCESS;
}


enum tfm_platform_err_t tfm_platform_hal_ioctl(tfm_platform_ioctl_req_t request,
                                               psa_invec  *in_vec,
                                               psa_outvec *out_vec)
{
    switch (request){
    case TFM_PLATFORM_IOCTL_GPIO_SERVICE:
        return tfm_platform_hal_gpio_service(in_vec, out_vec);
    default:
        return TFM_PLATFORM_ERR_NOT_SUPPORTED;
    }
}
