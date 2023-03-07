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
#include <stdio.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <lwip/mem.h>
#include <lwip/memp.h>
#include <lwip/dhcp.h>
#include <lwip/tcpip.h>
#include <lwip/ip_addr.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>

#ifdef CONFIG_CLI_CMD_ENABLE
#include <cli.h>
#endif
#include <hal_boot2.h>
#include <hal_sys.h>
#include <hosal_ota.h>
#include <utils_sha256.h>
#include <bl_sys_ota.h>
#include <bl_mtd.h>
#include <bl_wdt.h>

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

static int _check_ota_header(ota_header_t *ota_header, uint32_t *ota_len, int *use_xz)
{
    char str[33];//assume max segment size
    int i;

    memcpy(str, ota_header->u.s.header, sizeof(ota_header->u.s.header));
    str[sizeof(ota_header->u.s.header)] = '\0';
    puts("[OTA] [HEADER] ota header is ");
    puts(str);
    puts("\r\n");

    memcpy(str, ota_header->u.s.type, sizeof(ota_header->u.s.type));
    str[sizeof(ota_header->u.s.type)] = '\0';
    puts("[OTA] [HEADER] file type is ");
    puts(str);
    puts("\r\n");
    if (strstr(str, "XZ")) {
        *use_xz = 1;
    } else {
        *use_xz = 0;
    }

    memcpy(ota_len, &(ota_header->u.s.len), 4);
    printf("[OTA] [HEADER] file length (exclude ota header) is %lu\r\n", *ota_len);

    memcpy(str, ota_header->u.s.ver_hardware, sizeof(ota_header->u.s.ver_hardware));
    str[sizeof(ota_header->u.s.ver_hardware)] = '\0';
    puts("[OTA] [HEADER] ver_hardware is ");
    puts(str);
    puts("\r\n");

    memcpy(str, ota_header->u.s.ver_software, sizeof(ota_header->u.s.ver_software));
    str[sizeof(ota_header->u.s.ver_software)] = '\0';
    puts("[OTA] [HEADER] ver_software is ");
    puts(str);
    puts("\r\n");

    memcpy(str, ota_header->u.s.sha256, sizeof(ota_header->u.s.sha256));
    str[sizeof(ota_header->u.s.sha256)] = '\0';
    puts("[OTA] [HEADER] sha256 is ");
    for (i = 0; i < sizeof(ota_header->u.s.sha256); i++) {
        printf("%02X", str[i]);
    }
    puts("\r\n");

    return 0;
}

static void ota_tcp_api_cmd(char *buf, int len, int argc, char **argv)
{
    int sockfd;
    struct hostent *hostinfo;
    struct sockaddr_in dest;
    uint8_t *recv_buffer;
    
    if (3 != argc) {
        printf("Usage: %s IP\r\n", argv[0]);
        return;
    }
    hostinfo = gethostbyname(argv[1]);
    if (!hostinfo) {
        puts("gethostbyname Failed\r\n");
        return;
    }
   
    /* Create a socket */
    /*---Open socket for streaming---*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error in socket\r\n");
        return;
    }

    /*---Initialize server address/port struct---*/
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(OTA_TCP_PORT);
    dest.sin_addr = *((struct in_addr *) hostinfo->h_addr);
    uint32_t address = dest.sin_addr.s_addr;
    char *ip = inet_ntoa(address);

    int total = 0;
    int file_size = atoi(argv[2]);
    int ret;
    ret = hosal_ota_start(file_size);
    if (ret) {
        printf("start ota failed\r\n");
        return;
    }
    
    recv_buffer = pvPortMalloc(OTA_PROGRAM_SIZE);

    unsigned int buffer_offset = 0, flash_offset = 0;
    
    printf("Server ip Address : %s\r\n", ip); 
    /*---Connect to server---*/
    if (connect(sockfd, (struct sockaddr *)&dest, sizeof(dest)) != 0) {
        printf("Error in connect\r\n");
        close(sockfd);
        vPortFree(recv_buffer);
        return;
    }
    
    bl_wdt_disable();
    
    while (1) {
        /*first 512 bytes of TCP stream is OTA header*/
        ret = read(sockfd, recv_buffer + buffer_offset, OTA_PROGRAM_SIZE - buffer_offset);
        if (ret < 0) {
            printf("ret = %d, err = %d\n\r", ret, errno);
            break;
        } else {
            total += ret;
            if (0 == ret) {
                printf("[OTA] [TEST] seems ota file ends unexpectedly, already transfer %u\r\n", total);
                break;
            }
            printf("total = %d, ret = %d\n\r", total, ret);
            buffer_offset += ret;

            if (file_size != total) {
                if (buffer_offset < OTA_PROGRAM_SIZE) {
                    continue;
                } else if (buffer_offset > OTA_PROGRAM_SIZE) {
                    printf("[OTA] [TCP] Assert for unexpected error %d\r\n", buffer_offset);
                    while (1) {
                        /*empty*/
                    }
                }
            } else if (total > file_size) {
                printf("[OTA] [TCP] Server has bug?\r\n");
                while (1) {
                }
            }

            printf("Will Write %u to %08X from %p\r\n", buffer_offset, flash_offset, recv_buffer);
            ret = hosal_ota_update(file_size, flash_offset, recv_buffer, buffer_offset);        
            if (ret) {
                printf("update error\r\n");
                close(sockfd);
                vPortFree(recv_buffer);
                return;
            }
            flash_offset += buffer_offset;
            buffer_offset = 0;
            if (file_size == total) {
                close(sockfd);
                vPortFree(recv_buffer);
                ret = hosal_ota_finish(1, 1);    
                if (ret) {
                    printf("finish error\r\n");
                    close(sockfd);
                    vPortFree(recv_buffer);
                    return;
                }
            }
        }
    }
    close(sockfd);
    vPortFree(recv_buffer);
}

static void ota_tcp_cmd(char *buf, int len, int argc, char **argv)
{
    int sockfd, i;
    int ret;
    struct hostent *hostinfo;
    uint8_t *recv_buffer;
    struct sockaddr_in dest;
    iot_sha256_context ctx;
    uint8_t sha256_result[32];
    uint8_t sha256_img[32];
    bl_mtd_handle_t handle;

    if (2 != argc) {
        printf("Usage: %s IP\r\n", argv[0]);
        return;
    }
    hostinfo = gethostbyname(argv[1]);
    if (!hostinfo) {
        puts("gethostbyname Failed\r\n");
        return;
    }

    ret = bl_mtd_open(BL_MTD_PARTITION_NAME_FW_DEFAULT, &handle, BL_MTD_OPEN_FLAG_BACKUP);
    if (ret) {
        puts("Open Default FW partition failed\r\n");
        return;
    }

    /* Create a socket */
    /*---Open socket for streaming---*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error in socket\r\n");
        bl_mtd_close(handle);
        return;
    }

    /*---Initialize server address/port struct---*/
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(OTA_TCP_PORT);
    dest.sin_addr = *((struct in_addr *) hostinfo->h_addr);
    uint32_t address = dest.sin_addr.s_addr;
    char *ip = inet_ntoa(address);

    int total = 0;
    int ota_header_found, use_xz;
    ota_header_t *ota_header;

    recv_buffer = pvPortMalloc(OTA_PROGRAM_SIZE);

    unsigned int buffer_offset, flash_offset, ota_addr;
    uint32_t bin_size, part_size;
    uint8_t activeID;
    HALPartition_Entry_Config ptEntry;

    activeID = hal_boot2_get_active_partition();

    printf("Starting OTA test. OTA bin addr is %p\r\n", recv_buffer);

    printf("[OTA] [TEST] activeID is %u\r\n", activeID);

    if (hal_boot2_get_active_entries(BOOT2_PARTITION_TYPE_FW, &ptEntry)) {
        printf("PtTable_Get_Active_Entries fail\r\n");
        vPortFree(recv_buffer);
        bl_mtd_close(handle);
        return;
    }
    ota_addr = ptEntry.Address[!ptEntry.activeIndex];
    bin_size = ptEntry.maxLen[!ptEntry.activeIndex];
    part_size = ptEntry.maxLen[!ptEntry.activeIndex];
    (void) part_size;
    /*XXX if you use bin_size is product env, you may want to set bin_size to the actual
     * OTA BIN size, and also you need to splilt XIP_SFlash_Erase_With_Lock into
     * serveral pieces. Partition size vs bin_size check is also needed
     */
    printf("Starting OTA test. OTA size is %lu\r\n", bin_size);

    printf("[OTA] [TEST] activeIndex is %u, use OTA address=%08x\r\n", ptEntry.activeIndex, (unsigned int)ota_addr);

    printf("[OTA] [TEST] Erase flash with size %lu...", bin_size);
    hal_update_mfg_ptable();   
    bl_mtd_erase_all(handle);
    printf("Done\r\n");

    printf("Server ip Address : %s\r\n", ip);
    /*---Connect to server---*/
    if (connect(sockfd, (struct sockaddr *)&dest, sizeof(dest)) != 0) {
        printf("Error in connect\r\n");
        close(sockfd);
        vPortFree(recv_buffer);
        bl_mtd_close(handle);
        return;
    }
            
    buffer_offset = 0;
    flash_offset = 0;
    ota_header_found = 0;
    use_xz = 0;

    utils_sha256_init(&ctx);
    utils_sha256_starts(&ctx);
    memset(sha256_result, 0, sizeof(sha256_result));
    while (1) {
        /*first 512 bytes of TCP stream is OTA header*/
        ret = read(sockfd, recv_buffer + buffer_offset, OTA_PROGRAM_SIZE - buffer_offset);
        if (ret < 0) {
            printf("ret = %d, err = %d\n\r", ret, errno);
            break;
        } else {
            total += ret;
            if (0 == ret) {
                printf("[OTA] [TEST] seems ota file ends unexpectedly, already transfer %u\r\n", total);
                break;
            }
            printf("total = %d, ret = %d\n\r", total, ret);
            buffer_offset += ret;

            /*Only handle this case when ota header is NOT found*/
            if (0 == ota_header_found) {
                if (buffer_offset < OTA_PROGRAM_SIZE) {
                    continue;
                } else if (buffer_offset > OTA_PROGRAM_SIZE) {
                    printf("[OTA] [TCP] Assert for unexpected error %d\r\n", buffer_offset);
                    while (1) {
                        /*empty*/
                    }
                }
                /*ota_header is got, we assume alignment of recv_buffer is OK*/
                ota_header_found = 1;
                ota_header = (ota_header_t*)recv_buffer;
                if (_check_ota_header(ota_header, &bin_size, &use_xz)) {
                    /*ota header is NOT OK*/
                    break;
                }
                memcpy(sha256_img, ota_header->u.s.sha256, sizeof(sha256_img));
                /*we think OTA_PROGRAM_SIZE is the same OTA_HEADER_SIZE, and refix total to exclude OTA_PROGRAM_SIZE*/
                total -= OTA_PROGRAM_SIZE;
                buffer_offset = 0;
                printf("[OTA] [TCP] Update bin_size to %lu, file status %s\r\n", bin_size, use_xz ? "XZ" : "RAW");
                continue;
            }

            if (bin_size != total) {
                if (buffer_offset < OTA_PROGRAM_SIZE) {
                    continue;
                } else if (buffer_offset > OTA_PROGRAM_SIZE) {
                    printf("[OTA] [TCP] Assert for unexpected error %d\r\n", buffer_offset);
                    while (1) {
                        /*empty*/
                    }
                }
            } else if (total > bin_size) {
                printf("[OTA] [TCP] Server has bug?\r\n");
                while (1) {
                }
            }

            printf("Will Write %u to %08X from %p left %lu.\r\n", buffer_offset, ota_addr + flash_offset, recv_buffer, bin_size-total);
            utils_sha256_update(&ctx, recv_buffer, buffer_offset);
            bl_mtd_write(handle, flash_offset, buffer_offset, recv_buffer);
            flash_offset += buffer_offset;
            buffer_offset = 0;
            if (bin_size == total) {
                utils_sha256_finish(&ctx, sha256_result);
                puts("\r\nCalculated SHA256 Checksum:");
                for (i = 0; i < sizeof(sha256_result); i++) {
                    printf("%02X", sha256_result[i]);
                }
                puts("\r\nHeader SET SHA256 Checksum:");
                for (i = 0; i < sizeof(sha256_img); i++) {
                    printf("%02X", sha256_img[i]);
                }
                puts("\r\n");
                if (memcmp(sha256_img, sha256_result, sizeof(sha256_img))) {
                    /*Error found*/
                    printf("[OTA] [TCP] SHA256 NOT Correct\r\n");
                    break;
                }
                printf("[OTA] [TCP] prepare OTA partition info\r\n");
                ptEntry.len = bin_size;
                printf("[OTA] [TCP] Update PARTITION, partition len is %lu\r\n", ptEntry.len);
                hal_boot2_update_ptable(&ptEntry);
                printf("[OTA] [TCP] Rebooting\r\n");
                close(sockfd);
                vTaskDelay(1000);
                hal_reboot();
            }
        }
    }


    /*---Clean up---*/
    close(sockfd);
    vPortFree(recv_buffer);
    utils_sha256_free(&ctx);
    bl_mtd_close(handle);

    return;
}

void ota_tcp_server_handle(void)
{
    int sockfd, i;
    int ret;
    uint8_t *recv_buffer;
    iot_sha256_context ctx;
    uint8_t sha256_result[32];
    uint8_t sha256_img[32];
    bl_mtd_handle_t handle;

#if !LWIP_CONFIG_ENABLE_IPV4
    struct sockaddr_in6 server_addr, client_addr;
#else
    struct sockaddr_in server_addr, client_addr;
#endif
    int connected;
    uint32_t sin_size;

    ret = bl_mtd_open(BL_MTD_PARTITION_NAME_FW_DEFAULT, &handle, BL_MTD_OPEN_FLAG_BACKUP);
    if (ret) {
        puts("Open Default FW partition failed\r\n");
        return;
    }

#if !LWIP_CONFIG_ENABLE_IPV4
    if ((sockfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        printf("Error in socket\r\n");
        bl_mtd_close(handle);
        return;
    }
#else
    /* Create a socket */
    /*---Open socket for streaming---*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error in socket\r\n");
        bl_mtd_close(handle);
        return;
    }
#endif

#if !LWIP_CONFIG_ENABLE_IPV4
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_port = htons(OTA_TCP_PORT);
#else
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(OTA_TCP_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), 0x0, sizeof(server_addr.sin_zero));
#endif

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        printf("Unable to bind\r\n");
        return;
    }

    printf("bind success.\r\n");

    int total = 0;
    int ota_header_found, use_xz;
    ota_header_t *ota_header;

    recv_buffer = pvPortMalloc(OTA_PROGRAM_SIZE);

    unsigned int buffer_offset, flash_offset, ota_addr;
    uint32_t bin_size, part_size;
    uint8_t activeID;
    HALPartition_Entry_Config ptEntry;

    activeID = hal_boot2_get_active_partition();

    printf("Starting OTA test. OTA bin addr is %p\r\n", recv_buffer);

    printf("[OTA] [TEST] activeID is %u\r\n", activeID);

    if (hal_boot2_get_active_entries(BOOT2_PARTITION_TYPE_FW, &ptEntry)) {
        printf("PtTable_Get_Active_Entries fail\r\n");
        vPortFree(recv_buffer);
        bl_mtd_close(handle);
        return;
    }
    ota_addr = ptEntry.Address[!ptEntry.activeIndex];
    bin_size = ptEntry.maxLen[!ptEntry.activeIndex];
    part_size = ptEntry.maxLen[!ptEntry.activeIndex];
    (void)part_size;
    /*XXX if you use bin_size is product env, you may want to set bin_size to the actual
     * OTA BIN size, and also you need to splilt XIP_SFlash_Erase_With_Lock into
     * serveral pieces. Partition size vs bin_size check is also needed
     */
    printf("Starting OTA test. OTA size is %lu\r\n", bin_size);

    printf("[OTA] [TEST] activeIndex is %u, use OTA address=%08x\r\n", ptEntry.activeIndex, (unsigned int)ota_addr);

    printf("[OTA] [TEST] Erase flash with size %lu...", bin_size);
    hal_update_mfg_ptable();
    bl_mtd_erase_all(handle);
    printf("Done\r\n");

    if (listen(sockfd, 5) == -1) {
        printf("Listen error\r\n");
        return;
    }
    printf("listen success.\r\n");
    sin_size = sizeof(struct sockaddr_in);

    buffer_offset = 0;
    flash_offset = 0;
    ota_header_found = 0;
    use_xz = 0;

    utils_sha256_init(&ctx);
    utils_sha256_starts(&ctx);
    memset(sha256_result, 0, sizeof(sha256_result));

    memset(&client_addr, 0, sizeof(client_addr));
    errno = 0;
    while (1) {
        connected = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t *)&sin_size);
#if !LWIP_CONFIG_ENABLE_IPV4
        printf("IPV6 new client connected from (%s: %d)\r\n", inet_ntoa(client_addr.sin6_addr), ntohs(client_addr.sin6_port));
#else
        printf("IPV4 new client connected from (%s: %d)\r\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
#endif
        int flag = 1;
        setsockopt(connected,
                   IPPROTO_TCP,   /* set option at TCP level */
                   TCP_NODELAY,   /* name of option */
                   (void *)&flag, /* the cast is historical cruft */
                   sizeof(int));  /* length of option value */

        while (1) {
            /*first 512 bytes of TCP stream is OTA header*/
            ret = recv(connected, recv_buffer + buffer_offset, OTA_PROGRAM_SIZE - buffer_offset, 0);

            if (ret < 0) {
                printf("ret = %d, err = %d\n\r", ret, errno);
                break;
            } else {
                total += ret;
                if (0 == ret) {
                    printf("[OTA] [TEST] seems ota file ends unexpectedly, already transfer %u\r\n", total);
                    break;
                }
                printf("total = %d, ret = %d\n\r", total, ret);
                buffer_offset += ret;

                /*Only handle this case when ota header is NOT found*/
                if (0 == ota_header_found) {
                    if (buffer_offset < OTA_PROGRAM_SIZE) {
                        continue;
                    } else if (buffer_offset > OTA_PROGRAM_SIZE) {
                        printf("[OTA] [TCP] Assert for unexpected error %d\r\n", buffer_offset);
                        while (1) {
                            /*empty*/
                        }
                    }
                    /*ota_header is got, we assume alignment of recv_buffer is OK*/
                    ota_header_found = 1;
                    ota_header = (ota_header_t *)recv_buffer;
                    if (_check_ota_header(ota_header, &bin_size, &use_xz)) {
                        /*ota header is NOT OK*/
                        break;
                    }
                    memcpy(sha256_img, ota_header->u.s.sha256, sizeof(sha256_img));
                    /*we think OTA_PROGRAM_SIZE is the same OTA_HEADER_SIZE, and refix total to exclude OTA_PROGRAM_SIZE*/
                    total -= OTA_PROGRAM_SIZE;
                    buffer_offset = 0;
                    printf("[OTA] [TCP] Update bin_size to %lu, file status %s\r\n", bin_size, use_xz ? "XZ" : "RAW");
                    continue;
                }

                if (bin_size != total) {
                    if (buffer_offset < OTA_PROGRAM_SIZE) {
                        continue;
                    } else if (buffer_offset > OTA_PROGRAM_SIZE) {
                        printf("[OTA] [TCP] Assert for unexpected error %d\r\n", buffer_offset);
                        while (1) {
                            /*empty*/
                        }
                    }
                } else if (total > bin_size) {
                    printf("[OTA] [TCP] Server has bug?\r\n");
                    while (1) {
                    }
                }

                printf("Will Write %u to %08X from %p letf %lu bytes.\r\n", buffer_offset, ota_addr + flash_offset, recv_buffer, bin_size-total);
                utils_sha256_update(&ctx, recv_buffer, buffer_offset);
                bl_mtd_write(handle, flash_offset, buffer_offset, recv_buffer);
                flash_offset += buffer_offset;
                buffer_offset = 0;
                if (bin_size == total) {
                    utils_sha256_finish(&ctx, sha256_result);
                    puts("\r\nCalculated SHA256 Checksum:");
                    for (i = 0; i < sizeof(sha256_result); i++) {
                        printf("%02X", sha256_result[i]);
                    }
                    puts("\r\nHeader SET SHA256 Checksum:");
                    for (i = 0; i < sizeof(sha256_img); i++) {
                        printf("%02X", sha256_img[i]);
                    }
                    puts("\r\n");
                    if (memcmp(sha256_img, sha256_result, sizeof(sha256_img))) {
                        /*Error found*/
                        printf("[OTA] [TCP] SHA256 NOT Correct\r\n");
                        break;
                    }
                    printf("[OTA] [TCP] prepare OTA partition info\r\n");
                    ptEntry.len = bin_size;
                    printf("[OTA] [TCP] Update PARTITION, partition len is %lu\r\n", ptEntry.len);
                    hal_boot2_update_ptable(&ptEntry);
                    printf("[OTA] [TCP] Rebooting\r\n");
                    close(sockfd);
                    vTaskDelay(1000);
                    hal_reboot();
                }
            }
        }
    }

    /*---Clean up---*/
    close(sockfd);
    vPortFree(recv_buffer);
    utils_sha256_free(&ctx);
    bl_mtd_close(handle);

    return;
}

static void ota_tcp_server(char *buf, int len, int argc, char **argv)
{
    xTaskCreate(ota_tcp_server_handle, (char*)"ota_tcp_server", 1024, NULL, 15, NULL);
}

static void ota_dump_cmd(char *buf, int len, int argc, char **argv)
{
    hal_boot2_dump();
}

#ifdef CONFIG_CLI_CMD_ENABLE
// STATIC_CLI_CMD_ATTRIBUTE makes this(these) command(s) static
static const struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
    {"ota_tcp_api", "OTA from TCP server port 3333", ota_tcp_api_cmd},
    {"ota_tcp", "OTA from TCP server port 3333", ota_tcp_cmd},
    {"ota_tcp_server", "start OTA TCP server port 3333", ota_tcp_server},
    {"ota_dump", "dump partitions for ota related", ota_dump_cmd},
};
#endif

int bl_sys_ota_cli_init(void)
{
    // static command(s) do NOT need to call aos_cli_register_command(s) to register.
    // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
    // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
    //return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));          
    return 0;
}
