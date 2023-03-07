/*
 * Copyright (c) 2018-2020, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __TFM_SVCALLS_H__
#define __TFM_SVCALLS_H__

#include <stdint.h>

/**
 * \brief The C source of SVCall handlers
 *
 * \param[in] svc_args          The arguments list.
 * \param[in] exc_return        EXC_RETURN value of the SVC.
 *
 * \returns                     EXC_RETURN value indicates where to return.
 */
uint32_t tfm_core_svc_handler(uint32_t *svc_args, uint32_t exc_return);

#endif
