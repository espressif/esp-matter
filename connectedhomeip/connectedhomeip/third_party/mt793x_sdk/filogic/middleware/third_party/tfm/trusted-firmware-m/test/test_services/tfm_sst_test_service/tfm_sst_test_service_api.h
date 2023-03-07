/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_SST_TEST_SERVICE_API_H__
#define __TFM_SST_TEST_SERVICE_API_H__

#include "psa/error.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Requests the SST Test Service to call sst_system_prepare().
 *
 * \return Returns error code as specified in \ref psa_status_t
 */
psa_status_t tfm_sst_test_system_prepare(void);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_SST_TEST_SERVICE_API_H__ */
