// Copyright 2022 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <esp_check.h>
#include <esp_matter_controller_utils.h>
#include <esp_matter_controller_write_command.h>
#include <json_parser.h>
#if CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
#include <esp_matter_commissioner.h>
#else
#include <app/server/Server.h>
#endif

using namespace chip::app::Clusters;
using chip::ByteSpan;
using chip::DeviceProxy;
using chip::app::DataModel::List;
using chip::Platform::New;

static const char *TAG = "write_command";

namespace esp_matter {
namespace controller {

template <class T>
void write_command<T>::on_device_connected_fcn(void *context, ExchangeManager &exchangeMgr,
                                               SessionHandle &sessionHandle)
{
    write_command<T> *cmd = (write_command<T> *)context;
    CHIP_ERROR err = CHIP_NO_ERROR;
    WriteClient *write_client =
        New<WriteClient>(&exchangeMgr, &(cmd->get_chunked_write_callback()), chip::NullOptional, false);
    if (!write_client) {
        ESP_LOGE(TAG, "Failed to alloc memory for WriteClient");
        chip::Platform::Delete(cmd);
    }
    err = write_client->EncodeAttribute(cmd->get_attribute_path(), cmd->get_attribute_val(), chip::NullOptional);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to Encode Attribute for WriteClient");
        chip::Platform::Delete(cmd);
        chip::Platform::Delete(write_client);
        return;
    }

    err = write_client->SendWriteRequest(sessionHandle);
    if (err != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to Send Write Request");
        chip::Platform::Delete(cmd);
        chip::Platform::Delete(write_client);
    }
    return;
}

template <class T>
void write_command<T>::on_device_connection_failure_fcn(void *context, const ScopedNodeId &peerId, CHIP_ERROR error)
{
    write_command<T> *cmd = (write_command<T> *)context;
    delete cmd;
    return;
}

template <class T>
esp_err_t write_command<T>::send_command()
{
#if CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
    if (CHIP_NO_ERROR ==
        commissioner::get_device_commissioner()->GetConnectedDevice(m_node_id, &on_device_connected_cb,
                                                                    &on_device_connection_failure_cb)) {
        return ESP_OK;
    }
#else
    chip::Server *server = &(chip::Server::GetInstance());
    server->GetCASESessionManager()->FindOrEstablishSession(ScopedNodeId(m_node_id, /* fabric index */ 1),
                                                            &on_device_connected_cb, &on_device_connection_failure_cb);
    return ESP_OK;
#endif
    chip::Platform::Delete(this);
    return ESP_FAIL;
}

namespace clusters {
namespace on_off {

static esp_err_t write_attribute(uint64_t node_id, uint16_t endpoint_id, uint32_t attribute_id, char *attribute_val_str)
{
    esp_err_t err = ESP_OK;
    switch (attribute_id) {
    case OnOff::Attributes::OnTime::Id:
    case OnOff::Attributes::OffWaitTime::Id: {
        write_command<uint16_t> *cmd = New<write_command<uint16_t>>(node_id, endpoint_id, OnOff::Id, attribute_id,
                                                                    string_to_uint16(attribute_val_str));
        ESP_RETURN_ON_FALSE(cmd, ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for write_command");
        return cmd->send_command();
        break;
    }
    case OnOff::Attributes::StartUpOnOff::Id: {
        write_command<uint8_t> *cmd = New<write_command<uint8_t>>(node_id, endpoint_id, OnOff::Id, attribute_id,
                                                                  string_to_uint8(attribute_val_str));
        ESP_RETURN_ON_FALSE(cmd, ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for write_command");
        return cmd->send_command();
        break;
    }
    default:
        err = ESP_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

} // namespace on_off

namespace level_control {

static esp_err_t write_attribute(uint64_t node_id, uint16_t endpoint_id, uint32_t attribute_id, char *attribute_val_str)
{
    esp_err_t err = ESP_OK;
    switch (attribute_id) {
    case LevelControl::Attributes::OnOffTransitionTime::Id:
    case LevelControl::Attributes::OnTransitionTime::Id:
    case LevelControl::Attributes::OffTransitionTime::Id: {
        write_command<uint16_t> *cmd = New<write_command<uint16_t>>(node_id, endpoint_id, LevelControl::Id,
                                                                    attribute_id, string_to_uint16(attribute_val_str));
        ESP_RETURN_ON_FALSE(cmd, ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for write_command");
        return cmd->send_command();
        break;
    }
    case LevelControl::Attributes::OnLevel::Id:
    case LevelControl::Attributes::DefaultMoveRate::Id:
    case LevelControl::Attributes::Options::Id:
    case LevelControl::Attributes::StartUpCurrentLevel::Id: {
        write_command<uint8_t> *cmd = New<write_command<uint8_t>>(node_id, endpoint_id, LevelControl::Id, attribute_id,
                                                                  string_to_uint8(attribute_val_str));
        ESP_RETURN_ON_FALSE(cmd, ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for write_command");
        return cmd->send_command();
        break;
    }
    default:
        err = ESP_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

} // namespace level_control

namespace color_control {

static esp_err_t write_attribute(uint64_t node_id, uint16_t endpoint_id, uint32_t attribute_id, char *attribute_val_str)
{
    esp_err_t err = ESP_OK;
    switch (attribute_id) {
    case ColorControl::Attributes::StartUpColorTemperatureMireds::Id: {
        write_command<uint16_t> *cmd = New<write_command<uint16_t>>(node_id, endpoint_id, ColorControl::Id,
                                                                    attribute_id, string_to_uint16(attribute_val_str));
        ESP_RETURN_ON_FALSE(cmd, ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for write_command");
        return cmd->send_command();
        break;
    }
    case ColorControl::Attributes::Options::Id: {
        write_command<uint8_t> *cmd = New<write_command<uint8_t>>(node_id, endpoint_id, ColorControl::Id, attribute_id,
                                                                  string_to_uint8(attribute_val_str));
        ESP_RETURN_ON_FALSE(cmd, ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for write_command");
        return cmd->send_command();
        break;
    }
    default:
        err = ESP_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}
} // namespace color_control

namespace access_control {

using AccessControl::AuthMode;
using AccessControl::Privilege;

constexpr size_t k_max_acl_entries = CHIP_CONFIG_EXAMPLE_ACCESS_CONTROL_MAX_ENTRIES_PER_FABRIC;
constexpr size_t k_max_subjects_per_acl = CHIP_CONFIG_EXAMPLE_ACCESS_CONTROL_MAX_SUBJECTS_PER_ENTRY;
constexpr size_t k_max_targets_per_acl = CHIP_CONFIG_EXAMPLE_ACCESS_CONTROL_MAX_TARGETS_PER_ENTRY;

using acl_obj = AccessControl::Structs::AccessControlEntry::Type;
using acl_target_obj = AccessControl::Structs::Target::Type;
typedef struct acl_attr {
    acl_obj acl_array[k_max_acl_entries];
    uint64_t subjects_array[k_max_acl_entries][k_max_subjects_per_acl];
    acl_target_obj targets_array[k_max_acl_entries][k_max_targets_per_acl];
} acl_attr_t;

static void acl_attr_free(void *ctx)
{
    acl_attr_t *attr_ptr = reinterpret_cast<acl_attr_t *>(ctx);
    chip::Platform::Delete(attr_ptr);
}

static esp_err_t parse_acl_json(char *json_str, acl_attr_t *acl, size_t *acl_size)
{
    jparse_ctx_t jctx;
    ESP_RETURN_ON_FALSE(json_parse_start(&jctx, json_str, strlen(json_str)) == 0, ESP_ERR_INVALID_ARG, TAG,
                        "Failed to parse the ACL json string on json_parse_start");
    size_t acl_index = 0;
    while (acl_index < k_max_acl_entries && json_arr_get_object(&jctx, acl_index) == 0) {
        int int_val;
        // FabricIndex
        if (json_obj_get_int(&jctx, "fabricIndex", &int_val) == 0) {
            acl->acl_array[acl_index].fabricIndex = int_val;
        }
        // Privilege
        ESP_RETURN_ON_FALSE(json_obj_get_int(&jctx, "privilege", &int_val) == 0, ESP_ERR_INVALID_ARG, TAG,
                            "Failed to get privilege from the ACL json string");
        acl->acl_array[acl_index].privilege = Privilege(int_val);
        // AuthMode
        ESP_RETURN_ON_FALSE(json_obj_get_int(&jctx, "authMode", &int_val) == 0, ESP_ERR_INVALID_ARG, TAG,
                            "Failed to get authMode from the ACL json string");
        acl->acl_array[acl_index].authMode = AuthMode(int_val);
        // Subjects
        int subjects_num = 0;
        if (json_obj_get_array(&jctx, "subjects", &subjects_num) == 0 && subjects_num > 0) {
            ESP_RETURN_ON_FALSE(subjects_num <= k_max_subjects_per_acl, ESP_ERR_INVALID_ARG, TAG,
                                "Failed to get subjects from the ACL json string: Error on subjects_num");
            for (size_t subj_index = 0; subj_index < subjects_num; ++subj_index) {
                int64_t subject_val;
                ESP_RETURN_ON_FALSE(json_arr_get_int64(&jctx, subj_index, &subject_val) == 0, ESP_ERR_INVALID_ARG, TAG,
                                    "Failed to get subjects from the ACL json string: Error on subject-%u value",
                                    subj_index);
                acl->subjects_array[acl_index][subj_index] = subject_val;
            }
            acl->acl_array[acl_index].subjects.SetNonNull(acl->subjects_array[acl_index], subjects_num);
            json_obj_leave_array(&jctx);
        } else {
            acl->acl_array[acl_index].subjects.SetNull();
        }
        // Targets
        int targets_num = 0;
        if (json_obj_get_array(&jctx, "targets", &targets_num) == 0 && targets_num > 0) {
            ESP_RETURN_ON_FALSE(targets_num <= k_max_targets_per_acl, ESP_ERR_INVALID_ARG, TAG,
                                "Failed to get targets from the ACL json string: Error on targets length");
            for (size_t targ_index = 0; targ_index < targets_num; ++targ_index) {
                ESP_RETURN_ON_FALSE(json_arr_get_object(&jctx, targ_index) == 0, ESP_ERR_INVALID_ARG, TAG,
                                    "Failed to get targets from the ACL json string: Error on targets-%u value", targ_index);
                int64_t cluster_val, device_type_val;
                int endpoint_val;
                bool exist_cluster, exist_endpoint, exist_device_type;

                exist_cluster = json_obj_get_int64(&jctx, "cluster", &cluster_val) == 0;
                exist_endpoint = json_obj_get_int(&jctx, "endpoint", &endpoint_val) == 0;
                exist_device_type = json_obj_get_int64(&jctx, "deviceType", &device_type_val) == 0;
                if ((!exist_cluster && !exist_endpoint && !exist_device_type) ||
                    (exist_endpoint && exist_device_type)) {
                    ESP_LOGE(TAG, "Target-%u value is invalid, skip it", targ_index);
                    json_arr_leave_object(&jctx);
                    continue;
                }
                // Cluster
                if (exist_cluster) {
                    acl->targets_array[acl_index][targ_index].cluster.SetNonNull(static_cast<uint32_t>(cluster_val));
                } else {
                    acl->targets_array[acl_index][targ_index].cluster.SetNull();
                }
                // Endpoint
                if (exist_endpoint) {
                    acl->targets_array[acl_index][targ_index].endpoint.SetNonNull(static_cast<uint16_t>(endpoint_val));
                } else {
                    acl->targets_array[acl_index][targ_index].endpoint.SetNull();
                }
                // DeviceType
                if (exist_device_type) {
                    acl->targets_array[acl_index][targ_index].deviceType.SetNonNull(
                        static_cast<uint32_t>(device_type_val));
                } else {
                    acl->targets_array[acl_index][targ_index].deviceType.SetNull();
                }
                json_arr_leave_object(&jctx);
            }
            acl->acl_array[acl_index].targets.SetNonNull(acl->targets_array[acl_index], targets_num);
            json_obj_leave_array(&jctx);
        } else {
            acl->acl_array[acl_index].targets.SetNull();
        }
        // Leave Object
        json_arr_leave_object(&jctx);
        acl_index++;
    }
    *acl_size = acl_index;
    return ESP_OK;
}

// The extension data may be used to store arbitrary TLV-encoded data related to a fabric’s ACL Entries.
constexpr size_t k_max_extension_entries = CHIP_CONFIG_EXAMPLE_ACCESS_CONTROL_MAX_ENTRIES_PER_FABRIC;
constexpr size_t k_max_extension_data_len = 128;

using extension_obj = AccessControl::Structs::ExtensionEntry::Type;
typedef struct extension_attr {
    extension_obj extension_array[k_max_extension_entries];
    uint8_t data_array[k_max_extension_entries][k_max_extension_data_len];
} extension_attr_t;

static void extension_attr_free(void *ctx)
{
    extension_attr_t *attr = reinterpret_cast<extension_attr_t *>(ctx);
    chip::Platform::Delete(attr);
}

static esp_err_t parse_extension_json(char *json_str, extension_attr_t *extension, size_t *extension_size)
{
    jparse_ctx_t jctx;
    ESP_RETURN_ON_FALSE(json_parse_start(&jctx, json_str, strlen(json_str)) == 0, ESP_ERR_INVALID_ARG, TAG,
                        "Failed to parse the Extension json string on json_parse_start");
    size_t index = 0;
    while (index < k_max_extension_entries && json_arr_get_object(&jctx, index) == 0) {
        int fabric_index;
        if (json_obj_get_int(&jctx, "fabricIndex", &fabric_index) == 0) {
            extension->extension_array[index].fabricIndex = fabric_index;
        }

        char data_oct_str[k_max_extension_data_len * 2 + 1] = {0};
        if (json_obj_get_string(&jctx, "data", data_oct_str, k_max_extension_data_len * 2 + 1) != 0) {
            ESP_LOGE(TAG, "Failed to get data from the Extension json string");
            return ESP_ERR_INVALID_ARG;
        } else {
            size_t data_len = oct_str_to_byte_arr(data_oct_str, extension->data_array[index]);
            ESP_RETURN_ON_FALSE(data_len > 0, ESP_ERR_INVALID_ARG, TAG,
                                "Failed to convert the data octstring to byte array");
            extension->extension_array[index].data = ByteSpan(extension->data_array[index], data_len);
        }
        json_arr_leave_object(&jctx);
        index++;
    }
    *extension_size = index;
    return ESP_OK;
}

static esp_err_t write_attribute(uint64_t node_id, uint16_t endpoint_id, uint32_t attribute_id, char *attribute_val_str)
{
    esp_err_t err = ESP_OK;
    switch (attribute_id) {
    case AccessControl::Attributes::Acl::Id: {
        size_t acl_size = 0;
        acl_attr_t *attr_val = New<acl_attr_t>();
        ESP_RETURN_ON_FALSE(attr_val, ESP_ERR_NO_MEM, TAG, "Failed to alloc acl_attr_t");
        ESP_RETURN_ON_ERROR(parse_acl_json(attribute_val_str, attr_val, &acl_size), TAG,
                            "Failed to parse the ACL json string");
        List<acl_obj> access_control_list(attr_val->acl_array, acl_size);
        write_command<List<acl_obj>> *cmd = New<write_command<List<acl_obj>>>(node_id, endpoint_id, AccessControl::Id,
                                                                              attribute_id, access_control_list);
        ESP_RETURN_ON_FALSE(cmd, ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for write_command");
        cmd->set_attribute_free_handler(acl_attr_free, attr_val);
        return cmd->send_command();
        break;
    }
    case AccessControl::Attributes::Extension::Id: {
        size_t extension_size = 0;
        extension_attr_t *attr_val = New<extension_attr_t>();
        ESP_RETURN_ON_FALSE(attr_val, ESP_ERR_NO_MEM, TAG, "Failed to alloc extension_attr_t");
        ESP_RETURN_ON_ERROR(parse_extension_json(attribute_val_str, attr_val, &extension_size), TAG,
                            "Failed to parse the Extension json string");
        List<extension_obj> extension_list(attr_val->extension_array, extension_size);
        write_command<List<extension_obj>> *cmd = New<write_command<List<extension_obj>>>(
            node_id, endpoint_id, AccessControl::Id, attribute_id, extension_list);
        ESP_RETURN_ON_FALSE(cmd, ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for write_command");
        cmd->set_attribute_free_handler(extension_attr_free, attr_val);
        return cmd->send_command();
        break;
    }
    default:
        err = ESP_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

} // namespace access_control

namespace binding {
using binding_obj = Binding::Structs::TargetStruct::Type;
typedef struct binding_attr {
    binding_obj binding_array[CONFIG_MAX_BINDINGS];
} binding_attr_t;

static void binding_attr_free(void *ctx)
{
    binding_attr_t *attr_ptr = reinterpret_cast<binding_attr_t *>(ctx);
    chip::Platform::Delete(attr_ptr);
}

static esp_err_t parse_binding_json(char *json_str, binding_attr_t *binding, size_t *binding_size)
{
    jparse_ctx_t jctx;
    ESP_RETURN_ON_FALSE(json_parse_start(&jctx, json_str, strlen(json_str)) == 0, ESP_ERR_INVALID_ARG, TAG,
                        "Failed to parse the Binding json string on json_parse_start");
    size_t index = 0;
    while (index < CONFIG_MAX_BINDINGS && json_arr_get_object(&jctx, index) == 0) {
        int int_val;
        int64_t int64_val;
        // Fabric
        if (json_obj_get_int(&jctx, "fabricIndex", &int_val) == 0) {
            binding->binding_array[index].fabricIndex = int_val;
        }

        if (json_obj_get_int64(&jctx, "node", &int64_val) == 0) {
            // Unicast Binding
            binding->binding_array[index].node.SetValue(int64_val);
            binding->binding_array[index].group.ClearValue();

            ESP_RETURN_ON_FALSE(json_obj_get_int(&jctx, "endpoint", &int_val) == 0, ESP_ERR_INVALID_ARG, TAG,
                                "Failed to get endpoint from the Binding json string");
            binding->binding_array[index].endpoint.SetValue(int_val);

            ESP_RETURN_ON_FALSE(json_obj_get_int(&jctx, "cluster", &int_val) == 0, ESP_ERR_INVALID_ARG, TAG,
                                "Failed to get cluster from the Binding json string");
            binding->binding_array[index].cluster.SetValue(int_val);
        } else if (json_obj_get_int64(&jctx, "group", &int64_val) == 0) {
            // Group binding
            binding->binding_array[index].group.SetValue(int64_val);
            binding->binding_array[index].node.ClearValue();
            binding->binding_array[index].endpoint.ClearValue();
            binding->binding_array[index].cluster.ClearValue();
        } else {
            ESP_LOGE(TAG, "The Binding json string is invalid");
            return ESP_ERR_INVALID_ARG;
        }
        json_arr_leave_object(&jctx);
        index++;
    }
    *binding_size = index;
    return ESP_OK;
}

static esp_err_t write_attribute(uint64_t node_id, uint16_t endpoint_id, uint32_t attribute_id, char *attribute_val_str)
{
    esp_err_t err = ESP_OK;
    switch (attribute_id) {
    case Binding::Attributes::Binding::Id: {
        size_t binding_size = 0;
        binding_attr_t *attr_val = chip::Platform::New<binding_attr_t>();
        ESP_RETURN_ON_FALSE(attr_val, ESP_ERR_NO_MEM, TAG, "Failed to alloc binding_attr_t");
        ESP_RETURN_ON_ERROR(parse_binding_json(attribute_val_str, attr_val, &binding_size), TAG,
                            "Failed to parse the Binding json string");
        List<binding_obj> binding_list(attr_val->binding_array, binding_size);
        write_command<List<binding_obj>> *cmd =
            New<write_command<List<binding_obj>>>(node_id, endpoint_id, Binding::Id, attribute_id, binding_list);
        ESP_RETURN_ON_FALSE(cmd, ESP_ERR_NO_MEM, TAG, "Failed to alloc memory for write_command");
        cmd->set_attribute_free_handler(binding_attr_free, attr_val);
        return cmd->send_command();
        break;
    }
    default:
        err = ESP_ERR_NOT_SUPPORTED;
        break;
    }
    return err;
}

} // namespace binding

} // namespace clusters

esp_err_t send_write_attr_command(uint64_t node_id, uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                                  char *attribute_val_str)
{
    switch (cluster_id) {
    case OnOff::Id:
        return clusters::on_off::write_attribute(node_id, endpoint_id, attribute_id, attribute_val_str);
        break;
    case LevelControl::Id:
        return clusters::level_control::write_attribute(node_id, endpoint_id, attribute_id, attribute_val_str);
        break;
    case ColorControl::Id:
        return clusters::color_control::write_attribute(node_id, endpoint_id, attribute_id, attribute_val_str);
        break;
    case AccessControl::Id:
        return clusters::access_control::write_attribute(node_id, endpoint_id, attribute_id, attribute_val_str);
        break;
    case Binding::Id:
        return clusters::binding::write_attribute(node_id, endpoint_id, attribute_id, attribute_val_str);
        break;
    default:
        return ESP_ERR_NOT_SUPPORTED;
    }
    return ESP_ERR_NOT_SUPPORTED;
}

} // namespace controller
} // namespace esp_matter
