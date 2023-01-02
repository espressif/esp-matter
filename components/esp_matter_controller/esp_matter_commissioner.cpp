// Copyright 2022 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <app/clusters/network-commissioning/network-commissioning.h>
#include <app/server/Dnssd.h>
#include <app/server/OnboardingCodesUtil.h>
#include <app/server/Server.h>
#include <controller/CHIPDeviceControllerFactory.h>
#include <controller/ExampleOperationalCredentialsIssuer.h>
#include <credentials/GroupDataProviderImpl.h>
#include <credentials/attestation_verifier/DefaultDeviceAttestationVerifier.h>
#include <crypto/CHIPCryptoPAL.h>
#include <esp_heap_caps.h>
#include <esp_matter_commissioner.h>
#include <esp_matter_controller_pairing_command.h>
#include <lib/support/TestGroupData.h>
#include <platform/DeviceInstanceInfoProvider.h>
#include <platform/DiagnosticDataProvider.h>

using namespace chip;
using namespace chip::Credentials;
using namespace chip::DeviceLayer;
using namespace chip::Inet;
using namespace chip::Transport;
using namespace chip::app::Clusters;
using namespace chip::Controller;
using namespace chip::Messaging;
using namespace esp_matter::controller;

class ControllerServerStorageDelegate : public PersistentStorageDelegate {
    CHIP_ERROR SyncGetKeyValue(const char *key, void *buffer, uint16_t &size) override
    {
        ChipLogProgress(AppServer, "Retrieving value from controller server storage.");
        size_t bytesRead = 0;
        CHIP_ERROR err = PersistedStorage::KeyValueStoreMgr().Get(key, buffer, size, &bytesRead);

        if (err == CHIP_NO_ERROR) {
            ChipLogProgress(AppServer, "Retrieved value from server storage.");
        }
        size = static_cast<uint16_t>(bytesRead);
        return err;
    }

    CHIP_ERROR SyncSetKeyValue(const char *key, const void *value, uint16_t size) override
    {
        ChipLogProgress(AppServer, "Stored value in server storage");
        return PersistedStorage::KeyValueStoreMgr().Put(key, value, size);
    }

    CHIP_ERROR SyncDeleteKeyValue(const char *key) override
    {
        ChipLogProgress(AppServer, "Delete value in server storage");
        return PersistedStorage::KeyValueStoreMgr().Delete(key);
    }
};

constexpr uint64_t LOCAL_NODE_ID = 112233;

ControllerServerStorageDelegate controller_server_storage;
ExampleOperationalCredentialsIssuer op_creds_issuer;
NodeId local_node_id = LOCAL_NODE_ID;
Credentials::GroupDataProviderImpl group_data_provider;
AutoCommissioner auto_commissioner;

class ESPCommissionerCallback : public CommissionerCallback {
    void ReadyForCommissioning(uint32_t pincode, uint16_t discriminator, PeerAddress peerAddress) override
    {
        esp_matter::controller::pairing_on_network(pincode, pairing_command::get_instance().m_remote_node_id);
    }
};

ESPCommissionerCallback commissioner_callback;
DeviceCommissioner device_commissioner;
CommissionerDiscoveryController commissioner_discovery_controller;

constexpr uint16_t kUdcListenPort = 5560;

namespace esp_matter {
namespace commissioner {
esp_err_t init(uint16_t commissioner_port)
{
    Controller::FactoryInitParams factoryParams;
    Controller::SetupParams setupParams;

    // use a different listen port for the commissioner
    factoryParams.listenPort = commissioner_port;
    factoryParams.fabricIndependentStorage = &controller_server_storage;
    factoryParams.fabricTable = &Server::GetInstance().GetFabricTable();

    group_data_provider.SetStorageDelegate(&controller_server_storage);
    if (group_data_provider.Init() != CHIP_NO_ERROR) {
        return ESP_FAIL;
    }
    factoryParams.groupDataProvider = reinterpret_cast<Credentials::GroupDataProvider *>(&group_data_provider);

    setupParams.operationalCredentialsDelegate = &op_creds_issuer;
    uint16_t vendor_id;
    DeviceLayer::GetDeviceInstanceInfoProvider()->GetVendorId(vendor_id);
    ChipLogProgress(Support, " ----- Commissioner using vendorId 0x%04X", vendor_id);
    setupParams.controllerVendorId = static_cast<VendorId>(vendor_id);

    if (op_creds_issuer.Initialize(controller_server_storage) != CHIP_NO_ERROR) {
        return ESP_FAIL;
    }

    ChipLogProgress(Support, " ----- UDC listening on port %d", kUdcListenPort);
    if (device_commissioner.SetUdcListenPort(kUdcListenPort) != CHIP_NO_ERROR) {
        return ESP_FAIL;
    }

    // TODO: Root Store using spiffs
    const Credentials::AttestationTrustStore *testingRootStore = Credentials::GetTestAttestationTrustStore();
    SetDeviceAttestationVerifier(GetDefaultDACVerifier(testingRootStore));

    Platform::ScopedMemoryBuffer<uint8_t> noc;
    if (!noc.Alloc(Controller::kMaxCHIPDERCertLength)) {
        return ESP_ERR_NO_MEM;
    }
    MutableByteSpan nocSpan(noc.Get(), Controller::kMaxCHIPDERCertLength);

    Platform::ScopedMemoryBuffer<uint8_t> icac;
    if (!icac.Alloc(Controller::kMaxCHIPDERCertLength)) {
        return ESP_ERR_NO_MEM;
    }
    MutableByteSpan icacSpan(icac.Get(), Controller::kMaxCHIPDERCertLength);

    Platform::ScopedMemoryBuffer<uint8_t> rcac;
    if (!rcac.Alloc(Controller::kMaxCHIPDERCertLength)) {
        return ESP_ERR_NO_MEM;
    }
    MutableByteSpan rcacSpan(rcac.Get(), Controller::kMaxCHIPDERCertLength);

    Crypto::P256Keypair ephemeralKey;
    if (ephemeralKey.Initialize(Crypto::ECPKeyTarget::ECDSA) != CHIP_NO_ERROR) {
        return ESP_FAIL;
    }

    if (op_creds_issuer.GenerateNOCChainAfterValidation(local_node_id, /* fabricId = */ 1, chip::kUndefinedCATs,
                                                        ephemeralKey.Pubkey(), rcacSpan, icacSpan,
                                                        nocSpan) != CHIP_NO_ERROR) {
        return ESP_FAIL;
    }
    setupParams.operationalKeypair = &ephemeralKey;
    setupParams.controllerRCAC = rcacSpan;
    setupParams.controllerICAC = icacSpan;
    setupParams.controllerNOC = nocSpan;
    setupParams.defaultCommissioner = &auto_commissioner;

    auto &factory = Controller::DeviceControllerFactory::GetInstance();
    if (factory.Init(factoryParams) != CHIP_NO_ERROR) {
        return ESP_FAIL;
    }
    if (factory.SetupCommissioner(setupParams, *get_device_commissioner()) != CHIP_NO_ERROR) {
        return ESP_FAIL;
    }
    // Init InteractionModelEngine
    chip::app::InteractionModelEngine::GetInstance()->Init(&Server::GetInstance().GetExchangeManager(),
                                                           &Server::GetInstance().GetFabricTable(),
                                                           Server::GetInstance().GetCASESessionManager());

    FabricIndex fabricIndex = device_commissioner.GetFabricIndex();
    if (fabricIndex == kUndefinedFabricIndex) {
        return ESP_FAIL;
    }

    uint8_t compressedFabricId[sizeof(uint64_t)] = {0};
    MutableByteSpan compressedFabricIdSpan(compressedFabricId);
    if (device_commissioner.GetCompressedFabricIdBytes(compressedFabricIdSpan) != CHIP_NO_ERROR) {
        return ESP_FAIL;
    }
    ChipLogProgress(Support, "Setting up group data for Fabric Index %u with Compressed Fabric ID:",
                    static_cast<unsigned>(fabricIndex));
    ChipLogByteSpan(Support, compressedFabricIdSpan);

    // TODO: Once ExampleOperationalCredentialsIssuer has support, set default IPK on it as well so
    // that commissioned devices get the IPK set from real values rather than "test-only" internal hookups.
    ByteSpan defaultIpk = chip::GroupTesting::DefaultIpkValue::GetDefaultIpk();
    if (CHIP_NO_ERROR !=
        chip::Credentials::SetSingleIpkEpochKey(reinterpret_cast<GroupDataProvider *>(&group_data_provider),
                                                fabricIndex, defaultIpk, compressedFabricIdSpan)) {
        return ESP_FAIL;
    }

    get_discovery_controller()->SetUserDirectedCommissioningServer(
        get_device_commissioner()->GetUserDirectedCommissioningServer());
    get_discovery_controller()->SetCommissionerCallback(&commissioner_callback);

    get_device_commissioner()->RegisterPairingDelegate(&pairing_command::get_instance());

    ChipLogProgress(Support, "InitCommissioner nodeId=0x" ChipLogFormatX64 " fabricIndex=0x%x",
                    ChipLogValueX64(get_device_commissioner()->GetNodeId()), static_cast<unsigned>(fabricIndex));

    return ESP_OK;
}

void shutdown()
{
    UserDirectedCommissioningServer *udcServer = get_device_commissioner()->GetUserDirectedCommissioningServer();
    if (udcServer != nullptr) {
        udcServer->SetUserConfirmationProvider(nullptr);
    }

    get_device_commissioner()->Shutdown();
}

DeviceCommissioner *get_device_commissioner()
{
    return &device_commissioner;
}

CommissionerDiscoveryController *get_discovery_controller()
{
    return &commissioner_discovery_controller;
}

AutoCommissioner *get_auto_commissioner()
{
    return &auto_commissioner;
}

} // namespace commissioner
} // namespace esp_matter
