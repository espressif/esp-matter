/*
 * Copyright (c) 1997-2007  The Stanford SRP Authentication Project
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY 
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.  
 *
 * IN NO EVENT SHALL STANFORD BE LIABLE FOR ANY SPECIAL, INCIDENTAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF
 * THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Redistributions in source or binary form must retain an intact copy
 * of this copyright notice.
 */
#include "t_defines.h"
#include "srp.h"
#include "t_sha.h"

/*
 * SRP-6 has two minor refinements relative to SRP-3/RFC2945:
 * 1. The verifier is multipled by three in the server's
 *    calculation for B.
 * 2. The value of u is taken as the hash of A and B,
 *    instead of the top 32 bits of the hash of B.
 *    This eliminates the old restriction where the
 *    server had to receive A before it could send B.
 */

/*
 * The RFC2945 server keeps track of the running hash state via
 * SHA1_CTX structures pointed to by the meth_data pointer.
 * The "hash" member is the hash value that will be sent to the
 * other side; the "ckhash" member is the hash value expected
 * from the other side.  The server also keeps two more "old"
 * hash states, for backwards-compatibility.
 */
struct server_meth_st {
  SHA1_CTX hash;
  SHA1_CTX ckhash;
  SHA1_CTX oldhash;
  SHA1_CTX oldckhash;
  unsigned char k[RFC2945_KEY_LEN];
  unsigned char r[RFC2945_RESP_LEN];
};

#define SERVER_CTXP(srp)    ((struct server_meth_st *)(srp)->meth_data)

/*static SRP_RESULT
srp6_server_init(SRP * srp)
{
  srp->magic = SRP_MAGIC_SERVER;
  srp->flags = SRP_FLAG_MOD_ACCEL;
  srp->meth_data = os_malloc(sizeof(struct server_meth_st));
  SHA1Init(&SERVER_CTXP(srp)->hash);
  SHA1Init(&SERVER_CTXP(srp)->ckhash);
  SHA1Init(&SERVER_CTXP(srp)->oldhash);
  SHA1Init(&SERVER_CTXP(srp)->oldckhash);
  return SRP_SUCCESS;
}*/

static SRP_RESULT
srp6a_server_init(SRP * srp)
{
  srp->magic = SRP_MAGIC_SERVER;
  srp->flags = SRP_FLAG_MOD_ACCEL | SRP_FLAG_LEFT_PAD;
  srp->meth_data = os_malloc(sizeof(struct server_meth_st));
  SHA1Init(&SERVER_CTXP(srp)->hash);
  SHA1Init(&SERVER_CTXP(srp)->ckhash);
  SHA1Init(&SERVER_CTXP(srp)->oldhash);
  SHA1Init(&SERVER_CTXP(srp)->oldckhash);
  return SRP_SUCCESS;
}

static SRP_RESULT
srp6_server_finish(SRP * srp)
{
  if(srp->meth_data) {
    os_memset(srp->meth_data, 0, sizeof(struct server_meth_st));
    os_free(srp->meth_data);
  }
  return SRP_SUCCESS;
}

static SRP_RESULT
srp6_server_params(SRP * srp, const unsigned char * modulus, int modlen,
		   const unsigned char * generator, int genlen,
		   const unsigned char * salt, int saltlen)
{
  unsigned char buf1[SHA_DIGESTSIZE], buf2[SHA_DIGESTSIZE];
  SHA1_CTX ctxt;
  int i;

  /* Fields set by SRP_set_params */

  /* Update hash state */
  SHA1Init(&ctxt);
  SHA1Update(&ctxt, modulus, modlen);
  SHA1Final(buf1, &ctxt);	/* buf1 = H(modulus) */

  SHA1Init(&ctxt);
  SHA1Update(&ctxt, generator, genlen);
  SHA1Final(buf2, &ctxt);	/* buf2 = H(generator) */

  for(i = 0; i < sizeof(buf1); ++i)
    buf1[i] ^= buf2[i];		/* buf1 = H(modulus) XOR H(generator) */

  /* ckhash: H(N) xor H(g) */
  SHA1Update(&SERVER_CTXP(srp)->ckhash, buf1, sizeof(buf1));

  SHA1Init(&ctxt);
  SHA1Update(&ctxt,(unsigned char *) srp->username->data, srp->username->length);
  SHA1Final(buf1, &ctxt);	/* buf1 = H(user) */

  /* ckhash: (H(N) xor H(g)) | H(U) */
  SHA1Update(&SERVER_CTXP(srp)->ckhash, buf1, sizeof(buf1));

  /* ckhash: (H(N) xor H(g)) | H(U) | s */
  SHA1Update(&SERVER_CTXP(srp)->ckhash, salt, saltlen);

  return SRP_SUCCESS;
}

static SRP_RESULT
srp6_server_auth(SRP * srp, const unsigned char * a, int alen)
{
  /* On the server, the authenticator is the verifier */
  srp->verifier = BigIntegerFromBytes(a, alen);

  return SRP_SUCCESS;
}

/* Normally this method isn't called, except maybe by test programs */
static SRP_RESULT
srp6_server_passwd(SRP * srp, const unsigned char * p, int plen)
{
  SHA1_CTX ctxt;
  unsigned char dig[SHA_DIGESTSIZE];

  SHA1Init(&ctxt);
  SHA1Update(&ctxt, (unsigned char *)srp->username->data, srp->username->length);
  SHA1Update(&ctxt, (unsigned char *)":", 1);
  SHA1Update(&ctxt, p, plen);
  SHA1Final(dig, &ctxt);	/* dig = H(U | ":" | P) */

  SHA1Init(&ctxt);
  SHA1Update(&ctxt, (unsigned char *)srp->salt->data, srp->salt->length);
  SHA1Update(&ctxt, dig, sizeof(dig));
  SHA1Final(dig, &ctxt);	/* dig = H(s | H(U | ":" | P)) */
  os_memset(&ctxt, 0, sizeof(ctxt));

  srp->password = BigIntegerFromBytes(dig, sizeof(dig));
  os_memset(dig, 0, sizeof(dig));

  /* verifier = g^x mod N */
  srp->verifier = BigIntegerFromInt(0);
  BigIntegerModExp(srp->verifier, srp->generator, srp->password, srp->modulus, srp->bctx, srp->accel);

  return SRP_SUCCESS;
}

/* NOTE: this clobbers k */
static SRP_RESULT
srp6_server_genpub_ex(SRP * srp, cstr ** result, BigInteger k)
{
  cstr * bstr;
  int slen = (SRP_get_secret_bits(BigIntegerBitLen(srp->modulus)) + 7) / 8;

  if(result == NULL)
    bstr = cstr_new();
  else {
    if(*result == NULL)
      *result = cstr_new();
    bstr = *result;
  }

  cstr_set_length(bstr, BigIntegerByteLen(srp->modulus));
  t_random((unsigned char *)bstr->data, slen);
  srp->secret = BigIntegerFromBytes((unsigned char *)bstr->data, slen);
  srp->pubkey = BigIntegerFromInt(0);

  /* B = kv + g^b mod n (blinding) */
  BigIntegerMul(srp->pubkey, k, srp->verifier, srp->bctx);
  BigIntegerModExp(k, srp->generator, srp->secret, srp->modulus, srp->bctx, srp->accel);
  BigIntegerAdd(k, k, srp->pubkey);
  BigIntegerMod(srp->pubkey, k, srp->modulus, srp->bctx);

  BigIntegerToCstr(srp->pubkey, bstr);

  /* oldckhash: B */
  SHA1Update(&SERVER_CTXP(srp)->oldckhash, (unsigned char *)bstr->data, bstr->length);

  if(result == NULL)	/* bstr was a temporary */
    cstr_clear_free(bstr);

  return SRP_SUCCESS;
}

static SRP_RESULT
srp6_server_genpub(SRP * srp, cstr ** result)
{
  SRP_RESULT ret;
  BigInteger k;

  k = BigIntegerFromInt(3);
  ret = srp6_server_genpub_ex(srp, result, k);
  BigIntegerClearFree(k);
  return ret;
}

static SRP_RESULT
srp6a_server_genpub(SRP * srp, cstr ** result)
{
  SRP_RESULT ret;
  BigInteger k;
  cstr * s;
  SHA1_CTX ctxt;
  unsigned char dig[SHA_DIGESTSIZE];

  SHA1Init(&ctxt);
  s = cstr_new();
  BigIntegerToCstr(srp->modulus, s);
  SHA1Update(&ctxt, (unsigned char *)s->data, s->length);
  if(srp->flags & SRP_FLAG_LEFT_PAD)
    BigIntegerToCstrEx(srp->generator, s, s->length);
  else
    BigIntegerToCstr(srp->generator, s);
  SHA1Update(&ctxt, (unsigned char *)s->data, s->length);
  SHA1Final(dig, &ctxt);
  cstr_free(s);

  k = BigIntegerFromBytes(dig, SHA_DIGESTSIZE);
  if(BigIntegerCmpInt(k, 0) == 0)
    ret = SRP_ERROR;
  else
    ret = srp6_server_genpub_ex(srp, result, k);
  BigIntegerClearFree(k);
  return ret;
}

static SRP_RESULT
srp6_server_key(SRP * srp, cstr ** result,
		const unsigned char * pubkey, int pubkeylen)
{
  cstr * s;
  BigInteger t1, t2, t3;
  SHA1_CTX ctxt;
  unsigned char dig[SHA_DIGESTSIZE];
  int modlen;

  modlen = BigIntegerByteLen(srp->modulus);
  if(pubkeylen > modlen)
    return SRP_ERROR;

  /* ckhash: (H(N) xor H(g)) | H(U) | s | A */
  SHA1Update(&SERVER_CTXP(srp)->ckhash, pubkey, pubkeylen);

  s = cstr_new();
  BigIntegerToCstr(srp->pubkey, s);	/* get encoding of B */

  /* ckhash: (H(N) xor H(g)) | H(U) | s | A | B */
  SHA1Update(&SERVER_CTXP(srp)->ckhash, (unsigned char *)s->data, s->length);

  /* hash: A */
  SHA1Update(&SERVER_CTXP(srp)->hash, pubkey, pubkeylen);
  /* oldhash: A */
  SHA1Update(&SERVER_CTXP(srp)->oldhash, pubkey, pubkeylen);

  /* Compute u = H(pubkey || mypubkey) */
  SHA1Init(&ctxt);
  if(srp->flags & SRP_FLAG_LEFT_PAD) {
    if(pubkeylen < modlen) {
      cstr_set_length(s, modlen);
      os_memcpy(s->data + (modlen - pubkeylen), pubkey, pubkeylen);
      os_memset(s->data, 0, modlen - pubkeylen);
      SHA1Update(&ctxt, (unsigned char *)s->data, modlen);
      BigIntegerToCstrEx(srp->pubkey, s, modlen);
    }
    else {
      SHA1Update(&ctxt, pubkey, pubkeylen);
      if(s->length < modlen)
	BigIntegerToCstrEx(srp->pubkey, s, modlen);
    }
  }
  else {
    SHA1Update(&ctxt, pubkey, pubkeylen);
  }
  SHA1Update(&ctxt,(unsigned char *) s->data, s->length);
  SHA1Final(dig, &ctxt);	/* dig = H(A || B) */
  srp->u = BigIntegerFromBytes(dig, SHA_DIGESTSIZE);

  /* compute A*v^u */
  t1 = BigIntegerFromInt(0);
  BigIntegerModExp(t1, srp->verifier, srp->u, srp->modulus, srp->bctx, srp->accel); /* t1 = v^u */
  t2 = BigIntegerFromBytes(pubkey, pubkeylen); /* t2 = A */
  t3 = BigIntegerFromInt(0);
  BigIntegerModMul(t3, t2, t1, srp->modulus, srp->bctx); /* t3 = A*v^u (mod N) */
  BigIntegerFree(t2);

  if(BigIntegerCmpInt(t3, 1) <= 0) {	/* Reject A*v^u == 0,1 (mod N) */
    BigIntegerClearFree(t1);
    BigIntegerClearFree(t3);
    cstr_free(s);
    return SRP_ERROR;
  }

  BigIntegerAddInt(t1, t3, 1);
  if(BigIntegerCmp(t1, srp->modulus) == 0) {  /* Reject A*v^u == -1 (mod N) */
    BigIntegerClearFree(t1);
    BigIntegerClearFree(t3);
    cstr_free(s);
    return SRP_ERROR;
  }

  srp->key = BigIntegerFromInt(0);
  BigIntegerModExp(srp->key, t3, srp->secret, srp->modulus, srp->bctx, srp->accel);  /* (Av^u)^b */
  BigIntegerClearFree(t1);
  BigIntegerClearFree(t3);

  /* convert srp->key into session key, update hashes */
  BigIntegerToCstr(srp->key, s);

  // As HomeKit Accessory Protocol Pairing Specification R1, change function from t_mgf1 to SHA512
  //t_mgf1(SERVER_CTXP(srp)->k, RFC2945_KEY_LEN, (unsigned char *)s->data, s->length); /* Interleaved hash */
  //SHA512((unsigned char *)s->data, s->length,SERVER_CTXP(srp)->k);
  mbedtls_sha512((unsigned char *)s->data, s->length,SERVER_CTXP(srp)->k, 0);


  cstr_clear_free(s);

  /* ckhash: (H(N) xor H(g)) | H(U) | s | A | B | K */
  SHA1Update(&SERVER_CTXP(srp)->ckhash, SERVER_CTXP(srp)->k, RFC2945_KEY_LEN);
  /* ckhash: (H(N) xor H(g)) | H(U) | s | A | B | K | ex_data */
  if(srp->ex_data->length > 0)
    SHA1Update(&SERVER_CTXP(srp)->ckhash,
	       (unsigned char *)srp->ex_data->data, srp->ex_data->length);

  /* oldhash: A | K */
  SHA1Update(&SERVER_CTXP(srp)->oldhash, SERVER_CTXP(srp)->k, RFC2945_KEY_LEN);
  /* oldckhash: B | K */
  SHA1Update(&SERVER_CTXP(srp)->oldckhash, SERVER_CTXP(srp)->k,
	     RFC2945_KEY_LEN);

  if(result) {
    if(*result == NULL)
      *result = cstr_new();
    cstr_setn(*result, (char *)SERVER_CTXP(srp)->k, RFC2945_KEY_LEN);
  }

  return SRP_SUCCESS;
}

static SRP_RESULT
srp6_server_verify(SRP * srp, const unsigned char * proof, int prooflen)
{
  unsigned char expected[SHA_DIGESTSIZE];

  SHA1Final(expected, &SERVER_CTXP(srp)->oldckhash);
  if(prooflen == RFC2945_RESP_LEN && os_memcmp(expected, proof, prooflen) == 0) {
    SHA1Final(SERVER_CTXP(srp)->r, &SERVER_CTXP(srp)->oldhash);
    return SRP_SUCCESS;
  }
  SHA1Final(expected, &SERVER_CTXP(srp)->ckhash);
  if(prooflen == RFC2945_RESP_LEN && os_memcmp(expected, proof, prooflen) == 0) {
    /* hash: A | M | K */
    SHA1Update(&SERVER_CTXP(srp)->hash, expected, sizeof(expected));
    SHA1Update(&SERVER_CTXP(srp)->hash, SERVER_CTXP(srp)->k, RFC2945_KEY_LEN);
    SHA1Final(SERVER_CTXP(srp)->r, &SERVER_CTXP(srp)->hash);
    return SRP_SUCCESS;
  }
  return SRP_ERROR;
}

static SRP_RESULT
srp6_server_respond(SRP * srp, cstr ** proof)
{
  if(proof == NULL)
    return SRP_ERROR;

  if(*proof == NULL)
    *proof = cstr_new();

  cstr_set_length(*proof, RFC2945_RESP_LEN);
  os_memcpy((*proof)->data, SERVER_CTXP(srp)->r, RFC2945_RESP_LEN);
  return SRP_SUCCESS;
}

/*static SRP_METHOD srp6_server_meth = {
  "SRP-6 server (tjw)",
  srp6_server_init,
  srp6_server_finish,
  srp6_server_params,
  srp6_server_auth,
  srp6_server_passwd,
  srp6_server_genpub,
  srp6_server_key,
  srp6_server_verify,
  srp6_server_respond,
  NULL
};

_TYPE( SRP_METHOD * )
SRP6_server_method()
{
  return &srp6_server_meth;
}*/

static SRP_METHOD srp6a_server_meth = {
  "SRP-6a server (tjw)",
  srp6a_server_init,
  srp6_server_finish,
  srp6_server_params,
  srp6_server_auth,
  srp6_server_passwd,
  srp6a_server_genpub,
  srp6_server_key,
  srp6_server_verify,
  srp6_server_respond,
  NULL
};

_TYPE( SRP_METHOD * )
SRP6a_server_method()
{
  return &srp6a_server_meth;
}
