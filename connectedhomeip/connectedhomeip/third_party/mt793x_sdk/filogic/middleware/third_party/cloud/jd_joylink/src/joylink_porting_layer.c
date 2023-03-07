/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include "joylink_porting_layer.h"
#include "joylink_log.h"
#include "joylink_ret_code.h"
#include "joylink_extern.h"
#include "crc.h"
#include "wifi_api.h"

#include "netif.h"
#include "dhcp.h"
#include "task_def.h"

joylink_fota_ctx_t _g_fota_ctx;
httpclient_t _g_fota_httpclient = {0};

static const char fota_status_desc[4][30] = {
    {"downloading"},
    {"crc32 not match"},
    {"download fail"},
    {"connect server fail"}
};

void joylink_wait_network_up(void)
{
    uint8_t link_status = 0;
    struct netif *sta_if = NULL;

    printf("%s : start\n", __FUNCTION__);

    //check wifi link
    while(!link_status){
        wifi_connection_get_link_status(&link_status);
        if(!link_status){
            /*wait until wifi connect*/
            // printf("Waiting for STA link up...\n");
            vTaskDelay(1000);
        }
    }

    //check netif up
    while(!(sta_if && (sta_if->flags & NETIF_FLAG_UP))){
        sta_if = netif_find("st2");
        if(!(sta_if && (sta_if->flags & NETIF_FLAG_UP))){
            /*wait until netif up*/
            // printf("Waiting for netif up...\n");
            vTaskDelay(1000);
        }
    }

    while (!(sta_if->dhcp && (sta_if->dhcp->state == DHCP_STATE_BOUND || sta_if->dhcp->state == DHCP_STATE_RENEWING))) {
        /*wait until got ip address*/
        // printf("Waiting for got ip address...\n");
        vTaskDelay(1000);
    }

    printf("%s : end\n", __FUNCTION__);
}

void joylink_task(void *args)
{
    printf("%s : heap free size:%d\n", __FUNCTION__, xPortGetFreeHeapSize());

    joylink_wait_network_up();

    // TODO: delay 10s
    // vTaskDelay(10000);/* Print log may block other task, so sleep some ticks */

    nvdm_init();

    joylink_dev_register_attr_cb(JL_ATTR_WLAN24G, TGW_ATTR_J, joylink_get_wlan24g, joylink_set_wlan24g);
    joylink_dev_register_attr_cb(JL_ATTR_SUBDEVS, TGW_ATTR_J, joylink_get_subdev, joylink_set_subdev);
    joylink_dev_register_attr_cb(JL_ATTR_WAN_SPEED, TGW_ATTR_J, joylink_get_wlanspeed, joylink_set_wlanspeed);
    joylink_dev_register_attr_cb(JL_ATTR_UUID, TGW_ATTR_J, joylink_get_uuid, joylink_set_uuid);
    joylink_dev_register_attr_cb(JL_ATTR_FEEDID, TGW_ATTR_J, joylink_get_feedid, joylink_set_feedid);
    joylink_dev_register_attr_cb(JL_ATTR_ACCESSKEY, TGW_ATTR_J, joylink_get_accesskey, joylink_set_accesskey);
    joylink_dev_register_attr_cb(JL_ATTR_LOCALKEY, TGW_ATTR_J, joylink_get_localkey, joylink_set_localkey);
    joylink_dev_register_attr_cb(JL_ATTR_CONN_STATUS, TGW_ATTR_J, joylink_get_server_st, joylink_set_server_st);
    joylink_dev_register_attr_cb(JL_ATTR_MACADDR, TGW_ATTR_J, joylink_get_macaddr, joylink_set_macaddr);
    joylink_dev_register_attr_cb(JL_ATTR_VERSION, TGW_ATTR_J, joylink_get_version, joylink_set_version);
    joylink_dev_register_attr_cb(JL_ATTR_SERVER_INFO, TGW_ATTR_J, joylink_get_serverinfo, joylink_set_serverinfo);

    joylink_main_start();
}

int joylink_demo()
{
    printf("%s : heap free size:%d\n", __FUNCTION__, xPortGetFreeHeapSize());

    /* Disable joylink subdev feature, slim task stack size and RAM size
       In joylink_extern_sub_dev.c file, define JL_MAX_SUB (1) slim RAM size.
       In joylink_extern.c file, joylink_dev_get_client_list() disable slim task stack size.
       But joylink_proc_server()/joylink_enc2_gen_tables() still cost too must task stack size.
     */
    xTaskCreate(joylink_task, JOYLINK_TASK_NAME, JOYLINK_TASK_STACKSIZE/sizeof(portSTACK_TYPE), NULL, JOYLINK_TASK_PRIO, NULL); // set stack size 20KB

    return 0;
}

void* joylink_calloc(size_t size, size_t num)
{
    void *buf = NULL;
    buf = pvPortCalloc(num, size);

    if(NULL == buf)
        log_error("joylink_calloc() Error! size:%d, num:%d", size, num);

    /*
        uint32_t  free_size;
        free_size = xPortGetFreeHeapSize();
        log_debug("joylink_calloc() free_size:%d, buf:%p, sz:%d, num:%d", free_size, buf, size, num);
     */

    return buf;
}

void* joylink_malloc(size_t sz)
{
    void *buf = NULL;
    buf = pvPortMalloc(sz);

    if(NULL == buf)
        log_error("joylink_malloc() Error! size:%d", sz);

    /*
        uint32_t  free_size;
        free_size = xPortGetFreeHeapSize();
        log_debug("joylink_malloc() free_size:%d, buf:%p, sz:%d", free_size, buf, sz);
     */

    return buf;
}

void joylink_free(void *ptr)
{
    /*
        uint32_t  free_size;
        free_size = xPortGetFreeHeapSize();
        log_debug("joylink_free() free_size:%d, buf:%p", free_size, ptr);
     */

    if(ptr)
        vPortFree(ptr);
}

int joylink_get_wlan24g(char *buf, unsigned int buf_sz)
{
    nvdm_status_t status;

    status = nvdm_read_data_item(
        "User",
        "joylink_wlan24g",
        (uint8_t *)buf,
        (uint32_t *)&buf_sz);

    // log_debug("joylink_get_wlan24g() status:%d, buf_sz:%d", status, buf_sz);

    if('\0' == *buf)
        return 1;

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_set_wlan24g(const char *json_in)
{
    nvdm_status_t status;

    status = nvdm_write_data_item(
        "User",
        "joylink_wlan24g",
        NVDM_DATA_ITEM_TYPE_STRING,
        (const uint8_t *)json_in,
        strlen(json_in));

    // log_debug("joylink_set_wlan24g() status:%d", status);

    //control the real hardware to turn on Wlan 2.4G

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_get_subdev(char *buf, unsigned int buf_sz)
{
    nvdm_status_t status;

    status = nvdm_read_data_item(
        "User",
        "joylink_subdev",
        (uint8_t *)buf,
        (uint32_t *)&buf_sz);

    // log_debug("joylink_get_subdev() status:%d, buf_sz:%d", status, buf_sz);

    if('\0' == *buf)
        return 1;

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_set_subdev(const char *json_in)
{
    nvdm_status_t status;

    status = nvdm_write_data_item(
        "User",
        "joylink_subdev",
        NVDM_DATA_ITEM_TYPE_STRING,
        (const uint8_t *)json_in,
        strlen(json_in));

    // log_debug("joylink_set_subdev() status:%d", status);

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_get_wlanspeed(char *buf, unsigned int buf_sz)
{
    nvdm_status_t status;

    status = nvdm_read_data_item(
        "User",
        "joylink_wlanspeed",
        (uint8_t *)buf,
        (uint32_t *)&buf_sz);

    // log_debug("joylink_get_wlanspeed() status:%d, buf_sz:%d", status, buf_sz);

    if('\0' == *buf)
        return 1;

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_set_wlanspeed(const char *json_in)
{
    nvdm_status_t status;

    status = nvdm_write_data_item(
        "User",
        "joylink_wlanspeed",
        NVDM_DATA_ITEM_TYPE_STRING,
        (const uint8_t *)json_in,
        strlen(json_in));

    // log_debug("joylink_set_wlanspeed() status:%d", status);

    //control the hardware to enable speed settings

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_get_uuid(char *buf, unsigned int buf_sz)
{
    nvdm_status_t status;

    status = nvdm_read_data_item(
        "User",
        "joylink_uuid",
        (uint8_t *)buf,
        (uint32_t *)&buf_sz);

    // log_debug("joylink_get_uuid() status:%d, buf_sz:%d", status, buf_sz);

    if('\0' == *buf)
        return 1;

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_set_uuid(const char *json_in)
{
    nvdm_status_t status;

    status = nvdm_write_data_item(
        "User",
        "joylink_uuid",
        NVDM_DATA_ITEM_TYPE_STRING,
        (const uint8_t *)json_in,
        strlen(json_in));

    // log_debug("joylink_set_uuid() status:%d", status);

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_get_feedid(char *buf, unsigned int buf_sz)
{
    nvdm_status_t status;

    status = nvdm_read_data_item(
        "User",
        "joylink_feedid",
        (uint8_t *)buf,
        (uint32_t *)&buf_sz);

    // log_debug("joylink_get_feedid() status:%d, buf_sz:%d", status, buf_sz);

    if('\0' == *buf)
        return 1;

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_set_feedid(const char *json_in)
{
    nvdm_status_t status;

    status = nvdm_write_data_item(
        "User",
        "joylink_feedid",
        NVDM_DATA_ITEM_TYPE_STRING,
        (const uint8_t *)json_in,
        strlen(json_in));

    // log_debug("joylink_set_feedid() status:%d", status);

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_get_accesskey(char *buf, unsigned int buf_sz)
{
    nvdm_status_t status;

    status = nvdm_read_data_item(
        "User",
        "joylink_accesskey",
        (uint8_t *)buf,
        (uint32_t *)&buf_sz);

    // log_debug("joylink_get_accesskey() status:%d, buf_sz:%d", status, buf_sz);

    if('\0' == *buf)
        return 1;

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_set_accesskey(const char *json_in)
{
    nvdm_status_t status;

    status = nvdm_write_data_item(
        "User",
        "joylink_accesskey",
        NVDM_DATA_ITEM_TYPE_STRING,
        (const uint8_t *)json_in,
        strlen(json_in));

    // log_debug("joylink_set_accesskey() status:%d", status);

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_get_localkey(char *buf, unsigned int buf_sz)
{
    nvdm_status_t status;

    status = nvdm_read_data_item(
        "User",
        "joylink_localkey",
        (uint8_t *)buf,
        (uint32_t *)&buf_sz);

    // log_debug("joylink_get_localkey() status:%d, buf_sz:%d", status, buf_sz);

    if('\0' == *buf)
        return 1;

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_set_localkey(const char *json_in)
{
    nvdm_status_t status;

    status = nvdm_write_data_item(
        "User",
        "joylink_localkey",
        NVDM_DATA_ITEM_TYPE_STRING,
        (const uint8_t *)json_in,
        strlen(json_in));

    // log_debug("joylink_set_localkey() status:%d", status);

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_get_server_st(char *buf, unsigned int buf_sz)
{
    nvdm_status_t status;

    status = nvdm_read_data_item(
        "User",
        "joylink_server_st",
        (uint8_t *)buf,
        (uint32_t *)&buf_sz);

    // log_debug("joylink_get_server_st() status:%d, buf_sz:%d", status, buf_sz);

    if('\0' == *buf)
        return 1;

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_set_server_st(const char *json_in)
{
    nvdm_status_t status;

    status = nvdm_write_data_item(
        "User",
        "joylink_server_st",
        NVDM_DATA_ITEM_TYPE_STRING,
        (const uint8_t *)json_in,
        strlen(json_in));

    // log_debug("joylink_set_server_st() status:%d", status);

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_get_macaddr(char *buf, unsigned int buf_sz)
{
    nvdm_status_t status;

    status = nvdm_read_data_item(
        "User",
        "joylink_macaddr",
        (uint8_t *)buf,
        (uint32_t *)&buf_sz);

    // log_debug("joylink_get_macaddr() status:%d, buf_sz:%d", status, buf_sz);

    if('\0' == *buf)
        return 1;

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_set_macaddr(const char *json_in)
{
    nvdm_status_t status;

    status = nvdm_write_data_item(
        "User",
        "joylink_macaddr",
        NVDM_DATA_ITEM_TYPE_STRING,
        (const uint8_t *)json_in,
        strlen(json_in));

    // log_debug("joylink_set_macaddr() status:%d", status);

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_get_server_info(char *buf, unsigned int buf_sz)
{
    nvdm_status_t status;

    status = nvdm_read_data_item(
        "User",
        "joylink_server_info",
        (uint8_t *)buf,
        (uint32_t *)&buf_sz);

    // log_debug("joylink_get_server_info() status:%d, buf_sz:%d", status, buf_sz);

    if('\0' == *buf)
        return 1;

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_set_server_info(const char *json_in)
{
    nvdm_status_t status;

    status = nvdm_write_data_item(
        "User",
        "joylink_server_info",
        NVDM_DATA_ITEM_TYPE_STRING,
        (const uint8_t *)json_in,
        strlen(json_in));

    // log_debug("joylink_set_server_info() status:%d", status);

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_get_version(char *buf, unsigned int buf_sz)
{
    nvdm_status_t status;

    status = nvdm_read_data_item(
        "User",
        "joylink_version",
        (uint8_t *)buf,
        (uint32_t *)&buf_sz);

    // log_debug("joylink_get_version() status:%d, buf_sz:%d", status, buf_sz);

    if('\0' == *buf)
        return 1;

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_set_version(const char *json_in)
{
    nvdm_status_t status;

    status = nvdm_write_data_item(
        "User",
        "joylink_version",
        NVDM_DATA_ITEM_TYPE_STRING,
        (const uint8_t *)json_in,
        strlen(json_in));

    // log_debug("joylink_set_version() status:%d", status);

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_get_serverinfo(char *buf, unsigned int buf_sz)
{
    nvdm_status_t status;

    status = nvdm_read_data_item(
        "User",
        "joylink_serverinfo",
        (uint8_t *)buf,
        (uint32_t *)&buf_sz);

    // log_debug("joylink_get_serverinfo() status:%d, buf_sz:%d", status, buf_sz);

    if('\0' == *buf)
        return 1;

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

int joylink_set_serverinfo(const char *json_in)
{
    nvdm_status_t status;

    status = nvdm_write_data_item(
        "User",
        "joylink_serverinfo",
        NVDM_DATA_ITEM_TYPE_STRING,
        (const uint8_t *)json_in,
        strlen(json_in));

    // log_debug("joylink_set_serverinfo() status:%d", status);

    return (NVDM_STATUS_OK == status) ? 0 : 1;
}

void joylink_test_proc(void* para)
{
}


void joylink_fota_set_upgrade_status(joylink_fota_status_t status, uint32_t progress, joylink_fota_error_code_t error_code)
{
    _g_fota_ctx.upgrade_status = status;
    _g_fota_ctx.progress = progress;
    _g_fota_ctx.error_code = error_code;
}

int32_t joylink_fota_http_retrieve_get(char* buf, uint32_t len)
{
    int32_t ret = HTTPCLIENT_ERROR_CONN;
    int32_t write_ret;
    httpclient_data_t client_data = {0};
    char *get_url = _g_fota_ctx.download_url;
    int32_t count = 0;
    uint32_t data_len = 0;

    client_data.response_buf = buf;
    client_data.response_buf_len = len;

    ret = httpclient_send_request(&_g_fota_httpclient, get_url, HTTPCLIENT_GET, &client_data);
    if (ret < 0) {
        joylink_fota_set_upgrade_status(FOTA_STATUS_FAILURE, 0, FOTA_ERROR_CODE_DOWNLOAD_ERROR);
        return ret;
    }

    do {
        ret = httpclient_recv_response(&_g_fota_httpclient, &client_data);
        if (ret < 0) {
            return ret;
        }

        data_len = strlen(client_data.response_buf);
        count += data_len;
        vTaskDelay(100);/* Print log may block other task, so sleep some ticks */
        printf("data received %d, %s\n", count, client_data.response_buf);
        //client_data.response_buf[5] = '\0';
        //if (strcmp(string_table[seg++], client_data.response_buf) != 0) {
        //    return -1;
        //}
        write_ret = fota_write(FOTA_PARITION_TMP, (const uint8_t*)client_data.response_buf, data_len);
        if (FOTA_STATUS_OK != write_ret) {
            printf("fail to write flash, write_ret = %d\r\n", write_ret);
            return ret;
        }

        joylink_fota_set_upgrade_status(FOTA_STATUS_DOWNLOAD, count/client_data.response_content_len * 100, FOTA_ERROR_CODE_NONE);

    } while (ret == HTTPCLIENT_RETRIEVE_MORE_DATA);

    printf("total length: %d\n", client_data.response_content_len);
    if (count != client_data.response_content_len || httpclient_get_response_code(&_g_fota_httpclient) != 200) {
        printf("data received not completed, or invalid error code\r\n");
        joylink_fota_set_upgrade_status(FOTA_STATUS_FAILURE, 0, FOTA_ERROR_CODE_DOWNLOAD_ERROR);
        return -1;
    }

    _g_fota_ctx.data_len = count;
    return ret;
}


void
joylink_fota_download_task(void *parameter)
{
    char *get_url = _g_fota_ctx.download_url;
    int32_t ret = HTTPCLIENT_ERROR_CONN;
    uint32_t crc = 0;
    uint32_t data_len;

    char* buf = joylink_malloc(FOTA_BUF_SIZE);
    if (buf == NULL) {
        printf("buf malloc failed.\r\n");
        return;
    }

    ret = httpclient_connect(&_g_fota_httpclient, get_url);

    if (!ret) {
        ret = joylink_fota_http_retrieve_get(buf, FOTA_BUF_SIZE);
    }else {
        joylink_fota_set_upgrade_status(FOTA_STATUS_FAILURE, 0, FOTA_ERROR_CODE_CONNECT_SERVER_ERROR);
    }

    printf ("Download result = %d \r\n", ret);

    httpclient_close(&_g_fota_httpclient);

    // crc32 check
    uint32_t len;
    data_len = _g_fota_ctx.data_len;
    make_crc32_table();
    while (data_len > 0) { /* read 1KB data every time */
        if (data_len > FOTA_BUF_SIZE) {
            len = FOTA_BUF_SIZE;
            data_len -= FOTA_BUF_SIZE;
        }else { /* read the last time when < 1KB */
            len = data_len;
            data_len = 0;
        }

        if (FOTA_STATUS_OK != fota_read(FOTA_PARITION_TMP, (uint8_t *)buf, len)) {
            printf("fail to read flash\r\n");
            break;
        }

        crc = make_crc(crc, (unsigned char *)buf, len);
    }

    joylink_free(buf);
    buf = NULL;

    if (_g_fota_ctx.crc32 == crc) {
        // download success
        joylink_fota_set_upgrade_status(FOTA_STATUS_INSTALL, 0, FOTA_ERROR_CODE_NONE);
        fota_trigger_update();

        // TODO: reboot device
        hal_wdt_config_t wdt_config;
        wdt_config.mode = HAL_WDT_MODE_RESET;
        wdt_config.seconds = 5;
        hal_wdt_init(&wdt_config);
        hal_wdt_software_reset();
        //while(1); /* waiting the system reboot */

    }else {
        // download fail, crc32 not match.
        joylink_fota_set_upgrade_status(FOTA_STATUS_FAILURE, 0, FOTA_ERROR_CODE_CRC32_ERROR);
    }
    vTaskDelete(NULL);
}


void joylink_fota_download_package()
{
    // create task to execute download operation.
    xTaskCreate(
        joylink_fota_download_task,
        JOYLINK_FOTA_DOWNLOAD_TASK_NAME,
        JOYLINK_FOTA_DOWNLOAD_TASK_STACKSIZE/sizeof(portSTACK_TYPE),
        NULL,
        JOYLINK_FOTA_DOWNLOAD_TASK_PRIO,
        NULL);
}

const char* joylink_fota_get_status_desc(joylink_fota_error_code_t error_code)
{
    return fota_status_desc[error_code];
}
