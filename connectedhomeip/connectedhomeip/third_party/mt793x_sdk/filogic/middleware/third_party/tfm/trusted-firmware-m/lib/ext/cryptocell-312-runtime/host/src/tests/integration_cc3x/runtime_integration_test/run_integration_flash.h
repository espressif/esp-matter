/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BSV_INTEGRATION_FLASH_H_
#define _BSV_INTEGRATION_FLASH_H_

#include "stdint.h"
#include "run_integration_test.h"

/**
 * @brief                   Initiate the flash module, should be called before other flash operations.
 *                          The init function should initiate all that is needed to imitate flash operations
 *
 * @param flashSize         [input] size of flash to initialize
 *
 * @return                  RUNIT_ERROR__OK on success. RUNIT_ERROR__FAIL otherwise
 */
RunItError_t runIt_flashInit(size_t flashSize);

/**
 * @brief                   Take the necessary step to close resource allocated in favor of Flash PAL module.
 *                          Could be deallocation or reset of some kind
 *
 * @return                  RUNIT_ERROR__OK on success. RUNIT_ERROR__FAIL otherwise
 */
RunItError_t runIt_flashFinalize(void);

/**
 * @brief                   Write to flash at a certain offset.
 *
 * @param addr              [input] the offset from the start of the flash
 * @param buff              [input] the buffer which to write to flash
 * @param len               [input] length of data to write to flash
 *
 * @return                  RUNIT_ERROR__OK on success. RUNIT_ERROR__FAIL otherwise
 */
RunItError_t runIt_flashWrite(uint32_t addr, uint8_t* buff, size_t len);

/**
 * @brief                   Read from flash to buffer
 *
 * @param addr              [input] the offset from the start of the flash
 * @param buff              [output] the buffer to fill
 * @param len               [input] length of data to read from flash
 *
 * @return                  RUNIT_ERROR__OK on success. RUNIT_ERROR__FAIL otherwise
 */
RunItError_t runIt_flashRead(uint32_t addr, uint8_t* buff, size_t len);

#endif //_BSV_INTEGRATION_FLASH_H_
