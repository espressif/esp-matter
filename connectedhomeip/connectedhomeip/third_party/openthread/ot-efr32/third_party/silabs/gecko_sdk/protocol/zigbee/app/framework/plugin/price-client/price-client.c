/***************************************************************************//**
 * @file
 * @brief The Price client plugin is responsible for keeping track of the current
 * and future prices.
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
#include "app/framework/util/common.h"
#include "app/framework/plugin/price-common/price-common.h"
#include "app/framework/plugin/price-common/price-common-time.h"
#include "price-client.h"

#ifdef UC_BUILD
#include "zap-cluster-command-parser.h"
#endif

#define VALID  BIT(1)
#define CANCELLATION_START_TIME  0xFFFFFFFF

// keep track of last seen issuerEventId to discard old commands.
static uint32_t lastSeenIssuerEventId = 0x00;
static EmberAfPriceClientInfo priceInfo;

// Used by the CLI and CppEventCallback to determine the default policy for non-forced CPP Events.
uint8_t emberAfPriceClusterDefaultCppEventAuthorization = EMBER_AF_PLUGIN_PRICE_CPP_AUTH_ACCEPTED;

static void initPrice(EmberAfPluginPriceClientPrice *price);
static void printPrice(EmberAfPluginPriceClientPrice *price);
static void scheduleTick(uint8_t endpoint);
static void emberAfPriceInitBlockPeriod(uint8_t endpoint);
static void emberAfPriceInitBlockThresholdsTable(uint8_t endpoint);
static void emberAfPriceInitConversionFactorTable(uint8_t endpoint);
static void emberAfPriceInitCalorificValueTable(uint8_t endpoint);
static void emberAfPriceClusterInitCpp(uint8_t endpoint);
static void emberAfPriceClusterInitCO2Table(uint8_t endpoint);
static void emAfPriceClientTierLabelsInit(uint8_t endpoint);
static void emberAfPriceClusterInitCreditPaymentTable(uint8_t endpoint);
static void emberAfPriceClusterInitCurrencyConversionTable(uint8_t endpoint);
static void emberAfPriceInitBillingPeriodTable(uint8_t endpoint);
static void emAfPriceClientAddTierLabel(uint8_t endpoint, uint32_t providerId, uint32_t issuerEventId, uint32_t issuerTariffId,
                                        uint8_t numberOfLabels, uint8_t *tierLabelsPayload);

static uint8_t emAfPriceCommonGetMatchingOrUnusedIndex(EmberAfPriceClientCommonInfo *pcommon, uint8_t numElements,
                                                       uint32_t newProviderId, uint32_t newIssuerEventId);
static uint8_t emAfPriceCommonGetMatchingIndex(EmberAfPriceClientCommonInfo *pcommon, uint8_t numElements, uint32_t issuerEventId);
static uint8_t emAfPriceCommonGetActiveIndex(EmberAfPriceClientCommonInfo *pcommon, uint8_t numElements);

static EmberAfPluginPriceClientPrice priceTable[EMBER_AF_PRICE_CLUSTER_CLIENT_ENDPOINT_COUNT][EMBER_AF_PLUGIN_PRICE_CLIENT_TABLE_SIZE];

void emAfPriceClearPriceTable(uint8_t endpoint)
{
  uint8_t i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint,
                                                     ZCL_PRICE_CLUSTER_ID);

  if (ep == 0xFF) {
    assert(false);
    return;
  }

  for (i = 0; i < EMBER_AF_PLUGIN_PRICE_CLIENT_TABLE_SIZE; i++) {
    initPrice(&priceTable[ep][i]);
  }
}

void emberAfPriceClusterClientInitCallback(uint8_t endpoint)
{
  uint8_t i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint,
                                                     ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }
  for (i = 0; i < EMBER_AF_PLUGIN_PRICE_CLIENT_TABLE_SIZE; i++) {
    initPrice(&priceTable[ep][i]);
  }
  emberAfPriceInitConsolidatedBillsTable(endpoint);
  emAfPriceClientTierLabelsInit(endpoint);
  emberAfPriceInitBlockPeriod(endpoint);
  emberAfPriceInitBlockThresholdsTable(endpoint);
  emberAfPriceClusterInitCreditPaymentTable(endpoint);
  emberAfPriceClusterInitCurrencyConversionTable(endpoint);
  emberAfPriceClusterInitCpp(endpoint);
  emberAfPriceClusterInitCO2Table(endpoint);
  emberAfPriceInitConversionFactorTable(endpoint);
  emberAfPriceInitCalorificValueTable(endpoint);
  emberAfPriceInitBillingPeriodTable(endpoint);
}

void emberAfPriceClusterClientTickCallback(uint8_t endpoint)
{
  scheduleTick(endpoint);
}

//-----------------------
// ZCL commands callbacks

#ifdef UC_BUILD

bool emberAfPriceClusterPublishPriceCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_price_cluster_publish_price_command_t cmd_data;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint,
                                                     ZCL_PRICE_CLUSTER_ID);
  EmberAfPluginPriceClientPrice *price = NULL, *last;
  EmberAfStatus status;
  uint32_t endTime, now = emberAfGetCurrentTime();
  uint8_t i;

  if ((zcl_decode_price_cluster_publish_price_command(cmd, &cmd_data)
       != EMBER_ZCL_STATUS_SUCCESS)
      || ep == 0xFF) {
    return false;
  }

  last = &priceTable[ep][0];

  emberAfPriceClusterPrint("RX: PublishPrice 0x%4x, \"", cmd_data.providerId);
  emberAfPriceClusterPrintString(cmd_data.rateLabel);
  emberAfPriceClusterPrint("\"");
  emberAfPriceClusterFlush();
  emberAfPriceClusterPrint(", 0x%4x, 0x%4x, 0x%x, 0x%2x, 0x%x, 0x%x, 0x%4x",
                           cmd_data.issuerEventId,
                           cmd_data.currentTime,
                           cmd_data.unitOfMeasure,
                           cmd_data.currency,
                           cmd_data.priceTrailingDigitAndPriceTier,
                           cmd_data.numberOfPriceTiersAndRegisterTier,
                           cmd_data.startTime);
  emberAfPriceClusterFlush();
  emberAfPriceClusterPrint(", 0x%2x, 0x%4x, 0x%x, 0x%4x, 0x%x, 0x%4x, 0x%x",
                           cmd_data.durationInMinutes,
                           cmd_data.price,
                           cmd_data.priceRatio,
                           cmd_data.generationPrice,
                           cmd_data.generationPriceRatio,
                           cmd_data.alternateCostDelivered,
                           cmd_data.alternateCostUnit);
  emberAfPriceClusterFlush();
  emberAfPriceClusterPrintln(", 0x%x, 0x%x, 0x%x",
                             cmd_data.alternateCostTrailingDigit,
                             cmd_data.numberOfBlockThresholds,
                             cmd_data.priceControl);
  emberAfPriceClusterFlush();

  if (cmd_data.startTime == ZCL_PRICE_CLUSTER_START_TIME_NOW) {
    cmd_data.startTime = now;
  }
  endTime = (cmd_data.durationInMinutes == ZCL_PRICE_CLUSTER_DURATION_UNTIL_CHANGED
             ? ZCL_PRICE_CLUSTER_END_TIME_NEVER
             : cmd_data.startTime + cmd_data.durationInMinutes * 60);

  // If the price has already expired, don't bother with it.
  if (endTime <= now) {
    status = EMBER_ZCL_STATUS_FAILURE;
    goto kickout;
  }

  for (i = 0; i < EMBER_AF_PLUGIN_PRICE_CLIENT_TABLE_SIZE; i++) {
    // Ignore invalid prices, but remember the empty slot for later.
    if (!priceTable[ep][i].valid) {
      if (price == NULL) {
        price = &priceTable[ep][i];
      }
      continue;
    }

    // Reject duplicate prices based on the issuer event id.  This assumes that
    // issuer event ids are unique and that pricing priceInformation associated with
    // an issuer event id never changes.
    if (priceTable[ep][i].issuerEventId == cmd_data.issuerEventId) {
      status = EMBER_ZCL_STATUS_DUPLICATE_EXISTS;
      goto kickout;
    }

    // Nested and overlapping prices are not allowed.  Prices with the newer
    // issuer event ids takes priority over all nested and overlapping prices.
    // The only exception is when a price with a newer issuer event id overlaps
    // with the end of the current active price.  In this case, the duration of
    // the current active price is changed to "until changed" and it will expire
    // when the new price starts.
    if (priceTable[ep][i].startTime < endTime
        && priceTable[ep][i].endTime > cmd_data.startTime) {
      if (priceTable[ep][i].issuerEventId < cmd_data.issuerEventId) {
        if (priceTable[ep][i].active && now < cmd_data.startTime) {
          priceTable[ep][i].endTime = cmd_data.startTime;
          priceTable[ep][i].durationInMinutes = ZCL_PRICE_CLUSTER_DURATION_UNTIL_CHANGED;
        } else {
          if (priceTable[ep][i].active) {
            priceTable[ep][i].active = false;
            emberAfPluginPriceClientPriceExpiredCallback(&priceTable[ep][i]);
          }
          initPrice(&priceTable[ep][i]);
        }
      } else {
        status = EMBER_ZCL_STATUS_FAILURE;
        goto kickout;
      }
    }

    // Along the way, search for an empty slot for this new price and find the
    // price in the table with the latest start time.  If there are no empty
    // slots, we will either have to drop this price or the last one, depending
    // on the start times.
    if (price == NULL) {
      if (!priceTable[ep][i].valid) {
        price = &priceTable[ep][i];
      } else if (last->startTime < priceTable[ep][i].startTime) {
        last = &priceTable[ep][i];
      }
    }
  }

  // If there were no empty slots and this price starts after all of the other
  // prices in the table, drop this price.  Otherwise, drop the price with the
  // latest start time and replace it with this one.
  if (price == NULL) {
    if (last->startTime < cmd_data.startTime) {
      status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
      goto kickout;
    } else {
      price = last;
    }
  }

  price->valid                             = true;
  price->active                            = false;
  price->clientEndpoint                    = endpoint;
  price->providerId                        = cmd_data.providerId;
  emberAfCopyString(price->rateLabel, cmd_data.rateLabel, ZCL_PRICE_CLUSTER_MAXIMUM_RATE_LABEL_LENGTH);
  price->issuerEventId                     = cmd_data.issuerEventId;
  price->currentTime                       = cmd_data.currentTime;
  price->unitOfMeasure                     = cmd_data.unitOfMeasure;
  price->currency                          = cmd_data.currency;
  price->priceTrailingDigitAndPriceTier    = cmd_data.priceTrailingDigitAndPriceTier;
  price->numberOfPriceTiersAndRegisterTier = cmd_data.numberOfPriceTiersAndRegisterTier;
  price->startTime                         = cmd_data.startTime;
  price->endTime                           = endTime;
  price->durationInMinutes                 = cmd_data.durationInMinutes;
  price->price                             = cmd_data.price;
  price->priceRatio                        = cmd_data.priceRatio;
  price->generationPrice                   = cmd_data.generationPrice;
  price->generationPriceRatio              = cmd_data.generationPriceRatio;
  price->alternateCostDelivered            = cmd_data.alternateCostDelivered;
  price->alternateCostUnit                 = cmd_data.alternateCostUnit;
  price->alternateCostTrailingDigit        = cmd_data.alternateCostTrailingDigit;
  price->numberOfBlockThresholds           = cmd_data.numberOfBlockThresholds;
  price->priceControl                      = cmd_data.priceControl;

  // Now that we have saved the price in our table, we may have to reschedule
  // our tick to activate or expire prices.
  scheduleTick(endpoint);

  // If the acknowledgement is required, send it immediately.  Otherwise, a
  // default response is sufficient.
  if (cmd_data.priceControl & ZCL_PRICE_CLUSTER_PRICE_ACKNOWLEDGEMENT_MASK) {
    emberAfFillCommandPriceClusterPriceAcknowledgement(cmd_data.providerId,
                                                       cmd_data.issuerEventId,
                                                       now,
                                                       cmd_data.priceControl);
    emberAfSendResponse();
    return true;
  } else {
    status = EMBER_ZCL_STATUS_SUCCESS;
  }

  kickout:
  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfPriceClusterPublishBlockPeriodCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_price_cluster_publish_block_period_command_t cmd_data;
  uint8_t i;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);

  if ((zcl_decode_price_cluster_publish_block_period_command(cmd, &cmd_data)
       != EMBER_ZCL_STATUS_SUCCESS)
      || ep == 0xFF) {
    return false;
  }

  emberAfPriceClusterPrintln("RX: PublishBlockPeriod, 0x%4x, 0x%4x, 0x%4x, 0x%4X, 0x%X, 0x%X, 0x%X, 0x%X",
                             cmd_data.providerId,
                             cmd_data.issuerEventId,
                             cmd_data.blockPeriodStartTime,
                             cmd_data.blockPeriodDuration, // int24u
                             cmd_data.blockPeriodControl,
                             cmd_data.blockPeriodDurationType,
                             cmd_data.tariffType,
                             cmd_data.tariffResolutionPeriod);

  if ( cmd_data.blockPeriodStartTime == 0 ) {
    cmd_data.blockPeriodStartTime = emberAfGetCurrentTime();
  }

  // Find the entry to update
  // We will update any invalid entry, or the oldest event ID.
  i = emAfPriceCommonGetMatchingOrUnusedIndex(priceInfo.blockPeriodTable.commonInfos[ep],
                                              EMBER_AF_PLUGIN_PRICE_CLIENT_BLOCK_PERIOD_TABLE_SIZE,
                                              cmd_data.providerId, cmd_data.issuerEventId);
  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_BLOCK_PERIOD_TABLE_SIZE ) {
    priceInfo.blockPeriodTable.commonInfos[ep][i].valid = true;
    priceInfo.blockPeriodTable.commonInfos[ep][i].providerId = cmd_data.providerId;
    priceInfo.blockPeriodTable.commonInfos[ep][i].issuerEventId = cmd_data.issuerEventId;
    priceInfo.blockPeriodTable.commonInfos[ep][i].startTime = emberAfPluginPriceCommonClusterGetAdjustedStartTime(cmd_data.blockPeriodStartTime,
                                                                                                                  cmd_data.blockPeriodDurationType);
    priceInfo.blockPeriodTable.commonInfos[ep][i].durationSec =
      emberAfPluginPriceCommonClusterConvertDurationToSeconds(cmd_data.blockPeriodStartTime,
                                                              cmd_data.blockPeriodDuration,
                                                              cmd_data.blockPeriodDurationType);
    priceInfo.blockPeriodTable.blockPeriod[ep][i].blockPeriodStartTime = cmd_data.blockPeriodStartTime;
    priceInfo.blockPeriodTable.blockPeriod[ep][i].blockPeriodDuration = cmd_data.blockPeriodDuration;
    priceInfo.blockPeriodTable.blockPeriod[ep][i].blockPeriodControl = cmd_data.blockPeriodControl;
    priceInfo.blockPeriodTable.blockPeriod[ep][i].blockPeriodDurationType = cmd_data.blockPeriodDurationType;
    priceInfo.blockPeriodTable.blockPeriod[ep][i].tariffType = cmd_data.tariffType;
    priceInfo.blockPeriodTable.blockPeriod[ep][i].tariffResolutionPeriod = cmd_data.tariffResolutionPeriod;
  }

  return true;
}

bool emberAfPriceClusterPublishConversionFactorCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_price_cluster_publish_conversion_factor_command_t cmd_data;
  uint8_t  i;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);

  if ((zcl_decode_price_cluster_publish_conversion_factor_command(cmd, &cmd_data)
       != EMBER_ZCL_STATUS_SUCCESS)
      || ep == 0xFF) {
    return false;
  }

  emberAfPriceClusterPrintln("RX: PublishConversionFactor 0x%4X, 0x%4X, 0x%4X, 0x%X",
                             cmd_data.issuerEventId,
                             cmd_data.startTime,
                             cmd_data.conversionFactor,
                             cmd_data.conversionFactorTrailingDigit);

  if ( cmd_data.startTime == 0 ) {
    cmd_data.startTime = emberAfGetCurrentTime();
  }

  i = emAfPriceCommonGetMatchingOrUnusedIndex(priceInfo.conversionFactorTable.commonInfos[ep],
                                              EMBER_AF_PLUGIN_PRICE_CLIENT_CONVERSION_FACTOR_TABLE_SIZE,
                                              UNSPECIFIED_PROVIDER_ID,
                                              cmd_data.issuerEventId);
  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_CONVERSION_FACTOR_TABLE_SIZE ) {
    // Only update the entry if the new eventID is greater than this one.
    priceInfo.conversionFactorTable.commonInfos[ep][i].valid = true;
    priceInfo.conversionFactorTable.commonInfos[ep][i].providerId = UNSPECIFIED_PROVIDER_ID;
    priceInfo.conversionFactorTable.commonInfos[ep][i].issuerEventId = cmd_data.issuerEventId;
    priceInfo.conversionFactorTable.commonInfos[ep][i].startTime = cmd_data.startTime;
    priceInfo.conversionFactorTable.commonInfos[ep][i].durationSec = UNSPECIFIED_DURATION;
    priceInfo.conversionFactorTable.conversionFactor[ep][i].conversionFactor = cmd_data.conversionFactor;
    priceInfo.conversionFactorTable.conversionFactor[ep][i].conversionFactorTrailingDigit = cmd_data.conversionFactorTrailingDigit;
  }
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfPriceClusterPublishCalorificValueCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_price_cluster_publish_calorific_value_command_t cmd_data;
  uint8_t  i;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);

  if ((zcl_decode_price_cluster_publish_calorific_value_command(cmd, &cmd_data)
       != EMBER_ZCL_STATUS_SUCCESS)
      || ep == 0xFF) {
    return false;
  }

  emberAfPriceClusterPrintln("RX: PublishCalorificValue 0x%4X, 0x%4X, 0x%4X, 0x%X, 0x%X",
                             cmd_data.issuerEventId,
                             cmd_data.startTime,
                             cmd_data.calorificValue,
                             cmd_data.calorificValueUnit,
                             cmd_data.calorificValueTrailingDigit);

  if ( cmd_data.startTime == 0 ) {
    cmd_data.startTime = emberAfGetCurrentTime();
  }

  i = emAfPriceCommonGetMatchingOrUnusedIndex(priceInfo.calorificValueTable.commonInfos[ep], EMBER_AF_PLUGIN_PRICE_CLIENT_CALORIFIC_VALUE_TABLE_SIZE,
                                              UNSPECIFIED_PROVIDER_ID, cmd_data.issuerEventId);

  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_CALORIFIC_VALUE_TABLE_SIZE ) {
    // Only update the entry if the new eventID is greater than this one.
    priceInfo.calorificValueTable.commonInfos[ep][i].valid = true;
    priceInfo.calorificValueTable.commonInfos[ep][i].issuerEventId = cmd_data.issuerEventId;
    priceInfo.calorificValueTable.commonInfos[ep][i].startTime = cmd_data.startTime;
    priceInfo.calorificValueTable.calorificValue[ep][i].calorificValue = cmd_data.calorificValue;
    priceInfo.calorificValueTable.calorificValue[ep][i].calorificValueUnit = cmd_data.calorificValueUnit;
    priceInfo.calorificValueTable.calorificValue[ep][i].calorificValueTrailingDigit = cmd_data.calorificValueTrailingDigit;
  }
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfPriceClusterPublishTariffInformationCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_price_cluster_publish_tariff_information_command_t cmd_data;

  if (zcl_decode_price_cluster_publish_tariff_information_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfPriceClusterPrint("RX: PublishTariffInformation 0x%4x, 0x%4x, 0x%4x, 0x%4x, 0x%x, \"",
                           cmd_data.providerId,
                           cmd_data.issuerEventId,
                           cmd_data.issuerTariffId,
                           cmd_data.startTime,
                           cmd_data.tariffTypeChargingScheme);

  emberAfPriceClusterPrintString(cmd_data.tariffLabel);
  emberAfPriceClusterPrint("\"");
  emberAfPriceClusterPrint(", 0x%x, 0x%x, 0x%x, 0x%2x, 0x%x",
                           cmd_data.numberOfPriceTiersInUse,
                           cmd_data.numberOfBlockThresholdsInUse,
                           cmd_data.unitOfMeasure,
                           cmd_data.currency,
                           cmd_data.priceTrailingDigit);
  emberAfPriceClusterPrintln(", 0x%4x, 0x%x, 0x%4x, 0x%4x",
                             cmd_data.standingCharge,
                             cmd_data.tierBlockMode,
                             cmd_data.blockThresholdMultiplier,
                             cmd_data.blockThresholdDivisor);
  emberAfPriceClusterFlush();

  if (lastSeenIssuerEventId >= cmd_data.issuerEventId) {
    // reject old command.
    emberAfPriceClusterPrintln("Rejected command due to old issuer event id (0x%4X)!",
                               cmd_data.issuerEventId);
  } else {
    // accept command.
    // optional attributes are not updated for now.
    lastSeenIssuerEventId = cmd_data.issuerEventId;
  }

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#define BLOCK_THRESHOLD_SUB_PAYLOAD_ALL_TOU_TIERS  (1 << 0)

bool emberAfPriceClusterPublishBlockThresholdsCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_price_cluster_publish_block_thresholds_command_t cmd_data;
  uint8_t tierNumber;
  uint8_t numThresholds;
  uint8_t i;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);

  if ((zcl_decode_price_cluster_publish_block_thresholds_command(cmd, &cmd_data)
       != EMBER_ZCL_STATUS_SUCCESS)
      || ep == 0xFF) {
    return false;
  }
  emberAfPriceClusterPrintln("RX: PublishBlockThresholds 0x%4x, 0x%4x, 0x%4x, 0x%4x, 0x%x, 0x%x, 0x%x",
                             cmd_data.providerId,
                             cmd_data.issuerEventId,
                             cmd_data.startTime,
                             cmd_data.issuerTariffId,
                             cmd_data.commandIndex,
                             cmd_data.numberOfCommands,
                             cmd_data.subPayloadControl);

  i = emAfPriceCommonGetMatchingOrUnusedIndex(priceInfo.blockThresholdTable.commonInfos[ep],
                                              cmd_data.numberOfCommands, cmd_data.providerId, cmd_data.issuerEventId);

  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_BLOCK_THRESHOLD_TABLE_SIZE ) {
    // Only update the entry if the new eventID is greater than this one.
    tierNumber = cmd_data.payload[0] >> 4;
    numThresholds = cmd_data.payload[0] & 0x0F;
    if ( (numThresholds >= EMBER_AF_PLUGIN_PRICE_CLIENT_MAX_NUMBER_BLOCK_THRESHOLDS)
         || (tierNumber >= EMBER_AF_PLUGIN_PRICE_CLIENT_MAX_NUMBER_TIERS) ) {
      // Out of range
      goto kickout;
    }
    priceInfo.blockThresholdTable.commonInfos[ep][i].valid = true;
    priceInfo.blockThresholdTable.commonInfos[ep][i].providerId = cmd_data.providerId;
    priceInfo.blockThresholdTable.commonInfos[ep][i].issuerEventId = cmd_data.issuerEventId;
    priceInfo.blockThresholdTable.commonInfos[ep][i].startTime = cmd_data.startTime;
    priceInfo.blockThresholdTable.commonInfos[ep][i].durationSec = UNSPECIFIED_DURATION;
    priceInfo.blockThresholdTable.blockThreshold[ep][i].issuerTariffId = cmd_data.issuerTariffId;
    priceInfo.blockThresholdTable.blockThreshold[ep][i].subPayloadControl = cmd_data.subPayloadControl;
    if ( cmd_data.subPayloadControl & BLOCK_THRESHOLD_SUB_PAYLOAD_ALL_TOU_TIERS ) {
      priceInfo.blockThresholdTable.blockThreshold[ep][i].tierNumberOfBlockThresholds[0] = cmd_data.payload[0];
      MEMCOPY(priceInfo.blockThresholdTable.blockThreshold[ep][i].blockThreshold[0], &cmd_data.payload[1], (numThresholds * 6) );
    } else {
      // TODO:   Note that multiple tier/NumberOfBlockThresholds fields could be present in this case.
      // eg. The payload could specify 5 blocks for tier 1, 2 blocks for tier 2, etc.
      // However, we don't know how many bytes are in "payload", so we can't read them all.  =(
      // For now, read the first tiers worth of data.
      //x = 0;

      priceInfo.blockThresholdTable.blockThreshold[ep][i].tierNumberOfBlockThresholds[tierNumber] = cmd_data.payload[0];
      MEMCOPY(priceInfo.blockThresholdTable.blockThreshold[ep][i].blockThreshold[tierNumber], &cmd_data.payload[1], (numThresholds * 6) );
    }
  }

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  kickout:
  return true;
}

bool emberAfPriceClusterPublishCO2ValueCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_price_cluster_publish_co2_value_command_t cmd_data;
  uint8_t  i;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);

  if ((zcl_decode_price_cluster_publish_co2_value_command(cmd, &cmd_data)
       != EMBER_ZCL_STATUS_SUCCESS)
      || ep == 0xFF) {
    return false;
  }

  emberAfPriceClusterPrintln("RX: Publish CO2 Value");

  if ( cmd_data.startTime == 0 ) {
    cmd_data.startTime = emberAfGetCurrentTime();
  }
  i = emAfPriceCommonGetMatchingOrUnusedIndex(priceInfo.co2ValueTable.commonInfos[ep], EMBER_AF_PLUGIN_PRICE_CLIENT_CO2_TABLE_SIZE,
                                              cmd_data.providerId, cmd_data.issuerEventId);

  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_CO2_TABLE_SIZE ) {
    // Do NOT overwrite data if the entry is valid and the new
    // data has a smaller (or equal) event ID.
    priceInfo.co2ValueTable.commonInfos[ep][i].providerId = cmd_data.providerId;
    priceInfo.co2ValueTable.commonInfos[ep][i].issuerEventId = cmd_data.issuerEventId;
    priceInfo.co2ValueTable.commonInfos[ep][i].startTime = cmd_data.startTime;
    priceInfo.co2ValueTable.co2Value[ep][i].tariffType = cmd_data.tariffType;
    priceInfo.co2ValueTable.commonInfos[ep][i].valid = true;
    priceInfo.co2ValueTable.co2Value[ep][i].cO2Value = cmd_data.cO2Value;
    priceInfo.co2ValueTable.co2Value[ep][i].cO2ValueUnit = cmd_data.cO2ValueUnit;
    priceInfo.co2ValueTable.co2Value[ep][i].cO2ValueTrailingDigit = cmd_data.cO2ValueTrailingDigit;
  }
  return true;
}

bool emberAfPriceClusterPublishTierLabelsCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_price_cluster_publish_tier_labels_command_t cmd_data;
  uint8_t endpoint = emberAfCurrentEndpoint();

  if (zcl_decode_price_cluster_publish_tier_labels_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfPriceClusterPrintln("RX: PublishTierLabels");
  emAfPriceClientAddTierLabel(endpoint, cmd_data.providerId,
                              cmd_data.issuerEventId, cmd_data.issuerTariffId,
                              cmd_data.numberOfLabels, cmd_data.tierLabelsPayload);

  return true;
}

bool emberAfPriceClusterPublishBillingPeriodCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_price_cluster_publish_billing_period_command_t cmd_data;
  uint8_t  i;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);

  if ((zcl_decode_price_cluster_publish_billing_period_command(cmd, &cmd_data)
       != EMBER_ZCL_STATUS_SUCCESS)
      || ep == 0xFF) {
    return false;
  }

  emberAfPriceClusterPrintln("RX: PublishBillingPeriod, issuerId=%d,  start=0x%4x", cmd_data.issuerEventId, cmd_data.billingPeriodStartTime);

  if ( cmd_data.billingPeriodStartTime == 0 ) {
    cmd_data.billingPeriodStartTime = emberAfGetCurrentTime();
  }

  i = emAfPriceCommonGetMatchingOrUnusedIndex(priceInfo.billingPeriodTable.commonInfos[ep],
                                              EMBER_AF_PLUGIN_PRICE_CLIENT_BILLING_PERIOD_TABLE_SIZE,
                                              cmd_data.providerId, cmd_data.issuerEventId);

  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_BILLING_PERIOD_TABLE_SIZE ) {
    if ( cmd_data.billingPeriodStartTime == CANCELLATION_START_TIME ) {
      priceInfo.billingPeriodTable.commonInfos[ep][i].valid = false;
      emberAfPriceClusterPrintln("Canceling eventId=%d", cmd_data.issuerEventId);
    } else {
      priceInfo.billingPeriodTable.commonInfos[ep][i].valid = true;
      priceInfo.billingPeriodTable.commonInfos[ep][i].providerId = cmd_data.providerId;
      priceInfo.billingPeriodTable.commonInfos[ep][i].issuerEventId = cmd_data.issuerEventId;
      priceInfo.billingPeriodTable.commonInfos[ep][i].startTime =
        emberAfPluginPriceCommonClusterGetAdjustedStartTime(cmd_data.billingPeriodStartTime,
                                                            cmd_data.billingPeriodDurationType);
      priceInfo.billingPeriodTable.commonInfos[ep][i].durationSec =
        emberAfPluginPriceCommonClusterConvertDurationToSeconds(cmd_data.billingPeriodStartTime,
                                                                cmd_data.billingPeriodDuration,
                                                                cmd_data.billingPeriodDurationType);

      priceInfo.billingPeriodTable.billingPeriod[ep][i].billingPeriodStartTime = cmd_data.billingPeriodStartTime;
      priceInfo.billingPeriodTable.billingPeriod[ep][i].billingPeriodDuration = cmd_data.billingPeriodDuration;
      priceInfo.billingPeriodTable.billingPeriod[ep][i].billingPeriodDurationType = cmd_data.billingPeriodDurationType;
      priceInfo.billingPeriodTable.billingPeriod[ep][i].tariffType = cmd_data.tariffType;
    }
  }

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfPriceClusterPublishCppEventCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_price_cluster_publish_cpp_event_command_t cmd_data;
  uint8_t responseCppAuth;
  EmberAfStatus status;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);

  if ((zcl_decode_price_cluster_publish_cpp_event_command(cmd, &cmd_data)
       != EMBER_ZCL_STATUS_SUCCESS)
      || ep == 0xFF) {
    return false;
  }

  emberAfPriceClusterPrintln("RX: PublishCppEvent:");
  if ( (cmd_data.cppPriceTier > 1) || (cmd_data.cppAuth >= EMBER_AF_PLUGIN_PRICE_CPP_AUTH_RESERVED) ) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_INVALID_FIELD);
  } else {
    if ( cmd_data.startTime == 0x00000000 ) {
      cmd_data.startTime = emberAfGetCurrentTime();
    } else if ( cmd_data.startTime == CANCELLATION_START_TIME ) {
      // Cancellation attempt.
      if ( (priceInfo.cppEventTable.commonInfos[ep].providerId == cmd_data.providerId)
           && (priceInfo.cppEventTable.commonInfos[ep].issuerEventId == cmd_data.issuerEventId) ) {
        emberAfPriceClusterPrintln("CPP Event Cancelled");
        priceInfo.cppEventTable.commonInfos[ep].valid = false;
        status = EMBER_ZCL_STATUS_SUCCESS;
      } else {
        status = EMBER_ZCL_STATUS_NOT_FOUND;
      }
      emberAfSendImmediateDefaultResponse(status);
      goto kickout;
    }

    // If the cppAuth is PENDING, the client may decide to accept or reject the CPP event.
    if ( cmd_data.cppAuth == EMBER_AF_PLUGIN_PRICE_CPP_AUTH_PENDING ) {
      responseCppAuth = emberAfPluginPriceClientPendingCppEventCallback(cmd_data.cppAuth);
      emberAfPriceClusterPrintln("  Pending CPP Event, status=%d", responseCppAuth);
      emberAfFillCommandPriceClusterCppEventResponse(cmd_data.issuerEventId, responseCppAuth);
      emberAfSendResponse();
    } else {
      if ( (cmd_data.cppAuth == EMBER_AF_PLUGIN_PRICE_CPP_AUTH_ACCEPTED)
           || (cmd_data.cppAuth == EMBER_AF_PLUGIN_PRICE_CPP_AUTH_FORCED) ) {
        // Apply the CPP
        emberAfPriceClusterPrintln("CPP Event Accepted");
        priceInfo.cppEventTable.commonInfos[ep].providerId = cmd_data.providerId;
        priceInfo.cppEventTable.commonInfos[ep].issuerEventId = cmd_data.issuerEventId;
        priceInfo.cppEventTable.commonInfos[ep].startTime = cmd_data.startTime;
        priceInfo.cppEventTable.commonInfos[ep].durationSec = (cmd_data.durationInMinutes * 60);
        priceInfo.cppEventTable.commonInfos[ep].valid = true;
        priceInfo.cppEventTable.cppEvent[ep].durationInMinutes = cmd_data.durationInMinutes;
        priceInfo.cppEventTable.cppEvent[ep].tariffType = cmd_data.tariffType;
        priceInfo.cppEventTable.cppEvent[ep].cppPriceTier = cmd_data.cppPriceTier;
        priceInfo.cppEventTable.cppEvent[ep].cppAuth = cmd_data.cppAuth;
      } else if ( cmd_data.cppAuth == EMBER_AF_PLUGIN_PRICE_CPP_AUTH_REJECTED ) {
        emberAfPriceClusterPrintln("CPP Event Rejected");
      }
      emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    }
  }
  kickout:
  return true;
}

bool emberAfPriceClusterPublishCreditPaymentCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_price_cluster_publish_credit_payment_command_t cmd_data;
  uint8_t i;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);

  if ((zcl_decode_price_cluster_publish_credit_payment_command(cmd, &cmd_data)
       != EMBER_ZCL_STATUS_SUCCESS)
      || ep == 0xFF) {
    return false;
  }

  i = emAfPriceCommonGetMatchingOrUnusedIndex(priceInfo.creditPaymentTable.commonInfos[ep],
                                              EMBER_AF_PLUGIN_PRICE_CLIENT_CREDIT_PAYMENT_TABLE_SIZE,
                                              cmd_data.providerId, cmd_data.issuerEventId);

  emberAfPriceClusterPrintln("RX: PublishCreditPayment [%d], issuerEventId=%d", i, cmd_data.issuerEventId);
  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_CREDIT_PAYMENT_TABLE_SIZE ) {
    priceInfo.creditPaymentTable.commonInfos[ep][i].valid = true;
    priceInfo.creditPaymentTable.commonInfos[ep][i].providerId = cmd_data.providerId;
    priceInfo.creditPaymentTable.commonInfos[ep][i].issuerEventId = cmd_data.issuerEventId;
    priceInfo.creditPaymentTable.creditPayment[ep][i].creditPaymentDueDate = cmd_data.creditPaymentDueDate;
    priceInfo.creditPaymentTable.creditPayment[ep][i].creditPaymentOverDueAmount = cmd_data.creditPaymentOverDueAmount;
    priceInfo.creditPaymentTable.creditPayment[ep][i].creditPaymentStatus = cmd_data.creditPaymentStatus;
    priceInfo.creditPaymentTable.creditPayment[ep][i].creditPayment = cmd_data.creditPayment;
    priceInfo.creditPaymentTable.creditPayment[ep][i].creditPaymentDate = cmd_data.creditPaymentDate;
    emberAfCopyString(priceInfo.creditPaymentTable.creditPayment[ep][i].creditPaymentRef, cmd_data.creditPaymentRef, EMBER_AF_PLUGIN_PRICE_CLUSTER_MAX_CREDIT_PAYMENT_REF_LENGTH);
  }

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfPriceClusterPublishCurrencyConversionCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_price_cluster_publish_currency_conversion_command_t cmd_data;
  uint8_t i;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);

  if ((zcl_decode_price_cluster_publish_currency_conversion_command(cmd, &cmd_data)
       != EMBER_ZCL_STATUS_SUCCESS)
      || ep == 0xFF) {
    return false;
  }

  emberAfPriceClusterPrintln("RX: Currency Conversion, start=%d, pid=%d, eid=%d", cmd_data.startTime, cmd_data.providerId, cmd_data.issuerEventId);

  if ( cmd_data.startTime == 0 ) {
    cmd_data.startTime = emberAfGetCurrentTime();
  }

  i = emAfPriceCommonGetMatchingOrUnusedIndex(priceInfo.currencyConversionTable.commonInfos[ep],
                                              EMBER_AF_PLUGIN_PRICE_CLIENT_CURRENCY_CONVERSION_TABLE_SIZE,
                                              cmd_data.providerId, cmd_data.issuerEventId);

  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_CURRENCY_CONVERSION_TABLE_SIZE ) {
    if ( cmd_data.startTime == CANCELLATION_START_TIME ) {
      priceInfo.currencyConversionTable.commonInfos[ep][i].valid = false;
    } else {
      priceInfo.currencyConversionTable.commonInfos[ep][i].valid = true;
      priceInfo.currencyConversionTable.commonInfos[ep][i].providerId = cmd_data.providerId;
      priceInfo.currencyConversionTable.commonInfos[ep][i].issuerEventId = cmd_data.issuerEventId;
      priceInfo.currencyConversionTable.commonInfos[ep][i].startTime = cmd_data.startTime;
      priceInfo.currencyConversionTable.currencyConversion[ep][i].newCurrency = cmd_data.newCurrency;
      priceInfo.currencyConversionTable.currencyConversion[ep][i].conversionFactor = cmd_data.conversionFactor;
      priceInfo.currencyConversionTable.currencyConversion[ep][i].conversionFactorTrailingDigit = cmd_data.conversionFactorTrailingDigit;
      priceInfo.currencyConversionTable.currencyConversion[ep][i].currencyChangeControlFlags = cmd_data.currencyChangeControlFlags;
    }
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  }
  return true;
}

bool emberAfPriceClusterCancelTariffCallback(EmberAfClusterCommand *cmd)
{
#ifdef SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT
  sl_zcl_price_cluster_cancel_tariff_command_t cmd_data;

  if (zcl_decode_price_cluster_cancel_tariff_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  emberAfPriceClusterPrintln("RX: Cancel Tariff, providerId=%d, issuerTariffId=%d, tariffType=%d",
                             cmd_data.providerId, cmd_data.issuerTariffId, cmd_data.tariffType);
#endif // SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#else // !UC_BUILD

bool emberAfPriceClusterPublishPriceCallback(uint32_t providerId,
                                             uint8_t* rateLabel,
                                             uint32_t issuerEventId,
                                             uint32_t currentTime,
                                             uint8_t unitOfMeasure,
                                             uint16_t currency,
                                             uint8_t priceTrailingDigitAndPriceTier,
                                             uint8_t numberOfPriceTiersAndRegisterTier,
                                             uint32_t startTime,
                                             uint16_t durationInMinutes,
                                             uint32_t prc,
                                             uint8_t priceRatio,
                                             uint32_t generationPrice,
                                             uint8_t generationPriceRatio,
                                             uint32_t alternateCostDelivered,
                                             uint8_t alternateCostUnit,
                                             uint8_t alternateCostTrailingDigit,
                                             uint8_t numberOfBlockThresholds,
                                             uint8_t priceControl,
                                             uint8_t numberOfGenerationTiers,
                                             uint8_t generationTier,
                                             uint8_t extendedNumberOfPriceTiers,
                                             uint8_t extendedPriceTier,
                                             uint8_t extendedRegisterTier)
{
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint,
                                                     ZCL_PRICE_CLUSTER_ID);
  EmberAfPluginPriceClientPrice *price = NULL, *last;
  EmberAfStatus status;
  uint32_t endTime, now = emberAfGetCurrentTime();
  uint8_t i;

  if (ep == 0xFF) {
    return false;
  }

  last = &priceTable[ep][0];

  emberAfPriceClusterPrint("RX: PublishPrice 0x%4x, \"", providerId);
  emberAfPriceClusterPrintString(rateLabel);
  emberAfPriceClusterPrint("\"");
  emberAfPriceClusterFlush();
  emberAfPriceClusterPrint(", 0x%4x, 0x%4x, 0x%x, 0x%2x, 0x%x, 0x%x, 0x%4x",
                           issuerEventId,
                           currentTime,
                           unitOfMeasure,
                           currency,
                           priceTrailingDigitAndPriceTier,
                           numberOfPriceTiersAndRegisterTier,
                           startTime);
  emberAfPriceClusterFlush();
  emberAfPriceClusterPrint(", 0x%2x, 0x%4x, 0x%x, 0x%4x, 0x%x, 0x%4x, 0x%x",
                           durationInMinutes,
                           prc,
                           priceRatio,
                           generationPrice,
                           generationPriceRatio,
                           alternateCostDelivered,
                           alternateCostUnit);
  emberAfPriceClusterFlush();
  emberAfPriceClusterPrintln(", 0x%x, 0x%x, 0x%x",
                             alternateCostTrailingDigit,
                             numberOfBlockThresholds,
                             priceControl);
  emberAfPriceClusterFlush();

  if (startTime == ZCL_PRICE_CLUSTER_START_TIME_NOW) {
    startTime = now;
  }
  endTime = (durationInMinutes == ZCL_PRICE_CLUSTER_DURATION_UNTIL_CHANGED
             ? ZCL_PRICE_CLUSTER_END_TIME_NEVER
             : startTime + durationInMinutes * 60);

  // If the price has already expired, don't bother with it.
  if (endTime <= now) {
    status = EMBER_ZCL_STATUS_FAILURE;
    goto kickout;
  }

  for (i = 0; i < EMBER_AF_PLUGIN_PRICE_CLIENT_TABLE_SIZE; i++) {
    // Ignore invalid prices, but remember the empty slot for later.
    if (!priceTable[ep][i].valid) {
      if (price == NULL) {
        price = &priceTable[ep][i];
      }
      continue;
    }

    // Reject duplicate prices based on the issuer event id.  This assumes that
    // issuer event ids are unique and that pricing priceInformation associated with
    // an issuer event id never changes.
    if (priceTable[ep][i].issuerEventId == issuerEventId) {
      status = EMBER_ZCL_STATUS_DUPLICATE_EXISTS;
      goto kickout;
    }

    // Nested and overlapping prices are not allowed.  Prices with the newer
    // issuer event ids takes priority over all nested and overlapping prices.
    // The only exception is when a price with a newer issuer event id overlaps
    // with the end of the current active price.  In this case, the duration of
    // the current active price is changed to "until changed" and it will expire
    // when the new price starts.
    if (priceTable[ep][i].startTime < endTime
        && priceTable[ep][i].endTime > startTime) {
      if (priceTable[ep][i].issuerEventId < issuerEventId) {
        if (priceTable[ep][i].active && now < startTime) {
          priceTable[ep][i].endTime = startTime;
          priceTable[ep][i].durationInMinutes = ZCL_PRICE_CLUSTER_DURATION_UNTIL_CHANGED;
        } else {
          if (priceTable[ep][i].active) {
            priceTable[ep][i].active = false;
            emberAfPluginPriceClientPriceExpiredCallback(&priceTable[ep][i]);
          }
          initPrice(&priceTable[ep][i]);
        }
      } else {
        status = EMBER_ZCL_STATUS_FAILURE;
        goto kickout;
      }
    }

    // Along the way, search for an empty slot for this new price and find the
    // price in the table with the latest start time.  If there are no empty
    // slots, we will either have to drop this price or the last one, depending
    // on the start times.
    if (price == NULL) {
      if (!priceTable[ep][i].valid) {
        price = &priceTable[ep][i];
      } else if (last->startTime < priceTable[ep][i].startTime) {
        last = &priceTable[ep][i];
      }
    }
  }

  // If there were no empty slots and this price starts after all of the other
  // prices in the table, drop this price.  Otherwise, drop the price with the
  // latest start time and replace it with this one.
  if (price == NULL) {
    if (last->startTime < startTime) {
      status = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
      goto kickout;
    } else {
      price = last;
    }
  }

  price->valid                             = true;
  price->active                            = false;
  price->clientEndpoint                    = endpoint;
  price->providerId                        = providerId;
  emberAfCopyString(price->rateLabel, rateLabel, ZCL_PRICE_CLUSTER_MAXIMUM_RATE_LABEL_LENGTH);
  price->issuerEventId                     = issuerEventId;
  price->currentTime                       = currentTime;
  price->unitOfMeasure                     = unitOfMeasure;
  price->currency                          = currency;
  price->priceTrailingDigitAndPriceTier    = priceTrailingDigitAndPriceTier;
  price->numberOfPriceTiersAndRegisterTier = numberOfPriceTiersAndRegisterTier;
  price->startTime                         = startTime;
  price->endTime                           = endTime;
  price->durationInMinutes                 = durationInMinutes;
  price->price                             = prc;
  price->priceRatio                        = priceRatio;
  price->generationPrice                   = generationPrice;
  price->generationPriceRatio              = generationPriceRatio;
  price->alternateCostDelivered            = alternateCostDelivered;
  price->alternateCostUnit                 = alternateCostUnit;
  price->alternateCostTrailingDigit        = alternateCostTrailingDigit;
  price->numberOfBlockThresholds           = numberOfBlockThresholds;
  price->priceControl                      = priceControl;

  // Now that we have saved the price in our table, we may have to reschedule
  // our tick to activate or expire prices.
  scheduleTick(endpoint);

  // If the acknowledgement is required, send it immediately.  Otherwise, a
  // default response is sufficient.
  if (priceControl & ZCL_PRICE_CLUSTER_PRICE_ACKNOWLEDGEMENT_MASK) {
    emberAfFillCommandPriceClusterPriceAcknowledgement(providerId,
                                                       issuerEventId,
                                                       now,
                                                       priceControl);
    emberAfSendResponse();
    return true;
  } else {
    status = EMBER_ZCL_STATUS_SUCCESS;
  }

  kickout:
  emberAfSendImmediateDefaultResponse(status);
  return true;
}

bool emberAfPriceClusterPublishBlockPeriodCallback(uint32_t providerId,
                                                   uint32_t issuerEventId,
                                                   uint32_t blockPeriodStartTime,
                                                   uint32_t blockPeriodDuration,    // int24u
                                                   uint8_t  blockPeriodControl,
                                                   uint8_t  blockPeriodDurationType,
                                                   uint8_t  tariffType,
                                                   uint8_t  tariffResolutionPeriod)
{
  uint8_t i;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return false;
  }

  emberAfPriceClusterPrintln("RX: PublishBlockPeriod, 0x%4x, 0x%4x, 0x%4x, 0x%4X, 0x%X, 0x%X, 0x%X, 0x%X",
                             providerId,
                             issuerEventId,
                             blockPeriodStartTime,
                             blockPeriodDuration, // int24u
                             blockPeriodControl,
                             blockPeriodDurationType,
                             tariffType,
                             tariffResolutionPeriod);

  if ( blockPeriodStartTime == 0 ) {
    blockPeriodStartTime = emberAfGetCurrentTime();
  }

  // Find the entry to update
  // We will update any invalid entry, or the oldest event ID.
  i = emAfPriceCommonGetMatchingOrUnusedIndex(priceInfo.blockPeriodTable.commonInfos[ep],
                                              EMBER_AF_PLUGIN_PRICE_CLIENT_BLOCK_PERIOD_TABLE_SIZE,
                                              providerId, issuerEventId);
  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_BLOCK_PERIOD_TABLE_SIZE ) {
    priceInfo.blockPeriodTable.commonInfos[ep][i].valid = true;
    priceInfo.blockPeriodTable.commonInfos[ep][i].providerId = providerId;
    priceInfo.blockPeriodTable.commonInfos[ep][i].issuerEventId = issuerEventId;
    priceInfo.blockPeriodTable.commonInfos[ep][i].startTime = emberAfPluginPriceCommonClusterGetAdjustedStartTime(blockPeriodStartTime,
                                                                                                                  blockPeriodDurationType);
    priceInfo.blockPeriodTable.commonInfos[ep][i].durationSec =
      emberAfPluginPriceCommonClusterConvertDurationToSeconds(blockPeriodStartTime,
                                                              blockPeriodDuration,
                                                              blockPeriodDurationType);
    priceInfo.blockPeriodTable.blockPeriod[ep][i].blockPeriodStartTime = blockPeriodStartTime;
    priceInfo.blockPeriodTable.blockPeriod[ep][i].blockPeriodDuration = blockPeriodDuration;
    priceInfo.blockPeriodTable.blockPeriod[ep][i].blockPeriodControl = blockPeriodControl;
    priceInfo.blockPeriodTable.blockPeriod[ep][i].blockPeriodDurationType = blockPeriodDurationType;
    priceInfo.blockPeriodTable.blockPeriod[ep][i].tariffType = tariffType;
    priceInfo.blockPeriodTable.blockPeriod[ep][i].tariffResolutionPeriod = tariffResolutionPeriod;
  }

  return true;
}

bool emberAfPriceClusterPublishConversionFactorCallback(uint32_t issuerEventId,
                                                        uint32_t startTime,
                                                        uint32_t conversionFactor,
                                                        uint8_t conversionFactorTrailingDigit)
{
  uint8_t  i;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);

  emberAfPriceClusterPrintln("RX: PublishConversionFactor 0x%4X, 0x%4X, 0x%4X, 0x%X",
                             issuerEventId,
                             startTime,
                             conversionFactor,
                             conversionFactorTrailingDigit);

  if (ep == 0xFF) {
    return false;
  }

  if ( startTime == 0 ) {
    startTime = emberAfGetCurrentTime();
  }

  i = emAfPriceCommonGetMatchingOrUnusedIndex(priceInfo.conversionFactorTable.commonInfos[ep],
                                              EMBER_AF_PLUGIN_PRICE_CLIENT_CONVERSION_FACTOR_TABLE_SIZE,
                                              UNSPECIFIED_PROVIDER_ID,
                                              issuerEventId);
  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_CONVERSION_FACTOR_TABLE_SIZE ) {
    // Only update the entry if the new eventID is greater than this one.
    priceInfo.conversionFactorTable.commonInfos[ep][i].valid = true;
    priceInfo.conversionFactorTable.commonInfos[ep][i].providerId = UNSPECIFIED_PROVIDER_ID;
    priceInfo.conversionFactorTable.commonInfos[ep][i].issuerEventId = issuerEventId;
    priceInfo.conversionFactorTable.commonInfos[ep][i].startTime = startTime;
    priceInfo.conversionFactorTable.commonInfos[ep][i].durationSec = UNSPECIFIED_DURATION;
    priceInfo.conversionFactorTable.conversionFactor[ep][i].conversionFactor = conversionFactor;
    priceInfo.conversionFactorTable.conversionFactor[ep][i].conversionFactorTrailingDigit = conversionFactorTrailingDigit;
  }
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfPriceClusterPublishCalorificValueCallback(uint32_t issuerEventId,
                                                      uint32_t startTime,
                                                      uint32_t calorificValue,
                                                      uint8_t calorificValueUnit,
                                                      uint8_t calorificValueTrailingDigit)
{
  uint8_t  i;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);

  emberAfPriceClusterPrintln("RX: PublishCalorificValue 0x%4X, 0x%4X, 0x%4X, 0x%X, 0x%X",
                             issuerEventId,
                             startTime,
                             calorificValue,
                             calorificValueUnit,
                             calorificValueTrailingDigit);

  if (ep == 0xFF) {
    return false;
  }

  if ( startTime == 0 ) {
    startTime = emberAfGetCurrentTime();
  }

  i = emAfPriceCommonGetMatchingOrUnusedIndex(priceInfo.calorificValueTable.commonInfos[ep], EMBER_AF_PLUGIN_PRICE_CLIENT_CALORIFIC_VALUE_TABLE_SIZE,
                                              UNSPECIFIED_PROVIDER_ID, issuerEventId);

  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_CALORIFIC_VALUE_TABLE_SIZE ) {
    // Only update the entry if the new eventID is greater than this one.
    priceInfo.calorificValueTable.commonInfos[ep][i].valid = true;
    priceInfo.calorificValueTable.commonInfos[ep][i].issuerEventId = issuerEventId;
    priceInfo.calorificValueTable.commonInfos[ep][i].startTime = startTime;
    priceInfo.calorificValueTable.calorificValue[ep][i].calorificValue = calorificValue;
    priceInfo.calorificValueTable.calorificValue[ep][i].calorificValueUnit = calorificValueUnit;
    priceInfo.calorificValueTable.calorificValue[ep][i].calorificValueTrailingDigit = calorificValueTrailingDigit;
  }
  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfPriceClusterPublishTariffInformationCallback(uint32_t providerId,
                                                         uint32_t issuerEventId,
                                                         uint32_t issuerTariffId,
                                                         uint32_t startTime,
                                                         uint8_t tariffTypeChargingScheme,
                                                         uint8_t* tariffLabel,
                                                         uint8_t numberOfPriceTiersInUse,
                                                         uint8_t numberOfBlockThresholdsInUse,
                                                         uint8_t unitOfMeasure,
                                                         uint16_t currency,
                                                         uint8_t priceTrailingDigit,
                                                         uint32_t standingCharge,
                                                         uint8_t tierBlockMode,
                                                         uint32_t blockThresholdMultiplier,
                                                         uint32_t blockThresholdDivisor)
{
  emberAfPriceClusterPrint("RX: PublishTariffInformation 0x%4x, 0x%4x, 0x%4x, 0x%4x, 0x%x, \"",
                           providerId,
                           issuerEventId,
                           issuerTariffId,
                           startTime,
                           tariffTypeChargingScheme);

  emberAfPriceClusterPrintString(tariffLabel);
  emberAfPriceClusterPrint("\"");
  emberAfPriceClusterPrint(", 0x%x, 0x%x, 0x%x, 0x%2x, 0x%x",
                           numberOfPriceTiersInUse,
                           numberOfBlockThresholdsInUse,
                           unitOfMeasure,
                           currency,
                           priceTrailingDigit);
  emberAfPriceClusterPrintln(", 0x%4x, 0x%x, 0x%4x, 0x%4x",
                             standingCharge,
                             tierBlockMode,
                             blockThresholdMultiplier,
                             blockThresholdDivisor);
  emberAfPriceClusterFlush();

  if (lastSeenIssuerEventId >= issuerEventId) {
    // reject old command.
    emberAfPriceClusterPrintln("Rejected command due to old issuer event id (0x%4X)!",
                               issuerEventId);
  } else {
    // accept command.
    // optional attributes are not updated for now.
    lastSeenIssuerEventId = issuerEventId;
  }

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#define BLOCK_THRESHOLD_SUB_PAYLOAD_ALL_TOU_TIERS  (1 << 0)

bool emberAfPriceClusterPublishBlockThresholdsCallback(uint32_t providerId,
                                                       uint32_t issuerEventId,
                                                       uint32_t startTime,
                                                       uint32_t issuerTariffId,
                                                       uint8_t commandIndex,
                                                       uint8_t numberOfCommands,
                                                       uint8_t subPayloadControl,
                                                       uint8_t* payload)
{
  uint8_t tierNumber;
  uint8_t numThresholds;
  uint8_t i;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return false;
  }
  emberAfPriceClusterPrintln("RX: PublishBlockThresholds 0x%4x, 0x%4x, 0x%4x, 0x%4x, 0x%x, 0x%x, 0x%x",
                             providerId,
                             issuerEventId,
                             startTime,
                             issuerTariffId,
                             commandIndex,
                             numberOfCommands,
                             subPayloadControl);

  i = emAfPriceCommonGetMatchingOrUnusedIndex(priceInfo.blockThresholdTable.commonInfos[ep],
                                              numberOfCommands, providerId, issuerEventId);

  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_BLOCK_THRESHOLD_TABLE_SIZE ) {
    // Only update the entry if the new eventID is greater than this one.
    tierNumber = payload[0] >> 4;
    numThresholds = payload[0] & 0x0F;
    if ( (numThresholds >= EMBER_AF_PLUGIN_PRICE_CLIENT_MAX_NUMBER_BLOCK_THRESHOLDS)
         || (tierNumber >= EMBER_AF_PLUGIN_PRICE_CLIENT_MAX_NUMBER_TIERS) ) {
      // Out of range
      goto kickout;
    }
    priceInfo.blockThresholdTable.commonInfos[ep][i].valid = true;
    priceInfo.blockThresholdTable.commonInfos[ep][i].providerId = providerId;
    priceInfo.blockThresholdTable.commonInfos[ep][i].issuerEventId = issuerEventId;
    priceInfo.blockThresholdTable.commonInfos[ep][i].startTime = startTime;
    priceInfo.blockThresholdTable.commonInfos[ep][i].durationSec = UNSPECIFIED_DURATION;
    priceInfo.blockThresholdTable.blockThreshold[ep][i].issuerTariffId = issuerTariffId;
    priceInfo.blockThresholdTable.blockThreshold[ep][i].subPayloadControl = subPayloadControl;
    if ( subPayloadControl & BLOCK_THRESHOLD_SUB_PAYLOAD_ALL_TOU_TIERS ) {
      priceInfo.blockThresholdTable.blockThreshold[ep][i].tierNumberOfBlockThresholds[0] = payload[0];
      MEMCOPY(priceInfo.blockThresholdTable.blockThreshold[ep][i].blockThreshold[0], &payload[1], (numThresholds * 6) );
    } else {
      // TODO:   Note that multiple tier/NumberOfBlockThresholds fields could be present in this case.
      // eg. The payload could specify 5 blocks for tier 1, 2 blocks for tier 2, etc.
      // However, we don't know how many bytes are in "payload", so we can't read them all.  =(
      // For now, read the first tiers worth of data.
      //x = 0;

      priceInfo.blockThresholdTable.blockThreshold[ep][i].tierNumberOfBlockThresholds[tierNumber] = payload[0];
      MEMCOPY(priceInfo.blockThresholdTable.blockThreshold[ep][i].blockThreshold[tierNumber], &payload[1], (numThresholds * 6) );
    }
  }

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  kickout:
  return true;
}

bool emberAfPriceClusterPublishCO2ValueCallback(uint32_t providerId,
                                                uint32_t issuerEventId,
                                                uint32_t startTime,
                                                uint8_t tariffType,
                                                uint32_t cO2Value,
                                                uint8_t cO2ValueUnit,
                                                uint8_t cO2ValueTrailingDigit)
{
  uint8_t  i;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return false;
  }

  emberAfPriceClusterPrintln("RX: Publish CO2 Value");

  if ( startTime == 0 ) {
    startTime = emberAfGetCurrentTime();
  }
  i = emAfPriceCommonGetMatchingOrUnusedIndex(priceInfo.co2ValueTable.commonInfos[ep], EMBER_AF_PLUGIN_PRICE_CLIENT_CO2_TABLE_SIZE,
                                              providerId, issuerEventId);

  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_CO2_TABLE_SIZE ) {
    // Do NOT overwrite data if the entry is valid and the new
    // data has a smaller (or equal) event ID.
    priceInfo.co2ValueTable.commonInfos[ep][i].providerId = providerId;
    priceInfo.co2ValueTable.commonInfos[ep][i].issuerEventId = issuerEventId;
    priceInfo.co2ValueTable.commonInfos[ep][i].startTime = startTime;
    priceInfo.co2ValueTable.co2Value[ep][i].tariffType = tariffType;
    priceInfo.co2ValueTable.commonInfos[ep][i].valid = true;
    priceInfo.co2ValueTable.co2Value[ep][i].cO2Value = cO2Value;
    priceInfo.co2ValueTable.co2Value[ep][i].cO2ValueUnit = cO2ValueUnit;
    priceInfo.co2ValueTable.co2Value[ep][i].cO2ValueTrailingDigit = cO2ValueTrailingDigit;
  }
  return true;
}

bool emberAfPriceClusterPublishTierLabelsCallback(uint32_t providerId,
                                                  uint32_t issuerEventId,
                                                  uint32_t issuerTariffId,
                                                  uint8_t commandIndex,
                                                  uint8_t numberOfCommands,
                                                  uint8_t numberOfLabels,
                                                  uint8_t* tierLabelsPayload)
{
  uint8_t endpoint = emberAfCurrentEndpoint();
  emberAfPriceClusterPrintln("RX: PublishTierLabels");
  emAfPriceClientAddTierLabel(endpoint, providerId, issuerEventId, issuerTariffId,
                              numberOfLabels, tierLabelsPayload);

  return true;
}

bool emberAfPriceClusterPublishBillingPeriodCallback(uint32_t providerId,
                                                     uint32_t issuerEventId,
                                                     uint32_t billingPeriodStartTime,
                                                     uint32_t billingPeriodDuration,
                                                     uint8_t billingPeriodDurationType,
                                                     uint8_t tariffType)
{
  uint8_t  i;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return false;
  }

  emberAfPriceClusterPrintln("RX: PublishBillingPeriod, issuerId=%d,  start=0x%4x", issuerEventId, billingPeriodStartTime);

  if ( billingPeriodStartTime == 0 ) {
    billingPeriodStartTime = emberAfGetCurrentTime();
  }

  i = emAfPriceCommonGetMatchingOrUnusedIndex(priceInfo.billingPeriodTable.commonInfos[ep],
                                              EMBER_AF_PLUGIN_PRICE_CLIENT_BILLING_PERIOD_TABLE_SIZE,
                                              providerId, issuerEventId);

  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_BILLING_PERIOD_TABLE_SIZE ) {
    if ( billingPeriodStartTime == CANCELLATION_START_TIME ) {
      priceInfo.billingPeriodTable.commonInfos[ep][i].valid = false;
      emberAfPriceClusterPrintln("Canceling eventId=%d", issuerEventId);
    } else {
      priceInfo.billingPeriodTable.commonInfos[ep][i].valid = true;
      priceInfo.billingPeriodTable.commonInfos[ep][i].providerId = providerId;
      priceInfo.billingPeriodTable.commonInfos[ep][i].issuerEventId = issuerEventId;
      priceInfo.billingPeriodTable.commonInfos[ep][i].startTime =
        emberAfPluginPriceCommonClusterGetAdjustedStartTime(billingPeriodStartTime,
                                                            billingPeriodDurationType);
      priceInfo.billingPeriodTable.commonInfos[ep][i].durationSec =
        emberAfPluginPriceCommonClusterConvertDurationToSeconds(billingPeriodStartTime,
                                                                billingPeriodDuration,
                                                                billingPeriodDurationType);

      priceInfo.billingPeriodTable.billingPeriod[ep][i].billingPeriodStartTime = billingPeriodStartTime;
      priceInfo.billingPeriodTable.billingPeriod[ep][i].billingPeriodDuration = billingPeriodDuration;
      priceInfo.billingPeriodTable.billingPeriod[ep][i].billingPeriodDurationType = billingPeriodDurationType;
      priceInfo.billingPeriodTable.billingPeriod[ep][i].tariffType = tariffType;
    }
  }

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfPriceClusterPublishCppEventCallback(uint32_t providerId,
                                                uint32_t issuerEventId,
                                                uint32_t startTime,
                                                uint16_t durationInMinutes,
                                                uint8_t tariffType,
                                                uint8_t cppPriceTier,
                                                uint8_t cppAuth)
{
  uint8_t responseCppAuth;
  EmberAfStatus status;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return false;
  }

  emberAfPriceClusterPrintln("RX: PublishCppEvent:");
  if ( (cppPriceTier > 1) || (cppAuth >= EMBER_AF_PLUGIN_PRICE_CPP_AUTH_RESERVED) ) {
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_INVALID_FIELD);
  } else {
    if ( startTime == 0x00000000 ) {
      startTime = emberAfGetCurrentTime();
    } else if ( startTime == CANCELLATION_START_TIME ) {
      // Cancellation attempt.
      if ( (priceInfo.cppEventTable.commonInfos[ep].providerId == providerId)
           && (priceInfo.cppEventTable.commonInfos[ep].issuerEventId == issuerEventId) ) {
        emberAfPriceClusterPrintln("CPP Event Cancelled");
        priceInfo.cppEventTable.commonInfos[ep].valid = false;
        status = EMBER_ZCL_STATUS_SUCCESS;
      } else {
        status = EMBER_ZCL_STATUS_NOT_FOUND;
      }
      emberAfSendImmediateDefaultResponse(status);
      goto kickout;
    }

    // If the cppAuth is PENDING, the client may decide to accept or reject the CPP event.
    if ( cppAuth == EMBER_AF_PLUGIN_PRICE_CPP_AUTH_PENDING ) {
      responseCppAuth = emberAfPluginPriceClientPendingCppEventCallback(cppAuth);
      emberAfPriceClusterPrintln("  Pending CPP Event, status=%d", responseCppAuth);
      emberAfFillCommandPriceClusterCppEventResponse(issuerEventId, responseCppAuth);
      emberAfSendResponse();
    } else {
      if ( (cppAuth == EMBER_AF_PLUGIN_PRICE_CPP_AUTH_ACCEPTED)
           || (cppAuth == EMBER_AF_PLUGIN_PRICE_CPP_AUTH_FORCED) ) {
        // Apply the CPP
        emberAfPriceClusterPrintln("CPP Event Accepted");
        priceInfo.cppEventTable.commonInfos[ep].providerId = providerId;
        priceInfo.cppEventTable.commonInfos[ep].issuerEventId = issuerEventId;
        priceInfo.cppEventTable.commonInfos[ep].startTime = startTime;
        priceInfo.cppEventTable.commonInfos[ep].durationSec = (durationInMinutes * 60);
        priceInfo.cppEventTable.commonInfos[ep].valid = true;
        priceInfo.cppEventTable.cppEvent[ep].durationInMinutes = durationInMinutes;
        priceInfo.cppEventTable.cppEvent[ep].tariffType = tariffType;
        priceInfo.cppEventTable.cppEvent[ep].cppPriceTier = cppPriceTier;
        priceInfo.cppEventTable.cppEvent[ep].cppAuth = cppAuth;
      } else if ( cppAuth == EMBER_AF_PLUGIN_PRICE_CPP_AUTH_REJECTED ) {
        emberAfPriceClusterPrintln("CPP Event Rejected");
      }
      emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
    }
  }
  kickout:
  return true;
}

bool emberAfPriceClusterPublishCreditPaymentCallback(uint32_t providerId, uint32_t issuerEventId,
                                                     uint32_t creditPaymentDueDate, uint32_t creditPaymentOverDueAmount,
                                                     uint8_t creditPaymentStatus, uint32_t creditPayment,
                                                     uint32_t creditPaymentDate, uint8_t* creditPaymentRef)
{
  uint8_t i;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return false;
  }

  i = emAfPriceCommonGetMatchingOrUnusedIndex(priceInfo.creditPaymentTable.commonInfos[ep],
                                              EMBER_AF_PLUGIN_PRICE_CLIENT_CREDIT_PAYMENT_TABLE_SIZE,
                                              providerId, issuerEventId);

  emberAfPriceClusterPrintln("RX: PublishCreditPayment [%d], issuerEventId=%d", i, issuerEventId);
  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_CREDIT_PAYMENT_TABLE_SIZE ) {
    priceInfo.creditPaymentTable.commonInfos[ep][i].valid = true;
    priceInfo.creditPaymentTable.commonInfos[ep][i].providerId = providerId;
    priceInfo.creditPaymentTable.commonInfos[ep][i].issuerEventId = issuerEventId;
    priceInfo.creditPaymentTable.creditPayment[ep][i].creditPaymentDueDate = creditPaymentDueDate;
    priceInfo.creditPaymentTable.creditPayment[ep][i].creditPaymentOverDueAmount = creditPaymentOverDueAmount;
    priceInfo.creditPaymentTable.creditPayment[ep][i].creditPaymentStatus = creditPaymentStatus;
    priceInfo.creditPaymentTable.creditPayment[ep][i].creditPayment = creditPayment;
    priceInfo.creditPaymentTable.creditPayment[ep][i].creditPaymentDate = creditPaymentDate;
    emberAfCopyString(priceInfo.creditPaymentTable.creditPayment[ep][i].creditPaymentRef, creditPaymentRef, EMBER_AF_PLUGIN_PRICE_CLUSTER_MAX_CREDIT_PAYMENT_REF_LENGTH);
  }

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

bool emberAfPriceClusterPublishCurrencyConversionCallback(uint32_t providerId,
                                                          uint32_t issuerEventId,
                                                          uint32_t startTime,
                                                          uint16_t oldCurrency,
                                                          uint16_t newCurrency,
                                                          uint32_t conversionFactor,
                                                          uint8_t conversionFactorTrailingDigit,
                                                          uint32_t currencyChangeControlFlags)
{
  uint8_t i;
  uint8_t endpoint = emberAfCurrentEndpoint();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return false;
  }

  emberAfPriceClusterPrintln("RX: Currency Conversion, start=%d, pid=%d, eid=%d", startTime, providerId, issuerEventId);

  if ( startTime == 0 ) {
    startTime = emberAfGetCurrentTime();
  }

  i = emAfPriceCommonGetMatchingOrUnusedIndex(priceInfo.currencyConversionTable.commonInfos[ep],
                                              EMBER_AF_PLUGIN_PRICE_CLIENT_CURRENCY_CONVERSION_TABLE_SIZE,
                                              providerId, issuerEventId);

  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_CURRENCY_CONVERSION_TABLE_SIZE ) {
    if ( startTime == CANCELLATION_START_TIME ) {
      priceInfo.currencyConversionTable.commonInfos[ep][i].valid = false;
    } else {
      priceInfo.currencyConversionTable.commonInfos[ep][i].valid = true;
      priceInfo.currencyConversionTable.commonInfos[ep][i].providerId = providerId;
      priceInfo.currencyConversionTable.commonInfos[ep][i].issuerEventId = issuerEventId;
      priceInfo.currencyConversionTable.commonInfos[ep][i].startTime = startTime;
      priceInfo.currencyConversionTable.currencyConversion[ep][i].newCurrency = newCurrency;
      priceInfo.currencyConversionTable.currencyConversion[ep][i].conversionFactor = conversionFactor;
      priceInfo.currencyConversionTable.currencyConversion[ep][i].conversionFactorTrailingDigit = conversionFactorTrailingDigit;
      priceInfo.currencyConversionTable.currencyConversion[ep][i].currencyChangeControlFlags = currencyChangeControlFlags;
    }
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  }
  return true;
}

bool emberAfPriceClusterCancelTariffCallback(uint32_t providerId, uint32_t issuerTariffId, uint8_t tariffType)
{
  emberAfPriceClusterPrintln("RX: Cancel Tariff, providerId=%d, issuerTariffId=%d, tariffType=%d",
                             providerId, issuerTariffId, tariffType);

  emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_SUCCESS);
  return true;
}

#endif // UC_BUILD

void emAfPluginPriceClientPrintInfo(uint8_t endpoint)
{
#if ((defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_PRICE_CLUSTER)) || defined(UC_BUILD))
  uint8_t i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);

  if (ep == 0xFF) {
    return;
  }

  for (i = 0; i < EMBER_AF_PLUGIN_PRICE_CLIENT_TABLE_SIZE; i++) {
    emberAfPriceClusterFlush();
    emberAfPriceClusterPrintln("= Price %d =", i);
    printPrice(&priceTable[ep][i]);
    emberAfPriceClusterFlush();
  }
#endif //defined(EMBER_AF_PRINT_ENABLE) && defined(EMBER_AF_PRINT_PRICE_CLUSTER)
}

void emAfPluginPriceClientPrintByEventId(uint8_t endpoint, uint32_t issuerEventId)
{
  uint8_t i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  for ( i = 0; i < EMBER_AF_PLUGIN_PRICE_CLIENT_TABLE_SIZE; i++ ) {
    if ( priceTable[ep][i].issuerEventId == issuerEventId ) {
      emberAfPriceClusterPrintln("Matching Price [%d]", i);
      printPrice(&priceTable[ep][i]);
      break;
    }
  }
  if ( i >= EMBER_AF_PLUGIN_PRICE_CLIENT_TABLE_SIZE ) {
    emberAfPriceClusterPrintln("Error: Event ID %d not in price table", issuerEventId);
  }
}

static void initPrice(EmberAfPluginPriceClientPrice *price)
{
  price->valid                             = false;
  price->active                            = false;
  price->clientEndpoint                    = 0xFF;
  price->providerId                        = 0x00000000UL;
  price->rateLabel[0]                      = 0;
  price->issuerEventId                     = 0x00000000UL;
  price->currentTime                       = 0x00000000UL;
  price->unitOfMeasure                     = 0x00;
  price->currency                          = 0x0000;
  price->priceTrailingDigitAndPriceTier    = 0x00;
  price->numberOfPriceTiersAndRegisterTier = 0x00;
  price->startTime                         = 0x00000000UL;
  price->endTime                           = 0x00000000UL;
  price->durationInMinutes                 = 0x0000;
  price->price                             = 0x00000000UL;
  price->priceRatio                        = ZCL_PRICE_CLUSTER_PRICE_RATIO_NOT_USED;
  price->generationPrice                   = ZCL_PRICE_CLUSTER_GENERATION_PRICE_NOT_USED;
  price->generationPriceRatio              = ZCL_PRICE_CLUSTER_GENERATION_PRICE_RATIO_NOT_USED;
  price->alternateCostDelivered            = ZCL_PRICE_CLUSTER_ALTERNATE_COST_DELIVERED_NOT_USED;
  price->alternateCostUnit                 = ZCL_PRICE_CLUSTER_ALTERNATE_COST_UNIT_NOT_USED;
  price->alternateCostTrailingDigit        = ZCL_PRICE_CLUSTER_ALTERNATE_COST_TRAILING_DIGIT_NOT_USED;
  price->numberOfBlockThresholds           = ZCL_PRICE_CLUSTER_NUMBER_OF_BLOCK_THRESHOLDS_NOT_USED;
  price->priceControl                      = ZCL_PRICE_CLUSTER_PRICE_CONTROL_NOT_USED;
}

static void printPrice(EmberAfPluginPriceClientPrice *price)
{
  emberAfPriceClusterPrintln("    vld: %p", (price->valid ? "YES" : "NO"));
  emberAfPriceClusterPrintln("    act: %p", (price->active ? "YES" : "NO"));
  emberAfPriceClusterPrintln("    pid: 0x%4x", price->providerId);
  emberAfPriceClusterFlush();
  emberAfPriceClusterPrint("     rl: \"");
  emberAfPriceClusterPrintString(price->rateLabel);
  emberAfPriceClusterPrintln("\"");
  emberAfPriceClusterPrintln("   ieid: 0x%4x", price->issuerEventId);
  emberAfPriceClusterPrintln("     ct: 0x%4x", price->currentTime);
  emberAfPriceClusterPrintln("    uom: 0x%x", price->unitOfMeasure);
  emberAfPriceClusterPrintln("      c: 0x%2x", price->currency);
  emberAfPriceClusterPrintln(" ptdapt: 0x%x", price->priceTrailingDigitAndPriceTier);
  emberAfPriceClusterPrintln("noptart: 0x%x", price->numberOfPriceTiersAndRegisterTier);
  emberAfPriceClusterFlush();
  emberAfPriceClusterPrintln("     st: 0x%4x", price->startTime);
  emberAfPriceClusterPrintln("     et: 0x%4x", price->endTime);
  emberAfPriceClusterPrintln("    dim: 0x%2x", price->durationInMinutes);
  emberAfPriceClusterPrintln("      p: 0x%4x", price->price);
  emberAfPriceClusterPrintln("     pr: 0x%x", price->priceRatio);
  emberAfPriceClusterFlush();
  emberAfPriceClusterPrintln("     gp: 0x%4x", price->generationPrice);
  emberAfPriceClusterPrintln("    gpr: 0x%x", price->generationPriceRatio);
  emberAfPriceClusterPrintln("    acd: 0x%4x", price->alternateCostDelivered);
  emberAfPriceClusterPrintln("    acu: 0x%x", price->alternateCostUnit);
  emberAfPriceClusterPrintln("   actd: 0x%x", price->alternateCostTrailingDigit);
  emberAfPriceClusterPrintln("   nobt: 0x%x", price->numberOfBlockThresholds);
  emberAfPriceClusterPrintln("     pc: 0x%x", price->priceControl);
}

static void scheduleTick(uint8_t endpoint)
{
  uint32_t next = ZCL_PRICE_CLUSTER_END_TIME_NEVER;
  uint32_t now = emberAfGetCurrentTime();
  bool active = false;
  uint8_t i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint,
                                                     ZCL_PRICE_CLUSTER_ID);

  if (ep == 0xFF) {
    return;
  }

  for (i = 0; i < EMBER_AF_PLUGIN_PRICE_CLIENT_TABLE_SIZE; i++) {
    if (!priceTable[ep][i].valid) {
      continue;
    }

    // Remove old prices from the table.  This may result in the active price
    // being expired, which requires notifying the application.
    if (priceTable[ep][i].endTime <= now) {
      if (priceTable[ep][i].active) {
        priceTable[ep][i].active = false;
        emberAfPluginPriceClientPriceExpiredCallback(&priceTable[ep][i]);
      }
      initPrice(&priceTable[ep][i]);
      continue;
    }

    // If we don't have a price that should be active right now, we will need to
    // schedule the tick to wake us up when the next active price should start,
    // so keep track of the price with the start time soonest after the current
    // time.
    if (!active && priceTable[ep][i].startTime < next) {
      next = priceTable[ep][i].startTime;
    }

    // If we have a price that should be active now, a tick is scheduled for the
    // time remaining in the duration to wake us up and expire the price.  If
    // the price is transitioning from inactive to active for the first time, we
    // also need to notify the application the application.
    if (priceTable[ep][i].startTime <= now) {
      if (!priceTable[ep][i].active) {
        priceTable[ep][i].active = true;
        emberAfPluginPriceClientPriceStartedCallback(&priceTable[ep][i]);
      }
      active = true;
      next = priceTable[ep][i].endTime;
    }
  }

  // We need to wake up again to activate a new price or expire the current
  // price.  Otherwise, we don't have to do anything until we receive a new
  // price from the server.
  if (next != ZCL_PRICE_CLUSTER_END_TIME_NEVER) {
    slxu_zigbee_zcl_schedule_client_tick(endpoint,
                                         ZCL_PRICE_CLUSTER_ID,
                                         (next - now) * MILLISECOND_TICKS_PER_SECOND);
  } else {
    slxu_zigbee_zcl_deactivate_client_tick(endpoint, ZCL_PRICE_CLUSTER_ID);
  }
}

// Get the index of the EmberAfPriceClientCommonInfo structure whose event ID matches.
static uint8_t emAfPriceCommonGetMatchingIndex(EmberAfPriceClientCommonInfo *pcommon, uint8_t numElements, uint32_t issuerEventId)
{
  uint8_t i;
  for ( i = 0; i < numElements; i++ ) {
    if ( pcommon[i].valid && (pcommon[i].issuerEventId == issuerEventId) ) {
      break;
    }
  }
  return i;
}

static uint8_t emAfPriceCommonGetActiveIndex(EmberAfPriceClientCommonInfo *pcommon, uint8_t numElements)
{
  uint8_t i;
  uint32_t largestEventId = 0;
  uint8_t  largestEventIdIndex = 0xFF;
  uint32_t timeNow = emberAfGetCurrentTime();

  for ( i = 0; i < numElements; i++ ) {
    if ( pcommon[i].valid
         && ((pcommon[i].startTime <= timeNow)
             || (pcommon[i].startTime == 0))
         && (pcommon[i].issuerEventId > largestEventId) ) {
      //(PriceClientCo2Table[i].startTime > nearestTime) ){
      // Found entry that is closer to timeNow
      //nearestTime = PriceClientCo2Table[i].startTime;
      largestEventId = pcommon[i].issuerEventId;
      largestEventIdIndex = i;
    }
  }
  return largestEventIdIndex;
}

// Parses the EmberAfPriceClientCommonInfo structure to find a matching, unused, or oldest (smallest eventId)
// element that can be overwritten with new data.
static uint8_t emAfPriceCommonGetMatchingOrUnusedIndex(EmberAfPriceClientCommonInfo *pcommon, uint8_t numElements,
                                                       uint32_t newProviderId, uint32_t newIssuerEventId)
{
  uint32_t smallestEventId = 0xFFFFFFFF;
  uint8_t  smallestEventIdIndex = 0xFF;
  uint8_t  i;

  for ( i = 0; i < numElements; i++ ) {
    if ( !pcommon[i].valid ) {
      // Use the first invalid entry unless a matching entry is found.
      if ( smallestEventId > 0 ) {
        smallestEventId = 0;
        smallestEventIdIndex = i;
      }
    } else {
      if ( (pcommon[i].providerId == newProviderId) && (pcommon[i].issuerEventId == newIssuerEventId) ) {
        // Match found
        smallestEventIdIndex = i;
        smallestEventId = pcommon[i].issuerEventId;
        break;
      } else if ( pcommon[i].issuerEventId < smallestEventId ) {
        smallestEventId = pcommon[i].issuerEventId;
        smallestEventIdIndex = i;
      }
    }
  }
  // Do quick sanity check here to validate the index.
  // Expect the indexed entry to either:
  //  1. Be invalid, or
  //  2. Have an issuerEventId <= newIssuerEventId
  if ( (smallestEventIdIndex < numElements)
       && pcommon[smallestEventIdIndex].valid
       && (smallestEventId > newIssuerEventId) ) {
    // FAIL above conditions - return invalid index
    smallestEventIdIndex = 0xFF;
  }
  return smallestEventIdIndex;
}

static void emberAfPriceInitBlockPeriod(uint8_t endpoint)
{
  uint8_t i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }
  for ( i = 0; i < EMBER_AF_PLUGIN_PRICE_CLIENT_BLOCK_PERIOD_TABLE_SIZE; i++ ) {
    priceInfo.blockPeriodTable.commonInfos[ep][i].valid = false;
  }
}

uint8_t emAfPriceGetBlockPeriodTableIndexByEventId(uint8_t endpoint, uint32_t issuerEventId)
{
  uint8_t i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return 0xFF;
  }
  for ( i = 0; i < EMBER_AF_PLUGIN_PRICE_CLIENT_BLOCK_PERIOD_TABLE_SIZE; i++ ) {
    if ( priceInfo.blockPeriodTable.commonInfos[ep][i].valid
         && (priceInfo.blockPeriodTable.commonInfos[ep][i].issuerEventId == issuerEventId) ) {
      break;
    }
  }
  return i;
}

void emAfPricePrintBlockPeriodTableIndex(uint8_t endpoint, uint8_t index)
{
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }
  if ( index < EMBER_AF_PLUGIN_PRICE_CLIENT_BLOCK_PERIOD_TABLE_SIZE ) {
    emberAfPriceClusterPrintln("Print Block Period [%d]", index);
    emberAfPriceClusterPrintln("  valid=%d", priceInfo.blockPeriodTable.commonInfos[ep][index].valid);
    emberAfPriceClusterPrintln("  providerId=%d", priceInfo.blockPeriodTable.commonInfos[ep][index].providerId);
    emberAfPriceClusterPrintln("  issuerEventId=%d", priceInfo.blockPeriodTable.commonInfos[ep][index].issuerEventId);
    emberAfPriceClusterPrintln("  startTime=0x%4x", priceInfo.blockPeriodTable.commonInfos[ep][index].startTime);
    emberAfPriceClusterPrintln("  duration=%d", priceInfo.blockPeriodTable.commonInfos[ep][index].durationSec);
    emberAfPriceClusterPrintln("  rawStartTime=0x%4x", priceInfo.blockPeriodTable.blockPeriod[ep][index].blockPeriodStartTime);
    emberAfPriceClusterPrintln("  rawDuration=%d", priceInfo.blockPeriodTable.blockPeriod[ep][index].blockPeriodDuration);
    emberAfPriceClusterPrintln("  durationType=%d", priceInfo.blockPeriodTable.blockPeriod[ep][index].blockPeriodDurationType);
    emberAfPriceClusterPrintln("  blockPeriodControl=%d", priceInfo.blockPeriodTable.blockPeriod[ep][index].blockPeriodControl);
    emberAfPriceClusterPrintln("  tariffType=%d", priceInfo.blockPeriodTable.blockPeriod[ep][index].tariffType);
    emberAfPriceClusterPrintln("  tariffResolutionPeriod=%d", priceInfo.blockPeriodTable.blockPeriod[ep][index].tariffResolutionPeriod);
  } else {
    emberAfPriceClusterPrintln("Error: Block Period NOT FOUND");
  }
}

static void emberAfPriceInitBlockThresholdsTable(uint8_t endpoint)
{
  uint8_t i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }
  for ( i = 0; i < EMBER_AF_PLUGIN_PRICE_CLIENT_BLOCK_THRESHOLD_TABLE_SIZE; i++ ) {
    priceInfo.blockThresholdTable.commonInfos[ep][i].valid = false;
  }
}

static void emberAfPriceInitConversionFactorTable(uint8_t endpoint)
{
  uint8_t i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }
  for ( i = 0; i < EMBER_AF_PLUGIN_PRICE_CLIENT_CONVERSION_FACTOR_TABLE_SIZE; i++ ) {
    priceInfo.conversionFactorTable.commonInfos[ep][i].providerId = UNSPECIFIED_PROVIDER_ID;
    priceInfo.conversionFactorTable.commonInfos[ep][i].valid = false;
  }
}

uint8_t emAfPriceGetConversionFactorIndexByEventId(uint8_t endpoint, uint32_t issuerEventId)
{
  uint8_t i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return 0xFF;
  }

  i = emAfPriceCommonGetMatchingIndex(priceInfo.conversionFactorTable.commonInfos[ep],
                                      EMBER_AF_PLUGIN_PRICE_CLIENT_CONVERSION_FACTOR_TABLE_SIZE,
                                      issuerEventId);
  return i;
}

void emAfPricePrintConversionFactorEntryIndex(uint8_t endpoint, uint8_t i)
{
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }
  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_CONVERSION_FACTOR_TABLE_SIZE ) {
    emberAfPriceClusterPrintln("Print Conversion Factor [%d]", i);
    emberAfPriceClusterPrintln("  issuerEventId=%d", priceInfo.conversionFactorTable.commonInfos[ep][i].issuerEventId);
    emberAfPriceClusterPrintln("  startTime=%d", priceInfo.conversionFactorTable.commonInfos[ep][i].startTime);
    emberAfPriceClusterPrintln("  conversionFactor=%d", priceInfo.conversionFactorTable.conversionFactor[ep][i].conversionFactor);
    emberAfPriceClusterPrintln("  conversionFactorTrailingDigit=%d", priceInfo.conversionFactorTable.conversionFactor[ep][i].conversionFactorTrailingDigit);
  } else {
    emberAfPriceClusterPrintln("Conversion Factor NOT FOUND");
  }
}

static void emberAfPriceInitCalorificValueTable(uint8_t endpoint)
{
  uint8_t i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }
  for ( i = 0; i < EMBER_AF_PLUGIN_PRICE_CLIENT_CALORIFIC_VALUE_TABLE_SIZE; i++ ) {
    priceInfo.calorificValueTable.commonInfos[ep][i].valid = false;
    priceInfo.calorificValueTable.commonInfos[ep][i].providerId = UNSPECIFIED_PROVIDER_ID;
  }
}

uint8_t emAfPriceGetCalorificValueIndexByEventId(uint8_t endpoint, uint32_t issuerEventId)
{
  uint8_t i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return 0xFF;
  }
  i = emAfPriceCommonGetMatchingIndex(priceInfo.calorificValueTable.commonInfos[ep],
                                      EMBER_AF_PLUGIN_PRICE_CLIENT_CALORIFIC_VALUE_TABLE_SIZE,
                                      issuerEventId);
  return i;
}

void emAfPricePrintCalorificValueEntryIndex(uint8_t endpoint, uint8_t i)
{
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }
  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_CALORIFIC_VALUE_TABLE_SIZE ) {
    emberAfPriceClusterPrintln("Print Calorific Value [%d]", i);
    emberAfPriceClusterPrintln("  issuerEventId=%d", priceInfo.calorificValueTable.commonInfos[ep][i].issuerEventId);
    emberAfPriceClusterPrintln("  startTime=%d", priceInfo.calorificValueTable.commonInfos[ep][i].startTime);
    emberAfPriceClusterPrintln("  calorificValue=%d", priceInfo.calorificValueTable.calorificValue[ep][i].calorificValue);
    emberAfPriceClusterPrintln("  calorificValueUnit=%d", priceInfo.calorificValueTable.calorificValue[ep][i].calorificValueUnit);
    emberAfPriceClusterPrintln("  calorificValueTrailingDigit=%d", priceInfo.calorificValueTable.calorificValue[ep][i].calorificValueTrailingDigit);
  } else {
    emberAfPriceClusterPrintln("Calorific Value NOT FOUND");
  }
}

static void emberAfPriceClusterInitCO2Table(uint8_t endpoint)
{
  uint8_t i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }
  for ( i = 0; i < EMBER_AF_PLUGIN_PRICE_CLIENT_CO2_TABLE_SIZE; i++ ) {
    priceInfo.co2ValueTable.commonInfos[ep][i].valid = false;
  }
}

uint8_t emberAfPriceClusterGetActiveCo2ValueIndex(uint8_t endpoint)
{
  uint8_t i;

  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return 0xFF;
  }
  i = emAfPriceCommonGetActiveIndex(priceInfo.co2ValueTable.commonInfos[ep],
                                    EMBER_AF_PLUGIN_PRICE_CLIENT_CO2_TABLE_SIZE);
  return i;
}

void emAfPricePrintCo2ValueTablePrintIndex(uint8_t endpoint, uint8_t i)
{
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }
  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_CO2_TABLE_SIZE ) {
    emberAfPriceClusterPrintln("Print CO2 Value [%d]", i);
    emberAfPriceClusterPrintln("  isValid=%d", priceInfo.co2ValueTable.commonInfos[ep][i].valid);
    emberAfPriceClusterPrintln("  providerId=0x%4X", priceInfo.co2ValueTable.commonInfos[ep][i].providerId);
    emberAfPriceClusterPrintln("  issuerEventId=0x%4X", priceInfo.co2ValueTable.commonInfos[ep][i].issuerEventId);
    emberAfPriceClusterPrint("  startTime= ");
    emberAfPrintTime(priceInfo.co2ValueTable.commonInfos[ep][i].startTime);
    emberAfPriceClusterPrintln("  tariffType=0x%X", priceInfo.co2ValueTable.co2Value[ep][i].tariffType);
    emberAfPriceClusterPrintln("  cO2Value=0x%4X", priceInfo.co2ValueTable.co2Value[ep][i].cO2Value);
    emberAfPriceClusterPrintln("  cO2ValueUnit=0x%X", priceInfo.co2ValueTable.co2Value[ep][i].cO2ValueUnit);
    emberAfPriceClusterPrintln("  cO2ValueTrailingDigit=0x%X", priceInfo.co2ValueTable.co2Value[ep][i].cO2ValueTrailingDigit);
  }
}

static void emAfPriceClientTierLabelsInit(uint8_t endpoint)
{
  uint8_t i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }
  for ( i = 0; i < EMBER_AF_PLUGIN_PRICE_CLIENT_TIER_LABELS_TABLE_SIZE; i++ ) {
    priceInfo.tierLabelsTable.commonInfos[ep][i].valid = false;
  }
}

static void emAfPriceClientAddTierLabel(uint8_t endpoint, uint32_t providerId, uint32_t issuerEventId, uint32_t issuerTariffId,
                                        uint8_t numberOfLabels, uint8_t *tierLabelsPayload)
{
  uint8_t i, j, x;
  uint8_t tierLabelLen;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }

  i = emAfPriceCommonGetMatchingOrUnusedIndex(priceInfo.tierLabelsTable.commonInfos[ep],
                                              EMBER_AF_PLUGIN_PRICE_CLIENT_TIER_LABELS_TABLE_SIZE,
                                              providerId, issuerEventId);
  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_TIER_LABELS_TABLE_SIZE ) {
    priceInfo.tierLabelsTable.commonInfos[ep][i].providerId = providerId;
    priceInfo.tierLabelsTable.commonInfos[ep][i].issuerEventId = issuerEventId;
    priceInfo.tierLabelsTable.tierLabels[ep][i].issuerTariffId = issuerTariffId;
    priceInfo.tierLabelsTable.commonInfos[ep][i].valid = true;
    if ( numberOfLabels > EMBER_AF_PLUGIN_PRICE_CLIENT_MAX_TIERS_PER_TARIFF ) {
      numberOfLabels = EMBER_AF_PLUGIN_PRICE_CLIENT_MAX_TIERS_PER_TARIFF;
    }
    priceInfo.tierLabelsTable.tierLabels[ep][i].numberOfLabels = numberOfLabels;
    x = 0;
    for ( j = 0; j < numberOfLabels; j++ ) {
      priceInfo.tierLabelsTable.tierLabels[ep][i].tierIds[j] = tierLabelsPayload[x];
      tierLabelLen = tierLabelsPayload[x + 1];
      if ( tierLabelLen > 12 ) {
        tierLabelLen = 12;
        tierLabelsPayload[x + 1] = tierLabelLen;
      }
      MEMCOPY(priceInfo.tierLabelsTable.tierLabels[ep][i].tierLabels[j], &tierLabelsPayload[x + 1], tierLabelLen + 1);
      x += (tierLabelLen + 2);
    }
  }
}

void emAfPricePrintTierLabelTableEntryIndex(uint8_t endpoint, uint8_t i)
{
  uint8_t j;
  uint8_t numLabels;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }

  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_TIER_LABELS_TABLE_SIZE ) {
    emberAfPriceClusterPrintln("= CLIENT TIER LABEL TABLE [%d] =", i);
    emberAfPriceClusterPrintln("  providerId=%d", priceInfo.tierLabelsTable.commonInfos[ep][i].providerId);
    emberAfPriceClusterPrintln("  issuerEventId=%d", priceInfo.tierLabelsTable.commonInfos[ep][i].issuerEventId);
    emberAfPriceClusterPrintln("  issuerTariffId=%d", priceInfo.tierLabelsTable.tierLabels[ep][i].issuerTariffId);
    emberAfPriceClusterPrintln("  numberOfLabels=%d", priceInfo.tierLabelsTable.tierLabels[ep][i].numberOfLabels);
    numLabels = priceInfo.tierLabelsTable.tierLabels[ep][i].numberOfLabels;
    if ( numLabels > EMBER_AF_PLUGIN_PRICE_CLIENT_MAX_TIERS_PER_TARIFF ) {
      numLabels = EMBER_AF_PLUGIN_PRICE_CLIENT_MAX_TIERS_PER_TARIFF;
    }
    for ( j = 0; j < numLabels; j++ ) {
      emberAfPriceClusterPrintln("  tierId[%d]=%d", j, priceInfo.tierLabelsTable.tierLabels[ep][i].tierIds[j]);
    }
  } else {
    emberAfPriceClusterPrintln("Error:  Tier Label index %d not valid", i);
  }
}

uint8_t emAfPriceGetActiveTierLabelTableIndexByTariffId(uint8_t endpoint, uint32_t tariffId)
{
  uint32_t largestEventId = 0;
  uint8_t  largestEventIdIndex = 0xFF;
  uint8_t  i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return 0xFF;
  }

  emberAfPriceClusterPrintln("===========   TIER LABEL TABLE CHECK, tariff=%d", tariffId);
  for ( i = 0; i < EMBER_AF_PLUGIN_PRICE_CLIENT_TIER_LABELS_TABLE_SIZE; i++ ) {
    emberAfPriceClusterPrintln("  i=%d, val=%d, tariffId=%d, eventId=%d", i,
                               priceInfo.tierLabelsTable.commonInfos[ep][i].valid, priceInfo.tierLabelsTable.tierLabels[ep][i].issuerTariffId,
                               priceInfo.tierLabelsTable.commonInfos[ep][i].issuerEventId);

    if ( priceInfo.tierLabelsTable.commonInfos[ep][i].valid
         && (priceInfo.tierLabelsTable.tierLabels[ep][i].issuerTariffId == tariffId)
         && (priceInfo.tierLabelsTable.commonInfos[ep][i].issuerEventId > largestEventId) ) {
      largestEventId = priceInfo.tierLabelsTable.commonInfos[ep][i].issuerEventId;
      largestEventIdIndex = i;
      emberAfPriceClusterPrintln("   ___ UPDATING i=%d", i);
    }
  }
  emberAfPriceClusterPrintln("- RETURN %d", largestEventIdIndex);
  return largestEventIdIndex;
}

static void emberAfPriceInitBillingPeriodTable(uint8_t endpoint)
{
  uint8_t i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }
  for ( i = 0; i < EMBER_AF_PLUGIN_PRICE_CLIENT_BILLING_PERIOD_TABLE_SIZE; i++ ) {
    priceInfo.billingPeriodTable.commonInfos[ep][i].valid = false;
  }
}

uint8_t emAfPriceGetActiveBillingPeriodIndex(uint8_t endpoint)
{
  uint8_t i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return 0xFF;
  }
  // Get the event that started before current time, with largest event Id.
  i = emAfPriceCommonGetActiveIndex(priceInfo.billingPeriodTable.commonInfos[ep],
                                    EMBER_AF_PLUGIN_PRICE_CLIENT_BILLING_PERIOD_TABLE_SIZE);

  return i;
}

void emAfPricePrintBillingPeriodTableEntryIndex(uint8_t endpoint, uint8_t i)
{
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }
  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_BILLING_PERIOD_TABLE_SIZE ) {
    emberAfPriceClusterPrintln("Print Billing Period Table [%d]", i);
    emberAfPriceClusterPrintln("  providerId=%d", priceInfo.billingPeriodTable.commonInfos[ep][i].providerId);
    emberAfPriceClusterPrintln("  issuerEventId=%d", priceInfo.billingPeriodTable.commonInfos[ep][i].issuerEventId);
    emberAfPriceClusterPrintln("  startTime=%d", priceInfo.billingPeriodTable.billingPeriod[ep][i].billingPeriodStartTime);
    emberAfPriceClusterPrintln("  duration=%d", priceInfo.billingPeriodTable.billingPeriod[ep][i].billingPeriodDuration);
    emberAfPriceClusterPrintln("  durationType=%d", priceInfo.billingPeriodTable.billingPeriod[ep][i].billingPeriodDurationType);
    emberAfPriceClusterPrintln("  tariffType=%d", priceInfo.billingPeriodTable.billingPeriod[ep][i].tariffType);
  } else {
    emberAfPriceClusterPrintln("Billing Period Entry NOT FOUND");
  }
}

static void emberAfPriceClusterInitCpp(uint8_t endpoint)
{
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }
  priceInfo.cppEventTable.commonInfos[ep].valid = false;
  priceInfo.cppEventTable.commonInfos[ep].issuerEventId = 0;
}

void emberAfPricePrintCppEvent(uint8_t endpoint)
{
  uint32_t timeNow;
  uint32_t cppEndTime;
  bool cppActive;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }

  timeNow = emberAfGetCurrentTime();
  cppEndTime = priceInfo.cppEventTable.commonInfos[ep].startTime + priceInfo.cppEventTable.commonInfos[ep].durationSec;

  cppActive = (priceInfo.cppEventTable.commonInfos[ep].valid
               && (priceInfo.cppEventTable.commonInfos[ep].startTime <= timeNow)
               && (cppEndTime >= timeNow) );

  emberAfPriceClusterPrintln("  == startTime=0x%4x, endTime=0x%4x,  timeNow=0x%4x",
                             priceInfo.cppEventTable.commonInfos[ep].startTime, cppEndTime, priceInfo.cppEventTable.commonInfos[ep].durationSec);

  emberAfPriceClusterPrintln("= CPP Event =");
  emberAfPriceClusterPrintln("  active=%d", cppActive);
  emberAfPriceClusterPrintln("  valid=%d", priceInfo.cppEventTable.commonInfos[ep].valid);
  emberAfPriceClusterPrintln("  providerId=%d", priceInfo.cppEventTable.commonInfos[ep].providerId);
  emberAfPriceClusterPrintln("  issuerEventId=%d", priceInfo.cppEventTable.commonInfos[ep].issuerEventId);
  emberAfPriceClusterPrintln("  startTime=0x%4x", priceInfo.cppEventTable.commonInfos[ep].startTime);
  emberAfPriceClusterPrintln("  durationInMinutes=%d", priceInfo.cppEventTable.cppEvent[ep].durationInMinutes);
  emberAfPriceClusterPrintln("  tariffType=%d", priceInfo.cppEventTable.cppEvent[ep].tariffType);
  emberAfPriceClusterPrintln("  cppPriceTier=%d", priceInfo.cppEventTable.cppEvent[ep].cppPriceTier);
  emberAfPriceClusterPrintln("  cppAuth=%d", priceInfo.cppEventTable.cppEvent[ep].cppAuth);
}

static void emberAfPriceClusterInitCreditPaymentTable(uint8_t endpoint)
{
  uint8_t i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }
  for ( i = 0; i < EMBER_AF_PLUGIN_PRICE_CLIENT_CREDIT_PAYMENT_TABLE_SIZE; i++ ) {
    priceInfo.creditPaymentTable.commonInfos[ep][i].valid = false;
  }
}

uint8_t emAfPriceCreditPaymentTableGetIndexWithEventId(uint8_t endpoint, uint32_t issuerEventId)
{
  uint8_t i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return 0xFF;
  }

  i = emAfPriceCommonGetMatchingIndex(priceInfo.creditPaymentTable.commonInfos[ep],
                                      EMBER_AF_PLUGIN_PRICE_CLIENT_CREDIT_PAYMENT_TABLE_SIZE,
                                      issuerEventId);
  return i;
}

void emAfPricePrintCreditPaymentTableIndex(uint8_t endpoint, uint8_t index)
{
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }
  if ( index >= EMBER_AF_PLUGIN_PRICE_CLIENT_CREDIT_PAYMENT_TABLE_SIZE ) {
    emberAfPriceClusterPrintln("Error: Print index %d out of bounds.", index);
  } else if ( priceInfo.creditPaymentTable.commonInfos[ep][index].valid == false ) {
    emberAfPriceClusterPrintln("Error: Entry %d invalid", index);
  } else {
    emberAfPriceClusterPrintln("Print Credit Payment [%d]", index);
    emberAfPriceClusterPrintln("  valid=%d", priceInfo.creditPaymentTable.commonInfos[ep][index].valid);
    emberAfPriceClusterPrintln("  providerId=%d", priceInfo.creditPaymentTable.commonInfos[ep][index].providerId);
    emberAfPriceClusterPrintln("  issuerEventId=%d", priceInfo.creditPaymentTable.commonInfos[ep][index].issuerEventId);
    emberAfPriceClusterPrintln("  dueDate=%d", priceInfo.creditPaymentTable.creditPayment[ep][index].creditPaymentDueDate);
    emberAfPriceClusterPrintln("  overDueAmount=%d", priceInfo.creditPaymentTable.creditPayment[ep][index].creditPaymentOverDueAmount);
    emberAfPriceClusterPrintln("  status=%d", priceInfo.creditPaymentTable.creditPayment[ep][index].creditPaymentStatus);
    emberAfPriceClusterPrintln("  payment=%d", priceInfo.creditPaymentTable.creditPayment[ep][index].creditPayment);
    emberAfPriceClusterPrintln("  paymentDate=%d", priceInfo.creditPaymentTable.creditPayment[ep][index].creditPaymentDate);
    emberAfPriceClusterPrintln("  paymentRef=%d", priceInfo.creditPaymentTable.creditPayment[ep][index].creditPaymentRef);
  }
}

static void emberAfPriceClusterInitCurrencyConversionTable(uint8_t endpoint)
{
  uint8_t i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }
  for ( i = 0; i < EMBER_AF_PLUGIN_PRICE_CLIENT_CURRENCY_CONVERSION_TABLE_SIZE; i++ ) {
    priceInfo.currencyConversionTable.commonInfos[ep][i].valid = false;
  }
}

uint8_t emberAfPriceClusterGetActiveCurrencyIndex(uint8_t endpoint)
{
  uint8_t i;
  uint32_t timeNow = emberAfGetCurrentTime();
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return 0xFF;
  }

  i = emAfPriceCommonGetActiveIndex(priceInfo.currencyConversionTable.commonInfos[ep],
                                    EMBER_AF_PLUGIN_PRICE_CLIENT_CURRENCY_CONVERSION_TABLE_SIZE);

  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_CURRENCY_CONVERSION_TABLE_SIZE ) {
    emberAfPriceClusterPrintln("GET ACTIVE:  i=%d, startTime=%d,  currTime=%d",
                               i, priceInfo.currencyConversionTable.commonInfos[ep][i].startTime, timeNow);
  }

  return i;
}

uint8_t emberAfPriceClusterCurrencyConversionTableGetIndexByEventId(uint8_t endpoint, uint32_t issuerEventId)
{
  uint8_t i;
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return 0xFF;
  }
  i = emAfPriceCommonGetMatchingIndex(priceInfo.currencyConversionTable.commonInfos[ep],
                                      EMBER_AF_PLUGIN_PRICE_CLIENT_CURRENCY_CONVERSION_TABLE_SIZE,
                                      issuerEventId);
  return i;
}

void emAfPricePrintCurrencyConversionTableIndex(uint8_t endpoint, uint8_t i)
{
  uint8_t ep = emberAfFindClusterClientEndpointIndex(endpoint, ZCL_PRICE_CLUSTER_ID);
  if (ep == 0xFF) {
    return;
  }
  if ( i < EMBER_AF_PLUGIN_PRICE_CLIENT_CURRENCY_CONVERSION_TABLE_SIZE ) {
    emberAfPriceClusterPrintln("= Print Currency Conversion Table [%d]", i);
    emberAfPriceClusterPrintln("  providerId=%d", priceInfo.currencyConversionTable.commonInfos[ep][i].providerId);
    emberAfPriceClusterPrintln("  issuerEventId=%d", priceInfo.currencyConversionTable.commonInfos[ep][i].issuerEventId);
    emberAfPriceClusterPrintln("  startTime=%d", priceInfo.currencyConversionTable.commonInfos[ep][i].startTime);
    emberAfPriceClusterPrintln("  newCurrency=%d", priceInfo.currencyConversionTable.currencyConversion[ep][i].newCurrency);
    emberAfPriceClusterPrintln("  conversionFactor=%d", priceInfo.currencyConversionTable.currencyConversion[ep][i].conversionFactor);
    emberAfPriceClusterPrintln("  conversionFactorTrailingDigit=%d", priceInfo.currencyConversionTable.currencyConversion[ep][i].conversionFactorTrailingDigit);
    emberAfPriceClusterPrintln("  currencyChangeControlFlags=%d", priceInfo.currencyConversionTable.currencyConversion[ep][i].currencyChangeControlFlags);
  }
}

#ifdef UC_BUILD

bool emberAfPriceClusterPublishConsolidatedBillCallback(EmberAfClusterCommand *cmd);

uint32_t emberAfPriceClusterClientCommandParse(sl_service_opcode_t opcode,
                                               sl_service_function_context_t *context)
{
  (void)opcode;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_PUBLISH_PRICE_COMMAND_ID:
      {
        wasHandled = emberAfPriceClusterPublishPriceCallback(cmd);
        break;
      }
      case ZCL_PUBLISH_BLOCK_PERIOD_COMMAND_ID:
      {
        wasHandled = emberAfPriceClusterPublishBlockPeriodCallback(cmd);
        break;
      }
      case ZCL_PUBLISH_CONVERSION_FACTOR_COMMAND_ID:
      {
        wasHandled = emberAfPriceClusterPublishConversionFactorCallback(cmd);
        break;
      }
      case ZCL_PUBLISH_CALORIFIC_VALUE_COMMAND_ID:
      {
        wasHandled = emberAfPriceClusterPublishCalorificValueCallback(cmd);
        break;
      }
      case ZCL_PUBLISH_TARIFF_INFORMATION_COMMAND_ID:
      {
        wasHandled = emberAfPriceClusterPublishTariffInformationCallback(cmd);
        break;
      }
      case ZCL_PUBLISH_BLOCK_THRESHOLDS_COMMAND_ID:
      {
        wasHandled = emberAfPriceClusterPublishBlockThresholdsCallback(cmd);
        break;
      }
      case ZCL_PUBLISH_CO2_VALUE_COMMAND_ID:
      {
        wasHandled = emberAfPriceClusterPublishCO2ValueCallback(cmd);
        break;
      }
      case ZCL_PUBLISH_TIER_LABELS_COMMAND_ID:
      {
        wasHandled = emberAfPriceClusterPublishTierLabelsCallback(cmd);
        break;
      }
      case ZCL_PUBLISH_BILLING_PERIOD_COMMAND_ID:
      {
        wasHandled = emberAfPriceClusterPublishBillingPeriodCallback(cmd);
        break;
      }
      case ZCL_PUBLISH_CONSOLIDATED_BILL_COMMAND_ID:
      {
        wasHandled = emberAfPriceClusterPublishConsolidatedBillCallback(cmd);
        break;
      }
      case ZCL_PUBLISH_CPP_EVENT_COMMAND_ID:
      {
        wasHandled = emberAfPriceClusterPublishCppEventCallback(cmd);
        break;
      }
      case ZCL_PUBLISH_CREDIT_PAYMENT_COMMAND_ID:
      {
        wasHandled = emberAfPriceClusterPublishCreditPaymentCallback(cmd);
        break;
      }
      case ZCL_PUBLISH_CURRENCY_CONVERSION_COMMAND_ID:
      {
        wasHandled = emberAfPriceClusterPublishCurrencyConversionCallback(cmd);
        break;
      }
      case ZCL_CANCEL_TARIFF_COMMAND_ID:
      {
        wasHandled = emberAfPriceClusterCancelTariffCallback(cmd);
        break;
      }
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
