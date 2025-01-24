// Copyright 2024 Espressif Systems (Shanghai) PTE LTD
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

#include <sdkconfig.h>

#include <esp_check.h>
#include <esp_err.h>
#include <esp_matter_controller_client.h>
#include <esp_matter_controller_credentials_issuer.h>
#include <esp_matter_controller_pairing_command.h>

#include <controller/CHIPDeviceControllerFactory.h>
#include <controller/OperationalCredentialsDelegate.h>
#include <credentials/GroupDataProvider.h>
#include <credentials/attestation_verifier/DefaultDeviceAttestationVerifier.h>
#include <credentials/attestation_verifier/DeviceAttestationVerifier.h>
#include <crypto/CHIPCryptoPAL.h>
#include <data-model-providers/codegen/Instance.h>
#include <lib/core/CHIPError.h>
#include <lib/core/DataModelTypes.h>
#include <lib/support/CHIPMem.h>
#include <lib/support/ScopedBuffer.h>
#include <lib/support/Span.h>
#include <lib/support/TestGroupData.h>
#include <stdint.h>

#ifdef CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
#include <esp_matter_attestation_trust_store.h>
#endif

#if CONFIG_ENABLE_ESP32_BLE_CONTROLLER
#include <platform/ESP32/BLEManagerImpl.h>
#include <platform/internal/BLEManager.h>
#endif

#define TAG "MatterController"

using chip::Platform::ScopedMemoryBufferWithSize;

namespace esp_matter {
namespace controller {

#if CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY
ESPCommissionerCallback commissioner_callback;
#endif

esp_err_t matter_controller_client::init(NodeId node_id, FabricId fabric_id, uint16_t listen_port)
{
    chip::Controller::FactoryInitParams factory_init_params;
    ESP_RETURN_ON_FALSE(m_operational_keystore.Init(&m_default_storage) == CHIP_NO_ERROR, ESP_FAIL, TAG,
                        "Failed to initialize operational keystore");
    ESP_RETURN_ON_FALSE(m_operational_cert_store.Init(&m_default_storage) == CHIP_NO_ERROR, ESP_FAIL, TAG,
                        "Failed to initialize operational cert store");
    //    ESP_RETURN_ON_FALSE(m_icd_client_storage.Init(&m_default_storage, &m_session_key_store) == CHIP_NO_ERROR,
    //                        ESP_FAIL, TAG, "Failed to initialize ICD client store");
    factory_init_params.listenPort = listen_port;
    factory_init_params.fabricIndependentStorage = &m_default_storage;
    factory_init_params.operationalKeystore = &m_operational_keystore;
    factory_init_params.opCertStore = &m_operational_cert_store;
    factory_init_params.enableServerInteractions = m_operational_advertising;
    factory_init_params.sessionKeystore = &m_session_key_store;
    factory_init_params.dataModelProvider = chip::app::CodegenDataModelProviderInstance(&m_default_storage);
    m_controller_node_id = node_id;
    m_controller_fabric_id = fabric_id;

    m_group_data_provider.SetStorageDelegate(&m_default_storage);
    m_group_data_provider.SetSessionKeystore(factory_init_params.sessionKeystore);
    ESP_RETURN_ON_FALSE(m_group_data_provider.Init() == CHIP_NO_ERROR, ESP_FAIL, TAG,
                        "Failed to initialize group data provider");
    factory_init_params.groupDataProvider =
        reinterpret_cast<chip::Credentials::GroupDataProvider *>(&m_group_data_provider);
    chip::Credentials::SetGroupDataProvider(factory_init_params.groupDataProvider);
    ESP_RETURN_ON_FALSE(chip::Controller::DeviceControllerFactory::GetInstance().Init(factory_init_params) ==
                            CHIP_NO_ERROR,
                        ESP_FAIL, TAG, "Failed to initialize DeviceControllerFactory");
    return ESP_OK;
}

#ifdef CONFIG_ESP_MATTER_COMMISSIONER_ENABLE
esp_err_t matter_controller_client::setup_commissioner()
{
#if CONFIG_ENABLE_ESP32_BLE_CONTROLLER
    CHIP_ERROR err = chip::DeviceLayer::Internal::BLEMgr().Init();
    // This function will return CHIP_ERROR_INCORRECT_STATE if BLE Manager is already initialized.
    ESP_RETURN_ON_FALSE(err == CHIP_NO_ERROR || err == CHIP_ERROR_INCORRECT_STATE, ESP_FAIL, TAG,
                        "Failed to initialze the BLE manager");
    ESP_RETURN_ON_FALSE(chip::DeviceLayer::Internal::BLEMgrImpl().ConfigureBle(0, true) == CHIP_NO_ERROR, ESP_FAIL, TAG,
                        "Failed to configure BLEManager");
#endif
    chip::Controller::SetupParams commissioner_params;
    const chip::Credentials::AttestationTrustStore *trust_store = chip::Credentials::get_attestation_trust_store();
    chip::Credentials::DeviceAttestationVerifier *dac_verifier = chip::Credentials::GetDefaultDACVerifier(trust_store);
    chip::Credentials::SetDeviceAttestationVerifier(dac_verifier);
    commissioner_params.deviceAttestationVerifier = dac_verifier;
    m_credentials_issuer = get_credentials_issuer();
    ESP_RETURN_ON_FALSE(m_credentials_issuer, ESP_FAIL, TAG,
                        "Please set the custom credentials_issuer before calling setup_commissioner");
    ESP_RETURN_ON_ERROR(m_credentials_issuer->initialize_credentials_issuer(m_default_storage), TAG,
                        "Failed to initialize credentials_issuer");
    commissioner_params.operationalCredentialsDelegate = m_credentials_issuer->get_delegate();
    commissioner_params.controllerVendorId = chip::VendorId((uint16_t)CONFIG_ESP_MATTER_CONTROLLER_VENDOR_ID);

    // Commissioner NOC chain
    ScopedMemoryBufferWithSize<uint8_t> noc;
    noc.Calloc(chip::Controller::kMaxCHIPDERCertLength);
    ESP_RETURN_ON_FALSE(noc.Get(), ESP_ERR_NO_MEM, TAG, "Failed allocate memory for noc");
    chip::MutableByteSpan noc_span(noc.Get(), chip::Controller::kMaxCHIPDERCertLength);
    ScopedMemoryBufferWithSize<uint8_t> icac;
    icac.Calloc(chip::Controller::kMaxCHIPDERCertLength);
    ESP_RETURN_ON_FALSE(icac.Get(), ESP_ERR_NO_MEM, TAG, "Failed allocate memory for icac");
    chip::MutableByteSpan icac_span(icac.Get(), chip::Controller::kMaxCHIPDERCertLength);
    ScopedMemoryBufferWithSize<uint8_t> rcac;
    rcac.Calloc(chip::Controller::kMaxCHIPDERCertLength);
    ESP_RETURN_ON_FALSE(rcac.Get(), ESP_ERR_NO_MEM, TAG, "Failed allocate memory for rcac");
    chip::MutableByteSpan rcac_span(rcac.Get(), chip::Controller::kMaxCHIPDERCertLength);
    // NOC Keypair
    chip::Crypto::P256Keypair ephemeral_key;
    ESP_RETURN_ON_FALSE(ephemeral_key.Initialize(chip::Crypto::ECPKeyTarget::ECDSA) == CHIP_NO_ERROR, ESP_FAIL, TAG,
                        "Failed to initialize ephemeral_key pair");
    ESP_RETURN_ON_ERROR(m_credentials_issuer->generate_controller_noc_chain(m_controller_node_id,
                                                                            m_controller_fabric_id, ephemeral_key,
                                                                            rcac_span, icac_span, noc_span),
                        TAG, "Failed to generate NOC chain");
    commissioner_params.operationalKeypair = &ephemeral_key;
    commissioner_params.controllerRCAC = rcac_span;
    commissioner_params.controllerICAC = icac_span;
    commissioner_params.controllerNOC = noc_span;
    commissioner_params.defaultCommissioner = &m_auto_commissioner;
    commissioner_params.enableServerInteractions = m_operational_advertising;
    auto &factory = chip::Controller::DeviceControllerFactory::GetInstance();
    ESP_RETURN_ON_FALSE(factory.SetupCommissioner(commissioner_params, m_device_commissioner) == CHIP_NO_ERROR,
                        ESP_FAIL, TAG, "Failed to setup commissioner");

    // Initialize Group Data, including IPK
    chip::FabricIndex fabric_index = m_device_commissioner.GetFabricIndex();
    ESP_RETURN_ON_FALSE(fabric_index != chip::kUndefinedFabricIndex, ESP_FAIL, TAG, "Invalid Fabric Index");
    uint8_t compressed_fabric_id[sizeof(uint64_t)] = {0};
    chip::MutableByteSpan compressed_fabric_id_span(compressed_fabric_id);
    ESP_RETURN_ON_FALSE(m_device_commissioner.GetCompressedFabricIdBytes(compressed_fabric_id_span) == CHIP_NO_ERROR,
                        ESP_FAIL, TAG, "Failed to get compressed_fabric_id");
    chip::Credentials::GroupDataProvider *group_data_provider =
        reinterpret_cast<chip::Credentials::GroupDataProvider *>(&m_group_data_provider);
    ESP_RETURN_ON_FALSE(chip::GroupTesting::InitData(group_data_provider, fabric_index, compressed_fabric_id_span) ==
                            CHIP_NO_ERROR,
                        ESP_FAIL, TAG, "Failed to initialize group data");
    chip::ByteSpan default_ipk = chip::GroupTesting::DefaultIpkValue::GetDefaultIpk();
    ESP_RETURN_ON_FALSE(chip::Credentials::SetSingleIpkEpochKey(group_data_provider, fabric_index, default_ipk,
                                                                compressed_fabric_id_span) == CHIP_NO_ERROR,
                        ESP_FAIL, TAG, "Failed to set ipk for commissioner fabric");
    // m_icd_client_storage.UpdateFabricList(fabric_index);

#if CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY
    get_discovery_controller()->SetUserDirectedCommissioningServer(get_commissioner()->GetUserDirectedCommissioningServer());
    get_discovery_controller()->SetCommissionerCallback(&commissioner_callback);
#endif

    m_device_commissioner.RegisterPairingDelegate(&controller::pairing_command::get_instance());

    return ESP_OK;
}

#else
esp_err_t matter_controller_client::setup_controller(chip::MutableByteSpan &ipk)
{
    chip::Controller::SetupParams controller_params;
    // Controller doesn't need to verify device attestation. Set deviceAttestationVerifier to null.
    controller_params.deviceAttestationVerifier = nullptr;
    m_credentials_issuer = get_credentials_issuer();
    ESP_RETURN_ON_FALSE(m_credentials_issuer, ESP_FAIL, TAG,
                        "Please set the custom credentials_issuer before calling setup_controller");
    ESP_RETURN_ON_ERROR(m_credentials_issuer->initialize_credentials_issuer(m_default_storage), TAG,
                        "Failed to initialize credentials_issuer");
    controller_params.operationalCredentialsDelegate = m_credentials_issuer->get_delegate();
    controller_params.controllerVendorId = chip::VendorId((uint16_t)CONFIG_ESP_MATTER_CONTROLLER_VENDOR_ID);
    // Commissioner NOC chain
    ScopedMemoryBufferWithSize<uint8_t> noc;
    noc.Calloc(chip::Controller::kMaxCHIPDERCertLength);
    ESP_RETURN_ON_FALSE(noc.Get(), ESP_ERR_NO_MEM, TAG, "Failed allocate memory for noc");
    chip::MutableByteSpan noc_span(noc.Get(), chip::Controller::kMaxCHIPDERCertLength);
    ScopedMemoryBufferWithSize<uint8_t> icac;
    icac.Calloc(chip::Controller::kMaxCHIPDERCertLength);
    ESP_RETURN_ON_FALSE(icac.Get(), ESP_ERR_NO_MEM, TAG, "Failed allocate memory for icac");
    chip::MutableByteSpan icac_span(icac.Get(), chip::Controller::kMaxCHIPDERCertLength);
    ScopedMemoryBufferWithSize<uint8_t> rcac;
    rcac.Calloc(chip::Controller::kMaxCHIPDERCertLength);
    ESP_RETURN_ON_FALSE(rcac.Get(), ESP_ERR_NO_MEM, TAG, "Failed allocate memory for rcac");
    chip::MutableByteSpan rcac_span(rcac.Get(), chip::Controller::kMaxCHIPDERCertLength);
    chip::Crypto::P256Keypair ephemeral_key;
    ESP_RETURN_ON_ERROR(m_credentials_issuer->generate_controller_noc_chain(m_controller_node_id,
                                                                            m_controller_fabric_id, ephemeral_key,
                                                                            rcac_span, icac_span, noc_span),
                        TAG, "Failed to generate NOC chain");
    // Check whether the keypair is initialized in generate_controller_noc_chain
    bool is_keypair_initialized = false;
    {
        chip::Crypto::P256ECDSASignature signature;
        is_keypair_initialized = ephemeral_key.ECDSA_sign_msg(NULL, 0, signature) != CHIP_ERROR_UNINITIALIZED;
    }
    // If not initialized, use an empty keypair.
    controller_params.operationalKeypair = is_keypair_initialized ? &ephemeral_key : nullptr;
    controller_params.controllerRCAC = rcac_span;
    controller_params.controllerICAC = icac_span;
    controller_params.controllerNOC = noc_span;
    controller_params.defaultCommissioner = nullptr;
    controller_params.enableServerInteractions = m_operational_advertising;
    controller_params.permitMultiControllerFabrics = false;
    auto &factory = chip::Controller::DeviceControllerFactory::GetInstance();
    ESP_RETURN_ON_FALSE(factory.SetupController(controller_params, m_device_controller) == CHIP_NO_ERROR, ESP_FAIL, TAG,
                        "Failed to setup controller");

    chip::FabricIndex fabric_index = m_device_controller.GetFabricIndex();
    if (fabric_index != chip::kUndefinedFabricIndex && !ipk.empty()) {
        // If we have created fabric in SetupController and IPK input is not empty, initialize Group Data with IPK.
        // Otherwise we will initialize Group Data with IPK later.
        uint8_t compressed_fabric_id[sizeof(uint64_t)] = {0};
        chip::MutableByteSpan compressed_fabric_id_span(compressed_fabric_id);
        ESP_RETURN_ON_FALSE(m_device_controller.GetCompressedFabricIdBytes(compressed_fabric_id_span) == CHIP_NO_ERROR,
                            ESP_FAIL, TAG, "Failed to get compressed_fabric_id");
        chip::Credentials::GroupDataProvider *group_data_provider =
            reinterpret_cast<chip::Credentials::GroupDataProvider *>(&m_group_data_provider);
        chip::Credentials::GroupDataProvider::KeySet keyset;
        keyset.keyset_id = chip::Credentials::GroupDataProvider::kIdentityProtectionKeySetId;
        keyset.policy = chip::app::Clusters::GroupKeyManagement::GroupKeySecurityPolicyEnum::kTrustFirst;
        keyset.num_keys_used = 1;
        keyset.epoch_keys[0].start_time = 0;
        memcpy(keyset.epoch_keys[0].key, ipk.data(), chip::Crypto::CHIP_CRYPTO_SYMMETRIC_KEY_LENGTH_BYTES);
        ESP_RETURN_ON_FALSE(group_data_provider->SetKeySet(fabric_index, compressed_fabric_id_span, keyset) ==
                                CHIP_NO_ERROR,
                            ESP_FAIL, TAG, "Failed to set ipk for commissioner fabric");
    }

    return ESP_OK;
}
#endif // CONFIG_ESP_MATTER_COMMISSIONER_ENABLE

} // namespace controller
} // namespace esp_matter
