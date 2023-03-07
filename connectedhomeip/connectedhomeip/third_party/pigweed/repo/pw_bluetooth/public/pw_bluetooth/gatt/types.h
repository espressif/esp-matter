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
#include <optional>

#include "pw_bluetooth/types.h"
#include "pw_span/span.h"

namespace pw::bluetooth::gatt {

// A Handle uniquely identifies a service, characteristic, or descriptor.
enum class Handle : uint64_t {};

// Possible values for the characteristic properties bitfield. These specify
// the GATT procedures that are allowed for a particular characteristic.
enum class CharacteristicPropertyBits : uint16_t {
  kBroadcast = 0x1,
  kRead = 0x2,
  kWriteWithoutResponse = 0x4,
  kWrite = 0x8,
  kNotify = 0x10,
  kIndicate = 0x20,
  kAuthenticatedSignedWrites = 0x40,
  kReliableWrite = 0x100,
  kWritableAuxiliaries = 0x200
};

// Helper operators to allow combining and comparing CharacteristicPropertyBits
// values.
inline constexpr bool operator&(CharacteristicPropertyBits left,
                                CharacteristicPropertyBits right) {
  return static_cast<bool>(
      static_cast<std::underlying_type_t<CharacteristicPropertyBits>>(left) &
      static_cast<std::underlying_type_t<CharacteristicPropertyBits>>(right));
}

inline constexpr CharacteristicPropertyBits operator|(
    CharacteristicPropertyBits left, CharacteristicPropertyBits right) {
  return static_cast<CharacteristicPropertyBits>(
      static_cast<std::underlying_type_t<CharacteristicPropertyBits>>(left) |
      static_cast<std::underlying_type_t<CharacteristicPropertyBits>>(right));
}

inline constexpr CharacteristicPropertyBits& operator|=(
    CharacteristicPropertyBits& left, CharacteristicPropertyBits right) {
  return left = left | right;
}

// Represents encryption, authentication, and authorization permissions that can
// be assigned to a specific access permission.
struct SecurityRequirements {
  // If true, the physical link must be encrypted to access this attribute.
  bool encryption_required;

  // If true, the physical link must be authenticated to access this
  // attribute.
  bool authentication_required;

  // If true, the client needs to be authorized before accessing this
  // attribute.
  bool authorization_required;
};

/// Specifies the access permissions for a specific attribute value.
struct AttributePermissions {
  // Specifies whether or not an attribute has the read permission. If null,
  // then the attribute value cannot be read. Otherwise, it can be read only if
  // the permissions specified in the SecurityRequirements table are satisfied.
  std::optional<SecurityRequirements> read;

  // Specifies whether or not an attribute has the write permission. If null,
  // then the attribute value cannot be written. Otherwise, it can be written
  // only if the permissions specified in the SecurityRequirements table are
  // satisfied.
  std::optional<SecurityRequirements> write;

  // Specifies the security requirements for a client to subscribe to
  // notifications or indications on a characteristic. A characteristic's
  // support for notifications or indications is specified using the NOTIFY and
  // INDICATE characteristic properties. If a local characteristic has one of
  // these properties then this field can not be null. Otherwise, this field
  // must be left as null.
  //
  // This field is ignored for Descriptors.
  std::optional<SecurityRequirements> update;
};

// Represents a local or remote GATT characteristic descriptor.
struct Descriptor {
  // Uniquely identifies this descriptor within a service.
  // For local descriptors, the specified handle must be unique
  // across all characteristic and descriptor handles in this service.
  Handle handle;

  // The UUID that identifies the type of this descriptor.
  Uuid type;

  // The attribute permissions of this descriptor. For remote descriptors, this
  // value will be null until the permissions are discovered via read and write
  // requests.
  std::optional<AttributePermissions> permissions;
};

// Represents a local or remote GATT characteristic.
struct Characteristic {
  // Uniquely identifies this characteristic within a service.
  // For local characteristics, the specified handle must be unique across
  // all characteristic and descriptor handles in this service.
  Handle handle;

  // The UUID that identifies the type of this characteristic.
  Uuid type;

  // The characteristic properties bitfield. This is a logic or of any number of
  // values from `CharacteristicPropertyBits` above.
  CharacteristicPropertyBits properties;

  // The attribute permissions of this characteristic. For remote
  // characteristics, this value will be null until the permissions are
  // discovered via read and write requests.
  std::optional<AttributePermissions> permissions;

  // The descriptors of this characteristic.
  span<const Descriptor> descriptors;
};

}  // namespace pw::bluetooth::gatt