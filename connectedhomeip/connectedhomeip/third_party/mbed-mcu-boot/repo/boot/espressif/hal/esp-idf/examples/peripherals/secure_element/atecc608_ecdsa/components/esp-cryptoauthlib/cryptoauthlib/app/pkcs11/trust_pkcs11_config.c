/**
 * \file
 * \brief PKCS11 Trust Platform Configuration
 *
 * \copyright (c) 2015-2020 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#include "cryptoauthlib.h"
#include "pkcs11_config.h"
#include "pkcs11/pkcs11_object.h"
#include "pkcs11/pkcs11_slot.h"

#if defined(ATCA_TNGTLS_SUPPORT) || defined(ATCA_TNGLORA_SUPPORT) || defined(ATCA_TFLEX_SUPPORT)

#include "tng_root_cert.h"


const char pkcs11_trust_device_label[] = "device";
const char pkcs11_trust_signer_label[] = "signer";
const char pkcs11_trust_root_label[] = "root";
const char pkcs11_trust_device_private_key_label[] = "device private";
const char pkcs11_trust_device_public_key_label[] = "device public";

/* Helper function to assign the proper fields to an certificate object from a cert def */
CK_RV pkcs11_trust_config_cert(pkcs11_lib_ctx_ptr pLibCtx, pkcs11_slot_ctx_ptr pSlot, pkcs11_object_ptr pObject, CK_ATTRIBUTE_PTR pLabel)
{
    CK_RV rv = CKR_OK;

    (void)pLibCtx;
    (void)pSlot;

    if (!pObject || !pLabel)
    {
        return CKR_ARGUMENTS_BAD;
    }

    if (pLabel->ulValueLen >= PKCS11_MAX_LABEL_SIZE)
    {
        return CKR_ARGUMENTS_BAD;
    }

    if (!strncmp(pkcs11_trust_device_label, (char*)pLabel->pValue, pLabel->ulValueLen))
    {
        /* Slot 10 - Device Cert for Slot 0*/
        pkcs11_config_init_cert(pObject, pLabel->pValue, pLabel->ulValueLen);
        pObject->slot = 10;
        pObject->flags |= PKCS11_OBJECT_FLAG_TRUST_TYPE;
        pObject->class_type = CK_CERTIFICATE_CATEGORY_TOKEN_USER;

    }
    else if (!strncmp(pkcs11_trust_signer_label, (char*)pLabel->pValue, pLabel->ulValueLen))
    {
        /* Slot 12 - Signer Cert for Slot 10 */
        pkcs11_config_init_cert(pObject, pLabel->pValue, pLabel->ulValueLen);
        pObject->slot = 12;
        pObject->flags |= PKCS11_OBJECT_FLAG_TRUST_TYPE;
        pObject->class_type = CK_CERTIFICATE_CATEGORY_AUTHORITY;
    }
    else if (!strncmp(pkcs11_trust_root_label, (char*)pLabel->pValue, pLabel->ulValueLen))
    {
        /* Slot 12 - Signer Cert for Slot 10 */
        pkcs11_config_init_cert(pObject, pLabel->pValue, pLabel->ulValueLen);
        pObject->slot = 0xFFFF;
        pObject->flags |= PKCS11_OBJECT_FLAG_TRUST_TYPE;
        pObject->data = (CK_VOID_PTR)&g_cryptoauth_root_ca_002_cert;
        pObject->size = (CK_ULONG)g_cryptoauth_root_ca_002_cert_size;
        pObject->class_type = CK_CERTIFICATE_CATEGORY_AUTHORITY;
    }
    else
    {
        rv = CKR_ARGUMENTS_BAD;
    }

#if ATCA_CA_SUPPORT
    if (CKR_OK == rv)
    {
        pObject->config = &pSlot->cfg_zone;
    }
#endif


    return rv;
}

/* Helper function to assign the proper fields to a key object */
CK_RV pkcs11_trust_config_key(pkcs11_lib_ctx_ptr pLibCtx, pkcs11_slot_ctx_ptr pSlot, pkcs11_object_ptr pObject, CK_ATTRIBUTE_PTR pLabel)
{
    CK_RV rv = CKR_OK;

    (void)pLibCtx;

    if (!pObject || !pLabel || !pSlot)
    {
        return CKR_ARGUMENTS_BAD;
    }

    if (pLabel->ulValueLen >= PKCS11_MAX_LABEL_SIZE)
    {
        return CKR_ARGUMENTS_BAD;
    }

    if (!strncmp(pkcs11_trust_device_private_key_label, (char*)pLabel->pValue, pLabel->ulValueLen))
    {
        /* slot 0 - Device Private Key */
        pkcs11_config_init_private(pObject, pLabel->pValue, pLabel->ulValueLen);
        pObject->slot = 0;
        pObject->flags |= PKCS11_OBJECT_FLAG_TRUST_TYPE;
        pObject->config = &pSlot->cfg_zone;
    }
    else if (!strncmp(pkcs11_trust_device_public_key_label, (char*)pLabel->pValue, pLabel->ulValueLen))
    {
        /* slot 0 - Device Public Key */
        pkcs11_config_init_public(pObject, pLabel->pValue, pLabel->ulValueLen);
        pObject->slot = 0;
        pObject->flags |= PKCS11_OBJECT_FLAG_TRUST_TYPE;
        pObject->config = &pSlot->cfg_zone;
    }
    else
    {
        rv = CKR_ARGUMENTS_BAD;
    }

    return rv;
}

CK_RV pkcs11_trust_load_objects(pkcs11_slot_ctx_ptr pSlot)
{
    pkcs11_object_ptr pObject;
    CK_RV rv = CKR_OK;
    CK_ATTRIBUTE xLabel;

    if (CKR_OK == rv)
    {
        rv = pkcs11_object_alloc(&pObject);
        if (pObject)
        {
            /* Slot 0 - Device Private Key */
            pkcs11_config_init_private(pObject, (char*)pkcs11_trust_device_label, strlen(pkcs11_trust_device_label));
            pObject->slot = 0;
            pObject->flags |= PKCS11_OBJECT_FLAG_TRUST_TYPE;
            pObject->config = &pSlot->cfg_zone;
        }
    }

    if (CKR_OK == rv)
    {
        rv = pkcs11_object_alloc(&pObject);
        if (pObject)
        {
            /* Slot 0 - Device Public Key */
            pkcs11_config_init_public(pObject, (char*)pkcs11_trust_device_label, strlen(pkcs11_trust_device_label));
            pObject->slot = 0;
            pObject->flags |= PKCS11_OBJECT_FLAG_TRUST_TYPE;
            pObject->config = &pSlot->cfg_zone;
        }
    }

    if (CKR_OK == rv)
    {
        rv = pkcs11_object_alloc(&pObject);
        if (pObject)
        {
            /* Device Certificate */
            xLabel.pValue = (CK_VOID_PTR)pkcs11_trust_device_label;
            xLabel.ulValueLen = (CK_ULONG)strlen(xLabel.pValue);
            xLabel.type = CKA_LABEL;
            pkcs11_trust_config_cert(NULL, pSlot, pObject, &xLabel);
        }
    }

    if (CKR_OK == rv)
    {
        rv = pkcs11_object_alloc(&pObject);
        if (pObject)
        {
            /* Signer Certificate */
            xLabel.pValue = (CK_VOID_PTR)pkcs11_trust_signer_label;
            xLabel.ulValueLen = (CK_ULONG)strlen(xLabel.pValue);
            xLabel.type = CKA_LABEL;
            pkcs11_trust_config_cert(NULL, pSlot, pObject, &xLabel);
        }
    }

    return rv;
}

#if PKCS11_USE_STATIC_CONFIG
CK_RV pkcs11_config_cert(pkcs11_lib_ctx_ptr pLibCtx, pkcs11_slot_ctx_ptr pSlot, pkcs11_object_ptr pObject, CK_ATTRIBUTE_PTR pLabel)
{
    return pkcs11_trust_config_cert(pLibCtx, pSlot, pObject, pLabel);
}

CK_RV pkcs11_config_key(pkcs11_lib_ctx_ptr pLibCtx, pkcs11_slot_ctx_ptr pSlot, pkcs11_object_ptr pObject, CK_ATTRIBUTE_PTR pLabel)
{
    return pkcs11_trust_config_key(pLibCtx, pSlot, pObject, pLabel);
}

CK_RV pkcs11_config_load_objects(pkcs11_slot_ctx_ptr pSlot)
{
    return pkcs11_trust_load_objects(pSlot);
}
#endif

#endif
