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
"""Container class for a single progress bar task."""

from dataclasses import dataclass
from typing import Optional

from prompt_toolkit.application import get_app_or_none
from prompt_toolkit.shortcuts.progress_bar import ProgressBarCounter


def _redraw_ui() -> None:
    """Signal the prompt_toolkit app to re-draw"""
    pt_app = get_app_or_none()
    if pt_app:
        pt_app.invalidate()


@dataclass
class ProgressBarTaskCounter:
    """Class to hold a single progress bar state."""

    name: str
    total: int
    count: int = 0
    completed: bool = False
    canceled: bool = False
    prompt_toolkit_counter: Optional[ProgressBarCounter] = None

    def mark_canceled(self):
        self.canceled = True
        self.prompt_toolkit_counter.stopped = True  # type: ignore

    def mark_completed(self):
        self.completed = True
        self.prompt_toolkit_counter.done = True  # type: ignore

    def check_completion(self) -> None:
        # Check for completion
        if self.count >= self.total:
            self.mark_completed()

    def stop_updating_prompt_toolkit_counter(self) -> None:
        """If count is over total, stop updating the prompt_toolkit ETA."""
        if self.count >= self.total:
            self.prompt_toolkit_counter.done = True  # type: ignore

    def update(self, count: int = 1) -> None:
        """Increment this counter."""
        self.count += count

        if self.prompt_toolkit_counter:
            self.prompt_toolkit_counter.items_completed += count
            self.stop_updating_prompt_toolkit_counter()
            _redraw_ui()

    def set_new_total(self, new_total: int) -> None:
        """Set a new total count."""
        self.count = new_total

        if self.prompt_toolkit_counter:
            self.prompt_toolkit_counter.items_completed = new_total
            self.stop_updating_prompt_toolkit_counter()
            _redraw_ui()
