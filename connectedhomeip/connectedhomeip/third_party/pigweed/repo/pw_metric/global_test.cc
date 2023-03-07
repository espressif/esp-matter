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

#include "pw_metric/global.h"

#include "gtest/gtest.h"
#include "pw_log/log.h"
#include "pw_metric/metric.h"

namespace pw {
namespace metric {

// Create two global metrics; make sure they show up.
PW_METRIC_GLOBAL(stat_x, "stat_x", 123u);
PW_METRIC_GLOBAL(stat_y, "stat_y", 123u);

TEST(Global, Metrics) {
  Metric::Dump(global_metrics);
  EXPECT_EQ(global_metrics.size(), 2u);
}

// Create three global metric groups; make sure they show up.
// Also, register some sub-metrics in the global groups.
PW_METRIC_GROUP_GLOBAL(gyro_metrics, "gyro");
PW_METRIC(gyro_metrics, max_velocity, "max_velocity", 5.0f);

PW_METRIC_GROUP_GLOBAL(comms_metrics, "comms");
PW_METRIC(comms_metrics, packet_drops, "packet_drops", 10u);
PW_METRIC(comms_metrics, bandwidth, "bandwidth", 230.3f);

PW_METRIC_GROUP_GLOBAL(power_metrics, "power");
PW_METRIC(power_metrics, voltage, "voltage", 3.33f);
PW_METRIC(power_metrics, battery_cycles, "battery_cycles", 550u);
PW_METRIC(power_metrics, current_ma, "current_ma", 35.2f);

TEST(Global, Groups) {
  Group::Dump(global_groups);
  EXPECT_EQ(global_groups.size(), 4u);

  EXPECT_EQ(gyro_metrics.metrics().size(), 1u);
  EXPECT_EQ(comms_metrics.metrics().size(), 2u);
  EXPECT_EQ(power_metrics.metrics().size(), 3u);
}

}  // namespace metric
}  // namespace pw

// this is a compilation test to make sure metrics can be defined outside of
// ::pw::metric
PW_METRIC_GROUP_GLOBAL(global_group, "global group");
