/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#include <stdint.h>
#include <time.h>

#include <platform/CHIPDeviceEvent.h>

namespace chip::app::Clusters::TimeSynchronization {
class Delegate;
}

namespace TimeSync {

/** Return the persistent delegate that records accepted Matter UTC updates. */
chip::app::Clusters::TimeSynchronization::Delegate *GetDelegate();

/** Start SNTP and update clock readiness from connectivity events; call from CHIP context. */
void OnNetworkEvent(const chip::DeviceLayer::ChipDeviceEvent *event);

/** Return fresh SNTP- or Matter-synchronized UTC and its generation, or false when unavailable. */
bool Now(time_t &utc, uint32_t &generation);

} // namespace TimeSync
