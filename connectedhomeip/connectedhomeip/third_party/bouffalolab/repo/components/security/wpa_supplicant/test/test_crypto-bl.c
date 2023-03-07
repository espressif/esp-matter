#include <stdint.h>
#include <string.h>
#include "crypto/aes.h"

#define TEST_FAILED do { printf("%s: LN %d failed\r\n", __func__, __LINE__); } while (0)
#define TEST_SUCCEED do { printf("%s: succeed\r\n", __func__); } while (0)

void wpa_crypto_tc_cmac_1(void)
{
    uint8_t key[16] = {};
    uint8_t data[16] = { 0xab };

    const uint8_t mic_expected[16] = {0x84, 0xef, 0xaf, 0xf6, 0x1f, 0x67, 0xe3, 0xdc, 0x43, 0xe2, 0xe6, 0xea, 0x91, 0x16, 0x64, 0x1f};
    uint8_t mic[16] = {};

    if (omac1_aes_128(key, data, sizeof(data), mic)) {
        TEST_FAILED;
        return;
    }

    if (0 == memcmp(mic, mic_expected, sizeof(mic))) {
        TEST_SUCCEED;
    } else {
        TEST_FAILED;
    }
}

void wpa_crypto_test()
{
    wpa_crypto_tc_cmac_1();
}
