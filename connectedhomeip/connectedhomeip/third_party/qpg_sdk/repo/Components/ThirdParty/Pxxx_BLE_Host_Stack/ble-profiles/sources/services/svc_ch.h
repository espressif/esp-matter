/*************************************************************************************************/
/*!
 *  \file
 *
 *  \brief  Characteristic constants.
 *
 *  Copyright (c) 2011-2020 Arm Ltd. All Rights Reserved.
 *
 *  Copyright (c) 2019-2021 Packetcraft, Inc.
 *  
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  
 *      http://www.apache.org/licenses/LICENSE-2.0
 *  
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
/*************************************************************************************************/

#ifndef SVC_CH_H
#define SVC_CH_H

#ifdef __cplusplus
extern "C" {
#endif

/*! \addtogroup SERVICE_CONSTANTS
 *  \{ */

/**************************************************************************************************
  Macros
**************************************************************************************************/

/** \name Appearance Values
 *
 */
/**@{*/
#define CH_APPEAR_UNKNOWN                     0       /*!< \brief Unknown */
#define CH_APPEAR_PHONE                       64      /*!< \brief Generic Phone */
#define CH_APPEAR_COMPUTER                    128     /*!< \brief Generic Computer */
#define CH_APPEAR_WATCH                       192     /*!< \brief Generic Watch */
#define CH_APPEAR_WATCH_SPORTS                193     /*!< \brief Watch: Sports Watch */
#define CH_APPEAR_CLOCK                       256     /*!< \brief Generic Clock */
#define CH_APPEAR_DISPLAY                     320     /*!< \brief Generic Display */
#define CH_APPEAR_REMOTE                      384     /*!< \brief Generic Remote Control */
#define CH_APPEAR_GLASSES                     448     /*!< \brief Generic Eye-glasses */
#define CH_APPEAR_TAG                         512     /*!< \brief Generic Tag */
#define CH_APPEAR_KEYRING                     576     /*!< \brief Generic Keyring */
#define CH_APPEAR_PLAYER                      640     /*!< \brief Generic Media Player */
#define CH_APPEAR_BARCODE                     704     /*!< \brief Generic Barcode Scanner */
#define CH_APPEAR_THERM                       768     /*!< \brief Generic Thermometer */
#define CH_APPEAR_THERM_EAR                   769     /*!< \brief Thermometer: Ear */
#define CH_APPEAR_HR                          832     /*!< \brief Generic Heart rate Sensor */
#define CH_APPEAR_HR_BELT                     833     /*!< \brief Heart Rate Sensor: Heart Rate Belt  */
#define CH_APPEAR_BP                          896     /*!< \brief Generic Blood Pressure */
#define CH_APPEAR_BP_ARM                      897     /*!< \brief Blood Pressure: Arm */
#define CH_APPEAR_BP_WRIST                    898     /*!< \brief Blood Pressure: Wrist */
#define CH_APPEAR_HID                         960     /*!< \brief Human Interface Device (HID) */
#define CH_APPEAR_HID_KEYBOARD                961     /*!< \brief Keyboard */
#define CH_APPEAR_HID_MOUSE                   962     /*!< \brief Mouse */
#define CH_APPEAR_HID_JOYSTICK                963     /*!< \brief Joystick */
#define CH_APPEAR_HID_GAMEPAD                 964     /*!< \brief Gamepad */
#define CH_APPEAR_HID_TABLET                  965     /*!< \brief Digitizer Tablet */
#define CH_APPEAR_HID_READER                  966     /*!< \brief Card Reader */
#define CH_APPEAR_HID_PEN                     967     /*!< \brief Digital Pen */
#define CH_APPEAR_HID_BARCODE                 968     /*!< \brief Barcode Scanner */
#define CH_APPEAR_GLUCOSE                     1024    /*!< \brief Generic Glucose Meter */
#define CH_APPEAR_RUN                         1088    /*!< \brief Generic: Running Walking Sensor */
#define CH_APPEAR_RUN_IN_SHOE                 1089    /*!< \brief Running Walking Sensor: In-Shoe */
#define CH_APPEAR_RUN_ON_SHOE                 1090    /*!< \brief Running Walking Sensor: On-Shoe */
#define CH_APPEAR_RUN_ON_HIP                  1091    /*!< \brief Running Walking Sensor: On-Hip */
#define CH_APPEAR_CYCL                        1152    /*!< \brief Generic: Cycling */
#define CH_APPEAR_CYCL_COMP                   1153    /*!< \brief Cycling: Cycling Computer */
#define CH_APPEAR_CYCL_SPEED                  1154    /*!< \brief Cycling: Speed Sensor */
#define CH_APPEAR_CYCL_CAD                    1155    /*!< \brief Cycling: Cadence Sensor */
#define CH_APPEAR_CYCL_POWER                  1156    /*!< \brief Cycling: Power Sensor */
#define CH_APPEAR_CYCL_SPEED_CAD              1157    /*!< \brief Cycling: Speed and Cadence Sensor */

#define CH_APPEAR_LEN                         2       /*!< \brief Length in bytes*/
/**@}*/

/** \name Alert Level Values
 *
 */
/**@{*/
#define CH_ALERT_LVL_NONE                     0       /*!< \brief No Alert */
#define CH_ALERT_LVL_MILD                     1       /*!< \brief Mild Alert */
#define CH_ALERT_LVL_HIGH                     2       /*!< \brief High Alert */

#define CH_ALERT_LVL_LEN                      1       /*!< \brief Length in bytes*/
/**@}*/

/** \name Heart Rate Measurement Flags
 *
 */
/**@{*/
#define CH_HRM_FLAGS_VALUE_8BIT               0x00    /*!< \brief Heart Rate Value Format is set to UINT8 */
#define CH_HRM_FLAGS_VALUE_16BIT              0x01    /*!< \brief Heart Rate Value Format is set to UINT16 */
#define CH_HRM_FLAGS_SENSOR_NOT_SUP           0x00    /*!< \brief Sensor Contact feature is not supported */
#define CH_HRM_FLAGS_SENSOR_NOT_DET           0x04    /*!< \brief Sensor Contact feature is supported, but contact is not detected */
#define CH_HRM_FLAGS_SENSOR_DET               0x06    /*!< \brief Sensor Contact feature is supported and contact is detected */
#define CH_HRM_FLAGS_ENERGY_EXP               0x08    /*!< \brief Energy Expended field is present */
#define CH_HRM_FLAGS_RR_INTERVAL              0x10    /*!< \brief One or more RR-Interval values are present */
/**@}*/

/** \name Heart Rate Measurement Field Lengths
 *
 */
/**@{*/
#define CH_HRM_LEN_VALUE_8BIT                 1       /*!< \brief Heart rate measurement 8-bit value length. */
#define CH_HRM_LEN_VALUE_16BIT                2       /*!< \brief Heart rate measurement 16-bit value length. */
#define CH_HRM_LEN_ENERGY_EXP                 2       /*!< \brief Heart rate measurement exponent value length. */
#define CH_HRM_LEN_RR_INTERVAL                2       /*!< \brief Heart rate measurement RR interval value length. */
/**@}*/

/** \name Body Sensor Locations
 *
 */
/**@{*/
#define CH_BSENSOR_LOC_OTHER                  0       /*!< \brief Other */
#define CH_BSENSOR_LOC_CHEST                  1       /*!< \brief Chest */
#define CH_BSENSOR_LOC_WRIST                  2       /*!< \brief Wrist */
#define CH_BSENSOR_LOC_FINGER                 3       /*!< \brief Finger */
#define CH_BSENSOR_LOC_HAND                   4       /*!< \brief Hand */
#define CH_BSENSOR_LOC_EARLOBE                5       /*!< \brief Ear Lobe */
#define CH_BSENSOR_LOC_FOOT                   6       /*!< \brief Foot */

#define CH_BSENSOR_LOC_LEN                    1       /*!< \brief Length in bytes */
/**@}*/

/** \name Heart Rate Control Point
 *
 */
/**@{*/
#define CH_HRCP_RESET_ENERGY_EXP              1       /*!< \brief Reset Energy Expended */

#define CH_HRCP_LEN                           1       /*!< \brief Length of the control point value */
/**@}*/

/** \name Alert Notification Control Point
 *
 */
/**@{*/
#define CH_ANCP_ENABLE_NEW                    0       /*!< \brief Enable New Incoming Alert Notification */
#define CH_ANCP_ENABLE_UNREAD                 1       /*!< \brief Enable Unread Category Status Notification */
#define CH_ANCP_DISABLE_NEW                   2       /*!< \brief Disable New Incoming Alert Notification */
#define CH_ANCP_DISABLE_UNREAD                3       /*!< \brief Disable Unread Category Status Notification */
#define CH_ANCP_NOTIFY_NEW                    4       /*!< \brief Notify New Incoming Alert immediately */
#define CH_ANCP_NOTIFY_UNREAD                 5       /*!< \brief Notify Unread Category Status immediately */
/**@}*/

/** \name Alert Category ID
 *
 */
/**@{*/
#define CH_ALERT_CAT_ID_SIMPLE                0       /*!< \brief Simple Alert: General text alert or non-text alert */
#define CH_ALERT_CAT_ID_EMAIL                 1       /*!< \brief Email: Alert when Email messages arrives */
#define CH_ALERT_CAT_ID_NEWS                  2       /*!< \brief News: News feeds such as RSS, Atom */
#define CH_ALERT_CAT_ID_CALL                  3       /*!< \brief Call: Incoming call */
#define CH_ALERT_CAT_ID_MISSED                4       /*!< \brief Missed call: Missed Call */
#define CH_ALERT_CAT_ID_SMS                   5       /*!< \brief SMS/MMS: SMS/MMS message arrives */
#define CH_ALERT_CAT_ID_VMAIL                 6       /*!< \brief Voice mail: Voice mail */
#define CH_ALERT_CAT_ID_SCHED                 7       /*!< \brief Schedule: Alert occurred on calendar, planner */
#define CH_ALERT_CAT_ID_ALERT                 8       /*!< \brief High Prioritized Alert: Alert that should be handled as high priority */
#define CH_ALERT_CAT_ID_IM                    9       /*!< \brief Instant Message: Alert for incoming instant messages */
#define CH_ALERT_CAT_ID_ALL                   0xFF    /*!< \brief All Categories */
/**@}*/

/** \name Alert Category ID Bit Mask
 *
 */
/**@{*/
#define CH_ALERT_CAT_MASK_SIMPLE              0x0001  /*!< \brief Simple Alert: General text alert or non-text alert */
#define CH_ALERT_CAT_MASK_EMAIL               0x0002  /*!< \brief Email: Alert when Email messages arrives */
#define CH_ALERT_CAT_MASK_NEWS                0x0004  /*!< \brief News: News feeds such as RSS, Atom */
#define CH_ALERT_CAT_MASK_CALL                0x0008  /*!< \brief Call: Incoming call */
#define CH_ALERT_CAT_MASK_MISSED              0x0010  /*!< \brief Missed call: Missed Call */
#define CH_ALERT_CAT_MASK_SMS                 0x0020  /*!< \brief SMS/MMS: SMS/MMS message arrives */
#define CH_ALERT_CAT_MASK_VMAIL               0x0040  /*!< \brief Voice mail: Voice mail */
#define CH_ALERT_CAT_MASK_SCHED               0x0080  /*!< \brief Schedule: Alert occurred on calendar, planner */
#define CH_ALERT_CAT_MASK_ALERT               0x0100  /*!< \brief High Prioritized Alert: Alert that should be handled as high priority */
#define CH_ALERT_CAT_MASK_IM                  0x0200  /*!< \brief Instant Message: Alert for incoming instant messages */
/**@}*/

/** \name Ringer Control Point
 *
 */
/**@{*/
#define CH_RCP_SILENT                         1       /*!< \brief Silent Mode */
#define CH_RCP_MUTE_ONCE                      2       /*!< \brief Mute Once */
#define CH_RCP_CANCEL_SILENT                  3       /*!< \brief Cancel Silent Mode */
/**@}*/

/** \name Ringer Setting
 *
 */
/**@{*/
#define CH_RINGER_SET_SILENT                  0       /*!< \brief Ringer Silent */
#define CH_RINGER_SET_NORMAL                  1       /*!< \brief Ringer Normal */
/**@}*/

/** \name Alert Status
 *
 */
/**@{*/
#define CH_ALERT_STATUS_RINGER                0x01    /*!< \brief Ringer State active */
#define CH_ALERT_STATUS_VIBRATE               0x02    /*!< \brief Vibrate State active */
#define CH_ALERT_STATUS_DISPLAY               0x04    /*!< \brief Display Alert Status State active */
/**@}*/

/*! \brief System ID */
#define CH_SYSTEM_ID_LEN                      8       /*!< \brief System ID value length. */

/*! \brief PnP ID */
#define CH_PNP_ID_LEN                         7       /*!< \brief PnP ID value length. */

/*! \brief Battery Level */
#define CH_BATT_LEVEL_LEN                     1       /*!< \brief Battery level value length. */

/** \name Blood Pressure Measurement Flags
 *
 */
/**@{*/
#define CH_BPM_FLAG_UNITS_MMHG                0x00    /*!< \brief Blood pressure in units of mmHg */
#define CH_BPM_FLAG_UNITS_KPA                 0x01    /*!< \brief Blood pressure in units of kPa */
#define CH_BPM_FLAG_TIMESTAMP                 0x02    /*!< \brief Time Stamp Flag */
#define CH_BPM_FLAG_PULSE_RATE                0x04    /*!< \brief Pulse Rate Flag */
#define CH_BPM_FLAG_USER_ID                   0x08    /*!< \brief User ID Flag */
#define CH_BPM_FLAG_MEAS_STATUS               0x10    /*!< \brief Measurement Status Flag */
/**@}*/

/** \name Blood Pressure Measurement: Measurement Status Bitmasks
 *
 */
/**@{*/
/*! \brief Blood Pressure Measurement measurement status bitmasks */
#define CH_BPM_MS_BIT_MOVEMENT                0x0001  /*!< \brief Body Movement Detection */
#define CH_BPM_MS_BIT_CUFF_FIT                0x0002  /*!< \brief Cuff Fit Detection */
#define CH_BPM_MS_BIT_IRR_PULSE               0x0004  /*!< \brief Irregular Pulse Detection */
#define CH_BPM_MS_BIT_PULSE_RANGE             0x0018  /*!< \brief Pulse Rate Range Detection */
#define CH_BPM_MS_BIT_MEAS_POS                0x0020  /*!< \brief Measurement Position Detection */
/**@}*/

/** \name Blood Pressure Measurement: measurement status flags
 *
 */
/**@{*/
#define CH_BPM_MS_FLAG_MOVEMENT_NONE          0x0000  /*!< \brief No body movement */
#define CH_BPM_MS_FLAG_MOVEMENT               0x0001  /*!< \brief Body movement */
#define CH_BPM_MS_FLAG_CUFF_FIT_OK            0x0000  /*!< \brief Cuff fit ok */
#define CH_BPM_MS_FLAG_CUFF_FIT_LOOSE         0x0002  /*!< \brief Cuff fit loose */
#define CH_BPM_MS_FLAG_IRR_PULSE_NONE         0x0000  /*!< \brief No irregular pulse detected */
#define CH_BPM_MS_FLAG_IRR_PULSE              0x0004  /*!< \brief Irregular pulse detected */
#define CH_BPM_MS_FLAG_PULSE_RANGE_OK         0x0000  /*!< \brief Pulse rate is within the range */
#define CH_BPM_MS_FLAG_PULSE_RANGE_HIGH       0x0008  /*!< \brief Pulse rate exceeds upper limit */
#define CH_BPM_MS_FLAG_PULSE_RANGE_LOW        0x0010  /*!< \brief Pulse rate is less than lower limit */
#define CH_BPM_MS_FLAG_MEAS_POS               0x0000  /*!< \brief Proper measurement position */
#define CH_BPM_MS_FLAG_MEAS_POS_ERR           0x0020  /*!< \brief Improper measurement position */
/**@}*/

/** \name Blood Pressure Measurement Field Lengths
 *
 */
/**@{*/
#define CH_BPM_FLAGS_LEN                      1       /*!< \brief Blood pressure measurement flags length. */
#define CH_BPM_MEAS_LEN                       6       /*!< \brief Blood pressure measurement measurement length. */
#define CH_BPM_TIMESTAMP_LEN                  7       /*!< \brief Blood pressure measurement timestamp length. */
#define CH_BPM_PULSE_RATE_LEN                 2       /*!< \brief Blood pressure measurement pulse rate length. */
#define CH_BPM_USER_ID_LEN                    1       /*!< \brief Blood pressure measurement user ID length. */
#define CH_BPM_MEAS_STATUS_LEN                2       /*!< \brief Blood pressure measurement status length. */
/**@}*/

/** \name Blood Pressure Feature Flags
 *
 */
/**@{*/
#define CH_BPF_FLAG_MOVEMENT                  0x0001  /*!< \brief Body Movement Detection Support bit */
#define CH_BPF_FLAG_CUFF_FIT                  0x0002  /*!< \brief Cuff Fit Detection Support bit */
#define CH_BPF_FLAG_IRR_PULSE                 0x0004  /*!< \brief Irregular Pulse Detection Support bit */
#define CH_BPF_FLAG_PULSE_RANGE               0x0008  /*!< \brief Pulse Rate Range Detection Support bit */
#define CH_BPF_FLAG_MEAS_POS                  0x0010  /*!< \brief Measurement Position Detection Support bit */
#define CH_BPF_FLAG_MULTI_BOND                0x0020  /*!< \brief Multiple bond support bit */
/**@}*/

/** \name SFLOAT Special Values
 *
 */
/**@{*/
#define CH_SFLOAT_NAN                         0x07FF  /*!< \brief Not a number */
#define CH_SFLOAT_NRES                        0x07FF  /*!< \brief Not at this resolution */
#define CH_SFLOAT_POS_INF                     0x07FE  /*!< \brief Positive infinity */
#define CH_SFLOAT_NEG_INF                     0x0802  /*!< \brief Negative infinity */
#define CH_SFLOAT_RSVD                        0x0801  /*!< \brief Not at this resolution */
/**@}*/

/** \name Glucose Feature
 *
 */
/**@{*/
#define CH_GLF_LOW_BATT                       0x0001  /*!< \brief Low Battery Detection During Measurement Supported */
#define CH_GLF_MALFUNC                        0x0002  /*!< \brief Sensor Malfunction Detection Supported */
#define CH_GLF_SAMPLE_SIZE                    0x0004  /*!< \brief Sensor Sample Size Supported */
#define CH_GLF_INSERT_ERR                     0x0008  /*!< \brief Sensor Strip Insertion Error Detection Supported */
#define CH_GLF_TYPE_ERR                       0x0010  /*!< \brief Sensor Strip Type Error Detection Supported */
#define CH_GLF_RES_HIGH_LOW                   0x0020  /*!< \brief Sensor Result High-Low Detection Supported */
#define CH_GLF_TEMP_HIGH_LOW                  0x0040  /*!< \brief Sensor Temperature High-Low Detection Supported */
#define CH_GLF_READ_INT                       0x0080  /*!< \brief Sensor Read Interrupt Detection Supported */
#define CH_GLF_GENERAL_FAULT                  0x0100  /*!< \brief General Device Fault Supported */
#define CH_GLF_TIME_FAULT                     0x0200  /*!< \brief Time Fault Supported */
#define CH_GLF_MULTI_BOND                     0x0400  /*!< \brief Multiple Bond Supported */
/**@}*/

/** \name Glucose Measurement Flags
 *
 */
/**@{*/
#define CH_GLM_FLAG_TIME_OFFSET               0x01    /*!< \brief Time Offset Present */
#define CH_GLM_FLAG_CONC_TYPE_LOC             0x02    /*!< \brief Glucose Concentration, Type, and Sample Location Present */
#define CH_GLM_FLAG_UNITS_KG_L                0x00    /*!< \brief Glucose Concentration Units kg/L */
#define CH_GLM_FLAG_UNITS_MOL_L               0x04    /*!< \brief Glucose Concentration Units mol/L */
#define CH_GLM_FLAG_SENSOR_STATUS             0x08    /*!< \brief Sensor Status Annunciation Present */
#define CH_GLM_FLAG_CONTEXT_INFO              0x10    /*!< \brief Context Information Follows */
/**@}*/

/** \name Glucose Measurement Type
 *
 */
/**@{*/
#define CH_GLM_TYPE_CAP_BLOOD                 1       /*!< \brief Capillary whole blood */
#define CH_GLM_TYPE_CAP_PLASMA                2       /*!< \brief Capillary plasma */
#define CH_GLM_TYPE_VEN_BLOOD                 3       /*!< \brief Venous whole blood */
#define CH_GLM_TYPE_VEN_PLASMA                4       /*!< \brief Venous plasma */
#define CH_GLM_TYPE_ART_BLOOD                 5       /*!< \brief Arterial whole blood */
#define CH_GLM_TYPE_ART_PLASMA                6       /*!< \brief Arterial plasma */
#define CH_GLM_TYPE_UNDET_BLOOD               7       /*!< \brief Undetermined whole blood */
#define CH_GLM_TYPE_UNDET_PLASMA              8       /*!< \brief Undetermined plasma */
#define CH_GLM_TYPE_FLUID                     9       /*!< \brief Interstitial fluid (ISF) */
#define CH_GLM_TYPE_CONTROL                   10      /*!< \brief Control solution */
/**@}*/

/** \name Glucose Measurement Location
 *
 */
/**@{*/
#define CH_GLM_LOC_FINGER                     1     /*!< \brief Finger */
#define CH_GLM_LOC_AST                        2     /*!< \brief Alternate Site Test (AST) */
#define CH_GLM_LOC_EAR                        3     /*!< \brief Earlobe */
#define CH_GLM_LOC_CONTROL                    4     /*!< \brief Control solution */
#define CH_GLM_LOC_NOT_AVAIL                  15    /*!< \brief Sample Location value not available */
/**@}*/

/** \name Glucose Sensor Status Annunciation
 *
 */
/**@{*/
#define CH_GLM_STATUS_BATT_LOW                0x0001  /*!< \brief Device battery low at time of measurement */
#define CH_GLM_STATUS_SENSOR_FAULT            0x0002  /*!< \brief Sensor malfunction or faulting at time of measurement */
#define CH_GLM_STATUS_SAMPLE_SIZE             0x0004  /*!< \brief Sample size for blood or control solution insufficient at time of measurement */
#define CH_GLM_STATUS_STRIP_INSERT            0x0008  /*!< \brief Strip insertion error */
#define CH_GLM_STATUS_STRIP_TYPE              0x0010  /*!< \brief Strip type incorrect for device */
#define CH_GLM_STATUS_RESULT_HIGH             0x0020  /*!< \brief Sensor result higher than the device can process */
#define CH_GLM_STATUS_RESULT_LOW              0x0040  /*!< \brief Sensor result lower than the device can process */
#define CH_GLM_STATUS_TEMP_HIGH               0x0080  /*!< \brief Sensor temperature too high for valid test/result at time of measurement */
#define CH_GLM_STATUS_TEMP_LOW                0x0100  /*!< \brief Sensor temperature too low for valid test/result at time of measurement */
#define CH_GLM_STATUS_STRIP_PULL              0x0200  /*!< \brief Sensor read interrupted because strip was pulled too soon at time of measurement */
#define CH_GLM_STATUS_GENERAL_FAULT           0x0400  /*!< \brief General device fault has occurred in the sensor */
#define CH_GLM_STATUS_TIME_FAULT              0x0800  /*!< \brief Time fault has occurred in the sensor and time may be inaccurate */
/**@}*/

/** \name Glucose Measurement Field Lengths
 *
 */
/**@{*/
#define CH_GLM_FLAGS_LEN                      1       /*!< \brief Glucose measurement flags length. */
#define CH_GLM_SEQNUM_LEN                     2       /*!< \brief Glucose measurement sequence number length. */
#define CH_GLM_TIMESTAMP_LEN                  7       /*!< \brief Glucose measurement timestamp length. */
#define CH_GLM_TIME_OFFSET_LEN                2       /*!< \brief Glucose measurement time offset length. */
#define CH_GLM_CONC_TYPE_LOC_LEN              3       /*!< \brief Glucose measurement concentration type location length. */
#define CH_GLM_SENSOR_STATUS_LEN              2       /*!< \brief Glucose measurement sensor status length. */
/**@}*/

/** \name Glucose Measurement Context Flags
 *
 */
/**@{*/
#define CH_GLMC_FLAG_CARB                     0x01    /*!< \brief Carbohydrate id and carbohydrate present */
#define CH_GLMC_FLAG_MEAL                     0x02    /*!< \brief Meal present */
#define CH_GLMC_FLAG_TESTER                   0x04    /*!< \brief Tester-health present */
#define CH_GLMC_FLAG_EXERCISE                 0x08    /*!< \brief Exercise duration and exercise intensity present */
#define CH_GLMC_FLAG_MED                      0x10    /*!< \brief Medication ID and medication present */
#define CH_GLMC_FLAG_MED_KG                   0x00    /*!< \brief Medication value units, kilograms */
#define CH_GLMC_FLAG_MED_L                    0x20    /*!< \brief Medication value units, liters */
#define CH_GLMC_FLAG_HBA1C                    0x40    /*!< \brief Hba1c present */
#define CH_GLMC_FLAG_EXT                      0x80    /*!< \brief Extended flags present */
/**@}*/

/** \name Glucose Measurement Context Field Lenths
 *
 */
/**@{*/
#define CH_GLMC_FLAGS_LEN                     1       /*!< \brief Glucose measurement context flags length. */
#define CH_GLMC_SEQNUM_LEN                    2       /*!< \brief Glucose measurement context sequence number length. */
#define CH_GLMC_CARB_LEN                      3       /*!< \brief Glucose measurement context carbohydrate ID a d carbohydrate length. */
#define CH_GLMC_MEAL_LEN                      1       /*!< \brief Glucose measurement context meal present length. */
#define CH_GLMC_TESTER_LEN                    1       /*!< \brief Glucose measurement context tester-health length. */
#define CH_GLMC_EXERCISE_LEN                  3       /*!< \brief Glucose measurement context exercise duration and exercise intensity length. */
#define CH_GLMC_MED_LEN                       3       /*!< \brief Glucose measurement context medication ID length. */
#define CH_GLMC_HBA1C_LEN                     2       /*!< \brief Glucose measurement context Hba1c length. */
#define CH_GLMC_EXT_LEN                       1       /*!< \brief Glucose measurement context extended length. */
/**@}*/

/** \name Glucose measurement context carbohydrate ID
 *
 */
/**@{*/
#define CH_GLMC_CARB_BREAKFAST                1       /*!< \brief Breakfast */
#define CH_GLMC_CARB_LUNCH                    2       /*!< \brief Lunch */
#define CH_GLMC_CARB_DINNER                   3       /*!< \brief Dinner */
#define CH_GLMC_CARB_SNACK                    4       /*!< \brief Snack */
#define CH_GLMC_CARB_DRINK                    5       /*!< \brief Drink */
#define CH_GLMC_CARB_SUPPER                   6       /*!< \brief Supper */
#define CH_GLMC_CARB_BRUNCH                   7       /*!< \brief Brunch */
/**@}*/

/** \name Glucose Measurement Context Meal
 *
 */
/**@{*/
#define CH_GLMC_MEAL_PREPRANDIAL              1       /*!< \brief Preprandial (before meal) */
#define CH_GLMC_MEAL_POSTPRANDIAL             2       /*!< \brief Postprandial (after meal) */
#define CH_GLMC_MEAL_FASTING                  3       /*!< \brief Fasting */
#define CH_GLMC_MEAL_CASUAL                   4       /*!< \brief Casual (snacks, drinks, etc.) */
#define CH_GLMC_MEAL_BEDTIME                  5       /*!< \brief Bedtime */
/**@}*/

/** \name Glucose Measurement Context Tester
 *
 */
/**@{*/
#define CH_GLMC_TESTER_SELF                   1       /*!< \brief Self */
#define CH_GLMC_TESTER_PRO                    2       /*!< \brief Health care professional */
#define CH_GLMC_TESTER_LAB                    3       /*!< \brief Lab test */
#define CH_GLMC_TESTER_NOT_AVAIL              15      /*!< \brief Tester value not available */
/**@}*/

/** \name Glucose measurement Context Health
 *
 */
/**@{*/
#define CH_GLMC_HEALTH_MINOR                  1       /*!< \brief Minor health issues */
#define CH_GLMC_HEALTH_MAJOR                  2       /*!< \brief Major health issues */
#define CH_GLMC_HEALTH_MENSES                 3       /*!< \brief During menses */
#define CH_GLMC_HEALTH_STRESS                 4       /*!< \brief Under stress */
#define CH_GLMC_HEALTH_NONE                   5       /*!< \brief No health issues */
#define CH_GLMC_HEALTH_NOT_AVAIL              15      /*!< \brief Health value not available */
/**@}*/

/** \name Glucose Measurement Context Medication ID
 *
 */
/**@{*/
#define CH_GLMC_MED_RAPID                     1       /*!< \brief Rapid acting insulin */
#define CH_GLMC_MED_SHORT                     2       /*!< \brief Short acting insulin */
#define CH_GLMC_MED_INTERMED                  3       /*!< \brief Intermediate acting insulin */
#define CH_GLMC_MED_LONG                      4       /*!< \brief Long acting insulin */
#define CH_GLMC_MED_PREMIX                    5       /*!< \brief Pre-mixed insulin */
/**@}*/

/** \name Record Access Control Point Opcode
 *
 */
/**@{*/
#define CH_RACP_OPCODE_REPORT                 1       /*!< \brief Report stored records */
#define CH_RACP_OPCODE_DELETE                 2       /*!< \brief Delete stored records */
#define CH_RACP_OPCODE_ABORT                  3       /*!< \brief Abort operation */
#define CH_RACP_OPCODE_REPORT_NUM             4       /*!< \brief Report number of stored records */
#define CH_RACP_OPCODE_NUM_RSP                5       /*!< \brief Number of stored records response */
#define CH_RACP_OPCODE_RSP                    6       /*!< \brief Response code */

/** \name Record Access Control Point Operator
 *
 */
/**@{*/
#define CH_RACP_OPERATOR_NULL                 0       /*!< \brief Null operator */
#define CH_RACP_OPERATOR_ALL                  1       /*!< \brief All records */
#define CH_RACP_OPERATOR_LTEQ                 2       /*!< \brief Less than or equal to */
#define CH_RACP_OPERATOR_GTEQ                 3       /*!< \brief Greater than or equal to */
#define CH_RACP_OPERATOR_RANGE                4       /*!< \brief Within range of (inclusive) */
#define CH_RACP_OPERATOR_FIRST                5       /*!< \brief First record(i.e. oldest record) */
#define CH_RACP_OPERATOR_LAST                 6       /*!< \brief Last record (i.e. most recent record) */
/**@}*/

/** \name Record Access Control Point Response Code Values
 *
 */
/**@{*/
#define CH_RACP_RSP_SUCCESS                   1       /*!< \brief Success */
#define CH_RACP_RSP_OPCODE_NOT_SUP            2       /*!< \brief Op code not supported */
#define CH_RACP_RSP_INV_OPERATOR              3       /*!< \brief Invalid operator */
#define CH_RACP_RSP_OPERATOR_NOT_SUP          4       /*!< \brief Operator not supported */
#define CH_RACP_RSP_INV_OPERAND               5       /*!< \brief Invalid operand */
#define CH_RACP_RSP_NO_RECORDS                6       /*!< \brief No records found */
#define CH_RACP_RSP_ABORT_FAILED              7       /*!< \brief Abort unsuccessful */
#define CH_RACP_RSP_PROC_NOT_COMP             8       /*!< \brief Procedure not completed */
#define CH_RACP_RSP_OPERAND_NOT_SUP           9       /*!< \brief Operand not supported */
/**@}*/

/** \name Glucose Service Operand Filter Types And Field Lengths
 *
 */
/**@{*/
#define CH_RACP_GLS_FILTER_SEQ                1       /*!< \brief Sequence number */
#define CH_RACP_GLS_FILTER_TIME               2       /*!< \brief User facing time */
#define CH_RACP_GLS_FILTER_SEQ_LEN            2       /*!< \brief Sequence number filter length */
#define CH_RACP_GLS_FILTER_TIME_LEN           7       /*!< \brief User facing time filter length */
/**@}*/

/** \name Service Changed Length
 *
 */
/**@{*/
#define CH_SC_LEN                             4       /*!< \brief Length of service changed */
/**@}*/

/** \name Temperature Measurement flags
 *
 */
/**@{*/
#define CH_TM_FLAG_UNITS_C                    0x00    /*!< \brief Temperature in units of C */
#define CH_TM_FLAG_UNITS_F                    0x01    /*!< \brief Temperature in units of F */
#define CH_TM_FLAG_TIMESTAMP                  0x02    /*!< \brief Time Stamp Flag */
#define CH_TM_FLAG_TEMP_TYPE                  0x04    /*!< \brief Temperature Type Flag */
/**@}*/

/** \name Temperature Measurement Field Lengths
 *
 */
/**@{*/
#define CH_TM_FLAGS_LEN                       1       /*!< \brief Temperature measurement flags length. */
#define CH_TM_MEAS_LEN                        4       /*!< \brief Temperature measurement measurement length. */
#define CH_TM_TIMESTAMP_LEN                   7       /*!< \brief Temperature measurement timestamp length. */
#define CH_TM_TEMP_TYPE_LEN                   1       /*!< \brief Temperature measurement type length. */
/**@}*/

/** \name Temperature Type
 *
 */
/**@{*/
#define CH_TT_ARMPIT                          1       /*!< \brief Armpit */
#define CH_TT_BODY                            2       /*!< \brief Body (general) */
#define CH_TT_EAR                             3       /*!< \brief Ear (usually ear lobe) */
#define CH_TT_FINGER                          4       /*!< \brief Finger */
#define CH_TT_GI                              5       /*!< \brief Gastro-intestinal Tract */
#define CH_TT_MOUTH                           6       /*!< \brief Mouth */
#define CH_TT_RECTUM                          7       /*!< \brief Rectum */
#define CH_TT_TOE                             8       /*!< \brief Toe */
#define CH_TT_TYMPANUM                        9       /*!< \brief Tympanum (ear drum) */
/**@}*/

/** \name Weight Scale Feature Flags
 *
 */
/**@{*/
#define CH_WSF_FLAG_TIMESTAMP                 0x0001  /*!< \brief Time Stamp Supported bit */
#define CH_WSF_FLAG_MULTIUSER                 0x0002  /*!< \brief Multiple Users Supported bit */
#define CH_WSF_FLAG_BMI                       0x0004  /*!< \brief BMI Supported bit */
/**@}*/

/** \name Weight Scale Measurement Flags
 *
 */
/**@{*/
#define CH_WSM_FLAG_UNITS_KG                  0x00    /*!< \brief Weight in units of kilograms */
#define CH_WSM_FLAG_UNITS_LBS                 0x01    /*!< \brief Weight in units of pounds */
#define CH_WSM_FLAG_TIMESTAMP                 0x02    /*!< \brief Time stamp present */
#define CH_WSM_FLAG_USER_ID                   0x04    /*!< \brief User ID present */
#define CH_WSM_FLAG_BMI_HEIGHT                0x08    /*!< \brief BMI and height present */
/**@}*/

/** \name Weight Scale Measurement Field Lengths
 *
 */
/**@{*/
#define CH_WSM_FLAGS_LEN                      1       /*!< \brief Weight scale measurement flags length. */
#define CH_WSM_MEAS_LEN                       2       /*!< \brief Weight scale measurement measurement length. */
#define CH_WSM_TIMESTAMP_LEN                  7       /*!< \brief Weight scale measurement timestamp length. */
#define CH_WSM_USER_ID_LEN                    1       /*!< \brief Weight scale measurement user ID length. */
#define CH_WSM_BMI_HEIGHT_LEN                 4       /*!< \brief Weight scale measurement BMI-height length. */
/**@}*/

/** \name Scan Parameter Profile Field Lengths
 *
 */
/**@{*/
#define CH_SCPP_INTERVAL_WINDOW_LEN           4       /*!< \brief Scan parameter profile interval window length. */
#define CH_SCPP_REFRESH_LEN                   0       /*!< \brief Scan parameter profile refresh length. */
/**@}*/

/** \name Cycle Power Profile field Lengths
 *
 */
/**@{*/
#define CH_CPS_MEASUREMENT_LEN                34      /*!< \brief Cycling power profile measurement length. */
/**@}*/

/** \name Cycle Speed Profile Field Lengths
 *
 */
/**@{*/
#define CH_CSCS_MEASUREMENT_LEN               11      /*!< \brief Cycling speed profile measurement length. */
/**@}*/

/** \name Running Speed Profile Field Lengths
 *
 */
/**@{*/
#define CH_RSCS_MEASUREMENT_LEN               10      /*!< \brief Running speed profile measurement length. */
/**@}*/

/** \name Pulse Oximiter Profile Field Lengths
 *
 */
/**@{*/
#define CH_PLXS_SPOT_CHECK_LEN                19      /*!< \brief Pulse oximiter profile spot check length. */
#define CH_PLXS_CONTINUOUS_LEN                16      /*!< \brief Pulse oximiter profile continuous length. */
/**@}*/

/** \name Pulse Oximeter Common Field Lenths
 *
 */
/**@{*/
#define CH_PLX_FLAGS_LEN                      1       /*!< \brief Pulse oximiter flags length. */
#define CH_PLX_SPO2_LEN                       2       /*!< \brief Pulse oximiter SPO2 length, */
#define CH_PLX_PULSE_LEN                      2       /*!< \brief Pulse oximiter pulse length. */
#define CH_PLX_MEASUREMENT_STATUS_LEN         2       /*!< \brief Pulse oximiter measurement status length. */
#define CH_PLX_SENSOR_STATUS_LEN              3       /*!< \brief Pulse oximiter sensor status length. */
#define CH_PLX_PULSE_AMP_INDX_LEN             2       /*!< \brief Pulse AMP index length. */
/**@}*/

/** \name Pulse Oximeter Spot Check Measurement Field Lenths
 *
 */
/**@{*/
#define CH_PLXSC_TIMESTAMP_LEN                7       /*!< \brief Pulse oximiter spot check measurement timestamp length. */
/**@}*/

/** \name Pulse Oximeter Continuous Measurement Field Lenths
 *
 */
/**@{*/
#define CH_PLXC_SPO2PR_FAST_LEN               2       /*!< \brief Pulse oximiter continuous measurement SPO2PR fast length. */
#define CH_PLXC_SPO2PR_SLOW_LEN               2       /*!< \brief Pulse oximiter continuous measurement SPO2PR slow length. */
/**@}*/

/** \name  Pulse Oximeter Features Field Lenths
 *
 */
/**@{*/
#define CH_PLXF_MIN_FEATURES_LEN              2       /*!< \brief Pulse oximiter minimum features length. */
#define CH_PLXF_MAX_FEATURES_LEN              7       /*!< \brief Pulse oximiter maximum features length. */
#define CH_PLXF_SENSOR_SUPPORT_LEN            2       /*!< \brief Pulse oximiter sensor support length. */
#define CH_PLXF_MEASUREMENT_SUPPORT_LEN       2       /*!< \brief Pulse oximeter measurement suppported length. */
/**@}*/

/** \name Pulse Oximeter Spot Check Measurement Flags
 *
 */
/**@{*/
#define CH_PLXSC_FLAG_TIMESTAMP               0x01    /*!< \brief Timestamp field is present */
#define CH_PLXSC_FLAG_MEASUREMENT_STATUS      0x02    /*!< \brief Measurement Status Field Present */
#define CH_PLXSC_FLAG_SENSOR_STATUS           0x04    /*!< \brief Device and Sensor Status Field Present */
#define CH_PLXSC_FLAG_PULSE_AMP_INDX          0x08    /*!< \brief Pulse Amplitude Index field is present */
#define CH_PLXSC_FLAG_CLOCK_NOT_SET           0x10    /*!< \brief Device Clock is Not Set */
/**@}*/

/** \name Pulse Oximeter Continuous Measurement Flags
 *
 */
/**@{*/
#define CH_PLXC_FLAG_SPO2PR_FAST              0x01    /*!< \brief SpO2PR-Fast field is present */
#define CH_PLXC_FLAG_SPO2PR_SLOW              0x02    /*!< \brief SpO2PR-Slow field is present */
#define CH_PLXC_FLAG_MEASUREMENT_STATUS       0x04    /*!< \brief Measurement Status Field Present */
#define CH_PLXC_FLAG_SENSOR_STATUS            0x08    /*!< \brief Device and Sensor Status Field Present */
#define CH_PLXC_FLAG_PULSE_AMP_INDX           0x10    /*!< \brief Pulse Amplitude Index field is present */
/**@}*/

/** \name Pulse Oximeter Features
 *
 */
/**@{*/
#define CH_PLF_FLAG_MEAS_STATUS_SUP           0x01    /*!< \brief Measurement Status support is present */
#define CH_PLF_FLAG_SENSOR_STATUS_SUP         0x02    /*!< \brief Device and Sensor Status support is present */
#define CH_PLF_FLAG_SPOT_CHECK_STORAGE_SUP    0x04    /*!< \brief Measurement Storage for Spot-check measurements is supported */
#define CH_PLF_FLAG_SPOT_CHECK_SUP            0x08    /*!< \brief Timestamp for Spot-check measurements is supported */
#define CH_PLF_FLAG_SPO2PR_FAST_SUP           0x10    /*!< \brief SpO2PR-Fast metric is supported */
#define CH_PLF_FLAG_SPO2PR_SLOW_SUP           0x20    /*!< \brief SpO2PR-Slow metric is supported */
#define CH_PLF_FLAG_PULSE_AMP_SUP             0x40    /*!< \brief Pulse Amplitude Index field is supported */
#define CH_PLF_FLAG_MULTI_BOND_SUP            0x80    /*!< \brief Multiple Bonds Supported */
/**@}*/

/*! \} */    /* SERVICE_CONSTANTS */

#ifdef __cplusplus
};
#endif

#endif /* SVC_CH_H */

