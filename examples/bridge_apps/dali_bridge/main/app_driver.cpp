/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <esp_matter.h>
#include <app_priv.h>
#include <app_bridged_device.h>
#include <common_macros.h>
#include <platform/CHIPDeviceLayer.h>

#include <dali.h>

using namespace chip::app::Clusters;
using namespace esp_matter;

static const char *TAG = "app_driver";

#define DALI_STATUS_LAMP_FAILURE_BIT 0x02U /* bit 1 of QUERY STATUS response */
#define REACH_CONFIRM_NEEDED 2 /* consecutive passing polls to go reachable */

/* DALI master handle (singleton for this application) */
static dali_master_handle_t s_dali_handle = NULL;

/**
 * Map a Matter LevelControl CurrentLevel (1–254) to a DALI DAPC value (1–254).
 * Matter level 0 means off and is handled by the OnOff cluster, not here.
 */
static inline uint8_t matter_level_to_dali(uint8_t matter_level)
{
    if (matter_level == 0) {
        return 0;
    }
    return (matter_level > 254) ? 254 : matter_level;
}

/* -----------------------------------------------------------------------
 * Per-ballast control
 * ----------------------------------------------------------------------- */

static esp_err_t dali_ballast_set_power(dali_light_ctx_t *ctx, bool on)
{
#if DALI_MOCK_NO_HW
    ctx->power = on;
    ESP_LOGI(TAG, "[MOCK] addr=%u power -> %s", ctx->dali_addr, on ? "ON" : "OFF");
    return ESP_OK;
#endif
    esp_err_t err;
    if (on) {
        if (ctx->dali_level == 0) {
            /* No previous level stored — recall max */
            dali_master_transaction_config_t tx_cfg = {
                .addr_type = DALI_ADDR_SHORT,
                .addr = ctx->dali_addr,
                .is_cmd = true,
                .command = DALI_CMD_RECALL_MAX_LEVEL,
                .send_twice = false,
                .tx_timeout_ms = DALI_TX_TIMEOUT_MS,
            };
            err = dali_master_do_transaction(s_dali_handle, &tx_cfg, NULL);
        } else {
            /* Restore last known DAPC level */
            dali_master_transaction_config_t tx_cfg = {
                .addr_type = DALI_ADDR_SHORT,
                .addr = ctx->dali_addr,
                .is_cmd = false, /* DAPC */
                .command = ctx->dali_level,
                .send_twice = false,
                .tx_timeout_ms = DALI_TX_TIMEOUT_MS,
            };
            err = dali_master_do_transaction(s_dali_handle, &tx_cfg, NULL);
        }
    } else {
        dali_master_transaction_config_t tx_cfg = {
            .addr_type = DALI_ADDR_SHORT,
            .addr = ctx->dali_addr,
            .is_cmd = true,
            .command = DALI_CMD_OFF,
            .send_twice = false,
            .tx_timeout_ms = DALI_TX_TIMEOUT_MS,
        };
        err = dali_master_do_transaction(s_dali_handle, &tx_cfg, NULL);
    }

    if (err == ESP_OK) {
        ctx->power = on;
        ESP_LOGI(TAG, "DALI addr=%u power -> %s", ctx->dali_addr, on ? "ON" : "OFF");
    } else {
        ESP_LOGE(TAG, "DALI addr=%u set power failed: %s", ctx->dali_addr, esp_err_to_name(err));
    }
    return err;
}

static esp_err_t dali_ballast_set_level(dali_light_ctx_t *ctx, uint8_t matter_level)
{
    uint8_t dali_level = matter_level_to_dali(matter_level);
#if DALI_MOCK_NO_HW
    ctx->dali_level = dali_level;
    if (dali_level > 0) {
        ctx->power = true;
    }
    ESP_LOGI(TAG, "[MOCK] addr=%u level: matter=%u -> dali=%u",
             ctx->dali_addr, matter_level, dali_level);
    return ESP_OK;
#endif
    ESP_LOGI(TAG, "DALI addr=%u level: matter=%u -> dali=%u",
             ctx->dali_addr, matter_level, dali_level);

    dali_master_transaction_config_t tx_cfg = {
        .addr_type = DALI_ADDR_SHORT,
        .addr = ctx->dali_addr,
        .is_cmd = false, /* DAPC */
        .command = dali_level,
        .send_twice = false,
        .tx_timeout_ms = DALI_TX_TIMEOUT_MS,
    };
    esp_err_t err = dali_master_do_transaction(s_dali_handle, &tx_cfg, NULL);
    if (err == ESP_OK) {
        ctx->dali_level = dali_level;
        if (dali_level > 0) {
            ctx->power = true;
        }
    } else {
        ESP_LOGE(TAG, "DALI addr=%u set level failed: %s", ctx->dali_addr, esp_err_to_name(err));
    }
    return err;
}

/* -----------------------------------------------------------------------
 * DT8 (Part 209) color control — real DALI hardware implementation
 *
 * DA5-L C5 mode (RGB + CCT):
 *   - CT  → DALI_COLOR_CCT  (WW/W outputs via SET_COLOR_TEMPERATURE)
 *   - XY  → converted to RGB  via CIE XY → sRGB, sent as DALI_COLOR_RGB
 *   - HS  → converted to RGB  via Hue/Sat → RGB, sent as DALI_COLOR_RGB
 * ----------------------------------------------------------------------- */

/** Clamp a Mirek value to the physical DT8 driver limits. */
static inline uint16_t clamp_mirek(uint16_t mirek)
{
    if (mirek < COLOR_TEMP_MIN_MIREK) {
        return COLOR_TEMP_MIN_MIREK;
    }
    if (mirek > COLOR_TEMP_MAX_MIREK) {
        return COLOR_TEMP_MAX_MIREK;
    }
    return mirek;
}

/**
 * Convert Matter Hue (0–254) + Saturation (0–254) to RGB (0–254).
 * Uses the standard HSV-to-RGB algorithm with V=1 (full brightness).
 * The DAPC brightness level is controlled separately via LevelControl.
 */
static void hs_to_rgb(uint8_t hue8, uint8_t sat8,
                      uint8_t *r_out, uint8_t *g_out, uint8_t *b_out)
{
    /* Normalise to [0.0, 1.0] */
    float h = (float)hue8 / 254.0f * 360.0f; /* degrees */
    float s = (float)sat8 / 254.0f;
    float v = 1.0f;

    float c = v * s;
    float x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;
    float r1 = 0, g1 = 0, b1 = 0;

    if (h < 60) {
        r1 = c;
        g1 = x;
        b1 = 0;
    } else if (h < 120) {
        r1 = x;
        g1 = c;
        b1 = 0;
    } else if (h < 180) {
        r1 = 0;
        g1 = c;
        b1 = x;
    } else if (h < 240) {
        r1 = 0;
        g1 = x;
        b1 = c;
    } else if (h < 300) {
        r1 = x;
        g1 = 0;
        b1 = c;
    } else {
        r1 = c;
        g1 = 0;
        b1 = x;
    }

    auto to_dali = [m](float f) -> uint8_t {
        int v = (int)((f + m) * 254.0f + 0.5f);
        return (uint8_t)(v < 0 ? 0 : v > 254 ? 254 : v);
    };
    *r_out = to_dali(r1);
    *g_out = to_dali(g1);
    *b_out = to_dali(b1);
}

/**
 * Convert CIE 1931 XY (Matter format: 0x0000–0xFEFF maps to 0.0–1.0)
 * to sRGB then to DALI DAPC range (0–254).
 * Uses the D65-adapted sRGB matrix (IEC 61966-2-1).
 */
static void xy_to_rgb(uint16_t cx, uint16_t cy,
                      uint8_t *r_out, uint8_t *g_out, uint8_t *b_out)
{
    /* Matter encodes as uint16 where 0xFEFF = 1.0 */
    float x = (float)cx / 65279.0f;
    float y = (float)cy / 65279.0f;
    if (y < 1e-6f) {
        *r_out = *g_out = *b_out = 0;
        return;
    }

    /* XY to XYZ (Y=1) */
    float Y = 1.0f;
    float X = (Y / y) * x;
    float Z = (Y / y) * (1.0f - x - y);

    /* D65 sRGB matrix */
    float r = 3.2406f * X - 1.5372f * Y - 0.4986f * Z;
    float g = -0.9689f * X + 1.8758f * Y + 0.0415f * Z;
    float b = 0.0557f * X - 0.2040f * Y + 1.0570f * Z;

    /* Clamp negatives, normalise to max channel */
    if (r < 0) {
        r = 0;
    }
    if (g < 0) {
        g = 0;
    }
    if (b < 0) {
        b = 0;
    }
    float mx = r > g ? (r > b ? r : b) : (g > b ? g : b);
    if (mx < 1e-6f) {
        *r_out = *g_out = *b_out = 0;
        return;
    }
    r /= mx; g /= mx; b /= mx;

    auto to_dali = [](float f) -> uint8_t {
        int v = (int)(f * 254.0f + 0.5f);
        return (uint8_t)(v < 0 ? 0 : v > 254 ? 254 : v);
    };
    *r_out = to_dali(r);
    *g_out = to_dali(g);
    *b_out = to_dali(b);
}

/* RGB Color Settings */
static esp_err_t flush_rgb(dali_light_ctx_t *ctx)
{
#if DALI_MOCK_NO_HW
    ESP_LOGI(TAG, "[MOCK] addr=%u RGB R=%u G=%u B=%u",
             ctx->dali_addr, ctx->color_r, ctx->color_g, ctx->color_b);
    return ESP_OK;
#endif
    if (((ctx->color_caps >> 5) & 0x07) == 0) {
        ESP_LOGW(TAG, "[DT8] addr=%u RGB R=%u G=%u B=%u not supported (features=0x%02X)",
                 ctx->dali_addr, ctx->color_r, ctx->color_g, ctx->color_b, ctx->color_caps);
        return ESP_ERR_NOT_SUPPORTED;
    }

    dali_color_val_t val = {};
    val.rgb.r = ctx->color_r;
    val.rgb.g = ctx->color_g;
    val.rgb.b = ctx->color_b;
    esp_err_t err = dali_master_set_color(s_dali_handle,
                                          DALI_ADDR_SHORT, ctx->dali_addr,
                                          DALI_COLOR_RGB, val,
                                          DALI_TX_TIMEOUT_MS);
    ESP_LOGI(TAG, "[DT8] addr=%u RGB R=%u G=%u B=%u (SET_TEMPORARY_RGB_DIMLEVEL) → %s",
             ctx->dali_addr, val.rgb.r, val.rgb.g, val.rgb.b,
             esp_err_to_name(err));
    return err;
}

/**
 * Handle a ColorControl attribute change and issue the real DALI Part 209 command.
 */
static esp_err_t app_driver_color_update(dali_light_ctx_t *ctx,
                                         uint32_t attribute_id,
                                         esp_matter_attr_val_t *val)
{
    using namespace chip::app::Clusters::ColorControl::Attributes;

    switch (attribute_id) {

    /* ---- Color Temperature (CT mode) — map directly to DALI CCT ------ */
    case ColorTemperatureMireds::Id: {
        if ((ctx->color_caps & (1U << 1)) == 0) {
            return ESP_OK;
        }
        uint16_t mirek = clamp_mirek(val->val.u16);

        dali_color_val_t dv = {};
        dv.cct.mirek = mirek;
        esp_err_t err = dali_master_set_color(s_dali_handle,
                                              DALI_ADDR_SHORT, ctx->dali_addr,
                                              DALI_COLOR_CCT, dv,
                                              DALI_TX_TIMEOUT_MS);
        ESP_LOGI(TAG, "[DT8] addr=%u CT %u Mirek (~%u K) → %s",
                 ctx->dali_addr, mirek,
                 (mirek > 0) ? (1000000U / mirek) : 0,
                 esp_err_to_name(err));
        if (err == ESP_OK) {
            ctx->color_temp_mirek = mirek;
            ctx->color_mode = DALI_COLOR_CCT;
        }
        return err;
    }

    /* ---- XY chromaticity — buffer X; convert+send when Y arrives ------- */
    case CurrentX::Id:
        ctx->color_x = val->val.u16;
        /* Wait for Y before sending — Y callback follows immediately */
        return ESP_OK;

    case CurrentY::Id: {
        xy_to_rgb(ctx->color_x, val->val.u16,
                  &ctx->color_r, &ctx->color_g, &ctx->color_b);
        if (flush_rgb(ctx) == ESP_OK) {
            ctx->color_y = val->val.u16;
            ctx->color_mode = DALI_COLOR_RGB;
            return ESP_OK;
        }
        xy_to_rgb(ctx->color_x, ctx->color_y,
                  &ctx->color_r, &ctx->color_g, &ctx->color_b);
        return ESP_FAIL;
    }

    /* ---- Hue / Saturation — either update sends RGB from the cached pair ---- */
    case CurrentHue::Id:
        hs_to_rgb(val->val.u8, ctx->color_saturation,
                  &ctx->color_r, &ctx->color_g, &ctx->color_b);
        if (flush_rgb(ctx) == ESP_OK) {
            ctx->color_hue = val->val.u8;
            ctx->color_mode = DALI_COLOR_RGB;
            return ESP_OK;
        }
        hs_to_rgb(ctx->color_hue, ctx->color_saturation,
                  &ctx->color_r, &ctx->color_g, &ctx->color_b);
        return ESP_FAIL;

    case CurrentSaturation::Id: {
        hs_to_rgb(ctx->color_hue, val->val.u8,
                  &ctx->color_r, &ctx->color_g, &ctx->color_b);
        if (flush_rgb(ctx) == ESP_OK) {
            ctx->color_saturation = val->val.u8;
            ctx->color_mode = DALI_COLOR_RGB;
            return ESP_OK;
        }
        hs_to_rgb(ctx->color_hue, ctx->color_saturation,
                  &ctx->color_r, &ctx->color_g, &ctx->color_b);
        return ESP_FAIL;
    }

    /* Re-apply the cached color for the mode that just became active. */
    case ColorMode::Id:
        ESP_LOGD(TAG, "[DT8] addr=%u ColorMode -> %u (0=HS 1=XY 2=CT)",
                 ctx->dali_addr, val->val.u8);
        if (val->val.u8 == 2 && (ctx->color_caps & (1U << 1)) != 0) {
            if (dali_master_set_color(s_dali_handle,
                                      DALI_ADDR_SHORT, ctx->dali_addr,
                                      DALI_COLOR_CCT,
                                      dali_color_val_t{.cct = {.mirek = clamp_mirek(ctx->color_temp_mirek)}},
                                      DALI_TX_TIMEOUT_MS) == ESP_OK) {
                ctx->color_mode = DALI_COLOR_CCT;
                return ESP_OK;
            }
            return ESP_FAIL;
        }
        if (val->val.u8 == 1) {
            xy_to_rgb(ctx->color_x, ctx->color_y,
                      &ctx->color_r, &ctx->color_g, &ctx->color_b);
            if (flush_rgb(ctx) == ESP_OK) {
                ctx->color_mode = DALI_COLOR_RGB;
                return ESP_OK;
            }
            return ESP_FAIL;
        }
        if (val->val.u8 == 0) {
            hs_to_rgb(ctx->color_hue, ctx->color_saturation,
                      &ctx->color_r, &ctx->color_g, &ctx->color_b);
            if (flush_rgb(ctx) == ESP_OK) {
                ctx->color_mode = DALI_COLOR_RGB;
                return ESP_OK;
            }
            return ESP_FAIL;
        }
        break;

    default:
        break;
    }

    return ESP_OK;
}

/* -----------------------------------------------------------------------
 * Public API
 * ----------------------------------------------------------------------- */

esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle,
                                      uint16_t endpoint_id, uint32_t cluster_id,
                                      uint32_t attribute_id, esp_matter_attr_val_t *val)
{
    /* Endpoint priv_data is the bridged device. Runtime ballast state is its priv_data. */
    app_bridged_device_t *bridged_device = app_bridge_get_device(endpoint_id);
    dali_light_ctx_t *ctx = bridged_device ? (dali_light_ctx_t *)bridged_device->get_priv_data() : nullptr;
    if (!ctx) {
        /* Not a DALI light endpoint (for example the root/aggregator endpoint) */
        return ESP_OK;
    }

    if (!ctx->reachable) {
#if !DALI_MOCK_NO_HW
        /* Matter still stores the write. ctx is left unchanged so the next
         * reachable transition can replay anything that diverged. */
        return ESP_OK;
#else
        /* Mock mode: treat all virtual ballasts as reachable */
#endif
    }

    esp_err_t err = ESP_OK;

    if (cluster_id == OnOff::Id) {
        if (attribute_id == OnOff::Attributes::OnOff::Id) {
            err = dali_ballast_set_power(ctx, val->val.b);
        }
    } else if (cluster_id == LevelControl::Id) {
        if (attribute_id == LevelControl::Attributes::CurrentLevel::Id) {
            if (ctx->power) {
                err = dali_ballast_set_level(ctx, val->val.u8);
            } else {
                /* Light is off — store level, apply on next ON */
                ctx->dali_level = matter_level_to_dali(val->val.u8);
                ESP_LOGI(TAG, "DALI addr=%u off — level stored as dali=%u",
                         ctx->dali_addr, ctx->dali_level);
            }
        }
    } else if (cluster_id == ColorControl::Id) {
        err = app_driver_color_update(ctx, attribute_id, val);
    }

    return err;
}

esp_err_t app_driver_light_set_defaults(uint16_t endpoint_id, dali_light_ctx_t *ctx)
{
    esp_err_t err = ESP_OK;
    esp_matter_attr_val_t val = esp_matter_invalid(NULL);

    /* Read persisted brightness */
    attribute_t *level_attr = attribute::get(endpoint_id,
                                             LevelControl::Id,
                                             LevelControl::Attributes::CurrentLevel::Id);
    if (level_attr) {
        attribute::get_val(level_attr, &val);
        ctx->dali_level = matter_level_to_dali(val.val.u8);
    }

    /* Read persisted power state and apply to this ballast */
    attribute_t *onoff_attr = attribute::get(endpoint_id, OnOff::Id, OnOff::Attributes::OnOff::Id);
    if (onoff_attr) {
        attribute::get_val(onoff_attr, &val);
        err = dali_ballast_set_power(ctx, val.val.b);
    }

    return err;
}

app_driver_handle_t app_driver_dali_init(void)
{
#if DALI_MOCK_NO_HW
    ESP_LOGW(TAG, "[MOCK] DALI driver NOT initialised (no-hardware mode)");
    return (app_driver_handle_t)(uintptr_t)1;
#endif
    dali_master_config_t cfg = {
        .rx_gpio = (gpio_num_t)CONFIG_DALI_RX_GPIO,
        .tx_gpio = (gpio_num_t)CONFIG_DALI_TX_GPIO,
        .invert_tx = CONFIG_DALI_TX_INVERT,
        .invert_rx = CONFIG_DALI_RX_INVERT,
    };

    esp_err_t err = dali_new_master_rmt(&cfg, NULL, &s_dali_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "dali_new_master_rmt failed: %s", esp_err_to_name(err));
        return NULL;
    }
    ESP_LOGI(TAG, "DALI driver initialised (RX=%d TX=%d invert_tx=%d invert_rx=%d)",
             CONFIG_DALI_RX_GPIO, CONFIG_DALI_TX_GPIO,
             CONFIG_DALI_TX_INVERT, CONFIG_DALI_RX_INVERT);

    return (app_driver_handle_t)(uintptr_t)1;
}

esp_err_t app_driver_dali_scan_with_type(uint64_t *present_mask,
                                         dali_device_type_t dev_type_out[64],
                                         uint8_t color_caps_out[64])
{
    if (!present_mask || !dev_type_out) {
        return ESP_ERR_INVALID_ARG;
    }

#if DALI_MOCK_NO_HW
    /* ---- No-hardware mock: inject two virtual ballasts ---- */
    *present_mask = 0ULL;
    for (int i = 0; i < 64; i++) {
        dev_type_out[i] = DALI_DEV_TYPE_UNKNOWN;
        if (color_caps_out) {
            color_caps_out[i] = 0;
        }
    }
    /* addr=0 → DT6 dimmable light (single-channel) */
    *present_mask |= (1ULL << 0);
    dev_type_out[0] = DALI_DEV_TYPE_DT6;
    /* addr=1 → DT8 RGB + tunable white (3 RGBWAF channels and Tc) */
    *present_mask |= (1ULL << 1);
    dev_type_out[1] = DALI_DEV_TYPE_DT8;
    if (color_caps_out) {
        color_caps_out[1] = (uint8_t)((1U << 1) | (3U << 5)); /* Tc + 3 RGBWAF channels */
    }

    ESP_LOGW(TAG, "[MOCK] No-hardware mode: injecting addr=0(DT6) addr=1(DT8)");
    return ESP_OK;
#endif /* DALI_MOCK_NO_HW */

    uint8_t found[64];
    size_t found_count = 0;
    *present_mask = 0ULL;
    for (uint8_t addr = 0; addr < 64; addr++) {
        dev_type_out[addr] = DALI_DEV_TYPE_UNKNOWN;
        if (color_caps_out) {
            color_caps_out[addr] = 0;
        }
    }

    ESP_LOGI(TAG, "Scanning DALI short addresses 0–63 ...");
    for (uint8_t addr = 0; addr < 64; addr++) {
        int reply = DALI_RESULT_NO_REPLY;
        dali_master_transaction_config_t tx_cfg = {
            .addr_type = DALI_ADDR_SHORT,
            .addr = addr,
            .is_cmd = true,
            .command = DALI_CMD_QUERY_CONTROL_GEAR,
            .send_twice = false,
            .tx_timeout_ms = DALI_TX_TIMEOUT_MS,
        };
        esp_err_t err = dali_master_do_transaction(s_dali_handle, &tx_cfg, &reply);
        if (err == ESP_OK && reply == 0xFF) {
            *present_mask |= (1ULL << addr);
            found[found_count++] = addr;
        }
    }

    ESP_LOGI(TAG, "======== DALI Address Scan Report ========");
    ESP_LOGI(TAG, "  Devices found: %u", (unsigned)found_count);
    for (size_t i = 0; i < found_count; i++) {
        ESP_LOGI(TAG, "    [%2u] short address %u (0x%02X)",
                 (unsigned)(i + 1), found[i], found[i]);
    }
    ESP_LOGI(TAG, "==========================================");

    for (size_t i = 0; i < found_count; i++) {
        uint8_t addr = found[i];

        /* IEC 62386-209 requires ENABLE_DEVICE_TYPE 8 immediately before
         * its application-extended commands. */
        (void)dali_enable_device_type(s_dali_handle, 8, DALI_TX_TIMEOUT_MS);

        int reply = DALI_RESULT_NO_REPLY;
        dali_master_transaction_config_t tx_cfg = {
            .addr_type = DALI_ADDR_SHORT,
            .addr = addr,
            .is_cmd = true,
            .command = DALI_209_QUERY_COLOR_CAPABILITIES,
            .send_twice = false,
            .tx_timeout_ms = DALI_TX_TIMEOUT_MS,
        };
        esp_err_t err = dali_master_do_transaction(s_dali_handle, &tx_cfg, &reply);
        if (err == ESP_OK && DALI_RESULT_IS_VALID(reply)) {
            dev_type_out[addr] = DALI_DEV_TYPE_DT8;
            if (color_caps_out) {
                color_caps_out[addr] = (uint8_t)reply;
            }
            ESP_LOGI(TAG, " addr=%u DT8 features=0x%02X"
                     " (XY:%c Tc:%c PrimaryN:%u RGBWAF:%u)",
                     addr, (uint8_t)reply,
                     (reply & (1 << 0)) ? 'Y' : 'N',
                     (reply & (1 << 1)) ? 'Y' : 'N',
                     (unsigned)((reply >> 2) & 0x07),
                     (unsigned)((reply >> 5) & 0x07));
        } else {
            reply = DALI_RESULT_NO_REPLY;
            tx_cfg.command = DALI_CMD_QUERY_DEVICE_TYPE;
            err = dali_master_do_transaction(s_dali_handle, &tx_cfg, &reply);
            if (err == ESP_OK && DALI_RESULT_IS_VALID(reply)) {
                if ((uint8_t)reply == 6) {
                    dev_type_out[addr] = DALI_DEV_TYPE_DT6;
                } else if ((uint8_t)reply == 8) {
                    dev_type_out[addr] = DALI_DEV_TYPE_DT8;
                }
            }
        }

        ESP_LOGI(TAG, "  addr=%u device_type=%u (%s)", addr,
                 (uint8_t)dev_type_out[addr],
                 dev_type_out[addr] == DALI_DEV_TYPE_DT8 ? "DT8 color" :
                 dev_type_out[addr] == DALI_DEV_TYPE_DT6 ? "DT6 dimmer" :
                 "unknown→DT6");
    }

    ESP_LOGI(TAG, "DALI discovery complete: %u ballast(s) found", (unsigned)found_count);
    return ESP_OK;
}

/**
 * Push the cached reachability state to the bridged endpoint's
 * BridgedDeviceBasicInformation::Reachable attribute.
 */
static void update_reachable_attribute(dali_light_ctx_t *ctx, bool reachable)
{
    esp_matter_attr_val_t val = esp_matter_bool(reachable);
    esp_err_t err = attribute::update(ctx->endpoint_id,
                                      chip::app::Clusters::BridgedDeviceBasicInformation::Id,
                                      chip::app::Clusters::BridgedDeviceBasicInformation::Attributes::Reachable::Id,
                                      &val);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "DALI addr=%u failed to update Reachable=%s: %s",
                 ctx->dali_addr, reachable ? "true" : "false", esp_err_to_name(err));
    }
}

void app_driver_dali_poll_reachability(dali_light_ctx_t **ctx_array, size_t count)
{
#if DALI_MOCK_NO_HW
    /* In mock mode just ensure all virtual ballasts stay reachable */
    for (size_t i = 0; i < count; i++) {
        dali_light_ctx_t *ctx = ctx_array[i];
        if (!ctx || ctx->reachable) {
            continue;
        }
        ctx->reachable = true;
        ESP_LOGI(TAG, "[MOCK] addr=%u marked reachable", ctx->dali_addr);
        update_reachable_attribute(ctx, true);
    }
    return;
#endif
    /* Reassert permanent Part 103 quiescent mode before polling. Do not send
     * STOP_QUIESCENT_MODE in this application: DLS-203-P event frames can be
     * interpreted by Part 102 gear as DAPC brightness commands, causing the
     * observed fixed-level overwrite. */
    /* Chip stack lock is already held by the Matter write path. Taking it
     * here keeps the poll task off the bus until that write finishes. */
    chip::DeviceLayer::PlatformMgr().LockChipStack();
    dali_103_do_device_command(s_dali_handle, DALI_ADDR_BROADCAST, 0,
                               DALI_103_START_QUIESCENT_MODE,
                               true, DALI_TX_TIMEOUT_MS, NULL);
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    vTaskDelay(pdMS_TO_TICKS(10));

    for (size_t i = 0; i < count; i++) {
        dali_light_ctx_t *ctx = ctx_array[i];
        if (!ctx) {
            continue;
        }

        int reply = DALI_RESULT_NO_REPLY;
        dali_master_transaction_config_t tx_cfg = {
            .addr_type = DALI_ADDR_SHORT,
            .addr = ctx->dali_addr,
            .is_cmd = true,
            .command = DALI_CMD_QUERY_STATUS,
            .send_twice = false,
            .tx_timeout_ms = DALI_TX_TIMEOUT_MS,
        };
        chip::DeviceLayer::PlatformMgr().LockChipStack();
        esp_err_t err = dali_master_do_transaction(s_dali_handle, &tx_cfg, &reply);
        chip::DeviceLayer::PlatformMgr().UnlockChipStack();

        bool gear_present = (err == ESP_OK && DALI_RESULT_IS_VALID(reply));
        bool lamp_ok = gear_present && ((reply & DALI_STATUS_LAMP_FAILURE_BIT) == 0);
        bool raw_reachable = gear_present && lamp_ok;

        /* Apply hysteresis:
         *   false → true  : increment counter; only commit after REACH_CONFIRM_NEEDED passes
         *   true  → false : commit immediately; reset counter */
        bool new_reachable = ctx->reachable;
        if (raw_reachable) {
            if (!ctx->reachable) {
                ctx->reach_confirm_cnt++;
                if (ctx->reach_confirm_cnt >= REACH_CONFIRM_NEEDED) {
                    new_reachable = true;
                    ctx->reach_confirm_cnt = 0;
                }
            } else {
                ctx->reach_confirm_cnt = 0;
            }
        } else {
            new_reachable = false;
            ctx->reach_confirm_cnt = 0;
        }

        /* Retry an incomplete first-contact restore while the gear remains reachable. */
        if (new_reachable == ctx->reachable && (!new_reachable || ctx->ever_reached)) {
            continue;
        }

        if (new_reachable != ctx->reachable) {
            ESP_LOGI(TAG, "DALI addr=%u reachable -> %s (status=0x%02X gear=%s lamp=%s)",
                     ctx->dali_addr,
                     new_reachable ? "true" : "false (No Response)",
                     gear_present ? (uint8_t)reply : 0xFF,
                     gear_present ? "present" : "absent",
                     !gear_present ? "N/A" : (lamp_ok ? "ok" : "FAIL/missing"));
        }

        if (!new_reachable) {
            /* Gear removed from the bus: report the bridged device as unreachable.
             * Keep ctx->power so a missed poll does not swallow later level writes. */
            ctx->reachable = false;
            update_reachable_attribute(ctx, false);
        } else {
            chip::DeviceLayer::PlatformMgr().LockChipStack();
            ctx->reachable = true;
            esp_matter_attr_val_t val = esp_matter_invalid(NULL);
            if (!ctx->ever_reached) {
                /* First confirmed contact since boot — push persisted NVS state
                 * to the physical DALI hardware (gear may have power-cycled). */
                ctx->ever_reached = app_driver_light_set_defaults(ctx->endpoint_id, ctx) == ESP_OK;
                ESP_LOGI(TAG, "DALI addr=%u first-contact restore: power=%s level=%u status=%s",
                         ctx->dali_addr, ctx->power ? "ON" : "OFF", ctx->dali_level,
                         ctx->ever_reached ? "complete" : "pending");
            } else {
                /* Replay only when a write landed while the gear was unreachable.
                 * An unchanged Matter snapshot means the gear never lost the level
                 * the user set, so leave the bus alone. */
                attribute_t *onoff_attr = attribute::get(ctx->endpoint_id, OnOff::Id,
                                                         OnOff::Attributes::OnOff::Id);
                attribute_t *level_attr = attribute::get(ctx->endpoint_id, LevelControl::Id,
                                                         LevelControl::Attributes::CurrentLevel::Id);
                bool replay = false;
                if (onoff_attr) {
                    attribute::get_val(onoff_attr, &val);
                    if (val.val.b != ctx->power) {
                        replay = true;
                    }
                }
                if (level_attr) {
                    attribute::get_val(level_attr, &val);
                    if (matter_level_to_dali(val.val.u8) != ctx->dali_level) {
                        replay = true;
                    }
                }
                if (replay) {
                    app_driver_light_set_defaults(ctx->endpoint_id, ctx);
                    ESP_LOGI(TAG, "DALI addr=%u replay Matter state: power=%s level=%u",
                             ctx->dali_addr, ctx->power ? "ON" : "OFF", ctx->dali_level);
                } else {
                    ESP_LOGI(TAG, "DALI addr=%u transient recovery (no DALI cmd sent): power=%s level=%u",
                             ctx->dali_addr, ctx->power ? "ON" : "OFF", ctx->dali_level);
                }
            }
            if (ctx->color_caps != 0) {
                attribute_t *ct_attr = attribute::get(ctx->endpoint_id, ColorControl::Id,
                                                      ColorControl::Attributes::ColorMode::Id);
                if (ct_attr && attribute::get_val(ct_attr, &val) == ESP_OK) {
                    if (val.val.u8 == 2 && (ctx->color_caps & (1U << 1))) {
                        ct_attr = attribute::get(ctx->endpoint_id, ColorControl::Id,
                                                 ColorControl::Attributes::ColorTemperatureMireds::Id);
                        if (ct_attr && attribute::get_val(ct_attr, &val) == ESP_OK) {
                            ctx->ever_reached =
                                (app_driver_color_update(ctx, ColorControl::Attributes::ColorTemperatureMireds::Id,
                                                         &val) == ESP_OK) && ctx->ever_reached;
                        }
                    } else if (val.val.u8 == 1) {
                        attribute_t *x_attr = attribute::get(ctx->endpoint_id, ColorControl::Id,
                                                             ColorControl::Attributes::CurrentX::Id);
                        attribute_t *y_attr = attribute::get(ctx->endpoint_id, ColorControl::Id,
                                                             ColorControl::Attributes::CurrentY::Id);
                        if (x_attr && y_attr) {
                            esp_matter_attr_val_t yval = esp_matter_invalid(NULL);
                            if (attribute::get_val(x_attr, &val) == ESP_OK &&
                                    attribute::get_val(y_attr, &yval) == ESP_OK) {
                                xy_to_rgb(val.val.u16, yval.val.u16,
                                          &ctx->color_r, &ctx->color_g, &ctx->color_b);
                                if (flush_rgb(ctx) == ESP_OK) {
                                    ctx->color_x = val.val.u16;
                                    ctx->color_y = yval.val.u16;
                                    ctx->color_mode = DALI_COLOR_RGB;
                                } else {
                                    ctx->ever_reached = false;
                                }
                            } else {
                                ctx->ever_reached = false;
                            }
                        }
                    } else if (val.val.u8 == 0) {
                        attribute_t *hue_attr = attribute::get(ctx->endpoint_id, ColorControl::Id,
                                                               ColorControl::Attributes::CurrentHue::Id);
                        attribute_t *sat_attr = attribute::get(ctx->endpoint_id, ColorControl::Id,
                                                               ColorControl::Attributes::CurrentSaturation::Id);
                        if (hue_attr && sat_attr) {
                            esp_matter_attr_val_t satval = esp_matter_invalid(NULL);
                            if (attribute::get_val(hue_attr, &val) == ESP_OK &&
                                    attribute::get_val(sat_attr, &satval) == ESP_OK) {
                                hs_to_rgb(val.val.u8, satval.val.u8,
                                          &ctx->color_r, &ctx->color_g, &ctx->color_b);
                                if (flush_rgb(ctx) == ESP_OK) {
                                    ctx->color_hue = val.val.u8;
                                    ctx->color_saturation = satval.val.u8;
                                    ctx->color_mode = DALI_COLOR_RGB;
                                } else {
                                    ctx->ever_reached = false;
                                }
                            } else {
                                ctx->ever_reached = false;
                            }
                        }
                    }
                }
            }
            if (!ctx->ever_reached) {
                ESP_LOGW(TAG, "DALI addr=%u restore incomplete; retrying on next poll", ctx->dali_addr);
            }
            chip::DeviceLayer::PlatformMgr().UnlockChipStack();
            update_reachable_attribute(ctx, true);
        }
    }
}

/* -----------------------------------------------------------------------
 * Push the current DALI reachability to bridged Matter light endpoints
 * ----------------------------------------------------------------------- */
void app_driver_dali_push_all_states(dali_light_ctx_t **ctx_array, size_t count)
{
    ESP_LOGI(TAG, "Pushing reachability state for all %u ballast slots", (unsigned)count);
    for (size_t i = 0; i < count; i++) {
        dali_light_ctx_t *ctx = ctx_array[i];
        if (!ctx) {
            continue;
        }

        update_reachable_attribute(ctx, ctx->reachable);
        ESP_LOGI(TAG, "DALI addr=%u -> Reachable=%s", ctx->dali_addr,
                 ctx->reachable ? "true" : "false");
    }
}
