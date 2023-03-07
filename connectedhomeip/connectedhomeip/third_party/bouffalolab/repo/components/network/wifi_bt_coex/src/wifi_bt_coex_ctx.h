#ifndef __WIFI_BT_COEX_CTX_H__
#define __WIFI_BT_COEX_CTX_H__
#include <wifi_bt_coex.h>

int wifi_bt_coex_bt_inc(struct wifi_bt_coex_ctx *ctx, int step);
int wifi_bt_coex_bt_dec(struct wifi_bt_coex_ctx *ctx, int step);
#endif
