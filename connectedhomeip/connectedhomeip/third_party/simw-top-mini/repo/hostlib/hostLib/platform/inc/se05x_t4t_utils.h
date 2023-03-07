/*
 *
 * Copyright 2022 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

/** se05x_check_and_enable_cl_ct
 *
 * See @ref se05x_check_and_enable_cl_ct.
 * Check for platform specific condition and
 * start either contact / contactless interface.
 */
int se05x_check_and_enable_cl_ct(void);

/** se05x_enable_contactless_interface
 *
 * See @ref se05x_enable_contactless_interface.
 * Platform specific implementation to enable
 * contactless interface.
 */
int se05x_enable_contactless_interface(void);

/** se05x_enable_contact_interface
 *
 * See @ref se05x_enable_contact_interface.
 * Platform specific implementation to enable
 * contact interface.
 */
int se05x_enable_contact_interface(void);
