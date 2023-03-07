# Copyright 2020- Silicon Laboratories Inc. www.silabs.com
#
# The licensor of this software is Silicon Laboratories Inc. Your use of this
# software is governed by the terms of Silicon Labs Master Software License
# Agreement (MSLA) available at
# www.silabs.com/about-us/legal/master-software-license-agreement.
# This software is distributed to you in Source Code format and is governed
# by the sections of the MSLA applicable to Source Code.

import os
import sys
import xml.etree.ElementTree as ET
from gatt import GattCapability,GattConst, GattService, GattCharacteristic, GattAggregateFormat, GattDescriptor, GattDescription, GattAttribute, GattValue, BluetoothUuid
from gattdb import GattDatabase
import traceback
ATTRIBUTE_VALUE_MAX_SIZE = 512

class GattXmlError(Exception):
    def __init__(self, msg):
        self.msg = msg
    def __str__(self):
        return repr(self.msg)

def gatt_xml_assert(cond, msg):
    if cond is False:
        raise GattXmlError(msg)
     
class GattXmlParser(object):
    def __init__(self):
        self.gattdbs = []

    def to_string(self, xml):
        tree = ET.parse(xml)
        return ET.tostring(tree.getroot(), encoding="unicode")

    def merge(self, orddict):
        """
        Merge xml contents.
        This method puts all children of 'gatt' of each xml under a
        single 'gatt' parent.
        Additionally it merges all children of 'capabilities_declare'
        nodes under a single node.
        """
        rs = None
        rd = {}
        xml_tree = None
        try:
            for k, v in orddict.items():
                root = ET.fromstring(v)
                # if 'gatt' is the root node, create 'project' node and make it the root
                if root.tag == "gatt":
                    nroot = ET.Element("project")
                    nroot.append(root)
                    root = nroot
                for g in root.iter("gatt"):
                    if xml_tree is None:
                        xml_tree = root
                        insertion_point = xml_tree.findall("gatt")[0]
                    else:
                        self.combine(insertion_point, g)
            if xml_tree:
                # merge 'capabilities_declare' nodes into a single node
                # move children of 'capabilities_declare's under first
                # 'capabilities_declare' node
                cap_first = True
                for g in xml_tree.iter("gatt"):
                    cs = g.findall("capabilities_declare")
                    for c in cs:
                        if cap_first:
                            cap_first = False
                            # first 'capabilities_declare' is the insertion point
                            insertion_point = cs[0]
                        else:
                            # copy children of other 'capabilities_declare'
                            # node into first
                            insertion_point.extend(c)
                            # remove other 'capabilities_declare' node
                            g.remove(c)
                rs = ET.tostring(xml_tree, encoding="unicode")
                rd = {"success": True,
                      "message": ""}
            else:
                rs = None
                rd = {"success": False,
                      "message": "Error: No 'gatt' node found in xml!"}
        except:
            tr_type, tr_value, tr_traceback = sys.exc_info()
            rs = None
            rd = {"success": False,
                  "message": "".join(traceback.format_exception(tr_type, tr_value, tr_traceback))}
        finally:
            return rs, rd

    def combine(self, host, guest):
        # iterate on guest's child nodes
        for ng in guest:
            nh = None
            # find a node among host's child nodes which is identical to guest's child node
            for node in host:
                if (node.tag == ng.tag) and ("instance_id" in node.attrib) and ("instance_id" in ng.attrib) \
                and (node.get("instance_id") == ng.get("instance_id")) and (node.get("uuid", "00") == ng.get("uuid", "00")):
                    gatt_xml_assert(node.attrib == ng.attrib, "instance_id attributes match, but nodes are not identical: {}, {}".format(node, ng))
                    gatt_xml_assert(nh is None, "multiple match found: {}".format(node))
                    nh = node
            # check for match
            if nh is None:
                # no match found, append guest's child node to host
                host.append(ng)
            elif len(ng) != 0:
                # if 'ng' has child nodes continue the process recursively
                self.combine(nh, ng)
            else:
                # nodes 'ng' and 'nh' are identical, update text if empty
                if nh.text is None:
                    nh.text = ng.text

    def parse_string(self, xmlstr):
        rg = None
        rd = {}
        try:
            root = ET.fromstring(xmlstr)
            # find all 'gatt' nodes
            gattl = root.findall("gatt")
            # exactly one 'gatt' node is allowed
            if len(gattl) == 0 or len(gattl) > 1:
                rg = None
                rd = {"success": False,
                      "message": "Error: exactly one 'gatt' node is allowed!"}
            else:
                rg = self.parse_gattdb(gattl[0])
                rd = {"success": True,
                      "message": ""}
        except:
            tr_type, tr_value, tr_traceback = sys.exc_info()
            rg = None
            rd = {"success": False,
                  "message": "".join(traceback.format_exception(tr_type, tr_value, tr_traceback))}
        finally:
            return rg, rd

    def parse(self, xmls):
        for x in xmls:
            tree = ET.parse(x)
            root = tree.getroot()
            gnodes = root.iter('gatt')
            for gnd in gnodes:
                db = self.parse_gattdb(gnd)
        return self.gattdbs

    def parse_gattdb(self, xml):
        db = GattDatabase()
        db.cfile = xml.get('out', "gatt_db.c")
        db.hfile = xml.get('header', "gatt_db.h")
        db.header_macro = "__" + db.hfile.split('/')[-1].replace(".", "_").upper()
        db.is_default = xml.get('default', "true") == "true"
        db.id_prefix = xml.get('prefix', "")
        db.name = xml.get('db_name', "gattdb")
        db.add_generic_attribute_service = xml.get('generic_attribute_service', "false") == "true"
        services = []
        #parse capabilities declaration
        v=1
        db.capabilities=[]
        for dnode in xml.iterfind('capabilities_declare'):
            for cnode in dnode.iterfind('capability'):
                cap=GattCapability(cnode.text,v,cnode.get('enable', "true")=="true")
                db.capabilities.append(cap)
                db.capabilities_sum |= v
                v*=2
            

        for snode in xml.iterfind('service'):
            if snode.get('uuid', "00") == "1801":
                if db.add_generic_attribute_service:
                    print ("WARNING: XML node for service 1801 is ignored. A Generic Attribute Service will",
                           " be automatically generated at the beginning of the GATT database.")
                    continue
                gatt_xml_assert(len(services) == 0, "Error: Generic Attribute Service should be the first service in GATT database. Otherwise its attribute handles may change when the database is updated in future.")
                cnodes = snode.iterfind('characteristic')
                gatt_xml_assert(cnodes != None, "Error: Generic Attribute Service can't be empty.")
                index = 0
                for cnode in cnodes:
                    c_uuid = int(cnode.get('uuid', "00"), 16)
                    if c_uuid == 0x2a05:
                        gatt_xml_assert(index == 0, "Error: Service change characteristic should be the first characteristic of Generic Attribute Service.")
                    index += 1

            services.append(self.parse_service(snode,db.capabilities))
        db.set_services(services,db.add_generic_attribute_service)
        self.gattdbs.append(db)
        return db

    def parse_capabilities(self,xml,parent_caps):
        ncaps=None
        for dnode in xml.iterfind('capabilities'):
            ncaps=[]
            for cnode in dnode.iterfind('capability'):
                i=parent_caps.index(cnode.text)
                #TODO: check not found
                enabled=cnode.get('enable', "true")=="true"
                ncaps.append(GattCapability(cnode.text,parent_caps[i].value,enabled))
        #no new capabilities, inherit from parent
        if(ncaps==None):
            ncaps=parent_caps
        return ncaps

    def parse_service(self, xml,parent_caps):
        s = GattService()
        s.uuid = BluetoothUuid(xml.get('uuid', "00"))
        s.idstr = xml.get('id', "")
        s.secondary = xml.get('type', "primary") == "secondary"
        s.advertise = s.secondary is False and xml.get('advertise', "false") == "true"
        s.capabilities=self.parse_capabilities(xml,parent_caps)
        # description of service is informative and not exposed in the actual GATT database
        if xml.find('description') is not None:
            s.description = xml.find('description').text

        includes = xml.iterfind('include')
        for i in includes:
            s.include_service_ids.append(i.get('id', ""))

        cnodes = xml.iterfind('characteristic')
        for c in cnodes:
            s.chars.append(self.parse_characteristic(c,s.capabilities))
        return s

    def parse_characteristic(self, xml,parent_caps):
        c = GattCharacteristic()
        self.__parse_attrobj(xml, c)
        c.capabilities=self.parse_capabilities(xml,parent_caps)
        dn = xml.find('description')
        if dn is not None:
            d = GattDescription()
            self.__parse_attrobj(dn, d)
            valid_properties = (d.properties & ~(GattConst.CHR_PROPERTY_READ|GattConst.CHR_PROPERTY_WRITE)) == 0
            gatt_xml_assert(valid_properties, "Characteristic descriptor can only have readable and writable properties")
            c.descriptors.append(d)
            if d.properties & GattConst.CHR_PROPERTY_WRITE:
                c.properties |= GattConst.CHR_PROPERTY_EXTENDED
                c.ext_properties |= GattConst.CHR_EXTENDED_PROPERTY_WRITABLE_AUXILIARIES

        for n in xml.iterfind('descriptor'):
            d = GattDescriptor()
            self.__parse_attrobj(n, d)
            valid_properties = (d.properties & ~(GattConst.CHR_PROPERTY_READ|GattConst.CHR_PROPERTY_WRITE)) == 0
            gatt_xml_assert(valid_properties, "Characteristic descriptor can only have readable and writable properties")
            c.descriptors.append(d)
            # extended properties descriptor needs extended property
            if d.uuid == GattConst.UUID_CHR_EXTENDED:
                c.properties |= GattConst.CHR_PROPERTY_EXTENDED

        aggxml = xml.find('aggregate')
        if aggxml is not None:
            agg = GattAggregateFormat()
            self.__parse_attrobj(aggxml, agg)
            cond = (agg.properties == GattConst.CHR_PROPERTY_READ) and (agg.permissions == GattConst.Read | GattConst.Discoverable)
            gatt_xml_assert(cond, "Aggregate attribute must be readable without authentication")
            for a in aggxml.iterfind('attribute'):
                agg.ids.append(a.get('id', ""))
            gatt_xml_assert(len(agg.ids) > 1, "Aggregate arribute list must be at least 2 attributes long.")
            c.descriptors.append(agg)

        return c

    def parse_read_property(self, propxml):
        read = 0
        permit = 0

        permissions = propxml.find('read')
        if (permissions is not None):
            read |= GattConst.CHR_PROPERTY_READ
            permit |= GattConst.Read
            if permissions.get("encrypted","false") == "true":
                permit |= GattConst.EncryptedRead
            if permissions.get("authenticated","false") == "true":
                permit |= GattConst.AuthenticatedRead
            if permissions.get("bonded","false") == "true":
                permit |= GattConst.BondedRead

        # Backward compatibility to old property+permission mix attributes:
        if propxml.get('read', "false") == "true":
            read |= GattConst.CHR_PROPERTY_READ
            permit |= GattConst.Read
        if propxml.get("encrypted_read","false") == "true":
            read |= GattConst.CHR_PROPERTY_READ
            permit |= GattConst.EncryptedRead
        if propxml.get("authenticated_read","false") == "true":
            read |= GattConst.CHR_PROPERTY_READ
            permit |= GattConst.AuthenticatedRead
        if propxml.get("bonded_read","false") == "true":
            read |= GattConst.CHR_PROPERTY_READ
            permit |= GattConst.BondedRead

        return read, permit

    def parse_write_property(self, propxml):
        write = 0
        permit = 0
        ext_properties = 0

        permissions = propxml.find('write')
        if (permissions is not None):
            write |= GattConst.CHR_PROPERTY_WRITE
            permit |= GattConst.Write
            if permissions.get("encrypted","false") == "true":
                permit |= GattConst.EncryptedWrite
            if permissions.get("authenticated","false") == "true":
                permit |= GattConst.AuthenticatedWrite
            if permissions.get("bonded","false") == "true":
                permit |= GattConst.BondedWrite

        # Write, write_no_response and reliable_write should share the same
        # permissions. But if some inconsistencies end up in the xml for
        # some reason, we just raise the security level as a workaround.
        # See BG-5861
        permissions = propxml.find('write_no_response')
        if (permissions is not None):
            write |= GattConst.CHR_PROPERTY_WRITENORESP
            permit |= GattConst.WriteNoResp
            if permissions.get("encrypted","false") == "true":
                permit |= GattConst.EncryptedWrite
            if permissions.get("authenticated","false") == "true":
                permit |= GattConst.AuthenticatedWrite
            if permissions.get("bonded","false") == "true":
                permit |= GattConst.BondedWrite
        permissions = propxml.find('reliable_write')
        if (permissions is not None):
            write |= GattConst.CHR_PROPERTY_EXTENDED
            ext_properties |= GattConst.CHR_EXTENDED_PROPERTY_RELIABLE_WRITE
            if permissions.get("encrypted","false") == "true":
                permit |= GattConst.EncryptedWrite
            if permissions.get("authenticated","false") == "true":
                permit |= GattConst.AuthenticatedWrite
            if permissions.get("bonded","false") == "true":
                permit |= GattConst.BondedWrite

        # Backward compatibility to old property+permission mix attributes:
        if propxml.get('write', "false") == "true":
            write |= GattConst.CHR_PROPERTY_WRITE
            permit |= GattConst.Write
        if propxml.get("write_no_response","false") == "true":
            write |= GattConst.CHR_PROPERTY_WRITENORESP
            permit |= GattConst.WriteNoResp
        if propxml.get("reliable_write","false") == "true":
            write |= GattConst.CHR_PROPERTY_EXTENDED
            ext_properties |= GattConst.CHR_EXTENDED_PROPERTY_RELIABLE_WRITE
        if propxml.get("encrypted_write","false") == "true":
            write |= GattConst.CHR_PROPERTY_WRITE
            permit |= GattConst.EncryptedWrite
        if propxml.get("authenticated_write","false") == "true":
            write |= GattConst.CHR_PROPERTY_WRITE
            permit |= GattConst.AuthenticatedWrite
        if propxml.get("bonded_write","false") == "true":
            write |= GattConst.CHR_PROPERTY_WRITE
            permit |= GattConst.BondedWrite

        return write, permit, ext_properties

    def parse_client_config(self, propxml):
        config = 0
        permit = 0

        permissions = propxml.find('notify')
        if (permissions is not None):
            config |= GattConst.CHR_PROPERTY_NOTIFY
            if permissions.get("encrypted","false") == "true":
                permit |= GattConst.EncryptedNotify
            if permissions.get("authenticated","false") == "true":
                permit |= GattConst.AuthenticatedNotify
            if permissions.get("bonded","false") == "true":
                permit |= GattConst.BondedNotify

        # Indicate permissions, should be same as notify_permissions
        # Similar workaround as for write permissions
        permissions = propxml.find('indicate')
        if (permissions is not None):
            config |= GattConst.CHR_PROPERTY_INDICATE
            if permissions.get("encrypted","false") == "true":
                permit |= GattConst.EncryptedNotify
            if permissions.get("authenticated","false") == "true":
                permit |= GattConst.AuthenticatedNotify
            if permissions.get("bonded","false") == "true":
                permit |= GattConst.BondedNotify

        # Backward compatibility to old property+permission mix attributes:
        if propxml.get("notify","false") == "true":
            config |= GattConst.CHR_PROPERTY_NOTIFY
        if propxml.get("indicate","false") == "true":
            config |= GattConst.CHR_PROPERTY_INDICATE
        if propxml.get("encrypted_notify","false") == "true":
            permit |= GattConst.EncryptedNotify
        if propxml.get("authenticated_notify","false") == "true":
            permit |= GattConst.AuthenticatedNotify
        if propxml.get("bonded_notify","false") == "true":
            permit |= GattConst.BondedNotify

        if permit != 0 and (config & GattConst.CHR_PROPERTY_INDICATE)==0:
            config |= GattConst.CHR_PROPERTY_NOTIFY

        return config, permit

    def parse_properties(self, propxml):
        properties = 0
        permissions = GattConst.Discoverable
        const_value = False
        ext_properties = 0

        # Backward compatibility only. In new schema, 'const' is an attribute of characteristic itself:
        if propxml.get("const","false") == "true":
            const_value = True
        # Not sure about this one, has it even been used on Gecko?
        if propxml.get("broadcast","false") == "true":
            properties |= GattConst.CHR_PROPERTY_BROADCAST

        read, read_permit = self.parse_read_property(propxml)
        properties |= read
        permissions |= read_permit

        write, write_permit, ext_properties = self.parse_write_property(propxml)
        properties |= write
        permissions |= write_permit

        if propxml.get("discoverable","true") == "false":
            properties = 0
            permissions = 0

        notify, notify_permit = self.parse_client_config(propxml)
        properties |= notify
        permissions |= notify_permit

        return properties, permissions, const_value, ext_properties

    def parse_value(self, xml):        
        ttype = xml.get('type', "utf-8")
        if ttype == "user":
            return GattValue(GattConst.UserValue, None)

        if xml.text is None:
            bytes = bytearray([])
        elif ttype == "hex":
            hexstr = xml.text.lower()
            if hexstr.startswith("0x"):
                hexstr = hexstr.replace("0x","", 1)
            bytes = bytearray.fromhex(hexstr)
        else:
            bytes = xml.text.encode('utf-8')
        
        maxlen = int(xml.get('length', "0"))
        if maxlen == 0 and len(bytes) != 0:
            maxlen  = len(bytes)
        datalen = len(bytes)
        if xml.get('variable_length',"false") == "true":
            type = GattConst.VariableLengthValue
            
            #length defines the maximum length
            #if length is not given, then <value> length defines the maximum length
            #if length and <value> are given, length defines the maximum length and value defines the current length (up to max length, no padding)
            #add max_length, but length needs to be supported for backwards compatibility
            if int(xml.get('max_length', "0")) != 0:
                maxlen = int(xml.get('max_length', "0"))

            #create dummy data for backward compatibility
            if len(bytes)==0:
                bytes=bytearray(maxlen)

        else:
            #length always overrides what's used inside the <value> </value>
            #If no length is given, but data exists inside <value> </value>, then the length of data is used to define the length of characteristic
            #0 length produces an warning
            #too long length definition or data produces an error
            #inconsistent length and <value> (value shorter than length) padding will be added to the end, warning shown
            #no length or no <value> given produces an error
            type = GattConst.DynamicValue
        for i in range(maxlen-len(bytes)):
            bytes=bytearray(bytes)
            bytes.append(0)

        if maxlen < len(bytes):
            print ("WARNING: {0} length attribute is {1} but actual value length is {2}. {1} will be used.".format("",maxlen,len(bytes)))

            bytes = bytes[0:maxlen]
            datalen = len(bytes)
        if maxlen > len(bytes):
            if(len(bytes)>0):
                maxlen=len(bytes)
                print ("WARNING: {0} max length attribute is {1} but actual value length is {2}. {1} will be used.".format("",maxlen,len(bytes)))


        msg = ET.tostring(xml).strip()
        gatt_xml_assert(maxlen != 0, "Either a length or value must be given: \"" + str(msg) + "\"")
        gatt_xml_assert(maxlen <= ATTRIBUTE_VALUE_MAX_SIZE, "Too long attribute value: \"" + str(msg) + "\". Maximum is " + str(ATTRIBUTE_VALUE_MAX_SIZE) + " bytes")
        
        return GattValue(type, bytes, maxlen=maxlen,datalen=datalen)

    def __parse_attrobj(self, xml, attr):
        if attr.uuid == "0000":
            attr.uuid = BluetoothUuid(xml.get('uuid', "00"))
        attr.idstr = xml.get('id', "")

        const_value = True
        properties = GattConst.CHR_PROPERTY_READ
        permissions = GattConst.Read | GattConst.Discoverable
        ext_properties = 0
        if xml.find('properties') is not None:
            properties, permissions, const_value, ext_properties = self.parse_properties(xml.find('properties'))
        if xml.get("const","false") == "true":
            const_value = True
        attr.properties = properties
        attr.permissions = permissions
        attr.ext_properties = ext_properties
        if  xml.find('value') is not None:
            attr.value = self.parse_value( xml.find('value'))
        #if there is no value element, then use contents of the parent tag, backwards compability
        elif xml.text is not None:
            attr.value = self.parse_value(xml)
        if attr.value!=None and const_value is True:
            attr.value.type = GattConst.ConstValue
        if attr.value == None:
            #attribute value can't be just null
            attr.value = GattValue(GattConst.ConstValue, bytearray())
        return attr
