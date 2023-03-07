/*
 * wpa_supplicant/hostapd / Debug prints
 * Copyright (c) 2002-2013, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef WPA_DEBUG_H
#define WPA_DEBUG_H

#include "wpabuf.h"

extern int wpa_debug_level;
extern int wpa_debug_show_keys;
extern int wpa_debug_timestamp;
extern int wpa_debug_syslog;

/* Debugging function - conditional printf and hex dump. Driver wrappers can
 * use these for debugging purposes. */

/* Define debug category:
 * (1) ERROR (2) WARN (3) STATE (4) EVENT (5) TRACE (6) INFO (7) LOUD (8) TEMP
 */
#define DBG_BIT(n)              (1UL << (n))
#define MSG_ERROR        DBG_BIT(0)
#define MSG_WARNING      DBG_BIT(1)
#define MSG_INFO         DBG_BIT(2)
#define MSG_DEBUG        DBG_BIT(3)
#define MSG_MSGDUMP      DBG_BIT(4)
#define MSG_EXCESSIVE    DBG_BIT(5)
#define DBG_MINISUPP_LOG_LEVEL_ERROR \
	MSG_ERROR
#define DBG_MINISUPP_LOG_LEVEL_WARN \
	(DBG_MINISUPP_LOG_LEVEL_ERROR | \
	MSG_WARNING)
#define DBG_MINISUPP_LOG_LEVEL_INFO \
	(DBG_MINISUPP_LOG_LEVEL_WARN | \
	MSG_INFO)
#define DBG_MINISUPP_LOG_LEVEL_DEBUG \
	(DBG_MINISUPP_LOG_LEVEL_INFO | \
	MSG_DEBUG)
#define DBG_MINISUPP_LOG_LEVEL_EXTREME \
	(DBG_MINISUPP_LOG_LEVEL_DEBUG | \
	MSG_MSGDUMP | \
	MSG_EXCESSIVE)

#ifdef CONFIG_NO_STDOUT_DEBUG

#define wpa_debug_print_timestamp() do { } while (0)
#define wpa_printf(args...) do { } while (0)
#define wpa_hexdump(l,t,b,le) do { } while (0)
#define wpa_hexdump_buf(l,t,b) do { } while (0)
#define wpa_hexdump_key(l,t,b,le) do { } while (0)
#define wpa_hexdump_buf_key(l,t,b) do { } while (0)
#define wpa_hexdump_ascii(l,t,b,le) do { } while (0)
#define wpa_hexdump_ascii_key(l,t,b,le) do { } while (0)
#define wpa_debug_open_file(p) do { } while (0)
#define wpa_debug_close_file() do { } while (0)
#define wpa_debug_setup_stdout() do { } while (0)
#define wpa_dbg(args...) do { } while (0)

static inline int wpa_debug_reopen_file(void)
{
	return 0;
}

#else /* CONFIG_NO_STDOUT_DEBUG */

int wpa_debug_open_file(const char *path);
int wpa_debug_reopen_file(void);
void wpa_debug_close_file(void);
void wpa_debug_setup_stdout(void);

/**
 * wpa_debug_printf_timestamp - Print timestamp for debug output
 *
 * This function prints a timestamp in seconds_from_1970.microsoconds
 * format if debug output has been configured to include timestamps in debug
 * messages.
 */
void wpa_debug_print_timestamp(void);

/**
 * wpa_printf - conditional printf
 * @level: priority level (MSG_*) of the message
 * @fmt: printf format string, followed by optional arguments
 *
 * This function is used to print conditional debugging and error messages. The
 * output may be directed to stdout, stderr, and/or syslog based on
 * configuration.
 *
 * Note: New line '\n' is added to the end of the text when printing to stdout.
 */
#define CFG_MINISUPP_DBG_ENABLE_CLASS    DBG_MINISUPP_LOG_LEVEL_INFO

#define wpa_printf(level, ...) \
	wpa_printf_##level(level, ##__VA_ARGS__)
#define wpa_hexdump(level, ...) \
	wpa_hexdump_##level(level, ##__VA_ARGS__)
#define wpa_hexdump_key(level, ...) \
	wpa_hexdump_key_##level(level, ##__VA_ARGS__)
#define wpa_hexdump_ascii(level, ...) \
	wpa_hexdump_ascii_##level(level, ##__VA_ARGS__)
#define wpa_hexdump_ascii_key(level, ...) \
	wpa_hexdump_ascii_key_##level(level, ##__VA_ARGS__)
#define wpa_hexdump_buf(level, ...) \
	wpa_hexdump_buf_##level(level, ##__VA_ARGS__)
#define wpa_hexdump_buf_key(level, ...) \
	wpa_hexdump_buf_key_##level(level, ##__VA_ARGS__)
#define wpa_msg(wpa_s, level, ...) \
	wpa_msg_##level(wpa_s, level, ##__VA_ARGS__)
#define wpa_dbg(wpa_s, level, ...) \
	wpa_msg_##level(wpa_s, level, ##__VA_ARGS__)
#define wpa_msg_ctrl(wpa_s, level, ...) \
	wpa_msg_ctrl_##level(wpa_s, level, ##__VA_ARGS__)

#define __MINISUPP_DBGLOG_EMPTY__(level, ...) \
	do { \
	} while (0)

#define __MINISUPP_DBGLOG__(level, ...) \
	do { \
		if (level <= wpa_debug_level) { \
			LOG_I(minisupp, ##__VA_ARGS__); \
		} \
	} while (0)

#define __MINISUPP_HEXDUMP__(level, ...) \
	do { \
		minisupp_wpa_hexdump(level, ##__VA_ARGS__); \
	} while (0)

#define __MINISUPP_HEXDUMP_KEY__(level, ...) \
	do { \
		minisupp_wpa_hexdump_key(level, ##__VA_ARGS__); \
	} while (0)

#define __MINISUPP_HEXDUMP_ASCII__(level, ...) \
	do { \
		minisupp_wpa_hexdump_ascii(level, ##__VA_ARGS__); \
	} while (0)

#define __MINISUPP_HEXDUMP_ASCII_KEY__(level, ...) \
	do { \
		minisupp_wpa_hexdump_ascii_key(level, ##__VA_ARGS__); \
	} while (0)

#define __MINISUPP_HEXDUMP_BUF__(level, ...) \
	do { \
		minisupp_wpa_hexdump_buf(level, ##__VA_ARGS__); \
	} while (0)

#define __MINISUPP_HEXDUMP_BUF_KEY__(level, ...) \
	do { \
		minisupp_wpa_hexdump_buf_key(level, ##__VA_ARGS__); \
	} while (0)

#define __MINISUPP_WPA_MSG__(args...) \
	do { \
		minisupp_wpa_msg(args); \
	} while (0)

#define __MINISUPP_WPA_MSG_CTRL__(args...) \
	do { \
		minisupp_wpa_msg_ctrl(args); \
	} while (0)

#define __MINISUPP_WPA_PRINTF__(args...) \
	do { \
		minisupp_wpa_printf(args); \
	} while (0)

#define __MINISUPP_DBGLOG_ERROR__(level, ...) \
	do { \
		LOG_E(minisupp, ##__VA_ARGS__); \
	} while (0)

#define __MINISUPP_DBGLOG_WARNING__(level, ...) \
	do { \
		LOG_W(minisupp, ##__VA_ARGS__); \
	} while (0)

#define __MINISUPP_DBGLOG_INFO__(level, ...) \
	do { \
		LOG_I(minisupp, ##__VA_ARGS__); \
	} while (0)

#define __MINISUPP_DBGLOG_DEBUG__(level, ...) \
	do { \
		LOG_D(minisupp, ##__VA_ARGS__); \
	} while (0)

/* runtime debug level "wpa_debug_level", "level" */
#define wpa_printf_wpa_debug_level __MINISUPP_DBGLOG__
#define wpa_printf_level __MINISUPP_WPA_PRINTF__
#define wpa_hexdump_ascii_level __MINISUPP_DBGLOG__
#define wpa_msg_level __MINISUPP_WPA_MSG__
#define wpa_msg_ctrl_level __MINISUPP_WPA_MSG_CTRL__

#if (CFG_MINISUPP_DBG_ENABLE_CLASS & MSG_ERROR)
#if !defined(MTK_RELEASE_MODE) || (MTK_RELEASE_MODE != MTK_M_RELEASE)
#define wpa_printf_MSG_ERROR __MINISUPP_DBGLOG_ERROR__
#else
#define wpa_printf_MSG_ERROR __MINISUPP_WPA_PRINTF__
#endif
#define wpa_hexdump_MSG_ERROR __MINISUPP_HEXDUMP__
#define wpa_hexdump_key_MSG_ERROR __MINISUPP_HEXDUMP_KEY__
#define wpa_hexdump_ascii_MSG_ERROR __MINISUPP_HEXDUMP_ASCII__
#define wpa_hexdump_ascii_key_MSG_ERROR __MINISUPP_HEXDUMP_ASCII_KEY__
#define wpa_hexdump_buf_MSG_ERROR __MINISUPP_HEXDUMP_BUF__
#define wpa_hexdump_buf_key_MSG_ERROR __MINISUPP_HEXDUMP_BUF_KEY__
#define wpa_msg_MSG_ERROR __MINISUPP_WPA_MSG__
#define wpa_msg_ctrl_MSG_ERROR __MINISUPP_WPA_MSG_CTRL__
#else
#define wpa_printf_MSG_ERROR __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_MSG_ERROR __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_key_MSG_ERROR __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_ascii_MSG_ERROR __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_ascii_key_MSG_ERROR __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_buf_MSG_ERROR __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_buf_key_MSG_ERROR __MINISUPP_DBGLOG_EMPTY__
#define wpa_msg_MSG_ERROR __MINISUPP_DBGLOG_EMPTY__
#define wpa_msg_ctrl_MSG_ERROR __MINISUPP_DBGLOG_EMPTY__
#endif

#if (CFG_MINISUPP_DBG_ENABLE_CLASS & MSG_WARNING)
#if !defined(MTK_RELEASE_MODE) || (MTK_RELEASE_MODE != MTK_M_RELEASE)
#define wpa_printf_MSG_WARNING __MINISUPP_DBGLOG_WARNING__
#else
#define wpa_printf_MSG_WARNING __MINISUPP_WPA_PRINTF__
#endif
#define wpa_hexdump_MSG_WARNING __MINISUPP_HEXDUMP__
#define wpa_hexdump_key_MSG_WARNING __MINISUPP_HEXDUMP_KEY__
#define wpa_hexdump_ascii_MSG_WARNING __MINISUPP_HEXDUMP_ASCII__
#define wpa_hexdump_ascii_key_MSG_WARNING __MINISUPP_HEXDUMP_ASCII_KEY__
#define wpa_hexdump_buf_MSG_WARNING __MINISUPP_HEXDUMP_BUF__
#define wpa_hexdump_buf_key_MSG_WARNING __MINISUPP_HEXDUMP_BUF_KEY__
#define wpa_msg_MSG_WARNING __MINISUPP_WPA_MSG__
#define wpa_msg_ctrl_MSG_WARNING __MINISUPP_WPA_MSG_CTRL__
#else
#define wpa_printf_MSG_WARNING __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_MSG_WARNING __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_key_MSG_WARNING __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_ascii_MSG_WARNING __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_ascii_key_MSG_WARNING __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_buf_MSG_WARNING __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_buf_key_MSG_WARNING __MINISUPP_DBGLOG_EMPTY__
#define wpa_msg_MSG_WARNING __MINISUPP_DBGLOG_EMPTY__
#define wpa_msg_ctrl_MSG_WARNING __MINISUPP_DBGLOG_EMPTY__
#endif

#if (CFG_MINISUPP_DBG_ENABLE_CLASS & MSG_INFO)
#if !defined(MTK_RELEASE_MODE) || (MTK_RELEASE_MODE != MTK_M_RELEASE)
#define wpa_printf_MSG_INFO __MINISUPP_DBGLOG_INFO__
#else
#define wpa_printf_MSG_INFO __MINISUPP_WPA_PRINTF__
#endif
#define wpa_hexdump_MSG_INFO __MINISUPP_HEXDUMP__
#define wpa_hexdump_key_MSG_INFO __MINISUPP_HEXDUMP_KEY__
#define wpa_hexdump_ascii_MSG_INFO __MINISUPP_HEXDUMP_ASCII__
#define wpa_hexdump_ascii_key_MSG_INFO __MINISUPP_HEXDUMP_ASCII_KEY__
#define wpa_hexdump_buf_MSG_INFO __MINISUPP_HEXDUMP_BUF__
#define wpa_hexdump_buf_key_MSG_INFO __MINISUPP_HEXDUMP_BUF_KEY__
#define wpa_msg_MSG_INFO __MINISUPP_WPA_MSG__
#define wpa_msg_ctrl_MSG_INFO __MINISUPP_WPA_MSG_CTRL__
#else
#define wpa_printf_MSG_INFO __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_MSG_INFO __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_key_MSG_INFO __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_ascii_MSG_INFO __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_ascii_key_MSG_INFO __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_buf_MSG_INFO __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_buf_key_MSG_INFO __MINISUPP_DBGLOG_EMPTY__
#define wpa_msg_MSG_INFO __MINISUPP_DBGLOG_EMPTY__
#define wpa_msg_ctrl_MSG_INFO __MINISUPP_DBGLOG_EMPTY__
#endif

#if (CFG_MINISUPP_DBG_ENABLE_CLASS & MSG_DEBUG)
#if !defined(MTK_RELEASE_MODE) || (MTK_RELEASE_MODE != MTK_M_RELEASE)
#define wpa_printf_MSG_DEBUG __MINISUPP_DBGLOG_DEBUG__
#else
#define wpa_printf_MSG_DEBUG __MINISUPP_WPA_PRINTF__
#endif
#define wpa_hexdump_MSG_DEBUG __MINISUPP_HEXDUMP__
#define wpa_hexdump_key_MSG_DEBUG __MINISUPP_HEXDUMP_KEY__
#define wpa_hexdump_ascii_MSG_DEBUG __MINISUPP_HEXDUMP_ASCII__
#define wpa_hexdump_ascii_key_MSG_DEBUG __MINISUPP_HEXDUMP_ASCII_KEY__
#define wpa_hexdump_buf_MSG_DEBUG __MINISUPP_HEXDUMP_BUF__
#define wpa_hexdump_buf_key_MSG_DEBUG __MINISUPP_HEXDUMP_BUF_KEY__
#define wpa_msg_MSG_DEBUG __MINISUPP_WPA_MSG__
#define wpa_msg_ctrl_MSG_DEBUG __MINISUPP_WPA_MSG_CTRL__
#else
#define wpa_printf_MSG_DEBUG __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_MSG_DEBUG __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_key_MSG_DEBUG __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_ascii_MSG_DEBUG __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_ascii_key_MSG_DEBUG __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_buf_MSG_DEBUG __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_buf_key_MSG_DEBUG __MINISUPP_DBGLOG_EMPTY__
#define wpa_msg_MSG_DEBUG __MINISUPP_DBGLOG_EMPTY__
#define wpa_msg_ctrl_MSG_DEBUG __MINISUPP_DBGLOG_EMPTY__
#endif

#if (CFG_MINISUPP_DBG_ENABLE_CLASS & MSG_MSGDUMP)
#if !defined(MTK_RELEASE_MODE) || (MTK_RELEASE_MODE != MTK_M_RELEASE)
#define wpa_printf_MSG_MSGDUMP __MINISUPP_DBGLOG_DEBUG__
#else
#define wpa_printf_MSG_MSGDUMP __MINISUPP_WPA_PRINTF__
#endif
#define wpa_hexdump_MSG_MSGDUMP __MINISUPP_HEXDUMP__
#define wpa_hexdump_key_MSG_MSGDUMP __MINISUPP_HEXDUMP_KEY__
#define wpa_hexdump_ascii_MSG_MSGDUMP __MINISUPP_HEXDUMP_ASCII__
#define wpa_hexdump_ascii_key_MSG_MSGDUMP __MINISUPP_HEXDUMP_ASCII_KEY__
#define wpa_hexdump_buf_MSG_MSGDUMP __MINISUPP_HEXDUMP_BUF__
#define wpa_hexdump_buf_key_MSG_MSGDUMP __MINISUPP_HEXDUMP_BUF_KEY__
#define wpa_msg_MSG_MSGDUMP __MINISUPP_WPA_MSG__
#define wpa_msg_ctrl_MSG_MSGDUMP __MINISUPP_WPA_MSG_CTRL__
#else
#define wpa_printf_MSG_MSGDUMP __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_MSG_MSGDUMP __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_key_MSG_MSGDUMP __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_ascii_MSG_MSGDUMP __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_ascii_key_MSG_MSGDUMP __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_buf_MSG_MSGDUMP __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_buf_key_MSG_MSGDUMP __MINISUPP_DBGLOG_EMPTY__
#define wpa_msg_MSG_MSGDUMP __MINISUPP_DBGLOG_EMPTY__
#define wpa_msg_ctrl_MSG_MSGDUMP __MINISUPP_DBGLOG_EMPTY__
#endif

#if (CFG_MINISUPP_DBG_ENABLE_CLASS & MSG_EXCESSIVE)
#if !defined(MTK_RELEASE_MODE) || (MTK_RELEASE_MODE != MTK_M_RELEASE)
#define wpa_printf_MSG_EXCESSIVE __MINISUPP_DBGLOG_DEBUG__
#else
#define wpa_printf_MSG_EXCESSIVE __MINISUPP_WPA_PRINTF__
#endif
#define wpa_hexdump_MSG_EXCESSIVE __MINISUPP_HEXDUMP__
#define wpa_hexdump_key_MSG_EXCESSIVE __MINISUPP_HEXDUMP_KEY__
#define wpa_hexdump_ascii_MSG_EXCESSIVE __MINISUPP_HEXDUMP_ASCII__
#define wpa_hexdump_ascii_key_MSG_EXCESSIVE __MINISUPP_HEXDUMP_ASCII_KEY__
#define wpa_hexdump_buf_MSG_EXCESSIVE __MINISUPP_HEXDUMP_BUF__
#define wpa_hexdump_buf_key_MSG_EXCESSIVE __MINISUPP_HEXDUMP_BUF_KEY__
#define wpa_msg_MSG_EXCESSIVE __MINISUPP_WPA_MSG__
#define wpa_msg_ctrl_MSG_EXCESSIVE __MINISUPP_WPA_MSG_CTRL__
#else
#define wpa_printf_MSG_EXCESSIVE __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_MSG_EXCESSIVE __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_key_MSG_EXCESSIVE __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_ascii_MSG_EXCESSIVE __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_ascii_key_MSG_EXCESSIVE __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_buf_MSG_EXCESSIVE __MINISUPP_DBGLOG_EMPTY__
#define wpa_hexdump_buf_key_MSG_EXCESSIVE __MINISUPP_DBGLOG_EMPTY__
#define wpa_msg_MSG_EXCESSIVE __MINISUPP_DBGLOG_EMPTY__
#define wpa_msg_ctrl_MSG_EXCESSIVE __MINISUPP_DBGLOG_EMPTY__
#endif

/**
 * wpa_hexdump - conditional hex dump
 * @level: priority level (MSG_*) of the message
 * @title: title of for the message
 * @buf: data buffer to be dumped
 * @len: length of the buf
 *
 * This function is used to print conditional debugging and error messages. The
 * output may be directed to stdout, stderr, and/or syslog based on
 * configuration. The contents of buf is printed out has hex dump.
 */
void minisupp_wpa_hexdump(int level, const char *title, const void *buf,
			  size_t len);

static inline void minisupp_wpa_hexdump_buf(int level, const char *title,
					    const struct wpabuf *buf)
{
	minisupp_wpa_hexdump(level, title, buf ? wpabuf_head(buf) : NULL,
			     buf ? wpabuf_len(buf) : 0);
}

/**
 * wpa_hexdump_key - conditional hex dump, hide keys
 * @level: priority level (MSG_*) of the message
 * @title: title of for the message
 * @buf: data buffer to be dumped
 * @len: length of the buf
 *
 * This function is used to print conditional debugging and error messages. The
 * output may be directed to stdout, stderr, and/or syslog based on
 * configuration. The contents of buf is printed out has hex dump. This works
 * like wpa_hexdump(), but by default, does not include secret keys (passwords,
 * etc.) in debug output.
 */
void minisupp_wpa_hexdump_key(int level, const char *title, const void *buf,
			      size_t len);

static inline void minisupp_wpa_hexdump_buf_key(int level, const char *title,
						const struct wpabuf *buf)
{
	minisupp_wpa_hexdump_key(level, title, buf ? wpabuf_head(buf) : NULL,
				 buf ? wpabuf_len(buf) : 0);
}

/**
 * wpa_hexdump_ascii - conditional hex dump
 * @level: priority level (MSG_*) of the message
 * @title: title of for the message
 * @buf: data buffer to be dumped
 * @len: length of the buf
 *
 * This function is used to print conditional debugging and error messages. The
 * output may be directed to stdout, stderr, and/or syslog based on
 * configuration. The contents of buf is printed out has hex dump with both
 * the hex numbers and ASCII characters (for printable range) are shown. 16
 * bytes per line will be shown.
 */
void minisupp_wpa_hexdump_ascii(int level, const char *title, const void *buf,
				size_t len);

/**
 * wpa_hexdump_ascii_key - conditional hex dump, hide keys
 * @level: priority level (MSG_*) of the message
 * @title: title of for the message
 * @buf: data buffer to be dumped
 * @len: length of the buf
 *
 * This function is used to print conditional debugging and error messages. The
 * output may be directed to stdout, stderr, and/or syslog based on
 * configuration. The contents of buf is printed out has hex dump with both
 * the hex numbers and ASCII characters (for printable range) are shown. 16
 * bytes per line will be shown. This works like wpa_hexdump_ascii(), but by
 * default, does not include secret keys (passwords, etc.) in debug output.
 */
void minisupp_wpa_hexdump_ascii_key(int level, const char *title,
				    const void *buf, size_t len);

#endif /* CONFIG_NO_STDOUT_DEBUG */


#ifdef CONFIG_NO_STDOUT_DEBUG
#define minisupp_wpa_msg(args...) do { } while (0)
#define minisupp_wpa_msg_ctrl(args...) do { } while (0)
#define minisupp_wpa_printf(args...) do { } while (0)
#define wpa_msg(args...) do { } while (0)
#define wpa_msg_ctrl(args...) do { } while (0)
#define wpa_msg_global(args...) do { } while (0)
#define wpa_msg_global_ctrl(args...) do { } while (0)
#define wpa_msg_no_global(args...) do { } while (0)
#define wpa_msg_global_only(args...) do { } while (0)
#define wpa_msg_register_cb(f) do { } while (0)
#define wpa_msg_register_ifname_cb(f) do { } while (0)
#else /* CONFIG_NO_WPA_MSG */
/**
 * wpa_msg - Conditional printf for default target and ctrl_iface monitors
 * @ctx: Pointer to context data; this is the ctx variable registered
 *	with struct wpa_driver_ops::init()
 * @level: priority level (MSG_*) of the message
 * @fmt: printf format string, followed by optional arguments
 *
 * This function is used to print conditional debugging and error messages. The
 * output may be directed to stdout, stderr, and/or syslog based on
 * configuration. This function is like wpa_printf(), but it also sends the
 * same message to all attached ctrl_iface monitors.
 *
 * Note: New line '\n' is added to the end of the text when printing to stdout.
 */
void minisupp_wpa_msg(void *ctx, int level, const char *fmt, ...) PRINTF_FORMAT(3, 4);

/**
 * wpa_msg_ctrl - Conditional printf for ctrl_iface monitors
 * @ctx: Pointer to context data; this is the ctx variable registered
 *	with struct wpa_driver_ops::init()
 * @level: priority level (MSG_*) of the message
 * @fmt: printf format string, followed by optional arguments
 *
 * This function is used to print conditional debugging and error messages.
 * This function is like wpa_msg(), but it sends the output only to the
 * attached ctrl_iface monitors. In other words, it can be used for frequent
 * events that do not need to be sent to syslog.
 */
void minisupp_wpa_msg_ctrl(void *ctx, int level, const char *fmt, ...)
PRINTF_FORMAT(3, 4);

void minisupp_wpa_printf(int level, const char *fmt, ...) PRINTF_FORMAT(2, 3);

/**
 * wpa_msg_global - Global printf for ctrl_iface monitors
 * @ctx: Pointer to context data; this is the ctx variable registered
 *	with struct wpa_driver_ops::init()
 * @level: priority level (MSG_*) of the message
 * @fmt: printf format string, followed by optional arguments
 *
 * This function is used to print conditional debugging and error messages.
 * This function is like wpa_msg(), but it sends the output as a global event,
 * i.e., without being specific to an interface. For backwards compatibility,
 * an old style event is also delivered on one of the interfaces (the one
 * specified by the context data).
 */
void wpa_msg_global(void *ctx, int level, const char *fmt, ...)
PRINTF_FORMAT(3, 4);

/**
 * wpa_msg_global_ctrl - Conditional global printf for ctrl_iface monitors
 * @ctx: Pointer to context data; this is the ctx variable registered
 *	with struct wpa_driver_ops::init()
 * @level: priority level (MSG_*) of the message
 * @fmt: printf format string, followed by optional arguments
 *
 * This function is used to print conditional debugging and error messages.
 * This function is like wpa_msg_global(), but it sends the output only to the
 * attached global ctrl_iface monitors. In other words, it can be used for
 * frequent events that do not need to be sent to syslog.
 */
void wpa_msg_global_ctrl(void *ctx, int level, const char *fmt, ...)
PRINTF_FORMAT(3, 4);

/**
 * wpa_msg_no_global - Conditional printf for ctrl_iface monitors
 * @ctx: Pointer to context data; this is the ctx variable registered
 *	with struct wpa_driver_ops::init()
 * @level: priority level (MSG_*) of the message
 * @fmt: printf format string, followed by optional arguments
 *
 * This function is used to print conditional debugging and error messages.
 * This function is like wpa_msg(), but it does not send the output as a global
 * event.
 */
void wpa_msg_no_global(void *ctx, int level, const char *fmt, ...)
PRINTF_FORMAT(3, 4);

/**
 * wpa_msg_global_only - Conditional printf for ctrl_iface monitors
 * @ctx: Pointer to context data; this is the ctx variable registered
 *	with struct wpa_driver_ops::init()
 * @level: priority level (MSG_*) of the message
 * @fmt: printf format string, followed by optional arguments
 *
 * This function is used to print conditional debugging and error messages.
 * This function is like wpa_msg_global(), but it sends the output only as a
 * global event.
 */
void wpa_msg_global_only(void *ctx, int level, const char *fmt, ...)
PRINTF_FORMAT(3, 4);

enum wpa_msg_type {
	WPA_MSG_PER_INTERFACE,
	WPA_MSG_GLOBAL,
	WPA_MSG_NO_GLOBAL,
	WPA_MSG_ONLY_GLOBAL,
};

typedef void (*wpa_msg_cb_func)(void *ctx, int level, enum wpa_msg_type type,
				const char *txt, size_t len);

/**
 * wpa_msg_register_cb - Register callback function for wpa_msg() messages
 * @func: Callback function (%NULL to unregister)
 */
void wpa_msg_register_cb(wpa_msg_cb_func func);

typedef const char * (*wpa_msg_get_ifname_func)(void *ctx);
void wpa_msg_register_ifname_cb(wpa_msg_get_ifname_func func);

#endif /* CONFIG_NO_WPA_MSG */

#ifdef CONFIG_NO_HOSTAPD_LOGGER
#define hostapd_logger(args...) do { } while (0)
#define hostapd_logger_register_cb(f) do { } while (0)
#else /* CONFIG_NO_HOSTAPD_LOGGER */
void hostapd_logger(void *ctx, const u8 *addr, unsigned int module, int level,
		    const char *fmt, ...) PRINTF_FORMAT(5, 6);

typedef void (*hostapd_logger_cb_func)(void *ctx, const u8 *addr,
				       unsigned int module, int level,
				       const char *txt, size_t len);

/**
 * hostapd_logger_register_cb - Register callback function for hostapd_logger()
 * @func: Callback function (%NULL to unregister)
 */
void hostapd_logger_register_cb(hostapd_logger_cb_func func);
#endif /* CONFIG_NO_HOSTAPD_LOGGER */

#define HOSTAPD_MODULE_IEEE80211	0x00000001
#define HOSTAPD_MODULE_IEEE8021X	0x00000002
#define HOSTAPD_MODULE_RADIUS		0x00000004
#define HOSTAPD_MODULE_WPA		0x00000008
#define HOSTAPD_MODULE_DRIVER		0x00000010
#define HOSTAPD_MODULE_MLME		0x00000040

enum hostapd_logger_level {
	HOSTAPD_LEVEL_DEBUG_VERBOSE = 0,
	HOSTAPD_LEVEL_DEBUG = 1,
	HOSTAPD_LEVEL_INFO = 2,
	HOSTAPD_LEVEL_NOTICE = 3,
	HOSTAPD_LEVEL_WARNING = 4
};


#ifdef CONFIG_DEBUG_SYSLOG

void wpa_debug_open_syslog(void);
void wpa_debug_close_syslog(void);

#else /* CONFIG_DEBUG_SYSLOG */

static inline void wpa_debug_open_syslog(void)
{
}

static inline void wpa_debug_close_syslog(void)
{
}

#endif /* CONFIG_DEBUG_SYSLOG */

#ifdef CONFIG_DEBUG_LINUX_TRACING

int wpa_debug_open_linux_tracing(void);
void wpa_debug_close_linux_tracing(void);

#else /* CONFIG_DEBUG_LINUX_TRACING */

static inline int wpa_debug_open_linux_tracing(void)
{
	return 0;
}

static inline void wpa_debug_close_linux_tracing(void)
{
}

#endif /* CONFIG_DEBUG_LINUX_TRACING */


#ifdef EAPOL_TEST
#define WPA_ASSERT(a)						       \
	do {							       \
		if (!(a)) {					       \
			printf("WPA_ASSERT FAILED '" #a "' "	       \
			       "%s %s:%d\n",			       \
			       __FUNCTION__, __FILE__, __LINE__);      \
			exit(1);				       \
		}						       \
	} while (0)
#else
#define WPA_ASSERT(a) do { } while (0)
#endif

const char * debug_level_str(int level);
int str_to_debug_level(const char *s);

#endif /* WPA_DEBUG_H */
