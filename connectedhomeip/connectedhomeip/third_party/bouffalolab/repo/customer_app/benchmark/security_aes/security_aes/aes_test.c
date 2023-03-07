#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <vfs.h>
#include <aos/kernel.h>
#include <aos/yloop.h>
#include <event_device.h>
#include <cli.h>

#include <bl_sys_time.h>
#include <fdt.h>
#include <libfdt.h>
#include <mbedtls/aes.h>
#include "aes_test.h"
#include <mbedtls/gcm.h>
#include <bl_sec.h>
#include <blog.h>
#include <bl_timer.h>
#include <bl602_sec_eng.h>
#include "gmac_test.h"

#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                            \
{                                                       \
    (n) = ( (uint32_t) (b)[(i)    ] << 24 )             \
        | ( (uint32_t) (b)[(i) + 1] << 16 )             \
        | ( (uint32_t) (b)[(i) + 2] <<  8 )             \
        | ( (uint32_t) (b)[(i) + 3]       );            \
}
#endif

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n,b,i)                            \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n) >> 24 );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 3] = (unsigned char) ( (n)       );       \
}
#endif

static int AES_CRYPT(int mode, const unsigned char *input,
                     uint8_t *output, uint8_t *iv, uint8_t *key,
                     SEC_ENG_AES_Type AES_Type, int key_type, size_t length)
{
    uint8_t *input_temp;
    SEC_ENG_AES_Key_Type Key_Type;
    SEC_ENG_AES_EnDec_Type aesEnDec;
    SEC_ENG_AES_ID_Type aesId = SEC_ENG_AES_ID0;

    SEC_Eng_AES_Ctx aes_ctx;
    memset(&aes_ctx, 0, sizeof(SEC_Eng_AES_Ctx));

    if(((uint32_t)output & 0x00000003) != 0) {
        log_error("output is not 4 Byte alignment! \r\n");
        while (1);
    }

    input_temp = (uint8_t *)input;

    if (mode == AES_ENCRYPT) {
        aesEnDec = SEC_ENG_AES_ENCRYPTION;
    } else if (mode == AES_DECRYPT) {
        aesEnDec = SEC_ENG_AES_DECRYPTION;
    } else {
        log_error("error aes endec mode! \r\n");
        while (1);
    }

    switch (key_type)
    {
        case 10:
            Key_Type = SEC_ENG_AES_KEY_128BITS;
            break;
        case 12:
            Key_Type = SEC_ENG_AES_KEY_192BITS;
            break;
        case 14:
            Key_Type = SEC_ENG_AES_KEY_256BITS;
            break;
        case 16:
            Key_Type = SEC_ENG_AES_DOUBLE_KEY_128BITS;
            break;
        default:
            log_error("error aes cbc key_type! \r\n");
            while(1);
            return 0;
    }

    Sec_Eng_AES_Init(&aes_ctx, aesId,AES_Type,Key_Type,aesEnDec);
    Sec_Eng_AES_Set_Key_IV_BE(aesId,SEC_ENG_AES_KEY_SW,key,iv);

    Sec_Eng_AES_Crypt(&aes_ctx, aesId, input_temp, length, output);
    Sec_Eng_AES_Finish(aesId);

    return 0;
}

static int AES_CBC_CRYPT(int mode, const unsigned char *input, uint8_t *output, uint8_t *iv, uint8_t *key, int key_type, size_t length)
{
    return AES_CRYPT(mode, input, output, iv, key, SEC_ENG_AES_CBC, key_type, length);
}

static int AES_CTR_CRYPT(int mode, const unsigned char *input, uint8_t *output, uint8_t *iv, uint8_t *key, int key_type, size_t length)
{
    return AES_CRYPT(mode, input, output, iv, key, SEC_ENG_AES_CTR, key_type, length);
}

static int AES_ECB_CRYPT(int mode, const unsigned char *input, uint8_t *output, uint8_t *key, int key_type, size_t length)
{
    uint8_t iv[16] = {0};
    return AES_CRYPT(mode, input, output, iv, key, SEC_ENG_AES_ECB, key_type, length);
}

static int bl_aes_test(uint8_t *input, uint8_t *output, uint32_t length,
                                uint32_t count, int key_type, AES_Type aes_type)
{
    int i;
    uint32_t time_irq_start, speed_time;
    uint8_t iv[16]  = {0x38, 0x49, 0x32, 0x72, 0x23, 0xa4, 0x8c, 0x3f, 0x1d, 0x49, 0x27, 0x34, 0xb3, 0xd7, 0x78, 0x32};
    uint8_t key[32] = {0x2b, 0x45, 0xe4, 0x14, 0xf5, 0x62, 0x7e, 0x89, 0x32, 0x38, 0x72, 0x59, 0x29, 0x67, 0x21, 0x34,
                       0x38, 0x49, 0x32, 0x72, 0x23, 0xa4, 0x8c, 0x3f, 0x1d, 0x49, 0x27, 0x34, 0xb3, 0xd7, 0x78, 0x32};

    if ((!input) || (!output)) {
        log_error("Can not allocate sha test_buf memory!\r\n");
        return 0;
    }

    if (length % 16 != 0) {
        log_error("length is inviald!\r\n");
        return 0;
    }

    time_irq_start = bl_timer_now_us();
    taskENTER_CRITICAL();

    for (i = 0; i < count; i++) {
        switch (aes_type) {
            case AES_ECB_MODE:
                AES_ECB_CRYPT(AES_ENCRYPT, input, output, key, key_type, length);
                break;
            case AES_CBC_MODE:
                AES_CBC_CRYPT(AES_ENCRYPT, input, output, iv, key, key_type, length);
                break;
            case AES_CTR_MODE:
                AES_CTR_CRYPT(AES_ENCRYPT, input, output, iv, key, key_type, length);
                break;
            default:
                log_error("NO Support AES crypt mode!\r\n");
                return 0;
        }
    }

    taskEXIT_CRITICAL();
    speed_time = bl_timer_now_us() - time_irq_start;
    printf("speed_time is %ldus\r\n", speed_time);
    printf("aes encrypt speed is %.4lfMbps\r\n", length * count * 8.0 / speed_time);

    return 0;
}

static int __mbedtls_aes_crypt (mbedtls_aes_context *ctx,
                                int mode,
                                AES_Type aes_type,
                                const uint8_t *key,
                                uint32_t key_type,
                                uint8_t *vi,
                                uint8_t *input,
                                uint8_t *output,
                                uint32_t length)
{
    int j;

    size_t offset = 0;
    unsigned char nonce_counter[16];
    unsigned char stream_block[16];

    if (mode == MBEDTLS_AES_ENCRYPT) {
        mbedtls_aes_setkey_enc(ctx, key, key_type);
    } else {
        mbedtls_aes_setkey_dec(ctx, key, key_type);
    }

    switch (aes_type) {
    case AES_ECB_MODE:
        for(j = 0; j < length; j += 16) {
            mbedtls_aes_crypt_ecb( ctx, mode, input + j, output + j);
        }
        break;
    case AES_CBC_MODE:
        mbedtls_aes_crypt_cbc( ctx, mode, length, vi, input, output);
        break;
    case AES_CTR_MODE:
        mbedtls_aes_crypt_ctr(ctx, length, &offset, nonce_counter,
                              stream_block, input, output);
        break;
    default:
        return 0;
    }
    return 0;
}

static void __mbedtls_aes_test(uint8_t *input, uint8_t *output,
                               uint32_t length, uint32_t count,
                               int key_type, AES_Type aes_type)
{
    int i;
    uint32_t time_irq_start, speed_time;
    static uint8_t iv[16]  = {0x38, 0x49, 0x32, 0x72, 0x23, 0xa4, 0x8c, 0x3f, 0x1d, 0x49, 0x27, 0x34, 0xb3, 0xd7, 0x78, 0x32};
    static const uint8_t key[32] = {0x2b, 0x45, 0xe4, 0x14, 0xf5, 0x62, 0x7e, 0x89, 0x32, 0x38, 0x72, 0x59, 0x29, 0x67, 0x21, 0x34,
                                    0x38, 0x49, 0x32, 0x72, 0x23, 0xa4, 0x8c, 0x3f, 0x1d, 0x49, 0x27, 0x34, 0xb3, 0xd7, 0x78, 0x32};

    mbedtls_aes_context ctx;

    mbedtls_aes_init( &ctx );

    switch (key_type)
    {
        case 10:
            key_type = 128;
            break;
        case 12:
            key_type = 192;
            break;
        case 14:
            key_type = 256;
            break;
        default:
            printf("error aes key_type! \r\n");
            return;
    }
    time_irq_start = bl_timer_now_us();
    taskENTER_CRITICAL();

    for (i = 0; i < count; i++) {
        __mbedtls_aes_crypt(&ctx,
                            MBEDTLS_AES_ENCRYPT,
                            aes_type,
                            key,
                            key_type,
                            iv,
                            input,
                            output,
                            length);
    }
    taskEXIT_CRITICAL();
    speed_time = bl_timer_now_us() - time_irq_start;
    printf("speed_time is %ldus\r\n", speed_time);
    printf("aes encrypt speed is %.4lfMbps\r\n", length * count * 8.0 / speed_time);

    mbedtls_aes_free(&ctx);
}

static int gcm_setkey_hw( aes_gcm_hw_t *ctx,
                   const unsigned char *key,
                   unsigned int keybits )
{
    switch (keybits) {
        case 128:
            keybits = 10;
            break;
        case 192:
            keybits = 12;
            break;
        case 256:
            keybits = 14;
            break;
        default:
            printf("invalid keytype parametre!\r\n");
            return -1;
    }
    ctx->p_key = (uint8_t *)key;
    ctx->key_bitlen = keybits;
    return( 0 );
}

static int gcm_starts_hw( aes_gcm_hw_t *ctx,
                int mode,
                const unsigned char *iv,
                size_t iv_len,
                const unsigned char *add,
                size_t add_len )
{
    unsigned char work_buf[16] = {0};
    size_t i;
    const unsigned char *p;
    size_t use_len = 0;

    /* IV and AD are limited to 2^64 bits, so 2^61 bytes */
    if( ( (uint64_t) iv_len  ) >> 61 != 0 ||
        ( (uint64_t) add_len ) >> 61 != 0 )
    {
        return( -1 );
    }

    memset( ctx->y, 0x00, sizeof(ctx->y) );
    memset( ctx->buf, 0x00, sizeof(ctx->buf) );

    ctx->mode = mode;
    ctx->len = 0;
    ctx->add_len = 0;

    if( iv_len == 12 )
    {
        memcpy( ctx->y, iv, iv_len );
        ctx->y[15] = 1;
    }
    else
    {
        PUT_UINT32_BE( iv_len * 8, work_buf, 12 );

        p = iv;
        while ( iv_len > 0 ) {
            use_len = ( iv_len < 16 ) ? iv_len : 16;

            for( i = 0; i < use_len; i++ ) {
                ctx->y[i] ^= p[i];
            }

            iv_len -= use_len;
            p += use_len;
        }

        for( i = 0; i < 16; i++ )
            ctx->y[i] ^= work_buf[i];
    }
    AES_ECB_CRYPT(MBEDTLS_GCM_DECRYPT, ctx->y, ctx->base_ectr,
                  ctx->p_key, ctx->key_bitlen, 16);
    gmac_mult(ctx->base_ectr, ctx->buf, 16);

    ctx->add_len = add_len;
    p = add;
    while( add_len > 0 )
    {
        use_len = ( add_len < 16 ) ? add_len : 16;

        for( i = 0; i < use_len; i++ )
            ctx->buf[i] ^= p[i];

        gmac_mult(ctx->buf, ctx->buf, 16);

        add_len -= use_len;
        p += use_len;
    }

    return( 0 );
}

static int gcm_update_hw( aes_gcm_hw_t *ctx,
                size_t length,
                const unsigned char *input,
                unsigned char *output )
{
    size_t i;
    uint8_t first_block[16];
    unsigned char *p;
    unsigned char *out_p = output;

    if( output > input && (size_t) ( output - input ) < length ) {
        return( -1 );
    }
    ctx->len = length;

    /* Total length is restricted to 2^39 - 256 bits, ie 2^36 - 2^5 bytes
     * Also check for possible overflow */
    if( ctx->len + length < ctx->len ||
        (uint64_t) ctx->len + length > 0xFFFFFFFE0ull )
    {
        return( -1 );
    }

    p = pvPortMalloc(length);
    if (p == NULL) {
        return -1;
    }
    memcpy(p, input, length);

    AES_CTR_CRYPT(ctx->mode,
                  input,
                  out_p,
                  ctx->y,
                  ctx->p_key,
                  ctx->key_bitlen,
                  length);

    if( ctx->mode == MBEDTLS_GCM_DECRYPT ) {

        for (i = 0; i < 16; i++) {
            p[i] ^= ctx->buf[i];
        }
        gmac_mult(p, ctx->buf, length);

    } else if( ctx->mode == MBEDTLS_GCM_ENCRYPT ) {

        memcpy(first_block, out_p, 16);
        for (i = 0; i < 16; i++) {
            out_p[i] ^= ctx->buf[i];
        }
        gmac_mult(out_p, ctx->buf, length);
        memcpy(out_p, first_block, 16);

    } else {
        return -1;
    }
    vPortFree(p);

    return( 0 );
}

static int gcm_finish_hw( aes_gcm_hw_t *ctx,
                unsigned char *tag,
                size_t tag_len )
{
    unsigned char work_buf[16] = {0};
    size_t i;
    uint64_t orig_len = ctx->len * 8;
    uint64_t orig_add_len = ctx->add_len * 8;

    if( tag_len > 16 || tag_len < 4 ) {
        return( -1 );
    }

    memcpy( tag, ctx->base_ectr, tag_len );

    if( orig_len || orig_add_len ) {

        PUT_UINT32_BE( ( orig_add_len >> 32 ), work_buf, 0  );
        PUT_UINT32_BE( ( orig_add_len       ), work_buf, 4  );
        PUT_UINT32_BE( ( orig_len     >> 32 ), work_buf, 8  );
        PUT_UINT32_BE( ( orig_len           ), work_buf, 12 );

        for ( i = 0; i < 16; i++ ) {
            ctx->buf[i] ^= work_buf[i];
        }

        gmac_mult(ctx->buf, ctx->buf, 16);

        for( i = 0; i < tag_len; i++ ) {
            tag[i] ^= ctx->buf[i];
        }
    }

    return( 0 );
}

static int gcm_crypt_and_tag_hw( aes_gcm_hw_t *ctx,
                       int mode,
                       size_t length,
                       const unsigned char *iv,
                       size_t iv_len,
                       const unsigned char *add,
                       size_t add_len,
                       const unsigned char *input,
                       unsigned char *output,
                       size_t tag_len,
                       unsigned char *tag )
{
    int ret;

    if( ( ret = gcm_starts_hw( ctx, mode, iv, iv_len, add, add_len ) ) != 0 )
        return( ret );

    if( ( ret = gcm_update_hw( ctx, length, input, output ) ) != 0 )
        return( ret );

    if( ( ret = gcm_finish_hw( ctx, tag, tag_len ) ) != 0 )
        return( ret );

    return( 0 );
}

//#define TEST_GCM_DECRYPT

static void __mbedtls_gcm_test (uint8_t *input, uint8_t *output,
                                uint32_t length, uint32_t count,
                                int key_len, uint8_t is_soft)
{
    uint32_t time_irq_start, speed_time;
    mbedtls_gcm_context ctx;
    aes_gcm_hw_t ctx_hw;
#ifdef TEST_GCM_DECRYPT
    uint8_t *test_buf2;
#endif
    unsigned char tag_buf[16];
    int i;
    static const uint8_t iv[16]  = {0x38, 0x49, 0x32, 0x72, 0x23, 0xa4, 0x8c, 0x3f, 0x1d, 0x49, 0x27, 0x34, 0xb3, 0xd7, 0x78, 0x32};
    static const unsigned char key[32] =
    {
        0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
        0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08,
        0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,
        0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08,
    };
    static const unsigned char additional[20] =
    {
        0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
        0xfe, 0xed, 0xfa, 0xce, 0xde, 0xad, 0xbe, 0xef,
        0xab, 0xad, 0xda, 0xd2,
    };
#ifdef TEST_GCM_DECRYPT
    test_buf2 = pvPortMalloc(length);
#endif

    if (is_soft) {

        mbedtls_gcm_init(&ctx);
        mbedtls_gcm_setkey( &ctx, MBEDTLS_CIPHER_ID_AES, key, key_len );
        printf("GCM SOFT\r\n");

    } else {

        gcm_setkey_hw(&ctx_hw, key, key_len);
        printf("GCM HW\r\n");

    }

    time_irq_start = bl_timer_now_us();
    taskENTER_CRITICAL();

    if (is_soft) {

        for (i = 0; i < count; i++) {
            mbedtls_gcm_crypt_and_tag(&ctx, MBEDTLS_GCM_ENCRYPT,
                                      length,
                                      iv, sizeof(iv),
                                      additional, sizeof(additional),
    #ifdef TEST_GCM_DECRYPT
                                      input, test_buf2, 16, tag_buf );

            mbedtls_gcm_crypt_and_tag(&ctx, MBEDTLS_GCM_DECRYPT,
                                      length,
                                      iv, sizeof(iv),
                                      additional, sizeof(additional),
                                      test_buf2, test_buf2, 16, tag_buf );
            if (memcmp(input, test_buf2, length) != 0) {
                printf("error\r\n");
            }
    #else
                                       input, output, 16, tag_buf );
    #endif
        }

    } else {

        for (i = 0; i < count; i++) {
            gcm_crypt_and_tag_hw(&ctx_hw, MBEDTLS_GCM_ENCRYPT,
                                      length,
                                      iv, sizeof(iv),
                                      additional, sizeof(additional),
    #ifdef TEST_GCM_DECRYPT
                                      input, test_buf2, 16, tag_buf );

            gcm_crypt_and_tag_hw(&ctx_hw, MBEDTLS_GCM_DECRYPT,
                                      length,
                                      iv, sizeof(iv),
                                      additional, sizeof(additional),
                                      test_buf2, test_buf2, 16, tag_buf );
            if (memcmp(input, test_buf2, length) != 0) {
                printf("error\r\n");
            }
    #else
                                       input, output, 16, tag_buf );
    #endif
        }
    }

    taskEXIT_CRITICAL();
    speed_time = bl_timer_now_us() - time_irq_start;
    printf("speed_time is %ldus\r\n", speed_time);
    printf("aes encrypt speed is %.4lfMbps\r\n", length * count * 8.0 / speed_time);

#ifdef TEST_GCM_DECRYPT
    vPortFree(test_buf2);
#endif

    if (is_soft) {
        mbedtls_gcm_free(&ctx);
    }
}

void aes_ecb_test(uint32_t length, uint32_t count,
                  aes_key_t keytype, uint8_t is_soft, ram_type_t ram_type)
{
    uint8_t *ptr = NULL;
    int i;

    ptr = malloc_hw(length, ram_type);
    if (ptr == NULL) {
        log_error("malloc failed\r\n");
        return;
    }
    for (i = 0; i < length; i++) {
        ptr[i] = i & 0xff;
    }

    switch (keytype) {
        case AES_TEST_KEY_128:
            keytype = 10;
            printf("AES_KEY_128BITS\r\n");
            break;
        case AES_TEST_KEY_192:
            keytype = 12;
            printf("AES_KEY_192BITS\r\n");
            break;
        case AES_TEST_KEY_256:
            keytype = 14;
            printf("AES_KEY_256BITS\r\n");
            break;
        case AES_TEST_DOUBLE_KEY_128:
            keytype = 16;
            printf("AES_DOUBLE_KEY_128BITS\r\n");
            break;
        default:
            printf("invalid keytype parametre!\r\n");
            return;
    }
    if (is_soft) {
        printf("ECB SOFT\r\n");
        __mbedtls_aes_test(ptr, ptr, length, count, keytype, AES_ECB_MODE);
    } else {
        printf("ECB HW\r\n");
        bl_aes_test(ptr, ptr, length, count, keytype, AES_ECB_MODE);
    }
    free_hw(ptr);
}

void aes_cbc_test(uint32_t length, uint32_t count,
                  aes_key_t keytype, uint8_t is_soft, ram_type_t ram_type)
{
    uint8_t *ptr = NULL;
    int i;

    ptr = malloc_hw(length, ram_type);
    if (ptr == NULL) {
        printf("malloc failed\r\n");
        return;
    }
    for (i = 0; i < length; i++) {
        ptr[i] = i & 0xff;
    }

    switch (keytype) {
        case AES_TEST_KEY_128:
            keytype = 10;
            printf("AES_KEY_128BITS\r\n");
            break;
        case AES_TEST_KEY_192:
            keytype = 12;
            printf("AES_KEY_192BITS\r\n");
            break;
        case AES_TEST_KEY_256:
            keytype = 14;
            printf("AES_KEY_256BITS\r\n");
            break;
        case AES_TEST_DOUBLE_KEY_128:
            keytype = 16;
            printf("AES_DOUBLE_KEY_128BITS\r\n");
            break;
        default:
            printf("invalid keytype parametre!\r\n");
            return;
    }
    if (is_soft) {
        printf("CBC SOFT\r\n");
        __mbedtls_aes_test(ptr, ptr, length, count, keytype, AES_CBC_MODE);
    } else {
        printf("CBC HW\r\n");
        bl_aes_test(ptr, ptr, length, count, keytype, AES_CBC_MODE);
    }
    free_hw(ptr);
}

void aes_ctr_test(uint32_t length, uint32_t count,
                  aes_key_t keytype, uint8_t is_soft, ram_type_t ram_type)
{
    uint8_t *ptr = NULL;
    int i;

    ptr = malloc_hw(length, ram_type);
    if (ptr == NULL) {
        printf("malloc failed\r\n");
        return;
    }
    for (i = 0; i < length; i++) {
        ptr[i] = i & 0xff;
    }

    switch (keytype) {
        case AES_TEST_KEY_128:
            keytype = 10;
            printf("AES_KEY_128BITS\r\n");
            break;
        case AES_TEST_KEY_192:
            keytype = 12;
            printf("AES_KEY_192BITS\r\n");
            break;
        case AES_TEST_KEY_256:
            keytype = 14;
            printf("AES_KEY_256BITS\r\n");
            break;
        case AES_TEST_DOUBLE_KEY_128:
            keytype = 16;
            printf("AES_DOUBLE_KEY_128BITS\r\n");
            break;
        default:
            printf("invalid keytype parametre!\r\n");
            return;
    }
    if (is_soft) {
        printf("CTR SOFT\r\n");
        __mbedtls_aes_test(ptr, ptr, length, count, keytype, AES_CTR_MODE);
    } else {
        printf("CTR HW\r\n");
        bl_aes_test(ptr, ptr, length, count, keytype, AES_CTR_MODE);
    }
    
    free_hw(ptr);
}

void aes_gcm_test(uint32_t length, uint32_t count,
                  aes_key_t keytype, uint8_t is_soft, ram_type_t type)
{
    uint8_t *ptr = NULL;
    uint32_t key_bitlen, i;

    ptr = malloc_hw(length, type);
    if (ptr == NULL) {
        printf("malloc failed\r\n");
        return;
    }
    for (i = 0; i < length; i++) {
        ptr[i] = i & 0xff;
    }

    switch (keytype) {
        case AES_TEST_KEY_128:
            key_bitlen = 128;
            printf("AES_KEY_128BITS\r\n");
            break;
        case AES_TEST_KEY_192:
            key_bitlen = 192;
            printf("AES_KEY_192BITS\r\n");
            break;
        case AES_TEST_KEY_256:
            key_bitlen = 256;
            printf("AES_KEY_256BITS\r\n");
            break;
        default:
            printf("invalid keytype parametre!\r\n");
            return;
    }
    __mbedtls_gcm_test(ptr, ptr, length, count, key_bitlen, is_soft);
    free_hw(ptr);
}
