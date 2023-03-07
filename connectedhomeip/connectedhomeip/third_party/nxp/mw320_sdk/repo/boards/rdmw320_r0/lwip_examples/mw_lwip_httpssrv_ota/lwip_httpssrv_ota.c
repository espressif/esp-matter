/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#include "lwip/api.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"

#include "ksdk_mbedtls.h"
#include "httpsrv.h"
#include "httpsrv_port.h"

#include "mbedtls/certs.h"

#include "mflash_drv.h"
#include "partition.h"
#include "boot_flags.h"
#include "crc32.h"

#include "timers.h"

#include "fsl_aes.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/


#ifndef HTTPD_DEBUG
#define HTTPD_DEBUG LWIP_DBG_ON
#endif
#ifndef HTTPD_STACKSIZE
#define HTTPD_STACKSIZE 2048
#endif
#ifndef HTTPD_PRIORITY
#define HTTPD_PRIORITY DEFAULT_THREAD_PRIO
#endif
#ifndef DEBUG_WS
#define DEBUG_WS 0
#endif

#define OTA_TEST_MODE_FLAG  (1UL << 31U)
#define OTA_UPDATE_PART     (BL_FEATURE_SECONDARY_IMG_START - FlexSPI_AMBA_BASE)
#define OTA_MAX_IMAGE_SIZE  (BL_FEATURE_PRIMARY_IMG_PARTITION_SIZE - BL_IMG_HEADER_SIZE)
#define OTA_IMAGE_LOAD_ADDR (BL_FEATURE_PRIMARY_IMG_START + BL_IMG_HEADER_SIZE)

#define FW_MAGIC_STR (('M' << 0) | ('R' << 8) | ('V' << 16) | ('L' << 24))
#define FW_MAGIC_SIG ((0x7BUL << 0) | (0xF1UL << 8) | (0x9CUL << 16) | (0x2EUL << 24))

/*
 * Firmware magic signature
 *
 * First word is the string "MRVL" and is endian invariant.
 * Second word = magic value 0x2e9cf17b.
 * Third word = time stamp (seconds since 00:00:00 UTC, January 1, 1970).
 */
struct img_hdr
{
    uint32_t magic_str;
    uint32_t magic_sig;
    uint32_t time;
    uint32_t seg_cnt;
    uint32_t entry;
};

/* Maximum number of segments */
#define SEG_CNT 9

struct seg_hdr
{
    uint32_t type;
    uint32_t offset;
    uint32_t len;
    uint32_t laddr;
    uint32_t crc;
};

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

static int cgi_ota_upload(HTTPSRV_CGI_REQ_STRUCT *param);
static int cgi_ota_reboot(HTTPSRV_CGI_REQ_STRUCT *param);
static int cgi_ota_accept(HTTPSRV_CGI_REQ_STRUCT *param);

static int ssi_ota_status(HTTPSRV_SSI_PARAM_STRUCT *param);
static int ssi_disabled_input(HTTPSRV_SSI_PARAM_STRUCT *param);

/*******************************************************************************
 * Variables
 ******************************************************************************/

extern const HTTPSRV_FS_DIR_ENTRY httpsrv_fs_data[];
static struct partition_entry *part_entry[2]; /* part_entry[0] stores active partition. */

const HTTPSRV_CGI_LINK_STRUCT cgi_lnk_tbl[] = {
    {"upload", cgi_ota_upload},
    {"reboot", cgi_ota_reboot},
    {"accept", cgi_ota_accept},
    {0, 0} // DO NOT REMOVE - last item - end of table
};

const HTTPSRV_SSI_LINK_STRUCT ssi_lnk_tbl[] = {
    {"ota_status", ssi_ota_status},
    {"disabled_input", ssi_disabled_input},
    {0, 0} // DO NOT REMOVE - last item - end of table
};

/*******************************************************************************
 * Code
 ******************************************************************************/

enum ota_status_t
{
    OTA_STATUS_NONE = 0,
    OTA_STATUS_UPLOADED,
    OTA_STATUS_TESTING,
    OTA_STATUS_BOOTLOADER_ERROR,
} ota_status = OTA_STATUS_NONE;

char *ota_status_strings[] = {
    "Select file with updated firmware and click <b>Upload</b>.",
    "Update image uploaded, click <b>Reboot</b> to start in test mode.",
    "Running in test mode, click <b>Accept update</b> to make it permanent or <br>Reboot</b> for rollback.",
    "<b>Error</b> - check bootloader installation.",
};

/* Server Side Include callback for OTA status. */
static int ssi_ota_status(HTTPSRV_SSI_PARAM_STRUCT *param)
{
    char *status_string = ota_status_strings[ota_status];
    HTTPSRV_ssi_write(param->ses_handle, status_string, strlen(status_string));
    return (0);
}

/* Server Side Include callback to disable inputs that are of no use at the moment. */
static int ssi_disabled_input(HTTPSRV_SSI_PARAM_STRUCT *param)
{
    int disabled = 0;

    if (strcmp(param->com_param, "accept") == 0)
    {
        disabled = (ota_status != OTA_STATUS_TESTING);
    }

    if (disabled)
    {
        HTTPSRV_ssi_write(param->ses_handle, "disabled", strlen("disabled"));
    }

    return (0);
}

#define MULTIPART_READ_BUFSIZE (200)
#define MULTIPART_MAX_BOUNDARY (70)
#define FORM_DATA_NAME_SIZE    (20)

enum multipart_state
{
    MULTIPART_END = 0,
    MULTIPART_EXPECT_HEADERS,
    MULTIPART_EXPECT_DATA,
    MULTIPART_ERROR,
};

struct multipart_read_ctx
{
    uint32_t ses_handle;

    enum multipart_state state;

    /* Boundary string is 1..70 chars long. Keep 2 more reserved for CRLF */
    char boundary[MULTIPART_MAX_BOUNDARY + 2];
    uint32_t boundary_len; /* Actual lenght of the stored boundary string */

    char buffer[MULTIPART_READ_BUFSIZE];
    uint32_t buf_size; /* Size of the buffer (allocated space), this is to allow for possible dynamic allocation of the
                          buffer */
    char *buf_start;   /* Pointer to valid (not consumed so far) data in the buffer */
    char
        *buf_end; /* Points to location following the valid data, i.e. it may point one byte past the allocated space */

    char form_data_name[FORM_DATA_NAME_SIZE + 1]; /* Extra char for null termination */
};

static int multipart_read_init(struct multipart_read_ctx *ctx, uint32_t ses_handle)
{
    int read;

    memset(ctx, 0, sizeof(*ctx));

    ctx->ses_handle = ses_handle;
    ctx->buf_size   = MULTIPART_READ_BUFSIZE;
    ctx->buf_end = ctx->buf_start = ctx->buffer;

    /* Fill in the buffer with data */
    read = HTTPSRV_cgi_read(ctx->ses_handle, ctx->buffer, ctx->buf_size);
    if (read <= 0)
    {
        ctx->state = MULTIPART_ERROR;
        return -1;
    }
    ctx->buf_end += read;

    /* Determine length of boundary string by scanning its first occurence */
    while (ctx->buf_start < ctx->buf_end)
    {
        if (*ctx->buf_start == ' ' || *ctx->buf_start == '\r')
        {
            /* End of boundary string */
            break;
        }
        ctx->boundary_len++;
        ctx->buf_start++;
    }

    if (ctx->buf_start == ctx->buf_end)
    {
        /* End of buffer reached while boundary end was not found */
        ctx->state = MULTIPART_ERROR;
        return -1;
    }

    if (ctx->boundary_len < 1 || ctx->boundary_len > 70)
    {
        /* Length of  boundary string is out of spec */
        ctx->state = MULTIPART_ERROR;
        return -1;
    }

    /*  RFC: The boundary delimiter MUST occur at the beginning of a line.
        Implementation: Use 2 reserved bytes to prepend boundary with CRLF for convenient matching using a state machine
     */
    ctx->boundary[0] = '\r';
    ctx->boundary[1] = '\n';
    memcpy(ctx->boundary + 2, ctx->buffer, ctx->boundary_len);
    ctx->boundary_len += 2;

    /* There may be whitespaces at the end of boundary line, skip them */
    while (ctx->buf_start < ctx->buf_end && *ctx->buf_start == ' ')
    {
        ctx->buf_start++;
    }

    /* There should be at least 2 chars for line termination remaining */
    if (ctx->buf_end - ctx->buf_start < 2)
    {
        /* Either the buffer is small to fit a single line or we reached the end of the stream */
        ctx->state = MULTIPART_ERROR;
        return -1;
    }

    /* Consume CRLF at the end to the boundary line */
    if (ctx->buf_start[0] != '\r' || ctx->buf_start[1] != '\n')
    {
        ctx->state = MULTIPART_ERROR;
        return -1;
    }
    ctx->buf_start += 2;

    /* Expect headers */
    ctx->state = MULTIPART_EXPECT_HEADERS;

    return 0;
}

static int multipart_proc_header(struct multipart_read_ctx *ctx, char *buffer)
{
    char *param_ptr;

    if (strncmp(buffer, "Content-Disposition:", 20) == 0)
    {
        param_ptr = buffer + 20;
        if (strstr(param_ptr, " form-data;"))
        {
            char *name_ptr;
            char *end_ptr;

            name_ptr = strstr(param_ptr, " name=\"");
            if (name_ptr)
            {
                name_ptr += 7;
                end_ptr = strchr(name_ptr, '\"');
                if (end_ptr)
                {
                    int len = (end_ptr - name_ptr) < FORM_DATA_NAME_SIZE ? (end_ptr - name_ptr) : FORM_DATA_NAME_SIZE;
                    strncpy(ctx->form_data_name, name_ptr, len);
                }
            }
        }
    }
    return 0;
}

static void multipart_clear_headers(struct multipart_read_ctx *ctx)
{
    memset(ctx->form_data_name, 0, sizeof(ctx->form_data_name));
}

static int multipart_read_headers(struct multipart_read_ctx *ctx)
{
    char *line_start;
    char *line_lf;
    int read;
    int num_headers = 0;

    if (ctx->state != MULTIPART_EXPECT_HEADERS)
    {
        return 0;
    }

    multipart_clear_headers(ctx);

    /* Process buffer line by line. End of line is \n or \r\n */
    while (1)
    {
        line_start = ctx->buf_start;
        line_lf    = memchr(line_start, '\n', ctx->buf_end - ctx->buf_start);

        if (line_lf == NULL)
        {
            /* No end of line found in the buffer */
            if (ctx->buf_end - ctx->buf_start == ctx->buf_size)
            {
                /* The buffer is full but probably not large enough to keep the whole header line */
                ctx->state = MULTIPART_ERROR;
                return -1;
            }

            /* Move unprocessed data to the beginning of the buffer */
            memmove(ctx->buffer, ctx->buf_start, ctx->buf_end - ctx->buf_start);
            ctx->buf_end -= ctx->buf_start - ctx->buffer;
            ctx->buf_start = ctx->buffer;

            /* Top up the buffer */
            read = HTTPSRV_cgi_read(ctx->ses_handle, ctx->buf_end, ctx->buf_size - (ctx->buf_end - ctx->buf_start));
            if (read == 0)
            {
                /* End od stream */
                ctx->state = MULTIPART_ERROR;
                return -1;
            }
            ctx->buf_end += read;

            /* And restart parsing */
            continue;
        }

        /* Null terminate the line */
        *line_lf = '\0';
        if ((line_lf > line_start) && (*(line_lf - 1) == '\r'))
        {
            /* Discard optional CR */
            *(line_lf - 1) = '\0';
        }

        /* Move start of valid data in the buffer according to data consumed */
        ctx->buf_start = line_lf + 1;

        /* Empty line indicates end of headers */
        if (*line_start == '\0')
        {
            break;
        }

        /* Process the header */
        multipart_proc_header(ctx, line_start);
        num_headers++;
    }

    ctx->state = MULTIPART_EXPECT_DATA;
    return num_headers;
}

static int32_t multipart_read_data(struct multipart_read_ctx *ctx, uint8_t *buffer, int32_t len)
{
    int match_idx  = 0;
    int read_total = 0;

    if (ctx->state != MULTIPART_EXPECT_DATA)
    {
        return 0;
    }

    while (read_total != len)
    {
        if (ctx->buf_start + match_idx >= ctx->buf_end)
        {
            /* Boundary matching is beyond buffer end */
            uint32_t read;

            /* Move unprocessed data to the beginning of the buffer */
            memmove(ctx->buffer, ctx->buf_start, ctx->buf_end - ctx->buf_start);
            ctx->buf_end -= ctx->buf_start - ctx->buffer;
            ctx->buf_start = ctx->buffer;

            /* Top up the buffer */
            read = HTTPSRV_cgi_read(ctx->ses_handle, ctx->buf_end, ctx->buf_size - (ctx->buf_end - ctx->buf_start));
            if (read == 0)
            {
                /* End od stream unexpected at this point */
                ctx->state = MULTIPART_ERROR;
                break;
            }
            ctx->buf_end += read;
        }

        if (ctx->buf_start[match_idx] == ctx->boundary[match_idx])
        {
            if (++match_idx == ctx->boundary_len)
            {
                /* Boundary found, consume it and exit the loop*/
                ctx->buf_start += match_idx;
                break;
            }
            continue;
        }

        /* Mismatch, reset matching index */
        match_idx = 0;

        /* Copy character to the caller provided buffer */
        if (buffer != NULL)
        {
            *buffer++ = *ctx->buf_start;
        }
        ctx->buf_start++;

        read_total++;
    }

    if (match_idx == ctx->boundary_len)
    {
        /* Boundary was matched, presume that headers will follow unless further reading of the stream indicates the
         * processing should stop */
        ctx->state = MULTIPART_EXPECT_HEADERS;

        /* For simplicity of implementation, the closing double dash of last boundary is not strictly required.
           Just read until encountering single dash, LF or end of stream.  */
        do
        {
            /* Make sure the buffer is not empty */
            if (ctx->buf_end == ctx->buf_start)
            {
                uint32_t read;
                ctx->buf_start = ctx->buf_end = ctx->buffer;
                read                          = HTTPSRV_cgi_read(ctx->ses_handle, ctx->buffer, ctx->buf_size);
                if (read == 0)
                {
                    /* End od stream unexpected at this point */
                    ctx->state = MULTIPART_ERROR;
                    break;
                }
                ctx->buf_end += read;
            }
            if (*ctx->buf_start == '-')
            {
                /* Dash found, assume end of multipart content, rest of the buffer will be ignored */
                ctx->state = MULTIPART_END;
                break;
            }
        } while (*ctx->buf_start++ != '\n');
    }

    return (read_total);
}

static uint32_t calculate_image_crc(uint32_t flash_addr, uint32_t size)
{
    int32_t result;
    uint32_t buf[32];
    uint32_t addr = flash_addr;
    uint32_t crc  = 0U;

    for (addr = flash_addr; addr < flash_addr + size - sizeof(buf); addr += sizeof(buf))
    {
        result = mflash_drv_read(addr, buf, sizeof(buf));
        if (result != kStatus_Success)
        {
            assert(false);
        }
        crc = soft_crc32(buf, sizeof(buf), crc);
    }

    /* Remaining data */
    result = mflash_drv_read(addr, buf, flash_addr + size - addr);
    if (result != kStatus_Success)
    {
        assert(false);
    }
    crc = soft_crc32(buf, flash_addr + size - addr, crc);

    return crc;
}

int32_t validate_update_image(uint32_t flash_addr, uint32_t size)
{
    struct img_hdr ih;
    struct seg_hdr sh;
    int32_t result;

    if (size < (sizeof(ih) + sizeof(sh)))
    {
        return -1;
    }

    result = mflash_drv_read(flash_addr, (uint32_t *)&ih, sizeof(ih));
    if (result != kStatus_Success)
    {
        assert(false);
    }

    /* MCUXpresso SDK image has only 1 segment */
    if ((ih.magic_str != FW_MAGIC_STR) || (ih.magic_sig != FW_MAGIC_SIG) || ih.seg_cnt != 1U)
    {
        return -1;
    }

    result = mflash_drv_read(flash_addr + sizeof(ih), (uint32_t *)&sh, sizeof(sh));
    if (result != kStatus_Success)
    {
        assert(false);
    }

    /* Image size should just cover segment end. */
    if (sh.len + sh.offset != size)
    {
        return -1;
    }

    if (calculate_image_crc(flash_addr + sh.offset, sh.len) != sh.crc)
    {
        return -1;
    }

    return 0;
}

int32_t store_update_image(struct multipart_read_ctx *ctx, uint32_t flash_addr)
{
    int32_t result;

    uint8_t buffer[512];
    uint32_t chunk_addr = flash_addr;
    uint32_t chunk_len;

    uint32_t total_stored = 0;

    do
    {
        chunk_len = multipart_read_data(ctx, buffer, sizeof(buffer));
        if (chunk_len > 0)
        {
            result = mflash_drv_write(chunk_addr, (uint32_t *)buffer, chunk_len);
            if (result != 0)
            {
                /* Error during flash operation */
                return -1;
            }
            chunk_addr += chunk_len;
            total_stored += chunk_len;
        }

    } while (chunk_len > 0);

    return total_stored;
}

/* Common Gateway Interface callback for OTA update. */
static int cgi_ota_upload(HTTPSRV_CGI_REQ_STRUCT *param)
{
    struct multipart_read_ctx *mpr_ctx;

    HTTPSRV_CGI_RES_STRUCT response = {0};
    response.ses_handle             = param->ses_handle;
    response.status_code            = HTTPSRV_CODE_OK;

    if (param->request_method == HTTPSRV_REQ_POST)
    {
        mpr_ctx = (struct multipart_read_ctx *)httpsrv_mem_alloc(sizeof(struct multipart_read_ctx));
        if (mpr_ctx == NULL)
        {
            response.status_code = HTTPSRV_CODE_INTERNAL_ERROR;
            return (response.content_length);
        }

        multipart_read_init(mpr_ctx, param->ses_handle);

        while ((mpr_ctx->state == MULTIPART_EXPECT_HEADERS) && (response.status_code == HTTPSRV_CODE_OK))
        {
            int headers;
            headers = multipart_read_headers(mpr_ctx);
            if (headers <= 0)
            {
                response.status_code = HTTPSRV_CODE_BAD_REQ;
                break;
            }
            if (0 == strcmp(mpr_ctx->form_data_name, "update_file"))
            {
                int32_t stored;

                if (mflash_drv_erase(part_entry[1]->start, part_entry[1]->size) != kStatus_Success)
                {
                    response.status_code = HTTPSRV_CODE_INTERNAL_ERROR;
                }
                else if ((stored = store_update_image(mpr_ctx, part_entry[1]->start)) <= 0)
                {
                    /* Error during upload */
                    response.status_code = HTTPSRV_CODE_INTERNAL_ERROR;
                }
                else if (validate_update_image(part_entry[1]->start, stored) < 0)
                {
                    /* Image validation failed */
                    response.status_code = HTTPSRV_CODE_INTERNAL_ERROR;
                }
                else
                {
                    /* Image ok */
                    ota_status = OTA_STATUS_UPLOADED;
                }
            }
            else
            {
                /* Discard unknown multipart data block */
                multipart_read_data(mpr_ctx, NULL, -1);
            }
        }

        httpsrv_mem_free(mpr_ctx);

        /* Write the response using chunked transmission coding. */
        response.content_type = HTTPSRV_CONTENT_TYPE_HTML;
        /* Set content length to -1 to indicate unknown content length. */
        response.content_length = -1;
        response.data           = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">";
        response.data_length    = strlen(response.data);
        HTTPSRV_cgi_write(&response);

        if (response.status_code == HTTPSRV_CODE_OK)
        {
            response.data        = "<html><head><title>File upload successfull!</title>";
            response.data_length = strlen(response.data);
        }
        else
        {
            response.data        = "<html><head><title>File upload failed!</title>";
            response.data_length = strlen(response.data);
        }

        HTTPSRV_cgi_write(&response);
        response.data        = "<meta http-equiv=\"refresh\" content=\"0; url=ota.shtml\"></head><body></body></html>";
        response.data_length = strlen(response.data);
        HTTPSRV_cgi_write(&response);
        response.data_length = 0;
        HTTPSRV_cgi_write(&response);
    }

    return (response.content_length);
}

void reboot_timer_callback(TimerHandle_t timer)
{
    NVIC_SystemReset();
}

/* Common Gateway Interface callback for device reboot. */
static int cgi_ota_reboot(HTTPSRV_CGI_REQ_STRUCT *param)
{
    /* Static variable is used to avoid repetitive creation of the timer as that it expected to happen just once (reboot
     * follows) */
    static TimerHandle_t reboot_timer = NULL;
    int result;

    HTTPSRV_CGI_RES_STRUCT response = {0};
    response.ses_handle             = param->ses_handle;
    response.status_code            = HTTPSRV_CODE_OK;

    /* Write the response using chunked transmission coding. */
    response.content_type = HTTPSRV_CONTENT_TYPE_HTML;
    /* Set content length to -1 to indicate unknown content length. */
    response.content_length = -1;
    response.data           = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">";
    response.data_length    = strlen(response.data);
    HTTPSRV_cgi_write(&response);
    response.data        = "<html><head><title>Going down for reboot</title>";
    response.data_length = strlen(response.data);
    HTTPSRV_cgi_write(&response);
    response.data = "<meta http-equiv=\"refresh\" content=\"0; url=ota_reboot.html\"></head><body></body></html>";
    response.data_length = strlen(response.data);
    HTTPSRV_cgi_write(&response);
    response.data_length = 0;
    HTTPSRV_cgi_write(&response);

    if (ota_status == OTA_STATUS_UPLOADED)
    {
        /* There is an update waiting, instruct bootloader to test it */
        part_entry[1]->gen_level = OTA_TEST_MODE_FLAG;
        result                   = part_write_layout();
        PRINTF("update new image gen_level to [0x%x], result %d\r\n", OTA_TEST_MODE_FLAG, result);
    }

    if (reboot_timer == NULL)
    {
        /* Actual reboot is delayed to give the HTTP server a chance to send the content generated by CGI and gracefully
         * close the connection */
        reboot_timer = xTimerCreate("reboot_timer", pdMS_TO_TICKS(5000), pdFALSE, NULL, reboot_timer_callback);
        xTimerStart(reboot_timer, 0);
    }

    return (response.content_length);
}

/* Common Gateway Interface callback for accepting the update. */
static int cgi_ota_accept(HTTPSRV_CGI_REQ_STRUCT *param)
{
    HTTPSRV_CGI_RES_STRUCT response = {0};
    int result;

    response.ses_handle  = param->ses_handle;
    response.status_code = HTTPSRV_CODE_OK;

    /* Write the response using chunked transmission coding. */
    response.content_type = HTTPSRV_CONTENT_TYPE_HTML;
    /* Set content length to -1 to indicate unknown content length. */
    response.content_length = -1;
    response.data           = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0 Transitional//EN\">";
    response.data_length    = strlen(response.data);
    HTTPSRV_cgi_write(&response);
    response.data        = "<html><head><title>Accept update</title>";
    response.data_length = strlen(response.data);
    HTTPSRV_cgi_write(&response);
    response.data        = "<meta http-equiv=\"refresh\" content=\"0; url=ota.shtml\"></head><body></body></html>";
    response.data_length = strlen(response.data);
    HTTPSRV_cgi_write(&response);
    response.data_length = 0;
    HTTPSRV_cgi_write(&response);

    if (ota_status == OTA_STATUS_TESTING)
    {
        /* There is an update under test, make it bootable. */
        part_entry[0]->gen_level = part_entry[1]->gen_level + 1U;
        result                   = part_write_layout();
        PRINTF("update new image gen_level to [%d], result %d\r\n", part_entry[0]->gen_level, result);
        ota_status = OTA_STATUS_NONE;
    }

    return (response.content_length);
}

#if HTTPSRV_CFG_MBEDTLS_ENABLE
static HTTPSRV_TLS_PARAM_STRUCT tls_params;
#endif

/*!
 * @brief Initializes server.
 */
static void http_server_socket_init(void)
{
    HTTPSRV_PARAM_STRUCT params;
    uint32_t httpsrv_handle;

    /* Init Fs*/
    HTTPSRV_FS_init(httpsrv_fs_data);

    /* Init HTTPSRV parameters.*/
    memset(&params, 0, sizeof(params));
    params.root_dir   = "";
    params.index_page = "/index.html";
    /* params.auth_table  = auth_realms; */
    params.cgi_lnk_tbl = cgi_lnk_tbl;
    params.ssi_lnk_tbl = ssi_lnk_tbl;
#if HTTPSRV_CFG_MBEDTLS_ENABLE
    tls_params.certificate_buffer      = (const unsigned char *)mbedtls_test_srv_crt;
    tls_params.certificate_buffer_size = mbedtls_test_srv_crt_len;
    tls_params.private_key_buffer      = (const unsigned char *)mbedtls_test_srv_key;
    tls_params.private_key_buffer_size = mbedtls_test_srv_key_len;

    params.tls_param = &tls_params;
#endif
    /* Init HTTP Server.*/
    httpsrv_handle = HTTPSRV_init(&params);
    if (httpsrv_handle == 0)
    {
        LWIP_PLATFORM_DIAG(("HTTPSRV_init() is Failed"));
    }
}

/*!
 * @brief Initializes lwIP stack.
 */
int initNetwork(void);

/*!
 * @brief The main function containing server thread.
 */
static void main_thread(void *arg)
{
    LWIP_UNUSED_ARG(arg);

    initNetwork();
    http_server_socket_init();

    vTaskDelete(NULL);
}

/*!
 * @brief Main function.
 */
int main(void)
{
    int result;
    struct partition_entry *p;
    short history = 0;

    uint8_t hash[32];
    uint32_t len = sizeof(hash);

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    AES_Init(AES);

    BOARD_GetHash(hash, &len);
    assert(len > 0U);
    mbedtls_hardware_init_hash(hash, len);

    PRINTF("[OTA demo] Build Time: %s--%s \r\n", __DATE__, __TIME__);

    boot_init();
    mflash_drv_init();

    result = part_init();
    if (WM_SUCCESS != result)
    {
        assert(false);
    }

    part_entry[0] = part_get_layout_by_id(FC_COMP_FW, &history);
    part_entry[1] = part_get_layout_by_id(FC_COMP_FW, &history);

    /* OTA demo need dual partitions for MCUFW */
    assert((part_entry[0] != NULL) && (part_entry[1] != NULL));
    if (part_entry[0]->gen_level < part_entry[1]->gen_level)
    {
        /* swap the part_entry array to ensure part_entry[0] stands for active partition. */
        p             = part_entry[0];
        part_entry[0] = part_entry[1];
        part_entry[1] = p;
    }

    if ((part_entry[0]->gen_level & OTA_TEST_MODE_FLAG) != 0U)
    {
        /* Device is executiing in test mode */
        ota_status = OTA_STATUS_TESTING;
        part_entry[0]->gen_level = 0U;
        result                   = part_write_layout();
        PRINTF("Running in test mode: reset image gen_level to [0], result %d\r\n", result);
    }

    /* create server thread in RTOS */
    if (sys_thread_new("main", main_thread, NULL, HTTPD_STACKSIZE, HTTPD_PRIORITY) == NULL)
        LWIP_ASSERT("main(): Task creation failed.", 0);

    /* run RTOS */
    vTaskStartScheduler();

    /* should not reach this statement */
    for (;;)
        ;
}
