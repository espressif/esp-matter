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
"""Brighter PigweedCode pygments style."""

import re

from prompt_toolkit.styles.style_transformation import get_opposite_color

from pygments.style import Style  # type: ignore
from pygments.token import Token  # type: ignore

_style_list = {
    Token.Comment: '#778899',  # Lighter comments
    Token.Comment.Hashbang: '#778899',
    Token.Comment.Multiline: '#778899',
    Token.Comment.Preproc: '#ff79c6',
    Token.Comment.PreprocFile: '',
    Token.Comment.Single: '#778899',
    Token.Comment.Special: '#778899',
    Token.Error: '#f8f8f2',
    Token.Escape: '',
    Token.Generic.Deleted: '#8b080b',
    Token.Generic.Emph: '#f8f8f2',
    Token.Generic.Error: '#f8f8f2',
    Token.Generic.Heading: '#f8f8f2 bold',
    Token.Generic.Inserted: '#f8f8f2 bold',
    Token.Generic.Output: '#f8f8f2',
    Token.Generic.Prompt: '#bfbfbf',  # Darker Prompt
    Token.Generic.Strong: '#f8f8f2',
    Token.Generic.Subheading: '#f8f8f2 bold',
    Token.Generic.Traceback: '#f8f8f2',
    Token.Generic: '#f8f8f2',
    Token.Keyword.Constant: '#ff79c6',
    Token.Keyword.Declaration: '#8be9fd',
    Token.Keyword.Namespace: '#ff79c6',
    Token.Keyword.Pseudo: '#ff79c6',
    Token.Keyword.Reserved: '#ff79c6',
    Token.Keyword.Type: '#8be9fd',
    Token.Keyword: '#ff79c6',
    Token.Literal.Date: '#f8f8f2',
    Token.Literal.Number.Bin: '#bd93f9',
    Token.Literal.Number.Float: '#bd93f9',
    Token.Literal.Number.Hex: '#bd93f9',
    Token.Literal.Number.Integer.Long: '#bd93f9',
    Token.Literal.Number.Integer: '#bd93f9',
    Token.Literal.Number.Oct: '#bd93f9',
    Token.Literal.Number: '#bd93f9',
    Token.Literal.String.Affix: '',
    Token.Literal.String.Backtick: '#f1fa8c',
    Token.Literal.String.Char: '#f1fa8c',
    Token.Literal.String.Delimiter: '',
    Token.Literal.String.Doc: '#f1fa8c',
    Token.Literal.String.Double: '#f1fa8c',
    Token.Literal.String.Escape: '#f1fa8c',
    Token.Literal.String.Heredoc: '#f1fa8c',
    Token.Literal.String.Interpol: '#f1fa8c',
    Token.Literal.String.Other: '#f1fa8c',
    Token.Literal.String.Regex: '#f1fa8c',
    Token.Literal.String.Single: '#f1fa8c',
    Token.Literal.String.Symbol: '#f1fa8c',
    Token.Literal.String: '#f1fa8c',
    Token.Literal: '#f8f8f2',
    Token.Name.Attribute: '#50fa7b',
    Token.Name.Builtin: '#8be9fd',
    Token.Name.Builtin.Pseudo: '#f8f8f2',
    Token.Name.Class: '#50fa7b',
    Token.Name.Constant: '#f8f8f2',
    Token.Name.Decorator: '#f8f8f2',
    Token.Name.Entity: '#f8f8f2',
    Token.Name.Exception: '#f8f8f2',
    Token.Name.Function.Magic: '',
    Token.Name.Function: '#50fa7b',
    Token.Name.Label: '#8be9fd',
    Token.Name.Namespace: '#f8f8f2',
    Token.Name.Other: '#f8f8f2',
    Token.Name.Property: '',
    Token.Name.Tag: '#ff79c6',
    Token.Name.Variable: '#8be9fd',
    Token.Name.Variable.Class: '#8be9fd',
    Token.Name.Variable.Global: '#8be9fd',
    Token.Name.Variable.Instance: '#8be9fd',
    Token.Name.Variable.Magic: '',
    Token.Name: '#f8f8f2',
    Token.Operator.Word: '#ff79c6',
    Token.Operator: '#ff79c6',
    Token.Other: '#f8f8f2',
    Token.Punctuation: '#f8f8f2',
    Token.Text.Whitespace: '#f8f8f2',
    Token.Text: '#f8f8f2',
    Token: '',
}

_COLOR_REGEX = re.compile(r'#(?P<hex>[0-9a-fA-F]{6}) *(?P<rest>.*?)$')


def swap_light_dark(color: str) -> str:
    if not color:
        return color
    match = _COLOR_REGEX.search(color)
    if not match:
        return color
    match_groups = match.groupdict()
    opposite_color = get_opposite_color(match_groups['hex'])
    if not opposite_color:
        return color
    rest = match_groups.get('rest', '')
    return '#' + ' '.join([opposite_color, rest])


class PigweedCodeStyle(Style):

    background_color = '#2e2e2e'
    default_style = ''

    styles = _style_list


class PigweedCodeLightStyle(Style):

    background_color = '#f8f8f8'
    default_style = ''

    styles = {key: swap_light_dark(value) for key, value in _style_list.items()}
