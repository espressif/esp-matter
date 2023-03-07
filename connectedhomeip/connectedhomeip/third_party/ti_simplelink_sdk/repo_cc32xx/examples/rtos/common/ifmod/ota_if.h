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

//*****************************************************************************
// Includes
//*****************************************************************************
// Standard includes

#ifndef OTA_IF_H
#define OTA_IF_H

#include "ota_settings.h"
#include "ti/net/http/httpclient.h"
#include "httpsrv_if.h"
#include "utils_if.h"

#if OTA_SUPPORT

/*****************************************************************************************
 * OTA_IF Macro and Structures Definition
 * ***************************************************************************************
 */

#define OTA_THREAD_STACK_SIZE               (3*1024)
#define OTA_THREAD_PRIORITY                 2


// The following are available OTA flags
#define OTA_IF_FLAG_DISABLE_DOWNGRADE_PROTECTION      (OTA_DISABLE_DOWNGRADE_PROTECTION<<0)


typedef enum
{
    OTA_NOTIF_UPDATE_NOT_AVAILABLE = 1,
    OTA_NOTIF_IMAGE_DOWNLOADED,
    OTA_NOTIF_IMAGE_PENDING_COMMIT,
    OTA_NOTIF_UPDATE_TOO_OLD,
    OTA_NOTIF_GETLINK_ERROR = -1,
    OTA_NOTIF_DOWNLOAD_ERROR = -2,
    OTA_NOTIF_COMMIT_ERROR = -3,
    OTA_NOTIF_INSTALL_ERROR = -4,
} otaNotif_e;



typedef struct
{
    /* array to store the download URL of the image */
    char                        URL[OTA_URL_LEN];

    /* (pointer to) Image version -
     * By default this will be used to point to the timestamp that prefixes
     * the OTA Image name.
     * This can be replaced to point to any other user data (length is fixed and
     * defined by OTA_VERSION_LEN)
     * If a NULL pointer will be used - the version will be based on the timestamp
     * at the beginning the TAR file (first 14 Bytes)
     * During the download - the version will be stored to a file (ota.dat) and it
     * can be read using OTA_IF_getCurrentVersion to compare with new candidate.
     */
    uint8_t                     *pVersion;

    /* User's (client) private key file (path) */
    const char                  *pPrivateKey;

    /* User's (client) certificate file (path) */
    const char                  *pClientCert;

    /* Primary root CA file (path) -
     * By default TI provides the "RootCACerts.pem" that contains several root CA
     * certs required for both GITHUB and DROPBOX connection. It is possible to replace this or
     * to use the pRootCa2 as a fallback. CC323x may connect to server's that requires
     * any of the root cartificates in this file.
     * NOTE: CC3220 doesn't support the multiple certs pem file and only refers to the first
     *       certificate within the RootCACerts.pem ("digicert_flobal_root_ca").
     *       So, it will require the fallback (currently the recommended fallback
     *       certificate is "digicert_high_assurance_ca.der")
     */
    const char                  *pRootCa1;

    /* Fallback root CA file (path) or NULL (if no fallback is needed) -
     * This will be used in case the primary root CA will fail
     *   with SL_ERROR_BSD_ESEC_ASN_NO_SIGNER_E*/
    const char                  *pRootCa2;
} FileServerParams_t;


typedef struct
{
    /* The tar file full path (in the device file system) */
    char                        *pPath;

    /* a token to enable read access to the tar file */
    uint32_t                    token;

    /* (pointer to) OTA Image version -
     * A NULL pointer means that the version will be based on the timestamp
     * at the beginning the TAR file (first 14 Bytes) - This is the default in the example.
     * If replaced to point to any other user data (length is fixed and
     * defined by OTA_VERSION_LEN), no internal check is done and during the download -
     * the version will be stored to a file (ota.dat) which can be later read using
     *  OTA_IF_getCurrentVersion to compare with new candidate.
     */
    uint8_t                     *pVersion;
} TarFileParams_t;


typedef struct
{
    uint32_t                    errorCode; /* internal error code */
} OtaEventError_t;

typedef union
{
    OtaEventError_t err;
} OtaEventParam_u;

/*****************************************************************************************
 * User Callback Protoypes
 * ***************************************************************************************
 */
/*!

    \brief     Prototype of the user (async) Event handler callback
               (registered by the user in OTA_IF_init(), invoked by the
               OTA_IF thread upon operation completion or failure).

    \param[in] notification - notification type, see otaNotif_e.

    \param[in] pParams - union of notification-specific parameters.

    \return    None

*/
typedef void (*ota_eventCallback_f)(otaNotif_e notification, OtaEventParam_u *pParams);

/*!

    \brief     Prototype of an OTA Cloud Vendor's GetDownloadLink function
               (registered by the OTA_IF_downloadImageByCloudVendor() and called from the
               OTA_IF thread context).

    \param[out] pServerParams - a pointer to the caller allocated structure.
               Upon success the callback will fill the OTA image parameters needed for
               the download.

    \return    0 upon success or a negative error code
*/
typedef int32_t (*ota_getDownloadLink_f)(FileServerParams_t *pServerParams);


/*****************************************************************************************
 * OTA_IF API
 * ***************************************************************************************
 */

/*!

    \brief     OTA Module initialization and user callback registration.

   \param[in] pLocalHttpServerParams - For LOCAL_OTA_SUPPORT, this points to a structure
                 containing the HTTPS server configuration.
                 Use NULL if LOCAL_OTA_SUPPORT is disabled.
                 See details of the structure in HTTPSRV_IF.h
                 Note: The HTTP(S) server is enabled at init supporting the (HTTP) status GET requests.
                 OTA_IF_uploadImage() is needed to enable the "ota" POST request.

   \param[in] fOtaEventHndlr - a user callback (pointer) to be invoked upon OTA
                events (see otaNotif_e).

    \param[in] otaWatchdogTimeout - OTA (commit) watchdog Timeout (in seconds).
    \param[in] hWQ - handle to a Work Queue.
                     If provided - use the caller WQ.
                     If null - create the WQ for the module.

    \return    0 upon success or a negative error code

    \note      Should be called (typically at boot time) before any access
               to other OTA_IF.

*/
int OTA_IF_init(HTTPSRV_IF_params_t *pLocalHttpServerParams, ota_eventCallback_f fOtaEventHndlr, uint32_t otaWatchdogTimeout, workQ_t hWQ);

/*!

    \brief     Get version information of current SW image.

    \param[out] pVersion - a pointer to a user buffer (i.e. space was allocated by the caller)
                           that will store the version.
                           The buffer size should be >= OTA_VERSION_LEN

    \return                 0 upon success or a negative error code

    \note   1. This may be used by the user for downgrade protection.
            2. The version provided by the user will be set (to a volatile memory) during image loading.
               A value of all-zeros indicates the original programming image.
            3. When using the OTA_IF_downloadImageByCloudVendor - the version typically gets checked
               within the OTA vendor (getDoanloadLink) callback.

    \sa     OTA_IF_downloadImageByCloudVendor(), OTA_IF_downloadImageByFileURL(),
            OTA_IF_uploadImage(), OTA_IF_readImage()

*/
int OTA_IF_getCurrentVersion(uint8_t *pVersion);

#if CLOUD_OTA_SUPPORT
/*!

    \brief      Cloud OTA: Download OTA content from a cloud OTA server.
                 This is based on the user providing a callback for retrieving
                 the HTTP download link (the File server URL) from the primary
                 Cloud OTA vendor.
                 A second callback for backup server is optional. This will be used
                 in case of access failure to the primary server.
                 The function returns immediately. The completion of the download
                 (or a failure) will be indicated through the event handler (see
                 OTA_IF_init()).

    \param[in]  fPrimaryVendor - a user callback that connects to the primary OTA vendor
                                 to retrieve the OTA tar file download link.

    \param[in]  fBuckupVendor - a user callback that connects to a backup OTA vendor
                                 to retrieve the OTA tar file download link,
                                 or a NULL when no backup is used.

    \param[in]  flags - download flags (TBD)

    \return     0 upon success or a negative error code

    \note       1. TI provides references of OTA vendors' callbacks as part of the sample application.
                   Those callbacks can be used as is (when using the referred OTA vendors) or can be
                   replaced by a user code.
                2. Currently only a download based on HTTP is supported.
                3. Only TI's OTA TAR format (created by Uniflash or CCS) is supported for the download image.

    \sa    OTA_IF_init(), OTA_IF_downloadImageByFileURL(), OTA_IF_uploadImage(), OTA_IF_readImage()

*/
int OTA_IF_downloadImageByCloudVendor(ota_getDownloadLink_f fPrimaryVendor, ota_getDownloadLink_f fBackupVendor, uint32_t flags);

/*!

    \brief      Cloud OTA: Download OTA content from a cloud file server.
                 When the user already have the URL of an OTA (TAR) image for download,
                 it can use this API to download and install the content.
                 The function returns immediately. The completion of the download
                 (or a failure) will be indicated through the event handler (see
                 OTA_IF_init()).

    \param[in]  pOtaFileParams - OTA (TAR) Image download info (URL, certificate and version).

    \param[in]  flags - bit mask of loading flags, see below:
                    OTA_IF_FLAG_DISABLE_DOWNGRADE_PROTECTION - if set, then version check is ignored.

    \return     0 upon success or a negative error code

    \note       1. Currently only a download based on HTTP is supported.
                2. Only TI's OTA TAR format (created by Uniflash or CCS) is supported for the download image.

    \sa    OTA_IF_downloadImageByCloudVendor(), OTA_IF_uploadImage(), OTA_IF_readImage()

*/
int OTA_IF_downloadImageByFileURL(FileServerParams_t *pOtaFileParams, uint32_t flags);
#endif // CLOUD_OTA_SUPPORT

#if LOCAL_OTA_SUPPORT
/*!

    \brief      Local OTA: Upload image from an external device (HTTP Client) on the local network
                 (using the SimpleLink HTTP Server).
                 The function returns immediately (once the HTTP server is ready for processing the
                 POST "/ota" request).
                 The completion of the upload (or a failure) will be indicated through the event
                 handler (see OTA_IF_init()).

    \param[in]  flags - bit mask of loading flags, see below:
                    OTA_IF_FLAG_DISABLE_DOWNGRADE_PROTECTION - if set, then version check is ignored.

    \return     0 upon success or a negative error code

    \note       1. This can serve as the main OTA update method or as a backup for a cloud OTA
                    (in case of a cloud OTA failure).
                2. Both devices should be on the same local network.
                3. Currently only a download based on HTTP is supported.
                4. Only TI's OTA TAR format (created by Uniflash or CCS) is supported for the upload image.

    \sa         OTA_IF_downloadImageByCloudVendor(), OTA_IF_downloadImageByFileURL(),
                OTA_IF_readImage()

*/
int OTA_IF_uploadImage(uint32_t flags);
#endif // LOCAL_OTA_SUPPORT

#if INTERNAL_UPDATE_SUPPORT
/*!

    \brief      Internal Update: Extract a TAR file (OTA image) from the local file system given
                 the full file path and name.
                 The function returns immediately.
                 The completion of the file extraction (or a failure) will be indicated through
                 the event handler (see OTA_IF_init()).

    \param[in]  pFileParams - OTA (TAR) TAR file parameters (file path and name).

    \param[in]  flags - bit mask of loading flags, see below:
                    OTA_IF_FLAG_DISABLE_DOWNGRADE_PROTECTION - if set, then version check is ignored.

    \return     0 upon success or a negative error code

    \note       1. This method may be used if a non HTTP method will be used to download the TAR file.
                    In such case, the content can be extracted offline (other use-cases are possible)
                2. This can serve as the main OTA update method or as a backup for a cloud OTA
                    (in case of a cloud OTA failure).
                3. Only TI's OTA TAR format (created by Uniflash or CCS) is supported for the upload image.

    \sa         OTA_IF_downloadImageByCloudVendor(), OTA_IF_downloadImageByFileURL(),
                OTA_IF_readImage()

*/
int OTA_IF_readImage(TarFileParams_t *pFileParams, uint32_t flags);
#endif // INTERNAL_UPDATE_SUPPORT

/*!

    \brief      Installing the OTA update. This should be called by the user specifically
                 after the image is already loaded and extracted  (as indicated by the event
                 handler) as it will involve MCU reset.
                 (the loading itself can be done in parallel to running the operational code).

    \param[in]  None.

    \return     0 upon success or a negative error code

    \sa         OTA_IF_downloadImageByCloudVendor(), OTA_IF_downloadImageByFileURL(),
                OTA_IF_readImage()

*/
int OTA_IF_install();

/*!

    \brief      Committing the new (updated) image (making a test image functional).
                 This is typically done after paerforming sanity check for the image (e.g. pinging
                 cloud server) once the Pending Commit nofication is received (by the
                 event handler)

    \param[in]  None.

    \return     0 upon success or a negative error code

    \sa         OTA_IF_downloadImageByCloudVendor(), OTA_IF_downloadImageByFileURL(),
                OTA_IF_readImage(), OTA_IF_rollback()

*/
int OTA_IF_commit();

/*!

    \brief      Request to revert to the previous image (typically upon failure in
                new image testing)

    \param[in]  None.

    \return     0 upon success or a negative error code

    \sa         OTA_IF_downloadImageByCloudVendor(), OTA_IF_downloadImageByFileURL(),
                OTA_IF_readImage(), OTA_IF_commit()

*/
int OTA_IF_rollback();

/*!

    \brief      Compare new candidate version (timestamp string based) to the existing version.

    \param[in]  Version string (14 bytes timestamp)

    \return     TRUE if the candidate version is newer than the existing version (FALSE if not newer).

    \note       This is just an example and can be replaced with different comparison method.

*/
bool OTA_IF_isNewVersion(uint8_t *pCandidateVersion);

#endif

#endif // OTA_IF_H
