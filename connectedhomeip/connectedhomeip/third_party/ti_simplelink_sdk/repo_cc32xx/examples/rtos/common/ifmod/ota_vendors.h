/*
 * Copyright (C) 2016-2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OTA_VENDORS_H_
#define OTA_VENDORS_H_

#include <stdint.h>
#include <ti/net/http/httpclient.h>
#include "ota_settings.h"
#include "ota_if.h"


#if OTA_SUPPORT
#define OTA_VENDOR_NEW_IMAGE_FOUND        0
#define OTA_VENDOR_NO_NEW_IMAGE_FOUND     -35000


#if OTA_VENDOR_DROPBOX_SUPPORT
/*!

    \brief     Prototype of the user defined getDownloadLink function for
               retreiving the OTA image download link from Dropbox. Follows
               the ota_getDownloadLink_f function pointer prototype defined in
               ota_if.h for use with OTA_IF_downloadImageByCloudVendor.

    \param[out] pServerParams - a pointer to the caller allocated structure.
               Upon success will fill the OTA image parameters needed for
               the download.

    \return    0 upon success or a negative error code
                (OTA_VENDOR_NO_NEW_VERSION_FOUND - when new image was not found)


*/
int32_t OTA_DROPBOX_getDownloadLink(FileServerParams_t *pServerParams);
#endif

#if OTA_VENDOR_GITHUB_SUPPORT
/*!

    \brief     Prototype of the user defined getDownloadLink function for
               retreiving the OTA image download link from Github. Follows
               the ota_getDownloadLink_f function pointer prototype defined in
               ota_if.h for use with OTA_IF_downloadImageByCloudVendor.

    \param[out] pServerParams - a pointer to the caller allocated structure.
               Upon success will fill the OTA image parameters needed for
               the download.

    \return    0 upon success or a negative error code
                (OTA_VENDOR_NO_NEW_VERSION_FOUND - when new image was not found)

*/
int32_t OTA_GITHUB_getDownloadLink (FileServerParams_t *pServerParams);
#endif

#endif /* OTA_SUPPORT */

#endif /* OTA_VENDORS_H_ */
