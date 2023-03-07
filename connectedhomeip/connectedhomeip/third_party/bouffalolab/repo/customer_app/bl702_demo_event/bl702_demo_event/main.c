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
#include <device/vfs_uart.h>
#include <aos/kernel.h>
#include <aos/yloop.h>
#include <event_device.h>
#include <cli.h>

#include <bl_sys.h>
#include <bl_chip.h>
#include <bl_wireless.h>
#include <bl_irq.h>
#include <bl_sec.h>
#include <bl_rtc.h>
#include <bl_uart.h>
#include <bl_gpio.h>
#include <bl_flash.h>
#include <bl_hbn.h>
#include <bl_timer.h>
#include <bl_wdt.h>
#include <hal_boot2.h>
#include <hal_board.h>
#include <hosal_uart.h>
#include <hosal_gpio.h>
#include <hal_gpio.h>
#include <hal_button.h>
#include <hal_hwtimer.h>
#include <hal_pds.h>
#include <hal_tcal.h>
#include <FreeRTOS.h>
#include <timers.h>

#ifdef CFG_ETHERNET_ENABLE
#include <lwip/netif.h>
#include <lwip/etharp.h>
#include <lwip/udp.h>
#include <lwip/ip.h>
#include <lwip/init.h>
#include <lwip/ip_addr.h>
#include <lwip/tcpip.h>
#include <lwip/dhcp.h>
#include <lwip/inet.h>
#include <lwip/sockets.h>
#include <lwip/netifapi.h>

#include <bl_sys_ota.h>
#include <bl_emac.h>
#include <bl702_glb.h>
#include <bl702_common.h>
#include <bflb_platform.h>
#include <eth_bd.h>
#include <netutils/netutils.h>

#include <netif/ethernet.h>
#endif /* CFG_ETHERNET_ENABLE */
#include <easyflash.h>
#include <libfdt.h>
#include <utils_log.h>
#include <blog.h>

#ifdef EASYFLASH_ENABLE
#include <easyflash.h>
#endif
#include <utils_string.h>
#if defined(CONFIG_AUTO_PTS)
#include "bttester.h"
#include "autopts_uart.h"
#endif

#if defined(CFG_BLE_ENABLE)
#include "bluetooth.h"
#include "ble_cli_cmds.h"
#include <hci_driver.h>
#include "ble_lib_api.h"

#if defined(CONFIG_BLE_TP_SERVER)
#include "ble_tp_svc.h"
#endif

#if defined(CONFIG_BT_MESH)
#include "mesh_cli_cmds.h"
#endif
#endif

#if defined(CFG_ZIGBEE_ENABLE)
#include "zb_common.h"
#if defined(CFG_ZIGBEE_CLI)
#include "zb_stack_cli.h"
#endif
#include "zigbee_app.h"
//#include "zb_bdb.h"
#endif
#if defined(CONFIG_ZIGBEE_PROV)
#include "blsync_ble_app.h"
#endif
#if defined(CFG_USE_PSRAM)
#include "bl_psram.h"
#endif /* CFG_USE_PSRAM */


#define PDS_WAKEUP_GPIO 17
#define HBN_WAKEUP_GPIO 9

#ifdef CFG_ETHERNET_ENABLE
//extern err_t ethernetif_init(struct netif *netif);
extern err_t eth_init(struct netif *netif);

#define ETH_USE_DHCP 1

#if ETH_USE_DHCP
static void netif_status_callback(struct netif *netif)
{
    if (netif->flags & NETIF_FLAG_UP) {
#if LWIP_IPV4
        if(!ip4_addr_isany(netif_ip4_addr(netif))){
            char addr[INET_ADDRSTRLEN];
            const ip4_addr_t* ipv4addr = netif_ip4_addr(netif);
            inet_ntop(AF_INET, ipv4addr, addr, sizeof(addr));
            printf("IP: %s\r\n", addr);

            const ip4_addr_t* ipv4mask = netif_ip4_netmask(netif);
            inet_ntop(AF_INET, ipv4mask, addr, sizeof(addr));
            printf("MASK: %s\r\n", addr);

            const ip4_addr_t* ipv4gw = netif_ip4_gw(netif);
            inet_ntop(AF_INET, ipv4gw, addr, sizeof(addr));
            printf("Gateway: %s\r\n", addr);
        }
#endif

#if LWIP_IPV6
        for (uint32_t i = 0; i < LWIP_IPV6_NUM_ADDRESSES; i ++ ) {
          if (!ip6_addr_isany(netif_ip6_addr(netif, i))
                && ip6_addr_ispreferred(netif_ip6_addr_state(netif, i))
                ) {
            const ip6_addr_t* ip6addr = netif_ip6_addr(netif, i);
            char addr[INET6_ADDRSTRLEN];

            if (ip6_addr_isany(ip6addr)) {
                continue;
            }
            inet_ntop(AF_INET6, ip6addr, addr, sizeof(addr));
            if(ip6_addr_islinklocal(netif_ip6_addr(netif, i))){
                printf("LOCAL IP6 addr %s\r\n", addr);
            }
            else{
                printf("GLOBAL IP6 addr %s\r\n", addr);
            }
          }
        }
#endif
    }
    else {
        printf("interface is down status.\n");
    }
}
static int app_eth_callback(eth_link_state val)
{
    switch(val){
    case ETH_INIT_STEP_LINKUP:{

    }break;
    case ETH_INIT_STEP_READY:{
        netif_set_default(&eth_mac);
        netif_set_up(&eth_mac);
        dhcp_start(&eth_mac);
        printf("start dhcp....\r\n");
    }break;
    case ETH_INIT_STEP_LINKDOWN:{

    }break;
    }
    return 0;
}

void lwip_init_netif(void)
{
    ip_addr_t ipaddr, netmask, gw;
    IP4_ADDR(&ipaddr, 0,0,0,0);
    IP4_ADDR(&netmask, 0,0,0,0);
    IP4_ADDR(&gw, 0,0,0,0);
    netif_add(&eth_mac, &ipaddr, &netmask, &gw, NULL, eth_init, ethernet_input);

    ethernet_init(app_eth_callback);
    /* Set callback to be called when interface is brought up/down or address is changed while up */
    netif_set_status_callback(&eth_mac, netif_status_callback);
}
#else
void lwip_init_netif(void)
{
    ip_addr_t ipaddr, netmask, gw;

    IP4_ADDR(&gw, 192,168,99,1);
    IP4_ADDR(&ipaddr, 192,168,99,150);
    IP4_ADDR(&netmask, 255,255,255,0);

    netif_add(&eth_mac, &ipaddr, &netmask, &gw, NULL, eth_init, ethernet_input);
    netif_set_default(&eth_mac);
    netif_set_up(&eth_mac);
}
#endif /* ETH_USE_DHCP */
#endif /* CFG_ETHERNET_ENABLE */

#if defined(CFG_USE_PSRAM)
extern uint8_t _heap3_start;
extern uint8_t _heap3_size; // @suppress("Type cannot be resolved")
static HeapRegion_t xHeapRegionsPsram[] =  
{
    { &_heap3_start, (unsigned int) &_heap3_size },
    { NULL, 0 }, /* Terminates the array. */
    { NULL, 0 } /* Terminates the array. */
};
#endif /* CFG_USE_PSRAM */

bool pds_start = false;
bool wfi_disable = false;
void bl702_low_power_config(void);
#if defined(CFG_ZIGBEE_PDS) || (CFG_BLE_PDS)
static void cmd_start_pds(char *buf, int len, int argc, char **argv)
{
    pds_start = true;
}
#endif

#if defined(CFG_ZIGBEE_HBN)
bool hbn_start = false;
static void cmd_start_hbn(char *buf, int len, int argc, char **argv)
{
    hbn_start = true;
}
#endif

static void cmd_lowpower_config(char *buf, int len, int argc, char **argv)
{
    bl702_low_power_config();
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

//uint32_t bl_timer_now_us(void){return 0;}
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

    testbuf = aos_malloc(1024);
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
#if defined(CONFIG_ZIGBEE_PROV)
static void cmd_blsync_blezb_start(void)
{
    blsync_ble_start();
}
#endif

static void cmd_wdt_set(char *buf, int len, int argc, char **argv)
{
    if(argc != 2){
        log_info("Number of parameters error.\r\n");
        return;
    }
    if(strcmp(argv[1], "enable") == 0){
        bl_wdt_init(4000);
    }
    else if(strcmp(argv[1], "disable") == 0){
        bl_wdt_disable();
    }
    else{
        log_info("Second parameter error.\r\n");
    }
}

static void cmd_wdt_rst_cnt_get(char *buf, int len, int argc, char **argv)
{
    //extern int bl_sys_wdt_rst_count_get();
    if(argc != 1){
        log_info("Number of parameters error.\r\n");
        return;
    }
    printf("wdt reset count %d\r\n", bl_sys_wdt_rst_count_get());
}

const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = { 
        #if defined(CFG_ZIGBEE_PDS) || (CFG_BLE_PDS)
        {"pds_start", "enable pds", cmd_start_pds},
        #endif
        {"lw_cfg", "lowpower configuration for active current test", cmd_lowpower_config},
        #if defined(CFG_ZIGBEE_HBN)
        {"hbn_start", "enable hbn", cmd_start_hbn},
        #endif
        {"aligntc", "align case test", cmd_align},
        #if defined(CONFIG_ZIGBEE_PROV)
        { "blsync_blezb_start", "start zigbee provisioning via ble", cmd_blsync_blezb_start},
        #endif
        { "wdt_set", "enable or disable wdt", cmd_wdt_set},
        { "wdt_rst_cnt_get", "get wdt rest count", cmd_wdt_rst_cnt_get},
};

void vApplicationMallocFailedHook(void)
{
    printf("Memory Allocate Failed. Current left size is %d bytes\r\n"
#if defined(CFG_USE_PSRAM)
        "Current psram left size is %d bytes\r\n"
#endif /*CFG_USE_PSRAM*/
        ,xPortGetFreeHeapSize()
#if defined(CFG_USE_PSRAM)
        ,xPortGetFreeHeapSizePsram()
#endif /*CFG_USE_PSRAM*/
    );
    while (1) {
        /*empty here*/
    }
}

void vApplicationIdleHook(void)
{
    bl_wdt_feed();
    bool bWFI_disable =  false;
    #if defined (CFG_BLE_PDS) && !defined(CFG_ZIGBEE_PDS)
    bWFI_disable = wfi_disable;
    #else
    bWFI_disable = pds_start;
    #endif
    if(!bWFI_disable){
        __asm volatile(
                "   wfi     "
        );
        /*empty*/
    }
}

void bl702_low_power_config(void)
{
#if !defined(CFG_USB_CDC_ENABLE)
    // Power off DLL
    GLB_Power_Off_DLL();
#endif
    
    // Disable secure engine
    Sec_Eng_Trng_Disable();
    SEC_Eng_Turn_Off_Sec_Ring();
    
#if !defined(CFG_BLE_ENABLE)
    // if ble is not enabled, Disable BLE clock
    GLB_Set_BLE_CLK(0);
#endif
#if !defined(CFG_ZIGBEE_ENABLE)
    // if zigbee is not enabled, Disable Zigbee clock
    GLB_Set_MAC154_ZIGBEE_CLK(0);
#endif
    
    // Gate peripheral clock
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, 0x00214BC3);
}

#if ( configUSE_TICK_HOOK != 0 )
void vApplicationTickHook( void )
{
#if defined(CFG_USB_CDC_ENABLE)
    extern void usb_cdc_monitor(void);
    usb_cdc_monitor();
#endif
#if defined(CFG_ZIGBEE_ENABLE)
	extern void ZB_MONITOR(void);
	ZB_MONITOR();
#endif
}
#endif

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    //static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];
    static StackType_t uxIdleTaskStack[256];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    //*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE; 
    *pulIdleTaskStackSize = 256;//size 256 words is For ble pds mode, otherwise stack overflow of idle task will happen.
}

void _cli_init(int fd_console)
{
#if defined(CFG_USB_CDC_ENABLE)
    extern void usb_cdc_start(int fd_console);
    usb_cdc_start(fd_console);
#endif

    /*Put CLI which needs to be init here*/
#if defined(CFG_EFLASH_LOADER_ENABLE)
    extern int helper_eflash_loader_cli_init(void);
    helper_eflash_loader_cli_init();
#endif

#if defined(CFG_RFPHY_CLI_ENABLE)
    extern int helper_rfphy_cli_init(void);
    helper_rfphy_cli_init();
#endif
#ifdef CFG_ETHERNET_ENABLE
    /*Put CLI which needs to be init here*/
    network_netutils_iperf_cli_register();
    network_netutils_ping_cli_register();
    bl_sys_ota_cli_init();
#endif /* CFG_ETHERNET_ENABLE */
}

#if defined(CFG_BLE_ENABLE)
void ble_init(void)
{
    extern void ble_stack_start(void);
    ble_stack_start();
}
#endif

#if defined(CFG_ZIGBEE_ENABLE)
void zigbee_init(void)
{
    zbRet_t status;
    status = zb_stackInit();
    if (status != ZB_SUCC)
    {
        printf("BL Zbstack Init fail : 0x%08x\r\n", status);
        //ASSERT(false);
    }
    else
    {
        printf("BL Zbstack Init Success\r\n");
    }
    #if defined(CFG_ZIGBEE_CLI)
    zb_cli_register();
    #endif
    register_zb_cb();
    
    zb_app_startup();

    //zb_bdb_init();
}
#endif

void event_cb_key_event(input_event_t *event, void *private_data)
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

void _dump_lib_info(void)
{
#if defined(CFG_BLE_ENABLE)
    puts("BLE Controller LIB Version: ");
    puts(ble_controller_get_lib_ver());
    puts("\r\n");
#endif

#if defined(CFG_ZIGBEE_ENABLE)
    puts("Zigbee LIB Version: ");
    puts(zb_getLibVer());
    puts("\r\n");
#endif
}

#if defined(CFG_ZIGBEE_HBN)
void hbn_wakeup_pin_interrupt(void *arg)
{
    gpio_ctx_t *pstnode = (gpio_ctx_t *)arg;
    uint64_t time = bl_rtc_get_delta_time_ms(bl_hbn_get_wakeup_time());
    
    printf("GPIO%d released, total press time: %llu ms\r\n", pstnode->gpioPin, time);
}
#endif

static void system_init(void)
{
    bl_rtc_init();

#if defined(CFG_ZIGBEE_HBN)
    if(bl_sys_rstinfo_get() == BL_RST_SOFTWARE && bl_hbn_get_wakeup_source() == HBN_WAKEUP_BY_GPIO){
        uint8_t wkpin = __builtin_ctz(bl_hbn_get_wakeup_gpio());
        printf("HBN wakeup by GPIO%d\r\n", wkpin);
        bl_gpio_enable_input(wkpin, 0, 0);
        if(bl_gpio_input_get_value(wkpin) == 1){
            printf("GPIO%d already released\r\n", wkpin);
        }else{
            hal_gpio_register_handler(hbn_wakeup_pin_interrupt, wkpin, GLB_GPIO_INT_CONTROL_ASYNC, GLB_GPIO_INT_TRIG_POS_PULSE, NULL);
        }
    }
#endif

#if defined(CFG_ZIGBEE_HBN)
    extern void zb_hbn_init(void);
    zb_hbn_init();
#endif

    //hal_pds_init();
#if defined(CFG_ZIGBEE_PDS)
    extern void zb_pds_init(void);
    zb_pds_init();
#endif

#if defined(CFG_BLE_PDS)
    extern void ble_pds_init(void);
    ble_pds_init();
#endif

    //configure pds gpio wakeup after pds init and hbn init complete.
    #if (CFG_PDS_LEVEL == 3)
    bl_pds_gpio_wakeup_cfg_ex(1<<PDS_WAKEUP_GPIO);
    #endif
    hal_tcal_init();
#if defined(CFG_ZIGBEE_HBN)
    //configure hbn gpio wakeup after pds init and hbn init complete.
    uint8_t pin_list[1];
    pin_list[0] = HBN_WAKEUP_GPIO;
    bl_hbn_gpio_wakeup_cfg(pin_list, 1);
#endif

#if defined(CFG_USE_PSRAM)
    bl_psram_init();
    vPortDefineHeapRegionsPsram(xHeapRegionsPsram);
    printf("PSRAM Heap %u@%p\r\n",(unsigned int)&_heap3_size, &_heap3_start);
#endif /*CFG_USE_PSRAM*/
}

static void system_thread_init()
{
#ifndef CFG_ETHERNET_ENABLE
    uint32_t fdt = 0, offset = 0;

    if (0 == hal_board_get_dts_addr("gpio", &fdt, &offset)) {
        hal_gpio_init_from_dts(fdt, offset);
        fdt_button_module_init((const void *)fdt, (int)offset);
    }

    aos_register_event_filter(EV_KEY, event_cb_key_event, NULL);
#endif /* CFG_ETHERNET_ENABLE */

#if defined(CFG_BLE_ENABLE)
    #if defined(CONFIG_AUTO_PTS)
    pts_uart_init(1,115200,8,1,0,0);
    // Initialize BLE controller
    ble_controller_init(configMAX_PRIORITIES - 1);
    extern int hci_driver_init(void);
    // Initialize BLE Host stack
    hci_driver_init();

    tester_send(BTP_SERVICE_ID_CORE, CORE_EV_IUT_READY, BTP_INDEX_NONE,
            NULL, 0);
    #else
    ble_init();
    #endif
#endif
#if defined(CFG_ZIGBEE_ENABLE)
#if defined(CFG_ZIGBEE_SLEEPY_END_DEVICE_STARTUP) && (CFG_PDS_LEVEL == 31)
    zb_disableFlashCache();
#endif
    zigbee_init();

    #if defined(CONFIG_HW_SEC_ENG_DISABLE)
    //if sec engine is disabled, use software rand in bl_rand
    int seed = bl_timer_get_current_time();
    srand(seed);
    #endif
#endif
}

void rf_reset_done_callback(void)
{
#if !defined(CFG_ZIGBEE_HBN)
    hal_tcal_restart();
#endif
}

void main()
{
    system_init();
    system_thread_init();

#if defined(CONFIG_AUTO_PTS)
    tester_init();
#endif

#ifdef CFG_ETHERNET_ENABLE
    tcpip_init(NULL, NULL);
    lwip_init_netif();
#endif /*CFG_ETHERNET_ENABLE*/
}
