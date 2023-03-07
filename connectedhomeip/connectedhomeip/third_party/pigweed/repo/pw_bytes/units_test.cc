// Copyright 2020 The Pigweed Authors
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

#include "pw_bytes/units.h"

#include <cstddef>

namespace pw::bytes {
namespace {

using namespace pw::bytes::unit_literals;

// Byte Function tests
static_assert(B(1) == 1ull);
static_assert(B(42) == 42ull);

static_assert(KiB(1) == 1'024ull);
static_assert(KiB(42) == 43'008ull);

static_assert(MiB(1) == 1'048'576ull);
static_assert(MiB(42) == 44'040'192ull);

static_assert(GiB(1) == 1'073'741'824ull);
static_assert(GiB(42) == 45'097'156'608ull);

static_assert(TiB(1) == 1'099'511'627'776ull);
static_assert(TiB(42) == 46'179'488'366'592ull);

static_assert(PiB(1) == 1'125'899'906'842'624ull);
static_assert(PiB(42) == 47'287'796'087'390'208ull);

static_assert(EiB(1) == 1'152'921'504'606'846'976ull);
static_assert(EiB(4) == 4'611'686'018'427'387'904ull);

// User-defined literal tests
static_assert(1_B == 1ull);
static_assert(42_B == 42ull);

static_assert(1_KiB == 1'024ull);
static_assert(42_KiB == 43'008ull);

static_assert(1_MiB == 1'048'576ull);
static_assert(42_MiB == 44'040'192ull);

static_assert(1_GiB == 1'073'741'824ull);
static_assert(42_GiB == 45'097'156'608ull);

static_assert(1_TiB == 1'099'511'627'776ull);
static_assert(42_TiB == 46'179'488'366'592ull);

static_assert(1_PiB == 1'125'899'906'842'624ull);
static_assert(42_PiB == 47'287'796'087'390'208ull);

static_assert(1_EiB == 1'152'921'504'606'846'976ull);
static_assert(4_EiB == 4'611'686'018'427'387'904ull);

}  // namespace
}  // namespace pw::bytes
