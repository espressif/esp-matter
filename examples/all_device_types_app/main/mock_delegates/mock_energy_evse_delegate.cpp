#include "mock_energy_evse_delegate.h"
#include "esp_log.h"

namespace chip {
namespace app {
namespace Clusters {
namespace EnergyEvse {

Protocols::InteractionModel::Status MockEnergyEVSEDelegate::Disable()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockEnergyEVSEDelegate::EnableCharging(const DataModel::Nullable<uint32_t>  &enableChargeTime, const int64_t  &minimumChargeCurrent,
                                                                           const int64_t  &maximumChargeCurrent)
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockEnergyEVSEDelegate::EnableDischarging(const DataModel::Nullable<uint32_t>  &enableDischargeTime, const int64_t  &maximumDischargeCurrent)
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockEnergyEVSEDelegate::StartDiagnostics()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockEnergyEVSEDelegate::SetTargets(const DataModel::DecodableList<Structs::ChargingTargetScheduleStruct::DecodableType>  &chargingTargetSchedules)
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockEnergyEVSEDelegate::LoadTargets()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockEnergyEVSEDelegate::GetTargets(DataModel::List<const Structs::ChargingTargetScheduleStruct::Type>  &chargingTargetSchedules)
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

Protocols::InteractionModel::Status MockEnergyEVSEDelegate::ClearTargets()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return Protocols::InteractionModel::Status::Success;
}

StateEnum MockEnergyEVSEDelegate::GetState()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return StateEnum::kUnknownEnumValue;
}

SupplyStateEnum MockEnergyEVSEDelegate::GetSupplyState()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return SupplyStateEnum::kUnknownEnumValue;
}

FaultStateEnum MockEnergyEVSEDelegate::GetFaultState()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return FaultStateEnum::kNoError;
}

DataModel::Nullable<uint32_t> MockEnergyEVSEDelegate::GetChargingEnabledUntil()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<uint32_t>();
}

DataModel::Nullable<uint32_t> MockEnergyEVSEDelegate::GetDischargingEnabledUntil()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<uint32_t>();
}

int64_t MockEnergyEVSEDelegate::GetCircuitCapacity()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

int64_t MockEnergyEVSEDelegate::GetMinimumChargeCurrent()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

int64_t MockEnergyEVSEDelegate::GetMaximumChargeCurrent()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

int64_t MockEnergyEVSEDelegate::GetMaximumDischargeCurrent()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

int64_t MockEnergyEVSEDelegate::GetUserMaximumChargeCurrent()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

uint32_t MockEnergyEVSEDelegate::GetRandomizationDelayWindow()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return 0;
}

DataModel::Nullable<uint32_t> MockEnergyEVSEDelegate::GetNextChargeStartTime()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<uint32_t>();
}

DataModel::Nullable<uint32_t> MockEnergyEVSEDelegate::GetNextChargeTargetTime()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<uint32_t>();
}

DataModel::Nullable<int64_t> MockEnergyEVSEDelegate::GetNextChargeRequiredEnergy()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<int64_t>();
}

DataModel::Nullable<Percent> MockEnergyEVSEDelegate::GetNextChargeTargetSoC()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<Percent>();
}

DataModel::Nullable<uint16_t> MockEnergyEVSEDelegate::GetApproximateEVEfficiency()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<uint16_t>();
}

DataModel::Nullable<Percent> MockEnergyEVSEDelegate::GetStateOfCharge()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<Percent>();
}

DataModel::Nullable<int64_t> MockEnergyEVSEDelegate::GetBatteryCapacity()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<int64_t>();
}

DataModel::Nullable<CharSpan> MockEnergyEVSEDelegate::GetVehicleID()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<CharSpan>();
}

DataModel::Nullable<uint32_t> MockEnergyEVSEDelegate::GetSessionID()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<uint32_t>();
}

DataModel::Nullable<uint32_t> MockEnergyEVSEDelegate::GetSessionDuration()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<uint32_t>();
}

DataModel::Nullable<int64_t> MockEnergyEVSEDelegate::GetSessionEnergyCharged()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<int64_t>();
}

DataModel::Nullable<int64_t> MockEnergyEVSEDelegate::GetSessionEnergyDischarged()
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return DataModel::Nullable<int64_t>();
}

CHIP_ERROR MockEnergyEVSEDelegate::SetUserMaximumChargeCurrent(int64_t aNewValue)
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockEnergyEVSEDelegate::SetRandomizationDelayWindow(uint32_t aNewValue)
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

CHIP_ERROR MockEnergyEVSEDelegate::SetApproximateEVEfficiency(DataModel::Nullable<uint16_t> aNewValue)
{
    ESP_LOGI(LOG_TAG, "%s is not implemented", __func__);
    return CHIP_NO_ERROR;
}

} // namespace EnergyEvse
} // namespace Clusters
} // namespace app
} // namespace chip
