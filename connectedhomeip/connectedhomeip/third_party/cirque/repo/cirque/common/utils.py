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

import time
import shlex
import subprocess


class Return:

  def __init__(self, returncode, stdout, stderr):
    self.returncode = returncode
    self.stdout = stdout
    self.stderr = stderr


def sleep_time(logger, seconds, reason=None):
  if reason:
    reason = 'for ' + reason
  logger.debug('sleeps {}s {}'.format(seconds, reason))
  time.sleep(seconds)


def host_run(logger,
             command,
             namespace=None,
             stdin=None,
             stdout=subprocess.PIPE,
             stderr=subprocess.PIPE):
  if not isinstance(command, (list, str)):
    logger.error(
        'unable to parse desired commands..Supporting only list or str!')
    return -1
  exec_command = ' '.join(command) if isinstance(command, list) else command

  logger.debug('CMD RUN: {}'.format(exec_command))

  if namespace and 'ip netns exec' not in exec_command:
    exec_command = 'ip netns exec {} '.format(namespace) + exec_command

  process = None
  for command in exec_command.split('|'):
    stdin = process.stdout if process else None
    process = subprocess.Popen(
        shlex.split(command), stdin=stdin, stdout=stdout, stderr=stderr)
  stdout, stderr = process.communicate()
  returncode = process.returncode
  logger.debug('CMD RESULT: \n'
               'ReturnCode: {},\nStdout: {},\nStdin: {}'.format(
                   returncode, stdout, stderr))

  return Return(returncode, stdout, stderr)


def netns_run(logger,
              command,
              namespace,
              stdin=None,
              stdout=subprocess.PIPE,
              stderr=subprocess.PIPE):
  return host_run(logger, command, namespace, stdin, stdout, stderr)


def manipulate_iptable_src_dst_rule(logger, src, dst, action, add=True):
  chain = 'DOCKER-USER'
  manipulate_action = '-I' if add else '-D'
  ret = host_run(logger, 'iptables -L DOCKER-USER')
  if b'No chain/target/match by that name' in ret.stderr:
    chain = 'INPUT'
  cmd = [
      'iptables', manipulate_action, chain, '-s', src, '-d', dst, '-j', action
  ]
  ret = host_run(logger, cmd)
  if ret.returncode != 0:
    logger.error('Failed to add iptables rule %s', ' '.join(cmd))
