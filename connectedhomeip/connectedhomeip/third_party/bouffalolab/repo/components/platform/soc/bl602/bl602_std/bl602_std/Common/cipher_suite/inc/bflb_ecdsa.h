#ifndef _BFLB_ECDSA_H
#define _BFLB_ECDSA_H

#include "bflb_ecdsa_port.h"
#include "bflb_platform.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define BFLB_ECDSA_OK             0
#define BFLB_ECDSA_ERROR          -1

int32_t bflb_ecdsa_init(uint8_t id);

int32_t bflb_ecdsa_verify(uint8_t id,const uint32_t *hash, uint32_t hashLen,const uint32_t *pkX,const uint32_t *pkY, const uint32_t *r, const uint32_t *s);
int32_t bflb_ecdsa_deinit(uint8_t id);
int32_t bflb_ecdh_get_public_key( uint8_t id,const uint32_t *pkX,const uint32_t *pkY,const uint32_t *private_key,const uint32_t *pRx,const uint32_t *pRy);
int32_t bflb_ecc_get_random_value(uint32_t *randomData,uint32_t *maxRef,uint32_t size);
int32_t bflb_ecdsa_get_private_key( uint8_t id,uint32_t *private_key);
int32_t bflb_ecdsa_sign( uint8_t id,const uint32_t *private_key,const uint32_t *random_k,const uint32_t *hash,uint32_t *r,uint32_t *s);

#endif
