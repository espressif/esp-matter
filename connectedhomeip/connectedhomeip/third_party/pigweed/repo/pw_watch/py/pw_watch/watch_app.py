#!/usr/bin/env python
# Copyright 2022 The Pigweed Authors
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
""" Prompt toolkit application for pw watch. """

import asyncio
import logging
from pathlib import Path
import re
import sys
import time
from typing import Callable, Dict, List, NoReturn, Optional

from prompt_toolkit.application import Application
from prompt_toolkit.clipboard.pyperclip import PyperclipClipboard
from prompt_toolkit.filters import Condition
from prompt_toolkit.history import (
    InMemoryHistory,
    History,
    ThreadedHistory,
)
from prompt_toolkit.key_binding import (
    KeyBindings,
    KeyBindingsBase,
    merge_key_bindings,
)
from prompt_toolkit.layout import (
    Dimension,
    DynamicContainer,
    Float,
    FloatContainer,
    FormattedTextControl,
    HSplit,
    Layout,
    Window,
)
from prompt_toolkit.layout.controls import BufferControl
from prompt_toolkit.styles import DynamicStyle, merge_styles, Style
from prompt_toolkit.formatted_text import StyleAndTextTuples

from pw_console.console_app import get_default_colordepth
from pw_console.get_pw_console_app import PW_CONSOLE_APP_CONTEXTVAR
from pw_console.key_bindings import DEFAULT_KEY_BINDINGS
from pw_console.log_pane import LogPane
from pw_console.plugin_mixin import PluginMixin
from pw_console.plugins.twenty48_pane import Twenty48Pane
from pw_console.quit_dialog import QuitDialog
from pw_console.window_manager import WindowManager
import pw_console.style
from pw_console.style import get_theme_colors
import pw_console.widgets.border

from pw_build.project_builder_prefs import ProjectBuilderPrefs


class WatchAppPrefs(ProjectBuilderPrefs):
    """Add pw_console specific prefs standard ProjectBuilderPrefs."""

    def __init__(self, *args, **kwargs) -> None:
        super().__init__(*args, **kwargs)

        self.registered_commands = DEFAULT_KEY_BINDINGS
        self.registered_commands.update(self.user_key_bindings)

        self.default_config.update(
            {
                'key_bindings': DEFAULT_KEY_BINDINGS,
            }
        )
        self.reset_config()

    # Required pw_console preferences for key bindings and themes
    @property
    def user_key_bindings(self) -> Dict[str, List[str]]:
        return self._config.get('key_bindings', {})

    @property
    def ui_theme(self) -> str:
        return self._config.get('ui_theme', '')

    @ui_theme.setter
    def ui_theme(self, new_ui_theme: str) -> None:
        self._config['ui_theme'] = new_ui_theme

    @property
    def theme_colors(self):
        return get_theme_colors(self.ui_theme)

    def get_function_keys(self, name: str) -> List:
        """Return the keys for the named function."""
        try:
            return self.registered_commands[name]
        except KeyError as error:
            raise KeyError('Unbound key function: {}'.format(name)) from error

    def register_named_key_function(
        self, name: str, default_bindings: List[str]
    ) -> None:
        self.registered_commands[name] = default_bindings

    def register_keybinding(
        self, name: str, key_bindings: KeyBindings, **kwargs
    ) -> Callable:
        """Apply registered keys for the given named function."""

        def decorator(handler: Callable) -> Callable:
            "`handler` is a callable or Binding."
            for keys in self.get_function_keys(name):
                key_bindings.add(*keys.split(' '), **kwargs)(handler)
            return handler

        return decorator

    # Required pw_console preferences for using a log window pane.
    @property
    def spaces_between_columns(self) -> int:
        return 2

    @property
    def window_column_split_method(self) -> str:
        return 'horizontal'

    @property
    def hide_date_from_log_time(self) -> bool:
        return True

    @property
    def column_order(self) -> list:
        return []

    def column_style(  # pylint: disable=no-self-use
        self,
        _column_name: str,
        _column_value: str,
        default='',
    ) -> str:
        return default

    @property
    def show_python_file(self) -> bool:
        return self._config.get('show_python_file', False)

    @property
    def show_source_file(self) -> bool:
        return self._config.get('show_source_file', False)

    @property
    def show_python_logger(self) -> bool:
        return self._config.get('show_python_logger', False)


_NINJA_LOG = logging.getLogger('pw_watch_ninja_output')
_LOG = logging.getLogger('pw_watch')


class WatchWindowManager(WindowManager):
    def update_root_container_body(self):
        self.application.window_manager_container = self.create_root_container()


class WatchApp(PluginMixin):
    """Pigweed Watch main window application."""

    # pylint: disable=too-many-instance-attributes

    NINJA_FAILURE_TEXT = '\033[31mFAILED: '

    NINJA_BUILD_STEP = re.compile(
        r"^\[(?P<step>[0-9]+)/(?P<total_steps>[0-9]+)\] (?P<action>.*)$"
    )

    def __init__(
        self,
        event_handler,
        prefs: WatchAppPrefs,
        debug_logging: bool = False,
        log_file_name: Optional[str] = None,
    ):

        self.event_handler = event_handler

        self.external_logfile: Optional[Path] = (
            Path(log_file_name) if log_file_name else None
        )
        self.color_depth = get_default_colordepth()

        # Necessary for some of pw_console's window manager features to work
        # such as mouse drag resizing.
        PW_CONSOLE_APP_CONTEXTVAR.set(self)  # type: ignore

        self.prefs = prefs

        self.quit_dialog = QuitDialog(self, self.exit)  # type: ignore

        self.search_history: History = ThreadedHistory(InMemoryHistory())

        self.window_manager = WatchWindowManager(self)

        pw_console.python_logging.setup_python_logging()

        self._build_error_count = 0
        self._errors_in_output = False

        self.log_ui_update_frequency = 0.1  # 10 FPS
        self._last_ui_update_time = time.time()

        self.ninja_log_pane = LogPane(
            application=self, pane_title='Pigweed Watch'
        )
        self.ninja_log_pane.add_log_handler(_NINJA_LOG, level_name='INFO')
        self.ninja_log_pane.add_log_handler(
            _LOG, level_name=('DEBUG' if debug_logging else 'INFO')
        )
        # Set python log format to just the message itself.
        self.ninja_log_pane.log_view.log_store.formatter = logging.Formatter(
            '%(message)s'
        )
        self.ninja_log_pane.table_view = False
        # Disable line wrapping for improved error visibility.
        if self.ninja_log_pane.wrap_lines:
            self.ninja_log_pane.toggle_wrap_lines()
        # Blank right side toolbar text
        self.ninja_log_pane._pane_subtitle = ' '
        self.ninja_log_view = self.ninja_log_pane.log_view

        # Make tab and shift-tab search for next and previous error
        next_error_bindings = KeyBindings()

        @next_error_bindings.add('s-tab')
        def _previous_error(_event):
            self.jump_to_error(backwards=True)

        @next_error_bindings.add('tab')
        def _next_error(_event):
            self.jump_to_error()

        existing_log_bindings: Optional[
            KeyBindingsBase
        ] = self.ninja_log_pane.log_content_control.key_bindings

        key_binding_list: List[KeyBindingsBase] = []
        if existing_log_bindings:
            key_binding_list.append(existing_log_bindings)
        key_binding_list.append(next_error_bindings)
        self.ninja_log_pane.log_content_control.key_bindings = (
            merge_key_bindings(key_binding_list)
        )

        self.window_manager.add_pane(self.ninja_log_pane)

        self.time_waster = Twenty48Pane(include_resize_handle=True)
        self.time_waster.application = self
        self.time_waster.show_pane = False
        self.window_manager.add_pane(self.time_waster)

        self.window_manager_container = (
            self.window_manager.create_root_container()
        )

        self.status_bar_border_style = 'class:command-runner-border'

        self.root_container = FloatContainer(
            HSplit(
                [
                    pw_console.widgets.border.create_border(
                        HSplit(
                            [
                                # The top toolbar.
                                Window(
                                    content=FormattedTextControl(
                                        self.get_statusbar_text
                                    ),
                                    height=Dimension.exact(1),
                                    style='class:toolbar_inactive',
                                ),
                                # Result Toolbar.
                                Window(
                                    content=FormattedTextControl(
                                        self.get_resultbar_text
                                    ),
                                    height=lambda: len(
                                        self.event_handler.project_builder
                                    ),
                                    style='class:toolbar_inactive',
                                ),
                            ]
                        ),
                        border_style=lambda: self.status_bar_border_style,
                        base_style='class:toolbar_inactive',
                        left_margin_columns=1,
                        right_margin_columns=1,
                    ),
                    # The main content.
                    DynamicContainer(lambda: self.window_manager_container),
                ]
            ),
            floats=[
                Float(
                    content=self.quit_dialog,
                    top=2,
                    left=2,
                ),
            ],
        )

        key_bindings = KeyBindings()

        @key_bindings.add('enter', filter=self.input_box_not_focused())
        def _run_build(_event):
            "Rebuild."
            self.run_build()

        @key_bindings.add('c-t', filter=self.input_box_not_focused())
        def _pass_time(_event):
            "Rebuild."
            self.time_waster.show_pane = not self.time_waster.show_pane
            self.refresh_layout()
            self.window_manager.focus_first_visible_pane()

        register = self.prefs.register_keybinding

        @register('global.exit-no-confirmation', key_bindings)
        def _quit_no_confirm(_event):
            """Quit without confirmation."""
            _LOG.info('Got quit signal; exiting...')
            self.exit(0)

        @register('global.exit-with-confirmation', key_bindings)
        def _quit_with_confirm(_event):
            """Quit with confirmation dialog."""
            self.quit_dialog.open_dialog()

        self.key_bindings = merge_key_bindings(
            [
                self.window_manager.key_bindings,
                key_bindings,
            ]
        )

        self.current_theme = pw_console.style.generate_styles(
            self.prefs.ui_theme
        )
        self.style_overrides = Style.from_dict(
            {
                # 'search': 'bg:ansired ansiblack',
            }
        )

        self.layout = Layout(
            self.root_container, focused_element=self.ninja_log_pane
        )

        self.application: Application = Application(
            layout=self.layout,
            key_bindings=self.key_bindings,
            mouse_support=True,
            color_depth=self.color_depth,
            clipboard=PyperclipClipboard(),
            style=DynamicStyle(
                lambda: merge_styles(
                    [
                        self.current_theme,
                        self.style_overrides,
                    ]
                )
            ),
            full_screen=True,
        )

        self.plugin_init(
            plugin_callback=self.check_build_status,
            plugin_callback_frequency=0.5,
            plugin_logger_name='pw_watch_stdout_checker',
        )

    def logs_redraw(self):
        emit_time = time.time()
        # Has enough time passed since last UI redraw due to new logs?
        if emit_time > self._last_ui_update_time + self.log_ui_update_frequency:
            # Update last log time
            self._last_ui_update_time = emit_time

            # Trigger Prompt Toolkit UI redraw.
            self.redraw_ui()

    def jump_to_error(self, backwards: bool = False) -> None:
        if not self.ninja_log_pane.log_view.search_text:
            self.ninja_log_pane.log_view.set_search_regex(
                '^FAILED: ', False, None
            )
        if backwards:
            self.ninja_log_pane.log_view.search_backwards()
        else:
            self.ninja_log_pane.log_view.search_forwards()
        self.ninja_log_pane.log_view.log_screen.reset_logs(
            log_index=self.ninja_log_pane.log_view.log_index
        )

        self.ninja_log_pane.log_view.move_selected_line_to_top()

    def refresh_layout(self) -> None:
        self.window_manager.update_root_container_body()

    def update_menu_items(self):
        """Required by the Window Manager Class."""

    def redraw_ui(self):
        """Redraw the prompt_toolkit UI."""
        if hasattr(self, 'application'):
            self.application.invalidate()

    def focus_on_container(self, pane):
        """Set application focus to a specific container."""
        # Try to focus on the given pane
        try:
            self.application.layout.focus(pane)
        except ValueError:
            # If the container can't be focused, focus on the first visible
            # window pane.
            self.window_manager.focus_first_visible_pane()

    def focused_window(self):
        """Return the currently focused window."""
        return self.application.layout.current_window

    def focus_main_menu(self):
        """Focus on the main menu.

        Currently pw_watch has no main menu so focus on the first visible pane
        instead."""
        self.window_manager.focus_first_visible_pane()

    def command_runner_is_open(self) -> bool:
        # pylint: disable=no-self-use
        return False

    def clear_ninja_log(self) -> None:
        self.ninja_log_view.log_store.clear_logs()
        self.ninja_log_view._restart_filtering()  # pylint: disable=protected-access
        self.ninja_log_view.view_mode_changed()
        # Re-enable follow if needed
        if not self.ninja_log_view.follow:
            self.ninja_log_view.toggle_follow()

    def run_build(self):
        """Manually trigger a rebuild."""
        self.clear_ninja_log()
        self.event_handler.rebuild()

    def rebuild_on_filechange(self):
        self.ninja_log_view.log_store.clear_logs()
        self.ninja_log_view.view_mode_changed()

    def get_statusbar_text(self):
        status = self.event_handler.status_message
        fragments = [('class:logo', 'Pigweed Watch')]
        is_building = False
        if status:
            fragments = [status]
            is_building = status[1].endswith('Building')
        separator = ('', '  ')
        self.status_bar_border_style = 'class:theme-fg-green'

        if is_building:
            percent = self.event_handler.current_build_percent
            percent *= 100
            fragments.append(separator)
            fragments.append(('ansicyan', '{:.0f}%'.format(percent)))
            self.status_bar_border_style = 'class:theme-fg-yellow'

        if self.event_handler.current_build_errors > 0:
            fragments.append(separator)
            fragments.append(('', 'Errors:'))
            fragments.append(
                ('ansired', str(self.event_handler.current_build_errors))
            )
            self.status_bar_border_style = 'class:theme-fg-red'

        if is_building:
            fragments.append(separator)
            fragments.append(('', self.event_handler.current_build_step))

        return fragments

    def get_resultbar_text(self) -> StyleAndTextTuples:
        result = self.event_handler.result_message
        if not result:
            result = [('', 'Loading...')]
        return result

    def exit(self, exit_code: int = 0) -> None:
        log_file = self.external_logfile

        def _really_exit(future: asyncio.Future) -> NoReturn:
            if log_file:
                # Print a message showing where logs were saved to.
                print('Logs saved to: {}'.format(log_file.resolve()))
            sys.exit(future.result())

        if self.application.future:
            self.application.future.add_done_callback(_really_exit)
        self.application.exit(result=exit_code)

    def check_build_status(self) -> bool:
        if not self.event_handler.current_stdout:
            return False

        if self._errors_in_output:
            return True

        if self.event_handler.current_build_errors > self._build_error_count:
            self._errors_in_output = True
            self.jump_to_error()

        return True

    def run(self):
        self.plugin_start()
        # Run the prompt_toolkit application
        self.application.run(set_exception_handler=True)

    def input_box_not_focused(self) -> Condition:
        """Condition checking the focused control is not a text input field."""

        @Condition
        def _test() -> bool:
            """Check if the currently focused control is an input buffer.

            Returns:
                bool: True if the currently focused control is not a text input
                    box. For example if the user presses enter when typing in
                    the search box, return False.
            """
            return not isinstance(
                self.application.layout.current_control, BufferControl
            )

        return _test
