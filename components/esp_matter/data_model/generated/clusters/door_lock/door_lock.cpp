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
#include <esp_matter_delegate_callbacks.h>
#include <door_lock.h>
#include <door_lock_ids.h>
#include <binding.h>
#include <esp_matter_data_model_priv.h>

using namespace chip::app::Clusters;
using chip::app::CommandHandler;
using chip::app::DataModel::Decode;
using chip::TLV::TLVReader;
using namespace esp_matter;
using namespace esp_matter::cluster;
using namespace esp_matter::cluster::delegate_cb;

static const char *TAG = "door_lock_cluster";
constexpr uint16_t cluster_revision = 10;

static esp_err_t esp_matter_command_callback_lock_door(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                       void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::LockDoor::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterLockDoorCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_unlock_door(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                         void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::UnlockDoor::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterUnlockDoorCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_unlock_with_timeout(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                 void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::UnlockWithTimeout::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterUnlockWithTimeoutCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_week_day_schedule(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                   void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::SetWeekDaySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterSetWeekDayScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_get_week_day_schedule(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                   void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::GetWeekDaySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterGetWeekDayScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_clear_week_day_schedule(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                     void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::ClearWeekDaySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterClearWeekDayScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_year_day_schedule(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                   void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::SetYearDaySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterSetYearDayScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_get_year_day_schedule(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                   void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::GetYearDaySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterGetYearDayScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_clear_year_day_schedule(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                     void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::ClearYearDaySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterClearYearDayScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_holiday_schedule(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                  void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::SetHolidaySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterSetHolidayScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_get_holiday_schedule(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                  void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::GetHolidaySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterGetHolidayScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_clear_holiday_schedule(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                    void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::ClearHolidaySchedule::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterClearHolidayScheduleCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_unbolt_door(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                         void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::UnboltDoor::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterUnboltDoorCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_set_aliro_reader_config(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                     void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::SetAliroReaderConfig::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterSetAliroReaderConfigCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

static esp_err_t esp_matter_command_callback_clear_aliro_reader_config(const ConcreteCommandPath &command_path, TLVReader &tlv_data,
                                                                       void *opaque_ptr)
{
    chip::app::Clusters::DoorLock::Commands::ClearAliroReaderConfig::DecodableType command_data;
    CHIP_ERROR error = Decode(tlv_data, command_data);
    if (error == CHIP_NO_ERROR) {
        emberAfDoorLockClusterClearAliroReaderConfigCallback((CommandHandler *)opaque_ptr, command_path, command_data);
    }
    return ESP_OK;
}

namespace esp_matter {
namespace cluster {
namespace door_lock {

namespace feature {
namespace pin_credential {
uint32_t get_id()
{
    return PINCredential::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_number_of_pin_users_supported(cluster, config->number_of_pin_users_supported);
    attribute::create_max_pin_code_length(cluster, config->max_pin_code_length);
    attribute::create_min_pin_code_length(cluster, config->min_pin_code_length);
    attribute::create_require_pin_for_remote_operation(cluster, config->require_pin_for_remote_operation);

    return ESP_OK;
}
} /* pin_credential */

namespace weekday_access_schedules {
uint32_t get_id()
{
    return WeekdayAccessSchedules::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_number_of_week_day_schedules_supported_per_user(cluster, config->number_of_week_day_schedules_supported_per_user);
    command::create_set_week_day_schedule(cluster);
    command::create_get_week_day_schedule(cluster);
    command::create_get_week_day_schedule_response(cluster);
    command::create_clear_week_day_schedule(cluster);

    return ESP_OK;
}
} /* weekday_access_schedules */

namespace door_position_sensor {
uint32_t get_id()
{
    return DoorPositionSensor::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_door_state(cluster, config->door_state);
    event::create_door_state_change(cluster);

    return ESP_OK;
}
} /* door_position_sensor */

namespace credential_over_the_air_access {
uint32_t get_id()
{
    return CredentialOverTheAirAccess::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_require_pin_for_remote_operation(cluster, config->require_pin_for_remote_operation);

    return ESP_OK;
}
} /* credential_over_the_air_access */

namespace year_day_access_schedules {
uint32_t get_id()
{
    return YearDayAccessSchedules::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_number_of_year_day_schedules_supported_per_user(cluster, config->number_of_year_day_schedules_supported_per_user);
    command::create_set_year_day_schedule(cluster);
    command::create_get_year_day_schedule(cluster);
    command::create_get_year_day_schedule_response(cluster);
    command::create_clear_year_day_schedule(cluster);

    return ESP_OK;
}
} /* year_day_access_schedules */

namespace holiday_schedules {
uint32_t get_id()
{
    return HolidaySchedules::Id;
}

esp_err_t add(cluster_t *cluster, config_t *config)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    VerifyOrReturnError(config, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_number_of_holiday_schedules_supported(cluster, config->number_of_holiday_schedules_supported);
    command::create_set_holiday_schedule(cluster);
    command::create_get_holiday_schedule(cluster);
    command::create_get_holiday_schedule_response(cluster);
    command::create_clear_holiday_schedule(cluster);

    return ESP_OK;
}
} /* holiday_schedules */

namespace unbolting {
uint32_t get_id()
{
    return Unbolting::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    command::create_unbolt_door(cluster);

    return ESP_OK;
}
} /* unbolting */

namespace aliro_provisioning {
uint32_t get_id()
{
    return AliroProvisioning::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_aliro_reader_verification_key(cluster, NULL, 0);
    attribute::create_aliro_reader_group_identifier(cluster, NULL, 0);
    attribute::create_aliro_reader_group_sub_identifier(cluster, NULL, 0);
    attribute::create_aliro_expedited_transaction_supported_protocol_versions(cluster, NULL, 0, 0);
    attribute::create_number_of_aliro_credential_issuer_keys_supported(cluster, 0);
    attribute::create_number_of_aliro_endpoint_keys_supported(cluster, 0);
    command::create_set_aliro_reader_config(cluster);
    command::create_clear_aliro_reader_config(cluster);

    return ESP_OK;
}
} /* aliro_provisioning */

namespace aliro_bleuwb {
uint32_t get_id()
{
    return AliroBLEUWB::Id;
}

esp_err_t add(cluster_t *cluster)
{
    VerifyOrReturnError(cluster, ESP_ERR_INVALID_ARG);
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnError(has_feature(aliro_provisioning), ESP_ERR_INVALID_ARG);
    update_feature_map(cluster, get_id());
    attribute::create_aliro_group_resolving_key(cluster, NULL, 0);
    attribute::create_aliro_supported_bleuwb_protocol_versions(cluster, NULL, 0, 0);
    attribute::create_aliro_ble_advertising_version(cluster, 0);

    return ESP_OK;
}
} /* aliro_bleuwb */

} /* feature */

namespace attribute {
attribute_t *create_lock_state(cluster_t *cluster, nullable<uint8_t> value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, LockState::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(nullable<uint8_t>(3), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_lock_type(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, LockType::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(11), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_actuator_enabled(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, ActuatorEnabled::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
}

attribute_t *create_door_state(cluster_t *cluster, nullable<uint8_t> value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(door_position_sensor), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, DoorState::Id, ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(nullable<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(nullable<uint8_t>(5), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_door_open_events(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, DoorOpenEvents::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_door_closed_events(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, DoorClosedEvents::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_open_period(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, OpenPeriod::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_number_of_pin_users_supported(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(pin_credential), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, NumberOfPINUsersSupported::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0)), esp_matter_attr_val(static_cast<uint16_t>(65534)));
    return attribute;
}

attribute_t *create_number_of_week_day_schedules_supported_per_user(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(weekday_access_schedules), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, NumberOfWeekDaySchedulesSupportedPerUser::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(253)));
    return attribute;
}

attribute_t *create_number_of_year_day_schedules_supported_per_user(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(year_day_access_schedules), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, NumberOfYearDaySchedulesSupportedPerUser::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(253)));
    return attribute;
}

attribute_t *create_number_of_holiday_schedules_supported(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(holiday_schedules), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, NumberOfHolidaySchedulesSupported::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(253)));
    return attribute;
}

attribute_t *create_max_pin_code_length(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(pin_credential), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MaxPINCodeLength::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_min_pin_code_length(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(pin_credential), NULL);
    attribute_t *attribute = esp_matter::attribute::create(cluster, MinPINCodeLength::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0)), esp_matter_attr_val(static_cast<uint8_t>(254)));
    return attribute;
}

attribute_t *create_language(cluster_t *cluster, char *value, uint16_t length)
{
    VerifyOrReturnValue(length <= k_max_language_length + 1, NULL, ESP_LOGE(TAG, "Could not create attribute, string length out of bound"));
    return esp_matter::attribute::create(cluster, Language::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value, length), k_max_language_length + 1);
}

attribute_t *create_led_settings(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, LEDSettings::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(2), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_auto_relock_time(cluster_t *cluster, uint32_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, AutoRelockTime::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint32_t>(0)), esp_matter_attr_val(static_cast<uint32_t>(4294967294)));
    return attribute;
}

attribute_t *create_sound_volume(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SoundVolume::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(3), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_operating_mode(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, OperatingMode::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_enum));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_enum), esp_matter_attr_val(static_cast<uint8_t>(4), esp_matter_attr_val::uint_sub_type::k_enum));
    return attribute;
}

attribute_t *create_supported_operating_modes(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, SupportedOperatingModes::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint16_t>(65535), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_default_configuration_register(cluster_t *cluster, uint16_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, DefaultConfigurationRegister::Id, ATTRIBUTE_FLAG_NONE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint16_t>(0), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint16_t>(65535), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_enable_local_programming(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, EnableLocalProgramming::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value));
}

attribute_t *create_enable_one_touch_locking(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, EnableOneTouchLocking::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value));
}

attribute_t *create_enable_inside_status_led(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, EnableInsideStatusLED::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value));
}

attribute_t *create_enable_privacy_mode_button(cluster_t *cluster, bool value)
{
    return esp_matter::attribute::create(cluster, EnablePrivacyModeButton::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value));
}

attribute_t *create_local_programming_features(cluster_t *cluster, uint8_t value)
{
    attribute_t *attribute = esp_matter::attribute::create(cluster, LocalProgrammingFeatures::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value, esp_matter_attr_val::uint_sub_type::k_bitmap));
    esp_matter::attribute::add_bounds(attribute, esp_matter_attr_val(static_cast<uint8_t>(0), esp_matter_attr_val::uint_sub_type::k_bitmap), esp_matter_attr_val(static_cast<uint8_t>(15), esp_matter_attr_val::uint_sub_type::k_bitmap));
    return attribute;
}

attribute_t *create_require_pin_for_remote_operation(cluster_t *cluster, bool value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(((has_feature(credential_over_the_air_access)) && (has_feature(pin_credential))), NULL);
    return esp_matter::attribute::create(cluster, RequirePINforRemoteOperation::Id, ATTRIBUTE_FLAG_WRITABLE, esp_matter_attr_val(value));
}

attribute_t *create_aliro_reader_verification_key(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(aliro_provisioning), NULL);
    return esp_matter::attribute::create(cluster, AliroReaderVerificationKey::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length));
}

attribute_t *create_aliro_reader_group_identifier(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(aliro_provisioning), NULL);
    return esp_matter::attribute::create(cluster, AliroReaderGroupIdentifier::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length));
}

attribute_t *create_aliro_reader_group_sub_identifier(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(aliro_provisioning), NULL);
    return esp_matter::attribute::create(cluster, AliroReaderGroupSubIdentifier::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length));
}

attribute_t *create_aliro_expedited_transaction_supported_protocol_versions(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(aliro_provisioning), NULL);
    return esp_matter::attribute::create(cluster, AliroExpeditedTransactionSupportedProtocolVersions::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_aliro_group_resolving_key(cluster_t *cluster, uint8_t *value, uint16_t length)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(aliro_bleuwb), NULL);
    return esp_matter::attribute::create(cluster, AliroGroupResolvingKey::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY | ATTRIBUTE_FLAG_NULLABLE, esp_matter_attr_val(value, length));
}

attribute_t *create_aliro_supported_bleuwb_protocol_versions(cluster_t *cluster, uint8_t *value, uint16_t length, uint16_t count)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(aliro_bleuwb), NULL);
    return esp_matter::attribute::create(cluster, AliroSupportedBLEUWBProtocolVersions::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value, length, count));
}

attribute_t *create_aliro_ble_advertising_version(cluster_t *cluster, uint8_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(aliro_bleuwb), NULL);
    return esp_matter::attribute::create(cluster, AliroBLEAdvertisingVersion::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value));
}

attribute_t *create_number_of_aliro_credential_issuer_keys_supported(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(aliro_provisioning), NULL);
    return esp_matter::attribute::create(cluster, NumberOfAliroCredentialIssuerKeysSupported::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value));
}

attribute_t *create_number_of_aliro_endpoint_keys_supported(cluster_t *cluster, uint16_t value)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(aliro_provisioning), NULL);
    return esp_matter::attribute::create(cluster, NumberOfAliroEndpointKeysSupported::Id, ATTRIBUTE_FLAG_MANAGED_INTERNALLY, esp_matter_attr_val(value));
}

} /* attribute */
namespace command {
command_t *create_lock_door(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, LockDoor::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_lock_door);
}

command_t *create_unlock_door(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, UnlockDoor::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_unlock_door);
}

command_t *create_unlock_with_timeout(cluster_t *cluster)
{
    return esp_matter::command::create(cluster, UnlockWithTimeout::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_unlock_with_timeout);
}

command_t *create_set_week_day_schedule(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(weekday_access_schedules), NULL);
    return esp_matter::command::create(cluster, SetWeekDaySchedule::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_set_week_day_schedule);
}

command_t *create_get_week_day_schedule(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(weekday_access_schedules), NULL);
    return esp_matter::command::create(cluster, GetWeekDaySchedule::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_get_week_day_schedule);
}

command_t *create_get_week_day_schedule_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(weekday_access_schedules), NULL);
    return esp_matter::command::create(cluster, GetWeekDayScheduleResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_clear_week_day_schedule(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(weekday_access_schedules), NULL);
    return esp_matter::command::create(cluster, ClearWeekDaySchedule::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_clear_week_day_schedule);
}

command_t *create_set_year_day_schedule(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(year_day_access_schedules), NULL);
    return esp_matter::command::create(cluster, SetYearDaySchedule::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_set_year_day_schedule);
}

command_t *create_get_year_day_schedule(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(year_day_access_schedules), NULL);
    return esp_matter::command::create(cluster, GetYearDaySchedule::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_get_year_day_schedule);
}

command_t *create_get_year_day_schedule_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(year_day_access_schedules), NULL);
    return esp_matter::command::create(cluster, GetYearDayScheduleResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_clear_year_day_schedule(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(year_day_access_schedules), NULL);
    return esp_matter::command::create(cluster, ClearYearDaySchedule::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_clear_year_day_schedule);
}

command_t *create_set_holiday_schedule(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(holiday_schedules), NULL);
    return esp_matter::command::create(cluster, SetHolidaySchedule::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_set_holiday_schedule);
}

command_t *create_get_holiday_schedule(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(holiday_schedules), NULL);
    return esp_matter::command::create(cluster, GetHolidaySchedule::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_get_holiday_schedule);
}

command_t *create_get_holiday_schedule_response(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(holiday_schedules), NULL);
    return esp_matter::command::create(cluster, GetHolidayScheduleResponse::Id, COMMAND_FLAG_GENERATED, NULL);
}

command_t *create_clear_holiday_schedule(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(holiday_schedules), NULL);
    return esp_matter::command::create(cluster, ClearHolidaySchedule::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_clear_holiday_schedule);
}

command_t *create_unbolt_door(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(unbolting), NULL);
    return esp_matter::command::create(cluster, UnboltDoor::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_unbolt_door);
}

command_t *create_set_aliro_reader_config(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(aliro_provisioning), NULL);
    return esp_matter::command::create(cluster, SetAliroReaderConfig::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_set_aliro_reader_config);
}

command_t *create_clear_aliro_reader_config(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(aliro_provisioning), NULL);
    return esp_matter::command::create(cluster, ClearAliroReaderConfig::Id, COMMAND_FLAG_ACCEPTED, esp_matter_command_callback_clear_aliro_reader_config);
}

} /* command */

namespace event {
event_t *create_door_lock_alarm(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, DoorLockAlarm::Id);
}

event_t *create_door_state_change(cluster_t *cluster)
{
    uint32_t feature_map = get_feature_map_value(cluster);
    VerifyOrReturnValue(has_feature(door_position_sensor), NULL);
    return esp_matter::event::create(cluster, DoorStateChange::Id);
}

event_t *create_lock_operation(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, LockOperation::Id);
}

event_t *create_lock_operation_error(cluster_t *cluster)
{
    return esp_matter::event::create(cluster, LockOperationError::Id);
}

} /* event */

static void create_default_binding_cluster(endpoint_t *endpoint)
{
    binding::config_t config;
    binding::create(endpoint, &config, CLUSTER_FLAG_SERVER);
}

const function_generic_t function_list[] = {
    (function_generic_t)MatterDoorLockClusterServerAttributeChangedCallback,
    (function_generic_t)MatterDoorLockClusterServerShutdownCallback,
    (function_generic_t)MatterDoorLockClusterServerPreAttributeChangedCallback,
};

const int function_flags = CLUSTER_FLAG_ATTRIBUTE_CHANGED_FUNCTION | CLUSTER_FLAG_SHUTDOWN_FUNCTION | CLUSTER_FLAG_PRE_ATTRIBUTE_CHANGED_FUNCTION;

cluster_t *create(endpoint_t *endpoint, config_t *config, uint8_t flags)
{
    cluster_t *cluster = esp_matter::cluster::create(endpoint, door_lock::Id, flags);
    VerifyOrReturnValue(cluster, NULL, ESP_LOGE(TAG, "Could not create cluster. cluster_id: 0x%08" PRIX32, door_lock::Id));
    if (flags & CLUSTER_FLAG_SERVER) {
        VerifyOrReturnValue(config != NULL, ABORT_CLUSTER_CREATE(cluster));
        if (config->delegate != nullptr) {
            static const auto delegate_init_cb = DoorLockDelegateInitCB;
            set_delegate_and_init_callback(cluster, delegate_init_cb, config->delegate);
        }
        static const auto plugin_server_init_cb = CALL_ONCE(MatterDoorLockPluginServerInitCallback);
        set_plugin_server_init_callback(cluster, plugin_server_init_cb);
        add_function_list(cluster, function_list, function_flags);

        /* Attributes managed internally */
        global::attribute::create_feature_map(cluster, 0);

        /* Attributes not managed internally */
        global::attribute::create_cluster_revision(cluster, cluster_revision);

        attribute::create_lock_state(cluster, config->lock_state);
        attribute::create_lock_type(cluster, config->lock_type);
        attribute::create_actuator_enabled(cluster, config->actuator_enabled);
        attribute::create_operating_mode(cluster, config->operating_mode);
        attribute::create_supported_operating_modes(cluster, config->supported_operating_modes);
        command::create_lock_door(cluster);
        command::create_unlock_door(cluster);
        /* Events */
        event::create_door_lock_alarm(cluster);
        event::create_lock_operation(cluster);
        event::create_lock_operation_error(cluster);
    }

    if (flags & CLUSTER_FLAG_CLIENT) {
        create_default_binding_cluster(endpoint);
    }
    return cluster;
}

} /* door_lock */
} /* cluster */
} /* esp_matter */
