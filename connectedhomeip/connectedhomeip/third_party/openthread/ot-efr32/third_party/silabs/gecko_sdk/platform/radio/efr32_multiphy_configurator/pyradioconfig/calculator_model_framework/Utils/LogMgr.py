from enum import IntEnum
import os

from pycalcmodel.core.log import LogMsg, LogType


class ConsoleColor:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    YELLOW = '\033[93m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'
    CYAN = '\033[96m'
    RED = '\033[91m'
    WHITE = '\033[256m'


class LogMgr(object):

    _log_queue = []

    _is_pycharm_hosted = "PYCHARM_HOSTED" in os.environ

    @staticmethod
    def _add_queue(log_type, message):
        """
        :param log_type: LogType
        :param message: str
        :return:
        """
        assert isinstance(log_type, LogType)
        # assert isinstance(log_type, basestring)
        # print(message)
        logmsg = LogMsg(log_type, message)
        LogMgr._log_queue.append(logmsg)

    @staticmethod
    def reset():
        LogMgr._log_queue = []

    @staticmethod
    def get_queue():
        """
        :rtype: List(LogMessage)
        """
        return LogMgr._log_queue

    @staticmethod
    def Info(message, color=ConsoleColor.WHITE):
        LogMgr._print(color, message)
        LogMgr._add_queue(LogType.info, message)

    @staticmethod
    def Debug(message, color=ConsoleColor.OKBLUE):
        LogMgr._print(color, message)
        LogMgr._add_queue(LogType.debug, message)

    @staticmethod
    def Error(message, color=ConsoleColor.FAIL):
        LogMgr._print(color, message)
        LogMgr._add_queue(LogType.error, message)

    @staticmethod
    def Warning(message, color=ConsoleColor.WARNING):
        LogMgr._print(color, message)
        LogMgr._add_queue(LogType.warning, message)

    @staticmethod
    def _print(color, message):
        if LogMgr._is_pycharm_hosted:
            # Use color inside PyCharm, else don't send color codes to console,
            # which breaks tools that don't understand unicode color charachers
            print('{}{}{}'.format(color, message, ConsoleColor.ENDC))
        else:
            print(message)
