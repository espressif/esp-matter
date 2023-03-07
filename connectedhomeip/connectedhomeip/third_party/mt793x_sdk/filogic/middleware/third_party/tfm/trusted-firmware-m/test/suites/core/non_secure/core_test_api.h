/*
 * Copyright (c) 2017-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CORE_TEST_API_H__
#define __CORE_TEST_API_H__

#include <stdio.h>
#include "cmsis_compiler.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief This structure is to pass iovec arguments to the tfm_core_test_call
 *        function.
 */
struct tfm_core_test_call_args_t {
    struct psa_invec *in_vec;   /*!< Array of psa_invec objects */
    size_t in_len;              /*!< Number psa_invec objects in in_vec */
    struct psa_outvec *out_vec; /*!< Array of psa_outvec objects */
    size_t out_len;             /*!< Number psa_outvec objects in out_vec */
};

/**
 * \brief Calls the secure function provided in \c fn_ptr
 *
 * \param[in] fn_ptr  Secure function to be called.
 * \param[in] args    Arguments for fn_ptr.
 *
 * \return Returns value depending on fn_ptr.
 */
int32_t tfm_core_test_call(int32_t (*fn_ptr)(),
                                        struct tfm_core_test_call_args_t *args);

#ifdef __cplusplus
}
#endif

#endif /* __CORE_TEST_API_H__ */
