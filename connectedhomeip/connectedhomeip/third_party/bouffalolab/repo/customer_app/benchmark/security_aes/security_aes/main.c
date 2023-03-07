#include <FreeRTOS.h>
#include <task.h>
#include <stdlib.h>
#include <cli.h>
#include <bl_sys_time.h>
#include <mbedtls/aes.h>
#include <mbedtls/gcm.h>
#include <mbedtls/sha1.h>
#include <mbedtls/sha256.h>

#include "aes_test.h"
#include "gmac_test.h"
#include "sha_test.h"

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

static void test_cmd_aes_ecb(char *buf, int len, int argc, char **argv)
{
    uint32_t length = 32768;
    uint32_t count  = 256;
    aes_key_t keytype = AES_TEST_KEY_128;
    uint8_t aes_soft = 0;
    ram_type_t ram_type = DTCM;

    if (argc >= 2) {
        length = strtol(argv[1], NULL, 0);
    }
    if (argc >= 3) {
        count = strtol(argv[2], NULL, 0);
    }
    if (argc >= 4) {
        keytype = strtol(argv[3], NULL, 0);
    }
    if (argc >= 5) {
        aes_soft = strtol(argv[4], NULL, 0);
    }
    if (argc >= 6) {
        ram_type = strtol(argv[5], NULL, 0);
    }

    aes_ecb_test(length, count, keytype, aes_soft, ram_type);
}

static void test_cmd_aes_cbc(char *buf, int len, int argc, char **argv)
{
    uint32_t length = 32768;
    uint32_t count  = 256;
    aes_key_t keytype = AES_TEST_KEY_128;
    uint8_t aes_soft = 0;
    ram_type_t ram_type = DTCM;

    if (argc >= 2) {
        length = strtol(argv[1], NULL, 0);
    }
    if (argc >= 3) {
        count = strtol(argv[2], NULL, 0);
    }
    if (argc >= 4) {
        keytype = strtol(argv[3], NULL, 0);
    }
    if (argc >= 5) {
        aes_soft = strtol(argv[4], NULL, 0);
    }
    if (argc >= 6) {
        ram_type = strtol(argv[5], NULL, 0);
    }

    aes_cbc_test(length, count, keytype, aes_soft, ram_type);
}

static void test_cmd_aes_ctr(char *buf, int len, int argc, char **argv)
{
    uint32_t length = 32768;
    uint32_t count  = 256;
    aes_key_t keytype = AES_TEST_KEY_128;
    uint8_t aes_soft = 0;
    ram_type_t ram_type = DTCM;

    if (argc >= 2) {
        length = strtol(argv[1], NULL, 0);
    }
    if (argc >= 3) {
        count = strtol(argv[2], NULL, 0);
    }
    if (argc >= 4) {
        keytype = strtol(argv[3], NULL, 0);
    }
    if (argc >= 5) {
        aes_soft = strtol(argv[4], NULL, 0);
    }
    if (argc >= 6) {
        ram_type = strtol(argv[5], NULL, 0);
    }
    aes_ctr_test(length, count, keytype, aes_soft, ram_type);
}

static void test_cmd_aes_gcm(char *buf, int len, int argc, char **argv)
{
    uint32_t length = 32768;
    uint32_t count  = 256;
    aes_key_t keytype = AES_TEST_KEY_128;
    uint8_t aes_soft = 0;
    ram_type_t ram_type = DTCM;

    if (argc >= 2) {
        length = strtol(argv[1], NULL, 0);
    }
    if (argc >= 3) {
        count = strtol(argv[2], NULL, 0);
    }
    if (argc >= 4) {
        keytype = strtol(argv[3], NULL, 0);
    }
    if (argc >= 5) {
        aes_soft = strtol(argv[4], NULL, 0);
    }
    if (argc >= 6) {
        ram_type = strtol(argv[5], NULL, 0);
    }
    aes_gcm_test(length, count, keytype, aes_soft, ram_type);
}

static void test_cmd_gmac(char *buf, int len, int argc, char **argv)
{
    uint32_t length = 32768;
    uint32_t count  = 256;
    ram_type_t ram_type = DTCM;

    if (argc >= 2) {
        length = strtol(argv[1], NULL, 0);
    }
    if (argc >= 3) {
        count = strtol(argv[2], NULL, 0);
    }
    if (argc >= 4) {
        ram_type = strtol(argv[3], NULL, 0);
    }
    gmac_test(length, count, ram_type);
}

static void test_cmd_sha256(char *buf, int len, int argc, char **argv)
{
    uint32_t length = 32768;
    uint32_t count =  256;
    uint8_t sha_soft = 0;
    ram_type_t ram_type = DTCM;

    if (argc >= 2) {
        length = strtol(argv[1], NULL, 0);
    }
    if (argc >= 3) {
        count = strtol(argv[2], NULL, 0);
    }
    if (argc >= 4) {
        sha_soft = strtol(argv[3], NULL, 0);
    }
    if (argc >= 5) {
        ram_type = strtol(argv[4], NULL, 0);
    }
    sha_test(length, count, SHA_256_MODE, sha_soft, ram_type);
}

static void test_cmd_sha224(char *buf, int len, int argc, char **argv)
{
    uint32_t length = 32768;
    uint32_t count =  256;
    uint8_t sha_soft = 0;
    ram_type_t ram_type = DTCM;

    if (argc >= 2) {
        length = strtol(argv[1], NULL, 0);
    }
    if (argc >= 3) {
        count = strtol(argv[2], NULL, 0);
    }
    if (argc >= 4) {
        sha_soft = strtol(argv[3], NULL, 0);
    }
    if (argc >= 5) {
        ram_type = strtol(argv[4], NULL, 0);
    }
    sha_test(length, count, SHA_224_MODE, sha_soft, ram_type);
}

static void test_cmd_sha1(char *buf, int len, int argc, char **argv)
{
    uint32_t length = 32768;
    uint32_t count =  256;
    uint8_t sha_soft = 0;
    ram_type_t ram_type = DTCM;

    if (argc >= 2) {
        length = strtol(argv[1], NULL, 0);
    }
    if (argc >= 3) {
        count = strtol(argv[2], NULL, 0);
    }
    if (argc >= 4) {
        sha_soft = strtol(argv[3], NULL, 0);
    }
    if (argc >= 5) {
        ram_type = strtol(argv[4], NULL, 0);
    }
    sha_test(length, count, SHA_1_MODE, sha_soft, ram_type);
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    { "test_aes_ecb", "AES ECB -length -count -keytype -soft -ramtype", test_cmd_aes_ecb},
    { "test_aes_cbc", "AES CBC -length -count -keytype -soft -ramtype", test_cmd_aes_cbc},
    { "test_aes_ctr", "AES CTR -length -count -keytype -soft -ramtype", test_cmd_aes_ctr},
    { "test_aes_gcm", "AES GCM -length -count -keytype -soft -ramtype", test_cmd_aes_gcm},
    { "test_aes_gmac", "AES GMAC -length -count -ramtype", test_cmd_gmac},
    { "test_sha256", "SHA256 -length -count -soft -ramtype", test_cmd_sha256},
    { "test_sha224", "SHA224 -length -count -soft -ramtype", test_cmd_sha224},
    { "test_sha1", "SHA1 -length -count -soft -ramtype", test_cmd_sha1},
};

void main(void)
{
    system_thread_init();

    puts("[OS] proc_main_entry task...\r\n");
    xTaskCreate(proc_main_entry, (char*)"main_entry", 1024, NULL, 15, NULL);
}
