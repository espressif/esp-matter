/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_SECURE_CLIENT_2_API_H__
#define __TFM_SECURE_CLIENT_2_API_H__

#include <stddef.h>
#include <stdint.h>

#include "psa/error.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TFM_SECURE_CLIENT_2_ID_ITS_ACCESS_CTRL    1001
#define TFM_SECURE_CLIENT_2_ID_CRYPTO_ACCESS_CTRL 2001

/**
 * \brief Calls the test function with the supplied ID within the execution
 *        context of the Secure Client 2 partition and returns the resulting
 *        status.
 *
 * \param[in] id       The ID of the test function
 * \param[in] arg      Pointer to argument to pass to test function
 * \param[in] arg_len  Length of argument in bytes
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
psa_status_t tfm_secure_client_2_call_test(int32_t id, const void *arg,
                                           size_t arg_len);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_SECURE_CLIENT_2_API_H__ */
