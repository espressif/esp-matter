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
#pragma once

#include "pw_containers/intrusive_list.h"

//
// Baseline data container structure.
//
// This structure is a container for data used to establish a baseline for size
// report diffs.
//
// The baseline contains one global instance of this structure. Other binaries
// should contain one global instance of a subclass of this structure, adding
// any data that should be a part of the size report diff.
//
// This structure may be used to load sections of code and data in order to
// establish a basline for size report diffs. This structure causes sections to
// be loaded by referencing symbols within those sections.
//
// Simple references to symbols can be optimized out if they don't have any
// external effects. The LoadData method uses functions and other symbols to
// generate and return a run-time value. In this way, the symbol references
// cannot be optimized out.
//
struct BaseContainer {
  class BaseItem : public pw::IntrusiveList<BaseItem>::Item {};

  BaseContainer() {
    static BaseItem item;
    GetList().push_front(item);
  }

  //
  // Causes code and data sections to be loaded. Returns a generated value
  // based on symbols in those sections to force them to be loaded. The caller
  // should ensure that the return value has some external effect (e.g.,
  // returning the value from the "main" function).
  //
  long LoadData() { return reinterpret_cast<long>(this) ^ GetList().size(); }

  static pw::IntrusiveList<BaseItem>& GetList() {
    static pw::IntrusiveList<BaseItem> list;
    return list;
  }

  // Explicit padding. If this structure is empty, the baseline will include
  // padding that won't appear in subclasses, so the padding is explicitly
  // added here so it appears in both the baseline and all subclasses.
  char padding[8];
};
