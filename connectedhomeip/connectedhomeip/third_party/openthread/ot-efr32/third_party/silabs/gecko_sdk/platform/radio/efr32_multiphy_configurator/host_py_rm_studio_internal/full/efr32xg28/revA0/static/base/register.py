
__all__ = ['Base_RM_Register']

import copy

from collections import OrderedDict
import sys
from .. interface import IRegMapRegister
from .. common import GetFieldValue
from .. common import Offline_AccessManager
from .. common import RegisterMapInterface


class Base_RM_Register(IRegMapRegister):

    def __init__(self, rmio, label, baseAddress, addressOffset, name, fullname,
                 access, description, resetValue, resetMask,
                 setOffset=None, clrOffset=None, tglOffset=None):
        self.__dict__['zz_frozen'] = False
        self.zz_rmio = rmio
        self.zz_label = label
        self.zz_fdict = OrderedDict()
        self.baseAddress = baseAddress
        self.addressOffset = addressOffset
        self.address = baseAddress + addressOffset
        self.setOffset = setOffset
        self.clrOffset = clrOffset
        self.tglOffset = tglOffset
        self.name = name
        self.fullname = fullname
        self.pername = fullname.split('.')[0]
        self.access = access
        self.description = description
        if resetValue is None:
            self.resetValue = 0
        else:
            self.resetValue = resetValue
        self.resetMask = resetMask
        self.zz_accessed_flag = False
        self.dumpActive = True
        self._addressToNamesCb = None

    def __repr__(self):
        out = "    {}\n".format(self.getDesc())
        field_list = []
        for key in sorted(self.zz_fdict):
            f = self.zz_fdict[key]
            lsb = f.bitOffset
            fsize = f.bitWidth
            sliceStr = "{}:{}".format(lsb+fsize-1, lsb) if fsize > 1 else "{}".format(lsb)
            line = "        [{}] {}: {} <{}>\n".format(sliceStr, f.name, f.description, f.access)
            field_list.append([int(sliceStr.split(':')[0]), line])
        for msb, line in sorted(field_list, reverse=True):
            out += line
        return out

    def __setattr__(self, name, value):
        if self.__dict__['zz_frozen']:
            if name == 'io':
                if self.zz_rmio.accessed_flag_active:
                    self.setAccessedFlag()
                self._c_log_write(value)
                self._py_log_write(value)
                self.zz_rmio.writeRegister(self, value)
            else:
                raise AttributeError("ERROR: Unable to set '{}' to '{}'".format(name, value))
        else:
            # self.__dict__[name] = value
            super().__setattr__(name, value)

    def set(self, value=None):
        if value is None:
            value = 0xFFFFFFFF
        if self.zz_rmio.accessed_flag_active:
            self.setAccessedFlag()
        self._c_log_write(value)
        self._py_log_write(value)
        return self.zz_rmio.setRegister(self, value)

    def clr(self, value=None):
        if value is None:
            value = 0xFFFFFFFF
        if self.zz_rmio.accessed_flag_active:
            self.setAccessedFlag()
        self._c_log_write(value)
        self._py_log_write(value)
        return self.zz_rmio.clrRegister(self, value)

    def tgl(self, value=None):
        if value is None:
            value = 0xFFFFFFFF
        if self.zz_rmio.accessed_flag_active:
            self.setAccessedFlag()
        self._c_log_write(value)
        self._py_log_write(value)
        return self.zz_rmio.tglRegister(self, value)

    def getOfflineCopy(self):
        self.__dict__['zz_frozen'] = False
        latest_value = self.io
        live_rmio = self.zz_rmio
        self.zz_rmio = None
        reg_copy = copy.deepcopy(self)
        reg_copy.__dict__['zz_frozen'] = False
        accessMgr = Offline_AccessManager(self.zz_label)
        reg_copy.zz_rmio = RegisterMapInterface(accessMgr.ReadRegister,
                                                accessMgr.WriteRegister,
                                                simulated=True)
        reg_copy.io = latest_value
        reg_copy.zz_rmio.writeRegister(reg_copy, latest_value)
        # remove binding to main device instance
        reg_copy.registerAddressToNamesCb(None)
        reg_copy.__dict__['zz_frozen'] = True
        self.zz_rmio = live_rmio
        self.__dict__['zz_frozen'] = True
        return reg_copy

    def _c_log_read(self):
        if self.zz_rmio.c_logger_active:
            self.zz_rmio.c_logger.debug("    value = {}->{};".format(self.pername, self.name))

    def _py_log_read(self):
        if self.zz_rmio.py_logger_active:
            self.zz_rmio.py_logger.debug("    print({}{}.{}.io)".format(self.zz_rmio.py_rm_obj_prefix,
                                                                        self.pername, self.name))

    def _c_log_write(self, value):
        if self.zz_rmio.c_logger_active:
            self.zz_rmio.c_logger.debug("    {}->{} = {:#010x};".format(self.pername, self.name, value))

    def _py_log_write(self, value):
        if self.zz_rmio.py_logger_active:
            self.zz_rmio.py_logger.debug("    {}{}.{}.io = {:#010x}".format(self.zz_rmio.py_rm_obj_prefix,
                                                                            self.pername, self.name, value))

    def _getio(self):
        self._c_log_read()
        self._py_log_read()
        return self.zz_rmio.readRegister(self)

    def _setio(self, value):
        # unused due to the __setattr__ logic above
        pass

    io = property(_getio, _setio)

    def registerAddressToNamesCb(self, addressToNamesCb):
        self.__dict__['zz_frozen'] = False
        self._addressToNamesCb = addressToNamesCb
        self.__dict__['zz_frozen'] = True

    def getAliasedNames(self):
        if self._addressToNamesCb:
            aliases = self._addressToNamesCb(self.address)  # type: list
            self_idx = aliases.index(self.fullname)
            return aliases[:self_idx] + aliases[self_idx+1:]
        else:
            return []

    def isAliased(self):
        return self.getAliasedNames() != []

    def getFieldNames(self):
        nameList = []
        for key in sorted(self.zz_fdict):
            nameList.append(self.zz_fdict[key].fullname)
        return sorted(nameList)

    def isReadable(self):
        return self.access in [None, 'read-only', 'read-write', 'read-writeOnce']

    def isWriteable(self):
        return self.access in [None, 'write-only', 'read-write', 'writeOnce', 'read-writeOnce']

    def clearAccessedFlag(self):
        # clear the overall register access and all the member fields
        self.__dict__['zz_accessed_flag'] = False
        for key in sorted(self.zz_fdict):
            self.zz_fdict[key].clearAccessedFlag()

    def setAccessedFlag(self):
        self.__dict__['zz_accessed_flag'] = True

    def getAccessedFlag(self):
        return self.__dict__['zz_accessed_flag']

    def getAccessedFieldNames(self):
        nameList = []
        # access the whole register
        # only partial access
        for key in sorted(self.zz_fdict):
            if self.zz_fdict[key].getAccessedFlag():
                nameList.append(self.zz_fdict[key].fullname)
        return nameList

    def getDesc(self):
        flagStr = "**" if self.getAccessedFlag() else ""
        return u"{:#010x}  {}{}: {} <{}>".format(self.baseAddress + self.addressOffset,
                                                 self.name, flagStr,
                                                 self.description, self.access)

    def assignRegDefault(self):
        self.zz_rmio.assignRegisterDefault(self)

    def includeInDump(self):
        self.__dict__['dumpActive'] = True

    def excludeFromDump(self):
        self.__dict__['dumpActive'] = False

    def buildRegFilterList(self, outFH, filterList):
        if self.isReadable():
            filterList.append(self.fullname)
            # store both the name and desc string to help user
            outFH.write(u"    '{}',\n".format(self.fullname))
            outFH.write(u"      # {}\n".format(self.getDesc()))

    def dump(self, outFH=None, valueDict=None, ignoreFailures=False):
        if self.isReadable():
            if outFH is None:
                outFH = sys.stdout
            if valueDict is None:
                valueDict = {}
            # The initialization could fall back to simple read function,
            # so inspect the return type.  The old method will raise an
            # exception here.
            if ignoreFailures:
                self._c_log_read()
                self._py_log_read()
                result = self.zz_rmio.dumpRegister(self)
            else:
                result = self.io

            if isinstance(result, list):
                [errMsg, regValue] = result
            else:
                errMsg = ""
                regValue = result

            if errMsg == "":
                # read was successful, so log register and field details
                outFH.write(u"    ('{}', {:#010x}),\n".format(self.fullname, regValue))
                outFH.write(u"      # {}\n".format(self.getDesc()))
                valueDict[self.fullname] = regValue
                fieldCommentList = []
                for key in sorted(self.zz_fdict):
                    self.zz_fdict[key].dump(regValue, fieldCommentList)
                for msb, fullname, line in sorted(fieldCommentList, reverse=True):
                    outFH.write(line)
            else:
                # read failed
                outFH.write(u"    # skipping '{}' due to access error\n".format(self.fullname))

    def dump_field(self, outFH, valueDict, field_name, ignoreFailures=False):
        if self.isReadable():
            if ignoreFailures:
                self._c_log_read()
                self._py_log_read()
                result = self.zz_rmio.dumpRegister(self)
            else:
                result = self.io
            if isinstance(result, list):
                [errMsg, regValue] = result
            else:
                errMsg = ""
                regValue = result

            if errMsg == "":
                # read was successful, so log field value and details
                fieldObj = self.zz_fdict[field_name]
                fieldValue = GetFieldValue(regValue, fieldObj.bitOffset, fieldObj.bitWidth)
                outFH.write(u"    ('{}.{}', {:#x}),\n".format(self.fullname,
                                                              field_name,
                                                              fieldValue))
                outFH.write(u"      # {}\n".format(self.getDesc()))
                valueDict[self.fullname + '.' + field_name] = fieldValue
                fieldCommentList = []
                fieldObj.dump(regValue, fieldCommentList)
                outFH.write(fieldCommentList[0][2])
            else:
                # read failed
                outFH.write(u"    # skipping '{}.{}' due to access error\n".format(self.fullname, field_name))
