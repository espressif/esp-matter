/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_PLATFORM_CORE_API_H__
#define __TFM_PLATFORM_CORE_API_H__

#include <stdbool.h>

/**
 * \brief Should be called in case of access violation.
 *
 * There might be platform specific means, by which it is possible on a
 * subsystem to detect access violation. For example a platform can have a
 * Peripheral Protection Controller, to detect unauthorised accesses to
 * peripheral registers. Setting up the protection, and handling the violation
 * is implemented in platform specific code. However TF-M should be able to
 * decide how to proceed if a violation happens. So to notify TF-M, platform
 * code have to call this function, if a violation happens.
 */
void tfm_access_violation_handler(void);

/**
 * \brief Return whether a secure partition is privileged.
 *
 * \param[in] partition_idx  The index of the partition in the partition_db.
 *
 * \return True if the partition is privileged, false otherwise.
 */
bool tfm_is_partition_privileged(uint32_t partition_idx);

#endif /* __TFM_PLATFORM_CORE_API_H__ */
