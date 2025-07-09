#pragma once

#include <esp_matter_attribute_utils.h>
#include <esp_matter_data_model.h>
#include <esp_matter_mem.h>
#include <lib/support/Span.h>

#include <cassert>

namespace esp_matter {
namespace data_model {
class attribute_data_encode_buffer {
public:
    static constexpr bool kIsFabricScoped = false;
    attribute_data_encode_buffer(const esp_matter_attr_val_t attr_val)
        : m_attr_val(attr_val)
    {
    }

    CHIP_ERROR Encode(chip::TLV::TLVWriter &writer, chip::TLV::Tag tag) const;

private:
    const esp_matter_attr_val_t m_attr_val;
};

class attribute_data_decode_buffer {
public:
    static constexpr bool kIsFabricScoped = false;
    attribute_data_decode_buffer(esp_matter_val_type_t attr_val_type)
    {
        m_attr_val.type = attr_val_type;
        memset(&m_attr_val.val, 0, sizeof(m_attr_val.val));
    }

    ~attribute_data_decode_buffer()
    {
        if (m_attr_val.type == ESP_MATTER_VAL_TYPE_OCTET_STRING ||
            m_attr_val.type == ESP_MATTER_VAL_TYPE_LONG_OCTET_STRING ||
            m_attr_val.type == ESP_MATTER_VAL_TYPE_CHAR_STRING ||
            m_attr_val.type == ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING) {
            if (m_attr_val.val.a.b) {
                esp_matter_mem_free(m_attr_val.val.a.b);
            }
        }
    }

    esp_matter_attr_val_t &get_attr_val() { return m_attr_val; }

    CHIP_ERROR Decode(chip::TLV::TLVReader &reader);

private:
    esp_matter_attr_val_t m_attr_val;
};
} // namespace data_model
} // namespace esp_matter
