/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef CC_ECPKI_DOMAIN_H
#define CC_ECPKI_DOMAIN_H


/*!
@file
@brief This file defines the ecpki build domain API.
@defgroup cc_ecpki_domain CryptoCell ECC domain APIs
@{
@ingroup cryptocell_ecpki

*/


#include "cc_error.h"
#include "cc_ecpki_types.h"

#ifdef __cplusplus
extern "C"
{
#endif




/**********************************************************************************
 *                CC_EcpkiGetEcDomain function                *
 **********************************************************************************/

/*!
 * @brief  The function returns a pointer to an ECDSA saved domain (one of the supported domains).
 *
 * @return Domain pointer on success.
 * @return NULL on failure.
 */

const CCEcpkiDomain_t *CC_EcpkiGetEcDomain(CCEcpkiDomainID_t domainId /*!< [in] Index of one of the domain Id (must be one of the supported domains). */);

#ifdef __cplusplus
}
#endif

#endif

