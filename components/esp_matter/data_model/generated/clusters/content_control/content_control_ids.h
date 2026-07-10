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
namespace content_control {

inline constexpr uint32_t Id = 0x050F;

namespace feature {
namespace ScreenTime {
inline constexpr uint32_t Id = 0x1;
} /* ScreenTime */
namespace PINManagement {
inline constexpr uint32_t Id = 0x2;
} /* PINManagement */
namespace BlockUnrated {
inline constexpr uint32_t Id = 0x4;
} /* BlockUnrated */
namespace OnDemandContentRating {
inline constexpr uint32_t Id = 0x8;
} /* OnDemandContentRating */
namespace ScheduledContentRating {
inline constexpr uint32_t Id = 0x10;
} /* ScheduledContentRating */
namespace BlockChannels {
inline constexpr uint32_t Id = 0x20;
} /* BlockChannels */
namespace BlockApplications {
inline constexpr uint32_t Id = 0x40;
} /* BlockApplications */
namespace BlockContentTimeWindow {
inline constexpr uint32_t Id = 0x80;
} /* BlockContentTimeWindow */
} /* feature */

namespace attribute {
namespace Enabled {
inline constexpr uint32_t Id = 0x0000;
} /* Enabled */
namespace OnDemandRatings {
inline constexpr uint32_t Id = 0x0001;
} /* OnDemandRatings */
namespace OnDemandRatingThreshold {
inline constexpr uint32_t Id = 0x0002;
} /* OnDemandRatingThreshold */
namespace ScheduledContentRatings {
inline constexpr uint32_t Id = 0x0003;
} /* ScheduledContentRatings */
namespace ScheduledContentRatingThreshold {
inline constexpr uint32_t Id = 0x0004;
} /* ScheduledContentRatingThreshold */
namespace ScreenDailyTime {
inline constexpr uint32_t Id = 0x0005;
inline constexpr uint32_t Min = 0;
inline constexpr uint32_t Max = 86400;
} /* ScreenDailyTime */
namespace RemainingScreenTime {
inline constexpr uint32_t Id = 0x0006;
inline constexpr uint32_t Min = 0;
inline constexpr uint32_t Max = 86400;
} /* RemainingScreenTime */
namespace BlockUnrated {
inline constexpr uint32_t Id = 0x0007;
} /* BlockUnrated */
namespace BlockChannelList {
inline constexpr uint32_t Id = 0x0008;
} /* BlockChannelList */
namespace BlockApplicationList {
inline constexpr uint32_t Id = 0x0009;
} /* BlockApplicationList */
namespace BlockContentTimeWindow {
inline constexpr uint32_t Id = 0x000A;
} /* BlockContentTimeWindow */
} /* attribute */

namespace command {
namespace UpdatePIN {
inline constexpr uint32_t Id = 0x00;
} /* UpdatePIN */
namespace ResetPIN {
inline constexpr uint32_t Id = 0x01;
} /* ResetPIN */
namespace ResetPINResponse {
inline constexpr uint32_t Id = 0x02;
} /* ResetPINResponse */
namespace Enable {
inline constexpr uint32_t Id = 0x03;
} /* Enable */
namespace Disable {
inline constexpr uint32_t Id = 0x04;
} /* Disable */
namespace AddBonusTime {
inline constexpr uint32_t Id = 0x05;
} /* AddBonusTime */
namespace SetScreenDailyTime {
inline constexpr uint32_t Id = 0x06;
} /* SetScreenDailyTime */
namespace BlockUnratedContent {
inline constexpr uint32_t Id = 0x07;
} /* BlockUnratedContent */
namespace UnblockUnratedContent {
inline constexpr uint32_t Id = 0x08;
} /* UnblockUnratedContent */
namespace SetOnDemandRatingThreshold {
inline constexpr uint32_t Id = 0x09;
} /* SetOnDemandRatingThreshold */
namespace SetScheduledContentRatingThreshold {
inline constexpr uint32_t Id = 0x0A;
} /* SetScheduledContentRatingThreshold */
namespace AddBlockChannels {
inline constexpr uint32_t Id = 0x0B;
} /* AddBlockChannels */
namespace RemoveBlockChannels {
inline constexpr uint32_t Id = 0x0C;
} /* RemoveBlockChannels */
namespace AddBlockApplications {
inline constexpr uint32_t Id = 0x0D;
} /* AddBlockApplications */
namespace RemoveBlockApplications {
inline constexpr uint32_t Id = 0x0E;
} /* RemoveBlockApplications */
namespace SetBlockContentTimeWindow {
inline constexpr uint32_t Id = 0x0F;
} /* SetBlockContentTimeWindow */
namespace RemoveBlockContentTimeWindow {
inline constexpr uint32_t Id = 0x10;
} /* RemoveBlockContentTimeWindow */
} /* command */

namespace event {
namespace RemainingScreenTimeExpired {
inline constexpr uint32_t Id = 0x00;
} /* RemainingScreenTimeExpired */
namespace EnteringBlockContentTimeWindow {
inline constexpr uint32_t Id = 0x01;
} /* EnteringBlockContentTimeWindow */
} /* event */

} /* content_control */
} /* cluster */
} /* esp_matter */
