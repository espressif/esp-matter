/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <m5nfc.h>

#include <driver/i2c_master.h>
#include <esp_check.h>
#include <esp_err.h>
#include <esp_log.h>

#include <M5UnitUnified.hpp>
#include <nfc/isoDEP/isoDEP.hpp>
#include <nfc/layer/a/nfc_layer_a.hpp>
#include <unit/unit_ST25R3916.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace {

constexpr char kTag[] = "M5Nfc";
constexpr uint8_t kUnitNfcI2cAddr = 0x50;
constexpr uint32_t kUnitNfcI2cFreqHz = 400000;
constexpr uint8_t kIsoDepFsdi = 8;
constexpr unsigned kApduMaxAttempts = 3;

m5::unit::UnitUnified g_units;
m5::unit::UnitST25R3916 g_unit{kUnitNfcI2cAddr};
m5::nfc::NFCLayerA g_nfca{g_unit};
i2c_master_bus_handle_t g_i2c_bus{};

void log_i2c_scan()
{
    bool found = false;
    for (uint8_t addr = 0x08; addr < 0x78; ++addr) {
        if (i2c_master_probe(g_i2c_bus, addr, 50) == ESP_OK) {
            ESP_LOGI(kTag, "I2C device found at 7-bit address 0x%02x", addr);
            found = true;
        }
    }
    if (!found) {
        ESP_LOGW(kTag,
                 "no I2C devices found on SDA=%d SCL=%d; check wiring, pull-ups, and external port power. "
                 "Some M5Stack controllers require M5.Power.setExtOutput(true) before m5nfc_init().",
                 CONFIG_ST25R3916_PIN_SDA, CONFIG_ST25R3916_PIN_SCL);
    }
}

void apply_aliro_iso_dep_policy()
{
    auto *iso_dep = g_nfca.isoDEP();
    auto cfg = iso_dep->config();
    cfg.fwt_ms = std::max<uint32_t>(cfg.fwt_ms, CONFIG_ST25R3916_TRANSCEIVE_TIMEOUT_MS);
    cfg.wtx_max_ms = std::max<uint32_t>(cfg.wtx_max_ms, cfg.fwt_ms);
    cfg.use_cid = false;
    cfg.cid = 0;
    cfg.pcd_max_frame_rx = m5::unit::st25r3916::MAX_FIFO_DEPTH;
    cfg.pcd_max_frame_tx = m5::unit::st25r3916::MAX_FIFO_DEPTH;
    iso_dep->config(cfg);
}

esp_err_t exchange_single_apdu(const uint8_t *command, size_t command_len, uint8_t *response, size_t *response_len)
{
    uint16_t rx_len = static_cast<uint16_t>(*response_len);
    if (g_nfca.isoDEP()->transceiveINF(response, rx_len, command, static_cast<uint16_t>(command_len))) {
        *response_len = rx_len;
        ESP_LOGI(kTag, "APDU RX len=%u", static_cast<unsigned>(*response_len));
        ESP_LOG_BUFFER_HEX_LEVEL(kTag, response, *response_len, ESP_LOG_DEBUG);
        return ESP_OK;
    }

    ESP_LOGW(kTag, "APDU exchange failed rx_len=%u fwt_ms=%u", rx_len, g_nfca.isoDEP()->config().fwt_ms);
    return ESP_FAIL;
}

} // namespace

esp_err_t m5nfc_init(void)
{
    i2c_master_bus_config_t bus_config = {};
    bus_config.i2c_port = CONFIG_ST25R3916_I2C_PORT;
    bus_config.sda_io_num = static_cast<gpio_num_t>(CONFIG_ST25R3916_PIN_SDA);
    bus_config.scl_io_num = static_cast<gpio_num_t>(CONFIG_ST25R3916_PIN_SCL);
    bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
    bus_config.glitch_ignore_cnt = 7;
    bus_config.flags.enable_internal_pullup = true;
    ESP_RETURN_ON_ERROR(i2c_new_master_bus(&bus_config, &g_i2c_bus), kTag, "failed to create I2C master bus");

    auto component_config = g_unit.component_config();
    component_config.clock = kUnitNfcI2cFreqHz;
    g_unit.component_config(component_config);

    auto config = g_unit.config();
    config.mode = m5::nfc::NFC::A;
    config.using_irq = false;
    config.irq = 0;
    g_unit.config(config);

    ESP_RETURN_ON_FALSE(g_units.add(g_unit, g_i2c_bus), ESP_FAIL, kTag, "failed to register M5 Unit NFC on I2C bus");

    uint8_t ic_type = 0;
    uint8_t ic_rev = 0;
    if (!g_unit.readICIdentity(ic_type, ic_rev)) {
        ESP_LOGE(kTag, "failed to read ST25R3916 identity at I2C address 0x%02x", kUnitNfcI2cAddr);
        log_i2c_scan();
        return ESP_FAIL;
    }
    ESP_LOGI(kTag, "ST25R3916 identity type=0x%02x rev=0x%02x", ic_type, ic_rev);

    ESP_RETURN_ON_FALSE(g_units.begin(), ESP_FAIL, kTag, "failed to begin M5 Unit NFC: %s",
                        g_units.debugInfo().c_str());
    ESP_LOGI(kTag, "M5 Unit NFC initialized at I2C address 0x%02x", kUnitNfcI2cAddr);
    return ESP_OK;
}

void m5nfc_update(void)
{
    g_units.update();
}

bool m5nfc_activate(void)
{
    auto nfca_config = g_nfca.config();
    nfca_config.fsdi = kIsoDepFsdi;
    nfca_config.cid = 0;
    g_nfca.config(nfca_config);

    m5::nfc::a::PICC picc = {};
    if (!g_nfca.request(picc.atqa) || !g_nfca.select(picc) || !picc.isISO14443_4()) {
        return false;
    }

    apply_aliro_iso_dep_policy();

    auto iso_dep_config = g_nfca.isoDEP()->config();
    ESP_LOGI(kTag, "ATS len=%u FSCI=%u ISO-DEP FSC=%u FSDI=%u FWT=%u ms", picc.ats.TL, picc.ats.fsci(),
             iso_dep_config.fsc, kIsoDepFsdi, iso_dep_config.fwt_ms);

    ESP_LOGI(kTag, "NFC-A ISO-DEP activated ATQA=%04x SAK=%02x UID=%s", picc.atqa, picc.sak,
             picc.uidAsString().c_str());
    return true;
}

void m5nfc_deactivate(void)
{
    (void)g_nfca.deactivate();
}

esp_err_t m5nfc_message_exchange(const uint8_t *command, size_t command_len, uint8_t *response, size_t *response_len)
{
    ESP_RETURN_ON_FALSE(command && response && response_len, ESP_ERR_INVALID_ARG, kTag, "invalid APDU buffer");
    ESP_RETURN_ON_FALSE(command_len <= UINT16_MAX && *response_len <= UINT16_MAX, ESP_ERR_INVALID_SIZE, kTag,
                        "APDU buffer too large");

    ESP_LOGI(kTag, "APDU TX len=%u cla=%02x ins=%02x", static_cast<unsigned>(command_len),
             command_len > 0 ? command[0] : 0, command_len > 1 ? command[1] : 0);
    ESP_LOG_BUFFER_HEX_LEVEL(kTag, command, command_len, ESP_LOG_DEBUG);

    esp_err_t ret = ESP_FAIL;
    for (unsigned attempt = 1; attempt <= kApduMaxAttempts; ++attempt) {
        ret = exchange_single_apdu(command, command_len, response, response_len);
        if (ret == ESP_OK) {
            if (attempt > 1) {
                ESP_LOGI(kTag, "APDU exchange succeeded after %u attempts", attempt);
            }
            return ESP_OK;
        }
        ESP_LOGW(kTag, "APDU exchange attempt %u/%u failed", attempt, kApduMaxAttempts);
    }

    return ret;
}
