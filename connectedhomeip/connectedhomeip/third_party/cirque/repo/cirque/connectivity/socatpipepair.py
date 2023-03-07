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
import re
import subprocess


class SocatPipePair:

  @staticmethod
  def __find_socat_pts(stream, wait_lines=5):
    # socat creates pipe file /dev/pts/${NUMBER}
    r = re.compile('/dev/pts/.*$')
    for _ in range(wait_lines):
      line = stream.readline().decode()
      match = r.search(line)
      if match:
        return match.group()
    return ''

  def __init__(self):
    self.socat = None
    self.pipe0 = None
    self.pipe1 = None

  def open(self):
    self.socat = subprocess.Popen(
        ['socat', '-d', '-d', 'pty,raw,echo=0', 'pty,raw,echo=0'],
        stderr=subprocess.PIPE)
    self.pipe0 = SocatPipePair.__find_socat_pts(self.socat.stderr)
    self.pipe1 = SocatPipePair.__find_socat_pts(self.socat.stderr)

  def close(self):
    if self.socat is not None:
      self.socat.terminate()
    self.socat = None

  def __del__(self):
    self.close()
