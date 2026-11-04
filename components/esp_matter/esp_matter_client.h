// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "sdkconfig.h"

#include <app/data-model/EncodableToTLV.h>
#include <json_to_tlv.h>

#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
#include <esp_matter_client_binding.h>
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
#include <esp_matter_client_invoke_command.h>
#include <esp_matter_client_read_command.h>
#include <esp_matter_client_subscribe_command.h>
#include <esp_matter_client_target.h>
#include <esp_matter_client_write_command.h>

namespace esp_matter {
namespace client {

/**
 * Borrowed tagged JSON containing exactly one TLV value.
 * The supplied tag replaces the JSON member's tag.
 */
class json_encodable final : public chip::app::DataModel::EncodableToTLV {
public:
    explicit json_encodable(const char *json) : mJson(json) {}

    CHIP_ERROR EncodeTo(chip::TLV::TLVWriter &writer, chip::TLV::Tag tag) const override;

private:
    const char *mJson;
};

} // namespace client
} // namespace esp_matter
