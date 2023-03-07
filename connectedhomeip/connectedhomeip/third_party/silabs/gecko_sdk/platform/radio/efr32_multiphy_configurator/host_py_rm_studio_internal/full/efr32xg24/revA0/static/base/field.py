
__all__ = ['Base_RM_Field']

from .. interface import IRegMapField
from .. common import GetFieldValue


class Base_RM_Field(IRegMapField):
    def __init__(self, register, name, fullname, access, description, bitOffset, bitWidth, enum=None):
        self.__dict__['zz_frozen'] = False
        self.zz_accessed_flag = False
        self.zz_reg = register
        self.baseAddress = self.zz_reg.baseAddress
        self.addressOffset = self.zz_reg.addressOffset
        self.name = name
        self.fullname = fullname
        self.access = access
        self.description = description
        self.bitOffset = bitOffset
        self.bitWidth = bitWidth
        self.enum = enum
        self.mask = (0xFFFFFFFF >> (32 - bitWidth)) << bitOffset

    def __setattr__(self, name, value):
        if self.__dict__['zz_frozen']:
            if name == 'io':
                self._c_log_write(value)
                self._py_log_write(value)
                if self.zz_reg.zz_rmio.accessed_flag_active:
                    self.setAccessedFlag()
                    self.zz_reg.setAccessedFlag()
                self.zz_reg.zz_rmio.writeRegisterField(self.zz_reg, self, value)
            else:
                raise AttributeError("FATAL ERROR: Unable to set '{}' to '{}'".format(name, value))
        else:
            self.__dict__[name] = value

    def __repr__(self):
        flagStr = "**" if self.getAccessedFlag() else ""
        enumStr = ' ENUMERATED' if self.enum is not None else ''
        out = u"    {:#010x} {}[{}] contains field {}{}: {} <{}>{}\n".format(self.baseAddress + self.addressOffset,
                                                                             self.zz_reg.name, self.getSliceStr(),
                                                                             self.name, flagStr,
                                                                             self.description, self.access, enumStr)
        return out

    def _c_log_read(self):
        if self.zz_reg.zz_rmio.c_logger_active:
            self.zz_reg.zz_rmio.c_logger.debug("    value = {0}->{1}; /* for field "
                                               "{0}_{1}_{2} */".format(self.zz_reg.pername,
                                                                       self.zz_reg.name,
                                                                       self.name))

    def _py_log_read(self):
        if self.zz_reg.zz_rmio.py_logger_active:
            self.zz_reg.zz_rmio.py_logger.debug("    print({}{}.{}.{}.io)".format(self.zz_reg.zz_rmio.py_rm_obj_prefix,
                                                                                  self.zz_reg.pername,
                                                                                  self.zz_reg.name,
                                                                                  self.name))

    def _c_log_write(self, value):
        if self.zz_reg.zz_rmio.c_logger_active:
            self.zz_reg.zz_rmio.c_logger.debug("    {0}->{1} = ({0}->{1} "
                                               "& ~_{0}_{1}_{2}_MASK) | {3:#010x};".format(self.zz_reg.pername,
                                                                                           self.zz_reg.name,
                                                                                           self.name,
                                                                                           value))

    def _py_log_write(self, value):
        if self.zz_reg.zz_rmio.py_logger_active:
            self.zz_reg.zz_rmio.py_logger.debug("    {}{}.{}.{}.io = "
                                                "{:#010x}".format(self.zz_reg.zz_rmio.py_rm_obj_prefix,
                                                                  self.zz_reg.pername,
                                                                  self.zz_reg.name,
                                                                  self.name,
                                                                  value))

    def _getio(self):
        self._c_log_read()
        self._py_log_read()
        return self.zz_reg.zz_rmio.readRegisterField(self.zz_reg, self)

    def _setio(self, value):
        # unused due to the __setattr__ logic above
        pass

    io = property(_getio, _setio)

    def set(self, mask=None):
        if mask is None:
            mask = (0xFFFFFFFF >> (32 - self.bitWidth))
        # TODO add new methods to replicate behavior in the logger
        self._c_log_write(mask)
        self._py_log_write(mask)
        if self.zz_reg.zz_rmio.accessed_flag_active:
            self.setAccessedFlag()
            self.zz_reg.setAccessedFlag()
        self.zz_reg.zz_rmio.setRegisterField(self.zz_reg, self, mask)

    def clr(self, mask=None):
        if mask is None:
            mask = (0xFFFFFFFF >> (32 - self.bitWidth))
        # TODO add new methods to replicate behavior in the logger
        self._c_log_write(mask)
        self._py_log_write(mask)
        if self.zz_reg.zz_rmio.accessed_flag_active:
            self.setAccessedFlag()
            self.zz_reg.setAccessedFlag()
        self.zz_reg.zz_rmio.clrRegisterField(self.zz_reg, self, mask)

    def tgl(self, mask=None):
        if mask is None:
            mask = (0xFFFFFFFF >> (32 - self.bitWidth))
        # TODO add new methods to replicate behavior in the logger
        self._c_log_write(mask)
        self._py_log_write(mask)
        if self.zz_reg.zz_rmio.accessed_flag_active:
            self.setAccessedFlag()
            self.zz_reg.setAccessedFlag()
        self.zz_reg.zz_rmio.tglRegisterField(self.zz_reg, self, mask)

    def getEnumNames(self):
        nameList = []
        if self.enum is not None:
            for key in self.enum.zz_edict:
                nameList.append(self.fullname + '.enum.' + key)
        return sorted(nameList)

    def isReadable(self):
        return self.access in [None, 'read-only', 'read-write', 'read-writeOnce']

    def isWriteable(self):
        return self.access in [None, 'write-only', 'read-write', 'writeOnce', 'read-writeOnce']

    def clearAccessedFlag(self):
        self.__dict__['zz_accessed_flag'] = False

    def setAccessedFlag(self):
        self.__dict__['zz_accessed_flag'] = True

    def getAccessedFlag(self):
        return self.__dict__['zz_accessed_flag']

    def getSliceStr(self):
        lsb = self.bitOffset
        fsize = self.bitWidth
        return u"{}:{}".format(lsb+fsize-1, lsb) if fsize > 1 else "{}".format(lsb)

    def includeInDump(self):
        self.zz_reg.includeInDump()

    def excludeFromDump(self):
        self.zz_reg.excludeFromDump()

    def dump(self, regValue, fieldCommentList):
        sliceStr = self.getSliceStr()
        fieldValue = GetFieldValue(regValue, self.bitOffset, self.bitWidth)
        if self.enum:
            fieldValueStr = u"{:#x} (enum {}: {})".format(fieldValue,
                                                          self.enum.getNameByValue(fieldValue),
                                                          self.enum.getDescByValue(fieldValue))
        else:
            fieldValueStr = u"{:#x}".format(fieldValue)
        line  = u"        # [{}] {}: {}\n".format(sliceStr, self.name, fieldValueStr)
        fieldCommentList.append([int(sliceStr.split(':')[0]), self.fullname, line])
