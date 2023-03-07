/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 *
 * Alibaba Group retains all right, title and interest (including all
 * intellectual property rights) in and to this computer program, which is
 * protected by applicable intellectual property laws.  Unless you have
 * obtained a separate written license from Alibaba Group., you are not
 * authorized to utilize all or a part of this computer program for any
 * purpose (including reproduction, distribution, modification, and
 * compilation into object code), and you must immediately destroy or
 * return to Alibaba Group all copies of this computer program.  If you
 * are licensed by Alibaba Group, your rights to utilize this computer
 * program are limited by the terms of that license.  To obtain a license,
 * please contact Alibaba Group.
 *
 * This computer program contains trade secrets owned by Alibaba Group.
 * and, unless unauthorized by Alibaba Group in writing, you agree to
 * maintain the confidentiality of this computer program and related
 * information and to not disclose this computer program and related
 * information to any other person or entity.
 *
 * THIS COMPUTER PROGRAM IS PROVIDED AS IS WITHOUT ANY WARRANTIES, AND
 * Alibaba Group EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
 * INCLUDING THE WARRANTIES OF MERCHANTIBILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, TITLE, AND NONINFRINGEMENT.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "platform_config.h"
#include "platform.h"
#include "hal_sys.h"
#include "hal_cache.h"
#include "wifi_api.h"
#include "netif.h"
#include "dhcp.h"
#include "inet.h"
#include "alink_export.h"
#include "nvdm.h"
#include "syslog.h"

#define ALINK_CONFIG_FILE_NAME      "alinkconfig.db"
#define ROUTER_INFO_PATH        "/proc/net/route"
#define ROUTER_RECORD_SIZE      256

#ifdef ALINK_SDS_PRINTF
#define alink_log  printf
#else
#define alink_log(fmt, args...)     LOG_I(alink_sds, "[alink] "fmt, ##args)
#define alink_loge(fmt, args...)    LOG_E(alink_sds, "[alink] "fmt, ##args)
#endif

extern char *alink_inet_ntoa(uint32_t ip_addr, char *ip_str, unsigned int buff_size);
/*
{
    snprintf(ip_str, buff_size, "%s", inet_ntoa(*(struct in_addr *)&(ip_addr)));
    return ip_str;
}
*/

/*
route table format:
#cat /proc/net/route
Iface   Destination Gateway     Flags   RefCnt  Use Metric  Mask        MTU Window  IRTT
eth0    00000000    010AA8C0    0003    0   0   0   00000000    0   0   0
eth1    0000FEA9    00000000    0001    0   0   1000    0000FFFF    0   0   0
*/
char *platform_get_default_routing_ifname(char *ifname, int ifname_size)
{
    char *result = NULL;

    //strcpy(ifname, IFNAME);
	strncpy(ifname, IFNAME, ifname_size - 1);
	result = ifname;

    return result;
}

char *platform_wifi_get_mac(char mac_str[PLATFORM_MAC_LEN])
{
	int32_t ret = 0;
	uint8_t port;
    uint8_t temp_mac[6] = {0};
    alink_log("get mac");

	port = 	WIFI_PORT_STA;
    ret = wifi_config_get_mac_address(port, (uint8_t *)temp_mac);
    if(ret == 0)
        snprintf(mac_str, STR_MAC_LEN, "%02x:%02x:%02x:%02x:%02x:%02x",
            temp_mac[0],temp_mac[1],temp_mac[2],temp_mac[3],temp_mac[4],temp_mac[5]);

    return mac_str;
}

int platform_wifi_get_rssi_dbm(void)
{
    int8_t rssi = 0;
	int32_t ret = 0;

	ret = wifi_connection_get_rssi(&rssi);
	if (ret < 0) {
		rssi = 0;
		alink_loge("wifi get rssi fail");
	}

	return rssi;
}

uint32_t platform_wifi_get_ip(char ip_str[PLATFORM_IP_LEN], const char *ifname)
{
    struct netif *sta_if = NULL;
    static char address[16] = {0};
    uint32_t addr;
    if(ifname == NULL) {
        sta_if = netif_find(IFNAME);
    } else {
        sta_if = netif_find(ifname);
    }
    if (sta_if) {
        addr = ip4_addr_get_u32(netif_ip4_addr(sta_if));
        //alink_inet_ntoa(addr, address, 16);
        snprintf(ip_str, 16, "%s", inet_ntoa(*(struct in_addr *)&(addr)));
        alink_log("address: %s\n", ip_str);
    } else {
        alink_loge("netif_find fail\n");
    }

    return addr;
}

char *platform_get_chipid(char chipid[PLATFORM_CID_LEN])
{
    //strncpy(chipid, "unknown chipid", PLATFORM_CID_LEN);

#if (PRODUCT_VERSION == 7682)
    strncpy(chipid, "mt7682_hdk_12345678", PLATFORM_CID_LEN);
#elif (PRODUCT_VERSION == 7686)
    strncpy(chipid, "mt7686_hdk_12345678", PLATFORM_CID_LEN);
#elif (PRODUCT_VERSION == 7697)
    strncpy(chipid, "mt7697_hdk_12345678", PLATFORM_CID_LEN);
#else
    strncpy(chipid, "mt7687_hdk_12345678", PLATFORM_CID_LEN);
#endif
    chipid[PLATFORM_CID_LEN - 1] = '\0';

    return chipid;
}

char *platform_get_os_version(char os_ver[STR_SHORT_LEN])
{

    char *sdk_version = "SDK_V4.6.0";
	unsigned int len = STR_SHORT_LEN;

    memset(os_ver, 0, len);
    if(strlen(sdk_version) >= len)
        memcpy(os_ver, sdk_version, len-1);
    else
        memcpy(os_ver, sdk_version, strlen(sdk_version));

    alink_log("os_ver: %s", os_ver);

    return (char *)os_ver;
}

int platform_config_write(const char *buffer, int length)
{
    // unsigned int flashAddr;
    // int status;

    // hal_flash_erase(CM4_FLASH_ALINK_ADDR, HAL_FLASH_BLOCK_4K);
    // status = hal_flash_write(CM4_FLASH_ALINK_ADDR, buffer, len);

    nvdm_status_t status;
    status = nvdm_write_data_item(
        "Alink",
        "Config",
        NVDM_DATA_ITEM_TYPE_RAW_DATA,
        (const uint8_t *)buffer,
        (uint32_t)length);

    return status;
}

int platform_config_read(char *buffer, int length)
{
    printf("%s : ", __FUNCTION__);
    // unsigned int flashAddr;
    // int status;

    // status = hal_flash_read(CM4_FLASH_ALINK_ADDR, buffer, len);

    nvdm_status_t status;

    memset(buffer, 0, length);
    status= nvdm_read_data_item(
        "Alink",
        "Config",
        (uint8_t *)buffer,
        (uint32_t *)&length);

    return status;
}

int platform_sys_net_is_ready(void)
{
    uint8_t link_status = 0;
    struct netif *sta_if = NULL;

    //check wifi link
    while(!link_status){
        wifi_connection_get_link_status(&link_status);
        if(!link_status){
            /*wait until wifi connect*/
            alink_loge("Waiting for STA link up...");
            platform_msleep(1000);
        }
    }

    //check netif up
    while(!(sta_if && (sta_if->flags & NETIF_FLAG_UP))){
        sta_if = netif_find("st2");
        if(!(sta_if && (sta_if->flags & NETIF_FLAG_UP))){
            /*wait until netif up*/
            alink_loge("Waiting for netif up...");
            platform_msleep(1000);
        }
    }

    while (!(sta_if->dhcp && (sta_if->dhcp->state == DHCP_STATE_BOUND || sta_if->dhcp->state == DHCP_STATE_RENEWING))) {
        /*wait until got ip address*/
        alink_loge("Waiting for got ip address...");
        platform_msleep(1000);
    }

    return 1;
}

void platform_sys_reboot(void)
{
    alink_log("reboot Bye Bye Bye!!!!");

    //reboot api
    //hal_sys_reboot(HAL_SYS_REBOOT_MAGIC, WHOLE_SYSTEM_REBOOT_COMMAND);
    hal_wdt_software_reset();
}

char *platform_get_module_name(char name_str[STR_SHORT_LEN])
{
    strncpy(name_str, MODULE_NAME, STR_SHORT_LEN);
    name_str[STR_SHORT_LEN - 1] ='\0';
    return name_str;
}

const char *platform_get_storage_directory(void)
{
    return ALINK_STORAGE_PATH;
}
