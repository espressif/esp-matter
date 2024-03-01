// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

#include <esp_log.h>
#include <esp_matter_attribute.h>

static const char *TAG = "esp_matter_attribute";

using namespace chip::app::Clusters;

namespace esp_matter {
namespace cluster {

namespace global {
namespace attribute {

attribute_t *create_cluster_revision(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, Globals::Attributes::ClusterRevision::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

attribute_t *create_feature_map(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, Globals::Attributes::FeatureMap::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bitmap32(value));
}

attribute_t *create_event_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Globals::Attributes::EventList::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

} /* attribute */
} /* global */

namespace descriptor {
namespace attribute {

attribute_t *create_device_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Descriptor::Attributes::DeviceTypeList::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_server_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Descriptor::Attributes::ServerList::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_client_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Descriptor::Attributes::ClientList::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_parts_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Descriptor::Attributes::PartsList::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_tag_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Descriptor::Attributes::TagList::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

} /* attribute */
} /* descriptor */

namespace actions {
namespace attribute {

attribute_t *create_action_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Actions::Attributes::ActionList::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_endpoint_lists(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Actions::Attributes::EndpointLists::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_setup_url(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, Actions::Attributes::SetupURL::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_char_str(value, length));
}

} /* attribute */
} /* actions */

namespace access_control {
namespace attribute {

attribute_t *create_acl(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, AccessControl::Attributes::Acl::Id, ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_extension(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, AccessControl::Attributes::Extension::Id, ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_subjects_per_access_control_entry(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, AccessControl::Attributes::SubjectsPerAccessControlEntry::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_targets_per_access_control_entry(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, AccessControl::Attributes::TargetsPerAccessControlEntry::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_access_control_entries_per_fabric(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, AccessControl::Attributes::AccessControlEntriesPerFabric::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

} /* attribute */
} /* access_control */

namespace basic_information {
namespace attribute {

attribute_t *create_data_model_revision(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::DataModelRevision::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_vendor_name(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::VendorName::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_vendor_id(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::VendorID::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

attribute_t *create_product_name(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::ProductName::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_product_id(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::ProductID::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

attribute_t *create_node_label(cluster_t *cluster, char *value, uint16_t length)
{
    if (length > k_max_node_label_length) {
        ESP_LOGE(TAG, "Could not create attribute, string length out of bound");
        return NULL;
    }
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::NodeLabel::Id,
                                         ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_location(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::Location::Id,
                                         ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_hardware_version(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::HardwareVersion::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_hardware_version_string(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::HardwareVersionString::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_char_str(value, length));
}

attribute_t *create_software_version(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::SoftwareVersion::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_software_version_string(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::SoftwareVersionString::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_char_str(value, length));
}

attribute_t *create_capability_minima(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::CapabilityMinima::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_array(value, length, count));
}

attribute_t *create_manufacturing_date(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::ManufacturingDate::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_char_str(value, length));
}

attribute_t *create_part_number(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::PartNumber::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_product_url(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::ProductURL::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_product_label(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::ProductLabel::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_serial_number(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::SerialNumber::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_local_config_disabled(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::LocalConfigDisabled::Id,
                                         ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_bool(value));
}

attribute_t *create_reachable(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::Reachable::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bool(value));
}

attribute_t *create_unique_id(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::UniqueID::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_product_appearance(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::ProductAppearance::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_array(value, length, count));
}

attribute_t *create_specification_version(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::SpecificationVersion::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint32(value));
}

attribute_t *create_max_paths_per_invoke(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, BasicInformation::Attributes::MaxPathsPerInvoke::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

} /* attribute */
} /* basic_information */

namespace binding {
namespace attribute {

attribute_t *create_binding(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, Binding::Attributes::Binding::Id,
                                         ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_array(value, length, count));
}

} /* attribute */
} /* binding */

namespace ota_requestor {
namespace attribute {

attribute_t *create_default_ota_providers(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, OtaSoftwareUpdateRequestor::Attributes::DefaultOTAProviders::Id,
                                         ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_update_possible(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, OtaSoftwareUpdateRequestor::Attributes::UpdatePossible::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_bool(value));
}

attribute_t *create_update_state(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, OtaSoftwareUpdateRequestor::Attributes::UpdateState::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_update_state_progress(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, OtaSoftwareUpdateRequestor::Attributes::UpdateStateProgress::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint8(value));
}

} /* attribute */
} /* ota_requestor */

namespace general_commissioning {
namespace attribute {

attribute_t *create_breadcrumb(cluster_t *cluster, uint64_t value)
{
    return esp_matter::attribute::create(cluster, GeneralCommissioning::Attributes::Breadcrumb::Id,
                                         ATTRIBUTE_FLAG_WRITABLE, esp_matter_uint64(value));
}

attribute_t *create_basic_commissioning_info(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, GeneralCommissioning::Attributes::BasicCommissioningInfo::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_array(value, length, count));
}

attribute_t *create_regulatory_config(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, GeneralCommissioning::Attributes::RegulatoryConfig::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_location_capability(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, GeneralCommissioning::Attributes::LocationCapability::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_supports_concurrent_connection(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, GeneralCommissioning::Attributes::SupportsConcurrentConnection::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_bool(value));
}

} /* attribute */
} /* general_commissioning */

namespace network_commissioning {
namespace attribute {

attribute_t *create_max_networks(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, NetworkCommissioning::Attributes::MaxNetworks::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint8(value));
}

attribute_t *create_networks(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, NetworkCommissioning::Attributes::Networks::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_scan_max_time_seconds(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, NetworkCommissioning::Attributes::ScanMaxTimeSeconds::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint8(value));
}

attribute_t *create_connect_max_time_seconds(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, NetworkCommissioning::Attributes::ConnectMaxTimeSeconds::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint8(value));
}

attribute_t *create_interface_enabled(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, NetworkCommissioning::Attributes::InterfaceEnabled::Id,
                                         ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_bool(value));
}

attribute_t *create_last_networking_status(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, NetworkCommissioning::Attributes::LastNetworkingStatus::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint8(value));
}

attribute_t *create_last_network_id(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, NetworkCommissioning::Attributes::LastNetworkID::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_octet_str(value, length));
}

attribute_t *create_last_connect_error_value(cluster_t *cluster, nullable<int32_t> value)
{
    return esp_matter::attribute::create(cluster, NetworkCommissioning::Attributes::LastConnectErrorValue::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int32(value));
}

attribute_t *create_supported_wifi_bands(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, NetworkCommissioning::Attributes::SupportedWiFiBands::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_array(value, length, count));
}

attribute_t *create_supported_thread_features(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, NetworkCommissioning::Attributes::SupportedThreadFeatures::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_thread_version(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, NetworkCommissioning::Attributes::ThreadVersion::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

} /* attribute */
} /* network_commissioning */

namespace general_diagnostics {
namespace attribute {

attribute_t *create_network_interfaces(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, GeneralDiagnostics::Attributes::NetworkInterfaces::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_array(value, length, count));
}

attribute_t *create_reboot_count(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, GeneralDiagnostics::Attributes::RebootCount::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint16(value));
}

attribute_t *create_up_time(cluster_t *cluster, uint64_t value)
{
    return esp_matter::attribute::create(cluster, GeneralDiagnostics::Attributes::UpTime::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint64(value));
}

attribute_t *create_test_event_triggers_enabled(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, GeneralDiagnostics::Attributes::TestEventTriggersEnabled::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_bool(value));
}

} /* attribute */
} /* general_diagnostics */


namespace software_diagnostics {
namespace attribute {

attribute_t *create_current_heap_high_watermark(cluster_t *cluster, uint64_t value)
{
    return esp_matter::attribute::create(cluster, SoftwareDiagnostics::Attributes::CurrentHeapHighWatermark::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint64(value));
}

} /* attribute */
} /* software_diagnostics */

namespace administrator_commissioning {
namespace attribute {

attribute_t *create_window_status(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, AdministratorCommissioning::Attributes::WindowStatus::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint8(value));
}

attribute_t *create_admin_fabric_index(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, AdministratorCommissioning::Attributes::AdminFabricIndex::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_uint16(value));
}

attribute_t *create_admin_vendor_id(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, AdministratorCommissioning::Attributes::AdminVendorId::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_uint16(value));
}

} /* attribute */
} /* administrator_commissioning */

namespace operational_credentials {
namespace attribute {

attribute_t *create_nocs(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, OperationalCredentials::Attributes::NOCs::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_array(value, length, count));
}

attribute_t *create_fabrics(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, OperationalCredentials::Attributes::Fabrics::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_array(value, length, count));
}

attribute_t *create_supported_fabrics(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, OperationalCredentials::Attributes::SupportedFabrics::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint8(value));
}

attribute_t *create_commissioned_fabrics(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, OperationalCredentials::Attributes::CommissionedFabrics::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint8(value));
}

attribute_t *create_trusted_root_certificates(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, OperationalCredentials::Attributes::TrustedRootCertificates::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_array(value, length, count));
}

attribute_t *create_current_fabric_index(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, OperationalCredentials::Attributes::CurrentFabricIndex::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint8(value));
}

} /* attribute */
} /* operational_credentials */

namespace group_key_management {
namespace attribute {

attribute_t *create_group_key_map(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, GroupKeyManagement::Attributes::GroupKeyMap::Id,
                                         ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_group_table(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, GroupKeyManagement::Attributes::GroupTable::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_max_groups_per_fabric(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, GroupKeyManagement::Attributes::MaxGroupsPerFabric::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_max_group_keys_per_fabric(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, GroupKeyManagement::Attributes::MaxGroupKeysPerFabric::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

} /* attribute */
} /* group_key_management */

namespace icd_management {
namespace attribute {
attribute_t *create_idle_mode_duration(cluster_t *cluster, uint32_t value, uint32_t min, uint32_t max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, IcdManagement::Attributes::IdleModeDuration::Id,
                                                           ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint32(min), esp_matter_uint32(max));
    return attribute;
}

attribute_t *create_active_mode_duration(cluster_t *cluster, uint32_t value, uint32_t min)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, IcdManagement::Attributes::ActiveModeDuration::Id,
                                                           ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint32(min), esp_matter_uint32(UINT32_MAX));
    return attribute;
}

attribute_t *create_active_mode_threshold(cluster_t *cluster, uint16_t value, uint16_t min)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, IcdManagement::Attributes::ActiveModeThreshold::Id,
                                                           ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint16(min), esp_matter_uint16(UINT16_MAX));
    return attribute;
}

attribute_t *create_registered_clients(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, IcdManagement::Attributes::RegisteredClients::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_array(value,length, count));
}

attribute_t *create_icd_counter(cluster_t *cluster,uint32_t value)
{
    return esp_matter::attribute::create(cluster, IcdManagement::Attributes::ICDCounter::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_uint32(value));
}

attribute_t *create_clients_supported_per_fabric(cluster_t *cluster, uint16_t value, uint16_t min)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, IcdManagement::Attributes::ClientsSupportedPerFabric::Id,
                                                           ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint16(min), esp_matter_uint16(UINT16_MAX));
    return attribute;

}

} /* attribute */
} /* icd_management */

namespace diagnostics_network_wifi {
namespace attribute {

attribute_t *create_bssid(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, WiFiNetworkDiagnostics::Attributes::Bssid::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_octet_str(value, length));
}

attribute_t *create_security_type(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, WiFiNetworkDiagnostics::Attributes::SecurityType::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_enum8(value));
}

attribute_t *create_wifi_version(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, WiFiNetworkDiagnostics::Attributes::WiFiVersion::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint8(value));
}

attribute_t *create_channel_number(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, WiFiNetworkDiagnostics::Attributes::ChannelNumber::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_rssi(cluster_t *cluster, nullable<int8_t> value)
{
    return esp_matter::attribute::create(cluster, WiFiNetworkDiagnostics::Attributes::Rssi::Id, ATTRIBUTE_FLAG_NULLABLE,
                                         esp_matter_nullable_int8(value));
}

attribute_t *create_beacon_lost_count(cluster_t *cluster, nullable<uint32_t> value)
{
    return esp_matter::attribute::create(cluster, WiFiNetworkDiagnostics::Attributes::BeaconLostCount::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint32(value));
}

attribute_t *create_beacon_rx_count(cluster_t *cluster, nullable<uint32_t> value)
{
    return esp_matter::attribute::create(cluster, WiFiNetworkDiagnostics::Attributes::BeaconRxCount::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint32(value));
}

attribute_t *create_packet_multicast_rx_count(cluster_t *cluster, nullable<uint32_t> value)
{
    return esp_matter::attribute::create(cluster, WiFiNetworkDiagnostics::Attributes::PacketMulticastRxCount::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint32(value));
}

attribute_t *create_packet_multicast_tx_count(cluster_t *cluster, nullable<uint32_t> value)
{
    return esp_matter::attribute::create(cluster, WiFiNetworkDiagnostics::Attributes::PacketMulticastTxCount::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint32(value));
}

attribute_t *create_packet_unicast_rx_count(cluster_t *cluster, nullable<uint32_t> value)
{
    return esp_matter::attribute::create(cluster, WiFiNetworkDiagnostics::Attributes::PacketUnicastRxCount::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint32(value));
}

attribute_t *create_packet_unicast_tx_count(cluster_t *cluster, nullable<uint32_t> value)
{
    return esp_matter::attribute::create(cluster, WiFiNetworkDiagnostics::Attributes::PacketUnicastTxCount::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint32(value));
}

attribute_t *create_current_max_rate(cluster_t *cluster, nullable<uint64_t> value)
{
    return esp_matter::attribute::create(cluster, WiFiNetworkDiagnostics::Attributes::CurrentMaxRate::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint64(value));
}

attribute_t *create_overrun_count(cluster_t *cluster, nullable<uint64_t> value)
{
    return esp_matter::attribute::create(cluster, WiFiNetworkDiagnostics::Attributes::OverrunCount::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint64(value));
}

} /* attribute */
} /* diagnostics_network_wifi */

namespace diagnostics_network_thread {
namespace attribute {

attribute_t *create_channel(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, ThreadNetworkDiagnostics::Attributes::Channel::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_routing_role(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, ThreadNetworkDiagnostics::Attributes::RoutingRole::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint8(value));
}

attribute_t *create_network_name(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, ThreadNetworkDiagnostics::Attributes::NetworkName::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_char_str(value, length));
}

attribute_t *create_pan_id(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, ThreadNetworkDiagnostics::Attributes::PanId::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_extended_pan_id(cluster_t *cluster, nullable<uint64_t> value)
{
    return esp_matter::attribute::create(cluster, ThreadNetworkDiagnostics::Attributes::ExtendedPanId::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint64(value));
}

attribute_t *create_mesh_local_prefix(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, ThreadNetworkDiagnostics::Attributes::MeshLocalPrefix::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_octet_str(value, length));
}

attribute_t *create_neighbor_table(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ThreadNetworkDiagnostics::Attributes::NeighborTable::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_array(value, length, count));
}

attribute_t *create_route_table(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ThreadNetworkDiagnostics::Attributes::RouteTable::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_array(value, length, count));
}

attribute_t *create_partition_id(cluster_t *cluster, nullable<uint32_t> value)
{
    return esp_matter::attribute::create(cluster, ThreadNetworkDiagnostics::Attributes::PartitionId::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint32(value));
}

attribute_t *create_weighting(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, ThreadNetworkDiagnostics::Attributes::Weighting::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint8(value));
}

attribute_t *create_data_version(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, ThreadNetworkDiagnostics::Attributes::DataVersion::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint8(value));
}

attribute_t *create_stable_data_version(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, ThreadNetworkDiagnostics::Attributes::StableDataVersion::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint8(value));
}

attribute_t *create_leader_router_id(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, ThreadNetworkDiagnostics::Attributes::LeaderRouterId::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint8(value));
}

attribute_t *create_security_policy(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ThreadNetworkDiagnostics::Attributes::SecurityPolicy::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_array(value, length, count));
}

attribute_t *create_channel_page0_mask(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, ThreadNetworkDiagnostics::Attributes::ChannelPage0Mask::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_octet_str(value, length));
}

attribute_t *create_operational_dataset_components(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster,
                                         ThreadNetworkDiagnostics::Attributes::OperationalDatasetComponents::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_array(value, length, count));
}

attribute_t *create_active_network_faults(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ThreadNetworkDiagnostics::Attributes::ActiveNetworkFaultsList::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_array(value, length, count));
}

} /* attribute */
} /* diagnostics_network_thread */

namespace diagnostics_network_ethernet {
namespace attribute {

attribute_t *create_phy_rate(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, EthernetNetworkDiagnostics::Attributes::PHYRate::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_enum8(value));
}

attribute_t *create_full_duplex(cluster_t *cluster, nullable<bool> value)
{
    return esp_matter::attribute::create(cluster, EthernetNetworkDiagnostics::Attributes::FullDuplex::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_bool(value));
}

attribute_t *create_packet_rx_count(cluster_t *cluster, uint64_t value)
{
    return esp_matter::attribute::create(cluster, EthernetNetworkDiagnostics::Attributes::PacketRxCount::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint64(value));
}

attribute_t *create_packet_tx_count(cluster_t *cluster, uint64_t value)
{
    return esp_matter::attribute::create(cluster, EthernetNetworkDiagnostics::Attributes::PacketTxCount::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint64(value));
}

attribute_t *create_collision_count(cluster_t *cluster, uint64_t value)
{
    return esp_matter::attribute::create(cluster, EthernetNetworkDiagnostics::Attributes::CollisionCount::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint64(value));
}

attribute_t *create_overrun_count(cluster_t *cluster, uint64_t value)
{
    return esp_matter::attribute::create(cluster, EthernetNetworkDiagnostics::Attributes::OverrunCount::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint64(value));
}

attribute_t *create_carrier_detect(cluster_t *cluster, nullable<bool> value)
{
    return esp_matter::attribute::create(cluster, EthernetNetworkDiagnostics::Attributes::CarrierDetect::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_bool(value));
}

attribute_t *create_time_since_reset(cluster_t *cluster, uint64_t value)
{
    return esp_matter::attribute::create(cluster, EthernetNetworkDiagnostics::Attributes::TimeSinceReset::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint64(value));
}

} /* attribute */
} /* diagnostics_network_ethernet */

namespace bridged_device_basic_information {
namespace attribute {

attribute_t *create_vendor_name(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BridgedDeviceBasicInformation::Attributes::VendorName::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_vendor_id(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, BridgedDeviceBasicInformation::Attributes::VendorID::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

attribute_t *create_product_name(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BridgedDeviceBasicInformation::Attributes::ProductName::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_node_label(cluster_t *cluster, char *value, uint16_t length)
{
    if (length > k_max_node_label_length) {
        ESP_LOGE(TAG, "Could not create attribute, string length out of bound");
        return NULL;
    }
    return esp_matter::attribute::create(cluster, BridgedDeviceBasicInformation::Attributes::NodeLabel::Id,
                                         ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_hardware_version(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, BridgedDeviceBasicInformation::Attributes::HardwareVersion::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_hardware_version_string(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BridgedDeviceBasicInformation::Attributes::HardwareVersionString::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_char_str(value, length));
}

attribute_t *create_software_version(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, BridgedDeviceBasicInformation::Attributes::SoftwareVersion::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_software_version_string(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BridgedDeviceBasicInformation::Attributes::SoftwareVersionString::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_char_str(value, length));
}

attribute_t *create_manufacturing_date(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BridgedDeviceBasicInformation::Attributes::ManufacturingDate::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_char_str(value, length));
}

attribute_t *create_part_number(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BridgedDeviceBasicInformation::Attributes::PartNumber::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_product_url(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BridgedDeviceBasicInformation::Attributes::ProductURL::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_product_label(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BridgedDeviceBasicInformation::Attributes::ProductLabel::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_serial_number(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BridgedDeviceBasicInformation::Attributes::SerialNumber::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_reachable(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, BridgedDeviceBasicInformation::Attributes::Reachable::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bool(value));
}

attribute_t *create_unique_id(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, BridgedDeviceBasicInformation::Attributes::UniqueID::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_product_appearance(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, BridgedDeviceBasicInformation::Attributes::ProductAppearance::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_array(value, length, count));
}

} /* attribute */
} /* bridged_device_basic */

namespace user_label {
namespace attribute {

attribute_t *create_label_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, UserLabel::Attributes::LabelList::Id,
                                         ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_array(value, length, count));
}

} /* attribute */
} /* user_label */

namespace fixed_label {
namespace attribute {

attribute_t *create_label_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, FixedLabel::Attributes::LabelList::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_array(value, length, count));
}

} /* attribute */
} /* fixed_label */

namespace identify {
namespace attribute {

attribute_t *create_identify_time(cluster_t *cluster, uint16_t value, uint16_t min, uint16_t max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, Identify::Attributes::IdentifyTime::Id,
                                                           ATTRIBUTE_FLAG_WRITABLE, esp_matter_uint16(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint16(min), esp_matter_uint16(max));
    return attribute;
}

attribute_t *create_identify_type(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Identify::Attributes::IdentifyType::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

} /* attribute */
} /* identify */

namespace groups {
namespace attribute {

attribute_t *create_group_name_support(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Groups::Attributes::NameSupport::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bitmap8(value));
}

} /* attribute */
} /* groups */

namespace scenes_management {
namespace attribute {
attribute_t *create_last_configured_by(cluster_t *cluster, uint64_t value)
{
    return esp_matter::attribute::create(cluster, ScenesManagement::Attributes::LastConfiguredBy::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint64(value));
}

attribute_t *create_scene_table_size(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ScenesManagement::Attributes::SceneTableSize::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

attribute_t *create_fabric_scene_info(cluster_t *cluster, const uint8_t * value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ScenesManagement::Attributes::FabricSceneInfo::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array((uint8_t*)value, length, count));
}

} /* attribute */
} /* scenes_management */

namespace on_off {
namespace attribute {

attribute_t *create_on_off(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, OnOff::Attributes::OnOff::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_bool(value));
}

attribute_t *create_global_scene_control(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, OnOff::Attributes::GlobalSceneControl::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bool(value));
}

attribute_t *create_on_time(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, OnOff::Attributes::OnTime::Id,
                                         ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE,
                                         esp_matter_nullable_uint16(value));
}

attribute_t *create_off_wait_time(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, OnOff::Attributes::OffWaitTime::Id,
                                         ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE,
                                         esp_matter_nullable_uint16(value));
}

attribute_t *create_start_up_on_off(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, OnOff::Attributes::StartUpOnOff::Id,
                                         ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_NULLABLE,
                                         esp_matter_nullable_enum8(value));
}

} /* attribute */
} /* on_off */

namespace level_control {
namespace attribute {

attribute_t *create_current_level(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, LevelControl::Attributes::CurrentLevel::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_NULLABLE,
                                         esp_matter_nullable_uint8(value));
}

attribute_t *create_on_level(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, LevelControl::Attributes::OnLevel::Id,
                                         ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE,
                                         esp_matter_nullable_uint8(value));
}

attribute_t *create_options(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, LevelControl::Attributes::Options::Id,
                                                           ATTRIBUTE_FLAG_WRITABLE, esp_matter_bitmap8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_bitmap8(min), esp_matter_bitmap8(max));
    return attribute;
}

attribute_t *create_remaining_time(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, LevelControl::Attributes::RemainingTime::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

attribute_t *create_min_level(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, LevelControl::Attributes::MinLevel::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

attribute_t *create_max_level(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, LevelControl::Attributes::MaxLevel::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

attribute_t *create_current_frequency(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, LevelControl::Attributes::CurrentFrequency::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

attribute_t *create_min_frequency(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, LevelControl::Attributes::MinFrequency::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

attribute_t *create_max_frequency(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, LevelControl::Attributes::MaxFrequency::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

attribute_t *create_on_off_transition_time(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, LevelControl::Attributes::OnOffTransitionTime::Id,
                                         ATTRIBUTE_FLAG_WRITABLE, esp_matter_uint16(value));
}

attribute_t *create_on_transition_time(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, LevelControl::Attributes::OnTransitionTime::Id,
                                         ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE,
                                         esp_matter_nullable_uint16(value));
}

attribute_t *create_off_transition_time(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, LevelControl::Attributes::OffTransitionTime::Id,
                                         ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE,
                                         esp_matter_nullable_uint16(value));
}

attribute_t *create_default_move_rate(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, LevelControl::Attributes::DefaultMoveRate::Id,
                                         ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE,
                                         esp_matter_nullable_uint8(value));
}

attribute_t *create_start_up_current_level(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, LevelControl::Attributes::StartUpCurrentLevel::Id,
                                         ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_nullable_uint8(value));
}

} /* attribute */
} /* level_control */

namespace color_control {
namespace attribute {

attribute_t *create_current_hue(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max)
{
    attribute_t *attribute =
        esp_matter::attribute::create(cluster, ColorControl::Attributes::CurrentHue::Id,
               		              ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint8(min), esp_matter_uint8(max));
    return attribute;
}

attribute_t *create_current_saturation(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max)
{
    attribute_t *attribute =
        esp_matter::attribute::create(cluster, ColorControl::Attributes::CurrentSaturation::Id,
                                      ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint8(min), esp_matter_uint8(max));
    return attribute;
}

attribute_t *create_remaining_time(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ColorControl::Attributes::RemainingTime::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint16(value));
}

attribute_t *create_color_mode(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ColorControl::Attributes::ColorMode::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_enum8(value));
}

attribute_t *create_color_control_options(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ColorControl::Attributes::Options::Id, ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_bitmap8(value));
}

attribute_t *create_enhanced_color_mode(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max)
{
    attribute_t *attribute =
        esp_matter::attribute::create(cluster, ColorControl::Attributes::EnhancedColorMode::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_enum8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_enum8(min), esp_matter_enum8(max));
    return attribute;
}

attribute_t *create_color_capabilities(cluster_t *cluster, uint16_t value, uint16_t min, uint16_t max)
{
    attribute_t *attribute =
        esp_matter::attribute::create(cluster, ColorControl::Attributes::ColorCapabilities::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bitmap16(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_bitmap16(min), esp_matter_bitmap16(max));
    return attribute;
}

attribute_t *create_color_temperature_mireds(cluster_t *cluster, uint16_t value, uint16_t min, uint16_t max)
{
    attribute_t *attribute =
        esp_matter::attribute::create(cluster, ColorControl::Attributes::ColorTemperatureMireds::Id,
                                      ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint16(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint16(min), esp_matter_uint16(max));
    return attribute;
}

attribute_t *create_color_temp_physical_min_mireds(cluster_t *cluster, uint16_t value, uint16_t min, uint16_t max)
{
    attribute_t *attribute =
        esp_matter::attribute::create(cluster, ColorControl::Attributes::ColorTempPhysicalMinMireds::Id,
                                      ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint16(min), esp_matter_uint16(max));
    return attribute;
}

attribute_t *create_color_temp_physical_max_mireds(cluster_t *cluster, uint16_t value, uint16_t min, uint16_t max)
{
    attribute_t *attribute =
        esp_matter::attribute::create(cluster, ColorControl::Attributes::ColorTempPhysicalMaxMireds::Id,
                                      ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint16(min), esp_matter_uint16(max));
    return attribute;
}

attribute_t *create_couple_color_temp_to_level_min_mireds(cluster_t *cluster, uint16_t value, uint16_t min, uint16_t max)
{
    attribute_t *attribute =
        esp_matter::attribute::create(cluster, ColorControl::Attributes::CoupleColorTempToLevelMinMireds::Id,
                                      ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint16(min), esp_matter_uint16(max));
    return attribute;
}

attribute_t *create_startup_color_temperature_mireds(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, ColorControl::Attributes::StartUpColorTemperatureMireds::Id,
                                      ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE,
                                      esp_matter_nullable_uint16(value));
}

attribute_t *create_current_x(cluster_t *cluster, uint16_t value, uint16_t min, uint16_t max)
{
    attribute_t *attribute =
        esp_matter::attribute::create(cluster, ColorControl::Attributes::CurrentX::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_uint16(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint16(min), esp_matter_uint16(max));
    return attribute;
}

attribute_t *create_current_y(cluster_t *cluster, uint16_t value, uint16_t min, uint16_t max)
{
    attribute_t *attribute =
        esp_matter::attribute::create(cluster, ColorControl::Attributes::CurrentY::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_uint16(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint16(min), esp_matter_uint16(max));
    return attribute;
}

attribute_t *create_drift_compensation(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max)
{
    attribute_t *attribute =
	esp_matter::attribute::create(cluster, ColorControl::Attributes::DriftCompensation::Id,
			              ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_enum8(min), esp_matter_enum8(max));
    return attribute;
}

attribute_t *create_compensation_text(cluster_t *cluster, char *value, uint16_t length)
{
    if (length > k_max_compensation_text_length) {
        ESP_LOGE(TAG, "Could not create attribute, string length out of bound");
        return NULL;
    }
    return esp_matter::attribute::create(cluster, ColorControl::Attributes::CompensationText::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_enhanced_current_hue(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ColorControl::Attributes::EnhancedCurrentHue::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint16(value));
}

attribute_t *create_color_loop_active(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ColorControl::Attributes::ColorLoopActive::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint8(value));
}

attribute_t *create_color_loop_direction(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ColorControl::Attributes::ColorLoopDirection::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint8(value));
}

attribute_t *create_color_loop_time(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ColorControl::Attributes::ColorLoopTime::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint16(value));
}

attribute_t *create_color_loop_start_enhanced_hue(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ColorControl::Attributes::ColorLoopStartEnhancedHue::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_color_loop_stored_enhanced_hue(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, ColorControl::Attributes::ColorLoopStoredEnhancedHue::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_number_of_primaries(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, ColorControl::Attributes::NumberOfPrimaries::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint8(value));
}

attribute_t *create_primary_n_x(cluster_t *cluster, uint16_t value, uint8_t index)
{
    switch (index) {
    case 1:
        return esp_matter::attribute::create(cluster, ColorControl::Attributes::Primary1X::Id, ATTRIBUTE_FLAG_NONE,
                                                 esp_matter_uint16(value));
	break;
    case 2:
        return esp_matter::attribute::create(cluster, ColorControl::Attributes::Primary2X::Id, ATTRIBUTE_FLAG_NONE,
                                             esp_matter_uint16(value));
        break;
    case 3:
        return esp_matter::attribute::create(cluster, ColorControl::Attributes::Primary3X::Id, ATTRIBUTE_FLAG_NONE,
                                             esp_matter_uint16(value));
        break;
    case 4:
        return esp_matter::attribute::create(cluster, ColorControl::Attributes::Primary4X::Id, ATTRIBUTE_FLAG_NONE,
                                             esp_matter_uint16(value));
        break;
    case 5:
        return esp_matter::attribute::create(cluster, ColorControl::Attributes::Primary5X::Id, ATTRIBUTE_FLAG_NONE,
                                             esp_matter_uint16(value));
        break;
    case 6:
        return esp_matter::attribute::create(cluster, ColorControl::Attributes::Primary6X::Id, ATTRIBUTE_FLAG_NONE,
                                             esp_matter_uint16(value));
        break;
    default:
        break;
    }
    return NULL;

}

attribute_t *create_primary_n_y(cluster_t *cluster, uint16_t value, uint8_t index)
{
    switch (index) {
    case 1:
         return esp_matter::attribute::create(cluster, ColorControl::Attributes::Primary1Y::Id, ATTRIBUTE_FLAG_NONE,
                                             esp_matter_uint16(value));
        break;
    case 2:
        return esp_matter::attribute::create(cluster, ColorControl::Attributes::Primary2Y::Id, ATTRIBUTE_FLAG_NONE,
                                             esp_matter_uint16(value));
        break;
    case 3:
        return esp_matter::attribute::create(cluster, ColorControl::Attributes::Primary3Y::Id, ATTRIBUTE_FLAG_NONE,
                                             esp_matter_uint16(value));
        break;
    case 4:
        return esp_matter::attribute::create(cluster, ColorControl::Attributes::Primary4Y::Id, ATTRIBUTE_FLAG_NONE,
                                             esp_matter_uint16(value));
        break;
    case 5:
        return esp_matter::attribute::create(cluster, ColorControl::Attributes::Primary5Y::Id, ATTRIBUTE_FLAG_NONE,
                                             esp_matter_uint16(value));
        break;
    case 6:
        return esp_matter::attribute::create(cluster, ColorControl::Attributes::Primary6Y::Id, ATTRIBUTE_FLAG_NONE,
                                             esp_matter_uint16(value));
        break;
    default:
        break;
    }
    return NULL;
}

attribute_t *create_primary_n_intensity(cluster_t *cluster, nullable<uint8_t> value, uint8_t index)
{
    switch (index) {
    case 1:
        return esp_matter::attribute::create(cluster, ColorControl::Attributes::Primary1Intensity::Id,
                                             ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint8(value));
        break;
    case 2:
        return esp_matter::attribute::create(cluster, ColorControl::Attributes::Primary2Intensity::Id,
                                             ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint8(value));
        break;
    case 3:
        return esp_matter::attribute::create(cluster, ColorControl::Attributes::Primary3Intensity::Id,
                                             ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint8(value));
        break;
    case 4:
        return esp_matter::attribute::create(cluster, ColorControl::Attributes::Primary4Intensity::Id,
                                             ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint8(value));
        break;
    case 5:
        return esp_matter::attribute::create(cluster, ColorControl::Attributes::Primary5Intensity::Id,
                                             ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint8(value));
        break;
    case 6:
        return esp_matter::attribute::create(cluster, ColorControl::Attributes::Primary6Intensity::Id,
                                             ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint8(value));
        break;
    default:
        break;
    }
    return NULL;
}

} /* attribute */
} /* color_control */

namespace fan_control {
namespace attribute {

attribute_t *create_fan_mode(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max)
{
    attribute_t *attribute =
        esp_matter::attribute::create(cluster, FanControl::Attributes::FanMode::Id,
                                      ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_enum8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_enum8(min), esp_matter_enum8(max));
    return attribute;
}

attribute_t *create_fan_mode_sequence(cluster_t *cluster, const uint8_t value, uint8_t min, uint8_t max)
{
     attribute_t *attribute =
         esp_matter::attribute::create(cluster, FanControl::Attributes::FanModeSequence::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_enum8(min), esp_matter_enum8(max));
    return attribute;
}

attribute_t *create_percent_setting(cluster_t *cluster, nullable<uint8_t> value, nullable<uint8_t> min, nullable<uint8_t> max)
{
    attribute_t *attribute =
	esp_matter::attribute::create(cluster, FanControl::Attributes::PercentSetting::Id,
                                         ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_nullable_uint8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_nullable_uint8(min), esp_matter_nullable_uint8(max));
    return attribute;
}

attribute_t *create_percent_current(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max)
{
    attribute_t *attribute =
	esp_matter::attribute::create(cluster, FanControl::Attributes::PercentCurrent::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint8(min), esp_matter_uint8(max));
    return attribute;
}

attribute_t *create_speed_max(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max)
{
    attribute_t *attribute =
        esp_matter::attribute::create(cluster, FanControl::Attributes::SpeedMax::Id, ATTRIBUTE_FLAG_NONE, esp_matter_uint8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint8(min), esp_matter_uint8(max));
    return attribute;
}

attribute_t *create_speed_setting(cluster_t *cluster, nullable<uint8_t> value, nullable<uint8_t> min, nullable<uint8_t> max)
{
    attribute_t *attribute =
        esp_matter::attribute::create(cluster, FanControl::Attributes::SpeedSetting::Id,
                                      ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_nullable_uint8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_nullable_uint8(min), esp_matter_nullable_uint8(max));
    return attribute;
}

attribute_t *create_speed_current(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max)
{
    attribute_t *attribute =
        esp_matter::attribute::create(cluster, FanControl::Attributes::SpeedCurrent::Id, ATTRIBUTE_FLAG_NONE, esp_matter_uint8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint8(min), esp_matter_uint8(max));
    return attribute;
}

attribute_t *create_rock_support(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, FanControl::Attributes::RockSupport::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bitmap8(value));
}

attribute_t *create_rock_setting(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, FanControl::Attributes::RockSetting::Id, ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_bitmap8(value));
}

attribute_t *create_wind_support(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, FanControl::Attributes::WindSupport::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bitmap8(value));
}

attribute_t *create_wind_setting(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, FanControl::Attributes::WindSetting::Id, ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_bitmap8(value));
}

attribute_t *create_airflow_direction(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, FanControl::Attributes::AirflowDirection::Id, ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_enum8(value));
}

} /* attribute */
} /* fan_control */

namespace thermostat {
namespace attribute {

attribute_t *create_local_temperature(cluster_t *cluster, nullable<int16_t> value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::LocalTemperature::Id, ATTRIBUTE_FLAG_NULLABLE,
                                         esp_matter_nullable_int16(value));
}

attribute_t *create_outdoor_temperature(cluster_t *cluster, nullable<int16_t> value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::OutdoorTemperature::Id, ATTRIBUTE_FLAG_NULLABLE,
                                         esp_matter_nullable_int16(value));
}

attribute_t *create_occupancy(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::Occupancy::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_enum8(value));
}

attribute_t *create_abs_min_heat_setpoint_limit(cluster_t *cluster, int16_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::AbsMinHeatSetpointLimit::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_int16(value));
}

attribute_t *create_abs_max_heat_setpoint_limit(cluster_t *cluster, int16_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::AbsMaxHeatSetpointLimit::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_int16(value));
}

attribute_t *create_abs_min_cool_setpoint_limit(cluster_t *cluster, int16_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::AbsMinCoolSetpointLimit::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_int16(value));
}

attribute_t *create_abs_max_cool_setpoint_limit(cluster_t *cluster, int16_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::AbsMaxCoolSetpointLimit::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_int16(value));
}

attribute_t *create_pi_cooling_demand(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::PICoolingDemand::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

attribute_t *create_pi_heating_demand(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::PIHeatingDemand::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

attribute_t *create_hvac_system_type_config(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::HVACSystemTypeConfiguration::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_bitmap8(value));
}

attribute_t *create_local_temperature_calibration(cluster_t *cluster, int8_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::LocalTemperatureCalibration::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_int8(value));
}

attribute_t *create_occupied_cooling_setpoint(cluster_t *cluster, int16_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::OccupiedCoolingSetpoint::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_int16(value));
}

attribute_t *create_occupied_heating_setpoint(cluster_t *cluster, int16_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::OccupiedHeatingSetpoint::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_int16(value));
}

attribute_t *create_unoccupied_cooling_setpoint(cluster_t *cluster, int16_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::UnoccupiedCoolingSetpoint::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_int16(value));
}

attribute_t *create_unoccupied_heating_setpoint(cluster_t *cluster, int16_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::UnoccupiedHeatingSetpoint::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_int16(value));
}

attribute_t *create_min_heat_setpoint_limit(cluster_t *cluster, int16_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::MinHeatSetpointLimit::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_int16(value));
}

attribute_t *create_max_heat_setpoint_limit(cluster_t *cluster, int16_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::MaxHeatSetpointLimit::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_int16(value));
}

attribute_t *create_min_cool_setpoint_limit(cluster_t *cluster, int16_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::MinCoolSetpointLimit::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_int16(value));
}

attribute_t *create_max_cool_setpoint_limit(cluster_t *cluster, int16_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::MaxCoolSetpointLimit::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_int16(value));
}

attribute_t *create_min_setpoint_dead_band(cluster_t *cluster, int8_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::MinSetpointDeadBand::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_int8(value));
}

attribute_t *create_remote_sensing(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::RemoteSensing::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_bitmap8(value));
}

attribute_t *create_control_sequence_of_operation(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max)
{
    attribute_t *attribute =
        esp_matter::attribute::create(cluster, Thermostat::Attributes::ControlSequenceOfOperation::Id,
                                      ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_enum8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_enum8(min), esp_matter_enum8(max));
    return attribute;
}

attribute_t *create_system_mode(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max)
{
    attribute_t *attribute =
        esp_matter::attribute::create(cluster, Thermostat::Attributes::SystemMode::Id,
                                      ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_enum8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_enum8(min), esp_matter_enum8(max));
    return attribute;
}

attribute_t *create_thermostat_running_mode(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::ThermostatRunningMode::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_start_of_week(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::StartOfWeek::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_enum8(value));
}

attribute_t *create_number_of_weekly_transitions(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::NumberOfWeeklyTransitions::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint8(value));
}

attribute_t *create_number_of_daily_transitions(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::NumberOfDailyTransitions::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_temperature_setpoint_hold(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::TemperatureSetpointHold::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_enum8(value));
}

attribute_t *create_temperature_setpoint_hold_duration(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::TemperatureSetpointHoldDuration::Id,
                                         ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_nullable_uint16(value));
}

attribute_t *create_thermostat_programming_operation_mode(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::ThermostatProgrammingOperationMode::Id,
                                         ATTRIBUTE_FLAG_WRITABLE, esp_matter_bitmap8(value));
}

attribute_t *create_thermostat_running_state(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::ThermostatRunningState::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_bitmap16(value));
}

attribute_t *create_setpoint_change_source(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::SetpointChangeSource::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint8(value));
}

attribute_t *create_setpoint_change_amount(cluster_t *cluster, nullable<int16_t> value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::SetpointChangeAmount::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_nullable_int16(value));
}

attribute_t *create_setpoint_change_source_timestamp(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::SetpointChangeSourceTimestamp::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_occupied_setback(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::OccupiedSetback::Id,
                                         ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_nullable_uint8(value));
}

attribute_t *create_occupied_setback_min(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::OccupiedSetbackMin::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint8(value));
}

attribute_t *create_occupied_setback_max(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::OccupiedSetbackMax::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint8(value));
}

attribute_t *create_unoccupied_setback(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::UnoccupiedSetback::Id,
                                         ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_nullable_uint8(value));
}

attribute_t *create_unoccupied_setback_min(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::UnoccupiedSetbackMin::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint8(value));
}

attribute_t *create_unoccupied_setback_max(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::UnoccupiedSetbackMax::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint8(value));
}

attribute_t *create_emergency_heat_delta(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::EmergencyHeatDelta::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_uint8(value));
}

attribute_t *create_ac_type(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::ACType::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_uint8(value));
}

attribute_t *create_ac_capacity(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::ACCapacity::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_uint16(value));
}

attribute_t *create_ac_refrigerant_type(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::ACRefrigerantType::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_uint8(value));
}

attribute_t *create_ac_compressor_type(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::ACCompressorType::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_uint8(value));
}

attribute_t *create_ac_error_code(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::ACErrorCode::Id,
                                         ATTRIBUTE_FLAG_WRITABLE, esp_matter_bitmap32(value));
}

attribute_t *create_ac_louver_position(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::ACLouverPosition::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_uint8(value));
}

attribute_t *create_ac_coil_temperature(cluster_t *cluster, nullable<int16_t> value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::ACCoilTemperature::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
}

attribute_t *create_ac_capacity_format(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Thermostat::Attributes::ACCapacityformat::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE, esp_matter_uint8(value));
}

} /* attribute */
} /* thermostat */

namespace thermostat_user_interface_configuration {
namespace attribute {

attribute_t *create_temperature_display_mode(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ThermostatUserInterfaceConfiguration::Attributes::TemperatureDisplayMode::Id,
		                         ATTRIBUTE_FLAG_WRITABLE, esp_matter_enum8(value));
}

attribute_t *create_keypad_lockout(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ThermostatUserInterfaceConfiguration::Attributes::KeypadLockout::Id,
		                         ATTRIBUTE_FLAG_WRITABLE, esp_matter_enum8(value));
}

attribute_t *create_schedule_programming_visibility(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ThermostatUserInterfaceConfiguration::Attributes::ScheduleProgrammingVisibility::Id,
		                         ATTRIBUTE_FLAG_WRITABLE, esp_matter_enum8(value));
}

} /* attribute */
} /* thermostat_user_interface_configuration */

namespace air_quality {
namespace attribute {

attribute_t *create_air_quality(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, AirQuality::Attributes::AirQuality::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_enum8(value));
}

} /* attribute */
} /* air_quality */

namespace hepa_filter_monitoring {
namespace attribute {

attribute_t *create_condition(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, HepaFilterMonitoring::Attributes::Condition::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

attribute_t *create_degradation_direction(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, HepaFilterMonitoring::Attributes::DegradationDirection::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_enum8(value));
}

attribute_t *create_change_indication(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, HepaFilterMonitoring::Attributes::ChangeIndication::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_enum8(value));
}

attribute_t *create_in_place_indicator(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, HepaFilterMonitoring::Attributes::InPlaceIndicator::Id, ATTRIBUTE_FLAG_NONE,
		                         esp_matter_bool(value));
}

attribute_t *create_last_change_time(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, HepaFilterMonitoring::Attributes::LastChangedTime::Id,
                                         ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE,
		      			 esp_matter_nullable_uint8(value));
}

attribute_t *create_replacement_product_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, HepaFilterMonitoring::Attributes::ReplacementProductList::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

} /* attribute */
} /* hepa_filter_monitoring */

namespace activated_carbon_filter_monitoring {
namespace attribute {

attribute_t *create_condition(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ActivatedCarbonFilterMonitoring::Attributes::Condition::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

attribute_t *create_degradation_direction(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ActivatedCarbonFilterMonitoring::Attributes::DegradationDirection::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_enum8(value));
}

attribute_t *create_change_indication(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ActivatedCarbonFilterMonitoring::Attributes::ChangeIndication::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_enum8(value));
}

attribute_t *create_in_place_indicator(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, ActivatedCarbonFilterMonitoring::Attributes::InPlaceIndicator::Id, ATTRIBUTE_FLAG_NONE,
		                         esp_matter_bool(value));
}

attribute_t *create_last_change_time(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, ActivatedCarbonFilterMonitoring::Attributes::LastChangedTime::Id,
                                         ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE,
		      			 esp_matter_nullable_uint8(value));
}

attribute_t *create_replacement_product_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ActivatedCarbonFilterMonitoring::Attributes::ReplacementProductList::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

} /* attribute */
} /* activated_carbon_filter_monitoring */

namespace carbon_monoxide_concentration_measurement {
namespace attribute {
attribute_t *create_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_min_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MinMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_max_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MaxMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_peak_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::PeakMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_peak_measured_value_window(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::PeakMeasuredValueWindow::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_average_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::AverageMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_average_measured_value_window(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::AverageMeasuredValueWindow::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_uncertainty(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::Uncertainty::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_measurement_unit(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasurementUnit::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_measurement_medium(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasurementMedium::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_level_value(cluster_t *cluster, uint8_t value)
{
	return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::LevelValue::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

} /* attribute */
} /* carbon_monoxide_concentration_measurement */

namespace carbon_dioxide_concentration_measurement {
namespace attribute {
attribute_t *create_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_min_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MinMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_max_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MaxMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_peak_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::PeakMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_peak_measured_value_window(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::PeakMeasuredValueWindow::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_average_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::AverageMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_average_measured_value_window(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::AverageMeasuredValueWindow::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_uncertainty(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::Uncertainty::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_measurement_unit(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasurementUnit::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_measurement_medium(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasurementMedium::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_level_value(cluster_t *cluster, uint8_t value)
{
	return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::LevelValue::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

} /* attribute */
} /* carbon_dioxide_concentration_measurement */

namespace nitrogen_dioxide_concentration_measurement {
namespace attribute {
attribute_t *create_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_min_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MinMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_max_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MaxMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_peak_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::PeakMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_peak_measured_value_window(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::PeakMeasuredValueWindow::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_average_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::AverageMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_average_measured_value_window(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::AverageMeasuredValueWindow::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_uncertainty(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::Uncertainty::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_measurement_unit(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasurementUnit::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_measurement_medium(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasurementMedium::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_level_value(cluster_t *cluster, uint8_t value)
{
	return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::LevelValue::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

} /* attribute */
} /* nitrogen_dioxide_concentration_measurement */

namespace ozone_concentration_measurement {
namespace attribute {
attribute_t *create_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_min_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MinMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_max_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MaxMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_peak_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::PeakMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_peak_measured_value_window(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::PeakMeasuredValueWindow::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_average_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::AverageMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_average_measured_value_window(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::AverageMeasuredValueWindow::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_uncertainty(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::Uncertainty::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_measurement_unit(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasurementUnit::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_measurement_medium(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasurementMedium::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_level_value(cluster_t *cluster, uint8_t value)
{
	return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::LevelValue::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

} /* attribute */
} /* ozone_concentration_measurement */

namespace formaldehyde_concentration_measurement {
namespace attribute {
attribute_t *create_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_min_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MinMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_max_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MaxMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_peak_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::PeakMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_peak_measured_value_window(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::PeakMeasuredValueWindow::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_average_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::AverageMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_average_measured_value_window(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::AverageMeasuredValueWindow::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_uncertainty(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::Uncertainty::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_measurement_unit(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasurementUnit::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_measurement_medium(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasurementMedium::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_level_value(cluster_t *cluster, uint8_t value)
{
	return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::LevelValue::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

} /* attribute */
} /* formaldehyde_concentration_measurement */

namespace pm1_concentration_measurement {
namespace attribute {
attribute_t *create_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_min_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MinMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_max_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MaxMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_peak_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::PeakMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_peak_measured_value_window(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::PeakMeasuredValueWindow::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_average_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::AverageMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_average_measured_value_window(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::AverageMeasuredValueWindow::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_uncertainty(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::Uncertainty::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_measurement_unit(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasurementUnit::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_measurement_medium(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasurementMedium::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_level_value(cluster_t *cluster, uint8_t value)
{
	return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::LevelValue::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

} /* attribute */
} /* pm1_concentration_measurement */

namespace pm25_concentration_measurement {
namespace attribute {
attribute_t *create_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_min_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MinMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_max_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MaxMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_peak_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::PeakMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_peak_measured_value_window(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::PeakMeasuredValueWindow::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_average_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::AverageMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_average_measured_value_window(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::AverageMeasuredValueWindow::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_uncertainty(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::Uncertainty::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_measurement_unit(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasurementUnit::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_measurement_medium(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasurementMedium::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_level_value(cluster_t *cluster, uint8_t value)
{
	return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::LevelValue::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

} /* attribute */
} /* pm25_concentration_measurement */

namespace pm10_concentration_measurement {
namespace attribute {
attribute_t *create_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_min_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MinMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_max_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MaxMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_peak_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::PeakMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_peak_measured_value_window(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::PeakMeasuredValueWindow::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_average_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::AverageMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_average_measured_value_window(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::AverageMeasuredValueWindow::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_uncertainty(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::Uncertainty::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_measurement_unit(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasurementUnit::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_measurement_medium(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasurementMedium::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_level_value(cluster_t *cluster, uint8_t value)
{
	return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::LevelValue::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

} /* attribute */
} /* pm10_concentration_measurement */

namespace radon_concentration_measurement {
namespace attribute {
attribute_t *create_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_min_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MinMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_max_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MaxMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_peak_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::PeakMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_peak_measured_value_window(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::PeakMeasuredValueWindow::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_average_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::AverageMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_average_measured_value_window(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::AverageMeasuredValueWindow::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_uncertainty(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::Uncertainty::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_measurement_unit(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasurementUnit::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_measurement_medium(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasurementMedium::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_level_value(cluster_t *cluster, uint8_t value)
{
	return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::LevelValue::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

} /* attribute */
} /* radon_concentration_measurement */

namespace total_volatile_organic_compound_concentration_measurement {
namespace attribute {
attribute_t *create_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_min_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MinMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_max_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MaxMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_peak_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::PeakMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_peak_measured_value_window(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::PeakMeasuredValueWindow::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_average_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::AverageMeasuredValue::Id,
					 ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_average_measured_value_window(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::AverageMeasuredValueWindow::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_uncertainty(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::Uncertainty::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_measurement_unit(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasurementUnit::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_measurement_medium(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::MeasurementMedium::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_level_value(cluster_t *cluster, uint8_t value)
{
	return esp_matter::attribute::create(cluster, CarbonMonoxideConcentrationMeasurement::Attributes::LevelValue::Id,
					 ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

} /* attribute */
} /* total_volatile_organic_compound_concentration_measurement */

namespace operational_state {
namespace attribute {
attribute_t *create_phase_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, OperationalState::Attributes::PhaseList::Id, ATTRIBUTE_FLAG_NULLABLE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_current_phase(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, OperationalState::Attributes::CurrentPhase::Id, ATTRIBUTE_FLAG_NULLABLE,
		      			 esp_matter_nullable_uint8(value));
}

attribute_t *create_countdown_time(cluster_t *cluster, nullable<uint32_t> value)
{
    return esp_matter::attribute::create(cluster, OperationalState::Attributes::CountdownTime::Id, ATTRIBUTE_FLAG_NULLABLE,
                                         esp_matter_nullable_uint32(value));
}

attribute_t *create_operational_state_list(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, OperationalState::Attributes::OperationalStateList::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_operational_state(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, OperationalState::Attributes::OperationalState::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_enum8(value));
}

attribute_t *create_operational_error(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, OperationalState::Attributes::OperationalError::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_enum8(value));
}

} /* attribute */
} /* operational_state */

namespace laundry_washer_mode {
namespace attribute {

attribute_t *create_supported_modes(cluster_t *cluster, const uint8_t * value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, LaundryWasherMode::Attributes::SupportedModes::Id, ATTRIBUTE_FLAG_NONE, esp_matter_array((uint8_t*)value, length, count));
}

attribute_t *create_current_mode(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, LaundryWasherMode::Attributes::CurrentMode::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint8(value));
}

} /* attribute */
} /* laundry_washer_mode */

namespace laundry_washer_controls {
namespace attribute {

attribute_t *create_spin_speeds(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, LaundryWasherControls::Attributes::SpinSpeeds::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}

attribute_t *create_spin_speed_current(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, LaundryWasherControls::Attributes::SpinSpeedCurrent::Id,
		                         ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_nullable_uint8(value));

}

attribute_t *create_number_of_rinses(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, LaundryWasherControls::Attributes::NumberOfRinses::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_enum8(value));
}

attribute_t *create_supported_rinses(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, LaundryWasherControls::Attributes::SupportedRinses::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_array(value, length, count));
}
} /* attribute */
} /* laundry_washer_controls */

namespace dish_washer_mode {
namespace attribute {

attribute_t *create_supported_modes(cluster_t *cluster, const uint8_t * value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, LaundryWasherMode::Attributes::SupportedModes::Id, ATTRIBUTE_FLAG_NONE, esp_matter_array((uint8_t*)value, length, count));
}

attribute_t *create_current_mode(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, LaundryWasherMode::Attributes::CurrentMode::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint8(value));
}

} /* attribute */
} /* dish_washer_mode */

namespace smoke_co_alarm {
namespace attribute {
attribute_t *create_expressed_state(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, SmokeCoAlarm::Attributes::ExpressedState::Id, ATTRIBUTE_FLAG_NONVOLATILE,
					 esp_matter_enum8(value));
}

attribute_t *create_smoke_state(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, SmokeCoAlarm::Attributes::SmokeState::Id, ATTRIBUTE_FLAG_NONVOLATILE,
					 esp_matter_enum8(value));
}

attribute_t *create_co_state(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, SmokeCoAlarm::Attributes::COState::Id, ATTRIBUTE_FLAG_NONVOLATILE,
					 esp_matter_enum8(value));
}

attribute_t *create_battery_alert(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, SmokeCoAlarm::Attributes::BatteryAlert::Id, ATTRIBUTE_FLAG_NONVOLATILE,
					 esp_matter_enum8(value));
}

attribute_t *create_device_muted(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, SmokeCoAlarm::Attributes::DeviceMuted::Id, ATTRIBUTE_FLAG_NONVOLATILE,
		                        esp_matter_enum8(value));
}

attribute_t *create_test_in_progress(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, SmokeCoAlarm::Attributes::TestInProgress::Id, ATTRIBUTE_FLAG_NONE,
		                        esp_matter_bool(value));
}

attribute_t *create_hardware_fault_alert(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, SmokeCoAlarm::Attributes::HardwareFaultAlert::Id, ATTRIBUTE_FLAG_NONVOLATILE,
		                        esp_matter_bool(value));
}

attribute_t *create_end_of_service_alert(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, SmokeCoAlarm::Attributes::EndOfServiceAlert::Id, ATTRIBUTE_FLAG_NONVOLATILE,
		                        esp_matter_enum8(value));
}

attribute_t *create_interconnect_smoke_alarm(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, SmokeCoAlarm::Attributes::InterconnectSmokeAlarm::Id, ATTRIBUTE_FLAG_NONE,
		                        esp_matter_enum8(value));
}

attribute_t *create_interconnect_co_alarm(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, SmokeCoAlarm::Attributes::InterconnectCOAlarm::Id, ATTRIBUTE_FLAG_NONE,
		                        esp_matter_enum8(value));
}

attribute_t *create_contamination_state(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, SmokeCoAlarm::Attributes::ContaminationState::Id, ATTRIBUTE_FLAG_NONE,
		                        esp_matter_enum8(value));
}

attribute_t *create_smoke_sensitivity_level(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, SmokeCoAlarm::Attributes::SmokeSensitivityLevel::Id, ATTRIBUTE_FLAG_WRITABLE,
		                        esp_matter_enum8(value));
}

attribute_t *create_expiry_date(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, SmokeCoAlarm::Attributes::ContaminationState::Id, ATTRIBUTE_FLAG_NONE,
		                        esp_matter_uint32(value));
}

} /* attribute */
} /* smoke_co_alarm */

namespace door_lock {
namespace attribute {

attribute_t *create_lock_state(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, DoorLock::Attributes::LockState::Id, ATTRIBUTE_FLAG_NULLABLE,
                                         esp_matter_nullable_uint8(value));
}

attribute_t *create_lock_type(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, DoorLock::Attributes::LockType::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_enum8(value));
}

attribute_t *create_actuator_enabled(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, DoorLock::Attributes::ActuatorEnabled::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bool(value));
}

attribute_t *create_auto_relock_time(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, DoorLock::Attributes::AutoRelockTime::Id, ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_uint32(value));
}

attribute_t *create_operating_mode(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, DoorLock::Attributes::OperatingMode::Id,
                                                           ATTRIBUTE_FLAG_WRITABLE, esp_matter_enum8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_enum8(min), esp_matter_enum8(max));
    return attribute;
}

attribute_t *create_supported_operating_modes(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, DoorLock::Attributes::SupportedOperatingModes::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_bitmap16(value));
}

} /* attribute */
} /* door_lock */

namespace window_covering {
namespace attribute {

attribute_t *create_type(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::Type::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_enum8(value));
}

attribute_t *create_physical_closed_limit_lift(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::PhysicalClosedLimitLift::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_physical_closed_limit_tilt(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::PhysicalClosedLimitTilt::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
}

attribute_t *create_current_position_lift(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::CurrentPositionLift::Id,
                                         ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_nullable_uint16(value));
}

attribute_t *create_current_position_tilt(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::CurrentPositionTilt::Id,
                                         ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_nullable_uint16(value));
}

attribute_t *create_number_of_actuations_lift(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::NumberOfActuationsLift::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint16(value));
}

attribute_t *create_number_of_actuations_tilt(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::NumberOfActuationsTilt::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint16(value));
}

attribute_t *create_config_status(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::ConfigStatus::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_bitmap8(value));
}

attribute_t *create_current_position_lift_percentage(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::CurrentPositionLiftPercentage::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_NULLABLE,
                                         esp_matter_nullable_uint8(value));
}

attribute_t *create_current_position_tilt_percentage(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::CurrentPositionTiltPercentage::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_NULLABLE,
                                         esp_matter_nullable_uint8(value));
}

attribute_t *create_operational_status(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::OperationalStatus::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_bitmap8(value));
}

attribute_t *create_target_position_lift_percent_100ths(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::TargetPositionLiftPercent100ths::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_target_position_tilt_percent_100ths(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::TargetPositionTiltPercent100ths::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_end_product_type(cluster_t *cluster, const uint8_t value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::EndProductType::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_enum8(value));
}

attribute_t *create_current_position_lift_percent_100ths(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::CurrentPositionLiftPercent100ths::Id,
                                         ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_MASK_NONVOLATILE,
                                         esp_matter_nullable_uint16(value));
}

attribute_t *create_current_position_tilt_percent_100ths(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::CurrentPositionTiltPercent100ths::Id,
                                         ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_MASK_NONVOLATILE,
                                         esp_matter_nullable_uint16(value));
}

attribute_t *create_installed_open_limit_lift(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::InstalledOpenLimitLift::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint16(value));
}

attribute_t *create_installed_closed_limit_lift(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::InstalledClosedLimitLift::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint16(value));
}

attribute_t *create_installed_open_limit_tilt(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::InstalledOpenLimitTilt::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint16(value));
}

attribute_t *create_installed_closed_limit_tilt(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::InstalledClosedLimitTilt::Id,
                                         ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint16(value));
}

attribute_t *create_mode(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::Mode::Id, ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_WRITABLE,
                                         esp_matter_bitmap8(value));
}

attribute_t *create_safety_status(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, WindowCovering::Attributes::SafetyStatus::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bitmap16(value));
}

} /* attribute */
} /* window_covering */

namespace switch_cluster {
namespace attribute {

attribute_t *create_number_of_positions(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Switch::Attributes::NumberOfPositions::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

attribute_t *create_current_position(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Switch::Attributes::CurrentPosition::Id, ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_uint8(value));
}

attribute_t *create_multi_press_max(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, Switch::Attributes::MultiPressMax::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_uint8(value));
}

} /* attribute */
} /* switch_cluster */

namespace temperature_measurement {
namespace attribute {

attribute_t *create_temperature_measured_value(cluster_t *cluster, nullable<int16_t> value)
{
    return esp_matter::attribute::create(cluster, TemperatureMeasurement::Attributes::MeasuredValue::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
}

attribute_t *create_temperature_min_measured_value(cluster_t *cluster, nullable<int16_t> value)
{
    return esp_matter::attribute::create(cluster, TemperatureMeasurement::Attributes::MinMeasuredValue::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
}

attribute_t *create_temperature_max_measured_value(cluster_t *cluster, nullable<int16_t> value)
{
    return esp_matter::attribute::create(cluster, TemperatureMeasurement::Attributes::MaxMeasuredValue::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
}

} /* attribute */
} /* temperature_measurement */

namespace relative_humidity_measurement {
namespace attribute {

attribute_t *create_relative_humidity_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, RelativeHumidityMeasurement::Attributes::MeasuredValue::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_relative_humidity_min_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, RelativeHumidityMeasurement::Attributes::MinMeasuredValue::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_relative_humidity_max_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, RelativeHumidityMeasurement::Attributes::MaxMeasuredValue::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_relative_humidity_tolerance(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, RelativeHumidityMeasurement::Attributes::Tolerance::Id,
                                         ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

} /* attribute */
} /* relative_humidity_measurement */

namespace occupancy_sensing {
namespace attribute {

attribute_t *create_occupancy(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, OccupancySensing::Attributes::Occupancy::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bitmap8(value));
}

attribute_t *create_occupancy_sensor_type(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, OccupancySensing::Attributes::OccupancySensorType::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_occupancy_sensor_type_bitmap(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, OccupancySensing::Attributes::OccupancySensorTypeBitmap::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_bitmap8(value));
}

} /* attribute */
} /* occupancy_sensing */

namespace boolean_state {
namespace attribute {

attribute_t *state_value(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, BooleanState::Attributes::StateValue::Id, ATTRIBUTE_FLAG_NONE,
                                         esp_matter_bool(value));
}

} /* attribute */
} /* boolean_state */

namespace localization_configuration {
namespace attribute {

attribute_t *create_active_locale(cluster_t *cluster, char *value, uint16_t length)
{
    return esp_matter::attribute::create(cluster, LocalizationConfiguration::Attributes::ActiveLocale::Id,
                                         ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE,
                                         esp_matter_char_str(value, length));
}

attribute_t *create_supported_locales(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, LocalizationConfiguration::Attributes::SupportedLocales::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_array(value, length, count));
}

} /* attribute */
} /* localization_configuration */

namespace time_format_localization {
namespace attribute {

attribute_t *create_hour_format(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, TimeFormatLocalization::Attributes::HourFormat::Id,
                                         ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_NULLABLE,
                                         esp_matter_nullable_enum8(value));
}

attribute_t *create_active_calendar_type(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, TimeFormatLocalization::Attributes::ActiveCalendarType::Id,
                                         ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE | ATTRIBUTE_FLAG_NULLABLE,
                                         esp_matter_nullable_enum8(value));
}

attribute_t *create_supported_calendar_types(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, TimeFormatLocalization::Attributes::SupportedCalendarTypes::Id,
                                         ATTRIBUTE_FLAG_NONE, esp_matter_array(value, length, count));
}

} /* attribute */
} /* time_format_localization */

namespace illuminance_measurement {
namespace attribute {

attribute_t *create_illuminance_measured_value(cluster_t *cluster, nullable<uint16_t> value, nullable<uint16_t> min, nullable<uint16_t> max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, IlluminanceMeasurement::Attributes::MeasuredValue::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
    if (!attribute) {
	ESP_LOGE(TAG, "Could not create attribute");
	return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_nullable_uint16(min), esp_matter_nullable_uint16(max));
    return attribute;
}

attribute_t *create_illuminance_min_measured_value(cluster_t *cluster, nullable<uint16_t> value, nullable<uint16_t> min, nullable<uint16_t> max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, IlluminanceMeasurement::Attributes::MinMeasuredValue::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
    if (!attribute) {
	ESP_LOGE(TAG, "Could not create attribute");
	return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_nullable_uint16(min), esp_matter_nullable_uint16(max));
    return attribute;
}

attribute_t *create_illuminance_max_measured_value(cluster_t *cluster, nullable<uint16_t> value, nullable<uint16_t> min, nullable<uint16_t> max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, IlluminanceMeasurement::Attributes::MaxMeasuredValue::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
    if (!attribute) {
	ESP_LOGE(TAG, "Could not create attribute");
	return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_nullable_uint16(min), esp_matter_nullable_uint16(max));
    return attribute;
}

attribute_t *create_illuminance_tolerance(cluster_t *cluster, uint16_t value, uint16_t min, uint16_t max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, IlluminanceMeasurement::Attributes::Tolerance::Id, ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
   if (!attribute) {
	ESP_LOGE(TAG, "Could not create attribute");
	return NULL;
   }
   esp_matter::attribute::add_bounds(attribute, esp_matter_uint16(min), esp_matter_uint16(max));
   return attribute;
}

attribute_t *create_illuminance_light_sensor_type(cluster_t *cluster, nullable<uint8_t> value, nullable<uint8_t> min, nullable<uint8_t> max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, IlluminanceMeasurement::Attributes::LightSensorType::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_enum8(value));
    if (!attribute) {
	ESP_LOGE(TAG, "Could not create attribute");
	return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_nullable_enum8(min), esp_matter_nullable_enum8(max));
    return attribute;
}

} /* attribute */
} /* illuminance_measurement */

namespace pressure_measurement {
namespace attribute {

attribute_t *create_pressure_measured_value(cluster_t *cluster, nullable<int16_t> value)
{
	return esp_matter::attribute::create(cluster, PressureMeasurement::Attributes::MeasuredValue::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
}

attribute_t *create_pressure_min_measured_value(cluster_t *cluster, nullable<int16_t> value)
{
	return esp_matter::attribute::create(cluster, PressureMeasurement::Attributes::MinMeasuredValue::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
}

attribute_t *create_pressure_max_measured_value(cluster_t *cluster, nullable<int16_t> value)
{
    return esp_matter::attribute::create(cluster, PressureMeasurement::Attributes::MaxMeasuredValue::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
}

attribute_t *create_pressure_tolerance(cluster_t *cluster, uint16_t value, uint16_t min, uint16_t max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PressureMeasurement::Attributes::Tolerance::Id, ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
    if (!attribute) {
	ESP_LOGE(TAG, "Could not create attribute");
	return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint16(min), esp_matter_uint16(max));
    return attribute;
}

attribute_t *create_pressure_scaled_value(cluster_t *cluster, nullable<int16_t> value)
{
    return esp_matter::attribute::create(cluster, PressureMeasurement::Attributes::ScaledValue::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
}

attribute_t *create_pressure_min_scaled_value(cluster_t *cluster, nullable<int16_t> value)
{
    return esp_matter::attribute::create(cluster, PressureMeasurement::Attributes::MinScaledValue::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
}

attribute_t *create_pressure_max_scaled_value(cluster_t *cluster, nullable<int16_t> value)
{
    return esp_matter::attribute::create(cluster, PressureMeasurement::Attributes::MaxScaledValue::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
}

attribute_t *create_pressure_scaled_tolerance(cluster_t *cluster, uint16_t value, uint16_t min, uint16_t max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PressureMeasurement::Attributes::ScaledTolerance::Id, ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
    if (!attribute) {
	ESP_LOGE(TAG, "Could not create attribute");
	return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint16(min), esp_matter_uint16(max));
    return attribute;
}

attribute_t *create_pressure_scale(cluster_t *cluster, int8_t value)
{
    return esp_matter::attribute::create(cluster, PressureMeasurement::Attributes::Scale::Id, ATTRIBUTE_FLAG_NONE, esp_matter_int8(value));
}

} /* attribute */
} /* pressure_measurement */

namespace flow_measurement {
namespace attribute {

attribute_t *create_flow_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, FlowMeasurement::Attributes::MeasuredValue::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_flow_min_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, FlowMeasurement::Attributes::MinMeasuredValue::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_flow_max_measured_value(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, FlowMeasurement::Attributes::MaxMeasuredValue::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_flow_tolerance(cluster_t *cluster, uint16_t value, uint16_t min, uint16_t max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, FlowMeasurement::Attributes::Tolerance::Id, ATTRIBUTE_FLAG_NONE, esp_matter_uint16(value));
    if (!attribute) {
	ESP_LOGE(TAG, "Could not create attribute");
	return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint16(min), esp_matter_uint16(max));
    return attribute;
}
} /* attribute */
} /* flow_measurement */

namespace pump_configuration_and_control {
namespace attribute {

attribute_t *create_max_pressure(cluster_t *cluster, nullable<int16_t> value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::MaxPressure::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
}

attribute_t *create_max_speed(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::MaxSpeed::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_max_flow(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::MaxFlow::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_min_const_pressure(cluster_t *cluster, nullable<int16_t> value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::MinConstPressure::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
}

attribute_t *create_max_const_pressure(cluster_t *cluster, nullable<int16_t> value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::MaxConstPressure::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
}

attribute_t *create_min_comp_pressure(cluster_t *cluster, nullable<int16_t> value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::MinCompPressure::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
}

attribute_t *create_max_comp_pressure(cluster_t *cluster, nullable<int16_t> value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::MaxCompPressure::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
}

attribute_t *create_min_const_speed(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::MinConstSpeed::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_max_const_speed(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::MaxConstSpeed::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_min_const_flow(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::MinConstFlow::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_max_const_flow(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::MaxConstFlow::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_min_const_temp(cluster_t *cluster, nullable<int16_t> value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::MinConstTemp::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
}

attribute_t *create_max_const_temp(cluster_t *cluster, nullable<int16_t> value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::MaxConstTemp::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
}

attribute_t *create_pump_status(cluster_t *cluster, uint16_t value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::PumpStatus::Id, ATTRIBUTE_FLAG_NONE, esp_matter_bitmap16(value));
}

attribute_t *create_effective_operation_mode(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::EffectiveOperationMode::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_enum8(value));
}

attribute_t *create_effective_control_mode(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::EffectiveControlMode::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_enum8(value));
}

attribute_t *create_capacity(cluster_t *cluster, nullable<int16_t> value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::Capacity::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_int16(value));
}

attribute_t *create_speed(cluster_t *cluster, nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::Speed::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
}

attribute_t *create_lifetime_running_hours(cluster_t *cluster, nullable<uint32_t> value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::LifetimeRunningHours::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_nullable_uint32(value));
}

attribute_t *create_pump_power(cluster_t *cluster, nullable<uint32_t> value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::Power::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint32(value));
}

attribute_t *create_lifetime_energy_consumed(cluster_t *cluster, nullable<uint32_t> value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::LifetimeEnergyConsumed::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_nullable_uint32(value));
}

attribute_t *create_operation_mode(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::OperationMode::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_enum8(value));
}

attribute_t *create_control_mode(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, PumpConfigurationAndControl::Attributes::ControlMode::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_enum8(value));
}
} /* attribute */
} /* pump_configuration_and_control */

namespace mode_select {
namespace attribute {

attribute_t *create_mode_select_description(cluster_t *cluster, const char * value, uint16_t length)
{
    if (length > k_max_mode_select_description_length) {
        ESP_LOGE(TAG, "Could not create attribute, string length out of bound");
        return NULL;
    }
    return esp_matter::attribute::create(cluster, ModeSelect::Attributes::Description::Id, ATTRIBUTE_FLAG_NONE, esp_matter_char_str((char *)value, length));
}

attribute_t *create_standard_namespace(cluster_t *cluster, const nullable<uint16_t> value)
{
    return esp_matter::attribute::create(cluster, ModeSelect::Attributes::StandardNamespace::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_enum16(value));
}

attribute_t *create_supported_modes(cluster_t *cluster, const uint8_t * value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, ModeSelect::Attributes::SupportedModes::Id, ATTRIBUTE_FLAG_NONE, esp_matter_array((uint8_t*)value, length, count));
}

attribute_t *create_current_mode(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, ModeSelect::Attributes::CurrentMode::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint8(value));
}

attribute_t *create_start_up_mode(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, ModeSelect::Attributes::StartUpMode::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_nullable_uint8(value));
}

attribute_t *create_on_mode(cluster_t *cluster, nullable<uint8_t> value)
{
    return esp_matter::attribute::create(cluster, ModeSelect::Attributes::OnMode::Id, ATTRIBUTE_FLAG_WRITABLE | ATTRIBUTE_FLAG_NULLABLE | ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_nullable_uint8(value));
}

} /* attribute */
} /* mode_select */

namespace power_source {
namespace attribute {

attribute_t *create_status(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, PowerSource::Attributes::Status::Id, ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_order(cluster_t *cluster, uint8_t value, uint8_t min, uint8_t max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PowerSource::Attributes::Order::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint8(min), esp_matter_uint8(max));
    return attribute;
}

attribute_t *create_description(cluster_t *cluster, const char * value, uint16_t length)
{
    if (length > k_max_description_length) {
        ESP_LOGE(TAG, "Could not create attribute, string length out of bound");
        return NULL;
    }
    return esp_matter::attribute::create(cluster, PowerSource::Attributes::Description::Id, ATTRIBUTE_FLAG_NONE, esp_matter_char_str((char *)value, length));
}

attribute_t *create_wired_assessed_input_voltage(cluster_t *cluster, nullable<uint32_t> value, nullable<uint32_t> min, nullable<uint32_t> max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PowerSource::Attributes::WiredAssessedInputVoltage::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint32(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_nullable_uint32(min), esp_matter_nullable_uint32(max));
    return attribute;
}

attribute_t *create_wired_assessed_input_frequency(cluster_t *cluster, nullable<uint16_t> value, nullable<uint16_t> min, nullable<uint16_t> max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PowerSource::Attributes::WiredAssessedInputFrequency::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint16(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_nullable_uint16(min), esp_matter_nullable_uint16(max));
    return attribute;
}

attribute_t *create_wired_current_type(cluster_t *cluster, const uint8_t value)
{
    return esp_matter::attribute::create(cluster, PowerSource::Attributes::WiredCurrentType::Id, ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_wired_assessed_current(cluster_t *cluster, nullable<uint32_t> value, nullable<uint32_t> min, nullable<uint32_t> max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PowerSource::Attributes::WiredAssessedCurrent::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint32(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_nullable_uint32(min), esp_matter_nullable_uint32(max));
    return attribute;
}

attribute_t *create_wired_nominal_voltage(cluster_t *cluster, const uint32_t value, uint32_t min, uint32_t max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PowerSource::Attributes::WiredNominalVoltage::Id, ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint32(min), esp_matter_uint32(max));
    return attribute;
}

attribute_t *create_wired_maximum_current(cluster_t *cluster, const uint32_t value, uint32_t min, uint32_t max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PowerSource::Attributes::WiredMaximumCurrent::Id, ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint32(min), esp_matter_uint32(max));
    return attribute;
}

attribute_t *create_wired_present(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, PowerSource::Attributes::WiredPresent::Id, ATTRIBUTE_FLAG_NONE, esp_matter_bool(value));
}

attribute_t *create_active_wired_faults(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count)
{
    if (count > k_max_fault_count) {
        ESP_LOGE(TAG, "Could not create attribute, list out of bound");
        return NULL;
    }
    return esp_matter::attribute::create(cluster, PowerSource::Attributes::ActiveWiredFaults::Id, ATTRIBUTE_FLAG_NONE, esp_matter_array(value, length, count));
}

attribute_t *create_bat_voltage(cluster_t *cluster, nullable<uint32_t> value, nullable<uint32_t> min, nullable<uint32_t> max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PowerSource::Attributes::BatVoltage::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint32(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_nullable_uint32(min), esp_matter_nullable_uint32(max));
    return attribute;
}

attribute_t *create_bat_percent_remaining(cluster_t *cluster, nullable<uint8_t> value, nullable<uint8_t> min, nullable<uint8_t> max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PowerSource::Attributes::BatPercentRemaining::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_nullable_uint8(min), esp_matter_nullable_uint8(max));
    return attribute;
}

attribute_t *create_bat_time_remaining(cluster_t *cluster, nullable< uint32_t> value, nullable<uint32_t> min, nullable<uint32_t> max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PowerSource::Attributes::BatTimeRemaining::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint32(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_nullable_uint32(min), esp_matter_nullable_uint32(max));
    return attribute;
}

attribute_t *create_bat_charge_level(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, PowerSource::Attributes::BatChargeLevel::Id, ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_bat_replacement_needed(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, PowerSource::Attributes::BatReplacementNeeded::Id, ATTRIBUTE_FLAG_NONE, esp_matter_bool(value));
}

attribute_t *create_bat_replaceability(cluster_t *cluster, const uint8_t value)
{
    return esp_matter::attribute::create(cluster, PowerSource::Attributes::BatReplaceability::Id, ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_bat_present(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, PowerSource::Attributes::BatPresent::Id, ATTRIBUTE_FLAG_NONE, esp_matter_bool(value));
}

attribute_t *create_active_bat_faults(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count)
{
    if (count > k_max_fault_count) {
        ESP_LOGE(TAG, "Could not create attribute, list out of bound");
        return NULL;
    }
    return esp_matter::attribute::create(cluster, PowerSource::Attributes::ActiveBatFaults::Id, ATTRIBUTE_FLAG_NONE, esp_matter_array(value, length, count));
}

attribute_t *create_bat_replacement_description(cluster_t *cluster, const char * value, uint16_t length)
{
    if (length > k_max_description_length) {
        ESP_LOGE(TAG, "Could not create attribute, string size out of bound");
        return NULL;
    }
    return esp_matter::attribute::create(cluster, PowerSource::Attributes::BatReplacementDescription::Id, ATTRIBUTE_FLAG_NONE, esp_matter_char_str((char *)value, length));
}

attribute_t *create_bat_common_designation(cluster_t *cluster, const uint8_t value, uint8_t min, uint8_t max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PowerSource::Attributes::BatCommonDesignation::Id, ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_enum8(min), esp_matter_enum8(max));
    return attribute;
}

attribute_t *create_bat_ansi_designation(cluster_t *cluster, const char * value, uint16_t length)
{
    if (length > k_max_designation_count) {
        ESP_LOGE(TAG, "Could not create attribute, string size out of bound");
        return NULL;
    }
    return esp_matter::attribute::create(cluster, PowerSource::Attributes::BatANSIDesignation::Id, ATTRIBUTE_FLAG_NONE, esp_matter_char_str((char *)value, length));
}

attribute_t *create_bat_iec_designation(cluster_t *cluster, const char * value, uint16_t length)
{
    if (length > k_max_designation_count) {
        ESP_LOGE(TAG, "Could not create attribute, string size out of bound");
        return NULL;
    }
    return esp_matter::attribute::create(cluster, PowerSource::Attributes::BatIECDesignation::Id, ATTRIBUTE_FLAG_NONE, esp_matter_char_str((char *)value, length));
}

attribute_t *create_bat_approved_chemistry(cluster_t *cluster, const uint8_t value, uint8_t min, uint8_t max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PowerSource::Attributes::BatApprovedChemistry::Id, ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_enum8(min), esp_matter_enum8(max));
    return attribute;
}

attribute_t *create_bat_capacity(cluster_t *cluster, const uint32_t value, uint32_t min, uint32_t max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PowerSource::Attributes::BatCapacity::Id, ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint32(min), esp_matter_uint32(max));
    return attribute;
}

attribute_t *create_bat_quantity(cluster_t *cluster, const uint8_t value, uint8_t min, uint8_t max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PowerSource::Attributes::BatQuantity::Id, ATTRIBUTE_FLAG_NONE, esp_matter_uint8(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_uint8(min), esp_matter_uint8(max));
    return attribute;
}

attribute_t *create_bat_charge_state(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, PowerSource::Attributes::BatChargeState::Id, ATTRIBUTE_FLAG_NONE, esp_matter_enum8(value));
}

attribute_t *create_bat_time_to_full_charge(cluster_t *cluster, nullable<uint32_t> value, nullable<uint32_t> min, nullable<uint32_t> max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PowerSource::Attributes::BatTimeToFullCharge::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint32(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_nullable_uint32(min), esp_matter_nullable_uint32(max));
    return attribute;
}

attribute_t *create_bat_functional_while_charging(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, PowerSource::Attributes::BatFunctionalWhileCharging::Id, ATTRIBUTE_FLAG_NONE, esp_matter_bool(value));
}

attribute_t *create_bat_charging_current(cluster_t *cluster, nullable<uint32_t> value, nullable<uint32_t> min, nullable<uint32_t> max)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, PowerSource::Attributes::BatChargingCurrent::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_nullable_uint32(value));
    if (!attribute) {
        ESP_LOGE(TAG, "Could not create attribute");
        return NULL;
    }
    esp_matter::attribute::add_bounds(attribute, esp_matter_nullable_uint32(min), esp_matter_nullable_uint32(max));
    return attribute;
}

attribute_t *create_active_bat_charge_faults(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count)
{
    if (count > k_max_charge_faults_count) {
        ESP_LOGE(TAG, "Could not create attribute, list out of bound");
        return NULL;
    }
    return esp_matter::attribute::create(cluster, PowerSource::Attributes::ActiveBatChargeFaults::Id, ATTRIBUTE_FLAG_NONE, esp_matter_array(value, length, count));
}

} /* attribute */
} /* power_source */

namespace temperature_control {
namespace attribute {
attribute_t *create_temperature_setpoint(cluster_t *cluster, int16_t value)
{
    return esp_matter::attribute::create(cluster, TemperatureControl::Attributes::TemperatureSetpoint::Id, ATTRIBUTE_FLAG_NONE, esp_matter_int16(value));
}

attribute_t *create_min_temperature(cluster_t *cluster, const int16_t value)
{
    return esp_matter::attribute::create(cluster, TemperatureControl::Attributes::MinTemperature::Id, ATTRIBUTE_FLAG_NONE, esp_matter_int16(value));
}

attribute_t *create_max_temperature(cluster_t *cluster, const int16_t value)
{
    return esp_matter::attribute::create(cluster, TemperatureControl::Attributes::MaxTemperature::Id, ATTRIBUTE_FLAG_NONE, esp_matter_int16(value));
}

attribute_t *create_step(cluster_t *cluster, const int16_t value)
{
    return esp_matter::attribute::create(cluster, TemperatureControl::Attributes::Step::Id, ATTRIBUTE_FLAG_NONE, esp_matter_int16(value));
}

attribute_t *create_selected_temperature_level(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, TemperatureControl::Attributes::SelectedTemperatureLevel::Id, ATTRIBUTE_FLAG_NONE, esp_matter_uint8(value));
}

attribute_t *create_supported_temperature_levels(cluster_t *cluster, uint8_t * value, uint16_t length, uint16_t count)
{
    if (count > k_max_temp_level_count) {
        ESP_LOGE(TAG, "Could not create attribute, list out of bound");
        return NULL;
    }
    return esp_matter::attribute::create(cluster, TemperatureControl::Attributes::SupportedTemperatureLevels::Id, ATTRIBUTE_FLAG_NONE, esp_matter_array((uint8_t*)value, length, count));
}

} /* attribute */
} /* temperature_control */

namespace refrigerator_alarm {
namespace attribute {
attribute_t *create_mask(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, RefrigeratorAlarm::Attributes::Mask::Id, ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_state(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, RefrigeratorAlarm::Attributes::State::Id, ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

attribute_t *create_supported(cluster_t *cluster, uint32_t value)
{
    return esp_matter::attribute::create(cluster, RefrigeratorAlarm::Attributes::Supported::Id, ATTRIBUTE_FLAG_NONE, esp_matter_uint32(value));
}

} /* attribute */
} /* refrigerator_alarm */

namespace rvc_run_mode {
namespace attribute {

attribute_t *create_supported_modes(cluster_t *cluster, const uint8_t * value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, RvcRunMode::Attributes::SupportedModes::Id, ATTRIBUTE_FLAG_NONE, esp_matter_array((uint8_t*)value, length, count));
}

attribute_t *create_current_mode(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, RvcRunMode::Attributes::CurrentMode::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint8(value));
}

} /* attribute */
} /* rvc_run_mode */

namespace rvc_clean_mode {
namespace attribute {

attribute_t *create_supported_modes(cluster_t *cluster, const uint8_t * value, uint16_t length, uint16_t count)
{
    return esp_matter::attribute::create(cluster, RvcCleanMode::Attributes::SupportedModes::Id, ATTRIBUTE_FLAG_NONE, esp_matter_array((uint8_t*)value, length, count));
}

attribute_t *create_current_mode(cluster_t *cluster, uint8_t value)
{
    return esp_matter::attribute::create(cluster, RvcCleanMode::Attributes::CurrentMode::Id, ATTRIBUTE_FLAG_NONVOLATILE, esp_matter_uint8(value));
}

} /* attribute */
} /* rvc_clean_mode */

} /* cluster */
} /* esp_matter */
