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

#include "gtest/gtest.h"
#include "proto_root/aggregate.pwpb.h"
#include "proto_root/aggregate_wrapper.pwpb.h"
#include "proto_root/data_type/id/id.pwpb.h"
#include "proto_root/data_type/thing/thing.pwpb.h"
#include "proto_root/data_type/thing/type_of_thing.pwpb.h"

namespace pw::protobuf_compiler {
namespace {

TEST(NestedPackages, CompilesProtobufs) {
  using Aggregate = proto_root::pwpb::Aggregate::Message;
  using AggregateWrapper = proto_root::pwpb::AggregateWrapper::Message;
  using Id = proto_root::data_type::id::pwpb::Id::Message;
  using Thing = proto_root::data_type::thing::pwpb::Thing::Message;
  using proto_root::data_type::thing::pwpb::TypeOfThing;

  AggregateWrapper wrapper = {
      Id{0u, {1, 2}},
      TypeOfThing::kOther,

      Aggregate{
          Id{1u, {2, 3}},
          TypeOfThing::kOther,

          Thing{Id{2u, {3, 4}}, TypeOfThing::kOrganism},
          Thing{Id{3u, {4, 5}}, TypeOfThing::kSubstance},
          Thing{Id{4u, {5, 6}}, TypeOfThing::kObject},
      }};

  EXPECT_EQ(wrapper.id.id, 0u);
  EXPECT_EQ(wrapper.id.impl.foo, 1);
  EXPECT_EQ(wrapper.id.impl.bar, 2);
  EXPECT_EQ(wrapper.type, TypeOfThing::kOther);

  EXPECT_EQ(wrapper.aggregate.id.id, 1u);
  EXPECT_EQ(wrapper.aggregate.id.impl.foo, 2);
  EXPECT_EQ(wrapper.aggregate.id.impl.bar, 3);
  EXPECT_EQ(wrapper.aggregate.type, TypeOfThing::kOther);

  EXPECT_EQ(wrapper.aggregate.alice.id.id, 2u);
  EXPECT_EQ(wrapper.aggregate.alice.id.impl.foo, 3);
  EXPECT_EQ(wrapper.aggregate.alice.id.impl.bar, 4);
  EXPECT_EQ(wrapper.aggregate.alice.type, TypeOfThing::kOrganism);

  EXPECT_EQ(wrapper.aggregate.neodymium.id.id, 3u);
  EXPECT_EQ(wrapper.aggregate.neodymium.id.impl.foo, 4);
  EXPECT_EQ(wrapper.aggregate.neodymium.id.impl.bar, 5);
  EXPECT_EQ(wrapper.aggregate.neodymium.type, TypeOfThing::kSubstance);

  EXPECT_EQ(wrapper.aggregate.mountain.id.id, 4u);
  EXPECT_EQ(wrapper.aggregate.mountain.id.impl.foo, 5);
  EXPECT_EQ(wrapper.aggregate.mountain.id.impl.bar, 6);
  EXPECT_EQ(wrapper.aggregate.mountain.type, TypeOfThing::kObject);
}

}  // namespace
}  // namespace pw::protobuf_compiler
