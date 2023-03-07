
#ifndef __BFLB_PLATFORM__H__
#define __BFLB_PLATFORM__H__

#include "stdio.h"
#include "stdint.h"
#include "string.h"
#ifdef BFLB_MCU_SDK
#include "mcu_sdk_version.h"
#endif

void *	pvPortMalloc( size_t xWantedSize );
void* 	pvPortRealloc(void* ptr, size_t newsize);
void* 	pvPortCalloc(size_t numElements, size_t sizeOfElement);
void 	vPortFree( void *pv );

#define bflb_platform_malloc           pvPortMalloc
#define bflb_platform_free             vPortFree
#define bflb_platform_calloc           pvPortCalloc
#define bflb_platform_realloc          pvPortRealloc

void bflb_platform_init(uint32_t baudrate);
void bflb_platform_deinit(void);
void bflb_platform_printf(char *fmt,...);
void bflb_platform_dump(const void *data,uint32_t len);
void bflb_platform_prints(char *data);
void bflb_platform_printx(uint32_t val);
void bflb_platform_printc(char c);
void bflb_platform_print_set(uint8_t logDisable);
int bflb_platform_get_random(uint8_t *data,uint32_t len);
int bflb_platform_get_input(uint8_t *data,uint32_t maxLen);

void bflb_platform_clear_time(void);
uint32_t  bflb_platform_get_systick_int_cnt(void);
uint64_t  bflb_platform_get_time_ms();
void bflb_platform_start_time(void);
void bflb_platform_stop_time(void);
void bflb_platform_set_alarm_time(uint64_t time);
void bflb_platform_init_time(void);
void bflb_platform_deinit_time(void);
void bflb_platform_delay_ms(uint32_t time);
uint32_t bflb_platform_get_log(uint8_t *data,uint32_t maxlen);


#define MSG(a,...)              bflb_platform_printf(a,##__VA_ARGS__)
#define MSG_DBG(a,...)          bflb_platform_printf(a,##__VA_ARGS__)
#define MSG_WAR(a,...)          bflb_platform_printf(a,##__VA_ARGS__)
#define MSG_ERR(a,...)          bflb_platform_printf(a,##__VA_ARGS__)
#define BL_CASE_FAIL            {MSG(__FILE__);MSG(" Case Fail");while(1){BL602_Delay_US(10);}}
#define BL_CASE_SUCCESS         {MSG(__FILE__); MSG(" Case Success");while(1){BL602_Delay_US(10);}}

#endif
