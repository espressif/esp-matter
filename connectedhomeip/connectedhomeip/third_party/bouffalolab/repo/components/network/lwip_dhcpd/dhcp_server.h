
/**
 ****************************************************************************************
 *
 * @file dhcp_server.h
 * Copyright (C) Bouffalo Lab 2016-2018
 *
 ****************************************************************************************
 */


#ifndef DHCPV4_SERVER_H__
#define DHCPV4_SERVER_H__

#ifdef __cplusplus
extern "C" {
#endif

void dhcpd_start(struct netif *netif);

#ifdef __cplusplus
}
#endif

#endif

