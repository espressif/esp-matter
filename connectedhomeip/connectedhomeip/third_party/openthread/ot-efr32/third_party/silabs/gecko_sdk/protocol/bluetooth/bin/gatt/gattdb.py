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
from gatt import BluetoothUuid, GattConst, GattValue, GattAttribute, gatt_assert, GattService, GattCharacteristic

class GattDatabase(object):
    class ServiceItem(object):
        def __init__(self, attributes, start):
            self.attributes = attributes
            self.start_handle = start
            self.end_handle = start + len(attributes)
    
    def __init__(self):
        self.name = ""
        self.db_service_items = []
        self.cfile = ""
        self.hfile = ""
        self.header_macro = ""
        self.id_prefix = ""
        self.is_default = False
        self.add_generic_attribute_service = False
        self.uuid16s = []
        self.uuid128s = []
        self.attribute_rows = []
        self.num_ccfg=0
        self.capabilities= []
        self.capabilities_sum= 0
    def capabilities_mask(self):
        if(len(self.capabilities)==0):
            return 0xffff

        m = 0
        for c in self.capabilities:
            m|=c.value
        return m
    def capabilities_enabled(self):
        if(len(self.capabilities)==0):
            return 0xffff
        
        m = 0
        for c in self.capabilities:
            if c.enabled:
                m|=c.value
        return m       
    def create_generic_attribute_service(self):
        gsrv = GattService()
        gsrv.uuid = BluetoothUuid(GattConst.UUID_GENERIC_ATTRIBUTE)
        gsrv.idstr = "generic_attribute"
        gsrv.capabilities=self.capabilities

         #service->caps = db->caps_declare;
        gsrv.chars.append(GattCharacteristic(uuid="2a05",id="service_changed_char",properties=GattConst.CHR_PROPERTY_INDICATE,value=GattValue(GattConst.DynamicValue, bytearray(4)),permissions=GattConst.Discoverable,capabilities=self.capabilities))
        gsrv.chars.append(GattCharacteristic(uuid="2b2a",id="database_hash",properties=GattConst.CHR_PROPERTY_READ,value=GattValue(GattConst.DynamicValue, bytearray(16)),capabilities=self.capabilities))
        gsrv.chars.append(GattCharacteristic(uuid="2b29",id="client_support_features",properties=GattConst.CHR_PROPERTY_READ|GattConst.CHR_PROPERTY_WRITE,value=GattValue(GattConst.DynamicValue, bytearray(1)),permissions=GattConst.Discoverable|GattConst.Read|GattConst.Write,capabilities=self.capabilities))

        return gsrv

    def set_services(self, services,add_generic_attribute_service=True):
        self.uuid16s = [BluetoothUuid(GattConst.UUID_PRIMARY_SERVICE), BluetoothUuid(GattConst.UUID_SECONDARY_SERVICE), BluetoothUuid(GattConst.UUID_CHR)]
        self.uuid128s = []
        self.services = services

        #first add all uuids, this is to replicate bgbuild behaviour
        for s in self.services:
            self.__add_uuids_in_service(s)

        #create generic service
        if(add_generic_attribute_service):
            s=self.create_generic_attribute_service()
            self.__add_uuids_in_service(s)
            self.services=[s]+self.services
        #Create all attributes
        self.__build_atrribute_rows()

        #link includes
        #for s in self.services:
        #    self.__resolve_service_relations(s, services)
        for a in self.attribute_rows:
            #find include attributes
            if a.uuid==BluetoothUuid(GattConst.UUID_INCLUDE_SERVICE):
                included = next((s for s in services if s.idstr == a.include), None)
                gatt_assert(included != None, "Included service " + a.include + " does not exist!")
                #gatt_assert(included != service, "Included service " + id + " cannot be self!")

                a.include=included
                a.value=GattValue(GattConst.ServiceInclude,bytearray())


    def __uuid_index(self, uuid):
        if uuid.isshort:
            for idx, item in enumerate(self.uuid16s):
                if item.bytes == uuid.bytes:
                    return idx
        else:
            for idx, item in enumerate(self.uuid128s):
                if item.bytes == uuid.bytes:
                    return idx
        return -1

    def __build_atrribute_rows(self):
        self.attribute_rows = []
        for s in self.services:
            self.attribute_rows+=(self.__build_service(s))

        def __find_attribute_index(id):
            i = 0
            for a in self.attribute_rows:
                i+=1
                if(a.idstr == id):
                    return i
            return 0
        #build indexes
        idx=0
        for a in self.attribute_rows:
            if(a.value!=None and ((a.value.type == GattConst.ClientConfigValue)or(a.value.type == GattConst.ClientConfigValue))):
                a.value.bytes.append(self.num_ccfg)
                self.num_ccfg+=1
            if(a.uuid==GattConst.UUID_CHR_AGGREGAT_FORMAT):
                data = bytearray()
                for id in a.ids:
                    i = __find_attribute_index(id)
                    gatt_assert(i!=0, "Aggregated characteristic id " + id + " does not exist!")
                    data.append(i&0xff)
                    data.append((i>>8)&0xff)
                a.value = GattValue(GattConst.ConstValue, data)
            a.handle=idx+1
            idx+=1

            
    def __build_service(self, service):
        rows = []
        service.start=len(self.attribute_rows)+1
        permissions=GattConst.Read|GattConst.Discoverable
        # service declaration attribute:
        if service.secondary is True:
            uuid = BluetoothUuid(GattConst.UUID_SECONDARY_SERVICE)
        else:
            uuid = BluetoothUuid(GattConst.UUID_PRIMARY_SERVICE)
            if service.advertise is True:
                permissions|=GattConst.Advertise
        value = GattValue(GattConst.ConstValue, service.uuid.bytes)
        service_declare = GattAttribute(uuid=uuid, permissions=permissions, value=value, capabilities=service.capabilities, id=service.idstr)
        rows.append(service_declare)
        
        #service includes
        for inc in service.include_service_ids:
            a=GattAttribute(uuid=GattConst.UUID_INCLUDE_SERVICE, permissions=GattConst.Read|GattConst.Discoverable, capabilities=service.capabilities)
            a.include=inc
            rows.append(a)
            self.__add_uuid(a.uuid)

                
        #characteristic
        if service.chars is not None:
            for ch in service.chars:
                rows+=(self.__build_characteristic_db(ch, len(self.attribute_rows)+len(rows)+1))
        
        service.end=service.start+len(rows)-1

        return rows

    def uuid_handle(self,uuid):
        if(not isinstance(uuid,BluetoothUuid)):
            uuid=BluetoothUuid(uuid) 
        try:
            if(uuid.isshort):
                return self.uuid16s.index(uuid)
            return self.uuid128s.index(uuid)|0x8000
        except ValueError:
            print("Error, UUID not found:",str(uuid))
            return 0

    def __build_characteristic_db(self, ch, start_handle):
        #characteristic declaration attribute:
        rows=[]
        value = GattValue(GattConst.Characteristic, bytearray())
        char_declare = GattAttribute(uuid=ch.uuid, value=value, properties=ch.properties, capabilities=ch.capabilities)
        rows.append(char_declare)
        #characteristic value:
        a=GattAttribute(permissions=ch.permissions, id=ch.idstr, value=ch.value, properties=ch.properties, uuid=ch.uuid, capabilities=ch.capabilities)
        rows.append(a)

        #client config:
        #Add clientconfig, only if indicate and notify supported
        if (ch.properties & (GattConst.CHR_PROPERTY_INDICATE | GattConst.CHR_PROPERTY_NOTIFY) and ch.get_descriptor(GattConst.UUID_CHR_CLIENT_CONFIG)==None):
            #add uuid if not exist already
            self.__add_uuid(GattConst.UUID_CHR_CLIENT_CONFIG)
            flags=GattConst.CientConfigDisabled
            if ch.properties & GattConst.CHR_PROPERTY_NOTIFY:
                flags|=GattConst.CientConfigNotification
            if ch.properties & GattConst.CHR_PROPERTY_INDICATE:
                flags|=GattConst.CientConfigIndication
            data=bytearray([flags])
            permissions=GattConst.Write|GattConst.Read|GattConst.Discoverable

            #  Permission inheritance:
            if (ch.permissions & GattConst.EncryptedNotify):
                permissions |= GattConst.EncryptedWrite
            
            if (ch.permissions & GattConst.AuthenticatedNotify): 
                permissions |= GattConst.AuthenticatedWrite
                
            if (ch.permissions & GattConst.BondedNotify):
                permissions |= GattConst.BondedWrite

            a=GattAttribute(permissions=permissions, value=GattValue(GattConst.ClientConfigValue,data), uuid=GattConst.UUID_CHR_CLIENT_CONFIG, capabilities=ch.capabilities)
            rows.append(a)
        #Add serverconfig, only if broadcast supported
        if (ch.properties & GattConst.CHR_PROPERTY_BROADCAST):
            permissions=GattConst.Write
            
            if (ch.permissions & GattConst.AuthenticatedRead): 
                permissions |= GattConst.AuthenticatedRead
            data = bytearray()
            data.append(ch.ext_properties&GattConst.CHR_PROPERTY_BROADCAST)
            data.append((ch.ext_properties>>8)&0xff)
            a=GattAttribute(permissions=permissions, value=GattValue(GattConst.ServerConfigValue,data), uuid=GattConst.UUID_CHR_SERVER_CONFIG, capabilities=ch.capabilities)
            rows.append(a)
        #descriptors
        extended_properties_set = False
        try:
            for d in ch.descriptors:
                if(d.uuid==GattConst.UUID_CHR_CLIENT_CONFIG) and d.value.type!=GattConst.UserValue:
                    if ch.properties &(GattConst.CHR_PROPERTY_INDICATE | GattConst.CHR_PROPERTY_NOTIFY) == 0:
                        ch.properties|=GattConst.CHR_PROPERTY_NOTIFY
                    flags=GattConst.CientConfigDisabled
                    if ch.properties & GattConst.CHR_PROPERTY_NOTIFY:
                        flags|=GattConst.CientConfigNotification
                    if ch.properties & GattConst.CHR_PROPERTY_INDICATE:
                        flags|=GattConst.CientConfigIndication
                    data=bytearray([flags])
                    a=GattAttribute(permissions=d.permissions, value=GattValue(GattConst.ClientConfigValue,data), properties=d.properties, uuid=GattConst.UUID_CHR_CLIENT_CONFIG, capabilities=ch.capabilities)
                    rows.append(a)
                else:
                    handle = start_handle + len(rows) 
                    #print(d.value.type)
                    a=GattAttribute(permissions=d.permissions, id=d.idstr, value=d.value, properties=d.properties, uuid=d.uuid, capabilities=ch.capabilities, ids=d.ids)
                    rows.append(a)
                if(d.uuid==GattConst.UUID_CHR_EXTENDED):
                    extended_properties_set = True
        except AttributeError as e:
            print(e)
            pass
        # has extended properties and not defined yet, add
        if (ch.properties & GattConst.CHR_PROPERTY_EXTENDED) and extended_properties_set==False:
            self.__add_uuid(GattConst.UUID_CHR_EXTENDED)
            data = bytearray()
            data.append(ch.ext_properties&0xff)
            data.append((ch.ext_properties>>8)&0xff)
            a=GattAttribute(permissions=GattConst.Read,value=GattValue(GattConst.ConstValue, data),uuid=GattConst.UUID_CHR_EXTENDED,capabilities=ch.capabilities)
            rows.append(a)
        return rows
    
    def __add_uuids_in_service(self, service):
        for ch in service.chars:
            self.__add_uuid(ch.uuid)
            for desc in ch.descriptors:
                self.__add_uuid(desc.uuid)
            
    def __add_uuid(self, uuid):
        if type(uuid)is not BluetoothUuid:
            uuid=BluetoothUuid(uuid)
        
        if self.__uuid_index(uuid) == -1:
            if uuid.isshort:
                try:
                    self.uuid16s.index(uuid)
                except ValueError:
                    self.uuid16s.append(uuid)
            else:
                try:
                    self.uuid128s.index(uuid)
                except ValueError:
                    self.uuid128s.append(uuid)
        return 1