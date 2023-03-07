/*
 * Copyright 2016-2021, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/*
 *  The SHA-256 Secure Hash Standard was published by NIST in 2002.
 *
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#include <wiced_rtos.h>
#include "mbedtls/error.h"
#include "mbedtls/sha256.h"

#if defined(MBEDTLS_SHA256_ALT)

#define SHA256_VALIDATE_RET(cond) MBEDTLS_INTERNAL_VALIDATE_RET(cond, MBEDTLS_ERR_SHA256_BAD_INPUT_DATA)
#define SHA256_VALIDATE(cond) MBEDTLS_INTERNAL_VALIDATE(cond)

static void print_mem_alloc_fail(void)
{
    printf("SHA256_alt: alloc fail\n");
}

static sha_in_node_t *create_fill_node(const unsigned char *input, size_t ilen)
{
    sha_in_node_t *p_node;
    uint8_t *      p_data;

    if ((p_node = calloc(1, sizeof(sha_in_node_t))) == NULL)
        return NULL;
    if ((p_data = calloc(1, ilen)) == NULL)
    {
        free(p_node);
        return NULL;
    }

    memcpy(p_data, input, ilen);
    p_node->sha_in = p_data;
    p_node->len    = ilen;
    p_node->next   = NULL;

    return p_node;
}

static void free_all_node(slist_node_t *p_list)
{
    sha_in_node_t *p_node;

    while ((p_node = (sha_in_node_t *)slist_get(p_list)) != NULL)
    {
        free(p_node->sha_in);
        free(p_node);
    }
    INIT_SLIST_NODE(p_list);
}

void mbedtls_sha256_init(mbedtls_sha256_context *ctx)
{
    SHA256_VALIDATE(ctx != NULL);

    memset(ctx, 0, sizeof(mbedtls_sha256_context));
    INIT_SLIST_NODE(&ctx->sha_in_list);
}

void mbedtls_sha256_free(mbedtls_sha256_context *ctx)
{
    if (ctx == NULL)
        return;

    free_all_node(&ctx->sha_in_list);
    mbedtls_platform_zeroize(ctx, sizeof(mbedtls_sha256_context));
}

void mbedtls_sha256_clone(mbedtls_sha256_context *dst, const mbedtls_sha256_context *src)
{
    sha_in_node_t *p_dst_node, *p_src_first_node, *p_src_node;
    uint8_t *      p_dst_data, *p_temp;

    SHA256_VALIDATE(dst != NULL);
    SHA256_VALIDATE(src != NULL);

    if (src->sha_in_len != 0)
    {
        /* allocate dst node and sha input */
        if ((p_dst_node = calloc(1, sizeof(sha_in_node_t))) == NULL)
            goto error;
        if ((p_dst_data = calloc(1, src->sha_in_len)) == NULL)
        {
            free(p_dst_node);
            goto error;
        }

        /* copy data from source to destination */
        p_src_first_node = (sha_in_node_t *)slist_front((slist_node_t *)&src->sha_in_list);
        p_src_node       = p_src_first_node;
        p_temp           = p_dst_data;
        do
        {
            memcpy(p_temp, p_src_node->sha_in, p_src_node->len);
            p_temp += p_src_node->len;
            p_src_node = (sha_in_node_t *)p_src_node->next;
        } while (p_src_node != p_src_first_node);

        /* add the new allocated and copied node to destination list */
        p_dst_node->sha_in = p_dst_data;
        p_dst_node->len    = src->sha_in_len;
        slist_add_tail((slist_node_t *)p_dst_node, &dst->sha_in_list);
    }

    dst->sha_in_len = src->sha_in_len;
    dst->sha2_mode  = src->sha2_mode;

    return;

error:
    print_mem_alloc_fail();
}

/*
 * SHA-256 context setup
 */
int mbedtls_sha256_starts_ret(mbedtls_sha256_context *ctx, int is224)
{
    SHA256_VALIDATE_RET(ctx != NULL);
    SHA256_VALIDATE_RET(is224 == 0 || is224 == 1);

    if (is224 == 1)
        ctx->sha2_mode = HW_SHA224;
    else
        ctx->sha2_mode = HW_SHA256;

    free_all_node(&ctx->sha_in_list);
    ctx->sha_in_len = 0;

    return (0);
}

void mbedtls_sha256_starts(mbedtls_sha256_context *ctx, int is224)
{
    mbedtls_sha256_starts_ret(ctx, is224);
}

int mbedtls_internal_sha256_process(mbedtls_sha256_context *ctx, const unsigned char data[64])
{
    SHA256_VALIDATE_RET(ctx != NULL);
    SHA256_VALIDATE_RET((const unsigned char *)data != NULL);
    return 0;
}

void mbedtls_sha256_process(mbedtls_sha256_context *ctx, const unsigned char data[64])
{
    mbedtls_internal_sha256_process(ctx, data);
}

int mbedtls_sha256_update_ret(mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen)
{
    sha_in_node_t *p_node;

    SHA256_VALIDATE_RET(ctx != NULL);
    SHA256_VALIDATE_RET(ilen == 0 || input != NULL);

    if (ilen == 0)
        return (0);

    if ((p_node = create_fill_node(input, ilen)) == NULL)
    {
        print_mem_alloc_fail();
        return MBEDTLS_ERR_SHA256_HW_ACCEL_FAILED;
    }

    slist_add_tail((slist_node_t *)p_node, &ctx->sha_in_list);
    ctx->sha_in_len += ilen;

    return (0);
}

void mbedtls_sha256_update(mbedtls_sha256_context *ctx, const unsigned char *input, size_t ilen)
{
    mbedtls_sha256_update_ret(ctx, input, ilen);
}

/*
 * SHA-256 final digest
 */
int mbedtls_sha256_finish_ret(mbedtls_sha256_context *ctx, unsigned char output[32])
{
    uint8_t        sha_ret;
    tHW_SHA2       hw_sha256;
    uint8_t *      p_sha_in, *p_temp;
    sha_in_node_t *p_first_node, *p_node;

    SHA256_VALIDATE_RET(ctx != NULL);
    SHA256_VALIDATE_RET((unsigned char *)output != NULL);
    SHA256_VALIDATE_RET(!slist_empty(&ctx->sha_in_list));
    SHA256_VALIDATE_RET(ctx->sha_in_len != 0);

    if ((p_sha_in = calloc(1, ctx->sha_in_len)) == NULL)
    {
        print_mem_alloc_fail();
        return (MBEDTLS_ERR_SHA256_HW_ACCEL_FAILED);
    }

    p_first_node = (sha_in_node_t *)slist_front(&ctx->sha_in_list);
    p_node       = p_first_node;
    p_temp       = p_sha_in;
    do
    {
        memcpy(p_temp, p_node->sha_in, p_node->len);
        p_temp += p_node->len;
        p_node = (sha_in_node_t *)p_node->next;
    } while (p_node != p_first_node);

    hw_sha256.polling_flag = HW_SECENG_POLLING;
    hw_sha256.hmac_en      = 0;
    hw_sha256.sha2_mode    = ctx->sha2_mode;
    hw_sha256.msg_len      = ctx->sha_in_len;
    hw_sha256.key_len      = 0;
    hw_sha256.key_ptr      = NULL;
    hw_sha256.in_ptr       = (uint32_t *)p_sha_in;
    hw_sha256.out_ptr      = output;
    hw_sha256.callback     = NULL;

    sha_ret = hw_sha2_engine(&hw_sha256);

    /* free memory after finish and reset context  */
    free(p_sha_in);
    free_all_node(&ctx->sha_in_list);
    ctx->sha_in_len = 0;

    if (sha_ret != HW_SHA2_COMPLETE)
    {
        printf("SHA256_alt: HW fail %d\n", sha_ret);
        return MBEDTLS_ERR_SHA256_HW_ACCEL_FAILED;
    }

    return (0);
}

void mbedtls_sha256_finish(mbedtls_sha256_context *ctx, unsigned char output[32])
{
    mbedtls_sha256_finish_ret(ctx, output);
}

#endif /* MBEDTLS_SHA256_ALT */
