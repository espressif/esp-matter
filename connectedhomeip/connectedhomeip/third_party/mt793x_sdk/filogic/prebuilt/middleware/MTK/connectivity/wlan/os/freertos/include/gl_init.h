
#ifndef _GL_INIT_H
#define _GL_INIT_H

#include "gl_typedef.h"
#include "gl_os.h"
#include "debug.h"

/* keep allocated memory  not free */
extern struct ADAPTER *g_prAdpater;
extern uint8_t *g_pucSecBuf;
#define SEC_BUF_SIZE 2048


BaseType_t wifi_init_task(void);
BaseType_t wifi_exit_task(void);

void wlanRemoveAfterMainThread(struct GLUE_INFO *prGlueInfo);
void wlan_register_callback(wlan_netif_input_fn input, struct netif *netif, int opmode);
int mtk_wcn_wlan_gen4_tx(struct pbuf *p, struct netif *netif);
int mtk_wlan_tx(struct pbuf *p, struct netif *netif);

/* FW DL API for binary release */
uint32_t getWifiBaseAddr(void);
uint32_t getWifiExtBaseAddr(void);
uint32_t getWifiPatchBaseAddr(void);
uint32_t getWifiPwrTblBaseAddr(void);

/* Read Buffer bin */
uint32_t getBufBinAddr(void);

#endif
