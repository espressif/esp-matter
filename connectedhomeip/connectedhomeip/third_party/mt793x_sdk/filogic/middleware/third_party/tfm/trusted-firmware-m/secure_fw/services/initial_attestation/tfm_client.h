/*
 * Copyright (c) 2019, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_CLIENT_H__
#define __TFM_CLIENT_H__

#include "psa/client.h" /* psa_invec, psa_outvec */

#ifdef __cplusplus
extern "C" {
#endif

/* Intentionally empty, the aim of this file to include psa/client.h, which
 * contains the psa_invec and psa_outvec definitions in TF-M project, but these
 * might has different location in another projects.
 */
#ifdef __cplusplus
}
#endif

#endif /* __TFM_CLIENT_H__ */
