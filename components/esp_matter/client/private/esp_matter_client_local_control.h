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
#include <esp_matter_client_write_command.h>

namespace esp_matter {
namespace client {

namespace operation {
class EncodedElement;
} // namespace operation

namespace read_command {
CHIP_ERROR read_local(const chip::ScopedNodeId &target, const request_t &request,
                      chip::Optional<chip::System::Clock::Timestamp> deadline, delegate &receiver);
} // namespace read_command

namespace write_command {
class LocalWriteOperation;

CHIP_ERROR prepare_local(const chip::ScopedNodeId &target, const request_t &request,
                         chip::Optional<chip::System::Clock::Timestamp> deadline,
                         LocalWriteOperation * &operation);
CHIP_ERROR run_local(LocalWriteOperation *operation, delegate &receiver);
void stop_local(LocalWriteOperation *operation);
} // namespace write_command

namespace invoke_command {
class LocalInvokeOperation;

CHIP_ERROR prepare_local(const chip::ScopedNodeId &target, const request_t &request, LocalInvokeOperation * &operation);
CHIP_ERROR prepare_local(chip::GroupId group_id, chip::FabricIndex fabric_index, const request_t &request,
                         const operation::EncodedElement &fields, LocalInvokeOperation * &operation);
void run_local(LocalInvokeOperation *operation, delegate *receiver);
void stop_local(LocalInvokeOperation *operation);
} // namespace invoke_command

} // namespace client
} // namespace esp_matter
