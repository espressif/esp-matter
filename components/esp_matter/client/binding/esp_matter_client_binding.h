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

#pragma once

#include <esp_matter_client_invoke_command.h>
#include <esp_matter_client_read_command.h>
#include <esp_matter_client_subscribe_command.h>
#include <esp_matter_client_write_command.h>

namespace esp_matter {
namespace client {
namespace binding {

/** @brief Initialize the shared SDK BindingManager bridge after Matter server startup. */
void initialize();

class DispatchRequest;

/**
 * @brief Caller-owned BindingManager dispatch and native-operation lifetimes.
 *
 * Requests select one concrete cluster and are copied before return.
 * Bindings supply fabric and remote endpoint. Read/Subscribe skip groups.
 * Callbacks identify each target. OnDone() is per destination, not aggregate completion.
 * Subscription state callbacks may occur during dispatch().
 * SDK notifications may be replaced. CASE failures may produce no callback.
 * Native start errors are logged. Returned errors do not undo dispatched work.
 *
 * Removed bindings stop subscriptions; dispatched finite operations may finish.
 * Fabric removal cancels affected work. New bindings apply on the next dispatch().
 * Each dispatch selects a local endpoint; stop() cancels work across all endpoints.
 *
 * Call methods in CHIP context or with the stack lock.
 * Delegates are borrowed until stop(), which suppresses pending dispatch and native callbacks.
 * The next dispatch() or stop() releases completed handles.
 * OnDone() may dispatch, stop or destroy this object. Other callbacks may not stop or destroy it;
 * dispatch() returns CHIP_ERROR_INCORRECT_STATE during nonterminal native client callbacks.
 * Destruction requires CHIP context/lock unless already stopped.
 */
class dispatcher {
public:
    dispatcher() = default;
    ~dispatcher();
    dispatcher(const dispatcher &) = delete;
    dispatcher &operator=(const dispatcher &) = delete;

    CHIP_ERROR dispatch(chip::EndpointId local_endpoint, const invoke_command::request_t &request, invoke_command::delegate *delegate);
    CHIP_ERROR dispatch(chip::EndpointId local_endpoint, const read_command::request_t &request, read_command::delegate &delegate);
    CHIP_ERROR dispatch(chip::EndpointId local_endpoint, const write_command::request_t &request, write_command::delegate *delegate);
    CHIP_ERROR dispatch(chip::EndpointId local_endpoint, const subscribe_command::request_t &request, subscribe_command::delegate &delegate);
    void stop();

private:
    CHIP_ERROR Prune();
    DispatchRequest *mRequests = nullptr;
};

} // namespace binding
} // namespace client
} // namespace esp_matter
