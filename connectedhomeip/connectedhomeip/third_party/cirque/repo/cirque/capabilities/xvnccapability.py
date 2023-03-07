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
import glob
import subprocess
from threading import Lock

from cirque.capabilities.basecapability import BaseCapability


class XvncCapability(BaseCapability):
  X_SOCKET_PATH = '/tmp/.X11-unix'

  def __init__(self, localhost=True, display_id=0, docker_display_id=0):
    self.localhost = localhost
    self.__display_id = display_id
    self.__docker_display_id = docker_display_id
    self.__xvnc_process = None
    self.__launch_xvnc_server()

  def __launch_xvnc_server(self):
    xvnc_args = ['Xvnc', '--SecurityTypes=None']
    if self.__display_id == 0:
      self.__display_id = self.__get_next_display_id()
    if self.localhost:
      xvnc_args.append('-localhost')
    xvnc_args.append(':{}'.format(self.__display_id))
    self.xvnc_proces = subprocess.Popen(xvnc_args)

  def __get_next_display_id(self):
    if not os.path.exists(self.X_SOCKET_PATH) or \
       len(os.listdir(self.X_SOCKET_PATH)) == 0:
      return 0
    x_server_ids = glob.glob(os.path.join(self.X_SOCKET_PATH, 'X*'))
    x_server_ids = [int(os.path.basename(f)[1:]) for f in x_server_ids]
    return max(x_server_ids) + 1

  @property
  def name(self):
    return 'Xvnc'

  @property
  def description(self):
    return {
        'localhost': self.localhost,
        'display_id': self.__display_id,
    }

  def get_docker_run_args(self, dockernode):
    return {
        'environment': ['DISPLAY=:{}'.format(self.__docker_display_id)],
        'volumes': [
            '{}/X{}:{}/X{}'.format(self.X_SOCKET_PATH, self.__display_id,
                                   self.X_SOCKET_PATH, self.__docker_display_id)
        ]
    }

  def enable_capability(self, docker_node):
    pass

  def disable_capability(self, docker_node):
    if self.__xvnc_process:
      self.__xvnc_process.kill()
      self.__xvnc_process = None
