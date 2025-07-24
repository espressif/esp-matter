# Generating FirmwareBuildTime.h
execute_process(
    COMMAND date "+%s"
    OUTPUT_VARIABLE BUILD_SECONDS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
math(EXPR BUILD_SECONDS "${BUILD_SECONDS} - (946684800)")
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/FirmwareBuildTime.h
"#pragma once\n
#define CHIP_DEVICE_CONFIG_FIRMWARE_BUILD_TIME_MATTER_EPOCH_S ${BUILD_SECONDS}
")

# Generating access/AccessBuildConfig.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/access/AccessBuildConfig.h
"#pragma once\n
#include <sdkconfig.h>\n

#ifdef CONFIG_ENABLE_ACCESS_RESTRICTIONS
#define CHIP_CONFIG_USE_ACCESS_RESTRICTIONS 1
#else
#define CHIP_CONFIG_USE_ACCESS_RESTRICTIONS 0
#endif
")

# Generating app/AppBuildConfig.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/app/AppBuildConfig.h
"#pragma once\n
#include <sdkconfig.h>\n

#ifdef CONFIG_CHIP_CONFIG_IM_PRETTY_PRINT
#define CHIP_CONFIG_IM_PRETTY_PRINT 1
#else
#define CHIP_CONFIG_IM_PRETTY_PRINT 0
#endif

#define CHIP_CONFIG_IM_FORCE_FABRIC_QUOTA_CHECK 0
#define CHIP_CONFIG_ENABLE_SESSION_RESUMPTION 1
#define CHIP_CONFIG_ACCESS_CONTROL_POLICY_LOGGING_VERBOSITY 0

#ifdef CONFIG_ENABLE_PERSIST_SUBSCRIPTIONS
#define CHIP_CONFIG_PERSIST_SUBSCRIPTIONS 1
#define CHIP_CONFIG_SUBSCRIPTION_TIMEOUT_RESUMPTION 1
#else
#define CHIP_CONFIG_PERSIST_SUBSCRIPTIONS 0
#define CHIP_CONFIG_SUBSCRIPTION_TIMEOUT_RESUMPTION 0
#endif

#ifdef CONFIG_DISABLE_READ_CLIENT
#define CHIP_CONFIG_ENABLE_READ_CLIENT 0
#else
#define CHIP_CONFIG_ENABLE_READ_CLIENT 1
#endif

#define CHIP_CONFIG_STATIC_GLOBAL_INTERACTION_MODEL_ENGINE 1
#define TIME_SYNC_ENABLE_TSC_FEATURE 1
#define NON_SPEC_COMPLIANT_OTA_ACTION_DELAY_FLOOR -1
#define CHIP_DEVICE_CONFIG_DYNAMIC_SERVER 0
#define CHIP_CONFIG_ENABLE_BUSY_HANDLING_FOR_OPERATIONAL_SESSION_SETUP 1
#define CHIP_CONFIG_DATA_MODEL_CHECK_DIE_ON_FAILURE 0
#define CHIP_CONFIG_TERMS_AND_CONDITIONS_REQUIRED 0
#define CHIP_CONFIG_USE_DATA_MODEL_INTERFACE 0
#define CHIP_CONFIG_USE_EMBER_DATA_MODEL 1
")

# Generating app/icd/server/ICDServerBuildConfig.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/app/icd/server/ICDServerBuildConfig.h
"#pragma once\n
#include <sdkconfig.h>\n
#ifdef CONFIG_ENABLE_ICD_SERVER
#define CHIP_CONFIG_ENABLE_ICD_SERVER 1
#else
#define CHIP_CONFIG_ENABLE_ICD_SERVER 0
#endif

#ifdef CONFIG_ENABLE_ICD_LIT
#define CHIP_CONFIG_ENABLE_ICD_LIT 1
#else
#define CHIP_CONFIG_ENABLE_ICD_LIT 0
#endif

#ifdef CONFIG_ENABLE_ICD_CIP
#define CHIP_CONFIG_ENABLE_ICD_CIP 1
#else
#define CHIP_CONFIG_ENABLE_ICD_CIP 0
#endif

#ifdef CONFIG_ENABLE_ICD_USER_ACTIVE_MODE_TRIGGER
#define CHIP_CONFIG_ENABLE_ICD_UAT 1
#else
#define CHIP_CONFIG_ENABLE_ICD_UAT 0
#endif

#define CHIP_CONFIG_ENABLE_ICD_DSLS 0

#ifdef CONFIG_ICD_REPORT_ON_ACTIVE_MODE
#define ICD_REPORT_ON_ENTER_ACTIVE_MODE 1
#else
#define ICD_REPORT_ON_ENTER_ACTIVE_MODE 0
#endif

#ifdef CONFIG_ICD_MAX_NOTIFICATION_SUBSCRIBERS
#define ICD_MAX_NOTIFICATION_SUBSCRIBERS CONFIG_ICD_MAX_NOTIFICATION_SUBSCRIBERS
#else
#define ICD_MAX_NOTIFICATION_SUBSCRIBERS 1
#endif
")

# Generating asn1/ASN1OID.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/asn1/ASN1OID.h
"#pragma once\n
#include <cstdint>
#include <cstddef>
#include <sdkconfig.h>\n
namespace chip {
    namespace ASN1 {
    
    enum OIDCategory
    {
        kOIDCategory_PubKeyAlgo = 0x0100,
        kOIDCategory_SigAlgo = 0x0200,
        kOIDCategory_AttributeType = 0x0300,
        kOIDCategory_EllipticCurve = 0x0400,
        kOIDCategory_Extension = 0x0500,
        kOIDCategory_KeyPurpose = 0x0600,
    
        kOIDCategory_NotSpecified = 0,
        kOIDCategory_Unknown = 0x0F00,
        kOIDCategory_Mask = 0x0F00
    };
    
    typedef uint16_t OID;
    
    enum
    {
        kOID_PubKeyAlgo_ECPublicKey = 0x0101,
    
        kOID_SigAlgo_ECDSAWithSHA256 = 0x0201,
    
        kOID_AttributeType_CommonName = 0x0301,
        kOID_AttributeType_Surname = 0x0302,
        kOID_AttributeType_SerialNumber = 0x0303,
        kOID_AttributeType_CountryName = 0x0304,
        kOID_AttributeType_LocalityName = 0x0305,
        kOID_AttributeType_StateOrProvinceName = 0x0306,
        kOID_AttributeType_OrganizationName = 0x0307,
        kOID_AttributeType_OrganizationalUnitName = 0x0308,
        kOID_AttributeType_Title = 0x0309,
        kOID_AttributeType_Name = 0x030A,
        kOID_AttributeType_GivenName = 0x030B,
        kOID_AttributeType_Initials = 0x030C,
        kOID_AttributeType_GenerationQualifier = 0x030D,
        kOID_AttributeType_DNQualifier = 0x030E,
        kOID_AttributeType_Pseudonym = 0x030F,
        kOID_AttributeType_DomainComponent = 0x0310,
        kOID_AttributeType_MatterNodeId = 0x0311,
        kOID_AttributeType_MatterFirmwareSigningId = 0x0312,
        kOID_AttributeType_MatterICACId = 0x0313,
        kOID_AttributeType_MatterRCACId = 0x0314,
        kOID_AttributeType_MatterFabricId = 0x0315,
        kOID_AttributeType_MatterCASEAuthTag = 0x0316,
    
        kOID_EllipticCurve_prime256v1 = 0x0401,
    
        kOID_Extension_BasicConstraints = 0x0501,
        kOID_Extension_KeyUsage = 0x0502,
        kOID_Extension_ExtendedKeyUsage = 0x0503,
        kOID_Extension_SubjectKeyIdentifier = 0x0504,
        kOID_Extension_AuthorityKeyIdentifier = 0x0505,
        kOID_Extension_CSRRequest = 0x0506,
    
        kOID_KeyPurpose_ServerAuth = 0x0601,
        kOID_KeyPurpose_ClientAuth = 0x0602,
        kOID_KeyPurpose_CodeSigning = 0x0603,
        kOID_KeyPurpose_EmailProtection = 0x0604,
        kOID_KeyPurpose_TimeStamping = 0x0605,
        kOID_KeyPurpose_OCSPSigning = 0x0606,
    
        kOID_NotSpecified = 0,
        kOID_Unknown = 0xFFFF,
        kOID_EnumMask = 0x00FF
    };
    
    struct OIDTableEntry
    {
        OID EnumVal;
        const uint8_t *EncodedOID;
        uint16_t EncodedOIDLen;
    };
    
    struct OIDNameTableEntry
    {
        OID EnumVal;
        const char *Name;
    };
    
    extern const OIDTableEntry sOIDTable[];
    extern const OIDNameTableEntry sOIDNameTable[];
    extern const size_t sOIDTableSize;
    
    #ifdef ASN1_DEFINE_OID_TABLE
    
    static const uint8_t sOID_PubKeyAlgo_ECPublicKey[] = { 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02, 0x01 };
    static const uint8_t sOID_SigAlgo_ECDSAWithSHA256[] = { 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02 };
    static const uint8_t sOID_AttributeType_CommonName[] = { 0x55, 0x04, 0x03 };
    static const uint8_t sOID_AttributeType_Surname[] = { 0x55, 0x04, 0x04 };
    static const uint8_t sOID_AttributeType_SerialNumber[] = { 0x55, 0x04, 0x05 };
    static const uint8_t sOID_AttributeType_CountryName[] = { 0x55, 0x04, 0x06 };
    static const uint8_t sOID_AttributeType_LocalityName[] = { 0x55, 0x04, 0x07 };
    static const uint8_t sOID_AttributeType_StateOrProvinceName[] = { 0x55, 0x04, 0x08 };
    static const uint8_t sOID_AttributeType_OrganizationName[] = { 0x55, 0x04, 0x0A };
    static const uint8_t sOID_AttributeType_OrganizationalUnitName[] = { 0x55, 0x04, 0x0B };
    static const uint8_t sOID_AttributeType_Title[] = { 0x55, 0x04, 0x0C };
    static const uint8_t sOID_AttributeType_Name[] = { 0x55, 0x04, 0x29 };
    static const uint8_t sOID_AttributeType_GivenName[] = { 0x55, 0x04, 0x2A };
    static const uint8_t sOID_AttributeType_Initials[] = { 0x55, 0x04, 0x2B };
    static const uint8_t sOID_AttributeType_GenerationQualifier[] = { 0x55, 0x04, 0x2C };
    static const uint8_t sOID_AttributeType_DNQualifier[] = { 0x55, 0x04, 0x2E };
    static const uint8_t sOID_AttributeType_Pseudonym[] = { 0x55, 0x04, 0x41 };
    static const uint8_t sOID_AttributeType_DomainComponent[] = { 0x09, 0x92, 0x26, 0x89, 0x93, 0xF2, 0x2C, 0x64, 0x01, 0x19 };
    static const uint8_t sOID_AttributeType_MatterNodeId[] = { 0x2B, 0x06, 0x01, 0x04, 0x01, 0x82, 0xA2, 0x7C, 0x01, 0x01 };
    static const uint8_t sOID_AttributeType_MatterFirmwareSigningId[] = { 0x2B, 0x06, 0x01, 0x04, 0x01, 0x82, 0xA2, 0x7C, 0x01, 0x02 };
    static const uint8_t sOID_AttributeType_MatterICACId[] = { 0x2B, 0x06, 0x01, 0x04, 0x01, 0x82, 0xA2, 0x7C, 0x01, 0x03 };
    static const uint8_t sOID_AttributeType_MatterRCACId[] = { 0x2B, 0x06, 0x01, 0x04, 0x01, 0x82, 0xA2, 0x7C, 0x01, 0x04 };
    static const uint8_t sOID_AttributeType_MatterFabricId[] = { 0x2B, 0x06, 0x01, 0x04, 0x01, 0x82, 0xA2, 0x7C, 0x01, 0x05 };
    static const uint8_t sOID_AttributeType_MatterCASEAuthTag[] = { 0x2B, 0x06, 0x01, 0x04, 0x01, 0x82, 0xA2, 0x7C, 0x01, 0x06 };
    static const uint8_t sOID_EllipticCurve_prime256v1[] = { 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07 };
    static const uint8_t sOID_Extension_BasicConstraints[] = { 0x55, 0x1D, 0x13 };
    static const uint8_t sOID_Extension_KeyUsage[] = { 0x55, 0x1D, 0x0F };
    static const uint8_t sOID_Extension_ExtendedKeyUsage[] = { 0x55, 0x1D, 0x25 };
    static const uint8_t sOID_Extension_SubjectKeyIdentifier[] = { 0x55, 0x1D, 0x0E };
    static const uint8_t sOID_Extension_AuthorityKeyIdentifier[] = { 0x55, 0x1D, 0x23 };
    static const uint8_t sOID_Extension_CSRRequest[] = { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x09, 0x0E };
    static const uint8_t sOID_KeyPurpose_ServerAuth[] = { 0x2B, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x01 };
    static const uint8_t sOID_KeyPurpose_ClientAuth[] = { 0x2B, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x02 };
    static const uint8_t sOID_KeyPurpose_CodeSigning[] = { 0x2B, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x03 };
    static const uint8_t sOID_KeyPurpose_EmailProtection[] = { 0x2B, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x04 };
    static const uint8_t sOID_KeyPurpose_TimeStamping[] = { 0x2B, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x08 };
    static const uint8_t sOID_KeyPurpose_OCSPSigning[] = { 0x2B, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x09 };
    
    
    const OIDTableEntry sOIDTable[] =
    {
        { kOID_PubKeyAlgo_ECPublicKey, sOID_PubKeyAlgo_ECPublicKey, sizeof(sOID_PubKeyAlgo_ECPublicKey) },
        { kOID_SigAlgo_ECDSAWithSHA256, sOID_SigAlgo_ECDSAWithSHA256, sizeof(sOID_SigAlgo_ECDSAWithSHA256) },
        { kOID_AttributeType_CommonName, sOID_AttributeType_CommonName, sizeof(sOID_AttributeType_CommonName) },
        { kOID_AttributeType_Surname, sOID_AttributeType_Surname, sizeof(sOID_AttributeType_Surname) },
        { kOID_AttributeType_SerialNumber, sOID_AttributeType_SerialNumber, sizeof(sOID_AttributeType_SerialNumber) },
        { kOID_AttributeType_CountryName, sOID_AttributeType_CountryName, sizeof(sOID_AttributeType_CountryName) },
        { kOID_AttributeType_LocalityName, sOID_AttributeType_LocalityName, sizeof(sOID_AttributeType_LocalityName) },
        { kOID_AttributeType_StateOrProvinceName, sOID_AttributeType_StateOrProvinceName, sizeof(sOID_AttributeType_StateOrProvinceName) },
        { kOID_AttributeType_OrganizationName, sOID_AttributeType_OrganizationName, sizeof(sOID_AttributeType_OrganizationName) },
        { kOID_AttributeType_OrganizationalUnitName, sOID_AttributeType_OrganizationalUnitName, sizeof(sOID_AttributeType_OrganizationalUnitName) },
        { kOID_AttributeType_Title, sOID_AttributeType_Title, sizeof(sOID_AttributeType_Title) },
        { kOID_AttributeType_Name, sOID_AttributeType_Name, sizeof(sOID_AttributeType_Name) },
        { kOID_AttributeType_GivenName, sOID_AttributeType_GivenName, sizeof(sOID_AttributeType_GivenName) },
        { kOID_AttributeType_Initials, sOID_AttributeType_Initials, sizeof(sOID_AttributeType_Initials) },
        { kOID_AttributeType_GenerationQualifier, sOID_AttributeType_GenerationQualifier, sizeof(sOID_AttributeType_GenerationQualifier) },
        { kOID_AttributeType_DNQualifier, sOID_AttributeType_DNQualifier, sizeof(sOID_AttributeType_DNQualifier) },
        { kOID_AttributeType_Pseudonym, sOID_AttributeType_Pseudonym, sizeof(sOID_AttributeType_Pseudonym) },
        { kOID_AttributeType_DomainComponent, sOID_AttributeType_DomainComponent, sizeof(sOID_AttributeType_DomainComponent) },
        { kOID_AttributeType_MatterNodeId, sOID_AttributeType_MatterNodeId, sizeof(sOID_AttributeType_MatterNodeId) },
        { kOID_AttributeType_MatterFirmwareSigningId, sOID_AttributeType_MatterFirmwareSigningId, sizeof(sOID_AttributeType_MatterFirmwareSigningId) },
        { kOID_AttributeType_MatterICACId, sOID_AttributeType_MatterICACId, sizeof(sOID_AttributeType_MatterICACId) },
        { kOID_AttributeType_MatterRCACId, sOID_AttributeType_MatterRCACId, sizeof(sOID_AttributeType_MatterRCACId) },
        { kOID_AttributeType_MatterFabricId, sOID_AttributeType_MatterFabricId, sizeof(sOID_AttributeType_MatterFabricId) },
        { kOID_AttributeType_MatterCASEAuthTag, sOID_AttributeType_MatterCASEAuthTag, sizeof(sOID_AttributeType_MatterCASEAuthTag) },
        { kOID_EllipticCurve_prime256v1, sOID_EllipticCurve_prime256v1, sizeof(sOID_EllipticCurve_prime256v1) },
        { kOID_Extension_BasicConstraints, sOID_Extension_BasicConstraints, sizeof(sOID_Extension_BasicConstraints) },
        { kOID_Extension_KeyUsage, sOID_Extension_KeyUsage, sizeof(sOID_Extension_KeyUsage) },
        { kOID_Extension_ExtendedKeyUsage, sOID_Extension_ExtendedKeyUsage, sizeof(sOID_Extension_ExtendedKeyUsage) },
        { kOID_Extension_SubjectKeyIdentifier, sOID_Extension_SubjectKeyIdentifier, sizeof(sOID_Extension_SubjectKeyIdentifier) },
        { kOID_Extension_AuthorityKeyIdentifier, sOID_Extension_AuthorityKeyIdentifier, sizeof(sOID_Extension_AuthorityKeyIdentifier) },
        { kOID_Extension_CSRRequest, sOID_Extension_CSRRequest, sizeof(sOID_Extension_CSRRequest) },
        { kOID_KeyPurpose_ServerAuth, sOID_KeyPurpose_ServerAuth, sizeof(sOID_KeyPurpose_ServerAuth) },
        { kOID_KeyPurpose_ClientAuth, sOID_KeyPurpose_ClientAuth, sizeof(sOID_KeyPurpose_ClientAuth) },
        { kOID_KeyPurpose_CodeSigning, sOID_KeyPurpose_CodeSigning, sizeof(sOID_KeyPurpose_CodeSigning) },
        { kOID_KeyPurpose_EmailProtection, sOID_KeyPurpose_EmailProtection, sizeof(sOID_KeyPurpose_EmailProtection) },
        { kOID_KeyPurpose_TimeStamping, sOID_KeyPurpose_TimeStamping, sizeof(sOID_KeyPurpose_TimeStamping) },
        { kOID_KeyPurpose_OCSPSigning, sOID_KeyPurpose_OCSPSigning, sizeof(sOID_KeyPurpose_OCSPSigning) },
        { kOID_NotSpecified, NULL, 0 }
    };
    
    const size_t sOIDTableSize = sizeof(sOIDTable) / sizeof(OIDTableEntry);
    
    #endif // ASN1_DEFINE_OID_TABLE
    
    #ifdef ASN1_DEFINE_OID_NAME_TABLE
    
    const OIDNameTableEntry sOIDNameTable[] =
    {
        { kOID_PubKeyAlgo_ECPublicKey, \"ECPublicKey\" },
        { kOID_SigAlgo_ECDSAWithSHA256, \"ECDSAWithSHA256\" },
        { kOID_AttributeType_CommonName, \"CommonName\" },
        { kOID_AttributeType_Surname, \"Surname\" },
        { kOID_AttributeType_SerialNumber, \"SerialNumber\" },
        { kOID_AttributeType_CountryName, \"CountryName\" },
        { kOID_AttributeType_LocalityName, \"LocalityName\" },
        { kOID_AttributeType_StateOrProvinceName, \"StateOrProvinceName\" },
        { kOID_AttributeType_OrganizationName, \"OrganizationName\" },
        { kOID_AttributeType_OrganizationalUnitName, \"OrganizationalUnitName\" },
        { kOID_AttributeType_Title, \"Title\" },
        { kOID_AttributeType_Name, \"Name\" },
        { kOID_AttributeType_GivenName, \"GivenName\" },
        { kOID_AttributeType_Initials, \"Initials\" },
        { kOID_AttributeType_GenerationQualifier, \"GenerationQualifier\" },
        { kOID_AttributeType_DNQualifier, \"DNQualifier\" },
        { kOID_AttributeType_Pseudonym, \"Pseudonym\" },
        { kOID_AttributeType_DomainComponent, \"DomainComponent\" },
        { kOID_AttributeType_MatterNodeId, \"MatterNodeId\" },
        { kOID_AttributeType_MatterFirmwareSigningId, \"MatterFirmwareSigningId\" },
        { kOID_AttributeType_MatterICACId, \"MatterICACId\" },
        { kOID_AttributeType_MatterRCACId, \"MatterRCACId\" },
        { kOID_AttributeType_MatterFabricId, \"MatterFabricId\" },
        { kOID_AttributeType_MatterCASEAuthTag, \"MatterCASEAuthTag\" },
        { kOID_EllipticCurve_prime256v1, \"prime256v1\" },
        { kOID_Extension_BasicConstraints, \"BasicConstraints\" },
        { kOID_Extension_KeyUsage, \"KeyUsage\" },
        { kOID_Extension_ExtendedKeyUsage, \"ExtendedKeyUsage\" },
        { kOID_Extension_SubjectKeyIdentifier, \"SubjectKeyIdentifier\" },
        { kOID_Extension_AuthorityKeyIdentifier, \"AuthorityKeyIdentifier\" },
        { kOID_Extension_CSRRequest, \"CSRRequest\" },
        { kOID_KeyPurpose_ServerAuth, \"ServerAuth\" },
        { kOID_KeyPurpose_ClientAuth, \"ClientAuth\" },
        { kOID_KeyPurpose_CodeSigning, \"CodeSigning\" },
        { kOID_KeyPurpose_EmailProtection, \"EmailProtection\" },
        { kOID_KeyPurpose_TimeStamping, \"TimeStamping\" },
        { kOID_KeyPurpose_OCSPSigning, \"OCSPSigning\" },
        { kOID_NotSpecified, NULL }
    };
    
    #endif // ASN1_DEFINE_OID_NAME_TABLE
    
    } // namespace ASN1
    } // namespace chip
")

# Generating ble/BleBuildConfig.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/ble/BleBuildConfig.h
"#pragma once\n
#include <sdkconfig.h>\n
#ifdef CONFIG_ENABLE_CHIPOBLE
#define CONFIG_NETWORK_LAYER_BLE 1
#else
#define CONFIG_NETWORK_LAYER_BLE 0
#endif

#define CHIP_ENABLE_CHIPOBLE_TEST 0

#ifndef CONFIG_CHIP_ENABLE_EXTERNAL_PLATFORM
#define BLE_PLATFORM_CONFIG_INCLUDE <platform/ESP32/BlePlatformConfig.h>
#endif
")

# Generating core/CHIPBuildConfig.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/core/CHIPBuildConfig.h
"#pragma once\n
#include <sdkconfig.h>\n
#define CHIP_FUZZING_ENABLED 0

#ifdef CONFIG_BUILD_CHIP_TESTS
#define CHIP_CONFIG_TEST 1
#else
#define CHIP_CONFIG_TEST 0
#endif

#define CHIP_ERROR_LOGGING (CONFIG_CHIP_LOG_DEFAULT_LEVEL >= 1)
#define CHIP_PROGRESS_LOGGING (CONFIG_CHIP_LOG_DEFAULT_LEVEL >= 3)
#define CHIP_DETAIL_LOGGING (CONFIG_CHIP_LOG_DEFAULT_LEVEL >= 4)
#define CHIP_CONFIG_LOG_MESSAGE_MAX_SIZE 256
#define CHIP_AUTOMATION_LOGGING (CONFIG_CHIP_LOG_DEFAULT_LEVEL >= 5)
#define CHIP_PW_TOKENIZER_LOGGING 0
#define CHIP_USE_PW_LOGGING 0
#define CHIP_EXCHANGE_NODE_ID_LOGGING 0
#define CHIP_CONFIG_SHORT_ERROR_STR 1
#define CHIP_CONFIG_ENABLE_ARG_PARSER 1
#define CHIP_TARGET_STYLE_UNIX 0
#define CHIP_TARGET_STYLE_EMBEDDED 1
#define CHIP_CONFIG_MEMORY_MGMT_MALLOC 1
#define HAVE_MALLOC 1
#define HAVE_FREE 1
#define HAVE_NEW 0
#define CHIP_CONFIG_MEMORY_MGMT_PLATFORM 0
#define CHIP_CONFIG_MEMORY_DEBUG_CHECKS 0
#define CHIP_CONFIG_MEMORY_DEBUG_DMALLOC 0
#define CHIP_CONFIG_PROVIDE_OBSOLESCENT_INTERFACES 0
#define CHIP_CONFIG_TRANSPORT_TRACE_ENABLED 0
#define CHIP_CONFIG_TRANSPORT_PW_TRACE_ENABLED 0
#define CHIP_CONFIG_MINMDNS_DYNAMIC_OPERATIONAL_RESPONDER_LIST 0
#define CHIP_CONFIG_MINMDNS_MAX_PARALLEL_RESOLVES 2
#define CHIP_CONFIG_CANCELABLE_HAS_INFO_STRING_FIELD 0
#define CHIP_CONFIG_BIG_ENDIAN_TARGET 0
#define CHIP_CONFIG_TLV_VALIDATE_CHAR_STRING_ON_WRITE 1
#define CHIP_CONFIG_TLV_VALIDATE_CHAR_STRING_ON_READ 0
#define CHIP_CONFIG_COMMAND_SENDER_BUILTIN_SUPPORT_FOR_BATCHED_COMMANDS 0
")

# Generating crypto/CryptoBuildConfig.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/crypto/CryptoBuildConfig.h
"#pragma once\n
#include <sdkconfig.h>\n
#define CHIP_CRYPTO_MBEDTLS 1
#define CHIP_CRYPTO_PSA 0
#define CHIP_CRYPTO_PSA_SPAKE2P 0
#define CHIP_CRYPTO_OPENSSL 0
#define CHIP_CRYPTO_BORINGSSL 0
#define CHIP_CRYPTO_PLATFORM 0
")

# Generating inet/InetBuildConfig.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/inet/InetBuildConfig.h
"#pragma once\n
#include <sdkconfig.h>\n

#ifdef CONFIG_BUILD_CHIP_TESTS
#define INET_CONFIG_TEST 1
#else
#define INET_CONFIG_TEST 0
#endif

#ifdef CONFIG_DISABLE_IPV4
#define INET_CONFIG_ENABLE_IPV4 0
#else
#define INET_CONFIG_ENABLE_IPV4 1
#endif

#define INET_CONFIG_ENABLE_TCP_ENDPOINT 0
#define INET_CONFIG_ENABLE_UDP_ENDPOINT 1
#define HAVE_LWIP_RAW_BIND_NETIF 1

#ifndef CONFIG_CHIP_ENABLE_EXTERNAL_PLATFORM
#define INET_PLATFORM_CONFIG_INCLUDE <platform/ESP32/InetPlatformConfig.h>
#endif

#define INET_TCP_END_POINT_IMPL_CONFIG_FILE <inet/TCPEndPointImplLwIP.h>
#define INET_UDP_END_POINT_IMPL_CONFIG_FILE <inet/UDPEndPointImplLwIP.h>
")

# Generating lwip/lwip_buildconfig.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/lwip/lwip_buildconfig.h
"#pragma once\n
#include <sdkconfig.h>\n
#define HAVE_LWIP_UDP_BIND_NETIF 1
")

# Generating matter/tracing/build_config.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/matter/tracing/build_config.h
"#pragma once\n
#include <sdkconfig.h>\n
#if defined(CONFIG_ENABLE_ESP_INSIGHTS_TRACE) && !defined(CONFIG_IDF_TARGET_ESP32H2)
#define MATTER_TRACING_ENABLED 1
#else
#define MATTER_TRACING_ENABLED 0
#endif
")

if (NOT CONFIG_CHIP_ENABLE_EXTERNAL_PLATFORM)
# Generating platform/CHIPDeviceBuildConfig.h when external platform is not enabled.
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/platform/CHIPDeviceBuildConfig.h
"#pragma once\n
#include <sdkconfig.h>\n

#if defined(CONFIG_ENABLE_WIFI_STATION) || defined(CONFIG_ENABLE_WIFI_AP)
#define CHIP_DEVICE_CONFIG_ENABLE_WPA 1
#else
#define CHIP_DEVICE_CONFIG_ENABLE_WPA 0
#endif

#ifdef CONFIG_ENABLE_MATTER_OVER_THREAD
#define CHIP_ENABLE_OPENTHREAD 1
#else
#define CHIP_ENABLE_OPENTHREAD 0
#endif

#ifdef CONFIG_OPENTHREAD_FTD
#define CHIP_DEVICE_CONFIG_THREAD_FTD 1
#else
#define CHIP_DEVICE_CONFIG_THREAD_FTD 0
#endif

#ifdef CONFIG_OPENTHREAD_BORDER_ROUTER
#define CHIP_DEVICE_CONFIG_THREAD_BORDER_ROUTER 1
#else
#define CHIP_DEVICE_CONFIG_THREAD_BORDER_ROUTER 0
#endif

#define CHIP_DEVICE_CONFIG_USES_OTBR_POSIX_DBUS_STACK 0
#define CHIP_STACK_LOCK_TRACKING_ENABLED 1
#define CHIP_STACK_LOCK_TRACKING_ERROR_FATAL 1

#ifdef CONFIG_ENABLE_ROTATING_DEVICE_ID
#define CHIP_ENABLE_ADDITIONAL_DATA_ADVERTISING 1
#else
#define CHIP_ENABLE_ADDITIONAL_DATA_ADVERTISING 0
#endif

#define CHIP_DEVICE_CONFIG_RUN_AS_ROOT 1
#define CHIP_DISABLE_PLATFORM_KVS 0

#ifdef CONFIG_ENABLE_OTA_REQUESTOR
#define CHIP_DEVICE_CONFIG_ENABLE_OTA_REQUESTOR 1
#else
#define CHIP_DEVICE_CONFIG_ENABLE_OTA_REQUESTOR 0
#endif

#ifdef CONFIG_ENABLE_ESP32_FACTORY_DATA_PROVIDER
#define CHIP_USE_TRANSITIONAL_COMMISSIONABLE_DATA_PROVIDER 0
#else
#define CHIP_USE_TRANSITIONAL_COMMISSIONABLE_DATA_PROVIDER 1
#endif

#define CHIP_USE_TRANSITIONAL_DEVICE_INSTANCE_INFO_PROVIDER 1
#define CHIP_DEVICE_LAYER_TARGET_ESP32 1

#ifndef CONFIG_CHIP_ENABLE_EXTERNAL_PLATFORM
#define CHIP_DEVICE_PLATFORM_CONFIG_INCLUDE <platform/ESP32/CHIPDevicePlatformConfig.h>
#define CHIP_DEVICE_LAYER_TARGET ESP32
#endif

#define CHIP_DEVICE_CONFIG_MAX_DISCOVERED_IP_ADDRESSES 5

#if defined(CONFIG_THREAD_NETWORK_COMMISSIONING_DRIVER) && defined(CONFIG_THREAD_NETWORK_ENDPOINT_ID)
#define CHIP_DEVICE_CONFIG_THREAD_NETWORK_ENDPOINT_ID CONFIG_THREAD_NETWORK_ENDPOINT_ID
#else
#define CHIP_DEVICE_CONFIG_THREAD_NETWORK_ENDPOINT_ID 0
#endif

#define CHIP_DEVICE_CONFIG_ENABLE_DYNAMIC_MRP_CONFIG 0
#define CHIP_DEVICE_CONFIG_ENABLE_WIFIPAF 0
#ifdef CONFIG_ENABLE_CHIPOBLE
#define CHIP_DEVICE_CONFIG_ENABLE_CHIPOBLE 1
#else
#define CHIP_DEVICE_CONFIG_ENABLE_CHIPOBLE 0
#endif
")
endif()

# Generating setup_payload/CHIPAdditionalDataPayloadBuildConfig.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/setup_payload/CHIPAdditionalDataPayloadBuildConfig.h
"#pragma once\n
#include <sdkconfig.h>\n
#ifdef CONFIG_ENABLE_ROTATING_DEVICE_ID
#define CHIP_ENABLE_ROTATING_DEVICE_ID 1
#else
#define CHIP_ENABLE_ROTATING_DEVICE_ID 0
#endif
")

# Generating system/SystemBuildConfig.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/system/SystemBuildConfig.h
"#pragma once\n
#include <sdkconfig.h>\n
#define CONFIG_DEVICE_LAYER 1

#ifdef CONFIG_BUILD_CHIP_TESTS
#define CHIP_SYSTEM_CONFIG_TEST 1
#define CHIP_WITH_NLFAULTINJECTION 1
#else
#define CHIP_SYSTEM_CONFIG_TEST 0
#define CHIP_WITH_NLFAULTINJECTION 0
#endif

#define CHIP_SYSTEM_CONFIG_USE_DISPATCH 0
#define CHIP_SYSTEM_CONFIG_USE_LIBEV 0
#define CHIP_SYSTEM_CONFIG_USE_LWIP 1
#define CHIP_SYSTEM_CONFIG_USE_OPEN_THREAD_ENDPOINT 0
#define CHIP_SYSTEM_CONFIG_USE_SOCKETS 0
#define CHIP_SYSTEM_CONFIG_USE_NETWORK_FRAMEWORK 0
#define CHIP_SYSTEM_CONFIG_POSIX_LOCKING 0
#define CHIP_SYSTEM_CONFIG_FREERTOS_LOCKING 1
#define CHIP_SYSTEM_CONFIG_MBED_LOCKING 0
#define CHIP_SYSTEM_CONFIG_CMSIS_RTOS_LOCKING 0
#define CHIP_SYSTEM_CONFIG_ZEPHYR_LOCKING 0
#define CHIP_SYSTEM_CONFIG_NO_LOCKING 0
#define CHIP_SYSTEM_CONFIG_PROVIDE_STATISTICS 1
#define HAVE_CLOCK_GETTIME 1
#define HAVE_CLOCK_SETTIME 1
#define HAVE_GETTIMEOFDAY 0
#define HAVE_SYS_TIME_H 1
#define HAVE_NETINET_ICMP6_H 1
#define HAVE_ICMP6_FILTER 1
#define CONFIG_HAVE_VCBPRINTF 0
#define HAVE_SYS_SOCKET_H 0

#ifndef CONFIG_CHIP_ENABLE_EXTERNAL_PLATFORM
#define CHIP_PLATFORM_CONFIG_INCLUDE <platform/ESP32/CHIPPlatformConfig.h>
#define SYSTEM_PLATFORM_CONFIG_INCLUDE <platform/ESP32/SystemPlatformConfig.h>
// SYSTEM_PROJECT_CONFIG_INCLUDE and CHIP_PROJECT_CONFIG_INCLUDE are defined in CMakeLists.txt
#endif

#define CHIP_SYSTEM_LAYER_IMPL_CONFIG_FILE <system/SystemLayerImplFreeRTOS.h>
#define CHIP_SYSTEM_CONFIG_MULTICAST_HOMING 0
")

# Generating CHIPVersion.h
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/CHIPVersion.h
"/*
 *
 *    Copyright (c) 2020 Project CHIP Authors
 *    All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the \"License\");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an \"AS IS\" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

/**
 *    \@file
 *      This file defines constants and macros for introspecting and
 *      manipulating CHIP versions.
 *
 *      The constants and macros defined here may be used to, for ,
 *      example, conditionally-compile older, newer, or changed CHIP
 *      APIs based on the CHIP version. For example:
 *
 *          \@code
 *          #if CHIP_VERSION_CODE >= CHIP_VERSION_CODE_ENCODE(1, 5, 0)
 *              ...
 *          #else
 *              ...
 *          #endif
 *          \@endcode
 *
 */

#pragma once

#define _CHIP_VERSION_CODE_MAJOR_WIDTH     8
#define _CHIP_VERSION_CODE_MINOR_WIDTH     8
#define _CHIP_VERSION_CODE_PATCH_WIDTH     8

#define _CHIP_VERSION_CODE_MAJOR_MASK     ((1 << _CHIP_VERSION_CODE_MAJOR_WIDTH)  - 1)
#define _CHIP_VERSION_CODE_MINOR_MASK     ((1 << _CHIP_VERSION_CODE_MINOR_WIDTH)  - 1)
#define _CHIP_VERSION_CODE_PATCH_MASK     ((1 << _CHIP_VERSION_CODE_PATCH_WIDTH)  - 1)

#define _CHIP_VERSION_CODE_MAJOR_SHIFT    24
#define _CHIP_VERSION_CODE_MINOR_SHIFT    16
#define _CHIP_VERSION_CODE_PATCH_SHIFT     8

/**
 *  \@def CHIP_VERSION_CODE_ENCODE(major, minor, patch)
 *
 *  \@brief
 *    Encode a CHIP version code from its constituent \@a major, \@a minor, and \@a patch
 *    components.
 *
 *    This macro may be used in conjunction with CHIP_VERSION_CODE to, for
 *    example, conditionally-compile older, newer, or changed CHIP APIs based
 *    on the CHIP version. For example:
 *
 *        \@code
 *        #if CHIP_VERSION_CODE >= CHIP_VERSION_CODE_ENCODE(1, 5, 0)
 *            ...
 *        #else
 *            ...
 *        #endif
 *        \@endcode
 *
 */
#define CHIP_VERSION_CODE_ENCODE(major, minor, patch)                                   \
    ((((major)  & _CHIP_VERSION_CODE_MAJOR_MASK)  << _CHIP_VERSION_CODE_MAJOR_SHIFT)  | \
     (((minor)  & _CHIP_VERSION_CODE_MINOR_MASK)  << _CHIP_VERSION_CODE_MINOR_SHIFT)  | \
     (((patch)  & _CHIP_VERSION_CODE_PATCH_MASK)  << _CHIP_VERSION_CODE_PATCH_SHIFT))

/**
 *  \@def CHIP_VERSION_CODE_DECODE_MAJOR(code)
 *
 *  \@brief
 *    Decode a CHIP major version component from a CHIP version \@a code.
 *
 */
#define CHIP_VERSION_CODE_DECODE_MAJOR(code)  (((code) >> _CHIP_VERSION_CODE_MAJOR_SHIFT)  & _CHIP_VERSION_CODE_MAJOR_MASK)

/**
 *  \@def CHIP_VERSION_CODE_DECODE_MINOR(code)
 *
 *  \@brief
 *    Decode a CHIP minor version component from a CHIP version \@a code.
 *
 */
#define CHIP_VERSION_CODE_DECODE_MINOR(code)  (((code) >> _CHIP_VERSION_CODE_MINOR_SHIFT)  & _CHIP_VERSION_CODE_MINOR_MASK)

/**
 *  \@def CHIP_VERSION_CODE_DECODE_PATCH(code)
 *
 *  \@brief
 *    Decode a CHIP patch version component from a CHIP version \@a code.
 *
 */
#define CHIP_VERSION_CODE_DECODE_PATCH(code)  (((code) >> _CHIP_VERSION_CODE_PATCH_SHIFT)  & _CHIP_VERSION_CODE_PATCH_MASK)

/**
 *  \@def CHIP_VERSION_MAJOR
 *
 *  \@brief
 *    The CHIP version major component, as an unsigned integer.
 *
 */
#define CHIP_VERSION_MAJOR               0

/**
 *  \@def CHIP_VERSION_MINOR
 *
 *  \@brief
 *    The CHIP version minor component, as an unsigned integer.
 *
 */
#define CHIP_VERSION_MINOR               0

/**
 *  \@def CHIP_VERSION_PATCH
 *
 *  \@brief
 *    The CHIP version patch component, as an unsigned integer.
 *
 */
#define CHIP_VERSION_PATCH               0

/**
 *  \@def CHIP_VERSION_EXTRA
 *
 *  \@brief
 *    The CHIP version extra component, as a quoted C string.
 *
 */
#define CHIP_VERSION_EXTRA               \"\"

/**
 *  \@def CHIP_VERSION_STRING
 *
 *  \@brief
 *    The CHIP version, as a quoted C string.
 *
 */
#define CHIP_VERSION_STRING              \"0.0.0\"

/**
 *  \@def CHIP_VERSION_CODE
 *
 *  \@brief
 *    The CHIP version, including the major, minor, and patch components,
 *    encoded as an unsigned integer.
 *
 *    This macro may be used in conjunction with CHIP_VERSION_CODE_ENCODE
 *    to, for example, conditionally-compile older, newer, or changed CHIP
 *    APIs based on the CHIP version. For example:
 *
 *        \@code
 *        #if CHIP_VERSION_CODE >= CHIP_VERSION_CODE_ENCODE(1, 5, 0)
 *            ...
 *        #else
 *            ...
 *        #endif
 *        \@endcode
 *
 */
#define CHIP_VERSION_CODE   CHIP_VERSION_CODE_ENCODE(CHIP_VERSION_MAJOR, CHIP_VERSION_MINOR, CHIP_VERSION_PATCH)
")

# Automatically added defines for script
#define BLE_PROJECT_CONFIG_INCLUDE 1
#define INET_PROJECT_CONFIG_INCLUDE 1
#define LWIP_DEBUG 1
#define CHIP_DEVICE_CONFIG_ENABLE_NFC 1
#define CONFIG_CHIP_NFC_COMMISSIONING 1
#define CHIP_DEVICE_PROJECT_CONFIG_INCLUDE 1
#define CHIP_DEVICE_CONFIG_FIRWMARE_BUILD_DATE 1
#define CHIP_DEVICE_CONFIG_ENABLE_WIFI 1
#define CHIP_DEVICE_LAYER_TARGET_NXP_ZEPHYR 1
#define CHIP_DEVICE_ENABLE_DATA_MODEL 1
#define CHIP_DEVICE_LAYER_TARGET_${possible_device_layer_target_define} 1
#define CHIP_DEVICE_CONFIG_DEVICE_SOFTWARE_VERSION 1
#define CHIP_DEVICE_CONFIG_DEVICE_SOFTWARE_VERSION_STRING 1
#define CHIP_DEVICE_CONFIG_DEVICE_VENDOR_ID 1
#define CHIP_DEVICE_CONFIG_DEVICE_PRODUCT_ID 1
#define CHIP_DEVICE_LAYER_NONE 1
#define EXTERNAL_KEYVALUESTOREMANAGERIMPL_HEADER 1
#define SYSTEM_ENABLE_CLANG_THREAD_SAFETY_ANALYSIS 1 (configuration by gn 'is_clang')
