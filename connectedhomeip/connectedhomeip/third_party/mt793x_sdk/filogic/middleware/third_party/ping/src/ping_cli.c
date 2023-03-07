
#include <os.h>
#include <toi.h>

#include "ping.h"

#include "ping_cli.h"


uint8_t ping_cli_handler(uint8_t len, char *param[])
{
    uint32_t count;
    uint32_t pktsz;
    uint32_t interval;
    uint8_t  type;

#ifdef MTK_SIGMA_ENABLE
    g_ping_recv = 0;
#endif /* MTK_SIGMA_ENABLE */

    if (len < 1) {
        return 1;
    } else {
        count = 3;
        pktsz = 64;
        interval = 1000;
    }

    if (len > 1) {
        count = toi(param[1], &type);
        if (type == TOI_ERR) {
            return 2;
        }
    }

    if (len > 2) {
        pktsz = toi(param[2], &type);
        if (type == TOI_ERR) {
            return 2;
        }
    }
#ifdef MTK_CSI_PSRAM_ENABLE
    if (len > 3) {
        interval = toi(param[3], &type);
        if (type == TOI_ERR) {
            return 2;
        }
    }
#endif

    ping_init(count, param[0], os_strlen(param[0]), pktsz, interval);

    return 0;
}

