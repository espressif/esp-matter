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

/**
 * Provides a simple array-based queue that will block caller on await
 * queue.shift() if the queue is empty, until a new item is pushed to the
 * queue. */

export default class Queue<T> {
  queue = Array<T>();
  elementListeners = Array<() => void>();

  get length(): number {
    return this.queue.length;
  }

  push(...items: T[]): number {
    this.queue.push(...items);
    this._checkListeners();
    return this.length;
  }

  shift(): Promise<T> {
    return new Promise(resolve => {
      if (this.length > 0) {
        return resolve(this.queue.shift()!);
      } else {
        this.elementListeners.push(() => {
          return resolve(this.queue.shift()!);
        });
      }
    });
  }

  _checkListeners() {
    if (this.length > 0 && this.elementListeners.length > 0) {
      const listener = this.elementListeners.shift()!;
      listener.call(this);
      this._checkListeners();
    }
  }

  unshift(...items: T[]): number {
    this.queue.unshift(...items);
    this._checkListeners();
    return this.length;
  }
}
