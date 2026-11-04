// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
// SPDX-License-Identifier: Apache-2.0

#include <esp_matter_client.h>
#include <lib/support/CHIPMem.h>
#include <lib/support/CodeUtils.h>
#include <transport/raw/MessageHeader.h>

namespace esp_matter {
namespace client {

CHIP_ERROR json_encodable::EncodeTo(chip::TLV::TLVWriter &writer, chip::TLV::Tag tag) const
{
    VerifyOrReturnError(mJson != nullptr, CHIP_ERROR_INVALID_ARGUMENT);
    chip::Platform::ScopedMemoryBufferWithSize<uint8_t> buffer;
    VerifyOrReturnError(buffer.Alloc(chip::kMaxAppMessageLen), CHIP_ERROR_NO_MEMORY);
    chip::TLV::TLVWriter encoded;
    encoded.Init(buffer.Get(), buffer.AllocatedSize());
    esp_err_t err = json_to_tlv(mJson, encoded, chip::TLV::AnonymousTag());
    if (err != ESP_OK) {
        return err == ESP_ERR_INVALID_ARG ? CHIP_ERROR_INVALID_ARGUMENT : CHIP_ERROR_INTERNAL;
    }
    chip::TLV::TLVReader value;
    value.Init(buffer.Get(), encoded.GetLengthWritten());
    ReturnErrorOnFailure(value.Next());
    chip::TLV::TLVType type;
    ReturnErrorOnFailure(value.EnterContainer(type));
    VerifyOrReturnError(value.Next() == CHIP_NO_ERROR, CHIP_ERROR_INVALID_ARGUMENT);
    chip::TLV::TLVReader next = value;
    VerifyOrReturnError(next.Next() == CHIP_END_OF_TLV, CHIP_ERROR_INVALID_ARGUMENT);
    return writer.CopyElement(tag, value);
}

} // namespace client
} // namespace esp_matter
