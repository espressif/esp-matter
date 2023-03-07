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
"""Example Plugin that displays some dynamic content: a game of 2048."""

from random import choice
from typing import Iterable, List, Tuple, TYPE_CHECKING
import time

from prompt_toolkit.filters import has_focus
from prompt_toolkit.formatted_text import StyleAndTextTuples
from prompt_toolkit.key_binding import KeyBindings, KeyPressEvent
from prompt_toolkit.layout import (
    AnyContainer,
    Dimension,
    FormattedTextControl,
    HSplit,
    Window,
    WindowAlign,
    VSplit,
)
from prompt_toolkit.mouse_events import MouseEvent, MouseEventType
from prompt_toolkit.widgets import MenuItem

from pw_console.widgets import (
    create_border,
    FloatingWindowPane,
    ToolbarButton,
    WindowPaneToolbar,
)
from pw_console.plugin_mixin import PluginMixin
from pw_console.get_pw_console_app import get_pw_console_app

if TYPE_CHECKING:
    from pw_console.console_app import ConsoleApp

Twenty48Cell = Tuple[int, int, int]


class Twenty48Game:
    """2048 Game."""

    def __init__(self) -> None:
        self.colors = {
            2: 'bg:#dd6',
            4: 'bg:#da6',
            8: 'bg:#d86',
            16: 'bg:#d66',
            32: 'bg:#d6a',
            64: 'bg:#a6d',
            128: 'bg:#66d',
            256: 'bg:#68a',
            512: 'bg:#6a8',
            1024: 'bg:#6d6',
            2048: 'bg:#0f8',
            4096: 'bg:#0ff',
        }
        self.board: List[List[int]]
        self.last_board: List[Twenty48Cell]
        self.move_count: int
        self.width: int = 4
        self.height: int = 4
        self.max_value: int = 0
        self.start_time: float
        self.reset_game()

    def reset_game(self) -> None:
        self.start_time = time.time()
        self.max_value = 2
        self.move_count = 0
        self.board = []
        for _i in range(self.height):
            self.board.append([0] * self.width)
        self.last_board = list(self.all_cells())
        self.add_random_tiles(2)

    def stats(self) -> StyleAndTextTuples:
        """Returns stats on the game in progress."""
        elapsed_time = int(time.time() - self.start_time)
        minutes = int(elapsed_time / 60.0)
        seconds = elapsed_time % 60
        fragments: StyleAndTextTuples = []
        fragments.append(('', '\n'))
        fragments.append(('', f'Moves: {self.move_count}'))
        fragments.append(('', '\n'))
        fragments.append(('', 'Time:  {:0>2}:{:0>2}'.format(minutes, seconds)))
        fragments.append(('', '\n'))
        fragments.append(('', f'Max: {self.max_value}'))
        fragments.append(('', '\n\n'))
        fragments.append(('', 'Press R to restart\n'))
        fragments.append(('', '\n'))
        fragments.append(('', 'Arrow keys to move'))
        return fragments

    def __pt_formatted_text__(self) -> StyleAndTextTuples:
        """Returns the game board formatted in a grid with colors."""
        fragments: StyleAndTextTuples = []

        def print_row(row: List[int], include_number: bool = False) -> None:
            fragments.append(('', '  '))
            for col in row:
                style = 'class:theme-fg-default '
                if col > 0:
                    style = '#000 '
                style += self.colors.get(col, '')
                text = ' ' * 6
                if include_number:
                    text = '{:^6}'.format(col)
                fragments.append((style, text))
            fragments.append(('', '\n'))

        fragments.append(('', '\n'))
        for row in self.board:
            print_row(row)
            print_row(row, include_number=True)
            print_row(row)

        return fragments

    def __repr__(self) -> str:
        board = ''
        for row_cells in self.board:
            for column in row_cells:
                board += '{:^6}'.format(column)
            board += '\n'
        return board

    def all_cells(self) -> Iterable[Twenty48Cell]:
        for row, row_cells in enumerate(self.board):
            for col, cell_value in enumerate(row_cells):
                yield (row, col, cell_value)

    def update_max_value(self) -> None:
        for _row, _col, value in self.all_cells():
            if value > self.max_value:
                self.max_value = value

    def empty_cells(self) -> Iterable[Twenty48Cell]:
        for row, row_cells in enumerate(self.board):
            for col, cell_value in enumerate(row_cells):
                if cell_value != 0:
                    continue
                yield (row, col, cell_value)

    def _board_changed(self) -> bool:
        return self.last_board != list(self.all_cells())

    def complete_move(self) -> None:
        if not self._board_changed():
            # Move did nothing, ignore.
            return

        self.update_max_value()
        self.move_count += 1
        self.add_random_tiles()
        self.last_board = list(self.all_cells())

    def add_random_tiles(self, count: int = 1) -> None:
        for _i in range(count):
            empty_cells = list(self.empty_cells())
            if not empty_cells:
                return
            row, col, _value = choice(empty_cells)
            self.board[row][col] = 2

    def row(self, row_index: int) -> Iterable[Twenty48Cell]:
        for col, cell_value in enumerate(self.board[row_index]):
            yield (row_index, col, cell_value)

    def col(self, col_index: int) -> Iterable[Twenty48Cell]:
        for row, row_cells in enumerate(self.board):
            for col, cell_value in enumerate(row_cells):
                if col == col_index:
                    yield (row, col, cell_value)

    def non_zero_row_values(self, index: int) -> Tuple[List, List]:
        non_zero_values = [
            value for row, col, value in self.row(index) if value != 0
        ]
        padding = [0] * (self.width - len(non_zero_values))
        return (non_zero_values, padding)

    def move_right(self) -> None:
        for i in range(self.height):
            non_zero_values, padding = self.non_zero_row_values(i)
            self.board[i] = padding + non_zero_values

    def move_left(self) -> None:
        for i in range(self.height):
            non_zero_values, padding = self.non_zero_row_values(i)
            self.board[i] = non_zero_values + padding

    def add_horizontal(self, reverse=False) -> None:
        for i in range(self.width):
            this_row = list(self.row(i))
            if reverse:
                this_row = list(reversed(this_row))
            for row, col, this_cell in this_row:
                if this_cell == 0 or col >= self.width - 1:
                    continue
                next_cell = self.board[row][col + 1]
                if this_cell == next_cell:
                    self.board[row][col] = 0
                    self.board[row][col + 1] = this_cell * 2
                    break

    def non_zero_col_values(self, index: int) -> Tuple[List, List]:
        non_zero_values = [
            value for row, col, value in self.col(index) if value != 0
        ]
        padding = [0] * (self.height - len(non_zero_values))
        return (non_zero_values, padding)

    def _set_column(self, col_index: int, values: List[int]) -> None:
        for row, value in enumerate(values):
            self.board[row][col_index] = value

    def add_vertical(self, reverse=False) -> None:
        for i in range(self.height):
            this_column = list(self.col(i))
            if reverse:
                this_column = list(reversed(this_column))
            for row, col, this_cell in this_column:
                if this_cell == 0 or row >= self.height - 1:
                    continue
                next_cell = self.board[row + 1][col]
                if this_cell == next_cell:
                    self.board[row][col] = 0
                    self.board[row + 1][col] = this_cell * 2
                    break

    def move_down(self) -> None:
        for col_index in range(self.width):
            non_zero_values, padding = self.non_zero_col_values(col_index)
            self._set_column(col_index, padding + non_zero_values)

    def move_up(self) -> None:
        for col_index in range(self.width):
            non_zero_values, padding = self.non_zero_col_values(col_index)
            self._set_column(col_index, non_zero_values + padding)

    def press_down(self) -> None:
        self.move_down()
        self.add_vertical(reverse=True)
        self.move_down()
        self.complete_move()

    def press_up(self) -> None:
        self.move_up()
        self.add_vertical()
        self.move_up()
        self.complete_move()

    def press_right(self) -> None:
        self.move_right()
        self.add_horizontal(reverse=True)
        self.move_right()
        self.complete_move()

    def press_left(self) -> None:
        self.move_left()
        self.add_horizontal()
        self.move_left()
        self.complete_move()


class Twenty48Control(FormattedTextControl):
    """Example prompt_toolkit UIControl for displaying formatted text.

    This is the prompt_toolkit class that is responsible for drawing the 2048,
    handling keybindings if in focus, and mouse input.
    """

    def __init__(self, twenty48_pane: 'Twenty48Pane', *args, **kwargs) -> None:
        self.twenty48_pane = twenty48_pane
        self.game = self.twenty48_pane.game

        # Set some custom key bindings to toggle the view mode and wrap lines.
        key_bindings = KeyBindings()

        @key_bindings.add('R')
        def _restart(_event: KeyPressEvent) -> None:
            """Restart the game."""
            self.game.reset_game()

        @key_bindings.add('q')
        def _quit(_event: KeyPressEvent) -> None:
            """Quit the game."""
            self.twenty48_pane.close_dialog()

        @key_bindings.add('j')
        @key_bindings.add('down')
        def _move_down(_event: KeyPressEvent) -> None:
            """Move down"""
            self.game.press_down()

        @key_bindings.add('k')
        @key_bindings.add('up')
        def _move_up(_event: KeyPressEvent) -> None:
            """Move up."""
            self.game.press_up()

        @key_bindings.add('h')
        @key_bindings.add('left')
        def _move_left(_event: KeyPressEvent) -> None:
            """Move left."""
            self.game.press_left()

        @key_bindings.add('l')
        @key_bindings.add('right')
        def _move_right(_event: KeyPressEvent) -> None:
            """Move right."""
            self.game.press_right()

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

        # If not in focus, change focus to this 2048 pane and do nothing else.
        if not has_focus(self.twenty48_pane)():
            if mouse_event.event_type == MouseEventType.MOUSE_UP:
                get_pw_console_app().focus_on_container(self.twenty48_pane)
                # Mouse event handled, return None.
                return None

        # If code reaches this point, this window is already in focus.
        # if mouse_event.event_type == MouseEventType.MOUSE_UP:
        #     # Toggle the view mode.
        #     self.twenty48_pane.toggle_view_mode()
        #     # Mouse event handled, return None.
        #     return None

        # Mouse event not handled, return NotImplemented.
        return NotImplemented


class Twenty48Pane(FloatingWindowPane, PluginMixin):
    """Example Pigweed Console plugin to play 2048.

    The Twenty48Pane is a WindowPane based plugin that displays an interactive
    game of 2048. It inherits from both WindowPane and PluginMixin. It can be
    added on console startup by calling: ::

        my_console.add_window_plugin(Twenty48Pane())

    For an example see:
    https://pigweed.dev/pw_console/embedding.html#adding-plugins
    """

    def __init__(self, include_resize_handle: bool = True, **kwargs):

        super().__init__(
            pane_title='2048',
            height=Dimension(preferred=17),
            width=Dimension(preferred=50),
            **kwargs,
        )
        self.game = Twenty48Game()

        # Hide by default.
        self.show_pane = False

        # Create a toolbar for display at the bottom of the 2048 window. It
        # will show the window title and buttons.
        self.bottom_toolbar = WindowPaneToolbar(
            self, include_resize_handle=include_resize_handle
        )

        # Add a button to restart the game.
        self.bottom_toolbar.add_button(
            ToolbarButton(
                key='R',  # Key binding help text for this function
                description='Restart',  # Button name
                # Function to run when clicked.
                mouse_handler=self.game.reset_game,
            )
        )
        # Add a button to restart the game.
        self.bottom_toolbar.add_button(
            ToolbarButton(
                key='q',  # Key binding help text for this function
                description='Quit',  # Button name
                # Function to run when clicked.
                mouse_handler=self.close_dialog,
            )
        )

        # Every FormattedTextControl object (Twenty48Control) needs to live
        # inside a prompt_toolkit Window() instance. Here is where you specify
        # alignment, style, and dimensions. See the prompt_toolkit docs for all
        # opitons:
        # https://python-prompt-toolkit.readthedocs.io/en/latest/pages/reference.html#prompt_toolkit.layout.Window
        self.twenty48_game_window = Window(
            # Set the content to a Twenty48Control instance.
            content=Twenty48Control(
                self,  # This Twenty48Pane class
                self.game,  # Content from Twenty48Game.__pt_formatted_text__()
                show_cursor=False,
                focusable=True,
            ),
            # Make content left aligned
            align=WindowAlign.LEFT,
            # These two set to false make this window fill all available space.
            dont_extend_width=True,
            dont_extend_height=False,
            wrap_lines=False,
            width=Dimension(preferred=28),
            height=Dimension(preferred=15),
        )

        self.twenty48_stats_window = Window(
            content=Twenty48Control(
                self,  # This Twenty48Pane class
                self.game.stats,  # Content from Twenty48Game.stats()
                show_cursor=False,
                focusable=True,
            ),
            # Make content left aligned
            align=WindowAlign.LEFT,
            # These two set to false make this window fill all available space.
            width=Dimension(preferred=20),
            dont_extend_width=False,
            dont_extend_height=False,
            wrap_lines=False,
        )

        # self.container is the root container that contains objects to be
        # rendered in the UI, one on top of the other.
        self.container = self._create_pane_container(
            create_border(
                HSplit(
                    [
                        # Vertical split content
                        VSplit(
                            [
                                # Left side will show the game board.
                                self.twenty48_game_window,
                                # Stats will be shown on the right.
                                self.twenty48_stats_window,
                            ]
                        ),
                        # The bottom_toolbar is shown below the VSplit.
                        self.bottom_toolbar,
                    ]
                ),
                title='2048',
                border_style='class:command-runner-border',
                # left_margin_columns=1,
                # right_margin_columns=1,
            )
        )

        self.dialog_content: List[AnyContainer] = [
            # Vertical split content
            VSplit(
                [
                    # Left side will show the game board.
                    self.twenty48_game_window,
                    # Stats will be shown on the right.
                    self.twenty48_stats_window,
                ]
            ),
            # The bottom_toolbar is shown below the VSplit.
            self.bottom_toolbar,
        ]
        # Wrap the dialog content in a border
        self.bordered_dialog_content = create_border(
            HSplit(self.dialog_content),
            title='2048',
            border_style='class:command-runner-border',
        )
        # self.container is the root container that contains objects to be
        # rendered in the UI, one on top of the other.
        if include_resize_handle:
            self.container = self._create_pane_container(*self.dialog_content)
        else:
            self.container = self._create_pane_container(
                self.bordered_dialog_content
            )

        # This plugin needs to run a task in the background periodically and
        # uses self.plugin_init() to set which function to run, and how often.
        # This is provided by PluginMixin. See the docs for more info:
        # https://pigweed.dev/pw_console/plugins.html#background-tasks
        self.plugin_init(
            plugin_callback=self._background_task,
            # Run self._background_task once per second.
            plugin_callback_frequency=1.0,
            plugin_logger_name='pw_console_example_2048_plugin',
        )

    def get_top_level_menus(self) -> List[MenuItem]:
        def _toggle_dialog() -> None:
            self.toggle_dialog()

        return [
            MenuItem(
                '[2048]',
                children=[
                    MenuItem(
                        'Example Top Level Menu', handler=None, disabled=True
                    ),
                    # Menu separator
                    MenuItem('-', None),
                    MenuItem('Show/Hide 2048 Game', handler=_toggle_dialog),
                    MenuItem('Restart', handler=self.game.reset_game),
                ],
            ),
        ]

    def pw_console_init(self, app: 'ConsoleApp') -> None:
        """Set the Pigweed Console application instance.

        This function is called after the Pigweed Console starts up and allows
        access to the user preferences. Prefs is required for creating new
        user-remappable keybinds."""
        self.application = app

    def _background_task(self) -> bool:
        """Function run in the background for the ClockPane plugin."""
        # Optional: make a log message for debugging purposes. For more info
        # see:
        # https://pigweed.dev/pw_console/plugins.html#debugging-plugin-behavior
        # self.plugin_logger.debug('background_task_update_count: %s',
        #                          self.background_task_update_count)

        # Returning True in the background task will force the user interface to
        # re-draw.
        # Returning False means no updates required.

        if self.show_pane:
            # Return true so the game clock is updated.
            return True

        # Game window is hidden, don't redraw.
        return False
