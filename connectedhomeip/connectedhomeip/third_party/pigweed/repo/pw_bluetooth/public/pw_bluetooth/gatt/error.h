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

namespace pw::bluetooth::gatt {

/// The values correspond with those in Bluetooth 5.2 Vol. 3 Part G Table 3.4,
/// and Supplement to the Bluetooth Core Specification v9 Part B Table 1.1,
/// but this is for ease of reference only.  Clients should *not* rely on these
/// values remaining constant.  Omitted values from the spec are handled
/// internally and will not be returned to clients.
enum class Error : uint16_t {
  // ATT Errors

  /// The attribute indicated by the handle is invalid. It may have been
  /// removed.
  ///
  /// This may be returned by a LocalService method.
  kInvalidHandle = 0x1,

  /// This attribute is not readable.
  kReadNotPermitted = 0x2,

  /// This attribute is not writable.
  kWriteNotPermitted = 0x3,

  /// Indicates that the response received from the server was invalid.
  kInvalidPdu = 0x4,

  /// This attribute requires authentication, but the client is not
  /// authenticated.
  kInsufficientAuthentication = 0x5,

  /// Indicates that the offset used in a read or write request exceeds the
  /// bounds of the value.
  kInvalidOffset = 0x7,

  /// This attribute requires authorization, but the client is not authorized.
  kInsufficientAuthorization = 0x8,

  /// This attribute requires a connection encrypted by a larger encryption key.
  kInsufficientEncryptionKeySize = 0xC,

  /// Indicates that the value given in a write request would exceed the maximum
  /// length allowed for the destination characteristic or descriptor.
  kInvalidAttributeValueLength = 0xD,

  /// A general error occurred that can not be classified as one of the more
  /// specific errors.
  kUnlikelyError = 0xE,

  /// This attribute requires encryption, but the connection is not encrypted.
  kInsufficientEncryption = 0xF,

  /// The server had insufficient resources to complete the task.
  kInsufficientResources = 0x11,

  /// The value was not allowed.
  kValueNotAllowed = 0x13,

  // Application Errors

  /// Application Errors.  The uses of these are specified at the application
  /// level.
  kApplicationError80 = 0x80,
  kApplicationError81 = 0x81,
  kApplicationError82 = 0x82,
  kApplicationError83 = 0x83,
  kApplicationError84 = 0x84,
  kApplicationError85 = 0x85,
  kApplicationError86 = 0x86,
  kApplicationError87 = 0x87,
  kApplicationError88 = 0x88,
  kApplicationError89 = 0x89,
  kApplicationError8A = 0x8A,
  kApplicationError8B = 0x8B,
  kApplicationError8C = 0x8C,
  kApplicationError8D = 0x8D,
  kApplicationError8E = 0x8E,
  kApplicationError8F = 0x8F,
  kApplicationError90 = 0x90,
  kApplicationError91 = 0x91,
  kApplicationError92 = 0x92,
  kApplicationError93 = 0x93,
  kApplicationError94 = 0x94,
  kApplicationError95 = 0x95,
  kApplicationError96 = 0x96,
  kApplicationError97 = 0x97,
  kApplicationError98 = 0x98,
  kApplicationError99 = 0x99,
  kApplicationError9A = 0x9A,
  kApplicationError9B = 0x9B,
  kApplicationError9C = 0x9C,
  kApplicationError9D = 0x9D,
  kApplicationError9E = 0x9E,
  kApplicationError9F = 0x9F,

  // Common Profile and Service Error Codes

  /// Write request was rejected at the profile or service level.
  kWriteRequestRejected = 0xFC,

  /// The Client Characteristic Configuration Descriptor was improperly
  /// configured.
  kCccDescriptorImproperlyConfigured = 0xFD,

  /// Profile or service procedure already in progress.
  kProcedureAlreadyInProgress = 0xFE,

  /// A value was out of range at the profile or service level.
  kOutOfRange = 0xFF,

  // Errors not specified by Bluetooth.

  // One or more of the call parameters are invalid. See the parameter
  // documentation.
  kInvalidParameters = 0x101,

  // Generic failure.
  kFailure = 0x102,
};

}  // namespace pw::bluetooth::gatt
