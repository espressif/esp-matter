/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "rfid_nfc_reader.h"

#include <driver/i2c_master.h>
#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <M5UnitUnified.hpp>
#include <nfc/layer/a/nfc_layer_a.hpp>
#include <nfc/layer/b/nfc_layer_b.hpp>
#include <nfc/layer/f/nfc_layer_f.hpp>
#include <nfc/layer/v/nfc_layer_v.hpp>
#include <unit/unit_ST25R3916.hpp>

namespace {

const char * TAG = "rfid_nfc_reader";

#define RFID_NFC_TASK_STACK_SIZE 6144
#define RFID_NFC_TASK_PRIORITY 5

// M5Stack Unit NFC (ST25R3916) I2C address, per the hardware documentation.
constexpr uint8_t kUnitNfcI2cAddr = 0x50;
constexpr uint32_t kUnitNfcI2cFreqHz = 400000;

// Per-technology poll timeout. Each cycle tries A, B, F, V in turn, so the
// worst-case time to notice a fresh tag is roughly the sum of these.
constexpr uint32_t kPerTechTimeoutMs = 100;
constexpr uint32_t kIdlePollDelayMs = 100;

m5::unit::UnitUnified g_units;
m5::unit::UnitST25R3916 g_unit{kUnitNfcI2cAddr};
m5::nfc::NFCLayerA g_nfca{g_unit};
m5::nfc::NFCLayerB g_nfcb{g_unit};
m5::nfc::NFCLayerF g_nfcf{g_unit};
m5::nfc::NFCLayerV g_nfcv{g_unit};
i2c_master_bus_handle_t g_i2c_bus{};

// Switches the unit's active RF technology before polling with a given
// layer. The unit only demodulates one technology at a time, so the poll
// loop must reconfigure it between each attempt.
//
// UnitST25R3916::config()/config(cfg) only get/set the stored config_t --
// they do not reprogram the chip. configureNFCMode() is the call that
// actually reprograms the ST25R3916 and updates the mode NFCLayerB/F/V's
// internal guards check, so it (not config()) is what must be called here.
void SelectTech(m5::nfc::NFC tech)
{
    if (g_unit.isNFCMode(tech)) {
        return;
    }
    if (!g_unit.disableField() || !g_unit.configureNFCMode(tech)) {
        ESP_LOGW(TAG, "Failed to switch NFC mode to %d", static_cast<int>(tech));
    }
}

} // namespace

esp_err_t RfidNfcReader::Init(RfidTagDetectedCallback callback)
{
    if (mTaskHandle) {
        return ESP_ERR_INVALID_STATE;
    }
    mCallback = callback;

    i2c_master_bus_config_t bus_config = {};
    bus_config.i2c_port = CONFIG_RFID_ST25R3916_I2C_PORT;
    bus_config.sda_io_num = static_cast<gpio_num_t>(CONFIG_RFID_ST25R3916_PIN_SDA);
    bus_config.scl_io_num = static_cast<gpio_num_t>(CONFIG_RFID_ST25R3916_PIN_SCL);
    bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
    bus_config.glitch_ignore_cnt = 7;
    bus_config.flags.enable_internal_pullup = true;
    ESP_RETURN_ON_ERROR(i2c_new_master_bus(&bus_config, &g_i2c_bus), TAG, "failed to create I2C master bus");

    auto component_config = g_unit.component_config();
    component_config.clock = kUnitNfcI2cFreqHz;
    g_unit.component_config(component_config);

    auto config = g_unit.config();
    config.mode = m5::nfc::NFC::A;
    config.using_irq = false;
    config.irq = 0;
    g_unit.config(config);

    ESP_RETURN_ON_FALSE(g_units.add(g_unit, g_i2c_bus), ESP_FAIL, TAG, "failed to register M5 Unit NFC on I2C bus");

    uint8_t ic_type = 0;
    uint8_t ic_rev = 0;
    if (!g_unit.readICIdentity(ic_type, ic_rev)) {
        ESP_LOGE(TAG, "failed to read ST25R3916 identity at I2C address 0x%02x", kUnitNfcI2cAddr);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "ST25R3916 identity type=0x%02x rev=0x%02x", ic_type, ic_rev);

    ESP_RETURN_ON_FALSE(g_units.begin(), ESP_FAIL, TAG, "failed to begin M5 Unit NFC: %s", g_units.debugInfo().c_str());
    ESP_LOGI(TAG, "M5 Unit NFC initialized at I2C address 0x%02x", kUnitNfcI2cAddr);

    TaskHandle_t taskHandle = nullptr;
    xTaskCreate(PollTask, "rfid_nfc_detect", RFID_NFC_TASK_STACK_SIZE, this, RFID_NFC_TASK_PRIORITY, &taskHandle);
    mTaskHandle = taskHandle;
    return ESP_OK;
}

void RfidNfcReader::PollTask(void * context)
{
    auto * reader = static_cast<RfidNfcReader *>(context);
    while (true) {
        SelectTech(m5::nfc::NFC::A);
        m5::nfc::a::PICC piccA = {};
        if (g_nfca.detect(piccA, kPerTechTimeoutMs)) {
            ESP_LOGI(TAG, "NFC-A tag detected uid=%s", piccA.uidAsString().c_str());
            reader->mCallback(piccA.uid, piccA.size);
            vTaskDelay(pdMS_TO_TICKS(kIdlePollDelayMs));
            continue;
        }

        SelectTech(m5::nfc::NFC::B);
        m5::nfc::b::PICC piccB = {};
        if (g_nfcb.detect(piccB, /* afi = */ 0x00, kPerTechTimeoutMs)) {
            ESP_LOGI(TAG, "NFC-B tag detected");
            reader->mCallback(piccB.uid, sizeof(piccB.uid));
            vTaskDelay(pdMS_TO_TICKS(kIdlePollDelayMs));
            continue;
        }

        SelectTech(m5::nfc::NFC::F);
        m5::nfc::f::PICC piccF = {};
        if (g_nfcf.detect(piccF, kPerTechTimeoutMs)) {
            ESP_LOGI(TAG, "NFC-F (FeliCa) tag detected");
            reader->mCallback(piccF.idm, sizeof(piccF.idm));
            vTaskDelay(pdMS_TO_TICKS(kIdlePollDelayMs));
            continue;
        }

        SelectTech(m5::nfc::NFC::V);
        m5::nfc::v::PICC piccV = {};
        if (g_nfcv.detect(piccV, kPerTechTimeoutMs)) {
            ESP_LOGI(TAG, "NFC-V (ISO15693) tag detected");
            reader->mCallback(piccV.uid, sizeof(piccV.uid));
            vTaskDelay(pdMS_TO_TICKS(kIdlePollDelayMs));
            continue;
        }

        vTaskDelay(pdMS_TO_TICKS(kIdlePollDelayMs));
    }
}
