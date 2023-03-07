/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <vfs.h>
#include <aos/kernel.h>
#include <aos/yloop.h>
#include <event_device.h>
#include <cli.h>

#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <lwip/tcp.h>
#include <lwip/err.h>
#include <http_client.h>
#include <netutils/netutils.h>

#include <bl602_glb.h>
#include <bl602_hbn.h>
#include "bl602_adc.h"

#include <bl_sys.h>
#include <bl_uart.h>
#include <bl_chip.h>
#include <bl_wifi.h>
#include <hal_wifi.h>
#include <bl_sec.h>
#include <bl_cks.h>
#include <bl_irq.h>
#include <bl_timer.h>
#include <bl_dma.h>
#include <bl_gpio_cli.h>
#include <bl_wdt_cli.h>
#include <hosal_uart.h>
#include <hosal_adc.h>
#include <hal_sys.h>
#include <hal_gpio.h>
#include <hal_hbn.h>
#include <hal_boot2.h>
#include <hal_board.h>
#include <hal_button.h>
#include <looprt.h>
#include <loopset.h>
#include <sntp.h>
#include <bl_sys_time.h>
#include <bl_sys_ota.h>
#include <bl_romfs.h>
#include <fdt.h>
#include <device/vfs_uart.h>
#include <wifi_bt_coex.h>

//#include <easyflash.h>
#include <bl60x_fw_api.h>
#include <wifi_mgmr_ext.h>
#include <utils_log.h>
#include <libfdt.h>
#include <blog.h>
#include "ble_lib_api.h"
#include "hal_pds.h"
#include "bl_rtc.h"
#include "utils_string.h"

#if defined(CONFIG_AUTO_PTS)
#include "bttester.h"
#include "autopts_uart.h"
#endif

#define TASK_PRIORITY_FW            ( 30 )
#define mainHELLO_TASK_PRIORITY     ( 20 )
#define UART_ID_2 (2)
#define WIFI_AP_PSM_INFO_SSID           "conf_ap_ssid"
#define WIFI_AP_PSM_INFO_PASSWORD       "conf_ap_psk"
#define WIFI_AP_PSM_INFO_PMK            "conf_ap_pmk"
#define WIFI_AP_PSM_INFO_BSSID          "conf_ap_bssid"
#define WIFI_AP_PSM_INFO_CHANNEL        "conf_ap_channel"
#define WIFI_AP_PSM_INFO_IP             "conf_ap_ip"
#define WIFI_AP_PSM_INFO_MASK           "conf_ap_mask"
#define WIFI_AP_PSM_INFO_GW             "conf_ap_gw"
#define WIFI_AP_PSM_INFO_DNS1           "conf_ap_dns1"
#define WIFI_AP_PSM_INFO_DNS2           "conf_ap_dns2"
#define WIFI_AP_PSM_INFO_IP_LEASE_TIME  "conf_ap_ip_lease_time"
#define WIFI_AP_PSM_INFO_GW_MAC         "conf_ap_gw_mac"
#define CLI_CMD_AUTOSTART1              "cmd_auto1"
#define CLI_CMD_AUTOSTART2              "cmd_auto2"

#define TIME_5MS_IN_32768CYCLE  (164) // (5000/(1000000/32768))

bool pds_start = false;
extern void ble_stack_start(void);

static wifi_interface_t wifi_interface;
static wifi_conf_t conf =
{
    .country_code = "CN",
};

#if defined(CFG_BLE_PDS)
void vApplicationIdleHook(void)
{
    if(!pds_start){
        __asm volatile(
                "   wfi     "
        );
        /*empty*/
    }
}
#endif

#if ( configUSE_TICKLESS_IDLE != 0 )
void vApplicationSleep( TickType_t xExpectedIdleTime_ms )
{
#if defined(CFG_BLE_PDS)
    int32_t bleSleepDuration_32768cycles = 0;
    int32_t expectedIdleTime_32768cycles = 0;
    eSleepModeStatus eSleepStatus;
    bool freertos_max_idle = false;

    if (pds_start == 0)
        return;

    if(xExpectedIdleTime_ms + xTaskGetTickCount() == portMAX_DELAY){
        freertos_max_idle = true;
    }else{   
        xExpectedIdleTime_ms -= 1;
        expectedIdleTime_32768cycles = 32768 * xExpectedIdleTime_ms / 1000;
    }

    if((!freertos_max_idle)&&(expectedIdleTime_32768cycles < TIME_5MS_IN_32768CYCLE)){
        return;
    }

    /*Disable mtimer interrrupt*/
    *(volatile uint8_t*)configCLIC_TIMER_ENABLE_ADDRESS = 0;

    eSleepStatus = eTaskConfirmSleepModeStatus();
    if(eSleepStatus == eAbortSleep || ble_controller_sleep_is_ongoing())
    {
        /*A task has been moved out of the Blocked state since this macro was
        executed, or a context siwth is being held pending.Restart the tick 
        and exit the critical section. */
        /*Enable mtimer interrrupt*/
        *(volatile uint8_t*)configCLIC_TIMER_ENABLE_ADDRESS = 1;
        //printf("%s:not do ble sleep\r\n", __func__);
        return;
    }

    bleSleepDuration_32768cycles = ble_controller_sleep();

	if(bleSleepDuration_32768cycles < TIME_5MS_IN_32768CYCLE)
    {
        /*BLE controller does not allow sleep.  Do not enter a sleep state.Restart the tick 
        and exit the critical section. */
        /*Enable mtimer interrrupt*/
        //printf("%s:not do pds sleep\r\n", __func__);
        *(volatile uint8_t*)configCLIC_TIMER_ENABLE_ADDRESS = 1;
    }
    else
    {
        printf("%s:bleSleepDuration_32768cycles=%ld\r\n", __func__, bleSleepDuration_32768cycles);
        if(eSleepStatus == eStandardSleep && ((!freertos_max_idle) && (expectedIdleTime_32768cycles < bleSleepDuration_32768cycles)))
        {
           hal_pds_enter_with_time_compensation(1, expectedIdleTime_32768cycles - 40);//40);//20);
        }
        else
        {
           hal_pds_enter_with_time_compensation(1, bleSleepDuration_32768cycles - 40);//40);//20);
        }
    }
#endif
}
#endif

static void proc_hellow_entry(void *pvParameters)
{
    vTaskDelay(500);

    while (1) {
        printf("%s: RISC-V rv32imafc\r\n", __func__);
        vTaskDelay(10000);
    }
    vTaskDelete(NULL);
}

static unsigned char char_to_hex(char asccode)
{
    unsigned char ret;

    if('0'<=asccode && asccode<='9')
        ret=asccode-'0';
    else if('a'<=asccode && asccode<='f')
        ret=asccode-'a'+10;
    else if('A'<=asccode && asccode<='F')
        ret=asccode-'A'+10;
    else
        ret=0;

    return ret;
}

static void _chan_str_to_hex(uint8_t *chan_band, uint16_t *chan_freq, char *chan)
{
    int i, freq_len, base=1;
    uint8_t band;
    uint16_t freq = 0;
    char *p, *q;

    /*should have the following format
     * 2412|0
     * */
    p = strchr(chan, '|') + 1;
    if (NULL == p) {
        return;
    }
    band = char_to_hex(p[0]);
    (*chan_band) = band;

    freq_len = strlen(chan) - strlen(p) - 1;
    q = chan;
    q[freq_len] = '\0';
    for (i=0; i< freq_len; i++) {
       freq = freq + char_to_hex(q[freq_len-1-i]) * base;
       base = base * 10;
    }
    (*chan_freq) = freq;
}

static void bssid_str_to_mac(uint8_t *hex, char *bssid, int len)
{
   unsigned char l4,h4;
   int i,lenstr;
   lenstr = len;

   if(lenstr%2) {
       lenstr -= (lenstr%2);
   }

   if(lenstr==0){
       return;
   }

   for(i=0; i < lenstr; i+=2) {
       h4=char_to_hex(bssid[i]);
       l4=char_to_hex(bssid[i+1]);
       hex[i/2]=(h4<<4)+l4;
   }
}

static void _connect_wifi()
{
    /*XXX caution for BIG STACK*/
    char pmk[66], bssid[32], chan[10];
    char ssid[33], password[66];
    char val_buf[66];
//    char val_len = sizeof(val_buf) - 1;
    uint8_t mac[6];
    uint8_t band = 0;
    uint16_t freq = 0;

    wifi_interface = wifi_mgmr_sta_enable();
    printf("[APP] [WIFI] [T] %lld\r\n"
           "[APP]   Get STA %p from Wi-Fi Mgmr, pmk ptr %p, ssid ptr %p, password %p\r\n",
           aos_now_ms(),
           wifi_interface,
           pmk,
           ssid,
           password
    );
    memset(pmk, 0, sizeof(pmk));
    memset(ssid, 0, sizeof(ssid));
    memset(password, 0, sizeof(password));
    memset(bssid, 0, sizeof(bssid));
    memset(mac, 0, sizeof(mac));
    memset(chan, 0, sizeof(chan));

    memset(val_buf, 0, sizeof(val_buf));
//    ef_get_env_blob((const char *)WIFI_AP_PSM_INFO_SSID, val_buf, val_len, NULL);
    if (val_buf[0]) {
        /*We believe that when ssid is set, wifi_confi is OK*/
        strncpy(ssid, val_buf, sizeof(ssid) - 1);
    } else {
        /*Won't connect, since ssid config is empty*/
        puts("[APP]    Empty Config\r\n");
        puts("[APP]    Try to set the following ENV with psm_set command, then reboot\r\n");
        puts("[APP]    NOTE: " WIFI_AP_PSM_INFO_PMK " MUST be psm_unset when conf is changed\r\n");
        puts("[APP]    env: " WIFI_AP_PSM_INFO_SSID "\r\n");
        puts("[APP]    env: " WIFI_AP_PSM_INFO_PASSWORD "\r\n");
        puts("[APP]    env(optinal): " WIFI_AP_PSM_INFO_PMK "\r\n");
        return;
    }

    memset(val_buf, 0, sizeof(val_buf));
//    ef_get_env_blob((const char *)WIFI_AP_PSM_INFO_PASSWORD, val_buf, val_len, NULL);
    if (val_buf[0]) {
        strncpy(password, val_buf, sizeof(password) - 1);
    }

    memset(val_buf, 0, sizeof(val_buf));
//    ef_get_env_blob((const char *)WIFI_AP_PSM_INFO_PMK, val_buf, val_len, NULL);
    if (val_buf[0]) {
        strncpy(pmk, val_buf, sizeof(pmk) - 1);
    }
    if (0 == pmk[0]) {
        printf("[APP] [WIFI] [T] %lld\r\n",
           aos_now_ms()
        );
        puts("[APP]    Re-cal pmk\r\n");
        /*At lease pmk is not illegal, we re-cal now*/
        //XXX time consuming API, so consider lower-prirotiy for cal PSK to avoid sound glitch
        wifi_mgmr_psk_cal(
                password,
                ssid,
                strlen(ssid),
                pmk
        );
//        ef_set_env(WIFI_AP_PSM_INFO_PMK, pmk);
//        ef_save_env();
    }
    memset(val_buf, 0, sizeof(val_buf));
//    ef_get_env_blob((const char *)WIFI_AP_PSM_INFO_CHANNEL, val_buf, val_len, NULL);
    if (val_buf[0]) {
        strncpy(chan, val_buf, sizeof(chan) - 1);
        printf("connect wifi channel = %s\r\n", chan);
        _chan_str_to_hex(&band, &freq, chan);
    }
    memset(val_buf, 0, sizeof(val_buf));
//    ef_get_env_blob((const char *)WIFI_AP_PSM_INFO_BSSID, val_buf, val_len, NULL);
    if (val_buf[0]) {
        strncpy(bssid, val_buf, sizeof(bssid) - 1);
        printf("connect wifi bssid = %s\r\n", bssid);
        bssid_str_to_mac(mac, bssid, strlen(bssid));
        printf("mac = %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                mac[0],
                mac[1],
                mac[2],
                mac[3],
                mac[4],
                mac[5]
        );
    }
    printf("[APP] [WIFI] [T] %lld\r\n"
           "[APP]    SSID %s\r\n"
           "[APP]    SSID len %d\r\n"
           "[APP]    password %s\r\n"
           "[APP]    password len %d\r\n"
           "[APP]    pmk %s\r\n"
           "[APP]    bssid %s\r\n"
           "[APP]    channel band %d\r\n"
           "[APP]    channel freq %d\r\n",
           aos_now_ms(),
           ssid,
           strlen(ssid),
           password,
           strlen(password),
           pmk,
           bssid,
           band,
           freq
    );
    //wifi_mgmr_sta_connect(wifi_interface, ssid, pmk, NULL);
    wifi_mgmr_sta_connect(wifi_interface, ssid, password, pmk, mac, band, freq);
}

#if defined(CONFIG_BT_MESH_SYNC)
typedef struct _wifi_item {
    char ssid[32];
    uint32_t ssid_len;
    uint8_t bssid[6];
    uint8_t channel;
    uint8_t auth;
    int8_t rssi;
} _wifi_item_t;

struct _wifi_conn {
    char ssid[32];
    char ssid_tail[1];
    char pask[64];
};

struct _wifi_state {
    char ip[16];
    char gw[16];
    char mask[16];
    char ssid[32];
    char ssid_tail[1];
    uint8_t bssid[6];
    uint8_t state;
};
#endif /* CONFIG_BT_MESH_SYNC */
static void wifi_sta_connect(char *ssid, char *password)
{
    wifi_interface_t wifi_interface;

    wifi_interface = wifi_mgmr_sta_enable();
    wifi_mgmr_sta_connect(wifi_interface, ssid, password, NULL, NULL, 0, 0);
}
#if defined(CONFIG_BT_MESH_SYNC)
static void scan_item_cb(wifi_mgmr_ap_item_t *env, uint32_t *param1, wifi_mgmr_ap_item_t *item)
{
    _wifi_item_t wifi_item;
    void (*complete)(void *) = (void (*)(void *))param1;

    wifi_item.auth = item->auth;
    wifi_item.rssi = item->rssi;
    wifi_item.channel = item->channel;
    wifi_item.ssid_len = item->ssid_len;
    memcpy(wifi_item.ssid, item->ssid, sizeof(wifi_item.ssid));
    memcpy(wifi_item.bssid, item->bssid, sizeof(wifi_item.bssid));

    if (complete) {
        complete(&wifi_item);
    }
}

static void scan_complete_cb(void *p_arg, void *param)
{
    wifi_mgmr_scan_ap_all(NULL, p_arg, scan_item_cb);
}

static void wifiprov_scan(void *p_arg)
{
    wifi_mgmr_scan(p_arg, scan_complete_cb);
}

static void wifiprov_wifi_state_get(void *p_arg)
{
    int tmp_state;
    wifi_mgmr_sta_connect_ind_stat_info_t info;
    ip4_addr_t ip, gw, mask;
    struct _wifi_state state;
    void (*state_get_cb)(void *) = (void (*)(void *))p_arg;

    memset(&state, 0, sizeof(state));
    memset(&info, 0, sizeof(info));
    wifi_mgmr_state_get(&tmp_state);
    wifi_mgmr_sta_ip_get(&ip.addr, &gw.addr, &mask.addr);
    wifi_mgmr_sta_connect_ind_stat_get(&info);

    state.state = tmp_state;
    strcpy(state.ip, ip4addr_ntoa(&ip));
    strcpy(state.mask, ip4addr_ntoa(&mask));
    strcpy(state.gw, ip4addr_ntoa(&gw));
    memcpy(state.ssid, info.ssid, sizeof(state.ssid));
    memcpy(state.bssid, info.bssid, sizeof(state.bssid));
    state.ssid_tail[0] = 0;

    printf("IP  :%s \r\n", state.ip);
    printf("GW  :%s \r\n", state.gw);
    printf("MASK:%s \r\n", state.mask);

    if (state_get_cb) {
        state_get_cb(&state);
    }
}
#endif /* CONFIG_BT_MESH_SYNC */
static void event_cb_wifi_event(input_event_t *event, void *private_data)
{
    static char *ssid;
    static char *password;

    switch (event->code) {
        case CODE_WIFI_ON_INIT_DONE:
        {
            printf("[APP] [EVT] INIT DONE %lld\r\n", aos_now_ms());
            wifi_mgmr_start_background(&conf);
        }
        break;
        case CODE_WIFI_ON_MGMR_DONE:
        {
            printf("[APP] [EVT] MGMR DONE %lld\r\n", aos_now_ms());
            _connect_wifi();
        }
        break;
        case CODE_WIFI_ON_SCAN_DONE:
        {
            printf("[APP] [EVT] SCAN Done %lld, SCAN Result: %s\r\n",
                aos_now_ms(),
                WIFI_SCAN_DONE_EVENT_OK == event->value ? "OK" : "Busy now"
            );
            wifi_mgmr_cli_scanlist();
        }
        break;
        case CODE_WIFI_ON_DISCONNECT:
        {
            printf("[APP] [EVT] disconnect %lld, Reason: %s\r\n",
                aos_now_ms(),
                wifi_mgmr_status_code_str(event->value)
            );
        }
        break;
        case CODE_WIFI_ON_CONNECTING:
        {
            printf("[APP] [EVT] Connecting %lld\r\n", aos_now_ms());
        }
        break;
        case CODE_WIFI_CMD_RECONNECT:
        {
            printf("[APP] [EVT] Reconnect %lld\r\n", aos_now_ms());
        }
        break;
        case CODE_WIFI_ON_CONNECTED:
        {
            printf("[APP] [EVT] connected %lld\r\n", aos_now_ms());
        }
        break;
        case CODE_WIFI_ON_PRE_GOT_IP:
        {
            printf("[APP] [EVT] connected %lld\r\n", aos_now_ms());
        }
        break;
        case CODE_WIFI_ON_GOT_IP:
        {
            printf("[APP] [EVT] GOT IP %lld\r\n", aos_now_ms());
            printf("[SYS] Memory left is %d Bytes\r\n", xPortGetFreeHeapSize());
        }
        break;
        case CODE_WIFI_ON_PROV_SSID:
        {
            printf("[APP] [EVT] [PROV] [SSID] %lld: %s\r\n",
                    aos_now_ms(),
                    event->value ? (const char*)event->value : "UNKNOWN"
            );
            if (ssid) {
                vPortFree(ssid);
                ssid = NULL;
            }
            ssid = (char*)event->value;
        }
        break;
        case CODE_WIFI_ON_PROV_BSSID:
        {
            printf("[APP] [EVT] [PROV] [BSSID] %lld: %s\r\n",
                    aos_now_ms(),
                    event->value ? (const char*)event->value : "UNKNOWN"
            );
            if (event->value) {
                vPortFree((void*)event->value);
            }
        }
        break;
        case CODE_WIFI_ON_PROV_PASSWD:
        {
            printf("[APP] [EVT] [PROV] [PASSWD] %lld: %s\r\n", aos_now_ms(),
                    event->value ? (const char*)event->value : "UNKNOWN"
            );
            if (password) {
                vPortFree(password);
                password = NULL;
            }
            password = (char*)event->value;
        }
        break;
        case CODE_WIFI_ON_PROV_CONNECT:
        {
            printf("[APP] [EVT] [PROV] [CONNECT] %lld\r\n", aos_now_ms());
			#if defined(CONFIG_BT_MESH_SYNC)
			if(event->value){
				struct _wifi_conn *conn_info = (struct _wifi_conn *)event->value;
				wifi_sta_connect(conn_info->ssid, conn_info->pask);
				break;
			}
			#endif
			printf("connecting to %s:%s...\r\n", ssid, password);
			wifi_sta_connect(ssid, password);
        }
        break;
        case CODE_WIFI_ON_PROV_DISCONNECT:
        {
            printf("[APP] [EVT] [PROV] [DISCONNECT] %lld\r\n", aos_now_ms());
			#if defined(CONFIG_BT_MESH_SYNC)
            wifi_mgmr_sta_disconnect();
            vTaskDelay(1000);
            wifi_mgmr_sta_disable(NULL);
			#endif
        }
        break;
		#if defined(CONFIG_BT_MESH_SYNC)
		case CODE_WIFI_ON_PROV_SCAN_START:
		{
			printf("[APP] [EVT] [PROV] [SCAN] %lld\r\n", aos_now_ms());
			wifiprov_scan((void *)event->value);
		}
		break;
		case CODE_WIFI_ON_PROV_STATE_GET:
		{
			printf("[APP] [EVT] [PROV] [STATE] %lld\r\n", aos_now_ms());
			wifiprov_wifi_state_get((void *)event->value);
		}
		break;
		#endif /*CONFIG_BT_MESH_SYNC*/
        default:
        {
            printf("[APP] [EVT] Unknown code %u, %lld\r\n", event->code, aos_now_ms());
            /*nothing*/
        }
    }
}

static void __attribute__((unused)) cmd_aws(char *buf, int len, int argc, char **argv)
{
void aws_main_entry(void *arg);
    xTaskCreate(aws_main_entry, (char*)"aws_iot", 4096, NULL, 10, NULL);
}

static void cmd_pka(char *buf, int len, int argc, char **argv)
{
    bl_pka_test();
}

static void cmd_sha(char *buf, int len, int argc, char **argv)
{
    bl_sec_sha_test();
}

static void cmd_trng(char *buf, int len, int argc, char **argv)
{
    bl_sec_test();
}

static void cmd_aes(char *buf, int len, int argc, char **argv)
{
    bl_sec_aes_test();
}

static void cmd_cks(char *buf, int len, int argc, char **argv)
{
    bl_cks_test();
}

static void cmd_dma(char *buf, int len, int argc, char **argv)
{
    bl_dma_test();
}

static void cmd_exception_load(char *buf, int len, int argc, char **argv)
{
    bl_irq_exception_trigger(BL_IRQ_EXCEPTION_TYPE_LOAD_MISALIGN, (void*)0x22008001);
}

static void cmd_exception_l_illegal(char *buf, int len, int argc, char **argv)
{
    bl_irq_exception_trigger(BL_IRQ_EXCEPTION_TYPE_ACCESS_ILLEGAL, (void*)0x00200000);
}

static void cmd_exception_store(char *buf, int len, int argc, char **argv)
{
    bl_irq_exception_trigger(BL_IRQ_EXCEPTION_TYPE_STORE_MISALIGN, (void*)0x22008001);
}

static void cmd_exception_illegal_ins(char *buf, int len, int argc, char **argv)
{
    bl_irq_exception_trigger(BL_IRQ_EXCEPTION_TYPE_ILLEGAL_INSTRUCTION, (void*)0x22008001);
}

#define MAXBUF          128
#define BUFFER_SIZE     (12*1024)

#define PORT 80

static int client_demo(char *hostname)
{
    int sockfd;
    /* Get host address from the input name */
    struct hostent *hostinfo = gethostbyname(hostname);
    uint8_t *recv_buffer;

    if (!hostinfo) {
        printf("gethostbyname Failed\r\n");
        return -1;
    }

    struct sockaddr_in dest;

    char buffer[MAXBUF];
    /* Create a socket */
    /*---Open socket for streaming---*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error in socket\r\n");
        return -1;
    }

    /*---Initialize server address/port struct---*/
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(PORT);
    dest.sin_addr = *((struct in_addr *) hostinfo->h_addr);
//    char ip[16];
    uint32_t address = dest.sin_addr.s_addr;
    char *ip = inet_ntoa(address);

    printf("Server ip Address : %s\r\n", ip);
    /*---Connect to server---*/
    if (connect(sockfd,
             (struct sockaddr *)&dest,
             sizeof(dest)) != 0) {
        printf("Error in connect\r\n");
        return -1;
    }
    /*---Get "Hello?"---*/
    memset(buffer, 0, MAXBUF);
    char wbuf[]
        = "GET /ddm/ContentResource/music/204.mp3 HTTP/1.1\r\nHost: nf.cr.dandanman.com\r\nUser-Agent: wmsdk\r\nAccept: */*\r\n\r\n";
    write(sockfd, wbuf, sizeof(wbuf) - 1);

    int ret = 0;
    int total = 0;
    int debug_counter = 0;
    uint32_t ticks_start, ticks_end, time_consumed;

    ticks_start = xTaskGetTickCount();
    recv_buffer = pvPortMalloc(BUFFER_SIZE);
    if (NULL == recv_buffer) {
        goto out;
    }
    while (1) {
        ret = read(sockfd, recv_buffer, BUFFER_SIZE);
        if (ret == 0) {
            printf("eof\n\r");
            break;
        } else if (ret < 0) {
            printf("ret = %d, err = %d\n\r", ret, errno);
            break;
        } else {
            total += ret;
            /*use less debug*/
            if (0 == ((debug_counter++) & 0xFF)) {
                printf("total = %d, ret = %d\n\r", total, ret);
            }
            //vTaskDelay(2);
            if (total > 82050000) {
                ticks_end = xTaskGetTickCount();
                time_consumed = ((uint32_t)(((int32_t)ticks_end) - ((int32_t)ticks_start))) / 1000;
                printf("Download comlete, total time %u s, speed %u Kbps\r\n",
                        (unsigned int)time_consumed,
                        (unsigned int)(total / time_consumed * 8 / 1000)
                );
                break;
            }
        }
    }

    vPortFree(recv_buffer);
out:
    close(sockfd);
    return 0;
}

static void http_test_cmd(char *buf, int len, int argc, char **argv)
{
    // http://nf.cr.dandanman.com/ddm/ContentResource/music/204.mp3
    client_demo("nf.cr.dandanman.com");
}

static void cb_httpc_result(void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err)
{
    httpc_state_t **req = (httpc_state_t**)arg;

    printf("[HTTPC] Transfer finished. rx_content_len is %lu\r\n", rx_content_len);
    *req = NULL;
}

err_t cb_httpc_headers_done_fn(httpc_state_t *connection, void *arg, struct pbuf *hdr, u16_t hdr_len, u32_t content_len)
{
    printf("[HTTPC] hdr_len is %u, content_len is %lu\r\n", hdr_len, content_len);
    return ERR_OK;
}

static err_t cb_altcp_recv_fn(void *arg, struct altcp_pcb *conn, struct pbuf *p, err_t err)
{
    //printf("[HTTPC] Received %u Bytes\r\n", p->tot_len);
    static int count = 0;

    puts(".");
    if (0 == ((count++) & 0x3F)) {
        puts("\r\n");
    }
    altcp_recved(conn, p->tot_len);
    pbuf_free(p);

    return ERR_OK;
}

static void cmd_httpc_test(char *buf, int len, int argc, char **argv)
{
    static httpc_connection_t settings;
    static httpc_state_t *req;

    if (req) {
        printf("[CLI] req is on-going...\r\n");
        return;
    }
    memset(&settings, 0, sizeof(settings));
    settings.use_proxy = 0;
    settings.result_fn = cb_httpc_result;
    settings.headers_done_fn = cb_httpc_headers_done_fn;

    httpc_get_file_dns(
            "nf.cr.dandanman.com",
            80,
            "/ddm/ContentResource/music/204.mp3",
            &settings,
            cb_altcp_recv_fn,
            &req,
            &req
   );
}

static void cmd_stack_wifi(char *buf, int len, int argc, char **argv)
{
    /*wifi fw stack and thread stuff*/
    static uint8_t stack_wifi_init  = 0;


    if (1 == stack_wifi_init) {
        puts("Wi-Fi Stack Started already!!!\r\n");
        return;
    }
    stack_wifi_init = 1;

    hal_wifi_start_firmware_task();
    /*Trigger to start Wi-Fi*/
    aos_post_event(EV_WIFI, CODE_WIFI_ON_INIT_DONE, 0);

}

#ifndef CONFIG_BT_TL
static void cmd_stack_ble(char *buf, int len, int argc, char **argv)
{
    ble_stack_start();
}
#endif

#if defined(CFG_BLE_PDS)
static void cmd_start_pds(char *buf, int len, int argc, char **argv)
{
    if(argc != 2)
    {
        printf("Invalid params\r\n");
        return;
    }
    get_uint8_from_string(&argv[1], (uint8_t *)&pds_start);
    if (pds_start == 1)
    {
        hal_pds_init();
    }
}
#endif

static void cmd_hbn_enter(char *buf, int len, int argc, char **argv)
{
    uint32_t time;
    if (argc != 2) {
        printf("Please Input Parameter!\r\n");
        return;
    } else {
        time = (uint32_t)atoi(argv[1]);
        printf("time:%u\r\n", time);
        hal_hbn_enter(time);
    }
}

static void cmd_logen(char *buf, int len, int argc, char **argv)
{
    bl_sys_logall_enable();
}

static void cmd_logdis(char *buf, int len, int argc, char **argv)
{
    bl_sys_logall_disable();
}

static void cmd_load0w(char *buf, int len, int argc, char **argv)
{
    volatile uint32_t v = 0;

    /* backtrace */
    v = *(volatile uint32_t *)0;
}

typedef enum {
    TEST_OP_GET32 = 0,
    TEST_OP_GET16,
    TEST_OP_SET32 = 256,
    TEST_OP_SET16,
    TEST_OP_MAX = 0x7FFFFFFF
} test_op_t;
static __attribute__ ((noinline)) uint32_t misaligned_acc_test(void *ptr, test_op_t op, uint32_t v)
{
    uint32_t res = 0;

    switch (op) {
        case TEST_OP_GET32:
            res = *(volatile uint32_t *)ptr;
            break;
        case TEST_OP_GET16:
            res = *(volatile uint16_t *)ptr;
            break;
        case TEST_OP_SET32:
            *(volatile uint32_t *)ptr = v;
            break;
        case TEST_OP_SET16:
            *(volatile uint16_t *)ptr = v;
            break;
        default:
            break;
    }

    return res;
}

void test_align(uint32_t buf)
{
    volatile uint32_t testv[4] = {0};
    uint32_t t1 = 0;
    uint32_t t2 = 0;
    uint32_t t3 = 0;
    uint32_t i = 0;
    volatile uint32_t reg = buf;

    portDISABLE_INTERRUPTS();

    /* test get 32 */
    __asm volatile ("nop":::"memory");
    t1 = *(volatile uint32_t*)0x4000A52C;
    // 3*n + 5
    testv[0] = *(volatile uint32_t *)(reg + 0 * 8 + 1);
    t2 = *(volatile uint32_t*)0x4000A52C;
    // 3*n + 1
    testv[1] = *(volatile uint32_t *)(reg + 1 * 8 + 0);
    t3 = *(volatile uint32_t*)0x4000A52C;
    log_info("testv[0] = %08lx, testv[1] = %08lx\r\n", testv[0], testv[1]);
    log_info("time_us = %ld & %ld ---> %d\r\n", (t2 - t1), (t3 - t2), (t2 - t1)/(t3 - t2));

    /* test get 16 */
    __asm volatile ("nop":::"memory");
    t1 = bl_timer_now_us();
    for (i = 0; i < 1 * 1000 * 1000; i++) {
        testv[0] = misaligned_acc_test((void *)(reg + 2 * 8 + 1), TEST_OP_GET16, 0);
    }
    t2 = bl_timer_now_us();
    for (i = 0; i < 1 * 1000 * 1000; i++) {
        testv[1] = misaligned_acc_test((void *)(reg + 3 * 8 + 0), TEST_OP_GET16, 0);
    }
    t3 = bl_timer_now_us();
    log_info("testv[0] = %08lx, testv[1] = %08lx\r\n", testv[0], testv[1]);
    log_info("time_us = %ld & %ld ---> %d\r\n", (t2 - t1), (t3 - t2), (t2 - t1)/(t3 - t2));

    /* test set 32 */
    __asm volatile ("nop":::"memory");
    t1 = bl_timer_now_us();
    for (i = 0; i < 1 * 1000 * 1000; i++) {
        misaligned_acc_test((void *)(reg + 4 * 8 + 1), TEST_OP_SET32, 0x44332211);
    }
    t2 = bl_timer_now_us();
    for (i = 0; i < 1 * 1000 * 1000; i++) {
        misaligned_acc_test((void *)(reg + 5 * 8 + 0), TEST_OP_SET32, 0x44332211);
    }
    t3 = bl_timer_now_us();
    log_info("time_us = %ld & %ld ---> %d\r\n", (t2 - t1), (t3 - t2), (t2 - t1)/(t3 - t2));

    /* test set 16 */
    __asm volatile ("nop":::"memory");
    t1 = bl_timer_now_us();
    for (i = 0; i < 1 * 1000 * 1000; i++) {
        misaligned_acc_test((void *)(reg + 6 * 8 + 1), TEST_OP_SET16, 0x6655);
    }
    t2 = bl_timer_now_us();
    for (i = 0; i < 1 * 1000 * 1000; i++) {
        misaligned_acc_test((void *)(reg + 7 * 8 + 0), TEST_OP_SET16, 0x6655);
    }
    t3 = bl_timer_now_us();
    log_info("time_us = %ld & %ld ---> %d\r\n", (t2 - t1), (t3 - t2), (t2 - t1)/(t3 - t2));

    portENABLE_INTERRUPTS();
}

void test_misaligned_access(void) __attribute__((optimize("O0")));
void test_misaligned_access(void)// __attribute__((optimize("O0")))
{
#define TEST_V_LEN         (32)
    __attribute__ ((aligned(16))) volatile unsigned char test_vector[TEST_V_LEN] = {0};
    int i = 0;
    volatile uint32_t v = 0;
    uint32_t addr = (uint32_t)test_vector;
    volatile char *pb = (volatile char *)test_vector;
    register float a asm("fa0") = 0.0f;
    register float b asm("fa1") = 0.5f;

    for (i = 0; i < TEST_V_LEN; i ++)
        test_vector[i] = i;

    addr += 1; // offset 1
    __asm volatile ("nop");
    v = *(volatile uint16_t *)(addr); // 0x0201
    __asm volatile ("nop");
    printf("%s: v=%8lx, should be 0x0201\r\n", __func__, v);
    __asm volatile ("nop");
    *(volatile uint16_t *)(addr) = 0x5aa5;
    __asm volatile ("nop");
    __asm volatile ("nop");
    v = *(volatile uint16_t *)(addr); // 0x5aa5
    __asm volatile ("nop");
    printf("%s: v=%8lx, should be 0x5aa5\r\n", __func__, v);

    addr += 4; // offset 5
    __asm volatile ("nop");
    v = *(volatile uint32_t *)(addr); //0x08070605
    __asm volatile ("nop");
    printf("%s: v=%8lx, should be 0x08070605\r\n", __func__, v);
    __asm volatile ("nop");
    *(volatile uint32_t *)(addr) = 0xa5aa55a5;
    __asm volatile ("nop");
    __asm volatile ("nop");
    v = *(volatile uint32_t *)(addr); // 0xa5aa55a5
    __asm volatile ("nop");
    printf("%s: v=%8lx, should be 0xa5aa55a5\r\n", __func__, v);

    pb[0x11] = 0x00;
    pb[0x12] = 0x00;
    pb[0x13] = 0xc0;
    pb[0x14] = 0x3f;

    addr += 12; // offset 0x11
    __asm volatile ("nop");
    a = *(float *)(addr);
    __asm volatile ("nop");
    v = a * 4.0f; /* should be 6 */
    __asm volatile ("nop");
    __asm volatile ("nop");
    printf("%s: v=%8lx, should be 0x6\r\n", __func__, v);
    b = v / 12.0f;
    __asm volatile ("nop");
    addr += 4; // offset 0x15
    *(float *)(addr) = b;
    __asm volatile ("nop");
    v = *(volatile uint32_t *)(addr); // 0x3f000000
    __asm volatile ("nop");
    printf("%s: v=%8lx, should be 0x3f000000\r\n", __func__, v);
}

static void cmd_align(char *buf, int len, int argc, char **argv)
{
    char *testbuf = NULL;
    int i = 0;

    log_info("align test start.\r\n");
    test_misaligned_access();

    testbuf = aos_malloc(64);
    if (!testbuf) {
        log_error("mem error.\r\n");
    }
 
    memset(testbuf, 0xEE, 1024);
    for (i = 0; i < 32; i++) {
        testbuf[i] = i;
    }
    test_align((uint32_t)(testbuf));

    log_buf(testbuf, 64);
    aos_free(testbuf);

    log_info("align test end.\r\n");
}

void cmd_mfg(char *buf, int len, int argc, char **argv)
{
    bl_sys_mfg_config();
    hal_reboot();
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
        { "aws", "aws iot demo", cmd_aws},
        { "pka", "pka iot demo", cmd_pka},
        { "sha", "sha iot demo", cmd_sha},
        { "trng", "trng test", cmd_trng},
        { "aes", "trng test", cmd_aes},
        { "cks", "cks test", cmd_cks},
        { "dma", "dma test", cmd_dma},
        { "exception_load", "exception load test", cmd_exception_load},
        { "exception_l_illegal", "exception load test", cmd_exception_l_illegal},
        { "exception_store", "exception store test", cmd_exception_store},
        { "exception_inst_illegal", "exception illegal instruction", cmd_exception_illegal_ins},
        /*Stack Command*/
        { "stack_wifi", "Wi-Fi Stack", cmd_stack_wifi},
        #ifndef CONFIG_BT_TL
        { "stack_ble", "BLE Stack", cmd_stack_ble},
        #endif
        #if defined(CFG_BLE_PDS)
        { "pds_start", "enable or disable pds", cmd_start_pds},
        #endif
        /*TCP/IP network test*/
        {"http", "http client download test based on socket", http_test_cmd},
        {"httpc", "http client download test based on RAW TCP", cmd_httpc_test},
        {"hbnenter", "hbnenter", cmd_hbn_enter},
        {"logen", "logen", cmd_logen},
        {"logdis", "logdis", cmd_logdis},
        {"load0w", "load word from 0", cmd_load0w},
        {"aligntc", "align case test", cmd_align},
        {"mfg", "mfg", cmd_mfg},
};

static void _cli_init()
{
    /*Put CLI which needs to be init here*/
int codex_debug_cli_init(void);
    codex_debug_cli_init();
//    easyflash_cli_init();
    network_netutils_iperf_cli_register();
    network_netutils_tcpclinet_cli_register();
    network_netutils_tcpserver_cli_register();
    network_netutils_netstat_cli_register();
    network_netutils_ping_cli_register();
    sntp_cli_init();
    bl_sys_time_cli_init();
    bl_sys_ota_cli_init();
    blfdt_cli_init();
    wifi_mgmr_cli_init();
    bl_wdt_cli_init();
    bl_gpio_cli_init();
    looprt_test_cli_init();
    wifi_bt_coex_cli_init();
}

static void event_cb_key_event(input_event_t *event, void *private_data)
{
    switch (event->code) {
        case KEY_1:
        {
            printf("[KEY_1] [EVT] INIT DONE %lld\r\n", aos_now_ms());
            printf("short press \r\n");
        }
        break;
        case KEY_2:
        {
            printf("[KEY_2] [EVT] INIT DONE %lld\r\n", aos_now_ms());
            printf("long press \r\n");
        }
        break;
        case KEY_3:
        {
            printf("[KEY_3] [EVT] INIT DONE %lld\r\n", aos_now_ms());
            printf("longlong press \r\n");
        }
        break;
        default:
        {
            printf("[KEY] [EVT] Unknown code %u, %lld\r\n", event->code, aos_now_ms());
            /*nothing*/
        }
    }
}

#if defined(CONFIG_BT_TL)
extern void ble_uart_init(uint8_t uartid);
#endif

static void proc_main_entry(void *pvParameters)
{
#if 0
    if (0 == get_dts_addr("gpio", &fdt, &offset)) {
        hal_gpio_init_from_dts(fdt, offset);
        fdt_button_module_init((const void *)fdt, (int)offset);
    }
#endif
    _cli_init();

    aos_register_event_filter(EV_WIFI, event_cb_wifi_event, NULL);
    aos_register_event_filter(EV_KEY, event_cb_key_event, NULL);

    #if defined(CONFIG_BT_TL)
    //uart's pinmux has been configured in vfs_uart_init(load uart1's pin info from devicetree)
    ble_uart_init(1);
    ble_controller_init(configMAX_PRIORITIES - 1);
    #endif

    #if defined(CONFIG_AUTO_PTS)
    pts_uart_init(1,115200,8,1,0,0);
    // Initialize BLE controller
    ble_controller_init(configMAX_PRIORITIES - 1);
    extern int hci_driver_init(void);
    // Initialize BLE Host stack
    hci_driver_init();

    tester_send(BTP_SERVICE_ID_CORE, CORE_EV_IUT_READY, BTP_INDEX_NONE,
		    NULL, 0);
    #endif

    vTaskDelete(NULL);
}

#if defined(CFG_BLE_PDS)
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    //static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];
    static StackType_t uxIdleTaskStack[512];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    //*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE; 
    *pulIdleTaskStackSize = 512;//size 512 words is For ble pds mode, otherwise stack overflow of idle task will happen.
}
#endif

static void system_thread_init()
{
    /*nothing here*/
}

/* init adc for tsen*/
#ifdef CONF_ADC_ENABLE_TSEN
static hosal_adc_dev_t adc0;

static void adc_tsen_init()
{
    int ret = -1;

    adc0.port = 0;
    adc0.config.sampling_freq = 300;
    adc0.config.pin = 4;
    adc0.config.mode = 0;

    ret = hosal_adc_init(&adc0);
    if (ret) {
        log_error("adc init error!\r\n");
        return;
    }
}
#endif

void rijndael_aes_test(void);

void main()
{
    bl_sys_init();

    system_thread_init();
    
#ifdef CONF_ADC_ENABLE_TSEN
    adc_tsen_init();
#endif

    puts("[OS] Starting proc_hellow_entry task...\r\n");
    xTaskCreate(proc_hellow_entry, (char*)"hellow", 512, NULL, 15, NULL);
    puts("[OS] Starting aos_loop_proc task...\r\n");
    xTaskCreate(proc_main_entry, (char*)"main_entry", 1024, NULL, 15, NULL);
    puts("[OS] Starting TCP/IP Stack...\r\n");
    tcpip_init(NULL, NULL);

    //rijndael_aes_test();
#if defined(CONFIG_AUTO_PTS)
    tester_init();
#endif
}
