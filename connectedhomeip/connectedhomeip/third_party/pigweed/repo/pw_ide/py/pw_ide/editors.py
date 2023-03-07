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
"""Framework for configuring code editors for Pigweed projects.

Editors and IDEs vary in the way they're configured and the options they
provide for configuration. As long as an editor uses files we can parse to
store its settings, this framework can be used to provide a consistent
interface to managing those settings in the context of a Pigweed project.

Ideally, we want to provide three levels of editor settings for a project:

- User settings (specific to the user's checkout)
- Project settings (included in source control, consistent for all users)
- Default settings (defined by Pigweed)

... where the settings on top can override (or cascade over) settings defined
below.

Some editors already provide mechanisms for achieving this, but in ways that
are particular to that editor, and many other editors don't provide this
mechanism at all. So we provide it in a uniform way here by adding a fourth
settings level, active settings, which are the actual settings files the editor
uses. Active settings are *built* (rather than edited or cloned) by looking for
user, project, and default settings (which are defined by Pigweed and ignored
by the editor) and combining them in the order described above. In this way,
Pigweed can provide sensible defaults, projects can define additional settings
to provide a uniform development experience, and users have the freedom to make
their own changes.
"""

# TODO(chadnorvell): Import collections.OrderedDict when we don't need to
# support Python 3.8 anymore.
from collections import defaultdict
from contextlib import contextmanager
from dataclasses import dataclass
import enum
import json
from pathlib import Path
import time
from typing import (
    Any,
    Callable,
    Dict,
    Generator,
    Generic,
    Literal,
    Optional,
    OrderedDict,
    TypeVar,
)

import json5  # type: ignore

from pw_ide.settings import PigweedIdeSettings


class _StructuredFileFormat:
    """Base class for structured settings file formats."""

    @property
    def ext(self) -> str:
        return 'null'

    def load(self, *args, **kwargs) -> OrderedDict:
        raise ValueError(
            f'Cannot load from file with {self.__class__.__name__}!'
        )

    def dump(self, data: OrderedDict, *args, **kwargs) -> None:
        raise ValueError(f'Cannot dump to file with {self.__class__.__name__}!')


class JsonFileFormat(_StructuredFileFormat):
    """JSON file format."""

    @property
    def ext(self) -> str:
        return 'json'

    def load(self, *args, **kwargs) -> OrderedDict:
        """Load JSON into an ordered dict."""
        kwargs['object_pairs_hook'] = OrderedDict
        return json.load(*args, **kwargs)

    def dump(self, data: OrderedDict, *args, **kwargs) -> None:
        """Dump JSON in a readable format."""
        kwargs['indent'] = 2
        json.dump(data, *args, **kwargs)


class Json5FileFormat(_StructuredFileFormat):
    """JSON5 file format.

    Supports parsing files with comments and trailing commas.
    """

    @property
    def ext(self) -> str:
        return 'json'

    def load(self, *args, **kwargs) -> OrderedDict:
        """Load JSON into an ordered dict."""
        kwargs['object_pairs_hook'] = OrderedDict
        return json5.load(*args, **kwargs)

    def dump(self, data: OrderedDict, *args, **kwargs) -> None:
        """Dump JSON in a readable format."""
        kwargs['indent'] = 2
        kwargs['quote_keys'] = True
        json5.dump(data, *args, **kwargs)


# Allows constraining to dicts and dict subclasses, while also constraining to
# the *same* dict subclass.
_TDictLike = TypeVar('_TDictLike', bound=Dict)


def dict_deep_merge(
    src: _TDictLike,
    dest: _TDictLike,
    ctor: Optional[Callable[[], _TDictLike]] = None,
) -> _TDictLike:
    """Deep merge dict-like `src` into dict-like `dest`.

    `dest` is mutated in place and also returned.

    `src` and `dest` need to be the same subclass of dict. If they're anything
    other than basic dicts, you need to also provide a constructor that returns
    an empty dict of the same subclass.
    """
    # Ensure that src and dest are the same type of dict.
    # These kinds of direct class comparisons are un-Pythonic, but the invariant
    # here really is that they be exactly the same class, rather than "same" in
    # the polymorphic sense.
    if dest.__class__ != src.__class__:
        raise TypeError(
            'Cannot merge dicts of different subclasses!\n'
            f'src={src.__class__.__name__}, '
            f'dest={dest.__class__.__name__}'
        )

    # If a constructor for this subclass wasn't provided, try using a
    # zero-arg constructor for the provided dicts.
    if ctor is None:
        ctor = lambda: src.__class__()  # pylint: disable=unnecessary-lambda

    # Ensure that we have a way to construct an empty dict of the same type.
    try:
        empty_dict = ctor()
    except TypeError:
        # The constructor has required arguments.
        raise TypeError(
            'When merging a dict subclass, you must provide a '
            'constructor for the subclass that produces an empty '
            'dict.\n'
            f'src/dest={src.__class__.__name__}'
        )

    if empty_dict.__class__ != src.__class__:
        # The constructor returns something of the wrong type.
        raise TypeError(
            'When merging a dict subclass, you must provide a '
            'constructor for the subclass that produces an empty '
            'dict.\n'
            f'src/dest={src.__class__.__name__}, '
            f'constructor={ctor().__class__.__name__}'
        )

    for key, value in src.items():
        empty_dict = ctor()
        # The value is a nested dict; recursively merge.
        if isinstance(value, src.__class__):
            node = dest.setdefault(key, empty_dict)
            dict_deep_merge(value, node, ctor)
        # The value is something else; copy it over.
        # TODO(chadnorvell): This doesn't deep merge other data structures, e.g.
        # lists, lists of dicts, dicts of lists, etc.
        else:
            dest[key] = value

    return dest


# Editor settings are manipulated via this dict-like data structure. We use
# OrderedDict to avoid non-deterministic changes to settings files and to make
# diffs more readable. Note that the values here can't really be "Any". They
# need to be JSON serializable, and any embedded dicts should also be
# OrderedDicts.
EditorSettingsDict = OrderedDict[str, Any]

# A callback that provides default settings in dict form when given ``pw_ide``
# settings (which may be ignored in many cases).
DefaultSettingsCallback = Callable[[PigweedIdeSettings], EditorSettingsDict]


class EditorSettingsDefinition:
    """Provides access to a particular group of editor settings.

    A particular editor may have one or more settings *types* (e.g., editor
    settings vs. automated tasks settings, or separate settings files for
    each supported language). ``pw_ide`` also supports multiple settings
    *levels*, where the "active" settings are built from default, project,
    and user settings. Each combination of settings type and level will have
    one ``EditorSettingsDefinition``, which may be in memory (e.g., for default
    settings defined in code) or may be backed by a file (see
    ``EditorSettingsFile``).

    Settings are accessed using the ``modify`` context manager, which provides
    you a dict-like data structure to manipulate.

    Initial settings can be provided in the constructor via a callback that
    takes an instance of ``PigweedIdeSettings`` and returns a settings dict.
    This allows the initial settings to be dependent on overall IDE features
    settings.
    """

    def __init__(
        self,
        pw_ide_settings: Optional[PigweedIdeSettings] = None,
        data: Optional[DefaultSettingsCallback] = None,
    ):
        self._data: EditorSettingsDict = OrderedDict()

        if data is not None and pw_ide_settings is not None:
            self._data = data(pw_ide_settings)

    def __repr__(self) -> str:
        return f'<{self.__class__.__name__}: (in memory)>'

    def get(self) -> EditorSettingsDict:
        """Return the settings as an ordered dict."""
        return self._data

    @contextmanager
    def modify(self, reinit: bool = False):
        """Modify a settings file via an ordered dict."""
        if reinit:
            new_data: OrderedDict[str, Any] = OrderedDict()
            yield new_data
            self._data = new_data
        else:
            yield self._data

    def sync_to(self, settings: EditorSettingsDict) -> None:
        """Merge this set of settings on top of the provided settings."""
        self_settings = self.get()
        settings = dict_deep_merge(self_settings, settings)

    def is_present(self) -> bool:  # pylint: disable=no-self-use
        return True

    def delete(self) -> None:
        pass

    def delete_backups(self) -> None:
        pass


class EditorSettingsFile(EditorSettingsDefinition):
    """Provides access to an editor settings defintion stored in a file.

    It's assumed that the editor's settings are stored in a file format that
    can be deserialized to Python dicts. The settings are represented by
    an ordered dict to make the diff that results from modifying the settings
    as easy to read as possible (assuming it has a plain text representation).

    This represents the concept of a file; the file may not actually be
    present on disk yet.
    """

    def __init__(
        self, settings_dir: Path, name: str, file_format: _StructuredFileFormat
    ) -> None:
        self._name = name
        self._format = file_format
        self._path = settings_dir / f'{name}.{self._format.ext}'
        super().__init__()

    def __repr__(self) -> str:
        return f'<{self.__class__.__name__}: {str(self._path)}>'

    def _backup_filename(self, glob=False):
        timestamp = time.strftime('%Y%m%d_%H%M%S')
        timestamp = '*' if glob else timestamp
        backup_str = f'.{timestamp}.bak'
        return f'{self._name}{backup_str}.{self._format.ext}'

    def _make_backup(self) -> Path:
        return self._path.replace(self._path.with_name(self._backup_filename()))

    def _restore_backup(self, backup: Path) -> Path:
        return backup.replace(self._path)

    def get(self) -> EditorSettingsDict:
        """Read a settings file into an ordered dict.

        This does not keep the file context open, so while the dict is
        mutable, any changes will not be written to disk.
        """
        try:
            with self._path.open() as file:
                settings: OrderedDict = self._format.load(file)
        except FileNotFoundError:
            settings = OrderedDict()

        return settings

    @contextmanager
    def modify(self, reinit: bool = False):
        """Modify a settings file via an ordered dict.

        Get the dict when entering the context, then modify it like any
        other dict, with the caveat that whatever goes into it needs to be
        JSON-serializable. Example:

        .. code-block:: python

            with settings_file.modify() as settings:
                settings[foo] = bar

        After modifying the settings and leaving this context, the file will
        be written. If the file already exists, a backup will be made. If a
        failure occurs while writing the new file, it will be deleted and the
        backup will be restored.

        If the ``reinit`` argument is set, a new, empty file will be created
        instead of modifying any existing file. If there is an existing file,
        it will still be backed up.
        """
        if self._path.exists():
            should_load_existing = True
            should_backup = True
        else:
            should_load_existing = False
            should_backup = False

        if reinit:
            should_load_existing = False

        if should_load_existing:
            with self._path.open() as file:
                settings: OrderedDict = self._format.load(file)
        else:
            settings = OrderedDict()

        prev_settings = settings.copy()

        # TODO(chadnorvell): There's a subtle bug here where you can't assign
        # to this var and have it take effect. You have to modify it in place.
        # But you won't notice until things don't get written to disk.
        yield settings

        # If the settings haven't changed, don't create a backup.
        if should_load_existing:
            if settings == prev_settings:
                should_backup = False

        if should_backup:
            # Move the current file to a new backup file. This frees the main
            # file for open('x').
            backup = self._make_backup()
        else:
            backup = None
            # If the file exists and we didn't move it to a backup file, delete
            # it so we can open('x') it again.
            if self._path.exists():
                self._path.unlink()

        file = self._path.open('x')

        try:
            self._format.dump(settings, file)
        except TypeError:
            # We'll get this error if we try to sneak something in that's
            # not JSON-serializable. Unless we handle this, we'll end up
            # with a partially-written file that can't be parsed. So we
            # delete that and restore the backup.
            file.close()
            self._path.unlink()

            if backup is not None:
                self._restore_backup(backup)

            raise
        finally:
            if not file.closed:
                file.close()

    def is_present(self) -> bool:
        return self._path.exists()

    def delete(self) -> None:
        try:
            self._path.unlink()
        except FileNotFoundError:
            pass

    def delete_backups(self) -> None:
        glob = self._backup_filename(glob=True)

        for path in self._path.glob(glob):
            path.unlink()


_SettingsLevelName = Literal['default', 'active', 'project', 'user']


@dataclass(frozen=True)
class SettingsLevelData:
    name: _SettingsLevelName
    is_user_configurable: bool
    is_file: bool


class SettingsLevel(enum.Enum):
    """Cascading set of settings.

    This provides a unified mechanism for having active settings (those
    actually used by an editor) be built from default settings in Pigweed,
    project settings checked into the project's repository, and user settings
    particular to one checkout of the project, each of which can override
    settings higher up in the chain.
    """

    DEFAULT = SettingsLevelData(
        'default', is_user_configurable=False, is_file=False
    )
    PROJECT = SettingsLevelData(
        'project', is_user_configurable=True, is_file=True
    )
    USER = SettingsLevelData('user', is_user_configurable=True, is_file=True)
    ACTIVE = SettingsLevelData(
        'active', is_user_configurable=False, is_file=True
    )

    @property
    def is_user_configurable(self) -> bool:
        return self.value.is_user_configurable

    @property
    def is_file(self) -> bool:
        return self.value.is_file

    @classmethod
    def all_levels(cls) -> Generator['SettingsLevel', None, None]:
        return (level for level in cls)

    @classmethod
    def all_not_default(cls) -> Generator['SettingsLevel', None, None]:
        return (level for level in cls if level is not cls.DEFAULT)

    @classmethod
    def all_user_configurable(cls) -> Generator['SettingsLevel', None, None]:
        return (level for level in cls if level.is_user_configurable)

    @classmethod
    def all_files(cls) -> Generator['SettingsLevel', None, None]:
        return (level for level in cls if level.is_file)


# A map of configurable settings levels and the string that will be prepended
# to their files to indicate their settings level.
SettingsFilePrefixes = Dict[SettingsLevel, str]

# Each editor will have one or more settings types that typically reflect each
# of the files used to define their settings. So each editor should have an
# enum type that defines each of those settings types, and this type var
# represents that generically. The value of each enum case should be the file
# name of that settings file, without the extension.
# TODO(chadnorvell): Would be great to constrain this to enums, but bound=
# doesn't do what we want with Enum or EnumMeta.
_TSettingsType = TypeVar('_TSettingsType')

# Maps each settings type with the callback that generates the default settings
# for that settings type.
EditorSettingsTypesWithDefaults = Dict[_TSettingsType, DefaultSettingsCallback]


class EditorSettingsManager(Generic[_TSettingsType]):
    """Manages all settings for a particular editor.

    This is where you interact with an editor's settings (actually in a
    subclass of this class, not here). Initializing this class sets up access
    to one or more settings files for an editor (determined by
    ``_TSettingsType``, fulfilled by an enum that defines each of an editor's
    settings files), along with the cascading settings levels.
    """

    # Prefixes should only be defined for settings that will be stored on disk
    # and are not the active settings file, which will use the name without a
    # prefix. This may be overridden in child classes, but typically should
    # not be.
    prefixes: SettingsFilePrefixes = {
        SettingsLevel.PROJECT: 'pw_project_',
        SettingsLevel.USER: 'pw_user_',
    }

    # These must be overridden in child classes.
    default_settings_dir: Path = None  # type: ignore
    file_format: _StructuredFileFormat = _StructuredFileFormat()
    types_with_defaults: EditorSettingsTypesWithDefaults[_TSettingsType] = {}

    def __init__(
        self,
        pw_ide_settings: PigweedIdeSettings,
        settings_dir: Optional[Path] = None,
        file_format: Optional[_StructuredFileFormat] = None,
        types_with_defaults: Optional[
            EditorSettingsTypesWithDefaults[_TSettingsType]
        ] = None,
    ):
        if SettingsLevel.ACTIVE in self.__class__.prefixes:
            raise ValueError(
                'You cannot assign a file name prefix to '
                'an active settings file.'
            )

        # This lets us use ``self._prefixes`` transparently for any file,
        # including active settings files, since it will provide an empty
        # string prefix for those files. In other words, while the class
        # attribute `prefixes` can only be defined for configurable settings,
        # `self._prefixes` extends it to work for any settings file.
        self._prefixes = defaultdict(str, self.__class__.prefixes)

        # The default settings directory is defined by the subclass attribute
        # `default_settings_dir`, and that value is used the vast majority of
        # the time. But you can inject an alternative directory in the
        # constructor if needed (e.g. for tests).
        self._settings_dir = (
            settings_dir
            if settings_dir is not None
            else self.__class__.default_settings_dir
        )

        # The backing file format should normally be defined by the class
        # attribute ``file_format``, but can be overridden in the constructor.
        self._file_format: _StructuredFileFormat = (
            file_format
            if file_format is not None
            else self.__class__.file_format
        )

        # The settings types with their defaults should normally be defined by
        # the class attribute ``types_with_defaults``, but can be overridden
        # in the constructor.
        self._types_with_defaults = (
            types_with_defaults
            if types_with_defaults is not None
            else self.__class__.types_with_defaults
        )

        # For each of the settings levels, there is a settings definition for
        # each settings type. Those settings definitions may be stored in files
        # or not.
        self._settings_definitions: Dict[
            SettingsLevel, Dict[_TSettingsType, EditorSettingsDefinition]
        ] = {}

        self._settings_types = tuple(self._types_with_defaults.keys())

        # Initialize the default settings level for each settings type, which
        # defined in code, not files.
        self._settings_definitions[SettingsLevel.DEFAULT] = {}

        for (
            settings_type
        ) in (
            self._types_with_defaults
        ):  # pylint: disable=consider-using-dict-items
            self._settings_definitions[SettingsLevel.DEFAULT][
                settings_type
            ] = EditorSettingsDefinition(
                pw_ide_settings, self._types_with_defaults[settings_type]
            )

        # Initialize the settings definitions for each settings type for each
        # settings level that's stored on disk.
        for level in SettingsLevel.all_files():
            self._settings_definitions[level] = {}

            for settings_type in self._types_with_defaults:
                name = f'{self._prefixes[level]}{settings_type.value}'
                self._settings_definitions[level][
                    settings_type
                ] = EditorSettingsFile(
                    self._settings_dir, name, self._file_format
                )

    def default(self, settings_type: _TSettingsType):
        """Default settings for the provided settings type."""
        return self._settings_definitions[SettingsLevel.DEFAULT][settings_type]

    def project(self, settings_type: _TSettingsType):
        """Project settings for the provided settings type."""
        return self._settings_definitions[SettingsLevel.PROJECT][settings_type]

    def user(self, settings_type: _TSettingsType):
        """User settings for the provided settings type."""
        return self._settings_definitions[SettingsLevel.USER][settings_type]

    def active(self, settings_type: _TSettingsType):
        """Active settings for the provided settings type."""
        return self._settings_definitions[SettingsLevel.ACTIVE][settings_type]

    def delete_all_active_settings(self) -> None:
        """Delete all active settings files."""
        for settings_type in self._settings_types:
            self.project(settings_type).delete()
            self.user(settings_type).delete()
            self.active(settings_type).delete()

    def delete_all_backups(self) -> None:
        """Delete all backup files."""
        for settings_type in self._settings_types:
            self.project(settings_type).delete_backups()
            self.user(settings_type).delete_backups()
            self.active(settings_type).delete_backups()
