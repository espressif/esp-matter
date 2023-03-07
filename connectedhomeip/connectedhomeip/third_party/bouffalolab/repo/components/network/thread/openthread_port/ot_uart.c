
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <bl_uart.h>
#include <hosal_uart.h>

#include <openthread-core-config.h>
#include <openthread/config.h>

#include <openthread/platform/debug_uart.h>
#include <openthread/platform/logging.h>
#include <openthread_port.h>
#include <utils/uart.h>



#if SYS_AOS_LOOP_ENABLE
#include <vfs.h>
#include <device/vfs_uart.h>
#include <aos/kernel.h>
#include <aos/yloop.h>
#include <event_device.h>
#include <cli.h>

int ot_uart_vfs_file_handle = -1;
extern hosal_uart_dev_t uart_stdio;

static void ot_uartCli(char* pcWriteBuffer, int xWriteBufferLen, int argc, char** argv)
{
    static char buf[384];

    if (argc) {
        char *p = buf;
        for (uint32_t i = 1; i < argc; i ++) {
            memcpy(p, argv[i], strlen(argv[i]));
            p += strlen(argv[i]);
            *p++ = ' ';
        }
        p --;
        *p++ = '\r';
        *p++ = '\n';

        OT_THREAD_SAFE(
            otPlatUartReceived((uint8_t *)buf, p - buf);
        );
    }
}

const struct cli_command otcCliSet[] STATIC_CLI_CMD_ATTRIBUTE = {
    { "otc", "orignal openthread command line", ot_uartCli},
};

static void ot_uart_cb_read(int fd, void *param)
{
    char buffer[64];  /* adapt to usb cdc since usb fifo is 64 bytes */
    int ret = -1;

    do {
        ret = aos_read(fd, buffer, sizeof(buffer));
        if (ret > 0) {
            if (ret <= sizeof(buffer)) {
                ot_uart_vfs_file_handle = fd;
                OT_THREAD_SAFE(
                    otPlatUartReceived((uint8_t *)buffer, ret);
                );
            } else {
                printf("error: from aos_read for ret\r\n");
            }
        }
    } while (ret > 0);
}

void *ot_uart_event_cb_read_get()
{
    return ot_uart_cb_read;
}

otError otPlatUartEnable(void) {return OT_ERROR_NONE;}

otError otPlatUartDisable(void) { return OT_ERROR_NONE;}

otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength) 
{
    int iofst = 0, iret = 0, write_len = aBufLength;

    if (ot_uart_vfs_file_handle >= 0) {
        while (aBufLength) {
#if CFG_USB_CDC_ENABLE
            extern int usb_cdc_is_port_open(void);
            if (usb_cdc_is_port_open()) {
                write_len = 64;
            }
#endif
            iret = aos_write(ot_uart_vfs_file_handle, aBuf + iofst, aBufLength > write_len ? write_len : aBufLength);
            if (iret > 0) {
                iofst += iret;
                aBufLength -= iret;
            }
        }
    }

    otPlatUartSendDone();

    return OT_ERROR_NONE;
}

otError otPlatUartFlush(void) {return OT_ERROR_NONE;}

void ot_cli_init(void) 
{
    if (ot_uart_vfs_file_handle < 0) {
        ot_uart_vfs_file_handle = aos_open("/dev/ttyS0", 0);
        if (ot_uart_vfs_file_handle >= 0) {

            aos_poll_read_fd(ot_uart_vfs_file_handle, ot_uart_event_cb_read_get(), NULL);

#if defined(CFG_USB_CDC_ENABLE)
            extern void usb_cdc_start(int ot_uart_vfs_file_handle);
            usb_cdc_start(ot_uart_vfs_file_handle);
#endif
        }
    }
}

void ot_uartSetFd(int fd) 
{
    ot_uart_vfs_file_handle = fd;
}
void ot_uartTask (ot_system_event_t sevent) {}

#else
extern hosal_uart_dev_t uart_stdio;

void otPlatDebugUart_write_bytes(const uint8_t *pBytes, int nBytes)
{
}

typedef struct _otUart {
    uint16_t    start;
    uint16_t    end;
    uint32_t    recvLen;
    uint8_t     rxbuf[OT_UART_RX_BUFFSIZE];
} otUart_t;

static otUart_t otUart_var;

static int ot_uartRxdCb(void *p_arg);
static int ot_uartTxdCb(void *p_arg) 
{
    hosal_uart_ioctl(&uart_stdio, HOSAL_UART_TX_TRIGGER_OFF, NULL);

    return 0;
}

otError otPlatUartEnable(void)
{
    hosal_uart_finalize(&uart_stdio);

    hosal_uart_init(&uart_stdio);

    hosal_uart_callback_set(&uart_stdio, HOSAL_UART_RX_CALLBACK, ot_uartRxdCb, NULL);
    hosal_uart_callback_set(&uart_stdio, HOSAL_UART_TX_CALLBACK, ot_uartTxdCb, NULL);
    hosal_uart_ioctl(&uart_stdio, HOSAL_UART_MODE_SET, (void *)HOSAL_UART_MODE_INT);

    memset(&otUart_var, 0, sizeof(otUart_t));


    return OT_ERROR_NONE;
}

otError otPlatUartDisable(void)
{
    hosal_uart_finalize(&uart_stdio);
    memset(&otUart_var, 0, sizeof(otUart_t));
    return OT_ERROR_NONE;
}

otError otPlatUartSend(const uint8_t *aBuf, uint16_t aBufLength)
{
    OT_CLI_UART_OUTPUT_LOCK();

    hosal_uart_send(&uart_stdio, aBuf, aBufLength);
    otPlatUartSendDone();

    OT_CLI_UART_OUTPUT_UNLOCK();

    return OT_ERROR_NONE;
}

otError otPlatUartFlush(void)
{
    hosal_uart_ioctl(&uart_stdio, HOSAL_UART_FLUSH, NULL);

    return OT_ERROR_NONE;
}

static int ot_uartRxdCb(void *p_arg)
{
    uint32_t len = 0;

    if (otUart_var.start >= otUart_var.end) {
        otUart_var.start += hosal_uart_receive(&uart_stdio, otUart_var.rxbuf + otUart_var.start, 
            OT_UART_RX_BUFFSIZE - otUart_var.start - 1);
        if (otUart_var.start == OT_UART_RX_BUFFSIZE - 1) {
            otUart_var.start = hosal_uart_receive(&uart_stdio, otUart_var.rxbuf, 
                (OT_UART_RX_BUFFSIZE + otUart_var.end - 1) % OT_UART_RX_BUFFSIZE);
        }
    }
    else if (((otUart_var.start + 1) % OT_UART_RX_BUFFSIZE) != otUart_var.end) {
        otUart_var.start += hosal_uart_receive(&uart_stdio, otUart_var.rxbuf, 
            otUart_var.end - otUart_var.start - 1);
    }

    if (otUart_var.start != otUart_var.end) {

        len = (otUart_var.start + OT_UART_RX_BUFFSIZE - otUart_var.end) % OT_UART_RX_BUFFSIZE;
        if (otUart_var.recvLen != len) {
            otUart_var.recvLen = len;
            OT_NOTIFY_ISR(OT_SYSTEM_EVENT_UART_RXD);
        }
    }

    return 0;
}

void ot_uartTask (ot_system_event_t sevent) 
{
    if (!(OT_SYSETM_EVENT_UART_ALL_MASK & sevent)) {
        return;
    }

    if (OT_SYSTEM_EVENT_UART_RXD & sevent) {
        OT_ENTER_CRITICAL();

        if (otUart_var.start != otUart_var.end) {
            if (otUart_var.start > otUart_var.end) {
                otPlatUartReceived(otUart_var.rxbuf + otUart_var.end, otUart_var.start - otUart_var.end);
                otUart_var.end = otUart_var.start;
            }
            else {
                otPlatUartReceived(otUart_var.rxbuf + otUart_var.end, OT_UART_RX_BUFFSIZE - otUart_var.end);
                otUart_var.end = OT_UART_RX_BUFFSIZE - 1;
                if (otUart_var.start) {
                    otPlatUartReceived(otUart_var.rxbuf, otUart_var.start);
                    otUart_var.end = (OT_UART_RX_BUFFSIZE + otUart_var.start - 1) % OT_UART_RX_BUFFSIZE;
                }
            }
        }

        otUart_var.start = otUart_var.end = 0;
        otUart_var.recvLen = 0;
        OT_EXIT_CRITICAL();
    }
}
#endif

void ot_uartLog(const char *fmt, va_list argp)
{
#if CFG_USB_CDC_ENABLE || ! defined (CONFIG_NCP)
    static char *str;
    static char string[384];
    int ch;

    str = string;
    if (0 < vsprintf(str, fmt, argp)) {
        while ('\0' != (ch = *(str++))) {
            bl_uart_data_send(uart_stdio.config.uart_id, ch);
        }
    }
#endif
}


