// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdlib.h>
#include "umock_c/umock_c_prod.h"
#include "azure_c_shared_utility/gballoc.h"
#include "azure_c_shared_utility/sastoken.h"
#include "azure_c_shared_utility/azure_base64.h"
#include "azure_c_shared_utility/sha.h"
#include "azure_c_shared_utility/urlencode.h"
#include "azure_c_shared_utility/strings.h"
#include "azure_c_shared_utility/xlogging.h"
#include "azure_c_shared_utility/crt_abstractions.h"

#include "azure_hub_modules/device_auth_tpm.h"
#include "azure_hub_modules/device_auth.h"
#include "azure_hub_modules/tpm_comm.h"
#include "azure_hub_modules/tpm_codec.h"

#define EPOCH_TIME_T_VALUE          0
#define HMAC_LENGTH                 32
static const UINT32 TPM_20_SRK_HANDLE = HR_PERSISTENT | 0x00000001;
static const UINT32 TPM_20_EK_HANDLE = HR_PERSISTENT | 0x00010001;

TPM2B_AUTH      NullAuth = { 0 };
TSS_SESSION     NullPwSession;

typedef struct TPM_INFO_TAG
{
    TPM_HANDLE tpm_handle;
    TSS_DEVICE tpm_device;

    BUFFER_HANDLE endorsement_key;
    BUFFER_HANDLE storage_root_key;
    //TPM2B_PUBLIC    ekPub;
    //TPM2B_PUBLIC    srkPub;
} TPM_INFO;

static const XDA_SAS_INTERFACE_DESCRIPTION xda_tpm_interface_description =
{
    dev_auth_tpm_create,
    dev_auth_tpm_destroy,
    dev_auth_tpm_get_auth_type,
    dev_auth_tpm_store_key,
    dev_auth_tpm_generate_credentials,
    dev_auth_tpm_get_endorsement_key,
    dev_auth_tpm_get_storage_key,
    dev_auth_tpm_get_registration_key
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

static char* convert_key_to_string(const unsigned char* key_value, size_t key_length)
{
    char* result;

    result = malloc((key_length*2)+1);
    memset(result, 0, (key_length*2)+1);

    char hex_val[3];
    for (size_t index = 0; index < key_length; index++)
    {
        sprintf(hex_val, "%02x", key_value[index]);
        strcat(result, hex_val);
    }
    return result;
}


static unsigned char* tpm_codec_get_srk_cmd(size_t* cmd_len)
{
    // Temporary til tpm code is complete
    unsigned char* result = malloc(1);
    if (result == NULL)
    {
        LogError("Failure: malloc tpm command.");
    }
    else
    {
        result[0] = 0x01;
        *cmd_len = 1;
    }
    return result;
}

static unsigned char* tpm_codec_get_endorsement_key_cmd(size_t* cmd_len)
{
    // Temporary til tpm code is complete
    unsigned char* result = malloc(1);
    if (result == NULL)
    {
        LogError("Failure: malloc endorsement tpm command.");
    }
    else
    {
        result[0] = 0x02;
        *cmd_len = 1;
    }
    return result;
}

static unsigned char* tpm_codec_store_key_cmd(int slot, size_t* cmd_len)
{
    (void)slot;
    // Temporary til tpm code is complete
    unsigned char* result = malloc(1);
    if (result == NULL)
    {
        LogError("Failure: malloc endorsement tpm command.");
    }
    else
    {
        result[0] = 0x03;
        *cmd_len = 1;
    }
    return result;
}

static int load_endorsement_key(TPM_INFO* tpm_info)
{
    int result;
    (void)tpm_info;
    //unsigned char* get_ek_cmd;
    //size_t cmd_len;
    result = 0;
    /*if ( (get_ek_cmd = tpm_codec_get_endorsement_key_cmd(&cmd_len) ) == NULL)
    {
        LogError("Unable to construct endorsement Key tpm command");
        result = MU_FAILURE;
    }
    else if ((tpm_info->endorsement_key = tpm_comm_retrieve_tpm_data(get_ek_cmd, cmd_len) ) == NULL)
    {
        LogError("Unable to send command to the TPM");
        result = MU_FAILURE;
    }
    else
    {
        result = 0;
    }*/
    return 0;
}

static int create_persistent_key(TPM_INFO* tpm_info, TPM_HANDLE request_handle, TPMI_DH_OBJECT hierarchy, TPM2B_PUBLIC* inPub, TPM2B_PUBLIC* outPub)
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
        LogError("Failed calling TPM2_ReadPublic %d", tpm_result);
        result = MU_FAILURE;
    }
    else
    {
        if (TSS_CreatePrimary(&tpm_info->tpm_device, &NullPwSession, hierarchy, inPub, &tpm_info->tpm_handle, outPub) != TPM_RC_SUCCESS)
        {
            LogError("Failed calling TSS_CreatePrimary");
            result = MU_FAILURE;
        }
        else if (TPM2_EvictControl(&tpm_info->tpm_device, &NullPwSession, TPM_RH_OWNER, tpm_info->tpm_handle, request_handle) != TPM_RC_SUCCESS)
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
    return result;
}

static int initialize_tpm_device(TPM_INFO* tpm_info)
{
    int result;
    TPM2B_PUBLIC ekPub;
    TPM2B_PUBLIC srkPub;

    if (Initialize_TPM_Codec(&tpm_info->tpm_device) != TPM_RC_SUCCESS)
    {
        result = MU_FAILURE;
    }
    else if (TSS_CreatePwAuthSession(&NullAuth, &NullPwSession) != TPM_RC_SUCCESS)
    {
        result = MU_FAILURE;
    }
    else if (create_persistent_key(tpm_info, TPM_20_EK_HANDLE, TPM_RH_ENDORSEMENT, GetEkTemplate(), &ekPub) != 0)
    {
        result = MU_FAILURE;
    }
    else if (create_persistent_key(tpm_info, TPM_20_SRK_HANDLE, TPM_RH_OWNER, GetSrkTemplate(), &srkPub) != 0)
    {
        result = MU_FAILURE;
    }
    else
    {
        result = 0;
    }
    return result;
}

static int generate_hash_from_tpm(TPM_INFO* tpm_info, const char* data_payload, unsigned char** digest, uint32_t* digest_len)
{
    int result;
    (void)tpm_info;(void)data_payload;(void)digest;(void)digest_len;
    //uint32_t data_len = strlen(data_payload);

    //unsigned char* rgb_data = (unsigned char*)data_payload;
    result = MU_FAILURE;
    return result;
}

CONCRETE_XDA_HANDLE dev_auth_tpm_create()
{
    TPM_INFO* result;
    result = malloc(sizeof(TPM_INFO) );
    if (result == NULL)
    {
        LogError("Failure: malloc TPM_INFO.");
    }
    else
    {
        memset(result, 0, sizeof(TPM_INFO));
        if (initialize_tpm_device(result) != 0)
        {
            LogError("Failure initializing tpm device.");
            free(result);
            result = NULL;
        }
        else
        {
            result->endorsement_key = NULL;
            result->storage_root_key = NULL;
        }
    }
    return (CONCRETE_XDA_HANDLE)result;
}

void dev_auth_tpm_destroy(CONCRETE_XDA_HANDLE handle)
{
    if (handle != NULL)
    {
        TPM_INFO* tpm_info = (TPM_INFO*)handle;
        BUFFER_delete(tpm_info->endorsement_key);
        BUFFER_delete(tpm_info->storage_root_key);
        free(tpm_info);
    }
}

DEVICE_AUTH_TYPE dev_auth_tpm_get_auth_type(CONCRETE_XDA_HANDLE handle)
{
    DEVICE_AUTH_TYPE result;
    if (handle == NULL)
    {
        result = AUTH_TYPE_UNKNOWN;
    }
    else
    {
        result = AUTH_TYPE_SAS;
    }
    return result;
}

void* dev_auth_tpm_generate_credentials(CONCRETE_XDA_HANDLE handle, const DEVICE_AUTH_CREDENTIAL_INFO* dev_auth_cred)
{
    void* result;
    if (handle == NULL || dev_auth_cred == NULL)
    {
        LogError("Invalid argument specified handle: %p dev_auth_cred: %p.", handle, dev_auth_cred);
        result = NULL;
    }
    else if (dev_auth_cred->dev_auth_type != AUTH_TYPE_SAS)
    {
        LogError("Invalid dev_auth type specified.");
        result = NULL;
    }
    else
    {
        TPM_INFO* tpm_info = (TPM_INFO*)handle;
        char expire_token[64] = { 0 };

        if (size_tToString(expire_token, sizeof(expire_token), (size_t)dev_auth_cred->auth_cred_info.sas_info.expiry_seconds) != 0)
        {
            result = NULL;
            LogError("Failure creating empty key name string.");
        }
        else
        {
            unsigned char* hmac_buffer;
            uint32_t hmac_len = HMAC_LENGTH;
            size_t len = strlen(dev_auth_cred->auth_cred_info.sas_info.token_scope)+strlen(expire_token)+3;
            char* payload = malloc(len);
            if (payload == NULL)
            {
                result = NULL;
                LogError("Failure allocating payload.");
            }
            else
            {
                size_t total_len = sprintf(payload, "%s\n%s", dev_auth_cred->auth_cred_info.sas_info.token_scope, expire_token);
                if (total_len <= 0)
                {
                    result = NULL;
                    LogError("Failure constructing hash payload.");
                }
                else if (generate_hash_from_tpm(tpm_info, payload, &hmac_buffer, &hmac_len) != 0)
                {
                    STRING_HANDLE urlEncodedSignature;
                    STRING_HANDLE base64Signature;
                    STRING_HANDLE sas_token_handle;
                    if ((base64Signature = Azure_Base64_Encode_Bytes(hmac_buffer, hmac_len)) == NULL)
                    {
                        result = NULL;
                        LogError("Failure constructing base64 encoding.");
                    }
                    else if ((urlEncodedSignature = URL_Encode(base64Signature)) == NULL)
                    {
                        result = NULL;
                        LogError("Failure constructing url Signature.");
                        STRING_delete(base64Signature);
                    }
                    else
                    {
                        sas_token_handle = STRING_construct_sprintf("SharedAccessSignature sr=%s&sig=%s&se=%s&skn=", dev_auth_cred->auth_cred_info.sas_info.token_scope, STRING_c_str(urlEncodedSignature), expire_token);
                        if (sas_token_handle == NULL)
                        {
                            result = NULL;
                            LogError("Failure constructing url Signature.");
                        }
                        else
                        {
                            char* sas_token;
                            if (mallocAndStrcpy_s(&sas_token, STRING_c_str(sas_token_handle)) != 0)
                            {
                                result = NULL;
                                LogError("Failure allocating and copying string.");
                            }
                            else
                            {
                                result = sas_token;
                            }
                            STRING_delete(sas_token_handle);
                        }
                        STRING_delete(base64Signature);
                        STRING_delete(urlEncodedSignature);
                    }
                }
                free(payload);
            }
        }
    }
    return result;
}

int dev_auth_tpm_store_key(CONCRETE_XDA_HANDLE handle, KEY_TYPE key_type, const unsigned char* key, size_t key_len)
{
    int result;
    if (handle == NULL || key == NULL || key_len == 0)
    {
        LogError("Invalid argument specified handle: %p || key: %p || key_len: %d.", handle, key, key_len);
        result = MU_FAILURE;
    }
    else
    {
        int slot;
        //unsigned char* store_key_cmd;
        if (key_type == KEY_TYPE_DEVICE)
        {
            // Add into slot 0
            slot = 0;
        }
        else
        {
            // Add into slot 1
            slot = 1;
        }
        result = MU_FAILURE;

        // Construct store tpm command
        /*size_t cmd_len;
        if ((store_key_cmd = tpm_codec_store_key_cmd(slot, &cmd_len) ) == NULL)
        {
            LogError("Unable to construct store_key tpm command");
            result = MU_FAILURE;
        }
        else
        {
            if (tpm_comm_send_tpm_data(store_key_cmd, cmd_len) != 0)
            {
                LogError("Unable to send command to the TPM");
                result = MU_FAILURE;
            }
            else
            {
                result = 0;
            }
            free(store_key_cmd);
        }*/
    }
    return result;
}

char* dev_auth_tpm_get_endorsement_key(CONCRETE_XDA_HANDLE handle)
{
    char* result;
    if (handle == NULL)
    {
        LogError("Invalid handle value specified");
        result = NULL;
    }
    else
    {
        TPM_INFO* tpm_info = (TPM_INFO*)handle;
        if (tpm_info->endorsement_key == NULL)
        {
            if (load_endorsement_key(tpm_info) != 0)
            {
                LogError("Failed loading endorsement key");
                result = NULL;
            }
        }

        if (tpm_info->endorsement_key == NULL)
        {
            LogError("Failed retrieving endorsement key");
            result = NULL;
        }
        else
        {
            STRING_HANDLE encoded_key = Azure_Base64_Encode(tpm_info->endorsement_key);
            if (encoded_key == NULL)
            {
                LogError("Failed base64 encoding");
                result = NULL;
            }
            else
            {
                if (mallocAndStrcpy_s(&result, STRING_c_str(encoded_key)) != 0)
                {
                    LogError("Failed retrieving endorsement key");
                    result = NULL;
                }
                STRING_delete(encoded_key);
            }
        }
    }
    return result;
}

char* dev_auth_tpm_get_storage_key(CONCRETE_XDA_HANDLE handle)
{
    char* result;
    if (handle == NULL)
    {
        LogError("Invalid handle value specified");
        result = NULL;
    }
    else
    {
        result = NULL;
        /*TPM_INFO* tpm_info = (TPM_INFO*)handle;
        if (tpm_info->storage_root_key == NULL)
        {
            unsigned char* get_srk_cmd;
            size_t cmd_len;
            if ( (get_srk_cmd = tpm_codec_get_srk_cmd(&cmd_len) ) == NULL)
            {
                LogError("Unable to construct storage root Key tpm command");
                result = NULL;
            }
            else if ((tpm_info->storage_root_key = tpm_comm_retrieve_tpm_data(get_srk_cmd, cmd_len) ) == NULL)
            {
                LogError("Unable to send command to the TPM");
                result = NULL;
            }
        }

        if (tpm_info->storage_root_key == NULL)
        {
            LogError("Failed retrieving storage root key");
            result = NULL;
        }
        else
        {
            STRING_HANDLE encoded_key = Azure_Base64_Encode(tpm_info->storage_root_key);
            if (encoded_key == NULL)
            {
                LogError("Failed base64 encoding");
                result = NULL;
            }
            else
            {
                if (mallocAndStrcpy_s(&result, STRING_c_str(encoded_key)) != 0)
                {
                    LogError("Failed retrieving storage root key");
                    result = NULL;
                }
                STRING_delete(encoded_key);
            }
        }*/
    }
    return result;
}

char* dev_auth_tpm_get_registration_key(CONCRETE_XDA_HANDLE handle)
{
    char* result;
    if (handle == NULL)
    {
        LogError("Invalid handle value specified");
        result = NULL;
    }
    else
    {
        TPM_INFO* tpm_info = (TPM_INFO*)handle;
        if (tpm_info->endorsement_key == NULL)
        {
            if (load_endorsement_key(tpm_info) != 0)
            {
                LogError("Failed loading registration key");
                result = NULL;
            }
        }

        if (tpm_info->endorsement_key == NULL)
        {
            LogError("Failed retrieving registration key");
            result = NULL;
        }
        else
        {
            SHA256Context sha_ctx;
            uint8_t msg_digest[SHA256HashSize];
            size_t buff_length = BUFFER_length(tpm_info->endorsement_key);
            if (SHA256Reset(&sha_ctx) != 0)
            {
                LogError("Failed sha256 reset");
                result = NULL;
            }
            else if (SHA256Input(&sha_ctx, BUFFER_u_char(tpm_info->endorsement_key), buff_length) != 0)
            {
                LogError("Failed SHA256Input");
                result = NULL;
            }
            /*else if (SHA256FinalBits(&sha_ctx, const uint8_t bits, unsigned int bitcount) != 0)
            {
            }*/
            else if (SHA256Result(&sha_ctx, msg_digest) != 0)
            {
                LogError("Failed SHA256Result");
                result = NULL;
            }
            else
            {
                STRING_HANDLE encoded_key = Azure_Base64_Encode_Bytes(msg_digest, SHA256HashSize);
                if (encoded_key == NULL)
                {
                    LogError("Failed base64 encoding");
                    result = NULL;
                }
                else
                {
                    if (mallocAndStrcpy_s(&result, STRING_c_str(encoded_key)) != 0)
                    {
                        LogError("Failed allocating registration key");
                        result = NULL;
                    }
                    STRING_delete(encoded_key);
                }
                //result = convert_key_to_string(msg_digest, SHA256HashSize);
            }
        }
    }
    return result;
}

const XDA_SAS_INTERFACE_DESCRIPTION* dev_auth_tpm_interface_desc()
{
    return &xda_tpm_interface_description;
}
