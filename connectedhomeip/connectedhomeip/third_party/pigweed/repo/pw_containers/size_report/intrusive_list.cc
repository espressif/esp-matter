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

#include "base.h"

#ifndef ITEM_COUNT
#define ITEM_COUNT 1
#endif

struct ExampleItem : public pw::IntrusiveList<ExampleItem>::Item {};

struct ExampleContainer {
  pw::IntrusiveList<ExampleItem> item_list;
};

static struct IntrusiveListContainer : BaseContainer {
  ExampleContainer example_container;
  ExampleItem example_item[ITEM_COUNT];
} size_report_data;

int main() {
  for (auto& example_item : size_report_data.example_item) {
    size_report_data.example_container.item_list.push_front(example_item);
  }

  return size_report_data.LoadData();
}
