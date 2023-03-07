// Copyright 2022 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

#pragma once

#include <cstdint>

#include "pw_spi/chip_selector.h"
#include "pw_status/status.h"

namespace pw::spi {

class MockChipSelector : public ChipSelector {
 public:
  bool is_active() { return active_; }

 private:
  Status SetActive(bool active) override {
    active_ = active;
    return pw::OkStatus();
  }
  bool active_ = false;
};

}  // namespace pw::spi
