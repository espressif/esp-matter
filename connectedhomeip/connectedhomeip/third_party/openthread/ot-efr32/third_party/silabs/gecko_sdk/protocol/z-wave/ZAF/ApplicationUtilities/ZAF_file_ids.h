/**
 * @file
 * Contains a list of file IDs used by the ZAF.
 *
 * Range: 0x51000 - 0x51FFF
 *
 * @attention The file IDs MUST NOT be changed.
 *
 * @copyright 2019 Silicon Laboratories Inc.
 */

#ifndef ZAF_APPLICATIONUTILITIES_ZAF_FILE_IDS_H_
#define ZAF_APPLICATIONUTILITIES_ZAF_FILE_IDS_H_

/**
 * Base ID of ZAF files.
 *
 * MUST NOT be changed.
 */
#define ZAF_FILE_ID_BASE             (0x51000)

/**
 * All file IDs related to ZAF are defined here and they are all based on the above base ID.
 *
 * MUST NOT be changed.
 */
#define ZAF_FILE_ID_APP_VERSION                 (ZAF_FILE_ID_BASE + 0)
#define ZAF_FILE_ID_ASSOCIATIONINFO             (ZAF_FILE_ID_BASE + 1)
#define ZAF_FILE_ID_USERCODE                    (ZAF_FILE_ID_BASE + 2)
#define ZAF_FILE_ID_BATTERYDATA                 (ZAF_FILE_ID_BASE + 3)
#define ZAF_FILE_ID_NOTIFICATIONDATA            (ZAF_FILE_ID_BASE + 4)
#define ZAF_FILE_ID_WAKEUPCCDATA                (ZAF_FILE_ID_BASE + 5)
#define ZAF_FILE_ID_CC_FIRMWARE_UPDATE          (ZAF_FILE_ID_BASE + 6)
#define ZAF_FILE_ID_CC_CONFIGURATION            (ZAF_FILE_ID_BASE + 7)  //Obsolete ID nr. (Do not remove since legacy files may exist.)
#define ZAF_FILE_ID_KEY_FOB_NODES               (ZAF_FILE_ID_BASE + 8)
//Add file IDs for single files here.


//Base file ID number for CC_Configuration. 100 file ID numbers are allocated for future use.
#define ZAF_FILE_ID_CC_CONFIGURATION_BASE (ZAF_FILE_ID_BASE + 100)
#define ZAF_FILE_ID_CC_CONFIGURATION_LAST (ZAF_FILE_ID_BASE + 199)


#define ZAF_FILE_SIZE_APP_VERSION  (sizeof(uint32_t))

#endif /* ZAF_APPLICATIONUTILITIES_ZAF_FILE_IDS_H_ */
