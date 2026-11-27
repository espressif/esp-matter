/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "aliro_document.h"
#include "time_sync.h"
#include "door_lock_storage.h"

#include <string.h>

#include <esp_aliro_utils.h>
#include <esp_log.h>
#include <esp_matter.h>

namespace {

constexpr uint16_t kMatterCapabilities = 0x3f; // Matter profile requires all six operations.

struct ExtensionLimits {
    bool isAccess = false;
    uint32_t vendorId = 0;
    unsigned vendors = 0;
    unsigned entries = 0;
};

esp_err_t CheckExtension(const esp_aliro_access_extension_t *extension, void *context)
{
    auto &limits = *static_cast<ExtensionLimits *>(context);
    if (limits.isAccess != extension->is_access) {
        limits = {};
        limits.isAccess = extension->is_access;
    }
    if (limits.vendors == 0 || limits.vendorId != extension->vendor_id) {
        limits.vendorId = extension->vendor_id;
        ++limits.vendors;
        limits.entries = 0;
    }
    if (limits.vendors > 8 || ++limits.entries > 8 || extension->criticality_flags != 0 ||
            extension->extension_id > UINT32_MAX || extension->version > UINT32_MAX || extension->payload_size > 256) {
        return ESP_ERR_NOT_SUPPORTED;
    }
    return ESP_OK;
}

bool ValidateMatterAccessData(const esp_aliro_verified_access_document_t &verified)
{
    esp_aliro_access_data_t data = {};
    ExtensionLimits limits;
    if (verified.encoded_element_size > 2048 ||
            esp_aliro_utils_access_data_parse(&verified, CheckExtension, &limits, &data) != ESP_OK ||
            data.reader_rule_id_count != 0) {
        return false;
    }
    bool allow = false;
    for (uint8_t i = 0; i < data.access_rule_count; ++i) {
        const uint16_t mask = data.access_rule_capabilities[i];
        if ((mask & ~kMatterCapabilities) != 0) {
            return false;
        }
        allow |= mask == kMatterCapabilities;
    }
    return allow;
}

bool CopyDate(const uint8_t (&date)[20], char (&out)[21])
{
    if (date[19] != 'Z') {
        return false;
    }
    memcpy(out, date, 20);
    out[20] = 0;
    return true;
}

} // namespace

bool AliroDocument::CurrentTimeValid(const AliroProof &proof)
{
    time_t now;
    uint32_t generation;
    struct tm utc;
    char text[21];
    return TimeSync::Now(now, generation) && generation == proof.clockGeneration && gmtime_r(&now, &utc) &&
           strftime(text, sizeof(text), "%Y-%m-%dT%H:%M:%SZ", &utc) == 20 && strcmp(text, proof.validFrom) >= 0 &&
           strcmp(text, proof.validUntil) <= 0;
}

esp_err_t AliroDocument::Verify(const uint8_t *response, size_t responseSize,
                                const esp_aliro_expedited_result_t &authentication, AliroProof &proof)
{
    time_t now;
    uint32_t generation;
    esp_aliro_access_document_t document = {};
    esp_err_t err = esp_aliro_utils_access_document_parse(response, responseSize, &document);
    if (err != ESP_OK || !document.document) {
        return ESP_ERR_INVALID_RESPONSE;
    }
    bool found = false;
    for (uint16_t i = 1; i <= DoorLockCapabilities::kAliroCredentialIssuerKeySlots; ++i) {
        DoorLockStorage::CredentialRecord issuer;
        {
            esp_matter::lock::ScopedChipStackLock lock(portMAX_DELAY);
            DoorLockStorage &store = DoorLockStorage::Instance();
            if (store.Generation() != proof.generation ||
                    !store.ReadCredential(CredentialTypeEnum::kAliroCredentialIssuerKey, i, issuer)) {
                return ESP_ERR_INVALID_STATE;
            }
        }
        if (issuer.status != DlCredentialStatus::kOccupied) {
            continue;
        }
        if (!TimeSync::Now(now, generation) || generation != proof.clockGeneration) {
            return ESP_ERR_INVALID_STATE;
        }
        int64_t unixTime = now;
        esp_aliro_document_verification_params_t params = {};
        params.authenticated_credential_public_key = authentication.access_credential_public_key;
        params.authenticated_credential_public_key_size = sizeof(authentication.access_credential_public_key);
        params.trusted_issuer_public_key = issuer.data;
        params.trusted_issuer_public_key_size = issuer.size;
        params.trusted_current_unix_time_seconds = &unixTime;
        params.access_iteration = issuer.iteration;
        esp_aliro_verified_access_document_t verified = {};
        if (esp_aliro_utils_access_document_verify(&document, kElement, sizeof(kElement), &params, &verified) != ESP_OK) {
            continue;
        }
        if (found) {
            return ESP_ERR_INVALID_RESPONSE; // Never choose one of ambiguous trust records.
        }
        if (authentication.has_credential_signed_timestamp &&
                memcmp(authentication.credential_signed_timestamp, verified.signed_timestamp, 20) != 0) {
            return ESP_ERR_INVALID_RESPONSE;
        }
        if (!CopyDate(verified.valid_from, proof.validFrom) || !CopyDate(verified.valid_until, proof.validUntil)) {
            return ESP_ERR_INVALID_RESPONSE;
        }
        found = true;
        proof.issuerPath = true;
        proof.credential = {issuer.ownerUserIndex, i, CredentialTypeEnum::kAliroCredentialIssuerKey};
        proof.hasIteration = verified.has_document_access_iteration;
        proof.iteration = verified.document_access_iteration;
        proof.capabilityAllowed = ValidateMatterAccessData(verified);
    }
    // A verified higher iteration reaches finalization even for a denied profile.
    return found ? ESP_OK : ESP_ERR_INVALID_RESPONSE;
}
