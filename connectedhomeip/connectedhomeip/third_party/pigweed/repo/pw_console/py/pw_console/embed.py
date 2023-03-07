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
"""pw_console embed class."""

import asyncio
import logging
from pathlib import Path
from typing import Any, Dict, List, Iterable, Optional, Tuple, Union

from prompt_toolkit.completion import WordCompleter

from pw_console.console_app import ConsoleApp
from pw_console.get_pw_console_app import PW_CONSOLE_APP_CONTEXTVAR
from pw_console.plugin_mixin import PluginMixin
from pw_console.python_logging import (
    setup_python_logging as pw_console_setup_python_logging,
)
from pw_console.widgets import (
    FloatingWindowPane,
    WindowPane,
    WindowPaneToolbar,
)


def _set_console_app_instance(plugin: Any, console_app: ConsoleApp) -> None:
    if hasattr(plugin, 'pw_console_init'):
        plugin.pw_console_init(console_app)
    else:
        plugin.application = console_app


class PwConsoleEmbed:
    """Embed class for customizing the console before startup."""

    # pylint: disable=too-many-instance-attributes
    def __init__(
        self,
        global_vars=None,
        local_vars=None,
        loggers: Optional[
            Union[Dict[str, Iterable[logging.Logger]], Iterable]
        ] = None,
        test_mode=False,
        repl_startup_message: Optional[str] = None,
        help_text: Optional[str] = None,
        app_title: Optional[str] = None,
        config_file_path: Optional[Union[str, Path]] = None,
    ) -> None:
        """Call this to embed pw console at the call point within your program.

        Example usage:

        .. code-block:: python

            import logging

            from pw_console import PwConsoleEmbed

            # Create the pw_console embed instance
            console = PwConsoleEmbed(
                global_vars=globals(),
                local_vars=locals(),
                loggers={
                    'Host Logs': [
                        logging.getLogger(__package__),
                        logging.getLogger(__name__),
                    ],
                    'Device Logs': [
                        logging.getLogger('usb_gadget'),
                    ],
                },
                app_title='My Awesome Console',
                config_file_path='/home/user/project/.pw_console.yaml',
            )
            # Optional: Add custom completions
            console.add_sentence_completer(
                {
                    'some_function', 'Function',
                    'some_variable', 'Variable',
                }
            )

            # Setup Python loggers to output to a file instead of STDOUT.
            console.setup_python_logging()

            # Then run the console with:
            console.embed()

        Args:
            global_vars: Dictionary representing the desired global symbol
                table. Similar to what is returned by `globals()`.
            local_vars: Dictionary representing the desired local symbol
                table. Similar to what is returned by `locals()`.
            loggers: Dict with keys of log window titles and values of either:

                    1. List of `logging.getLogger()
                       <https://docs.python.org/3/library/logging.html#logging.getLogger>`_
                       instances.
                    2. A single pw_console.log_store.LogStore instance.

            app_title: Custom title text displayed in the user interface.
            repl_startup_message: Custom text shown by default in the repl
                output pane.
            help_text: Custom text shown at the top of the help window before
                keyboard shortcuts.
            config_file_path: Path to a pw_console yaml config file.
        """

        self.global_vars = global_vars
        self.local_vars = local_vars
        self.loggers = loggers
        self.test_mode = test_mode
        self.repl_startup_message = repl_startup_message
        self.help_text = help_text
        self.app_title = app_title
        self.config_file_path = (
            Path(config_file_path) if config_file_path else None
        )

        self.console_app: Optional[ConsoleApp] = None
        self.extra_completers: List = []

        self.setup_python_logging_called = False
        self.hidden_by_default_windows: List[str] = []
        self.window_plugins: List[WindowPane] = []
        self.floating_window_plugins: List[Tuple[FloatingWindowPane, Dict]] = []
        self.top_toolbar_plugins: List[WindowPaneToolbar] = []
        self.bottom_toolbar_plugins: List[WindowPaneToolbar] = []

    def add_window_plugin(self, window_pane: WindowPane) -> None:
        """Include a custom window pane plugin.

        Args:
            window_pane: Any instance of the WindowPane class.
        """
        self.window_plugins.append(window_pane)

    def add_floating_window_plugin(
        self, window_pane: FloatingWindowPane, **float_args
    ) -> None:
        """Include a custom floating window pane plugin.

        This adds a FloatingWindowPane class to the pw_console UI. The first
        argument should be the window to add and the remaining keyword arguments
        are passed to the prompt_toolkit Float() class. This allows positioning
        of the floating window. By default the floating window will be
        centered. To anchor the window to a side or corner of the screen set the
        ``left``, ``right``, ``top``, or ``bottom`` keyword args.

        For example:

        .. code-block:: python

           from pw_console import PwConsoleEmbed

           console = PwConsoleEmbed(...)
           my_plugin = MyPlugin()
           # Anchor this floating window 2 rows away from the top and 4 columns
           # away from the left edge of the screen.
           console.add_floating_window_plugin(my_plugin, top=2, left=4)

        See all possible keyword args in the prompt_toolkit documentation:
        https://python-prompt-toolkit.readthedocs.io/en/stable/pages/reference.html#prompt_toolkit.layout.Float

        Args:
            window_pane: Any instance of the FloatingWindowPane class.
            left: Distance to the left edge of the screen
            right: Distance to the right edge of the screen
            top: Distance to the top edge of the screen
            bottom: Distance to the bottom edge of the screen
        """
        self.floating_window_plugins.append((window_pane, float_args))

    def add_top_toolbar(self, toolbar: WindowPaneToolbar) -> None:
        """Include a toolbar plugin to display on the top of the screen.

        Top toolbars appear above all window panes and just below the main menu
        bar. They span the full width of the screen.

        Args:
            toolbar: Instance of the WindowPaneToolbar class.
        """
        self.top_toolbar_plugins.append(toolbar)

    def add_bottom_toolbar(self, toolbar: WindowPaneToolbar) -> None:
        """Include a toolbar plugin to display at the bottom of the screen.

        Bottom toolbars appear below all window panes and span the full width of
        the screen.

        Args:
            toolbar: Instance of the WindowPaneToolbar class.
        """
        self.bottom_toolbar_plugins.append(toolbar)

    def add_sentence_completer(
        self, word_meta_dict: Dict[str, str], ignore_case=True
    ) -> None:
        """Include a custom completer that matches on the entire repl input.

        Args:
            word_meta_dict: Dictionary representing the sentence completions
                and descriptions. Keys are completion text, values are
                descriptions.
        """

        # Don't modify completion if empty.
        if len(word_meta_dict) == 0:
            return

        sentences: List[str] = list(word_meta_dict.keys())
        word_completer = WordCompleter(
            sentences,
            meta_dict=word_meta_dict,
            ignore_case=ignore_case,
            # Whole input field should match
            sentence=True,
        )

        self.extra_completers.append(word_completer)

    def _setup_log_panes(self) -> None:
        """Add loggers to ConsoleApp log pane(s)."""
        if not self.loggers:
            return

        assert isinstance(self.console_app, ConsoleApp)

        if isinstance(self.loggers, list):
            self.console_app.add_log_handler('Logs', self.loggers)

        elif isinstance(self.loggers, dict):
            for window_title, logger_instances in self.loggers.items():
                window_pane = self.console_app.add_log_handler(
                    window_title, logger_instances
                )

                if (
                    window_pane
                    and window_pane.pane_title()
                    in self.hidden_by_default_windows
                ):
                    window_pane.show_pane = False

    def setup_python_logging(
        self,
        last_resort_filename: Optional[str] = None,
        loggers_with_no_propagation: Optional[Iterable[logging.Logger]] = None,
    ) -> None:
        """Setup friendly logging for full-screen prompt_toolkit applications.

        This function sets up Python log handlers to be friendly for full-screen
        prompt_toolkit applications. That is, logging to terminal STDOUT and
        STDERR is disabled so the terminal user interface can be drawn.

        Specifically, all Python STDOUT and STDERR log handlers are
        disabled. It also sets `log propagation to True
        <https://docs.python.org/3/library/logging.html#logging.Logger.propagate>`_.
        to ensure that all log messages are sent to the root logger.

        Args:
            last_resort_filename: If specified use this file as a fallback for
                unhandled Python logging messages. Normally Python will output
                any log messages with no handlers to STDERR as a fallback. If
                None, a temp file will be created instead. See Python
                documentation on `logging.lastResort
                <https://docs.python.org/3/library/logging.html#logging.lastResort>`_
                for more info.
            loggers_with_no_propagation: List of logger instances to skip
               setting ``propagate = True``. This is useful if you would like
               log messages from a particular source to not appear in the root
               logger.
        """
        self.setup_python_logging_called = True
        pw_console_setup_python_logging(
            last_resort_filename, loggers_with_no_propagation
        )

    def hide_windows(self, *window_titles) -> None:
        """Hide window panes specified by title on console startup."""
        for window_title in window_titles:
            self.hidden_by_default_windows.append(window_title)

    def embed(self, override_window_config: Optional[Dict] = None) -> None:
        """Start the console."""

        # Create the ConsoleApp instance.
        self.console_app = ConsoleApp(
            global_vars=self.global_vars,
            local_vars=self.local_vars,
            repl_startup_message=self.repl_startup_message,
            help_text=self.help_text,
            app_title=self.app_title,
            extra_completers=self.extra_completers,
            floating_window_plugins=self.floating_window_plugins,
        )
        PW_CONSOLE_APP_CONTEXTVAR.set(self.console_app)  # type: ignore
        # Setup Python logging and log panes.
        if not self.setup_python_logging_called:
            self.setup_python_logging()
        self._setup_log_panes()

        # Add window pane plugins to the layout.
        for window_pane in self.window_plugins:
            _set_console_app_instance(window_pane, self.console_app)
            # Hide window plugins if the title is hidden by default.
            if window_pane.pane_title() in self.hidden_by_default_windows:
                window_pane.show_pane = False
            self.console_app.window_manager.add_pane(window_pane)

        # Add toolbar plugins to the layout.
        for toolbar in self.top_toolbar_plugins:
            _set_console_app_instance(toolbar, self.console_app)
            self.console_app.window_manager.add_top_toolbar(toolbar)
        for toolbar in self.bottom_toolbar_plugins:
            _set_console_app_instance(toolbar, self.console_app)
            self.console_app.window_manager.add_bottom_toolbar(toolbar)

        # Init floating window plugins.
        for floating_window, _ in self.floating_window_plugins:
            _set_console_app_instance(floating_window, self.console_app)

        # Rebuild prompt_toolkit containers, menu items, and help content with
        # any new plugins added above.
        self.console_app.refresh_layout()

        # Load external config if passed in.
        if self.config_file_path:
            self.console_app.load_clean_config(self.config_file_path)

        if override_window_config:
            self.console_app.prefs.set_windows(override_window_config)
        self.console_app.apply_window_config()

        # Hide the repl pane if it's in the hidden windows list.
        if 'Python Repl' in self.hidden_by_default_windows:
            self.console_app.repl_pane.show_pane = False

        # Start a thread for running user code.
        self.console_app.start_user_code_thread()

        # Startup any background threads and tasks required by plugins.
        for window_pane in self.window_plugins:
            if isinstance(window_pane, PluginMixin):
                window_pane.plugin_start()
        for toolbar in self.bottom_toolbar_plugins:
            if isinstance(toolbar, PluginMixin):
                toolbar.plugin_start()
        for toolbar in self.top_toolbar_plugins:
            if isinstance(toolbar, PluginMixin):
                toolbar.plugin_start()

        # Start the prompt_toolkit UI app.
        asyncio.run(
            self.console_app.run(test_mode=self.test_mode), debug=self.test_mode
        )
