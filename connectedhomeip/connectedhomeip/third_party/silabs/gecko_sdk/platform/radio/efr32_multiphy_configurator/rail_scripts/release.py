#!/usr/bin/python
#

from _version import __version__

import re
import pytest
import argparse
import distutils.dir_util
import os, shutil
from subprocess import call

# Initialize colorama, if available
try:
    import colorama
    colorama.init()
    coloramaIsAvailable = True
except ImportError:
    coloramaIsAvailable = False

import sys
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'tests'))
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), '..'))

# The release process is the following:
#   1. Update internal release version according to "bump"type specified.
#   2. Delete all (possibly) stale output files, for both single and multi Phy:
#       ../tests/single_phy/output_files
#       ../tests/multi_phy/output_files
#   3. Run tests to generate the output files, ignore test result.
#   4. Copy generated files over to reference files folder.
#   5. Re-run the tests, everything should pass at this point.
#   6. Verify the changes in the updated reference files. Usually, the only
#      change should be the version number, unless new features are added.
def release(bump="patch"):

    versionPattern = r"(?P<major>\d+)\.(?P<minor>\d+)\.(?P<patch>\d+)"
    versionRegEx = re.compile(versionPattern)

    currentVersion = re.search(versionPattern, __version__)

    print("Current version is {}".format(currentVersion.group(0)))

    versionMajor = int(currentVersion.group("major"))
    versionMinor = int(currentVersion.group("minor"))
    versionPatch = int(currentVersion.group("patch"))

    if bump == "test":
        # Used to generate new files and just to test the output
        pass
    elif bump == "patch":
        versionPatch += 1
    elif bump == "minor":
        versionMinor += 1
        versionPatch = 0
    else: #major bump
        versionMajor += 1
        versionMinor = 0
        versionPatch = 0

    newVersion = "{}.{}.{}".format(versionMajor, versionMinor, versionPatch)

    print("New version is {}".format(newVersion))

    cur_dir = os.path.dirname(os.path.realpath(__file__))
    os.chdir(os.path.join(cur_dir, "../../../..//"))
    efr32_multi_phy_radio_configurator_commit = os.popen("git rev-parse HEAD").read()[:-1]
    os.chdir(os.path.join(cur_dir, "../..//"))
    host_py_radio_config_commit = os.popen("git rev-parse HEAD").read()[:-1]
    os.chdir(cur_dir)

    with open("_version.py", 'r') as versionFile:
        data = versionFile.read()

    match = re.search(versionPattern, data)

    data = data.replace(match.group(0), newVersion)

    data = data.split("\n")

    for line in data:
        if "host_py_radio_config_commit" in line:
            data[data.index(line)] = "host_py_radio_config_commit = \'" + host_py_radio_config_commit + "\'"
        if "efr32_multi_phy_radio_configurator_commit" in line:
            data[data.index(line)] = "efr32_multi_phy_radio_configurator_commit = \'" + efr32_multi_phy_radio_configurator_commit + "\'"

    with open("_version.py", 'w') as versionFile:
        print("Updating '_version.py'")
        # Don't keep appending extra newlines
        for line in data[:-1]:
            versionFile.write(line + '\n')

    print("Deleting current files in output directories...")
    current_dir = os.path.dirname(__file__)
    singlePhyOutputFilesPath = os.path.join(current_dir, "tests/single_phy/output_files")
    multiPhyOutputFilesPath = os.path.join(current_dir, "tests/multi_phy/output_files")
    try:
        shutil.rmtree(singlePhyOutputFilesPath)
        shutil.rmtree(multiPhyOutputFilesPath)
    except:
        pass

    print("Running first round of tests (release mode) to generate new output files...")
    results = pytest.main(["-q", "--release", "tests"])

    # There should be no failures at this point (from _pytest.main, 0 == EXIT_OK)
    if results == 0:

        print("Copying output files to 'reference_files' folder...")
        distutils.dir_util.copy_tree("tests/single_phy/output_files", "tests/single_phy/reference_files")
        distutils.dir_util.copy_tree("tests/multi_phy/output_files", "tests/multi_phy/reference_files")

        print("Running second round of tests...")
        results = pytest.main(["-xq", "tests"])

        assert results == 0, "Can't release due to failed tests"

        print("\n\n {} Release files updated. Commit, tag and push the changes to complete the release\n\n"\
            .format(colorama.Fore.GREEN if coloramaIsAvailable else ""))

# Allow stand-alone execution
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('-b', '--bump', type=str, default="patch", choices=["major", "minor", "patch", "test"], help="Type of bump for new release.")
    args = parser.parse_args()
    bump = args.bump

    release(bump)
