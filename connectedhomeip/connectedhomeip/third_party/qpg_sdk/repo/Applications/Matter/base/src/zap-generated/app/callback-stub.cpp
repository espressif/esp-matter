#include <app-common/zap-generated/callback.h>
#include <app-common/zap-generated/cluster-id.h>
#include <lib/support/Span.h>
#include <protocols/interaction_model/Constants.h>

using namespace chip;

// Cluster Init Functions
void emberAfClusterInitCallback(EndpointId endpoint, ClusterId clusterId)
{
    switch(clusterId)
    {
        case ZCL_ACCESS_CONTROL_CLUSTER_ID:
            emberAfAccessControlClusterInitCallback(endpoint);
            break;
        case ZCL_BASIC_CLUSTER_ID:
            emberAfBasicClusterInitCallback(endpoint);
            break;
        case ZCL_DESCRIPTOR_CLUSTER_ID:
            emberAfDescriptorClusterInitCallback(endpoint);
            break;
        case ZCL_GENERAL_COMMISSIONING_CLUSTER_ID:
            emberAfGeneralCommissioningClusterInitCallback(endpoint);
            break;
        case ZCL_GENERAL_DIAGNOSTICS_CLUSTER_ID:
            emberAfGeneralDiagnosticsClusterInitCallback(endpoint);
            break;
        case ZCL_GROUP_KEY_MANAGEMENT_CLUSTER_ID:
            emberAfGroupKeyManagementClusterInitCallback(endpoint);
            break;
        case ZCL_LOCALIZATION_CONFIGURATION_CLUSTER_ID:
            emberAfLocalizationConfigurationClusterInitCallback(endpoint);
            break;
        case ZCL_NETWORK_COMMISSIONING_CLUSTER_ID:
            emberAfNetworkCommissioningClusterInitCallback(endpoint);
            break;
        case ZCL_OPERATIONAL_CREDENTIALS_CLUSTER_ID:
            emberAfOperationalCredentialsClusterInitCallback(endpoint);
            break;
        case ZCL_OTA_SOFTWARE_UPDATE_PROVIDER_CLUSTER_ID:
            emberAfOtaSoftwareUpdateProviderClusterInitCallback(endpoint);
            break;
        case ZCL_OTA_SOFTWARE_UPDATE_REQUESTOR_CLUSTER_ID:
            emberAfOtaSoftwareUpdateRequestorClusterInitCallback(endpoint);
            break;
        case ZCL_TIME_FORMAT_LOCALIZATION_CLUSTER_ID:
            emberAfTimeFormatLocalizationClusterInitCallback(endpoint);
            break;
        case ZCL_UNIT_LOCALIZATION_CLUSTER_ID:
            emberAfUnitLocalizationClusterInitCallback(endpoint);
            break;
        default:
            // Unrecognized cluster ID
            break;
    }
}
void __attribute__((weak)) emberAfAccessControlClusterInitCallback(EndpointId endpoint)
{
    // To prevent warning
    (void)endpoint;
}
void __attribute__((weak)) emberAfBasicClusterInitCallback(EndpointId endpoint)
{
    // To prevent warning
    (void)endpoint;
}
void __attribute__((weak)) emberAfDescriptorClusterInitCallback(EndpointId endpoint)
{
    // To prevent warning
    (void)endpoint;
}
void __attribute__((weak)) emberAfGeneralCommissioningClusterInitCallback(EndpointId endpoint)
{
    // To prevent warning
    (void)endpoint;
}
void __attribute__((weak)) emberAfGeneralDiagnosticsClusterInitCallback(EndpointId endpoint)
{
    // To prevent warning
    (void)endpoint;
}
void __attribute__((weak)) emberAfGroupKeyManagementClusterInitCallback(EndpointId endpoint)
{
    // To prevent warning
    (void)endpoint;
}
void __attribute__((weak)) emberAfLocalizationConfigurationClusterInitCallback(EndpointId endpoint)
{
    // To prevent warning
    (void)endpoint;
}
void __attribute__((weak)) emberAfNetworkCommissioningClusterInitCallback(EndpointId endpoint)
{
    // To prevent warning
    (void)endpoint;
}
void __attribute__((weak)) emberAfOperationalCredentialsClusterInitCallback(EndpointId endpoint)
{
    // To prevent warning
    (void)endpoint;
}
void __attribute__((weak)) emberAfOtaSoftwareUpdateProviderClusterInitCallback(EndpointId endpoint)
{
    // To prevent warning
    (void)endpoint;
}
void __attribute__((weak)) emberAfOtaSoftwareUpdateRequestorClusterInitCallback(EndpointId endpoint)
{
    // To prevent warning
    (void)endpoint;
}
void __attribute__((weak)) emberAfTimeFormatLocalizationClusterInitCallback(EndpointId endpoint)
{
    // To prevent warning
    (void)endpoint;
}
void __attribute__((weak)) emberAfUnitLocalizationClusterInitCallback(EndpointId endpoint)
{
    // To prevent warning
    (void)endpoint;
}
