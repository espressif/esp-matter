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

#include "pw_metric/metric_service_pwpb.h"

#include "gtest/gtest.h"
#include "pw_log/log.h"
#include "pw_metric_proto/metric_service.pwpb.h"
#include "pw_protobuf/decoder.h"
#include "pw_rpc/pwpb/test_method_context.h"
#include "pw_rpc/raw/test_method_context.h"
#include "pw_span/span.h"

namespace pw::metric {
namespace {

#define MetricMethodContext \
  PW_PWPB_TEST_METHOD_CONTEXT(MetricService, Get, 4, 256)

size_t CountEncodedMetrics(ConstByteSpan serialized_path) {
  protobuf::Decoder decoder(serialized_path);
  size_t num_metrics = 0;
  while (decoder.Next().ok()) {
    switch (decoder.FieldNumber()) {
      case static_cast<uint32_t>(
          pw::metric::proto::pwpb::MetricResponse::Fields::METRICS): {
        num_metrics++;
      }
    }
  }
  return num_metrics;
}

size_t SumMetricInts(ConstByteSpan serialized_path) {
  protobuf::Decoder decoder(serialized_path);
  size_t metrics_sum = 0;
  while (decoder.Next().ok()) {
    switch (decoder.FieldNumber()) {
      case static_cast<uint32_t>(
          pw::metric::proto::pwpb::Metric::Fields::AS_INT): {
        uint32_t metric_value;
        EXPECT_EQ(OkStatus(), decoder.ReadUint32(&metric_value));
        metrics_sum += metric_value;
      }
    }
  }
  return metrics_sum;
}

size_t GetMetricsSum(ConstByteSpan serialized_metric_buffer) {
  protobuf::Decoder decoder(serialized_metric_buffer);
  size_t metrics_sum = 0;
  while (decoder.Next().ok()) {
    switch (decoder.FieldNumber()) {
      case static_cast<uint32_t>(
          pw::metric::proto::pwpb::MetricResponse::Fields::METRICS): {
        ConstByteSpan metric_buffer;
        EXPECT_EQ(OkStatus(), decoder.ReadBytes(&metric_buffer));
        metrics_sum += SumMetricInts(metric_buffer);
      }
    }
  }
  return metrics_sum;
}

TEST(MetricService, EmptyGroupAndNoMetrics) {
  // Empty root group.
  PW_METRIC_GROUP(root, "/");

  // Run the RPC and ensure it completes.

  PW_RAW_TEST_METHOD_CONTEXT(MetricService, Get)
  ctx{root.metrics(), root.children()};
  ctx.call({});
  EXPECT_TRUE(ctx.done());
  EXPECT_EQ(OkStatus(), ctx.status());

  // No metrics should be in the response.
  EXPECT_EQ(0u, ctx.responses().size());
}

TEST(MetricService, OneGroupOneMetric) {
  // One root group with one metric.
  PW_METRIC_GROUP(root, "/");
  PW_METRIC(root, a, "a", 3u);

  // Run the RPC and ensure it completes.

  PW_RAW_TEST_METHOD_CONTEXT(MetricService, Get)
  ctx{root.metrics(), root.children()};
  ctx.call({});
  EXPECT_TRUE(ctx.done());
  EXPECT_EQ(OkStatus(), ctx.status());

  // One metric should be in the response.
  EXPECT_EQ(1u, ctx.responses().size());

  // Sum should be 3.
  EXPECT_EQ(3u, GetMetricsSum(ctx.responses()[0]));
}

TEST(MetricService, OneGroupFiveMetrics) {
  // One root group with five metrics.
  PW_METRIC_GROUP(root, "/");
  PW_METRIC(root, a, "a", 1u);
  PW_METRIC(root, b, "b", 2u);  // Note: Max # per response is 3.
  PW_METRIC(root, c, "c", 3u);
  PW_METRIC(root, x, "x", 4u);
  PW_METRIC(root, y, "y", 5u);

  // Run the RPC and ensure it completes.

  PW_RAW_TEST_METHOD_CONTEXT(MetricService, Get)
  ctx{root.metrics(), root.children()};
  ctx.call({});
  EXPECT_TRUE(ctx.done());
  EXPECT_EQ(OkStatus(), ctx.status());

  // Two metrics should be in the response.
  EXPECT_EQ(2u, ctx.responses().size());
  EXPECT_EQ(3u, CountEncodedMetrics(ctx.responses()[0]));
  EXPECT_EQ(2u, CountEncodedMetrics(ctx.responses()[1]));

  // The metrics are the numbers 1..5; sum them and compare.
  EXPECT_EQ(
      15u,
      GetMetricsSum(ctx.responses()[0]) + GetMetricsSum(ctx.responses()[1]));
}

TEST(MetricService, NestedGroupFiveMetrics) {
  // Set up a nested group of metrics.
  PW_METRIC_GROUP(root, "/");
  PW_METRIC(root, a, "a", 1u);
  PW_METRIC(root, b, "b", 2u);

  PW_METRIC_GROUP(inner, "inner");
  PW_METRIC(root, x, "x", 3u);  // Note: Max # per response is 3.
  PW_METRIC(inner, y, "y", 4u);
  PW_METRIC(inner, z, "z", 5u);

  root.Add(inner);

  // Run the RPC and ensure it completes.

  PW_RAW_TEST_METHOD_CONTEXT(MetricService, Get)
  ctx{root.metrics(), root.children()};
  ctx.call({});
  EXPECT_TRUE(ctx.done());
  EXPECT_EQ(OkStatus(), ctx.status());

  // Two metrics should be in the response.
  EXPECT_EQ(2u, ctx.responses().size());
  EXPECT_EQ(3u, CountEncodedMetrics(ctx.responses()[0]));
  EXPECT_EQ(2u, CountEncodedMetrics(ctx.responses()[1]));

  EXPECT_EQ(
      15u,
      GetMetricsSum(ctx.responses()[0]) + GetMetricsSum(ctx.responses()[1]));
}

TEST(MetricService, NestedGroupsWithBatches) {
  // Set up a nested group of metrics that will not fit in a single batch.
  PW_METRIC_GROUP(root, "/");
  PW_METRIC(root, a, "a", 1u);
  PW_METRIC(root, d, "d", 2u);
  PW_METRIC(root, f, "f", 3u);

  PW_METRIC_GROUP(inner_1, "inner1");
  PW_METRIC(inner_1, x, "x", 4u);
  PW_METRIC(inner_1, y, "y", 5u);
  PW_METRIC(inner_1, z, "z", 6u);

  PW_METRIC_GROUP(inner_2, "inner2");
  PW_METRIC(inner_2, p, "p", 7u);
  PW_METRIC(inner_2, q, "q", 8u);
  PW_METRIC(inner_2, r, "r", 9u);
  PW_METRIC(inner_2, s, "s", 10u);  // Note: Max # per response is 3.
  PW_METRIC(inner_2, t, "t", 11u);
  PW_METRIC(inner_2, u, "u", 12u);

  root.Add(inner_1);
  root.Add(inner_2);

  // Run the RPC and ensure it completes.
  PW_RAW_TEST_METHOD_CONTEXT(MetricService, Get)
  ctx{root.metrics(), root.children()};
  ctx.call({});
  EXPECT_TRUE(ctx.done());
  EXPECT_EQ(OkStatus(), ctx.status());

  // The response had to be split into four parts; check that they have the
  // appropriate sizes.
  EXPECT_EQ(4u, ctx.responses().size());
  EXPECT_EQ(3u, CountEncodedMetrics(ctx.responses()[0]));
  EXPECT_EQ(3u, CountEncodedMetrics(ctx.responses()[1]));
  EXPECT_EQ(3u, CountEncodedMetrics(ctx.responses()[2]));
  EXPECT_EQ(3u, CountEncodedMetrics(ctx.responses()[3]));

  EXPECT_EQ(78u,
            GetMetricsSum(ctx.responses()[0]) +
                GetMetricsSum(ctx.responses()[1]) +
                GetMetricsSum(ctx.responses()[2]) +
                GetMetricsSum(ctx.responses()[3]));
}

}  // namespace
}  // namespace pw::metric
