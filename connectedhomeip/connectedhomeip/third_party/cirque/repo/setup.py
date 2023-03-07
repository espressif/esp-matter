# Copyright 2020 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
import setuptools
from setuptools.command.install import install
import glob
import pkg_resources

ENABLE_GRPC = not os.environ.get('NO_GRPC', False)
PACKAGE_PATH = os.path.realpath(os.path.dirname(__file__))
SRC_PROTOS = os.path.join(PACKAGE_PATH, 'cirque/proto')
VERSION_FILE = os.path.join(PACKAGE_PATH, 'version')

SETUP_REQUIRES = ('grpcio-tools',)
REQUIREMENTS_TXT = 'requirements.txt'

if not ENABLE_GRPC:
  SETUP_REQUIRES = ()
  REQUIREMENTS_TXT = 'requirements_nogrpc.txt'

with open(REQUIREMENTS_TXT) as f:
  INSTALL_REQUIRES = f.read().splitlines()


def version():
  vers = ''
  with open(VERSION_FILE, 'r') as fp:
    vers = fp.readline().rstrip()
  return vers


def compile_protos():
  from grpc_tools import protoc

  proto_include = pkg_resources.resource_filename('grpc_tools', '_proto')

  for proto in glob.glob(os.path.join(SRC_PROTOS, '*.proto')):
    command = [
        'grpc_tools.protoc',
        '--proto_path={}'.format(PACKAGE_PATH),
        '--python_out={}'.format(PACKAGE_PATH),
        '--grpc_python_out={}'.format(PACKAGE_PATH),
    ] + [proto]

    if protoc.main(command + ['-I{}'.format(proto_include)]) != 0:
      raise Exception('Error: Installation failed')


here = os.path.abspath(os.path.dirname(__file__))
long_description = open(os.path.join(here, 'README.md')).read()

setuptools.setup(
    name='cirque',
    version=version(),
    description='a virtual home orchestration platform for different kinds of'
    ' simulates devices',
    long_description=long_description,
    python_requires='>=3.5',
    packages=setuptools.find_packages(),
    setup_requires=SETUP_REQUIRES,
    install_requires=INSTALL_REQUIRES,
    entry_points={
        'console_scripts': ['cirqueservice=cirque.resources.run_flask:main'],
    },
    include_package_data=True,
    data_files=[('', ['version', 'requirements.txt',
                      'requirements_nogrpc.txt']),
                ('cirque', glob.glob('cirque/proto/*.proto'))],
    zip_safe=True,
    author='Google LLC',
    platforms=['Linux'],
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'License :: OSI Approved :: Apache Software License',
        'Intended Audience :: Developers', 'Intended Audience :: Education',
        'Operating System :: POSIX :: Linux',
        'Topic :: Software Development :: Testing',
        'Topic :: Software Development :: Embedded Systems',
        'Topic :: System :: Emulators', 'Topic :: System :: Networking'
    ],
    license='Apache',
    url='https://github.com/openweave/cirque/',
)

if ENABLE_GRPC:
  compile_protos()
