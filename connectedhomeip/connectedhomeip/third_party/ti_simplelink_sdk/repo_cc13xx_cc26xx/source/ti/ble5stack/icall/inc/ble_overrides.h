
#ifndef _INCLUDE_STACK_OVERRIDE_H_
#define _INCLUDE_STACK_OVERRIDE_H_

#include "ble_user_config.h"

// NB! Do NOT change the name of the BLE_STACK_OVERRIDES macro! Doing so will cause
//     compilation failure in the generated file 'ti_radio_config.c'

// CTE Overrides:
//     0x00158000, // S2RCFG: Capture S2R from FrontEnd, on event (CM0 will arm)
//     0x000E51D0, // After FRAC
//     ((CTE_CONFIG << 16) | 0x8BB3), // Enable CTE capture
//     ((CTE_OFFSET << 24) | ((CTE_SAMPLING_CONFIG_1MBPS | (CTE_SAMPLING_CONFIG_2MBPS << 4)) << 16) | 0x0BC3), // Sampling rate, offset
//     0xC0080341, // Pointers in next two entries
//     (uint32_t) NULL, // Pointer to antenna switching table
//     (uint32_t) NULL, // Pointer to auto copy configuration

#define PRIVACY_OVERRIDE_OFFSET     2
#ifdef DONT_TRANSMIT_NEW_RPA
#define OVR_1 0x001D8B73
#else
#define OVR_1 0x001F8B73
#endif

#ifdef RTLS_CTE
#define CTE_OVERRIDES_OFFSET        3

#define BLE_STACK_OVERRIDES()\
    (uint32_t) OVR_1,\
    (uint32_t) 0xC0040361,\
    (uint32_t) 0x00000000,\
    0x00158000, \
    0x000E51D0, \
    ((CTE_CONFIG << 16) | 0x8BB3), \
    ((CTE_OFFSET << 24) | ((CTE_SAMPLING_CONFIG_1MBPS | (CTE_SAMPLING_CONFIG_2MBPS << 4)) << 16) | 0x0BC3), \
    0xC0080341, \
    (uint32_t) NULL, \
    (uint32_t) NULL
#else
#define BLE_STACK_OVERRIDES()\
    (uint32_t) OVR_1,\
    (uint32_t) 0xC0040361,\
    (uint32_t) 0x00000000
#endif


#endif // _INCLUDE_STACK_OVERRIDE_H_
