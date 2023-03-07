/*
 * Copyright (c) 2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PSA_CONFIG_H__
#define __PSA_CONFIG_H__

/*
 * This is an implementation-specific file that supporting PSA standard
 * files. The reason to name with prefix 'psa' is to showcase a straight
 * reference configuration. Because both NSPE and SPE clients are using
 * the same API but link with the different declarations, this file can
 * help to indicate the current building to apply PE-specific or
 * configuration-specific settings.
 */

/*
 * A customized flag indicating SPE build. Systems like CMSIS may use
 * 'DOMAIN_NS' as the indicator. As NSPE is not guaranteed to be
 * CMSIS compatible, a customized flag provides availability.
 */
#if defined(CONFIG_TFM_BUILDING_SPE)

/* SPE has specific configurations. */
#include "config_impl.h"

#endif

#endif /* __PSA_CONFIG_H__ */
