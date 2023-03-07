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
"""WindowList"""

import collections
from enum import Enum
import functools
import logging
from typing import Any, List, Optional, TYPE_CHECKING

from prompt_toolkit.filters import has_focus
from prompt_toolkit.layout import (
    Dimension,
    FormattedTextControl,
    HSplit,
    HorizontalAlign,
    VSplit,
    Window,
    WindowAlign,
)
from prompt_toolkit.mouse_events import MouseEvent, MouseEventType, MouseButton

from pw_console.widgets import mouse_handlers as pw_console_mouse_handlers

if TYPE_CHECKING:
    # pylint: disable=ungrouped-imports
    from pw_console.window_manager import WindowManager

_LOG = logging.getLogger(__package__)


class DisplayMode(Enum):
    """WindowList display modes."""

    STACK = 'Stacked'
    TABBED = 'Tabbed'


DEFAULT_DISPLAY_MODE = DisplayMode.STACK

# Weighted amount for adjusting window dimensions when enlarging and shrinking.
_WINDOW_HEIGHT_ADJUST = 1


class WindowListHSplit(HSplit):
    """PromptToolkit HSplit class with some additions for size and mouse resize.

    This HSplit has a write_to_screen function that saves the width and height
    of the container for the current render pass. It also handles overriding
    mouse handlers for triggering window resize adjustments.
    """

    def __init__(self, parent_window_list, *args, **kwargs):
        # Save a reference to the parent window pane.
        self.parent_window_list = parent_window_list
        super().__init__(*args, **kwargs)

    def write_to_screen(
        self,
        screen,
        mouse_handlers,
        write_position,
        parent_style: str,
        erase_bg: bool,
        z_index: Optional[int],
    ) -> None:
        new_mouse_handlers = mouse_handlers
        # Is resize mode active?
        if self.parent_window_list.resize_mode:
            # Ignore future mouse_handler updates.
            new_mouse_handlers = pw_console_mouse_handlers.EmptyMouseHandler()
            # Set existing mouse_handlers to the parent_window_list's
            # mouse_handler. This will handle triggering resize events.
            mouse_handlers.set_mouse_handler_for_range(
                write_position.xpos,
                write_position.xpos + write_position.width,
                write_position.ypos,
                write_position.ypos + write_position.height,
                self.parent_window_list.mouse_handler,
            )

        # Save the width, height, and draw position for the current render pass.
        self.parent_window_list.update_window_list_size(
            write_position.width,
            write_position.height,
            write_position.xpos,
            write_position.ypos,
        )
        # Continue writing content to the screen.
        super().write_to_screen(
            screen,
            new_mouse_handlers,
            write_position,
            parent_style,
            erase_bg,
            z_index,
        )


class WindowList:
    """WindowList holds a stack of windows for the WindowManager."""

    # pylint: disable=too-many-instance-attributes,too-many-public-methods
    def __init__(
        self,
        window_manager: 'WindowManager',
    ):
        self.window_manager = window_manager
        self.application = window_manager.application

        self.current_window_list_width: int = 0
        self.current_window_list_height: int = 0
        self.last_window_list_width: int = 0
        self.last_window_list_height: int = 0

        self.current_window_list_xposition: int = 0
        self.last_window_list_xposition: int = 0
        self.current_window_list_yposition: int = 0
        self.last_window_list_yposition: int = 0

        self.display_mode = DEFAULT_DISPLAY_MODE
        self.active_panes: collections.deque = collections.deque()
        self.focused_pane_index: Optional[int] = None

        self.height = Dimension(preferred=10)
        self.width = Dimension(preferred=10)

        self.resize_mode = False
        self.resize_target_pane_index = None
        self.resize_target_pane = None
        self.resize_current_row = 0

        # Reference to the current prompt_toolkit window split for the current
        # set of active_panes.
        self.container = None

    def _calculate_actual_heights(self) -> List[int]:
        heights = [
            p.height.preferred if p.show_pane else 0 for p in self.active_panes
        ]
        available_height = self.current_window_list_height
        remaining_rows = available_height - sum(heights)
        window_index = 0

        # Distribute remaining unaccounted rows to each window in turn.
        while remaining_rows > 0:
            # 0 heights are hiden windows, only add +1 to visible windows.
            if heights[window_index] > 0:
                heights[window_index] += 1
                remaining_rows -= 1
            window_index = (window_index + 1) % len(heights)

        return heights

    def _update_resize_current_row(self):
        heights = self._calculate_actual_heights()
        start_row = 0

        # Find the starting row
        for i in range(self.resize_target_pane_index + 1):
            # If we are past the current pane, exit the loop.
            if i > self.resize_target_pane_index:
                break
            # 0 heights are hidden windows, only count visible windows.
            if heights[i] > 0:
                start_row += heights[i]
        self.resize_current_row = start_row

    def start_resize(self, target_pane, pane_index):
        # Can only resize if view mode is stacked.
        if self.display_mode != DisplayMode.STACK:
            return

        # Check the target_pane isn't the last one in the list
        visible_panes = [pane for pane in self.active_panes if pane.show_pane]
        if target_pane == visible_panes[-1]:
            return

        self.resize_mode = True
        self.resize_target_pane_index = pane_index
        self._update_resize_current_row()

    def stop_resize(self):
        self.resize_mode = False
        self.resize_target_pane_index = None
        self.resize_current_row = 0

    def get_tab_mode_active_pane(self):
        if self.focused_pane_index is None:
            self.focused_pane_index = 0

        pane = None
        try:
            pane = self.active_panes[self.focused_pane_index]
        except IndexError:
            # Ignore ValueError which can be raised by the self.active_panes
            # deque if existing_pane can't be found.
            self.focused_pane_index = 0
            pane = self.active_panes[self.focused_pane_index]
        return pane

    def get_current_active_pane(self):
        """Return the current active window pane."""
        focused_pane = None

        command_runner_focused_pane = None
        if self.application.command_runner_is_open():
            command_runner_focused_pane = (
                self.application.command_runner_last_focused_pane()
            )

        for index, pane in enumerate(self.active_panes):
            in_focus = False
            if has_focus(pane)():
                in_focus = True
            elif command_runner_focused_pane and pane.has_child_container(
                command_runner_focused_pane
            ):
                in_focus = True

            if in_focus:
                focused_pane = pane
                self.focused_pane_index = index
                break
        return focused_pane

    def get_pane_titles(self, omit_subtitles=False, use_menu_title=True):
        fragments = []
        separator = ('', ' ')
        fragments.append(separator)
        for pane_index, pane in enumerate(self.active_panes):
            title = pane.menu_title() if use_menu_title else pane.pane_title()
            subtitle = pane.pane_subtitle()
            text = f' {title} {subtitle} '
            if omit_subtitles:
                text = f' {title} '

            fragments.append(
                (
                    # Style
                    (
                        'class:window-tab-active'
                        if pane_index == self.focused_pane_index
                        else 'class:window-tab-inactive'
                    ),
                    # Text
                    text,
                    # Mouse handler
                    functools.partial(
                        pw_console_mouse_handlers.on_click,
                        functools.partial(self.switch_to_tab, pane_index),
                    ),
                )
            )
            fragments.append(separator)
        return fragments

    def switch_to_tab(self, index: int):
        self.focused_pane_index = index

        # Make the selected tab visible and hide the rest.
        for i, pane in enumerate(self.active_panes):
            pane.show_pane = False
            if i == index:
                pane.show_pane = True

        # refresh_ui() will focus on the new tab container.
        self.refresh_ui()

    def set_display_mode(self, mode: DisplayMode):
        self.display_mode = mode

        if self.display_mode == DisplayMode.TABBED:
            # Default to focusing on the first window / tab.
            self.focused_pane_index = 0
            # Hide all other panes so log redraw events are not triggered.
            for pane in self.active_panes:
                pane.show_pane = False
            # Keep the selected tab visible
            self.active_panes[self.focused_pane_index].show_pane = True
        else:
            # Un-hide all panes if switching from tabbed back to stacked.
            for pane in self.active_panes:
                pane.show_pane = True

        self.application.focus_main_menu()
        self.refresh_ui()

    def refresh_ui(self):
        self.window_manager.update_root_container_body()
        # Update menu after the window manager rebuilds the root container.
        self.application.update_menu_items()

        if self.display_mode == DisplayMode.TABBED:
            self.application.focus_on_container(
                self.active_panes[self.focused_pane_index]
            )

        self.application.redraw_ui()

    def _set_window_heights(self, new_heights: List[int]):
        for pane in self.active_panes:
            if not pane.show_pane:
                continue
            pane.height = Dimension(preferred=new_heights[0])
            new_heights = new_heights[1:]

    def rebalance_window_heights(self):
        available_height = self.current_window_list_height

        old_values = [
            p.height.preferred for p in self.active_panes if p.show_pane
        ]
        # Make sure the old total is not zero.
        old_total = max(sum(old_values), 1)
        percentages = [value / old_total for value in old_values]
        new_heights = [
            int(available_height * percentage) for percentage in percentages
        ]

        self._set_window_heights(new_heights)

    def update_window_list_size(
        self, width, height, xposition, yposition
    ) -> None:
        """Save width and height of the repl pane for the current UI render
        pass."""
        if width:
            self.last_window_list_width = self.current_window_list_width
            self.current_window_list_width = width
        if height:
            self.last_window_list_height = self.current_window_list_height
            self.current_window_list_height = height
        if xposition:
            self.last_window_list_xposition = self.current_window_list_xposition
            self.current_window_list_xposition = xposition
        if yposition:
            self.last_window_list_yposition = self.current_window_list_yposition
            self.current_window_list_yposition = yposition

        if (
            self.current_window_list_width != self.last_window_list_width
            or self.current_window_list_height != self.last_window_list_height
        ):
            self.rebalance_window_heights()

    def mouse_handler(self, mouse_event: MouseEvent):
        mouse_position = mouse_event.position

        if (
            mouse_event.event_type == MouseEventType.MOUSE_MOVE
            and mouse_event.button == MouseButton.LEFT
        ):
            self.mouse_resize(mouse_position.x, mouse_position.y)
        elif mouse_event.event_type == MouseEventType.MOUSE_UP:
            self.stop_resize()
            # Mouse event handled, return None.
            return None
        else:
            self.stop_resize()

        # Mouse event not handled, return NotImplemented.
        return NotImplemented

    def update_container(self):
        """Re-create the window list split depending on the display mode."""

        if self.display_mode == DisplayMode.STACK:
            content_split = WindowListHSplit(
                self,
                list(pane for pane in self.active_panes if pane.show_pane),
                height=lambda: self.height,
                width=lambda: self.width,
            )

        elif self.display_mode == DisplayMode.TABBED:
            content_split = WindowListHSplit(
                self,
                [
                    self._create_window_tab_toolbar(),
                    self.get_tab_mode_active_pane(),
                ],
                height=lambda: self.height,
                width=lambda: self.width,
            )

        self.container = content_split

    def _create_window_tab_toolbar(self):
        tab_bar_control = FormattedTextControl(
            functools.partial(
                self.get_pane_titles, omit_subtitles=True, use_menu_title=False
            )
        )
        tab_bar_window = Window(
            content=tab_bar_control,
            align=WindowAlign.LEFT,
            dont_extend_width=True,
        )

        spacer = Window(
            content=FormattedTextControl([('', '')]),
            align=WindowAlign.LEFT,
            dont_extend_width=False,
        )

        tab_toolbar = VSplit(
            [
                tab_bar_window,
                spacer,
            ],
            style='class:toolbar_dim_inactive',
            height=1,
            align=HorizontalAlign.LEFT,
        )
        return tab_toolbar

    def empty(self) -> bool:
        return len(self.active_panes) == 0

    def pane_index(self, pane):
        pane_index = None
        try:
            pane_index = self.active_panes.index(pane)
        except ValueError:
            # Ignore ValueError which can be raised by the self.active_panes
            # deque if existing_pane can't be found.
            pass
        return pane_index

    def add_pane_no_checks(self, pane: Any, add_at_beginning=False):
        if add_at_beginning:
            self.active_panes.appendleft(pane)
        else:
            self.active_panes.append(pane)

    def add_pane(self, new_pane, existing_pane=None, add_at_beginning=False):
        existing_pane_index = self.pane_index(existing_pane)
        if existing_pane_index is not None:
            self.active_panes.insert(new_pane, existing_pane_index + 1)
        else:
            if add_at_beginning:
                self.active_panes.appendleft(new_pane)
            else:
                self.active_panes.append(new_pane)

        self.refresh_ui()

    def remove_pane_no_checks(self, pane: Any):
        try:
            self.active_panes.remove(pane)
        except ValueError:
            # ValueError will be raised if the the pane is not found
            pass
        return pane

    def remove_pane(self, existing_pane):
        existing_pane_index = self.pane_index(existing_pane)
        if existing_pane_index is None:
            return

        self.active_panes.remove(existing_pane)
        self.refresh_ui()

        # Set focus to the previous window pane
        if len(self.active_panes) > 0:
            existing_pane_index -= 1
            try:
                self.application.focus_on_container(
                    self.active_panes[existing_pane_index]
                )
            except ValueError:
                # ValueError will be raised if the the pane at
                # existing_pane_index can't be accessed.
                # Focus on the main menu if the existing pane is hidden.
                self.application.focus_main_menu()

        self.application.redraw_ui()

    def enlarge_pane(self):
        """Enlarge the currently focused window pane."""
        pane = self.get_current_active_pane()
        if pane:
            self.adjust_pane_size(pane, _WINDOW_HEIGHT_ADJUST)

    def shrink_pane(self):
        """Shrink the currently focused window pane."""
        pane = self.get_current_active_pane()
        if pane:
            self.adjust_pane_size(pane, -_WINDOW_HEIGHT_ADJUST)

    def mouse_resize(self, _xpos, ypos) -> None:
        if self.resize_target_pane_index is None:
            return

        target_pane = self.active_panes[self.resize_target_pane_index]

        diff = ypos - self.resize_current_row
        if not self.window_manager.vertical_window_list_spliting():
            # The mouse ypos value includes rows from other window lists. If
            # horizontal splitting is active we need to check the diff relative
            # to the starting y position row. Subtract the start y position and
            # an additional 1 for the top menu bar.
            diff -= self.current_window_list_yposition - 1

        if diff == 0:
            return
        self.adjust_pane_size(target_pane, diff)
        self._update_resize_current_row()
        self.application.redraw_ui()

    def adjust_pane_size(self, pane, diff: int = _WINDOW_HEIGHT_ADJUST) -> None:
        """Increase or decrease a given pane's height."""
        # Placeholder next_pane value to allow setting width and height without
        # any consequences if there is no next visible pane.
        next_pane = HSplit(
            [], height=Dimension(preferred=10), width=Dimension(preferred=10)
        )  # type: ignore
        # Try to get the next visible pane to subtract a weight value from.
        next_visible_pane = self._get_next_visible_pane_after(pane)
        if next_visible_pane:
            next_pane = next_visible_pane

        # If the last pane is selected, and there are at least 2 panes, make
        # next_pane the previous pane.
        try:
            if len(self.active_panes) >= 2 and (
                self.active_panes.index(pane) == len(self.active_panes) - 1
            ):
                next_pane = self.active_panes[-2]
        except ValueError:
            # Ignore ValueError raised if self.active_panes[-2] doesn't exist.
            pass

        old_height = pane.height.preferred
        if diff < 0 and old_height <= 1:
            return
        next_old_height = next_pane.height.preferred  # type: ignore

        # Add to the current pane
        new_height = old_height + diff
        if new_height <= 0:
            new_height = old_height

        # Subtract from the next pane
        next_new_height = next_old_height - diff
        if next_new_height <= 0:
            next_new_height = next_old_height

        # If new height is too small or no change, make no adjustments.
        if new_height < 3 or next_new_height < 3 or old_height == new_height:
            return

        # Set new heigts of the target pane and next pane.
        pane.height.preferred = new_height
        next_pane.height.preferred = next_new_height  # type: ignore

    def reset_pane_sizes(self):
        """Reset all active pane heights evenly."""

        available_height = self.current_window_list_height
        old_values = [
            p.height.preferred for p in self.active_panes if p.show_pane
        ]
        new_heights = [int(available_height / len(old_values))] * len(
            old_values
        )

        self._set_window_heights(new_heights)

    def move_pane_up(self):
        pane = self.get_current_active_pane()
        pane_index = self.pane_index(pane)
        if pane_index is None or pane_index <= 0:
            # Already at the beginning
            return

        # Swap with the previous pane
        previous_pane = self.active_panes[pane_index - 1]
        self.active_panes[pane_index - 1] = pane
        self.active_panes[pane_index] = previous_pane

        self.refresh_ui()

    def move_pane_down(self):
        pane = self.get_current_active_pane()
        pane_index = self.pane_index(pane)
        pane_count = len(self.active_panes)
        if pane_index is None or pane_index + 1 >= pane_count:
            # Already at the end
            return

        # Swap with the next pane
        next_pane = self.active_panes[pane_index + 1]
        self.active_panes[pane_index + 1] = pane
        self.active_panes[pane_index] = next_pane

        self.refresh_ui()

    def _get_next_visible_pane_after(self, target_pane):
        """Return the next visible pane that appears after the target pane."""
        try:
            target_pane_index = self.active_panes.index(target_pane)
        except ValueError:
            # If pane can't be found, focus on the main menu.
            return None

        # Loop through active panes (not including the target_pane).
        for i in range(1, len(self.active_panes)):
            next_pane_index = (target_pane_index + i) % len(self.active_panes)
            next_pane = self.active_panes[next_pane_index]
            if next_pane.show_pane:
                return next_pane
        return None
