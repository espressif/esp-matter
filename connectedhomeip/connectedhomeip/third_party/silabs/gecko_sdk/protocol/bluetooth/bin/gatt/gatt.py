# Copyright 2020- Silicon Laboratories Inc. www.silabs.com
#
# The licensor of this software is Silicon Laboratories Inc. Your use of this
# software is governed by the terms of Silicon Labs Master Software License
# Agreement (MSLA) available at
# www.silabs.com/about-us/legal/master-software-license-agreement.
# This software is distributed to you in Source Code format and is governed
# by the sections of the MSLA applicable to Source Code.

import os
import uuid as py_uuid_module
import binascii
import struct

class GattConst:
    # BT SIG UUIDs
    UUID_PRIMARY_SERVICE          = "2800"
    UUID_SECONDARY_SERVICE        = "2801"
    UUID_INCLUDE_SERVICE          = "2802"
    UUID_GENERIC_ATTRIBUTE        = "1801"
    UUID_CHR                      = "2803"
    UUID_CHR_EXTENDED             = "2900"
    UUID_CHR_USER_DESCRIPTION     = "2901"
    UUID_CHR_CLIENT_CONFIG        = "2902"
    UUID_CHR_SERVER_CONFIG        = "2903"
    UUID_CHR_PRESENTATION_FORMAT  = "2904"
    UUID_CHR_AGGREGAT_FORMAT      = "2905"
    UUID_CHR_SERVICE_CHANGED      = "2a05"

    # Characteristic property flags
    CHR_PROPERTY_BROADCAST                     = 0x01
    CHR_PROPERTY_READ                          = 0x02
    CHR_PROPERTY_WRITENORESP                   = 0x04
    CHR_PROPERTY_WRITE                         = 0x08
    CHR_PROPERTY_NOTIFY                        = 0x10
    CHR_PROPERTY_INDICATE                      = 0x20
    CHR_PROPERTY_WRITESIGN                     = 0x40
    CHR_PROPERTY_EXTENDED                      = 0x80
    CHR_EXTENDED_PROPERTY_RELIABLE_WRITE       = 0x0001
    CHR_EXTENDED_PROPERTY_WRITABLE_AUXILIARIES = 0x0002

    # Attribute permission flags
    Read      = 0x0001
    Write               = 0x0002
    WriteNoResp         = 0x0004
    EncryptedRead       = 0x0010
    AuthenticatedRead   = 0x0020
    BondedRead          = 0x0040
    EncryptedWrite      = 0x0100
    AuthenticatedWrite  = 0x0200
    BondedWrite         = 0x0400
    Discoverable        = 0x0800
    EncryptedNotify     = 0x1000
    AuthenticatedNotify = 0x2000
    BondedNotify        = 0x4000
    Advertise           = 0x8000

    # Attribute value types
    ConstValue = 0
    DynamicValue = 1
    VariableLengthValue = 2
    ClientConfigValue = 3
    ServerConfigValue = 4
    Characteristic = 5
    ServiceInclude = 6
    UserValue = 7

    # Client config flags
    CientConfigDisabled = 0
    CientConfigNotification = 1
    CientConfigIndication = 2

    BaseUUID = py_uuid_module.UUID("00000000-0000-1000-8000-00805F9B34FB")

class GattError(Exception):
    def __init__(self, msg):
        self.msg = msg
    def __str__(self):
        return repr(self.msg)

def gatt_assert(cond, msg):
    if cond is False:
        raise GattError(msg)

def is_base_uuid(uuid):
    if uuid.bytes[:2] == GattConst.BaseUUID.bytes[:2] and uuid.bytes[4:] == GattConst.BaseUUID.bytes[4:]:
        return True
    else:
        return False

class BluetoothUuid(object):
    def __eq__(self, other):
        if other.__class__ != BluetoothUuid:
            other=BluetoothUuid(other)
        return (
            self.bytes == other.bytes
         )
    def __str__(self):
        return '0x'+binascii.hexlify(self.bytes).decode('ascii')
    def u16(self):
        return struct.unpack('<H',self.bytes)

    def __init__(self, hexstr):
        self.short = 0
        self.isshort = False
        self.bytes = None
        hexstr = hexstr.lower()
        if hexstr.startswith("0x"):
            hexstr = hexstr.replace("0x","", 1)
        if len(hexstr) <= 4:
            self.bytes = bytearray.fromhex(hexstr)
            self.bytes.reverse()
            self.isshort = True
            self.short = int(hexstr, 16)
        else:
            py_uuid = py_uuid_module.UUID(hexstr)
            if is_base_uuid(py_uuid):
                # downgrade base UUIDs
                self.bytes = bytearray(py_uuid.bytes[2:4])
                self.bytes.reverse()
                self.isshort = True
                self.short = int.from_bytes(self.bytes, "little")
            else:
                self.bytes = bytearray(py_uuid.bytes)
                self.bytes.reverse()



class GattValue(object):
    def __init__(self, type, value, maxlen = 0,datalen=0):
        self.type = type
        self.len = 0
        if value is not None:
            self.len = len(value)
        
        self.maxlen = self.len
        if type == GattConst.VariableLengthValue:
            self.maxlen = maxlen
        self.bytes = value
        self.datalen = datalen

class GattCapability():
    def __init__(self,id,value, enabled=True):
        self.id=id
        self.value=value
        self.enabled=enabled
    def __eq__(self, other):
        if other.__class__ != BluetoothUuid:
            other=GattCapability(other,0)
        return (
            self.id == other.id
         )

class GattAttribute(object):
    def __init__(self, uuid="0000", id="", properties=0, ext_properties=0, permissions=GattConst.Discoverable|GattConst.Read, state=0, value=None, capabilities=[], ids=[]):
        if(type(uuid)==str):
            uuid=BluetoothUuid(uuid)
        self.handle = 0
        self.uuid = uuid
        self.idstr = id
        self.properties = properties
        self.ext_properties = ext_properties
        self.permissions = permissions
        self.value = value
        self.state = state
        self.capabilities = capabilities
        self.ids = ids

    def capabilities_mask(self):
        v=0
        if(len(self.capabilities)==0):
            return 0xffff
        for c in self.capabilities:
            v+=c.value
        return v
        
class GattDescriptor(GattAttribute):
    def __init__(self):
        super(GattDescriptor, self).__init__()

class GattDescription(GattDescriptor):
    def __init__(self):
        super(GattDescription, self).__init__()
        self.uuid = BluetoothUuid(GattConst.UUID_CHR_USER_DESCRIPTION)

class GattAggregateFormat(GattDescriptor):
    def __init__(self):
        super(GattAggregateFormat, self).__init__()
        self.uuid = BluetoothUuid(GattConst.UUID_CHR_AGGREGAT_FORMAT)

class GattCharacteristic(GattAttribute):
    def __init__(self,uuid="0000",id="",properties=0,value=None,capabilities=[],permissions=GattConst.Discoverable|GattConst.Read):
        super(GattCharacteristic, self).__init__(uuid=uuid,id=id,properties=properties,value=value,capabilities=capabilities,permissions=permissions)
        self.descriptors = []

    def get_descriptor(self, uuid):
        return next((d for d in self.descriptors if d.uuid == uuid), None);
    
class GattService(GattAttribute):
    def __init__(self):
        super(GattService, self).__init__()
        self.permissions = GattConst.Read | GattConst.Discoverable
        self.secondary = False
        self.advertise = False
        self.include_service_ids = []
        self.chars = []
        self.start=0
        self.end=0
        self.description=""
    