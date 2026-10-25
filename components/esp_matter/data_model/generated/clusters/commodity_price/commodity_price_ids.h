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

/* THIS IS A GENERATED FILE, DO NOT EDIT */

#pragma once
#include <esp_matter_data_model.h>

namespace esp_matter {
namespace cluster {
namespace commodity_price {

inline constexpr uint32_t Id = 0x0095;

namespace feature {
namespace Forecasting {
inline constexpr uint32_t Id = 0x1;
} /* Forecasting */
} /* feature */

namespace attribute {
namespace TariffUnit {
inline constexpr uint32_t Id = 0x0000;
inline constexpr uint8_t Min = 0;
inline constexpr uint8_t Max = 1;
} /* TariffUnit */
namespace Currency {
inline constexpr uint32_t Id = 0x0001;
} /* Currency */
namespace CurrentPrice {
inline constexpr uint32_t Id = 0x0002;
} /* CurrentPrice */
namespace PriceForecast {
inline constexpr uint32_t Id = 0x0003;
} /* PriceForecast */
} /* attribute */

namespace command {
namespace GetDetailedPriceRequest {
inline constexpr uint32_t Id = 0x00;
} /* GetDetailedPriceRequest */
namespace GetDetailedPriceResponse {
inline constexpr uint32_t Id = 0x01;
} /* GetDetailedPriceResponse */
namespace GetDetailedForecastRequest {
inline constexpr uint32_t Id = 0x02;
} /* GetDetailedForecastRequest */
namespace GetDetailedForecastResponse {
inline constexpr uint32_t Id = 0x03;
} /* GetDetailedForecastResponse */
} /* command */

namespace event {
namespace PriceChange {
inline constexpr uint32_t Id = 0x00;
} /* PriceChange */
} /* event */

} /* commodity_price */
} /* cluster */
} /* esp_matter */
