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
"""Code for improving interactive use of Python functions."""

import inspect
import textwrap
from typing import Callable


def _annotation_name(annotation: object) -> str:
    if isinstance(annotation, str):
        return annotation

    return getattr(annotation, '__name__', repr(annotation))


def format_parameter(param: inspect.Parameter) -> str:
    """Formats a parameter for printing in a function signature."""
    if param.kind == param.VAR_POSITIONAL:
        name = '*' + param.name
    elif param.kind == param.VAR_KEYWORD:
        name = '**' + param.name
    else:
        name = param.name

    if param.default is param.empty:
        default = ''
    else:
        default = f' = {param.default}'

    if param.annotation is param.empty:
        annotation = ''
    else:
        annotation = f': {_annotation_name(param.annotation)}'

    return f'{name}{annotation}{default}'


def format_signature(name: str, signature: inspect.Signature) -> str:
    """Formats a function signature as if it were source code.

    Does not yet handle / and * markers.
    """
    params = ', '.join(
        format_parameter(arg) for arg in signature.parameters.values()
    )
    if signature.return_annotation is signature.empty:
        return_annotation = ''
    else:
        return_annotation = ' -> ' + _annotation_name(
            signature.return_annotation
        )

    return f'{name}({params}){return_annotation}'


def format_function_help(function: Callable) -> str:
    """Formats a help string with a declaration and docstring."""
    signature = format_signature(
        function.__name__, inspect.signature(function, follow_wrapped=False)
    )

    docs = inspect.getdoc(function) or '(no docstring)'
    return f'{signature}:\n\n{textwrap.indent(docs, "    ")}'


def help_as_repr(function: Callable) -> Callable:
    """Wraps a function so that its repr() and docstring provide detailed help.

    This is useful for creating commands in an interactive console. In a
    console, typing a function's name and hitting Enter shows rich documentation
    with the full function signature, type annotations, and docstring when the
    function is wrapped with help_as_repr.
    """

    def display_help(_):
        return format_function_help(function)

    return type(
        function.__name__,
        (),
        dict(
            __call__=staticmethod(function),
            __doc__=format_function_help(function),
            __repr__=display_help,
        ),
    )()
