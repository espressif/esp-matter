/*
 * Copyright (c) 2020 Bouffalolab.
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
#include "FreeRTOS.h"
#include "task.h"
#include <utils_log.h>
#include <stdlib.h>
#include <wifi_mgmr_ext.h>
#include <lwip/ip4_addr.h>
#include <http_client.h>
#include <utils_sha256.h>
#include <hal_boot2.h>
#include <bl_mtd.h>
#include "bl_http_ota.h"
#include <hal_sys.h>
#include <semphr.h>
#include <cli.h>

static xSemaphoreHandle otaSema;
static ssize_t otaTotalLength = 0;
static uint8_t *otaBuffer = NULL;
static iot_sha256_context otaCtx;
static uint8_t otaSha256Result[32];
static uint8_t otaSha256Img[32];
static bl_mtd_handle_t otaHandle = NULL;
static HALPartition_Entry_Config otaEntry;
#define OTA_BUFFER_SIZE 4096
typedef struct ota_header {
    union {
        struct {
            uint8_t header[16];

            uint8_t type[4];//RAW XZ
            uint32_t len;//body len
            uint8_t pad0[8];

            uint8_t ver_hardware[16];
            uint8_t ver_software[16];

            uint8_t sha256[32];
        } s;
        uint8_t _pad[512];
    } u;
} ota_header_t;
#define OTA_HEADER_SIZE (sizeof(ota_header_t))

static int check_ota_header(ota_header_t *ota_header, uint32_t *ota_len, int *use_xz)
{
    char str[33];//assume max segment size
    int i;

    memcpy(str, ota_header->u.s.header, sizeof(ota_header->u.s.header));
    str[sizeof(ota_header->u.s.header)] = '\0';
    printf("[OTA] [HEADER] ota header is ");
    printf(str);
    printf("\r\n");

    memcpy(str, ota_header->u.s.type, sizeof(ota_header->u.s.type));
    str[sizeof(ota_header->u.s.type)] = '\0';
    printf("[OTA] [HEADER] file type is ");
    printf(str);
    printf("\r\n");
    if (strstr(str, "XZ")) {
        *use_xz = 1;
    } else {
        *use_xz = 0;
    }

    memcpy(ota_len, &(ota_header->u.s.len), 4);
    printf("[OTA] [HEADER] file length (exclude ota header) is %lu\r\n", *ota_len);

    memcpy(str, ota_header->u.s.ver_hardware, sizeof(ota_header->u.s.ver_hardware));
    str[sizeof(ota_header->u.s.ver_hardware)] = '\0';
    printf("[OTA] [HEADER] ver_hardware is ");
    printf(str);
    printf("\r\n");

    memcpy(str, ota_header->u.s.ver_software, sizeof(ota_header->u.s.ver_software));
    str[sizeof(ota_header->u.s.ver_software)] = '\0';
    printf("[OTA] [HEADER] ver_software is ");
    printf(str);
    printf("\r\n");

    memcpy(str, ota_header->u.s.sha256, sizeof(ota_header->u.s.sha256));
    str[sizeof(ota_header->u.s.sha256)] = '\0';
    printf("[OTA] [HEADER] sha256 is ");
    for (i = 0; i < sizeof(ota_header->u.s.sha256); i++) {
        printf("%02X", str[i]);
    }
    printf("\r\n");

    return 0;
}

static void cb_httpc_result(void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err)
{   
    httpc_state_t **req = (httpc_state_t**)arg;
    
    printf("[HTTPC] Transfer finished. rx_content_len is %lu\r\n", rx_content_len);
    *req = NULL;
	xSemaphoreGive(otaSema);
	if(otaHandle != NULL){
        bl_mtd_close(otaHandle);
        if(otaBuffer != NULL) vPortFree(otaBuffer);
        utils_sha256_free(&otaCtx);
	}
}

static err_t cb_httpc_headers_done_fn(httpc_state_t *connection, void *arg, struct pbuf *hdr, u16_t hdr_len, u32_t content_len)
{
    unsigned int ota_addr;
    uint32_t bin_size, part_size;
    uint8_t activeID;

    printf("[HTTPC] hdr_len is %u, content_len is %lu\r\n", hdr_len, content_len);
    int ret = bl_mtd_open(BL_MTD_PARTITION_NAME_FW_DEFAULT, &otaHandle, BL_MTD_OPEN_FLAG_BACKUP);
    if (ret) {
        printf("Open Default FW partition failed\r\n");
		return ERR_OK;
    }
    activeID = hal_boot2_get_active_partition();

    if (hal_boot2_get_active_entries(BOOT2_PARTITION_TYPE_FW, &otaEntry)) {
        printf("PtTable_Get_Active_Entries fail\r\n");
        return ERR_OK;
    }
	ota_addr = otaEntry.Address[!otaEntry.activeIndex];
	bin_size = otaEntry.maxLen[!otaEntry.activeIndex];
	part_size = otaEntry.maxLen[!otaEntry.activeIndex];
	bl_mtd_erase_all(otaHandle);
	otaTotalLength = content_len;
	printf("otaTotalLength : %lu, bin_size : %lu\r\n", content_len, bin_size);
	if (otaTotalLength < bin_size){
		otaBuffer = pvPortMalloc(OTA_BUFFER_SIZE);
        utils_sha256_init(&otaCtx);
        utils_sha256_starts(&otaCtx);
        memset(otaSha256Result, 0, sizeof(otaSha256Result));
	}else{
        printf("otaTotalLength > bin_size abort\n");
        xSemaphoreGive(otaSema);
        if(otaHandle != NULL){
            bl_mtd_close(otaHandle);
        }

    }
    return ERR_OK;
}

static err_t cb_altcp_recv_fn(void *arg, struct altcp_pcb *conn, struct pbuf *p, err_t err)
{
    printf("[HTTPC] Received %u Bytes\r\n", p->tot_len);

	static bool otaHeader = true;
	static uint32_t flashOffset;
	static uint32_t otaOffset = 0;
	struct pbuf *lp = p;
	int use_xz;
	uint32_t ota_bin_size;

	if(otaBuffer != NULL){
		do{
			memcpy(otaBuffer+otaOffset, lp->payload, lp->len);
			otaOffset += lp->len;
			lp = lp->next;
		} while(lp != NULL);
		printf("otaoffset : %lu\r\n", otaOffset);

		if(otaHeader){
			if(otaOffset >= 512){
				check_ota_header((ota_header_t*)otaBuffer, &ota_bin_size, &use_xz);
				memcpy(otaSha256Img, ((ota_header_t*)otaBuffer)->u.s.sha256, sizeof(otaSha256Img));
				otaHeader = false;
				flashOffset = 0;
				for(int i=0; i<otaOffset-512; i++){
					otaBuffer[i] = otaBuffer[i+512];
				}
				otaOffset -= 512;
			}
		} else {
                utils_sha256_update(&otaCtx, otaBuffer, otaOffset);
                bl_mtd_write(otaHandle, flashOffset, otaOffset, otaBuffer);
                flashOffset += otaOffset;
                printf("Write %lu[%d]\r\n", flashOffset, otaTotalLength);
                otaOffset = 0;
                if ((flashOffset + 512) == otaTotalLength) {
                    utils_sha256_finish(&otaCtx, otaSha256Result);
                    printf("\r\nCalculated SHA256 Checksum:");
                    for (int i = 0; i < sizeof(otaSha256Result); i++) {
                        printf("%02X", otaSha256Result[i]);
                    }
                    printf("\r\nHeader SET SHA256 Checksum:");
                    for (int i = 0; i < sizeof(otaSha256Img); i++) {
                        printf("%02X", otaSha256Img[i]);
                    }
                    printf("\r\n");
                    if (memcmp(otaSha256Img, otaSha256Result, sizeof(otaSha256Img))) {
                        /*Error found*/
                        printf("[OTA] [TCP] SHA256 NOT Correct, OTA failed\r\n");
                    } else {
                        printf("[OTA] [TCP] prepare OTA partition info\r\n");
                        otaEntry.len = flashOffset;
                        printf("[OTA] [TCP] Update PARTITION, partition len is %lu\r\n", otaEntry.len);
                        hal_boot2_update_ptable(&otaEntry);
                        hal_reboot();
                    }
			}
		}
        altcp_recved(conn,p->tot_len);
        pbuf_free(p);
	}
    return ERR_OK;
}

static void ota_http_cmd(char *buf, int len, int argc, char **argv)
{
    static httpc_connection_t settings;
    static httpc_state_t *req;

    if (2 != argc) {
        printf("Usage: %s IP\r\n", argv[0]);
        return;
    }

    printf("ota host=%s\n",argv[1]);
   

	otaSema = xSemaphoreCreateBinary();

    memset(&settings, 0, sizeof(settings));
    settings.use_proxy = 0;
    settings.result_fn = cb_httpc_result;
    settings.headers_done_fn = cb_httpc_headers_done_fn;

    httpc_get_file_dns(
            argv[1],
            9090,
            "FW_OTA.bin.xz.ota",
            &settings,
            cb_altcp_recv_fn,
            &req,
            &req
	);

	xSemaphoreTake(otaSema, portMAX_DELAY);
	printf("ota download is done!\r\n");

	vSemaphoreDelete(otaSema);

	return;
}

static const struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"ota_http_cmd", "BL602_HTTP_OTA  port ", ota_http_cmd},
 
};

int bl_http_ota_cli_init(void)
{
    return 0;
}
