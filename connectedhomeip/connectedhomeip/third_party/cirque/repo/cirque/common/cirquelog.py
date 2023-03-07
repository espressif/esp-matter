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

import logging


class CirqueLog:

  logger = None

  @classmethod
  def setup_cirque_logger(cls, level=logging.INFO):
    if cls.logger:
      return
    cls.logger = logging.getLogger('cirque')
    cls.logger.setLevel(level)
    sh = logging.StreamHandler()
    sh.setFormatter(
        logging.Formatter('%(asctime)s [%(name)s] %(levelname)s %(message)s'))
    cls.logger.addHandler(sh)

  @classmethod
  def get_cirque_logger(cls, name=None):
    name = 'cirque' if not name else 'cirque.{}'.format(name)
    return logging.getLogger(name)
