/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_IOCTL_API__
#define __TFM_IOCTL_API__

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "tfm_api.h"
#include "tfm_platform_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \enum tfm_platform_ioctl_reqest_types_t
 *
 * \brief ioctl request types (supported types may vary based on the platform)
 */
enum tfm_platform_ioctl_reqest_types_t {
    TFM_PLATFORM_IOCTL_BASIC_REQUEST = 0,
    TFM_PLATFORM_IOCTL_POWER_REQUEST,
    TFM_PLATFORM_IOCTL_INVALID_REQUEST = INT_MAX      /*!< Max to force enum max size */
};

/*!
 * \enum tfm_basic_request_type_t
 *
 * \brief basic request types (supported types may vary based on the platform)
 */
enum tfm_basic_request_type_t {
    TFM_PLATFORM_IOCTL_BASIC_REQUEST_TYPE_INIT = 0,          /*!< Init */
    TFM_PLATFORM_IOCTL_BASIC_REQUEST_TYPE_CONFIG,            /*!< Basic config */
    TFM_PLATFORM_IOCTL_BASIC_REQUEST_TYPE_READ,            /*!< Basic read */
    TFM_PLATFORM_IOCTL_BASIC_REQUEST_TYPE_MAX = INT_MAX      /*!< Max to force enum max size */
};

/*!
 * \enum basic_request_error_t
 *
 * \brief basic request error types (supported types may vary based on the platform)
 */
enum basic_request_error_t {
    BASIC_REQUEST_ERR_NONE = 0,      /*!< No error */
    BASIC_REQUEST_ERR_INVALID_ARG,   /*!< Error invalid input argument */
};

/*!
 * \struct tfm_basic_request_args_t
 *
 * \brief Argument list for each basic platform service
 */
typedef struct tfm_basic_request_args_t {
    enum tfm_basic_request_type_t type;
    union {
        struct basic_config_args { /*!< TFM_PLATFORM_IOCTL_BASIC_REQUEST_TYPE_CONFIG */
            uint32_t num_or_mask;
            uint32_t data;
        } basic_config;
    } u;
} tfm_basic_request_args_t;

/*!
 * \struct tfm_basic_request_out_t
 *
 * \brief Output list for each basic platform service
 */
typedef struct tfm_basic_request_out_t {
    union {
        uint32_t result;               /*!< Generic result */
        struct basic_read_result { /*!< TFM_PLATFORM_IOCTL_BASIC_REQUEST_TYPE_READ */
            uint32_t result;
            uint32_t data;
        } basic_read_result;
    } u;
} tfm_basic_request_out_t;

#ifdef __cplusplus
}
#endif

#endif /* __TFM_IOCTL_API__ */
