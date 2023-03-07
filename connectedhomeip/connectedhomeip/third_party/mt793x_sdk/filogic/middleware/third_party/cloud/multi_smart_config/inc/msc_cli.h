#ifndef _MSC_CLI_H_
#define _MSC_CLI_H_

#ifdef __cplusplus
extern "C" {
#endif
extern uint8_t _multi_test(uint8_t len, char *param[]);
#define MULTI_CLI_ENTRY { "mulset",   "multi smart config",    _multi_test },

#ifdef __cplusplus
}
#endif


#endif

