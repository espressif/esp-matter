/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


/************* Include Files ****************/
#include "cc_pal_mem.h"
#include "cc_ecpki_types.h"
#include "cc_common.h"
#include "cc_common_math.h"
#include "cc_ecpki_error.h"
#include "cc_ecpki_local.h"
#include "pki.h"
#include "ec_wrst.h"
#include "cc_ecpki_types.h"
#include "cc_ecpki_error.h"
#include "cc_fips_defs.h"
#include "cc_ecpki_domains_defs.h"


/************************ Defines ***************************************/

/************************ Enums *****************************************/

/************************ Typedefs **************************************/

/************************ Global Data ***********************************/

extern const getDomainFuncP ecDomainsFuncP[CC_ECPKI_DomainID_OffMode];
/************* Private function prototype *******************************/

/************************ Public Functions ******************************/

/**
 @brief    the function returns the domain pointer
 @return   return domain pointer

*/
const CCEcpkiDomain_t *CC_EcpkiGetEcDomain(CCEcpkiDomainID_t domainId)
{
    if (domainId >= CC_ECPKI_DomainID_OffMode) {
        return NULL;
    }

    if (ecDomainsFuncP[domainId] == NULL) {
        return NULL;
    }

    return ((ecDomainsFuncP[domainId])());
}
