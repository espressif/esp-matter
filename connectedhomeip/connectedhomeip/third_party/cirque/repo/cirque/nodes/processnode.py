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
import subprocess
import sys

from cirque.connectivity.ipnamespace import IpNameSpace


class ProcessNode:

  @staticmethod
  def tailfile(filename, tail):
    with open(filename, 'r') as f:
      guess_line_length = 60
      f.seek(0, os.SEEK_END)
      pos = f.tell() - guess_line_length * tail
      while pos > 0:
        f.seek(pos, os.SEEK_SET)
        lines = f.read().splitlines()
        if len(lines) > tail:
          return '\n'.join(lines[-tail:])
        else:
          pos -= guess_line_length * tail
      f.seek(0, os.SEEK_SET)
      return '\n'.join(f.read().splitlines()[-tail:])

  def __init__(self, cmd, log_file=None, ip_namespace=None):
    self._cmd = cmd
    self.raw_cmd = cmd
    self.ip_namespace = ip_namespace
    self.process = None
    self.log_file = log_file
    self.capabilities = set()
    if log_file is not None:
      self.log_fd = open(log_file, 'w')
    else:
      self.log_fd = None
    if ip_namespace is not None:
      self._cmd = 'ip netns exec {} {}'.format(ip_namespace.namespace_name, cmd)

  def run(self):
    target_output = self.log_fd if self.log_fd is not None else sys.stdout
    self.process = subprocess.Popen(
        self._cmd, shell=True, stdout=target_output, stderr=target_output)

  def get_device_log(self, tail='all'):
    if self.log_file is not None:
      if tail == 'all':
        with open(self.log_file, 'r') as f:
          return f.read()
      else:
        return ProcessNode.tailfile(self.log_file, tail)
    return ''

  def stop(self):
    if self.process is not None:
      self.process.kill()
    if self.log_fd is not None:
      self.log_fd.close()

  def __del__(self):
    self.stop()

  @property
  def id(self):
    if self.process is not None:
      return self.process.pid
    return None

  @property
  def name(self):
    if self.process is not None:
      return '{}:{}'.format(self.proces.pid, self.raw_cmd.split()[0])
    return None
