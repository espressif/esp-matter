#include <data_model/esp_matter_attribute_utils.h>
#include <data_model/esp_matter_data_model.h>
#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_console.h>

#define TAG "attribute_console"

namespace esp_matter {
namespace console {

static esp_matter::console::engine attribute_console;

static esp_err_t console_set_handler(int argc, char **argv)
{
    VerifyOrReturnError(argc >= 4, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "The arguments for this command is invalid"));

    uint16_t endpoint_id = strtoul((const char *)&argv[0][2], NULL, 16);
    uint32_t cluster_id = strtoul((const char *)&argv[1][2], NULL, 16);
    uint32_t attribute_id = strtoul((const char *)&argv[2][2], NULL, 16);

    attribute_t *attr = attribute::get(endpoint_id, cluster_id, attribute_id);
    if (!attr) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    ESP_RETURN_ON_ERROR(attribute::get_val(attr, &val), TAG, "Failed to get current valure");
    switch (val.type) {
    case ESP_MATTER_VAL_TYPE_NULLABLE_BOOLEAN:
        if (strncmp(argv[3], "null", sizeof("null")) == 0) {
            chip::app::NumericAttributeTraits<bool>::SetNull(*(uint8_t *)(&(val.val.b)));
        } else {
            val.val.b = (atoi(argv[3]) == 1);
        }
        break;
    case ESP_MATTER_VAL_TYPE_BOOLEAN:
        val.val.b = (atoi(argv[3]) == 1);
        break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_ENUM8:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP8:
        if (strncmp(argv[3], "null", sizeof("null")) == 0) {
            chip::app::NumericAttributeTraits<uint8_t>::SetNull(val.val.u8);
        } else {
            val.val.u8 = strtoul(argv[3], nullptr, 0);
        }
        break;
    case ESP_MATTER_VAL_TYPE_UINT8:
    case ESP_MATTER_VAL_TYPE_ENUM8:
    case ESP_MATTER_VAL_TYPE_BITMAP8:
        val.val.u8 = strtoul(argv[3], nullptr, 0);
        break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_ENUM16:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP16:
        if (strncmp(argv[3], "null", sizeof("null")) == 0) {
            chip::app::NumericAttributeTraits<uint16_t>::SetNull(val.val.u16);
        } else {
            val.val.u16 = strtoul(argv[3], nullptr, 0);
        }
        break;
    case ESP_MATTER_VAL_TYPE_UINT16:
    case ESP_MATTER_VAL_TYPE_ENUM16:
    case ESP_MATTER_VAL_TYPE_BITMAP16:
        val.val.u16 = strtoul(argv[3], nullptr, 0);
        break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT32:
    case ESP_MATTER_VAL_TYPE_NULLABLE_BITMAP32:
        if (strncmp(argv[3], "null", sizeof("null")) == 0) {
            chip::app::NumericAttributeTraits<uint32_t>::SetNull(val.val.u32);
        } else {
            val.val.u32 = strtoul(argv[3], nullptr, 0);
        }
        break;
    case ESP_MATTER_VAL_TYPE_UINT32:
    case ESP_MATTER_VAL_TYPE_BITMAP32:
        val.val.u32 = strtoul(argv[3], nullptr, 0);
        break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_UINT64:
        if (strncmp(argv[3], "null", sizeof("null")) == 0) {
            chip::app::NumericAttributeTraits<uint64_t>::SetNull(val.val.u64);
        } else {
            val.val.u64 = strtoull(argv[3], nullptr, 0);
        }
        break;
    case ESP_MATTER_VAL_TYPE_UINT64:
        val.val.u64 = strtoull(argv[3], nullptr, 0);
        break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT8:
        if (strncmp(argv[3], "null", sizeof("null")) == 0) {
            chip::app::NumericAttributeTraits<int8_t>::SetNull(val.val.i8);
        } else {
            val.val.i8 = strtol(argv[3], nullptr, 0);
        }
        break;
    case ESP_MATTER_VAL_TYPE_INT8:
        val.val.i8 = strtol(argv[3], nullptr, 0);
        break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT16:
        if (strncmp(argv[3], "null", sizeof("null")) == 0) {
            chip::app::NumericAttributeTraits<int16_t>::SetNull(val.val.i16);
        } else {
            val.val.i16 = strtol(argv[3], nullptr, 0);
        }
        break;
    case ESP_MATTER_VAL_TYPE_INT16:
        val.val.i16 = strtol(argv[3], nullptr, 0);
        break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT32:
        if (strncmp(argv[3], "null", sizeof("null")) == 0) {
            chip::app::NumericAttributeTraits<int32_t>::SetNull(val.val.i32);
        } else {
            val.val.i32 = strtol(argv[3], nullptr, 0);
        }
        break;
    case ESP_MATTER_VAL_TYPE_INT32:
        val.val.i32 = strtol(argv[3], nullptr, 0);
        break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_INT64:
        if (strncmp(argv[3], "null", sizeof("null")) == 0) {
            chip::app::NumericAttributeTraits<int64_t>::SetNull(val.val.i64);
        } else {
            val.val.i64 = strtoll(argv[3], nullptr, 0);
        }
        break;
    case ESP_MATTER_VAL_TYPE_INT64:
        val.val.i64 = strtoll(argv[3], nullptr, 0);
        break;
    case ESP_MATTER_VAL_TYPE_NULLABLE_FLOAT:
        if (strncmp(argv[3], "null", sizeof("null")) == 0) {
            chip::app::NumericAttributeTraits<float>::SetNull(val.val.f);
        } else {
            val.val.f = (float)atof(argv[3]);
        }
        break;
    case ESP_MATTER_VAL_TYPE_FLOAT:
        val.val.f = (float)atof(argv[3]);
        break;
    case ESP_MATTER_VAL_TYPE_CHAR_STRING: {
        char *value = argv[3];
        val = esp_matter_char_str(value, strlen(value));
        break;
    }
    case ESP_MATTER_VAL_TYPE_LONG_CHAR_STRING: {
        char *value = argv[3];
        val = esp_matter_long_char_str(value, strlen(value));
        break;
    }
    default:
        ESP_LOGE(TAG, "Type not handled: %d", val.type);
        return ESP_ERR_INVALID_ARG;
    }
    return attribute::update(endpoint_id, cluster_id, attribute_id, &val);
}

static esp_err_t console_get_handler(int argc, char **argv)
{
    VerifyOrReturnError(argc >= 3, ESP_ERR_INVALID_ARG, ESP_LOGE(TAG, "The arguments for this command is invalid"));
    uint16_t endpoint_id = strtoul((const char *)&argv[0][2], NULL, 16);
    uint32_t cluster_id = strtoul((const char *)&argv[1][2], NULL, 16);
    uint32_t attribute_id = strtoul((const char *)&argv[2][2], NULL, 16);

    attribute_t *attr = attribute::get(endpoint_id, cluster_id, attribute_id);
    if (!attr) {
        return ESP_ERR_INVALID_ARG;
    }
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);
    ESP_RETURN_ON_ERROR(esp_matter::attribute::get_val(attr, &val), TAG, "Failed to get current valure");
    /* Here, the val_print function gets called on attribute read. */
    attribute::val_print(endpoint_id, cluster_id, attribute_id, &val, true);
    return ESP_OK;
}

static esp_err_t console_dispatch(int argc, char **argv)
{
    VerifyOrReturnError(argc > 0, ESP_OK,
                        attribute_console.for_each_command(esp_matter::console::print_description, NULL));
    return attribute_console.exec_command(argc, argv);
}

esp_err_t attribute_register_commands()
{
    static bool init_done = false;
    VerifyOrReturnError(!init_done, ESP_ERR_INVALID_STATE);
    static const esp_matter::console::command_t command = {
        .name = "attribute",
        .description = "This can be used to simulate on-device control. ",
        .handler = console_dispatch,
    };

    static const esp_matter::console::command_t attribute_commands[] = {
        {
            .name = "set",
            .description = "Set an attribute value of a cluster on an endpoint. "
                           "Usage: matter esp attribute set <endpoint_id> <cluster_id> <attribute_id> <value>. "
                           "Example: matter esp attribute set 0x0001 0x0006 0x0000 1.",
            .handler = console_set_handler,
        },
        {
            .name = "get",
            .description = "Get an attribute value of a cluster on an endpoint. "
                           "Usage: matter esp attribute get <endpoint_id> <cluster_id> <attribute_id>. "
                           "Example: matter esp attribute get 0x0001 0x0006 0x0000.",
            .handler = console_get_handler,
        },
    };
    attribute_console.register_commands(attribute_commands,
                                        sizeof(attribute_commands) / sizeof(esp_matter::console::command_t));
    esp_matter::console::add_commands(&command, 1);
    init_done = true;
    return ESP_OK;
}

} // namespace console
} // namespace esp_matter


