#include "blsync_ble_app.h"
#include "wifi_prov_api.h"
#include <FreeRTOS.h>
#include <task.h>
#include "blsync_ble.h"
#include "bluetooth.h"
#include "ble_cli_cmds.h"
#include "hci_driver.h"
#include "ble_lib_api.h"

static bl_ble_sync_t *gp_index = NULL;

static void wifiprov_connect_ap_ind(struct wifi_conn *info)
{
    extern int wifi_mgmr_status_code_clean_internal();
    wifi_mgmr_status_code_clean_internal();
    printf("Recevied indication to connect to AP\r\n");    
    wifi_prov_api_event_trigger_connect(info);
}

static void wifiprov_disc_from_ap_ind(void)
{
    printf("Recevied indication to disconnect to AP\r\n");
    wifi_prov_api_event_trigger_disconnect();
}

static void wifiprov_wifi_scan(void(*complete)(void *))
{
    printf("Recevied indication to wifi scan\r\n");
    wifi_prov_api_event_trigger_scan(complete);
}

static void wifiprov_api_state_get(void(*state_get)(void *))
{
    printf("Recevied indication to wifi state get\r\n");
    wifi_prov_api_event_state_get(state_get);
}

static void blesync_complete_cb (void *p_arg)
{
    bl_ble_sync_t *p_index = (bl_ble_sync_t *)p_arg;
    bl_ble_sync_stop(p_index);
    vPortFree(p_index);
}

static struct blesync_wifi_func WifiProv_conn_callback = {
	.local_connect_remote_ap = wifiprov_connect_ap_ind,
	.local_disconnect_remote_ap = wifiprov_disc_from_ap_ind,
	.local_wifi_scan = wifiprov_wifi_scan,
	.local_wifi_state_get = wifiprov_api_state_get,
};

static void blsync_init(int err)
{
    if (err != 0) {
        return;
    }
    if (gp_index != NULL) {
        printf("blsync already started\r\n");
        return;
    }
#ifdef CONFIG_BT_STACK_CLI 
    ble_cli_register();
#endif /* CONFIG_BT_STACK_CLI */
    gp_index = pvPortMalloc(sizeof(bl_ble_sync_t));
    if (gp_index == NULL) {
        return;
    }
    bl_ble_sync_start(gp_index,
                      &WifiProv_conn_callback,
                      blesync_complete_cb,
                      (void *)gp_index);
}

static void ble_stack_start(void)
{
     // Initialize BLE controller
    ble_controller_init(configMAX_PRIORITIES - 1);
    // Initialize BLE Host stack
    hci_driver_init();
    bt_enable(blsync_init);
}

void blsync_ble_start (void)
{
    static uint8_t stack_started = 0;

    if (stack_started == 1) {
        blsync_init(0);
        return;
    }
    ble_stack_start();
    stack_started = 1;
}

void blsync_ble_stop (void)
{
    bl_ble_sync_stop(gp_index);
    vPortFree(gp_index);
    gp_index = NULL;
}

