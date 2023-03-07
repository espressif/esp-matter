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
"""UI Color Styles for ConsoleApp."""

import logging
from dataclasses import dataclass

from prompt_toolkit.formatted_text import StyleAndTextTuples
from prompt_toolkit.formatted_text.base import OneStyleAndTextTuple
from prompt_toolkit.styles import Style
from prompt_toolkit.filters import has_focus

_LOG = logging.getLogger(__package__)


@dataclass
class HighContrastDarkColors:
    # pylint: disable=too-many-instance-attributes
    default_bg = '#100f10'
    default_fg = '#ffffff'

    dim_bg = '#000000'
    dim_fg = '#e0e6f0'

    button_active_bg = '#4e4e4e'
    button_inactive_bg = '#323232'

    active_bg = '#323232'
    active_fg = '#f4f4f4'

    inactive_bg = '#1e1e1e'
    inactive_fg = '#bfc0c4'

    line_highlight_bg = '#2f2f2f'
    selected_line_bg = '#4e4e4e'
    dialog_bg = '#3c3c3c'

    red_accent = '#ffc0bf'
    orange_accent = '#f5ca80'
    yellow_accent = '#eedc82'
    green_accent = '#88ef88'
    cyan_accent = '#60e7e0'
    blue_accent = '#92d9ff'
    purple_accent = '#cfcaff'
    magenta_accent = '#ffb8ff'


@dataclass
class DarkColors:
    # pylint: disable=too-many-instance-attributes
    default_bg = '#2e2e2e'
    default_fg = '#eeeeee'

    dim_bg = '#262626'
    dim_fg = '#dfdfdf'

    button_active_bg = '#626262'
    button_inactive_bg = '#525252'

    active_bg = '#525252'
    active_fg = '#dfdfdf'

    inactive_bg = '#3f3f3f'
    inactive_fg = '#bfbfbf'

    line_highlight_bg = '#525252'
    selected_line_bg = '#626262'
    dialog_bg = '#3c3c3c'

    red_accent = '#ff6c6b'
    orange_accent = '#da8548'
    yellow_accent = '#ffcc66'
    green_accent = '#98be65'
    cyan_accent = '#66cccc'
    blue_accent = '#6699cc'
    purple_accent = '#a9a1e1'
    magenta_accent = '#c678dd'


@dataclass
class NordColors:
    # pylint: disable=too-many-instance-attributes
    default_bg = '#2e3440'
    default_fg = '#eceff4'

    dim_bg = '#272c36'
    dim_fg = '#e5e9f0'

    button_active_bg = '#4c566a'
    button_inactive_bg = '#434c5e'

    active_bg = '#434c5e'
    active_fg = '#eceff4'

    inactive_bg = '#373e4c'
    inactive_fg = '#d8dee9'

    line_highlight_bg = '#191c25'
    selected_line_bg = '#4c566a'
    dialog_bg = '#2c333f'

    red_accent = '#bf616a'
    orange_accent = '#d08770'
    yellow_accent = '#ebcb8b'
    green_accent = '#a3be8c'
    cyan_accent = '#88c0d0'
    blue_accent = '#81a1c1'
    purple_accent = '#a9a1e1'
    magenta_accent = '#b48ead'


@dataclass
class NordLightColors:
    # pylint: disable=too-many-instance-attributes
    default_bg = '#e5e9f0'
    default_fg = '#3b4252'
    dim_bg = '#d8dee9'
    dim_fg = '#2e3440'
    button_active_bg = '#aebacf'
    button_inactive_bg = '#b8c5db'
    active_bg = '#b8c5db'
    active_fg = '#3b4252'
    inactive_bg = '#c2d0e7'
    inactive_fg = '#60728c'
    line_highlight_bg = '#f0f4fc'
    selected_line_bg = '#f0f4fc'
    dialog_bg = '#d8dee9'

    red_accent = '#99324b'
    orange_accent = '#ac4426'
    yellow_accent = '#9a7500'
    green_accent = '#4f894c'
    cyan_accent = '#398eac'
    blue_accent = '#3b6ea8'
    purple_accent = '#842879'
    magenta_accent = '#97365b'


@dataclass
class MoonlightColors:
    # pylint: disable=too-many-instance-attributes
    default_bg = '#212337'
    default_fg = '#c8d3f5'
    dim_bg = '#191a2a'
    dim_fg = '#b4c2f0'
    button_active_bg = '#444a73'
    button_inactive_bg = '#2f334d'
    active_bg = '#2f334d'
    active_fg = '#c8d3f5'
    inactive_bg = '#222436'
    inactive_fg = '#a9b8e8'
    line_highlight_bg = '#383e5c'
    selected_line_bg = '#444a73'
    dialog_bg = '#1e2030'

    red_accent = '#d95468'
    orange_accent = '#d98e48'
    yellow_accent = '#ebbf83'
    green_accent = '#8bd49c'
    cyan_accent = '#70e1e8'
    blue_accent = '#5ec4ff'
    purple_accent = '#b62d65'
    magenta_accent = '#e27e8d'


@dataclass
class AnsiTerm:
    # pylint: disable=too-many-instance-attributes
    default_bg = 'default'
    default_fg = 'default'

    dim_bg = 'default'
    dim_fg = 'default'

    button_active_bg = 'default underline'
    button_inactive_bg = 'default'

    active_bg = 'default'
    active_fg = 'default'

    inactive_bg = 'default'
    inactive_fg = 'default'

    line_highlight_bg = 'ansidarkgray white'
    selected_line_bg = 'default reverse'
    dialog_bg = 'default'

    red_accent = 'ansired'
    orange_accent = 'orange'
    yellow_accent = 'ansiyellow'
    green_accent = 'ansigreen'
    cyan_accent = 'ansicyan'
    blue_accent = 'ansiblue'
    purple_accent = 'ansipurple'
    magenta_accent = 'ansimagenta'


_THEME_NAME_MAPPING = {
    'moonlight': MoonlightColors(),
    'nord': NordColors(),
    'nord-light': NordLightColors(),
    'dark': DarkColors(),
    'high-contrast-dark': HighContrastDarkColors(),
    'ansi': AnsiTerm(),
}


def get_theme_colors(theme_name=''):
    theme = _THEME_NAME_MAPPING.get(theme_name, DarkColors())
    return theme


def generate_styles(theme_name='dark'):
    """Return prompt_toolkit styles for the given theme name."""
    # Use DarkColors() if name not found.
    theme = _THEME_NAME_MAPPING.get(theme_name, DarkColors())

    pw_console_styles = {
        # Default text and background.
        'default': 'bg:{} {}'.format(theme.default_bg, theme.default_fg),
        # Dim inactive panes.
        'pane_inactive': 'bg:{} {}'.format(theme.dim_bg, theme.dim_fg),
        # Use default for active panes.
        'pane_active': 'bg:{} {}'.format(theme.default_bg, theme.default_fg),
        # Brighten active pane toolbars.
        'toolbar_active': 'bg:{} {}'.format(theme.active_bg, theme.active_fg),
        'toolbar_inactive': 'bg:{} {}'.format(
            theme.inactive_bg, theme.inactive_fg
        ),
        # Dimmer toolbar.
        'toolbar_dim_active': 'bg:{} {}'.format(
            theme.active_bg, theme.active_fg
        ),
        'toolbar_dim_inactive': 'bg:{} {}'.format(
            theme.default_bg, theme.inactive_fg
        ),
        # Used for pane titles
        'toolbar_accent': theme.cyan_accent,
        'toolbar-button-decoration': '{}'.format(theme.cyan_accent),
        'toolbar-setting-active': 'bg:{} {}'.format(
            theme.green_accent,
            theme.active_bg,
        ),
        'toolbar-button-active': 'bg:{}'.format(theme.button_active_bg),
        'toolbar-button-inactive': 'bg:{}'.format(theme.button_inactive_bg),
        # prompt_toolkit scrollbar styles:
        'scrollbar.background': 'bg:{} {}'.format(
            theme.default_bg, theme.default_fg
        ),
        # Scrollbar handle, bg is the bar color.
        'scrollbar.button': 'bg:{} {}'.format(
            theme.purple_accent, theme.default_bg
        ),
        'scrollbar.arrow': 'bg:{} {}'.format(
            theme.default_bg, theme.blue_accent
        ),
        # Unstyled scrollbar classes:
        # 'scrollbar.start'
        # 'scrollbar.end'
        # Top menu bar styles
        'menu-bar': 'bg:{} {}'.format(theme.inactive_bg, theme.inactive_fg),
        'menu-bar.selected-item': 'bg:{} {}'.format(
            theme.blue_accent, theme.inactive_bg
        ),
        # Menu background
        'menu': 'bg:{} {}'.format(theme.dialog_bg, theme.dim_fg),
        # Menu item separator
        'menu-border': theme.magenta_accent,
        # Top bar logo + keyboard shortcuts
        'logo': '{} bold'.format(theme.magenta_accent),
        'keybind': '{} bold'.format(theme.purple_accent),
        'keyhelp': theme.dim_fg,
        # Help window styles
        'help_window_content': 'bg:{} {}'.format(theme.dialog_bg, theme.dim_fg),
        'frame.border': 'bg:{} {}'.format(theme.dialog_bg, theme.purple_accent),
        'pane_indicator_active': 'bg:{}'.format(theme.magenta_accent),
        'pane_indicator_inactive': 'bg:{}'.format(theme.inactive_bg),
        'pane_title_active': '{} bold'.format(theme.magenta_accent),
        'pane_title_inactive': '{}'.format(theme.purple_accent),
        'window-tab-active': 'bg:{} {}'.format(
            theme.active_bg, theme.cyan_accent
        ),
        'window-tab-inactive': 'bg:{} {}'.format(
            theme.inactive_bg, theme.inactive_fg
        ),
        'pane_separator': 'bg:{} {}'.format(
            theme.default_bg, theme.purple_accent
        ),
        # Search matches
        'search': 'bg:{} {}'.format(theme.cyan_accent, theme.default_bg),
        'search.current': 'bg:{} {}'.format(
            theme.cyan_accent, theme.default_bg
        ),
        # Highlighted line styles
        'selected-log-line': 'bg:{}'.format(theme.line_highlight_bg),
        'marked-log-line': 'bg:{}'.format(theme.selected_line_bg),
        'cursor-line': 'bg:{} nounderline'.format(theme.line_highlight_bg),
        # Messages like 'Window too small'
        'warning-text': 'bg:{} {}'.format(
            theme.default_bg, theme.yellow_accent
        ),
        'log-time': 'bg:{} {}'.format(theme.default_fg, theme.default_bg),
        # Apply foreground only for level and column values. This way the text
        # can inherit the background color of the parent window pane or line
        # selection.
        'log-level-{}'.format(logging.CRITICAL): '{} bold'.format(
            theme.red_accent
        ),
        'log-level-{}'.format(logging.ERROR): '{}'.format(theme.red_accent),
        'log-level-{}'.format(logging.WARNING): '{}'.format(
            theme.yellow_accent
        ),
        'log-level-{}'.format(logging.INFO): '{}'.format(theme.purple_accent),
        'log-level-{}'.format(logging.DEBUG): '{}'.format(theme.blue_accent),
        'log-table-column-0': '{}'.format(theme.cyan_accent),
        'log-table-column-1': '{}'.format(theme.green_accent),
        'log-table-column-2': '{}'.format(theme.yellow_accent),
        'log-table-column-3': '{}'.format(theme.magenta_accent),
        'log-table-column-4': '{}'.format(theme.purple_accent),
        'log-table-column-5': '{}'.format(theme.blue_accent),
        'log-table-column-6': '{}'.format(theme.orange_accent),
        'log-table-column-7': '{}'.format(theme.red_accent),
        'search-bar': 'bg:{}'.format(theme.inactive_bg),
        'search-bar-title': 'bg:{} {}'.format(
            theme.cyan_accent, theme.default_bg
        ),
        'search-bar-setting': '{}'.format(theme.cyan_accent),
        'search-bar-border': 'bg:{} {}'.format(
            theme.inactive_bg, theme.cyan_accent
        ),
        'search-match-count-dialog': 'bg:{}'.format(theme.inactive_bg),
        'search-match-count-dialog-title': '{}'.format(theme.cyan_accent),
        'search-match-count-dialog-default-fg': '{}'.format(theme.default_fg),
        'search-match-count-dialog-border': 'bg:{} {}'.format(
            theme.inactive_bg, theme.cyan_accent
        ),
        'filter-bar': 'bg:{}'.format(theme.inactive_bg),
        'filter-bar-title': 'bg:{} {}'.format(
            theme.red_accent, theme.default_bg
        ),
        'filter-bar-setting': '{}'.format(theme.cyan_accent),
        'filter-bar-delete': '{}'.format(theme.red_accent),
        'filter-bar-delimiter': '{}'.format(theme.purple_accent),
        'saveas-dialog': 'bg:{}'.format(theme.inactive_bg),
        'saveas-dialog-title': 'bg:{} {}'.format(
            theme.inactive_bg, theme.default_fg
        ),
        'saveas-dialog-setting': '{}'.format(theme.cyan_accent),
        'saveas-dialog-border': 'bg:{} {}'.format(
            theme.inactive_bg, theme.cyan_accent
        ),
        'selection-dialog': 'bg:{}'.format(theme.inactive_bg),
        'selection-dialog-title': '{}'.format(theme.yellow_accent),
        'selection-dialog-default-fg': '{}'.format(theme.default_fg),
        'selection-dialog-action-bg': 'bg:{}'.format(theme.yellow_accent),
        'selection-dialog-action-fg': '{}'.format(theme.button_inactive_bg),
        'selection-dialog-border': 'bg:{} {}'.format(
            theme.inactive_bg, theme.yellow_accent
        ),
        'quit-dialog': 'bg:{}'.format(theme.inactive_bg),
        'quit-dialog-border': 'bg:{} {}'.format(
            theme.inactive_bg, theme.red_accent
        ),
        'command-runner': 'bg:{}'.format(theme.inactive_bg),
        'command-runner-title': 'bg:{} {}'.format(
            theme.inactive_bg, theme.default_fg
        ),
        'command-runner-setting': '{}'.format(theme.purple_accent),
        'command-runner-border': 'bg:{} {}'.format(
            theme.inactive_bg, theme.purple_accent
        ),
        'command-runner-selected-item': 'bg:{}'.format(theme.selected_line_bg),
        'command-runner-fuzzy-highlight-0': '{}'.format(theme.blue_accent),
        'command-runner-fuzzy-highlight-1': '{}'.format(theme.cyan_accent),
        'command-runner-fuzzy-highlight-2': '{}'.format(theme.green_accent),
        'command-runner-fuzzy-highlight-3': '{}'.format(theme.yellow_accent),
        'command-runner-fuzzy-highlight-4': '{}'.format(theme.orange_accent),
        'command-runner-fuzzy-highlight-5': '{}'.format(theme.red_accent),
        # Progress Bar Styles
        # Entire set of ProgressBars - no title is used in pw_console
        'title': '',
        # Actual bar title
        'label': 'bold',
        'percentage': '{}'.format(theme.green_accent),
        'bar': '{}'.format(theme.magenta_accent),
        # Filled part of the bar
        'bar-a': '{} bold'.format(theme.cyan_accent),
        # End of current progress
        'bar-b': '{} bold'.format(theme.purple_accent),
        # Empty part of the bar
        'bar-c': '',
        # current/total counts
        'current': '{}'.format(theme.cyan_accent),
        'total': '{}'.format(theme.cyan_accent),
        'time-elapsed': '{}'.format(theme.purple_accent),
        'time-left': '{}'.format(theme.magenta_accent),
        # Named theme color classes for use in user plugins.
        'theme-fg-red': '{}'.format(theme.red_accent),
        'theme-fg-orange': '{}'.format(theme.orange_accent),
        'theme-fg-yellow': '{}'.format(theme.yellow_accent),
        'theme-fg-green': '{}'.format(theme.green_accent),
        'theme-fg-cyan': '{}'.format(theme.cyan_accent),
        'theme-fg-blue': '{}'.format(theme.blue_accent),
        'theme-fg-purple': '{}'.format(theme.purple_accent),
        'theme-fg-magenta': '{}'.format(theme.magenta_accent),
        'theme-bg-red': 'bg:{}'.format(theme.red_accent),
        'theme-bg-orange': 'bg:{}'.format(theme.orange_accent),
        'theme-bg-yellow': 'bg:{}'.format(theme.yellow_accent),
        'theme-bg-green': 'bg:{}'.format(theme.green_accent),
        'theme-bg-cyan': 'bg:{}'.format(theme.cyan_accent),
        'theme-bg-blue': 'bg:{}'.format(theme.blue_accent),
        'theme-bg-purple': 'bg:{}'.format(theme.purple_accent),
        'theme-bg-magenta': 'bg:{}'.format(theme.magenta_accent),
        'theme-bg-active': 'bg:{}'.format(theme.active_bg),
        'theme-fg-active': '{}'.format(theme.active_fg),
        'theme-bg-inactive': 'bg:{}'.format(theme.inactive_bg),
        'theme-fg-inactive': '{}'.format(theme.inactive_fg),
        'theme-fg-default': '{}'.format(theme.default_fg),
        'theme-bg-default': 'bg:{}'.format(theme.default_bg),
        'theme-fg-dim': '{}'.format(theme.dim_fg),
        'theme-bg-dim': 'bg:{}'.format(theme.dim_bg),
        'theme-bg-dialog': 'bg:{}'.format(theme.dialog_bg),
        'theme-bg-line-highlight': 'bg:{}'.format(theme.line_highlight_bg),
        'theme-bg-button-active': 'bg:{}'.format(theme.button_active_bg),
        'theme-bg-button-inactive': 'bg:{}'.format(theme.button_inactive_bg),
    }

    return Style.from_dict(pw_console_styles)


def get_toolbar_style(pt_container, dim=False) -> str:
    """Return the style class for a toolbar if pt_container is in focus."""
    if has_focus(pt_container.__pt_container__())():
        return 'class:toolbar_dim_active' if dim else 'class:toolbar_active'
    return 'class:toolbar_dim_inactive' if dim else 'class:toolbar_inactive'


def get_button_style(pt_container) -> str:
    """Return the style class for a toolbar if pt_container is in focus."""
    if has_focus(pt_container.__pt_container__())():
        return 'class:toolbar-button-active'
    return 'class:toolbar-button-inactive'


def get_pane_style(pt_container) -> str:
    """Return the style class for a pane title if pt_container is in focus."""
    if has_focus(pt_container.__pt_container__())():
        return 'class:pane_active'
    return 'class:pane_inactive'


def get_pane_indicator(
    pt_container, title, mouse_handler=None, hide_indicator=False
) -> StyleAndTextTuples:
    """Return formatted text for a pane indicator and title."""

    inactive_indicator: OneStyleAndTextTuple
    active_indicator: OneStyleAndTextTuple
    inactive_title: OneStyleAndTextTuple
    active_title: OneStyleAndTextTuple

    if mouse_handler:
        inactive_indicator = (
            'class:pane_indicator_inactive',
            ' ',
            mouse_handler,
        )
        active_indicator = ('class:pane_indicator_active', ' ', mouse_handler)
        inactive_title = ('class:pane_title_inactive', title, mouse_handler)
        active_title = ('class:pane_title_active', title, mouse_handler)
    else:
        inactive_indicator = ('class:pane_indicator_inactive', ' ')
        active_indicator = ('class:pane_indicator_active', ' ')
        inactive_title = ('class:pane_title_inactive', title)
        active_title = ('class:pane_title_active', title)

    fragments: StyleAndTextTuples = []
    if has_focus(pt_container.__pt_container__())():
        if not hide_indicator:
            fragments.append(active_indicator)
        fragments.append(active_title)
    else:
        if not hide_indicator:
            fragments.append(inactive_indicator)
        fragments.append(inactive_title)
    return fragments
