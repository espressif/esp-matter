#!/usr/bin/env python
import sys
import os
from setuptools import setup, find_packages

#
#  Builds egg file for python distro
#

execfile('pyradioconfig\_version.py')
print(__version__)

setup (
    name = "pyradioconfig",
    version = __version__,
    description="Radio Configurator.",
    long_description="""\
Radio Configurator""",
    author="Silicon Labs",
    author_email="", # Removed to limit spam harvesting.
    url="http://silabs.com",
    #package_dir = {'': 'Package'}, # See packages below
    #package_data = {'': ['*.xml']},
    packages = find_packages(exclude="unit_tests"),
    # Use this line if you've uncommented package_dir above.
    #packages = find_packages("", exclude="/unit_tests"),

    #entry_points = {
    #    'console_scripts': ['watcher = watcher.core.main:main']
    #                },

    download_url = "",
    zip_safe = False
)