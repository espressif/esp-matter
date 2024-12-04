# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD

# SPDX-License-Identifier: CC0-1.0

# This file is used in CI generate binary files for different kinds of apps

import argparse
import sys
from pathlib import Path
from typing import List

import logging

idf_build_apps_logger = logging.getLogger('idf_build_apps')
from idf_build_apps import App, build_apps, find_apps, setup_logging

# from idf_ci_utils import IDF_PATH, get_pytest_app_paths, get_pytest_cases, get_ttfw_app_paths

PROJECT_ROOT = Path(__file__).parent.parent.parent.absolute()
DEF_APP_PATH = PROJECT_ROOT / 'examples'
APPS_BUILD_PER_JOB = 30
PYTEST_C6_APPS = [
    {"target": "esp32c6", "name": "light"},
]
MAINFEST_FILES = [
    str(PROJECT_ROOT / 'examples' / '.build-rules.yml'),
]

PYTEST_H2_APPS = [
    {"target": "esp32h2", "name": "light"},
]
MAINFEST_FILES = [
    str(PROJECT_ROOT / 'examples' / '.build-rules.yml'),
]

PYTEST_C3_APPS = [
    {"target": "esp32c3", "name": "light"},
]
MAINFEST_FILES = [
    str(PROJECT_ROOT / 'examples' / '.build-rules.yml'),
]

PYTEST_C2_APPS = [
    {"target": "esp32c2", "name": "light"},
]
MAINFEST_FILES = [
    str(PROJECT_ROOT / 'examples' / '.build-rules.yml'),
]

# Exclude list for no-pytest apps in CI on merge request or branch pipelines.
# The below examples will be built on main branch pipeline.
NO_PYTEST_REMAINING_APPS = [
    {"target": "esp32c2", "name": "light_switch"},
    {"target": "esp32c6", "name": "light_switch"},
    {"target": "esp32h2", "name": "light_switch"},
    {"target": "esp32"  , "name": "light_switch"},
    {"target": "esp32c2", "name": "generic_switch"},
    {"target": "esp32c6", "name": "generic_switch"},
    {"target": "esp32h2", "name": "generic_switch"},
    {"target": "esp32h2", "name": "multiple_on_off_plugin_units"},
    {"target": "esp32c3", "name": "multiple_on_off_plugin_units"},
    {"target": "esp32"  , "name": "multiple_on_off_plugin_units"},
    {"target": "esp32s3", "name": "multiple_on_off_plugin_units"},
    {"target": "esp32"  , "name": "room_air_conditioner"},
    {"target": "esp32c3", "name": "room_air_conditioner"},
    {"target": "esp32c2", "name": "room_air_conditioner"},
    {"target": "esp32c6", "name": "room_air_conditioner"},
    {"target": "esp32h2", "name": "room_air_conditioner"},
    {"target": "esp32"  , "name": "door_lock"},
    {"target": "esp32c3", "name": "door_lock"},
    {"target": "esp32c2", "name": "door_lock"},
    {"target": "esp32c6", "name": "door_lock"},
    {"target": "esp32h2", "name": "door_lock"},
    {"target": "esp32s3", "name": "ota_provider"},
    {"target": "esp32c3", "name": "sensors"},
    {"target": "esp32"  , "name": "refrigerator"},
    {"target": "esp32c3", "name": "refrigerator"},
    {"target": "esp32c2", "name": "refrigerator"},
    {"target": "esp32c6", "name": "refrigerator"},
    {"target": "esp32h2", "name": "refrigerator"},
    {"target": "esp32"  , "name": "demo/badge"},
]
MAINFEST_FILES = [
    str(PROJECT_ROOT / 'examples' / '.build-rules.yml'),
]

def _is_c6_pytest_app(app: App) -> bool:
    print(app.name, app.target)
    for pytest_app in PYTEST_C6_APPS:
        print(pytest_app["name"], pytest_app["target"])
        if app.name == pytest_app["name"] and app.target == pytest_app["target"]:
            return True
    return False

def _is_h2_pytest_app(app: App) -> bool:
    for pytest_app in PYTEST_H2_APPS:
        if app.name == pytest_app["name"] and app.target == pytest_app["target"]:
            return True
    return False

def _is_c3_pytest_app(app: App) -> bool:
    for pytest_app in PYTEST_C3_APPS:
        if app.name == pytest_app["name"] and app.target == pytest_app["target"]:
            return True
    return False

def _is_c2_pytest_app(app: App) -> bool:
    for pytest_app in PYTEST_C2_APPS:
        if app.name == pytest_app["name"] and app.target == pytest_app["target"]:
            return True
    return False

# Function to check for no_pytest excluded list apps.
def _is_no_pytest_remaining_app(app: App) -> bool:
    for no_pytest_app in NO_PYTEST_REMAINING_APPS:
        if app.name == no_pytest_app["name"] and app.target == no_pytest_app["target"]:
            return True
    return False

def get_cmake_apps(
    paths: List[str],
    target: str,
    config_rules_str: List[str],
) -> List[App]:
    apps = find_apps(
        paths,
        recursive=True,
        target=target,
        build_dir='build_@t_@w',
        config_rules_str=config_rules_str,
        build_log_filename='build_log.txt',
        size_json_filename='size.json',
        check_warnings=False,
        manifest_files=MAINFEST_FILES,
    )
    return apps

def main(args: argparse.Namespace) -> None:
    apps = get_cmake_apps(args.paths, args.target, args.config)

    # no_pytest and only_pytest can not be both True
    assert not (args.no_pytest and args.pytest_c6 and args.pytest_h2 and args.pytest_c3 and args.pytest_c2)
    if args.no_pytest:
        apps_for_build = [app for app in apps if not (_is_c6_pytest_app(app) or _is_h2_pytest_app(app) or _is_no_pytest_remaining_app(app))]
    elif args.pytest_c6:
        apps_for_build = [app for app in apps if _is_c6_pytest_app(app)]
    elif args.pytest_h2:
        apps_for_build = [app for app in apps if _is_h2_pytest_app(app)]
    elif args.pytest_c3:
        apps_for_build = [app for app in apps if _is_c3_pytest_app(app)]
    elif args.pytest_c2:
        apps_for_build = [app for app in apps if _is_c2_pytest_app(app)]
    elif args.no_pytest_remaining:
        apps_for_build = [app for app in apps if _is_no_pytest_remaining_app(app)]
    else:
        apps_for_build = apps[:]

    idf_build_apps_logger.info('Found %d apps after filtering', len(apps_for_build))
    idf_build_apps_logger.info(
        'Suggest setting the parallel count to %d for this build job',
        len(apps_for_build) // APPS_BUILD_PER_JOB + 1,
    )

    ret_code = build_apps(
        apps_for_build,
        parallel_count=args.parallel_count,
        parallel_index=args.parallel_index,
        dry_run=False,
        collect_size_info=args.collect_size_info,
        # build_verbose=0,
        keep_going=True,
        ignore_warning_strs=[r".*"],
        copy_sdkconfig=True,
    )

    sys.exit(ret_code)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description='Build all the apps for different test types. Will auto remove those non-test apps binaries',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument('paths', nargs='*', help='Paths to the apps to build.')
    parser.add_argument(
        '-t',
        '--target',
        default='all',
        help='Build apps for given target. could pass "all" to get apps for all targets',
    )
    parser.add_argument(
        '--config',
        default=['sdkconfig.ci=default', 'sdkconfig.ci.*=', '=default'],
        action='append',
        help='Adds configurations (sdkconfig file names) to build. This can either be '
        'FILENAME[=NAME] or FILEPATTERN. FILENAME is the name of the sdkconfig file, '
        'relative to the project directory, to be used. Optional NAME can be specified, '
        'which can be used as a name of this configuration. FILEPATTERN is the name of '
        'the sdkconfig file, relative to the project directory, with at most one wildcard. '
        'The part captured by the wildcard is used as the name of the configuration.',
    )
    parser.add_argument(
        '--parallel-count', default=1, type=int, help='Number of parallel build jobs.'
    )
    parser.add_argument(
        '--parallel-index',
        default=1,
        type=int,
        help='Index (1-based) of the job, out of the number specified by --parallel-count.',
    )
    parser.add_argument(
        '--no_pytest',
        action="store_true",
        help='Exclude pytest apps definded in PYTEST_H2_APPS and PYTEST_C6_APPS and some optional no-pytest apps',
    )
    parser.add_argument(
        '--no_pytest_remaining',
        action="store_true",
        help='Build the excluded no-pytest apps using manual trigger.',
    )
    parser.add_argument(
        '--pytest_c6',
        action="store_true",
        help='Only build pytest apps, definded in PYTEST_C6_APPS',
    )
    parser.add_argument(
        '--pytest_h2',
        action="store_true",
        help='Only build pytest apps, definded in PYTEST_H2_APPS',
    )
    parser.add_argument(
        '--pytest_c3',
        action="store_true",
        help='Only build pytest apps, definded in PYTEST_C3_APPS',
    )
    parser.add_argument(
        '--pytest_c2',
        action="store_true",
        help='Only build pytest apps, definded in PYTEST_C2_APPS',
    )
    parser.add_argument(
        '--collect-size-info',
        type=argparse.FileType('w'),
        help='If specified, the test case name and size info json will be written to this file',
    )

    arguments = parser.parse_args()
    if not arguments.paths:
        arguments.paths = [DEF_APP_PATH]
    setup_logging(verbose=1)  # Info
    main(arguments)

