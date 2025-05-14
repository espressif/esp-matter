/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <platform/internal/CHIPDeviceLayerInternal.h>
#include <platform/CHIPDeviceConfig.h>
#include <ble/CHIPBleServiceData.h>
#include <platform/internal/BLEManager.h>
#include <esp_err.h>
#include <host/ble_gap.h>
#include <host/ble_gatt.h>
#include <host/ble_uuid.h>
#include <host/util/util.h>
#include <protocomm.h>
#include <stdint.h>
#include <string.h>

#include "protocomm_matter_ble.h"
#include "esp_cpu.h"

ESP_EVENT_DEFINE_BASE(PROTOCOMM_TRANSPORT_MATTER_BLE_EVENT);

int ble_uuid_flat(const ble_uuid_t *, void *);

typedef struct {
    protocomm_t *pc_matter_ble;
    uint8_t ble_uuid_base[BLE_UUID128_VAL_LENGTH];
    protocomm_matter_ble_name_uuid_t *g_name_uuid_array;
    ssize_t g_name_uuid_array_len;
    uint16_t gatt_mtu;
    unsigned ble_link_encryption:1;
} _protocomm_matter_ble_internal_t;

static _protocomm_matter_ble_internal_t *protoble_internal = nullptr;
static struct ble_gatt_svc_def *s_gatt_db = nullptr;
static ble_hs_adv_fields s_secondary_adv_fields;
static ble_hs_adv_fields s_secondary_resp_fields;

#define BLE_GATT_UUID_CHAR_DSC 0x2901
#define TAG "protocomm_matter_ble"

struct data_mbuf {
    SLIST_ENTRY(data_mbuf) node;
    uint8_t *outbuf;
    ssize_t outlen;
    uint16_t attr_handle;
};

static SLIST_HEAD(data_mbuf_head, data_mbuf) data_mbuf_list =
    SLIST_HEAD_INITIALIZER(data_mbuf_list);

static struct data_mbuf *find_attr_with_handle(uint16_t attr_handle)
{
    struct data_mbuf *cur;
    SLIST_FOREACH(cur, &data_mbuf_list, node) {
        if (cur->attr_handle == attr_handle) {
            return cur;
        }
    }
    return nullptr;
}

static void free_gatt_ble_misc_memory(struct ble_gatt_svc_def * svc_def)
{
    if (svc_def) {
        if (svc_def->characteristics) {
            for (int i = 0; i < protoble_internal->g_name_uuid_array_len; ++i) {
                if (svc_def->characteristics[i].descriptors) {
                    free((void *)svc_def->characteristics[i].descriptors->uuid);
                    free(svc_def->characteristics[i].descriptors);
                }
                free((void *)svc_def->characteristics[i].uuid);
            }
            free((void *)svc_def->characteristics);
        }
        if (svc_def->uuid) {
            free((void *)svc_def->uuid);
        }
    }
}

static void protocomm_matter_ble_cleanup()
{
    if (protoble_internal) {
        if (protoble_internal->g_name_uuid_array) {
            for (unsigned i = 0; i < protoble_internal->g_name_uuid_array_len; ++i) {
                if (protoble_internal->g_name_uuid_array[i].name) {
                    free((void *)protoble_internal->g_name_uuid_array[i].name);
                }
            }
            free(protoble_internal->g_name_uuid_array);
        }
        free(protoble_internal);
        protoble_internal = nullptr;
    }
}

static esp_err_t ble_gatt_add_primary_svcs(struct ble_gatt_svc_def *gatt_db_svc, int char_count)
{
    gatt_db_svc->type = BLE_GATT_SVC_TYPE_PRIMARY;
    gatt_db_svc->characteristics =
        (struct ble_gatt_chr_def *) calloc((char_count + 1), sizeof(struct ble_gatt_chr_def));
    if (!gatt_db_svc->characteristics) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

static const char *uuid128_to_handler(uint8_t *uuid)
{
    uint8_t *uuid16 = uuid + 12;
    for (int i = 0; i < protoble_internal->g_name_uuid_array_len; i++) {
        if (protoble_internal->g_name_uuid_array[i].uuid == *(uint16_t *)uuid16) {
            ESP_LOGD(TAG, "UUID (0x%x) matched with proto-name = %s", *uuid16, protoble_internal->g_name_uuid_array[i].name);
            return protoble_internal->g_name_uuid_array[i].name;
        }
    }
    return nullptr;
}

static int matter_ble_gatts_get_attr_value(uint16_t attr_handle, ssize_t *outlen, uint8_t **outbuf)
{
    struct data_mbuf *attr_mbuf = find_attr_with_handle(attr_handle);
    if (!attr_mbuf) {
        ESP_LOGE(TAG, "Outbuf with handle %d not found", attr_handle);
        return ESP_ERR_NOT_FOUND;
    }
    *outbuf = attr_mbuf->outbuf;
    *outlen = attr_mbuf->outlen;
    return ESP_OK;
}

static int matter_ble_gatts_set_attr_value(uint16_t attr_handle, ssize_t outlen, uint8_t *outbuf)
{
    struct data_mbuf *attr_mbuf = find_attr_with_handle(attr_handle);
    if (!attr_mbuf) {
        attr_mbuf = (struct data_mbuf *)calloc(1, sizeof(struct data_mbuf));
        if (!attr_mbuf) {
            ESP_LOGE(TAG, "Failed to allocate memory for storing outbuf and outlen");
            return ESP_ERR_NO_MEM;
        }
        SLIST_INSERT_HEAD(&data_mbuf_list, attr_mbuf, node);
        attr_mbuf->attr_handle = attr_handle;
    } else {
        free(attr_mbuf->outbuf);
    }
    attr_mbuf->outbuf = outbuf;
    attr_mbuf->outlen = outlen;
    return ESP_OK;
}

static int matter_ble_gatt_svr_chr_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt,
                                          void *arg)
{
    int rc = 0;
    esp_err_t err;
    char buf[BLE_UUID_STR_LEN] = { 0 };
    ssize_t temp_outlen = 0;
    uint8_t *temp_outbuf = nullptr;
    uint8_t *uuid = nullptr;
    uint8_t *data_buf = nullptr;
    uint16_t data_len = 0;
    uint16_t data_buf_len = 0;
    switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_READ_CHR:
        ESP_LOGD(TAG, "Read attempted for characteristic UUID = %s, attr_handle = %d",
                 ble_uuid_to_str(ctxt->chr->uuid, buf), attr_handle);
        rc = matter_ble_gatts_get_attr_value(attr_handle, &temp_outlen, &temp_outbuf);
        if (rc != 0) {
            ESP_LOGE(TAG, "Characteristic with attr_handle = %d is not added to the list", attr_handle);
            return 0;
        }
        if (os_mbuf_append(ctxt->om, temp_outbuf, static_cast<uint16_t>(temp_outlen)) != 0) {
                rc = BLE_ATT_ERR_INSUFFICIENT_RES;
        }
        break;
    case BLE_GATT_ACCESS_OP_WRITE_CHR:
        if (ctxt->om->om_len == 0) {
            ESP_LOGD(TAG,"Empty packet");
            return 0;
        }
        uuid = (uint8_t *) calloc(BLE_UUID128_VAL_LENGTH, sizeof(uint8_t));
        if (!uuid) {
            ESP_LOGE(TAG, "Error allocating memory for 128 bit UUID");
            return BLE_ATT_ERR_INSUFFICIENT_RES;
        }
        assert(ctxt->chr->uuid->type == BLE_UUID_TYPE_128);
        {
            const ble_uuid128_t *uuid128 = (const ble_uuid128_t *)ctxt->chr->uuid;
            memcpy(uuid, uuid128->value, BLE_UUID128_VAL_LENGTH);
        }
        if (rc != 0) {
            free(uuid);
            ESP_LOGE(TAG, "Error fetching Characteristic UUID128");
            return rc;
        }
        data_len = OS_MBUF_PKTLEN(ctxt->om);
        ESP_LOGD(TAG, "Write attempt for uuid = %s, attr_handle = %d, data_len = %d",
                 ble_uuid_to_str(ctxt->chr->uuid, buf), attr_handle, data_len);
        data_buf = (uint8_t *)calloc(1, data_len);
        if (!data_buf) {
            ESP_LOGE(TAG, "Error allocating memory for characteristic value");
            free(uuid);
            return BLE_ATT_ERR_INSUFFICIENT_RES;
        }
        rc = ble_hs_mbuf_to_flat(ctxt->om, data_buf, data_len, &data_buf_len);
        if (rc != 0) {
            ESP_LOGE(TAG, "Error getting data from memory buffers");
            free(uuid);
            free(data_buf);
            return BLE_ATT_ERR_UNLIKELY;
        }
        err = protocomm_req_handle(protoble_internal->pc_matter_ble, uuid128_to_handler(uuid), conn_handle, data_buf,
                                   data_buf_len, &temp_outbuf, &temp_outlen);
        free(uuid);
        free(data_buf);
        if (err == ESP_OK) {
            rc = matter_ble_gatts_set_attr_value(attr_handle, temp_outlen, temp_outbuf);
            if (rc != 0) {
                ESP_LOGE(TAG, "Failed to set outbuf for characteristic with attr_handle = %d", attr_handle);
                free(temp_outbuf);
            }
        } else {
            ESP_LOGE(TAG, "Invalid content received, killing connection");
            rc = BLE_ATT_ERR_INVALID_PDU;
        }
        break;
    default:
        rc = BLE_ATT_ERR_UNLIKELY;
    }
    return rc;
}

static int matter_ble_gatt_svr_dsc_access(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt,
                                          void *arg)
{
    if (ctxt->op != BLE_GATT_ACCESS_OP_READ_DSC) {
        return BLE_ATT_ERR_UNLIKELY;
    }
    int rc;
    size_t temp_outlen = strlen((const char *)ctxt->dsc->arg);
    rc = os_mbuf_append(ctxt->om, ctxt->dsc->arg, (uint16_t)temp_outlen);
    return rc;

}

static esp_err_t ble_gatt_add_characteristics(void *character, int idx)
{
    struct ble_gatt_chr_def *characteristics = (struct ble_gatt_chr_def *)character;
    ble_uuid128_t temp_uuid128 = { 0 };
    temp_uuid128.u.type = BLE_UUID_TYPE_128;
    memcpy(temp_uuid128.value, protoble_internal->ble_uuid_base, BLE_UUID128_VAL_LENGTH);
    memcpy(&temp_uuid128.value[12], &protoble_internal->g_name_uuid_array[idx].uuid, 2);

    (characteristics + idx)->flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE;
    if (protoble_internal->ble_link_encryption) {
        (characteristics + idx)->flags |= BLE_GATT_CHR_F_READ_ENC | BLE_GATT_CHR_F_WRITE_ENC;
    }

    (characteristics + idx)->access_cb = matter_ble_gatt_svr_chr_access;

    (characteristics + idx)->uuid = (ble_uuid_t *)calloc(1, sizeof(ble_uuid128_t));
    if (!(characteristics + idx)->uuid) {
        return ESP_ERR_NO_MEM;
    }
    memcpy((void *)(characteristics + idx)->uuid, &temp_uuid128, sizeof(ble_uuid128_t));
    return ESP_OK;
}

static esp_err_t ble_gatt_add_char_dsc(void *ch, int idx, uint16_t dsc_uuid)
{
    struct ble_gatt_chr_def *characteristics = (struct ble_gatt_chr_def *)ch;
    ble_uuid16_t uuid16;
    uuid16.u.type = BLE_UUID_TYPE_16;
    uuid16.value = dsc_uuid;
    (characteristics + idx)->descriptors =
        (struct ble_gatt_dsc_def *) calloc(2, sizeof(struct ble_gatt_dsc_def));
    if (!(characteristics + idx)->descriptors) {
        return ESP_ERR_NO_MEM;
    }
    (characteristics + idx)->descriptors[0].uuid = (ble_uuid_t *) calloc(1, sizeof(ble_uuid16_t));
    if (!(characteristics + idx)->descriptors[0].uuid) {
        return ESP_ERR_NO_MEM;
    }
    memcpy((void *)(characteristics + idx)->descriptors[0].uuid, &uuid16,
           sizeof(ble_uuid16_t));
    (characteristics + idx)->descriptors[0].att_flags = BLE_ATT_F_READ;
    (characteristics + idx)->descriptors[0].access_cb = matter_ble_gatt_svr_dsc_access;
    (characteristics + idx)->descriptors[0].arg = (void *)protoble_internal->g_name_uuid_array[idx].name;
    return ESP_OK;
}

static esp_err_t populate_gatt_db(struct ble_gatt_svc_def **gatt_db_svc_ptr, const protocomm_matter_ble_config_t *config)
{
    if (*gatt_db_svc_ptr) {
        return ESP_ERR_INVALID_ARG;
    }
    *gatt_db_svc_ptr = (struct ble_gatt_svc_def *) calloc(1, sizeof(struct ble_gatt_svc_def));
    if (!*gatt_db_svc_ptr) {
        return ESP_ERR_NO_MEM;
    }
     (*gatt_db_svc_ptr)->uuid = (ble_uuid_t *) calloc(1, sizeof(ble_uuid128_t));
    if (!(*gatt_db_svc_ptr)->uuid) {
        return ESP_ERR_NO_MEM;
    }
    ble_uuid128_t uuid128 = { 0 };
    uuid128.u.type = BLE_UUID_TYPE_128;
    memcpy(uuid128.value, config->service_uuid, BLE_UUID128_VAL_LENGTH);
    memcpy((void *) (*gatt_db_svc_ptr)->uuid, &uuid128, sizeof(ble_uuid128_t));
    // Add primary service
    esp_err_t err = ble_gatt_add_primary_svcs(*gatt_db_svc_ptr, config->name_uuid_array_len);
    if (err != ESP_OK) {
        return err;
    }
    for (int i = 0; i < config->name_uuid_array_len; ++i) {
        err = ble_gatt_add_characteristics((void *)(*gatt_db_svc_ptr)->characteristics, i);
        if (err != ESP_OK) {
            return err;
        }
        err = ble_gatt_add_char_dsc((void *) (*gatt_db_svc_ptr)->characteristics,
                                    i, BLE_GATT_UUID_CHAR_DSC);
        if (err != ESP_OK) {
            return err;
        }
    }
    return ESP_OK;
}

static void transport_matter_ble_connect(struct ble_gap_event *event, void *arg)
{
    esp_err_t err = ESP_OK;
    ESP_LOGD(TAG, "Inside BLE connect w/ conn_id - %d", event->connect.conn_handle);

    if (!protoble_internal) {
        ESP_LOGI(TAG,"Protocomm layer has already stopped");
        return;
    }

    err = protocomm_open_session(protoble_internal->pc_matter_ble, event->connect.conn_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "error creating the session");
    } else {
        protocomm_matter_ble_event_t ble_event = {};
        ble_event.evt_type = PROTOCOMM_TRANSPORT_MATTER_BLE_CONNECTED;
        ble_event.conn_handle = event->connect.conn_handle;
        ble_event.conn_status = event->connect.status;
        if (esp_event_post(PROTOCOMM_TRANSPORT_MATTER_BLE_EVENT, PROTOCOMM_TRANSPORT_MATTER_BLE_CONNECTED, &ble_event,
                           sizeof(protocomm_matter_ble_event_t), portMAX_DELAY) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to post transport pairing event");
        }
    }
}

static void transport_matter_ble_disconnect(struct ble_gap_event *event, void *arg)
{
    esp_err_t err = ESP_OK;
    ESP_LOGD(TAG, "Inside disconnect w/ session - %d", event->disconnect.conn.conn_handle);
    if (!protoble_internal) {
        ESP_LOGI(TAG,"Protocomm layer has already stopped");
        return;
    }

    if (protoble_internal->pc_matter_ble) {
        err = protocomm_close_session(protoble_internal->pc_matter_ble, event->disconnect.conn.conn_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "error closing the session after disconnect");
        } else {
            protocomm_matter_ble_event_t ble_event = {};
            ble_event.evt_type = PROTOCOMM_TRANSPORT_MATTER_BLE_DISCONNECTED;
            ble_event.conn_handle = event->disconnect.conn.conn_handle;
            ble_event.disconnect_reason = event->disconnect.reason;
            if (esp_event_post(PROTOCOMM_TRANSPORT_MATTER_BLE_EVENT, PROTOCOMM_TRANSPORT_MATTER_BLE_DISCONNECTED, &ble_event,
                               sizeof(protocomm_matter_ble_event_t), portMAX_DELAY) != ESP_OK) {
                ESP_LOGE(TAG, "Failed to post transport disconnection event");
            }
        }
    }
    protoble_internal->gatt_mtu = BLE_ATT_MTU_DFLT;
}

static void transport_matter_ble_set_mtu(struct ble_gap_event *event, void *arg)
{
    protoble_internal->gatt_mtu = event->mtu.value;
}

static int protocomm_matter_ble_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;

    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status == 0) {
            transport_matter_ble_connect(event, arg);
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            if (rc != 0) {
                ESP_LOGE(TAG, "No open connection with the specified handle");
                return rc;
            }
        }
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        transport_matter_ble_disconnect(event, arg);
        start_secondary_ble_adv();
        break;
    case BLE_GAP_EVENT_MTU:
        transport_matter_ble_set_mtu(event, arg);
        break;
    default:
        break;
    }
    return 0;
}

int start_secondary_ble_adv()
{
    if (!ble_hs_is_enabled()) {
        return 0;
    }
    int ret = 0;
    uint8_t secondary_own_addr_type;
    ret = ble_hs_util_ensure_addr(0);
    if (ret != 0) {
        ESP_LOGE(TAG, "ble_hs_util_ensure_addr() failed");
        return ret;
    }
    ret = ble_hs_id_infer_auto(0, &secondary_own_addr_type);
    if (ret != 0) {
        ESP_LOGE(TAG, "ble_hs_id_infer_auto() failed");
        return ret;
    }
    ble_gap_ext_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.scannable     = 1;
    adv_params.own_addr_type = secondary_own_addr_type;
    adv_params.legacy_pdu    = 1;
    adv_params.connectable = 1;
    adv_params.itvl_min = 0x100;
    adv_params.itvl_max = 0x100;

    if (ble_gap_ext_adv_active(1)) {
        ESP_LOGI(TAG, "Device already advertising, stop active advertisement and restart");
        ret = ble_gap_ext_adv_stop(1);
        if (ret != 0) {
            ESP_LOGE(TAG, "ble_gap_ext_adv_stop() failed, cannot restart");
            return ret;
        }
    }

    ret = ble_gap_ext_adv_configure(1, &adv_params, NULL, protocomm_matter_ble_gap_event, NULL);
    if (ret != 0) {
        ESP_LOGE(TAG, "ble_gap_ext_adv_configure() failed");
        return ret;
    }
    struct os_mbuf * data = os_msys_get_pkthdr(BLE_HCI_MAX_ADV_DATA_LEN, 0);
    assert(data);
    ret = ble_hs_adv_set_fields_mbuf(&s_secondary_adv_fields, data);
    if (ret != 0) {
        ESP_LOGE(TAG, "ble_hs_adv_set_fields_mbuf() failed");
        return ret;
    }
    ret = ble_gap_ext_adv_set_data(1, data);
    if (ret != 0) {
        ESP_LOGE(TAG, "ble_gap_ext_adv_set_data()");
        return ret;
    }
    struct os_mbuf * resp_data = os_msys_get_pkthdr(BLE_HCI_MAX_ADV_DATA_LEN, 0);
    assert(resp_data);
    ret = ble_hs_adv_set_fields_mbuf(&s_secondary_resp_fields, resp_data);
    if (ret != 0) {
        ESP_LOGE(TAG, "ble_hs_adv_set_fields_mbuf() failed");
        return ret;
    }
    ret = ble_gap_ext_adv_rsp_set_data(1, resp_data);
    if (ret != 0) {
        ESP_LOGE(TAG, "ble_gap_ext_adv_rsp_set_data() failed");
        return ret;
    }
    ret = ble_gap_ext_adv_start(1, 0, 0);
    if (ret != 0) {
        ESP_LOGE(TAG, "ble_gap_ext_adv_start() failed");
        return ret;
    }
    return 0;
}

esp_err_t protocomm_matter_ble_start(protocomm_t *pc, const protocomm_matter_ble_config_t *config)
{
    if (!pc || !config || !config->name_uuid_array_len || !config->name_uuid_array) {
        return ESP_ERR_INVALID_ARG;
    }
    if (protoble_internal) {
        return ESP_ERR_INVALID_STATE;
    }
    protoble_internal = (_protocomm_matter_ble_internal_t *) calloc(1, sizeof(_protocomm_matter_ble_internal_t));
    if (!protoble_internal) {
        return ESP_ERR_NO_MEM;
    }
    memcpy(protoble_internal->ble_uuid_base, config->service_uuid, BLE_UUID128_VAL_LENGTH);
    protoble_internal->g_name_uuid_array_len = config->name_uuid_array_len;
    protoble_internal->g_name_uuid_array = (protocomm_matter_ble_name_uuid_t *)malloc(config->name_uuid_array_len * sizeof(protocomm_matter_ble_name_uuid_t));
    if (!protoble_internal->g_name_uuid_array) {
        protocomm_matter_ble_cleanup();
        return ESP_ERR_NO_MEM;
    }

    for (unsigned i = 0; i < protoble_internal->g_name_uuid_array_len; i++) {
        protoble_internal->g_name_uuid_array[i].uuid = config->name_uuid_array[i].uuid;
        protoble_internal->g_name_uuid_array[i].name = strdup(config->name_uuid_array[i].name);
        if (!protoble_internal->g_name_uuid_array[i].name) {
            ESP_LOGE(TAG, "Error allocating internal name UUID entry");
            protocomm_matter_ble_cleanup();
            return ESP_ERR_NO_MEM;
        }
    }

    protoble_internal->pc_matter_ble = pc;
    protoble_internal->gatt_mtu = BLE_ATT_MTU_DFLT;
    protoble_internal->ble_link_encryption = config->ble_link_encryption;

    if (populate_gatt_db(&s_gatt_db, config) != ESP_OK) {
        free_gatt_ble_misc_memory(s_gatt_db);
    }

    std::vector<struct ble_gatt_svc_def> extGattSvcs;
    extGattSvcs.push_back(*s_gatt_db);
    chip::DeviceLayer::Internal::BLEMgrImpl().ConfigureExtraServices(extGattSvcs, false);
    memset(&s_secondary_adv_fields, 0, sizeof(s_secondary_adv_fields));
    s_secondary_adv_fields.flags = (BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP);
    s_secondary_adv_fields.num_uuids128 = 1;
    s_secondary_adv_fields.uuids128_is_complete = 1;
    static ble_uuid128_t s_secondary_adv_uuid;
    s_secondary_adv_uuid.u.type = BLE_UUID_TYPE_128;
    memcpy(s_secondary_adv_uuid.value, config->service_uuid, sizeof(config->service_uuid));
    s_secondary_adv_fields.uuids128 = &s_secondary_adv_uuid;

    memset(&s_secondary_resp_fields, 0, sizeof(s_secondary_resp_fields));
    s_secondary_resp_fields.name = (const uint8_t *)config->device_name;
    s_secondary_resp_fields.name_len = (uint8_t)strlen(config->device_name);
    s_secondary_resp_fields.name_is_complete = 1;
    return ESP_OK;
}

esp_err_t protocomm_matter_ble_stop(protocomm_t *pc)
{
    ESP_LOGD(TAG, "protocomm_ble_stop called here...");

    if ((pc != NULL) && (protoble_internal != NULL ) && (pc == protoble_internal->pc_matter_ble)) {
        protoble_internal->pc_matter_ble = nullptr;
    }
    chip::DeviceLayer::Internal::BLEMgr().Shutdown();
    free_gatt_ble_misc_memory(s_gatt_db);
    protocomm_matter_ble_cleanup();
    return ESP_OK;
}
