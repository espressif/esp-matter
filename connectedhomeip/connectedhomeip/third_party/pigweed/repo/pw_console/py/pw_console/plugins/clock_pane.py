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
"""Example Plugin that displays some dynamic content (a clock) and examples of
text formatting."""

from datetime import datetime

from prompt_toolkit.filters import Condition, has_focus
from prompt_toolkit.formatted_text import (
    FormattedText,
    HTML,
    merge_formatted_text,
)
from prompt_toolkit.key_binding import KeyBindings, KeyPressEvent
from prompt_toolkit.layout import FormattedTextControl, Window, WindowAlign
from prompt_toolkit.mouse_events import MouseEvent, MouseEventType

from pw_console.plugin_mixin import PluginMixin
from pw_console.widgets import ToolbarButton, WindowPane, WindowPaneToolbar
from pw_console.get_pw_console_app import get_pw_console_app

# Helper class used by the ClockPane plugin for displaying dynamic text,
# handling key bindings and mouse input. See the ClockPane class below for the
# beginning of the plugin implementation.


class ClockControl(FormattedTextControl):
    """Example prompt_toolkit UIControl for displaying formatted text.

    This is the prompt_toolkit class that is responsible for drawing the clock,
    handling keybindings if in focus, and mouse input.
    """

    def __init__(self, clock_pane: 'ClockPane', *args, **kwargs) -> None:
        self.clock_pane = clock_pane

        # Set some custom key bindings to toggle the view mode and wrap lines.
        key_bindings = KeyBindings()

        # If you press the v key this _toggle_view_mode function will be run.
        @key_bindings.add('v')
        def _toggle_view_mode(_event: KeyPressEvent) -> None:
            """Toggle view mode."""
            self.clock_pane.toggle_view_mode()

        # If you press the w key this _toggle_wrap_lines function will be run.
        @key_bindings.add('w')
        def _toggle_wrap_lines(_event: KeyPressEvent) -> None:
            """Toggle line wrapping."""
            self.clock_pane.toggle_wrap_lines()

        # Include the key_bindings keyword arg when passing to the parent class
        # __init__ function.
        kwargs['key_bindings'] = key_bindings
        # Call the parent FormattedTextControl.__init__
        super().__init__(*args, **kwargs)

    def mouse_handler(self, mouse_event: MouseEvent):
        """Mouse handler for this control."""
        # If the user clicks anywhere this function is run.

        # Mouse positions relative to this control. x is the column starting
        # from the left size as zero. y is the row starting with the top as
        # zero.
        _click_x = mouse_event.position.x
        _click_y = mouse_event.position.y

        # Mouse click behavior usually depends on if this window pane is in
        # focus. If not in focus, then focus on it when left clicking. If
        # already in focus then perform the action specific to this window.

        # If not in focus, change focus to this clock pane and do nothing else.
        if not has_focus(self.clock_pane)():
            if mouse_event.event_type == MouseEventType.MOUSE_UP:
                get_pw_console_app().focus_on_container(self.clock_pane)
                # Mouse event handled, return None.
                return None

        # If code reaches this point, this window is already in focus.
        # On left click
        if mouse_event.event_type == MouseEventType.MOUSE_UP:
            # Toggle the view mode.
            self.clock_pane.toggle_view_mode()
            # Mouse event handled, return None.
            return None

        # Mouse event not handled, return NotImplemented.
        return NotImplemented


class ClockPane(WindowPane, PluginMixin):
    """Example Pigweed Console plugin window that displays a clock.

    The ClockPane is a WindowPane based plugin that displays a clock and some
    formatted text examples. It inherits from both WindowPane and
    PluginMixin. It can be added on console startup by calling: ::

        my_console.add_window_plugin(ClockPane())

    For an example see:
    https://pigweed.dev/pw_console/embedding.html#adding-plugins
    """

    def __init__(self, *args, **kwargs):
        super().__init__(*args, pane_title='Clock', **kwargs)
        # Some toggle settings to change view and wrap lines.
        self.view_mode_clock: bool = True
        self.wrap_lines: bool = False
        # Counter variable to track how many times the background task runs.
        self.background_task_update_count: int = 0

        # ClockControl is responsible for rendering the dynamic content provided
        # by self._get_formatted_text() and handle keyboard and mouse input.
        # Using a control is always necessary for displaying any content that
        # will change.
        self.clock_control = ClockControl(
            self,  # This ClockPane class
            self._get_formatted_text,  # Callable to get text for display
            # These are FormattedTextControl options.
            # See the prompt_toolkit docs for all possible options
            # https://python-prompt-toolkit.readthedocs.io/en/latest/pages/reference.html#prompt_toolkit.layout.FormattedTextControl
            show_cursor=False,
            focusable=True,
        )

        # Every FormattedTextControl object (ClockControl) needs to live inside
        # a prompt_toolkit Window() instance. Here is where you specify
        # alignment, style, and dimensions. See the prompt_toolkit docs for all
        # opitons:
        # https://python-prompt-toolkit.readthedocs.io/en/latest/pages/reference.html#prompt_toolkit.layout.Window
        self.clock_control_window = Window(
            # Set the content to the clock_control defined above.
            content=self.clock_control,
            # Make content left aligned
            align=WindowAlign.LEFT,
            # These two set to false make this window fill all available space.
            dont_extend_width=False,
            dont_extend_height=False,
            # Content inside this window will have its lines wrapped if
            # self.wrap_lines is True.
            wrap_lines=Condition(lambda: self.wrap_lines),
        )

        # Create a toolbar for display at the bottom of this clock window. It
        # will show the window title and buttons.
        self.bottom_toolbar = WindowPaneToolbar(self)

        # Add a button to toggle the view mode.
        self.bottom_toolbar.add_button(
            ToolbarButton(
                key='v',  # Key binding for this function
                description='View Mode',  # Button name
                # Function to run when clicked.
                mouse_handler=self.toggle_view_mode,
            )
        )

        # Add a checkbox button to display if wrap_lines is enabled.
        self.bottom_toolbar.add_button(
            ToolbarButton(
                key='w',  # Key binding for this function
                description='Wrap',  # Button name
                # Function to run when clicked.
                mouse_handler=self.toggle_wrap_lines,
                # Display a checkbox in this button.
                is_checkbox=True,
                # lambda that returns the state of the checkbox
                checked=lambda: self.wrap_lines,
            )
        )

        # self.container is the root container that contains objects to be
        # rendered in the UI, one on top of the other.
        self.container = self._create_pane_container(
            # Display the clock window on top...
            self.clock_control_window,
            # and the bottom_toolbar below.
            self.bottom_toolbar,
        )

        # This plugin needs to run a task in the background periodically and
        # uses self.plugin_init() to set which function to run, and how often.
        # This is provided by PluginMixin. See the docs for more info:
        # https://pigweed.dev/pw_console/plugins.html#background-tasks
        self.plugin_init(
            plugin_callback=self._background_task,
            # Run self._background_task once per second.
            plugin_callback_frequency=1.0,
            plugin_logger_name='pw_console_example_clock_plugin',
        )

    def _background_task(self) -> bool:
        """Function run in the background for the ClockPane plugin."""
        self.background_task_update_count += 1
        # Make a log message for debugging purposes. For more info see:
        # https://pigweed.dev/pw_console/plugins.html#debugging-plugin-behavior
        self.plugin_logger.debug(
            'background_task_update_count: %s',
            self.background_task_update_count,
        )

        # Returning True in the background task will force the user interface to
        # re-draw.
        # Returning False means no updates required.
        return True

    def toggle_view_mode(self):
        """Toggle the view mode between the clock and formatted text example."""
        self.view_mode_clock = not self.view_mode_clock
        self.redraw_ui()

    def toggle_wrap_lines(self):
        """Enable or disable line wraping/truncation."""
        self.wrap_lines = not self.wrap_lines
        self.redraw_ui()

    def _get_formatted_text(self):
        """This function returns the content that will be displayed in the user
        interface depending on which view mode is active."""
        if self.view_mode_clock:
            return self._get_clock_text()
        return self._get_example_text()

    def _get_clock_text(self):
        """Create the time with some color formatting."""
        # pylint: disable=no-self-use

        # Get the date and time
        date, time = (
            datetime.now().isoformat(sep='_', timespec='seconds').split('_')
        )

        # Formatted text is represented as (style, text) tuples.
        # For more examples see:
        # https://python-prompt-toolkit.readthedocs.io/en/latest/pages/printing_text.html

        # These styles are selected using class names and start with the
        # 'class:' prefix. For all classes defined by Pigweed Console see:
        # https://cs.pigweed.dev/pigweed/+/main:pw_console/py/pw_console/style.py;l=189

        # Date in cyan matching the current Pigweed Console theme.
        date_with_color = ('class:theme-fg-cyan', date)
        # Time in magenta
        time_with_color = ('class:theme-fg-magenta', time)

        # No color styles for line breaks and spaces.
        line_break = ('', '\n')
        space = ('', ' ')

        # Concatenate the (style, text) tuples.
        return FormattedText(
            [
                line_break,
                space,
                space,
                date_with_color,
                space,
                time_with_color,
            ]
        )

    def _get_example_text(self):
        """Examples of how to create formatted text."""
        # pylint: disable=no-self-use
        # Make a list to hold all the formatted text to display.
        fragments = []

        # Some spacing vars
        wide_space = ('', '       ')
        space = ('', ' ')
        newline = ('', '\n')

        # HTML() is a shorthand way to style text. See:
        # https://python-prompt-toolkit.readthedocs.io/en/latest/pages/printing_text.html#html
        # This formats 'Foreground Colors' as underlined:
        fragments.append(HTML('<u>Foreground Colors</u>\n'))

        # Standard ANSI colors examples
        fragments.append(
            FormattedText(
                [
                    # These tuples follow this format:
                    #   (style_string, text_to_display)
                    ('ansiblack', 'ansiblack'),
                    wide_space,
                    ('ansired', 'ansired'),
                    wide_space,
                    ('ansigreen', 'ansigreen'),
                    wide_space,
                    ('ansiyellow', 'ansiyellow'),
                    wide_space,
                    ('ansiblue', 'ansiblue'),
                    wide_space,
                    ('ansimagenta', 'ansimagenta'),
                    wide_space,
                    ('ansicyan', 'ansicyan'),
                    wide_space,
                    ('ansigray', 'ansigray'),
                    wide_space,
                    newline,
                    ('ansibrightblack', 'ansibrightblack'),
                    space,
                    ('ansibrightred', 'ansibrightred'),
                    space,
                    ('ansibrightgreen', 'ansibrightgreen'),
                    space,
                    ('ansibrightyellow', 'ansibrightyellow'),
                    space,
                    ('ansibrightblue', 'ansibrightblue'),
                    space,
                    ('ansibrightmagenta', 'ansibrightmagenta'),
                    space,
                    ('ansibrightcyan', 'ansibrightcyan'),
                    space,
                    ('ansiwhite', 'ansiwhite'),
                    space,
                ]
            )
        )

        fragments.append(HTML('\n<u>Background Colors</u>\n'))
        fragments.append(
            FormattedText(
                [
                    # Here's an example of a style that specifies both
                    # background and foreground colors. The background color is
                    # prefixed with 'bg:'. The foreground color follows that
                    # with no prefix.
                    ('bg:ansiblack ansiwhite', 'ansiblack'),
                    wide_space,
                    ('bg:ansired', 'ansired'),
                    wide_space,
                    ('bg:ansigreen', 'ansigreen'),
                    wide_space,
                    ('bg:ansiyellow', 'ansiyellow'),
                    wide_space,
                    ('bg:ansiblue ansiwhite', 'ansiblue'),
                    wide_space,
                    ('bg:ansimagenta', 'ansimagenta'),
                    wide_space,
                    ('bg:ansicyan', 'ansicyan'),
                    wide_space,
                    ('bg:ansigray', 'ansigray'),
                    wide_space,
                    ('', '\n'),
                    ('bg:ansibrightblack', 'ansibrightblack'),
                    space,
                    ('bg:ansibrightred', 'ansibrightred'),
                    space,
                    ('bg:ansibrightgreen', 'ansibrightgreen'),
                    space,
                    ('bg:ansibrightyellow', 'ansibrightyellow'),
                    space,
                    ('bg:ansibrightblue', 'ansibrightblue'),
                    space,
                    ('bg:ansibrightmagenta', 'ansibrightmagenta'),
                    space,
                    ('bg:ansibrightcyan', 'ansibrightcyan'),
                    space,
                    ('bg:ansiwhite', 'ansiwhite'),
                    space,
                ]
            )
        )

        # pylint: disable=line-too-long
        # These themes use Pigweed Console style classes. See full list in:
        # https://cs.pigweed.dev/pigweed/+/main:pw_console/py/pw_console/style.py;l=189
        # pylint: enable=line-too-long
        fragments.append(HTML('\n\n<u>Current Theme Foreground Colors</u>\n'))
        fragments.append(
            [
                ('class:theme-fg-red', 'class:theme-fg-red'),
                newline,
                ('class:theme-fg-orange', 'class:theme-fg-orange'),
                newline,
                ('class:theme-fg-yellow', 'class:theme-fg-yellow'),
                newline,
                ('class:theme-fg-green', 'class:theme-fg-green'),
                newline,
                ('class:theme-fg-cyan', 'class:theme-fg-cyan'),
                newline,
                ('class:theme-fg-blue', 'class:theme-fg-blue'),
                newline,
                ('class:theme-fg-purple', 'class:theme-fg-purple'),
                newline,
                ('class:theme-fg-magenta', 'class:theme-fg-magenta'),
                newline,
            ]
        )

        fragments.append(HTML('\n<u>Current Theme Background Colors</u>\n'))
        fragments.append(
            [
                ('class:theme-bg-red', 'class:theme-bg-red'),
                newline,
                ('class:theme-bg-orange', 'class:theme-bg-orange'),
                newline,
                ('class:theme-bg-yellow', 'class:theme-bg-yellow'),
                newline,
                ('class:theme-bg-green', 'class:theme-bg-green'),
                newline,
                ('class:theme-bg-cyan', 'class:theme-bg-cyan'),
                newline,
                ('class:theme-bg-blue', 'class:theme-bg-blue'),
                newline,
                ('class:theme-bg-purple', 'class:theme-bg-purple'),
                newline,
                ('class:theme-bg-magenta', 'class:theme-bg-magenta'),
                newline,
            ]
        )

        fragments.append(HTML('\n<u>Theme UI Colors</u>\n'))
        fragments.append(
            [
                ('class:theme-fg-default', 'class:theme-fg-default'),
                space,
                ('class:theme-bg-default', 'class:theme-bg-default'),
                space,
                ('class:theme-bg-active', 'class:theme-bg-active'),
                space,
                ('class:theme-fg-active', 'class:theme-fg-active'),
                space,
                ('class:theme-bg-inactive', 'class:theme-bg-inactive'),
                space,
                ('class:theme-fg-inactive', 'class:theme-fg-inactive'),
                newline,
                ('class:theme-fg-dim', 'class:theme-fg-dim'),
                space,
                ('class:theme-bg-dim', 'class:theme-bg-dim'),
                space,
                ('class:theme-bg-dialog', 'class:theme-bg-dialog'),
                space,
                (
                    'class:theme-bg-line-highlight',
                    'class:theme-bg-line-highlight',
                ),
                space,
                (
                    'class:theme-bg-button-active',
                    'class:theme-bg-button-active',
                ),
                space,
                (
                    'class:theme-bg-button-inactive',
                    'class:theme-bg-button-inactive',
                ),
                space,
            ]
        )

        # Return all formatted text lists merged together.
        return merge_formatted_text(fragments)
