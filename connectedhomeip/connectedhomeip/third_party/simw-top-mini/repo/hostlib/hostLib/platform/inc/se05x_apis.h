/*
 *
 * Copyright 2018-2019 NXP
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SE05X_API_H
#define _SE05X_API_H

/*
 * Define Reset logic for reset pin on SE
 * Active high for SE050
 */
#define SE_RESET_LOGIC 1

void se05x_ic_reset(void);

#endif // _SE05X_API_H
