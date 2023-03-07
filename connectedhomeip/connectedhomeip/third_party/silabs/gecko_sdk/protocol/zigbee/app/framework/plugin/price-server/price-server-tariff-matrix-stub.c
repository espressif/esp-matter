/***************************************************************************//**
 * @file
 * @brief Matrix routines for the Price Server plugin.
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
#include "price-server.h"
#include "price-server-tick.h"

#ifdef UC_BUILD
#include "sl_component_catalog.h"
#endif // UC_BUILD

void emberAfPriceClearTariffTable(uint8_t endpoint)
{
  (void) endpoint;
}

void emberAfPriceClearPriceMatrixTable(uint8_t endpoint)
{
  (void) endpoint;
}

void emberAfPriceClearBlockThresholdsTable(uint8_t endpoint)
{
  (void) endpoint;
}

bool emberAfPriceSetTariffTableEntry(uint8_t endpoint,
                                     uint8_t index,
                                     EmberAfPriceCommonInfo *info,
                                     const EmberAfScheduledTariff *tariff)
{
  (void) endpoint;
  (void) index;
  (void) *info;
  (void) *tariff;
  return false;
}

bool emberAfPriceGetTariffTableEntry(uint8_t endpoint,
                                     uint8_t index,
                                     EmberAfPriceCommonInfo *info,
                                     EmberAfScheduledTariff *tariff)
{
  (void) endpoint;
  (void) index;
  (void) *info;
  (void) *tariff;
  return false;
}

bool emberAfPriceAddTariffTableEntry(uint8_t endpoint,
                                     EmberAfPriceCommonInfo *info,
                                     const EmberAfScheduledTariff *curTariff)
{
  (void) endpoint;
  (void) *info;
  (void) *curTariff;
  return false;
}

bool emberAfPriceGetPriceMatrix(uint8_t endpoint,
                                uint8_t index,
                                EmberAfPriceCommonInfo *info,
                                EmberAfScheduledPriceMatrix *pm)
{
  (void) endpoint;
  (void) index;
  (void) *info;
  (void) *pm;
  return false;
}

bool emberAfPriceGetBlockThresholdsTableEntry(uint8_t endpoint,
                                              uint8_t index,
                                              EmberAfScheduledBlockThresholds *bt)
{
  (void) endpoint;
  (void) index;
  (void) *bt;
  return false;
}

bool emberAfPriceGetTariffByIssuerTariffId(uint8_t endpoint,
                                           uint32_t issuerTariffId,
                                           EmberAfPriceCommonInfo *info,
                                           EmberAfScheduledTariff *tariff)
{
  (void) endpoint;
  (void) issuerTariffId;
  (void) *info;
  (void) *tariff;
  return false;
}

bool emberAfPriceGetPriceMatrixByIssuerTariffId(uint8_t endpoint,
                                                uint32_t issuerTariffId,
                                                EmberAfPriceCommonInfo *info,
                                                EmberAfScheduledPriceMatrix *pm)
{
  (void) endpoint;
  (void) issuerTariffId;
  (void) *info;
  (void) *pm;
  return false;
}

bool emberAfPriceGetBlockThresholdsByIssuerTariffId(uint8_t endpoint,
                                                    uint32_t issuerTariffId,
                                                    EmberAfPriceCommonInfo *info,
                                                    EmberAfScheduledBlockThresholds *bt)
{
  (void) endpoint;
  (void) issuerTariffId;
  (void) *info;
  (void) *bt;
  return false;
}

bool emberAfPriceSetPriceMatrix(uint8_t endpoint,
                                uint8_t index,
                                EmberAfPriceCommonInfo *info,
                                const EmberAfScheduledPriceMatrix *pm)
{
  (void) endpoint;
  (void) index;
  (void) *info;
  (void) *pm;
  return false;
}

bool emberAfPriceSetBlockThresholdsTableEntry(uint8_t endpoint,
                                              uint8_t index,
                                              const EmberAfPriceCommonInfo *info,
                                              const EmberAfScheduledBlockThresholds *bt)
{
  (void) endpoint;
  (void) index;
  (void) *info;
  (void) *bt;
  return false;
}

void emberAfPricePrintTariff(const EmberAfPriceCommonInfo *info,
                             const EmberAfScheduledTariff *tariff)
{
  (void) *info;
  (void) *tariff;
}
void emberAfPricePrintPriceMatrix(uint8_t endpoint,
                                  const EmberAfPriceCommonInfo *info,
                                  const EmberAfScheduledPriceMatrix *pm)
{
  (void) endpoint;
  (void) *info;
  (void) *pm;
}
void emberAfPricePrintBlockThresholds(uint8_t endpoint,
                                      const EmberAfPriceCommonInfo *info,
                                      const EmberAfScheduledBlockThresholds *bt)
{
  (void) endpoint;
  (void) *info;
  (void) *bt;
}
void emberAfPricePrintTariffTable(uint8_t endpoint)
{
  (void) endpoint;
}
void emberAfPricePrintPriceMatrixTable(uint8_t endpoint)
{
  (void) endpoint;
}
void emberAfPricePrintBlockThresholdsTable(uint8_t endpoint)
{
  (void) endpoint;
}
bool emberAfPriceAddBlockThresholdsTableEntry(uint8_t endpoint,
                                              uint32_t providerId,
                                              uint32_t issuerEventId,
                                              uint32_t startTime,
                                              uint32_t issuerTariffId,
                                              uint8_t commandIndex,
                                              uint8_t numberOfCommands,
                                              uint8_t subpayloadControl,
                                              uint8_t* payload)
{
  (void) endpoint;
  (void) providerId;
  (void) issuerEventId;
  (void) startTime;
  (void) issuerTariffId;
  (void) commandIndex;
  (void) numberOfCommands;
  (void) subpayloadControl;
  (void) *payload;
  return false;
}

bool emberAfPriceAddPriceMatrixRaw(uint8_t endpoint,
                                   uint32_t providerId,
                                   uint32_t issuerEventId,
                                   uint32_t startTime,
                                   uint32_t issuerTariffId,
                                   uint8_t commandIndex,
                                   uint8_t numberOfCommands,
                                   uint8_t subPayloadControl,
                                   uint8_t* payload)
{
  (void) endpoint;
  (void) providerId;
  (void) issuerEventId;
  (void) startTime;
  (void) issuerTariffId;
  (void) commandIndex;
  (void) numberOfCommands;
  (void) subPayloadControl;
  (void) *payload;
  return false;
}
