/**
 * @file
 * Handler for Command Class Simple AV.
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _CC_SIMPLEAV_H_
#define _CC_SIMPLEAV_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZW_typedefs.h>
#include <CC_Common.h>


#define MUTE                  0x0001 /**< Buttons*/
#define BT_0                  0x0006 /**< Buttons*/
#define BT_1                  0x0007 /**< Buttons*/
#define BT_2                  0x0008 /**< Buttons*/

#define BT_3                  0x0009 /**< Buttons*/
#define BT_4                  0x000A /**< Buttons*/
#define BT_5                  0x000B /**< Buttons*/
#define BT_6                  0x000C /**< Buttons*/
#define BT_7                  0x000D /**< Buttons*/
#define BT_8                  0x000E /**< Buttons*/
#define BT_9                  0x000F /**< Buttons*/

#define PLAY                  0x0013 /**< Buttons*/
#define STOP                  0x0014 /**< Buttons*/
#define PAUSE                 0x0015 /**< Buttons*/
#define FWD                   0x0016 /**< Buttons*/
#define REV                   0x0017 /**< Buttons*/

#define MENU                  0x001D /**< Buttons*/
#define UP                    0x001E /**< Buttons*/
#define DOWN                  0x001F /**< Buttons*/
#define LEFT                  0x0020 /**< Buttons*/
#define RIGHT                 0x0021 /**< Buttons*/
#define PAGEUP                0x0022 /**< Buttons*/
#define PAGE_DOWN             0x0023 /**< Buttons*/
#define ENTER                 0x0024 /**< Buttons*/
#define ON_OFF                0x0027 /**< Buttons*/

#define ANGLE                 0x003C /**< Buttons*/
#define AUDIO                 0x0041 /**< Buttons*/
#define RETURN                0x004B /**< Buttons*/
#define DELETE                0x007D /**< Buttons*/
#define USB_DVDROM_EJECT      0x0091 /**< Buttons*/
#define BLUE                  0x009A /**< Buttons*/
#define GREEN                 0x009B /**< Buttons*/
#define RED                   0x009D /**< Buttons*/
#define YELLOW                0x009F /**< Buttons*/
#define HOME                  0x00AF /**< Buttons*/
#define REPEAT                0x0107 /**< Buttons*/
#define SETUP                 0x0115 /**< Buttons*/
#define NEXT                  0x011B /**< Buttons*/
#define PREV                  0x011C /**< Buttons*/
#define SLOW                  0x011E /**< Buttons*/
#define SUBTITLE              0x0130 /**< Buttons*/
#define TITLE                 0x0156 /**< Buttons*/
#define ZOOM                  0x0169 /**< Buttons*/
#define INFO                  0x017A /**< Buttons*/
#define CAPS_NUM              0x017B /**< Buttons*/
#define TV_MODE               0x017C /**< Buttons*/
#define SOURCE                0x017D /**< Buttons*/
#define FILE_MODE             0x017E /**< Buttons*/
#define TIME_SEEK             0x017F /**< Buttons*/
#define SUSPEND               0x0194 /**< Buttons*/

#define NO_KEY                0xFFFF  /**< no Buttons*/
#define NOT_BTN               0x0000 /**< not Buttons*/

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/


/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/

/**
 * @brief getApplSimpleAvSupported
 * Get the supported AV commands bitmask bytes assigned to report number reportNo
 * The supported AV commands
 * @param[in] reportNo the report number of the AV commands bitmask bytes
 * @param[out] avCmdBitMask the generated AV commands bitmask bytes
 * @return length of the supported AV cmd report
 */
extern uint8_t
getApplSimpleAvSupported (uint8_t reportNo,
                         uint8_t *avCmdBitMask);

/**
 * @brief getApplSimpleAvReports
 * Get the supported AV commands bitmask bytes reports number
 * The supported AV commands is reported as a bit mask where bit 0 in bytes 1 is 1 if AV cmd#1
 * is supported else its false. The bitmask always start from AV cmd# 1 to latest supported AV cmd#
 * the bit mask and be devided over several reports.
 * @return number of the AV commands bitmask bytes reports
  */
extern uint8_t
getApplSimpleAvReports ();


/**
 * @brief CmdClassSimpleAvSet
 * Sent a somple AV command to a destination node
 * @param[in] pTxOptionsEx Transmit options of type TRANSMIT_OPTIONS_TYPE_EX
 * @param[in] bCommand: AV command
 * @param[in] bKeyAttrib: AV command attribute
 * @param[out] pCbFunc: call back function
 * @return JOB status
 */
extern JOB_STATUS
CmdClassSimpleAvSet(
    TRANSMIT_OPTIONS_TYPE_SINGLE_EX* pTxOptionsEx,
  uint16_t bCommand,
  uint8_t bKeyAttrib,
  VOID_CALLBACKFUNC(pCbFunc)(TRANSMISSION_RESULT * pTransmissionResult));

#endif /*  _CC_SIMPLEAV_H_ */
