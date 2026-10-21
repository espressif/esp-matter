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

#include <cbor.h>
#include <esp_aliro_utils.h>
#include <esp_log.h>
#include <esp_matter.h>

namespace {

constexpr uint64_t kMatterCapabilities = 0x3f; // Matter profile requires all six operations.

bool Unsigned(CborValue &value, uint64_t &out, uint64_t max)
{
    if (!cbor_value_is_unsigned_integer(&value) || cbor_value_get_uint64(&value, &out) != CborNoError || out > max) {
        return false;
    }
    return cbor_value_advance_fixed(&value) == CborNoError;
}

bool Container(CborValue &parent, CborValue &child, bool map, size_t max, size_t &count)
{
    if (map ? !cbor_value_is_map(&parent) : !cbor_value_is_array(&parent)) {
        return false;
    }
    CborError err = map ? cbor_value_get_map_length(&parent, &count) : cbor_value_get_array_length(&parent, &count);
    return err == CborNoError && count > 0 && count <= max &&
           cbor_value_enter_container(&parent, &child) == CborNoError;
}

bool Bytes(CborValue &value, size_t min, size_t max)
{
    size_t size;
    return cbor_value_is_byte_string(&value) && cbor_value_get_string_length(&value, &size) == CborNoError &&
           size >= min && size <= max && cbor_value_advance(&value) == CborNoError;
}

bool ReadExtensionList(CborValue &value, bool accessExtensions)
{
    CborValue entries;
    size_t count;
    if (!Container(value, entries, false, 8, count)) {
        return false;
    }
    const size_t fieldCount = accessExtensions ? 4 : 3;
    for (size_t i = 0; i < count; ++i) {
        CborValue fields;
        size_t length;
        uint64_t ignored;
        if (!Container(entries, fields, false, fieldCount, length) || length != fieldCount) {
            return false;
        }
        // Unknown access extensions may be ignored only with explicit noncriticality.
        if (accessExtensions && !Unsigned(fields, ignored, 0)) {
            return false;
        }
        if (!Unsigned(fields, ignored, UINT32_MAX) || !Unsigned(fields, ignored, UINT32_MAX) ||
                !Bytes(fields, 0, 256) || !cbor_value_at_end(&fields) ||
                cbor_value_leave_container(&entries, &fields) != CborNoError) {
            return false;
        }
    }
    return cbor_value_leave_container(&value, &entries) == CborNoError;
}

bool ReadExtensions(CborValue &value, bool accessExtensions)
{
    CborValue vendors;
    size_t count;
    if (!Container(value, vendors, true, 8, count)) {
        return false;
    }
    uint64_t vendorIds[8] = {};
    for (size_t i = 0; i < count; ++i) {
        if (!Unsigned(vendors, vendorIds[i], 0xffffff)) {
            return false;
        }
        for (size_t j = 0; j < i; ++j) {
            if (vendorIds[j] == vendorIds[i]) {
                return false;
            }
        }
        if (!ReadExtensionList(vendors, accessExtensions)) {
            return false;
        }
    }
    return cbor_value_leave_container(&value, &vendors) == CborNoError;
}

bool ReadRule(CborValue &value, bool &allow)
{
    CborValue fields;
    size_t count;
    if (!Container(value, fields, true, 3, count)) {
        return false;
    }
    unsigned seen = 0;
    uint64_t capabilities = 0;
    for (size_t i = 0; i < count; ++i) {
        uint64_t label, mask;
        if (!Unsigned(fields, label, 2) || (seen & (1u << label))) {
            return false;
        }
        seen |= 1u << label;
        if (!Unsigned(fields, mask, label == 0 ? kMatterCapabilities : 255)) {
            return false;
        }
        if (label == 0) {
            capabilities = mask;
        } else if (mask != 0) {
            return false; // Schedules/deny masks must not be silently ignored.
        }
    }
    if (!cbor_value_at_end(&fields) || cbor_value_leave_container(&value, &fields) != CborNoError) {
        return false;
    }
    allow |= capabilities == kMatterCapabilities;
    return true;
}

bool ReadRules(CborValue &value, bool &allow)
{
    CborValue rules;
    size_t count;
    if (!Container(value, rules, false, 8, count)) {
        return false;
    }
    for (size_t i = 0; i < count; ++i) {
        if (!ReadRule(rules, allow)) {
            return false;
        }
    }
    return cbor_value_leave_container(&value, &rules) == CborNoError;
}

bool ValidateMatterAccessData(const uint8_t *encoded, size_t size)
{
    if (!encoded || size == 0 || size > 2048) {
        return false;
    }
    CborParser parser;
    CborValue value, fields;
    size_t count;
    if (cbor_parser_init(encoded, size, 0, &parser, &value) != CborNoError ||
            !Container(value, fields, true, 7, count)) {
        return false;
    }
    unsigned seen = 0;
    bool allow = false;
    for (size_t i = 0; i < count; ++i) {
        uint64_t label, version;
        if (!Unsigned(fields, label, 6) || (seen & (1u << label))) {
            return false;
        }
        seen |= 1u << label;
        switch (label) {
        case 0:
            if (!Unsigned(fields, version, 1) || version != 1) {
                return false;
            }
            break;
        case 1:
            if (!Bytes(fields, 1, 16)) {
                return false;
            }
            break;
        case 2:
            if (!ReadRules(fields, allow)) {
                return false;
            }
            break;
        case 5:
        case 6:
            if (!ReadExtensions(fields, label == 6)) {
                return false;
            }
            break;
        default:
            // No Aliro schedules or reader-rule ID semantics.
            return false;
        }
    }
    return (seen & 1) && allow && cbor_value_at_end(&fields) &&
           cbor_value_leave_container(&value, &fields) == CborNoError &&
           cbor_value_get_next_byte(&value) == encoded + size;
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
        proof.capabilityAllowed = ValidateMatterAccessData(verified.encoded_element, verified.encoded_element_size);
    }
    // A verified higher iteration reaches finalization even for a denied profile.
    return found ? ESP_OK : ESP_ERR_INVALID_RESPONSE;
}
