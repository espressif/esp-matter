#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <stdio.h>
#include <vfs.h>
#include <aos/kernel.h>
#include <aos/yloop.h>
#include <event_device.h>
#include <cli.h>
#include <lwip/apps/mdns.h>
#include <lwip/tcpip.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <lwip/tcp.h>
#include <lwip/err.h>
#include <lwip/netifapi.h>
#include <http_client.h>
#include <netutils/netutils.h>
#ifdef EASYFLASH_ENABLE
#include <easyflash.h>
#endif
#include <bl_uart.h>
#include <bl_chip.h>
#include <bl_wifi.h>
#include <bl_sec.h>
#include <bl_cks.h>
#include <bl_irq.h>
#include <bl_dma.h>
#include <bl_gpio_cli.h>
#include <bl_wdt_cli.h>
#include <hal_sys.h>
#include <hal_gpio.h>
#include <hal_boot2.h>
#include <hal_board.h>
#include <looprt.h>
#include <loopset.h>
#include <bl_sys_time.h>
#include <bl_romfs.h>
#include <fdt.h>
#include <wifi_mgmr_ext.h>
#include <libfdt.h>
#include <blog.h>
#include <mdns_server.h>

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

extern void ble_stack_start(void);

static wifi_conf_t conf =
{
    .country_code = "CN",
};
static wifi_interface_t wifi_interface;

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
    char ssid[33], password[66], *val;
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

    val = ef_get_env(WIFI_AP_PSM_INFO_SSID);
    if (val) {
        /*We believe that when ssid is set, wifi_confi is OK*/
        strncpy(ssid, val, sizeof(ssid) - 1);
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
    val = ef_get_env(WIFI_AP_PSM_INFO_PASSWORD);
    if (val) {
        strncpy(password, val, sizeof(password) - 1);
    }
    val = ef_get_env(WIFI_AP_PSM_INFO_PMK);
    if (val) {
        strncpy(pmk, val, sizeof(pmk) - 1);
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
        ef_set_env(WIFI_AP_PSM_INFO_PMK, pmk);
        ef_save_env();
    }
    val = ef_get_env(WIFI_AP_PSM_INFO_CHANNEL);
    if (val) {
        strncpy(chan, val, sizeof(chan) - 1);
        printf("connect wifi channel = %s\r\n", chan);
        _chan_str_to_hex(&band, &freq, chan);
    }
    val = ef_get_env(WIFI_AP_PSM_INFO_BSSID);
    if (val) {
        strncpy(bssid, val, sizeof(bssid) - 1);
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

static void wifi_sta_connect(char *ssid, char *password)
{
    wifi_interface_t wifi_interface;

    wifi_interface = wifi_mgmr_sta_enable();
    wifi_mgmr_sta_connect(wifi_interface, ssid, password, NULL, NULL, 0, 0);
}

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
            printf("[APP] [EVT] SCAN Done %lld\r\n", aos_now_ms());
            wifi_mgmr_cli_scanlist();
        }
        break;
        case CODE_WIFI_ON_DISCONNECT:
        {
            printf("[APP] [EVT] disconnect %lld\r\n", aos_now_ms());
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
            printf("connecting to %s:%s...\r\n", ssid, password);
            wifi_sta_connect(ssid, password);
        }
        break;
        case CODE_WIFI_ON_PROV_DISCONNECT:
        {
            printf("[APP] [EVT] [PROV] [DISCONNECT] %lld\r\n", aos_now_ms());
        }
        break;
        default:
        {
            printf("[APP] [EVT] Unknown code %u, %lld\r\n", event->code, aos_now_ms());
            /*nothing*/
        }
    }
}

static void cmd_stack_wifi(char *buf, int len, int argc, char **argv)
{
    /*wifi fw stack and thread stuff*/
    static StackType_t wifi_fw_stack[1024];
    static StaticTask_t wifi_fw_task;
    static uint8_t stack_wifi_init  = 0;


    if (1 == stack_wifi_init) {
        puts("Wi-Fi Stack Started already!!!\r\n");
        return;
    }
    stack_wifi_init = 1;
    void wifi_main(void *param);
    xTaskCreateStatic(wifi_main, (char*)"fw", 1024, NULL, TASK_PRIORITY_FW, wifi_fw_stack, &wifi_fw_task);
    /*Trigger to start Wi-Fi*/
    aos_post_event(EV_WIFI, CODE_WIFI_ON_INIT_DONE, 0);

}

typedef struct mdns {
    struct netif *netif;
    int slot;  
} mdns_t;

static mdns_t mdns = {NULL, -1};

static err_t mdns_responder_start_netifapi_errt_fn(struct netif *netif)
{
    return mdns_responder_start(netif);
}

static err_t mdns_responder_stop_netifapi_errt_fn(struct netif *netif)
{
    return mdns_responder_stop(netif);
}

void mdns_task(void *pvParameters)
{
    int slot;
    uint32_t pre_ip, cur_ip;
    struct netif *netif;
    
    netif = wifi_mgmr_sta_netif_get();
    if (netif == NULL) {
        printf("find failed\r\n");
        goto _failed;
    }

    cur_ip = netif->ip_addr.addr;  
    pre_ip = cur_ip;

    slot = netifapi_netif_common(netif, NULL, mdns_responder_start_netifapi_errt_fn);
    if (slot < 0) {
        printf("start mdns failed\r\n");
        goto _failed;
    }

    mdns.netif = netif;
    mdns.slot = slot;
    while (1) {
        cur_ip = netif->ip_addr.addr;
        if (pre_ip != cur_ip) {
            netifapi_netif_common(netif, mdns_resp_announce, NULL);
            pre_ip = cur_ip;
        }
        vTaskDelay(1000);
    }
_failed:
    vTaskDelete(NULL);
}

static void cmd_mdns_cli(char *buf, int len, int argc, char **argv)
{
    static xTaskHandle xHandle;
    static int start_flag = 1;
    static int stop_flag = 0;
    int ret; 
    
    if (argc != 2) {
        printf("Please Input Parameter!\r\n");
        return;
    }

    if (0 == strcmp(argv[1], "start")) {
        if (1 == start_flag) {
            xTaskCreate(mdns_task, "mdns_task", 1024, NULL, 20, &xHandle);
            start_flag = 0;
            stop_flag = 1;
            printf("start mdns\r\n");
        } else {
            printf("mdns already start\r\n");
        }
    } else if (0 == strcmp(argv[1], "stop")) {
        if (1 == stop_flag) {
            ret = netifapi_netif_common(mdns.netif, NULL, mdns_responder_stop_netifapi_errt_fn);
            if (ret != 0) {
                printf("stop mdns failed\r\n");
                return;
            }
            vTaskDelete(xHandle);
            stop_flag = 0;
            start_flag = 1;
            printf("stop mdns\r\n");
        } else {
            printf("mdns already stop\r\n");
        }
    } else{
        printf("please input parameter 'start' or 'stop'\r\n");
    }
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
        /*Stack Command*/
        { "stack_wifi", "Wi-Fi Stack", cmd_stack_wifi},
        { "mdns", "test mdns", cmd_mdns_cli},
};

static void _cli_init()
{
    /*Put CLI which needs to be init here*/ 
    wifi_mgmr_cli_init();
}

static void proc_main_entry(void *pvParameters)
{

    _cli_init();
    aos_register_event_filter(EV_WIFI, event_cb_wifi_event, NULL);

    vTaskDelete(NULL);
}

void main()
{
    puts("[OS] proc_main_entry task...\r\n");
    xTaskCreate(proc_main_entry, (char*)"main_entry", 1024, NULL, 15, NULL);

    puts("[OS] Starting TCP/IP Stack...\r\n");
    tcpip_init(NULL, NULL);
}
