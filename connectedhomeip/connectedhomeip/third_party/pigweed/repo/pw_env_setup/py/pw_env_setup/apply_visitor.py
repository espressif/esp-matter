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
"""Applies an Environment to the current process."""

import os

# Disable super() warnings since this file must be Python 2 compatible.
# pylint: disable=super-with-arguments


class ApplyVisitor(object):  # pylint: disable=useless-object-inheritance
    """Applies an Environment to the current process."""

    def __init__(self, *args, **kwargs):
        pathsep = kwargs.pop('pathsep', os.pathsep)
        super(ApplyVisitor, self).__init__(*args, **kwargs)
        self._pathsep = pathsep
        self._environ = None
        self._unapply_steps = None

    def apply(self, env, environ):
        self._unapply_steps = []
        try:
            self._environ = environ
            env.accept(self)
        finally:
            self._environ = None

    def visit_set(self, set):  # pylint: disable=redefined-builtin
        self._environ[set.name] = set.value

    def visit_clear(self, clear):
        if clear.name in self._environ:
            del self._environ[clear.name]

    def visit_remove(self, remove):
        values = self._environ.get(remove.name, '').split(self._pathsep)
        norm = os.path.normpath
        values = [x for x in values if norm(x) != norm(remove.value)]
        self._environ[remove.name] = self._pathsep.join(values)

    def visit_prepend(self, prepend):
        self._environ[prepend.name] = self._pathsep.join(
            (prepend.value, self._environ.get(prepend.name, ''))
        )

    def visit_append(self, append):
        self._environ[append.name] = self._pathsep.join(
            (self._environ.get(append.name, ''), append.value)
        )

    def visit_echo(self, echo):
        pass  # Not relevant for apply.

    def visit_comment(self, comment):
        pass  # Not relevant for apply.

    def visit_command(self, command):
        pass  # Not relevant for apply.

    def visit_doctor(self, doctor):
        pass  # Not relevant for apply.

    def visit_blank_line(self, blank_line):
        pass  # Not relevant for apply.

    def visit_function(self, function):
        pass  # Not relevant for apply.

    def visit_hash(self, hash):  # pylint: disable=redefined-builtin
        pass  # Not relevant for apply.
