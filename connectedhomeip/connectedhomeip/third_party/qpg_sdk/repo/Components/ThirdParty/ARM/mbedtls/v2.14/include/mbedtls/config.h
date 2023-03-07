/**
 * \file config.h
 *
 * \brief Configuration options (set of defines)
 *
 *  This set of compile-time options may be used to enable
 *  or disable features selectively, and reduce the global
 *  memory footprint.
 */
/*
 *  Copyright (C) 2006-2018, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

/* Mbedtls supports defining the USER CONFIG FILE via commandline argument
 * But passing string arguments via command line does not work with Qorvo
 * framework for IAR builds.
 */
#ifdef GP_TLS_DIVERSITY_USER_DEFINED_MBEDTLS_CONFIG
/* User defined mbedtls config */
#include "mbedtls_user_config.h"
#else
/* default configuration that comes with ARM mbedtls library
 * Renamed from config.h to mbedtls_default_config.h */
#include "mbedtls_default_config.h"
#endif

#endif /* MBEDTLS_CONFIG_H */
