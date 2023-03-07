
#ifndef __HTTPD_CFG_H__
#define __HTTPD_CFG_H__

//#define LWIP_HTTPD_CGI                       (1)
//#define LWIP_HTTPD_SUPPORT_11_KEEPALIVE      (1)

#define LWIP_HTTPD_EXAMPLE_CUSTOMFILES       (1)
#define HTTPD_FSDATA_FILE                    "fsdata_custom.h"
#define LWIP_HTTPD_FS_ASYNC_READ             (0) /* now not support 1 */
#define LWIP_HTTPD_DYNAMIC_FILE_READ         (1)
#define LWIP_HTTPD_CUSTOM_FILES              (1)
#define LWIP_HTTPD_FSROOT_DIR                "/romfs"

#endif
