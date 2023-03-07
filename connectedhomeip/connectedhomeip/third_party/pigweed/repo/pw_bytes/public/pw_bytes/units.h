// Copyright 2021 The Pigweed Authors
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

namespace pw::bytes {

// Size constants for bytes in powers of two as defined by IEC 60027-2 A.2 and
// ISO/IEC 80000:13-2008:

// Kibibytes (KiB): 1024^1 or 2^10
inline constexpr unsigned long long int kBytesInKibibyte = 1ull << 10;

// Mebibytes (MiB): 1024^2 or 2^20
inline constexpr unsigned long long int kBytesInMebibyte = 1ull << 20;

// Gibibytes (GiB): 1024^3 or 2^30
inline constexpr unsigned long long int kBytesInGibibyte = 1ull << 30;

// Tebibytes (TiB): 1024^4 or 2^40
inline constexpr unsigned long long int kBytesInTebibyte = 1ull << 40;

// Pebibytes (PiB): 1024^5 or 2^50
inline constexpr unsigned long long int kBytesInPebibyte = 1ull << 50;

// Exbibytes (EiB): 1024^6 or 2^60
inline constexpr unsigned long long int kBytesInExbibyte = 1ull << 60;

// Functions for specifying a number of bytes in powers of two, as defined by
// IEC 60027-2 A.2 and ISO/IEC 80000:13-2008.
//
// These are useful in headers when using user-defined literals are disallowed.
//
//   #include "pw_bytes/units.h"
//
//   constexpr size_t kBufferSizeBytes = pw::bytes::MiB(1) + pw::bytes::KiB(42);
constexpr unsigned long long int B(unsigned long long int bytes) {
  return bytes;
}

constexpr unsigned long long int KiB(unsigned long long int kibibytes) {
  return kibibytes * kBytesInKibibyte;
}

constexpr unsigned long long int MiB(unsigned long long int mebibytes) {
  return mebibytes * kBytesInMebibyte;
}

constexpr unsigned long long int GiB(unsigned long long int gibibytes) {
  return gibibytes * kBytesInGibibyte;
}

constexpr unsigned long long int TiB(unsigned long long int tebibytes) {
  return tebibytes * kBytesInTebibyte;
}

constexpr unsigned long long int PiB(unsigned long long int pebibytes) {
  return pebibytes * kBytesInPebibyte;
}

constexpr unsigned long long int EiB(unsigned long long int exbibytes) {
  return exbibytes * kBytesInExbibyte;
}

namespace unit_literals {

// User-defined literals for specifying a number of bytes in powers of two, as
// defined by IEC 60027-2 A.2 and ISO/IEC 80000:13-2008.
//
// The supported prefixes include:
// _B   for bytes     (1024^0)
// _KiB for kibibytes (1024^1)
// _MiB for mebibytes (1024^2)
// _GiB for gibibytes (1024^3)
// _TiB for tebibytes (1024^4)
// _PiB for pebibytes (1024^5)
// _EiB for exbibytes (1024^6)
//
// In order to use these you must use a using namespace directive, for example:
//
//  #include "pw_bytes/units.h"
//
//  using namespace pw::bytes::unit_literals;
//
//  constepxr size_t kRandomBufferSizeBytes = 1_MiB + 42_KiB;
constexpr unsigned long long int operator""_B(unsigned long long int bytes) {
  return bytes;
}

constexpr unsigned long long int operator""_KiB(
    unsigned long long int kibibytes) {
  return kibibytes * kBytesInKibibyte;
}

constexpr unsigned long long int operator""_MiB(
    unsigned long long int mebibytes) {
  return mebibytes * kBytesInMebibyte;
}

constexpr unsigned long long int operator""_GiB(
    unsigned long long int gibibytes) {
  return gibibytes * kBytesInGibibyte;
}

constexpr unsigned long long int operator""_TiB(
    unsigned long long int tebibytes) {
  return tebibytes * kBytesInTebibyte;
}

constexpr unsigned long long int operator""_PiB(
    unsigned long long int pebibytes) {
  return pebibytes * kBytesInPebibyte;
}

constexpr unsigned long long int operator""_EiB(
    unsigned long long int exbibytes) {
  return exbibytes * kBytesInExbibyte;
}

}  // namespace unit_literals
}  // namespace pw::bytes
