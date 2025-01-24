#include <app-common/zap-generated/callback.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <lib/support/Span.h>
#include <protocols/interaction_model/Constants.h>

using namespace chip;

// Cluster Init Functions
void emberAfClusterInitCallback(EndpointId endpoint, ClusterId clusterId)
{
    switch (clusterId)
    {
    case app::Clusters::AccessControl::Id:
        emberAfAccessControlClusterInitCallback(endpoint);
        break;
    case app::Clusters::AccountLogin::Id:
        emberAfAccountLoginClusterInitCallback(endpoint);
        break;
    case app::Clusters::Actions::Id:
        emberAfActionsClusterInitCallback(endpoint);
        break;
    case app::Clusters::ActivatedCarbonFilterMonitoring::Id:
        emberAfActivatedCarbonFilterMonitoringClusterInitCallback(endpoint);
        break;
    case app::Clusters::AdministratorCommissioning::Id:
        emberAfAdministratorCommissioningClusterInitCallback(endpoint);
        break;
    case app::Clusters::AirQuality::Id:
        emberAfAirQualityClusterInitCallback(endpoint);
        break;
    case app::Clusters::ApplicationBasic::Id:
        emberAfApplicationBasicClusterInitCallback(endpoint);
        break;
    case app::Clusters::ApplicationLauncher::Id:
        emberAfApplicationLauncherClusterInitCallback(endpoint);
        break;
    case app::Clusters::AudioOutput::Id:
        emberAfAudioOutputClusterInitCallback(endpoint);
        break;
    case app::Clusters::BallastConfiguration::Id:
        emberAfBallastConfigurationClusterInitCallback(endpoint);
        break;
    case app::Clusters::BasicInformation::Id:
        emberAfBasicInformationClusterInitCallback(endpoint);
        break;
    case app::Clusters::Binding::Id:
        emberAfBindingClusterInitCallback(endpoint);
        break;
    case app::Clusters::BooleanState::Id:
        emberAfBooleanStateClusterInitCallback(endpoint);
        break;
    case app::Clusters::BooleanStateConfiguration::Id:
        emberAfBooleanStateConfigurationClusterInitCallback(endpoint);
        break;
    case app::Clusters::BridgedDeviceBasicInformation::Id:
        emberAfBridgedDeviceBasicInformationClusterInitCallback(endpoint);
        break;
    case app::Clusters::CameraAvSettingsUserLevelManagement::Id:
        emberAfCameraAvSettingsUserLevelManagementClusterInitCallback(endpoint);
        break;
    case app::Clusters::CameraAvStreamManagement::Id:
        emberAfCameraAvStreamManagementClusterInitCallback(endpoint);
        break;
    case app::Clusters::CarbonDioxideConcentrationMeasurement::Id:
        emberAfCarbonDioxideConcentrationMeasurementClusterInitCallback(endpoint);
        break;
    case app::Clusters::CarbonMonoxideConcentrationMeasurement::Id:
        emberAfCarbonMonoxideConcentrationMeasurementClusterInitCallback(endpoint);
        break;
    case app::Clusters::Channel::Id:
        emberAfChannelClusterInitCallback(endpoint);
        break;
    case app::Clusters::Chime::Id:
        emberAfChimeClusterInitCallback(endpoint);
        break;
    case app::Clusters::ColorControl::Id:
        emberAfColorControlClusterInitCallback(endpoint);
        break;
    case app::Clusters::CommissionerControl::Id:
        emberAfCommissionerControlClusterInitCallback(endpoint);
        break;
    case app::Clusters::ContentAppObserver::Id:
        emberAfContentAppObserverClusterInitCallback(endpoint);
        break;
    case app::Clusters::ContentControl::Id:
        emberAfContentControlClusterInitCallback(endpoint);
        break;
    case app::Clusters::ContentLauncher::Id:
        emberAfContentLauncherClusterInitCallback(endpoint);
        break;
    case app::Clusters::Descriptor::Id:
        emberAfDescriptorClusterInitCallback(endpoint);
        break;
    case app::Clusters::DeviceEnergyManagement::Id:
        emberAfDeviceEnergyManagementClusterInitCallback(endpoint);
        break;
    case app::Clusters::DeviceEnergyManagementMode::Id:
        emberAfDeviceEnergyManagementModeClusterInitCallback(endpoint);
        break;
    case app::Clusters::DiagnosticLogs::Id:
        emberAfDiagnosticLogsClusterInitCallback(endpoint);
        break;
    case app::Clusters::DishwasherAlarm::Id:
        emberAfDishwasherAlarmClusterInitCallback(endpoint);
        break;
    case app::Clusters::DishwasherMode::Id:
        emberAfDishwasherModeClusterInitCallback(endpoint);
        break;
    case app::Clusters::DoorLock::Id:
        emberAfDoorLockClusterInitCallback(endpoint);
        break;
    case app::Clusters::EcosystemInformation::Id:
        emberAfEcosystemInformationClusterInitCallback(endpoint);
        break;
    case app::Clusters::ElectricalEnergyMeasurement::Id:
        emberAfElectricalEnergyMeasurementClusterInitCallback(endpoint);
        break;
    case app::Clusters::ElectricalPowerMeasurement::Id:
        emberAfElectricalPowerMeasurementClusterInitCallback(endpoint);
        break;
    case app::Clusters::EnergyEvse::Id:
        emberAfEnergyEvseClusterInitCallback(endpoint);
        break;
    case app::Clusters::EnergyEvseMode::Id:
        emberAfEnergyEvseModeClusterInitCallback(endpoint);
        break;
    case app::Clusters::EnergyPreference::Id:
        emberAfEnergyPreferenceClusterInitCallback(endpoint);
        break;
    case app::Clusters::EthernetNetworkDiagnostics::Id:
        emberAfEthernetNetworkDiagnosticsClusterInitCallback(endpoint);
        break;
    case app::Clusters::FanControl::Id:
        emberAfFanControlClusterInitCallback(endpoint);
        break;
    case app::Clusters::FaultInjection::Id:
        emberAfFaultInjectionClusterInitCallback(endpoint);
        break;
    case app::Clusters::FixedLabel::Id:
        emberAfFixedLabelClusterInitCallback(endpoint);
        break;
    case app::Clusters::FlowMeasurement::Id:
        emberAfFlowMeasurementClusterInitCallback(endpoint);
        break;
    case app::Clusters::FormaldehydeConcentrationMeasurement::Id:
        emberAfFormaldehydeConcentrationMeasurementClusterInitCallback(endpoint);
        break;
    case app::Clusters::GeneralCommissioning::Id:
        emberAfGeneralCommissioningClusterInitCallback(endpoint);
        break;
    case app::Clusters::GeneralDiagnostics::Id:
        emberAfGeneralDiagnosticsClusterInitCallback(endpoint);
        break;
    case app::Clusters::GroupKeyManagement::Id:
        emberAfGroupKeyManagementClusterInitCallback(endpoint);
        break;
    case app::Clusters::Groups::Id:
        emberAfGroupsClusterInitCallback(endpoint);
        break;
    case app::Clusters::HepaFilterMonitoring::Id:
        emberAfHepaFilterMonitoringClusterInitCallback(endpoint);
        break;
    case app::Clusters::IcdManagement::Id:
        emberAfIcdManagementClusterInitCallback(endpoint);
        break;
    case app::Clusters::Identify::Id:
        emberAfIdentifyClusterInitCallback(endpoint);
        break;
    case app::Clusters::IlluminanceMeasurement::Id:
        emberAfIlluminanceMeasurementClusterInitCallback(endpoint);
        break;
    case app::Clusters::KeypadInput::Id:
        emberAfKeypadInputClusterInitCallback(endpoint);
        break;
    case app::Clusters::LaundryDryerControls::Id:
        emberAfLaundryDryerControlsClusterInitCallback(endpoint);
        break;
    case app::Clusters::LaundryWasherControls::Id:
        emberAfLaundryWasherControlsClusterInitCallback(endpoint);
        break;
    case app::Clusters::LaundryWasherMode::Id:
        emberAfLaundryWasherModeClusterInitCallback(endpoint);
        break;
    case app::Clusters::LevelControl::Id:
        emberAfLevelControlClusterInitCallback(endpoint);
        break;
    case app::Clusters::LocalizationConfiguration::Id:
        emberAfLocalizationConfigurationClusterInitCallback(endpoint);
        break;
    case app::Clusters::LowPower::Id:
        emberAfLowPowerClusterInitCallback(endpoint);
        break;
    case app::Clusters::MediaInput::Id:
        emberAfMediaInputClusterInitCallback(endpoint);
        break;
    case app::Clusters::MediaPlayback::Id:
        emberAfMediaPlaybackClusterInitCallback(endpoint);
        break;
    case app::Clusters::Messages::Id:
        emberAfMessagesClusterInitCallback(endpoint);
        break;
    case app::Clusters::MicrowaveOvenControl::Id:
        emberAfMicrowaveOvenControlClusterInitCallback(endpoint);
        break;
    case app::Clusters::MicrowaveOvenMode::Id:
        emberAfMicrowaveOvenModeClusterInitCallback(endpoint);
        break;
    case app::Clusters::ModeSelect::Id:
        emberAfModeSelectClusterInitCallback(endpoint);
        break;
    case app::Clusters::NetworkCommissioning::Id:
        emberAfNetworkCommissioningClusterInitCallback(endpoint);
        break;
    case app::Clusters::NitrogenDioxideConcentrationMeasurement::Id:
        emberAfNitrogenDioxideConcentrationMeasurementClusterInitCallback(endpoint);
        break;
    case app::Clusters::OccupancySensing::Id:
        emberAfOccupancySensingClusterInitCallback(endpoint);
        break;
    case app::Clusters::OnOff::Id:
        emberAfOnOffClusterInitCallback(endpoint);
        break;
    case app::Clusters::OperationalCredentials::Id:
        emberAfOperationalCredentialsClusterInitCallback(endpoint);
        break;
    case app::Clusters::OperationalState::Id:
        emberAfOperationalStateClusterInitCallback(endpoint);
        break;
    case app::Clusters::OtaSoftwareUpdateProvider::Id:
        emberAfOtaSoftwareUpdateProviderClusterInitCallback(endpoint);
        break;
    case app::Clusters::OtaSoftwareUpdateRequestor::Id:
        emberAfOtaSoftwareUpdateRequestorClusterInitCallback(endpoint);
        break;
    case app::Clusters::OvenCavityOperationalState::Id:
        emberAfOvenCavityOperationalStateClusterInitCallback(endpoint);
        break;
    case app::Clusters::OvenMode::Id:
        emberAfOvenModeClusterInitCallback(endpoint);
        break;
    case app::Clusters::OzoneConcentrationMeasurement::Id:
        emberAfOzoneConcentrationMeasurementClusterInitCallback(endpoint);
        break;
    case app::Clusters::Pm10ConcentrationMeasurement::Id:
        emberAfPm10ConcentrationMeasurementClusterInitCallback(endpoint);
        break;
    case app::Clusters::Pm1ConcentrationMeasurement::Id:
        emberAfPm1ConcentrationMeasurementClusterInitCallback(endpoint);
        break;
    case app::Clusters::Pm25ConcentrationMeasurement::Id:
        emberAfPm25ConcentrationMeasurementClusterInitCallback(endpoint);
        break;
    case app::Clusters::PowerSource::Id:
        emberAfPowerSourceClusterInitCallback(endpoint);
        break;
    case app::Clusters::PowerSourceConfiguration::Id:
        emberAfPowerSourceConfigurationClusterInitCallback(endpoint);
        break;
    case app::Clusters::PowerTopology::Id:
        emberAfPowerTopologyClusterInitCallback(endpoint);
        break;
    case app::Clusters::PressureMeasurement::Id:
        emberAfPressureMeasurementClusterInitCallback(endpoint);
        break;
    case app::Clusters::ProxyConfiguration::Id:
        emberAfProxyConfigurationClusterInitCallback(endpoint);
        break;
    case app::Clusters::ProxyDiscovery::Id:
        emberAfProxyDiscoveryClusterInitCallback(endpoint);
        break;
    case app::Clusters::ProxyValid::Id:
        emberAfProxyValidClusterInitCallback(endpoint);
        break;
    case app::Clusters::PulseWidthModulation::Id:
        emberAfPulseWidthModulationClusterInitCallback(endpoint);
        break;
    case app::Clusters::PumpConfigurationAndControl::Id:
        emberAfPumpConfigurationAndControlClusterInitCallback(endpoint);
        break;
    case app::Clusters::PushAvStreamTransport::Id:
        emberAfPushAvStreamTransportClusterInitCallback(endpoint);
        break;
    case app::Clusters::RadonConcentrationMeasurement::Id:
        emberAfRadonConcentrationMeasurementClusterInitCallback(endpoint);
        break;
    case app::Clusters::RefrigeratorAlarm::Id:
        emberAfRefrigeratorAlarmClusterInitCallback(endpoint);
        break;
    case app::Clusters::RefrigeratorAndTemperatureControlledCabinetMode::Id:
        emberAfRefrigeratorAndTemperatureControlledCabinetModeClusterInitCallback(endpoint);
        break;
    case app::Clusters::RelativeHumidityMeasurement::Id:
        emberAfRelativeHumidityMeasurementClusterInitCallback(endpoint);
        break;
    case app::Clusters::RvcCleanMode::Id:
        emberAfRvcCleanModeClusterInitCallback(endpoint);
        break;
    case app::Clusters::RvcOperationalState::Id:
        emberAfRvcOperationalStateClusterInitCallback(endpoint);
        break;
    case app::Clusters::RvcRunMode::Id:
        emberAfRvcRunModeClusterInitCallback(endpoint);
        break;
    case app::Clusters::SampleMei::Id:
        emberAfSampleMeiClusterInitCallback(endpoint);
        break;
    case app::Clusters::ScenesManagement::Id:
        emberAfScenesManagementClusterInitCallback(endpoint);
        break;
    case app::Clusters::ServiceArea::Id:
        emberAfServiceAreaClusterInitCallback(endpoint);
        break;
    case app::Clusters::SmokeCoAlarm::Id:
        emberAfSmokeCoAlarmClusterInitCallback(endpoint);
        break;
    case app::Clusters::SoftwareDiagnostics::Id:
        emberAfSoftwareDiagnosticsClusterInitCallback(endpoint);
        break;
    case app::Clusters::Switch::Id:
        emberAfSwitchClusterInitCallback(endpoint);
        break;
    case app::Clusters::TargetNavigator::Id:
        emberAfTargetNavigatorClusterInitCallback(endpoint);
        break;
    case app::Clusters::TemperatureControl::Id:
        emberAfTemperatureControlClusterInitCallback(endpoint);
        break;
    case app::Clusters::TemperatureMeasurement::Id:
        emberAfTemperatureMeasurementClusterInitCallback(endpoint);
        break;
    case app::Clusters::Thermostat::Id:
        emberAfThermostatClusterInitCallback(endpoint);
        break;
    case app::Clusters::ThermostatUserInterfaceConfiguration::Id:
        emberAfThermostatUserInterfaceConfigurationClusterInitCallback(endpoint);
        break;
    case app::Clusters::ThreadBorderRouterManagement::Id:
        emberAfThreadBorderRouterManagementClusterInitCallback(endpoint);
        break;
    case app::Clusters::ThreadNetworkDiagnostics::Id:
        emberAfThreadNetworkDiagnosticsClusterInitCallback(endpoint);
        break;
    case app::Clusters::ThreadNetworkDirectory::Id:
        emberAfThreadNetworkDirectoryClusterInitCallback(endpoint);
        break;
    case app::Clusters::TimeFormatLocalization::Id:
        emberAfTimeFormatLocalizationClusterInitCallback(endpoint);
        break;
    case app::Clusters::TimeSynchronization::Id:
        emberAfTimeSynchronizationClusterInitCallback(endpoint);
        break;
    case app::Clusters::TlsCertificateManagement::Id:
        emberAfTlsCertificateManagementClusterInitCallback(endpoint);
        break;
    case app::Clusters::TotalVolatileOrganicCompoundsConcentrationMeasurement::Id:
        emberAfTotalVolatileOrganicCompoundsConcentrationMeasurementClusterInitCallback(endpoint);
        break;
    case app::Clusters::UnitLocalization::Id:
        emberAfUnitLocalizationClusterInitCallback(endpoint);
        break;
    case app::Clusters::UnitTesting::Id:
        emberAfUnitTestingClusterInitCallback(endpoint);
        break;
    case app::Clusters::UserLabel::Id:
        emberAfUserLabelClusterInitCallback(endpoint);
        break;
    case app::Clusters::ValveConfigurationAndControl::Id:
        emberAfValveConfigurationAndControlClusterInitCallback(endpoint);
        break;
    case app::Clusters::WakeOnLan::Id:
        emberAfWakeOnLanClusterInitCallback(endpoint);
        break;
    case app::Clusters::WaterHeaterManagement::Id:
        emberAfWaterHeaterManagementClusterInitCallback(endpoint);
        break;
    case app::Clusters::WaterHeaterMode::Id:
        emberAfWaterHeaterModeClusterInitCallback(endpoint);
        break;
    case app::Clusters::WebRTCTransportProvider::Id:
        emberAfWebRTCTransportProviderClusterInitCallback(endpoint);
        break;
    case app::Clusters::WebRTCTransportRequestor::Id:
        emberAfWebRTCTransportRequestorClusterInitCallback(endpoint);
        break;
    case app::Clusters::WiFiNetworkDiagnostics::Id:
        emberAfWiFiNetworkDiagnosticsClusterInitCallback(endpoint);
        break;
    case app::Clusters::WiFiNetworkManagement::Id:
        emberAfWiFiNetworkManagementClusterInitCallback(endpoint);
        break;
    case app::Clusters::WindowCovering::Id:
        emberAfWindowCoveringClusterInitCallback(endpoint);
        break;
    case app::Clusters::ZoneManagement::Id:
        emberAfZoneManagementClusterInitCallback(endpoint);
        break;
    default:
        // Unrecognized cluster ID
        break;
    }
}
