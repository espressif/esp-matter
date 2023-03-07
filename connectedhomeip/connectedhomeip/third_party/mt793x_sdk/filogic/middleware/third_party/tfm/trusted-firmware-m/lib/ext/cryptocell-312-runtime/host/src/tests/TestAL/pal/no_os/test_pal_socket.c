/*******************************************************************************
* The confidential and proprietary information contained in this file may      *
* only be used by a person authorised under and to the extent permitted        *
* by a subsisting licensing agreement from ARM Limited or its affiliates.      *
*   (C) COPYRIGHT [2001-2017] ARM Limited or its affiliates.                   *
*       ALL RIGHTS RESERVED                                                    *
* This entire notice must be reproduced on all copies of this file             *
* and copies of this file may only be made by a person if such person is       *
* permitted to do so under the terms of a subsisting license agreement         *
* from ARM Limited or its affiliates.                                          *
*******************************************************************************/

#include <stdint.h>

#include "test_pal_socket.h"
#include "test_pal_mem.h"

/* the time to wait for sent/recive replay */
#define TIMEOUT_MS  100

/******************************************************************************/
uint32_t Test_PalSocket(tp_socket *s, enum tp_sock_domain domain,
        enum tp_sock_type type, enum tp_sock_protocol protocol,
        const uint32_t recvTimeout_ms)
{
    return 0;
}

/******************************************************************************/
uint32_t Test_PalCloseSocket(tp_socket s)
{
    return 0;
}

/******************************************************************************/
uint32_t Test_PalConnect(tp_socket s, const uint8_t *addr, uint32_t port)
{
    return 0;

}

/******************************************************************************/
uint32_t Test_PalBind(tp_socket s, uint32_t port)
{
    return 0;
}

/******************************************************************************/
uint32_t Test_PalListen(tp_socket s, uint32_t backlog)
{
    return 0;
}

/******************************************************************************/
uint32_t Test_PalAccept(tp_socket s, tp_socket *acptS, uint8_t *addr,
             uint32_t *port)
{
    return 0;
}

/******************************************************************************/
tp_socket Test_PalShutdown(tp_socket s)
{
    return 0;
}

/******************************************************************************/
uint32_t Test_PalSend(tp_socket s, const uint8_t *buf,
              size_t len)
{
    return 0;
}

/******************************************************************************/
uint32_t Test_PalSendTo(tp_socket s, const uint8_t *buf,
            size_t len, const uint8_t *addr, uint32_t port)
{
    return 0;
}

/******************************************************************************/
uint32_t Test_PalRecvFrom(tp_socket s, const uint8_t *buf,
            size_t len, uint8_t *addr, uint32_t *port)
{
    return 0;
}

/******************************************************************************/
uint32_t Test_PalRecv(tp_socket s, const uint8_t *buf,
              size_t len)
{
    return 0;
}

/******************************************************************************/
uint32_t Test_PalHtonl(uint32_t val)
{
    return 0;
}

/******************************************************************************/
uint16_t Test_PalHtons(uint16_t val)
{
    return 0;
}

/******************************************************************************/
uint32_t Test_PalNtohl(uint32_t val)
{
    return 0;
}

/******************************************************************************/
uint16_t Test_PalNtohs(uint16_t val)
{
    return 0;
}

