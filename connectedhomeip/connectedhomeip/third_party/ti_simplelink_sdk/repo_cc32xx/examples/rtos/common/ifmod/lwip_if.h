/*
 * Copyright (C) 2016-2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//*****************************************************************************
// Includes
//*****************************************************************************
// Standard includes

#ifndef LWIP_IF_H
#define LWIP_IF_H
#include "lwip/netif.h"

/****************************************************************************
              TYPE DEFINITIONS
 ****************************************************************************/
typedef enum 
{
    E_NETIF_STATUS_LINK_UP,
    E_NETIF_STATUS_LINK_DOWN,
    E_NETIF_STATUS_IP_ACQUIRED,
    E_NETIF_STATUS_IP_LOST,
    E_NETIF_STATUS_MAX
} NetIfStatus_e;
 
typedef void (*UserCallback_f)(struct netif *pNetIf, NetIfStatus_e status, void *pParams);

/****************************************************************************
             PUBLIC API
 ****************************************************************************/


/*!

    \brief     LWIP init (dhould be called once before any other API)

    \param[in] fUserCallback - user callback for state change inidcation
    \param[in] bStackInit - flag indicating whether to initialize LWIP stack 
 
    \return    0 upon success or negative error code 
*/
int LWIP_IF_init(UserCallback_f fUserCallback, bool bStackInit);

/*!

    \brief     User request to register and enable an interface 
               This will be used to enbale the interface both at 
               simplelink and LWIP.

    \return    pointer to netif or NULL in case of failure 
*/
struct netif * LWIP_IF_addInterface();

/*!

    \brief     Driver indication when the Link gets UP 
    		(i.e. AP connection is established)

    \return    0 upon success or negative error code 
*/
int LWIP_IF_setLinkUp(struct netif *pNetIf);

/*!

    \brief     Driver indication when the Link is Down 
    		(i.e. upon disconnection from the AP)

    \return    0 upon success or negative error code 
*/
int LWIP_IF_setLinkDown(struct netif *pNetIf);




#endif // OTA_IF_H
