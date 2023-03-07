#ifndef __USBD_CONF__H__
#define __USBD_CONF__H__

#include "bl702_common.h"
#include "usb_def.h"

/*---------- -----------*/
#define USBD_MAX_NUM_INTERFACES            1
/*---------- -----------*/
#define USBD_MAX_NUM_CONFIGURATION        1
/*---------- -----------*/
#define USBD_MAX_STR_DESC_SIZ            512
/*---------- -----------*/
#define USBD_SUPPORT_USER_STRING        0
/*---------- -----------*/
#define USBD_DEBUG_LEVEL                0
/*---------- -----------*/
#define USBD_SELF_POWERED                1
/*---------- -----------*/
#define MAX_STATIC_ALLOC_SIZE            512
/*---------- -----------*/
#define USBD_LPM_ENABLED                0
/*---------- -----------*/

/* #define for FS and HS identification */
#define DEVICE_FS         0

/** Alias for memory allocation. */
#define USBD_malloc         (uint32_t *)USBD_static_malloc

/** Alias for memory release. */
#define USBD_free           USBD_static_free

/** Alias for memory set. */
#define USBD_memset         /* Not used */

/** Alias for memory copy. */
#define USBD_memcpy         /* Not used */

#define USBD_Delay          BL702_Delay_MS


#if (USBD_DEBUG_LEVEL > 0)
#define USBD_UsrLog(str)    MSG(str)
#else
#define USBD_UsrLog(...)
#endif


#if (USBD_DEBUG_LEVEL > 1)
#define USBD_ErrLog(str)    MSG(str)
#else
#define USBD_ErrLog(...)
#endif


#if (USBD_DEBUG_LEVEL > 2)
#define USBD_DbgLog(a,...)    MSG(a,##__VA_ARGS__)
#else
#define USBD_DbgLog(a,...)
#endif


void *USBD_static_malloc(uint32_t size);
void USBD_static_free(void *p);


#endif
