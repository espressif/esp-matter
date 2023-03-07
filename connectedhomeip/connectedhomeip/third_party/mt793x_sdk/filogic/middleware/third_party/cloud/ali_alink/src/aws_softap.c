/*
	softap tcp交互部分：目的为APP和设备通过tcp交换ssid和passwd信息
	1）softap流程�?wifi设备开启tcp server，等待APP进行连接
	2）APP连接上设备的tcp server后，发送ssid&passwd&bssid信息给wifi设备，wifi设备进行解析
	3）wifi设备连接通过active scan，获取无线网络加密方式，进行联网
	4）wifi设备联网成功后，发送配网成功的通知给APP�?
	//修改记录
	0421：在广播信息里添加sn、mac字段，增加对私有云对接的支持
		绑定过程，设备端等待tcp连接超时时间�?min
	0727�?添加功能说明
	0813�?添加softap dns配置说明
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> /* for strncpy */
#include <errno.h>

#include <sys/types.h>

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "type_def.h"
#include "syslog.h"
#include "wifi_api.h"
#include "ethernetif.h"
#include "dhcpd.h"

#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "lwip/ip4_addr.h"
#include "lwip/inet.h"

#include <nvdm.h>

#include "aws_lib.h"
#include "aws_platform.h"
#include "platform_porting.h"

#define SOFTAP_GATEWAY_IP		"172.31.254.250"
#define SOFTAP_TCP_SERVER_PORT		(65125)

#ifndef	info
#define info(format, ...)	printf(format, ##__VA_ARGS__)
#endif


/*
	以下为softap配网时，设备起的softap tcp server sample
*/
///////////////softap tcp server sample/////////////////////
#define	STR_SSID_LEN		(32 + 1)
#define STR_PASSWD_LEN		(64 + 1)
char aws_ssid[STR_SSID_LEN];
char aws_passwd[STR_PASSWD_LEN];
unsigned char aws_bssid[6];

/* json info parser */
int get_ssid_and_passwd(char *msg)
{
	char *ptr, *end, *name;
	int len;

	//ssid
	name = "\"ssid\":";
	ptr = strstr(msg, name);
	if (!ptr) {
		info("%s not found!\n", name);
		goto exit;
	}
	ptr += strlen(name);
	while (*ptr++ == ' ');/* eating the beginning " */
	end = strchr(ptr, '"');
	len = end - ptr;

	assert(len < sizeof(aws_ssid));
	strncpy(aws_ssid, ptr, len);
	aws_ssid[len] = '\0';

	//passwd
	name = "\"passwd\":";
	ptr = strstr(msg, name);
	if (!ptr) {
		info("%s not found!\n", name);
		goto exit;
	}

	ptr += strlen(name);
	while (*ptr++ == ' ');/* eating the beginning " */
	end = strchr(ptr, '"');
	len = end - ptr;

	assert(len < sizeof(aws_passwd));
	strncpy(aws_passwd, ptr, len);
	aws_passwd[len] = '\0';

	//bssid-mac
	name = "\"bssid\":";
	ptr = strstr(msg, name);
	if (!ptr) {
		info("%s not found!\n", name);
		goto exit;
	}

	ptr += strlen(name);
	while (*ptr++ == ' ');/* eating the beginning " */
	end = strchr(ptr, '"');
	len = end - ptr;

#if 0
	memset(aws_bssid, 0, sizeof(aws_bssid));

	sscanf(ptr, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
			&aws_bssid[0], &aws_bssid[1], &aws_bssid[2],
			&aws_bssid[3], &aws_bssid[4], &aws_bssid[5]);
#endif

	return 0;
exit:
	return -1;
}

//setup softap server
int aws_softap_tcp_server(void)
{
	struct sockaddr_in server, client;
	socklen_t socklen = sizeof(client);
	int fd = -1, connfd, len, ret;
	char *buf, *msg;
	int opt = 1, buf_size = 512, msg_size = 512;

	info("setup softap & tcp-server\n");

	buf = alink_malloc_func(buf_size);
	msg = alink_malloc_func(msg_size);
	assert(fd && msg);

	fd = socket(AF_INET, SOCK_STREAM, 0);
	assert(fd >= 0);

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);//SOFTAP_GATEWAY_IP, 0xFAFE1FAC;
	server.sin_port = htons(SOFTAP_TCP_SERVER_PORT);

	ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	assert(!ret);

	ret = bind(fd, (struct sockaddr *)&server, sizeof(server));
	assert(!ret);

	ret = listen(fd, 10);
	assert(!ret);

	info("server %x %d created\n", ntohl(server.sin_addr.s_addr),
			ntohs(server.sin_port));

	connfd = accept(fd, (struct sockaddr *)&client, &socklen);
	assert(connfd > 0);
	info("client %x %d connected!\n", ntohl(client.sin_addr.s_addr),
			ntohs(client.sin_port));


	len = recvfrom(connfd, buf, buf_size, 0,
			(struct sockaddr *)&client, &socklen);
	assert(len >= 0);

	buf[len] = 0;
	info("softap tcp server recv: %s\n", buf);

	ret = get_ssid_and_passwd(buf);
	if (!ret) {
		snprintf(msg, buf_size,
			"{\"code\":1000, \"msg\":\"format ok\", \"model\":\"%s\", \"mac\":\"%s\"}",
			vendor_get_model(), vendor_get_mac());
	} else
		snprintf(msg, buf_size,
			"{\"code\":2000, \"msg\":\"format error\", \"model\":\"%s\", \"mac\":\"%s\"}",
			vendor_get_model(), vendor_get_mac());

	len = sendto(connfd, msg, strlen(msg), 0,
			(struct sockaddr *)&client, socklen);
	assert(len >= 0);
	info("ack %s, len %d\n", msg, len);

	close(connfd);
	close(fd);

	alink_free_func(buf);
	alink_free_func(msg);

	return 0;
}

// extern struct netif ap_if;
// extern struct netif sta_if;
#ifdef MTK_MINISUPP_ENABLE
extern int g_supplicant_ready;
#endif
void aws_softap_setup(void)
{
	/*
	 * wilress params: 11BGN
	 * channel: auto, or 1, 6, 11
	 * authentication: OPEN
	 * encryption: NONE
	 * gatewayip: 172.31.254.250, netmask: 255.255.255.0
	 * DNS server: 172.31.254.250. 	IMPORTANT!!!  ios depend on it!
	 * DHCP: enable
	 * SSID: 32 ascii char at most
	 * softap timeout: 5min
	 */

	char ssid[WIFI_MAX_LENGTH_OF_SSID + 1] = {0};
	//ssid: max 32Bytes(excluding '\0')
	snprintf(ssid, WIFI_MAX_LENGTH_OF_SSID + 1, "alink_%s", vendor_get_model());

    wifi_config_set_opmode(WIFI_MODE_AP_ONLY);
    wifi_config_set_security_mode(WIFI_PORT_AP, WIFI_AUTH_MODE_OPEN, WIFI_ENCRYPT_TYPE_WEP_DISABLED);
    wifi_config_set_channel(WIFI_PORT_AP, 6);
    wifi_config_set_ssid(WIFI_PORT_AP, (uint8_t *)ssid, strlen(ssid));
    wifi_config_reload_setting();

    /*
    {
        char ip_buf[] = "172.31.254.250";
        char mask_buf[] = "255.255.255.0";
        struct ip4_addr addr;

        netif_set_status_callback(&sta_if, NULL);

        nvdm_write_data_item("network", "IpAddr", NVDM_DATA_ITEM_TYPE_STRING, ip_buf, sizeof(ip_buf));
        nvdm_write_data_item("network", "IpNetmask", NVDM_DATA_ITEM_TYPE_STRING, mask_buf, sizeof(mask_buf));
        nvdm_write_data_item("network", "IpGateway", NVDM_DATA_ITEM_TYPE_STRING, ip_buf, sizeof(ip_buf));

        inet_aton(mask_buf, &addr);
        netif_set_netmask(&sta_if, &addr);
        inet_aton(ip_buf, &addr);
        netif_set_ipaddr(&sta_if, &addr);
        netif_set_gw(&sta_if, &addr);

        dhcp_stop(&sta_if);
        netif_set_link_up(&sta_if);
        netif_set_default(&sta_if);
        dhcpd_start(0);
        printf("start dhcpd, stop dhcp. g_supplicant_ready:%d\n", g_supplicant_ready);
    }
    */

    if(1)
    {
        char ip_buf[] = "172.31.254.250";
        char mask_buf[] = "255.255.255.0";
        char start_ip[] = "172.31.254.251";
        char end_ip[] = "172.31.254.254";
        char primary_dns[] = "172.31.254.250";
        char secondary_dns[] = "8.8.4.4";
        struct ip4_addr addr;
        struct netif *sta_if;
        struct netif *ap_if;
        ap_if = netif_find_by_type(NETIF_TYPE_AP);
        sta_if = netif_find_by_type(NETIF_TYPE_STA);

        netif_set_status_callback(ap_if, NULL);

        inet_aton(mask_buf, &addr);
        netif_set_netmask(ap_if, &addr);
        inet_aton(ip_buf, &addr);
        netif_set_ipaddr(ap_if, &addr);
        netif_set_gw(ap_if, &addr);

        dhcp_stop(sta_if);
        netif_set_link_up(ap_if);
        netif_set_default(ap_if);
        dhcpd_settings_t dhcpd_settings;
        memset(&dhcpd_settings, 0, sizeof(dhcpd_settings_t));
        strcpy((char *)dhcpd_settings.dhcpd_server_address, ip_buf);
        strcpy((char *)dhcpd_settings.dhcpd_netmask, mask_buf);
        strcpy((char *)dhcpd_settings.dhcpd_gateway, ip_buf);
        strcpy((char *)dhcpd_settings.dhcpd_primary_dns, primary_dns);
        strcpy((char *)dhcpd_settings.dhcpd_secondary_dns, secondary_dns);
        strcpy((char *)dhcpd_settings.dhcpd_ip_pool_start, start_ip);
        strcpy((char *)dhcpd_settings.dhcpd_ip_pool_end, end_ip);
        dhcpd_start(&dhcpd_settings);
#ifdef MTK_MINISUPP_ENABLE
        printf("start dhcpd, stop dhcp. g_supplicant_ready:%d\n", g_supplicant_ready);
#endif
    }
}

void aws_softap_exit(void)
{
/*
    dhcpd_stop();
    wifi_config_set_opmode(WIFI_MODE_STA_ONLY);

    dhcp_start(&sta_if);
    netif_set_link_up(&sta_if);
 */
    struct netif *sta_if;
    struct netif *ap_if;
    ap_if = netif_find_by_type(NETIF_TYPE_AP);
    dhcpd_stop();
    netif_set_link_down(ap_if);

    sta_if = netif_find_by_type(NETIF_TYPE_STA);
    netif_set_default(sta_if);
    // netif_set_status_callback(sta_if, ip_ready_callback);
    dhcp_start(sta_if);
}

int aws_config_softap(char *ssid_buf, char *passwd_buf)
{
    int ret = 0;
	/* prepare and setup softap */
	aws_softap_setup();

	/* tcp server to get ssid & passwd */
	aws_softap_tcp_server();

    strncpy(ssid_buf, aws_ssid, strlen(aws_ssid));
    ssid_buf[strlen(aws_ssid)] = 0;
    strncpy(passwd_buf, aws_passwd, strlen(aws_passwd));
    passwd_buf[strlen(aws_passwd)] = 0;

    aws_softap_exit();
	return ret;
}
