/*
 * Copyright (c) 2015 - 2016, Freescale Semiconductor, Inc.
 * Copyright 2016 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __USB_OSA_BM_H__
#define __USB_OSA_BM_H__

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define USB_OSA_SR_ALLOC() uint32_t usbOsaCurrentSr;
#define USB_OSA_ENTER_CRITICAL() USB_OsaEnterCritical(&usbOsaCurrentSr)
#define USB_OSA_EXIT_CRITICAL() USB_OsaExitCritical(usbOsaCurrentSr)

/*******************************************************************************
 * API
 ******************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

extern void USB_OsaEnterCritical(uint32_t *sr);
extern void USB_OsaExitCritical(uint32_t sr);

#if defined(__cplusplus)
}
#endif

#endif /* __USB_OSA_BM_H__ */
