// Copyright 2026 Espressif Systems (Shanghai) PTE LTD
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

#include <optional>
#include <vector>
#include <platform/CHIPDeviceLayer.h>
#include <controller/CHIPDeviceController.h>
#include <crypto/CHIPCryptoPAL.h>
#include <esp_log.h>
#include <esp_matter_controller_client.h>
#include <esp_matter_controller_pairing_command.h>
#include <lib/support/ThreadOperationalDataset.h>
#include <setup_payload/SetupPayload.h>
#include <system/SystemClock.h>

using namespace chip;
using namespace chip::Controller;

namespace esp_matter {
namespace controller {
namespace pairing {

static const char *TAG = "controller_pairing";

/* One commissioner and pairing; the SDK delegate has commissioner lifetime, independent of caller handles. */
class interaction::Session final : public DevicePairingDelegate {
public:
    static Session &Get()
    {
        static Session session;
        return session;
    }

    CHIP_ERROR Start(interaction *owner, const request_t &request, const options_t &options, delegate *delegate)
    {
        VerifyOrDie(!mDeliveringCallbacks);
        VerifyOrReturnError(mState == State::Idle, CHIP_ERROR_BUSY);
        VerifyOrReturnError(IsOperationalNodeId(request.node_id) && options.timeout_ms > 0, CHIP_ERROR_INVALID_ARGUMENT);

        mCommissioner = matter_controller_client::get_instance().get_commissioner();
        VerifyOrReturnError(mCommissioner != nullptr && mCommissioner->GetFabricIndex() != kUndefinedFabricIndex &&
                            DeviceLayer::SystemLayer().IsInitialized(), CHIP_ERROR_INCORRECT_STATE);
        VerifyOrReturnError(mCommissioner->GetPairingDelegate() == nullptr, CHIP_ERROR_BUSY);

        /* AutoCommissioner copies credentials on submission; the temporary ICD key is securely erased locally. */
        CommissioningParameters parameters;
        Crypto::SensitiveDataFixedBuffer<Crypto::kAES_CCM128_Key_Length> icd_key;
        CHIP_ERROR error = Prepare(request, parameters);
        if (error == CHIP_NO_ERROR && options.register_icd) {
            error = PrepareIcd(parameters, icd_key);
        }
        if (error == CHIP_NO_ERROR) {
            error = DeviceLayer::SystemLayer().StartTimer(System::Clock::Milliseconds32(options.timeout_ms),
                                                          OnDeadline, this);
        }
        if (error != CHIP_NO_ERROR) {
            return error;
        }

        /* Accept before SDK calls; inline SDK failures complete the accepted operation. */
        mOwner = owner;
        mDelegate = delegate;
        mCompletion.node_id = request.node_id;
        mState = request.method == method_t::address || request.params.setup_code.transport == transport_t::ble
                 ? State::Pase : State::Discovering;
        mCommissioner->RegisterPairingDelegate(this);
        NotifyProgress(Phase());

        mInSdkCall = true;
        error = Submit(request, parameters);
        mInSdkCall = false;
        if (error != CHIP_NO_ERROR && mState != State::Idle && mState != State::Finishing) {
            RequestCancel(error);
        }
        Advance();
        return CHIP_NO_ERROR;
    }

    bool Active(const interaction *owner) const
    {
        return mOwner == owner && mState != State::Idle && mState != State::Finishing;
    }

    CHIP_ERROR Cancel(interaction *owner)
    {
        VerifyOrDie(!mDeliveringCallbacks);
        VerifyOrReturnError(Active(owner) && !Cancelling(), CHIP_ERROR_INCORRECT_STATE);
        RequestCancel(CHIP_ERROR_CANCELLED);
        return CHIP_NO_ERROR;
    }

    void Stop(interaction *owner)
    {
        VerifyOrDie(!mDeliveringCallbacks);
        if (mOwner != owner) {
            return;
        }
        mOwner = nullptr;
        mDelegate = nullptr;
        if (!Cancelling()) {
            RequestCancel(CHIP_ERROR_CANCELLED);
        }
    }

private:
    enum class State { Idle, Discovering, Pase, Commissioning, CancelRequested, Cancelling, Finishing };

    bool Cancelling() const
    {
        return mState == State::CancelRequested || mState == State::Cancelling;
    }

    phase_t Phase() const
    {
        switch (mState) {
        case State::Discovering: return phase_t::discovery;
        case State::Pase: return phase_t::pase;
        case State::Commissioning: return phase_t::commissioning;
        case State::CancelRequested:
        case State::Cancelling: return mCompletion.phase;
        default: return phase_t::discovery;
        }
    }

    CHIP_ERROR Prepare(const request_t &request, CommissioningParameters &parameters)
    {
        mIcdRegistered = false;
        mCompletion = {};
        if (request.method == method_t::address) {
            const auto &address = request.params.address;
            const auto &ip = address.peer.GetIPAddress();
            VerifyOrReturnError(address.peer.GetTransportType() == Transport::Type::kUdp &&
                                ip != Inet::IPAddress::Any && !ip.IsIPv6Multicast() &&
                                address.peer.GetPort() != 0 && SetupPayload::IsValidSetupPIN(address.pin) &&
                                (!ip.IsIPv6LinkLocal() || address.peer.GetInterface().IsPresent()), CHIP_ERROR_INVALID_ARGUMENT);
        } else {
            VerifyOrReturnError(request.method == method_t::setup_code, CHIP_ERROR_INVALID_ARGUMENT);
            const auto &code = request.params.setup_code;
            VerifyOrReturnError(code.payload != nullptr && code.payload[0] != '\0', CHIP_ERROR_INVALID_ARGUMENT);
            VerifyOrReturnError(code.transport == transport_t::automatic || code.transport == transport_t::ble ||
                                code.transport == transport_t::on_network, CHIP_ERROR_INVALID_ARGUMENT);
#if !CONFIG_ENABLE_ESP32_BLE_CONTROLLER /* Forced BLE cannot fall back to IP. */
            VerifyOrReturnError(code.transport != transport_t::ble, CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE);
#endif // !CONFIG_ENABLE_ESP32_BLE_CONTROLLER
        }
        return SetCredentials(request.credentials, parameters);
    }

    CHIP_ERROR SetCredentials(const credentials_t &credentials, CommissioningParameters &parameters)
    {
        if (credentials.wifi.HasValue()) {
            const wifi_credentials_t &wifi = credentials.wifi.Value();
            VerifyOrReturnError(wifi.ssid.data() != nullptr && !wifi.ssid.empty() &&
                                wifi.ssid.size() <= CommissioningParameters::kMaxSsidLen &&
                                wifi.password.size() <= CommissioningParameters::kMaxCredentialsLen &&
                                (wifi.password.empty() || wifi.password.data() != nullptr), CHIP_ERROR_INVALID_ARGUMENT);
            parameters.SetWiFiCredentials(WiFiCredentials{wifi.ssid, wifi.password});
        }
        if (credentials.thread_dataset.HasValue()) {
            ByteSpan dataset = credentials.thread_dataset.Value();
            VerifyOrReturnError(dataset.data() != nullptr && !dataset.empty() &&
                                dataset.size() <= CommissioningParameters::kMaxThreadDatasetLen, CHIP_ERROR_INVALID_ARGUMENT);
            Thread::OperationalDataset parsed;
            ReturnErrorOnFailure(parsed.Init(dataset));
            parameters.SetThreadOperationalDataset(dataset);
        }
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR PrepareIcd(CommissioningParameters &parameters,
                          Crypto::SensitiveDataFixedBuffer<Crypto::kAES_CCM128_Key_Length> &key)
    {
        ReturnErrorOnFailure(Crypto::DRBG_get_bytes(key.Bytes(), key.Length()));
        NodeId commissioner_node = mCommissioner->GetNodeId();
        parameters.SetICDRegistrationStrategy(ICDRegistrationStrategy::kBeforeComplete)
                  .SetICDClientType(app::Clusters::IcdManagement::ClientTypeEnum::kPermanent)
                  .SetICDCheckInNodeId(commissioner_node).SetICDMonitoredSubject(commissioner_node)
                  .SetICDSymmetricKey(ByteSpan(key.ConstBytes(), key.Length()));
        return CHIP_NO_ERROR;
    }

    CHIP_ERROR Submit(const request_t &request, CommissioningParameters &parameters)
    {
        if (request.method == method_t::address) {
            RendezvousParameters rendezvous;
            rendezvous.SetPeerAddress(request.params.address.peer).SetSetupPINCode(request.params.address.pin);
            return mCommissioner->PairDevice(mCompletion.node_id, rendezvous, parameters);
        }
        const auto &code = request.params.setup_code;
        if (code.transport != transport_t::ble) {
            DiscoveryType discovery = code.transport == transport_t::on_network
                                      ? DiscoveryType::kDiscoveryNetworkOnly : DiscoveryType::kAll;
            return mCommissioner->PairDevice(mCompletion.node_id, code.payload, parameters, discovery);
        }
#if CONFIG_ENABLE_ESP32_BLE_CONTROLLER /* Forced BLE ignores QR transport flags. */
        std::vector<SetupPayload> payloads;
        ReturnErrorOnFailure(SetupPayload::FromStringRepresentation(code.payload, payloads));
        VerifyOrReturnError(payloads.size() == 1, CHIP_ERROR_INVALID_ARGUMENT);
        /* Preserve short discriminators from manual setup codes. */
        RendezvousParameters rendezvous;
        rendezvous.SetSetupPINCode(payloads[0].setUpPINCode).SetSetupDiscriminator(payloads[0].discriminator)
                  .SetPeerAddress(Transport::PeerAddress::BLE());
        return mCommissioner->PairDevice(mCompletion.node_id, rendezvous, parameters);
#else // !CONFIG_ENABLE_ESP32_BLE_CONTROLLER
        return CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE;
#endif // CONFIG_ENABLE_ESP32_BLE_CONTROLLER
    }

    void Advance()
    {
        /* Inline callbacks defer stop/completion until the SDK call returns. */
        if (mInSdkCall) {
            return;
        }
        switch (mState) {
        case State::CancelRequested: StopSdkPairing(); break;
        case State::Finishing: DeliverCompletion(); break;
        default: break;
        }
    }

    void RequestCancel(CHIP_ERROR error)
    {
        if (Cancelling()) {
            return;
        }
        mCompletion.phase = Phase();
        mCompletion.error = error;
        mState = State::CancelRequested;
        DeviceLayer::SystemLayer().CancelTimer(OnDeadline, this);
        NotifyProgress(phase_t::cancelling);
        Advance();
    }

    void StopSdkPairing()
    {
        mState = State::Cancelling;
        /* The permanent delegate receives asynchronous StopPairing completion after owner detach. */
        mInSdkCall = true;
        CHIP_ERROR err = mCommissioner->StopPairing(mCompletion.node_id);
        CommissioneeDeviceProxy *device = nullptr;
        /* Quiescent SetUpCodePairer may retain PASE after failed setup; a second stop closes that descriptor. */
        if (err == CHIP_NO_ERROR && mState == State::Cancelling &&
                mCommissioner->GetCommissioningStage() == CommissioningStage::kSecurePairing &&
                mCommissioner->GetDeviceBeingCommissioned(mCompletion.node_id, &device) == CHIP_NO_ERROR) {
            err = mCommissioner->StopPairing(mCompletion.node_id);
        }
        mInSdkCall = false;
        if (err != CHIP_NO_ERROR && err != CHIP_ERROR_INVALID_DEVICE_DESCRIPTOR) {
            ESP_LOGE(TAG, "StopPairing: %" CHIP_ERROR_FORMAT, err.Format());
        }
        if (mState == State::Cancelling) {
            CommissioningStage stage = mCommissioner->GetCommissioningStage();
            /* Discovery/PASE may have no terminal callback; check quiescence once after StopPairing. */
            if (stage == CommissioningStage::kSecurePairing &&
                    mCommissioner->GetDeviceBeingCommissioned(mCompletion.node_id, &device) != CHIP_NO_ERROR) {
                Finish(mCompletion.error);
            }
        }
        Advance();
    }

    static void OnDeadline(System::Layer *, void *context)
    {
        auto *session = static_cast<Session *>(context);
        if (session->mState == State::Discovering || session->mState == State::Pase ||
                session->mState == State::Commissioning) {
            session->RequestCancel(CHIP_ERROR_TIMEOUT);
        }
    }

    void OnStatusUpdate(Status status) override
    {
        if (status == SecurePairingFailed && (mState == State::Discovering || mState == State::Pase)) {
            /* Discovery may end without OnPairingComplete(); status failure is terminal CONNECTION_ABORTED. */
            RequestCancel(CHIP_ERROR_CONNECTION_ABORTED);
        }
    }

    void OnPairingComplete(CHIP_ERROR error, const std::optional<RendezvousParameters> &,
                           const std::optional<SetupPayload> &) override
    {
        if (mState == State::Idle || mState == State::Finishing || Cancelling()) {
            return;
        }
        if (error != CHIP_NO_ERROR) {
            /* Clear retained SetUpCodePairer state after its final error. */
            RequestCancel(error);
        } else {
            mState = State::Commissioning;
            NotifyProgress(phase_t::commissioning);
        }
    }

    void OnCommissioningStageStart(PeerId, CommissioningStage stage) override
    {
        if (mState == State::Commissioning && mDelegate != nullptr) {
            mDeliveringCallbacks = true;
            mDelegate->OnCommissioningStage(stage);
            mDeliveringCallbacks = false;
        }
    }

    void OnICDRegistrationComplete(ScopedNodeId node, uint32_t counter) override
    {
        VerifyOrReturn(mState == State::Commissioning);
        auto key = mCommissioner->GetCommissioningParameters().GetICDSymmetricKey();
        if (!key.HasValue()) {
            mCompletion.error = CHIP_ERROR_INCORRECT_STATE;
            return;
        }
        app::ICDClientInfo info;
        info.peer_node = node;
        info.check_in_node = ScopedNodeId(mCommissioner->GetNodeId(), node.GetFabricIndex());
        info.start_icd_counter = counter;
        info.offset = 0;
        info.monitored_subject = mCommissioner->GetNodeId();
        info.client_type = app::Clusters::IcdManagement::ClientTypeEnum::kPermanent;
        auto &storage = matter_controller_client::get_instance().get_icd_client_storage();
        mCompletion.error = storage.SetKey(info, key.Value());
        if (mCompletion.error == CHIP_NO_ERROR) {
            mCompletion.error = storage.StoreEntry(info);
        }
        if (mCompletion.error != CHIP_NO_ERROR) {
            storage.RemoveKey(info);
            return;
        }
        mIcdRegistered = true;
    }

    void OnICDStayActiveComplete(ScopedNodeId node, uint32_t duration) override
    {
        ESP_LOGI(TAG, "ICD " ChipLogFormatScopedNodeId " promised active duration: %" PRIu32,
                 ChipLogValueScopedNodeId(node), duration);
    }

    void OnCommissioningSuccess(PeerId) override
    {
        if (mState != State::Idle && mState != State::Finishing) {
            Finish(CHIP_NO_ERROR);
        }
    }

    void OnCommissioningFailure(PeerId, const CompletionStatus &status) override
    {
        if (mState == State::Idle || mState == State::Finishing) {
            return;
        }
        mCompletion.details = status;
        Finish(status.err);
    }

    void NotifyProgress(phase_t phase)
    {
        if (mDelegate != nullptr) {
            mDeliveringCallbacks = true;
            mDelegate->OnProgress(phase);
            mDeliveringCallbacks = false;
        }
    }

    void Finish(CHIP_ERROR error)
    {
        /* Cancellation overrides cleanup errors; otherwise SDK failures override ICD persistence failures. */
        if (!Cancelling()) {
            if (error != CHIP_NO_ERROR) {
                mCompletion.error = error;
            }
            mCompletion.phase = Phase();
        }
        mState = State::Finishing;
        DeviceLayer::SystemLayer().CancelTimer(OnDeadline, this);
        Advance();
    }

    void DeliverCompletion()
    {
        if (mCompletion.error != CHIP_NO_ERROR && mIcdRegistered) {
            CHIP_ERROR error = matter_controller_client::get_instance().get_icd_client_storage().DeleteEntry(
                                   ScopedNodeId(mCompletion.node_id, mCommissioner->GetFabricIndex()));
            if (error != CHIP_NO_ERROR) {
                ESP_LOGE(TAG, "ICD cleanup: %" CHIP_ERROR_FORMAT, error.Format());
            }
        }
        if (mDelegate != nullptr) {
            completion_t result{mCompletion.node_id, mCompletion.error, mCompletion.phase,
                                mCompletion.details ? &*mCompletion.details : nullptr};
            mDeliveringCallbacks = true;
            mDelegate->OnDone(result);
            mDeliveringCallbacks = false;
        } else if (mOwner != nullptr && mCompletion.error != CHIP_NO_ERROR) {
            ESP_LOGE(TAG, "Pairing node 0x" ChipLogFormatX64 " failed: %" CHIP_ERROR_FORMAT,
                     ChipLogValueX64(mCompletion.node_id), mCompletion.error.Format());
        }
        mOwner = nullptr;
        mDelegate = nullptr;
        mCommissioner->RegisterPairingDelegate(nullptr);
        mCompletion = {};
        mState = State::Idle;
    }

    State mState = State::Idle;
    interaction *mOwner = nullptr;
    delegate *mDelegate = nullptr;
    DeviceCommissioner *mCommissioner = nullptr;
    struct {
        NodeId node_id = kUndefinedNodeId;
        CHIP_ERROR error = CHIP_NO_ERROR;
        phase_t phase = phase_t::discovery;
        std::optional<CompletionStatus> details;
    } mCompletion;
    bool mInSdkCall = false;
    bool mDeliveringCallbacks = false;
    bool mIcdRegistered = false;
};

/* Session outlives all handles, including static handles. Construction performs no SDK work. */
interaction::interaction()
{
    (void)Session::Get();
}
interaction::~interaction()
{
    stop();
}
CHIP_ERROR interaction::start(const request_t &request, const options_t &options, delegate *delegate)
{
    return Session::Get().Start(this, request, options, delegate);
}
CHIP_ERROR interaction::cancel()
{
    return Session::Get().Cancel(this);
}
void interaction::stop()
{
    Session::Get().Stop(this);
}
bool interaction::active() const
{
    return Session::Get().Active(this);
}

} // namespace pairing
} // namespace controller
} // namespace esp_matter
