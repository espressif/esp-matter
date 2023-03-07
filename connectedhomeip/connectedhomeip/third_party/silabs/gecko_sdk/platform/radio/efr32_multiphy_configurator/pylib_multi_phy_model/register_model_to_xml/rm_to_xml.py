import sys
import os

if sys.version_info[0] < 3:
    from StringIO import StringIO
else:
    from io import StringIO

from host_py_rm_studio_internal.full.efr32xg21.revA0.static import Base_RM_Register, Base_RM_Field, Base_RM_Peripheral
from pylib_multi_phy_model.register_model_to_xml.Bindings import device, peripheralType, registerType, fieldType, enumeratedValuesType


class RMtoXML(object):

    def export_to_file(self, rm, filename, regFilterList=None):
        with open(filename, 'w') as outfile:
            return self._write_to_stream(rm, outfile, pretty_print=True, regFilterList=regFilterList)


    def export_to_string(self, rm, regFilterList=None):
        xml_string = None
        outfile = StringIO()  # this is an "in memory" file
        try:
            retur_val = self._write_to_stream(rm, outfile, pretty_print=False, regFilterList=regFilterList)
            if retur_val == 0:
                xml_string = outfile.getvalue()
        finally:
            outfile.close()
        return xml_string

    def _write_to_stream(self, rm, outstream, pretty_print=True, regFilterList=None):
        model = self._load_rm_into_model(rm, regFilterList)
        if self._validate():
            outstream.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n")
            model.export(outstream, 0, pretty_print=pretty_print)
            return 0
        else:
            sys.stderr.write("ERROR: Type data model is invalid. Unable to create XML.")
            return -1

    def _load_rm_into_model(self, rm, regFilterList=None):
        device_root = device()
        device_root.name = rm.zz_name
        device_root.description = rm.svdInfo.filename
        if regFilterList:
            for name in regFilterList:
                obj = rm.getObjectByName(name)
                if self._isinstance(obj, Base_RM_Peripheral.__name__):
                    self._load_peripheral(device_root, obj, load_regsiters=True)
                elif self._isinstance(obj, Base_RM_Register.__name__):
                    peripheral = self._load_peripheral(device_root, rm.zz_pdict[obj.pername], load_regsiters=False)
                    register = self._load_register(peripheral, obj, load_fields=True)
                elif self._isinstance(obj, Base_RM_Field.__name__):
                    obj = rm.getObjectByName(rm.getRegisterNameFromFieldName(name))
                    peripheral = self._load_peripheral(device_root, rm.zz_pdict[obj.pername], load_regsiters=False)
                    register = self._load_register(peripheral, obj, load_fields=False)
                    field_name = name.split('.')[-1]
                    field = self._load_field(register, obj.zz_fdict[field_name])
                else:
                    sys.stderr.write("WARN: Skipping invalid register name '{}'\n".format(name))
                    pass
        else:
            # no filter list, so export the whole register model
            for peripheral_name in sorted(rm.zz_pdict):
                rm_per = rm.zz_pdict[peripheral_name]
                self._load_peripheral(device_root, rm_per, load_regsiters=True)

        return device_root

    def _validate(self):
        # reserved for some future use
        return True

    def _load_peripheral(self, device_root, rm_per, load_regsiters=True):
        peripheral_already_loaded = False
        for peripheral in device_root.peripherals.peripheral:
            if rm_per.name == peripheral.name:
                peripheral_already_loaded = True
                return peripheral

        if not peripheral_already_loaded:
            peripheral = peripheralType()
            peripheral.name = rm_per.name
            peripheral.description = rm_per.description
            peripheral.baseAddress = rm_per.baseAddress

            if load_regsiters:
                for register_name in sorted(rm_per.zz_rdict):
                    rm_reg = rm_per.zz_rdict[register_name]
                    self._load_register(peripheral, rm_reg)

            device_root.peripherals.add_peripheral(peripheral)
            return peripheral

        return None

    def _load_register(self, peripheral, rm_reg, load_fields=True):
        register_already_loaded = False
        for register in peripheral.registers.register:
            if rm_reg.name == register.name:
                register_already_loaded = True
                return register

        if not register_already_loaded:
            register = registerType()
            register.name = rm_reg.name
            register.displayName = rm_reg.fullname
            register.description = rm_reg.description
            register.addressOffset = rm_reg.addressOffset
            register.access = rm_reg.access
            register.resetValue = rm_reg.resetValue
            register.resetMask = rm_reg.resetMask

            if rm_reg.zz_accessed_flag:
                register.modifiedWriteValues = rm_reg.io

            if load_fields:
                for field_name in sorted(rm_reg.zz_fdict):
                    rm_field = rm_reg.zz_fdict[field_name]
                    self._load_field(register, rm_field)

            peripheral.registers.add_register(register)
            return register

        return None

    def _load_field(self, register, rm_field):
        fields_already_loaded = False
        for field in register.fields.field:
            if rm_field.name == field.name:
                fields_already_loaded = True
                return field

        if not fields_already_loaded:
            field = fieldType()
            field.name = rm_field.name
            field.description = rm_field.description
            field.bitOffset = rm_field.bitOffset
            field.bitWidth = rm_field.bitWidth
            field.access = rm_field.access

            if rm_field.zz_accessed_flag:
                field.modifiedWriteValues = rm_field.io

            if rm_field.enum is not None:
                enumeratedValues = enumeratedValuesType()
                enumeratedValues.name = rm_field.enum.name
                enum_obj = field.enumeratedValues.add_enumeratedValues(enumeratedValues)
                # TODO: Add additional logic to handle enum values here
                pass

            register.fields.add_field(field)
            return field

        return None

    def _isinstance(self, obj, baseclassname):
        class_instance = getattr(obj, "__class__")
        for base in class_instance.__bases__:
            if base.__name__ == baseclassname:
                return True
        return False
