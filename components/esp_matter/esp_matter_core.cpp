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

#include <esp_check.h>
#include <esp_log.h>
#include <esp_matter.h>
#include <esp_matter_core.h>
#include <esp_matter_icd_configuration.h>
#include <esp_matter_test_event_trigger.h>
#include <nvs.h>
#include <nvs_flash.h>

#include <app/server/Dnssd.h>
#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
#include <app/server/Server.h>
#include <esp_matter_ota.h>
#ifdef CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
#include <esp_matter_nvs.h>
#include <data_model_provider/esp_matter_data_model_provider.h>
#include <esp_matter_data_model_priv.h>
#else
#include <data-model-providers/codegen/Instance.h>
#endif // CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
#include <access/Privilege.h>
#include <credentials/DeviceAttestationCredsProvider.h>
#include <credentials/FabricTable.h>
#include <credentials/GroupDataProviderImpl.h>
#include <lib/core/DataModelTypes.h>
#include <platform/CHIPDeviceLayer.h>
#include <platform/DeviceInfoProvider.h>
#include <platform/DiagnosticDataProvider.h>
#if CHIP_DEVICE_CONFIG_ENABLE_WIFI
#ifdef CONFIG_CHIP_ENABLE_EXTERNAL_PLATFORM
#ifndef EXTERNAL_ESP32UTILS_HEADER
#error "Please define EXTERNAL_ESP32UTILS_HEADER in your external platform gn/cmake file"
#endif // !EXTERNAL_ESP32UTILS_HEADER
#include EXTERNAL_ESP32UTILS_HEADER
#else
#include <platform/ESP32/ESP32Utils.h>
#endif // CONFIG_CHIP_ENABLE_EXTERNAL_PLATFORM
#endif // CHIP_DEVICE_CONFIG_ENABLE_WIFI
#include <esp_matter_ota.h>
#include <esp_matter_mem.h>
#include <esp_matter_providers.h>

using chip::DeviceLayer::ChipDeviceEvent;
using chip::DeviceLayer::ConfigurationMgr;
using chip::DeviceLayer::ConnectivityManager;
using chip::DeviceLayer::ConnectivityMgr;
using chip::DeviceLayer::PlatformMgr;
using chip::DeviceLayer::GetDiagnosticDataProvider;
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
using chip::DeviceLayer::ThreadStackMgr;
#endif

#define MAX_GROUPS_PER_FABRIC_PER_ENDPOINT CONFIG_MAX_GROUPS_PER_FABRIC_PER_ENDPOINT

static const char *TAG = "esp_matter_core";
static bool esp_matter_started = false;
static chip::CommonCaseDeviceServerInitParams *s_server_init_params = nullptr;

namespace esp_matter {

#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
namespace {

void PostEvent(uint16_t eventType)
{
    chip::DeviceLayer::ChipDeviceEvent event;
    event.Type = eventType;
    CHIP_ERROR error = chip::DeviceLayer::PlatformMgr().PostEvent(&event);
    VerifyOrReturn(error == CHIP_NO_ERROR, ESP_LOGE(TAG, "Failed to post event for event type:%" PRIu16 ", err:%" CHIP_ERROR_FORMAT, eventType, error.Format()));
}

class AppDelegateImpl : public AppDelegate
{
public:
    void OnCommissioningSessionStarted()
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kCommissioningSessionStarted);
    }

    void OnCommissioningSessionStopped()
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kCommissioningSessionStopped);
    }

    void OnCommissioningWindowOpened()
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kCommissioningWindowOpened);
    }

    void OnCommissioningWindowClosed()
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kCommissioningWindowClosed);
    }
};

class FabricDelegateImpl : public chip::FabricTable::Delegate
{
public:
    void FabricWillBeRemoved(const chip::FabricTable & fabricTable,chip::FabricIndex fabricIndex)
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kFabricWillBeRemoved);
    }

    void OnFabricRemoved(const chip::FabricTable & fabricTable,chip::FabricIndex fabricIndex)
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kFabricRemoved);
    }

    void OnFabricCommitted(const chip::FabricTable & fabricTable, chip::FabricIndex fabricIndex)
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kFabricCommitted);
    }

    void OnFabricUpdated(const chip::FabricTable & fabricTable, chip::FabricIndex fabricIndex)
    {
        PostEvent(chip::DeviceLayer::DeviceEventType::kFabricUpdated);
    }
};

AppDelegateImpl s_app_delegate;

FabricDelegateImpl s_fabric_delegate;

}  // namespace
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER

namespace lock {
#define DEFAULT_TICKS (500 / portTICK_PERIOD_MS) /* 500 ms in ticks */
status_t ScopedChipStackLock::chip_stack_lock(uint32_t ticks_to_wait)
{
#if CHIP_STACK_LOCK_TRACKING_ENABLED
    VerifyOrReturnValue(!PlatformMgr().IsChipStackLockedByCurrentThread(), ALREADY_TAKEN);
#endif
    VerifyOrReturnValue(ticks_to_wait != portMAX_DELAY, SUCCESS, PlatformMgr().LockChipStack());
    uint32_t ticks_remaining = ticks_to_wait;
    uint32_t ticks = DEFAULT_TICKS;
    while (ticks_remaining > 0) {
        VerifyOrReturnValue(!PlatformMgr().TryLockChipStack(), SUCCESS);
        ticks = ticks_remaining < DEFAULT_TICKS ? ticks_remaining : DEFAULT_TICKS;
        ticks_remaining -= ticks;
        ESP_LOGI(TAG, "Did not get lock yet. Retrying...");
        vTaskDelay(ticks);
    }
    ESP_LOGE(TAG, "Could not get lock");
    return FAILED;
}

esp_err_t ScopedChipStackLock::chip_stack_unlock()
{
    PlatformMgr().UnlockChipStack();
    return ESP_OK;
}
} /* lock */

#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
static void deinit_ble_if_commissioned(intptr_t unused)
{
#if CONFIG_USE_BLE_ONLY_FOR_COMMISSIONING
    if(chip::Server::GetInstance().GetFabricTable().FabricCount() > 0) {
        chip::DeviceLayer::Internal::BLEMgr().Shutdown();
    }
#endif /* CONFIG_USE_BLE_ONLY_FOR_COMMISSIONING */
}

static void esp_matter_chip_init_task(intptr_t context)
{
    TaskHandle_t task_to_notify = reinterpret_cast<TaskHandle_t>(context);
    static chip::CommonCaseDeviceServerInitParams defaultInitParams;

    chip::CommonCaseDeviceServerInitParams &initParams =
        s_server_init_params ? *s_server_init_params : defaultInitParams;

    if (initParams.InitializeStaticResourcesBeforeServerInit() != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to initialize static resources before server initialization");
        return;
    }
    if (!initParams.appDelegate) {
        initParams.appDelegate = &s_app_delegate;
    }
    if (!initParams.testEventTriggerDelegate) {
        initParams.testEventTriggerDelegate = test_event_trigger::get_delegate();
    }
    if (!initParams.dataModelProvider) {
#ifdef CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
        initParams.dataModelProvider = &esp_matter::data_model::provider::get_instance();
#else
        initParams.dataModelProvider = chip::app::CodegenDataModelProviderInstance(initParams.persistentStorageDelegate);
#endif
    }

#ifdef CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
    {
        // We should reserve one endpoint for root node endpoint
        uint8_t max_groups_server_cluster_count = CONFIG_ESP_MATTER_MAX_DYNAMIC_ENDPOINT_COUNT - 1;
        uint16_t max_groups_per_fabric = max_groups_server_cluster_count * MAX_GROUPS_PER_FABRIC_PER_ENDPOINT;
        static chip::Credentials::GroupDataProviderImpl groupDataProvider(max_groups_per_fabric,
                CHIP_CONFIG_MAX_GROUP_KEYS_PER_FABRIC);
        groupDataProvider.SetStorageDelegate(initParams.persistentStorageDelegate);
        groupDataProvider.SetSessionKeystore(initParams.sessionKeystore);
        if (groupDataProvider.Init() != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Failed to initialize group data provider");
        }
        initParams.groupDataProvider = &groupDataProvider;
    }
#endif // CONFIG_ESP_MATTER_ENABLE_DATA_MODEL

    CHIP_ERROR ret = chip::Server::GetInstance().GetFabricTable().AddFabricDelegate(&s_fabric_delegate);
    if (ret != CHIP_NO_ERROR)
    {
        ESP_LOGE(TAG, "Failed to add fabric delegate, err:%" CHIP_ERROR_FORMAT, ret.Format());
    }
    ret = chip::Server::GetInstance().Init(initParams);
    if (ret != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Failed to init server instance, err:%" CHIP_ERROR_FORMAT, ret.Format());
    }

#ifdef CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
    if (endpoint::enable_all() != ESP_OK) {
        ESP_LOGE(TAG, "Enable all endpoints failure");
    }
#endif // CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
#if CHIP_CONFIG_ENABLE_ICD_SERVER
    if (!icd::get_icd_server_enabled()) {
        // ICD server has been initialized in chip::Server::GetInstance().Init(). disable it here if
        // icd_server_enabled is set to false.
        chip::app::InteractionModelEngine::GetInstance()->SetICDManager(nullptr);
        chip::app::DnssdServer::Instance().SetICDManager(nullptr);
        chip::TestEventTriggerDelegate *test_event_trigger = chip::Server::GetInstance().GetTestEventTriggerDelegate();
        if (test_event_trigger) {
            test_event_trigger->RemoveHandler(&chip::Server::GetInstance().GetICDManager());
        }
        chip::Server::GetInstance().GetICDManager().Shutdown();
    }
#endif // CHIP_CONFIG_ENABLE_ICD_SERVER
    if (PlatformMgr().ScheduleWork(deinit_ble_if_commissioned, reinterpret_cast<intptr_t>(nullptr)) != CHIP_NO_ERROR) {
        ESP_LOGE(TAG, "Schedule ble deinitialization fails");
    }
    xTaskNotifyGive(task_to_notify);
}
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER

static void device_callback_internal(const ChipDeviceEvent * event, intptr_t arg)
{
    switch (event->Type)
    {
    case chip::DeviceLayer::DeviceEventType::kInterfaceIpAddressChanged:
#if CHIP_DEVICE_CONFIG_ENABLE_WIFI || CHIP_DEVICE_CONFIG_ENABLE_ETHERNET
        if (event->InterfaceIpAddressChanged.Type == chip::DeviceLayer::InterfaceIpChangeType::kIpV6_Assigned ||
                event->InterfaceIpAddressChanged.Type == chip::DeviceLayer::InterfaceIpChangeType::kIpV4_Assigned) {
            chip::app::DnssdServer::Instance().StartServer();
        }
#endif
        break;
#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    case chip::DeviceLayer::DeviceEventType::kDnssdInitialized:
        esp_matter_ota_requestor_start();
        /* Initialize binding manager */
        client::binding_manager_init();
        break;

    case chip::DeviceLayer::DeviceEventType::kCommissioningComplete:
        ESP_LOGI(TAG, "Commissioning Complete");
        if (PlatformMgr().ScheduleWork(deinit_ble_if_commissioned, reinterpret_cast<intptr_t>(nullptr)) != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Schedule ble deinitialization fails");
        }
        break;

    case chip::DeviceLayer::DeviceEventType::kCHIPoBLEConnectionClosed:
        ESP_LOGI(TAG, "BLE Disconnected");
        break;
#endif
    default:
        break;
    }
}

static esp_err_t init_thread_stack_and_start_thread_task()
{
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#ifdef CONFIG_ESP_MATTER_ENABLE_OPENTHREAD
    VerifyOrReturnError(ThreadStackMgr().InitThreadStack() == CHIP_NO_ERROR, ESP_FAIL,
                        ESP_LOGE(TAG, "Failed to initialize Thread stack"));
#if CHIP_CONFIG_ENABLE_ICD_SERVER
    if (icd::get_icd_server_enabled()) {
        VerifyOrReturnError(ConnectivityMgr().SetThreadDeviceType(ConnectivityManager::kThreadDeviceType_SleepyEndDevice) == CHIP_NO_ERROR,
                            ESP_FAIL, ESP_LOGE(TAG, "Failed to set the Thread device type"));
    } else {
        VerifyOrReturnError(ConnectivityMgr().SetThreadDeviceType(ConnectivityManager::kThreadDeviceType_MinimalEndDevice) == CHIP_NO_ERROR,
                            ESP_FAIL, ESP_LOGE(TAG, "Failed to set the Thread device type"));
    }

#elif CHIP_DEVICE_CONFIG_THREAD_FTD
    VerifyOrReturnError(ConnectivityMgr().SetThreadDeviceType(ConnectivityManager::kThreadDeviceType_Router) == CHIP_NO_ERROR,
                        ESP_FAIL, ESP_LOGE(TAG, "Failed to set the Thread device type"));
#else
    VerifyOrReturnError(ConnectivityMgr().SetThreadDeviceType(ConnectivityManager::kThreadDeviceType_MinimalEndDevice) == CHIP_NO_ERROR,
                        ESP_FAIL, ESP_LOGE(TAG, "Failed to set the Thread device type"));
#endif
    VerifyOrReturnError(ThreadStackMgr().StartThreadTask() == CHIP_NO_ERROR, ESP_FAIL,
                        ESP_LOGE(TAG, "Failed to launch Thread task"));
#endif // CONFIG_ESP_MATTER_ENABLE_OPENTHREAD
#endif // CHIP_DEVICE_CONFIG_ENABLE_THREAD
    return ESP_OK;
}

static esp_err_t chip_init(event_callback_t callback, intptr_t callback_arg)
{
    VerifyOrReturnError(chip::Platform::MemoryInit() == CHIP_NO_ERROR, ESP_ERR_NO_MEM, ESP_LOGE(TAG, "Failed to initialize CHIP memory pool"));
    VerifyOrReturnError(PlatformMgr().InitChipStack() == CHIP_NO_ERROR, ESP_FAIL, ESP_LOGE(TAG, "Failed to initialize CHIP stack"));

    setup_providers();
    // ConnectivityMgr().SetWiFiAPMode(ConnectivityManager::kWiFiAPMode_Enabled);
    if (PlatformMgr().StartEventLoopTask() != CHIP_NO_ERROR) {
        chip::Platform::MemoryShutdown();
        ESP_LOGE(TAG, "Failed to launch Matter main task");
        return ESP_FAIL;
    }
    if (PlatformMgr().AddEventHandler(device_callback_internal, static_cast<intptr_t>(NULL))  != CHIP_NO_ERROR) {
        (void)PlatformMgr().StopEventLoopTask();
        chip::Platform::MemoryShutdown();
        ESP_LOGE(TAG, "Failed to add internal device callback");
        return ESP_FAIL;
    }
    if(callback) {
        if (PlatformMgr().AddEventHandler(callback, callback_arg) != CHIP_NO_ERROR) {
            (void)PlatformMgr().StopEventLoopTask();
            chip::Platform::MemoryShutdown();
            ESP_LOGE(TAG, "Failed to add user callback");
            return ESP_FAIL;
       }
    }
    init_thread_stack_and_start_thread_task();
#if CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    if (PlatformMgr().ScheduleWork(esp_matter_chip_init_task, reinterpret_cast<intptr_t>(xTaskGetCurrentTaskHandle())) != CHIP_NO_ERROR) {
        (void)PlatformMgr().StopEventLoopTask();
        chip::Platform::MemoryShutdown();
        ESP_LOGE(TAG, "Failed to schedule chip init task");
        return ESP_FAIL;
    }
    // Wait for the matter stack to be initialized
    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER

    return ESP_OK;
}

bool is_started()
{
    return esp_matter_started;
}

esp_err_t set_server_init_params(chip::CommonCaseDeviceServerInitParams *server_init_params)
{
    VerifyOrReturnError(!esp_matter_started, ESP_ERR_INVALID_STATE, ESP_LOGE(TAG, "esp_matter has started"));
    s_server_init_params = server_init_params;
    return ESP_OK;
}

esp_err_t start(event_callback_t callback, intptr_t callback_arg)
{
    VerifyOrReturnError(!esp_matter_started, ESP_ERR_INVALID_STATE, ESP_LOGE(TAG, "esp_matter has started"));
    esp_err_t err = esp_event_loop_create_default();

    // In case create event loop returns ESP_ERR_INVALID_STATE it is not necessary to fail startup
    // as of it means that default event loop is already initialized and no additional actions should be done.
    VerifyOrReturnError((err == ESP_OK || err == ESP_ERR_INVALID_STATE), err, ESP_LOGE(TAG, "Error create default event loop"));
#if CHIP_DEVICE_CONFIG_ENABLE_WIFI
    VerifyOrReturnError(chip::DeviceLayer::Internal::ESP32Utils::InitWiFiStack() == CHIP_NO_ERROR, ESP_FAIL, ESP_LOGE(TAG, "Error initializing Wi-Fi stack"));
#endif // CHIP_DEVICE_CONFIG_ENABLE_WIFI
#ifdef CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
    esp_matter_ota_requestor_init();
#endif // CONFIG_ESP_MATTER_ENABLE_DATA_MODEL

    err = chip_init(callback, callback_arg);
    VerifyOrReturnError(err == ESP_OK, err, ESP_LOGE(TAG, "Error initializing matter"));
#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#if defined(CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER) && defined(CONFIG_ESP_MATTER_ENABLE_OPENTHREAD)
    // If Thread is Provisioned, publish the dns service
    if (chip::DeviceLayer::ConnectivityMgr().IsThreadProvisioned() &&
        (chip::Server::GetInstance().GetFabricTable().FabricCount() != 0)) {
        PlatformMgr().ScheduleWork([](intptr_t){ chip::app::DnssdServer::Instance().StartServer(); },
                                   reinterpret_cast<intptr_t>(nullptr));
    }
#endif // CONFIG_ESP_MATTER_ENABLE_OPENTHREAD
#endif // CHIP_DEVICE_CONFIG_ENABLE_THREAD
    esp_matter_started = true;
#if defined(CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER) && defined(CONFIG_ESP_MATTER_ENABLE_DATA_MODEL)
    err = node::read_min_unused_endpoint_id();
    // If the min_unused_endpoint_id is not found, we will write the current min_unused_endpoint_id in nvs.
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        err = node::store_min_unused_endpoint_id();
    }
#endif // defined(CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER) && defined(CONFIG_ESP_MATTER_ENABLE_DATA_MODEL)
    return err;
}

esp_err_t factory_reset()
{
    esp_err_t err = ESP_OK;
#ifdef CONFIG_ESP_MATTER_ENABLE_DATA_MODEL
    node_t *node = node::get();
    if (node) {
        /* ESP Matter data model is used. Erase all the data that we have added in nvs. */
        nvs_handle_t handle;
        err = nvs_open_from_partition(ESP_MATTER_NVS_PART_NAME, ESP_MATTER_KVS_NAMESPACE, NVS_READWRITE, &handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to open esp_matter nvs partition ");
        } else {
            err = nvs_erase_all(handle);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to erase esp_matter nvs namespace");
            } else {
                nvs_commit(handle);
            }
            nvs_close(handle);
        }
    }
#endif
    /* Submodule factory reset. This also restarts after completion. */
#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    chip::Server::GetInstance().ScheduleFactoryReset();
#else // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    ConfigurationMgr().InitiateFactoryReset();
#endif // !CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
    return err;
}

esp_err_t esp_matter_nvs_init()
{
#ifdef CONFIG_NVS_ENCRYPTION
    nvs_sec_cfg_t cfg = {};
    const esp_partition_t * key_part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS_KEYS, NULL);
    VerifyOrReturnError(key_part != NULL, ESP_ERR_NOT_FOUND,
                        ESP_LOGE(TAG, "NVS encrypt is enabled, but no nvs_keys partition found"));

    VerifyOrReturnError(nvs_flash_read_security_cfg(key_part, &cfg) == ESP_OK, ESP_FAIL,
                        ESP_LOGE(TAG, "Failed to read NVS security cfg"));

    return nvs_flash_secure_init_partition(CONFIG_ESP_MATTER_NVS_PART_NAME, &cfg);
#else
    return nvs_flash_init_partition(CONFIG_ESP_MATTER_NVS_PART_NAME);
#endif
}

} /* esp_matter */
