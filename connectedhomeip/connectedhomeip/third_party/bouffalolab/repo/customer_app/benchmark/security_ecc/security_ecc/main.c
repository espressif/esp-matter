#include <FreeRTOS.h>
#include <task.h>
#include <cli.h>

#include <bl_sys_time.h>
#include <bl_timer.h>
#include <mbedtls/aes.h>
#include <mbedtls/gcm.h>
#include <mbedtls/sha1.h>
#include <mbedtls/sha256.h>

#include <bflb_ecdsa.h>
#include <bl602_sec_eng.h>

#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/ecdh.h>

#include <hw_acc/hw_common.h>

#define mbedtls_printf          printf

static void _cli_init()
{
    /*Put CLI which needs to be init here*/
    bl_sys_time_cli_init();
}

static void proc_main_entry(void *pvParameters)
{
    _cli_init();

    vTaskDelete(NULL);
}

static void system_thread_init()
{
    /*nothing here*/
}

static const uint8_t secp256r1N[32] ALIGN4 ={
        0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
        0xbc,0xe6,0xfa,0xad,0xa7,0x17,0x9e,0x84,0xf3,0xb9,0xca,0xc2,0xfc,0x63,0x25,0x51
};

static void dump_buf(char *info, uint8_t *buf, uint32_t len)
{
    mbedtls_printf("%s", info);
    for (int i = 0; i < len; i++) {
        mbedtls_printf("%s%02X%s", i % 16 == 0 ? "\r\n     ":" ",
                        buf[i], i == len - 1 ? "\r\n":"");
    }
}

static void test_ecdh(char *buf, int len, int argc, char **argv)
{
    int32_t ret;
    uint32_t time_irq_start, speed_time, total_time = 0;
    uint32_t ecdh_keyx_a[8];
    uint32_t ecdh_keyy_a[8];
    uint8_t ecdh_private_key_a[32];
    uint32_t ecdh_keyx_b[8];
    uint32_t ecdh_keyy_b[8];
    uint8_t ecdh_private_key_b[32];

    printf("ECDH Case\r\n");

    ret=bflb_ecc_get_random_value((uint32_t *)ecdh_private_key_a,
                                  (uint32_t *)secp256r1N,
                                  32);
    if(ret!=0){
        printf("Create random fail\r\n");
    }

    ret=bflb_ecc_get_random_value((uint32_t *)ecdh_private_key_b,
                                  (uint32_t *)secp256r1N,
                                  32);
    if(ret!=0){
        printf("Create random fail\r\n");
    }

    Sec_Eng_PKA_Reset();
    Sec_Eng_PKA_BigEndian_Enable();

    time_irq_start = bl_timer_now_us();
    taskENTER_CRITICAL();
    bflb_ecdh_get_public_key(0,
                             NULL,
                             NULL,
                             (uint32_t *)ecdh_private_key_a,
                             ecdh_keyx_a,
                             ecdh_keyy_a);
    taskEXIT_CRITICAL();
    speed_time = bl_timer_now_us() - time_irq_start;
    printf("A generate public time is %ldus\r\n", speed_time);
    dump_buf("get_public_x", (uint8_t *)ecdh_keyx_a, 32);
    dump_buf("get_public_y", (uint8_t *)ecdh_keyy_a, 32);
    printf("\r\n");

    Sec_Eng_PKA_Reset();
    Sec_Eng_PKA_BigEndian_Enable();

    time_irq_start = bl_timer_now_us();
    taskENTER_CRITICAL();
    bflb_ecdh_get_public_key(0,
                             NULL,
                             NULL,
                             (uint32_t *)ecdh_private_key_b,
                             ecdh_keyx_b,
                             ecdh_keyy_b);
    taskEXIT_CRITICAL();
    speed_time = bl_timer_now_us() - time_irq_start;
    printf("B generate public time is %ldus\r\n", speed_time);
    total_time += speed_time;
    dump_buf("get_public_x", (uint8_t *)ecdh_keyx_b, 32);
    dump_buf("get_public_y", (uint8_t *)ecdh_keyy_b, 32);
    printf("\r\n");

    Sec_Eng_PKA_Reset();
    Sec_Eng_PKA_BigEndian_Enable();

    time_irq_start = bl_timer_now_us();
    taskENTER_CRITICAL();
    bflb_ecdh_get_public_key(0,
                             ecdh_keyx_a,
                             ecdh_keyy_a,
                             (uint32_t *)ecdh_private_key_b,
                             ecdh_keyx_a,
                             ecdh_keyy_a);
    taskEXIT_CRITICAL();
    speed_time = bl_timer_now_us() - time_irq_start;
    printf("generate A secret time is %ldus\r\n", speed_time);
    total_time += speed_time;
    dump_buf("gen secret x", (uint8_t *)ecdh_keyx_a, 32);
    dump_buf("gen secret y", (uint8_t *)ecdh_keyy_a, 32);
    printf("\r\n");

    Sec_Eng_PKA_Reset();
    Sec_Eng_PKA_BigEndian_Enable();

    time_irq_start = bl_timer_now_us();
    taskENTER_CRITICAL();
    bflb_ecdh_get_public_key(0,
                             ecdh_keyx_b,
                             ecdh_keyy_b,
                             (uint32_t *)ecdh_private_key_a,
                             ecdh_keyx_b,
                             ecdh_keyy_b);
    taskEXIT_CRITICAL();
    speed_time = bl_timer_now_us() - time_irq_start;
    printf("generate B secret time is %ldus\r\n", speed_time);
    total_time += speed_time;
    dump_buf("gen secret x", (uint8_t *)ecdh_keyx_b, 32);
    dump_buf("gen secret y", (uint8_t *)ecdh_keyy_b, 32);
    printf("\r\n");

    if (memcmp(ecdh_keyx_b, ecdh_keyx_a, sizeof(ecdh_keyx_b)) != 0) {
        printf("error\r\n");
    }
    if (memcmp(ecdh_keyy_b, ecdh_keyy_a, sizeof(ecdh_keyy_b)) != 0) {
        printf("error\r\n");
    }
    printf("ecdh successful total time %ldus\r\n", total_time);
}

static void test_ecdsa(char *buf, int len, int argc, char **argv)
{
    uint32_t i=0;
    int ret;
    uint8_t *p;
    uint32_t time_irq_start, speed_time, total_time = 0;

    static uint8_t ecdsa_hash[32]={0xAF,0x2B,0xDB,0xE1,0xAA,0x9B,0x6E,0xC1,0xE2,0xAD,0xE1,0xD6,0x94,0xF4,0x1F,0xC7,
                        0x1A,0x83,0x1D,0x02,0x68,0xE9,0x89,0x15,0x62,0x11,0x3D,0x8A,0x62,0xAD,0xD1,0xBF};
    static uint8_t ecdsa_private_key[32]={0xC9,0xAF,0xA9,0xD8,0x45,0xBA,0x75,0x16,0x6B,0x5C,0x21,0x57,0x67,0xB1,0xD6,0x93,
                              0x4E,0x50,0xC3,0xDB,0x36,0xE8,0x9B,0x12,0x7B,0x8A,0x62,0x2B,0x12,0x0F,0x67,0x21};
    static uint8_t ecdsa_public_keyx[32]={0x60,0xFE,0xD4,0xBA,0x25,0x5A,0x9D,0x31,0xC9,0x61,0xEB,0x74,0xC6,0x35,0x6D,0x68,
                              0xC0,0x49,0xB8,0x92,0x3B,0x61,0xFA,0x6C,0xE6,0x69,0x62,0x2E,0x60,0xF2,0x9F,0xB6};
    static uint8_t ecdsa_public_keyy[32]={0x79,0x03,0xFE,0x10,0x08,0xB8,0xBC,0x99,0xA4,0x1A,0xE9,0xE9,0x56,0x28,0xBC,0x64,
                              0xF2,0xF1,0xB2,0x0C,0x2D,0x7E,0x9F,0x51,0x77,0xA3,0xC2,0x94,0xD4,0x46,0x22,0x99};
    static uint8_t ecdsa_k[32]={0xA6,0xE3,0xC5,0x7D,0xD0,0x1A,0xBE,0x90,0x08,0x65,0x38,0x39,0x83,0x55,0xDD,0x4C,
                    0x3B,0x17,0xAA,0x87,0x33,0x82,0xB0,0xF2,0x4D,0x61,0x29,0x49,0x3D,0x8A,0xAD,0x60};
    static uint8_t ecdsa_r[32]={0xEF,0xD4,0x8B,0x2A,0xAC,0xB6,0xA8,0xFD,0x11,0x40,0xDD,0x9C,0xD4,0x5E,0x81,0xD6,
                    0x9D,0x2C,0x87,0x7B,0x56,0xAA,0xF9,0x91,0xC3,0x4D,0x0E,0xA8,0x4E,0xAF,0x37,0x16};
    static uint8_t ecdsa_s[32]={0xF7,0xCB,0x1C,0x94,0x2D,0x65,0x7C,0x41,0xD4,0x36,0xC7,0xA1,0xB6,0xE2,0x9F,0x65,
                    0xF3,0xE9,0x00,0xDB,0xB9,0xAF,0xF4,0x06,0x4D,0xC4,0xAB,0x2F,0x84,0x3A,0xCD,0xA8};

    uint32_t ecdh_keyx[8];
    uint32_t ecdh_keyy[8];

    printf("ECDSA Case\r\n");

    Sec_Eng_PKA_Reset();
    Sec_Eng_PKA_BigEndian_Enable();

    time_irq_start = bl_timer_now_us();
    taskENTER_CRITICAL();
    bflb_ecdh_get_public_key(0,
                             NULL,
                             NULL,
                             (uint32_t *)ecdsa_private_key,
                             ecdh_keyx,
                             ecdh_keyy);
    taskEXIT_CRITICAL();
    speed_time = bl_timer_now_us() - time_irq_start;
    printf("generate public time is %ldus\r\n", speed_time);
    total_time += speed_time;
    dump_buf("get_public_x", (uint8_t *)ecdh_keyx, 32);
    dump_buf("get_public_y", (uint8_t *)ecdh_keyy, 32);

    p=(uint8_t *)ecdh_keyx;
    for(i=0;i<32;i++){
        if(ecdsa_public_keyx[i]!=p[i]){
            printf("ECDSA Compare fail\r\n");
        }
    }
    p=(uint8_t *)ecdh_keyy;
    for(i=0;i<32;i++){
        if(ecdsa_public_keyy[i]!=p[i]){
            printf("ECDSA Compare fail\r\n");
        }
    }

    Sec_Eng_PKA_Reset();
    Sec_Eng_PKA_BigEndian_Enable();

    time_irq_start = bl_timer_now_us();
    taskENTER_CRITICAL();
    bflb_ecdsa_sign(0,
                    (uint32_t *)ecdsa_private_key,
                    (uint32_t *)ecdsa_k,
                    (uint32_t *)ecdsa_hash,
                    ecdh_keyx,
                    ecdh_keyy);
    taskEXIT_CRITICAL();
    speed_time = bl_timer_now_us() - time_irq_start;
    printf("ecdsa sign time is %ldus\r\n", speed_time);
    total_time += speed_time;
    dump_buf("get_public_r", (uint8_t *)ecdh_keyx, 32);
    dump_buf("get_public_s", (uint8_t *)ecdh_keyy, 32);

    p=(uint8_t *)ecdh_keyx;
    for(i=0;i<32;i++){
        if(ecdsa_r[i]!=p[i]){
            printf("ECDSA Compare fail\r\n");
        }
    }
    p=(uint8_t *)ecdh_keyy;
    for(i=0;i<32;i++){
        if(ecdsa_s[i]!=p[i]){
            printf("ECDSA Compare fail\r\n");
        }
    }

    Sec_Eng_PKA_Reset();
    Sec_Eng_PKA_BigEndian_Enable();

    time_irq_start = bl_timer_now_us();
    taskENTER_CRITICAL();
    ret = bflb_ecdsa_verify(0,
                            (uint32_t *)ecdsa_hash,
                            32,
                            (uint32_t *)ecdsa_public_keyx,
                            (uint32_t *)ecdsa_public_keyy,
                            ecdh_keyx,
                            ecdh_keyy);
    taskEXIT_CRITICAL();
    speed_time = bl_timer_now_us() - time_irq_start;
    printf("ecdsa verify time is %ldus\r\n", speed_time);
    total_time += speed_time;

    if(ret != 0){
        printf("Verify Fail\r\n");
    }else{
        printf("Verify Success total time %ldus\r\n", total_time);
    }
}

#define assert_exit(a, b)                     \
        if (!(a)) {                           \
            mbedtls_printf("failed\r\n");     \
            goto exit;                        \
        } else {                              \
            mbedtls_printf("sucessful\r\n");  \
        }

static void test_ecdh_soft(char *p_buf, int len, int argc, char **argv)
{
    int ret;
    size_t olen;
    uint8_t buf[65];
    uint32_t time_irq_start, speed_time, total_time = 0;
    static mbedtls_ecp_group grp;
    static mbedtls_mpi cli_secret, srv_secret;
    static mbedtls_mpi cli_pri, srv_pri;
    static mbedtls_ecp_point cli_pub, srv_pub;
    static mbedtls_entropy_context entropy;
    static mbedtls_ctr_drbg_context ctr_drbg;
    char *pers = "simple_ecdh";

    mbedtls_mpi_init(&cli_pri); //
    mbedtls_mpi_init(&srv_pri);
    mbedtls_mpi_init(&cli_secret);
    mbedtls_mpi_init(&srv_secret);
    mbedtls_ecp_group_init(&grp); //初始化椭圆曲线群结构体
    mbedtls_ecp_point_init(&cli_pub); //初始化椭圆曲线点结构体 cli
    mbedtls_ecp_point_init(&srv_pub);//初始化椭圆曲线点结构体 srv
    mbedtls_entropy_init(&entropy); //初始化熵结构体
    mbedtls_ctr_drbg_init(&ctr_drbg);//初始化随机数结构体
/*
    mbedtls_entropy_add_source(&entropy, entropy_source, NULL,
                       MBEDTLS_ENTROPY_MAX_GATHER, MBEDTLS_ENTROPY_SOURCE_STRONG);*/
    mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                (const uint8_t *) pers, strlen(pers));
    mbedtls_printf("  . setup rng ... ok\r\n");

    //加载椭圆曲线，选择SECP256R1
    ret = mbedtls_ecp_group_load(&grp, MBEDTLS_ECP_DP_SECP256R1);
    mbedtls_printf("  . select ecp group SECP256R1 ... ok\r\n");

    time_irq_start = bl_timer_now_us();
    taskENTER_CRITICAL();
    //cli生成公开参数
    ret = mbedtls_ecdh_gen_public(&grp,    //椭圆曲线结构体
                                  &cli_pri,//输出cli私密参数d
                                  &cli_pub,//输出cli公开参数Q
                                  mbedtls_ctr_drbg_random, &ctr_drbg);
    assert_exit(ret == 0, ret);

    taskEXIT_CRITICAL();
    speed_time = bl_timer_now_us() - time_irq_start;
    printf("A generate public time is %ldus\r\n", speed_time);
    total_time += speed_time;

    mbedtls_ecp_point_write_binary(&grp, &cli_pub, //把cli的公开参数到处到buf中
                            MBEDTLS_ECP_PF_UNCOMPRESSED, &olen, buf, sizeof(buf));
    dump_buf("ecdh client generate public parameter:", buf, olen);

    time_irq_start = bl_timer_now_us();
    taskENTER_CRITICAL();
    //srv生成公开参plf数
    ret = mbedtls_ecdh_gen_public(&grp,    //椭圆曲线结构体
                                  &srv_pri,//输出srv私密参数d
                                  &srv_pub,//输出srv公开参数Q
                                  mbedtls_ctr_drbg_random, &ctr_drbg);
    assert_exit(ret == 0, ret);

    taskEXIT_CRITICAL();
    speed_time = bl_timer_now_us() - time_irq_start;
    printf("B generate public time is %ldus\r\n", speed_time);
    total_time += speed_time;

    mbedtls_ecp_point_write_binary(&grp, &srv_pub, //把srv的公开参数导出到buf中
                            MBEDTLS_ECP_PF_UNCOMPRESSED, &olen, buf, sizeof(buf));
    dump_buf("ecdh server generate public parameter:", buf, olen);

    time_irq_start = bl_timer_now_us();
    taskENTER_CRITICAL();
    //cli计算共享密钥
    ret = mbedtls_ecdh_compute_shared(&grp,    //椭圆曲线结构体
                                      &cli_secret, //cli计算出的共享密钥
                                      &srv_pub, //输入srv公开参数Q
                                      &cli_pri, //输入cli本身的私密参数d
                                      mbedtls_ctr_drbg_random, &ctr_drbg);
    assert_exit(ret == 0, ret);

    taskEXIT_CRITICAL();
    speed_time = bl_timer_now_us() - time_irq_start;
    printf("generate A secret time is %ldus\r\n", speed_time);
    total_time += speed_time;

    //把cli计算出的共享密钥导出buf中
    mbedtls_mpi_write_binary(&cli_secret, buf, mbedtls_mpi_size(&cli_secret));
    dump_buf("ecdh client generate secret:", buf, mbedtls_mpi_size(&cli_secret));

    time_irq_start = bl_timer_now_us();
    taskENTER_CRITICAL();
    //srv计算共享密钥
    ret = mbedtls_ecdh_compute_shared(&grp,   //椭圆曲线结构体
                                      &srv_secret, //srv计算出的共享密钥
                                      &cli_pub, //输入cli公开参数Q
                                      &srv_pri, //输入srv本身的私密参数d
                                      mbedtls_ctr_drbg_random, &ctr_drbg);
    assert_exit(ret == 0, ret);

    taskEXIT_CRITICAL();
    speed_time = bl_timer_now_us() - time_irq_start;
    printf("generate B secret time is %ldus\r\n", speed_time);
    total_time += speed_time;

    //把srv计算出的共享密钥导出buf中
    mbedtls_mpi_write_binary(&srv_secret, buf, mbedtls_mpi_size(&srv_secret));
    dump_buf("ecdh server generate secret:", buf, mbedtls_mpi_size(&srv_secret));

    //比较2个大数是否相等
    ret = mbedtls_mpi_cmp_mpi(&cli_secret, &srv_secret);
    assert_exit(ret == 0, ret);
    mbedtls_printf("ecdh checking secrets ok total time %ldus\r\n", total_time);

exit:
    mbedtls_mpi_free(&cli_pri);
    mbedtls_mpi_free(&srv_pri);
    mbedtls_mpi_free(&cli_secret);
    mbedtls_mpi_free(&srv_secret);
    mbedtls_ecp_group_free(&grp);
    mbedtls_ecp_point_free(&cli_pub);
    mbedtls_ecp_point_free(&srv_pub);
    mbedtls_entropy_free(&entropy);
    mbedtls_ctr_drbg_free(&ctr_drbg);
}

static void dump_pubkey( const char *title, mbedtls_ecdsa_context *key )
{
    unsigned char buf[300];
    size_t len;

    if( mbedtls_ecp_point_write_binary( &key->grp, &key->Q,
                MBEDTLS_ECP_PF_UNCOMPRESSED, &len, buf, sizeof buf ) != 0 ) {
        mbedtls_printf("internal error\r\n");
        return;
    }
    dump_buf((char *)title, buf, len );
}

static void test_ecdsa_soft(char *p_buf, int len, int argc, char **argv)
{
    int ret = 1;
    uint32_t time_irq_start, speed_time, total_time = 0;
    static mbedtls_ecdsa_context ctx_sign, ctx_verify;
    static mbedtls_entropy_context entropy;
    static mbedtls_ctr_drbg_context ctr_drbg;
    static unsigned char message[100];
    static unsigned char hash[32];
    static unsigned char sig[MBEDTLS_ECDSA_MAX_LEN];
    size_t sig_len;
    const char *pers = "ecdsa";
    ((void) argv);

    mbedtls_ecdsa_init( &ctx_sign );
    mbedtls_ecdsa_init( &ctx_verify );
    mbedtls_ctr_drbg_init( &ctr_drbg );

    memset( sig, 0, sizeof( sig ) );
    memset( message, 0x25, sizeof( message ) );

    mbedtls_entropy_init( &entropy );
    if( ( ret = mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy,
                               (const unsigned char *) pers,
                               strlen( pers ) ) ) != 0 ) {
        mbedtls_printf( " failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret );
        goto exit;
    }

    time_irq_start = bl_timer_now_us();
    taskENTER_CRITICAL();
    if( ( ret = mbedtls_ecdsa_genkey( &ctx_sign, MBEDTLS_ECP_DP_SECP256R1,
                              mbedtls_ctr_drbg_random, &ctr_drbg ) ) != 0 ) {
        mbedtls_printf( " failed\r\n  ! mbedtls_ecdsa_genkey returned %d\r\n", ret );
        goto exit;
    }
    taskEXIT_CRITICAL();
    speed_time = bl_timer_now_us() - time_irq_start;
    printf("generate public time is %ldus\r\n", speed_time);
    total_time += speed_time;
    dump_pubkey( "Public key: ", &ctx_sign );

    if( ( ret = mbedtls_sha256_ret( message, sizeof( message ), hash, 0)) != 0 ) {
        mbedtls_printf( " failed\r\n  ! mbedtls_sha256_ret returned %d\r\n", ret );
        goto exit;
    }

    time_irq_start = bl_timer_now_us();
    taskENTER_CRITICAL();
    if((ret = mbedtls_ecdsa_write_signature( &ctx_sign, MBEDTLS_MD_SHA256,
                                       hash, sizeof( hash ),
                                       sig, &sig_len,
                                       mbedtls_ctr_drbg_random, &ctr_drbg)) != 0) {
        mbedtls_printf( " failed\r\n  ! mbedtls_ecdsa_genkey returned %d\r\n", ret );
        goto exit;
    }
    taskEXIT_CRITICAL();
    speed_time = bl_timer_now_us() - time_irq_start;
    printf("ecdsa sign time is %ldus\r\n", speed_time);
    total_time += speed_time;
    dump_buf("signature: ", sig, sig_len);

    if((ret = mbedtls_ecp_group_copy( &ctx_verify.grp, &ctx_sign.grp)) != 0 ) {
        mbedtls_printf( " failed\r\n  ! mbedtls_ecp_group_copy returned %d\r\n", ret );
        goto exit;
    }

    if((ret = mbedtls_ecp_copy( &ctx_verify.Q, &ctx_sign.Q ) ) != 0) {
        mbedtls_printf( " failed\r\n  ! mbedtls_ecp_copy returned %d\r\n", ret );
        goto exit;
    }

    time_irq_start = bl_timer_now_us();
    taskENTER_CRITICAL();
    if((ret = mbedtls_ecdsa_read_signature( &ctx_verify,
                                      hash, sizeof( hash ),
                                      sig, sig_len)) != 0) {
        mbedtls_printf(" failed\r\n  ! mbedtls_ecdsa_read_signature returned %d\r\n", ret );
        goto exit;
    }
    taskEXIT_CRITICAL();
    speed_time = bl_timer_now_us() - time_irq_start;
    printf("cecdsa verify time is %ldus\r\n", speed_time);
    total_time += speed_time;

    mbedtls_printf("check signature ok total time %ldus\r\n", total_time);
exit:
    mbedtls_ecdsa_free( &ctx_verify );
    mbedtls_ecdsa_free( &ctx_sign );
    mbedtls_ctr_drbg_free( &ctr_drbg );
    mbedtls_entropy_free( &entropy );
}

#define CHECK(r) \
    do {         \
        if ((ret = r)) { \
            printf("%s: LN %d returned %d\r\n", __func__, __LINE__, ret); \
            goto out; \
        } \
    } while (0)

#define CRT_ENT taskENTER_CRITICAL()
#define CRT_LEV taskEXIT_CRITICAL()
#define DEF_TMR uint32_t tick_
#define FED_TMR tick_ = bl_timer_now_us()
#define PRN_ELP(tag) printf(tag " cost %lu us\r\n", bl_timer_now_us() - tick_)

static void test_point_mul_task(void *p_)
{
    int ret;
    mbedtls_ecp_group grp;
    mbedtls_mpi scalar;
    mbedtls_ecp_point point;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

    mbedtls_ecp_group_init(&grp);
    mbedtls_mpi_init(&scalar);
    mbedtls_ecp_point_init(&point);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    CHECK(mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0));

#define TEST_CNT_PER_CURVE 10
    // XXX Enable all curves to do the test
    for (int id = MBEDTLS_ECP_DP_NONE + 1; id <= MBEDTLS_ECP_DP_CURVE448; ++id) {
        printf("====================== Testing ID  %d ======================\r\n", id);
        CHECK(mbedtls_ecp_group_load(&grp, id));
        for (int cnt = 0; cnt < TEST_CNT_PER_CURVE; ++cnt) {
            printf("--- Testing id %d, %d/%d ---\r\n", id, cnt + 1, TEST_CNT_PER_CURVE);
            CHECK( mbedtls_ecp_gen_privkey( &grp, &scalar, mbedtls_ctr_drbg_random, &ctr_drbg ) );

            DEF_TMR;
            CRT_ENT;
            FED_TMR;
            CHECK( mbedtls_ecp_mul( &grp, &point, &scalar, &grp.G, mbedtls_ctr_drbg_random, &ctr_drbg ) );
            PRN_ELP("ecp_mul");
            CRT_LEV;

            mbedtls_ecp_point_free(&point);
        }
    }

out:
    mbedtls_ecp_group_free(&grp);
    mbedtls_mpi_free(&scalar);
    mbedtls_ecp_point_free(&point);
    mbedtls_entropy_free(&entropy);
    mbedtls_ctr_drbg_free(&ctr_drbg);

    vTaskDelete(NULL);
}

static void test_point_mul(char *p_buf, int len, int argc, char **argv)
{
    xTaskCreate(test_point_mul_task, (char*)"ecp-mul", 2048, NULL, 15, NULL);
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    { "test_ecdh_hw", "ecdh", test_ecdh},
    { "test_ecdsa_hw", "ecdsa", test_ecdsa},
    // Though called "soft", these might actually be "hw".
    { "test_ecdh_soft", "ecdh", test_ecdh_soft},
    { "test_ecdsa_soft", "ecdh", test_ecdsa_soft},
    { "test_point_mul", "test point mul", test_point_mul },
};

void main(void)
{
    system_thread_init();

    puts("[OS] proc_main_entry task...\r\n");
    xTaskCreate(proc_main_entry, (char*)"main_entry", 1024, NULL, 15, NULL);
}
