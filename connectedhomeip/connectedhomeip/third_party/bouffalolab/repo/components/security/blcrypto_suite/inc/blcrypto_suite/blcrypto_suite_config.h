/**
 * \file config.h
 *
 * \brief Configuration options (set of defines)
 *
 *  This set of compile-time options may be used to enable
 *  or disable features selectively, and reduce the global
 *  memory footprint.
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
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

#ifndef BLCRYPTO_SUITE_CONFIG_H
#define BLCRYPTO_SUITE_CONFIG_H

#define BLCRYPTO_SUITE_PLATFORM_C
#define BLCRYPTO_SUITE_PLATFORM_MEMORY

#define BLCRYPTO_SUITE_ECP_C
#define BLCRYPTO_SUITE_ECP_DP_SECP256R1_ENABLED

#define BLCRYPTO_SUITE_NO_PLATFORM_ENTROPY

#define BLCRYPTO_SUITE_ECP_NO_INTERNAL_RNG
#define BLCRYPTO_SUITE_BIGNUM_C

#include "blcrypto_suite/blcrypto_suite_check_config.h"

#endif /* BLCRYPTO_SUITE_CONFIG_H */
