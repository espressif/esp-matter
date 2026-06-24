// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
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

/* THIS IS A GENERATED FILE, DO NOT EDIT */

#include <esp_log.h>
#include <esp_matter_core.h>
#include <esp_matter.h>

#include <app-common/zap-generated/cluster-enums.h>
#include <app-common/zap-generated/callback.h>
#include <app/InteractionModelEngine.h>
#include <zap_common/app/PluginApplicationCallbacks.h>
#include <joint_fabric_datastore.h>
#include <joint_fabric_datastore_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;

static const char *TAG = "joint_fabric_datastore_cluster";
constexpr uint16_t cluster_revision = 1;

static esp_err_t esp_matter_command_callback_add_key_set(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                         void *opaque_ptr)
{
    chip::app::Clusters::JointFabricDatastore::Commands::AddKeySet::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfJointFabricDatastoreClusterAddKeySetCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_update_key_set(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                            void *opaque_ptr)
{
    chip::app::Clusters::JointFabricDatastore::Commands::UpdateKeySet::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfJointFabricDatastoreClusterUpdateKeySetCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_remove_key_set(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                            void *opaque_ptr)
{
    chip::app::Clusters::JointFabricDatastore::Commands::RemoveKeySet::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfJointFabricDatastoreClusterRemoveKeySetCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_add_group(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                       void *opaque_ptr)
{
    chip::app::Clusters::JointFabricDatastore::Commands::AddGroup::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfJointFabricDatastoreClusterAddGroupCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_update_group(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                          void *opaque_ptr)
{
    chip::app::Clusters::JointFabricDatastore::Commands::UpdateGroup::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfJointFabricDatastoreClusterUpdateGroupCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_remove_group(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                          void *opaque_ptr)
{
    chip::app::Clusters::JointFabricDatastore::Commands::RemoveGroup::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfJointFabricDatastoreClusterRemoveGroupCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_add_admin(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                       void *opaque_ptr)
{
    chip::app::Clusters::JointFabricDatastore::Commands::AddAdmin::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfJointFabricDatastoreClusterAddAdminCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_update_admin(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                          void *opaque_ptr)
{
    chip::app::Clusters::JointFabricDatastore::Commands::UpdateAdmin::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfJointFabricDatastoreClusterUpdateAdminCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_remove_admin(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                          void *opaque_ptr)
{
    chip::app::Clusters::JointFabricDatastore::Commands::RemoveAdmin::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfJointFabricDatastoreClusterRemoveAdminCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_add_pending_node(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                              void *opaque_ptr)
{
    chip::app::Clusters::JointFabricDatastore::Commands::AddPendingNode::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfJointFabricDatastoreClusterAddPendingNodeCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_refresh_node(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                          void *opaque_ptr)
{
    chip::app::Clusters::JointFabricDatastore::Commands::RefreshNode::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfJointFabricDatastoreClusterRefreshNodeCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_update_node(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                         void *opaque_ptr)
{
    chip::app::Clusters::JointFabricDatastore::Commands::UpdateNode::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfJointFabricDatastoreClusterUpdateNodeCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_remove_node(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                         void *opaque_ptr)
{
    chip::app::Clusters::JointFabricDatastore::Commands::RemoveNode::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfJointFabricDatastoreClusterRemoveNodeCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_update_endpoint_for_node(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                      void *opaque_ptr)
{
    chip::app::Clusters::JointFabricDatastore::Commands::UpdateEndpointForNode::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfJointFabricDatastoreClusterUpdateEndpointForNodeCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_add_group_id_to_endpoint_for_node(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                               void *opaque_ptr)
{
    chip::app::Clusters::JointFabricDatastore::Commands::AddGroupIDToEndpointForNode::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfJointFabricDatastoreClusterAddGroupIDToEndpointForNodeCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_remove_group_id_from_endpoint_for_node(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                                    void *opaque_ptr)
{
    chip::app::Clusters::JointFabricDatastore::Commands::RemoveGroupIDFromEndpointForNode::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfJointFabricDatastoreClusterRemoveGroupIDFromEndpointForNodeCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_add_binding_to_endpoint_for_node(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                              void *opaque_ptr)
{
    chip::app::Clusters::JointFabricDatastore::Commands::AddBindingToEndpointForNode::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfJointFabricDatastoreClusterAddBindingToEndpointForNodeCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_remove_binding_from_endpoint_for_node(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                                   void *opaque_ptr)
{
    chip::app::Clusters::JointFabricDatastore::Commands::RemoveBindingFromEndpointForNode::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfJointFabricDatastoreClusterRemoveBindingFromEndpointForNodeCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_add_acl_to_node(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                             void *opaque_ptr)
{
    chip::app::Clusters::JointFabricDatastore::Commands::AddACLToNode::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfJointFabricDatastoreClusterAddACLToNodeCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_remove_acl_from_node(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                  void *opaque_ptr)
{
    chip::app::Clusters::JointFabricDatastore::Commands::RemoveACLFromNode::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfJointFabricDatastoreClusterRemoveACLFromNodeCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

namespace esp_matter {
namespace cluster {
namespace joint_fabric_datastore {

namespace attribute {
attribute_t *create_anchor_root_ca(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, AnchorRootCA::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length));
}

attribute_t *create_anchor_node_id(cluster_t *cluster, uint64_t value)
{
    return esp_matter::attribute::create(cluster, AnchorNodeID::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_anchor_vendor_id(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, AnchorVendorID::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value));
}

attribute_t *create_friendly_name(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, FriendlyName::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length));
}

attribute_t *create_group_key_set_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, GroupKeySetList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_group_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, GroupList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_node_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, NodeList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_admin_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, AdminList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_status(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Status::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_endpoint_group_id_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, EndpointGroupIDList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_endpoint_binding_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, EndpointBindingList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_node_key_set_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, NodeKeySetList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_node_acl_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, NodeACLList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

attribute_t *create_node_endpoint_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, NodeEndpointList::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_attr_val(value, length, count));
}

} /* attribute */
namespace command {
command_t *create_add_key_set(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddKeySet::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_add_key_set);
}

command_t *create_update_key_set(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, UpdateKeySet::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_update_key_set);
}

command_t *create_remove_key_set(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveKeySet::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_remove_key_set);
}

command_t *create_add_group(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddGroup::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_add_group);
}

command_t *create_update_group(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, UpdateGroup::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_update_group);
}

command_t *create_remove_group(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveGroup::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_remove_group);
}

command_t *create_add_admin(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddAdmin::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_add_admin);
}

command_t *create_update_admin(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, UpdateAdmin::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_update_admin);
}

command_t *create_remove_admin(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveAdmin::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_remove_admin);
}

command_t *create_add_pending_node(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddPendingNode::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_add_pending_node);
}

command_t *create_refresh_node(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RefreshNode::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_refresh_node);
}

command_t *create_update_node(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, UpdateNode::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_update_node);
}

command_t *create_remove_node(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveNode::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_remove_node);
}

command_t *create_update_endpoint_for_node(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, UpdateEndpointForNode::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_update_endpoint_for_node);
}

command_t *create_add_group_id_to_endpoint_for_node(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddGroupIDToEndpointForNode::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_add_group_id_to_endpoint_for_node);
}

command_t *create_remove_group_id_from_endpoint_for_node(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveGroupIDFromEndpointForNode::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_remove_group_id_from_endpoint_for_node);
}

command_t *create_add_binding_to_endpoint_for_node(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddBindingToEndpointForNode::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_add_binding_to_endpoint_for_node);
}

command_t *create_remove_binding_from_endpoint_for_node(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveBindingFromEndpointForNode::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_remove_binding_from_endpoint_for_node);
}

command_t *create_add_acl_to_node(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, AddACLToNode::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_add_acl_to_node);
}

command_t *create_remove_acl_from_node(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, RemoveACLFromNode::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_remove_acl_from_node);
}

} /* command */

const function_generic_t *function_list = NULL;

const int function_flags = CLUSTER_FLAG_NONE;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, joint_fabric_datastore::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, joint_fabric_datastore::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        static const auto plugin_server_init_cb = CALL_ONCE(MatterJointFabricDatastorePluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_anchor_root_ca(cluster, NULL, 0);
        attribute::create_anchor_node_id(cluster, 0);
        attribute::create_anchor_vendor_id(cluster, 0);
        attribute::create_friendly_name(cluster, NULL, 0);
        attribute::create_group_key_set_list(cluster, NULL, 0, 0);
        attribute::create_group_list(cluster, NULL, 0, 0);
        attribute::create_node_list(cluster, NULL, 0, 0);
        attribute::create_admin_list(cluster, NULL, 0, 0);
        attribute::create_status(cluster, NULL, 0, 0);
        attribute::create_endpoint_group_id_list(cluster, NULL, 0, 0);
        attribute::create_endpoint_binding_list(cluster, NULL, 0, 0);
        attribute::create_node_key_set_list(cluster, NULL, 0, 0);
        attribute::create_node_acl_list(cluster, NULL, 0, 0);
        attribute::create_node_endpoint_list(cluster, NULL, 0, 0);
        command::create_add_key_set(cluster);
        command::create_update_key_set(cluster);
        command::create_remove_key_set(cluster);
        command::create_add_group(cluster);
        command::create_update_group(cluster);
        command::create_remove_group(cluster);
        command::create_add_admin(cluster);
        command::create_update_admin(cluster);
        command::create_remove_admin(cluster);
        command::create_add_pending_node(cluster);
        command::create_refresh_node(cluster);
        command::create_update_node(cluster);
        command::create_remove_node(cluster);
        command::create_update_endpoint_for_node(cluster);
        command::create_add_group_id_to_endpoint_for_node(cluster);
        command::create_remove_group_id_from_endpoint_for_node(cluster);
        command::create_add_binding_to_endpoint_for_node(cluster);
        command::create_remove_binding_from_endpoint_for_node(cluster);
        command::create_add_acl_to_node(cluster);
        command::create_remove_acl_from_node(cluster);
    }

    return cluster;
}

} /* joint_fabric_datastore */
} /* cluster */
} /* esp_matter */
