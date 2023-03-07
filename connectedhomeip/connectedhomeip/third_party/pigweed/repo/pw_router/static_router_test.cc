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

#include "pw_router/static_router.h"

#include "gtest/gtest.h"
#include "pw_assert/check.h"
#include "pw_router/egress_function.h"

namespace pw::router {
namespace {

struct BasicPacket {
  static constexpr uint32_t kMagic = 0x8badf00d;

  constexpr BasicPacket(uint32_t addr, uint64_t data)
      : magic(kMagic), address(addr), priority(0), payload(data) {}

  constexpr BasicPacket(uint32_t addr, uint32_t prio, uint64_t data)
      : magic(kMagic), address(addr), priority(prio), payload(data) {}

  ConstByteSpan data() const { return as_bytes(span(this, 1)); }

  uint32_t magic;
  uint32_t address;
  uint32_t priority;
  uint64_t payload;
};

class BasicPacketParser : public PacketParser {
 public:
  constexpr BasicPacketParser() : packet_(nullptr) {}

  bool Parse(pw::ConstByteSpan packet) final {
    packet_ = reinterpret_cast<const BasicPacket*>(packet.data());
    return packet_->magic == BasicPacket::kMagic;
  }

  std::optional<uint32_t> GetDestinationAddress() const final {
    PW_DCHECK_NOTNULL(packet_);
    return packet_->address;
  }

  uint32_t priority() const {
    PW_DCHECK_NOTNULL(packet_);
    return packet_->priority;
  }

 private:
  const BasicPacket* packet_;
};

EgressFunction GoodEgress(+[](ConstByteSpan, const PacketParser&) {
  return OkStatus();
});
EgressFunction BadEgress(+[](ConstByteSpan, const PacketParser&) {
  return Status::ResourceExhausted();
});

TEST(StaticRouter, RoutePacket_RoutesToAnEgress) {
  BasicPacketParser parser;
  constexpr StaticRouter::Route routes[] = {{1, GoodEgress}, {2, BadEgress}};
  StaticRouter router(routes);

  EXPECT_EQ(router.RoutePacket(BasicPacket(1, 0xdddd).data(), parser),
            OkStatus());
  EXPECT_EQ(router.RoutePacket(BasicPacket(2, 0xdddd).data(), parser),
            Status::Unavailable());
}

TEST(StaticRouter, RoutePacket_ForwardsPacketParser) {
  uint32_t parser_priority = 0xffffffff;

  EgressFunction parser_egress(
      [&parser_priority](ConstByteSpan, const PacketParser& parser) {
        const BasicPacketParser& basic_parser =
            static_cast<const BasicPacketParser&>(parser);
        parser_priority = basic_parser.priority();
        return OkStatus();
      });

  StaticRouter::Route routes[] = {{1, parser_egress}};
  StaticRouter router(routes);
  BasicPacketParser parser;

  EXPECT_EQ(router.RoutePacket(BasicPacket(1, 71, 0xdddd).data(), parser),
            OkStatus());
  EXPECT_EQ(parser_priority, 71u);
}

TEST(StaticRouter, RoutePacket_ReturnsParserError) {
  BasicPacketParser parser;
  constexpr StaticRouter::Route routes[] = {{1, GoodEgress}, {2, BadEgress}};
  StaticRouter router(routes);

  BasicPacket bad_magic(1, 0xdddd);
  bad_magic.magic = 0x1badda7a;
  EXPECT_EQ(router.RoutePacket(bad_magic.data(), parser), Status::DataLoss());
}

TEST(StaticRouter, RoutePacket_ReturnsNotFoundOnInvalidRoute) {
  BasicPacketParser parser;
  constexpr StaticRouter::Route routes[] = {{1, GoodEgress}, {2, BadEgress}};
  StaticRouter router(routes);

  EXPECT_EQ(router.RoutePacket(BasicPacket(42, 0xdddd).data(), parser),
            Status::NotFound());
}

TEST(StaticRouter, RoutePacket_TracksNumberOfDrops) {
  BasicPacketParser parser;
  constexpr StaticRouter::Route routes[] = {{1, GoodEgress}, {2, BadEgress}};
  StaticRouter router(routes);

  // Good
  EXPECT_EQ(router.RoutePacket(BasicPacket(1, 0xdddd).data(), parser),
            OkStatus());

  // Egress error
  EXPECT_EQ(router.RoutePacket(BasicPacket(2, 0xdddd).data(), parser),
            Status::Unavailable());

  // Parser error
  BasicPacket bad_magic(1, 0xdddd);
  bad_magic.magic = 0x1badda7a;
  EXPECT_EQ(router.RoutePacket(bad_magic.data(), parser), Status::DataLoss());

  // Good
  EXPECT_EQ(router.RoutePacket(BasicPacket(1, 0xdddd).data(), parser),
            OkStatus());

  // Bad route
  EXPECT_EQ(router.RoutePacket(BasicPacket(42, 0xdddd).data(), parser),
            Status::NotFound());

  EXPECT_EQ(router.dropped_packets(), 3u);
}

}  // namespace
}  // namespace pw::router
