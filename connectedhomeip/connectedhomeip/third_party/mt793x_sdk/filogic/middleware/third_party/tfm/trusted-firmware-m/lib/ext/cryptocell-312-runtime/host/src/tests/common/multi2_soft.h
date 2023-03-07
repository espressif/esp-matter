/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* file multi2_soft.h
 *  Soft implementatoion for Multi2 (missing in tomcrypt library).
 *  Cipher mode valid: ecb, cbc.
 */

#ifndef __MULTI2_SOFT_H__
#define __MULTI2_SOFT_H__

#include "tomcrypt.h"

int  multi2_soft_ecb_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey);

int multi2_soft_ecb_encrypt(const unsigned char *pt, unsigned char *ct, unsigned long len, symmetric_key *skey);

int multi2_soft_ecb_decrypt(const unsigned char *ct, unsigned char *pt, unsigned long len, symmetric_key *skey);

int  multi2_soft_cbc_setup(const unsigned char *iv, const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey);

int multi2_soft_cbc_encrypt(const unsigned char *pt, unsigned char *ct, unsigned long len, symmetric_key *skey);

int multi2_soft_cbc_decrypt(const unsigned char *ct, unsigned char *pt, unsigned long len, symmetric_key *skey);

void multi2_soft_done(symmetric_key *skey);

#endif  // __MULTI2_SOFT_H__


