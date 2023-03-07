#include <stdio.h>
#include <wifi_bt_coex.h>

#define COEX_CTX_DEBUG(fmt, args...) printf(fmt, ##args)

int wifi_bt_coex_bt_inc(struct wifi_bt_coex_ctx *ctx, int step)
{
    int target;

    target = ctx->timer_toggle_end + step;
    if (target <= 0 || ctx->timer_toggle_end > ctx->timer_max) {
        return -1;
    }
    ctx->timer_toggle_end = target;

    COEX_CTX_DEBUG("[COEX] [CTX] using timer_toggle_end %lu\r\n", ctx->timer_toggle_end); 

    return 0;
}

int wifi_bt_coex_bt_dec(struct wifi_bt_coex_ctx *ctx, int step)
{
    return 0;
}
