#include "sha_test.h"

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
#include <mbedtls/sha1.h>
#include <mbedtls/sha256.h>
#include <bl_sec.h>
#include <blog.h>
#include <bl_timer.h>
#include <bl602_sec_eng.h>

static int SHA_CHECK(uint8_t *input, uint8_t *output, SEC_ENG_SHA_Type shaType, uint32_t length)
{
    uint32_t shaTmpBuf[16] = {0};
    uint32_t padding[16] = {0};
    SEC_Eng_SHA256_Ctx shaCtx;
    SEC_ENG_SHA_ID_Type shaID = SEC_ENG_SHA_ID0;

    Sec_Eng_SHA256_Init(&shaCtx, shaID, shaType, shaTmpBuf, padding);
    Sec_Eng_SHA_Start(shaID);

    Sec_Eng_SHA256_Update(&shaCtx, shaID, input,length);
    if (Sec_Eng_SHA256_Finish(&shaCtx, shaID, output) != SUCCESS) {
        printf("Sec_Eng_SHA256_Finish error \r\n");
    }
    return 0;
}

static int bl_sha_test(uint8_t *input, uint8_t *output, uint32_t length,
                uint32_t count, SHA_Type sha_type)
{
    uint32_t time_irq_start, speed_time;
    int i;
    speed_time = 0;

    if ((!input) || (!output)) {
        log_error("Can not allocate sha test_buf memory!\r\n");
        return 0;
    }

    // Link mode is not used in this test.
    // Disable potentially enabled link mode
    Sec_Eng_SHA_Disable_Link(SEC_ENG_SHA_ID0);

    time_irq_start = bl_timer_now_us();
    taskENTER_CRITICAL();

    for (i = 0; i < count ; i++) {
        switch (sha_type) {
        case SHA_256_MODE:
            SHA_CHECK(input, output, SEC_ENG_SHA256, length);
            break;
        case SHA_224_MODE:
            SHA_CHECK(input, output, SEC_ENG_SHA224, length);
            break;
        case SHA_1_MODE:
        case SHA_1_RSVD_MODE:
            SHA_CHECK(input, output, SEC_ENG_SHA1, length);
            break;
        default:
            printf("NO Support SHA MODE!\r\n");
            return 0;
        }
    }

    taskEXIT_CRITICAL();
    speed_time = bl_timer_now_us() - time_irq_start;

    printf("speed_time is %ldus\r\n", speed_time);
    printf("sha speed is %.4lfMbps\r\n", length * count * 8.0 / speed_time);

    return 0;
}

static void __sha1_soft_test (uint32_t length, uint32_t count)
{
    uint8_t *test_buf;
    uint32_t time_irq_start, speed_time;
    int i;
    mbedtls_sha1_context ctx1;
    unsigned char sha1sum[20];

    speed_time = 0;
    mbedtls_sha1_init( &ctx1 );

    test_buf = pvPortMalloc(length);
    if (!test_buf) {
        printf("Can not allocate sha test_buf memory!\r\n");
        return;
    }

    for (i = 0; i < length; i++) {
        test_buf[i] = i & 0xff;
    }

    time_irq_start = bl_timer_now_us();

    for (i = 0; i < count ; i++) {
        mbedtls_sha1_starts( &ctx1 );
        mbedtls_sha1_update( &ctx1, test_buf, length );
        mbedtls_sha1_finish( &ctx1, sha1sum );
    }

    speed_time = bl_timer_now_us() - time_irq_start;

    printf("speed_time is %ldus\r\n", speed_time);
    printf("sha speed is %.4lfMbps\r\n", length * count * 8.0 / speed_time);

    mbedtls_sha1_free( &ctx1 );
    vPortFree(test_buf);
}

static void __sha256_soft_test (uint32_t length, uint32_t count, uint8_t is_224)
{
    uint8_t *test_buf;
    uint32_t time_irq_start, speed_time;
    int i;
    mbedtls_sha256_context ctx1;
    unsigned char sha1sum[32];

    speed_time = 0;
    mbedtls_sha256_init( &ctx1 );

    test_buf = pvPortMalloc(length);
    if (!test_buf) {
        printf("Can not allocate sha test_buf memory!\r\n");
        return;
    }

    for (i = 0; i < length; i++) {
        test_buf[i] = i & 0xff;
    }

    time_irq_start = bl_timer_now_us();

    for (i = 0; i < count ; i++) {
        mbedtls_sha256_starts( &ctx1, is_224);
        mbedtls_sha256_update( &ctx1, test_buf, length );
        mbedtls_sha256_finish( &ctx1, sha1sum );
    }

    speed_time = bl_timer_now_us() - time_irq_start;

    printf("speed_time is %ldus\r\n", speed_time);
    printf("sha speed is %.4lfMbps\r\n", length * count * 8.0 / speed_time);

    mbedtls_sha256_free( &ctx1 );
    vPortFree(test_buf);
}

static void __mbedtls_sha_test(uint32_t length, uint32_t count, SHA_Type sha_type)
{
    switch (sha_type) {
        case SHA_256_MODE:
            __sha256_soft_test(length, count, 0);
            break;
        case SHA_224_MODE:
            __sha256_soft_test(length, count, 1);
            break;
        case SHA_1_MODE:
        case SHA_1_RSVD_MODE:
            __sha1_soft_test(length, count);
            break;
        default:
            printf("invalid keytype parametre!\r\n");
            return;
    }
}

void sha_test(uint32_t length, uint32_t count,
              SHA_Type mode, uint8_t is_soft, ram_type_t ram_type)
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

    if (is_soft) {
        printf("SHA SOFT\r\n");
        __mbedtls_sha_test(length, count, mode);
    } else {
        printf("SHA HW\r\n");
        bl_sha_test(ptr, ptr, length, count, mode);
    }
    free_hw(ptr);
}

