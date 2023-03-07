#ifndef __SOFTCRC_H__
#define __SOFTCRC_H__

#include "stdint.h"

uint16_t BFLB_Soft_CRC16 (void *dataIn, uint32_t len);
uint32_t BFLB_Soft_CRC32 (void *dataIn, uint32_t len);

#endif
