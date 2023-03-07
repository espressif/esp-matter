// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _SUPPLICANT_OPT_H
#define _SUPPLICANT_OPT_H

/* #include "sdkconfig.h" */

//#define DEBUG_PRINT
//#define WPS_DEBUG

//XXX TODO remove bl606p
#if defined(CFG_CHIP_BL606P) || defined(CFG_CHIP_BL808)
#define USE_MBEDTLS_CRYPTO
#endif

// TODO sync with FULLMAC config
#define CONFIG_AUTHENTICATOR_MAX_STA 4
#define CONFIG_IEEE80211W
#define CONFIG_WPA3_SAE
//#define IEEE8021X_EAPOL // PMKSA cache


#define WPA_SUPPLICANT_4WAY_HANDSHAKE_TIMEOUT_MS (10 * 1000)

#define SAE_FFC 0

// WPS
#define CONFIG_WPS2
#define CONFIG_WPS_PIN


//#define CONFIG_SUPPLICANT_REKEY_WHEN_TIMEDOUT
//#define CONFIG_AUTHENTICATOR_REKEY_WHEN_TIMEDOUT

// XXX MIC failure countermeasure disabled for now
//#define CONFIG_MIC_FAILURE_COUNTERMEASURE

//#define CONFIG_VALIDATE_IE_IN_PROCESS_3_OF_4

//#define CONFIG_AP_SPP

#if 0
#if CONFIG_WPA_MBEDTLS_CRYPTO
#else
#define CONFIG_TLS_INTERNAL_CLIENT
#define CONFIG_TLSV12
#endif

#if CONFIG_WPA_DEBUG_PRINT
#define DEBUG_PRINT
#endif

#if CONFIG_WPA_11KV_SUPPORT
#define ROAMING_SUPPORT 1
#endif

#if CONFIG_WPA_SCAN_CACHE
#define SCAN_CACHE_SUPPORTED
#endif
#endif

#endif /* _SUPPLICANT_OPT_H */
