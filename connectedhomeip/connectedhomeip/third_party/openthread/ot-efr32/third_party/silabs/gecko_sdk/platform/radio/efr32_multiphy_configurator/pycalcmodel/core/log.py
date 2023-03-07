from enum import IntEnum


class LogType(IntEnum):
    info = 0,
    warning = 1,
    error = 2,
    debug = 100


class LogMsg(object):

    def __init__(self, log_type, message):
        self.log_type = log_type
        self.message = message
