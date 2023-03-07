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
"""Mouse handler fuctions."""

from prompt_toolkit.application.current import get_app
from prompt_toolkit.filters import has_focus
from prompt_toolkit.mouse_events import MouseEvent, MouseEventType


def focus_handler(container, mouse_event: MouseEvent):
    """Focus container on click."""
    if not has_focus(container)():
        if mouse_event.event_type == MouseEventType.MOUSE_UP:
            get_app().layout.focus(container)
            return None
    return NotImplemented
