#ifndef __PHY_RFTLV_H__
#define __PHY_RFTLV_H__

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define RFTLV_DEBUG                    (0) /* 0-disable, 1-enable */

#define RFTLV_SIZE_HEAD1               (8)
#define RFTLV_SIZE_HEAD                (16)
#define RFTLV_SIZE_TYPE                (2)
#define RFTLV_SIZE_LENGTH              (2)
#define RFTLV_SIZE_TL                  (RFTLV_SIZE_TYPE + RFTLV_SIZE_LENGTH)
#define RFTLV_SIZE_PARTION             (2048)

#define RFTLF_TYPE_MAGIC_FW_HEAD1      (0x4152415046524C42)
#define RFTLF_TYPE_MAGIC_FW_HEAD2      (0x6B3162586B44364F)
#define RFTLV_TYPE_INVALID             0X0000
#define RFTLV_TYPE_XTAL_MODE           0X0001
#define RFTLV_TYPE_XTAL                0X0002
#define RFTLV_TYPE_PWR_MODE            0X0003
#define RFTLV_TYPE_PWR_TABLE           0X0004
#define RFTLV_TYPE_PWR_TABLE_11B       0X0005
#define RFTLV_TYPE_PWR_TABLE_11G       0X0006
#define RFTLV_TYPE_PWR_TABLE_11N       0X0007
#define RFTLV_TYPE_PWR_OFFSET          0X0008
#define RFTLV_TYPE_CHAN_DIV_TAB        0X0009
#define RFTLV_TYPE_CHAN_CNT_TAB        0X000A
#define RFTLV_TYPE_LO_FCAL_DIV         0X000B
#define RFTLV_TYPE_EN_TCAL             0X0020
#define RFTLV_TYPE_LINEAR_OR_FOLLOW    0X0021
#define RFTLV_TYPE_TCHANNELS           0X0022
#define RFTLV_TYPE_TCHANNEL_OS         0X0023
#define RFTLV_TYPE_TCHANNEL_OS_LOW     0X0024
#define RFTLV_TYPE_TROOM_OS            0X0025
#define RFTLV_TYPE_PWR_TABLE_BLE       0X0030

#define RFTLV_TYPE(dst,src,len)        (memcpy(dst,src,len))
#define RFTLV_LENGTH(dst,src,len)      (memcpy(dst,src,len))
#define RFTLV_VALUE(dst,src,len)       (memcpy(dst,src,len))

#if RFTLV_DEBUG
#define rftlv_print      printf
#else
#define rftlv_print(...)
#endif

/*
 * return : 1-valid, other-invalid
 */
int rftlv_valid(uint32_t tlv_addr);

/*
 * input:
 *        type: type
 *        value_len: value max len
 *        value: point value
 * return : ">0"-success, "<0"-invalid and end, "==0"-invalid and can next
 */
int rftlv_get(uint32_t tlv_addr, uint16_t type, uint32_t value_len, void *value);

#if RFTLV_DEBUG
void rftlv_test(void);
#endif

#endif
