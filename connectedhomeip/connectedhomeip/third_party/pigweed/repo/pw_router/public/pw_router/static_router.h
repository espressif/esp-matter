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

#include "pw_bytes/span.h"
#include "pw_metric/metric.h"
#include "pw_router/egress.h"
#include "pw_router/packet_parser.h"
#include "pw_span/span.h"
#include "pw_status/status.h"

namespace pw::router {

// A packet router with a static routing table.
//
// Thread-safety:
//   Internal packet parsing and calls to the provided PacketParser are
//   synchronized. Synchronization at the egress level must be implemented by
//   derived egresses.
//
class StaticRouter {
 public:
  struct Route {
    // TODO(frolv): Consider making address size configurable.
    uint32_t address;
    Egress& egress;
  };

  StaticRouter(span<const Route> routes) : routes_(routes) {}

  StaticRouter(const StaticRouter&) = delete;
  StaticRouter(StaticRouter&&) = delete;
  StaticRouter& operator=(const StaticRouter&) = delete;
  StaticRouter& operator=(StaticRouter&&) = delete;

  uint32_t dropped_packets() const {
    return parser_errors_.value() + route_errors_.value() +
           egress_errors_.value();
  }

  const metric::Group& metrics() { return metrics_; }

  // Routes a single packet through the appropriate egress.
  // Returns one of the following to indicate a router-side error:
  //
  //   OK - Packet sent successfully.
  //   DATA_LOSS - Packet corrupt or incomplete.
  //   NOT_FOUND - No registered route for the packet.
  //   UNAVAILABLE - Route egress did not accept packet.
  //
  Status RoutePacket(ConstByteSpan packet, PacketParser& parser);

 private:
  const span<const Route> routes_;
  PW_METRIC_GROUP(metrics_, "static_router");
  PW_METRIC(metrics_, parser_errors_, "parser_errors", 0u);
  PW_METRIC(metrics_, route_errors_, "route_errors", 0u);
  PW_METRIC(metrics_, egress_errors_, "egress_errors", 0u);
};

}  // namespace pw::router
