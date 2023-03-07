/***************************************************************************//**
 * @file
 * @brief Routines for interacting with the prepayment-server.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "app/framework/include/af.h"
#include "app/framework/util/af-main.h"
#include "app/framework/util/util.h"
#include "prepayment-server.h"
#include "prepayment-modes-table.h"
#include "prepayment-debt-log.h"
#include "prepayment-debt-schedule.h"
#include "prepayment-tick.h"
#include "app/framework/plugin/prepayment-snapshot-storage/prepayment-snapshot-storage.h"

#ifdef UC_BUILD
#include "prepayment-server-config.h"
#endif // UC_BUILD

typedef uint16_t PaymentControlConfiguration;
typedef uint32_t PrepaySnapshotPayloadCause;
typedef uint8_t  PrepaySnapshotPayloadType;
typedef uint8_t  FriendlyCredit;

void emAfPrepaymentPrintAfTime(EmberAfTimeStruct *pafTime)
{
  emberAfPrepaymentClusterPrintln("== AF TIME ==");
  emberAfPrepaymentClusterPrintln("  Year=%d", pafTime->year);
  emberAfPrepaymentClusterPrintln("  Month=%d", pafTime->month);
  emberAfPrepaymentClusterPrintln("  Day=%d", pafTime->day);
  emberAfPrepaymentClusterPrintln("  Hour=%d", pafTime->hours);
  emberAfPrepaymentClusterPrintln("  Min=%d", pafTime->minutes);
  emberAfPrepaymentClusterPrintln("  Sec=%d", pafTime->seconds);
}

#define SECONDS_PER_DAY (3600 * 24)
extern emDebtScheduleEntry DebtSchedule[];

#ifdef UC_BUILD

#include "sl_cli.h"

void emAfPrepaymentServerCliInit(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint;
  endpoint = sl_cli_get_argument_uint8(arguments, 0);
  emberAfPrepaymentClusterServerInitCallback(endpoint);
}

// plugin prepayment-server writeAttribute <endpoint:1> <attributeId:2> <attributeType:1> <numBytes:1> <value:4>
void emAfPrepaymentServerCliWriteAttribute(sl_cli_command_arg_t *arguments)
{
  uint16_t attributeId;
  uint8_t  status;
  uint8_t  endpoint;
  uint8_t  numBytes;
  uint8_t  attributeType;
  uint32_t value;

  endpoint      = sl_cli_get_argument_uint8(arguments, 0);
  attributeId   = sl_cli_get_argument_uint16(arguments, 1);
  attributeType = sl_cli_get_argument_uint8(arguments, 2);
  numBytes      = sl_cli_get_argument_uint8(arguments, 3);
  value         = sl_cli_get_argument_uint32(arguments, 4);

  status = emberAfWriteAttribute(endpoint, ZCL_PREPAYMENT_CLUSTER_ID, attributeId,
                                 CLUSTER_MASK_SERVER, (uint8_t *)&value, attributeType);
  emberAfPrepaymentClusterPrintln("Write Attribute status=0x%x", status);
}

void emAfPrepaymentServerCliVerifyPaymentMode(sl_cli_command_arg_t *arguments)
{
  PaymentControlConfiguration expectedPaymentControlConfiguration;
  PaymentControlConfiguration readPaymentControlConfiguration;
  EmberAfStatus status;
  uint8_t endpoint;
  uint8_t dataType;

  endpoint = sl_cli_get_argument_uint8(arguments, 0);
  expectedPaymentControlConfiguration = sl_cli_get_argument_uint16(arguments, 1);
  status = emberAfReadAttribute(endpoint, ZCL_PREPAYMENT_CLUSTER_ID,
                                ZCL_PAYMENT_CONTROL_CONFIGURATION_ATTRIBUTE_ID, CLUSTER_MASK_SERVER,
                                (uint8_t *)&readPaymentControlConfiguration, 2, &dataType);

  if ( (status == EMBER_ZCL_STATUS_SUCCESS) && (expectedPaymentControlConfiguration == readPaymentControlConfiguration) ) {
    emberAfPrepaymentClusterPrintln("Payment Mode Match Success - %d", readPaymentControlConfiguration);
  } else {
    emberAfPrepaymentClusterPrintln("Payment Mode Failed Match, status=0x%x, read=%d, exp=%d", status, readPaymentControlConfiguration, expectedPaymentControlConfiguration);
  }
}

void emAfPrepaymentServerCliVerifyAttribute(sl_cli_command_arg_t *arguments)
{
  EmberAfStatus status;
  uint32_t readAttributeValue;
  uint32_t expectedAttributeValue;
  uint16_t attributeId;

  uint8_t endpoint;
  uint8_t dataType;
  uint8_t attributeSize;

  endpoint = sl_cli_get_argument_uint8(arguments, 0);
  attributeId = sl_cli_get_argument_uint16(arguments, 1);
  attributeSize = sl_cli_get_argument_uint8(arguments, 2);
  expectedAttributeValue = sl_cli_get_argument_uint32(arguments, 3);
  status = emberAfReadAttribute(endpoint, ZCL_PREPAYMENT_CLUSTER_ID,
                                attributeId, CLUSTER_MASK_SERVER,
                                (uint8_t *)&readAttributeValue, attributeSize, &dataType);

  if ( (status == EMBER_ZCL_STATUS_SUCCESS) && (expectedAttributeValue == readAttributeValue) ) {
    emberAfPrepaymentClusterPrintln("Attribute Read Match Success - %d", status);
  } else {
    emberAfPrepaymentClusterPrintln("Attribute Read Failed Match status=0x%x, read=%d, exp=%d", status, readAttributeValue, expectedAttributeValue);
  }
}

#define MAX_SNAPSHOT_PAYLOAD_LEN  24
void emAfPrepaymentServerCliPublishPrepaySnapshot(sl_cli_command_arg_t *arguments)
{
  EmberNodeId nodeId = (EmberNodeId)sl_cli_get_argument_uint16(arguments, 0);
  uint8_t srcEndpoint = sl_cli_get_argument_uint8(arguments, 1);
  uint8_t dstEndpoint = sl_cli_get_argument_uint8(arguments, 2);
  uint32_t snapshotTableIndex = sl_cli_get_argument_uint32(arguments, 3);

  emberAfPluginPrepaymentSnapshotStoragePublishSnapshot(nodeId,
                                                        srcEndpoint,
                                                        dstEndpoint,
                                                        snapshotTableIndex);
}

void emAfPrepaymentServerCliAddSnapshotEvent(sl_cli_command_arg_t *arguments)
{
  uint32_t snapshotCause;
  uint8_t  endpoint;

  endpoint = sl_cli_get_argument_uint8(arguments, 0);
  snapshotCause = sl_cli_get_argument_uint32(arguments, 1);
  emberAfPrepaymentClusterPrintln("CLI Add Snapshot Event, endpoint=%d cause=0x%4x", endpoint, snapshotCause);

  emberAfPluginPrepaymentSnapshotStorageTakeSnapshot(endpoint, snapshotCause);
}

void emAfPrepaymentReadDebtLog(sl_cli_command_arg_t *arguments)
{
  uint8_t index;
  index = sl_cli_get_argument_uint8(arguments, 0);
  emberAfPluginPrepaymentPrintDebtLogIndex(index);
}

void emAfPrepaymentReadDebtAttributes(sl_cli_command_arg_t *arguments)
{
  uint8_t endpoint;
  uint8_t index;
  endpoint = sl_cli_get_argument_uint8(arguments, 0);
  index = sl_cli_get_argument_uint8(arguments, 1);
  emberAfPluginPrepaymentPrintDebtAttributes(endpoint, index);
}

void emAfPrepaymentGetTopUpPercentage(sl_cli_command_arg_t *arguments)
{
  uint8_t  endpoint;
  uint32_t topUpValue;

  endpoint = sl_cli_get_argument_uint8(arguments, 0);
  topUpValue = sl_cli_get_argument_uint32(arguments, 1);
  emberAfPluginPrepaymentGetDebtRecoveryTopUpPercentage(endpoint, topUpValue);
}

void emAfPrepaymentCheckCalendarCli(sl_cli_command_arg_t *arguments)
{
  uint32_t utcTime;
  uint32_t calcUtcTime;
  EmberAfTimeStruct afTime;

  utcTime = sl_cli_get_argument_uint32(arguments, 0);
  emberAfFillTimeStructFromUtc(utcTime, &afTime);
  emAfPrepaymentPrintAfTime(&afTime);

  calcUtcTime = emberAfGetUtcFromTimeStruct(&afTime);
  if ( calcUtcTime == utcTime ) {
    emberAfPrepaymentClusterPrintln("= UTC Times Match, 0x%4x", calcUtcTime);
  } else {
    emberAfPrepaymentClusterPrintln(" ERROR: UTC Times Don't Match, 0x%4x != 0x%4x", utcTime, calcUtcTime);
  }
}

void emAfPrepaymentGetWeekdayCli(sl_cli_command_arg_t *arguments)
{
  uint8_t weekday;
  uint32_t utcTime;

  utcTime = sl_cli_get_argument_uint32(arguments, 0);
  weekday = emberAfGetWeekdayFromUtc(utcTime);
  emberAfPrepaymentClusterPrintln("UTC Time=0x%4x, Weekday=%d", utcTime, weekday);
}

void emAfPrepaymentScheduleDebtRepaymentCli(sl_cli_command_arg_t *arguments)
{
  uint8_t  endpoint;
  uint8_t  debtType;
  uint16_t collectionTime;
  uint32_t issuerEventId;
  uint32_t collectionTimeSec;
  uint32_t startTime;
  uint8_t  collectionFrequency;
  uint8_t  i;

  endpoint = sl_cli_get_argument_uint8(arguments, 0);
  issuerEventId = sl_cli_get_argument_uint32(arguments, 1);
  debtType = sl_cli_get_argument_uint8(arguments, 2);
  collectionTime = sl_cli_get_argument_uint16(arguments, 3);
  startTime = sl_cli_get_argument_uint32(arguments, 4);
  collectionFrequency = sl_cli_get_argument_uint8(arguments, 5);

  emberAfPluginPrepaymentServerScheduleDebtRepayment(endpoint, issuerEventId, debtType, collectionTime, startTime, collectionFrequency);

  // Convert collectionTime (mins) to seconds for future comparisons.
  collectionTimeSec = ((uint32_t)collectionTime) * 60;

  // After calling the ScheduleDebtRepayment() function, verify a couple things.
  if ( debtType >= 3 ) {
    emberAfPrepaymentClusterPrintln("Debt type out of bounds");
  } else {
    i = debtType;
    if ( (DebtSchedule[i].firstCollectionTimeSec >= startTime)
         && (DebtSchedule[i].issuerEventId == issuerEventId)
         && (DebtSchedule[i].collectionFrequency == collectionFrequency)
         && (DebtSchedule[i].nextCollectionTimeUtc >= startTime)
         && ((DebtSchedule[i].nextCollectionTimeUtc % SECONDS_PER_DAY) == collectionTimeSec)
         && ((DebtSchedule[i].firstCollectionTimeSec % SECONDS_PER_DAY) == collectionTimeSec) ) {
      emberAfPrepaymentClusterPrintln("Valid Debt Schedule");
    } else {
      emberAfPrepaymentClusterPrintln("INVALID Debt Schedule");
      emberAfPrepaymentClusterPrintln("  first=%d, startTime=%d", DebtSchedule[i].firstCollectionTimeSec, startTime);
      emberAfPrepaymentClusterPrintln("  issuerEvtId=%d, %d", DebtSchedule[i].issuerEventId, issuerEventId);
      emberAfPrepaymentClusterPrintln("  collFreq=%d, %d", DebtSchedule[i].collectionFrequency, collectionFrequency);
      emberAfPrepaymentClusterPrintln("  nextColl=%d, startTime=%d", DebtSchedule[i].nextCollectionTimeUtc, startTime);
      emberAfPrepaymentClusterPrintln("  nextMOD=%d, collectTimeSec=%d", (DebtSchedule[i].nextCollectionTimeUtc % SECONDS_PER_DAY), collectionTimeSec);
      emberAfPrepaymentClusterPrintln("  firstMOD=%d, collectTimeSec=%d", (DebtSchedule[i].firstCollectionTimeSec % SECONDS_PER_DAY), collectionTimeSec);
    }
  }
}

#else

void emAfPrepaymentServerCliInit(void);
void emAfPrepaymentServerCliWriteAttribute(void);
void emAfPrepaymentServerCliChangePaymentModeRelative(void);
void emAfPrepaymentServerCliVerifyPaymentMode(void);
void emAfPrepaymentServerCliVerifyAttribute(void);
void emAfPrepaymentServerCliPublishPrepaySnapshot(void);
void emAfPrepaymentServerCliAddSnapshotEvent(void);
void emAfPrepaymentReadDebtLog(void);
void emAfPrepaymentReadDebtAttributes(void);
void emAfPrepaymentGetTopUpPercentage(void);
void emAfPrepaymentCheckCalendarCli(void);
void emAfPrepaymentGetWeekdayCli(void);
void emAfPrepaymentScheduleDebtRepaymentCli(void);

void emAfPrepaymentServerCliInit()
{
  uint8_t endpoint;
  endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  emberAfPrepaymentClusterServerInitCallback(endpoint);
}

// plugin prepayment-server writeAttribute <endpoint:1> <attributeId:2> <attributeType:1> <numBytes:1> <value:4>
void emAfPrepaymentServerCliWriteAttribute()
{
  uint16_t attributeId;
  uint8_t  status;
  uint8_t  endpoint;
  uint8_t  numBytes;
  uint8_t  attributeType;
  uint32_t value;

  endpoint      = (uint8_t) emberUnsignedCommandArgument(0);
  attributeId   = (uint16_t)emberUnsignedCommandArgument(1);
  attributeType = (uint8_t) emberUnsignedCommandArgument(2);
  numBytes      = (uint8_t) emberUnsignedCommandArgument(3);
  value         = (uint32_t)emberUnsignedCommandArgument(4);

  status = emberAfWriteAttribute(endpoint, ZCL_PREPAYMENT_CLUSTER_ID, attributeId,
                                 CLUSTER_MASK_SERVER, (uint8_t *)&value, attributeType);
  emberAfPrepaymentClusterPrintln("Write Attribute status=0x%x", status);
}

void emAfPrepaymentServerCliVerifyPaymentMode()
{
  PaymentControlConfiguration expectedPaymentControlConfiguration;
  PaymentControlConfiguration readPaymentControlConfiguration;
  EmberAfStatus status;
  uint8_t endpoint;
  uint8_t dataType;

  endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  expectedPaymentControlConfiguration = (uint16_t)emberUnsignedCommandArgument(1);
  status = emberAfReadAttribute(endpoint, ZCL_PREPAYMENT_CLUSTER_ID,
                                ZCL_PAYMENT_CONTROL_CONFIGURATION_ATTRIBUTE_ID, CLUSTER_MASK_SERVER,
                                (uint8_t *)&readPaymentControlConfiguration, 2, &dataType);

  if ( (status == EMBER_ZCL_STATUS_SUCCESS) && (expectedPaymentControlConfiguration == readPaymentControlConfiguration) ) {
    emberAfPrepaymentClusterPrintln("Payment Mode Match Success - %d", readPaymentControlConfiguration);
  } else {
    emberAfPrepaymentClusterPrintln("Payment Mode Failed Match, status=0x%x, read=%d, exp=%d", status, readPaymentControlConfiguration, expectedPaymentControlConfiguration);
  }
}

void emAfPrepaymentServerCliVerifyAttribute()
{
  EmberAfStatus status;
  uint32_t readAttributeValue;
  uint32_t expectedAttributeValue;
  uint16_t attributeId;

  uint8_t endpoint;
  uint8_t dataType;
  uint8_t attributeSize;

  endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  attributeId = (uint16_t)emberUnsignedCommandArgument(1);
  attributeSize = (uint8_t)emberUnsignedCommandArgument(2);
  expectedAttributeValue = (uint32_t)emberUnsignedCommandArgument(3);
  status = emberAfReadAttribute(endpoint, ZCL_PREPAYMENT_CLUSTER_ID,
                                attributeId, CLUSTER_MASK_SERVER,
                                (uint8_t *)&readAttributeValue, attributeSize, &dataType);

  if ( (status == EMBER_ZCL_STATUS_SUCCESS) && (expectedAttributeValue == readAttributeValue) ) {
    emberAfPrepaymentClusterPrintln("Attribute Read Match Success - %d", status);
  } else {
    emberAfPrepaymentClusterPrintln("Attribute Read Failed Match status=0x%x, read=%d, exp=%d", status, readAttributeValue, expectedAttributeValue);
  }
}

#define MAX_SNAPSHOT_PAYLOAD_LEN  24
void emAfPrepaymentServerCliPublishPrepaySnapshot()
{
  EmberNodeId nodeId = (EmberNodeId)emberUnsignedCommandArgument(0);
  uint8_t srcEndpoint = (uint8_t)emberUnsignedCommandArgument(1);
  uint8_t dstEndpoint = (uint8_t)emberUnsignedCommandArgument(2);
  uint32_t snapshotTableIndex = (uint32_t)emberUnsignedCommandArgument(3);

  emberAfPluginPrepaymentSnapshotStoragePublishSnapshot(nodeId,
                                                        srcEndpoint,
                                                        dstEndpoint,
                                                        snapshotTableIndex);
}

void emAfPrepaymentServerCliAddSnapshotEvent()
{
  uint32_t snapshotCause;
  uint8_t  endpoint;

  endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  snapshotCause = (uint32_t)emberUnsignedCommandArgument(1);
  emberAfPrepaymentClusterPrintln("CLI Add Snapshot Event, endpoint=%d cause=0x%4x", endpoint, snapshotCause);

  emberAfPluginPrepaymentSnapshotStorageTakeSnapshot(endpoint, snapshotCause);
}

void emAfPrepaymentReadDebtLog()
{
  uint8_t index;
  index = (uint8_t)emberUnsignedCommandArgument(0);
  emberAfPluginPrepaymentPrintDebtLogIndex(index);
}

void emAfPrepaymentReadDebtAttributes()
{
  uint8_t endpoint;
  uint8_t index;
  endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  index = (uint8_t)emberUnsignedCommandArgument(1);
  emberAfPluginPrepaymentPrintDebtAttributes(endpoint, index);
}

void emAfPrepaymentGetTopUpPercentage()
{
  uint8_t  endpoint;
  uint32_t topUpValue;

  endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  topUpValue = (uint32_t)emberUnsignedCommandArgument(1);
  emberAfPluginPrepaymentGetDebtRecoveryTopUpPercentage(endpoint, topUpValue);
}

void emAfPrepaymentCheckCalendarCli()
{
  uint32_t utcTime;
  uint32_t calcUtcTime;
  EmberAfTimeStruct afTime;

  utcTime = (uint32_t)emberUnsignedCommandArgument(0);
  emberAfFillTimeStructFromUtc(utcTime, &afTime);
  emAfPrepaymentPrintAfTime(&afTime);

  calcUtcTime = emberAfGetUtcFromTimeStruct(&afTime);
  if ( calcUtcTime == utcTime ) {
    emberAfPrepaymentClusterPrintln("= UTC Times Match, 0x%4x", calcUtcTime);
  } else {
    emberAfPrepaymentClusterPrintln(" ERROR: UTC Times Don't Match, 0x%4x != 0x%4x", utcTime, calcUtcTime);
  }
}

void emAfPrepaymentGetWeekdayCli()
{
  uint8_t weekday;
  uint32_t utcTime;

  utcTime = (uint32_t)emberUnsignedCommandArgument(0);
  weekday = emberAfGetWeekdayFromUtc(utcTime);
  emberAfPrepaymentClusterPrintln("UTC Time=0x%4x, Weekday=%d", utcTime, weekday);
}

void emAfPrepaymentScheduleDebtRepaymentCli()
{
  uint8_t  endpoint;
  uint8_t  debtType;
  uint16_t collectionTime;
  uint32_t issuerEventId;
  uint32_t collectionTimeSec;
  uint32_t startTime;
  uint8_t  collectionFrequency;
  uint8_t  i;

  endpoint = (uint8_t)emberUnsignedCommandArgument(0);
  issuerEventId = (uint32_t)emberUnsignedCommandArgument(1);
  debtType = (uint8_t)emberUnsignedCommandArgument(2);
  collectionTime = (uint16_t)emberUnsignedCommandArgument(3);
  startTime = (uint32_t)emberUnsignedCommandArgument(4);
  collectionFrequency = (uint8_t)emberUnsignedCommandArgument(5);

  emberAfPluginPrepaymentServerScheduleDebtRepayment(endpoint, issuerEventId, debtType, collectionTime, startTime, collectionFrequency);

  // Convert collectionTime (mins) to seconds for future comparisons.
  collectionTimeSec = ((uint32_t)collectionTime) * 60;

  // After calling the ScheduleDebtRepayment() function, verify a couple things.
  if ( debtType >= 3 ) {
    emberAfPrepaymentClusterPrintln("Debt type out of bounds");
  } else {
    i = debtType;
    if ( (DebtSchedule[i].firstCollectionTimeSec >= startTime)
         && (DebtSchedule[i].issuerEventId == issuerEventId)
         && (DebtSchedule[i].collectionFrequency == collectionFrequency)
         && (DebtSchedule[i].nextCollectionTimeUtc >= startTime)
         && ((DebtSchedule[i].nextCollectionTimeUtc % SECONDS_PER_DAY) == collectionTimeSec)
         && ((DebtSchedule[i].firstCollectionTimeSec % SECONDS_PER_DAY) == collectionTimeSec) ) {
      emberAfPrepaymentClusterPrintln("Valid Debt Schedule");
    } else {
      emberAfPrepaymentClusterPrintln("INVALID Debt Schedule");
      emberAfPrepaymentClusterPrintln("  first=%d, startTime=%d", DebtSchedule[i].firstCollectionTimeSec, startTime);
      emberAfPrepaymentClusterPrintln("  issuerEvtId=%d, %d", DebtSchedule[i].issuerEventId, issuerEventId);
      emberAfPrepaymentClusterPrintln("  collFreq=%d, %d", DebtSchedule[i].collectionFrequency, collectionFrequency);
      emberAfPrepaymentClusterPrintln("  nextColl=%d, startTime=%d", DebtSchedule[i].nextCollectionTimeUtc, startTime);
      emberAfPrepaymentClusterPrintln("  nextMOD=%d, collectTimeSec=%d", (DebtSchedule[i].nextCollectionTimeUtc % SECONDS_PER_DAY), collectionTimeSec);
      emberAfPrepaymentClusterPrintln("  firstMOD=%d, collectTimeSec=%d", (DebtSchedule[i].firstCollectionTimeSec % SECONDS_PER_DAY), collectionTimeSec);
    }
  }
}
#endif
