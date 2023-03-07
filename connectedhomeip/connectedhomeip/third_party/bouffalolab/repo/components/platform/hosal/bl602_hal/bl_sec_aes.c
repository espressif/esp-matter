/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <string.h>

#include <sec_eng_reg.h>
#include <bl602_sec_eng.h>

#include "bl_irq.h"
#include "bl_sec.h"
#include "bflb_crypt.h"

#include <blog.h>
#define USER_UNUSED(a) ((void)(a))

bflb_crypt_handle_t crypt_handle;

int bl_sec_aes_enc(uint8_t *key, int keysize, uint8_t *input, uint8_t *output)
{
    return 0;
}

static void Aes_Compare_Data(const uint8_t *expected, uint8_t *input, uint32_t len)
{
    int i = 0, is_failed = 0;

    for (i = 0; i < len; i++) {
        if (input[i] != expected[i]) {
            is_failed = 1;
            blog_info("%s[%02d], %02x %02x\r\n",
                input[i] ==expected[i] ? "S" : "F",
                i,
                input[i],
                expected[i]
            );
        }
    }
    if (is_failed) {
        blog_error("====== Failed %lu Bytes======\r\n", len);
    } else {
        blog_info("====== Success %lu Bytes=====\r\n", len);
    }
}

static void _dump_iv_status(SEC_Eng_AES_Link_Config_Type *linkCfg)
{
    int i;
    uint8_t *iv;

    USER_UNUSED(iv);
    USER_UNUSED(i);

    for (i = 0, iv = (uint8_t*)&(linkCfg->aesIV0); i < 16; i++) {
        blog_print("%02x", iv[i]);
    }
    blog_print("\r\n");
}

static const uint8_t aesSrcBuf_data[] = 
{
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
};

void Sec_Eng_AES_Link_Case_CBC_128(SEC_ENG_AES_ID_Type aesId)
{
    uint8_t aesDstBuf[32];
    static const uint8_t aesResult_entrypted_cbc_128[] =
    {
        0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46, 0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
        0x4c, 0xbb, 0xc8, 0x58, 0x75, 0x6b, 0x35, 0x81, 0x25, 0x52, 0x9e, 0x96, 0x98, 0xa3, 0x8f, 0x44,
        0x9f, 0x6f, 0x07, 0x96, 0xee, 0x3e, 0x47, 0xb0, 0xd8, 0x7c, 0x76, 0x1b, 0x20, 0x52, 0x7f, 0x78,
        0x07, 0x01, 0x34, 0x08, 0x5f, 0x02, 0x75, 0x17, 0x55, 0xef, 0xca, 0x3b, 0x4c, 0xdc, 0x7d, 0x62,
    };

    
    SEC_Eng_AES_Link_Config_Type linkCfg = {
        .aesMode = SEC_ENG_AES_KEY_128BITS,                            /* 128-bit key mode select */
        .aesDecEn = SEC_ENG_AES_ENCRYPTION,                            /* Encode */
        .aesDecKeySel = SEC_ENG_AES_USE_NEW,                           /* Use new key */
        .aesIntClr = 0,                                                /* Not clear intrrupt */
        .aesIntSet = 1,                                                /* Not set interrupt */
        .aesBlockMode = SEC_ENG_AES_CBC,                               /* ECB mode select */
        .aesIVSel = SEC_ENG_AES_USE_NEW,                               /* Use new iv */
        .aesMsgLen = 1,                                                /* Number of 128-bit block */
        .aesSrcAddr = 0,                                               /* Message source address */
        .aesDstAddr = 0,                                               /* Message destination address */
        .aesIV0 = 0x03020100,                                          /* Big endian initial vector(MSB) */
        .aesIV1 = 0x07060504,                                          /* Big endian initial vector */
        .aesIV2 = 0x0b0a0908,                                          /* Big endian initial vector */
        .aesIV3 = 0x0f0e0d0c,                                          /* Big endian initial vector(LSB)(CTR mode:counter initial value) */
        .aesKey0 = 0x16157e2b,                                         /* Big endian aes key(aes-128/256 key MSB) */
        .aesKey1 = 0xa6d2ae28,                                         /* Big endian aes key */
        .aesKey2 = 0x8815f7ab,                                         /* Big endian aes key */
        .aesKey3 = 0x3c4fcf09,                                         /* Big endian aes key(aes-128 key LSB) */
        .aesKey4 = 0,                                                  /* Big endian aes key */
        .aesKey5 = 0,                                                  /* Big endian aes key */
        .aesKey6 = 0,                                                  /* Big endian aes key */
        .aesKey7 = 0                                                   /* Big endian aes key(aes-256 key LSB) */
    };

    Sec_Eng_AES_Enable_Link(aesId);
    
    puts("[CBC] AES-128-CBC case...\r\n");

    blog_info("[CBC] IV Status Initial, %08lx\r\n", linkCfg.aesSrcAddr);
    _dump_iv_status(&linkCfg);
    Sec_Eng_AES_Link_Work(aesId, (uint32_t)&linkCfg, &(aesSrcBuf_data[0]), 32, aesDstBuf);
    Aes_Compare_Data(aesResult_entrypted_cbc_128, (uint8_t*)linkCfg.aesDstAddr, 32);
    blog_info("[CBC] IV Status After, %08lx\r\n", linkCfg.aesSrcAddr);
    _dump_iv_status(&linkCfg);
    
    Sec_Eng_AES_Link_Work(aesId, (uint32_t)&linkCfg, &(aesSrcBuf_data[0]) + 32, 16, aesDstBuf);
    Aes_Compare_Data(&(aesResult_entrypted_cbc_128[0]) + 32, (uint8_t*)linkCfg.aesDstAddr, 16);
    blog_info("[CBC] IV Status After %08lx\r\n", linkCfg.aesSrcAddr);
    _dump_iv_status(&linkCfg);
    
    Sec_Eng_AES_Link_Work(aesId, (uint32_t)&linkCfg, &(aesSrcBuf_data[0]) + 48, 16, aesDstBuf);
    Aes_Compare_Data(&(aesResult_entrypted_cbc_128[0]) + 48, (uint8_t*)linkCfg.aesDstAddr, 16);
    blog_info("[CBC] IV Status After, %08lx\r\n", linkCfg.aesSrcAddr);
    _dump_iv_status(&linkCfg);

    Sec_Eng_AES_Disable_Link(aesId);
}

void Sec_Eng_AES_Link_Case_CTR_128(SEC_ENG_AES_ID_Type aesId)
{
    uint8_t aesDstBuf[32];
    static const uint8_t aesResult_entrypted_ctr_128[] =
    {
        0x87, 0x4d, 0x61, 0x91, 0xb6, 0x20, 0xe3, 0x26, 0x1b, 0xef, 0x68, 0x64, 0x99, 0x0d, 0xb6, 0xce,
        0x5d, 0xea, 0xc2, 0xde, 0x49, 0x33, 0xce, 0xf5, 0xf1, 0x9d, 0x09, 0xc6, 0x8f, 0xc3, 0x64, 0x84,
        0x01, 0xed, 0x7d, 0x9a, 0x56, 0xc9, 0xa8, 0xd9, 0x57, 0x89, 0xb6, 0x0a, 0x64, 0x29, 0x7b, 0x6e,
        0x83, 0x5d, 0x87, 0x7d, 0xde, 0xb1, 0x07, 0x50, 0x3d, 0x37, 0x4f, 0xca, 0x66, 0xff, 0xbc, 0xd4,
    };
    SEC_Eng_AES_Link_Config_Type linkCfg = {
        .aesMode = SEC_ENG_AES_KEY_128BITS,                            /* 128-bit key mode select */
        .aesDecEn = SEC_ENG_AES_ENCRYPTION,                            /* Encode */
        .aesDecKeySel = SEC_ENG_AES_USE_NEW,                           /* Use new key */
        .aesIntClr = 0,                                                /* Not clear intrrupt */
        .aesIntSet = 1,                                                /* Not set interrupt */
        .aesBlockMode = SEC_ENG_AES_CTR,                               /* CTR mode select */
        .aesIVSel = SEC_ENG_AES_USE_NEW,                               /* Use new iv */
        .aesMsgLen = 1,                                                /* Number of 128-bit block */
        .aesSrcAddr = 0,                                               /* Message source address */
        .aesDstAddr = 0,                                               /* Message destination address */
        .aesIV0 = 0xf3f2f1f0,                                          /* Big endian initial vector(MSB) */
        .aesIV1 = 0xf7f6f5f4,                                          /* Big endian initial vector */
        .aesIV2 = 0xfbfaf9f8,                                          /* Big endian initial vector */
        .aesIV3 = 0xfffefdfc,                                          /* Big endian initial vector(LSB)(CTR mode:counter initial value) */
        .aesKey0 = 0x16157e2b,                                         /* Big endian aes key(aes-128/256 key MSB) */
        .aesKey1 = 0xa6d2ae28,                                         /* Big endian aes key */
        .aesKey2 = 0x8815f7ab,                                         /* Big endian aes key */
        .aesKey3 = 0x3c4fcf09,                                         /* Big endian aes key(aes-128 key LSB) */
        .aesKey4 = 0,                                                  /* Big endian aes key */
        .aesKey5 = 0,                                                  /* Big endian aes key */
        .aesKey6 = 0,                                                  /* Big endian aes key */
        .aesKey7 = 0                                                   /* Big endian aes key(aes-256 key LSB) */
    };

    Sec_Eng_AES_Enable_Link(aesId);
    
    puts("[CTR] AES-128-CTR case...\r\n");
    blog_info("[CTR] IV Status Initial, %08lx\r\n", linkCfg.aesSrcAddr);
    _dump_iv_status(&linkCfg);

    Sec_Eng_AES_Link_Work(aesId, (uint32_t)&linkCfg, &(aesSrcBuf_data[0]), 32, aesDstBuf);
    Aes_Compare_Data(aesResult_entrypted_ctr_128, (uint8_t*)linkCfg.aesDstAddr, 32);

    blog_info("[CTR] IV Status After %08lx\r\n", linkCfg.aesSrcAddr);
    _dump_iv_status(&linkCfg);
    
    Sec_Eng_AES_Link_Work(aesId, (uint32_t)&linkCfg, &(aesSrcBuf_data[0]) + 32, 16, aesDstBuf);
    Aes_Compare_Data(&(aesResult_entrypted_ctr_128[0]) + 32, (uint8_t*)linkCfg.aesDstAddr, 16);
    blog_info("[CTR] IV Status After %08lx\r\n", linkCfg.aesSrcAddr);
    _dump_iv_status(&linkCfg);
    
    Sec_Eng_AES_Link_Work(aesId, (uint32_t)&linkCfg, &(aesSrcBuf_data[0]) + 48, 16, aesDstBuf);
    Aes_Compare_Data(&(aesResult_entrypted_ctr_128[0]) + 48, (uint8_t*)linkCfg.aesDstAddr, 16);
    blog_info("[CTR] IV Status After, %08lx\r\n", linkCfg.aesSrcAddr);
    _dump_iv_status(&linkCfg);

    Sec_Eng_AES_Disable_Link(aesId);
}

void Sec_Eng_AES_Link_Case_ECB_128(SEC_ENG_AES_ID_Type aesId)
{
    uint8_t aesDstBuf[32];
    static const uint8_t aesResult_entrypted_ecb_128[] =
    {
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
        0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97,
    };
    SEC_Eng_AES_Link_Config_Type linkCfg = {
        .aesMode = SEC_ENG_AES_KEY_128BITS,                            /* 128-bit key mode select */
        .aesDecEn = SEC_ENG_AES_ENCRYPTION,                            /* Encode */
        .aesDecKeySel = SEC_ENG_AES_USE_NEW,                           /* Use new key */
        .aesIntClr = 0,                                                /* Not clear intrrupt */
        .aesIntSet = 1,                                                /* Not set interrupt */
        .aesBlockMode = SEC_ENG_AES_ECB,                               /* CTR mode select */
        .aesIVSel = SEC_ENG_AES_USE_NEW,                               /* Use new iv */
        .aesMsgLen = 1,                                                /* Number of 128-bit block */
        .aesSrcAddr = 0,                                               /* Message source address */
        .aesDstAddr = 0,                                               /* Message destination address */
        .aesIV0 = 0xf3f2f1f0,                                          /* Big endian initial vector(MSB) */
        .aesIV1 = 0xf7f6f5f4,                                          /* Big endian initial vector */
        .aesIV2 = 0xfbfaf9f8,                                          /* Big endian initial vector */
        .aesIV3 = 0xfffefdfc,                                          /* Big endian initial vector(LSB)(CTR mode:counter initial value) */
        .aesKey0 = 0x16157e2b,                                         /* Big endian aes key(aes-128/256 key MSB) */
        .aesKey1 = 0xa6d2ae28,                                         /* Big endian aes key */
        .aesKey2 = 0x8815f7ab,                                         /* Big endian aes key */
        .aesKey3 = 0x3c4fcf09,                                         /* Big endian aes key(aes-128 key LSB) */
        .aesKey4 = 0,                                                  /* Big endian aes key */
        .aesKey5 = 0,                                                  /* Big endian aes key */
        .aesKey6 = 0,                                                  /* Big endian aes key */
        .aesKey7 = 0                                                   /* Big endian aes key(aes-256 key LSB) */
    };

    puts("[ECB] AES-128-ECB case...\r\n");

    Sec_Eng_AES_Enable_Link(aesId);

    Sec_Eng_AES_Link_Work(aesId, (uint32_t)&linkCfg, &(aesSrcBuf_data[0]), 32, aesDstBuf);
    Aes_Compare_Data(aesResult_entrypted_ecb_128, (uint8_t*)linkCfg.aesDstAddr, 32);
    
    Sec_Eng_AES_Link_Work(aesId, (uint32_t)&linkCfg, &(aesSrcBuf_data[0]) + 32, 16, aesDstBuf);
    Aes_Compare_Data(&(aesResult_entrypted_ecb_128[0]) + 32, (uint8_t*)linkCfg.aesDstAddr, 16);
    
    Sec_Eng_AES_Link_Work(aesId, (uint32_t)&linkCfg, &(aesSrcBuf_data[0]) + 48, 16, aesDstBuf);
    Aes_Compare_Data(&(aesResult_entrypted_ecb_128[0]) + 48, (uint8_t*)linkCfg.aesDstAddr, 16);

    Sec_Eng_AES_Disable_Link(aesId);
}

int bl_sec_aes_test(void)
{
    bl_irq_register(SEC_AES_IRQn, bl_sec_aes_IRQHandler);
    bl_irq_enable(SEC_AES_IRQn);
    blog_print("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\r\n");
    Sec_Eng_AES_Link_Case_CBC_128(SEC_ENG_AES_ID0);
    blog_print("####################################################################################\r\n");
    Sec_Eng_AES_Link_Case_CTR_128(SEC_ENG_AES_ID0);
    blog_print("####################################################################################\r\n");
    Sec_Eng_AES_Link_Case_ECB_128(SEC_ENG_AES_ID0);
    blog_print("------------------------------------------------------------------------------------\r\n");
    return 0;
}


static void _clear_aes_int()
{
    uint32_t AESx = SEC_ENG_BASE;
    uint32_t val;

    val = BL_RD_REG(AESx, SEC_ENG_SE_AES_0_CTRL);
    val = BL_SET_REG_BIT(val, SEC_ENG_SE_AES_0_INT_CLR_1T);
    BL_WR_REG(AESx, SEC_ENG_SE_AES_0_CTRL, val);
}

void bl_sec_aes_IRQHandler(void)
{
    blog_print("--->>> AES IRQ\r\n");
    _clear_aes_int();
}

static SEC_ENG_AES_Key_Type get_key_type(uint32_t keybits)
{
    switch(keybits)
    {
        case 24:
            return SEC_ENG_AES_KEY_192BITS;
        case 32:
            return SEC_ENG_AES_KEY_256BITS;
        default:
            return SEC_ENG_AES_KEY_128BITS;
    }
}

static int get_cbc_mac_input(size_t length,
                           const unsigned char *iv, size_t iv_len,
                           const unsigned char *add, size_t add_len,
                           const unsigned char *input, size_t tag_len,
                           unsigned char *b_input)
{
    int ret = -1;
    unsigned char i;
    unsigned char q;
    size_t len_left;
    unsigned char b[16];
    const unsigned char *src;
    unsigned int  cbc_adata_round = 0;
    unsigned int  cbc_pt_round = 0;

    if( tag_len == 2 || tag_len > 16 || tag_len % 2 != 0 )
        return(ret);

    /* Also implies q is within bounds */
    if( iv_len < 7 || iv_len > 13 )
        return(ret);

    if( add_len >= 0xFF00 )
        return(ret);

    q = 16 - 1 - (unsigned char) iv_len;

    /*
     * rfc3610_Vector22
     *
     *length = 19
     *iv_len = 13
     *add_len = 12
     *
     * First block B_0:
     * 0              Flags   -> 61 (0_1_100_001)
     *1 ... 15-L     Nonce N -> 00 5B 8C CB  CD 9A F8 3C  96 96 76 6C  FA
     *16-L ... 15    l(m)    -> 19 (00 13)
     *
     * With flags as (bits):
     * 7        Reserved        0 
     * 6        Adata           1
     * 5 .. 3   M'(t - 2) / 2   4
     * 2 .. 0   L'q - 1         1
     */
    /*
     
    */

    b[0] = 0;
    b[0] |= ( add_len > 0 ) << 6;
    b[0] |= ( ( tag_len - 2 ) / 2 ) << 3;
    b[0] |= q - 1;

    BL602_MemCpy_Fast( b + 1, iv, iv_len );

    for(i = 0, len_left = length; i < q; i++, len_left >>= 8)
        b[15 - i] = (unsigned char)(len_left & 0xFF);

    if(len_left > 0 )
        return(ret);

    BL602_MemCpy_Fast(b_input, b, 16);

    /*
     * If there is additional data, update CBC-MAC with
     * add_len, add, 0 (padding to a block boundary)
     */
    if( add_len > 0 )
    {
        size_t use_len;
        len_left = add_len;
        src = add;

        BL602_MemSet( b, 0, 16 );

        /* if add_len more than 2^16-2^8 */

        /*
        if( (add_len >= ((1 << 16) - (1 << 8))) && (add_len < (1 << 32) ) )
        {
            b[0] = 0xFF;
            b[1] = 0xFE;
        }
        else if( (add_len >=((1 << 32) ) && (add_len < (1 << 64) ) )
        {
            b[0] = 0xFF;
            b[1] = 0xFF;
        }
        else if( (add_len > 0 ) && (add_len < ((1 << 16) - (1 << 8)) ) )
        {
            b[0] = (unsigned char)( ( add_len >> 8 ) & 0xFF );
            b[1] = (unsigned char)( ( add_len      ) & 0xFF );
        }
        */

        /* first round for adata */
        cbc_adata_round = 1;

        b[0] = (unsigned char)((add_len >> 8 ) & 0xFF);
        b[1] = (unsigned char)((add_len      ) & 0xFF);

        use_len = len_left < 16 - 2 ? len_left : 16 - 2;
        BL602_MemCpy_Fast(b + 2, src, use_len );
        len_left -= use_len;
        src += use_len;

        BL602_MemCpy_Fast(b_input + cbc_adata_round * 16, b, 16 );

        while( len_left > 0 )
        {
            cbc_adata_round++;

            use_len = len_left > 16 ? 16 : len_left;
            if (use_len < 16) {
                BL602_MemSet(b_input + cbc_adata_round * 16, 0, 16);
            }

            BL602_MemCpy_Fast( b_input + cbc_adata_round * 16, src, use_len);

            len_left -= use_len;
            src += use_len;
        }
    }

    /*
     * Authenticate and {en,de}crypt the message.
     *
     * The only difference between encryption and decryption is
     * the respective order of authentication and {en,de}cryption.
     */
    len_left = length;
    src = input;

    /* Plaintext block is  followed by adata block*/
    cbc_pt_round = cbc_adata_round + 1;

    while(len_left > 0)
    {
        size_t use_len = len_left > 16 ? 16 : len_left;

        BL602_MemCpy_Fast(b_input + cbc_pt_round * 16, src, use_len);

        src += use_len;
        len_left -= use_len;

        cbc_pt_round++;
    }

    return (0);
}                           

static int get_ctr_enc_input(size_t length, const unsigned char *input, unsigned char *b_input, unsigned char *b_output, unsigned int  cbc_pt_round)
{
    size_t len_left = 0;
    const unsigned char *src;
	unsigned int  ctr_pt_round = 0;

    len_left = length;
    src = input;

    // Copy CRC plaintext last block output to here as CTR first block input
    BL602_MemCpy_Fast(b_input, b_output + (cbc_pt_round - 1) * 16, 16);

    ctr_pt_round = 1;

    while( len_left > 0 )
    {
        size_t use_len = len_left > 16 ? 16 : len_left;

        BL602_MemCpy_Fast(b_input + ctr_pt_round * 16, src, use_len);

        src += use_len;
        len_left -= use_len;

        ctr_pt_round++;
    }

    return (0);
}

static int get_ctr_dec_input(size_t length, const unsigned char *input, const unsigned char *tag, size_t tag_len, unsigned char *b_input)
{
    size_t len_left = 0;
    const unsigned char *src;
    unsigned int  ctr_ct_round = 0;

    len_left = length;
    src = input;

    // Copy tag in first block to calucate T
    BL602_MemCpy_Fast(b_input, tag, tag_len);

    ctr_ct_round = 1;

    while( len_left > 0 )
    {
        size_t use_len = len_left > 16 ? 16 : len_left;

        BL602_MemCpy_Fast(b_input + ctr_ct_round * 16, src, use_len);

        src += use_len;
        len_left -= use_len;

        ctr_ct_round++;
    }

    return (0);
}

static int get_ctr_iv(unsigned char *ctr_iv, const unsigned char *iv, size_t iv_len)
{
    unsigned char q;

    if( iv_len < 7 || iv_len > 13 )
    {
        return(-1);
    }

    q = 16 - 1 - (unsigned char) iv_len;
    ctr_iv[0] |= q - 1;
    BL602_MemCpy_Fast(ctr_iv + 1, iv, iv_len);

    return (0);
}

static unsigned int get_cbc_mac_input_size(size_t msg_len, size_t add_len)
{
    //for B0
    unsigned int block_cnt = 1;
	size_t len_left = 0;

	if(add_len > 0)
	{
	    //for B1, the most significant two bytes are l(a), so will use (16-2) bytes additional data at most.
	    block_cnt ++;
		//Check if there is remaining additional data
		if(add_len > (16-2))
		{
		    len_left = add_len - (16 - 2);
            block_cnt += len_left/16;
			if(len_left % 16)
			    block_cnt++;
			len_left = 0;
		}
	}

    block_cnt += msg_len/16;
	if(msg_len % 16)
		block_cnt++;

	return (block_cnt * 16);
}


static unsigned int get_ctr_input_output_size(size_t msg_len)
{
    unsigned int block_cnt = 1;//One Block for U/T
    
	block_cnt += msg_len/16;
	if(msg_len % 16)
		block_cnt++;

	return (block_cnt * 16);
}

int bl_sec_ccm_encrypt_and_tag(const uint8_t *key, unsigned int key_bytelen, size_t length, const unsigned char *iv, size_t iv_len, const unsigned char *add, size_t add_len,
                         const unsigned char *input, unsigned char *output, unsigned char *tag, size_t tag_len)
{
	// Use cbc, clear iv to zero for CBC-MAC
	unsigned char cbc_iv[16] = {0};
	unsigned char ctr_iv[16] = {0};
	unsigned int cbc_length = 0;
	unsigned int ctr_length = 0;
	unsigned int b_input_size= 0;
	unsigned int b_output_size = 0;
	unsigned char *b_input = NULL;
	unsigned char *b_output = NULL;
	int ret = 0;

    // TODO check input arguments
    cbc_length = get_cbc_mac_input_size(length, add_len);
	//get the max size for b_input_size between cbc and ctr, max size is the value of cbc's input size
	b_input_size = cbc_length;
	//Allocate memory for input block, one block size is 16 bytes.
	b_input = pvPortMalloc(b_input_size);
	
	if(!b_input)
	{
	    ret = -1;
	    goto exit;
	}

    //get the max size for b_output_size between cbc and ctr, max size is the value of ctr's output size
	/* b_output_size = get_ctr_input_output_size(length); */
    // reserve enough space for CBC
	b_output_size = cbc_length;
	b_output = pvPortMalloc(b_output_size);

	if(!b_output)
	{
	    ret = -1;
	    goto exit;
	}

	BL602_MemSet(b_input, 0, b_input_size);
	BL602_MemSet(b_output, 0, b_output_size);
	
	ret = get_cbc_mac_input(length, iv, iv_len, add, add_len, input, tag_len, b_input);

	if(ret)
	{
	    ret = -1;
		goto exit;
	}
   
    /**************AES-CBC*************/
	bflb_crypt_init(&crypt_handle, BFLB_CRYPT_TYPE_AES_CBC);
    bflb_crypt_setkey(&crypt_handle, key, get_key_type(key_bytelen), key_bytelen, cbc_iv, 16, BFLB_CRYPT_DIR_ENCRYPT);
    if(bflb_crypt_encrypt(&crypt_handle, b_input, cbc_length, 0, b_output) != BFLB_CRYPT_OK)
	{
		ret = -1;
		goto exit;
	}

	/**************AES-CTR*************/
	get_ctr_iv(ctr_iv, iv, iv_len);

	get_ctr_enc_input(length, input, b_input, b_output, cbc_length/16);

	ctr_length = get_ctr_input_output_size(length);
	BL602_MemSet(b_output, 0, b_output_size);
	
    bflb_crypt_init(&crypt_handle, BFLB_CRYPT_TYPE_AES_CTR);
	bflb_crypt_setkey(&crypt_handle, key, get_key_type(key_bytelen), key_bytelen, ctr_iv, 16, BFLB_CRYPT_DIR_ENCRYPT);
	if(bflb_crypt_encrypt(&crypt_handle, b_input, ctr_length, 0, b_output) != BFLB_CRYPT_OK)
	{
	    ret = -1;
		goto exit;
	}
	

	/*first output block is tag info*/
	BL602_MemCpy_Fast(tag, b_output, tag_len);

	/*second ~ ? output block is pt info*/
	BL602_MemCpy_Fast(output, b_output + 16, length);

exit:
	 if(b_input)
		 vPortFree(b_input);
	 if(b_output)
		 vPortFree(b_output);	
	 return ret;

}

 int bl_sec_ccm_auth_decrypt(const uint8_t *key, unsigned int key_bytelen, size_t length,const unsigned char *iv, size_t iv_len, const unsigned char *add,
							 size_t add_len, const unsigned char *input, unsigned char *output, const unsigned char *tag, size_t tag_len)
 {
    unsigned char cbc_mac[16] = {0};
    unsigned char cbc_iv[16] = {0};
    unsigned char ctr_iv[16] = {0};
    unsigned int cbc_length = 0;
	unsigned int ctr_length = 0;
    unsigned int b_input_size = 0;
    unsigned int b_output_size = 0;
    unsigned char *b_input  = NULL;
    unsigned char *b_output = NULL;
    int ret = 0;

    // TODO check input arguments
    //get the max size for b_input_size between cbc and ctr,max size is the value of cbc's input size
    cbc_length = get_cbc_mac_input_size(length, add_len);
    b_input_size = cbc_length;
	//Allocate memory for input block, one block size is 16 bytes.
	b_input = pvPortMalloc(b_input_size);

	if(!b_input)
	{
	    ret = -1;
		goto exit;
	}

    ctr_length = get_ctr_input_output_size(length);
    //get the max size for b_output_size between cbc and ctr, max size is the value of ctr's output size
    // reserve enough space for CBC
    b_output_size = cbc_length;
	b_output = pvPortMalloc(b_output_size);

	if(!b_output)
	{
	    ret = -1;
		goto exit;
	}

	 BL602_MemSet(b_input, 0, b_input_size);
	 
	 BL602_MemSet(b_output, 0, b_output_size);
	
	 /* ctr part*/
	 get_ctr_iv(ctr_iv, iv, iv_len);
 
	 get_ctr_dec_input(length, input, tag, tag_len, b_input);
	 
	 bflb_crypt_init(&crypt_handle, BFLB_CRYPT_TYPE_AES_CTR);
	 bflb_crypt_setkey(&crypt_handle, key, get_key_type(key_bytelen), key_bytelen, ctr_iv, 16, BFLB_CRYPT_DIR_ENCRYPT);
  	 if(bflb_crypt_encrypt(&crypt_handle, b_input, ctr_length, 0, b_output) != BFLB_CRYPT_OK)
  	 {
		 ret = -1;
		 goto exit;
	 }
	
	 /* Copy CBC-MAC here */
	 BL602_MemCpy_Fast(cbc_mac, b_output, tag_len);
 
	 /* Copy plaintext here */
	 BL602_MemCpy_Fast(output, b_output + 16, length);
 
	 /* CBC-MAC part*/
 
	 ret = get_cbc_mac_input(length, iv, iv_len, add, add_len, output, tag_len, b_input);
 
	 if(ret)
	 {
		 ret = -1;
		 goto exit;
	 }
	
	 BL602_MemSet(b_output, 0, b_output_size);

	 /**************AES-CBC*************/
	 bflb_crypt_init(&crypt_handle, BFLB_CRYPT_TYPE_AES_CBC);
	 bflb_crypt_setkey(&crypt_handle, key, get_key_type(key_bytelen), key_bytelen, cbc_iv, 16, BFLB_CRYPT_DIR_ENCRYPT);
	 if(bflb_crypt_encrypt(&crypt_handle, b_input, cbc_length, 0, b_output) != BFLB_CRYPT_OK)
	 {
		 ret = -1;
		 goto exit;
	 }

	 /* Check CBC-MAC */
	 if (BL602_MemCmp(cbc_mac, b_output + (cbc_length/16 - 1) * 16, tag_len) != 0)
	 {
		 ret = -1;
		 goto exit;
	 }

exit:
	 if(b_input)
	     vPortFree(b_input);
	 if(b_output)
	     vPortFree(b_output);	
     return ret;
 }

int bl_sec_aes_ecb_encrypt(const uint8_t *key, unsigned int key_bytelen, size_t length, const unsigned char *input, unsigned char *output)
{
     uint8_t decrypt_iv[16]={0};
     bflb_crypt_init(&crypt_handle, BFLB_CRYPT_TYPE_AES_ECB);
     bflb_crypt_setkey(&crypt_handle, key, get_key_type(key_bytelen), key_bytelen, decrypt_iv, 16, BFLB_CRYPT_DIR_ENCRYPT);
    
     if(bflb_crypt_encrypt(&crypt_handle, input, length, 0, output) != BFLB_CRYPT_OK)
         return -1;
     return 0;
}

int bl_sec_aes_ecb_decrypt(const uint8_t *key, unsigned int key_bytelen, size_t length, const unsigned char *input, unsigned char *output)
{
     uint8_t decrypt_iv[16]={0};
     bflb_crypt_init(&crypt_handle, BFLB_CRYPT_TYPE_AES_ECB);
     bflb_crypt_setkey(&crypt_handle, key, get_key_type(key_bytelen), key_bytelen, decrypt_iv, 16, BFLB_CRYPT_DIR_DECRYPT);
    
     if(bflb_crypt_encrypt(&crypt_handle, input, length, 0, output) != BFLB_CRYPT_OK)
         return -1;
     return 0;
}

