#pragma once

#include <access/Privilege.h>

// Prevent changing generated format
// clang-format off

////////////////////////////////////////////////////////////////////////////////

// Parallel array data (*cluster*, attribute, privilege) for read attribute
#define GENERATED_ACCESS_READ_ATTRIBUTE__CLUSTER { \
    0x0000001F, /* Cluster: Access Control, Attribute: ACL, Privilege: administer */ \
    0x0000001F, /* Cluster: Access Control, Attribute: Extension, Privilege: administer */ \
    0x00000030, /* Cluster: General Commissioning, Attribute: TCAcceptedVersion, Privilege: administer */ \
    0x00000030, /* Cluster: General Commissioning, Attribute: TCMinRequiredVersion, Privilege: administer */ \
    0x00000030, /* Cluster: General Commissioning, Attribute: TCAcknowledgements, Privilege: administer */ \
    0x00000030, /* Cluster: General Commissioning, Attribute: TCAcknowledgementsRequired, Privilege: administer */ \
    0x00000030, /* Cluster: General Commissioning, Attribute: TCUpdateDeadline, Privilege: administer */ \
    0x00000031, /* Cluster: Network Commissioning, Attribute: MaxNetworks, Privilege: administer */ \
    0x00000031, /* Cluster: Network Commissioning, Attribute: Networks, Privilege: administer */ \
    0x00000031, /* Cluster: Network Commissioning, Attribute: LastNetworkingStatus, Privilege: administer */ \
    0x00000031, /* Cluster: Network Commissioning, Attribute: LastNetworkID, Privilege: administer */ \
    0x00000031, /* Cluster: Network Commissioning, Attribute: LastConnectErrorValue, Privilege: administer */ \
    0x0000003E, /* Cluster: Operational Credentials, Attribute: NOCs, Privilege: administer */ \
    0x00000046, /* Cluster: ICD Management, Attribute: RegisteredClients, Privilege: administer */ \
    0x00000046, /* Cluster: ICD Management, Attribute: ICDCounter, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Attribute: AliroReaderVerificationKey, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Attribute: AliroReaderGroupIdentifier, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Attribute: AliroReaderGroupSubIdentifier, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Attribute: AliroExpeditedTransactionSupportedProtocolVersions, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Attribute: AliroGroupResolvingKey, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Attribute: AliroSupportedBLEUWBProtocolVersions, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Attribute: AliroBLEAdvertisingVersion, Privilege: administer */ \
    0x00000451, /* Cluster: Wi-Fi Network Management, Attribute: PassphraseSurrogate, Privilege: manage */ \
    0x00000453, /* Cluster: Thread Network Directory, Attribute: PreferredExtendedPanID, Privilege: manage */ \
    0x00000453, /* Cluster: Thread Network Directory, Attribute: ThreadNetworks, Privilege: operate */ \
    0x0000050D, /* Cluster: Application Basic, Attribute: AllowedVendorList, Privilege: administer */ \
    0x00000750, /* Cluster: Ecosystem Information, Attribute: DeviceDirectory, Privilege: manage */ \
    0x00000750, /* Cluster: Ecosystem Information, Attribute: LocationDirectory, Privilege: manage */ \
    0x00000751, /* Cluster: Commissioner Control, Attribute: SupportedDeviceCategories, Privilege: manage */ \
}

// Parallel array data (cluster, *attribute*, privilege) for read attribute
#define GENERATED_ACCESS_READ_ATTRIBUTE__ATTRIBUTE { \
    0x00000000, /* Cluster: Access Control, Attribute: ACL, Privilege: administer */ \
    0x00000001, /* Cluster: Access Control, Attribute: Extension, Privilege: administer */ \
    0x00000005, /* Cluster: General Commissioning, Attribute: TCAcceptedVersion, Privilege: administer */ \
    0x00000006, /* Cluster: General Commissioning, Attribute: TCMinRequiredVersion, Privilege: administer */ \
    0x00000007, /* Cluster: General Commissioning, Attribute: TCAcknowledgements, Privilege: administer */ \
    0x00000008, /* Cluster: General Commissioning, Attribute: TCAcknowledgementsRequired, Privilege: administer */ \
    0x00000009, /* Cluster: General Commissioning, Attribute: TCUpdateDeadline, Privilege: administer */ \
    0x00000000, /* Cluster: Network Commissioning, Attribute: MaxNetworks, Privilege: administer */ \
    0x00000001, /* Cluster: Network Commissioning, Attribute: Networks, Privilege: administer */ \
    0x00000005, /* Cluster: Network Commissioning, Attribute: LastNetworkingStatus, Privilege: administer */ \
    0x00000006, /* Cluster: Network Commissioning, Attribute: LastNetworkID, Privilege: administer */ \
    0x00000007, /* Cluster: Network Commissioning, Attribute: LastConnectErrorValue, Privilege: administer */ \
    0x00000000, /* Cluster: Operational Credentials, Attribute: NOCs, Privilege: administer */ \
    0x00000003, /* Cluster: ICD Management, Attribute: RegisteredClients, Privilege: administer */ \
    0x00000004, /* Cluster: ICD Management, Attribute: ICDCounter, Privilege: administer */ \
    0x00000080, /* Cluster: Door Lock, Attribute: AliroReaderVerificationKey, Privilege: administer */ \
    0x00000081, /* Cluster: Door Lock, Attribute: AliroReaderGroupIdentifier, Privilege: administer */ \
    0x00000082, /* Cluster: Door Lock, Attribute: AliroReaderGroupSubIdentifier, Privilege: administer */ \
    0x00000083, /* Cluster: Door Lock, Attribute: AliroExpeditedTransactionSupportedProtocolVersions, Privilege: administer */ \
    0x00000084, /* Cluster: Door Lock, Attribute: AliroGroupResolvingKey, Privilege: administer */ \
    0x00000085, /* Cluster: Door Lock, Attribute: AliroSupportedBLEUWBProtocolVersions, Privilege: administer */ \
    0x00000086, /* Cluster: Door Lock, Attribute: AliroBLEAdvertisingVersion, Privilege: administer */ \
    0x00000001, /* Cluster: Wi-Fi Network Management, Attribute: PassphraseSurrogate, Privilege: manage */ \
    0x00000000, /* Cluster: Thread Network Directory, Attribute: PreferredExtendedPanID, Privilege: manage */ \
    0x00000001, /* Cluster: Thread Network Directory, Attribute: ThreadNetworks, Privilege: operate */ \
    0x00000007, /* Cluster: Application Basic, Attribute: AllowedVendorList, Privilege: administer */ \
    0x00000000, /* Cluster: Ecosystem Information, Attribute: DeviceDirectory, Privilege: manage */ \
    0x00000001, /* Cluster: Ecosystem Information, Attribute: LocationDirectory, Privilege: manage */ \
    0x00000000, /* Cluster: Commissioner Control, Attribute: SupportedDeviceCategories, Privilege: manage */ \
}

// Parallel array data (cluster, attribute, *privilege*) for read attribute
#define GENERATED_ACCESS_READ_ATTRIBUTE__PRIVILEGE { \
    chip::Access::Privilege::kAdminister, /* Cluster: Access Control, Attribute: ACL, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Access Control, Attribute: Extension, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: General Commissioning, Attribute: TCAcceptedVersion, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: General Commissioning, Attribute: TCMinRequiredVersion, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: General Commissioning, Attribute: TCAcknowledgements, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: General Commissioning, Attribute: TCAcknowledgementsRequired, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: General Commissioning, Attribute: TCUpdateDeadline, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Network Commissioning, Attribute: MaxNetworks, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Network Commissioning, Attribute: Networks, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Network Commissioning, Attribute: LastNetworkingStatus, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Network Commissioning, Attribute: LastNetworkID, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Network Commissioning, Attribute: LastConnectErrorValue, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Operational Credentials, Attribute: NOCs, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: ICD Management, Attribute: RegisteredClients, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: ICD Management, Attribute: ICDCounter, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Attribute: AliroReaderVerificationKey, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Attribute: AliroReaderGroupIdentifier, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Attribute: AliroReaderGroupSubIdentifier, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Attribute: AliroExpeditedTransactionSupportedProtocolVersions, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Attribute: AliroGroupResolvingKey, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Attribute: AliroSupportedBLEUWBProtocolVersions, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Attribute: AliroBLEAdvertisingVersion, Privilege: administer */ \
    chip::Access::Privilege::kManage, /* Cluster: Wi-Fi Network Management, Attribute: PassphraseSurrogate, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thread Network Directory, Attribute: PreferredExtendedPanID, Privilege: manage */ \
    chip::Access::Privilege::kOperate, /* Cluster: Thread Network Directory, Attribute: ThreadNetworks, Privilege: operate */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Application Basic, Attribute: AllowedVendorList, Privilege: administer */ \
    chip::Access::Privilege::kManage, /* Cluster: Ecosystem Information, Attribute: DeviceDirectory, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Ecosystem Information, Attribute: LocationDirectory, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Commissioner Control, Attribute: SupportedDeviceCategories, Privilege: manage */ \
}

////////////////////////////////////////////////////////////////////////////////

// Parallel array data (*cluster*, attribute, privilege) for write attribute
#define GENERATED_ACCESS_WRITE_ATTRIBUTE__CLUSTER { \
    0x00000006, /* Cluster: On/Off, Attribute: StartUpOnOff, Privilege: manage */ \
    0x00000008, /* Cluster: Level Control, Attribute: StartUpCurrentLevel, Privilege: manage */ \
    0x0000001E, /* Cluster: Binding, Attribute: Binding, Privilege: manage */ \
    0x0000001F, /* Cluster: Access Control, Attribute: ACL, Privilege: administer */ \
    0x0000001F, /* Cluster: Access Control, Attribute: Extension, Privilege: administer */ \
    0x00000028, /* Cluster: Basic Information, Attribute: NodeLabel, Privilege: manage */ \
    0x00000028, /* Cluster: Basic Information, Attribute: Location, Privilege: administer */ \
    0x00000028, /* Cluster: Basic Information, Attribute: LocalConfigDisabled, Privilege: manage */ \
    0x0000002A, /* Cluster: OTA Software Update Requestor, Attribute: DefaultOTAProviders, Privilege: administer */ \
    0x0000002B, /* Cluster: Localization Configuration, Attribute: ActiveLocale, Privilege: manage */ \
    0x0000002C, /* Cluster: Time Format Localization, Attribute: HourFormat, Privilege: manage */ \
    0x0000002C, /* Cluster: Time Format Localization, Attribute: ActiveCalendarType, Privilege: manage */ \
    0x0000002D, /* Cluster: Unit Localization, Attribute: TemperatureUnit, Privilege: manage */ \
    0x00000030, /* Cluster: General Commissioning, Attribute: Breadcrumb, Privilege: administer */ \
    0x00000031, /* Cluster: Network Commissioning, Attribute: InterfaceEnabled, Privilege: administer */ \
    0x0000003F, /* Cluster: Group Key Management, Attribute: GroupKeyMap, Privilege: manage */ \
    0x00000041, /* Cluster: User Label, Attribute: LabelList, Privilege: manage */ \
    0x0000005C, /* Cluster: Smoke CO Alarm, Attribute: SmokeSensitivityLevel, Privilege: manage */ \
    0x00000099, /* Cluster: Energy EVSE, Attribute: UserMaximumChargeCurrent, Privilege: manage */ \
    0x00000099, /* Cluster: Energy EVSE, Attribute: RandomizationDelayWindow, Privilege: manage */ \
    0x00000099, /* Cluster: Energy EVSE, Attribute: ApproximateEVEfficiency, Privilege: manage */ \
    0x00000101, /* Cluster: Door Lock, Attribute: DoorOpenEvents, Privilege: manage */ \
    0x00000101, /* Cluster: Door Lock, Attribute: DoorClosedEvents, Privilege: manage */ \
    0x00000101, /* Cluster: Door Lock, Attribute: OpenPeriod, Privilege: manage */ \
    0x00000101, /* Cluster: Door Lock, Attribute: Language, Privilege: manage */ \
    0x00000101, /* Cluster: Door Lock, Attribute: LEDSettings, Privilege: manage */ \
    0x00000101, /* Cluster: Door Lock, Attribute: AutoRelockTime, Privilege: manage */ \
    0x00000101, /* Cluster: Door Lock, Attribute: SoundVolume, Privilege: manage */ \
    0x00000101, /* Cluster: Door Lock, Attribute: OperatingMode, Privilege: manage */ \
    0x00000101, /* Cluster: Door Lock, Attribute: EnableLocalProgramming, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Attribute: EnableOneTouchLocking, Privilege: manage */ \
    0x00000101, /* Cluster: Door Lock, Attribute: EnableInsideStatusLED, Privilege: manage */ \
    0x00000101, /* Cluster: Door Lock, Attribute: EnablePrivacyModeButton, Privilege: manage */ \
    0x00000101, /* Cluster: Door Lock, Attribute: LocalProgrammingFeatures, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Attribute: WrongCodeEntryLimit, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Attribute: UserCodeTemporaryDisableTime, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Attribute: SendPINOverTheAir, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Attribute: RequirePINforRemoteOperation, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Attribute: ExpiringUserTimeout, Privilege: administer */ \
    0x00000102, /* Cluster: Window Covering, Attribute: Mode, Privilege: manage */ \
    0x00000200, /* Cluster: Pump Configuration and Control, Attribute: LifetimeRunningHours, Privilege: manage */ \
    0x00000200, /* Cluster: Pump Configuration and Control, Attribute: LifetimeEnergyConsumed, Privilege: manage */ \
    0x00000200, /* Cluster: Pump Configuration and Control, Attribute: OperationMode, Privilege: manage */ \
    0x00000200, /* Cluster: Pump Configuration and Control, Attribute: ControlMode, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: HVACSystemTypeConfiguration, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: LocalTemperatureCalibration, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: MinHeatSetpointLimit, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: MaxHeatSetpointLimit, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: MinCoolSetpointLimit, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: MaxCoolSetpointLimit, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: MinSetpointDeadBand, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: RemoteSensing, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: ControlSequenceOfOperation, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: SystemMode, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: TemperatureSetpointHold, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: TemperatureSetpointHoldDuration, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: ThermostatProgrammingOperationMode, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: OccupiedSetback, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: UnoccupiedSetback, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: EmergencyHeatDelta, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: ACType, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: ACCapacity, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: ACRefrigerantType, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: ACCompressorType, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: ACErrorCode, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: ACLouverPosition, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: ACCapacityformat, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: Presets, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Attribute: Schedules, Privilege: manage */ \
    0x00000204, /* Cluster: Thermostat User Interface Configuration, Attribute: KeypadLockout, Privilege: manage */ \
    0x00000204, /* Cluster: Thermostat User Interface Configuration, Attribute: ScheduleProgrammingVisibility, Privilege: manage */ \
    0x00000300, /* Cluster: Color Control, Attribute: WhitePointX, Privilege: manage */ \
    0x00000300, /* Cluster: Color Control, Attribute: WhitePointY, Privilege: manage */ \
    0x00000300, /* Cluster: Color Control, Attribute: ColorPointRX, Privilege: manage */ \
    0x00000300, /* Cluster: Color Control, Attribute: ColorPointRY, Privilege: manage */ \
    0x00000300, /* Cluster: Color Control, Attribute: ColorPointRIntensity, Privilege: manage */ \
    0x00000300, /* Cluster: Color Control, Attribute: ColorPointGX, Privilege: manage */ \
    0x00000300, /* Cluster: Color Control, Attribute: ColorPointGY, Privilege: manage */ \
    0x00000300, /* Cluster: Color Control, Attribute: ColorPointGIntensity, Privilege: manage */ \
    0x00000300, /* Cluster: Color Control, Attribute: ColorPointBX, Privilege: manage */ \
    0x00000300, /* Cluster: Color Control, Attribute: ColorPointBY, Privilege: manage */ \
    0x00000300, /* Cluster: Color Control, Attribute: ColorPointBIntensity, Privilege: manage */ \
    0x00000300, /* Cluster: Color Control, Attribute: StartUpColorTemperatureMireds, Privilege: manage */ \
    0x00000301, /* Cluster: Ballast Configuration, Attribute: MinLevel, Privilege: manage */ \
    0x00000301, /* Cluster: Ballast Configuration, Attribute: MaxLevel, Privilege: manage */ \
    0x00000301, /* Cluster: Ballast Configuration, Attribute: IntrinsicBallastFactor, Privilege: manage */ \
    0x00000301, /* Cluster: Ballast Configuration, Attribute: BallastFactorAdjustment, Privilege: manage */ \
    0x00000301, /* Cluster: Ballast Configuration, Attribute: LampType, Privilege: manage */ \
    0x00000301, /* Cluster: Ballast Configuration, Attribute: LampManufacturer, Privilege: manage */ \
    0x00000301, /* Cluster: Ballast Configuration, Attribute: LampRatedHours, Privilege: manage */ \
    0x00000301, /* Cluster: Ballast Configuration, Attribute: LampBurnHours, Privilege: manage */ \
    0x00000301, /* Cluster: Ballast Configuration, Attribute: LampAlarmMode, Privilege: manage */ \
    0x00000301, /* Cluster: Ballast Configuration, Attribute: LampBurnHoursTripPoint, Privilege: manage */ \
    0x00000406, /* Cluster: Occupancy Sensing, Attribute: HoldTime, Privilege: manage */ \
    0x00000406, /* Cluster: Occupancy Sensing, Attribute: PIROccupiedToUnoccupiedDelay, Privilege: manage */ \
    0x00000406, /* Cluster: Occupancy Sensing, Attribute: PIRUnoccupiedToOccupiedDelay, Privilege: manage */ \
    0x00000406, /* Cluster: Occupancy Sensing, Attribute: PIRUnoccupiedToOccupiedThreshold, Privilege: manage */ \
    0x00000406, /* Cluster: Occupancy Sensing, Attribute: UltrasonicOccupiedToUnoccupiedDelay, Privilege: manage */ \
    0x00000406, /* Cluster: Occupancy Sensing, Attribute: UltrasonicUnoccupiedToOccupiedDelay, Privilege: manage */ \
    0x00000406, /* Cluster: Occupancy Sensing, Attribute: UltrasonicUnoccupiedToOccupiedThreshold, Privilege: manage */ \
    0x00000406, /* Cluster: Occupancy Sensing, Attribute: PhysicalContactOccupiedToUnoccupiedDelay, Privilege: manage */ \
    0x00000406, /* Cluster: Occupancy Sensing, Attribute: PhysicalContactUnoccupiedToOccupiedDelay, Privilege: manage */ \
    0x00000406, /* Cluster: Occupancy Sensing, Attribute: PhysicalContactUnoccupiedToOccupiedThreshold, Privilege: manage */ \
    0x00000453, /* Cluster: Thread Network Directory, Attribute: PreferredExtendedPanID, Privilege: manage */ \
}

// Parallel array data (cluster, *attribute*, privilege) for write attribute
#define GENERATED_ACCESS_WRITE_ATTRIBUTE__ATTRIBUTE { \
    0x00004003, /* Cluster: On/Off, Attribute: StartUpOnOff, Privilege: manage */ \
    0x00004000, /* Cluster: Level Control, Attribute: StartUpCurrentLevel, Privilege: manage */ \
    0x00000000, /* Cluster: Binding, Attribute: Binding, Privilege: manage */ \
    0x00000000, /* Cluster: Access Control, Attribute: ACL, Privilege: administer */ \
    0x00000001, /* Cluster: Access Control, Attribute: Extension, Privilege: administer */ \
    0x00000005, /* Cluster: Basic Information, Attribute: NodeLabel, Privilege: manage */ \
    0x00000006, /* Cluster: Basic Information, Attribute: Location, Privilege: administer */ \
    0x00000010, /* Cluster: Basic Information, Attribute: LocalConfigDisabled, Privilege: manage */ \
    0x00000000, /* Cluster: OTA Software Update Requestor, Attribute: DefaultOTAProviders, Privilege: administer */ \
    0x00000000, /* Cluster: Localization Configuration, Attribute: ActiveLocale, Privilege: manage */ \
    0x00000000, /* Cluster: Time Format Localization, Attribute: HourFormat, Privilege: manage */ \
    0x00000001, /* Cluster: Time Format Localization, Attribute: ActiveCalendarType, Privilege: manage */ \
    0x00000000, /* Cluster: Unit Localization, Attribute: TemperatureUnit, Privilege: manage */ \
    0x00000000, /* Cluster: General Commissioning, Attribute: Breadcrumb, Privilege: administer */ \
    0x00000004, /* Cluster: Network Commissioning, Attribute: InterfaceEnabled, Privilege: administer */ \
    0x00000000, /* Cluster: Group Key Management, Attribute: GroupKeyMap, Privilege: manage */ \
    0x00000000, /* Cluster: User Label, Attribute: LabelList, Privilege: manage */ \
    0x0000000B, /* Cluster: Smoke CO Alarm, Attribute: SmokeSensitivityLevel, Privilege: manage */ \
    0x00000009, /* Cluster: Energy EVSE, Attribute: UserMaximumChargeCurrent, Privilege: manage */ \
    0x0000000A, /* Cluster: Energy EVSE, Attribute: RandomizationDelayWindow, Privilege: manage */ \
    0x00000027, /* Cluster: Energy EVSE, Attribute: ApproximateEVEfficiency, Privilege: manage */ \
    0x00000004, /* Cluster: Door Lock, Attribute: DoorOpenEvents, Privilege: manage */ \
    0x00000005, /* Cluster: Door Lock, Attribute: DoorClosedEvents, Privilege: manage */ \
    0x00000006, /* Cluster: Door Lock, Attribute: OpenPeriod, Privilege: manage */ \
    0x00000021, /* Cluster: Door Lock, Attribute: Language, Privilege: manage */ \
    0x00000022, /* Cluster: Door Lock, Attribute: LEDSettings, Privilege: manage */ \
    0x00000023, /* Cluster: Door Lock, Attribute: AutoRelockTime, Privilege: manage */ \
    0x00000024, /* Cluster: Door Lock, Attribute: SoundVolume, Privilege: manage */ \
    0x00000025, /* Cluster: Door Lock, Attribute: OperatingMode, Privilege: manage */ \
    0x00000028, /* Cluster: Door Lock, Attribute: EnableLocalProgramming, Privilege: administer */ \
    0x00000029, /* Cluster: Door Lock, Attribute: EnableOneTouchLocking, Privilege: manage */ \
    0x0000002A, /* Cluster: Door Lock, Attribute: EnableInsideStatusLED, Privilege: manage */ \
    0x0000002B, /* Cluster: Door Lock, Attribute: EnablePrivacyModeButton, Privilege: manage */ \
    0x0000002C, /* Cluster: Door Lock, Attribute: LocalProgrammingFeatures, Privilege: administer */ \
    0x00000030, /* Cluster: Door Lock, Attribute: WrongCodeEntryLimit, Privilege: administer */ \
    0x00000031, /* Cluster: Door Lock, Attribute: UserCodeTemporaryDisableTime, Privilege: administer */ \
    0x00000032, /* Cluster: Door Lock, Attribute: SendPINOverTheAir, Privilege: administer */ \
    0x00000033, /* Cluster: Door Lock, Attribute: RequirePINforRemoteOperation, Privilege: administer */ \
    0x00000035, /* Cluster: Door Lock, Attribute: ExpiringUserTimeout, Privilege: administer */ \
    0x00000017, /* Cluster: Window Covering, Attribute: Mode, Privilege: manage */ \
    0x00000015, /* Cluster: Pump Configuration and Control, Attribute: LifetimeRunningHours, Privilege: manage */ \
    0x00000017, /* Cluster: Pump Configuration and Control, Attribute: LifetimeEnergyConsumed, Privilege: manage */ \
    0x00000020, /* Cluster: Pump Configuration and Control, Attribute: OperationMode, Privilege: manage */ \
    0x00000021, /* Cluster: Pump Configuration and Control, Attribute: ControlMode, Privilege: manage */ \
    0x00000009, /* Cluster: Thermostat, Attribute: HVACSystemTypeConfiguration, Privilege: manage */ \
    0x00000010, /* Cluster: Thermostat, Attribute: LocalTemperatureCalibration, Privilege: manage */ \
    0x00000015, /* Cluster: Thermostat, Attribute: MinHeatSetpointLimit, Privilege: manage */ \
    0x00000016, /* Cluster: Thermostat, Attribute: MaxHeatSetpointLimit, Privilege: manage */ \
    0x00000017, /* Cluster: Thermostat, Attribute: MinCoolSetpointLimit, Privilege: manage */ \
    0x00000018, /* Cluster: Thermostat, Attribute: MaxCoolSetpointLimit, Privilege: manage */ \
    0x00000019, /* Cluster: Thermostat, Attribute: MinSetpointDeadBand, Privilege: manage */ \
    0x0000001A, /* Cluster: Thermostat, Attribute: RemoteSensing, Privilege: manage */ \
    0x0000001B, /* Cluster: Thermostat, Attribute: ControlSequenceOfOperation, Privilege: manage */ \
    0x0000001C, /* Cluster: Thermostat, Attribute: SystemMode, Privilege: manage */ \
    0x00000023, /* Cluster: Thermostat, Attribute: TemperatureSetpointHold, Privilege: manage */ \
    0x00000024, /* Cluster: Thermostat, Attribute: TemperatureSetpointHoldDuration, Privilege: manage */ \
    0x00000025, /* Cluster: Thermostat, Attribute: ThermostatProgrammingOperationMode, Privilege: manage */ \
    0x00000034, /* Cluster: Thermostat, Attribute: OccupiedSetback, Privilege: manage */ \
    0x00000037, /* Cluster: Thermostat, Attribute: UnoccupiedSetback, Privilege: manage */ \
    0x0000003A, /* Cluster: Thermostat, Attribute: EmergencyHeatDelta, Privilege: manage */ \
    0x00000040, /* Cluster: Thermostat, Attribute: ACType, Privilege: manage */ \
    0x00000041, /* Cluster: Thermostat, Attribute: ACCapacity, Privilege: manage */ \
    0x00000042, /* Cluster: Thermostat, Attribute: ACRefrigerantType, Privilege: manage */ \
    0x00000043, /* Cluster: Thermostat, Attribute: ACCompressorType, Privilege: manage */ \
    0x00000044, /* Cluster: Thermostat, Attribute: ACErrorCode, Privilege: manage */ \
    0x00000045, /* Cluster: Thermostat, Attribute: ACLouverPosition, Privilege: manage */ \
    0x00000047, /* Cluster: Thermostat, Attribute: ACCapacityformat, Privilege: manage */ \
    0x00000050, /* Cluster: Thermostat, Attribute: Presets, Privilege: manage */ \
    0x00000051, /* Cluster: Thermostat, Attribute: Schedules, Privilege: manage */ \
    0x00000001, /* Cluster: Thermostat User Interface Configuration, Attribute: KeypadLockout, Privilege: manage */ \
    0x00000002, /* Cluster: Thermostat User Interface Configuration, Attribute: ScheduleProgrammingVisibility, Privilege: manage */ \
    0x00000030, /* Cluster: Color Control, Attribute: WhitePointX, Privilege: manage */ \
    0x00000031, /* Cluster: Color Control, Attribute: WhitePointY, Privilege: manage */ \
    0x00000032, /* Cluster: Color Control, Attribute: ColorPointRX, Privilege: manage */ \
    0x00000033, /* Cluster: Color Control, Attribute: ColorPointRY, Privilege: manage */ \
    0x00000034, /* Cluster: Color Control, Attribute: ColorPointRIntensity, Privilege: manage */ \
    0x00000036, /* Cluster: Color Control, Attribute: ColorPointGX, Privilege: manage */ \
    0x00000037, /* Cluster: Color Control, Attribute: ColorPointGY, Privilege: manage */ \
    0x00000038, /* Cluster: Color Control, Attribute: ColorPointGIntensity, Privilege: manage */ \
    0x0000003A, /* Cluster: Color Control, Attribute: ColorPointBX, Privilege: manage */ \
    0x0000003B, /* Cluster: Color Control, Attribute: ColorPointBY, Privilege: manage */ \
    0x0000003C, /* Cluster: Color Control, Attribute: ColorPointBIntensity, Privilege: manage */ \
    0x00004010, /* Cluster: Color Control, Attribute: StartUpColorTemperatureMireds, Privilege: manage */ \
    0x00000010, /* Cluster: Ballast Configuration, Attribute: MinLevel, Privilege: manage */ \
    0x00000011, /* Cluster: Ballast Configuration, Attribute: MaxLevel, Privilege: manage */ \
    0x00000014, /* Cluster: Ballast Configuration, Attribute: IntrinsicBallastFactor, Privilege: manage */ \
    0x00000015, /* Cluster: Ballast Configuration, Attribute: BallastFactorAdjustment, Privilege: manage */ \
    0x00000030, /* Cluster: Ballast Configuration, Attribute: LampType, Privilege: manage */ \
    0x00000031, /* Cluster: Ballast Configuration, Attribute: LampManufacturer, Privilege: manage */ \
    0x00000032, /* Cluster: Ballast Configuration, Attribute: LampRatedHours, Privilege: manage */ \
    0x00000033, /* Cluster: Ballast Configuration, Attribute: LampBurnHours, Privilege: manage */ \
    0x00000034, /* Cluster: Ballast Configuration, Attribute: LampAlarmMode, Privilege: manage */ \
    0x00000035, /* Cluster: Ballast Configuration, Attribute: LampBurnHoursTripPoint, Privilege: manage */ \
    0x00000003, /* Cluster: Occupancy Sensing, Attribute: HoldTime, Privilege: manage */ \
    0x00000010, /* Cluster: Occupancy Sensing, Attribute: PIROccupiedToUnoccupiedDelay, Privilege: manage */ \
    0x00000011, /* Cluster: Occupancy Sensing, Attribute: PIRUnoccupiedToOccupiedDelay, Privilege: manage */ \
    0x00000012, /* Cluster: Occupancy Sensing, Attribute: PIRUnoccupiedToOccupiedThreshold, Privilege: manage */ \
    0x00000020, /* Cluster: Occupancy Sensing, Attribute: UltrasonicOccupiedToUnoccupiedDelay, Privilege: manage */ \
    0x00000021, /* Cluster: Occupancy Sensing, Attribute: UltrasonicUnoccupiedToOccupiedDelay, Privilege: manage */ \
    0x00000022, /* Cluster: Occupancy Sensing, Attribute: UltrasonicUnoccupiedToOccupiedThreshold, Privilege: manage */ \
    0x00000030, /* Cluster: Occupancy Sensing, Attribute: PhysicalContactOccupiedToUnoccupiedDelay, Privilege: manage */ \
    0x00000031, /* Cluster: Occupancy Sensing, Attribute: PhysicalContactUnoccupiedToOccupiedDelay, Privilege: manage */ \
    0x00000032, /* Cluster: Occupancy Sensing, Attribute: PhysicalContactUnoccupiedToOccupiedThreshold, Privilege: manage */ \
    0x00000000, /* Cluster: Thread Network Directory, Attribute: PreferredExtendedPanID, Privilege: manage */ \
}

// Parallel array data (cluster, attribute, *privilege*) for write attribute
#define GENERATED_ACCESS_WRITE_ATTRIBUTE__PRIVILEGE { \
    chip::Access::Privilege::kManage, /* Cluster: On/Off, Attribute: StartUpOnOff, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Level Control, Attribute: StartUpCurrentLevel, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Binding, Attribute: Binding, Privilege: manage */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Access Control, Attribute: ACL, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Access Control, Attribute: Extension, Privilege: administer */ \
    chip::Access::Privilege::kManage, /* Cluster: Basic Information, Attribute: NodeLabel, Privilege: manage */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Basic Information, Attribute: Location, Privilege: administer */ \
    chip::Access::Privilege::kManage, /* Cluster: Basic Information, Attribute: LocalConfigDisabled, Privilege: manage */ \
    chip::Access::Privilege::kAdminister, /* Cluster: OTA Software Update Requestor, Attribute: DefaultOTAProviders, Privilege: administer */ \
    chip::Access::Privilege::kManage, /* Cluster: Localization Configuration, Attribute: ActiveLocale, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Time Format Localization, Attribute: HourFormat, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Time Format Localization, Attribute: ActiveCalendarType, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Unit Localization, Attribute: TemperatureUnit, Privilege: manage */ \
    chip::Access::Privilege::kAdminister, /* Cluster: General Commissioning, Attribute: Breadcrumb, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Network Commissioning, Attribute: InterfaceEnabled, Privilege: administer */ \
    chip::Access::Privilege::kManage, /* Cluster: Group Key Management, Attribute: GroupKeyMap, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: User Label, Attribute: LabelList, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Smoke CO Alarm, Attribute: SmokeSensitivityLevel, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Energy EVSE, Attribute: UserMaximumChargeCurrent, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Energy EVSE, Attribute: RandomizationDelayWindow, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Energy EVSE, Attribute: ApproximateEVEfficiency, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Door Lock, Attribute: DoorOpenEvents, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Door Lock, Attribute: DoorClosedEvents, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Door Lock, Attribute: OpenPeriod, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Door Lock, Attribute: Language, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Door Lock, Attribute: LEDSettings, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Door Lock, Attribute: AutoRelockTime, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Door Lock, Attribute: SoundVolume, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Door Lock, Attribute: OperatingMode, Privilege: manage */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Attribute: EnableLocalProgramming, Privilege: administer */ \
    chip::Access::Privilege::kManage, /* Cluster: Door Lock, Attribute: EnableOneTouchLocking, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Door Lock, Attribute: EnableInsideStatusLED, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Door Lock, Attribute: EnablePrivacyModeButton, Privilege: manage */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Attribute: LocalProgrammingFeatures, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Attribute: WrongCodeEntryLimit, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Attribute: UserCodeTemporaryDisableTime, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Attribute: SendPINOverTheAir, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Attribute: RequirePINforRemoteOperation, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Attribute: ExpiringUserTimeout, Privilege: administer */ \
    chip::Access::Privilege::kManage, /* Cluster: Window Covering, Attribute: Mode, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Pump Configuration and Control, Attribute: LifetimeRunningHours, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Pump Configuration and Control, Attribute: LifetimeEnergyConsumed, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Pump Configuration and Control, Attribute: OperationMode, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Pump Configuration and Control, Attribute: ControlMode, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: HVACSystemTypeConfiguration, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: LocalTemperatureCalibration, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: MinHeatSetpointLimit, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: MaxHeatSetpointLimit, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: MinCoolSetpointLimit, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: MaxCoolSetpointLimit, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: MinSetpointDeadBand, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: RemoteSensing, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: ControlSequenceOfOperation, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: SystemMode, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: TemperatureSetpointHold, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: TemperatureSetpointHoldDuration, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: ThermostatProgrammingOperationMode, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: OccupiedSetback, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: UnoccupiedSetback, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: EmergencyHeatDelta, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: ACType, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: ACCapacity, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: ACRefrigerantType, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: ACCompressorType, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: ACErrorCode, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: ACLouverPosition, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: ACCapacityformat, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: Presets, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Attribute: Schedules, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat User Interface Configuration, Attribute: KeypadLockout, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat User Interface Configuration, Attribute: ScheduleProgrammingVisibility, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Color Control, Attribute: WhitePointX, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Color Control, Attribute: WhitePointY, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Color Control, Attribute: ColorPointRX, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Color Control, Attribute: ColorPointRY, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Color Control, Attribute: ColorPointRIntensity, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Color Control, Attribute: ColorPointGX, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Color Control, Attribute: ColorPointGY, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Color Control, Attribute: ColorPointGIntensity, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Color Control, Attribute: ColorPointBX, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Color Control, Attribute: ColorPointBY, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Color Control, Attribute: ColorPointBIntensity, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Color Control, Attribute: StartUpColorTemperatureMireds, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Ballast Configuration, Attribute: MinLevel, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Ballast Configuration, Attribute: MaxLevel, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Ballast Configuration, Attribute: IntrinsicBallastFactor, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Ballast Configuration, Attribute: BallastFactorAdjustment, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Ballast Configuration, Attribute: LampType, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Ballast Configuration, Attribute: LampManufacturer, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Ballast Configuration, Attribute: LampRatedHours, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Ballast Configuration, Attribute: LampBurnHours, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Ballast Configuration, Attribute: LampAlarmMode, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Ballast Configuration, Attribute: LampBurnHoursTripPoint, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Occupancy Sensing, Attribute: HoldTime, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Occupancy Sensing, Attribute: PIROccupiedToUnoccupiedDelay, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Occupancy Sensing, Attribute: PIRUnoccupiedToOccupiedDelay, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Occupancy Sensing, Attribute: PIRUnoccupiedToOccupiedThreshold, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Occupancy Sensing, Attribute: UltrasonicOccupiedToUnoccupiedDelay, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Occupancy Sensing, Attribute: UltrasonicUnoccupiedToOccupiedDelay, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Occupancy Sensing, Attribute: UltrasonicUnoccupiedToOccupiedThreshold, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Occupancy Sensing, Attribute: PhysicalContactOccupiedToUnoccupiedDelay, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Occupancy Sensing, Attribute: PhysicalContactUnoccupiedToOccupiedDelay, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Occupancy Sensing, Attribute: PhysicalContactUnoccupiedToOccupiedThreshold, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thread Network Directory, Attribute: PreferredExtendedPanID, Privilege: manage */ \
}

////////////////////////////////////////////////////////////////////////////////

// Parallel array data (*cluster*, command, privilege) for invoke command
#define GENERATED_ACCESS_INVOKE_COMMAND__CLUSTER { \
    0x00000003, /* Cluster: Identify, Command: Identify, Privilege: manage */ \
    0x00000003, /* Cluster: Identify, Command: TriggerEffect, Privilege: manage */ \
    0x00000004, /* Cluster: Groups, Command: AddGroup, Privilege: manage */ \
    0x00000004, /* Cluster: Groups, Command: RemoveGroup, Privilege: manage */ \
    0x00000004, /* Cluster: Groups, Command: RemoveAllGroups, Privilege: manage */ \
    0x00000004, /* Cluster: Groups, Command: AddGroupIfIdentifying, Privilege: manage */ \
    0x0000001F, /* Cluster: Access Control, Command: ReviewFabricRestrictions, Privilege: administer */ \
    0x00000030, /* Cluster: General Commissioning, Command: ArmFailSafe, Privilege: administer */ \
    0x00000030, /* Cluster: General Commissioning, Command: SetRegulatoryConfig, Privilege: administer */ \
    0x00000030, /* Cluster: General Commissioning, Command: CommissioningComplete, Privilege: administer */ \
    0x00000030, /* Cluster: General Commissioning, Command: SetTCAcknowledgements, Privilege: administer */ \
    0x00000031, /* Cluster: Network Commissioning, Command: ScanNetworks, Privilege: administer */ \
    0x00000031, /* Cluster: Network Commissioning, Command: AddOrUpdateWiFiNetwork, Privilege: administer */ \
    0x00000031, /* Cluster: Network Commissioning, Command: AddOrUpdateThreadNetwork, Privilege: administer */ \
    0x00000031, /* Cluster: Network Commissioning, Command: RemoveNetwork, Privilege: administer */ \
    0x00000031, /* Cluster: Network Commissioning, Command: ConnectNetwork, Privilege: administer */ \
    0x00000031, /* Cluster: Network Commissioning, Command: ReorderNetwork, Privilege: administer */ \
    0x00000031, /* Cluster: Network Commissioning, Command: QueryIdentity, Privilege: administer */ \
    0x00000033, /* Cluster: General Diagnostics, Command: TestEventTrigger, Privilege: manage */ \
    0x00000034, /* Cluster: Software Diagnostics, Command: ResetWatermarks, Privilege: manage */ \
    0x00000035, /* Cluster: Thread Network Diagnostics, Command: ResetCounts, Privilege: manage */ \
    0x00000037, /* Cluster: Ethernet Network Diagnostics, Command: ResetCounts, Privilege: manage */ \
    0x00000038, /* Cluster: Time Synchronization, Command: SetUTCTime, Privilege: administer */ \
    0x00000038, /* Cluster: Time Synchronization, Command: SetTrustedTimeSource, Privilege: administer */ \
    0x00000038, /* Cluster: Time Synchronization, Command: SetTimeZone, Privilege: manage */ \
    0x00000038, /* Cluster: Time Synchronization, Command: SetDSTOffset, Privilege: manage */ \
    0x00000038, /* Cluster: Time Synchronization, Command: SetDefaultNTP, Privilege: administer */ \
    0x0000003C, /* Cluster: Administrator Commissioning, Command: OpenCommissioningWindow, Privilege: administer */ \
    0x0000003C, /* Cluster: Administrator Commissioning, Command: OpenBasicCommissioningWindow, Privilege: administer */ \
    0x0000003C, /* Cluster: Administrator Commissioning, Command: RevokeCommissioning, Privilege: administer */ \
    0x0000003E, /* Cluster: Operational Credentials, Command: AttestationRequest, Privilege: administer */ \
    0x0000003E, /* Cluster: Operational Credentials, Command: CertificateChainRequest, Privilege: administer */ \
    0x0000003E, /* Cluster: Operational Credentials, Command: CSRRequest, Privilege: administer */ \
    0x0000003E, /* Cluster: Operational Credentials, Command: AddNOC, Privilege: administer */ \
    0x0000003E, /* Cluster: Operational Credentials, Command: UpdateNOC, Privilege: administer */ \
    0x0000003E, /* Cluster: Operational Credentials, Command: UpdateFabricLabel, Privilege: administer */ \
    0x0000003E, /* Cluster: Operational Credentials, Command: RemoveFabric, Privilege: administer */ \
    0x0000003E, /* Cluster: Operational Credentials, Command: AddTrustedRootCertificate, Privilege: administer */ \
    0x0000003F, /* Cluster: Group Key Management, Command: KeySetWrite, Privilege: administer */ \
    0x0000003F, /* Cluster: Group Key Management, Command: KeySetRead, Privilege: administer */ \
    0x0000003F, /* Cluster: Group Key Management, Command: KeySetRemove, Privilege: administer */ \
    0x0000003F, /* Cluster: Group Key Management, Command: KeySetReadAllIndices, Privilege: administer */ \
    0x00000046, /* Cluster: ICD Management, Command: RegisterClient, Privilege: manage */ \
    0x00000046, /* Cluster: ICD Management, Command: UnregisterClient, Privilege: manage */ \
    0x00000046, /* Cluster: ICD Management, Command: StayActiveRequest, Privilege: manage */ \
    0x00000062, /* Cluster: Scenes Management, Command: AddScene, Privilege: manage */ \
    0x00000062, /* Cluster: Scenes Management, Command: RemoveScene, Privilege: manage */ \
    0x00000062, /* Cluster: Scenes Management, Command: RemoveAllScenes, Privilege: manage */ \
    0x00000062, /* Cluster: Scenes Management, Command: StoreScene, Privilege: manage */ \
    0x00000094, /* Cluster: Water Heater Management, Command: Boost, Privilege: manage */ \
    0x00000094, /* Cluster: Water Heater Management, Command: CancelBoost, Privilege: manage */ \
    0x00000101, /* Cluster: Door Lock, Command: SetWeekDaySchedule, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Command: GetWeekDaySchedule, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Command: ClearWeekDaySchedule, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Command: SetYearDaySchedule, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Command: GetYearDaySchedule, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Command: ClearYearDaySchedule, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Command: SetHolidaySchedule, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Command: GetHolidaySchedule, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Command: ClearHolidaySchedule, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Command: SetUser, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Command: GetUser, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Command: ClearUser, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Command: SetCredential, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Command: GetCredentialStatus, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Command: ClearCredential, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Command: SetAliroReaderConfig, Privilege: administer */ \
    0x00000101, /* Cluster: Door Lock, Command: ClearAliroReaderConfig, Privilege: administer */ \
    0x00000201, /* Cluster: Thermostat, Command: SetWeeklySchedule, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Command: ClearWeeklySchedule, Privilege: manage */ \
    0x00000201, /* Cluster: Thermostat, Command: AtomicRequest, Privilege: manage */ \
    0x00000451, /* Cluster: Wi-Fi Network Management, Command: NetworkPassphraseRequest, Privilege: manage */ \
    0x00000452, /* Cluster: Thread Border Router Management, Command: GetActiveDatasetRequest, Privilege: manage */ \
    0x00000452, /* Cluster: Thread Border Router Management, Command: GetPendingDatasetRequest, Privilege: manage */ \
    0x00000452, /* Cluster: Thread Border Router Management, Command: SetActiveDatasetRequest, Privilege: manage */ \
    0x00000452, /* Cluster: Thread Border Router Management, Command: SetPendingDatasetRequest, Privilege: manage */ \
    0x00000453, /* Cluster: Thread Network Directory, Command: AddNetwork, Privilege: manage */ \
    0x00000453, /* Cluster: Thread Network Directory, Command: RemoveNetwork, Privilege: manage */ \
    0x00000507, /* Cluster: Media Input, Command: RenameInput, Privilege: manage */ \
    0x0000050B, /* Cluster: Audio Output, Command: RenameOutput, Privilege: manage */ \
    0x0000050E, /* Cluster: Account Login, Command: GetSetupPIN, Privilege: administer */ \
    0x0000050E, /* Cluster: Account Login, Command: Login, Privilege: administer */ \
    0x00000751, /* Cluster: Commissioner Control, Command: RequestCommissioningApproval, Privilege: manage */ \
    0x00000751, /* Cluster: Commissioner Control, Command: CommissionNode, Privilege: manage */ \
    0xFFF1FC06, /* Cluster: Fault Injection, Command: FailAtFault, Privilege: manage */ \
    0xFFF1FC06, /* Cluster: Fault Injection, Command: FailRandomlyAtFault, Privilege: manage */ \
}

// Parallel array data (cluster, *command*, privilege) for invoke command
#define GENERATED_ACCESS_INVOKE_COMMAND__COMMAND { \
    0x00000000, /* Cluster: Identify, Command: Identify, Privilege: manage */ \
    0x00000040, /* Cluster: Identify, Command: TriggerEffect, Privilege: manage */ \
    0x00000000, /* Cluster: Groups, Command: AddGroup, Privilege: manage */ \
    0x00000003, /* Cluster: Groups, Command: RemoveGroup, Privilege: manage */ \
    0x00000004, /* Cluster: Groups, Command: RemoveAllGroups, Privilege: manage */ \
    0x00000005, /* Cluster: Groups, Command: AddGroupIfIdentifying, Privilege: manage */ \
    0x00000000, /* Cluster: Access Control, Command: ReviewFabricRestrictions, Privilege: administer */ \
    0x00000000, /* Cluster: General Commissioning, Command: ArmFailSafe, Privilege: administer */ \
    0x00000002, /* Cluster: General Commissioning, Command: SetRegulatoryConfig, Privilege: administer */ \
    0x00000004, /* Cluster: General Commissioning, Command: CommissioningComplete, Privilege: administer */ \
    0x00000006, /* Cluster: General Commissioning, Command: SetTCAcknowledgements, Privilege: administer */ \
    0x00000000, /* Cluster: Network Commissioning, Command: ScanNetworks, Privilege: administer */ \
    0x00000002, /* Cluster: Network Commissioning, Command: AddOrUpdateWiFiNetwork, Privilege: administer */ \
    0x00000003, /* Cluster: Network Commissioning, Command: AddOrUpdateThreadNetwork, Privilege: administer */ \
    0x00000004, /* Cluster: Network Commissioning, Command: RemoveNetwork, Privilege: administer */ \
    0x00000006, /* Cluster: Network Commissioning, Command: ConnectNetwork, Privilege: administer */ \
    0x00000008, /* Cluster: Network Commissioning, Command: ReorderNetwork, Privilege: administer */ \
    0x00000009, /* Cluster: Network Commissioning, Command: QueryIdentity, Privilege: administer */ \
    0x00000000, /* Cluster: General Diagnostics, Command: TestEventTrigger, Privilege: manage */ \
    0x00000000, /* Cluster: Software Diagnostics, Command: ResetWatermarks, Privilege: manage */ \
    0x00000000, /* Cluster: Thread Network Diagnostics, Command: ResetCounts, Privilege: manage */ \
    0x00000000, /* Cluster: Ethernet Network Diagnostics, Command: ResetCounts, Privilege: manage */ \
    0x00000000, /* Cluster: Time Synchronization, Command: SetUTCTime, Privilege: administer */ \
    0x00000001, /* Cluster: Time Synchronization, Command: SetTrustedTimeSource, Privilege: administer */ \
    0x00000002, /* Cluster: Time Synchronization, Command: SetTimeZone, Privilege: manage */ \
    0x00000004, /* Cluster: Time Synchronization, Command: SetDSTOffset, Privilege: manage */ \
    0x00000005, /* Cluster: Time Synchronization, Command: SetDefaultNTP, Privilege: administer */ \
    0x00000000, /* Cluster: Administrator Commissioning, Command: OpenCommissioningWindow, Privilege: administer */ \
    0x00000001, /* Cluster: Administrator Commissioning, Command: OpenBasicCommissioningWindow, Privilege: administer */ \
    0x00000002, /* Cluster: Administrator Commissioning, Command: RevokeCommissioning, Privilege: administer */ \
    0x00000000, /* Cluster: Operational Credentials, Command: AttestationRequest, Privilege: administer */ \
    0x00000002, /* Cluster: Operational Credentials, Command: CertificateChainRequest, Privilege: administer */ \
    0x00000004, /* Cluster: Operational Credentials, Command: CSRRequest, Privilege: administer */ \
    0x00000006, /* Cluster: Operational Credentials, Command: AddNOC, Privilege: administer */ \
    0x00000007, /* Cluster: Operational Credentials, Command: UpdateNOC, Privilege: administer */ \
    0x00000009, /* Cluster: Operational Credentials, Command: UpdateFabricLabel, Privilege: administer */ \
    0x0000000A, /* Cluster: Operational Credentials, Command: RemoveFabric, Privilege: administer */ \
    0x0000000B, /* Cluster: Operational Credentials, Command: AddTrustedRootCertificate, Privilege: administer */ \
    0x00000000, /* Cluster: Group Key Management, Command: KeySetWrite, Privilege: administer */ \
    0x00000001, /* Cluster: Group Key Management, Command: KeySetRead, Privilege: administer */ \
    0x00000003, /* Cluster: Group Key Management, Command: KeySetRemove, Privilege: administer */ \
    0x00000004, /* Cluster: Group Key Management, Command: KeySetReadAllIndices, Privilege: administer */ \
    0x00000000, /* Cluster: ICD Management, Command: RegisterClient, Privilege: manage */ \
    0x00000002, /* Cluster: ICD Management, Command: UnregisterClient, Privilege: manage */ \
    0x00000003, /* Cluster: ICD Management, Command: StayActiveRequest, Privilege: manage */ \
    0x00000000, /* Cluster: Scenes Management, Command: AddScene, Privilege: manage */ \
    0x00000002, /* Cluster: Scenes Management, Command: RemoveScene, Privilege: manage */ \
    0x00000003, /* Cluster: Scenes Management, Command: RemoveAllScenes, Privilege: manage */ \
    0x00000004, /* Cluster: Scenes Management, Command: StoreScene, Privilege: manage */ \
    0x00000000, /* Cluster: Water Heater Management, Command: Boost, Privilege: manage */ \
    0x00000001, /* Cluster: Water Heater Management, Command: CancelBoost, Privilege: manage */ \
    0x0000000B, /* Cluster: Door Lock, Command: SetWeekDaySchedule, Privilege: administer */ \
    0x0000000C, /* Cluster: Door Lock, Command: GetWeekDaySchedule, Privilege: administer */ \
    0x0000000D, /* Cluster: Door Lock, Command: ClearWeekDaySchedule, Privilege: administer */ \
    0x0000000E, /* Cluster: Door Lock, Command: SetYearDaySchedule, Privilege: administer */ \
    0x0000000F, /* Cluster: Door Lock, Command: GetYearDaySchedule, Privilege: administer */ \
    0x00000010, /* Cluster: Door Lock, Command: ClearYearDaySchedule, Privilege: administer */ \
    0x00000011, /* Cluster: Door Lock, Command: SetHolidaySchedule, Privilege: administer */ \
    0x00000012, /* Cluster: Door Lock, Command: GetHolidaySchedule, Privilege: administer */ \
    0x00000013, /* Cluster: Door Lock, Command: ClearHolidaySchedule, Privilege: administer */ \
    0x0000001A, /* Cluster: Door Lock, Command: SetUser, Privilege: administer */ \
    0x0000001B, /* Cluster: Door Lock, Command: GetUser, Privilege: administer */ \
    0x0000001D, /* Cluster: Door Lock, Command: ClearUser, Privilege: administer */ \
    0x00000022, /* Cluster: Door Lock, Command: SetCredential, Privilege: administer */ \
    0x00000024, /* Cluster: Door Lock, Command: GetCredentialStatus, Privilege: administer */ \
    0x00000026, /* Cluster: Door Lock, Command: ClearCredential, Privilege: administer */ \
    0x00000028, /* Cluster: Door Lock, Command: SetAliroReaderConfig, Privilege: administer */ \
    0x00000029, /* Cluster: Door Lock, Command: ClearAliroReaderConfig, Privilege: administer */ \
    0x00000001, /* Cluster: Thermostat, Command: SetWeeklySchedule, Privilege: manage */ \
    0x00000003, /* Cluster: Thermostat, Command: ClearWeeklySchedule, Privilege: manage */ \
    0x000000FE, /* Cluster: Thermostat, Command: AtomicRequest, Privilege: manage */ \
    0x00000000, /* Cluster: Wi-Fi Network Management, Command: NetworkPassphraseRequest, Privilege: manage */ \
    0x00000000, /* Cluster: Thread Border Router Management, Command: GetActiveDatasetRequest, Privilege: manage */ \
    0x00000001, /* Cluster: Thread Border Router Management, Command: GetPendingDatasetRequest, Privilege: manage */ \
    0x00000003, /* Cluster: Thread Border Router Management, Command: SetActiveDatasetRequest, Privilege: manage */ \
    0x00000004, /* Cluster: Thread Border Router Management, Command: SetPendingDatasetRequest, Privilege: manage */ \
    0x00000000, /* Cluster: Thread Network Directory, Command: AddNetwork, Privilege: manage */ \
    0x00000001, /* Cluster: Thread Network Directory, Command: RemoveNetwork, Privilege: manage */ \
    0x00000003, /* Cluster: Media Input, Command: RenameInput, Privilege: manage */ \
    0x00000001, /* Cluster: Audio Output, Command: RenameOutput, Privilege: manage */ \
    0x00000000, /* Cluster: Account Login, Command: GetSetupPIN, Privilege: administer */ \
    0x00000002, /* Cluster: Account Login, Command: Login, Privilege: administer */ \
    0x00000000, /* Cluster: Commissioner Control, Command: RequestCommissioningApproval, Privilege: manage */ \
    0x00000001, /* Cluster: Commissioner Control, Command: CommissionNode, Privilege: manage */ \
    0x00000000, /* Cluster: Fault Injection, Command: FailAtFault, Privilege: manage */ \
    0x00000001, /* Cluster: Fault Injection, Command: FailRandomlyAtFault, Privilege: manage */ \
}

// Parallel array data (cluster, command, *privilege*) for invoke command
#define GENERATED_ACCESS_INVOKE_COMMAND__PRIVILEGE { \
    chip::Access::Privilege::kManage, /* Cluster: Identify, Command: Identify, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Identify, Command: TriggerEffect, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Groups, Command: AddGroup, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Groups, Command: RemoveGroup, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Groups, Command: RemoveAllGroups, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Groups, Command: AddGroupIfIdentifying, Privilege: manage */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Access Control, Command: ReviewFabricRestrictions, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: General Commissioning, Command: ArmFailSafe, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: General Commissioning, Command: SetRegulatoryConfig, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: General Commissioning, Command: CommissioningComplete, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: General Commissioning, Command: SetTCAcknowledgements, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Network Commissioning, Command: ScanNetworks, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Network Commissioning, Command: AddOrUpdateWiFiNetwork, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Network Commissioning, Command: AddOrUpdateThreadNetwork, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Network Commissioning, Command: RemoveNetwork, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Network Commissioning, Command: ConnectNetwork, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Network Commissioning, Command: ReorderNetwork, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Network Commissioning, Command: QueryIdentity, Privilege: administer */ \
    chip::Access::Privilege::kManage, /* Cluster: General Diagnostics, Command: TestEventTrigger, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Software Diagnostics, Command: ResetWatermarks, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thread Network Diagnostics, Command: ResetCounts, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Ethernet Network Diagnostics, Command: ResetCounts, Privilege: manage */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Time Synchronization, Command: SetUTCTime, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Time Synchronization, Command: SetTrustedTimeSource, Privilege: administer */ \
    chip::Access::Privilege::kManage, /* Cluster: Time Synchronization, Command: SetTimeZone, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Time Synchronization, Command: SetDSTOffset, Privilege: manage */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Time Synchronization, Command: SetDefaultNTP, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Administrator Commissioning, Command: OpenCommissioningWindow, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Administrator Commissioning, Command: OpenBasicCommissioningWindow, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Administrator Commissioning, Command: RevokeCommissioning, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Operational Credentials, Command: AttestationRequest, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Operational Credentials, Command: CertificateChainRequest, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Operational Credentials, Command: CSRRequest, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Operational Credentials, Command: AddNOC, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Operational Credentials, Command: UpdateNOC, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Operational Credentials, Command: UpdateFabricLabel, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Operational Credentials, Command: RemoveFabric, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Operational Credentials, Command: AddTrustedRootCertificate, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Group Key Management, Command: KeySetWrite, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Group Key Management, Command: KeySetRead, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Group Key Management, Command: KeySetRemove, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Group Key Management, Command: KeySetReadAllIndices, Privilege: administer */ \
    chip::Access::Privilege::kManage, /* Cluster: ICD Management, Command: RegisterClient, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: ICD Management, Command: UnregisterClient, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: ICD Management, Command: StayActiveRequest, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Scenes Management, Command: AddScene, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Scenes Management, Command: RemoveScene, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Scenes Management, Command: RemoveAllScenes, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Scenes Management, Command: StoreScene, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Water Heater Management, Command: Boost, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Water Heater Management, Command: CancelBoost, Privilege: manage */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Command: SetWeekDaySchedule, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Command: GetWeekDaySchedule, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Command: ClearWeekDaySchedule, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Command: SetYearDaySchedule, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Command: GetYearDaySchedule, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Command: ClearYearDaySchedule, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Command: SetHolidaySchedule, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Command: GetHolidaySchedule, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Command: ClearHolidaySchedule, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Command: SetUser, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Command: GetUser, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Command: ClearUser, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Command: SetCredential, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Command: GetCredentialStatus, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Command: ClearCredential, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Command: SetAliroReaderConfig, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Door Lock, Command: ClearAliroReaderConfig, Privilege: administer */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Command: SetWeeklySchedule, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Command: ClearWeeklySchedule, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thermostat, Command: AtomicRequest, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Wi-Fi Network Management, Command: NetworkPassphraseRequest, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thread Border Router Management, Command: GetActiveDatasetRequest, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thread Border Router Management, Command: GetPendingDatasetRequest, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thread Border Router Management, Command: SetActiveDatasetRequest, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thread Border Router Management, Command: SetPendingDatasetRequest, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thread Network Directory, Command: AddNetwork, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Thread Network Directory, Command: RemoveNetwork, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Media Input, Command: RenameInput, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Audio Output, Command: RenameOutput, Privilege: manage */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Account Login, Command: GetSetupPIN, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Account Login, Command: Login, Privilege: administer */ \
    chip::Access::Privilege::kManage, /* Cluster: Commissioner Control, Command: RequestCommissioningApproval, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Commissioner Control, Command: CommissionNode, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Fault Injection, Command: FailAtFault, Privilege: manage */ \
    chip::Access::Privilege::kManage, /* Cluster: Fault Injection, Command: FailRandomlyAtFault, Privilege: manage */ \
}

////////////////////////////////////////////////////////////////////////////////

// Parallel array data (*cluster*, event, privilege) for read event
#define GENERATED_ACCESS_READ_EVENT__CLUSTER { \
    0x0000001F, /* Cluster: Access Control, Event: AccessControlEntryChanged, Privilege: administer */ \
    0x0000001F, /* Cluster: Access Control, Event: AccessControlExtensionChanged, Privilege: administer */ \
    0x0000001F, /* Cluster: Access Control, Event: FabricRestrictionReviewUpdate, Privilege: administer */ \
    0x00000751, /* Cluster: Commissioner Control, Event: CommissioningRequestResult, Privilege: manage */ \
}

// Parallel array data (cluster, *event*, privilege) for read event
#define GENERATED_ACCESS_READ_EVENT__EVENT { \
    0x00000000, /* Cluster: Access Control, Event: AccessControlEntryChanged, Privilege: administer */ \
    0x00000001, /* Cluster: Access Control, Event: AccessControlExtensionChanged, Privilege: administer */ \
    0x00000002, /* Cluster: Access Control, Event: FabricRestrictionReviewUpdate, Privilege: administer */ \
    0x00000000, /* Cluster: Commissioner Control, Event: CommissioningRequestResult, Privilege: manage */ \
}

// Parallel array data (cluster, event, *privilege*) for read event
#define GENERATED_ACCESS_READ_EVENT__PRIVILEGE { \
    chip::Access::Privilege::kAdminister, /* Cluster: Access Control, Event: AccessControlEntryChanged, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Access Control, Event: AccessControlExtensionChanged, Privilege: administer */ \
    chip::Access::Privilege::kAdminister, /* Cluster: Access Control, Event: FabricRestrictionReviewUpdate, Privilege: administer */ \
    chip::Access::Privilege::kManage, /* Cluster: Commissioner Control, Event: CommissioningRequestResult, Privilege: manage */ \
}

////////////////////////////////////////////////////////////////////////////////

// clang-format on
