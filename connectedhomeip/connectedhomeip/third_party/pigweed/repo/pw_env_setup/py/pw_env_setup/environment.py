# Copyright 2020 The Pigweed Authors
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
"""Stores the environment changes necessary for Pigweed."""

import contextlib
import os
import re

# The order here is important. On Python 2 we want StringIO.StringIO and not
# io.StringIO. On Python 3 there is no StringIO module so we want io.StringIO.
# Not using six because six is not a standard package we can expect to have
# installed in the system Python.
try:
    from StringIO import StringIO  # type: ignore
except ImportError:
    from io import StringIO

from . import apply_visitor
from . import batch_visitor
from . import gni_visitor
from . import json_visitor
from . import shell_visitor

# Disable super() warnings since this file must be Python 2 compatible.
# pylint: disable=super-with-arguments


class BadNameType(TypeError):
    pass


class BadValueType(TypeError):
    pass


class EmptyValue(ValueError):
    pass


class NewlineInValue(TypeError):
    pass


class BadVariableName(ValueError):
    pass


class UnexpectedAction(ValueError):
    pass


class AcceptNotOverridden(TypeError):
    pass


class _Action(object):  # pylint: disable=useless-object-inheritance
    def unapply(self, env, orig_env):
        pass

    def accept(self, visitor):
        del visitor
        raise AcceptNotOverridden(
            'accept() not overridden for {}'.format(self.__class__.__name__)
        )

    def write_deactivate(
        self, outs, windows=(os.name == 'nt'), replacements=()
    ):
        pass


class _VariableAction(_Action):
    # pylint: disable=keyword-arg-before-vararg
    def __init__(self, name, value, allow_empty_values=False, *args, **kwargs):
        super(_VariableAction, self).__init__(*args, **kwargs)
        self.name = name
        self.value = value
        self.allow_empty_values = allow_empty_values

        self._check()

    def _check(self):
        try:
            # In python2, unicode is a distinct type.
            valid_types = (str, unicode)
        except NameError:
            valid_types = (str,)

        if not isinstance(self.name, valid_types):
            raise BadNameType(
                'variable name {!r} not of type str'.format(self.name)
            )
        if not isinstance(self.value, valid_types):
            raise BadValueType(
                '{!r} value {!r} not of type str'.format(self.name, self.value)
            )

        # Empty strings as environment variable values have different behavior
        # on different operating systems. Just don't allow them.
        if not self.allow_empty_values and self.value == '':
            raise EmptyValue(
                '{!r} value {!r} is the empty string'.format(
                    self.name, self.value
                )
            )

        # Many tools have issues with newlines in environment variable values.
        # Just don't allow them.
        if '\n' in self.value:
            raise NewlineInValue(
                '{!r} value {!r} contains a newline'.format(
                    self.name, self.value
                )
            )

        if not re.match(r'^[A-Z_][A-Z0-9_]*$', self.name, re.IGNORECASE):
            raise BadVariableName('bad variable name {!r}'.format(self.name))

    def unapply(self, env, orig_env):
        if self.name in orig_env:
            env[self.name] = orig_env[self.name]
        else:
            env.pop(self.name, None)

    def __repr__(self):
        return '{}({}, {})'.format(
            self.__class__.__name__, self.name, self.value
        )


class Set(_VariableAction):
    """Set a variable."""

    def __init__(self, *args, **kwargs):
        deactivate = kwargs.pop('deactivate', True)
        super(Set, self).__init__(*args, **kwargs)
        self.deactivate = deactivate

    def accept(self, visitor):
        visitor.visit_set(self)


class Clear(_VariableAction):
    """Remove a variable from the environment."""

    def __init__(self, *args, **kwargs):
        kwargs['value'] = ''
        kwargs['allow_empty_values'] = True
        super(Clear, self).__init__(*args, **kwargs)

    def accept(self, visitor):
        visitor.visit_clear(self)


class Remove(_VariableAction):
    """Remove a value from a PATH-like variable."""

    def accept(self, visitor):
        visitor.visit_remove(self)


class BadVariableValue(ValueError):
    pass


def _append_prepend_check(action):
    if '=' in action.value:
        raise BadVariableValue('"{}" contains "="'.format(action.value))


class Prepend(_VariableAction):
    """Prepend a value to a PATH-like variable."""

    def __init__(self, name, value, join, *args, **kwargs):
        super(Prepend, self).__init__(name, value, *args, **kwargs)
        self._join = join

    def _check(self):
        super(Prepend, self)._check()
        _append_prepend_check(self)

    def accept(self, visitor):
        visitor.visit_prepend(self)


class Append(_VariableAction):
    """Append a value to a PATH-like variable. (Uncommon, see Prepend.)"""

    def __init__(self, name, value, join, *args, **kwargs):
        super(Append, self).__init__(name, value, *args, **kwargs)
        self._join = join

    def _check(self):
        super(Append, self)._check()
        _append_prepend_check(self)

    def accept(self, visitor):
        visitor.visit_append(self)


class BadEchoValue(ValueError):
    pass


class Echo(_Action):
    """Echo a value to the terminal."""

    def __init__(self, value, newline, *args, **kwargs):
        # These values act funny on Windows.
        if value.lower() in ('off', 'on'):
            raise BadEchoValue(value)
        super(Echo, self).__init__(*args, **kwargs)
        self.value = value
        self.newline = newline

    def accept(self, visitor):
        visitor.visit_echo(self)

    def __repr__(self):
        return 'Echo({}, newline={})'.format(self.value, self.newline)


class Comment(_Action):
    """Add a comment to the init script."""

    def __init__(self, value, *args, **kwargs):
        super(Comment, self).__init__(*args, **kwargs)
        self.value = value

    def accept(self, visitor):
        visitor.visit_comment(self)

    def __repr__(self):
        return 'Comment({})'.format(self.value)


class Command(_Action):
    """Run a command."""

    def __init__(self, command, *args, **kwargs):
        exit_on_error = kwargs.pop('exit_on_error', True)
        super(Command, self).__init__(*args, **kwargs)
        assert isinstance(command, (list, tuple))
        self.command = command
        self.exit_on_error = exit_on_error

    def accept(self, visitor):
        visitor.visit_command(self)

    def __repr__(self):
        return 'Command({})'.format(self.command)


class Doctor(Command):
    def __init__(self, *args, **kwargs):
        log_level = 'warn' if 'PW_ENVSETUP_QUIET' in os.environ else 'info'
        cmd = ['pw', '--no-banner', '--loglevel', log_level, 'doctor']
        super(Doctor, self).__init__(command=cmd, *args, **kwargs)

    def accept(self, visitor):
        visitor.visit_doctor(self)

    def __repr__(self):
        return 'Doctor()'


class BlankLine(_Action):
    """Write a blank line to the init script."""

    def accept(self, visitor):
        visitor.visit_blank_line(self)

    def __repr__(self):
        return 'BlankLine()'


class Function(_Action):
    def __init__(self, name, body, *args, **kwargs):
        super(Function, self).__init__(*args, **kwargs)
        self.name = name
        self.body = body

    def accept(self, visitor):
        visitor.visit_function(self)

    def __repr__(self):
        return 'Function({}, {})'.format(self.name, self.body)


class Hash(_Action):
    def accept(self, visitor):
        visitor.visit_hash(self)

    def __repr__(self):
        return 'Hash()'


class Join(object):  # pylint: disable=useless-object-inheritance
    def __init__(self, pathsep=os.pathsep):
        self.pathsep = pathsep


# TODO(mohrr) remove disable=useless-object-inheritance once in Python 3.
# pylint: disable=useless-object-inheritance
class Environment(object):
    """Stores the environment changes necessary for Pigweed.

    These changes can be accessed by writing them to a file for bash-like
    shells to source or by using this as a context manager.
    """

    def __init__(self, *args, **kwargs):
        pathsep = kwargs.pop('pathsep', os.pathsep)
        windows = kwargs.pop('windows', os.name == 'nt')
        allcaps = kwargs.pop('allcaps', windows)
        super(Environment, self).__init__(*args, **kwargs)
        self._actions = []
        self._pathsep = pathsep
        self._windows = windows
        self._allcaps = allcaps
        self.replacements = []
        self._join = Join(pathsep)
        self._finalized = False

    def add_replacement(self, variable, value=None):
        self.replacements.append((variable, value))

    def normalize_key(self, name):
        if self._allcaps:
            try:
                return name.upper()
            except AttributeError:
                # The _Action class has code to handle incorrect types, so
                # we just ignore this error here.
                pass
        return name

    # A newline is printed after each high-level operation. Top-level
    # operations should not invoke each other (this is why _remove() exists).

    def set(self, name, value, deactivate=True):
        """Set a variable."""
        assert not self._finalized
        name = self.normalize_key(name)
        self._actions.append(Set(name, value, deactivate=deactivate))
        self._blankline()

    def clear(self, name):
        """Remove a variable."""
        assert not self._finalized
        name = self.normalize_key(name)
        self._actions.append(Clear(name))
        self._blankline()

    def _remove(self, name, value):
        """Remove a value from a variable."""
        assert not self._finalized
        name = self.normalize_key(name)
        if self.get(name, None):
            self._actions.append(Remove(name, value, self._pathsep))

    def remove(self, name, value):
        """Remove a value from a PATH-like variable."""
        assert not self._finalized
        self._remove(name, value)
        self._blankline()

    def append(self, name, value):
        """Add a value to a PATH-like variable. Rarely used, see prepend()."""
        assert not self._finalized
        name = self.normalize_key(name)
        if self.get(name, None):
            self._remove(name, value)
            self._actions.append(Append(name, value, self._join))
        else:
            self._actions.append(Set(name, value))
        self._blankline()

    def prepend(self, name, value):
        """Add a value to the beginning of a PATH-like variable."""
        assert not self._finalized
        name = self.normalize_key(name)
        if self.get(name, None):
            self._remove(name, value)
            self._actions.append(Prepend(name, value, self._join))
        else:
            self._actions.append(Set(name, value))
        self._blankline()

    def echo(self, value='', newline=True):
        """Echo a value to the terminal."""
        # echo() deliberately ignores self._finalized.
        self._actions.append(Echo(value, newline))
        if value:
            self._blankline()

    def comment(self, comment):
        """Add a comment to the init script."""
        # comment() deliberately ignores self._finalized.
        self._actions.append(Comment(comment))
        self._blankline()

    def command(self, command, exit_on_error=True):
        """Run a command."""
        # command() deliberately ignores self._finalized.
        self._actions.append(Command(command, exit_on_error=exit_on_error))
        self._blankline()

    def doctor(self):
        """Run 'pw doctor'."""
        self._actions.append(Doctor())

    def function(self, name, body):
        """Define a function."""
        assert not self._finalized
        self._actions.append(Command(name, body))
        self._blankline()

    def _blankline(self):
        self._actions.append(BlankLine())

    def finalize(self):
        """Run cleanup at the end of environment setup."""
        assert not self._finalized
        self._finalized = True
        self._actions.append(Hash())
        self._blankline()

        if not self._windows:
            buf = StringIO()
            self.write_deactivate(buf)
            self._actions.append(Function('_pw_deactivate', buf.getvalue()))
            self._blankline()

    def accept(self, visitor):
        for action in self._actions:
            action.accept(visitor)

    def gni(self, outs, project_root):
        gni_visitor.GNIVisitor(project_root).serialize(self, outs)

    def json(self, outs):
        json_visitor.JSONVisitor().serialize(self, outs)

    def write(self, outs):
        if self._windows:
            visitor = batch_visitor.BatchVisitor(pathsep=self._pathsep)
        else:
            visitor = shell_visitor.ShellVisitor(pathsep=self._pathsep)
        visitor.serialize(self, outs)

    def write_deactivate(self, outs):
        if self._windows:
            return
        visitor = shell_visitor.DeactivateShellVisitor(pathsep=self._pathsep)
        visitor.serialize(self, outs)

    @contextlib.contextmanager
    def __call__(self, export=True):
        """Set environment as if this was written to a file and sourced.

        Within this context os.environ is updated with the environment
        defined by this object. If export is False, os.environ is not updated,
        but in both cases the updated environment is yielded.

        On exit, previous environment is restored. See contextlib documentation
        for details on how this function is structured.

        Args:
          export(bool): modify the environment of the running process (and
            thus, its subprocesses)

        Yields the new environment object.
        """
        try:
            if export:
                orig_env = os.environ.copy()
                env = os.environ
            else:
                env = os.environ.copy()

            apply = apply_visitor.ApplyVisitor(pathsep=self._pathsep)
            apply.apply(self, env)

            yield env

        finally:
            if export:
                for key in set(os.environ):
                    try:
                        os.environ[key] = orig_env[key]
                    except KeyError:
                        del os.environ[key]
                for key in set(orig_env) - set(os.environ):
                    os.environ[key] = orig_env[key]

    def get(self, key, default=None):
        """Get the value of a variable within context of this object."""
        key = self.normalize_key(key)
        with self(export=False) as env:
            return env.get(key, default)

    def __getitem__(self, key):
        """Get the value of a variable within context of this object."""
        key = self.normalize_key(key)
        with self(export=False) as env:
            return env[key]
