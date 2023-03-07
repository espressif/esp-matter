#
# CMakeLists.txt file for cryptoauthlib
#
cmake_minimum_required(VERSION 3.5)
set(CRYPTOAUTHLIB_DIR "cryptoauthlib/lib")
set(COMPONENT_SRCDIRS       "${CRYPTOAUTHLIB_DIR}/atcacert"
                            "${CRYPTOAUTHLIB_DIR}/calib"
                            "${CRYPTOAUTHLIB_DIR}/crypto"
                            "${CRYPTOAUTHLIB_DIR}/crypto/hashes"
                            "${CRYPTOAUTHLIB_DIR}/host"
                            "${CRYPTOAUTHLIB_DIR}/mbedtls"
                            "${CRYPTOAUTHLIB_DIR}"
                            "${CRYPTOAUTHLIB_DIR}/../app/tng"
                            "port"
                            )

set(COMPONENT_SRCS          "${COMPONENT_DIR}/cryptoauthlib/lib/hal/atca_hal.c"
                            "${COMPONENT_DIR}/cryptoauthlib/lib/hal/hal_freertos.c"
                            "${COMPONENT_DIR}/cryptoauthlib/third_party/hal/esp32/hal_esp32_i2c.c"
                            "${COMPONENT_DIR}/cryptoauthlib/third_party/hal/esp32/hal_esp32_timer.c"
                            )

set(COMPONENT_INCLUDEDIRS   "${CRYPTOAUTHLIB_DIR}/"
                            "${CRYPTOAUTHLIB_DIR}/hal"
                            "${CRYPTOAUTHLIB_DIR}/../app/tng"
                            "port"
                            )

set(COMPONENT_PRIV_INCLUDEDIRS "port/include")

set(COMPONENT_REQUIRES      "mbedtls" "freertos"  "driver")

# Don't include the default interface configurations from cryptoauthlib
set(COMPONENT_EXCLUDE_SRCS "${CRYPTOAUTHLIB_DIR}/atca_cfgs.c")
set(COMPONENT_CFLAGS "ESP32")

idf_component_register(     SRC_DIRS        "${COMPONENT_SRCDIRS}"
                            INCLUDE_DIRS    "${COMPONENT_INCLUDEDIRS}"
                            PRIV_INCLUDE_DIRS "${COMPONENT_PRIV_INCLUDEDIRS}"
                            REQUIRES        "${COMPONENT_REQUIRES}"
                            EXCLUDE_SRCS    "${COMPONENT_EXCLUDE_SRCS}"
                            )

target_sources(${COMPONENT_LIB} PRIVATE ${COMPONENT_SRCS})
target_compile_definitions(${COMPONENT_LIB} PRIVATE ${COMPONENT_CFLAGS})
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-pointer-sign)
