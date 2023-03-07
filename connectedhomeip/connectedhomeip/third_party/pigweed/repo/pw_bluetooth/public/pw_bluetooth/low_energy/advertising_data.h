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

#include "pw_bluetooth/types.h"
#include "pw_span/span.h"

namespace pw::bluetooth::low_energy {

// A service data field in an advertising data payload.
struct ServiceData {
  Uuid uuid;
  span<const std::byte> data;
};

// A manufacturer data field in an advertising data payload.
struct ManufacturerData {
  uint16_t company_id = 0;
  span<const std::byte> data;
};

// Represents advertising and scan response data that are transmitted by a LE
// peripheral or broadcaster.
struct AdvertisingData {
  // Long or short name of the device.
  std::string_view name;

  // The appearance of the local device.
  Appearance appearance = Appearance::kUnknown;

  span<const Uuid> service_uuids;

  span<const ServiceData> service_data;

  span<const ManufacturerData> manufacturer_data;

  // String representing a URI to be advertised, as defined in IETF STD 66:
  // https://tools.ietf.org/html/std66. Each entry should be a UTF-8 string
  // including the scheme. For more information, see:
  // https://www.iana.org/assignments/uri-schemes/uri-schemes.xhtml for allowed
  // schemes;
  // https://www.bluetooth.com/specifications/assigned-numbers/uri-scheme-name-string-mapping
  // for code-points used by the system to compress the scheme to save space in
  // the payload.
  span<const std::string_view> uris;

  // Indicates whether the current TX power level should be included in the
  // advertising data.
  bool include_tx_power_level = false;
};

}  // namespace pw::bluetooth::low_energy
