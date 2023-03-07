/** @file main.c
 *
 *  @brief main file
 *
 *  Copyright 2008-2020 NXP
 *
 *  NXP CONFIDENTIAL
 *  The source code contained or described herein and all documents related to
 *  the source code ("Materials") are owned by NXP, its
 *  suppliers and/or its licensors. Title to the Materials remains with NXP,
 *  its suppliers and/or its licensors. The Materials contain
 *  trade secrets and proprietary and confidential information of NXP, its
 *  suppliers and/or its licensors. The Materials are protected by worldwide copyright
 *  and trade secret laws and treaty provisions. No part of the Materials may be
 *  used, copied, reproduced, modified, published, uploaded, posted,
 *  transmitted, distributed, or disclosed in any way without NXP's prior
 *  express written permission.
 *
 *  No license under any patent, copyright, trade secret or other intellectual
 *  property right is granted to or conferred upon you by disclosure or delivery
 *  of the Materials, either expressly, by implication, inducement, estoppel or
 *  otherwise. Any license under such intellectual property rights must be
 *  express and approved by NXP in writing.
 *
 */

///////////////////////////////////////////////////////////////////////////////
//  Includes
///////////////////////////////////////////////////////////////////////////////

// SDK Included Files
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "mflash_drv.h"

#include "wlan.h"
#include "wifi.h"
#include "wm_net.h"
#include <wm_os.h>
#include "dhcp-server.h"
#include "cli.h"
#include "ping.h"
#include "iperf.h"
#include "partition.h"
#include "boot_flags.h"
#include "network_flash_storage.h"
#include "lpm.h"

#include "fsl_sdmmc_host.h"
#include "fsl_aes.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define APP_AES AES
#define CONNECTION_INFO_FILENAME "connection_info.dat"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void saveProfile(int argc, char **argv);
static void loadProfile(int argc, char **argv);
static void resetProfile(int argc, char **argv);

static void wlanIeeePowerSave(int argc, char **argv);
static void wlanDeepSleep(int argc, char **argv);
static void mcuPowerMode(int argc, char **argv);

/*******************************************************************************
 * Variables
 ******************************************************************************/
static SemaphoreHandle_t aesLock;

static struct wlan_network sta_network;
static struct wlan_network uap_network;

static struct cli_command saveload[] = {
    {"save-profile", "<profile_name>", saveProfile},
    {"load-profile", NULL, loadProfile},
    {"reset-profile", NULL, resetProfile},
};

static struct cli_command wlanPower[] = {
    {"wlan-ieee-power-save", "<on/off> <wakeup condition>", wlanIeeePowerSave},
    {"wlan-deepsleep", "<on/off>", wlanDeepSleep},
};

static struct cli_command mcuPower[] = {
    {"mcu-power-mode", "<pm0/pm1/pm2/pm4> [<pm2_io_exclude_mask>]", mcuPowerMode},
};

const int TASK_MAIN_PRIO       = OS_PRIO_3;
const int TASK_MAIN_STACK_SIZE = 800;

portSTACK_TYPE *task_main_stack = NULL;
TaskHandle_t task_main_task_handler;

/*******************************************************************************
 * Code
 ******************************************************************************/
static void printSeparator(void)
{
    PRINTF("----------------------------------------\r\n");
}

static status_t APP_AES_Lock(void)
{
    if (pdTRUE == xSemaphoreTakeRecursive(aesLock, portMAX_DELAY))
    {
        return kStatus_Success;
    }
    else
    {
        return kStatus_Fail;
    }
}

static void APP_AES_Unlock(void)
{
    xSemaphoreGiveRecursive(aesLock);
}

static void saveProfile(int argc, char **argv)
{
    int ret;
    struct wlan_network network;

    if (argc < 2)
    {
        PRINTF("Usage: %s <profile_name>\r\n", argv[0]);
        PRINTF("Error: specify network to save\r\n");
        return;
    }

    ret = wlan_get_network_byname(argv[1], &network);
    if (ret != WM_SUCCESS)
    {
        PRINTF("Error: network not found\r\n");
    }
    else
    {
        ret = save_wifi_network(CONNECTION_INFO_FILENAME, (uint8_t *)&network, sizeof(network));
        if (ret != WM_SUCCESS)
        {
            PRINTF("Error: write network to flash failed\r\n");
        }
    }
}

static void loadProfile(int argc, char **argv)
{
    int ret;
    struct wlan_network network;
    uint32_t len = sizeof(network);

    ret = get_saved_wifi_network(CONNECTION_INFO_FILENAME, (uint8_t *)&network, &len);
    if (ret != WM_SUCCESS || len != sizeof(network))
    {
        PRINTF("Error: No network saved\r\n");
    }
    else
    {
        ret = wlan_add_network(&network);
        if (ret != WM_SUCCESS)
        {
            PRINTF("Error: network data corrupted or network already added\r\n");
        }
    }
}

static void resetProfile(int argc, char **argv)
{
    int ret;

    ret = reset_saved_wifi_network(CONNECTION_INFO_FILENAME);
    if (ret != WM_SUCCESS)
    {
        PRINTF("Error: Reset profile failed\r\n");
    }
}

static void wlanIeeePowerSave(int argc, char **argv)
{
    bool on  = false;
    bool off = false;
    uint32_t cond;
    int ret;

    if (argc >= 2)
    {
        on  = (strcmp(argv[1], "on") == 0);
        off = (strcmp(argv[1], "off") == 0);
    }
    if ((argc < 2) || (!on && !off) || (on && argc < 3))
    {
        PRINTF("Usage: %s <on/off> [<wakeup condition>]\r\n", argv[0]);
        PRINTF("       wakeup condictions needed by \"on\" command:\r\n");
        PRINTF("       bit0=1: broadcast data\r\n");
        PRINTF("       bit1=1: unicast data\r\n");
        PRINTF("       bit2=1: mac events\r\n");
        PRINTF("       bit3=1: multicast data\r\n");
        PRINTF("       bit4=1: arp broadcast data\r\n");
        PRINTF("       bit6=1: management frame\r\n");
        return;
    }

    if (on)
    {
        cond = strtoul(argv[2], NULL, 0);
        ret  = wlan_ieeeps_on(cond);
    }
    else
    {
        ret = wlan_ieeeps_off();
    }

    if (ret != WM_SUCCESS)
    {
        PRINTF("Cannot request IEEE power save mode change!\r\n");
    }
    else
    {
        PRINTF("IEEE power save mode change requested!\r\n");
    }
}

static void wlanDeepSleep(int argc, char **argv)
{
    bool on;
    int ret;

    if ((argc < 2) || ((strcmp(argv[1], "on") != 0) && (strcmp(argv[1], "off") != 0)))
    {
        PRINTF("Usage: %s <on/off>\r\n", argv[0]);
        PRINTF("Error: specify deep sleep on or off.\r\n");
        return;
    }

    on = (strcmp(argv[1], "on") == 0);
    if (on)
    {
        ret = wlan_deepsleepps_on();
    }
    else
    {
        ret = wlan_deepsleepps_off();
    }

    if (ret != WM_SUCCESS)
    {
        PRINTF("Cannot request deep sleep mode change!\r\n");
    }
    else
    {
        PRINTF("Deep sleep mode change requested!\r\n");
    }
}

static void mcuPowerMode(int argc, char **argv)
{
    uint32_t excludeIo = 0U;

    if ((argc < 2) || (strlen(argv[1]) != 3) || (argv[1][0] != 'p') || (argv[1][1] != 'm') || (argv[1][2] < '0') ||
        (argv[1][2] > '4') || (argv[1][2] == '3'))
    {
        PRINTF("Usage: %s <pm0/pm1/pm2/pm4> [<pm2_io_exclude_mask>]\r\n", argv[0]);
        PRINTF("       pm2_io_exclude_mask: bitmask of io domains to keep on in PM2.\r\n");
        PRINTF("                            e.g. 0x5 means VDDIO0 and VDDIO2 will not be powered off in PM2\r\n");
        PRINTF("Error: specify power mode to enter.\r\n");
        return;
    }

    if (argv[1][2] - '0' == 2U)
    {
        if (argc < 3)
        {
            PRINTF("Error: PM2 need 3rd parameter.\r\n");
            return;
        }
        else
        {
            excludeIo = strtoul(argv[2], NULL, 0);
        }
    }

    LPM_SetPowerMode(argv[1][2] - '0', excludeIo);
}

/* Function called when system executes idle thread */
void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
    uint32_t primask;
    enum wlan_connection_state staState = WLAN_DISCONNECTED;
    enum wlan_connection_state uapState = WLAN_UAP_STOPPED;
    enum wlan_ps_mode psMode            = WLAN_ACTIVE;
    bool allowDeepSleep                 = false;
    uint32_t result                     = 0U;
    uint32_t curMode;

    curMode = LPM_GetPowerMode();
    if (curMode > 0U)
    {
        PRINTF("Enter idle for %d ticks\r\n", xExpectedIdleTime);
    }

    primask = DisableGlobalIRQ();

    /* Only when no context switch is pending and no task is waiting for the scheduler
     * to be unsuspended then enter low power entry.
     */
    if (eTaskConfirmSleepModeStatus() != eAbortSleep)
    {
        wlan_get_connection_state(&staState);
        wlan_get_uap_connection_state(&uapState);
        wlan_get_ps_mode(&psMode);

        if ((psMode == WLAN_DEEP_SLEEP) || ((staState == WLAN_DISCONNECTED) && (uapState == WLAN_UAP_STOPPED)))
        {
            allowDeepSleep = true;
        }
        result = LPM_WaitForInterrupt((uint64_t)1000 * xExpectedIdleTime / configTICK_RATE_HZ, allowDeepSleep);
    }

    EnableGlobalIRQ(primask);

    if (curMode > 0U)
    {
        PRINTF("Exit from power mode %d\r\n", result);
    }
}

static void mcuInitPower(void)
{
    lpm_config_t config = {
        /* System PM2/PM3 less than 50 ms will be skipped. */
        .threshold = 50U,
        /* SFLL config and  RC32M setup takes approx 14 ms. */
        .latency          = 15U,
        .enableWakeupPin0 = true,
        .enableWakeupPin1 = true,
        .handler          = NULL,
    };

    LPM_Init(&config);
}

/* Callback Function passed to WLAN Connection Manager. The callback function
 * gets called when there are WLAN Events that need to be handled by the
 * application.
 */
int wlan_event_callback(enum wlan_event_reason reason, void *data)
{
    int ret;
    struct wlan_ip_config addr;
    char ip[16];
    static int auth_fail = 0;

    printSeparator();
    PRINTF("app_cb: WLAN: received event %d\r\n", reason);
    printSeparator();

    switch (reason)
    {
        case WLAN_REASON_INITIALIZED:
            PRINTF("app_cb: WLAN initialized\r\n");
            printSeparator();

            ret = wlan_basic_cli_init();
            if (ret != WM_SUCCESS)
            {
                PRINTF("Failed to initialize BASIC WLAN CLIs\r\n");
                return 0;
            }

            ret = wlan_cli_init();
            if (ret != WM_SUCCESS)
            {
                PRINTF("Failed to initialize WLAN CLIs\r\n");
                return 0;
            }
            PRINTF("WLAN CLIs are initialized\r\n");
            printSeparator();

            ret = ping_cli_init();
            if (ret != WM_SUCCESS)
            {
                PRINTF("Failed to initialize PING CLI\r\n");
                return 0;
            }

            ret = iperf_cli_init();
            if (ret != WM_SUCCESS)
            {
                PRINTF("Failed to initialize IPERF CLI\r\n");
                return 0;
            }

            ret = dhcpd_cli_init();
            if (ret != WM_SUCCESS)
            {
                PRINTF("Failed to initialize DHCP Server CLI\r\n");
                return 0;
            }

            if (cli_register_commands(saveload, sizeof(saveload) / sizeof(struct cli_command)))
            {
                return -WM_FAIL;
            }

            if (cli_register_commands(wlanPower, sizeof(wlanPower) / sizeof(struct cli_command)))
            {
                return -WM_FAIL;
            }

            if (cli_register_commands(mcuPower, sizeof(mcuPower) / sizeof(struct cli_command)))
            {
                return -WM_FAIL;
            }

            PRINTF("CLIs Available:\r\n");
            printSeparator();
            help_command(0, NULL);
            printSeparator();
            break;
        case WLAN_REASON_INITIALIZATION_FAILED:
            PRINTF("app_cb: WLAN: initialization failed\r\n");
            break;
        case WLAN_REASON_SUCCESS:
            PRINTF("app_cb: WLAN: connected to network\r\n");
            ret = wlan_get_address(&addr);
            if (ret != WM_SUCCESS)
            {
                PRINTF("failed to get IP address\r\n");
                return 0;
            }

            net_inet_ntoa(addr.ipv4.address, ip);

            ret = wlan_get_current_network(&sta_network);
            if (ret != WM_SUCCESS)
            {
                PRINTF("Failed to get External AP network\r\n");
                return 0;
            }

            PRINTF("Connected to following BSS:\r\n");
            PRINTF("SSID = [%s], IP = [%s]\r\n", sta_network.ssid, ip);
            auth_fail = 0;
            break;
        case WLAN_REASON_CONNECT_FAILED:
            PRINTF("app_cb: WLAN: connect failed\r\n");
            break;
        case WLAN_REASON_NETWORK_NOT_FOUND:
            PRINTF("app_cb: WLAN: network not found\r\n");
            break;
        case WLAN_REASON_NETWORK_AUTH_FAILED:
            PRINTF("app_cb: WLAN: network authentication failed\r\n");
            auth_fail++;
            if (auth_fail >= 3)
            {
                PRINTF("Authentication Failed. Disconnecting ... \r\n");
                wlan_disconnect();
                auth_fail = 0;
            }
            break;
        case WLAN_REASON_ADDRESS_SUCCESS:
            PRINTF("network mgr: DHCP new lease\r\n");
            break;
        case WLAN_REASON_ADDRESS_FAILED:
            PRINTF("app_cb: failed to obtain an IP address\r\n");
            break;
        case WLAN_REASON_USER_DISCONNECT:
            PRINTF("app_cb: disconnected\r\n");
            auth_fail = 0;
            break;
        case WLAN_REASON_LINK_LOST:
            PRINTF("app_cb: WLAN: link lost\r\n");
            break;
        case WLAN_REASON_CHAN_SWITCH:
            PRINTF("app_cb: WLAN: channel switch\r\n");
            break;
        case WLAN_REASON_UAP_SUCCESS:
            PRINTF("app_cb: WLAN: UAP Started\r\n");
            ret = wlan_get_current_uap_network(&uap_network);

            if (ret != WM_SUCCESS)
            {
                PRINTF("Failed to get Soft AP network\r\n");
                return 0;
            }

            printSeparator();
            PRINTF("Soft AP \"%s\" started successfully\r\n", uap_network.ssid);
            printSeparator();
            if (dhcp_server_start(net_get_uap_handle()))
                PRINTF("Error in starting dhcp server\r\n");

            PRINTF("DHCP Server started successfully\r\n");
            printSeparator();
            break;
        case WLAN_REASON_UAP_CLIENT_ASSOC:
            PRINTF("app_cb: WLAN: UAP a Client Associated\r\n");
            printSeparator();
            PRINTF("Client => ");
            print_mac((const char *)data);
            PRINTF("Associated with Soft AP\r\n");
            printSeparator();
            break;
        case WLAN_REASON_UAP_CLIENT_DISSOC:
            PRINTF("app_cb: WLAN: UAP a Client Dissociated\r\n");
            printSeparator();
            PRINTF("Client => ");
            print_mac((const char *)data);
            PRINTF("Dis-Associated from Soft AP\r\n");
            printSeparator();
            break;
        case WLAN_REASON_UAP_STOPPED:
            PRINTF("app_cb: WLAN: UAP Stopped\r\n");
            printSeparator();
            PRINTF("Soft AP \"%s\" stopped successfully\r\n", uap_network.ssid);
            printSeparator();

            dhcp_server_stop();

            PRINTF("DHCP Server stopped successfully\r\n");
            printSeparator();
            break;
        case WLAN_REASON_PS_ENTER:
            PRINTF("app_cb: WLAN: PS_ENTER\r\n");
            break;
        case WLAN_REASON_PS_EXIT:
            PRINTF("app_cb: WLAN: PS EXIT\r\n");
            break;
        default:
            PRINTF("app_cb: WLAN: Unknown Event: %d\r\n", reason);
    }
    return 0;
}

void task_main(void *param)
{
    int32_t result = 0;
    struct partition_entry *p;
    short history = 0;
    struct partition_entry *f1, *f2;
    flash_desc_t fl;
    uint32_t *wififw;
#ifdef CONFIG_USE_PSM
    struct partition_entry *psm;
#endif

    mcuInitPower();

    boot_init();

    mflash_drv_init();

    PRINTF("Initialize CLI\r\n");
    printSeparator();

    result = cli_init();
    if (WM_SUCCESS != result)
    {
        assert(false);
    }

    PRINTF("Initialize WLAN Driver\r\n");
    printSeparator();

    result = part_init();
    if (WM_SUCCESS != result)
    {
        assert(false);
    }

#ifdef CONFIG_USE_PSM
    psm = part_get_layout_by_id(FC_COMP_PSM, NULL);
    part_to_flash_desc(psm, &fl);
#else
    fl.fl_dev   = 0U;
    fl.fl_start = MFLASH_FILE_BASEADDR;
    fl.fl_size  = MFLASH_FILE_SIZE;
#endif

    init_flash_storage(CONNECTION_INFO_FILENAME, &fl);

    f1 = part_get_layout_by_id(FC_COMP_WLAN_FW, &history);
    f2 = part_get_layout_by_id(FC_COMP_WLAN_FW, &history);

    if (f1 && f2)
    {
        p = part_get_active_partition(f1, f2);
    }
    else if (!f1 && f2)
    {
        p = f2;
    }
    else if (!f2 && f1)
    {
        p = f1;
    }
    else
    {
        PRINTF(" Wi-Fi Firmware not detected\r\n");
        p = NULL;
    }

    if (p != NULL)
    {
        part_to_flash_desc(p, &fl);

        wififw = (uint32_t *)mflash_drv_phys2log(fl.fl_start, fl.fl_size);
        assert(wififw != NULL);
        /* First word in WIFI firmware is magic number. */
        assert(*wififw == (('W' << 0) | ('L' << 8) | ('F' << 16) | ('W' << 24)));

        /* Initialize WIFI Driver */
        /* Second word in WIFI firmware is WIFI firmware length in bytes. */
        /* Real WIFI binary starts from 3rd word. */
        result = wlan_init((const uint8_t *)(wififw + 2U), *(wififw + 1U));
        if (WM_SUCCESS != result)
        {
            assert(false);
        }

        result = wlan_start(wlan_event_callback);
        if (WM_SUCCESS != result)
        {
            assert(false);
        }
    }

    while (1)
    {
        /* wait for interface up */
        os_thread_sleep(os_msec_to_ticks(5000));
    }
}

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

int main(void)
{
    BaseType_t result = 0;
    (void)result;

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    CLOCK_EnableXtal32K(kCLOCK_Osc32k_External);
    CLOCK_AttachClk(kXTAL32K_to_RTC);

    printSeparator();
    PRINTF("wifi cli demo\r\n");
    printSeparator();

    aesLock = xSemaphoreCreateRecursiveMutex();
    assert(aesLock != NULL);

    AES_Init(APP_AES);
    AES_SetLockFunc(APP_AES_Lock, APP_AES_Unlock);

    result =
        xTaskCreate(task_main, "main", TASK_MAIN_STACK_SIZE, task_main_stack, TASK_MAIN_PRIO, &task_main_task_handler);
    assert(pdPASS == result);

    vTaskStartScheduler();
    for (;;)
        ;
}
