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

#include "pw_kvs/key.h"

#include <string_view>

#include "gtest/gtest.h"

namespace pw::kvs {

namespace {

constexpr const char kTestString[] = "test_string";
constexpr std::string_view kTestStringView{kTestString};

// kTestString2 starts with same string as kTestString.
constexpr const char kTestString2[] = "test_string2";

}  // namespace

TEST(Key, ConstructorEmpty) {
  Key key;
  EXPECT_EQ(key.size(), 0u);
  EXPECT_TRUE(key.empty());
  EXPECT_EQ(key.data(), nullptr);
  EXPECT_EQ(key.begin(), nullptr);
  EXPECT_EQ(key.end(), nullptr);
}

TEST(Key, ConstructorString) {
  std::string str{kTestStringView};
  Key key{str};
  EXPECT_EQ(key.size(), kTestStringView.size());
  EXPECT_FALSE(key.empty());
  EXPECT_EQ(key.data(), str.data());
  EXPECT_EQ(key.front(), kTestStringView.front());
  EXPECT_EQ(key.back(), kTestStringView.back());
}

TEST(Key, ConstructorStringView) {
  Key key{kTestStringView};
  EXPECT_EQ(key.size(), kTestStringView.size());
  EXPECT_FALSE(key.empty());
  EXPECT_EQ(key.data(), kTestStringView.data());
  EXPECT_EQ(key.front(), kTestStringView.front());
  EXPECT_EQ(key.back(), kTestStringView.back());
}

TEST(Key, ConstructorNullTermString) {
  Key key{kTestString};
  EXPECT_EQ(key.size(), kTestStringView.size());
  EXPECT_FALSE(key.empty());
  EXPECT_EQ(key.data(), kTestString);
  EXPECT_EQ(key.front(), kTestStringView.front());
  EXPECT_EQ(key.back(), kTestStringView.back());
}

TEST(Key, ConstructorCharPtrLength) {
  Key key{kTestString, kTestStringView.size() + 1};  // include null terminator
  EXPECT_EQ(key.size(), kTestStringView.size() + 1);
  EXPECT_FALSE(key.empty());
  EXPECT_EQ(key.data(), kTestStringView.data());
  EXPECT_EQ(key.front(), kTestStringView.front());
  EXPECT_EQ(key.back(), '\0');
}

TEST(Key, ConstructorCopy) {
  Key key1{kTestString};
  Key key{key1};
  EXPECT_EQ(key.size(), kTestStringView.size());
  EXPECT_FALSE(key.empty());
  EXPECT_EQ(key.data(), kTestStringView.data());
  EXPECT_EQ(key.front(), kTestStringView.front());
  EXPECT_EQ(key.back(), kTestStringView.back());
}

TEST(Key, Access) {
  Key key{kTestStringView};
  for (size_t i = 0; i < key.size(); i++) {
    EXPECT_EQ(key[i], kTestStringView[i]);
    EXPECT_EQ(key.at(i), kTestStringView.at(i));
  }
}

TEST(Key, Iterator) {
  size_t i = 0;
  for (auto c : Key{kTestString}) {
    EXPECT_EQ(c, kTestString[i++]);
  }
}

TEST(Key, Same) {
  // Since start of two test strings are the same, verify those are equal.
  EXPECT_TRUE((Key{kTestString} == Key{kTestString2, kTestStringView.size()}));
  EXPECT_FALSE((Key{kTestString} != Key{kTestString2, kTestStringView.size()}));
}

TEST(Key, Different) {
  EXPECT_FALSE(Key{kTestString} == Key{kTestString2});
  EXPECT_TRUE(Key{kTestString} != Key{kTestString2});
}

TEST(Key, DifferentWithSameLength) {
  // Start second test string offset by one.
  EXPECT_FALSE(
      (Key{kTestString} == Key{kTestString2 + 1, kTestStringView.size()}));
  EXPECT_TRUE(
      (Key{kTestString} != Key{kTestString2 + 1, kTestStringView.size()}));
}

TEST(Key, ConvertToStringView) {
  std::string_view view = Key{kTestString};
  EXPECT_TRUE(view == kTestStringView);
}

}  // namespace pw::kvs
