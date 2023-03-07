/*
 * Copyright (c) 2001-2019, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



/* file multi2_soft.c
 * This code is a patch for multi2 (based on libtomcrypt-1.17
 *   It includes the following additions:
 *  1) bug fix when num_of_rounds%4 !=0
 *  2) num_rounds is not restricted to 128
 *  3) support cbc mode (with IV)
 */

#include "tomcrypt.h"

unsigned char cbc_iv[8];

static void pi1(ulong32 *p)
{
   p[1] ^= p[0];
}

static void pi2(ulong32 *p, ulong32 *k)
{
   ulong32 t;
   t = (p[1] + k[0]) & 0xFFFFFFFFUL;
   t = (ROL(t, 1) + t - 1)  & 0xFFFFFFFFUL;
   t = (ROL(t, 4) ^ t)  & 0xFFFFFFFFUL;
   p[0] ^= t;
}

static void pi3(ulong32 *p, ulong32 *k)
{
   ulong32 t;
   t = p[0] + k[1];
   t = (ROL(t, 2) + t + 1)  & 0xFFFFFFFFUL;
   t = (ROL(t, 8) ^ t)  & 0xFFFFFFFFUL;
   t = (t + k[2])  & 0xFFFFFFFFUL;
   t = (ROL(t, 1) - t)  & 0xFFFFFFFFUL;
   t = ROL(t, 16) ^ (p[0] | t);
   p[1] ^= t;
}

static void pi4(ulong32 *p, ulong32 *k)
{
   ulong32 t;
   t = (p[1] + k[3])  & 0xFFFFFFFFUL;
   t = (ROL(t, 2) + t + 1)  & 0xFFFFFFFFUL;
   p[0] ^= t;
}

static void setup(ulong32 *dk, ulong32 *k, ulong32 *uk)
{
   int n, t;
   ulong32 p[2];

   p[0] = dk[0]; p[1] = dk[1];

   t = 4;
   n = 0;
      pi1(p);
      pi2(p, k);
      uk[n++] = p[0];
      pi3(p, k);
      uk[n++] = p[1];
      pi4(p, k);
      uk[n++] = p[0];
      pi1(p);
      uk[n++] = p[1];
      pi2(p, k+t);
      uk[n++] = p[0];
      pi3(p, k+t);
      uk[n++] = p[1];
      pi4(p, k+t);
      uk[n++] = p[0];
      pi1(p);
      uk[n++] = p[1];
}

static void encrypt(ulong32 *p, int N, ulong32 *uk)
{
   int n, t;
   for (t = n = 0; ; ) {
      pi1(p); if (++n == N) break;
      pi2(p, uk+t); if (++n == N) break;
      pi3(p, uk+t); if (++n == N) break;
      pi4(p, uk+t); if (++n == N) break;
      t ^= 4;
   }
}

static void decrypt(ulong32 *p, int N, ulong32 *uk)
{
   int n, t;
   for (t = 4*(((N-1)>>2)&1), n = N; ;  ) {
      switch (n<=4 ? n : ((n-1)%4)+1) {
         case 4: pi4(p, uk+t); --n;
         case 3: pi3(p, uk+t); --n;
         case 2: pi2(p, uk+t); --n;
         case 1: pi1(p); --n; break;
     case 0: return;
     default: return;
      }
      t ^= 4;
   }
}

int  multi2_soft_ecb_setup(const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
{
   ulong32 sk[8], dk[2];
   int      x;

   LTC_ARGCHK(key  != NULL);
   LTC_ARGCHK(skey != NULL);

   if (keylen != 40) return CRYPT_INVALID_KEYSIZE;

   skey->multi2.N = num_rounds;
   for (x = 0; x < 8; x++) {
       LOAD32H(sk[x], key + x*4);
   }
   LOAD32H(dk[0], key + 32);
   LOAD32H(dk[1], key + 36);
   setup(dk, sk, skey->multi2.uk);

   zeromem(sk, sizeof(sk));
   zeromem(dk, sizeof(dk));
   return CRYPT_OK;
}

/**
  Encrypts a block of text with multi2
  @param pt The input plaintext (8 bytes)
  @param ct The output ciphertext (8 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
int multi2_soft_ecb_encrypt(const unsigned char *pt, unsigned char *ct, unsigned long len, symmetric_key *skey)
{
   ulong32 p[2];
   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);

   if (len % 8) {
      return CRYPT_INVALID_ARG;
   }

   while (len) {

       LOAD32H(p[0], pt);
       LOAD32H(p[1], pt+4);
       encrypt(p, skey->multi2.N, skey->multi2.uk);
       STORE32H(p[0], ct);
       STORE32H(p[1], ct+4);

       ct  += 8;
       pt  += 8;
       len -= 8;
   }

   return CRYPT_OK;
}

/**
  Decrypts a block of text with multi2
  @param ct The input ciphertext (8 bytes)
  @param pt The output plaintext (8 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
int multi2_soft_ecb_decrypt(const unsigned char *ct, unsigned char *pt, unsigned long len, symmetric_key *skey)
{
   ulong32 p[2];
   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);

   if (len % 8) {
      return CRYPT_INVALID_ARG;
   }

   while (len) {

       LOAD32H(p[0], ct);
       LOAD32H(p[1], ct+4);
       decrypt(p, skey->multi2.N, skey->multi2.uk);
       STORE32H(p[0], pt);
       STORE32H(p[1], pt+4);

       ct  += 8;
       pt  += 8;
       len -= 8;
   }

   return CRYPT_OK;
}

int  multi2_soft_cbc_setup(const unsigned char *iv, const unsigned char *key, int keylen, int num_rounds, symmetric_key *skey)
{
   ulong32 sk[8], dk[2];
   int      x;

   LTC_ARGCHK(key  != NULL);
   LTC_ARGCHK(skey != NULL);
   LTC_ARGCHK(iv != NULL);

   if (keylen != 40) return CRYPT_INVALID_KEYSIZE;
   //if (num_rounds == 0) num_rounds = 128;

   skey->multi2.N = num_rounds;
   for (x = 0; x < 8; x++) {
       LOAD32H(sk[x], key + x*4);
   }
   LOAD32H(dk[0], key + 32);
   LOAD32H(dk[1], key + 36);
   setup(dk, sk, skey->multi2.uk);

   zeromem(sk, sizeof(sk));
   zeromem(dk, sizeof(dk));

   /* copy IV */
   memcpy(&cbc_iv,iv,8);

   return CRYPT_OK;
}

/**
  Encrypts a block of text with multi2
  @param pt The input plaintext (8 bytes)
  @param ct The output ciphertext (8 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
int multi2_soft_cbc_encrypt(unsigned char *pt, unsigned char *ct, unsigned long len, symmetric_key *skey)
{
   int      x;
   ulong32 p[2];
   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);

   if (len % 8) {
      return CRYPT_INVALID_ARG;
   }

   while (len) {
       /* xor IV against plaintext */
       for (x = 0; x < 8; x++) {
           pt[x] ^= cbc_iv[x];
       }

       /* encrypt */
       LOAD32H(p[0], pt);
       LOAD32H(p[1], pt+4);
       encrypt(p, skey->multi2.N, skey->multi2.uk);
       STORE32H(p[0], ct);
       STORE32H(p[1], ct+4);

       /* store IV [ciphertext] for a future block */
       for (x = 0; x < 8; x++) {
           cbc_iv[x] = ct[x];
       }

       ct  += 8;
       pt  += 8;
       len -= 8;
   }

   return CRYPT_OK;
}

/**
  Decrypts a block of text with multi2
  @param ct The input ciphertext (8 bytes)
  @param pt The output plaintext (8 bytes)
  @param skey The key as scheduled
  @return CRYPT_OK if successful
*/
int multi2_soft_cbc_decrypt(unsigned char *ct, unsigned char *pt, unsigned long len, symmetric_key *skey)
{
   int      x;
   ulong32 p[2];
   LTC_ARGCHK(pt   != NULL);
   LTC_ARGCHK(ct   != NULL);
   LTC_ARGCHK(skey != NULL);

   if (len % 8) {
      return CRYPT_INVALID_ARG;
   }

   while (len) {

       /* decrypt */
       LOAD32H(p[0], ct);
       LOAD32H(p[1], ct+4);
       decrypt(p, skey->multi2.N, skey->multi2.uk);
       STORE32H(p[0], pt);
       STORE32H(p[1], pt+4);

       /* xor IV against plaintext */
       for (x = 0; x < 8; x++) {
           pt[x] ^= cbc_iv[x];
       }

       /* store IV [ciphertext] for a future block */
       for (x = 0; x < 8; x++) {
           cbc_iv[x] = ct[x];
       }

       ct  += 8;
       pt  += 8;
       len -= 8;
   }

   return CRYPT_OK;
}


/** Terminate the context
   @param skey    The scheduled key
*/
void multi2_soft_done(symmetric_key *skey)
{
}




