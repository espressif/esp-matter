/*
 * Copyright (c) 2022, Arm Limited and Contributors. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "emac_cs300.h"

/* Provide EMAC interface to LwIP. This is required until MDH provides a factory
 * function for it.
 */
mdh_emac_t *mdh_emac_get_default_instance(void)
{
    return cs300_emac_get_default_instance();
}
