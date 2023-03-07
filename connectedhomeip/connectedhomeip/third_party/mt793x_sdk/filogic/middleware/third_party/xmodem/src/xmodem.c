/*
 * Copyright 2001-2022 Georges Menie (www.menie.org)
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* this code needs standard functions memcpy() and memset()
   and input/output functions _inbyte() and _outbyte().

   the prototypes of the input/output functions are:
     int _inbyte(unsigned short timeout); // msec timeout
     void _outbyte(int c);

 */

#include "crc16.h"
#include "hal.h"
#include "io_def.h"
#include "string.h"
#include "xmodem.h"

#include "memory_attribute.h"

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define ETB  0x17
#define CAN  0x18
#define CTRLZ 0x1A

#define DLY_PAYLOAD_MS 50
#define DLY_HANDSHAKE_MS 1000
#define MAXRETRANS 30

#define XMODEM_CANCEL_BY_REMOTE -1
#define XMODEM_SYNC_ERROR -2
#define XMODEM_RETRY_ERROR -3
#define XMODEM_XMIT_ERROR -4
#define XMODEM_TX_FAILED -5

ATTR_TEXT_IN_SYSRAM
static int check(int crc, const unsigned char *buf, int sz)
{
    if (crc) {
        unsigned short crc = crc16_ccitt(buf, sz);
        unsigned short tcrc = (buf[sz] << 8) + buf[sz + 1];
        if (crc == tcrc)
            return 1;
    } else {
        int i;
        unsigned char cks = 0;
        for (i = 0; i < sz; ++i) {
            cks += buf[i];
        }
        if (cks == buf[sz])
            return 1;
    }

    return 0;
}

ATTR_TEXT_IN_SYSRAM
static void flushinput(void)
{
    uint32_t ret;
    do {
        ret = hal_uart_get_available_receive_bytes(CONSOLE_UART);
        if (ret)
            bsp_io_def_uart_getchar();
    } while (ret);
}

static int _getchar(void)
{
    uint32_t ret;
    ret = hal_uart_get_available_receive_bytes(CONSOLE_UART);
    if (ret)
        return bsp_io_def_uart_getchar();
    return -1;
}

ATTR_TEXT_IN_SYSRAM
static int _inbyte(unsigned short timeout)
{
    int c;
    uint32_t timeout_us;
    int delay_us = 100000;
    uint32_t loop_cnt;

    timeout_us = (uint32_t)timeout * 1000;
    loop_cnt = timeout_us / delay_us;

    while (1) {
        if ((c = _getchar()) >= 0) {
            return c;
        }

        if (loop_cnt == 0)
            return -1;
        else
            loop_cnt--;

        /* 115200 bps, 1 byte receiving time ~= 1000000/115200*10 ~= 86 us.
           921600 bps, 1 byte receiving time ~= 1000000/921600*10 ~= 11 us.*/
        hal_gpt_delay_us(delay_us);
    }

    return -1;
}

ATTR_TEXT_IN_SYSRAM
static void _outbyte(int c)
{
    uint32_t n;

    do {
        n = hal_uart_send_dma(CONSOLE_UART, (uint8_t *) &c, 1);
    } while (!n);
}

/**
 * This is the original implementation of xmodem.c
 */
int xmodemReceive(unsigned char *dest, int destsz)
{
    unsigned char xbuff[1030]; /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
    unsigned char *p;
    int bufsz, crc = 0;
    unsigned char trychar = 'C';
    unsigned char packetno = 1;
    int i, c, len = 0;
    int retry, retrans = MAXRETRANS;

    for (;;) {
        for (retry = 0; retry < MAXRETRANS; ++retry) {
            if (trychar)
                _outbyte(trychar);
            if ((c = _inbyte((DLY_HANDSHAKE_MS) << 1)) >= 0) {
                switch (c) {
                    case SOH:
                        bufsz = 128;
                        goto start_recv;
                    case STX:
                        bufsz = 1024;
                        goto start_recv;
                    case EOT:
                        flushinput();
                        _outbyte(ACK);
                        return len; /* normal end */
                    case CAN:
                        if ((c = _inbyte(DLY_HANDSHAKE_MS)) == CAN) {
                            flushinput();
                            _outbyte(ACK);
                            return XMODEM_CANCEL_BY_REMOTE; /* canceled by remote */
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        if (trychar == 'C') {
            trychar = NAK;
            continue;
        }
        flushinput();
        _outbyte(CAN);
        _outbyte(CAN);
        _outbyte(CAN);
        return XMODEM_SYNC_ERROR; /* sync error */

start_recv:
        if (trychar == 'C')
            crc = 1;
        trychar = 0;
        p = xbuff;
        *p++ = c;
        for (i = 0;  i < (3 + bufsz + (crc ? 1 : 0)); ++i) {
            if ((c = _inbyte(DLY_PAYLOAD_MS)) == -1)
                goto reject;
            *p++ = c;
        }

        if (xbuff[1] == (unsigned char)(~xbuff[2]) &&
            (xbuff[1] == packetno || xbuff[1] == (unsigned char)packetno - 1) &&
            check(crc, &xbuff[3], bufsz)) {
            if (xbuff[1] == packetno)    {
                register int count = destsz - len;
                if (count > bufsz)
                    count = bufsz;
                if (count > 0) {
                    memcpy(&dest[len], &xbuff[3], count);
                    len += count;
                }
                ++packetno;
                retrans = MAXRETRANS + 1;
            }
            if (--retrans <= 0) {
                flushinput();
                _outbyte(CAN);
                _outbyte(CAN);
                _outbyte(CAN);
                return XMODEM_RETRY_ERROR; /* too many retry error */
            }
            _outbyte(ACK);
            continue;
        }
reject:
        flushinput();
        _outbyte(NAK);
    }
}

ATTR_TEXT_IN_SYSRAM
int xmodemTransmit(unsigned char *src, int srcsz)
{
    unsigned char xbuff[1030]; /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
    int bufsz, crc;
    unsigned char packetno = 1;
    int i, c, len = 0;
    int retry;

    for (retry = 0; retry < MAXRETRANS; ++retry) {
        if ((c = _inbyte((DLY_HANDSHAKE_MS) << 1)) >= 0) {
            switch (c) {
                case 'C':
                    crc = 1;
                    goto start_trans;
                case NAK:
                    crc = 0;
                    goto start_trans;
                case CAN:
                    if ((c = _inbyte(DLY_HANDSHAKE_MS)) == CAN) {
                        _outbyte(ACK);
                        flushinput();
                        return -1; /* canceled by remote */
                    }
                    break;
                default:
                    break;
            }
        }
    }
    _outbyte(CAN);
    _outbyte(CAN);
    _outbyte(CAN);
    flushinput();
    return XMODEM_SYNC_ERROR; /* no sync */

start_trans:
    xbuff[0] = STX;
    bufsz = 1024; //1k-xmodem
    xbuff[1] = packetno;
    xbuff[2] = ~packetno;
    c = srcsz - len;
    if (c > bufsz)
        c = bufsz;
    if (c >= 0) {
        memset(&xbuff[3], 0, bufsz);
        if (c == 0) {
            xbuff[3] = CTRLZ;
        } else {
            memcpy(&xbuff[3], &src[len], c);
            if (c < bufsz) {
                xbuff[3 + c] = CTRLZ;
            }
        }
        if (crc) {
            unsigned short ccrc = crc16_ccitt(&xbuff[3], bufsz);
            xbuff[bufsz + 3] = (ccrc >> 8) & 0xFF;
            xbuff[bufsz + 4] = ccrc & 0xFF;
        } else {
            unsigned char ccks = 0;
            for (i = 3; i < bufsz + 3; ++i) {
                ccks += xbuff[i];
            }
            xbuff[bufsz + 3] = ccks;
        }
        for (retry = 0; retry < MAXRETRANS; ++retry) {
            for (i = 0; i < bufsz + 4 + (crc ? 1 : 0); ++i) {
                _outbyte(xbuff[i]);
            }
            if ((c = _inbyte(DLY_HANDSHAKE_MS)) >= 0) {
                switch (c) {
                    case ACK:
                        ++packetno;
                        len += bufsz;
                        goto start_trans;
                    case CAN:
                        if ((c = _inbyte(DLY_HANDSHAKE_MS)) == CAN) {
                            _outbyte(ACK);
                            flushinput();
                            return -1; /* canceled by remote */
                        }
                        break;
                    case NAK:
                    default:
                        break;
                }
            }
        }
        _outbyte(CAN);
        _outbyte(CAN);
        _outbyte(CAN);
        flushinput();
        return XMODEM_XMIT_ERROR; /* xmit error */
    } else {
        for (retry = 0; retry < MAXRETRANS; ++retry) {
            _outbyte(EOT);
            if ((c = _inbyte((DLY_HANDSHAKE_MS) << 1)) == ACK) {
                break;
            }
        }
        flushinput();
        return (c == ACK) ? len : XMODEM_TX_FAILED;
    }
}

#if MTK_CSI_PSRAM_ENABLE
uint8_t xmodem_send_csi_data(uint32_t addr, uint32_t size)
{
    int st, i = 0;
    uint8_t type;

    printf("Prepare your terminal emulator to receive data now...\n");
    printf("addr: 0x%x\n", (unsigned int)addr);
    printf("size: 0x%x\n", (unsigned int)size);

    st = xmodemTransmit((unsigned char *)addr, size);
    if (st < 0) {
        printf("Xmodem transmit error: status: %d\n", st);
        return 1;
    }

    printf("Xmodem successfully transmitted %d bytes\n", st);

    return 0;
}
#endif /* #if MTK_CSI_PSRAM_ENABLE */

#ifdef TEST_XMODEM_RECEIVE
int main(void)
{
    int st;

    printf("Send data using the xmodem protocol from your terminal emulator now...\n");
    /* the following should be changed for your environment:
       0x30000 is the download address,
       65536 is the maximum size to be written at this address
     */
    st = xmodemReceive((char *)0x30000, 65536);
    if (st < 0) {
        printf("Xmodem receive error: status: %d\n", st);
    } else  {
        printf("Xmodem successfully received %d bytes\n", st);
    }

    return 0;
}
#endif /* #ifdef TEST_XMODEM_RECEIVE */
#ifdef TEST_XMODEM_SEND
int main(void)
{
    int st;

    printf("Prepare your terminal emulator to receive data now...\n");
    /* the following should be changed for your environment:
       0x30000 is the download address,
       12000 is the maximum size to be send from this address
     */
    st = xmodemTransmit((char *)0xA0000000, 0x10000000);
    if (st < 0) {
        printf("Xmodem transmit error: status: %d\n", st);
    } else  {
        printf("Xmodem successfully transmitted %d bytes\n", st);
    }

    return 0;
}
#endif /* #ifdef TEST_XMODEM_SEND */
