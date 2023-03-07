/**
 * @file ecp_alt_ax.c
 * @author NXP Semiconductors
 * @version 1.0
 * @par License
 *
 * Copyright 2017-2019 NXP
 * SPDX-License-Identifier: Apache-2.0
 *
 * @par Description
 * Implementation of key association between NXP Secure Element and mbedtls.
 *
 *****************************************************************************/

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ECP_C)

#include <string.h>

#include "mbedtls/ecp.h"
#include "mbedtls/version.h"

#if defined(MBEDTLS_ECP_ALT) && SSS_HAVE_MBEDTLS_ALT_SSS

#include <sss_mbedtls.h>

/*
 * Unallocate (the components of) a key pair
 */
void mbedtls_ecp_keypair_free_o(mbedtls_ecp_keypair *key);

void mbedtls_ecp_keypair_free(mbedtls_ecp_keypair *key)
{
    if (key == NULL) {
        return;
    }

    // if ( key->grp.hlse_handle != 0 )
    // {
    //     key->grp.hlse_handle = 0;
    // }
    mbedtls_ecp_keypair_free_o(key);
}

/*
 * Set a group from an ECParameters record (RFC 4492)
 */
int mbedtls_ecp_tls_read_group(mbedtls_ecp_group *grp, const unsigned char **buf, size_t len)
{
    int ret;
    sss_object_t *backup_type_SSS_Object = grp->pSSSObject;
    sss_key_store_t *backup_type_hostKs  = grp->hostKs;
    ret                                  = mbedtls_ecp_tls_read_group_o(grp, buf, len);
    grp->pSSSObject                      = backup_type_SSS_Object;
    grp->hostKs                          = backup_type_hostKs;

    return ret;
}

#endif /* MBEDTLS_ECP_ALT */

#endif /* MBEDTLS_ECP_C */
