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
"""Bandwidth Monitor Toolbar"""

from prompt_toolkit.layout import WindowAlign

from pw_console.plugin_mixin import PluginMixin
from pw_console.widgets import ToolbarButton, WindowPaneToolbar
from pw_console.pyserial_wrapper import BANDWIDTH_HISTORY_CONTEXTVAR


class BandwidthToolbar(WindowPaneToolbar, PluginMixin):
    """Toolbar for displaying bandwidth history."""

    TOOLBAR_HEIGHT = 1

    def _update_toolbar_text(self):
        """Format toolbar text.

        This queries pyserial_wrapper's EventCountHistory context var to
        retrieve the byte count history for read, write and totals."""
        tokens = []
        self.plugin_logger.debug('BandwidthToolbar _update_toolbar_text')

        for count_name, events in self.history.items():
            tokens.extend(
                [
                    ('', '  '),
                    (
                        'class:theme-bg-active class:theme-fg-active',
                        ' {}: '.format(count_name.title()),
                    ),
                    (
                        'class:theme-bg-active class:theme-fg-cyan',
                        '{:.3f} '.format(events.last_count()),
                    ),
                    (
                        'class:theme-bg-active class:theme-fg-orange',
                        '{} '.format(events.display_unit_title),
                    ),
                ]
            )
            if count_name == 'total':
                tokens.append(
                    ('class:theme-fg-cyan', '{}'.format(events.sparkline()))
                )

        self.formatted_text = tokens

    def get_left_text_tokens(self):
        """Formatted text to display on the far left side."""
        return self.formatted_text

    def get_right_text_tokens(self):
        """Formatted text to display on the far right side."""
        return [('class:theme-fg-blue', 'Serial Bandwidth Usage ')]

    def __init__(self, *args, **kwargs):
        super().__init__(
            *args, center_section_align=WindowAlign.RIGHT, **kwargs
        )

        self.history = BANDWIDTH_HISTORY_CONTEXTVAR.get()
        self.show_toolbar = True
        self.formatted_text = []

        # Buttons for display in the center
        self.add_button(
            ToolbarButton(
                description='Refresh', mouse_handler=self._update_toolbar_text
            )
        )

        # Set plugin options
        self.background_task_update_count: int = 0
        self.plugin_init(
            plugin_callback=self._background_task,
            plugin_callback_frequency=1.0,
            plugin_logger_name='pw_console_bandwidth_toolbar',
        )

    def _background_task(self) -> bool:
        self.background_task_update_count += 1
        self._update_toolbar_text()
        self.plugin_logger.debug(
            'BandwidthToolbar Scheduled Update: #%s',
            self.background_task_update_count,
        )
        return True
