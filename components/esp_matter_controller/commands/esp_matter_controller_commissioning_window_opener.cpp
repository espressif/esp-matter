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

#include <sdkconfig.h>

#include <esp_matter_controller_commissioning_window_opener.h>
#include <esp_matter_controller_utils.h>
#include <esp_matter_client_invoke_command.h>
#include <app-common/zap-generated/cluster-objects.h>
#include <crypto/CHIPCryptoPAL.h>
#include <lib/support/CHIPMem.h>
#include <lib/support/CodeUtils.h>
#include <protocols/secure_channel/PASESession.h>

#ifndef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
#include <esp_matter_controller_client.h>
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER

using namespace chip;
using namespace chip::app;
using namespace chip::Crypto;

namespace esp_matter {
namespace controller {
namespace commissioning_window {

class interaction::Impl final : public client::invoke_command::delegate {
public:
    ~Impl()
    {
        VerifyOrDie(!mDeliveringCallbacks);
    }

    CHIP_ERROR Start(const request_t &request, commissioning_window::delegate *delegate)
    {
        VerifyOrReturnError(IsOperationalNodeId(request.node_id) && request.window_timeout_seconds != 0 &&
                            request.timed_invoke_timeout_ms != 0,
                            CHIP_ERROR_INVALID_ARGUMENT);
        VerifyOrReturnError(request.method == method_t::basic || request.method == method_t::passcode ||
                            request.method == method_t::verifier, CHIP_ERROR_INVALID_ARGUMENT);
        mDelegate = delegate;

        if (request.method == method_t::basic) {
            Clusters::AdministratorCommissioning::Commands::OpenBasicCommissioningWindow::Type command;
            command.commissioningTimeout = request.window_timeout_seconds;
            return Submit(request.node_id, command, request.timed_invoke_timeout_ms);
        }

        uint32_t iterations;
        uint16_t discriminator;
        ByteSpan salt;
        ByteSpan verifier;
        Spake2pVerifierSerialized generated_verifier;
        // salt borrows this buffer until Submit() below.
        // cppcheck-suppress variableScope
        uint8_t generated_salt[kSpake2p_Max_PBKDF_Salt_Length];
        if (request.method == method_t::passcode) {
            const auto &params = request.params.passcode;
            iterations = params.iterations;
            discriminator = params.discriminator;
            VerifyOrReturnError(!params.setup_pin.HasValue() || SetupPayload::IsValidSetupPIN(params.setup_pin.Value()),
                                CHIP_ERROR_INVALID_ARGUMENT);
            if (params.salt.HasValue()) {
                salt = params.salt.Value();
            } else {
                ReturnErrorOnFailure(DRBG_get_bytes(generated_salt, sizeof(generated_salt)));
                salt = ByteSpan(generated_salt);
            }
            ReturnErrorOnFailure(ValidateEnhanced(discriminator, iterations, salt));
            uint32_t pin = params.setup_pin.ValueOr(0);
            Spake2pVerifier verifier_value;
            ReturnErrorOnFailure(chip::PASESession::GeneratePASEVerifier(
                                     verifier_value, iterations, salt, !params.setup_pin.HasValue(), pin));
            MutableByteSpan serialized(generated_verifier);
            ReturnErrorOnFailure(verifier_value.Serialize(serialized));
            verifier = serialized;
            mPayload.SetValue(SetupPayload());
            mPayload.Value().version = 0;
            mPayload.Value().discriminator.SetLongValue(discriminator);
            mPayload.Value().setUpPINCode = pin;
            mPayload.Value().rendezvousInformation.SetValue(RendezvousInformationFlag::kOnNetwork);
        } else {
            const auto &params = request.params.verifier;
            iterations = params.iterations;
            discriminator = params.discriminator;
            salt = params.salt;
            verifier = params.verifier;
            ReturnErrorOnFailure(ValidateEnhanced(discriminator, iterations, salt));
            VerifyOrReturnError(verifier.data() != nullptr && verifier.size() == kSpake2p_VerifierSerialized_Length,
                                CHIP_ERROR_INVALID_ARGUMENT);
        }
        Clusters::AdministratorCommissioning::Commands::OpenCommissioningWindow::Type command;
        command.commissioningTimeout = request.window_timeout_seconds;
        command.discriminator = discriminator;
        command.iterations = iterations;
        command.salt = salt;
        command.PAKEPasscodeVerifier = verifier;
        return Submit(request.node_id, command, request.timed_invoke_timeout_ms);
    }

    bool Active() const
    {
        return mInvoke.active();
    }
    bool DeliveringCallbacks() const
    {
        return mDeliveringCallbacks;
    }

private:
    static CHIP_ERROR ValidateEnhanced(uint16_t discriminator, uint32_t iterations, ByteSpan salt)
    {
        VerifyOrReturnError(discriminator < (1u << SetupDiscriminator::kLongBits) &&
                            iterations >= kSpake2p_Min_PBKDF_Iterations && iterations <= kSpake2p_Max_PBKDF_Iterations &&
                            salt.data() != nullptr && salt.size() >= kSpake2p_Min_PBKDF_Salt_Length &&
                            salt.size() <= kSpake2p_Max_PBKDF_Salt_Length, CHIP_ERROR_INVALID_ARGUMENT);
        return CHIP_NO_ERROR;
    }

    template <typename Command>
    CHIP_ERROR Submit(NodeId node, const Command &command, uint16_t timed_timeout_ms)
    {
#ifdef CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
        FabricIndex fabric = esp_matter::controller::get_fabric_index();
#else // !CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
        FabricIndex fabric = matter_controller_client::get_instance().get_fabric_index();
#endif // CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER
        VerifyOrReturnError(fabric != kUndefinedFabricIndex, CHIP_ERROR_INCORRECT_STATE);
        client::node_target_t target{ ScopedNodeId(node, fabric), client::execution_t::remote };
        chip::app::DataModel::EncodableType<Command> fields(command);
        client::invoke_command::request_t request;
        request.endpoint_id = 0;
        request.cluster_id = Command::GetClusterId();
        request.command_id = Command::GetCommandId();
        request.fields = &fields;
        request.options.timed_timeout_ms.SetValue(timed_timeout_ms);
        return mInvoke.start(target, request, this);
    }

    void OnResponse(const esp_matter::client::target_t &, const chip::app::ConcreteCommandPath &, const chip::app::StatusIB &status,
                    TLV::TLVReader *) override
    {
        if (!status.IsSuccess()) {
            mResponseError = status.ToChipError();
        }
        if (mDelegate != nullptr) {
            response_t response{status, status.IsSuccess() && mPayload.HasValue() ? &mPayload.Value() : nullptr};
            mDeliveringCallbacks = true;
            mDelegate->OnResponse(response);
            mDeliveringCallbacks = false;
        }
    }

    void OnDone(const esp_matter::client::target_t &, CHIP_ERROR error) override
    {
        error = error == CHIP_NO_ERROR ? mResponseError : error;
        commissioning_window::delegate *delegate = mDelegate;
        mDelegate = nullptr;
        if (delegate != nullptr) {
            delegate->OnDone(error);
        } else if (error != CHIP_NO_ERROR) {
            ChipLogError(Controller, "Open commissioning window failed: %" CHIP_ERROR_FORMAT, error.Format());
        }
    }

    client::invoke_command::interaction mInvoke;
    commissioning_window::delegate *mDelegate = nullptr;
    Optional<SetupPayload> mPayload;
    CHIP_ERROR mResponseError = CHIP_NO_ERROR;
    bool mDeliveringCallbacks = false;
};

interaction::~interaction()
{
    stop();
}

CHIP_ERROR interaction::start(const request_t &request, commissioning_window::delegate *delegate)
{
    VerifyOrDie(mImpl == nullptr || !mImpl->DeliveringCallbacks());
    VerifyOrReturnError(!active(), CHIP_ERROR_BUSY);
    stop();
    auto *impl = chip::Platform::New<Impl>();
    VerifyOrReturnError(impl != nullptr, CHIP_ERROR_NO_MEMORY);
    CHIP_ERROR error = impl->Start(request, delegate);
    if (error != CHIP_NO_ERROR) {
        chip::Platform::Delete(impl);
        return error;
    }
    mImpl = impl;
    return CHIP_NO_ERROR;
}

void interaction::stop()
{
    VerifyOrDie(mImpl == nullptr || !mImpl->DeliveringCallbacks());
    chip::Platform::Delete(mImpl);
    mImpl = nullptr;
}

bool interaction::active() const
{
    return mImpl != nullptr && mImpl->Active();
}

} // namespace commissioning_window
} // namespace controller
} // namespace esp_matter
