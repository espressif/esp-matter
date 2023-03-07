# Copyright 2021 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
"""Wrapers for pyserial classes to log read and write data."""

import collections
import logging
from dataclasses import dataclass
import time
from typing import Optional

_LOG = logging.getLogger('pw_console')


@dataclass
class EventCountHistory:
    """Track counts of events over time.

    Example usage: ::

        events = EventCountHistory(
            base_count_units='Bytes',
            display_unit_title='KiB/s',
            display_unit_factor=0.001,
            interval=1.0,
            show_sparkline=True)

        # Log 1 event now.
        events.log(1)
        time.sleep(1)

        # Log 100 events at this time.
        events.log(100)
        time.sleep(1)

        events.log(200)
        time.sleep(1)
        events.log(400)
        print(events)
        ▂▄█ 0.400 [KiB/s]

    """

    base_count_units: str = 'Bytes'
    display_unit_title: str = 'KiB/s'
    display_unit_factor: float = 0.001
    interval: float = 1.0  # Number of seconds per sum of events.
    history_limit: int = 20
    scale_characters = ' ▁▂▃▄▅▆▇█'
    history: collections.deque = collections.deque()
    show_sparkline: bool = False
    _this_count: int = 0
    _last_count: int = 0
    _last_update_time: float = time.time()

    def log(self, count: int) -> None:
        self._this_count += count

        this_time = time.time()
        if this_time - self._last_update_time >= self.interval:
            self._last_update_time = this_time
            self._last_count = self._this_count
            self._this_count = 0
            self.history.append(self._last_count)

            if len(self.history) > self.history_limit:
                self.history.popleft()

    def last_count(self) -> float:
        return self._last_count * self.display_unit_factor

    def last_count_raw(self) -> int:
        return self._last_count

    def last_count_with_units(self) -> str:
        return '{:.3f} [{}]'.format(
            self._last_count * self.display_unit_factor, self.display_unit_title
        )

    def __repr__(self) -> str:
        sparkline = ''
        if self.show_sparkline:
            sparkline = self.sparkline()
        return ' '.join([sparkline, self.last_count_with_units()])

    def __pt_formatted_text__(self):
        return [('', self.__repr__())]

    def sparkline(
        self, min_value: int = 0, max_value: Optional[int] = None
    ) -> str:
        msg = ''.rjust(self.history_limit)
        if len(self.history) == 0:
            return msg

        minimum = min_value
        maximum = max_value if max_value else max(self.history)
        max_minus_min = maximum - min_value
        if max_minus_min == 0:
            return msg

        msg = ''
        for i in self.history:
            # (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min
            index = int(
                (((1.0 * i) - minimum) / max_minus_min)
                * len(self.scale_characters)
            )
            if index >= len(self.scale_characters):
                index = len(self.scale_characters) - 1
            msg += self.scale_characters[index]
        return msg.rjust(self.history_limit)
