/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _RUN_INTEGRATION_TEST_API_H_
#define _RUN_INTEGRATION_TEST_API_H_

#include "stdint.h"

#include "run_integration_test.h"

RunItError_t runIt_aesTest(void);
RunItError_t runIt_rsaTest(void);
RunItError_t runIt_ccmTest(void);
RunItError_t runIt_gcmTest(void);
RunItError_t runIt_shaTest(void);
RunItError_t runIt_ecdsaTest(void);
RunItError_t runIt_eciesTest(void);
RunItError_t runIt_ecdhTest(void);
RunItError_t runIt_ctrDrbgTest(void);
RunItError_t runIt_keyDerivationTest(void);
RunItError_t runIt_ChachaTest(void);
RunItError_t runIt_srpTest(void);
RunItError_t runIt_macTest(void);
RunItError_t runIt_dhmTest(void);
RunItError_t runIt_assetProvTest(void);
RunItError_t runIt_extDmaTest(void);
RunItError_t runIt_secureBootTest(void);
#endif //_RUN_INTEGRATION_TEST_API_H_
