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
"""Finds files for a given product."""

from typing import Any, List, Optional, Set, Tuple

import pathlib
import re


def parse_product_str(product_str: str) -> Tuple[str, Set[str], str]:
    """Parses provided product string.

    Args:
        product_str: target supplied product string.

    Returns:
        (family, defines, name) where
            `family` is the stm32 product family (ex. 'stm32l5xx')
            `defines` is a list of potential product defines for the HAL.
                There can be multiple because some products use a subfamily
                for their define.
                (ex. The only stm32f411 define is `STM32F411xE`)
                The correct define can be validated using `select_define()`
            `name` is the standardized name for the product string.
                (ex. product_str = 'STM32F429', name = 'stm32f429xx')
                This is the product name expected by the filename matching
                functions (`match_filename()`, etc.)

    Raises:
        ValueError if the product string does not start with 'stm32' or specify
            at least the chip model (9 chars).
    """
    product_name = product_str.lower()
    if not product_name.startswith('stm32'):
        raise ValueError("Product string must start with 'stm32'")

    if len(product_name) < 9:
        raise ValueError(
            "Product string too short. Must specify at least the chip model."
        )

    family = product_name[:7] + 'xx'
    name = product_name

    # Pad the full name with 'x' to reach the max expected length.
    name = product_name.ljust(11, 'x')

    # This generates all potential define suffixes for a given product name
    # This is required because some boards have more specific defines
    # ex. STM32F411xE, while most others are generic, ex. STM32F439xx
    # So if the user specifies `stm32f207zgt6u`, this should generate the
    # following as potential defines
    #  STM32F207xx, STM32F207Zx, STM32F207xG, STM32F207ZG
    define_suffixes = ['xx']
    if name[9] != 'x':
        define_suffixes.append(name[9].upper() + 'x')
    if name[10] != 'x':
        define_suffixes.append('x' + name[10].upper())
    if name[9] != 'x' and name[10] != 'x':
        define_suffixes.append(name[9:11].upper())

    defines = set(map(lambda x: product_name[:9].upper() + x, define_suffixes))
    return (family, defines, name)


def select_define(defines: Set[str], family_header: str) -> str:
    """Selects valid define from set of potential defines.

    Looks for the defines in the family header to pick the correct one.

    Args:
        defines: set of defines provided by `parse_product_str`
        family_header: `{family}.h` read into a string

    Returns:
        A single valid define

    Raises:
        ValueError if exactly one define is not found.
    """
    valid_defines = list(
        filter(
            lambda x: f'defined({x})' in family_header
            or f'defined ({x})' in family_header,
            defines,
        )
    )

    if len(valid_defines) != 1:
        raise ValueError("Unable to select a valid define")

    return valid_defines[0]


def match_filename(product_name: str, filename: str):
    """Matches linker and startup filenames with product name.

    Args:
        product_name: the name standardized by `parse_product_str`
        filename: a linker or startup filename

    Returns:
        True if the filename could be associated with the product.
        False otherwise.
    """
    stm32_parts = list(
        filter(
            lambda x: x.startswith('stm32'), re.split(r'\.|_', filename.lower())
        )
    )

    if len(stm32_parts) != 1:
        return False

    pattern = stm32_parts[0].replace('x', '.')

    return re.match(pattern, product_name) is not None


def find_linker_files(
    product_name: str, files: List[str], stm32cube_path: pathlib.Path
) -> Tuple[Optional[pathlib.Path], Optional[pathlib.Path]]:
    """Finds linker file for the given product.

    This searches `files` for linker scripts by name.

    Args:
        product_name: the name standardized by `parse_product_str`
        files: list of file paths
        stm32cube_path: the root path that `files` entries are relative to

    Returns:
        (gcc_linker, iar_linker) where gcc_linker / iar_linker are paths to a
            linker file or None

    Raises:
        ValueError if `product_name` matches with no linker files, or with
            multiple .ld/.icf files.
    """
    linker_files = list(
        filter(
            lambda x: (x.endswith('.ld') or x.endswith('.icf'))
            and '_flash.' in x.lower(),
            files,
        )
    )
    matching_linker_files = list(
        filter(
            lambda x: match_filename(product_name, pathlib.Path(x).name),
            linker_files,
        )
    )

    matching_ld_files = list(
        filter(lambda x: x.endswith('.ld'), matching_linker_files)
    )
    matching_icf_files = list(
        filter(lambda x: x.endswith('.icf'), matching_linker_files)
    )

    if len(matching_ld_files) > 1 or len(matching_icf_files) > 1:
        raise ValueError(
            f'Too many linker file matches for {product_name}. '
            'Provide a more specific product string or your own linker script'
        )
    if not matching_ld_files and not matching_icf_files:
        raise ValueError(f'No linker script matching {product_name} found')

    return (
        stm32cube_path / matching_ld_files[0] if matching_ld_files else None,
        stm32cube_path / matching_icf_files[0] if matching_icf_files else None,
    )


def find_startup_file(
    product_name: str, files: List[str], stm32cube_path: pathlib.Path
) -> pathlib.Path:
    """Finds startup file for the given product.

    Searches for gcc startup files.

    Args:
        product_name: the name standardized by `parse_product_str`
        files: list of file paths
        stm32cube_path: the root path that `files` entries are relative to

    Returns:
        Path to matching startup file

    Raises:
        ValueError if no / > 1 matching startup files found.
    """
    # ST provides startup files for gcc, iar, and arm compilers. They have the
    # same filenames, so this looks for a 'gcc' folder in the path.
    matching_startup_files = list(
        filter(
            lambda f: '/gcc/' in f
            and f.endswith('.s')
            and match_filename(product_name, f),
            files,
        )
    )

    if not matching_startup_files:
        raise ValueError(f'No matching startup file found for {product_name}')
    if len(matching_startup_files) == 1:
        return stm32cube_path / matching_startup_files[0]

    raise ValueError(
        f'Multiple matching startup files found for {product_name}'
    )


_INCLUDE_DIRS = [
    'hal_driver/Inc',
    'hal_driver/Inc/Legacy',
    'cmsis_device/Include',
    'cmsis_core/Include',
    'cmsis_core/DSP/Include',
]


def get_include_dirs(stm32cube_path: pathlib.Path) -> List[pathlib.Path]:
    """Get HAL include directories."""
    return list(map(lambda f: stm32cube_path / f, _INCLUDE_DIRS))


def get_sources_and_headers(
    files: List[str], stm32cube_path: pathlib.Path
) -> Tuple[List[str], List[str]]:
    """Gets list of all sources and headers needed to build the stm32cube hal.

    Args:
        files: list of file paths
        stm32cube_path: the root path that `files` entries are relative to

    Returns:
        (sources, headers) where
            `sources` is a list of absolute paths to all core (non-template)
                sources needed for the hal
            `headers` is a list of absolute paths to all needed headers
    """
    source_files = filter(
        lambda f: f.startswith('hal_driver/Src')
        and f.endswith('.c')
        and 'template' not in f,
        files,
    )

    header_files = filter(
        lambda f: (any(f.startswith(dir) for dir in _INCLUDE_DIRS))
        and f.endswith('.h'),
        files,
    )

    rebase_path = lambda f: str(stm32cube_path / f)
    return list(map(rebase_path, source_files)), list(
        map(rebase_path, header_files)
    )


def parse_files_txt(stm32cube_path: pathlib.Path) -> List[str]:
    """Reads files.txt into list."""
    with open(stm32cube_path / 'files.txt', 'r') as files:
        return list(
            filter(
                lambda x: not x.startswith('#'),
                map(lambda f: f.strip(), files.readlines()),
            )
        )


def _gn_str_out(name: str, val: Any):
    """Outputs scoped string in GN format."""
    print(f'{name} = "{val}"')


def _gn_list_str_out(name: str, val: List[Any]):
    """Outputs list of strings in GN format with correct escaping."""
    list_str = ','.join(
        '"' + str(x).replace('"', r'\"').replace('$', r'\$') + '"' for x in val
    )
    print(f'{name} = [{list_str}]')


def find_files(stm32cube_path: pathlib.Path, product_str: str, init: bool):
    """Generates and outputs the required GN args for the build."""
    file_list = parse_files_txt(stm32cube_path)

    include_dirs = get_include_dirs(stm32cube_path)
    sources, headers = get_sources_and_headers(file_list, stm32cube_path)
    (family, defines, name) = parse_product_str(product_str)

    family_header_path = list(
        filter(lambda p: p.endswith(f'/{family}.h'), headers)
    )[0]

    with open(family_header_path, 'rb') as family_header:
        family_header_str = family_header.read().decode(
            'utf-8', errors='ignore'
        )

    define = select_define(defines, family_header_str)

    _gn_str_out('family', family)
    _gn_str_out('product_define', define)
    _gn_list_str_out('sources', sources)
    _gn_list_str_out('headers', headers)
    _gn_list_str_out('include_dirs', include_dirs)

    if init:
        startup_file_path = find_startup_file(name, file_list, stm32cube_path)
        gcc_linker, iar_linker = find_linker_files(
            name, file_list, stm32cube_path
        )

        _gn_str_out('startup', startup_file_path)
        _gn_str_out('gcc_linker', gcc_linker if gcc_linker else '')
        _gn_str_out('iar_linker', iar_linker if iar_linker else '')
