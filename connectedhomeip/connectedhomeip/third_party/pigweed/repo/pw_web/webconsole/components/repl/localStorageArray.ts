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

export default class LocalStorageArray {
  data: string[] = [];
  maxSize: number;
  key: string = "__pw_repl_history";

  constructor(maxSize: number = 4) {
    this.maxSize = maxSize;
    const curHistory = localStorage.getItem(this.key);
    if (curHistory) {
      this.data = JSON.parse(localStorage.getItem(this.key)!)
    }
  }

  unshift(data: string) {
    this.data.unshift(data);
    if (this.data.length > this.maxSize) {
      this.data = this.data.slice(0, this.maxSize);
    }
    localStorage.setItem(this.key, JSON.stringify(this.data));
  }

  clear() {
    this.data = [];
    localStorage.removeItem(this.key);
  }
}
