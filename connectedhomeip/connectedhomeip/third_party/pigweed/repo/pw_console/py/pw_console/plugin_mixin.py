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
"""Window pane base class."""

import asyncio
import logging
from threading import Thread
import time
from typing import Callable, Optional

from pw_console.get_pw_console_app import get_pw_console_app


class PluginMixin:
    """Handles background task management in a Pigweed Console plugin.

    Pigweed Console plugins can inherit from this class if they require running
    tasks in the background. This is important as any plugin code not in its
    own dedicated thread can potentially block the user interface

    Example usage: ::

        import logging
        from pw_console.plugin_mixin import PluginMixin
        from pw_console.widgets import WindowPaneToolbar

        class AwesomeToolbar(WindowPaneToolbar, PluginMixin):
            TOOLBAR_HEIGHT = 1

            def __init__(self, *args, **kwargs):
                # Call parent class WindowPaneToolbar.__init__
                super().__init__(*args, **kwargs)

                # Set PluginMixin to execute
                # self._awesome_background_task every 10 seconds.
                self.plugin_init(
                    plugin_callback=self._awesome_background_task,
                    plugin_callback_frequency=10.0,
                    plugin_logger_name='awesome_toolbar_plugin')

            # This function will be run in a separate thread every 10 seconds.
            def _awesome_background_task(self) -> bool:
                time.sleep(1)  # Do real work here.

                if self.new_data_processed:
                    # If new data was processed, and the user interface
                    # should be updated return True.

                    # Log using self.plugin_logger for debugging.
                    self.plugin_logger.debug('New data processed')

                    # Return True to signal a UI redraw.
                    return True

                # Returning False means no updates needed.
                return False

    Attributes:
        plugin_callback: Callable that is run in a background thread.
        plugin_callback_frequency: Number of seconds to wait between
            executing plugin_callback.
        plugin_logger: logging instance for this plugin. Useful for debugging
            code running in a separate thread.
        plugin_callback_future: `Future`_ object for the plugin background task.
        plugin_event_loop: asyncio event loop running in the background thread.
        plugin_enable_background_task: If True, keep periodically running
            plugin_callback at the desired frequency. If False the background
            task will stop.

    .. _Future: https://docs.python.org/3/library/asyncio-future.html
    """

    def plugin_init(
        self,
        plugin_callback: Optional[Callable[..., bool]] = None,
        plugin_callback_frequency: float = 30.0,
        plugin_logger_name: Optional[str] = 'pw_console_plugins',
    ) -> None:
        """Call this on __init__() to set plugin background task variables.

        Args:
            plugin_callback: Callable to run in a separate thread from the
                Pigweed Console UI. This function should return True if the UI
                should be redrawn after execution.
            plugin_callback_frequency: Number of seconds to wait between
                executing plugin_callback.
            plugin_logger_name: Unique name for this plugin's Python
                logger. Useful for debugging code running in a separate thread.
        """
        self.plugin_callback = plugin_callback
        self.plugin_callback_frequency = plugin_callback_frequency
        self.plugin_logger = logging.getLogger(plugin_logger_name)

        self.plugin_callback_future = None

        # Event loop for executing plugin code.
        self.plugin_event_loop = asyncio.new_event_loop()
        self.plugin_enable_background_task = True

    def plugin_start(self):
        """Function used to start this plugin's background thead and task."""

        # Create an entry point for the plugin thread.
        def _plugin_thread_entry():
            # Disable log propagation
            self.plugin_logger.propagate = False
            asyncio.set_event_loop(self.plugin_event_loop)
            self.plugin_event_loop.run_forever()

        # Create a thread for running user code so the UI isn't blocked.
        thread = Thread(target=_plugin_thread_entry, args=(), daemon=True)
        thread.start()

        self.plugin_logger.debug('Starting plugin: %s', self)
        if self.plugin_callback is None:
            return

        self.plugin_enable_background_task = True
        self.plugin_callback_future = asyncio.run_coroutine_threadsafe(
            # This function will be executed in a separate thread.
            self._plugin_periodically_run_callback(),
            # Using this asyncio event loop.
            self.plugin_event_loop,
        )  # type: ignore

    def plugin_stop(self):
        self.plugin_enable_background_task = False

    async def _plugin_periodically_run_callback(self) -> None:
        while self.plugin_enable_background_task:
            start_time = time.time()
            # Run the callback and redraw the UI if return value is True
            if self.plugin_callback and self.plugin_callback():
                get_pw_console_app().redraw_ui()
            run_time = time.time() - start_time
            await asyncio.sleep(self.plugin_callback_frequency - run_time)
