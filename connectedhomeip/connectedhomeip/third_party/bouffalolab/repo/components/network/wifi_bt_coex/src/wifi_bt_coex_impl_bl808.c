#include <stdio.h>
#include <wifi_bt_coex.h>

#include "wifi_bt_coex_impl.h"
#include "wifi_bt_coex_ctx.h"

#define PTA_REG_BASE_ADDR               (0x24920400)
#define PTA_REG_REVISION                (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x00))
#define PTA_REG_CONFIG                  (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x04))
#define PTA_REG_BT_TX                   (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x08))
#define PTA_REG_BT_TX_ABORT             (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x0C))
#define PTA_REG_BT_RX                   (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x10))
#define PTA_REG_BT_RX_ABORT             (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x14))
#define PTA_REG_WLAN_TX                 (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x18))
#define PTA_REG_WLAN_TX_ABORT           (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x1C))
#define PTA_REG_WLAN_RX                 (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x20))
#define PTA_REG_WLAN_RX_ABORT           (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x24))
#define PTA_REG_CONFIG2                 (*(volatile uint32_t *)(PTA_REG_BASE_ADDR + 0x28))

#define WLAN_COEX_REG_BASE_ADDR         (0x24B00400)
#define WLAN_COEX_REG_CoexControlReg    (*(volatile uint32_t *)(WLAN_COEX_REG_BASE_ADDR + 0x00))
#define WLAN_COEX_REG_CoexPTIReg        (*(volatile uint32_t *)(WLAN_COEX_REG_BASE_ADDR + 0x04))
#define WLAN_COEX_REG_CoexStatReg       (*(volatile uint32_t *)(WLAN_COEX_REG_BASE_ADDR + 0x08))
#define WLAN_COEX_REG_CoexIntReg        (*(volatile uint32_t *)(WLAN_COEX_REG_BASE_ADDR + 0x0C))
#define WLAN_COEX_REG_CoexControl2Reg   (*(volatile uint32_t *)(WLAN_COEX_REG_BASE_ADDR + 0x10))

static enum WIFI_BT_COEX_FORCE_MODE wifi_bt_coex_bl808_mode = WIFI_BT_COEX_FORCE_MODE_PTA_FORCE;

extern struct wifi_bt_coex_ctx coex_timing_control_ctx;

int wifi_bt_coex_dump_all_impl(void)
{
    uint32_t val;

    printf("============BL808 PTA Reg Dump============\r\n");
    printf("REVISIOIN       %08lx\r\n", PTA_REG_REVISION);
    printf("CONFIG          %08lx\r\n", PTA_REG_CONFIG);
    printf("BT TX           %08lx\r\n", PTA_REG_BT_TX);
    printf("BT TX Abort     %08lx\r\n", PTA_REG_BT_TX_ABORT);
    printf("BT RX           %08lx\r\n", PTA_REG_BT_RX);
    printf("BT RX Abort     %08lx\r\n", PTA_REG_BT_RX_ABORT);
    printf("WLAN TX         %08lx\r\n", PTA_REG_WLAN_TX);
    printf("WLAN TX Abort   %08lx\r\n", PTA_REG_WLAN_TX_ABORT);
    printf("WLAN RX         %08lx\r\n", PTA_REG_WLAN_RX);
    printf("WLAN RX Abort   %08lx\r\n", PTA_REG_WLAN_RX_ABORT);
    printf("CONFIG2         %08lx\r\n", PTA_REG_CONFIG2);
    printf("============BL808 Coex (Wi-Fi) Reg Dump============\r\n");
    printf("CoexControlReg  %08lx\r\n", WLAN_COEX_REG_CoexControlReg);

    val = WLAN_COEX_REG_CoexPTIReg;
    printf("CoexPTIReg      %08lx\r\n", val);
    printf("    ACK     %lu\r\n", (val >>  0) & 0xF);
    printf("    Cntrl   %lu\r\n", (val >>  4) & 0xF);
    printf("    Mgt     %lu\r\n", (val >>  8) & 0xF);
    printf("    VO      %lu\r\n", (val >> 12) & 0xF);
    printf("    VI      %lu\r\n", (val >> 16) & 0xF);
    printf("    BE      %lu\r\n", (val >> 20) & 0xF);
    printf("    BK      %lu\r\n", (val >> 24) & 0xF);
    printf("    BCN     %lu\r\n", (val >> 28) & 0xF);

    printf("CoexStatReg     %08lx\r\n", WLAN_COEX_REG_CoexStatReg);
    printf("CoexIntReg      %08lx\r\n", WLAN_COEX_REG_CoexIntReg);
    val = WLAN_COEX_REG_CoexControl2Reg;
    printf("CoexControl2Reg %08lx\r\n", val);
    printf("    Force BK    %lu\r\n", (val >> 0) & 0x1);
    printf("    Force BE    %lu\r\n", (val >> 1) & 0x1);
    printf("    Force VI    %lu\r\n", (val >> 2) & 0x1);
    printf("    Force VO    %lu\r\n", (val >> 3) & 0x1);
    printf("    Force BCN   %lu\r\n", (val >> 4) & 0x1);

    return 0;
}

int wifi_bt_coex_force_wlan_impl(void)
{
    //puts("BL808 COEX WLAN force impl called\r\n");
    //Force to RF1/BT/WLAN
    switch (wifi_bt_coex_bl808_mode) {
        case WIFI_BT_COEX_FORCE_MODE_PTA_FORCE: 
        {
            PTA_REG_CONFIG2 = 0;
            PTA_REG_CONFIG = ((0x50000000 | (1 << 16) | (1 << 17)));
        }
        break;
        case WIFI_BT_COEX_FORCE_MODE_PTI_DEFAULT_FORCE:
        {
            PTA_REG_CONFIG2 = 0;
            //                              PTA En       WLAN      Default Priority
            PTA_REG_CONFIG = ((0x50000000 | (1 << 0) | (1 << 1) | (0 << 4)));
        }
        break;
        case WIFI_BT_COEX_FORCE_MODE_PTI_PRIORITY_FORCE:
        {
            PTA_REG_CONFIG2 = 0;
            //                              PTA En       WLAN        Priority
            PTA_REG_CONFIG = ((0x50000000 | (1 << 0) | (1 << 1) | (1 << 4)));
            WLAN_COEX_REG_CoexControlReg = 0xF48;
            WLAN_COEX_REG_CoexPTIReg = 0xFFFFFFFF;
            WLAN_COEX_REG_CoexControlReg = 0xF49;
        }
        break;
        case WIFI_BT_COEX_FORCE_MODE_PTI_PRIORITY_FORCE2:
        {
            PTA_REG_CONFIG2 = 0;
            WLAN_COEX_REG_CoexControlReg = 0xF0000F58;
            //                              PTA En       WLAN        Priority
            PTA_REG_CONFIG = ((0x50000000 | (1 << 0) | (1 << 1) | (1 << 4)));
            WLAN_COEX_REG_CoexControlReg = 0xF0000F59;
        }
        break;
        default:
        {
            /*empty here*/
        }
    }
    return 0;
}

int wifi_bt_coex_force_bt_impl(void)
{
    //puts("BL808 COEX BT force impl called\r\n");
    //Force to RF1/BT/WLAN
    switch (wifi_bt_coex_bl808_mode) {
        case WIFI_BT_COEX_FORCE_MODE_PTA_FORCE: 
        {
            PTA_REG_CONFIG2 = 0;
            PTA_REG_CONFIG = ((0x50000000 | (1 << 18) | (1 << 19)));
        }
        break;
        case WIFI_BT_COEX_FORCE_MODE_PTI_DEFAULT_FORCE:
        {
            PTA_REG_CONFIG2 = 0;
            //                              PTA En       WLAN      Priority
            PTA_REG_CONFIG = ((0x50000000 | (1 << 0) | (0 << 1) | (0 << 4)));
        }
        break;
        case WIFI_BT_COEX_FORCE_MODE_PTI_PRIORITY_FORCE:
        {
            PTA_REG_CONFIG2 = 0;
            //                              PTA En       WLAN        Priority
            PTA_REG_CONFIG = ((0x50000000 | (1 << 0) | (1 << 1) | (1 << 4)));
            WLAN_COEX_REG_CoexControlReg = 0x48;
            WLAN_COEX_REG_CoexPTIReg = 0x00;
            WLAN_COEX_REG_CoexControlReg = 0x49;
        }
        break;
        case WIFI_BT_COEX_FORCE_MODE_PTI_PRIORITY_FORCE2:
        {
            PTA_REG_CONFIG2 = 0;
            WLAN_COEX_REG_CoexControlReg = 0x048;
            //                              PTA En       WLAN        Priority
            PTA_REG_CONFIG = ((0x50000000 | (1 << 0) | (1 << 1) | (1 << 4)));
            WLAN_COEX_REG_CoexControlReg = 0x049;
        }
        break;
        default:
        {
            /*empty here*/
        }
    }
    return 0;
}

int wifi_bt_coex_force_mode_impl(enum WIFI_BT_COEX_FORCE_MODE mode)
{
    wifi_bt_coex_bl808_mode = mode;
    return 0;
}

int wifi_bt_coex_event_handler_impl(enum WIFI_BT_COEX_EVENT event, void *event_arg)
{
    switch (event) {
        case WIFI_BT_COEX_EVENT_BT_A2DP_UNDERRUN:
        {
            wifi_bt_coex_bt_inc(&coex_timing_control_ctx, 1);
        }
        break;
        default:
        {
            /*empty here*/
        }
    }
    return 0;
}
