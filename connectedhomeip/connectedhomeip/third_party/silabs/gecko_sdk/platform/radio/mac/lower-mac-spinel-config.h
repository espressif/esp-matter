#ifndef LOWER_MAC_SPINEL_CONFIG_H
#define LOWER_MAC_SPINEL_CONFIG_H

/**
 * Log level None.
 *
 * @note Log Levels are defines so that embedded implementations can eliminate code at compile time via
 * #if/#else/#endif.
 *
 */
#define OT_LOG_LEVEL_NONE               0

/**
 * Log level Critical.
 *
 * @note Log Levels are defines so that embedded implementations can eliminate code at compile time via
 * #if/#else/#endif.
 *
 */
#define OT_LOG_LEVEL_CRIT               1

/**
 * Log level Warning.
 *
 * @note Log Levels are defines so that embedded implementations can eliminate code at compile time via
 * #if/#else/#endif.
 *
 */
#define OT_LOG_LEVEL_WARN               2

/**
 * Log level Notice.
 *
 * @note Log Levels are defines so that embedded implementations can eliminate code at compile time via
 * #if/#else/#endif.
 *
 */
#define OT_LOG_LEVEL_NOTE               3

/**
 * Log level Informational.
 *
 * @note Log Levels are defines so that embedded implementations can eliminate code at compile time via
 * #if/#else/#endif.
 *
 */
#define OT_LOG_LEVEL_INFO               4

/**
 * Log level Debug.
 *
 * @note Log Levels are defines so that embedded implementations can eliminate code at compile time via
 * #if/#else/#endif.
 *
 */
#define OT_LOG_LEVEL_DEBG               5

#define OPENTHREAD_FTD 1
#define OPENTHREAD_CONFIG_HEAP_EXTERNAL_ENABLE 1
#define OPENTHREAD_SPINEL_CONFIG_RCP_RESTORATION_MAX_COUNT 100

// This is a workaround for a Mac OS X specific problem in hdlc_interface.cpp,
// in which an include of <util.h> accidentally includes app/framework/util.h.
#define OPENTHREAD_POSIX_CONFIG_RCP_PTY_ENABLE 0

/**
 * Transmit timeout
 *
 * @note The host waits this amount of time to receieve a response from the RCP after which it will notify
 * the stack of a failure.
 *
 */
#define SL_HOST_TX_WAIT_MS         5 * 1000

/**
 * Log to Platform
 *
 * We only use platform logging for now
 *
 */
#define OPENTHREAD_CONFIG_LOG_PLATFORM              1

#define OPENTHREAD_CONFIG_LOG_LEVEL_DYNAMIC_ENABLE  1
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <e>  MULTIPAN RCP
#define OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE       1
// </e>

// <o   OPENTHREAD_CONFIG_LOG_LEVEL> LOG_LEVEL
//      <OT_LOG_LEVEL_NONE       => NONE
//      <OT_LOG_LEVEL_CRIT       => CRIT
//      <OT_LOG_LEVEL_WARN       => WARN
//      <OT_LOG_LEVEL_NOTE       => NOTE
//      <OT_LOG_LEVEL_INFO       => INFO
//      <OT_LOG_LEVEL_DEBG       => DEBG
// <i>  Default: OT_LOG_LEVEL_DEBG
#define OPENTHREAD_CONFIG_LOG_LEVEL OT_LOG_LEVEL_DEBG

//Dont use PSA for multiPAN builds.
#define OPENTHREAD_CONFIG_PLATFORM_KEY_REFERENCES_ENABLE 0

// for mfglib commands
#define OPENTHREAD_CONFIG_DIAG_ENABLE 1

// <<< end of configuration section >>>

#endif // LOWER_MAC_SPINEL_CONFIG_H
