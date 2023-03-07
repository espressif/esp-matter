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

from abc import ABC
from typing import Any, Callable, List, Optional, Tuple, TYPE_CHECKING, Union
import functools

from prompt_toolkit.layout.dimension import AnyDimension

from prompt_toolkit.filters import Condition
from prompt_toolkit.layout import (
    AnyContainer,
    ConditionalContainer,
    Dimension,
    HSplit,
    walk,
)
from prompt_toolkit.widgets import MenuItem

from pw_console.get_pw_console_app import get_pw_console_app
from pw_console.style import get_pane_style

if TYPE_CHECKING:
    from pw_console.console_app import ConsoleApp


class WindowPaneHSplit(HSplit):
    """PromptToolkit HSplit that saves the current width and height.

    This overrides the write_to_screen function to save the width and height of
    the container to be rendered.
    """

    def __init__(self, parent_window_pane, *args, **kwargs):
        # Save a reference to the parent window pane.
        self.parent_window_pane = parent_window_pane
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
        # Save the width and height for the current render pass. This will be
        # used by the log pane to render the correct amount of log lines.
        self.parent_window_pane.update_pane_size(
            write_position.width, write_position.height
        )
        # Continue writing content to the screen.
        super().write_to_screen(
            screen,
            mouse_handlers,
            write_position,
            parent_style,
            erase_bg,
            z_index,
        )


class WindowPane(ABC):
    """The Pigweed Console Window Pane parent class."""

    # pylint: disable=too-many-instance-attributes
    def __init__(
        self,
        application: Union['ConsoleApp', Any] = None,
        pane_title: str = 'Window',
        height: Optional[AnyDimension] = None,
        width: Optional[AnyDimension] = None,
    ):
        if application:
            self.application = application
        else:
            self.application = get_pw_console_app()

        self._pane_title = pane_title
        self._pane_subtitle: str = ''

        # Default width and height to 10 lines each. They will be resized by the
        # WindowManager later.
        self.height = height if height else Dimension(preferred=10)
        self.width = width if width else Dimension(preferred=10)

        # Boolean to show or hide this window pane
        self.show_pane = True
        # Booleans for toggling top and bottom toolbars
        self.show_top_toolbar = True
        self.show_bottom_toolbar = True

        # Height and width values for the current rendering pass.
        self.current_pane_width = 0
        self.current_pane_height = 0
        self.last_pane_width = 0
        self.last_pane_height = 0

    def __repr__(self) -> str:
        """Create a repr with this pane's title and subtitle."""
        repr_str = f'{type(self).__qualname__}(pane_title="{self.pane_title()}"'
        if self.pane_subtitle():
            repr_str += f', pane_subtitle="{self.pane_subtitle()}"'
        repr_str += ')'
        return repr_str

    def pane_title(self) -> str:
        return self._pane_title

    def set_pane_title(self, title: str) -> None:
        self._pane_title = title

    def menu_title(self) -> str:
        """Return a title to display in the Window menu."""
        return self.pane_title()

    def pane_subtitle(self) -> str:  # pylint: disable=no-self-use
        """Further title info for display in the Window menu."""
        return ''

    def redraw_ui(self) -> None:
        """Redraw the prompt_toolkit UI."""
        if not hasattr(self, 'application'):
            return
        # Thread safe way of sending a repaint trigger to the input event loop.
        self.application.redraw_ui()

    def focus_self(self) -> None:
        """Switch prompt_toolkit focus to this window pane."""
        if not hasattr(self, 'application'):
            return
        self.application.focus_on_container(self)

    def __pt_container__(self):
        """Return the prompt_toolkit root container for this log pane.

        This allows self to be used wherever prompt_toolkit expects a container
        object."""
        return self.container  # pylint: disable=no-member

    def get_all_key_bindings(self) -> List:
        """Return keybinds for display in the help window.

        For example:

        Using a prompt_toolkit control:

          return [self.some_content_control_instance.get_key_bindings()]

        Hand-crafted bindings for display in the HelpWindow:

          return [{
              'Execute code': ['Enter', 'Option-Enter', 'Meta-Enter'],
              'Reverse search history': ['Ctrl-R'],
              'Erase input buffer.': ['Ctrl-C'],
              'Show settings.': ['F2'],
              'Show history.': ['F3'],
          }]
        """
        # pylint: disable=no-self-use
        return []

    def get_window_menu_options(
        self,
    ) -> List[Tuple[str, Union[Callable, None]]]:
        """Return menu options for the window pane.

        Should return a list of tuples containing with the display text and
        callable to invoke on click.
        """
        # pylint: disable=no-self-use
        return []

    def get_top_level_menus(self) -> List[MenuItem]:
        """Return MenuItems to be displayed on the main pw_console menu bar."""
        # pylint: disable=no-self-use
        return []

    def pane_resized(self) -> bool:
        """Return True if the current window size has changed."""
        return (
            self.last_pane_width != self.current_pane_width
            or self.last_pane_height != self.current_pane_height
        )

    def update_pane_size(self, width, height) -> None:
        """Save pane width and height for the current UI render pass."""
        if width:
            self.last_pane_width = self.current_pane_width
            self.current_pane_width = width
        if height:
            self.last_pane_height = self.current_pane_height
            self.current_pane_height = height

    def _create_pane_container(self, *content) -> ConditionalContainer:
        return ConditionalContainer(
            WindowPaneHSplit(
                self,
                content,
                # Window pane dimensions
                height=lambda: self.height,
                width=lambda: self.width,
                style=functools.partial(get_pane_style, self),
            ),
            filter=Condition(lambda: self.show_pane),
        )

    def has_child_container(self, child_container: AnyContainer) -> bool:
        if not child_container:
            return False
        for container in walk(self.__pt_container__()):
            if container == child_container:
                return True
        return False


class FloatingWindowPane(WindowPane):
    """The Pigweed Console FloatingWindowPane class."""

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        # Tracks the last focused container, to enable restoring focus after
        # closing the dialog.
        self.last_focused_pane = None

    def close_dialog(self) -> None:
        """Close runner dialog box."""
        self.show_pane = False

        # Restore original focus if possible.
        if self.last_focused_pane:
            self.application.focus_on_container(self.last_focused_pane)
        else:
            # Fallback to focusing on the main menu.
            self.application.focus_main_menu()

        self.application.update_menu_items()

    def open_dialog(self) -> None:
        self.show_pane = True
        self.last_focused_pane = self.application.focused_window()
        self.focus_self()
        self.application.redraw_ui()

        self.application.update_menu_items()

    def toggle_dialog(self) -> bool:
        if self.show_pane:
            self.close_dialog()
        else:
            self.open_dialog()
        # The focused window has changed. Return true so
        # ConsoleApp.run_pane_menu_option does not set the focus to the main
        # menu.
        return True
