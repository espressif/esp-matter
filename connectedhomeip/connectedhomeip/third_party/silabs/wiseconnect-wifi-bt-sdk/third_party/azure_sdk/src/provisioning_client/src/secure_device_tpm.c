// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include <stdbool.h>
#include "umock_c/umock_c_prod.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/azure_base64.h"
#include "azure_c_shared_utility/sha.h"
#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/buffer_.h"

#include "azure_hub_modules/secure_device_factory.h"
#include "azure_hub_modules/secure_device_tpm.h"

#if 0
#include "azure_utpm_c/tpm_comm.h"
#include "azure_utpm_c/tpm_codec.h"

#include "azure_utpm_c/Marshal_fp.h"     // for activation blob unmarshaling

#define EPOCH_TIME_T_VALUE          0
#define HMAC_LENGTH                 32

static TPM2B_AUTH      NullAuth = { 0 };
static TSS_SESSION     NullPwSession;
static const UINT32 TPM_20_SRK_HANDLE = HR_PERSISTENT | 0x00000001;
static const UINT32 TPM_20_EK_HANDLE = HR_PERSISTENT | 0x00010001;
static const UINT32 DRS_ID_KEY_HANDLE = HR_PERSISTENT | 0x00000100;

typedef struct SEC_DEVICE_INFO_TAG
{
    TPM_HANDLE tpm_handle;
    TSS_DEVICE tpm_device;
    TPM2B_PUBLIC ek_pub;
    TPM2B_PUBLIC srk_pub;

    TPM2B_PUBLIC id_key_public;
    TPM2B_PRIVATE id_key_dup_blob;
    TPM2B_PRIVATE id_key_priv;
} SEC_DEVICE_INFO;

static const SEC_TPM_INTERFACE sec_tpm_interface =
{
    secure_dev_tpm_create,
    secure_dev_tpm_destroy,
    secure_dev_tpm_import_key,
    secure_dev_tpm_get_endorsement_key,
    secure_dev_tpm_get_storage_key,
    secure_dev_tpm_sign_data,
    secure_dev_tpm_decrypt_data
};

static TPMS_RSA_PARMS  RsaStorageParams = {
    { TPM_ALG_AES, 128, TPM_ALG_CFB },      // TPMT_SYM_DEF_OBJECT  symmetric
    { TPM_ALG_NULL },                       // TPMT_RSA_SCHEME      scheme
    2048,                                   // TPMI_RSA_KEY_BITS    keyBits
    0                                       // UINT32               exponent
};

static TPM2B_PUBLIC* GetEkTemplate ()
{
    static TPM2B_PUBLIC EkTemplate = { 0,   // size will be computed during marshaling
    {
        TPM_ALG_RSA,                    // TPMI_ALG_PUBLIC      type
        TPM_ALG_SHA256,                 // TPMI_ALG_HASH        nameAlg
        { 0 },                          // TPMA_OBJECT  objectAttributes (set below)
        {32,
        { 0x83, 0x71, 0x97, 0x67, 0x44, 0x84, 0xb3, 0xf8,
        0x1a, 0x90, 0xcc, 0x8d, 0x46, 0xa5, 0xd7, 0x24,
        0xfd, 0x52, 0xd7, 0x6e, 0x06, 0x52, 0x0b, 0x64,
        0xf2, 0xa1, 0xda, 0x1b, 0x33, 0x14, 0x69, 0xaa }
        },                              // TPM2B_DIGEST         authPolicy
        { 0 },                          // TPMU_PUBLIC_PARMS    parameters (set below)
        { 0 }                           // TPMU_PUBLIC_ID       unique
    } };
    EkTemplate.publicArea.objectAttributes = ToTpmaObject(
        Restricted | Decrypt | FixedTPM | FixedParent | AdminWithPolicy | SensitiveDataOrigin);
    EkTemplate.publicArea.parameters.rsaDetail = RsaStorageParams;
    return &EkTemplate;
}

static TPM2B_PUBLIC* GetSrkTemplate()
{
    static TPM2B_PUBLIC SrkTemplate = { 0,  // size will be computed during marshaling
    {
        TPM_ALG_RSA,                // TPMI_ALG_PUBLIC      type
        TPM_ALG_SHA256,             // TPMI_ALG_HASH        nameAlg
        { 0 },                      // TPMA_OBJECT  objectAttributes (set below)
        { 0 },                      // TPM2B_DIGEST         authPolicy
        { 0 },                      // TPMU_PUBLIC_PARMS    parameters (set before use)
        { 0 }                       // TPMU_PUBLIC_ID       unique
    } };
    SrkTemplate.publicArea.objectAttributes = ToTpmaObject(
        Restricted | Decrypt | FixedTPM | FixedParent | NoDA | UserWithAuth | SensitiveDataOrigin);
    SrkTemplate.publicArea.parameters.rsaDetail = RsaStorageParams;
    return &SrkTemplate;
}

#define DRS_UNMARSHAL(Type, pValue) \
{                                                                       \
    TPM_RC rc = Type##_Unmarshal(pValue, &curr_pos, (INT32*)&act_size);         \
    if (rc != TPM_RC_SUCCESS)                                           \
    {                                                                   \
        LogError(#Type"_Unmarshal() for " #pValue " failed");           \
    }                                                                   \
}

#define DRS_UNMARSHAL_FLAGGED(Type, pValue) \
{                                                                       \
    TPM_RC rc = Type##_Unmarshal(pValue, &curr_pos, (INT32*)&act_size, TRUE);   \
    if (rc != TPM_RC_SUCCESS)                                           \
    {                                                                   \
        LogError(#Type"_Unmarshal() for " #pValue " failed");           \
    }                                                                   \
}

#define DRS_UNMARSHAL_ARRAY(dstPtr, arrSize) \
    DRS_UNMARSHAL(UINT32, &(arrSize));                                          \
    printf("act_size %d < actSize %d\r\n", act_size, arrSize);   \
    if (act_size < arrSize)                                                     \
    {                                                                           \
        LogError("Unmarshaling " #dstPtr " failed: Need %d bytes, while only %d left", arrSize, act_size);  \
        result = MU_FAILURE;       \
    }                                                                           \
    else                            \
    {                                   \
        dstPtr = curr_pos - sizeof(UINT16);                                         \
        *(UINT16*)dstPtr = (UINT16)arrSize;                                         \
        curr_pos += arrSize;                         \
    }

static int dps_umarshal_array(unsigned char* dst_ptr, uint32_t dest_size, unsigned char* act_buff, uint32_t act_size)
{
    int result;
    uint8_t* curr_pos = act_buff;

    DRS_UNMARSHAL(UINT32, &(dest_size));
    if (act_size < dest_size)
    {
        LogError("Unmarshaling failed: Need %d bytes, while only %d left", dest_size, act_size);
        result = __LINE__;
    }
    else
    {
        dst_ptr = act_buff - sizeof(UINT16);
        *(UINT16*)dst_ptr = (UINT16)dest_size;
        act_buff += dest_size;
        result = 0;
    }
    return result;
}

static int unmarshal_array(uint8_t* dstptr, uint32_t size, uint8_t** curr_pos, uint32_t* curr_size)
{
    int result;

    TPM_RC tpm_res = UINT32_Unmarshal((uint32_t*)dstptr, curr_pos, (int32_t*)curr_size);
    if (tpm_res != TPM_RC_SUCCESS)
    {
        LogError("Failure: unmarshalling array.");
        result = MU_FAILURE;
    }
    else if (*curr_size < size)
    {
        LogError("Failure: unmarshalling array need %d bytes, while only %d left.", size, *curr_size);
        result = MU_FAILURE;
    }
    else
    {
        dstptr = *curr_pos - sizeof(UINT16);
        *(UINT16*)dstptr = (UINT16)size;
        curr_pos += size;
        result = 0;
    }
    return result;
}

static int marshal_array_values(const unsigned char* key, size_t key_len, uint8_t** decrypt_blob, uint8_t** decrypt_secret, uint8_t** decrypt_wrap_key, TPM2B_PRIVATE* enc_key_blob)
{
    int result = 0;
    uint8_t* curr_pos = (uint8_t*)key;
    uint32_t act_size = (int32_t)key_len;
    uint32_t decrypt_size = 0;
    uint32_t decrypt_secret_size = 0;
    uint32_t decrypt_key_size = 0;
    TPM2B_PUBLIC id_key_Public = { TPM_ALG_NULL };
    UINT16 gratuitousSizeField;    // WORKAROUND for the current protocol

    DRS_UNMARSHAL_ARRAY(*decrypt_blob, decrypt_size);
    if (result != 0)
    {
        LogError("Failure: decrypting blob");
    }
    else
    {
        DRS_UNMARSHAL_ARRAY(*decrypt_secret, decrypt_secret_size);
        if (result != 0)
        {
            LogError("Failure: decrypting secret");
        }
        else
        {
            DRS_UNMARSHAL_ARRAY(*decrypt_wrap_key, decrypt_key_size);
            if (result != 0)
            {
                LogError("Failure: decrypting wrap secret");
            }
            else
            {
                DRS_UNMARSHAL_FLAGGED(TPM2B_PUBLIC, &id_key_Public);
                if (result != 0)
                {
                    LogError("Failure: id key public");
                }
                else
                {
                    DRS_UNMARSHAL(UINT16, &gratuitousSizeField);
                    if (result != 0)
                    {
                        LogError("Failure: gratuitousSizeField");
                    }
                    else
                    {
                        DRS_UNMARSHAL(TPM2B_PRIVATE, enc_key_blob);
                        if (result != 0)
                        {
                            LogError("Failure: enc key blob");
                        }
                    }
                }
            }
        }
    }
    return result;
}

static int create_tpm_session(SEC_DEVICE_INFO* sec_info, TSS_SESSION* tpm_session)
{
    int result;
    TPMA_SESSION sess_attrib = { 1 };
    if (TSS_StartAuthSession(&sec_info->tpm_device, TPM_SE_POLICY, TPM_ALG_SHA256, sess_attrib, tpm_session) != TPM_RC_SUCCESS)
    {
        LogError("Failure: Starting EK policy session");
        result = MU_FAILURE;
    }
    else if (TSS_PolicySecret(&sec_info->tpm_device, &NullPwSession, TPM_RH_ENDORSEMENT, tpm_session, NULL, 0) != TPM_RC_SUCCESS)
    {
        LogError("Failure: PolicySecret() for EK");
        result = MU_FAILURE;
    }
    else
    {
        result = 0;
    }
    return result;
}

static int insert_key_in_tpm(SEC_DEVICE_INFO* sec_info, const unsigned char* key, size_t key_len)
{
    int result;
    TSS_SESSION ek_sess = { { TPM_RH_NULL } };
    if (create_tpm_session(sec_info, &ek_sess) != 0)
    {
        LogError("Failure: Starting EK policy session");
        result = MU_FAILURE;
    }
    else
    {
        TPMT_SYM_DEF_OBJECT Aes128SymDef = { TPM_ALG_AES, 128, TPM_ALG_CFB };
        TPM2B_ID_OBJECT enc_key_blob;
        TPM2B_ENCRYPTED_SECRET tpm_enc_secret;
        TPM2B_PRIVATE id_key_dup_blob;
        TPM2B_ENCRYPTED_SECRET encrypt_wrap_key;
        TPM2B_PUBLIC id_key_Public = { TPM_ALG_NULL };
        UINT16 enc_data_size = 0;
        TPM2B_DIGEST inner_wrap_key = { 0 };
        TPM2B_PRIVATE id_key_priv;
        TPM_HANDLE load_id_key = TPM_ALG_NULL;

        uint8_t* curr_pos = (uint8_t*)key;
        uint32_t act_size = (int32_t)key_len;

        DRS_UNMARSHAL(TPM2B_ID_OBJECT, &enc_key_blob);
        DRS_UNMARSHAL(TPM2B_ENCRYPTED_SECRET, &tpm_enc_secret);
        DRS_UNMARSHAL(TPM2B_PRIVATE, &id_key_dup_blob);
        DRS_UNMARSHAL(TPM2B_ENCRYPTED_SECRET, &encrypt_wrap_key);
        DRS_UNMARSHAL_FLAGGED(TPM2B_PUBLIC, &id_key_Public);

        // The given TPM may support larger TPM2B_MAX_BUFFER than this API headers define.
        // So instead of unmarshaling data in a standalone data structure just reuse the
        // original activation buffer (after updating byte order of the UINT16 counter)
        DRS_UNMARSHAL(UINT16, &enc_data_size);

        if (TPM2_ActivateCredential(&sec_info->tpm_device, &NullPwSession, &ek_sess, TPM_20_SRK_HANDLE, TPM_20_EK_HANDLE,
            &enc_key_blob, &tpm_enc_secret, &inner_wrap_key) != TPM_RC_SUCCESS)
        {
            LogError("Failure: TPM2_ActivateCredential");
            result = MU_FAILURE;
        }
        else if (TPM2_Import(&sec_info->tpm_device, &NullPwSession, TPM_20_SRK_HANDLE, (TPM2B_DATA*)&inner_wrap_key, &id_key_Public, &id_key_dup_blob, &encrypt_wrap_key, &Aes128SymDef, &id_key_priv) != TPM_RC_SUCCESS)
        {
            LogError("Failure: importing dps Id key");
            result = MU_FAILURE;
        }
        else
        {
            TPM2B_SENSITIVE_CREATE sen_create = { 0 };
            TPM2B_PUBLIC sym_pub = { 0 };
            TPM2B_PRIVATE sym_priv = { 0 };

            static TPM2B_PUBLIC symTemplate = { 0,   // size will be computed during marshaling
            {
                TPM_ALG_SYMCIPHER,              // TPMI_ALG_PUBLIC      type
                TPM_ALG_SHA256,                 // TPMI_ALG_HASH        nameAlg
                { 0 },                          // TPMA_OBJECT  objectAttributes (set below)
                { 0 },                          // TPM2B_DIGEST         authPolicy
                { 0 },                          // TPMU_PUBLIC_PARMS    parameters (set below)
                { 0 }                           // TPMU_PUBLIC_ID       unique
            } };
            symTemplate.publicArea.objectAttributes = ToTpmaObject(Decrypt | FixedTPM | FixedParent | UserWithAuth);
            symTemplate.publicArea.parameters.symDetail.sym.algorithm = TPM_ALG_AES;
            symTemplate.publicArea.parameters.symDetail.sym.keyBits.sym = inner_wrap_key.t.size * 8;
            symTemplate.publicArea.parameters.symDetail.sym.mode.sym = TPM_ALG_CFB;

            memcpy(sen_create.sensitive.data.t.buffer, inner_wrap_key.t.buffer, inner_wrap_key.t.size);
            sen_create.sensitive.data.t.size = inner_wrap_key.t.size;

            if (TSS_Create(&sec_info->tpm_device, &NullPwSession, TPM_20_SRK_HANDLE, &sen_create, &symTemplate, &sym_priv, &sym_pub) != TPM_RC_SUCCESS)
            {
                LogError("Failed to inject symmetric key data");
                result = MU_FAILURE;
            }
            else if (TPM2_Load(&sec_info->tpm_device, &NullPwSession, TPM_20_SRK_HANDLE, &id_key_priv, &id_key_Public, &load_id_key, NULL) != TPM_RC_SUCCESS)
            {
                LogError("Failed Load Id key.");
                result = MU_FAILURE;
            }
            else
            {
                // Remove old Id key
                (void)TPM2_EvictControl(&sec_info->tpm_device, &NullPwSession, TPM_RH_OWNER, DRS_ID_KEY_HANDLE, DRS_ID_KEY_HANDLE);

                if (TPM2_EvictControl(&sec_info->tpm_device, &NullPwSession, TPM_RH_OWNER, load_id_key, DRS_ID_KEY_HANDLE) != TPM_RC_SUCCESS)
                {
                    LogError("Failed Load Id key.");
                    result = MU_FAILURE;
                }
                else if (TPM2_FlushContext(&sec_info->tpm_device, load_id_key) != TPM_RC_SUCCESS)
                {
                    LogError("Failed Load Id key.");
                    result = MU_FAILURE;
                }
                else
                {
                    result = 0;
                }
            }
        }
    }
    return result;
}

static BUFFER_HANDLE decrypt_data(SEC_DEVICE_INFO* sec_info, const unsigned char* key, size_t key_len)
{
    BUFFER_HANDLE result;
    TSS_SESSION ek_sess = { { TPM_RH_NULL } };
    UINT32 max_tpm_data_size;

    if (create_tpm_session(sec_info, &ek_sess) != 0)
    {
        LogError("Failure: Starting EK policy session");
        result = NULL;
    }
    else if ((max_tpm_data_size = TSS_GetTpmProperty(&sec_info->tpm_device, TPM_PT_INPUT_BUFFER)) == 0)
    {
        LogError("Failure: TSS_GetTpmProperty Input buffer");
        result = NULL;
    }
    else
    {
        TPM2B_ID_OBJECT tpm_blob;
        TPM2B_ENCRYPTED_SECRET tpm_enc_secret;
        TPM2B_ENCRYPTED_SECRET tpm_enc_key;
        TPM2B_PRIVATE id_key_dup_blob;
        UINT16 enc_data_size = 0;
        TPM2B_PUBLIC id_key_Public = { TPM_ALG_NULL };
        TPM2B_MAX_BUFFER* target_data = NULL;
        TPM2B_DIGEST inner_wrap_key = { 0 };

        uint8_t* curr_pos = (uint8_t*)key;
        uint32_t act_size = (int32_t)key_len;

        DRS_UNMARSHAL(TPM2B_ID_OBJECT, &tpm_blob);
        DRS_UNMARSHAL(TPM2B_ENCRYPTED_SECRET, &tpm_enc_secret);
        DRS_UNMARSHAL(TPM2B_PRIVATE, &id_key_dup_blob);
        DRS_UNMARSHAL(TPM2B_ENCRYPTED_SECRET, &tpm_enc_key);
        DRS_UNMARSHAL_FLAGGED(TPM2B_PUBLIC, &id_key_Public);

        // The given TPM may support larger TPM2B_MAX_BUFFER than this API headers define.
        // So instead of unmarshaling data in a standalone data structure just reuse the
        // original activation buffer (after updating byte order of the UINT16 counter)
        DRS_UNMARSHAL(UINT16, &enc_data_size);

        if (enc_data_size > max_tpm_data_size)
        {
            LogError("Failure: The encrypted data len (%zu) is too long for tpm", enc_data_size);
            result = NULL;
        }
        else
        {
            target_data = (TPM2B_MAX_BUFFER*)(curr_pos - sizeof(UINT16));
            target_data->t.size = enc_data_size;

            // Update local vars in case activation blob contains more data to unmarshal
            curr_pos += enc_data_size;
            act_size -= enc_data_size;

            // decrypts encrypted symmetric key ?encSecret? and returns it as 'tpm_blob'.
            // Later 'tpm_blob' is used as the inner wrapper key for import of the HMAC key blob.
            if (TPM2_ActivateCredential(&sec_info->tpm_device, &NullPwSession, &ek_sess, TPM_20_SRK_HANDLE, TPM_20_EK_HANDLE, &tpm_blob, &tpm_enc_secret, &inner_wrap_key) != TPM_RC_SUCCESS)
            {
                LogError("Failure: TPM2_ActivateCredential");
                result = NULL;
            }
            else
            {
                result = BUFFER_create(tpm_blob.t.credential, tpm_blob.t.size);
                if (result == NULL)
                {
                    LogError("Failure building buffer");
                    result = NULL;
                }
            }
        }
    }
    return result;
}

static int create_persistent_key(SEC_DEVICE_INFO* tpm_info, TPM_HANDLE request_handle, TPMI_DH_OBJECT hierarchy, TPM2B_PUBLIC* inPub, TPM2B_PUBLIC* outPub)
{
    int result;
    TPM_RC tpm_result;
    TPM2B_NAME name;
    TPM2B_NAME qName;

    tpm_result = TPM2_ReadPublic(&tpm_info->tpm_device, request_handle, outPub, &name, &qName);
    if (tpm_result == TPM_RC_SUCCESS)
    {
        tpm_info->tpm_handle = request_handle;
        result = 0;
    }
    else if (tpm_result != TPM_RC_HANDLE)
    {
        LogError("Failed calling TPM2_ReadPublic 0%x", tpm_result);
        result = MU_FAILURE;
    }
    else
    {
        if (TSS_CreatePrimary(&tpm_info->tpm_device, &NullPwSession, hierarchy, inPub, &tpm_info->tpm_handle, outPub) != TPM_RC_SUCCESS)
        {
            LogError("Failed calling TSS_CreatePrimary");
            result = MU_FAILURE;
        }
        else
        {
            if (TPM2_EvictControl(&tpm_info->tpm_device, &NullPwSession, TPM_RH_OWNER, tpm_info->tpm_handle, request_handle) != TPM_RC_SUCCESS)
            {
                LogError("Failed calling TSS_CreatePrimary");
                result = MU_FAILURE;
            }
            else if (TPM2_FlushContext(&tpm_info->tpm_device, tpm_info->tpm_handle) != TPM_RC_SUCCESS)
            {
                LogError("Failed calling TSS_CreatePrimary");
                result = MU_FAILURE;
            }
            else
            {
                tpm_info->tpm_handle = request_handle;
                result = 0;
            }
        }
    }
    return result;
}

static int initialize_tpm_device(SEC_DEVICE_INFO* tpm_info)
{
    int result;
    if (TSS_CreatePwAuthSession(&NullAuth, &NullPwSession) != TPM_RC_SUCCESS)
    {
        LogError("Failure calling TSS_CreatePwAuthSession");
        result = MU_FAILURE;
    }
    else if (Initialize_TPM_Codec(&tpm_info->tpm_device) != TPM_RC_SUCCESS)
    {
        LogError("Failure initializeing TPM Codec");
        result = MU_FAILURE;
    }
    /* Codes_secure_dev_tpm_create shall get a handle to the Endorsement Key and Storage Root Key.*/
    else if (create_persistent_key(tpm_info, TPM_20_EK_HANDLE, TPM_RH_ENDORSEMENT, GetEkTemplate(), &tpm_info->ek_pub) != 0)
    {
        LogError("Failure calling creating persistent key for Endorsement key");
        result = MU_FAILURE;
    }
    /* Codes_secure_dev_tpm_create shall get a handle to the Endorsement Key and Storage Root Key.*/
    else if (create_persistent_key(tpm_info, TPM_20_SRK_HANDLE, TPM_RH_OWNER, GetSrkTemplate(), &tpm_info->srk_pub) != 0)
    {
        LogError("Failure calling creating persistent key for Storage Root key");
        result = MU_FAILURE;
    }
    else
    {
        result = 0;
    }
    return result;
}

SEC_DEVICE_HANDLE secure_dev_tpm_create()
{
    SEC_DEVICE_INFO* result;
    /* Codes_SRS_SECURE_DEVICE_TPM_07_002: [ On success secure_dev_tpm_create shall allocate a new instance of the secure device tpm interface. ] */
    result = malloc(sizeof(SEC_DEVICE_INFO) );
    if (result == NULL)
    {
        /* Codes_SRS_SECURE_DEVICE_TPM_07_001: [ If any failure is encountered secure_dev_tpm_create shall return NULL ] */
        LogError("Failure: malloc SEC_DEVICE_INFO.");
    }
    else
    {
        memset(result, 0, sizeof(SEC_DEVICE_INFO));
        /* Codes_secure_dev_tpm_create shall call into the tpm_codec to initialize a TSS session.*/
        if (initialize_tpm_device(result) != 0)
        {
            /* Codes_SRS_SECURE_DEVICE_TPM_07_001: [ If any failure is encountered secure_dev_tpm_create shall return NULL ] */
            LogError("Failure initializing tpm device.");
            free(result);
            result = NULL;
        }
    }
    return (SEC_DEVICE_HANDLE)result;
}

void secure_dev_tpm_destroy(SEC_DEVICE_HANDLE handle)
{
    /* Codes_SRS_SECURE_DEVICE_TPM_07_005: [ if handle is NULL, secure_dev_tpm_destroy shall do nothing. ] */
    if (handle != NULL)
    {
        /* Codes_SRS_SECURE_DEVICE_TPM_07_004: [ secure_dev_tpm_destroy shall free the SEC_DEVICE_INFO instance. ] */
        Deinit_TPM_Codec(&handle->tpm_device);
        /* Codes_SRS_SECURE_DEVICE_TPM_07_006: [ secure_dev_tpm_destroy shall free all resources allocated in this module. ]*/
        free(handle);
    }
}

int secure_dev_tpm_import_key(SEC_DEVICE_HANDLE handle, const unsigned char* key, size_t key_len)
{
    int result;
    if (handle == NULL || key == NULL || key_len == 0)
    {
        /* Codes_SRS_SECURE_DEVICE_TPM_07_007: [ if handle or key are NULL, or key_len is 0 secure_dev_tpm_import_key shall return a non-zero value ] */
        LogError("Invalid argument specified handle: %p, key: %p, key_len: %zu", handle, key, key_len);
        result = MU_FAILURE;
    }
    else
    {
        /* Codes_secure_dev_tpm_import_key shall establish a tpm session in preparation to inserting the key into the tpm. */
        if (insert_key_in_tpm(handle, key, key_len))
        {
            LogError("Failure inserting key into tpm");
            result = MU_FAILURE;
        }
        else
        {
            /* Codes_SRS_SECURE_DEVICE_TPM_07_008: [ On success secure_dev_tpm_import_key shall return zero ] */
            result = 0;
        }
    }
    return result;
}

char* secure_dev_tpm_get_endorsement_key(SEC_DEVICE_HANDLE handle)
{
    char* result;
    if (handle == NULL)
    {
        /* Codes_SRS_SECURE_DEVICE_TPM_07_013: [ If handle is NULL secure_dev_tpm_get_endorsement_key shall return NULL. ] */
        LogError("Invalid handle value specified");
        result = NULL;
    }
    else if (handle->ek_pub.publicArea.unique.rsa.t.size == 0)
    {
        /* Codes_SRS_SECURE_DEVICE_TPM_07_027: [ If the ek_public was not initialized secure_dev_tpm_get_endorsement_key shall return NULL. ] */
        LogError("Endorsement key is invalid");
        result = NULL;
    }
    else
    {
        unsigned char data_bytes[1024];
        unsigned char* data_pos = data_bytes;
        /* Codes_SRS_SECURE_DEVICE_TPM_07_014: [ secure_dev_tpm_get_endorsement_key shall allocate and return the Endorsement Key. ] */
        uint32_t data_length = TPM2B_PUBLIC_Marshal(&handle->ek_pub, &data_pos, NULL);
        STRING_HANDLE encoded_ek = Azure_Base64_Encode_Bytes(data_bytes, data_length);
        if (encoded_ek == NULL)
        {
            /* Codes_SRS_SECURE_DEVICE_TPM_07_015: [ If a failure is encountered, secure_dev_tpm_get_endorsement_key shall return NULL. ] */
            LogError("Failure encoding endorsement key");
            result = NULL;
        }
        else
        {
            if (mallocAndStrcpy_s(&result, STRING_c_str(encoded_ek)) != 0)
            {
                /* Codes_SRS_SECURE_DEVICE_TPM_07_015: [ If a failure is encountered, secure_dev_tpm_get_endorsement_key shall return NULL. ] */
                LogError("Failure allocating endorsement key");
                result = NULL;
            }
            STRING_delete(encoded_ek);
        }
    }
    return result;
}

char* secure_dev_tpm_get_storage_key(SEC_DEVICE_HANDLE handle)
{
    char* result;
    if (handle == NULL)
    {
        /* Codes_SRS_SECURE_DEVICE_TPM_07_016: [ If handle is NULL, secure_dev_tpm_get_storage_key shall return NULL. ] */
        LogError("Invalid handle value specified");
        result = NULL;
    }
    else if (handle->srk_pub.publicArea.unique.rsa.t.size == 0)
    {
        /* Codes_SRS_SECURE_DEVICE_TPM_07_017: [ If the srk_public value was not initialized, secure_dev_tpm_get_storage_key shall return NULL. ] */
        LogError("storage root key is invalid");
        result = NULL;
    }
    else
    {
        unsigned char data_bytes[1024];
        unsigned char* data_pos = data_bytes;
        /* Codes_SRS_SECURE_DEVICE_TPM_07_018: [ secure_dev_tpm_get_storage_key shall allocate and return the Storage Root Key. ] */
        uint32_t data_length = TPM2B_PUBLIC_Marshal(&handle->srk_pub, &data_pos, NULL);
        STRING_HANDLE encoded_srk = Azure_Base64_Encode_Bytes(data_bytes, data_length);
        if (encoded_srk == NULL)
        {
            /* Codes_SRS_SECURE_DEVICE_TPM_07_019: [ If any failure is encountered, secure_dev_tpm_get_storage_key shall return NULL. ] */
            LogError("Failure encoding storage root key");
            result = NULL;
        }
        else
        {
            if (mallocAndStrcpy_s(&result, STRING_c_str(encoded_srk)) != 0)
            {
                /* Codes_SRS_SECURE_DEVICE_TPM_07_019: [ If any failure is encountered, secure_dev_tpm_get_storage_key shall return NULL. ] */
                LogError("Failure allocating storage root key");
                result = NULL;
            }
            STRING_delete(encoded_srk);
        }
    }
    return result;
}

BUFFER_HANDLE secure_dev_tpm_sign_data(SEC_DEVICE_HANDLE handle, const unsigned char* data, size_t data_len)
{
    BUFFER_HANDLE result;
    /* Codes_SRS_SECURE_DEVICE_TPM_07_020: [ If handle or data is NULL or data_len is 0, secure_dev_tpm_sign_data shall return NULL. ] */
    if (handle == NULL || data == NULL || data_len == 0)
    {
        LogError("Invalid handle value specified handle: %p, data: %p", handle, data);
        result = NULL;
    }
    else
    {
        BYTE data_signature[1024];
        BYTE* data_copy = (unsigned char*)data;
        /* Codes_SRS_SECURE_DEVICE_TPM_07_021: [ secure_dev_tpm_sign_data shall call into the tpm to hash the supplied data value. ] */
        uint32_t sign_len = SignData(&handle->tpm_device, &NullPwSession, data_copy, data_len, data_signature, sizeof(data_signature) );
        if (sign_len == 0)
        {
            /* Codes_SRS_SECURE_DEVICE_TPM_07_023: [ If an error is encountered secure_dev_tpm_sign_data shall return NULL. ] */
            LogError("Failure signing data from hash");
            result = NULL;
        }
        else
        {
            /* Codes_SRS_SECURE_DEVICE_TPM_07_022: [ If hashing the data was successful, secure_dev_tpm_sign_data shall create a BUFFER_HANDLE with the supplied signed data. ] */
            result = BUFFER_create(data_signature, sign_len);
            if (result == NULL)
            {
                /* Codes_SRS_SECURE_DEVICE_TPM_07_023: [ If an error is encountered secure_dev_tpm_sign_data shall return NULL. ] */
                LogError("Failure allocating sign data");
            }
        }
    }
    return result;
}

BUFFER_HANDLE secure_dev_tpm_decrypt_data(SEC_DEVICE_HANDLE handle, const unsigned char* data, size_t data_len)
{
    BUFFER_HANDLE result;
    /* Codes_SRS_SECURE_DEVICE_TPM_07_025: [ If handle or data is NULL or data_len is 0, secure_dev_tpm_decrypt_data shall return NULL. ] */
    if (handle == NULL || data == NULL || data_len == 0)
    {
        LogError("Invalid handle value specified handle: %p, data: %p, data_len: %zu", handle, data, data_len);
        result = NULL;
    }
    else
    {
        /* Codes_SRS_SECURE_DEVICE_TPM_07_024: [ secure_dev_tpm_decrypt_data shall call into the tpm to decrypt the supplied data value. ] */
        result = decrypt_data(handle, data, data_len);
        if (result == NULL)
        {
            /* Codes_SRS_SECURE_DEVICE_TPM_07_029: [ If an error is encountered secure_dev_tpm_sign_data shall return NULL. ] */
            LogError("Failure decrypting data with tpm");
        }
    }
    return result;
}

/* Codes_SRS_SECURE_DEVICE_TPM_07_026: [ secure_dev_tpm_interface shall return the SEC_TPM_INTERFACE structure. ] */
const SEC_TPM_INTERFACE* secure_dev_tpm_interface()
{
    return &sec_tpm_interface;
}
