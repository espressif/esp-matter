#ifndef __PING_CLI_H__
#define __PING_CLI_H__


#include <cli.h>


#if defined(MTK_PING_OUT_ENABLE)

uint8_t ping_cli_handler(uint8_t len, char *param[]);

#define PING_CLI_ENTRY { "ping",                            \
                         "ping <addr> <count> <pkt_len>",   \
                         ping_cli_handler,                  \
                         NULL },
#else
#define PING_CLI_ENTRY
#endif


#endif /* __PING_CLI_H__ */
