#pragma once
/* Cryptoauthlib Configuration File */
#ifndef ATCA_CONFIG_H
#define ATCA_CONFIG_H

/* Include HALS */
#define ATCA_HAL_I2C
#define ATCA_USE_RTOS_TIMER 1
#define ATCA_MBEDTLS
//#define ATCA_CA_SUPPORT
/* Included device support */
#define ATCA_ATECC608_SUPPORT

#define ATCA_TNG_LEGACY_SUPPORT
#define ATCA_TFLEX_SUPPORT
#define ATCA_TNGTLS_SUPPORT
#define ATCA_TNGLORA_SUPPORT

/* \brief How long to wait after an initial wake failure for the POST to
 *         complete.
 * If Power-on self test (POST) is enabled, the self test will run on waking
 * from sleep or during power-on, which delays the wake reply.
 */
#ifndef ATCA_POST_DELAY_MSEC
#define ATCA_POST_DELAY_MSEC 25
#endif

#define ATCA_PLATFORM_MALLOC malloc
#define ATCA_PLATFORM_FREE free

#define hal_delay_ms atca_delay_ms
#define ATCA_PRINTF
#endif // ATCA_CONFIG_H
