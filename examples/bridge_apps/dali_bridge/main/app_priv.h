/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <esp_err.h>
#include <esp_matter.h>
#include <stdint.h>
#include <dali.h>

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#include "esp_openthread_types.h"
#endif

#define DALI_MOCK_NO_HW 0 /* 1 = no-hardware mock mode, 0 = real hardware */

/* -----------------------------------------------------------------------
 * Default attribute values
 * ----------------------------------------------------------------------- */
#define DEFAULT_POWER true
#define DEFAULT_BRIGHTNESS 64
#define DEFAULT_HUE 0 /**< Default hue (0 = red) */
#define DEFAULT_SATURATION 0 /**< Default saturation (0 = no color) */

/* Color temperature defaults and hardware limits (Mirek = 1 000 000 / Kelvin).
 *   153 Mirek ≈ 6500 K (cool white)
 *   370 Mirek ≈ 2700 K (warm white)
 * Adjust COLOR_TEMP_MIN/MAX_MIREK to match the DT8 driver's Tc_coolest / Tc_warmest. */
#define DEFAULT_COLOR_TEMP_MIREK 250U /**< Default: ~4000 K */
#define COLOR_TEMP_MIN_MIREK 153U /**< Coolest: 6500 K (0x0099) */
#define COLOR_TEMP_MAX_MIREK 370U /**< Warmest: 2700 K (0x0172) */

/* -----------------------------------------------------------------------
 * Per-ballast runtime context stored as priv_data on app_dali_bridged_device_t.
 * The DALI short address and colour capabilities are persisted separately as
 * the bridged-device address.
 *
 * The Bridge maps:
 *   Matter bridged endpoint  <-->  DALI short address (0–63)
 *
 * Each ballast keeps its own on/off state and last DAPC level so that
 * "turn off then turn on" restores the previous brightness.
 * ----------------------------------------------------------------------- */
/**
 * DALI application device type (IEC 62386 Part 2xx).
 *   DALI_DEV_TYPE_UNKNOWN : replied to QUERY CONTROL GEAR but device type
 *                           query returned no answer → treat as DT6 (dimmer).
 *   DALI_DEV_TYPE_DT6     : fluorescent / LED dimmer (IEC 62386-206)
 *   DALI_DEV_TYPE_DT8     : color control gear (IEC 62386-209)
 */
typedef enum {
    DALI_DEV_TYPE_UNKNOWN = 0xFF, /**< No device-type reply, default to DT6 behaviour */
    DALI_DEV_TYPE_DT6 = 6,
    DALI_DEV_TYPE_DT8 = 8,
} dali_device_type_t;

typedef struct dali_light_ctx {
    uint8_t dali_addr; /**< DALI short address (0–63) */
    bool power; /**< Current on/off state */
    uint8_t dali_level; /**< Last DAPC value sent (0–254) */
    bool reachable; /**< Last known reachability state */
    uint16_t endpoint_id; /**< Matter endpoint ID (cached) */
    uint8_t reach_confirm_cnt; /**< Consecutive "reachable" poll count for hysteresis */
    /** True once the gear has been confirmed reachable at least once since boot.
     *  First contact always restores Matter state to the gear. A later recovery
     *  resends only when Matter attributes differ from this context. */
    bool ever_reached;
    dali_device_type_t dev_type; /**< DT6 / DT8 / UNKNOWN (set at scan time) */
    /** IEC 62386-209 QUERY COLOUR TYPE FEATURES (0xF9).
     *  bit0=xy bit1=Tc bit2-4=Primary N count
     *  bit5-7=RGBWAF channel count.
     *  0 for DT6/UNKNOWN devices. */
    uint8_t color_caps;

    /* ---------------------------------------------------------------
     * DT8 (Part 209) color state — populated by ColorControl cluster
     * ------------------------------------------------------------- */
    dali_color_mode_t color_mode; /**< Active color mode (CT / XY / HS) */
    uint16_t color_temp_mirek; /**< Last color temperature in Mirek (CT mode) */
    uint16_t color_x; /**< Last CIE1931 X (0x0000–0xFEFF, XY mode) */
    uint16_t color_y; /**< Last CIE1931 Y (0x0000–0xFEFF, XY mode) */
    uint8_t color_hue; /**< Last hue (0–254, HS mode) */
    uint8_t color_saturation; /**< Last saturation (0–254, HS mode) */
    /* RGBWAF channels (DALI 209 §8.x, used when DT8 driver supports RGBWAF) */
    uint8_t color_r; /**< Red channel DAPC (0x00–0xFE) */
    uint8_t color_g; /**< Green channel DAPC (0x00–0xFE) */
    uint8_t color_b; /**< Blue channel DAPC (0x00–0xFE) */
} dali_light_ctx_t;

typedef void *app_driver_handle_t;

/* -----------------------------------------------------------------------
 * Public API
 * ----------------------------------------------------------------------- */

/**
 * @brief Initialise the DALI RMT driver (singleton).
 *
 * @return Non-NULL handle on success, NULL on failure.
 */
app_driver_handle_t app_driver_dali_init(void);

/**
 * @brief Extended scan: detect present ballasts AND query each one's Device Type.
 *
 * For every address where QUERY CONTROL GEAR returns YES, probes for DT8 via
 * dali_enable_device_type(8) + QUERY_COLOR_CAPABILITIES first, then falls back to
 * QUERY_DEVICE_TYPE (Part-102) for DT6 and other types.
 *
 * @param[out] present_mask     64-bit bitmask of responding short addresses.
 * @param[out] dev_type_out     Array of 64 dali_device_type_t values indexed by
 *                              short address.  Only entries whose bit is set in
 *                              present_mask are meaningful.
 * @param[out] color_caps_out  Array of 64 uint8_t.  For DT8 devices this holds
 *                              the QUERY COLOUR TYPE FEATURES byte (bit0=xy,
 *                              bit1=Tc, bit2-4=Primary N count, bit5-7=RGBWAF
 *                              channel count).  For non-DT8 devices the entry
 *                              is 0.  May be NULL.
 * @return ESP_OK on success.
 */
esp_err_t app_driver_dali_scan_with_type(uint64_t *present_mask,
                                         dali_device_type_t dev_type_out[64],
                                         uint8_t color_caps_out[64]);

/**
 * @brief Attribute update callback forwarded from app_main.
 *
 * @param driver_handle  priv_data cast to app_driver_handle_t (unused for DALI).
 * @param endpoint_id    Matter endpoint that changed.
 * @param cluster_id     Cluster that changed.
 * @param attribute_id   Attribute that changed.
 * @param val            New value.
 * @return ESP_OK on success.
 */
esp_err_t app_driver_attribute_update(app_driver_handle_t driver_handle,
                                      uint16_t endpoint_id,
                                      uint32_t cluster_id,
                                      uint32_t attribute_id,
                                      esp_matter_attr_val_t *val);

/**
 * @brief Apply NVS-persisted defaults to a bridged light endpoint.
 */
esp_err_t app_driver_light_set_defaults(uint16_t endpoint_id, dali_light_ctx_t *ctx);

/**
 * @brief Poll all registered ballasts and update each bridged endpoint state.
 *
 * Issues QUERY STATUS (0x90) to every ballast in @p ctx_array and mirrors the
 * result to BridgedDeviceBasicInformation::Reachable on the corresponding
 * bridged endpoint:
 *   - device stops responding  → Reachable = false
 *   - device confirmed present → Reachable = true
 *
 * A hysteresis of two consecutive passing polls is required before a device
 * is marked reachable again; going unreachable is applied immediately.
 *
 * @param ctx_array  Array of DALI_MAX_BALLASTS pointers; NULL entries are skipped.
 * @param count      Number of entries in ctx_array (should be DALI_MAX_BALLASTS).
 */
void app_driver_dali_poll_reachability(dali_light_ctx_t **ctx_array, size_t count);

/**
 * @brief Force-push the current reachability state for all registered ballasts
 *        to the Matter stack, without requiring a state change.
 *
 * Call this once after the Matter server is fully ready (kServerReady event) to
 * ensure that controllers reflect the correct Reachable value of every bridged
 * endpoint, including endpoints whose DALI gear was already absent at startup.
 *
 * @param ctx_array  Array of DALI_MAX_BALLASTS pointers; NULL entries are skipped.
 * @param count      Number of entries in ctx_array (should be DALI_MAX_BALLASTS).
 */
void app_driver_dali_push_all_states(dali_light_ctx_t **ctx_array, size_t count);

#if CHIP_DEVICE_CONFIG_ENABLE_THREAD
#define ESP_OPENTHREAD_DEFAULT_RADIO_CONFIG() \
    { \
        .radio_mode = RADIO_MODE_NATIVE, \
    }

#define ESP_OPENTHREAD_DEFAULT_HOST_CONFIG() \
    { \
        .host_connection_mode = HOST_CONNECTION_MODE_NONE, \
    }

#define ESP_OPENTHREAD_DEFAULT_PORT_CONFIG() \
    { \
        .storage_partition_name = "nvs", .netif_queue_size = 10, .task_queue_size = 10, \
    }
#endif
