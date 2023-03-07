#pragma once
void MatterAccessControlPluginServerInitCallback();
void MatterBasicPluginServerInitCallback();
void MatterDescriptorPluginServerInitCallback();
void MatterGeneralCommissioningPluginServerInitCallback();
void MatterGeneralDiagnosticsPluginServerInitCallback();
void MatterGroupKeyManagementPluginServerInitCallback();
void MatterLocalizationConfigurationPluginServerInitCallback();
void MatterNetworkCommissioningPluginServerInitCallback();
void MatterOperationalCredentialsPluginServerInitCallback();
void MatterOtaSoftwareUpdateRequestorPluginServerInitCallback();
void MatterTimeFormatLocalizationPluginServerInitCallback();
void MatterUnitLocalizationPluginServerInitCallback();

#define MATTER_PLUGINS_INIT                                     \
    MatterAccessControlPluginServerInitCallback();              \
    MatterBasicPluginServerInitCallback();                      \
    MatterDescriptorPluginServerInitCallback();                 \
    MatterGeneralCommissioningPluginServerInitCallback();       \
    MatterGeneralDiagnosticsPluginServerInitCallback();         \
    MatterGroupKeyManagementPluginServerInitCallback();         \
    MatterLocalizationConfigurationPluginServerInitCallback();  \
    MatterNetworkCommissioningPluginServerInitCallback();       \
    MatterOperationalCredentialsPluginServerInitCallback();     \
    MatterOtaSoftwareUpdateRequestorPluginServerInitCallback(); \
    MatterTimeFormatLocalizationPluginServerInitCallback();     \
    MatterUnitLocalizationPluginServerInitCallback();
