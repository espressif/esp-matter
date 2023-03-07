/*
 * Copyright (c) 2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __MTK_CI_TEST_H__
#define __MTK_CI_TEST_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief TFM exception handler test for CI
 */
void tfm_mtk_ci_tfm_excp_handl_test(void);

/**
 * \brief Platform isolation test for CI
 */
int32_t tfm_mtk_ci_plat_iso_test(void);

#ifdef __cplusplus
}
#endif

#endif /* __MTK_CI_TEST_H__ */
