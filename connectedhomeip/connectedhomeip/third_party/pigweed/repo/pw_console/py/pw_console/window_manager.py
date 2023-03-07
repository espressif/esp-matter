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
"""WindowManager"""

import collections
import copy
import functools
from itertools import chain
import logging
import operator
from typing import Any, Dict, Iterable, List, Optional

from prompt_toolkit.key_binding import KeyBindings
from prompt_toolkit.layout import (
    Dimension,
    HSplit,
    VSplit,
    FormattedTextControl,
    Window,
    WindowAlign,
)
from prompt_toolkit.mouse_events import MouseEvent, MouseEventType, MouseButton
from prompt_toolkit.widgets import MenuItem

from pw_console.console_prefs import ConsolePrefs, error_unknown_window
from pw_console.log_pane import LogPane
from pw_console.widgets import (
    WindowPaneToolbar,
    to_checkbox_text,
)
from pw_console.widgets import mouse_handlers as pw_console_mouse_handlers
from pw_console.window_list import WindowList, DisplayMode

_LOG = logging.getLogger(__package__)

# Amount for adjusting window dimensions when enlarging and shrinking.
_WINDOW_SPLIT_ADJUST = 1


class WindowListResizeHandle(FormattedTextControl):
    """Button to initiate window list resize drag events."""

    def __init__(
        self, window_manager, window_list: Any, *args, **kwargs
    ) -> None:
        self.window_manager = window_manager
        self.window_list = window_list
        super().__init__(*args, **kwargs)

    def mouse_handler(self, mouse_event: MouseEvent):
        """Mouse handler for this control."""
        # Start resize mouse drag event
        if mouse_event.event_type == MouseEventType.MOUSE_DOWN:
            self.window_manager.start_resize(self.window_list)
            # Mouse event handled, return None.
            return None

        # Mouse event not handled, return NotImplemented.
        return NotImplemented


class WindowManagerVSplit(VSplit):
    """PromptToolkit VSplit class with some additions for size and mouse resize.

    This VSplit has a write_to_screen function that saves the width and height
    of the container for the current render pass. It also handles overriding
    mouse handlers for triggering window resize adjustments.
    """

    def __init__(self, parent_window_manager, *args, **kwargs):
        # Save a reference to the parent window pane.
        self.parent_window_manager = parent_window_manager
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
        if self.parent_window_manager.resize_mode:
            # Ignore future mouse_handler updates.
            new_mouse_handlers = pw_console_mouse_handlers.EmptyMouseHandler()
            # Set existing mouse_handlers to the parent_window_managers's
            # mouse_handler. This will handle triggering resize events.
            mouse_handlers.set_mouse_handler_for_range(
                write_position.xpos,
                write_position.xpos + write_position.width,
                write_position.ypos,
                write_position.ypos + write_position.height,
                self.parent_window_manager.mouse_handler,
            )

        # Save the width and height for the current render pass.
        self.parent_window_manager.update_window_manager_size(
            write_position.width, write_position.height
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


class WindowManagerHSplit(HSplit):
    """PromptToolkit HSplit class with some additions for size and mouse resize.

    This HSplit has a write_to_screen function that saves the width and height
    of the container for the current render pass. It also handles overriding
    mouse handlers for triggering window resize adjustments.
    """

    def __init__(self, parent_window_manager, *args, **kwargs):
        # Save a reference to the parent window pane.
        self.parent_window_manager = parent_window_manager
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
        if self.parent_window_manager.resize_mode:
            # Ignore future mouse_handler updates.
            new_mouse_handlers = pw_console_mouse_handlers.EmptyMouseHandler()
            # Set existing mouse_handlers to the parent_window_managers's
            # mouse_handler. This will handle triggering resize events.
            mouse_handlers.set_mouse_handler_for_range(
                write_position.xpos,
                write_position.xpos + write_position.width,
                write_position.ypos,
                write_position.ypos + write_position.height,
                self.parent_window_manager.mouse_handler,
            )

        # Save the width and height for the current render pass.
        self.parent_window_manager.update_window_manager_size(
            write_position.width, write_position.height
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


class WindowManager:
    """WindowManager class

    This class handles adding/removing/resizing windows and rendering the
    prompt_toolkit split layout."""

    # pylint: disable=too-many-public-methods,too-many-instance-attributes

    def __init__(
        self,
        application: Any,
    ):
        self.application = application
        self.window_lists: collections.deque = collections.deque()
        self.window_lists.append(WindowList(self))
        self.key_bindings = self._create_key_bindings()
        self.top_toolbars: List[WindowPaneToolbar] = []
        self.bottom_toolbars: List[WindowPaneToolbar] = []

        self.resize_mode: bool = False
        self.resize_target_window_list_index: Optional[int] = None
        self.resize_target_window_list: Optional[int] = None
        self.resize_current_row: int = 0
        self.resize_current_column: int = 0

        self.current_window_manager_width: int = 0
        self.current_window_manager_height: int = 0
        self.last_window_manager_width: int = 0
        self.last_window_manager_height: int = 0

    def update_window_manager_size(self, width, height):
        """Save width and height for the current UI render pass."""
        if width:
            self.last_window_manager_width = self.current_window_manager_width
            self.current_window_manager_width = width
        if height:
            self.last_window_manager_height = self.current_window_manager_height
            self.current_window_manager_height = height

        if (
            self.current_window_manager_width != self.last_window_manager_width
            or self.current_window_manager_height
            != self.last_window_manager_height
        ):
            self.rebalance_window_list_sizes()

    def _set_window_list_sizes(
        self, new_heights: List[int], new_widths: List[int]
    ) -> None:
        for window_list in self.window_lists:
            window_list.height = Dimension(preferred=new_heights[0])
            new_heights = new_heights[1:]
            window_list.width = Dimension(preferred=new_widths[0])
            new_widths = new_widths[1:]

    def vertical_window_list_spliting(self) -> bool:
        return self.application.prefs.window_column_split_method == 'vertical'

    def rebalance_window_list_sizes(self) -> None:
        """Adjust relative split sizes to fill available space."""
        available_height = self.current_window_manager_height
        available_width = self.current_window_manager_width

        old_heights = [w.height.preferred for w in self.window_lists]
        old_widths = [w.width.preferred for w in self.window_lists]

        # Make sure the old totals are not zero.
        old_height_total = max(sum(old_heights), 1)
        old_width_total = max(sum(old_widths), 1)

        height_percentages = [value / old_height_total for value in old_heights]
        width_percentages = [value / old_width_total for value in old_widths]

        new_heights = [
            int(available_height * percentage)
            for percentage in height_percentages
        ]
        new_widths = [
            int(available_width * percentage)
            for percentage in width_percentages
        ]

        if self.vertical_window_list_spliting():
            new_heights = [
                self.current_window_manager_height for h in new_heights
            ]
        else:
            new_widths = [self.current_window_manager_width for h in new_widths]

        self._set_window_list_sizes(new_heights, new_widths)

    def _create_key_bindings(self) -> KeyBindings:
        key_bindings = KeyBindings()
        register = self.application.prefs.register_keybinding

        @register('window-manager.move-pane-left', key_bindings)
        def move_pane_left(_event):
            """Move window pane left."""
            self.move_pane_left()

        @register('window-manager.move-pane-right', key_bindings)
        def move_pane_right(_event):
            """Move window pane right."""
            self.move_pane_right()

        @register('window-manager.move-pane-down', key_bindings)
        def move_pane_down(_event):
            """Move window pane down."""
            self.move_pane_down()

        @register('window-manager.move-pane-up', key_bindings)
        def move_pane_up(_event):
            """Move window pane up."""
            self.move_pane_up()

        @register('window-manager.enlarge-pane', key_bindings)
        def enlarge_pane(_event):
            """Enlarge the active window pane."""
            self.enlarge_pane()

        @register('window-manager.shrink-pane', key_bindings)
        def shrink_pane(_event):
            """Shrink the active window pane."""
            self.shrink_pane()

        @register('window-manager.shrink-split', key_bindings)
        def shrink_split(_event):
            """Shrink the current window split."""
            self.shrink_split()

        @register('window-manager.enlarge-split', key_bindings)
        def enlarge_split(_event):
            """Enlarge the current window split."""
            self.enlarge_split()

        @register('window-manager.focus-prev-pane', key_bindings)
        def focus_prev_pane(_event):
            """Switch focus to the previous window pane or tab."""
            self.focus_previous_pane()

        @register('window-manager.focus-next-pane', key_bindings)
        def focus_next_pane(_event):
            """Switch focus to the next window pane or tab."""
            self.focus_next_pane()

        @register('window-manager.balance-window-panes', key_bindings)
        def balance_window_panes(_event):
            """Balance all window sizes."""
            self.balance_window_sizes()

        return key_bindings

    def delete_empty_window_lists(self):
        empty_lists = [
            window_list
            for window_list in self.window_lists
            if window_list.empty()
        ]
        for empty_list in empty_lists:
            self.window_lists.remove(empty_list)

    def add_top_toolbar(self, toolbar: WindowPaneToolbar) -> None:
        self.top_toolbars.append(toolbar)

    def add_bottom_toolbar(self, toolbar: WindowPaneToolbar) -> None:
        self.bottom_toolbars.append(toolbar)

    def create_root_container(self):
        """Create vertical or horizontal splits for all active panes."""
        self.delete_empty_window_lists()

        for window_list in self.window_lists:
            window_list.update_container()

        vertical_split = self.vertical_window_list_spliting()

        window_containers = []
        for i, window_list in enumerate(self.window_lists):
            window_containers.append(window_list.container)
            if (i + 1) >= len(self.window_lists):
                continue

            if vertical_split:
                separator_padding = Window(
                    content=WindowListResizeHandle(self, window_list, "│"),
                    char='│',
                    width=1,
                    dont_extend_height=False,
                )
                resize_separator = HSplit(
                    [
                        separator_padding,
                        Window(
                            content=WindowListResizeHandle(
                                self, window_list, "║\n║\n║"
                            ),
                            char='│',
                            width=1,
                            dont_extend_height=True,
                        ),
                        separator_padding,
                    ],
                    style='class:pane_separator',
                )
            else:
                resize_separator = Window(
                    content=WindowListResizeHandle(self, window_list, "════"),
                    char='─',
                    height=1,
                    align=WindowAlign.CENTER,
                    dont_extend_width=False,
                    style='class:pane_separator',
                )
            window_containers.append(resize_separator)

        if vertical_split:
            split = WindowManagerVSplit(self, window_containers)
        else:
            split = WindowManagerHSplit(self, window_containers)

        split_items = []
        split_items.extend(self.top_toolbars)
        split_items.append(split)
        split_items.extend(self.bottom_toolbars)
        return HSplit(split_items)

    def update_root_container_body(self):
        # Replace the root MenuContainer body with the new split.
        self.application.root_container.container.content.children[
            1
        ] = self.create_root_container()

    def _get_active_window_list_and_pane(self):
        active_pane = None
        active_window_list = None
        for window_list in self.window_lists:
            active_pane = window_list.get_current_active_pane()
            if active_pane:
                active_window_list = window_list
                break
        return active_window_list, active_pane

    def window_list_index(self, window_list: WindowList) -> Optional[int]:
        index = None
        try:
            index = self.window_lists.index(window_list)
        except ValueError:
            # Ignore ValueError which can be raised by the self.window_lists
            # deque if the window_list can't be found.
            pass
        return index

    def run_action_on_active_pane(self, function_name):
        (
            _active_window_list,
            active_pane,
        ) = self._get_active_window_list_and_pane()
        if not hasattr(active_pane, function_name):
            return
        method_to_call = getattr(active_pane, function_name)
        method_to_call()
        return

    def focus_previous_pane(self) -> None:
        """Focus on the previous visible window pane or tab."""
        self.focus_next_pane(reverse_order=True)

    def focus_next_pane(self, reverse_order=False) -> None:
        """Focus on the next visible window pane or tab."""
        (
            active_window_list,
            active_pane,
        ) = self._get_active_window_list_and_pane()
        if active_window_list is None:
            return

        # Total count of window lists and panes
        window_list_count = len(self.window_lists)
        pane_count = len(active_window_list.active_panes)

        # Get currently focused indices
        active_window_list_index = self.window_list_index(active_window_list)
        if active_window_list_index is None:
            return
        active_pane_index = active_window_list.pane_index(active_pane)

        increment = -1 if reverse_order else 1
        # Assume we can switch to the next pane in the current window_list
        next_pane_index = active_pane_index + increment

        # Case 1: next_pane_index does not exist in this window list.
        # Action: Switch to the first pane of the next window list.
        if next_pane_index >= pane_count or next_pane_index < 0:
            # Get the next window_list
            next_window_list_index = (
                active_window_list_index + increment
            ) % window_list_count
            next_window_list = self.window_lists[next_window_list_index]

            # If tabbed window mode is enabled, switch to the first tab.
            if next_window_list.display_mode == DisplayMode.TABBED:
                if reverse_order:
                    next_window_list.switch_to_tab(
                        len(next_window_list.active_panes) - 1
                    )
                else:
                    next_window_list.switch_to_tab(0)
                return

            # Otherwise switch to the first visible window pane.
            pane_list = next_window_list.active_panes
            if reverse_order:
                pane_list = reversed(pane_list)
            for pane in pane_list:
                if pane.show_pane:
                    self.application.focus_on_container(pane)
                    return

        # Case 2: next_pane_index does exist and display mode is tabs.
        # Action: Switch to the next tab of the current window list.
        if active_window_list.display_mode == DisplayMode.TABBED:
            active_window_list.switch_to_tab(next_pane_index)
            return

        # Case 3: next_pane_index does exist and display mode is stacked.
        # Action: Switch to the next visible window pane.
        index_range = range(1, pane_count)
        if reverse_order:
            index_range = range(pane_count - 1, 0, -1)
        for i in index_range:
            next_pane_index = (active_pane_index + i) % pane_count
            next_pane = active_window_list.active_panes[next_pane_index]
            if next_pane.show_pane:
                self.application.focus_on_container(next_pane)
                return
        return

    def move_pane_left(self):
        (
            active_window_list,
            active_pane,
        ) = self._get_active_window_list_and_pane()
        if not active_window_list:
            return

        window_list_index = self.window_list_index(active_window_list)
        # Move left should pick the previous window_list
        target_window_list_index = window_list_index - 1

        # Check if a new WindowList should be created on the left
        if target_window_list_index == -1:
            # Add the new WindowList
            target_window_list = WindowList(self)
            self.window_lists.appendleft(target_window_list)
            self.reset_split_sizes()
            # New index is 0
            target_window_list_index = 0

        # Get the destination window_list
        target_window_list = self.window_lists[target_window_list_index]

        # Move the pane
        active_window_list.remove_pane_no_checks(active_pane)
        target_window_list.add_pane(active_pane, add_at_beginning=True)
        target_window_list.reset_pane_sizes()
        self.delete_empty_window_lists()

    def move_pane_right(self):
        (
            active_window_list,
            active_pane,
        ) = self._get_active_window_list_and_pane()
        if not active_window_list:
            return

        window_list_index = self.window_list_index(active_window_list)
        # Move right should pick the next window_list
        target_window_list_index = window_list_index + 1

        # Check if a new WindowList should be created
        if target_window_list_index == len(self.window_lists):
            # Add a new WindowList
            target_window_list = WindowList(self)
            self.window_lists.append(target_window_list)
            self.reset_split_sizes()

        # Get the destination window_list
        target_window_list = self.window_lists[target_window_list_index]

        # Move the pane
        active_window_list.remove_pane_no_checks(active_pane)
        target_window_list.add_pane(active_pane, add_at_beginning=True)
        target_window_list.reset_pane_sizes()
        self.delete_empty_window_lists()

    def move_pane_up(self):
        (
            active_window_list,
            _active_pane,
        ) = self._get_active_window_list_and_pane()
        if not active_window_list:
            return

        active_window_list.move_pane_up()

    def move_pane_down(self):
        (
            active_window_list,
            _active_pane,
        ) = self._get_active_window_list_and_pane()
        if not active_window_list:
            return

        active_window_list.move_pane_down()

    def shrink_pane(self):
        (
            active_window_list,
            _active_pane,
        ) = self._get_active_window_list_and_pane()
        if not active_window_list:
            return

        active_window_list.shrink_pane()

    def enlarge_pane(self):
        (
            active_window_list,
            _active_pane,
        ) = self._get_active_window_list_and_pane()
        if not active_window_list:
            return

        active_window_list.enlarge_pane()

    def shrink_split(self):
        if len(self.window_lists) < 2:
            return

        (
            active_window_list,
            _active_pane,
        ) = self._get_active_window_list_and_pane()
        if not active_window_list:
            return

        self.adjust_split_size(active_window_list, -_WINDOW_SPLIT_ADJUST)

    def enlarge_split(self):
        (
            active_window_list,
            _active_pane,
        ) = self._get_active_window_list_and_pane()
        if not active_window_list:
            return

        self.adjust_split_size(active_window_list, _WINDOW_SPLIT_ADJUST)

    def balance_window_sizes(self):
        """Reset all splits and pane sizes."""
        self.reset_pane_sizes()
        self.reset_split_sizes()

    def reset_split_sizes(self):
        """Reset all active pane width and height to defaults"""
        available_height = self.current_window_manager_height
        available_width = self.current_window_manager_width
        old_heights = [w.height.preferred for w in self.window_lists]
        old_widths = [w.width.preferred for w in self.window_lists]
        new_heights = [int(available_height / len(old_heights))] * len(
            old_heights
        )
        new_widths = [int(available_width / len(old_widths))] * len(old_widths)

        self._set_window_list_sizes(new_heights, new_widths)

    def _get_next_window_list_for_resizing(
        self, window_list: WindowList
    ) -> Optional[WindowList]:
        window_list_index = self.window_list_index(window_list)
        if window_list_index is None:
            return None

        next_window_list_index = (window_list_index + 1) % len(
            self.window_lists
        )

        # Use the previous window if we are on the last split
        if window_list_index == len(self.window_lists) - 1:
            next_window_list_index = window_list_index - 1

        next_window_list = self.window_lists[next_window_list_index]
        return next_window_list

    def adjust_split_size(
        self, window_list: WindowList, diff: int = _WINDOW_SPLIT_ADJUST
    ) -> None:
        """Increase or decrease a given window_list's vertical split width."""
        # No need to resize if only one split.
        if len(self.window_lists) < 2:
            return

        # Get the next split to subtract from.
        next_window_list = self._get_next_window_list_for_resizing(window_list)
        if not next_window_list:
            return

        if self.vertical_window_list_spliting():
            # Get current width
            old_value = window_list.width.preferred
            next_old_value = next_window_list.width.preferred  # type: ignore
        else:
            # Get current height
            old_value = window_list.height.preferred
            next_old_value = next_window_list.height.preferred  # type: ignore

        # Add to the current split
        new_value = old_value + diff
        if new_value <= 0:
            new_value = old_value

        # Subtract from the next split
        next_new_value = next_old_value - diff
        if next_new_value <= 0:
            next_new_value = next_old_value

        # If new height is too small or no change, make no adjustments.
        if new_value < 3 or next_new_value < 3 or old_value == new_value:
            return

        if self.vertical_window_list_spliting():
            # Set new width
            window_list.width.preferred = new_value
            next_window_list.width.preferred = next_new_value  # type: ignore
        else:
            # Set new height
            window_list.height.preferred = new_value
            next_window_list.height.preferred = next_new_value  # type: ignore
            window_list.rebalance_window_heights()
            next_window_list.rebalance_window_heights()

    def toggle_pane(self, pane):
        """Toggle a pane on or off."""
        window_list, _pane_index = self._find_window_list_and_pane_index(pane)

        # Don't hide the window if tabbed mode is enabled. Switching to a
        # separate tab is preffered.
        if window_list.display_mode == DisplayMode.TABBED:
            return
        pane.show_pane = not pane.show_pane
        self.update_root_container_body()
        self.application.update_menu_items()

        # Set focus to the top level menu. This has the effect of keeping the
        # menu open if it's already open.
        self.application.focus_main_menu()

    def focus_first_visible_pane(self):
        """Focus on the first visible container."""
        for pane in self.active_panes():
            if pane.show_pane:
                self.application.application.layout.focus(pane)
                break

    def check_for_all_hidden_panes_and_unhide(self) -> None:
        """Scan for window_lists containing only hidden panes."""
        for window_list in self.window_lists:
            all_hidden = all(
                not pane.show_pane for pane in window_list.active_panes
            )
            if all_hidden:
                # Unhide the first pane
                self.toggle_pane(window_list.active_panes[0])

    def add_pane_no_checks(self, pane: Any):
        self.window_lists[0].add_pane_no_checks(pane)

    def add_pane(self, pane: Any):
        self.window_lists[0].add_pane(pane, add_at_beginning=True)

    def first_window_list(self):
        return self.window_lists[0]

    def active_panes(self):
        """Return all active panes from all window lists."""
        return chain.from_iterable(
            map(operator.attrgetter('active_panes'), self.window_lists)
        )

    def start_resize_pane(self, pane):
        window_list, pane_index = self._find_window_list_and_pane_index(pane)
        window_list.start_resize(pane, pane_index)

    def mouse_resize(self, xpos, ypos):
        if self.resize_target_window_list_index is None:
            return
        target_window_list = self.window_lists[
            self.resize_target_window_list_index
        ]

        diff = ypos - self.resize_current_row
        if self.vertical_window_list_spliting():
            diff = xpos - self.resize_current_column
        if diff == 0:
            return

        self.adjust_split_size(target_window_list, diff)
        self._resize_update_current_row_column()
        self.application.redraw_ui()

    def mouse_handler(self, mouse_event: MouseEvent):
        """MouseHandler used when resize_mode == True."""
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

    def _calculate_actual_widths(self) -> List[int]:
        widths = [w.width.preferred for w in self.window_lists]

        available_width = self.current_window_manager_width
        # Subtract 1 for each separator
        available_width -= len(self.window_lists) - 1
        remaining_rows = available_width - sum(widths)
        window_list_index = 0
        # Distribute remaining unaccounted columns to each window in turn.
        while remaining_rows > 0:
            widths[window_list_index] += 1
            remaining_rows -= 1
            window_list_index = (window_list_index + 1) % len(widths)

        return widths

    def _calculate_actual_heights(self) -> List[int]:
        heights = [w.height.preferred for w in self.window_lists]

        available_height = self.current_window_manager_height
        # Subtract 1 for each vertical separator
        available_height -= len(self.window_lists) - 1
        remaining_rows = available_height - sum(heights)
        window_list_index = 0
        # Distribute remaining unaccounted columns to each window in turn.
        while remaining_rows > 0:
            heights[window_list_index] += 1
            remaining_rows -= 1
            window_list_index = (window_list_index + 1) % len(heights)

        return heights

    def _resize_update_current_row_column(self) -> None:
        if self.resize_target_window_list_index is None:
            return

        widths = self._calculate_actual_widths()
        heights = self._calculate_actual_heights()

        start_column = 0
        start_row = 0

        # Find the starting column
        for i in range(self.resize_target_window_list_index + 1):
            # If we are past the target window_list, exit the loop.
            if i > self.resize_target_window_list_index:
                break
            start_column += widths[i]
            start_row += heights[i]
            if i < self.resize_target_window_list_index - 1:
                start_column += 1
                start_row += 1

        self.resize_current_column = start_column
        self.resize_current_row = start_row

    def start_resize(self, window_list):
        # Check the target window_list isn't the last one.
        if window_list == self.window_lists[-1]:
            return

        list_index = self.window_list_index(window_list)
        if list_index is None:
            return

        self.resize_mode = True
        self.resize_target_window_list = window_list
        self.resize_target_window_list_index = list_index
        self._resize_update_current_row_column()

    def stop_resize(self):
        self.resize_mode = False
        self.resize_target_window_list = None
        self.resize_target_window_list_index = None
        self.resize_current_row = 0
        self.resize_current_column = 0

    def _find_window_list_and_pane_index(self, pane: Any):
        pane_index = None
        parent_window_list = None
        for window_list in self.window_lists:
            pane_index = window_list.pane_index(pane)
            if pane_index is not None:
                parent_window_list = window_list
                break
        return parent_window_list, pane_index

    def remove_pane(self, existing_pane: Any):
        window_list, _pane_index = self._find_window_list_and_pane_index(
            existing_pane
        )
        if window_list:
            window_list.remove_pane(existing_pane)
            # Reset focus if this list is empty
            if len(window_list.active_panes) == 0:
                self.application.focus_main_menu()

    def reset_pane_sizes(self):
        for window_list in self.window_lists:
            window_list.reset_pane_sizes()

    def _remove_panes_from_layout(
        self, pane_titles: Iterable[str]
    ) -> Dict[str, Any]:
        # Gather pane objects and remove them from the window layout.
        collected_panes = {}

        for window_list in self.window_lists:
            # Make a copy of active_panes to prevent mutating the while
            # iterating.
            for pane in copy.copy(window_list.active_panes):
                if pane.pane_title() in pane_titles:
                    collected_panes[
                        pane.pane_title()
                    ] = window_list.remove_pane_no_checks(pane)
        return collected_panes

    def _set_pane_options(  # pylint: disable=no-self-use
        self, pane, options: dict
    ) -> None:
        if options.get('hidden', False):
            # Hide this pane
            pane.show_pane = False
        if options.get('height', False):
            # Apply new height
            new_height = options['height']
            assert isinstance(new_height, int)
            pane.height.preferred = new_height

    def _set_window_list_display_modes(self, prefs: ConsolePrefs) -> None:
        # Set column display modes
        for column_index, column_type in enumerate(prefs.window_column_modes):
            mode = DisplayMode.STACK
            if 'tabbed' in column_type:
                mode = DisplayMode.TABBED
            self.window_lists[column_index].set_display_mode(mode)

    def _create_new_log_pane_with_loggers(
        self, window_title, window_options, existing_pane_titles
    ) -> LogPane:
        if 'loggers' not in window_options:
            error_unknown_window(window_title, existing_pane_titles)

        new_pane = LogPane(
            application=self.application, pane_title=window_title
        )
        # Add logger handlers
        for logger_name, logger_options in window_options.get(
            'loggers', {}
        ).items():

            log_level_name = logger_options.get('level', None)
            new_pane.add_log_handler(logger_name, level_name=log_level_name)
        return new_pane

    # TODO(tonymd): Split this large function up.
    def apply_config(self, prefs: ConsolePrefs) -> None:
        """Apply window configuration from loaded ConsolePrefs."""
        if not prefs.windows:
            return

        unique_titles = prefs.unique_window_titles
        collected_panes = self._remove_panes_from_layout(unique_titles)
        existing_pane_titles = [
            p.pane_title()
            for p in collected_panes.values()
            if isinstance(p, LogPane)
        ]

        # Keep track of original non-duplicated pane titles
        already_added_panes = []

        for column_index, column in enumerate(
            prefs.windows.items()
        ):  # pylint: disable=too-many-nested-blocks
            _column_type, windows = column
            # Add a new window_list if needed
            if column_index >= len(self.window_lists):
                self.window_lists.append(WindowList(self))

            # Set column display mode to stacked by default.
            self.window_lists[column_index].display_mode = DisplayMode.STACK

            # Add windows to the this column (window_list)
            for window_title, window_dict in windows.items():
                window_options = window_dict if window_dict else {}
                new_pane = None
                desired_window_title = window_title
                # Check for duplicate_of: Title value
                window_title = window_options.get('duplicate_of', window_title)

                # Check if this pane is brand new, ready to be added, or should
                # be duplicated.
                if (
                    window_title not in already_added_panes
                    and window_title not in collected_panes
                ):
                    # New pane entirely
                    new_pane = self._create_new_log_pane_with_loggers(
                        window_title, window_options, existing_pane_titles
                    )

                elif window_title not in already_added_panes:
                    # First time adding this pane
                    already_added_panes.append(window_title)
                    new_pane = collected_panes[window_title]

                elif window_title in collected_panes:
                    # Pane added once, duplicate it
                    new_pane = collected_panes[window_title].create_duplicate()
                    # Rename this duplicate pane
                    assert isinstance(new_pane, LogPane)
                    new_pane.set_pane_title(desired_window_title)

                if new_pane:
                    # Set window size and visibility
                    self._set_pane_options(new_pane, window_options)
                    # Add the new pane
                    self.window_lists[column_index].add_pane_no_checks(new_pane)
                    # Apply log pane options
                    if isinstance(new_pane, LogPane):
                        new_pane.apply_filters_from_config(window_options)
                        # Auto-start the websocket log server if requested.
                        if window_options.get('view_in_web', False):
                            new_pane.toggle_websocket_server()

        # Update column display modes.
        self._set_window_list_display_modes(prefs)
        # Check for columns where all panes are hidden and unhide at least one.
        self.check_for_all_hidden_panes_and_unhide()

        # Update prompt_toolkit containers.
        self.update_root_container_body()
        self.application.update_menu_items()

        # Focus on the first visible pane.
        self.focus_first_visible_pane()

    def create_window_menu_items(self) -> List[MenuItem]:
        """Build the [Window] menu for the current set of window lists."""
        root_menu_items = []
        for window_list_index, window_list in enumerate(self.window_lists):
            menu_items = []
            menu_items.append(
                MenuItem(
                    'Column {index} View Modes'.format(
                        index=window_list_index + 1
                    ),
                    children=[
                        MenuItem(
                            '{check} {display_mode} Windows'.format(
                                display_mode=display_mode.value,
                                check=to_checkbox_text(
                                    window_list.display_mode == display_mode,
                                    end='',
                                ),
                            ),
                            handler=functools.partial(
                                window_list.set_display_mode, display_mode
                            ),
                        )
                        for display_mode in DisplayMode
                    ],
                )
            )
            menu_items.extend(
                MenuItem(
                    '{index}: {title}'.format(
                        index=pane_index + 1,
                        title=pane.menu_title(),
                    ),
                    children=[
                        MenuItem(
                            '{check} Show/Hide Window'.format(
                                check=to_checkbox_text(pane.show_pane, end='')
                            ),
                            handler=functools.partial(self.toggle_pane, pane),
                        ),
                    ]
                    + [
                        MenuItem(
                            text,
                            handler=functools.partial(
                                self.application.run_pane_menu_option, handler
                            ),
                        )
                        for text, handler in pane.get_window_menu_options()
                    ],
                )
                for pane_index, pane in enumerate(window_list.active_panes)
            )
            if window_list_index + 1 < len(self.window_lists):
                menu_items.append(MenuItem('-'))
            root_menu_items.extend(menu_items)

        return root_menu_items
