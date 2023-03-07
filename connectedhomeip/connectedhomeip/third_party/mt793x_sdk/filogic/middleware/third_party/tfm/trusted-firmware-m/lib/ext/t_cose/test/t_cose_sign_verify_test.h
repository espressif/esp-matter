/*
 *  t_cose_sign_verify_test.h
 *
 * Copyright 2019, Laurence Lundblade
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * See BSD-3-Clause license in README.md
 */

#ifndef t_cose_sign_verify_test_h
#define t_cose_sign_verify_test_h

#include <stdint.h>


/**
 * \file t_cose_sign_verify_test.h
 *
 * \brief Tests that need public key crypto to be implemented
 */


/**
 * \brief Self test using openssl crypto.
 *
 * \return non-zero on failure.
 */
int_fast32_t sign_verify_basic_test(void);


/*
 * Sign some data, perturb the data and see that sig validation fails
 */
int_fast32_t sign_verify_sig_fail_test(void);


/*
 * Make a CWT and compare it to the one in the CWT RFC
 */
int_fast32_t sign_verify_make_cwt_test(void);


/*
 * Test the ability to calculate size of a COSE_Sign1
 */
int_fast32_t sign_verify_get_size_test(void);

#endif /* t_cose_sign_verify_test_h */
