/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <stdint.h>

namespace DoorLockCapabilities {

// Capacity profile:
//   - 2 users with Aliro NFC and PIN credentials.
//   - 4 additional users with PIN credentials only.
//   - 4 Aliro endpoint keys shared by the Aliro users.
//
// Matter advertises these as independent limits; it does not reserve user slots
// by role or require a fixed relationship between issuer and endpoint keys.

// Maximum number of user records that the lock can store.
constexpr uint16_t kUsers = 6;
// Maximum number of credential references associated with one user. An
// Aliro-and-PIN user can reference one PIN, one issuer key, and two endpoint keys.
constexpr uint8_t kCredentialsPerUser = 4;
// Total number of PIN credential slots, shared by all users.
constexpr uint16_t kPinCredentialSlots = 6;
// Week Day remains at one until the SDK exposes its count as a feature configuration.
constexpr uint8_t kWeekdaySchedulesPerUser = 1;
constexpr uint8_t kYeardaySchedulesPerUser = 1;
constexpr uint8_t kHolidaySchedules = 1;

// Total number of Aliro credential issuer key slots.
constexpr uint16_t kAliroCredentialIssuerKeySlots = 2;
// Total occupied capacity shared across Aliro evictable and non-evictable
// endpoint keys. Each endpoint-key type has this index range, but Matter
// enforces the limit against their combined occupied count.
constexpr uint16_t kAliroEndpointKeySlots = 4;

// Fixed storage stride reserved for every credential-type namespace. It must
// accommodate the largest advertised credential capacity.
constexpr uint16_t kCredentialSlotsPerType = kPinCredentialSlots;
static_assert(kCredentialSlotsPerType >= kPinCredentialSlots);
static_assert(kCredentialSlotsPerType >= kAliroCredentialIssuerKeySlots);
static_assert(kCredentialSlotsPerType >= kAliroEndpointKeySlots);

} // namespace DoorLockCapabilities
