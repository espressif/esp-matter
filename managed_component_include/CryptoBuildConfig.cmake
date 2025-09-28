# Generating crypto/CryptoBuildConfig.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/crypto/CryptoBuildConfig.h
"#pragma once\n
#include <sdkconfig.h>\n
#define CHIP_CRYPTO_MBEDTLS 1
#define CHIP_CRYPTO_PSA 0
#define CHIP_CRYPTO_PSA_SPAKE2P 0
#define CHIP_CRYPTO_KEYSTORE_PSA 0
#define CHIP_CRYPTO_KEYSTORE_RAW 1
#define CHIP_CRYPTO_KEYSTORE_APP 0
#define CHIP_CRYPTO_OPENSSL 0
#define CHIP_CRYPTO_BORINGSSL 0
#define CHIP_CRYPTO_PLATFORM 0
")
