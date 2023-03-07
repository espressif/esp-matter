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
"""Serializes an Environment into a shell file."""

import inspect

# Disable super() warnings since this file must be Python 2 compatible.
# pylint: disable=super-with-arguments


class _BaseShellVisitor(object):  # pylint: disable=useless-object-inheritance
    def __init__(self, *args, **kwargs):
        pathsep = kwargs.pop('pathsep', ':')
        super(_BaseShellVisitor, self).__init__(*args, **kwargs)
        self._pathsep = pathsep
        self._outs = None

    def _remove_value_from_path(self, variable, value):
        return (
            '{variable}="$(echo "${variable}"'
            ' | sed "s|{pathsep}{value}{pathsep}|{pathsep}|g;"'
            ' | sed "s|^{value}{pathsep}||g;"'
            ' | sed "s|{pathsep}{value}$||g;"'
            ')"\nexport {variable}\n'.format(
                variable=variable, value=value, pathsep=self._pathsep
            )
        )

    def visit_hash(self, hash):  # pylint: disable=redefined-builtin
        del hash
        self._outs.write(
            inspect.cleandoc(
                '''
        # This should detect bash and zsh, which have a hash command that must
        # be called to get it to forget past commands. Without forgetting past
        # commands the $PATH changes we made may not be respected.
        if [ -n "${BASH:-}" -o -n "${ZSH_VERSION:-}" ] ; then
            hash -r\n
        fi
        '''
            )
        )


class ShellVisitor(_BaseShellVisitor):
    """Serializes an Environment into a shell file."""

    def __init__(self, *args, **kwargs):
        super(ShellVisitor, self).__init__(*args, **kwargs)
        self._replacements = ()

    def serialize(self, env, outs):
        try:
            self._replacements = tuple(
                (key, env.get(key) if value is None else value)
                for key, value in env.replacements
            )
            self._outs = outs

            env.accept(self)

        finally:
            self._replacements = ()
            self._outs = None

    def _apply_replacements(self, action):
        value = action.value
        for var, replacement in self._replacements:
            if var != action.name:
                value = value.replace(replacement, '${}'.format(var))
        return value

    def visit_set(self, set):  # pylint: disable=redefined-builtin
        value = self._apply_replacements(set)
        self._outs.write(
            '{name}="{value}"\nexport {name}\n'.format(
                name=set.name, value=value
            )
        )

    def visit_clear(self, clear):
        self._outs.write('unset {name}\n'.format(**vars(clear)))

    def visit_remove(self, remove):
        value = self._apply_replacements(remove)
        self._outs.write(
            '# Remove \n#   {value}\n# from\n#   {value}\n# '
            'before adding it back.\n'
        )
        self._outs.write(self._remove_value_from_path(remove.name, value))

    def _join(self, *args):
        if len(args) == 1 and isinstance(args[0], (list, tuple)):
            args = args[0]
        return self._pathsep.join(args)

    def visit_prepend(self, prepend):
        value = self._apply_replacements(prepend)
        value = self._join(value, '${}'.format(prepend.name))
        self._outs.write(
            '{name}="{value}"\nexport {name}\n'.format(
                name=prepend.name, value=value
            )
        )

    def visit_append(self, append):
        value = self._apply_replacements(append)
        value = self._join('${}'.format(append.name), value)
        self._outs.write(
            '{name}="{value}"\nexport {name}\n'.format(
                name=append.name, value=value
            )
        )

    def visit_echo(self, echo):
        # TODO(mohrr) use shlex.quote().
        self._outs.write('if [ -z "${PW_ENVSETUP_QUIET:-}" ]; then\n')
        if echo.newline:
            self._outs.write('  echo "{}"\n'.format(echo.value))
        else:
            self._outs.write('  echo -n "{}"\n'.format(echo.value))
        self._outs.write('fi\n')

    def visit_comment(self, comment):
        for line in comment.value.splitlines():
            self._outs.write('# {}\n'.format(line))

    def visit_command(self, command):
        # TODO(mohrr) use shlex.quote here?
        self._outs.write('{}\n'.format(' '.join(command.command)))
        if not command.exit_on_error:
            return

        # Assume failing command produced relevant output.
        self._outs.write('if [ "$?" -ne 0 ]; then\n  return 1\nfi\n')

    def visit_doctor(self, doctor):
        self._outs.write('if [ -z "$PW_ACTIVATE_SKIP_CHECKS" ]; then\n')
        self.visit_command(doctor)
        self._outs.write('else\n')
        self._outs.write(
            'echo Skipping environment check because '
            'PW_ACTIVATE_SKIP_CHECKS is set\n'
        )
        self._outs.write('fi\n')

    def visit_blank_line(self, blank_line):
        del blank_line
        self._outs.write('\n')

    def visit_function(self, function):
        self._outs.write(
            '{name}() {{\n{body}\n}}\n'.format(
                name=function.name, body=function.body
            )
        )


class DeactivateShellVisitor(_BaseShellVisitor):
    """Removes values from an Environment."""

    def __init__(self, *args, **kwargs):
        pathsep = kwargs.pop('pathsep', ':')
        super(DeactivateShellVisitor, self).__init__(*args, **kwargs)
        self._pathsep = pathsep

    def serialize(self, env, outs):
        try:
            self._outs = outs

            env.accept(self)

        finally:
            self._outs = None

    def visit_set(self, set):  # pylint: disable=redefined-builtin
        if set.deactivate:
            self._outs.write('unset {name}\n'.format(name=set.name))

    def visit_clear(self, clear):
        pass  # Not relevant.

    def visit_remove(self, remove):
        pass  # Not relevant.

    def visit_prepend(self, prepend):
        self._outs.write(
            self._remove_value_from_path(prepend.name, prepend.value)
        )

    def visit_append(self, append):
        self._outs.write(
            self._remove_value_from_path(append.name, append.value)
        )

    def visit_echo(self, echo):
        pass  # Not relevant.

    def visit_comment(self, comment):
        pass  # Not relevant.

    def visit_command(self, command):
        pass  # Not relevant.

    def visit_doctor(self, doctor):
        pass  # Not relevant.

    def visit_blank_line(self, blank_line):
        pass  # Not relevant.

    def visit_function(self, function):
        pass  # Not relevant.
