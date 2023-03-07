
__all__ = [ 'Base_RM_Device' ]

from collections import OrderedDict
import io
import warnings
try:
    unicode = unicode
except NameError:
    # 'unicode' is undefined, must be Python 3
    str = str
    unicode = str
    bytes = bytes
    basestring = (str,bytes)
    long = int
else:
    # 'unicode' exists, must be Python 2
    str = str
    unicode = unicode
    bytes = str
    basestring = basestring
    long = long
from .. interface import IRegMapDevice
from .. common import Offline_AccessManager, \
    RegisterMapInterface, \
    RegMapAccessError, \
    RegMapNameError, \
    RegMapAddressError, \
    RegMapValueError
from .peripheral import Base_RM_Peripheral
from . register import Base_RM_Register
from . field import Base_RM_Field

class Base_RM_Device(IRegMapDevice):

    def __init__(self, rmio, label, name, svd_info):
        self.__dict__['zz_frozen'] = False
        self.zz_name = name
        self.zz_label = label
        self.zz_svd_info = svd_info
        if rmio is None:
            self.offline = True
            accessMgr = Offline_AccessManager(self.zz_label)
            self.zz_rmio = RegisterMapInterface(accessMgr.ReadRegister,
                                                accessMgr.WriteRegister,
                                                simulated=True)
        else:
            self.offline = False
            self.zz_rmio = rmio
        self.zz_pdict = OrderedDict()
        self.zz_reg_addr_to_name = {}
        self.zz_reg_addr_to_names = {}
        self.zz_dump_cb_functions = []

    def __setattr__(self, name, value):
        if self.__dict__['zz_frozen']:
            if name not in self.zz_pdict:
                raise AttributeError("ERROR: Invalid peripheral '{}'\n".format(name))
            else:
                raise AttributeError("ERROR: Unable to set '{}' to '{}'\n".format(name, value))
        else:
            self.__dict__[name] = value

    def __repr__(self):
        out = "{} ({} peripherals)\n".format(self.zz_name, len(self.zz_pdict))
        for key in sorted(self.zz_pdict):
            out += "  {}\n".format(key)
        return out

    @property
    def svdInfo(self):
        return self.zz_svd_info

    def addressToName(self, address):
        warnings.warn("addressToName() is deprecated; use addressToNames() for aliased register support",
                      DeprecationWarning, stacklevel=2)
        if not isinstance(address, (int, long)):
            raise RegMapAddressError("Invalid address '{}'".format(address))
        try:
            return self.zz_reg_addr_to_name[address]
        except KeyError:
            raise RegMapAddressError("No register found for address {:#010x}".format(address))

    def addressToNames(self, address):
        if not isinstance(address, (int, long)):
            raise RegMapAddressError("Invalid address '{}'".format(address))
        try:
            return self.zz_reg_addr_to_names[address]
        except KeyError:
            raise RegMapAddressError("No registers found for address {:#010x}".format(address))

    def updateAddressToNameCache(self, peripheral):
        """TODO: Remove the update of legacy dict once the addressToName() is removed."""
        if not isinstance(peripheral, Base_RM_Peripheral):
            raise RegMapNameError("Invalid peripheral attribute {}".format(peripheral))
        # prune legacy mapping, which uses single reg fullname string value
        for addr in list(self.zz_reg_addr_to_name.keys()):
            fullname = self.zz_reg_addr_to_name[addr]
            if fullname.split('.')[0] == peripheral.name:
                # remove this peripheral's register fullname from dict
                del self.zz_reg_addr_to_name[addr]
        # assign legacy mapping with peripheral register names
        for reg_name in peripheral.zz_rdict:
            reg = peripheral.zz_rdict[reg_name]
            self.zz_reg_addr_to_name[reg.baseAddress + reg.addressOffset] = reg.fullname
        # prune new mapping, which uses list of reg fullname strings value
        for addr in list(self.zz_reg_addr_to_names.keys()):
            for fullname in self.zz_reg_addr_to_names[addr]:
                if fullname.split('.')[0] == peripheral.name:
                    # remove this peripheral's register name from value list
                    self.zz_reg_addr_to_names[addr].remove(fullname)
                    # assign legacy mapping with peripheral
        # update new mapping with peripheral register names
        for reg_name in peripheral.zz_rdict:
            reg = peripheral.zz_rdict[reg_name]
            try:
                if reg.fullname not in self.zz_reg_addr_to_names[reg.baseAddress + reg.addressOffset]:
                    self.zz_reg_addr_to_names[reg.baseAddress + reg.addressOffset].append(reg.fullname)
            except KeyError:
                # need assert on invalid reads in addressToNames(), so can't use defaultdict
                self.zz_reg_addr_to_names[reg.baseAddress + reg.addressOffset] = [reg.fullname]

    def nameToAddress(self, name):
        reg_or_field = self.getObjectByName(name)
        return reg_or_field.baseAddress + reg_or_field.addressOffset

    def writeByName(self, name, value):
        if not isinstance(value, (int, long)):
            raise RegMapValueError("Invalid value {} for '{}'".format(value, name))
        self.getObjectByName(name).io = value

    def readByName(self, name):
        return self.getObjectByName(name).io

    def getObjectByName(self, name):
        if not isinstance(name, basestring):
            raise RegMapNameError("Invalid name '{}' must be str".format(name))
        items = name.split('.')
        if len(items) == 1:
            try:
                return self.zz_pdict[items[0]]
            except KeyError:
                raise RegMapNameError("Invalid peripheral name '{}'".format(name))
        elif len(items) == 2:
            try:
                return self.zz_pdict[items[0]].zz_rdict[items[1]]
            except KeyError:
                raise RegMapNameError("Invalid register name '{}'".format(name))
        elif len(items) == 3:
            try:
                return self.zz_pdict[items[0]].zz_rdict[items[1]].zz_fdict[items[2]]
            except KeyError:
                raise RegMapNameError("Invalid register field name '{}'".format(name))
        else:
            raise RegMapNameError("Invalid name '{}', must be PER, PER.REG or PER.REG.FIELD".format(name))

    def isReadable(self, name):
        obj = self.getObjectByName(name)
        return obj.isReadable()

    def isWriteable(self, name):
        obj = self.getObjectByName(name)
        return obj.isWriteable()

    def getRegisterNameFromFieldName(self, name):
        periods = len(name.split('.'))
        if periods == 2 or periods == 3:
            return '.'.join(name.split('.')[:2])
        else:
            raise RegMapNameError("Invalid name '{}', must be PER.REG or PER.REG.FIELD".format(name))

    def getPeripheralNames(self):
        return sorted(self.zz_pdict.keys())

    def getRegisterNames(self):
        nameList = []
        for key in sorted(self.zz_pdict):
            nameList.extend(self.zz_pdict[key].getRegisterNames())
        return nameList

    def forceRegister(self, name, value):
        if not self.offline:
            raise RegMapAccessError("Cannot directly assign within a live connection")
        reg = self.getObjectByName(name)
        if not isinstance(reg, Base_RM_Register):
            raise RegMapNameError("Name must be register name")
        self.zz_rmio.forceRegister(reg, value)

    def enableAccessedFlags(self):
        self.zz_rmio.enableAccessedFlags()

    def disableAccessedFlags(self):
        self.zz_rmio.disableAccessedFlags()

    def clearAccessedFlags(self):
        for key in sorted(self.zz_pdict):
            self.zz_pdict[key].clearAccessedFlags()

    def setAccessedFlags(self):
        for key in sorted(self.zz_pdict):
            self.zz_pdict[key].setAccessedFlags()

    def getAccessedRegisterNames(self):
        nameList = []
        for key in sorted(self.zz_pdict):
            nameList.extend(self.zz_pdict[key].getAccessedRegisterNames())
        return nameList

    def getAccessedFieldNames(self):
        nameList = []
        for key in sorted(self.zz_pdict):
            nameList.extend(self.zz_pdict[key].getAccessedFieldNames())
        return nameList

    @staticmethod
    def convertListToOrderedDict(inList, defaultValue=0):
        return OrderedDict([ (key, defaultValue) for key in inList])

    def writeData(self, dataDict):
        for key in dataDict:
            self.writeByName(key, dataDict[key])

    def readData(self, dataDict):
        for key in dataDict:
            dataDict[key] = self.readByName(key)

    def verifyData(self, expectedDict):
        diffDict = OrderedDict()
        for key in expectedDict:
            actualValue= self.readByName(key)
            if actualValue != expectedDict[key]:
                diffDict[key] = actualValue
        return diffDict

    def readAccessedRegisters(self):
        valueDict = OrderedDict()
        for key in self.getAccessedRegisterNames():
            valueDict[key] = self.readByName(key)
        return valueDict

    def readAccessedFields(self):
        valueDict = OrderedDict()
        for key in self.getAccessedFieldNames():
            valueDict[key] = self.readByName(key)
        return valueDict

    def getExcludedDumpRegisterNames(self):
        nameList = []
        for key in sorted(self.zz_pdict):
            nameList.extend(self.zz_pdict[key].getExcludedDumpRegisterNames())
        return nameList

    def excludeFromDumpByName(self, nameList):
        for name in nameList:
            obj = self.getObjectByName(name)
            try:
                obj.excludeFromDump()
            except AttributeError:
                raise RegMapNameError("Invalid name '{}', please use register or field name".format(name))

    def includeInDump(self):
        for key in self.zz_pdict:
            self.zz_pdict[key].includeInDump()

    def appendDumpDescCallback(self, cb_function):
        if not callable(cb_function):
            raise ValueError("'{}' is not callable".format(cb_function))
        self.zz_dump_cb_functions.append(cb_function)

    def clearDumpDescCallbacks(self):
        self.__dict__['zz_dump_cb_functions'] = []

    def buildRegFilterList(self, filename, listname='regFilterList'):
        filterList = []
        with io.open(filename, mode='w', encoding='utf-8') as outFH:
            outFH.write(u"\n# -*- coding: utf-8 -*- \n")
            outFH.write(u"\n# Register Map Register Name Filter List\n")
            outFH.write(u"\n{} = [\n".format(listname))
            for key in sorted(self.zz_pdict):
                self.zz_pdict[key].buildRegFilterList(outFH, filterList)
            outFH.write(u"]\n\n")
        return filterList

    def dump(self, filename, regFilterList=None, userMsg='', ignoreFailures=False):
        valueDict = OrderedDict()
        with io.open(filename, mode='w', encoding='utf-8') as outFH:
            outFH.write(u"\n# -*- coding: utf-8 -*- \n")
            for cb_function in self.zz_dump_cb_functions:
                desc = cb_function()
                for line in desc.split('\n'):
                    if line == '' or line.startswith('#'):
                        out_line = line + '\n'
                    else:
                        out_line = '# ' + line + '\n'
                    outFH.write(unicode(out_line))

            if userMsg:
                for line in userMsg.split('\n'):
                    if not line.startswith('#'):
                        outFH.write(u"# {}\n".format(line))
                    else:
                        outFH.write(u"{}\n".format(line))

            outFH.write(u"\nfrom collections import OrderedDict\n")
            outFH.write(u"\nREGISTER_DUMP = OrderedDict([\n")
            if regFilterList:
                for name in regFilterList:
                    obj = self.getObjectByName(name)
                    if isinstance(obj, Base_RM_Register):
                        obj.dump(outFH, valueDict, ignoreFailures=ignoreFailures)
                    elif isinstance(obj, Base_RM_Field):
                        obj = self.getObjectByName(self.getRegisterNameFromFieldName(name))
                        field_name = name.split('.')[-1]
                        obj.dump_field(outFH, valueDict, field_name, ignoreFailures=ignoreFailures)
                    else:
                        outFH.write(u"    # Skipping invalid name '{}'\n".format(name))
            else:
                # no filter list, so call peripheral dump with check for dump
                # flag, to allow skipping
                for key in sorted(self.zz_pdict):
                    self.zz_pdict[key].dump(outFH, valueDict, ignoreFailures=ignoreFailures)
            outFH.write(u"])\n\n")
        return valueDict

    def openCLangLogger(self, logger_fn, logger_header='',
                        logger_footer='', logger_label='rm_c_source'):
        self.zz_rmio.openCLangLogger(logger_fn, logger_header, logger_footer, logger_label)

    def enableCLangLogger(self):
        self.zz_rmio.enableCLangLogger()

    def disableCLangLogger(self):
        self.zz_rmio.disableCLangLogger()

    def closeCLangLogger(self):
        self.zz_rmio.closeCLangLogger()

    def openPyLangLogger(self, logger_fn, rm_obj_prefix='dut.rm.',
                         logger_header='', logger_footer='',
                         logger_label='rm_py_source'):
        self.zz_rmio.openPyLangLogger(logger_fn, rm_obj_prefix, logger_header,
                                      logger_footer, logger_label)

    def enablePyLangLogger(self):
        self.zz_rmio.enablePyLangLogger()

    def disablePyLangLogger(self):
        self.zz_rmio.disablePyLangLogger()

    def closePyLangLogger(self):
        self.zz_rmio.closePyLangLogger()

    def assignRawWordBlock(self, address, wordList):
        self.zz_rmio.assignRawWordBlock(address, wordList)