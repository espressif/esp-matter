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
"""LogPane class."""

import functools
import logging
import re
import time
from typing import (
    Any,
    Callable,
    List,
    Optional,
    TYPE_CHECKING,
    Tuple,
    Union,
)

from prompt_toolkit.application.current import get_app
from prompt_toolkit.filters import (
    Condition,
    has_focus,
)
from prompt_toolkit.formatted_text import StyleAndTextTuples
from prompt_toolkit.key_binding import (
    KeyBindings,
    KeyPressEvent,
    KeyBindingsBase,
)
from prompt_toolkit.layout import (
    ConditionalContainer,
    Float,
    FloatContainer,
    FormattedTextControl,
    HSplit,
    UIContent,
    UIControl,
    VerticalAlign,
    VSplit,
    Window,
    WindowAlign,
)
from prompt_toolkit.mouse_events import MouseEvent, MouseEventType, MouseButton

from pw_console.log_view import LogView
from pw_console.log_pane_toolbars import (
    LineInfoBar,
    TableToolbar,
)
from pw_console.log_pane_saveas_dialog import LogPaneSaveAsDialog
from pw_console.log_pane_selection_dialog import LogPaneSelectionDialog
from pw_console.log_store import LogStore
from pw_console.search_toolbar import SearchToolbar
from pw_console.filter_toolbar import FilterToolbar

from pw_console.style import (
    get_pane_style,
)
from pw_console.widgets import (
    ToolbarButton,
    WindowPane,
    WindowPaneHSplit,
    WindowPaneToolbar,
    create_border,
    mouse_handlers,
    to_checkbox_text,
    to_keybind_indicator,
)


if TYPE_CHECKING:
    from pw_console.console_app import ConsoleApp

_LOG_OUTPUT_SCROLL_AMOUNT = 5
_LOG = logging.getLogger(__package__)


class LogContentControl(UIControl):
    """LogPane prompt_toolkit UIControl for displaying LogContainer lines."""

    def __init__(self, log_pane: 'LogPane') -> None:
        # pylint: disable=too-many-locals
        self.log_pane = log_pane
        self.log_view = log_pane.log_view

        # Mouse drag visual selection flags.
        self.visual_select_mode_drag_start = False
        self.visual_select_mode_drag_stop = False

        self.uicontent: Optional[UIContent] = None
        self.lines: List[StyleAndTextTuples] = []

        # Key bindings.
        key_bindings = KeyBindings()
        register = log_pane.application.prefs.register_keybinding

        @register('log-pane.shift-line-to-top', key_bindings)
        def _shift_log_to_top(_event: KeyPressEvent) -> None:
            """Shift the selected log line to the top."""
            self.log_view.move_selected_line_to_top()

        @register('log-pane.shift-line-to-center', key_bindings)
        def _shift_log_to_center(_event: KeyPressEvent) -> None:
            """Shift the selected log line to the center."""
            self.log_view.center_log_line()

        @register('log-pane.toggle-wrap-lines', key_bindings)
        def _toggle_wrap_lines(_event: KeyPressEvent) -> None:
            """Toggle log line wrapping."""
            self.log_pane.toggle_wrap_lines()

        @register('log-pane.toggle-table-view', key_bindings)
        def _toggle_table_view(_event: KeyPressEvent) -> None:
            """Toggle table view."""
            self.log_pane.toggle_table_view()

        @register('log-pane.duplicate-log-pane', key_bindings)
        def _duplicate(_event: KeyPressEvent) -> None:
            """Duplicate this log pane."""
            self.log_pane.duplicate()

        @register('log-pane.remove-duplicated-log-pane', key_bindings)
        def _delete(_event: KeyPressEvent) -> None:
            """Remove log pane."""
            if self.log_pane.is_a_duplicate:
                self.log_pane.application.window_manager.remove_pane(
                    self.log_pane
                )

        @register('log-pane.clear-history', key_bindings)
        def _clear_history(_event: KeyPressEvent) -> None:
            """Clear log pane history."""
            self.log_pane.clear_history()

        @register('log-pane.scroll-to-top', key_bindings)
        def _scroll_to_top(_event: KeyPressEvent) -> None:
            """Scroll to top."""
            self.log_view.scroll_to_top()

        @register('log-pane.scroll-to-bottom', key_bindings)
        def _scroll_to_bottom(_event: KeyPressEvent) -> None:
            """Scroll to bottom."""
            self.log_view.scroll_to_bottom()

        @register('log-pane.toggle-follow', key_bindings)
        def _toggle_follow(_event: KeyPressEvent) -> None:
            """Toggle log line following."""
            self.log_pane.toggle_follow()

        @register('log-pane.toggle-web-browser', key_bindings)
        def _toggle_browser(_event: KeyPressEvent) -> None:
            """View logs in browser."""
            self.log_pane.toggle_websocket_server()

        @register('log-pane.move-cursor-up', key_bindings)
        def _up(_event: KeyPressEvent) -> None:
            """Move cursor up."""
            self.log_view.scroll_up()

        @register('log-pane.move-cursor-down', key_bindings)
        def _down(_event: KeyPressEvent) -> None:
            """Move cursor down."""
            self.log_view.scroll_down()

        @register('log-pane.visual-select-up', key_bindings)
        def _visual_select_up(_event: KeyPressEvent) -> None:
            """Select previous log line."""
            self.log_view.visual_select_up()

        @register('log-pane.visual-select-down', key_bindings)
        def _visual_select_down(_event: KeyPressEvent) -> None:
            """Select next log line."""
            self.log_view.visual_select_down()

        @register('log-pane.scroll-page-up', key_bindings)
        def _pageup(_event: KeyPressEvent) -> None:
            """Scroll the logs up by one page."""
            self.log_view.scroll_up_one_page()

        @register('log-pane.scroll-page-down', key_bindings)
        def _pagedown(_event: KeyPressEvent) -> None:
            """Scroll the logs down by one page."""
            self.log_view.scroll_down_one_page()

        @register('log-pane.save-copy', key_bindings)
        def _start_saveas(_event: KeyPressEvent) -> None:
            """Save logs to a file."""
            self.log_pane.start_saveas()

        @register('log-pane.search', key_bindings)
        def _start_search(_event: KeyPressEvent) -> None:
            """Start searching."""
            self.log_pane.start_search()

        @register('log-pane.search-next-match', key_bindings)
        def _next_search(_event: KeyPressEvent) -> None:
            """Next search match."""
            self.log_view.search_forwards()

        @register('log-pane.search-previous-match', key_bindings)
        def _previous_search(_event: KeyPressEvent) -> None:
            """Previous search match."""
            self.log_view.search_backwards()

        @register('log-pane.visual-select-all', key_bindings)
        def _select_all_logs(_event: KeyPressEvent) -> None:
            """Clear search."""
            self.log_pane.log_view.visual_select_all()

        @register('log-pane.deselect-cancel-search', key_bindings)
        def _clear_search_and_selection(_event: KeyPressEvent) -> None:
            """Clear selection or search."""
            if self.log_pane.log_view.visual_select_mode:
                self.log_pane.log_view.clear_visual_selection()
            elif self.log_pane.search_bar_active:
                self.log_pane.search_toolbar.cancel_search()

        @register('log-pane.search-apply-filter', key_bindings)
        def _apply_filter(_event: KeyPressEvent) -> None:
            """Apply current search as a filter."""
            self.log_pane.search_toolbar.close_search_bar()
            self.log_view.apply_filter()

        @register('log-pane.clear-filters', key_bindings)
        def _clear_filter(_event: KeyPressEvent) -> None:
            """Reset / erase active filters."""
            self.log_view.clear_filters()

        self.key_bindings: KeyBindingsBase = key_bindings

    def is_focusable(self) -> bool:
        return True

    def get_key_bindings(self) -> Optional[KeyBindingsBase]:
        return self.key_bindings

    def preferred_width(self, max_available_width: int) -> int:
        """Return the width of the longest line."""
        line_lengths = [len(l) for l in self.lines]
        return max(line_lengths)

    def preferred_height(
        self,
        width: int,
        max_available_height: int,
        wrap_lines: bool,
        get_line_prefix,
    ) -> Optional[int]:
        """Return the preferred height for the log lines."""
        content = self.create_content(width, None)
        return content.line_count

    def create_content(self, width: int, height: Optional[int]) -> UIContent:
        # Update lines to render
        self.lines = self.log_view.render_content()

        # Create a UIContent instance if none exists
        if self.uicontent is None:
            self.uicontent = UIContent(
                get_line=lambda i: self.lines[i],
                line_count=len(self.lines),
                show_cursor=False,
            )

        # Update line_count
        self.uicontent.line_count = len(self.lines)

        return self.uicontent

    def mouse_handler(self, mouse_event: MouseEvent):
        """Mouse handler for this control."""
        mouse_position = mouse_event.position

        # Left mouse button release should:
        # 1. check if a mouse drag just completed.
        # 2. If not in focus, switch focus to this log pane
        #    If in focus, move the cursor to that position.
        if (
            mouse_event.event_type == MouseEventType.MOUSE_UP
            and mouse_event.button == MouseButton.LEFT
        ):

            # If a drag was in progress and this is the first mouse release
            # press, set the stop flag.
            if (
                self.visual_select_mode_drag_start
                and not self.visual_select_mode_drag_stop
            ):
                self.visual_select_mode_drag_stop = True

            if not has_focus(self)():
                # Focus the save as dialog if open.
                if self.log_pane.saveas_dialog_active:
                    get_app().layout.focus(self.log_pane.saveas_dialog)
                # Focus the search bar if open.
                elif self.log_pane.search_bar_active:
                    get_app().layout.focus(self.log_pane.search_toolbar)
                # Otherwise, focus on the log pane content.
                else:
                    get_app().layout.focus(self)
                # Mouse event handled, return None.
                return None

            # Log pane in focus already, move the cursor to the position of the
            # mouse click.
            self.log_pane.log_view.scroll_to_position(mouse_position)
            # Mouse event handled, return None.
            return None

        # Mouse drag with left button should start selecting lines.
        # The log pane does not need to be in focus to start this.
        if (
            mouse_event.event_type == MouseEventType.MOUSE_MOVE
            and mouse_event.button == MouseButton.LEFT
        ):
            # If a previous mouse drag was completed, clear the selection.
            if (
                self.visual_select_mode_drag_start
                and self.visual_select_mode_drag_stop
            ):
                self.log_pane.log_view.clear_visual_selection()
            # Drag select in progress, set flags accordingly.
            self.visual_select_mode_drag_start = True
            self.visual_select_mode_drag_stop = False

            self.log_pane.log_view.visual_select_line(mouse_position)
            # Mouse event handled, return None.
            return None

        # Mouse wheel events should move the cursor +/- some amount of lines
        # even if this pane is not in focus.
        if mouse_event.event_type == MouseEventType.SCROLL_DOWN:
            self.log_pane.log_view.scroll_down(lines=_LOG_OUTPUT_SCROLL_AMOUNT)
            # Mouse event handled, return None.
            return None

        if mouse_event.event_type == MouseEventType.SCROLL_UP:
            self.log_pane.log_view.scroll_up(lines=_LOG_OUTPUT_SCROLL_AMOUNT)
            # Mouse event handled, return None.
            return None

        # Mouse event not handled, return NotImplemented.
        return NotImplemented


class LogPaneWebsocketDialog(ConditionalContainer):
    """Dialog box for showing the websocket URL."""

    # Height of the dialog box contens in lines of text.
    DIALOG_HEIGHT = 2

    def __init__(self, log_pane: 'LogPane'):
        self.log_pane = log_pane

        self._last_action_message: str = ''
        self._last_action_time: float = 0

        info_bar_control = FormattedTextControl(self.get_info_fragments)
        info_bar_window = Window(
            content=info_bar_control,
            height=1,
            align=WindowAlign.LEFT,
            dont_extend_width=False,
        )

        message_bar_control = FormattedTextControl(self.get_message_fragments)
        message_bar_window = Window(
            content=message_bar_control,
            height=1,
            align=WindowAlign.RIGHT,
            dont_extend_width=False,
        )

        action_bar_control = FormattedTextControl(self.get_action_fragments)
        action_bar_window = Window(
            content=action_bar_control,
            height=1,
            align=WindowAlign.RIGHT,
            dont_extend_width=True,
        )

        super().__init__(
            create_border(
                HSplit(
                    [
                        info_bar_window,
                        VSplit([message_bar_window, action_bar_window]),
                    ],
                    height=LogPaneWebsocketDialog.DIALOG_HEIGHT,
                    style='class:saveas-dialog',
                ),
                content_height=LogPaneWebsocketDialog.DIALOG_HEIGHT,
                title='Websocket Log Server',
                border_style='class:saveas-dialog-border',
                left_margin_columns=1,
            ),
            filter=Condition(lambda: self.log_pane.websocket_dialog_active),
        )

    def focus_self(self) -> None:
        # Nothing in this dialog can be focused, focus on the parent log_pane
        # instead.
        self.log_pane.application.focus_on_container(self.log_pane)

    def close_dialog(self) -> None:
        """Close this dialog."""
        self.log_pane.toggle_websocket_server()
        self.log_pane.websocket_dialog_active = False
        self.log_pane.application.focus_on_container(self.log_pane)
        self.log_pane.redraw_ui()

    def _set_action_message(self, text: str) -> None:
        self._last_action_time = time.time()
        self._last_action_message = text

    def copy_url_to_clipboard(self) -> None:
        self.log_pane.application.application.clipboard.set_text(
            self.log_pane.log_view.get_web_socket_url()
        )
        self._set_action_message('Copied!')

    def get_message_fragments(self):
        """Return FormattedText with the last action message."""
        # Mouse handlers
        focus = functools.partial(mouse_handlers.on_click, self.focus_self)
        # Separator should have the focus mouse handler so clicking on any
        # whitespace focuses the input field.
        separator_text = ('', '  ', focus)

        if self._last_action_time + 10 > time.time():
            return [
                ('class:theme-fg-yellow', self._last_action_message, focus),
                separator_text,
            ]
        return [separator_text]

    def get_info_fragments(self):
        """Return FormattedText with current URL info."""
        # Mouse handlers
        focus = functools.partial(mouse_handlers.on_click, self.focus_self)
        # Separator should have the focus mouse handler so clicking on any
        # whitespace focuses the input field.
        separator_text = ('', '  ', focus)

        fragments = [
            ('class:saveas-dialog-setting', 'URL:  ', focus),
            (
                'class:saveas-dialog-title',
                self.log_pane.log_view.get_web_socket_url(),
                focus,
            ),
            separator_text,
        ]
        return fragments

    def get_action_fragments(self):
        """Return FormattedText with the action buttons."""
        # Mouse handlers
        focus = functools.partial(mouse_handlers.on_click, self.focus_self)
        cancel = functools.partial(mouse_handlers.on_click, self.close_dialog)
        copy = functools.partial(
            mouse_handlers.on_click,
            self.copy_url_to_clipboard,
        )

        # Separator should have the focus mouse handler so clicking on any
        # whitespace focuses the input field.
        separator_text = ('', '  ', focus)

        # Default button style
        button_style = 'class:toolbar-button-inactive'

        fragments = []

        # Action buttons
        fragments.extend(
            to_keybind_indicator(
                key=None,
                description='Stop',
                mouse_handler=cancel,
                base_style=button_style,
            )
        )

        fragments.append(separator_text)
        fragments.extend(
            to_keybind_indicator(
                key=None,
                description='Copy to Clipboard',
                mouse_handler=copy,
                base_style=button_style,
            )
        )

        # One space separator
        fragments.append(('', ' ', focus))

        return fragments


class LogPane(WindowPane):
    """LogPane class."""

    # pylint: disable=too-many-instance-attributes,too-many-public-methods

    def __init__(
        self,
        application: Any,
        pane_title: str = 'Logs',
        log_store: Optional[LogStore] = None,
    ):
        super().__init__(application, pane_title)

        # TODO(tonymd): Read these settings from a project (or user) config.
        self.wrap_lines = False
        self._table_view = True
        self.is_a_duplicate = False

        # Create the log container which stores and handles incoming logs.
        self.log_view: LogView = LogView(
            self, self.application, log_store=log_store
        )

        # Log pane size variables. These are updated just befor rendering the
        # pane by the LogLineHSplit class.
        self.current_log_pane_width = 0
        self.current_log_pane_height = 0
        self.last_log_pane_width = None
        self.last_log_pane_height = None

        # Search tracking
        self.search_bar_active = False
        self.search_toolbar = SearchToolbar(self)
        self.filter_toolbar = FilterToolbar(self)

        self.saveas_dialog = LogPaneSaveAsDialog(self)
        self.saveas_dialog_active = False
        self.visual_selection_dialog = LogPaneSelectionDialog(self)

        self.websocket_dialog = LogPaneWebsocketDialog(self)
        self.websocket_dialog_active = False

        # Table header bar, only shown if table view is active.
        self.table_header_toolbar = TableToolbar(self)

        # Create the bottom toolbar for the whole log pane.
        self.bottom_toolbar = WindowPaneToolbar(self)
        self.bottom_toolbar.add_button(
            ToolbarButton('/', 'Search', self.start_search)
        )
        self.bottom_toolbar.add_button(
            ToolbarButton('Ctrl-o', 'Save', self.start_saveas)
        )
        self.bottom_toolbar.add_button(
            ToolbarButton(
                'f',
                'Follow',
                self.toggle_follow,
                is_checkbox=True,
                checked=lambda: self.log_view.follow,
            )
        )
        self.bottom_toolbar.add_button(
            ToolbarButton(
                't',
                'Table',
                self.toggle_table_view,
                is_checkbox=True,
                checked=lambda: self.table_view,
            )
        )
        self.bottom_toolbar.add_button(
            ToolbarButton(
                'w',
                'Wrap',
                self.toggle_wrap_lines,
                is_checkbox=True,
                checked=lambda: self.wrap_lines,
            )
        )
        self.bottom_toolbar.add_button(
            ToolbarButton('C', 'Clear', self.clear_history)
        )

        self.bottom_toolbar.add_button(
            ToolbarButton(
                'Shift-o',
                'Open in browser',
                self.toggle_websocket_server,
                is_checkbox=True,
                checked=lambda: self.log_view.websocket_running,
            )
        )

        self.log_content_control = LogContentControl(self)

        self.log_display_window = Window(
            content=self.log_content_control,
            # Scrolling is handled by LogScreen
            allow_scroll_beyond_bottom=False,
            # Line wrapping is handled by LogScreen
            wrap_lines=False,
            # Selected line highlighting is handled by LogScreen
            cursorline=False,
            # Don't make the window taller to fill the parent split container.
            # Window should match the height of the log line content. This will
            # also allow the parent HSplit to justify the content to the bottom
            dont_extend_height=True,
            # Window width should be extended to make backround highlighting
            # extend to the end of the container. Otherwise backround colors
            # will only appear until the end of the log line.
            dont_extend_width=False,
            # Needed for log lines ANSI sequences that don't specify foreground
            # or background colors.
            style=functools.partial(get_pane_style, self),
        )

        # Root level container
        self.container = ConditionalContainer(
            FloatContainer(
                # Horizonal split containing the log lines and the toolbar.
                WindowPaneHSplit(
                    self,  # LogPane reference
                    [
                        self.table_header_toolbar,
                        self.log_display_window,
                        self.filter_toolbar,
                        self.search_toolbar,
                        self.bottom_toolbar,
                    ],
                    # Align content with the bottom of the container.
                    align=VerticalAlign.BOTTOM,
                    height=lambda: self.height,
                    width=lambda: self.width,
                    style=functools.partial(get_pane_style, self),
                ),
                floats=[
                    Float(top=0, right=0, height=1, content=LineInfoBar(self)),
                    Float(
                        top=0,
                        right=0,
                        height=LogPaneSelectionDialog.DIALOG_HEIGHT,
                        content=self.visual_selection_dialog,
                    ),
                    Float(
                        top=3,
                        left=2,
                        right=2,
                        height=LogPaneSaveAsDialog.DIALOG_HEIGHT + 2,
                        content=self.saveas_dialog,
                    ),
                    Float(
                        top=1,
                        left=2,
                        right=2,
                        height=LogPaneWebsocketDialog.DIALOG_HEIGHT + 2,
                        content=self.websocket_dialog,
                    ),
                ],
            ),
            filter=Condition(lambda: self.show_pane),
        )

    @property
    def table_view(self):
        if self.log_view.websocket_running:
            return False
        return self._table_view

    @table_view.setter
    def table_view(self, table_view):
        self._table_view = table_view

    def menu_title(self):
        """Return the title to display in the Window menu."""
        title = self.pane_title()

        # List active filters
        if self.log_view.filtering_on:
            title += ' (FILTERS: '
            title += ' '.join(
                [
                    log_filter.pattern()
                    for log_filter in self.log_view.filters.values()
                ]
            )
            title += ')'
        return title

    def append_pane_subtitle(self, text):
        if not self._pane_subtitle:
            self._pane_subtitle = text
        else:
            self._pane_subtitle = self._pane_subtitle + ', ' + text

    def pane_subtitle(self) -> str:
        if not self._pane_subtitle:
            return ', '.join(self.log_view.log_store.channel_counts.keys())
        logger_names = self._pane_subtitle.split(', ')
        additional_text = ''
        if len(logger_names) > 1:
            additional_text = ' + {} more'.format(len(logger_names))

        return logger_names[0] + additional_text

    def start_search(self):
        """Show the search bar to begin a search."""
        if self.log_view.websocket_running:
            return
        # Show the search bar
        self.search_bar_active = True
        # Focus on the search bar
        self.application.focus_on_container(self.search_toolbar)

    def start_saveas(self, **export_kwargs) -> bool:
        """Show the saveas bar to begin saving logs to a file."""
        # Show the search bar
        self.saveas_dialog_active = True
        # Set export options if any
        self.saveas_dialog.set_export_options(**export_kwargs)
        # Focus on the search bar
        self.application.focus_on_container(self.saveas_dialog)
        return True

    def pane_resized(self) -> bool:
        """Return True if the current window size has changed."""
        return (
            self.last_log_pane_width != self.current_log_pane_width
            or self.last_log_pane_height != self.current_log_pane_height
        )

    def update_pane_size(self, width, height):
        """Save width and height of the log pane for the current UI render
        pass."""
        if width:
            self.last_log_pane_width = self.current_log_pane_width
            self.current_log_pane_width = width
        if height:
            # Subtract the height of the bottom toolbar
            height -= WindowPaneToolbar.TOOLBAR_HEIGHT
            if self._table_view:
                height -= TableToolbar.TOOLBAR_HEIGHT
            if self.search_bar_active:
                height -= SearchToolbar.TOOLBAR_HEIGHT
            if self.log_view.filtering_on:
                height -= FilterToolbar.TOOLBAR_HEIGHT
            self.last_log_pane_height = self.current_log_pane_height
            self.current_log_pane_height = height

    def toggle_table_view(self):
        """Enable or disable table view."""
        self._table_view = not self._table_view
        self.log_view.view_mode_changed()
        self.redraw_ui()

    def toggle_wrap_lines(self):
        """Enable or disable line wraping/truncation."""
        self.wrap_lines = not self.wrap_lines
        self.log_view.view_mode_changed()
        self.redraw_ui()

    def toggle_follow(self):
        """Enable or disable following log lines."""
        self.log_view.toggle_follow()
        self.redraw_ui()

    def clear_history(self):
        """Erase stored log lines."""
        self.log_view.clear_scrollback()
        self.redraw_ui()

    def toggle_websocket_server(self):
        """Start or stop websocket server to send logs."""
        if self.log_view.websocket_running:
            self.log_view.stop_websocket_thread()
            self.websocket_dialog_active = False
        else:
            self.search_toolbar.close_search_bar()
            self.log_view.start_websocket_thread()
            self.application.start_http_server()
            self.saveas_dialog_active = False
            self.websocket_dialog_active = True

    def get_all_key_bindings(self) -> List:
        """Return all keybinds for this pane."""
        # Return log content control keybindings
        return [self.log_content_control.get_key_bindings()]

    def get_window_menu_options(
        self,
    ) -> List[Tuple[str, Union[Callable, None]]]:
        """Return all menu options for the log pane."""

        options = [
            # Menu separator
            ('-', None),
            (
                'Save/Export a copy',
                self.start_saveas,
            ),
            ('-', None),
            (
                '{check} Line wrapping'.format(
                    check=to_checkbox_text(self.wrap_lines, end='')
                ),
                self.toggle_wrap_lines,
            ),
            (
                '{check} Table view'.format(
                    check=to_checkbox_text(self._table_view, end='')
                ),
                self.toggle_table_view,
            ),
            (
                '{check} Follow'.format(
                    check=to_checkbox_text(self.log_view.follow, end='')
                ),
                self.toggle_follow,
            ),
            (
                '{check} Open in web browser'.format(
                    check=to_checkbox_text(
                        self.log_view.websocket_running, end=''
                    )
                ),
                self.toggle_websocket_server,
            ),
            # Menu separator
            ('-', None),
            (
                'Clear history',
                self.clear_history,
            ),
            (
                'Duplicate pane',
                self.duplicate,
            ),
        ]
        if self.is_a_duplicate:
            options += [
                (
                    'Remove/Delete pane',
                    functools.partial(
                        self.application.window_manager.remove_pane, self
                    ),
                )
            ]

        # Search / Filter section
        options += [
            # Menu separator
            ('-', None),
            (
                'Hide search highlighting',
                self.log_view.disable_search_highlighting,
            ),
            (
                'Create filter from search results',
                self.log_view.apply_filter,
            ),
            (
                'Clear/Reset active filters',
                self.log_view.clear_filters,
            ),
        ]

        return options

    def apply_filters_from_config(self, window_options) -> None:
        if 'filters' not in window_options:
            return

        for field, criteria in window_options['filters'].items():
            for matcher_name, search_string in criteria.items():
                inverted = matcher_name.endswith('-inverted')
                matcher_name = re.sub(r'-inverted$', '', matcher_name)
                if field == 'all':
                    field = None
                if self.log_view.new_search(
                    search_string,
                    invert=inverted,
                    field=field,
                    search_matcher=matcher_name,
                    interactive=False,
                ):
                    self.log_view.install_new_filter()

        # Trigger any existing log messages to be added to the view.
        self.log_view.new_logs_arrived()

    def create_duplicate(self) -> 'LogPane':
        """Create a duplicate of this LogView."""
        new_pane = LogPane(self.application, pane_title=self.pane_title())
        # Set the log_store
        log_store = self.log_view.log_store
        new_pane.log_view.log_store = log_store
        # Register the duplicate pane as a viewer
        log_store.register_viewer(new_pane.log_view)

        # Set any existing search state.
        new_pane.log_view.search_text = self.log_view.search_text
        new_pane.log_view.search_filter = self.log_view.search_filter
        new_pane.log_view.search_matcher = self.log_view.search_matcher
        new_pane.log_view.search_highlight = self.log_view.search_highlight

        # Mark new pane as a duplicate so it can be deleted.
        new_pane.is_a_duplicate = True
        return new_pane

    def duplicate(self) -> None:
        new_pane = self.create_duplicate()
        # Add the new pane.
        self.application.window_manager.add_pane(new_pane)

    def add_log_handler(
        self,
        logger: Union[str, logging.Logger],
        level_name: Optional[str] = None,
    ) -> None:
        """Add a log handlers to this LogPane."""

        if isinstance(logger, logging.Logger):
            logger_instance = logger
        elif isinstance(logger, str):
            logger_instance = logging.getLogger(logger)

        if level_name:
            if not hasattr(logging, level_name):
                raise Exception(f'Unknown log level: {level_name}')
            logger_instance.level = getattr(logging, level_name, logging.INFO)
        logger_instance.addHandler(self.log_view.log_store)  # type: ignore
        self.append_pane_subtitle(logger_instance.name)  # type: ignore
